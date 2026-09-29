/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/scenario/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/scenario/catalog. */

#ifdef THANDOR_TEST_AIDS
#include <stdlib.h>
#include <thandor/platform/bootstrap/image.h>

/* Port-only test aid: UTF-16 list row text as ANSI for the log (non-ASCII becomes '?'). */
static const char *ScenarioCatalog_TestAidRowName(const uint16_t *text, char *out, unsigned capacity)
{
  unsigned length = 0;
  while (text[length] != 0 && length + 1 < capacity) {
    out[length] = (text[length] < 128) ? (char)text[length] : '?';
    length++;
  }
  if (length != 0 && out[length - 1] == '.') { /* the list rows keep the extension dot of the file name */
    length--;
  }
  out[length] = 0;
  return out;
}

/* Port-only test aid for unattended mission runs, called when the "Choose game" page opens in a local game:
     OPEN_THANDOR_LIST_SCENARIOS=1 logs the names of all single games and campaigns ("scenario: ...") and
                                   ends the process;
     OPEN_THANDOR_CAMPAIGN=<name>  starts that campaign (name as in the campaigns list, or its 0-based row)
                                   like its Start button, once per process; OPEN_THANDOR_CAMPAIGN_LEVEL picks
                                   the level (see ScenarioCatalog_TestAidSelectCampaignLevel).
   Returns nonzero when a campaign was started. */
static int ScenarioCatalog_TestAidApplyScenarioOptions(void)
{
  static int used;
  const char *wanted;
  UiListControl *list;
  char name[64];
  uint32_t row;
  if (used) {
    return 0;
  }
  used = 1;
  if (getenv("OPEN_THANDOR_LIST_SCENARIOS") != NULL) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList);
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: single \"%s\"",
                  ScenarioCatalog_TestAidRowName((const uint16_t *)list->rowSlots[row],name,sizeof name));
    }
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
    list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
    for (row = 0; row < list->rowCount; row++) {
      Thandor_Log("scenario: campaign %u \"%s\"",row,
                  ScenarioCatalog_TestAidRowName((const uint16_t *)list->rowSlots[row],name,sizeof name));
    }
    ExitProcess(0);
  }
  wanted = getenv("OPEN_THANDOR_CAMPAIGN");
  if (wanted == NULL) {
    return 0;
  }
  ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  list = (UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
  for (row = 0; row < list->rowCount; row++) {
    ScenarioCatalog_TestAidRowName((const uint16_t *)list->rowSlots[row],name,sizeof name);
    if (_stricmp(name,wanted) == 0 ||
        (wanted[0] >= '0' && wanted[0] <= '9' && (uint32_t)atoi(wanted) == row)) {
      Thandor_Log("test aid: starting campaign %u \"%s\"",row,name);
      ScenarioCatalog_SelectCampaignAndShowDescription(g_LocalPlayerRuntimeId,0,0,row);
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,row);
      return 1;
    }
  }
  Thandor_Log("test aid: campaign \"%s\" not found",wanted);
  return 0;
}

/* Port-only test aid: logs the levels of a just loaded campaign (CampaignAsset) and, with
   OPEN_THANDOR_CAMPAIGN_LEVEL=<n>, makes its n-th level (1-based, in record order) the first one. */
static void ScenarioCatalog_TestAidSelectCampaignLevel(uint8_t *campaignBytes)
{
  CampaignAsset *campaign = (CampaignAsset *)campaignBytes;
  const char *wanted = getenv("OPEN_THANDOR_CAMPAIGN_LEVEL");
  int count = campaign->levelRecordCount;
  int index;
  char name[64];
  for (index = 0; index < count; index++) {
    const CampaignLevelRecord *record = &campaign->levels[index];
    Thandor_Log("campaign level %d: id %d \"%s\"%s",index + 1,record->levelId,
                ScenarioCatalog_TestAidRowName(record->levelFileName,name,sizeof name),
                (record->levelId == campaign->firstLevelId) ? " (first)" : "");
  }
  if (wanted != NULL && atoi(wanted) >= 1 && atoi(wanted) <= count) {
    campaign->firstLevelId = campaign->levels[atoi(wanted) - 1].levelId;
    Thandor_Log("test aid: campaign starts at level %d",atoi(wanted));
  }
}
#endif

/* Address: 0x00549E50.
   Handler of action 0x2039, the saved-games list (slot 57 of g_FrontendUiActionHandlersPage20.handlers00_54):
   a changed selection shows the saved game's description, locally or on every peer through the frontend
   command queue; a confirmed row (double click) starts it like the Start button.
*/
void FrontendScenarioSelection_SelectOrStartSavedGame(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexAndConfirmed(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectSavedGameAndShowDescription(g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SAVED_GAME,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,savedGamesList,gameSelectStartButton));
  return;
}


/* Address: 0x00549EB0.
   Handler of FRONTEND_ACTION_SELECT_SINGLE_GAME, the single-games list (slot 58 of
   g_FrontendUiActionHandlersPage20.handlers00_54): a changed selection shows the level's description, locally
   or on every peer through the frontend command queue; a confirmed row (double click) starts it like the
   Start button.
*/
void FrontendScenarioSelection_SelectOrStartLevel(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexAndConfirmed(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectLevelAndShowDescription
                (g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SINGLE_GAME,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,missionsList,gameSelectStartButton));
  return;
}


/* Address: 0x00549F10.
   Handler of FRONTEND_ACTION_SELECT_CAMPAIGN, the campaigns list (slot 59 of
   g_FrontendUiActionHandlersPage20.handlers00_54): a changed selection shows the campaign's description,
   locally or on every peer through the frontend command queue; a confirmed row (double click) starts it
   like the Start button.
*/
void FrontendScenarioSelection_SelectOrStartCampaign(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexAndConfirmed(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectCampaignAndShowDescription
                (g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_CAMPAIGN,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,campaignsList,gameSelectStartButton));
  return;
}


/* Address: 0x0054A280.
   Opens page 10 "Choose game" (tabs Load game / Single game / Campaigns) and fills the list of the active tab;
   network sessions always use the Single game tab. Network clients only watch: all buttons are hidden. With
   the command-line option KARTE="<level>" matching a single mission, that mission is selected and loaded
   directly; network hosts never get the Load game tab.
*/
void FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView *scenarioSelectionPage)

{
  /* scenarioOptionRow0..4 are the page's Cancel, Start, Load game, Single game and Campaigns buttons. */
  UiNodeFlags *controlFlags;
  short codeUnit;
  CommandPayload selectionIndex;
  UiControlCount activeTabIndex;
  int remainingCount;
  uint32_t compareUnitsRemaining;
  uint8_t *scanCursor;
  uint8_t *textCursor;
  short *levelNameCursor;
  bool namesMatch;
  SelectableGroupNodeResult selectedTab;
  CommandLineOptionResult mapOption;

  UiPageStack_SetActiveIndex(FRONTEND_PAGE_STACK_CHOOSE_GAME,&scenarioSelectionPage->primaryPageStack);
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    /* compactLayoutControl.nodeFlags is menuRoomModelView's contextFlags (+0x4C): the page covers the menu room */
    controlFlags = &(scenarioSelectionPage->compactLayoutControl).nodeFlags;
    *controlFlags = *controlFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  selectedTab = UiSelectableGroup_NoneVisibleSelected(3,
      FRONTEND_UI(scenarioSelectionPage,loadGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,campaignsTabButton));
  activeTabIndex = selectedTab.controlIndexOrCount;
  if (selectedTab.noneSelected) {
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
  mapOption = g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 14);
  if (!mapOption.notFound) {
    UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,loadGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,singleGameTabButton),
      FRONTEND_UI(scenarioSelectionPage,campaignsTabButton));
    activeTabIndex = SCENARIO_SELECTION_TAB_SINGLE_GAMES;
  }
  /* Rebuild the tab's list, then refresh the description of its selected entry. */
  g_FrontendUiActionHandlersPage20.scenarioCatalogRebuildCallbacks[activeTabIndex](0,0,0,0);
  g_FrontendScenarioMapOptionHandlerTable[activeTabIndex](0,0,0,0);
#ifdef THANDOR_TEST_AIDS
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL &&
      ScenarioCatalog_TestAidApplyScenarioOptions()) {
    return;
  }
#endif
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
  mapOption = g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 14);
  textCursor = mapOption.option;
  if (!mapOption.notFound) {
    /* The level name runs from after KARTE=" to the closing quote, which must end the command line. */
    remainingCount = PACKAGE_SCRATCH_BUFFER_BYTES / 2 - 1;
    for (scanCursor = textCursor + 7; *scanCursor != '"'; scanCursor++) {
      if ((*scanCursor < ' ') || (remainingCount--, remainingCount == 0))
        goto updateButtonAvailability;
    }
    *scanCursor = 0;
    if (scanCursor[1] == 0) {
      Text_CopyNarrowToUtf16(PACKAGE_SCRATCH_BUFFER_BYTES,(uint16_t *)g_PackageScratchBuffer,textCursor + 7);
      WidePath_SetExtensionCode(0,(uint16_t *)g_PackageScratchBuffer);
      *scanCursor = '"';
      /* "KARTE" -> "kARTE": the option is used only once, returning to this page later finds no match. */
      *textCursor = 'k';
      /* wcslen + 1 (REPNE SCASW): scanCursor ends behind the terminator, which the compare includes. */
      remainingCount = 0x400000;
      textCursor = g_PackageScratchBuffer;
      do {
        scanCursor = textCursor;
        if (remainingCount == 0) break;
        remainingCount--;
        scanCursor = textCursor + 2;
        codeUnit = *(short *)textCursor;
        textCursor = scanCursor;
      } while (codeUnit != 0);
      remainingCount = ((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowCount;
      if (remainingCount != 0) {
        selectionIndex = 0;
        namesMatch = true;
        do {
          /* REPE CMPSW of the name against the row's text */
          compareUnitsRemaining = (uint32_t)((int)scanCursor - (int)g_PackageScratchBuffer) >> 1;
          textCursor = g_PackageScratchBuffer;
          levelNameCursor =
               (short *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots[selectionIndex];
          do {
            if (compareUnitsRemaining == 0) break;
            compareUnitsRemaining--;
            namesMatch = *(short *)textCursor == *levelNameCursor;
            textCursor += 2;
            levelNameCursor++;
          } while (namesMatch);
          if (namesMatch) {
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
            Resource_Release(g_FrontendLoadedCampaignAsset);
            g_FrontendLoadedCampaignAsset = NULL;
            /* Select the mission and load it: directly, or in a network session through the command queue. */
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              ScenarioCatalog_SelectLevelAndShowDescription
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SINGLE_GAME,0,0,selectionIndex);
            }
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendScenarioSession_LoadOrRequestLevelAsset
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LOAD_LEVEL,0,0,selectionIndex);
            }
            return;
          }
          selectionIndex++;
          remainingCount--;
          namesMatch = remainingCount == 0;
        } while (!namesMatch);
      }
    }
  }

updateButtonAvailability:
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


/* Address: 0x0054A610.
   Handler of the "Load game" tab (actions 0x2035 and 0x2052, slots 53 and 82 of
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
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildSaveRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SHOW_SAVED_GAMES,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_SelectSavedGameAndShowDescription(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SAVED_GAME,0,0,0);
  }
  return;
}


/* Address: 0x0054A690.
   Handler of the "Single game" tab (actions 0x2036 and 0x2053, slots 54 and 83 of
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
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SHOW_SINGLE_GAMES,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_SelectLevelAndShowDescription(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SINGLE_GAME,0,0,0);
  }
  return;
}


/* Address: 0x0054A710.
   Handler of the "Campaigns" tab (actions 0x2037 and 0x2054, slots 55 and 84 of
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
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SHOW_CAMPAIGNS,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_SelectCampaignAndShowDescription(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_CAMPAIGN,0,0,0);
  }
  return;
}


/* Address: 0x00549A70.
   Handler of action 0x2041 (slot 65 of g_FrontendUiActionHandlersPage20.handlers00_54): loads the selected
   level's field grid (FrontendScenarioSession_LoadOrRequestFieldGrid), directly in a local game or on every
   peer through the frontend command queue.
*/
void FrontendScenarioAction_StartFieldGridLoad(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    /* the original pushes the four command-handler arguments (g_LocalPlayerRuntimeId,0,0,0) */
    FrontendScenarioSession_LoadOrRequestFieldGrid(g_LocalPlayerRuntimeId);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LOAD_FIELD_GRID,0,0,0);
  }
  return;
}


/* Address: 0x00549FD0.
   Rebuilds g_ScenarioCatalog, the list behind the "Choose game" tabs: the single missions of level\level.dat
   and the campaigns of level\campagne.dat, each updated by the add-on files level00..99.dat /
   campagne00..99.dat (records merged by name), followed by the header record of every save\*.sve.
   The catalog is also what a network host sends to its clients.
*/
void ScenarioCatalog_Rebuild(void)

{
  ScenarioCatalogHeader *catalog;
  uint32_t recordCount;
  void *handle;
  uint32_t dwordsRemaining;
  uint32_t saveFilesRemaining;
  uint16_t *saveFileName;
  uint32_t *levelSourceDword;
  uint32_t *campaignSourceDword;
  ScenarioCatalogRecord *recordsBase;
  ScenarioCatalogRecord *recordCopyCursor;
  ScenarioCatalogSaveRecord *saveRecord;
  ArenaAllocResult allocation;
  FatalErrorCheckResult checkedResult;
  FileSystemOpenResult openResult;
  ResourceLoadResult loadedResource;
  DirectoryEnumerationResult saveEnumeration;
  void *handleToClose;

  g_MemoryApi.free(g_ScenarioCatalog);
  allocation = g_MemoryApi.alloc(SCENARIO_CATALOG_CAPACITY);
  checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
  catalog = (ScenarioCatalogHeader *)checkedResult.valueOrError;
  g_ScenarioCatalogUsedBytes = SCENARIO_CATALOG_HEADER_SIZE;
  g_ScenarioCatalog = catalog;
  catalog->levelRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->campaignRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->saveRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->levelRecordCount = 0;
  catalog->campaignRecordCount = 0;
  catalog->saveRecordCount = 0;
  loadedResource = Resource_Load((uint16_t *)u_level_level_dat_0050da0e);
  catalog = g_ScenarioCatalog;
  if (!loadedResource.failed) {
    recordCount = loadedResource.byteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    /* REP MOVSD of the whole file into the level section */
    levelSourceDword = (uint32_t *)loadedResource.bufferOrError;
    recordCopyCursor = recordsBase;
    for (dwordsRemaining = loadedResource.byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
      *(uint32_t *)recordCopyCursor->identifier = *levelSourceDword;
      levelSourceDword++;
      recordCopyCursor = (ScenarioCatalogRecord *)(recordCopyCursor->identifier + 2);
    }
    Resource_Release((uint32_t *)loadedResource.bufferOrError);
    /* level00.dat .. level99.dat: the two digit code units are packed as one dword (UTF16_DIGIT_PAIR); the
       units digit counts '0'..'9', then subtracting UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP resets it to '0' and
       increments the tens digit. */
    g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits = UTF16_DIGIT_PAIR('0','0');
    do {
      loadedResource = Resource_Load(g_ScenarioLevelDataPathTemplateUtf16.prefixCodeUnits);
      if (!loadedResource.failed) {
        recordCount = ScenarioCatalog_MergeRecordsByName
                          (loadedResource.byteCount,(ScenarioCatalogRecord *)loadedResource.bufferOrError,recordCount,recordsBase);
        Resource_Release((ScenarioCatalogRecord *)loadedResource.bufferOrError);
      }
      g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1]++;
    } while ((g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[1] < '9' + 1) ||
            (g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits =
                  g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits - UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP,
            g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.codeUnits[0] < '9' + 1));
    do {
      catalog->campaignRecordsOffset = catalog->campaignRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->levelRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  loadedResource = Resource_Load((uint16_t *)u_level_campagne_dat_0050da52);
  catalog = g_ScenarioCatalog;
  if (!loadedResource.failed) {
    recordCount = loadedResource.byteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->campaignRecordsOffset);
    campaignSourceDword = (uint32_t *)loadedResource.bufferOrError;
    recordCopyCursor = recordsBase;
    for (dwordsRemaining = loadedResource.byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
      *(uint32_t *)recordCopyCursor->identifier = *campaignSourceDword;
      campaignSourceDword++;
      recordCopyCursor = (ScenarioCatalogRecord *)(recordCopyCursor->identifier + 2);
    }
    Resource_Release((uint32_t *)loadedResource.bufferOrError);
    /* campagne00.dat .. campagne99.dat, counted like the level files above */
    g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits = UTF16_DIGIT_PAIR('0','0');
    do {
      loadedResource = Resource_Load(g_ScenarioCampaignDataPathTemplateUtf16.prefixCodeUnits);
      if (!loadedResource.failed) {
        recordCount = ScenarioCatalog_MergeRecordsByName
                          (loadedResource.byteCount,(ScenarioCatalogRecord *)loadedResource.bufferOrError,recordCount,recordsBase);
        Resource_Release((ScenarioCatalogRecord *)loadedResource.bufferOrError);
      }
      g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1]++;
    } while ((g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[1] < '9' + 1) ||
            (g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits =
                  g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits - UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP,
            g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.codeUnits[0] < '9' + 1));
    do {
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->campaignRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save___sve_0050d9c8,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  saveEnumeration = g_FileSystemEnumerateDirectoryOrVolumeEntries
                     (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  catalog = g_ScenarioCatalog;
  saveFilesRemaining = saveEnumeration.entryCount;
  if ((!saveEnumeration.failed) && (saveFilesRemaining != 0)) {
    saveRecord = (ScenarioCatalogSaveRecord *)
                 ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->saveRecordsOffset);
    saveFileName = (uint16_t *)g_PackageScratchBuffer;
    do {
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,(uint16_t *)u_save_0050daa2,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_ScenarioCatalogPathScratchUtf16,saveFileName,
                 (uint16_t *)&g_ScenarioCatalogPathScratchUtf16);
      openResult = g_FileSystemOpen
                         (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,(uint16_t *)&g_ScenarioCatalogPathScratchUtf16
                         );
      checkedResult = FatalError_ExitIfFailed(openResult.handleOrError,openResult.failed);
      handle = (void *)checkedResult.valueOrError;
      handleToClose = handle;
      /* The save's catalog record is the second 0x100-byte block of the file. */
      g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,SCENARIO_CATALOG_RECORD_SIZE,handle);
      g_FileSystemReadExact(SCENARIO_CATALOG_RECORD_SIZE,saveRecord,handle);
      g_FileSystemClose(handleToClose);
      /* Turn the stored level title index and the optional campaign title index (negative = none) into
         text resource ids. */
      saveRecord->levelTitleTextId = saveRecord->levelTitleTextId + TEXT_ID_LEVEL_TITLE_BASE;
      if (-1 < saveRecord->campaignTitleTextId) {
        saveRecord->campaignTitleTextId =
             saveRecord->campaignTitleTextId + TEXT_ID_CAMPAIGN_TITLE_BASE;
      }
      saveRecord++;
      catalog->saveRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      saveFileName = (uint16_t *)((int)saveFileName + saveEnumeration.recordSizeBytes);
      saveFilesRemaining--;
    } while (saveFilesRemaining != 0);
  }
  return;
}


/* Address: 0x00545290.
   Handler of FRONTEND_COMMAND_STOP_ROM_TRANSITION (frontend command signature: player id and three arguments,
   all ignored): skips the running menu-room camera flight via FrontendRomTransition_RequestStop.
*/
void ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3)

{
  FrontendRomTransition_RequestStop();
  return;
}


/* Address: 0x00547860.
   Network client, once per frontend frame: when the asset announced in g_FrontendScenarioTransferState has
   arrived in the transfer mailbox, unpacks it (scenario catalog, level, field grid, or a level/campaign bundle),
   frees the mailbox buffer and reports the new state to the host through the frontend command queue (or
   directly when no network session runs). Every packet starts with the unpacked size(s), then the packed data.
*/
void FrontendScenarioTransfer_ProcessReceivedAsset(void)

{
  /* The three command payload dwords double as a 96-bit mask of levels that are new in the
     received catalog: the original ORs bit n into [ESP + EBX*4] (EBX 3..1), and those slots are
     the 0xE00 command's arguments. Index 1 is the first argument after the command code. */
  uint32_t changedLevelMask[4];
  uint32_t payloadSizeBytes;
  ScenarioCatalogRecordCount recordCount;
  ScenarioCatalogByteOffset oldLevelRecordsOffset;
  ScenarioCatalogByteOffset *oldCatalogBase;
  uint32_t *receivedDwords;
  FrontendLoadedLevelAsset *levelAsset;
  uint8_t *levelPathOrCurrentLevelField;
  uint32_t maskBit;
  int maskSlotOrRecordsLeft;
  ScenarioCatalogRecordCount newRecordsRemaining;
  uint8_t *streamOrRecordCursor;
  bool recordAlreadyKnown;
  ArenaAllocResult allocation;
  FatalErrorCheckResult checkedResult;
  MailboxReceiveResult received;
  ScenarioCatalogHeader *previousCatalog;

  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) &&
     (g_FrontendScenarioTransferState != SCENARIO_TRANSFER_NONE)) {
    if (g_FrontendScenarioTransferState == SCENARIO_TRANSFER_CATALOG) {
      /* packet: unpacked size, packed catalog */
      received = UiTransferMailbox_GetReceivedBuffer();
      receivedDwords = (uint32_t *)received.buffer;
      if (!received.unavailable) {
        payloadSizeBytes = *receivedDwords;
        allocation = g_MemoryApi.alloc(payloadSizeBytes);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        changedLevelMask[1] = 0;
        changedLevelMask[2] = 0;
        changedLevelMask[3] = 0;
        previousCatalog = g_ScenarioCatalog;
        g_ScenarioCatalog = (ScenarioCatalogHeader *)checkedResult.valueOrError;
        g_ScenarioCatalogUsedBytes = payloadSizeBytes;
        PckCodec_DecodeHuffmanRle(payloadSizeBytes,(uint8_t *)checkedResult.valueOrError,received.byteCount - 4,(uint8_t *)(receivedDwords + 1));
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
        /* Mark every received level record (up to 96) that the previous catalog did not contain. */
        newRecordsRemaining = g_ScenarioCatalog->levelRecordCount;
        if (previousCatalog != NULL) {
          recordCount = previousCatalog->levelRecordCount;
          receivedDwords = (uint32_t *)((uint8_t *)g_ScenarioCatalog +
                            g_ScenarioCatalog->levelRecordsOffset);
          oldLevelRecordsOffset = previousCatalog->levelRecordsOffset;
          oldCatalogBase = &previousCatalog->levelRecordsOffset;
          if ((newRecordsRemaining != 0) && (recordCount != 0)) {
            maskBit = 1;
            maskSlotOrRecordsLeft = 3;
            do {
              recordAlreadyKnown = DwordBlock64Array_ContainsExactRecord
                                 (recordCount,(uint32_t *)((int)oldCatalogBase + oldLevelRecordsOffset),receivedDwords);
              if (!recordAlreadyKnown) {
                changedLevelMask[maskSlotOrRecordsLeft] = changedLevelMask[maskSlotOrRecordsLeft] | maskBit;
              }
              receivedDwords = receivedDwords + SCENARIO_CATALOG_RECORD_SIZE / 4;
              maskBit = maskBit * 2;
              if (maskBit == 0) {
                maskBit = 1;
                maskSlotOrRecordsLeft--;
                if (maskSlotOrRecordsLeft == 0) break;
              }
              newRecordsRemaining--;
            } while (newRecordsRemaining != 0);
          }
        }
        g_MemoryApi.free(previousCatalog);
        FrontendCommandQueue_EnqueueLocalPlayerCommand
                  (FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED,changedLevelMask[1],changedLevelMask[2],changedLevelMask[3]);
      }
    }
    else if (g_FrontendScenarioTransferState < SCENARIO_TRANSFER_FIELD_GRID) {
      /* SCENARIO_TRANSFER_LEVEL, packet: unpacked size, packed level asset */
      received = UiTransferMailbox_GetReceivedBuffer();
      receivedDwords = (uint32_t *)received.buffer;
      if (!received.unavailable) {
        payloadSizeBytes = *receivedDwords;
        /* The level's path offset field holds the loaded field grid once one was attached (values above
           0xFFFF are pointers). */
        if ((g_FrontendLoadedLevelAsset != NULL) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = NULL;
        allocation = g_MemoryApi.alloc(payloadSizeBytes);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedResult.valueOrError;
        PckCodec_DecodeHuffmanRle
                  (payloadSizeBytes,(uint8_t *)g_FrontendLoadedLevelAsset,received.byteCount - 4,(uint8_t *)(receivedDwords + 1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkTaskAssignmentReadyById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LEVEL_RECEIVED,0,0,0);
        }
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
      }
    }
    else if (g_FrontendScenarioTransferState == SCENARIO_TRANSFER_FIELD_GRID) {
      /* packet: unpacked size, packed field grid; it is attached to the already loaded level */
      received = UiTransferMailbox_GetReceivedBuffer();
      receivedDwords = (uint32_t *)received.buffer;
      if (!received.unavailable) {
        payloadSizeBytes = *receivedDwords;
        allocation = g_MemoryApi.alloc(payloadSizeBytes);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedResult.valueOrError
        ;
        PckCodec_DecodeFieldGrid
                  (payloadSizeBytes,(FieldGridAsset *)checkedResult.valueOrError,received.byteCount - 4,(uint8_t *)(receivedDwords + 1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkLevelReceivedById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_FIELD_GRID_RECEIVED,0,0,0);
        }
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
      }
    }
    else if (g_FrontendScenarioTransferState < SCENARIO_TRANSFER_LEVEL_BUNDLE) {
      /* SCENARIO_TRANSFER_CAMPAIGN_BUNDLE, packet: unpacked sizes of level, campaign and field grid, packed
         sizes of the three, then the three packed streams */
      received = UiTransferMailbox_GetReceivedBuffer();
      receivedDwords = (uint32_t *)received.buffer;
      if (!received.unavailable) {
        if ((g_FrontendLoadedLevelAsset != NULL) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = NULL;
        allocation = g_MemoryApi.alloc(*receivedDwords);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedResult.valueOrError;
        PckCodec_DecodeHuffmanRle
                  (*receivedDwords,(uint8_t *)g_FrontendLoadedLevelAsset,receivedDwords[3],(uint8_t *)(receivedDwords + 6));
        streamOrRecordCursor = (uint8_t *)((int)(receivedDwords + 6) + receivedDwords[3]);
        allocation = g_MemoryApi.alloc(receivedDwords[1]);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        g_FrontendLoadedCampaignAsset = (uint8_t *)checkedResult.valueOrError;
        PckCodec_DecodeHuffmanRle(receivedDwords[1],g_FrontendLoadedCampaignAsset,receivedDwords[4],streamOrRecordCursor);
        levelAsset = g_FrontendLoadedLevelAsset;
        payloadSizeBytes = receivedDwords[4];
        /* The level's relative path (asset base + path offset) becomes <exe dir>\<level>.fld, the path the
           game uses for the field grid; afterwards the offset field holds the received field grid. */
        levelPathOrCurrentLevelField = (uint8_t *)g_FrontendLoadedLevelAsset +
                                       (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
        WidePath_SetExtensionCode(ASSET_MAGIC_FLD,(uint16_t *)levelPathOrCurrentLevelField);
        WidePath_CombineDirectoryAndLeaf
                  ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)levelPathOrCurrentLevelField,
                   (uint16_t *)&g_ExecutableDirectoryUtf16);
        allocation = g_MemoryApi.alloc(receivedDwords[2]);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedResult.valueOrError;
        PckCodec_DecodeFieldGrid(receivedDwords[2],(FieldGridAsset *)checkedResult.valueOrError,receivedDwords[5],streamOrRecordCursor + payloadSizeBytes);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkLevelLoadedById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_BUNDLE_RECEIVED,0,0,0);
        }
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        streamOrRecordCursor = g_FrontendLoadedCampaignAsset;
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
        /* The campaign starts at its first level (stored as the current level): find that level's record
           and build level\<name>.lev. streamOrRecordCursor is the asset base advanced by whole
           CampaignLevelRecords, so its levels[0] is the record under the cursor. */
        levelPathOrCurrentLevelField =
             (uint8_t *)&((CampaignAsset *)g_FrontendLoadedCampaignAsset)->firstLevelId;
        maskSlotOrRecordsLeft = ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->levelRecordCount;
        ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->currentLevelId = *(int *)levelPathOrCurrentLevelField;
        do {
          if (*(int *)levelPathOrCurrentLevelField ==
              ((CampaignAsset *)streamOrRecordCursor)->levels[0].levelId) break;
          streamOrRecordCursor = streamOrRecordCursor + sizeof(CampaignLevelRecord);
          maskSlotOrRecordsLeft--;
        } while (maskSlotOrRecordsLeft != 0);
        WidePath_CombineDirectoryAndLeaf
                  (&g_FrontendScenarioPathScratchUtf16,
                   ((CampaignAsset *)streamOrRecordCursor)->levels[0].levelFileName,
                   (uint16_t *)u_level_0050daac);
        WidePath_SetExtensionCode(ASSET_MAGIC_LEV,&g_FrontendScenarioPathScratchUtf16);
        FrontendPlayerRuntime_InitializeFactionAssignments();
      }
    }
    else {
      /* SCENARIO_TRANSFER_LEVEL_BUNDLE, packet: unpacked sizes of level and field grid, their packed sizes,
         then the two packed streams */
      received = UiTransferMailbox_GetReceivedBuffer();
      receivedDwords = (uint32_t *)received.buffer;
      if (!received.unavailable) {
        if ((g_FrontendLoadedLevelAsset != NULL) &&
           (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid
           )) {
          Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                   levelPathOffsetOrLoadedFieldGrid);
        }
        Resource_Release(g_FrontendLoadedLevelAsset);
        g_FrontendLoadedLevelAsset = NULL;
        allocation = g_MemoryApi.alloc(*receivedDwords);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        levelAsset = (FrontendLoadedLevelAsset *)checkedResult.valueOrError;
        g_FrontendLoadedLevelAsset = levelAsset;
        PckCodec_DecodeHuffmanRle(*receivedDwords,(uint8_t *)levelAsset,receivedDwords[2],(uint8_t *)(receivedDwords + 4));
        payloadSizeBytes = receivedDwords[2];
        levelPathOrCurrentLevelField = (uint8_t *)levelAsset +
                                       (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
        WidePath_SetExtensionCode(ASSET_MAGIC_FLD,(uint16_t *)levelPathOrCurrentLevelField);
        WidePath_CombineDirectoryAndLeaf
                  ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)levelPathOrCurrentLevelField,
                   (uint16_t *)&g_ExecutableDirectoryUtf16);
        allocation = g_MemoryApi.alloc(receivedDwords[1]);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedResult.valueOrError;
        PckCodec_DecodeFieldGrid
                  (receivedDwords[1],(FieldGridAsset *)checkedResult.valueOrError,receivedDwords[3],
                   (uint8_t *)((int)(receivedDwords + 4) + payloadSizeBytes));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkLevelLoadedById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_BUNDLE_RECEIVED,0,0,0);
        }
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
        FrontendPlayerRuntime_InitializeFactionAssignments();
      }
    }
  }
  return;
}


/* Address: 0x005443B0.
   Loads the field grid (.fld) of g_FrontendLoadedLevelAsset once (while some player still lacks
   FRONTEND_PLAYER_STATE_LEVEL_RECEIVED): the host or a local game loads it from its package, a host also
   publishes it (PckCodec_EncodeFieldGrid) in the transfer mailbox; a client loads it itself when it has the level
   locally and otherwise requests it through the mailbox (SCENARIO_TRANSFER_FIELD_GRID). Every other player
   that has the level locally counts as ready; then the frontend returns to the main page with code 1.
   Reached as frontend command FRONTEND_COMMAND_LOAD_FIELD_GRID (command-handler format: playerRuntimeId and
   three unused arguments, RET 0x10 in the original).
   Original quirk: FRONTEND_PLAYER_STATE_LEVEL_RECEIVED is set on the first player block, not on the player the
   scan stopped at (0x005443F6 MOV EAX,[g_FrontendPlayerRuntimeBlocks]; 0x00544401 OR [EAX+0x60],8, while ESI
   is reloaded with the level asset). The flag thus marks "grid loaded on this machine"; a later call would find the
   same other player still unflagged (unless the has-level-locally pass below flagged it) and load again.
*/
void FrontendScenarioSession_LoadOrRequestFieldGrid(uint32_t playerRuntimeId)

{
  FrontendRoleStateFlags *roleFlags;
  uint32_t bytes;
  uint32_t levelPathOffset;
  PckDecodedByteCount sourceImageSizeBytes;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendLoadedLevelAsset *levelAsset;
  FrontendLoadedLevelAsset *clientLevelAsset;
  FrontendPlayerRuntimeRecord *playerScanBase;
  FieldGridAsset *sourceGrid;
  FrontendPlayerRuntimeBlockCount playersToCheck;
  uint32_t dwordsRemaining;
  int otherPlayersRemaining;
  uint8_t *pathOrEncodeBuffer;
  uint32_t *encodedSourceDwords;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *outgoingDwordCursor;
  PackageLoadResult loadedEntry;
  FatalErrorCheckResult checkedResult;
  PckCodecResult encodeResult;
  ArenaAllocResult allocation;
  
  levelAsset = g_FrontendLoadedLevelAsset;
  playersToCheck = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (((playerRecord->factionAssignment).roleStateFlags & FRONTEND_PLAYER_STATE_LEVEL_RECEIVED) == 0) {
      levelPathOffset = (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
      /* original quirk: the flag goes to the first player record, not to the one found (see above) */
      roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_LEVEL_RECEIVED;
      /* the level's own path (an offset into the asset) with the extension changed to .fld; the loaded grid
         later replaces that offset */
      pathOrEncodeBuffer = (uint8_t *)levelAsset + levelPathOffset;
      WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,(uint16_t *)pathOrEncodeBuffer);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)pathOrEncodeBuffer,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      clientLevelAsset = g_FrontendLoadedLevelAsset;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
        /* Client: find the local player among the other players. */
        playerRecord = g_FrontendPlayerRuntimeBlocks + 1;
        otherPlayersRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
        while (g_LocalPlayerRuntimeId != playerRecord->playerRuntimeId) {
          playerRecord++;
          otherPlayersRemaining--;
          if (otherPlayersRemaining == 0) break;
        }
        if ((otherPlayersRemaining != 0) &&
            (((playerRecord->factionAssignment).roleStateFlags & FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY) != 0)) {
          loadedEntry = Package_LoadEntry((uint16_t *)pathOrEncodeBuffer);
          checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
          (clientLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedResult.valueOrError;
        }
        else {
          /* Not found (the record one past the last player is written, as in the original) or the
             field grid is not available locally: request it through the transfer mailbox. */
          *(uint32_t *)playerRecord->snapshotPayload = 0;
          UiTransferMailbox_MarkUnavailable();
          g_FrontendScenarioTransferState = SCENARIO_TRANSFER_FIELD_GRID;
        }
        break;
      }
      loadedEntry = Package_LoadEntry((uint16_t *)pathOrEncodeBuffer);
      checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
      sourceGrid = (FieldGridAsset *)checkedResult.valueOrError;
      (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
      encodedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        /* transfer image: the decoded size, then the encoded grid; copied into its own buffer */
        sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
        pathOrEncodeBuffer = g_PackageScratchBuffer + 4;
        *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
        encodeResult = PckCodec_EncodeFieldGrid(PACKAGE_SCRATCH_BUFFER_BYTES - 4,pathOrEncodeBuffer,
                                                sourceImageSizeBytes,sourceGrid);
        checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
        bytes = checkedResult.valueOrError + 4;
        allocation = g_MemoryApi.alloc(bytes);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        outgoingDwordCursor = (uint32_t *)checkedResult.valueOrError;
        for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining--) {
          *outgoingDwordCursor = *encodedSourceDwords;
          encodedSourceDwords++;
          outgoingDwordCursor++;
        }
        UiTransferMailbox_SetOutgoingBuffer(bytes,(uint32_t *)checkedResult.valueOrError);
      }
      break;
    }
    playerRecord++;
    playersToCheck--;
  } while (playersToCheck != 0);
  /* Every other player that has the level locally counts as having received it. */
  playerScanBase = g_FrontendPlayerRuntimeBlocks;
  for (playersRemaining = g_FrontendPlayerRuntimeBlockCount - 1; playersRemaining != 0;
      playersRemaining--) {
    playerScanBase++;
    if ((playerScanBase->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY) != 0) {
      roleFlags = &playerScanBase->factionAssignment.roleStateFlags;
      *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_LEVEL_RECEIVED;
      playerScanBase->transferProgressBytes = INT32_MAX;
    }
  }
  FrontendSession_ReturnToMainPage(playerRuntimeId,0,0,1);
  return;
}


/* Address: 0x00544AC0.
   Starts the campaign in row selectedRecordIndex of the campaigns list. The host (or a local game) loads
   level\<name>.cgn as g_FrontendLoadedCampaignAsset, finds the record of the campaign's current level, loads
   that level (replacing g_FrontendLoadedLevelAsset) and its field grid, publishes level, campaign and grid
   as one encoded transfer bundle when hosting, and sets up the faction assignments; a client instead waits
   for that bundle (SCENARIO_TRANSFER_CAMPAIGN_BUNDLE). Then the frontend closes its dialog pages and runs
   ROM action table entry 3. Reached as frontend command FRONTEND_COMMAND_LOAD_CAMPAIGN.
*/
void FrontendScenarioSession_LoadOrRequestCampaignBundle
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRecordIndex)

{
  FrontendRoleStateFlags *roleFlags;
  FieldGridAsset *sourceGrid;
  uint32_t encodedLevelBytes;
  uint32_t encodedCampaignBytes;
  int campaignRecordsRemaining;
  uint32_t dwordsRemaining;
  uint8_t *cursorOrSize;
  int frontendRoot;
  uint8_t *recordOrEncodeCursor;
  uint8_t *transferBundleBytes;
  FrontendLoadedLevelAsset *source;
  uint32_t *transferCopyDestination;
  PackageLoadResult loadedEntry;
  FatalErrorCheckResult checkedResult;
  PckCodecResult encodeResult;
  ArenaAllocResult allocation;
  PckDecodedByteCount campaignDecodedSizeBytes;
  
  frontendRoot = g_FrontendRootNode;
  roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_LEVEL_LOADED;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,
               (uint16_t *)((UiListControl *)FRONTEND_UI(frontendRoot,campaignsList))->rowSlots[selectedRecordIndex],
               (uint16_t *)u_level_0050dab8);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_CGN,&g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
    recordOrEncodeCursor = (uint8_t *)checkedResult.valueOrError;
#ifdef THANDOR_TEST_AIDS
    ScenarioCatalog_TestAidSelectCampaignLevel(recordOrEncodeCursor);
#endif
    /* CampaignAsset: the first level becomes the current one; find its record. recordOrEncodeCursor is the
       asset base advanced by whole CampaignLevelRecords, so its levels[0] is the record under the cursor. */
    cursorOrSize = (uint8_t *)&((CampaignAsset *)recordOrEncodeCursor)->firstLevelId;
    campaignRecordsRemaining = ((CampaignAsset *)recordOrEncodeCursor)->levelRecordCount;
    g_FrontendLoadedCampaignAsset = recordOrEncodeCursor;
    ((CampaignAsset *)recordOrEncodeCursor)->currentLevelId = *(int *)cursorOrSize;
    do {
      if (*(int *)cursorOrSize == ((CampaignAsset *)recordOrEncodeCursor)->levels[0].levelId) break;
      recordOrEncodeCursor = recordOrEncodeCursor + sizeof(CampaignLevelRecord);
      campaignRecordsRemaining--;
    } while (campaignRecordsRemaining != 0);
    if (campaignRecordsRemaining == 0) {
      /* No record for the current level. As in the original (XOR EAX,EAX clears CF) this check never
         fails; the cursor then points behind the last record. */
      FatalError_ExitIfFailed(0,false);
    }
    /* above 0xFFFF the level's path field holds its loaded field grid instead of the path offset */
    if ((g_FrontendLoadedLevelAsset != NULL) &&
       (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
      Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                               levelPathOffsetOrLoadedFieldGrid);
    }
    Resource_Release(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = NULL;
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,((CampaignAsset *)recordOrEncodeCursor)->levels[0].levelFileName,
               (uint16_t *)u_level_0050daac);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,&g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedResult.valueOrError;
    cursorOrSize = (uint8_t *)g_FrontendLoadedLevelAsset +
                   (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,(uint16_t *)cursorOrSize);
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)cursorOrSize,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    /* the original does not check this load (no CF dispatch) */
    loadedEntry = Package_LoadEntry((uint16_t *)cursorOrSize);
    cursorOrSize = g_FrontendLoadedCampaignAsset;
    source = g_FrontendLoadedLevelAsset;
    transferBundleBytes = g_PackageScratchBuffer;
    sourceGrid = loadedEntry.bufferOrError;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      /* bundle header (6 dwords): decoded sizes of level, campaign and grid, then their encoded sizes;
         the three encoded images follow */
      ((ScenarioCampaignBundleHeader *)g_PackageScratchBuffer)->levelDecodedBytes =
           (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
      campaignDecodedSizeBytes = ((CampaignAsset *)cursorOrSize)->decodedSizeBytes;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->fieldGridDecodedBytes =
           (sourceGrid->common).allocationSizeBytes;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->campaignDecodedBytes = campaignDecodedSizeBytes;
      recordOrEncodeCursor = transferBundleBytes + sizeof(ScenarioCampaignBundleHeader);
      encodeResult = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 24,recordOrEncodeCursor,(source->header).common.allocationSizeBytes,(uint8_t *)source
                         );
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      encodedLevelBytes = checkedResult.valueOrError;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->levelEncodedBytes = encodedLevelBytes;
      recordOrEncodeCursor = recordOrEncodeCursor + encodedLevelBytes;
      encodeResult = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes,recordOrEncodeCursor,
                          ((CampaignAsset *)cursorOrSize)->decodedSizeBytes,cursorOrSize);
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      encodedCampaignBytes = checkedResult.valueOrError;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->campaignEncodedBytes = encodedCampaignBytes;
      encodeResult = PckCodec_EncodeFieldGrid
                         ((PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes) - encodedCampaignBytes,recordOrEncodeCursor + encodedCampaignBytes,
                          (sourceGrid->common).allocationSizeBytes,sourceGrid);
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->fieldGridEncodedBytes = checkedResult.valueOrError;
      cursorOrSize = recordOrEncodeCursor + encodedCampaignBytes + (checkedResult.valueOrError - (int)transferBundleBytes);
      allocation = g_MemoryApi.alloc((uint32_t)cursorOrSize);
      checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
      transferCopyDestination = (uint32_t *)checkedResult.valueOrError;
      for (dwordsRemaining = (uint32_t)cursorOrSize >> 2; dwordsRemaining != 0; dwordsRemaining--) {
        *transferCopyDestination = *(uint32_t *)transferBundleBytes;
        transferBundleBytes += 4;
        transferCopyDestination++;
      }
      UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)cursorOrSize,(uint32_t *)checkedResult.valueOrError);
    }
    (source->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
    FrontendPlayerRuntime_InitializeFactionAssignments();
  }
  else {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = SCENARIO_TRANSFER_CAMPAIGN_BUNDLE;
  }
  ((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags =
       ((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags &
       ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  OldUnitRuntime_ResetPendingTables();
  FrontendState_DispatchCode(3); /* ROM action table entry 3 */
  return;
}


/* Address: 0x00544DC0.
   Shows the "Load game" tab: selects its tab button and page and fills the saved-games list with the save
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
  void **rowPointers;
  void **rowPointerCursor;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,loadGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_SAVED_GAMES,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != NULL) {
    rowCount = g_ScenarioCatalog->saveRecordCount;
    /* the section offsets count from the catalog start; the row pointer array is built behind the records */
    saveRecord = (void *)((uint8_t *)g_ScenarioCatalog +
                     g_ScenarioCatalog->saveRecordsOffset);
    rowPointers = (void **)(rowCount * SCENARIO_CATALOG_RECORD_SIZE + (int)saveRecord);
    rowPointerCursor = rowPointers;
    for (remainingRows = rowCount; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = saveRecord;
      rowPointerCursor++;
      saveRecord = (void *)((int)saveRecord + SCENARIO_CATALOG_RECORD_SIZE);
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
            (0,NULL,(UiPointerListControl *)FRONTEND_UI(firstNode,savedGamesList));
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  return;
}


/* Address: 0x00544EA0.
   Shows the "Single game" tab: selects its tab button and page and fills the single-games list with the level
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
  ScenarioCatalogDisplayRecord **rowPointerCursor;
  TextResolveResult resolvedText;
  UiListRowCount rowCount;
  ScenarioCatalogDisplayRecord **rowPointers;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_SINGLE_GAMES,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != NULL) {
    remainingRows = g_ScenarioCatalog->levelRecordCount;
    scenarioRecord =
         (ScenarioCatalogDisplayRecord *)
         ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    rowPointerCursor = (ScenarioCatalogDisplayRecord **)(scenarioRecord + remainingRows);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = scenarioRecord;
      resolvedText = TextResource_Resolve(scenarioRecord->titleTextResourceId + TEXT_ID_LEVEL_COLUMN50_BASE);
      scenarioRecord->titleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->titleResolvedText = resolvedText.text;
      resolvedText = TextResource_Resolve(scenarioRecord->subtitleTextResourceId + TEXT_ID_LEVEL_COLUMN60_BASE);
      scenarioRecord->subtitleDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->subtitleResolvedText = resolvedText.text;
      /* +0x70 is the level title; its text starts with palette colour 1 */
      resolvedText = TextResource_Resolve(scenarioRecord->scenarioTextResourceId + TEXT_ID_LEVEL_TITLE_BASE);
      *resolvedText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_1;
      scenarioRecord->scenarioDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->scenarioResolvedText = resolvedText.text;
      resolvedText = TextResource_Resolve(scenarioRecord->modeTextResourceId + TEXT_ID_LEVEL_COLUMN80_BASE);
      scenarioRecord->modeDisplayTag = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      scenarioRecord->modeResolvedText = resolvedText.text;
      rowPointerCursor++;
      scenarioRecord++;
    }
    control = (int32_t *)FRONTEND_UI(firstNode,missionsList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
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
                (0,NULL,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,NULL,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
  }
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  UiNodeList_SuppressActionId(FRONTEND_ACTION_SELECT_SINGLE_GAME,firstNode);
  return;
}


/* Address: 0x00545020.
   Shows the "Campaigns" tab: selects its tab button and page and fills the campaigns list with the campaign
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
  void **rowPointerCursor;
  TextResolveResult resolvedText;
  UiListRowCount rowCount;
  void **rowPointers;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(SCENARIO_SELECTION_TAB_CAMPAIGNS,
                             (UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != NULL) {
    remainingRows = g_ScenarioCatalog->campaignRecordCount;
    campaignRecord = (void *)((uint8_t *)g_ScenarioCatalog +
                     g_ScenarioCatalog->campaignRecordsOffset);
    rowPointerCursor = (void **)(remainingRows * SCENARIO_CATALOG_RECORD_SIZE + (int)campaignRecord);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows--) {
      *rowPointerCursor = campaignRecord;
      /* title index, then its jump record (command, text pointer) */
      resolvedText = TextResource_Resolve(((ScenarioCatalogDisplayRecord *)campaignRecord)->titleTextResourceId +
                                          TEXT_ID_CAMPAIGN_TITLE_BASE);
      ((ScenarioCatalogDisplayRecord *)campaignRecord)->titleDisplayTag =
           RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_JUMP_NESTED;
      ((ScenarioCatalogDisplayRecord *)campaignRecord)->titleResolvedText = resolvedText.text;
      rowPointerCursor++;
      campaignRecord = (void *)((int)campaignRecord + SCENARIO_CATALOG_RECORD_SIZE);
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
                (0,NULL,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,NULL,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
  }
  UiNodeList_SuppressActionId(FRONTEND_ACTION_START_SELECTED_GAME,firstNode);
  UiNodeList_SuppressActionId(FRONTEND_ACTION_SELECT_CAMPAIGN,firstNode);
  return;
}


/* Address: 0x00549F70.
   Merges sourceByteCount / 0x100 catalog records into destinationRecords, matching them by their 0x40-byte
   UTF-16 identifier: a match is overwritten, a new identifier is appended. Returns the new destination
   record count (EDX in the original). The original scans with REPE CMPSD and copies with REP MOVSD; like it,
   the loops assume at least one source and one existing destination record.
*/
ScenarioCatalogRecordCount ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords)

{
  uint32_t sourceRecordsRemaining;
  int dwordsRemaining;
  int copyDwordsRemaining;
  ScenarioCatalogRecordCount destinationRecordsRemaining;
  ScenarioCatalogRecord *sourceNameCursor;
  ScenarioCatalogRecord *destinationRecordCursor;
  ScenarioCatalogRecord *destinationNameCursor;
  bool recordNamesEqual;
  
  recordNamesEqual = true;
  sourceRecordsRemaining = sourceByteCount / SCENARIO_CATALOG_RECORD_SIZE;
  destinationRecordsRemaining = existingRecordCount;
  destinationRecordCursor = destinationRecords;
  do {
    /* compare the 0x40-byte identifiers dword by dword (the cursors advance by 4 bytes) */
    dwordsRemaining = sizeof(sourceRecords->identifier) / 4;
    sourceNameCursor = sourceRecords;
    destinationNameCursor = destinationRecordCursor;
    do {
      if (dwordsRemaining == 0) break;
      dwordsRemaining--;
      recordNamesEqual =
           *(int *)sourceNameCursor->identifier == *(int *)destinationNameCursor->identifier;
      sourceNameCursor = (ScenarioCatalogRecord *)(sourceNameCursor->identifier + 2);
      destinationNameCursor = (ScenarioCatalogRecord *)(destinationNameCursor->identifier + 2);
    } while (recordNamesEqual);
    if (!recordNamesEqual) {
      destinationRecordCursor++;
      destinationRecordsRemaining--;
      recordNamesEqual = destinationRecordsRemaining == 0;
      if (!recordNamesEqual) continue; /* compare with the next destination record */
      /* No match: append after the existing records. */
      existingRecordCount++;
    }
    /* copy the whole 0x100-byte record; this also advances sourceRecords to the next source record */
    for (copyDwordsRemaining = SCENARIO_CATALOG_RECORD_SIZE / 4; copyDwordsRemaining != 0; copyDwordsRemaining--) {
      *(uint32_t *)destinationRecordCursor->identifier = *(uint32_t *)sourceRecords->identifier;
      sourceRecords = (ScenarioCatalogRecord *)(sourceRecords->identifier + 2);
      destinationRecordCursor = (ScenarioCatalogRecord *)(destinationRecordCursor->identifier + 2);
    }
    sourceRecordsRemaining--;
    recordNamesEqual = false;
    destinationRecordsRemaining = existingRecordCount;
    destinationRecordCursor = destinationRecords;
    if (sourceRecordsRemaining == 0) {
      return existingRecordCount;
    }
  } while( true );
}


/* Address: 0x00544870.
   Handler for starting a single-game level (frontend command 0x920 in a network game): builds the level path,
   returns the frontend to the main page, drops the previously loaded level and its field grid, then loads the
   level. The host also packs it into the transfer mailbox for the clients; a client loads it from its own disk
   only when its catalog level mask has it, otherwise it requests it (SCENARIO_TRANSFER_LEVEL). Every other
   player whose mask has the level is marked as having it locally with a finished transfer.
*/
void FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex)

{
  FrontendRoleStateFlags *roleFlags;
  UiPageStackControl *pageStack;
  PckDecodedByteCount levelSizeBytes;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendLoadedLevelAsset *levelAsset;
  uint32_t maskWordIndex; /* also the dword counter of the mailbox copy */
  uint32_t byteCountOrOffset; /* packed size (host) or byte offset of the level record in the catalog */
  int rootOrRemaining;
  uint8_t *packedDestination;
  uint32_t *packedSourceDwords;
  FrontendPlayerRuntimeRecord *playerCursor;
  uint32_t *outgoingDwordCursor;
  PackageLoadResult loadedEntry;
  FatalErrorCheckResult checkedResult;
  PckCodecResult encodeResult;
  ArenaAllocResult allocation;
  bool levelLoadedLocally;

  rootOrRemaining = g_FrontendRootNode;
  pageStack = (UiPageStackControl *)FRONTEND_UI(g_FrontendRootNode,frontendPageStack);
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             (uint16_t *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                     [selectedRowIndex],
             (uint16_t *)u_level_0050daac);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,&g_FrontendScenarioPathScratchUtf16);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,pageStack);
  ((FrontendModelPointerContext *)FRONTEND_UI(rootOrRemaining,menuRoomModelView))->contextFlags =
       ((FrontendModelPointerContext *)FRONTEND_UI(rootOrRemaining,menuRoomModelView))->contextFlags &
       ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  FrontendState_DispatchCode(1); /* ROM action table entry 1 */
  /* levelPathOffsetOrLoadedFieldGrid holds the field-grid path offset until the grid is loaded, then its
     pointer: only values above 0xFFFF are loaded grids */
  if ((g_FrontendLoadedLevelAsset != NULL) &&
     (0xffff < g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release((void *)g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid);
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = NULL;
  roleFlags = &g_FrontendPlayerRuntimeBlocks->factionAssignment.roleStateFlags;
  *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
    packedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
    levelAsset = (FrontendLoadedLevelAsset *)checkedResult.valueOrError;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      /* host: mailbox packet = unpacked size dword + Huffman/RLE-packed level */
      levelSizeBytes = levelAsset->header.common.allocationSizeBytes;
      packedDestination = g_PackageScratchBuffer + 4;
      g_FrontendLoadedLevelAsset = levelAsset;
      *(PckDecodedByteCount *)g_PackageScratchBuffer = levelSizeBytes;
      encodeResult = PckCodec_EncodeHuffmanRle(PACKAGE_SCRATCH_BUFFER_BYTES - 4,packedDestination,levelSizeBytes,
                                               (uint8_t *)levelAsset);
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      byteCountOrOffset = checkedResult.valueOrError + 4;
      allocation = g_MemoryApi.alloc(byteCountOrOffset);
      checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
      outgoingDwordCursor = (uint32_t *)checkedResult.valueOrError;
      for (maskWordIndex = byteCountOrOffset >> 2; maskWordIndex != 0; maskWordIndex--) {
        *outgoingDwordCursor = *packedSourceDwords;
        packedSourceDwords++;
        outgoingDwordCursor++;
      }
      UiTransferMailbox_SetOutgoingBuffer(byteCountOrOffset,(uint32_t *)checkedResult.valueOrError);
      levelAsset = g_FrontendLoadedLevelAsset;
    }
  }
  else {
    /* The level's bit in the players' level masks: record offset / 0x100 is the level index, split into
       mask dword (offset >> 13) and bit ((offset >> 8) & 31); there are three mask dwords (96 levels). */
    byteCountOrOffset = ((int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                   [selectedRowIndex] -
            (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
    maskWordIndex = byteCountOrOffset >> 13;
    /* Client: load the level locally when the local player's level mask has it, else request it. */
    levelLoadedLocally = false;
    if (maskWordIndex < 3) {
      rootOrRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
      playerCursor = g_FrontendPlayerRuntimeBlocks;
      do {
        if (g_LocalPlayerRuntimeId == playerCursor[1].playerRuntimeId) {
          if (((&playerCursor[1].scenarioAvailabilityMask0)[maskWordIndex] &
              1 << ((uint8_t)(byteCountOrOffset >> 8) & 31)) != 0) {
            loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
            levelAsset = loadedEntry.bufferOrError;
            levelLoadedLocally = !loadedEntry.failed;
          }
          break;
        }
        rootOrRemaining--;
        playerCursor++;
      } while (rootOrRemaining != 0);
    }
    if (!levelLoadedLocally) {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = SCENARIO_TRANSFER_LEVEL;
      levelAsset = g_FrontendLoadedLevelAsset;
    }
  }
  g_FrontendLoadedLevelAsset = levelAsset;
  byteCountOrOffset = ((int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                 [selectedRowIndex] -
          (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
  maskWordIndex = byteCountOrOffset >> 13;
  playerCursor = g_FrontendPlayerRuntimeBlocks;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  if (maskWordIndex < 3) {
    /* every other player (block 1..) */
    while (playerRecord = playerCursor, playersRemaining = playersRemaining - 1, playersRemaining != 0) {
      playerCursor = playerRecord + 1;
      if (((&playerRecord[1].scenarioAvailabilityMask0)[maskWordIndex] & 1 << ((uint8_t)(byteCountOrOffset >> 8) & 31))
          != 0) {
        roleFlags = &playerRecord[1].factionAssignment.roleStateFlags;
        *roleFlags = *roleFlags | (FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY | FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT);
        playerRecord[1].transferProgressBytes = INT32_MAX; /* transfer progress: complete */
      }
    }
  }
  return;
}


/* Address: 0x00545140.
   Selection callback of the saved-games list: selects the row and shows the saved game's description, the
   text of the save record's +0x70 id alone, or (when the +0x90 id is not negative) both texts inserted into
   the TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE; the box keeps the empty placeholder while the list has no rows.
   Reached as frontend command FRONTEND_COMMAND_SELECT_SAVED_GAME and through
   g_FrontendScenarioMapOptionHandlerTable[SCENARIO_SELECTION_TAB_SAVED_GAMES].
*/
void ScenarioCatalog_SelectSavedGameAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int rowTableOrRecord;
  TextResourceId resourceId;
  int frontendRoot;
  TextResolveResult primaryText;
  TextResolveResult fieldText;
  
  frontendRoot = g_FrontendRootNode;
  rowTableOrRecord = (int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,savedGameDescriptionText))->text =
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowTableOrRecord != 0) {
    UiPointerList_SelectColumnListIndex(selectionIndex,control);
    rowTableOrRecord = *(int *)(rowTableOrRecord + selectionIndex * 4);
    if (((ScenarioCatalogSaveRecord *)rowTableOrRecord)->campaignTitleTextId < 0) {
      resourceId = ((ScenarioCatalogSaveRecord *)rowTableOrRecord)->levelTitleTextId;
      primaryText = TextResource_Resolve(resourceId);
      /* the first code unit becomes palette colour 0 */
      *primaryText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text = (uint16_t *)resourceId;
    }
    else {
      primaryText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
      fieldText = TextResource_Resolve(((ScenarioCatalogSaveRecord *)rowTableOrRecord)->levelTitleTextId);
      *fieldText.text = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,primaryText.text);
      fieldText = TextResource_Resolve(((ScenarioCatalogSaveRecord *)rowTableOrRecord)->campaignTitleTextId);
      RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,primaryText.text);
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text =
           (uint16_t *)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
    }
  }
  return;
}


/* Address: 0x00545240.
   Selection callback of the campaigns list: selects the row and shows the campaign's description text
   (TEXT_ID_CAMPAIGN_DESCRIPTION_BASE + the record's title index at +0x50) in the description box, which keeps
   the empty placeholder while the list has no rows. Reached as frontend command FRONTEND_COMMAND_SELECT_CAMPAIGN
   and through g_FrontendScenarioMapOptionHandlerTable[SCENARIO_SELECTION_TAB_CAMPAIGNS].
*/
void ScenarioCatalog_SelectCampaignAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int rowPointers;
  int frontendRoot;
  
  frontendRoot = g_FrontendRootNode;
  rowPointers = (int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,campaignDescriptionText))->text =
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != 0) {
    UiPointerList_SelectColumnListIndex(selectionIndex,control);
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,campaignDescriptionText))->text =
         (uint16_t *)(((ScenarioCatalogDisplayRecord *)*(int *)(rowPointers + selectionIndex * 4))->
                        titleTextResourceId +
                      TEXT_ID_CAMPAIGN_DESCRIPTION_BASE);
  }
  return;
}


/* Address: 0x00549CC0.
   Handler of FRONTEND_ACTION_START_SELECTED_GAME, the Start button of the "Choose game" page (slot 56 of
   g_FrontendUiActionHandlersPage20.handlers00_54; the three list handlers call it directly for a confirmed
   row). Drops a loaded campaign and starts the selection of the active tab: loads the single game's level or
   the campaign bundle (locally or on every peer through the frontend command queue), or, for a saved game,
   puts save\<name>.sve into g_FrontendScenarioPathScratchUtf16 and returns to the main page with code 2.
*/
void FrontendScenarioSelection_ActivateSelectedRecord(FrontendScenarioSelectionControlAddress32 selectionControl)

{
  UiListRowIndex selectedRowIndex;
  ListSelectionResult selectedRow;
  SelectableGroupNodeResult selectedGroup;
  int scenarioPathPointerTableAddress;

  /* selectionControl is the frontend template's gameSelectStartButton; the other nodes are its siblings. */
  selectedGroup = UiSelectableGroup_NoneVisibleSelected(3,
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,loadGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,singleGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsTabButton));
  if (selectedGroup.noneSelected) {
    return;
  }
  if (selectedGroup.controlIndexOrCount != 0) {
    if (selectedGroup.controlIndexOrCount < 2) {
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = NULL;
      selectedRow = UiPointerList_GetSelectedIndexAndConfirmed
                        ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,missionsList));
      selectedRowIndex = selectedRow.rowIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendScenarioSession_LoadOrRequestLevelAsset(g_LocalPlayerRuntimeId,0,0,selectedRowIndex)
        ;
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LOAD_LEVEL,0,0,selectedRowIndex);
      }
      return;
    }
    Resource_Release(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = NULL;
    selectedRow = UiPointerList_GetSelectedIndexAndConfirmed
                      ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsList));
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex)
      ;
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LOAD_CAMPAIGN,0,0,selectedRow.rowIndex);
    }
    return;
  }
  scenarioPathPointerTableAddress =
       (int)((UiListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList))->rowSlots;
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = NULL;
  selectedRow = UiPointerList_GetSelectedIndexAndConfirmed
                    ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList));
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(uint16_t **)(scenarioPathPointerTableAddress + selectedRow.rowIndex * 4),
             (uint16_t *)u_save_0050daa2);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,&g_FrontendScenarioPathScratchUtf16);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,2);
  }
  return;
}


/* Address: 0x005451F0.
   Selection callback of the single-game (missions) list: selects the row and shows the level's description
   text (TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * the record's title index at +0x70) in
   the description box, which keeps the empty placeholder while the list has no rows.
*/
void ScenarioCatalog_SelectLevelAndShowDescription
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *listControl;
  int rowPointers;
  int frontendRoot;

  frontendRoot = g_FrontendRootNode;
  rowPointers = (int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots;
  listControl = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,missionDescriptionText))->text =
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != 0) {
    UiPointerList_SelectColumnListIndex(selectionIndex,listControl);
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,missionDescriptionText))->text =
         (uint16_t *)(((ScenarioCatalogDisplayRecord *)*(int *)(rowPointers + selectionIndex * 4))->
                        scenarioTextResourceId * TEXT_ID_LEVEL_DESCRIPTION_STRIDE +
                      TEXT_ID_LEVEL_DESCRIPTION_BASE);
  }
  return;
}

