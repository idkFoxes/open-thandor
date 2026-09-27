/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/scenario/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/scenario/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/scenario/catalog. */

/* Address: 0x00549E50.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[57]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[57] (0x2039). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordLocalizedText,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyLocalizedTextSelection(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexVariantB(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordLocalizedText(g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)THANDOR_UI_AT(listControl,-0x2c8)
             /* savedGamesList -> gameSelectStartButton */);
  return;
}


/* Address: 0x00549EB0.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[58]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[58] (0x203A). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordField70DisplayId,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField70Selection(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexVariantB(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                (g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)THANDOR_UI_AT(listControl,-0x490)
             /* missionsList -> gameSelectStartButton */);
  return;
}


/* Address: 0x00549F10.
   Ownership: assets/scenario/catalog.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[59]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[59] (0x203B). Return datatype is preserved for non-queue direct callers.
   Local calls: ScenarioCatalog_RefreshSelectedRecordField50DisplayId,
   FrontendScenarioSelection_ActivateSelectedRecord.
   Cross-module calls: UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioSelection_ApplyField50Selection(UiPointerListControl *listControl)

{
  ListSelectionResult selectedRow;
  
  selectedRow = UiPointerList_GetSelectedIndexVariantB(listControl);
  if (!selectedRow.confirmed) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RefreshSelectedRecordField50DisplayId
                (g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12f0,0,0,selectedRow.rowIndex);
    }
    return;
  }
  FrontendScenarioSelection_ActivateSelectedRecord
            ((FrontendScenarioSelectionControlAddress32)THANDOR_UI_AT(listControl,-0x670)
             /* campaignsList -> gameSelectStartButton */);
  return;
}


/* Address: 0x0054A280.
   Opens page 10 "Choose game" (tabs Load game / Single game / Campaigns) and fills the list of the active tab;
   network sessions always use the Single game tab. Network clients only watch: all buttons are hidden. With
   the command-line option KARTE="<level>" matching a single mission, that mission is selected and loaded
   directly; network hosts never get the Load game tab.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelectionPage_InitializeAndApplyMapOption
          (FrontendScenarioSelectionPageView26C4 *scenarioSelectionPage)

{
  /* scenarioOptionRow0..4 are the page's Cancel, Start, Load game, Single game and Campaigns buttons. */
  UiNodeFlags *controlFlags;
  short codeUnit;
  CommandPayloadDword04 selectionIndex;
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
    /* compactLayoutControl.nodeFlags is the menuRoomModelView field +0x4C; 0x2000 selects the compact layout. */
    controlFlags = &(scenarioSelectionPage->compactLayoutControl).nodeFlags;
    *controlFlags = *controlFlags | 0x2000;
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
  mapOption = g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
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
  mapOption = g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
  textCursor = mapOption.option;
  if (!mapOption.notFound) {
    /* The level name runs from after KARTE=" to the closing quote, which must end the command line. */
    remainingCount = 0x3fffff;
    for (scanCursor = textCursor + 7; *scanCursor != '"'; scanCursor++) {
      if ((*scanCursor < ' ') || (remainingCount--, remainingCount == 0))
      goto
      FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
      ;
    }
    *scanCursor = 0;
    if (scanCursor[1] == 0) {
      Text_CopyNarrowToUtf16(0x800000,(uint16_t *)g_PackageScratchBuffer,textCursor + 7);
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
            textCursor = textCursor + 2;
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
            /* Select the mission and load it: directly, or in a network session through the command queue
               (0x12A0 / 0x920 are the command codes of the two functions). */
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              ScenarioCatalog_RefreshSelectedRecordField70DisplayId
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,selectionIndex);
            }
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendScenarioSession_LoadOrRequestLevelAsset
                        (g_LocalPlayerRuntimeId,0,0,selectionIndex);
            }
            else {
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,selectionIndex);
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

  FrontendScenarioSelectionPage_InitializeAndApplyMapOption_UpdateNetworkRoleActionAvailabilityAndReturn
  :
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
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the save-record heading, and either rebuilds and refreshes
   locally or queues commands 0x0E70 and 0x11F0. Queued UI action handler for
   FRONTEND_PAGE20[53],FRONTEND_PAGE20[82] (0x2035,0x2052). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildSaveRecordListPage, ScenarioCatalog_RefreshSelectedRecordLocalizedText.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendScenarioPage_OpenSaveRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,savedGameDescriptionText))->text = (uint16_t *)0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildSaveRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xe70,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordLocalizedText(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x11f0,0,0,0);
  }
  return;
}


/* Address: 0x0054A690.
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the level-record heading, and either rebuilds and refreshes
   locally or queues commands 0x0F50 and 0x12A0. Queued UI action handler for
   FRONTEND_PAGE20[54],FRONTEND_PAGE20[83] (0x2036,0x2053). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildLevelRecordListPage, ScenarioCatalog_RefreshSelectedRecordField70DisplayId.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax FrontendScenarioPage_OpenLevelRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,missionDescriptionText))->text = (uint16_t *)0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildLevelRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xf50,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordField70DisplayId(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12a0,0,0,0);
  }
  return;
}


/* Address: 0x0054A710.
   Ownership: assets/scenario/catalog.
   Purpose: Walks to the scenario-page root, selects the campaign-record heading, and either rebuilds and refreshes
   locally or queues commands 0x10D0 and 0x12F0. Queued UI action handler for
   FRONTEND_PAGE20[55],FRONTEND_PAGE20[84] (0x2037,0x2054). Return datatype is preserved for non-queue direct
   callers.
   Local calls: ScenarioCatalog_RebuildCampaignRecordListPage,
   ScenarioCatalog_RefreshSelectedRecordField50DisplayId.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendScenarioPage_OpenCampaignRecordsAndRefresh(UiNodeBase *sourceNode)

{
  UiNodeBase *parentCursor;
  
  parentCursor = sourceNode->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    sourceNode = sourceNode->parent;
    parentCursor = sourceNode->parent;
  }
  ((UiWrappedTextControl *)FRONTEND_UI(sourceNode,campaignDescriptionText))->text = (uint16_t *)0x215d;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RebuildCampaignRecordListPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x10d0,0,0,0);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    ScenarioCatalog_RefreshSelectedRecordField50DisplayId(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x12f0,0,0,0);
  }
  return;
}


/* Address: 0x00549A70.
   Ownership: assets/scenario/catalog.
   Purpose: Starts the field-grid load directly in local mode or queues command 0x460 in shared frontend modes.
   Queued UI action handler for FRONTEND_PAGE20[65] (0x2041). Return datatype is preserved for non-queue direct
   callers.
   Local calls: FrontendScenarioSession_LoadOrRequestFieldGrid.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendScenarioAction_StartFieldGridLoad(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendScenarioSession_LoadOrRequestFieldGrid(g_LocalPlayerRuntimeId);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x460,0,0,0);
  }
  return;
}


/* Address: 0x00549FD0.
   Rebuilds g_ScenarioCatalog, the list behind the "Choose game" tabs: the single missions of level\level.dat
   and the campaigns of level\campagne.dat, each updated by the add-on files level00..99.dat /
   campagne00..99.dat (records merged by name), followed by the header record of every save\*.sve.
   The catalog is also what a network host sends to its clients.
*/
void __thandor_void_preserve_eax_ecx_edx ScenarioCatalog_Rebuild(void)

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
             ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset);
    /* REP MOVSD of the whole file into the level section */
    levelSourceDword = (uint32_t *)loadedResource.bufferOrError;
    recordCopyCursor = recordsBase;
    for (dwordsRemaining = loadedResource.byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
      *(uint32_t *)recordCopyCursor->identifier = *levelSourceDword;
      levelSourceDword++;
      recordCopyCursor = (ScenarioCatalogRecord *)(recordCopyCursor->identifier + 2);
    }
    Resource_Release((uint32_t *)loadedResource.bufferOrError);
    /* level00.dat .. level99.dat: the two digit code units are packed as one dword, 0x300030 = "00"; the
       units digit counts '0'..'9', then subtracting 0x9FFFF resets it to '0' and increments the tens digit. */
    g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
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
                  g_ScenarioLevelDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff,
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
             ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->campaignRecordsOffset
             );
    campaignSourceDword = (uint32_t *)loadedResource.bufferOrError;
    recordCopyCursor = recordsBase;
    for (dwordsRemaining = loadedResource.byteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
      *(uint32_t *)recordCopyCursor->identifier = *campaignSourceDword;
      campaignSourceDword++;
      recordCopyCursor = (ScenarioCatalogRecord *)(recordCopyCursor->identifier + 2);
    }
    Resource_Release((uint32_t *)loadedResource.bufferOrError);
    /* campagne00.dat .. campagne99.dat, counted like the level files above */
    g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits = 0x300030;
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
                  g_ScenarioCampaignDataPathTemplateUtf16.decimalDigits.packedDigits - 0x9ffff,
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
                     (FILESYSTEM_ENUMERATE_FILES,0xffffffff,0x800000,g_PackageScratchBuffer,
                      &g_ScenarioCatalogPathScratchUtf16);
  catalog = g_ScenarioCatalog;
  saveFilesRemaining = saveEnumeration.entryCount;
  if ((!saveEnumeration.failed) && (saveFilesRemaining != 0)) {
    saveRecord = (ScenarioCatalogSaveRecord *)
                 ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->saveRecordsOffset
                 );
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
      saveRecord->localizedStringId70 = saveRecord->localizedStringId70 + TEXT_ID_LEVEL_TITLE_BASE;
      if (-1 < saveRecord->optionalLocalizedStringId90) {
        saveRecord->optionalLocalizedStringId90 =
             saveRecord->optionalLocalizedStringId90 + TEXT_ID_CAMPAIGN_TITLE_BASE;
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
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RequestRomTransitionStopCallback(uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
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
void __thandor_void_preserve_eax_ecx_edx FrontendScenarioTransfer_ProcessReceivedAsset(void)

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
  FrontendLoadedLevelRuntimeImage370 *levelAsset;
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
          receivedDwords = (uint32_t *)((int)&g_ScenarioCatalog->levelRecordsOffset +
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
                  (0xe00,changedLevelMask[1],changedLevelMask[2],changedLevelMask[3]);
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
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
        PckCodec_DecodeHuffmanRle
                  (payloadSizeBytes,(uint8_t *)g_FrontendLoadedLevelAsset,received.byteCount - 4,(uint8_t *)(receivedDwords + 1));
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_MarkFlag02ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x8d0,0,0,0);
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
          FrontendPlayerRuntime_MarkFlag08ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x360,0,0,0);
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
        g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
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
        levelPathOrCurrentLevelField = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.
                 assetRelativeAddressAnchor28 +
                 ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
                 0x28);
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
          FrontendPlayerRuntime_MarkFlag04ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x410,0,0,0);
        }
        g_MemoryApi.free(receivedDwords);
        UiTransferMailbox_ClearReceivedState();
        streamOrRecordCursor = g_FrontendLoadedCampaignAsset;
        g_FrontendScenarioTransferState = SCENARIO_TRANSFER_NONE;
        /* The campaign starts at its first level (+0xB4, stored as current level +0xC4): find that level's
           record (0x180 bytes each, id at +0x300 and file name at +0x30C from the asset base, +0xB8 = count)
           and build level\<name>.lev. */
        levelPathOrCurrentLevelField = g_FrontendLoadedCampaignAsset + 0xb4;
        maskSlotOrRecordsLeft = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
        *(int *)(g_FrontendLoadedCampaignAsset + 0xc4) = *(int *)levelPathOrCurrentLevelField;
        do {
          if (*(int *)levelPathOrCurrentLevelField == *(int *)(streamOrRecordCursor + 0x300)) break;
          streamOrRecordCursor = streamOrRecordCursor + 0x180;
          maskSlotOrRecordsLeft--;
        } while (maskSlotOrRecordsLeft != 0);
        WidePath_CombineDirectoryAndLeaf
                  (&g_FrontendScenarioPathScratchUtf16,(uint16_t *)(streamOrRecordCursor + 0x30c),
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
        levelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
        g_FrontendLoadedLevelAsset = levelAsset;
        PckCodec_DecodeHuffmanRle(*receivedDwords,(uint8_t *)levelAsset,receivedDwords[2],(uint8_t *)(receivedDwords + 4));
        payloadSizeBytes = receivedDwords[2];
        levelPathOrCurrentLevelField = (levelAsset->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid - 0x28);
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
          FrontendPlayerRuntime_MarkFlag04ById(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x410,0,0,0);
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
   Ownership: assets/scenario/catalog.
   Purpose: Loads the selected level field-grid locally or obtains it through the frontend transfer mailbox,
   optionally publishes an encoded host copy, updates player readiness fields, and returns to the main frontend
   page. Typed parameters: p2 selectedLevelIndex→UiListRowIndex_V300. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], WidePath_CombineDirectoryAndLeaf
   [core/text/path], Package_LoadEntry [assets/package/runtime], PckCodec_EncodeFieldGrid [assets/package/codec],
   UiTransferMailbox_SetOutgoingBuffer [network/protocol/transfer], UiTransferMailbox_MarkUnavailable
   [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestFieldGrid(UiListRowIndex selectedLevelIndex)

{
  FrontendRoleStateFlags *roleFlags;
  uint32_t bytes;
  uint32_t levelPathOffset;
  PckDecodedByteCount sourceImageSizeBytes;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendLoadedLevelRuntimeImage370 *levelAsset;
  FrontendLoadedLevelRuntimeImage370 *clientLevelAsset;
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
    if (((playerRecord->factionAssignment).roleStateFlags & 8) == 0) {
      levelPathOffset = (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
      roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | 8;
      pathOrEncodeBuffer = (levelAsset->header).common.buildMetadata.assetRelativeAddressAnchor28 + (levelPathOffset - 0x28);
      WidePath_SetExtensionCode(0x646c66,(uint16_t *)pathOrEncodeBuffer);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)pathOrEncodeBuffer,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      clientLevelAsset = g_FrontendLoadedLevelAsset;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
        /* Client: find the local player among the other players. */
        playerRecord = g_FrontendPlayerRuntimeBlocks + 1;
        otherPlayersRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
        while (g_LocalPlayerRuntimeId != playerRecord->playerRuntimeId) {
          playerRecord = playerRecord + 1;
          otherPlayersRemaining = otherPlayersRemaining + -1;
          if (otherPlayersRemaining == 0) break;
        }
        if ((otherPlayersRemaining != 0) && (((playerRecord->factionAssignment).roleStateFlags & 0x10) != 0)) {
          loadedEntry = Package_LoadEntry((uint16_t *)pathOrEncodeBuffer);
          checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
          (clientLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = checkedResult.valueOrError;
        }
        else {
          /* Not found (the record one past the last player is written, as in the original) or the
             field grid is not available locally: request it through the transfer mailbox. */
          *(uint32_t *)playerRecord->snapshotPayloadB0_13AF = 0;
          UiTransferMailbox_MarkUnavailable();
          g_FrontendScenarioTransferState = 3;
        }
        break;
      }
      loadedEntry = Package_LoadEntry((uint16_t *)pathOrEncodeBuffer);
      checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
      sourceGrid = (FieldGridAsset *)checkedResult.valueOrError;
      (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
      encodedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
        pathOrEncodeBuffer = g_PackageScratchBuffer + 4;
        *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
        encodeResult = PckCodec_EncodeFieldGrid(0x7ffffc,pathOrEncodeBuffer,sourceImageSizeBytes,sourceGrid);
        checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
        bytes = checkedResult.valueOrError + 4;
        allocation = g_MemoryApi.alloc(bytes);
        checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
        outgoingDwordCursor = (uint32_t *)checkedResult.valueOrError;
        for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
          *outgoingDwordCursor = *encodedSourceDwords;
          encodedSourceDwords = encodedSourceDwords + 1;
          outgoingDwordCursor = outgoingDwordCursor + 1;
        }
        UiTransferMailbox_SetOutgoingBuffer(bytes,(uint32_t *)checkedResult.valueOrError);
      }
      break;
    }
    playerRecord = playerRecord + 1;
    playersToCheck = playersToCheck - 1;
  } while (playersToCheck != 0);
  /* Every other player that has the field grid (flag 0x10) becomes ready (flag 8). */
  playerScanBase = g_FrontendPlayerRuntimeBlocks;
  for (playersRemaining = g_FrontendPlayerRuntimeBlockCount - 1; playersRemaining != 0;
      playersRemaining = playersRemaining - 1) {
    playerScanBase = playerScanBase + 1;
    if ((playerScanBase->factionAssignment.roleStateFlags & 0x10) != 0) {
      roleFlags = &playerScanBase->factionAssignment.roleStateFlags;
      *roleFlags = *roleFlags | 8;
      playerScanBase->runtimeState70 = 0x7fffffff;
    }
  }
  FrontendSession_ReturnToMainPage(selectedLevelIndex,0,0,1);
  return;
}


/* Address: 0x00544AC0.
   Ownership: assets/scenario/catalog.
   Purpose: Loads the selected campaign image, resolves its level and field-grid assets, optionally publishes the
   three encoded images as one transfer bundle, initializes faction assignments, and switches the frontend state to
   the campaign path.
   Cross-module calls: WidePath_CombineDirectoryAndLeaf [core/text/path], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime],
   PckCodec_EncodeHuffmanRle [assets/package/codec], PckCodec_EncodeFieldGrid [assets/package/codec].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestCampaignBundle
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
  FrontendLoadedLevelRuntimeImage370 *source;
  uint32_t *transferCopyDestination;
  PackageLoadResult loadedEntry;
  FatalErrorCheckResult checkedResult;
  PckCodecResult encodeResult;
  ArenaAllocResult allocation;
  PckDecodedByteCount campaignDecodedSizeBytes;
  
  frontendRoot = g_FrontendRootNode;
  roleFlags = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
  *roleFlags = *roleFlags | 4;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,
               (uint16_t *)((UiListControl *)FRONTEND_UI(frontendRoot,campaignsList))->rowSlots[selectedRecordIndex],
               (uint16_t *)u_level_0050dab8);
    WidePath_SetExtensionCode(0x6e6763,&g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
    recordOrEncodeCursor = (uint8_t *)checkedResult.valueOrError;
    cursorOrSize = recordOrEncodeCursor + 0xb4;
    campaignRecordsRemaining = *(int *)(recordOrEncodeCursor + 0xb8);
    g_FrontendLoadedCampaignAsset = recordOrEncodeCursor;
    *(int *)(recordOrEncodeCursor + 0xc4) = *(int *)cursorOrSize;
    do {
      if (*(int *)cursorOrSize == *(int *)(recordOrEncodeCursor + 0x300)) break;
      recordOrEncodeCursor = recordOrEncodeCursor + 0x180;
      campaignRecordsRemaining = campaignRecordsRemaining + -1;
    } while (campaignRecordsRemaining != 0);
    if (campaignRecordsRemaining == 0) {
      /* No record for the selected level. */
      FatalError_ExitIfFailed(0,false);
    }
    if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
       (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
      Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                               levelPathOffsetOrLoadedFieldGrid);
    }
    Resource_Release(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
    WidePath_CombineDirectoryAndLeaf
              (&g_FrontendScenarioPathScratchUtf16,(uint16_t *)(recordOrEncodeCursor + 0x30c),(uint16_t *)u_level_0050daac
              );
    WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
    loadedEntry = Package_LoadEntry(&g_FrontendScenarioPathScratchUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)loadedEntry.bufferOrError,loadedEntry.failed);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
    cursorOrSize = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.assetRelativeAddressAnchor28
             + ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
               0x28);
    WidePath_SetExtensionCode(0x646c66,(uint16_t *)cursorOrSize);
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)cursorOrSize,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    loadedEntry = Package_LoadEntry((uint16_t *)cursorOrSize);
    cursorOrSize = g_FrontendLoadedCampaignAsset;
    source = g_FrontendLoadedLevelAsset;
    transferBundleBytes = g_PackageScratchBuffer;
    sourceGrid = loadedEntry.bufferOrError;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      *(AssetAllocationSizeBytes *)g_PackageScratchBuffer =
           (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
      campaignDecodedSizeBytes = *(PckDecodedByteCount *)(cursorOrSize + 4);
      *(AssetAllocationSizeBytes *)(transferBundleBytes + 8) =
           (sourceGrid->common).allocationSizeBytes;
      *(PckDecodedByteCount *)(transferBundleBytes + 4) = campaignDecodedSizeBytes;
      recordOrEncodeCursor = transferBundleBytes + 0x18;
      encodeResult = PckCodec_EncodeHuffmanRle
                         (0x7fffe8,recordOrEncodeCursor,(source->header).common.allocationSizeBytes,(uint8_t *)source
                         );
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      encodedLevelBytes = checkedResult.valueOrError;
      *(uint32_t *)(transferBundleBytes + 0xc) = encodedLevelBytes;
      recordOrEncodeCursor = recordOrEncodeCursor + encodedLevelBytes;
      encodeResult = PckCodec_EncodeHuffmanRle
                         (0x7fffe8 - encodedLevelBytes,recordOrEncodeCursor,*(PckDecodedByteCount *)(cursorOrSize + 4),cursorOrSize);
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      encodedCampaignBytes = checkedResult.valueOrError;
      *(uint32_t *)(transferBundleBytes + 0x10) = encodedCampaignBytes;
      encodeResult = PckCodec_EncodeFieldGrid
                         ((0x7fffe8 - encodedLevelBytes) - encodedCampaignBytes,recordOrEncodeCursor + encodedCampaignBytes,
                          (sourceGrid->common).allocationSizeBytes,sourceGrid);
      checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
      *(uint32_t *)(transferBundleBytes + 0x14) = checkedResult.valueOrError;
      cursorOrSize = recordOrEncodeCursor + encodedCampaignBytes + (checkedResult.valueOrError - (int)transferBundleBytes);
      allocation = g_MemoryApi.alloc((uint32_t)cursorOrSize);
      checkedResult = FatalError_ExitIfFailed(allocation.payloadOrError,allocation.failed);
      transferCopyDestination = (uint32_t *)checkedResult.valueOrError;
      for (dwordsRemaining = (uint32_t)cursorOrSize >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
        *transferCopyDestination = *(uint32_t *)transferBundleBytes;
        transferBundleBytes = transferBundleBytes + 4;
        transferCopyDestination = transferCopyDestination + 1;
      }
      UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)cursorOrSize,(uint32_t *)checkedResult.valueOrError);
    }
    (source->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
    FrontendPlayerRuntime_InitializeFactionAssignments();
  }
  else {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 4;
  }
  FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,uint32_t) =
       FRONTEND_UI_FIELD(frontendRoot,menuRoomModelView,0x4C,uint32_t) & 0xffffdfff;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  OldUnitRuntime_ResetPendingTables();
  FrontendState_DispatchCode(3);
  return;
}


/* Address: 0x00544DC0.
   Ownership: assets/scenario/catalog.
   Purpose: Four-argument frontend callback. It selects the save-record page, constructs a pointer list over exact
   0x100-byte save records from ScenarioCatalogHeader.saveRecordsOffset, rebuilds the list control, and enables
   action 0x2038 only when at least one save record exists. EAX is preserved. Special action-table slot
   FRONTEND_PAGE20[85] uses four stack arguments and RET 0x10; it is deliberately partitioned from the generic
   queued handler ABI.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiPointerList_SortByDwordPairFieldDescending [ui/controls/lists], UiPointerList_SelectIndexVariantB
   [ui/controls/lists], UiNodeList_UnsuppressActionId [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildSaveRecordListPage
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4)

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
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    rowCount = g_ScenarioCatalog->saveRecordCount;
    saveRecord = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->saveRecordsOffset);
    rowPointers = (void **)(rowCount * 0x100 + (int)saveRecord);
    rowPointerCursor = rowPointers;
    for (remainingRows = rowCount; remainingRows != 0; remainingRows = remainingRows - 1) {
      *rowPointerCursor = saveRecord;
      rowPointerCursor = rowPointerCursor + 1;
      saveRecord = (void *)((int)saveRecord + 0x100);
    }
    control = (int32_t *)FRONTEND_UI(firstNode,savedGamesList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordPairFieldDescending(0xf0,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      UiNodeList_UnsuppressActionId(0x2038,firstNode);
      return;
    }
  }
  UiPointerList_InitializeColumnLayout
            (0,(void **)0x0,(UiPointerListControl *)FRONTEND_UI(firstNode,savedGamesList));
  UiNodeList_SuppressActionId(0x2038,firstNode);
  return;
}


/* Address: 0x00544EA0.
   Ownership: assets/scenario/catalog.
   Purpose: Four-argument frontend callback. It selects the level-record page, constructs a pointer list over exact
   0x100-byte level records, resolves four localized display fields per record, rebuilds the list control, and
   toggles actions 0x2038 and 0x203A according to availability and mode. EAX is preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], TextResource_Resolve [assets/text/resources], UiPointerList_InitializeColumnLayout
   [ui/controls/lists], UiPointerList_SortByExpandedTextFieldAscending [ui/controls/text],
   UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildLevelRecordListPage
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4)

{
  int32_t *control;
  UiNodeBase *firstNode;
  UiListRowCount remainingRows;
  ScenarioCatalogRuntimeExpandedRecord100 *scenarioRecord;
  ScenarioCatalogRuntimeExpandedRecord100 **rowPointerCursor;
  TextResolveResult resolvedText;
  UiListRowCount rowCount;
  ScenarioCatalogRuntimeExpandedRecord100 **rowPointers;
  
  firstNode = g_FrontendRootNode;
  UiSelectableGroup_SelectExclusive(3,FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton));
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    remainingRows = g_ScenarioCatalog->levelRecordCount;
    scenarioRecord =
         (ScenarioCatalogRuntimeExpandedRecord100 *)
         ((int)&g_ScenarioCatalog->levelRecordsOffset + g_ScenarioCatalog->levelRecordsOffset);
    rowPointerCursor = (ScenarioCatalogRuntimeExpandedRecord100 **)(scenarioRecord + remainingRows);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows = remainingRows - 1) {
      *rowPointerCursor = scenarioRecord;
      resolvedText = TextResource_Resolve(scenarioRecord->titleTextResourceId + 0x220a);
      scenarioRecord->titleDisplayTag = 0x8019;
      scenarioRecord->titleResolvedText = resolvedText.text;
      resolvedText = TextResource_Resolve(scenarioRecord->subtitleTextResourceId + 0x2200);
      scenarioRecord->subtitleDisplayTag = 0x8019;
      scenarioRecord->subtitleResolvedText = resolvedText.text;
      resolvedText = TextResource_Resolve(scenarioRecord->scenarioTextResourceId + 0x2230);
      *resolvedText.text = 0x8001;
      scenarioRecord->scenarioDisplayTag = 0x8019;
      scenarioRecord->scenarioResolvedText = resolvedText.text;
      resolvedText = TextResource_Resolve(scenarioRecord->modeTextResourceId + 0x2205);
      scenarioRecord->modeDisplayTag = 0x8019;
      scenarioRecord->modeResolvedText = resolvedText.text;
      rowPointerCursor = rowPointerCursor + 1;
      scenarioRecord = scenarioRecord + 1;
    }
    control = (int32_t *)FRONTEND_UI(firstNode,missionsList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByExpandedTextFieldAscending(0x74,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(0x2038,firstNode);
        UiNodeList_UnsuppressActionId(0x203a,firstNode);
        return;
      }
    }
    else {
      UiPointerList_InitializeColumnLayout
                (0,(void **)0x0,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,(void **)0x0,(UiPointerListControl *)FRONTEND_UI(firstNode,missionsList));
  }
  UiNodeList_SuppressActionId(0x2038,firstNode);
  UiNodeList_SuppressActionId(0x203a,firstNode);
  return;
}


/* Address: 0x00545020.
   Ownership: assets/scenario/catalog.
   Purpose: Selects the campaign scenario page, builds its record-pointer list, resolves localized campaign labels,
   sorts by record field 0x50, and updates actions 0x2038 and 0x203B.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists], TextResource_Resolve
   [assets/text/resources], UiPointerList_SortByDwordFieldAscending [ui/controls/lists],
   UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RebuildCampaignRecordListPage
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

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
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)FRONTEND_UI(firstNode,gameSelectTabStack));
  if (g_ScenarioCatalog != (ScenarioCatalogHeader *)0x0) {
    remainingRows = g_ScenarioCatalog->campaignRecordCount;
    campaignRecord = (void *)((int)&g_ScenarioCatalog->levelRecordsOffset +
                     g_ScenarioCatalog->campaignRecordsOffset);
    rowPointerCursor = (void **)(remainingRows * 0x100 + (int)campaignRecord);
    rowCount = remainingRows;
    rowPointers = rowPointerCursor;
    for (; remainingRows != 0; remainingRows = remainingRows - 1) {
      *rowPointerCursor = campaignRecord;
      resolvedText = TextResource_Resolve(*(int *)((int)campaignRecord + 0x50) + 0x2220);
      *(uint16_t *)((int)campaignRecord + 0x54) = 0x8019;
      *(uint16_t **)((int)campaignRecord + 0x56) = resolvedText.text;
      rowPointerCursor = rowPointerCursor + 1;
      campaignRecord = (void *)((int)campaignRecord + 0x100);
    }
    control = (int32_t *)FRONTEND_UI(firstNode,campaignsList);
    if (rowCount != 0) {
      UiPointerList_InitializeColumnLayout(rowCount,rowPointers,(UiPointerListControl *)control);
      UiPointerList_SortByDwordFieldAscending(0x50,(UiPointerListControl *)control);
      UiPointerList_SelectIndexVariantB(0,(UiPointerListControl *)control);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
        UiNodeList_UnsuppressActionId(0x2038,firstNode);
        UiNodeList_UnsuppressActionId(0x203b,firstNode);
        return;
      }
    }
    else {
      UiPointerList_InitializeColumnLayout
                (0,(void **)0x0,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
    }
  }
  else {
    UiPointerList_InitializeColumnLayout
              (0,(void **)0x0,(UiPointerListControl *)FRONTEND_UI(firstNode,campaignsList));
  }
  UiNodeList_SuppressActionId(0x2038,firstNode);
  UiNodeList_SuppressActionId(0x203b,firstNode);
  return;
}


/* Address: 0x00549F70.
   Merges sourceByteCount / 0x100 catalog records into destinationRecords, matching them by their 0x40-byte
   UTF-16 identifier: a match is overwritten, a new identifier is appended. Returns the new destination
   record count (EDX in the original). The original scans with REPE CMPSD and copies with REP MOVSD; like it,
   the loops assume at least one source and one existing destination record.
*/
ScenarioCatalogRecordCount __thandor_void_preserve_eax_ecx
ScenarioCatalog_MergeRecordsByName
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
    dwordsRemaining = 0x10;
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
    for (copyDwordsRemaining = 0x40; copyDwordsRemaining != 0; copyDwordsRemaining--) {
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
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex)

{
  FrontendRoleStateFlags *roleFlags;
  UiPageStackControl *pageStack;
  PckDecodedByteCount levelSizeBytes;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendLoadedLevelRuntimeImage370 *levelAsset;
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
  WidePath_SetExtensionCode(0x76656c /* "lev" */,&g_FrontendScenarioPathScratchUtf16);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,pageStack);
  FRONTEND_UI_FIELD(rootOrRemaining,menuRoomModelView,0x4C,uint32_t) =
       FRONTEND_UI_FIELD(rootOrRemaining,menuRoomModelView,0x4C,uint32_t) & ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
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
    levelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
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
          if ((*(uint32_t *)(playerCursor[1].reserved78_7F + maskWordIndex * 4 + 0xc) &
              1 << ((uint8_t)(byteCountOrOffset >> 8) & 0x1f)) != 0) {
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
      if ((*(uint32_t *)(playerRecord[1].reserved78_7F + maskWordIndex * 4 + 0xc) & 1 << ((uint8_t)(byteCountOrOffset >> 8) & 0x1f))
          != 0) {
        roleFlags = &playerRecord[1].factionAssignment.roleStateFlags;
        *roleFlags = *roleFlags | (FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY | FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT);
        playerRecord[1].runtimeState70 = 0x7fffffff; /* transfer progress: complete */
      }
    }
  }
  return;
}


/* Address: 0x00545140.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the first scenario list. It resolves the selected record, updates the
   output resource ID, and patches localized fields +0x70 and +0x90 when the latter is nonnegative. Typed
   parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordLocalizedText
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
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,savedGameDescriptionText))->text = (uint16_t *)0x215d;
  if (rowTableOrRecord != 0) {
    UiPointerList_SelectIndexVariantB(selectionIndex,control);
    rowTableOrRecord = *(int *)(rowTableOrRecord + selectionIndex * 4);
    if (*(int *)(rowTableOrRecord + 0x90) < 0) {
      resourceId = *(TextResourceId *)(rowTableOrRecord + 0x70);
      primaryText = TextResource_Resolve(resourceId);
      *primaryText.text = 0x8000;
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text = (uint16_t *)resourceId;
    }
    else {
      primaryText = TextResource_Resolve(0x215e);
      fieldText = TextResource_Resolve(*(TextResourceId *)(rowTableOrRecord + 0x70));
      *fieldText.text = 0x8000;
      RichTextCommandStream_PatchPayloadBySelector(1,fieldText.text,primaryText.text);
      fieldText = TextResource_Resolve(*(TextResourceId *)(rowTableOrRecord + 0x90));
      RichTextCommandStream_PatchPayloadBySelector(0,fieldText.text,primaryText.text);
      ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,savedGameDescriptionText))->text = (uint16_t *)0x215e;
    }
  }
  return;
}


/* Address: 0x00545240.
   Ownership: assets/scenario/catalog.
   Purpose: Selection-change callback for the third scenario list. It derives the output display ID as record field
   +0x50 plus 0x230000. Typed parameters: p3 selectionIndex→UiListRowIndex_V300. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: UiPointerList_SelectIndexVariantB [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField50DisplayId
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,UiListRowIndex selectionIndex)

{
  UiPointerListControl *control;
  int rowPointers;
  int frontendRoot;
  
  frontendRoot = g_FrontendRootNode;
  rowPointers = (int)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList))->rowSlots;
  control = (UiPointerListControl *)FRONTEND_UI(g_FrontendRootNode,campaignsList);
  ((UiWrappedTextControl *)FRONTEND_UI(g_FrontendRootNode,campaignDescriptionText))->text = (uint16_t *)0x215d;
  if (rowPointers != 0) {
    UiPointerList_SelectIndexVariantB(selectionIndex,control);
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,campaignDescriptionText))->text =
         (uint16_t *)(*(int *)(*(int *)(rowPointers + selectionIndex * 4) + 0x50) + 0x230000);
  }
  return;
}


/* Address: 0x00549CC0.
   Ownership: assets/scenario/catalog.
   Purpose: Reads the active save, level, or campaign selection group, releases stale campaign state, dispatches
   the matching local loader or network command, and prepares the selected save path when the save group is active.
   Queued UI action handler for FRONTEND_PAGE20[56] (0x2038). Return datatype is preserved for non-queue direct
   callers. Typed parameters: p0 selectionControl→FrontendScenarioSelectionControlAddress32_V345. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: FrontendScenarioSession_LoadOrRequestLevelAsset,
   FrontendScenarioSession_LoadOrRequestCampaignBundle.
   Cross-module calls: UiSelectableGroup_NoneVisibleSelected [ui/controls/lists], Resource_Release
   [assets/resource/runtime], UiPointerList_GetSelectedIndexVariantB [ui/controls/lists],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], WidePath_CombineDirectoryAndLeaf
   [core/text/path], WidePath_SetExtensionCode [core/text/path].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendScenarioSelection_ActivateSelectedRecord
          (FrontendScenarioSelectionControlAddress32 selectionControl)

{
  UiListRowIndex selectedRowIndex;
  ListSelectionResult selectedRow;
  SelectableGroupNodeResult selectedGroup;
  int scenarioPathPointerTableAddress;

  /* selectionControl is the frontend template's gameSelectStartButton (+0x1CF4); the offsets below are
     relative to it: +0x60 loadGameTabButton, +0xC0 singleGameTabButton, +0x120 campaignsTabButton,
     +0x2C8 savedGamesList, +0x490 missionsList, +0x670 campaignsList. */
  selectedGroup = UiSelectableGroup_NoneVisibleSelected(3,
      THANDOR_UI_AT(selectionControl,0x60) /* loadGameTabButton */,
      THANDOR_UI_AT(selectionControl,0xc0) /* singleGameTabButton */,
      THANDOR_UI_AT(selectionControl,0x120) /* campaignsTabButton */);
  if (selectedGroup.noneSelected) {
    return;
  }
  if (selectedGroup.controlIndexOrCount != 0) {
    if (selectedGroup.controlIndexOrCount < 2) {
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = (void *)0x0;
      selectedRow = UiPointerList_GetSelectedIndexVariantB
                        ((UiPointerListControl *)THANDOR_UI_AT(selectionControl,0x490) /* missionsList */);
      selectedRowIndex = selectedRow.rowIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendScenarioSession_LoadOrRequestLevelAsset(g_LocalPlayerRuntimeId,0,0,selectedRowIndex)
        ;
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x920,0,0,selectedRowIndex);
      }
      return;
    }
    Resource_Release(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = (void *)0x0;
    selectedRow = UiPointerList_GetSelectedIndexVariantB
                      ((UiPointerListControl *)THANDOR_UI_AT(selectionControl,0x670) /* campaignsList */);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendScenarioSession_LoadOrRequestCampaignBundle(g_LocalPlayerRuntimeId,0,0,selectedRow.rowIndex)
      ;
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xb70,0,0,selectedRow.rowIndex);
    }
    return;
  }
  scenarioPathPointerTableAddress =
       (int)((UiListControl *)THANDOR_UI_AT(selectionControl,0x2c8) /* savedGamesList */)->rowSlots;
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  selectedRow = UiPointerList_GetSelectedIndexVariantB
                    ((UiPointerListControl *)THANDOR_UI_AT(selectionControl,0x2c8) /* savedGamesList */);
  WidePath_CombineDirectoryAndLeaf
            (&g_FrontendScenarioPathScratchUtf16,
             *(uint16_t **)(scenarioPathPointerTableAddress + selectedRow.rowIndex * 4),
             (uint16_t *)u_save_0050daa2);
  WidePath_SetExtensionCode(0x657673,&g_FrontendScenarioPathScratchUtf16);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,2);
  }
  return;
}


/* Address: 0x005451F0.
   Selection callback of the single-game (missions) list: selects the row and shows the level's description
   text (TEXT_ID_LEVEL_DESCRIPTION_BASE + 0x10 * the record's title index at +0x70) in the description box,
   which keeps the empty placeholder while the list has no rows.
*/
void __thandor_void_preserve_eax_ecx_edx
ScenarioCatalog_RefreshSelectedRecordField70DisplayId
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
    UiPointerList_SelectIndexVariantB(selectionIndex,listControl);
    ((UiWrappedTextControl *)FRONTEND_UI(frontendRoot,missionDescriptionText))->text =
         (uint16_t *)(*(int *)(*(int *)(rowPointers + selectionIndex * 4) + 0x70) * 0x10 +
                      TEXT_ID_LEVEL_DESCRIPTION_BASE);
  }
  return;
}

