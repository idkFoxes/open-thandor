/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/scenario/catalog.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* UTF-16 L"level\\*.lev" after the save pattern; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedLevelPatternUtf16[12] = L"level\\*.lev";

/* UTF-16 L"level\\*.cgn"; no code reference found */
__declspec(align(4)) uint16_t g_UnreferencedCampaignPatternUtf16[12] = L"level\\*.cgn";

__declspec(align(4)) uint32_t g_FrontendLoadedCampaignAsset = 0;

static uint16_t g_LevelLevelDatPathUtf16[16] = L"level\\level.dat";

static ScenarioLevelDataPathTemplate24 g_ScenarioLevelDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x6C, 0x65, 0x76, 0x65, 0x6C},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".dat"};

static uint16_t g_LevelCampagneDatPathUtf16[19] = L"level\\campagne.dat";

static ScenarioCampaignDataPathTemplate2A g_ScenarioCampaignDataPathTemplateUtf16 = {
    .prefixCodeUnits = {0x6C, 0x65, 0x76, 0x65, 0x6C, 0x5C, 0x63, 0x61, 0x6D, 0x70, 0x61, 0x67, 0x6E, 0x65},
    .decimalDigits = {.codeUnits = {0x30, 0x30}},
    .suffixCodeUnits = L".dat"};

static uint16_t g_CampaignLevelDirectoryUtf16[6] = L"level";

/* list refresh handler per scenario selection tab (SCENARIO_SELECTION_TAB_*) */
static ScenarioCatalogRefreshSelectedRecordCallback *const g_FrontendScenarioMapOptionHandlerTable[3] = {
    /* 0 */ ScenarioCatalog_SelectSavedGameAndShowDescription,
    /* 1 */ ScenarioCatalog_SelectLevelAndShowDescription,
    /* 2 */ ScenarioCatalog_SelectCampaignAndShowDescription};

ScenarioCatalogHeader *g_ScenarioCatalog = 0;

uint32_t g_ScenarioCatalogUsedBytes = 0;

uint16_t g_SaveSvePatternUtf16[11] = L"save\\*.sve";

uint16_t g_ScenarioLevelDirectoryUtf16[6] = L"level";

uint16_t g_LevelResourcePathScratchUtf16[256] = {0};

FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset = 0;

uint32_t g_FrontendScenarioTransferState = 0;

uint16_t g_FrontendScenarioPathScratchUtf16[256] = {0};

/* Implementation ownership: assets/scenario/catalog. */

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
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectSavedGameAndShowDescription(g_LocalPlayerRuntimeId,0,0,selectedRowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SAVED_GAME,0,0,selectedRowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,savedGamesList,gameSelectStartButton));
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
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectLevelAndShowDescription
                (g_LocalPlayerRuntimeId,0,0,selectedRowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_SINGLE_GAME,0,0,selectedRowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,missionsList,gameSelectStartButton));
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
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_SelectCampaignAndShowDescription
                (g_LocalPlayerRuntimeId,0,0,selectedRowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SELECT_CAMPAIGN,0,0,selectedRowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)
             THANDOR_UI_SIBLING(listControl,FrontendUiImage,campaignsList,gameSelectStartButton));
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
  if (mapOption == NULL) {
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
  if (!UiSelectableGroup_FindVisibleSelected(NULL,&activeTabIndex,3,
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
  if (mapOption != NULL) {
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


/* Handler of action 0x2041 (slot 65 of g_FrontendUiActionHandlersPage20.handlers00_54): loads the selected
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


/* Copies a whole loaded catalog file (byteCount / 4 dwords) into a catalog section. */
static void ScenarioCatalog_CopyFileIntoSection
          (ScenarioCatalogRecord *sectionRecords,const void *fileBytes,uint32_t byteCount)
{
  uint32_t *destinationDword = (uint32_t *)sectionRecords;
  const uint32_t *sourceDword = (const uint32_t *)fileBytes;
  uint32_t dwordsRemaining;

  for (dwordsRemaining = byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *destinationDword = *sourceDword;
    sourceDword++;
    destinationDword++;
  }
}


/* Merges the add-on files <prefix>00.dat .. <prefix>99.dat of a path template into a catalog section and
   returns the new record count. The two digit code units are packed as one dword (UTF16_DIGIT_PAIR): the units
   digit counts '0'..'9', then subtracting UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP resets it to '0' and increments
   the tens digit; the loop ends when the tens digit passes '9'. */
static ScenarioCatalogRecordCount ScenarioCatalog_MergeAddOnFiles
          (uint16_t *pathTemplate,union Utf16DecimalDigitPair4 *decimalDigits,
           ScenarioCatalogRecordCount recordCount,ScenarioCatalogRecord *sectionRecords)
{
  void *loadedBuffer;
  uint32_t loadedByteCount;

  decimalDigits->packedDigits = UTF16_DIGIT_PAIR('0','0');
  do {
    if (Resource_Load(pathTemplate,&loadedBuffer,&loadedByteCount,NULL)) {
      recordCount = ScenarioCatalog_MergeRecordsByName
                        (loadedByteCount,(ScenarioCatalogRecord *)loadedBuffer,recordCount,sectionRecords);
      Resource_Release((ScenarioCatalogRecord *)loadedBuffer);
    }
    decimalDigits->codeUnits[1]++;
    if (decimalDigits->codeUnits[1] >= '9' + 1) {
      /* units digit wrapped: back to '0', tens digit + 1 (the tens digit is checked only then; before, it
         is at most '9') */
      decimalDigits->packedDigits = decimalDigits->packedDigits - UTF16_DIGIT_PAIR_TENS_DOWN_ONES_UP;
    }
  } while (decimalDigits->codeUnits[0] < '9' + 1);
  return recordCount;
}


/* Rebuilds g_ScenarioCatalog, the list behind the "Choose game" tabs: the single missions of level\level.dat
   and the campaigns of level\campagne.dat, each updated by the add-on files level00..99.dat /
   campagne00..99.dat (records merged by name), followed by the header record of every save\*.sve.
   The catalog is also what a network host sends to its clients.
*/
void ScenarioCatalog_Rebuild(void)

{
  ScenarioCatalogHeader *catalog;
  uint32_t recordCount;
  void *handle;
  uint32_t saveFilesRemaining;
  uint8_t *saveFileEntry; /* FILESYSTEM_ENUMERATION_RECORD_BYTES per entry, starting with the file name */
  ScenarioCatalogRecord *recordsBase;
  ScenarioCatalogSaveRecord *saveRecord;
  uint32_t allocationError;
  void *allocationPayload;
  uint32_t checkedValue;
  uint32_t openError;
  Bool8 loaded;
  void *loadedBuffer;
  uint32_t loadedByteCount;
  void *handleToClose;

  g_MemoryApi.free(g_ScenarioCatalog);
  allocationError = g_MemoryApi.alloc(SCENARIO_CATALOG_CAPACITY,&allocationPayload);
  checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uint32_t)allocationPayload,allocationError != 0);
  catalog = (ScenarioCatalogHeader *)checkedValue;
  g_ScenarioCatalogUsedBytes = SCENARIO_CATALOG_HEADER_SIZE;
  g_ScenarioCatalog = catalog;
  catalog->levelRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->campaignRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->saveRecordsOffset = SCENARIO_CATALOG_HEADER_SIZE;
  catalog->levelRecordCount = 0;
  catalog->campaignRecordCount = 0;
  catalog->saveRecordCount = 0;
  loaded = Resource_Load((uint16_t *)g_LevelLevelDatPathUtf16,&loadedBuffer,&loadedByteCount,NULL);
  catalog = g_ScenarioCatalog;
  if (loaded) {
    recordCount = loadedByteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    ScenarioCatalog_CopyFileIntoSection(recordsBase,loadedBuffer,loadedByteCount);
    Resource_Release((uint32_t *)loadedBuffer);
    /* level00.dat .. level99.dat */
    recordCount = ScenarioCatalog_MergeAddOnFiles
                      (g_ScenarioLevelDataPathTemplateUtf16.prefixCodeUnits,
                       &g_ScenarioLevelDataPathTemplateUtf16.decimalDigits,recordCount,recordsBase);
    /* count the records (at least one, as in the original) */
    do {
      catalog->campaignRecordsOffset = catalog->campaignRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->levelRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  loaded = Resource_Load((uint16_t *)g_LevelCampagneDatPathUtf16,&loadedBuffer,&loadedByteCount,NULL);
  catalog = g_ScenarioCatalog;
  if (loaded) {
    recordCount = loadedByteCount / SCENARIO_CATALOG_RECORD_SIZE;
    recordsBase = (ScenarioCatalogRecord *)
             ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->campaignRecordsOffset);
    ScenarioCatalog_CopyFileIntoSection(recordsBase,loadedBuffer,loadedByteCount);
    Resource_Release((uint32_t *)loadedBuffer);
    /* campagne00.dat .. campagne99.dat */
    recordCount = ScenarioCatalog_MergeAddOnFiles
                      (g_ScenarioCampaignDataPathTemplateUtf16.prefixCodeUnits,
                       &g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits,recordCount,recordsBase);
    /* count the records (at least one, as in the original) */
    do {
      catalog->saveRecordsOffset = catalog->saveRecordsOffset + SCENARIO_CATALOG_RECORD_STRIDE;
      catalog->campaignRecordCount++;
      g_ScenarioCatalogUsedBytes = g_ScenarioCatalogUsedBytes + SCENARIO_CATALOG_RECORD_STRIDE;
      recordCount--;
    } while (recordCount != 0);
  }
  WidePath_CombineDirectoryAndLeaf
            (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveSvePatternUtf16,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
  saveFilesRemaining = g_FileSystemEnumerateDirectoryOrVolumeEntries
                     (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                      (uint8_t *)g_ScenarioCatalogPathScratchUtf16);
  catalog = g_ScenarioCatalog;
  if (saveFilesRemaining != 0) {
    saveRecord = (ScenarioCatalogSaveRecord *)
                 ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->saveRecordsOffset);
    saveFileEntry = g_PackageScratchBuffer;
    do {
      WidePath_CombineDirectoryAndLeaf
                (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)g_SaveDirectoryUtf16,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      WidePath_CombineDirectoryAndLeaf
                (g_ScenarioCatalogPathScratchUtf16,(uint16_t *)saveFileEntry,
                 g_ScenarioCatalogPathScratchUtf16);
      openError = g_FileSystemOpen
                         (FILESYSTEM_OPEN_EXCLUSIVE_SHARE,g_ScenarioCatalogPathScratchUtf16,
                          &handle);
      FatalError_ExitIfFailed(openError,openError != 0); /* does not return on failure */
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
      saveFileEntry = saveFileEntry + FILESYSTEM_ENUMERATION_RECORD_BYTES;
      saveFilesRemaining--;
    } while (saveFilesRemaining != 0);
  }
  return;
}


/* Handler of FRONTEND_COMMAND_STOP_ROM_TRANSITION (frontend command signature: player id and three arguments,
   all ignored): skips the running menu-room camera flight via FrontendRomTransition_RequestStop.
*/
void ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
                                                 uint32_t unusedArg3)

{
  FrontendRomTransition_RequestStop();
  return;
}


/* Allocates byteCount bytes through g_MemoryApi; FatalError_ExitIfFailed does not return on failure. */
static uint32_t FrontendScenarioTransfer_AllocateOrExit(uint32_t byteCount)
{
  uint32_t allocationError;
  void *allocationPayload;

  allocationError = g_MemoryApi.alloc(byteCount,&allocationPayload);
  return FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uint32_t)allocationPayload,
                                 allocationError != 0);
}


/* Frees g_FrontendLoadedLevelAsset and the field grid attached to it: the level's path offset field holds
   the loaded field grid once one was attached (values above 0xFFFF are pointers). */
static void FrontendScenarioTransfer_ReleaseLoadedLevelAsset(void)
{
  if ((g_FrontendLoadedLevelAsset != NULL) &&
     (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid);
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = NULL;
}


/* Frees the received mailbox buffer and ends the transfer. */
static void FrontendScenarioTransfer_FinishReceive(uint32_t *receivedDwords)
{
  g_MemoryApi.free(receivedDwords);
  UiTransferMailbox_ClearReceivedState();
  g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
}


/* The level's relative path (asset base + path offset) becomes <exe dir>\<level>.fld in
   g_LevelResourcePathScratchUtf16, the path the game uses for the field grid. */
static void FrontendScenarioTransfer_SetFieldGridPathOfLevel(FrontendLoadedLevelAsset *levelAsset)
{
  uint8_t *levelPath;

  levelPath = (uint8_t *)levelAsset + (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
  WidePath_SetExtensionCode(ASSET_MAGIC_FLD,(uint16_t *)levelPath);
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,(uint16_t *)levelPath,
             (uint16_t *)&g_ExecutableDirectoryUtf16);
}


/* SCENARIO_TRANSFER_CATALOG, packet: unpacked size, packed catalog. Replaces g_ScenarioCatalog and reports
   which received level records (up to 96) the previous catalog did not contain. */
static void FrontendScenarioTransfer_ProcessReceivedCatalog(void)
{
  /* The three command payload dwords double as a 96-bit mask of levels that are new in the
     received catalog: the original sets bit n in the payload dwords 3..1 directly, and those dwords are
     the 0xE00 command's arguments. Index 1 is the first argument after the command code. */
  uint32_t changedLevelMask[4];
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  uint32_t payloadSizeBytes;
  uint32_t checkedValue;
  ScenarioCatalogHeader *previousCatalog;
  ScenarioCatalogRecordCount oldRecordCount;
  ScenarioCatalogRecordCount newRecordsRemaining;
  uint32_t *oldLevelRecords;
  uint32_t *receivedRecord;
  uint32_t maskBit;
  int maskSlot;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  payloadSizeBytes = *receivedDwords;
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  changedLevelMask[1] = 0;
  changedLevelMask[2] = 0;
  changedLevelMask[3] = 0;
  previousCatalog = g_ScenarioCatalog;
  g_ScenarioCatalog = (ScenarioCatalogHeader *)checkedValue;
  g_ScenarioCatalogUsedBytes = payloadSizeBytes;
  PckCodec_DecodeHuffmanRle(payloadSizeBytes,(uint8_t *)checkedValue,receivedByteCount - 4,(uint8_t *)(receivedDwords + 1),NULL,NULL);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  /* Mark every received level record (up to 96) that the previous catalog did not contain. */
  newRecordsRemaining = g_ScenarioCatalog->levelRecordCount;
  if (previousCatalog != NULL) {
    oldRecordCount = previousCatalog->levelRecordCount;
    receivedRecord = (uint32_t *)((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
    oldLevelRecords = (uint32_t *)((uint8_t *)previousCatalog + previousCatalog->levelRecordsOffset);
    if ((newRecordsRemaining != 0) && (oldRecordCount != 0)) {
      maskBit = 1;
      maskSlot = 3;
      do {
        if (!DwordBlock64Array_ContainsExactRecord(oldRecordCount,oldLevelRecords,receivedRecord)) {
          changedLevelMask[maskSlot] = changedLevelMask[maskSlot] | maskBit;
        }
        receivedRecord = receivedRecord + SCENARIO_CATALOG_RECORD_SIZE / 4;
        maskBit = maskBit * 2;
        if (maskBit == 0) {
          maskBit = 1;
          maskSlot--;
          if (maskSlot == 0) break;
        }
        newRecordsRemaining--;
      } while (newRecordsRemaining != 0);
    }
  }
  g_MemoryApi.free(previousCatalog);
  FrontendCommandQueue_EnqueueLocalPlayerCommand
            (FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED,changedLevelMask[1],changedLevelMask[2],changedLevelMask[3]);
}


/* SCENARIO_TRANSFER_LEVEL, packet: unpacked size, packed level asset; replaces g_FrontendLoadedLevelAsset. */
static void FrontendScenarioTransfer_ProcessReceivedLevel(void)
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  uint32_t payloadSizeBytes;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  payloadSizeBytes = *receivedDwords;
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  PckCodec_DecodeHuffmanRle
            (payloadSizeBytes,(uint8_t *)g_FrontendLoadedLevelAsset,receivedByteCount - 4,(uint8_t *)(receivedDwords + 1),
             NULL,NULL);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkTaskAssignmentReadyById(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LEVEL_RECEIVED,0,0,0);
  }
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
}


/* SCENARIO_TRANSFER_FIELD_GRID, packet: unpacked size, packed field grid; it is attached to the already
   loaded level. */
static void FrontendScenarioTransfer_ProcessReceivedFieldGrid(void)
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  uint32_t payloadSizeBytes;
  uint32_t checkedValue;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  payloadSizeBytes = *receivedDwords;
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedValue;
  PckCodec_DecodeFieldGrid
            (payloadSizeBytes,(FieldGridAsset *)checkedValue,receivedByteCount - 4,(uint8_t *)(receivedDwords + 1),
             NULL,NULL);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkLevelReceivedById(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_FIELD_GRID_RECEIVED,0,0,0);
  }
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
}


/* SCENARIO_TRANSFER_CAMPAIGN_BUNDLE, packet: ScenarioCampaignBundleHeader (unpacked sizes of level, campaign
   and field grid, then their packed sizes), then the three packed streams. Afterwards the campaign starts at
   its first level: its record gives level\<name>.lev in g_FrontendScenarioPathScratchUtf16. */
static void FrontendScenarioTransfer_ProcessReceivedCampaignBundle(void)
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  ScenarioCampaignBundleHeader *bundle;
  uint8_t *campaignStream;
  uint8_t *fieldGridStream;
  FrontendLoadedLevelAsset *levelAsset;
  CampaignAsset *campaignAsset;
  uint8_t *levelRecordCursor;
  int levelRecordsRemaining;
  uint32_t checkedValue;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  bundle = (ScenarioCampaignBundleHeader *)receivedDwords;
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  g_FrontendLoadedLevelAsset =
       (FrontendLoadedLevelAsset *)FrontendScenarioTransfer_AllocateOrExit(bundle->levelDecodedBytes);
  PckCodec_DecodeHuffmanRle
            (bundle->levelDecodedBytes,(uint8_t *)g_FrontendLoadedLevelAsset,bundle->levelEncodedBytes,
             (uint8_t *)(bundle + 1),NULL,NULL);
  campaignStream = (uint8_t *)(bundle + 1) + bundle->levelEncodedBytes;
  g_FrontendLoadedCampaignAsset = FrontendScenarioTransfer_AllocateOrExit(bundle->campaignDecodedBytes);
  PckCodec_DecodeHuffmanRle(bundle->campaignDecodedBytes,(uint8_t *)g_FrontendLoadedCampaignAsset,
                            bundle->campaignEncodedBytes,campaignStream,NULL,NULL);
  levelAsset = g_FrontendLoadedLevelAsset;
  fieldGridStream = campaignStream + bundle->campaignEncodedBytes;
  /* afterwards the level's path offset field holds the received field grid */
  FrontendScenarioTransfer_SetFieldGridPathOfLevel(g_FrontendLoadedLevelAsset);
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(bundle->fieldGridDecodedBytes);
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedValue;
  PckCodec_DecodeFieldGrid(bundle->fieldGridDecodedBytes,(FieldGridAsset *)checkedValue,
                           bundle->fieldGridEncodedBytes,fieldGridStream,NULL,NULL);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkLevelLoadedById(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_BUNDLE_RECEIVED,0,0,0);
  }
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  /* The campaign starts at its first level (stored as the current level): find that level's record
     and build level\<name>.lev. levelRecordCursor is the asset base advanced by whole
     CampaignLevelRecords, so its levels[0] is the record under the cursor. Without a match the cursor
     ends behind the last record, as in the original. */
  campaignAsset = (CampaignAsset *)g_FrontendLoadedCampaignAsset;
  levelRecordCursor = (uint8_t *)campaignAsset;
  levelRecordsRemaining = campaignAsset->levelRecordCount;
  campaignAsset->currentLevelId = campaignAsset->firstLevelId;
  do {
    if (campaignAsset->firstLevelId == ((CampaignAsset *)levelRecordCursor)->levels[0].levelId) break;
    levelRecordCursor = levelRecordCursor + sizeof(CampaignLevelRecord);
    levelRecordsRemaining--;
  } while (levelRecordsRemaining != 0);
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             ((CampaignAsset *)levelRecordCursor)->levels[0].levelFileName,
             (uint16_t *)g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(ASSET_MAGIC_LEV,g_FrontendScenarioPathScratchUtf16);
  FrontendPlayerRuntime_InitializeFactionAssignments();
}


/* SCENARIO_TRANSFER_LEVEL_BUNDLE, packet: ScenarioLevelBundleHeader (unpacked sizes of level and field grid,
   their packed sizes), then the two packed streams. */
static void FrontendScenarioTransfer_ProcessReceivedLevelBundle(void)
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  ScenarioLevelBundleHeader *bundle;
  FrontendLoadedLevelAsset *levelAsset;
  uint32_t checkedValue;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  bundle = (ScenarioLevelBundleHeader *)receivedDwords;
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  levelAsset = (FrontendLoadedLevelAsset *)FrontendScenarioTransfer_AllocateOrExit(bundle->levelDecodedBytes);
  g_FrontendLoadedLevelAsset = levelAsset;
  PckCodec_DecodeHuffmanRle(bundle->levelDecodedBytes,(uint8_t *)levelAsset,bundle->levelEncodedBytes,
                            (uint8_t *)(bundle + 1),NULL,NULL);
  FrontendScenarioTransfer_SetFieldGridPathOfLevel(levelAsset);
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(bundle->fieldGridDecodedBytes);
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedValue;
  PckCodec_DecodeFieldGrid
            (bundle->fieldGridDecodedBytes,(FieldGridAsset *)checkedValue,bundle->fieldGridEncodedBytes,
             (uint8_t *)(bundle + 1) + bundle->levelEncodedBytes,NULL,NULL);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkLevelLoadedById(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_BUNDLE_RECEIVED,0,0,0);
  }
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  FrontendPlayerRuntime_InitializeFactionAssignments();
}


/* Network client, once per frontend frame: when the asset announced in g_FrontendScenarioTransferState has
   arrived in the transfer mailbox, unpacks it (scenario catalog, level, field grid, or a level/campaign bundle),
   frees the mailbox buffer and reports the new state to the host through the frontend command queue (or
   directly when no network session runs). Every packet starts with the unpacked size(s), then the packed data.
*/
void FrontendScenarioTransfer_ProcessReceivedAsset(void)

{
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) &&
     (g_FrontendScenarioTransferState != SCENARIO_TRANSFER_NONE)) {
    if (g_FrontendScenarioTransferState == SCENARIO_TRANSFER_CATALOG) {
      FrontendScenarioTransfer_ProcessReceivedCatalog();
    }
    else if (g_FrontendScenarioTransferState < SCENARIO_TRANSFER_FIELD_GRID) {
      FrontendScenarioTransfer_ProcessReceivedLevel();
    }
    else if (g_FrontendScenarioTransferState == SCENARIO_TRANSFER_FIELD_GRID) {
      FrontendScenarioTransfer_ProcessReceivedFieldGrid();
    }
    else if (g_FrontendScenarioTransferState < SCENARIO_TRANSFER_LEVEL_BUNDLE) {
      FrontendScenarioTransfer_ProcessReceivedCampaignBundle();
    }
    else {
      FrontendScenarioTransfer_ProcessReceivedLevelBundle();
    }
  }
  return;
}


/* Loading part of FrontendScenarioSession_LoadOrRequestFieldGrid, run when some player still lacks
   FRONTEND_PLAYER_STATE_LEVEL_RECEIVED: flags the first player block, turns the level's path into the .fld
   path, then loads the grid (host: also publishes it in the transfer mailbox) or, on a client without the
   level, requests it. levelAsset is g_FrontendLoadedLevelAsset as read on entry of the caller. */
static void FrontendScenarioSession_LoadFieldGridOfLevel(FrontendLoadedLevelAsset *levelAsset)
{
  FrontendRoleStateFlags *roleFlags;
  uint32_t packetByteCount;
  uint32_t levelPathOffset;
  PckDecodedByteCount sourceImageSizeBytes;
  FrontendLoadedLevelAsset *clientLevelAsset;
  FieldGridAsset *sourceGrid;
  uint32_t dwordsRemaining;
  int otherPlayersRemaining;
  uint8_t *fieldGridPath;
  uint8_t *encodeDestination;
  uint32_t *encodedSourceDwords;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *outgoingDwordCursor;
  void *loadedEntry;
  uint32_t loadErrorCode;
  uint32_t checkedValue;
  Bool8 encodeOk;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  uint32_t allocationError;
  void *allocationPayload;

  levelPathOffset = (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
  /* original quirk: the flag goes to the first player record, not to the one found (see the caller) */
  roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_LEVEL_RECEIVED;
  /* the level's own path (an offset into the asset) with the extension changed to .fld; the loaded grid
     later replaces that offset */
  fieldGridPath = (uint8_t *)levelAsset + levelPathOffset;
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,(uint16_t *)fieldGridPath);
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,(uint16_t *)fieldGridPath,
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
      loadedEntry = Package_LoadEntry((uint16_t *)fieldGridPath,&loadErrorCode);
      checkedValue = FatalError_ExitIfFailed
                          (loadedEntry != NULL ? (uint32_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
      (clientLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedValue;
    }
    else {
      /* Not found (the record one past the last player is written, as in the original) or the
         field grid is not available locally: request it through the transfer mailbox. */
      *(uint32_t *)playerRecord->snapshotPayload = 0;
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = SCENARIO_TRANSFER_FIELD_GRID;
    }
    return;
  }
  loadedEntry = Package_LoadEntry((uint16_t *)fieldGridPath,&loadErrorCode);
  checkedValue = FatalError_ExitIfFailed
                      (loadedEntry != NULL ? (uint32_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
  sourceGrid = (FieldGridAsset *)checkedValue;
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
  encodedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    /* transfer image: the decoded size, then the encoded grid; copied into its own buffer */
    sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
    encodeDestination = g_PackageScratchBuffer + 4;
    *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
    encodeOk = PckCodec_EncodeFieldGrid(PACKAGE_SCRATCH_BUFFER_BYTES - 4,encodeDestination,
                                        sourceImageSizeBytes,sourceGrid,&encodedByteCount,&encodeErrorCode);
    checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
    packetByteCount = checkedValue + 4;
    allocationError = g_MemoryApi.alloc(packetByteCount,&allocationPayload);
    checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uint32_t)allocationPayload,allocationError != 0);
    outgoingDwordCursor = (uint32_t *)checkedValue;
    for (dwordsRemaining = packetByteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
      *outgoingDwordCursor = *encodedSourceDwords;
      encodedSourceDwords++;
      outgoingDwordCursor++;
    }
    UiTransferMailbox_SetOutgoingBuffer(packetByteCount,(uint32_t *)checkedValue);
  }
}


/* Loads the field grid (.fld) of g_FrontendLoadedLevelAsset once (while some player still lacks
   FRONTEND_PLAYER_STATE_LEVEL_RECEIVED): the host or a local game loads it from its package, a host also
   publishes it (PckCodec_EncodeFieldGrid) in the transfer mailbox; a client loads it itself when it has the level
   locally and otherwise requests it through the mailbox (SCENARIO_TRANSFER_FIELD_GRID). Every other player
   that has the level locally counts as ready; then the frontend returns to the main page with code 1.
   Reached as frontend command FRONTEND_COMMAND_LOAD_FIELD_GRID (command-handler format: playerRuntimeId and
   three unused arguments).
   Original quirk: FRONTEND_PLAYER_STATE_LEVEL_RECEIVED is set on the first player block, not on the player the
   scan stopped at (the original sets it through g_FrontendPlayerRuntimeBlocks itself, its scan cursor having
   been reused for the level asset). The flag thus marks "grid loaded on this machine"; a later call would find the
   same other player still unflagged (unless the has-level-locally pass below flagged it) and load again.
*/
void FrontendScenarioSession_LoadOrRequestFieldGrid(uint32_t playerRuntimeId)

{
  FrontendRoleStateFlags *roleFlags;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendLoadedLevelAsset *levelAsset;
  FrontendPlayerRuntimeRecord *playerScanBase;
  FrontendPlayerRuntimeBlockCount playersToCheck;
  FrontendPlayerRuntimeRecord *playerRecord;

  levelAsset = g_FrontendLoadedLevelAsset;
  /* load the grid once if any player still lacks it */
  playersToCheck = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (((playerRecord->factionAssignment).roleStateFlags & FRONTEND_PLAYER_STATE_LEVEL_RECEIVED) == 0) {
      FrontendScenarioSession_LoadFieldGridOfLevel(levelAsset);
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


/* Starts the campaign in row selectedRecordIndex of the campaigns list. The host (or a local game) loads
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
  uint32_t encodedFieldGridBytes;
  uint32_t bundleByteCount;
  int campaignRecordsRemaining;
  uint32_t dwordsRemaining;
  int frontendRoot;
  CampaignAsset *campaignAsset;
  uint8_t *levelRecordCursor; /* campaign asset base advanced by whole CampaignLevelRecords */
  uint8_t *fieldGridPath;
  uint8_t *encodeCursor;
  uint8_t *transferBundleBytes;
  FrontendLoadedLevelAsset *source;
  uint32_t *transferCopyDestination;
  void *loadedEntry;
  uint32_t loadErrorCode;
  uint32_t checkedValue;
  Bool8 encodeOk;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  uint32_t allocationError;
  void *allocationPayload;
  PckDecodedByteCount campaignDecodedSizeBytes;
  
  frontendRoot = g_FrontendRootNode;
  roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_LEVEL_LOADED;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    WidePath_CombineDirectoryAndLeaf
              (g_FrontendScenarioPathScratchUtf16,
               (uint16_t *)((UiListControl *)FRONTEND_UI(frontendRoot,campaignsList))->rowSlots[selectedRecordIndex],
               (uint16_t *)g_CampaignLevelDirectoryUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_CGN,g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(g_FrontendScenarioPathScratchUtf16,&loadErrorCode);
    checkedValue = FatalError_ExitIfFailed
                        (loadedEntry != NULL ? (uint32_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
    campaignAsset = (CampaignAsset *)checkedValue;
    DebugHook_CampaignLoaded(campaignAsset);
    /* CampaignAsset: the first level becomes the current one; find its record. levelRecordCursor is the
       asset base advanced by whole CampaignLevelRecords, so its levels[0] is the record under the cursor. */
    levelRecordCursor = (uint8_t *)campaignAsset;
    campaignRecordsRemaining = campaignAsset->levelRecordCount;
    g_FrontendLoadedCampaignAsset = (uint32_t)campaignAsset;
    campaignAsset->currentLevelId = campaignAsset->firstLevelId;
    do {
      if (campaignAsset->firstLevelId == ((CampaignAsset *)levelRecordCursor)->levels[0].levelId) break;
      levelRecordCursor = levelRecordCursor + sizeof(CampaignLevelRecord);
      campaignRecordsRemaining--;
    } while (campaignRecordsRemaining != 0);
    if (campaignRecordsRemaining == 0) {
      /* No record for the current level. As in the original this check never fails (it passes a cleared
         failure flag); the cursor then points behind the last record. */
      FatalError_ExitIfFailed(0,false);
    }
    FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
    WidePath_CombineDirectoryAndLeaf
              (g_FrontendScenarioPathScratchUtf16,((CampaignAsset *)levelRecordCursor)->levels[0].levelFileName,
               (uint16_t *)g_ScenarioLevelDirectoryUtf16);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(g_FrontendScenarioPathScratchUtf16,&loadErrorCode);
    checkedValue = FatalError_ExitIfFailed
                        (loadedEntry != NULL ? (uint32_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedValue;
    fieldGridPath = (uint8_t *)g_FrontendLoadedLevelAsset +
                    (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,(uint16_t *)fieldGridPath);
    WidePath_CombineDirectoryAndLeaf
              (g_LevelResourcePathScratchUtf16,(uint16_t *)fieldGridPath,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    /* the original does not check this load for failure */
    sourceGrid = Package_LoadEntry((uint16_t *)fieldGridPath,&loadErrorCode);
    if (sourceGrid == NULL) {
      /* Original quirk: the error code is used as the grid */
      sourceGrid = (FieldGridAsset *)loadErrorCode;
    }
    campaignAsset = (CampaignAsset *)g_FrontendLoadedCampaignAsset;
    source = g_FrontendLoadedLevelAsset;
    transferBundleBytes = g_PackageScratchBuffer;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      /* bundle header (6 dwords): decoded sizes of level, campaign and grid, then their encoded sizes;
         the three encoded images follow */
      ((ScenarioCampaignBundleHeader *)g_PackageScratchBuffer)->levelDecodedBytes =
           (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
      campaignDecodedSizeBytes = campaignAsset->decodedSizeBytes;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->fieldGridDecodedBytes =
           (sourceGrid->common).allocationSizeBytes;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->campaignDecodedBytes = campaignDecodedSizeBytes;
      encodeCursor = transferBundleBytes + sizeof(ScenarioCampaignBundleHeader);
      encodeOk = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 24,encodeCursor,(source->header).common.allocationSizeBytes,(uint8_t *)source,
                          &encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      encodedLevelBytes = checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->levelEncodedBytes = encodedLevelBytes;
      encodeCursor = encodeCursor + encodedLevelBytes;
      encodeOk = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes,encodeCursor,
                          campaignAsset->decodedSizeBytes,(uint8_t *)campaignAsset,&encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      encodedCampaignBytes = checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->campaignEncodedBytes = encodedCampaignBytes;
      encodeOk = PckCodec_EncodeFieldGrid
                         ((PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes) - encodedCampaignBytes,encodeCursor + encodedCampaignBytes,
                          (sourceGrid->common).allocationSizeBytes,sourceGrid,&encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      encodedFieldGridBytes = checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->fieldGridEncodedBytes = encodedFieldGridBytes;
      /* header, encoded level and campaign (up to encodeCursor + encodedCampaignBytes), then the grid */
      bundleByteCount = (uint32_t)(encodeCursor - transferBundleBytes) + encodedCampaignBytes + encodedFieldGridBytes;
      allocationError = g_MemoryApi.alloc(bundleByteCount,&allocationPayload);
      checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uint32_t)allocationPayload,allocationError != 0);
      transferCopyDestination = (uint32_t *)checkedValue;
      for (dwordsRemaining = bundleByteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
        *transferCopyDestination = *(uint32_t *)transferBundleBytes;
        transferBundleBytes += 4;
        transferCopyDestination++;
      }
      UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)bundleByteCount,(uint32_t *)checkedValue);
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
  ScenarioCatalogDisplayRecord **rowPointerCursor;
  uint16_t *resolvedText;
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
  void **rowPointerCursor;
  uint16_t *resolvedText;
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
      ((ScenarioCatalogDisplayRecord *)campaignRecord)->titleResolvedText = resolvedText;
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


/* Compares the 0x40-byte identifiers of two catalog records dword by dword. */
static Bool8 ScenarioCatalog_RecordIdentifiersEqual
          (const ScenarioCatalogRecord *firstRecord,const ScenarioCatalogRecord *secondRecord)
{
  const uint32_t *firstDwords = (const uint32_t *)firstRecord->identifier;
  const uint32_t *secondDwords = (const uint32_t *)secondRecord->identifier;
  uint32_t dwordIndex;

  for (dwordIndex = 0; dwordIndex < sizeof(firstRecord->identifier) / 4; dwordIndex++) {
    if (firstDwords[dwordIndex] != secondDwords[dwordIndex]) {
      return false;
    }
  }
  return true;
}


/* Merges sourceByteCount / 0x100 catalog records into destinationRecords, matching them by their 0x40-byte
   UTF-16 identifier: a match is overwritten, a new identifier is appended. Returns the new destination
   record count. Like the original, the loops assume at least one source and one existing destination record.
*/
ScenarioCatalogRecordCount ScenarioCatalog_MergeRecordsByName
          (ScenarioCatalogSourceByteCount sourceByteCount,ScenarioCatalogRecord *sourceRecords,
          ScenarioCatalogRecordCount existingRecordCount,ScenarioCatalogRecord *destinationRecords)

{
  uint32_t sourceRecordsRemaining;
  ScenarioCatalogRecordCount destinationRecordsRemaining;
  ScenarioCatalogRecord *destinationRecordCursor;

  sourceRecordsRemaining = sourceByteCount / SCENARIO_CATALOG_RECORD_SIZE;
  do {
    /* find the destination record with the same identifier */
    destinationRecordsRemaining = existingRecordCount;
    destinationRecordCursor = destinationRecords;
    while (!ScenarioCatalog_RecordIdentifiersEqual(sourceRecords,destinationRecordCursor)) {
      destinationRecordCursor++;
      destinationRecordsRemaining--;
      if (destinationRecordsRemaining == 0) {
        /* No match: append after the existing records (the cursor is already there). */
        existingRecordCount++;
        break;
      }
    }
    /* copy the whole 0x100-byte record */
    *destinationRecordCursor = *sourceRecords;
    sourceRecords++;
    sourceRecordsRemaining--;
  } while (sourceRecordsRemaining != 0);
  return existingRecordCount;
}


/* Handler for starting a single-game level (frontend command 0x920 in a network game): builds the level path,
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
  uint32_t maskWordIndex;
  uint32_t levelRecordOffset; /* byte offset of the level record in the catalog's level section */
  uint32_t packetByteCount;
  uint32_t dwordsRemaining;
  int frontendRoot;
  int otherPlayersRemaining;
  uint8_t *packedDestination;
  uint32_t *packedSourceDwords;
  uint32_t *outgoingDwordCursor;
  void *loadedEntry;
  uint32_t loadErrorCode;
  uint32_t checkedValue;
  Bool8 encodeOk;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  uint32_t allocationError;
  void *allocationPayload;
  Bool8 levelLoadedLocally;

  frontendRoot = g_FrontendRootNode;
  pageStack = (UiPageStackControl *)FRONTEND_UI(g_FrontendRootNode,frontendPageStack);
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             (uint16_t *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                     [selectedRowIndex],
             (uint16_t *)g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,pageStack);
  ((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags =
       ((FrontendModelPointerContext *)FRONTEND_UI(frontendRoot,menuRoomModelView))->contextFlags &
       ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  FrontendState_DispatchCode(1); /* ROM action table entry 1 */
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  roleFlags = &g_FrontendPlayerRuntimeBlocks->factionAssignment.roleStateFlags;
  *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    loadedEntry = Package_LoadEntry(g_FrontendScenarioPathScratchUtf16,&loadErrorCode);
    checkedValue = FatalError_ExitIfFailed
                        (loadedEntry != NULL ? (uint32_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
    packedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
    levelAsset = (FrontendLoadedLevelAsset *)checkedValue;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      /* host: mailbox packet = unpacked size dword + Huffman/RLE-packed level */
      levelSizeBytes = levelAsset->header.common.allocationSizeBytes;
      packedDestination = g_PackageScratchBuffer + 4;
      g_FrontendLoadedLevelAsset = levelAsset;
      *(PckDecodedByteCount *)g_PackageScratchBuffer = levelSizeBytes;
      encodeOk = PckCodec_EncodeHuffmanRle(PACKAGE_SCRATCH_BUFFER_BYTES - 4,packedDestination,levelSizeBytes,
                                           (uint8_t *)levelAsset,&encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      packetByteCount = checkedValue + 4;
      allocationError = g_MemoryApi.alloc(packetByteCount,&allocationPayload);
      checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uint32_t)allocationPayload,allocationError != 0);
      outgoingDwordCursor = (uint32_t *)checkedValue;
      for (dwordsRemaining = packetByteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
        *outgoingDwordCursor = *packedSourceDwords;
        packedSourceDwords++;
        outgoingDwordCursor++;
      }
      UiTransferMailbox_SetOutgoingBuffer(packetByteCount,(uint32_t *)checkedValue);
      levelAsset = g_FrontendLoadedLevelAsset;
    }
  }
  else {
    /* The level's bit in the players' level masks: record offset / 0x100 is the level index, split into
       mask dword (offset >> 13) and bit ((offset >> 8) & 31); there are three mask dwords (96 levels). */
    levelRecordOffset = ((int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                   [selectedRowIndex] -
            (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
    maskWordIndex = levelRecordOffset >> 13;
    /* Client: load the level locally when the local player's level mask has it, else request it. */
    levelLoadedLocally = false;
    if (maskWordIndex < 3) {
      /* find the local player among the other players (block 1..) */
      otherPlayersRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
      playerRecord = g_FrontendPlayerRuntimeBlocks + 1;
      do {
        if (g_LocalPlayerRuntimeId == playerRecord->playerRuntimeId) {
          if (((&playerRecord->scenarioAvailabilityMask0)[maskWordIndex] &
              1 << ((uint8_t)(levelRecordOffset >> 8) & 31)) != 0) {
            /* on failure levelAsset is replaced below */
            levelAsset = Package_LoadEntry(g_FrontendScenarioPathScratchUtf16,NULL);
            levelLoadedLocally = levelAsset != NULL;
          }
          break;
        }
        otherPlayersRemaining--;
        playerRecord++;
      } while (otherPlayersRemaining != 0);
    }
    if (!levelLoadedLocally) {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = SCENARIO_TRANSFER_LEVEL;
      levelAsset = g_FrontendLoadedLevelAsset;
    }
  }
  g_FrontendLoadedLevelAsset = levelAsset;
  levelRecordOffset = ((int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                 [selectedRowIndex] -
          (int)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
  maskWordIndex = levelRecordOffset >> 13;
  if (maskWordIndex < 3) {
    /* every other player (block 1..) */
    playerRecord = g_FrontendPlayerRuntimeBlocks + 1;
    for (playersRemaining = g_FrontendPlayerRuntimeBlockCount - 1; playersRemaining != 0; playersRemaining--) {
      if (((&playerRecord->scenarioAvailabilityMask0)[maskWordIndex] & 1 << ((uint8_t)(levelRecordOffset >> 8) & 31))
          != 0) {
        roleFlags = &playerRecord->factionAssignment.roleStateFlags;
        *roleFlags = *roleFlags | (FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY | FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT);
        playerRecord->transferProgressBytes = INT32_MAX; /* transfer progress: complete */
      }
      playerRecord++;
    }
  }
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
  ScenarioCatalogSaveRecord **rowPointers;
  ScenarioCatalogSaveRecord *saveRecord;
  TextResourceId resourceId;
  int frontendRoot;
  uint16_t *primaryText;
  uint16_t *fieldText;

  frontendRoot = g_FrontendRootNode;
  rowPointers =
       (ScenarioCatalogSaveRecord **)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,savedGamesList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,savedGameDescriptionText))->text =
       (uint16_t *)TEXT_ID_SCENARIO_DESCRIPTION_EMPTY;
  if (rowPointers != NULL) {
    UiPointerList_SelectColumnListIndex(selectionIndex,control);
    saveRecord = rowPointers[selectionIndex];
    if (saveRecord->campaignTitleTextId < 0) {
      resourceId = saveRecord->levelTitleTextId;
      primaryText = TextResource_Resolve(resourceId);
      /* the first code unit becomes palette colour 0 */
      *primaryText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text = (uint16_t *)resourceId;
    }
    else {
      primaryText = TextResource_Resolve(TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE);
      fieldText = TextResource_Resolve(saveRecord->levelTitleTextId);
      *fieldText = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_COLOR_PALETTE_0;
      RichTextCommandStream_PatchPayloadBySelector(1,fieldText,primaryText);
      fieldText = TextResource_Resolve(saveRecord->campaignTitleTextId);
      RichTextCommandStream_PatchPayloadBySelector(0,fieldText,primaryText);
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text =
           (uint16_t *)TEXT_ID_SAVED_GAME_DESCRIPTION_TEMPLATE;
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
  int scenarioPathPointerTableAddress;

  /* selectionControl is the frontend template's gameSelectStartButton; the other nodes are its siblings. */
  if (!UiSelectableGroup_FindVisibleSelected(NULL,&selectedTabIndex,3,
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,loadGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,singleGameTabButton),
      THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsTabButton))) {
    return;
  }
  if (selectedTabIndex != 0) {
    if (selectedTabIndex < 2) {
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = NULL;
      selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                        ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,missionsList),
                         NULL);
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
    selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                      ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,campaignsList),
                       NULL);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,selectedRowIndex)
      ;
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_LOAD_CAMPAIGN,0,0,selectedRowIndex);
    }
    return;
  }
  scenarioPathPointerTableAddress =
       (int)((UiListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList))->rowSlots;
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = NULL;
  selectedRowIndex = UiPointerList_GetSelectedIndexAndConfirmed
                    ((UiPointerListControl *)THANDOR_UI_SIBLING(selectionControl,FrontendUiImage,gameSelectStartButton,savedGamesList),
                     NULL);
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             *(uint16_t **)(scenarioPathPointerTableAddress + selectedRowIndex * 4),
             (uint16_t *)g_SaveDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_SVE,g_FrontendScenarioPathScratchUtf16);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,2);
  }
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

