/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: ui/frontend/runtime. */

/* Frontend_MainLoop: presents UI frames until a page action is pending, then flushes the input and counts the
   action depth. Returns false instead when no action is pending and the UI root stack is empty (the player
   quit the game). */
static bool FrontendMainLoop_PresentFramesUntilPageAction(void)
{
  do {
    if (g_UiRootNode != UI_ROOT_STACK_END) {
      FrontendRomTransition_ProcessPendingRecord();
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    g_FrontendPendingPageActionDepth = 0;
    if ((g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_NONE) && (g_UiRootNode == UI_ROOT_STACK_END)) {
      return false;
    }
  } while (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_NONE);
  UiFrame_FlushInputAndResetPendingTicks();
  g_FrontendPendingPageActionDepth++;
  return true;
}

/* Frontend_MainLoop, client side of the scenario selection wait: unpacks the snapshot table the host sent
   (receivedBuffer holds the unpacked size, then the packed data) into g_PackageScratchBuffer and merges it into
   the player blocks. Each player's transfer flags are ORed in; when they mark the payload complete, the
   snapshot payload follows them. */
static void FrontendMainLoop_TakeReceivedSnapshots(PckDecodedByteCount *receivedBuffer,uint32_t receivedByteCount)
{
  FrontendSnapshotTransferFlags *receivedFlagsCursor;
  FrontendSnapshotTransferFlags receivedTransferFlags;
  FrontendSnapshotTransferFlags *payloadCursor;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  int remainingPayloadDwords;

  PckCodec_DecodeHuffmanRle
            (*receivedBuffer,g_PackageScratchBuffer,receivedByteCount - 4,(uint8_t *)(receivedBuffer + 1),NULL,NULL);
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  receivedFlagsCursor = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    receivedTransferFlags = *receivedFlagsCursor;
    playerBlock->snapshotTransferFlags = playerBlock->snapshotTransferFlags | receivedTransferFlags;
    receivedFlagsCursor++;
    if ((receivedTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
      payloadCursor = (FrontendSnapshotTransferFlags *)playerBlock->snapshotPayload;
      for (remainingPayloadDwords = FRONTEND_SNAPSHOT_PAYLOAD_BYTES / sizeof(uint32_t); remainingPayloadDwords != 0;
           remainingPayloadDwords--) {
        *payloadCursor = *receivedFlagsCursor;
        receivedFlagsCursor++;
        payloadCursor++;
      }
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SNAPSHOTS_RECEIVED,0,0,0);
  UiTransferMailbox_ClearReceivedState();
}

/* Frontend_MainLoop, scenario catalogue exchange: processes a received scenario asset; unless the local player
   already has the catalogue, marks it, rebuilds the catalogue and either offers it to the clients (host: the
   catalogue is compressed into its own buffer right behind the used bytes, prefixed with the uncompressed size)
   or starts waiting for the host's (client). */
static void FrontendMainLoop_ExchangeScenarioCatalog(void)
{
  FrontendPlayerRuntimeRecord *localPlayerBlock;
  FrontendRoleStateFlags *localRoleStateFlags;
  ScenarioCatalogHeader *scenarioCatalog;
  uint32_t catalogUsedBytes;
  uint8_t *encodedCatalog;
  PckOutputCapacityBytes destinationCapacityBytes;
  bool encodeOk;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  uint32_t checkedValue;

  localPlayerBlock = g_FrontendPlayerRuntimeBlocks;
  FrontendScenarioTransfer_ProcessReceivedAsset();
  if ((localPlayerBlock->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_SCENARIO_CATALOG) != 0) {
    return;
  }
  localRoleStateFlags = &localPlayerBlock->factionAssignment.roleStateFlags;
  *localRoleStateFlags = *localRoleStateFlags | FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
  ScenarioCatalog_Rebuild();
  catalogUsedBytes = g_ScenarioCatalogUsedBytes;
  scenarioCatalog = g_ScenarioCatalog;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 1;
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    encodedCatalog = (uint8_t *)g_ScenarioCatalog + g_ScenarioCatalogUsedBytes + sizeof(uint32_t);
    destinationCapacityBytes = (int)(SCENARIO_CATALOG_CAPACITY - sizeof(uint32_t)) - g_ScenarioCatalogUsedBytes;
    *(uint32_t *)(encodedCatalog - 4) = g_ScenarioCatalogUsedBytes;
    encodeOk = PckCodec_EncodeHuffmanRle
                   (destinationCapacityBytes,encodedCatalog,catalogUsedBytes,(uint8_t *)scenarioCatalog,
                    &encodedByteCount,&encodeErrorCode);
    checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
    UiTransferMailbox_SetOutgoingBuffer(checkedValue + 4,encodedCatalog - 4);
  }
}

/* Frontend_MainLoop, FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE (checked once per frame): in a network session
   first wait until every player's snapshot is published; a client takes the snapshot table the host sends
   meanwhile. Then wait for the scenario catalogue exchange (the host sends its catalogue, a client receives it)
   before the page opens. */
static void FrontendMainLoop_PollScenarioSelectionPage(void)
{
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendRoleStateFlags *roleStateFlags;
  PckDecodedByteCount *receivedBuffer; /* unpacked size, then the packed snapshot flags */
  uint32_t receivedByteCount;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
    do {
      if ((playerBlock->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
          receivedBuffer = (PckDecodedByteCount *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
          if (receivedBuffer != NULL) {
            FrontendMainLoop_TakeReceivedSnapshots(receivedBuffer,receivedByteCount);
          }
        }
        return;
      }
      playerBlock++;
      remainingPlayerBlocks--;
    } while (remainingPlayerBlocks != 0);
  }
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  do {
    /* the AND really clears every other progress bit of the player */
    roleStateFlags = &playerBlock->factionAssignment.roleStateFlags;
    *roleStateFlags = *roleStateFlags & FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
    if (*roleStateFlags == 0) {
      FrontendMainLoop_ExchangeScenarioCatalog();
      return;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_SetOutgoingBuffer(0,NULL);
  }
  FrontendScenarioSelectionPage_InitializeAndApplyMapOption
            ((FrontendScenarioSelectionPageView *)g_FrontendRootNode);
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
}

/* Frontend_MainLoop: true when every player block has one of the roleStateFlags bits in stateMask. */
static bool FrontendMainLoop_AllPlayersHaveRoleState(FrontendRoleStateFlags stateMask)
{
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;

  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((playerBlock->factionAssignment.roleStateFlags & stateMask) == 0) {
      return false;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  return true;
}

/* Frontend_MainLoop, task assignment and mission briefing pages: the host releases its outgoing transfer before
   the page opens. */
static void FrontendMainLoop_ReleaseHostTransfer(void)
{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
    UiTransferMailbox_SetOutgoingBuffer(0,NULL);
  }
}

/* Frontend_MainLoop: runs the prepared session in g_FrontendLoadedLevelAsset (loadExistingSession 1 resumes the
   saved session) and afterwards flushes the settings and input and clears every player's progress bits. */
static void FrontendMainLoop_RunSession(FrontendBooleanState32 loadExistingSession)
{
  uint32_t sessionError;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;

  if (!InGameRuntime_RunSessionUntilExit
         ((LevelAssetRuntimePrefix *)g_FrontendLoadedLevelAsset,loadExistingSession,
          (uint16_t *)&g_FrontendScenarioPathScratchUtf16,&sessionError)) {
    FatalError_ExitIfFailed(sessionError,true);
  }
  PersistentSettings_Flush();
  UiFrame_FlushInputAndResetPendingTicks();
  g_FrontendScenarioInitializationCount = 0;
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    playerBlock->factionAssignment.roleStateFlags = 0;
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
}

/* Frontend_MainLoop, after a session in a campaign: follows the successor of the campaign's current level chosen
   by the end movie selection and writes its level path (level\<name>.lev) to g_FrontendScenarioPathScratchUtf16.
   Returns true when a successor level was selected. A successor id missing from the campaign finishes it (the
   campaign asset is released); a negative successor id or a current level without a record leaves it loaded.
   The record cursors start at the asset base and advance by one CampaignLevelRecord, so level record i is
   ((CampaignAsset *)cursor)->levels[0]. */
static bool FrontendMainLoop_SelectCampaignSuccessorLevel(void)
{
  CampaignAsset *campaign;
  CampaignAsset *levelRecordView;
  int remainingLevelRecords;
  int successorLevelId;

  campaign = (CampaignAsset *)g_FrontendLoadedCampaignAsset;
  if (campaign == NULL) {
    return false;
  }
  remainingLevelRecords = campaign->levelRecordCount;
  levelRecordView = campaign;
  while (campaign->currentLevelId != levelRecordView->levels[0].levelId) {
    levelRecordView = (CampaignAsset *)((CampaignLevelRecord *)levelRecordView + 1);
    remainingLevelRecords--;
    if (remainingLevelRecords == 0) {
      return false;
    }
  }
  successorLevelId = levelRecordView->levels[0].successorLevelIds[(int)g_EndMovieSelectionIndex];
  if (successorLevelId < 0) {
    return false;
  }
  remainingLevelRecords = campaign->levelRecordCount;
  campaign->currentLevelId = successorLevelId;
  levelRecordView = campaign;
  while (successorLevelId != levelRecordView->levels[0].levelId) {
    levelRecordView = (CampaignAsset *)((CampaignLevelRecord *)levelRecordView + 1);
    remainingLevelRecords--;
    if (remainingLevelRecords == 0) {
      /* Successor level missing: the campaign is finished. */
      Resource_Release((void *)g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = 0;
      g_FrontendScenarioInitializationCount = 0;
      return false;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_FrontendScenarioPathScratchUtf16,levelRecordView->levels[0].levelFileName,
             (uint16_t *)u_level_0050daac);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,(uint16_t *)&g_FrontendScenarioPathScratchUtf16);
  return true;
}

/* Frontend_MainLoop, host: builds the level transfer in the package scratch buffer (level size, grid size, packed
   level size, packed grid size, then both packed images), copies it to its own allocation and offers it to the
   clients. */
static void FrontendMainLoop_OfferLevelToClients(FrontendLoadedLevelAsset *loadedLevelAsset,FieldGridAsset *fieldGrid)
{
  ScenarioLevelBundleHeader *bundleHeader;
  AssetAllocationSizeBytes fieldGridAllocationSize;
  uint8_t *encodedImages;
  bool encodeOk;
  uint32_t encodedByteCount;
  uint32_t encodeErrorCode;
  uint32_t levelEncodedBytes;
  uint32_t fieldGridEncodedBytes;
  uint32_t transferByteCount;
  uint32_t allocError;
  void *allocPayload;
  uint32_t *transferAllocation;
  uint32_t *transferDwordCursor;
  uint32_t *transferSourceDwords;
  uint32_t remainingDwords;

  bundleHeader = (ScenarioLevelBundleHeader *)g_PackageScratchBuffer;
  fieldGridAllocationSize = fieldGrid->common.allocationSizeBytes;
  bundleHeader->levelDecodedBytes = loadedLevelAsset->header.common.allocationSizeBytes;
  bundleHeader->fieldGridDecodedBytes = fieldGridAllocationSize;
  encodedImages = (uint8_t *)(bundleHeader + 1);
  /* capacity: the scratch buffer minus 24 bytes, the size of the campaign bundle header
     (ScenarioCampaignBundleHeader), although this header has 16 */
  encodeOk = PckCodec_EncodeHuffmanRle
                 (PACKAGE_SCRATCH_BUFFER_BYTES - 24,encodedImages,loadedLevelAsset->header.common.allocationSizeBytes,
                  (uint8_t *)loadedLevelAsset,&encodedByteCount,&encodeErrorCode);
  levelEncodedBytes = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
  bundleHeader->levelEncodedBytes = levelEncodedBytes;
  encodeOk = PckCodec_EncodeFieldGrid
                 (PACKAGE_SCRATCH_BUFFER_BYTES - 24 - levelEncodedBytes,encodedImages + levelEncodedBytes,
                  fieldGrid->common.allocationSizeBytes,fieldGrid,&encodedByteCount,&encodeErrorCode);
  fieldGridEncodedBytes = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
  bundleHeader->fieldGridEncodedBytes = fieldGridEncodedBytes;
  transferByteCount =
       (uint32_t)(encodedImages - (uint8_t *)bundleHeader) + levelEncodedBytes + fieldGridEncodedBytes;
  allocError = g_MemoryApi.alloc(transferByteCount,&allocPayload);
  transferAllocation =
       (uint32_t *)FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  transferDwordCursor = transferAllocation;
  transferSourceDwords = (uint32_t *)bundleHeader;
  for (remainingDwords = transferByteCount >> 2; remainingDwords != 0; remainingDwords--) {
    *transferDwordCursor = *transferSourceDwords;
    transferSourceDwords++;
    transferDwordCursor++;
  }
  UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)transferByteCount,transferAllocation);
}

/* Frontend_MainLoop, host and local game: replaces the loaded level by the one at
   g_FrontendScenarioPathScratchUtf16, loads its field grid (the level's path with the extension "fld", under the
   executable directory), offers both to the clients when hosting and assigns the factions. */
static void FrontendMainLoop_LoadSelectedLevel(void)
{
  void *loadedPackageEntry;
  uint32_t packageLoadErrorCode;
  uint32_t checkedValue;
  uint16_t *fieldGridPath;
  FieldGridAsset *fieldGrid;
  FrontendLoadedLevelAsset *loadedLevelAsset;

  /* levelPathOffsetOrLoadedFieldGrid holds the field grid pointer once loaded, an offset (<= 0xFFFF) into the
     level before */
  if ((g_FrontendLoadedLevelAsset != NULL) &&
     (0xffff < g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release((void *)g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid);
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = NULL;
  loadedPackageEntry = Package_LoadEntry((uint16_t *)&g_FrontendScenarioPathScratchUtf16,&packageLoadErrorCode);
  checkedValue = FatalError_ExitIfFailed
                      (loadedPackageEntry != NULL ? (uint32_t)loadedPackageEntry : packageLoadErrorCode,
                       loadedPackageEntry == NULL);
  g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedValue;
  fieldGridPath = (uint16_t *)((uint8_t *)g_FrontendLoadedLevelAsset +
                               g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,fieldGridPath);
  WidePath_CombineDirectoryAndLeaf
            ((uint16_t *)&g_LevelResourcePathScratchUtf16,fieldGridPath,(uint16_t *)&g_ExecutableDirectoryUtf16);
  fieldGrid = Package_LoadEntry(fieldGridPath,&packageLoadErrorCode);
  if (fieldGrid == NULL) {
    /* Original quirk: a failed field grid load is not checked; the error code is used as the grid */
    fieldGrid = (FieldGridAsset *)packageLoadErrorCode;
  }
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    FrontendMainLoop_OfferLevelToClients(loadedLevelAsset,fieldGrid);
  }
  loadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)fieldGrid;
  FrontendPlayerRuntime_InitializeFactionAssignments();
}

/* Frontend_MainLoop: rebuilds the menu in the briefing room and loads the level (host and local game) or waits
   for it from the host (client); FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE opens once every player has it.
   Returns false with Frontend_Init's error in *outError when building the menu fails. */
static bool FrontendMainLoop_EnterMissionBriefing(uint32_t *outError)
{
  FrontendRoleStateFlags *localRoleStateFlags;

  if (!Frontend_Init(FRONTEND_ROM_RECORD_MISSION_BRIEFING,outError)) {
    return false;
  }
  g_FrontendScenarioInitializationCount++;
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE;
  localRoleStateFlags = &g_FrontendPlayerRuntimeBlocks->factionAssignment.roleStateFlags;
  *localRoleStateFlags = *localRoleStateFlags | FRONTEND_PLAYER_STATE_LEVEL_LOADED;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendMainLoop_LoadSelectedLevel();
  }
  else {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = 5;
  }
  return true;
}

/* Frontend_MainLoop: rebuilds the menu at nextRomRecordId with nextPageAction pending (set even when the build
   fails). Returns false with Frontend_Init's error in *outError when building the menu fails. */
static bool FrontendMainLoop_RebuildMenu(RomRecordId nextRomRecordId,uint32_t nextPageAction,uint32_t *outError)
{
  bool menuBuilt;

  menuBuilt = Frontend_Init(nextRomRecordId,outError);
  g_FrontendPendingPageAction = nextPageAction;
  return menuBuilt;
}

/* Frontend_MainLoop, after a session: a campaign continues with the successor level in the briefing room. Without
   one, a scenario path left from the session is loaded there again; with none the menu goes back to the scenario
   selection. Returns false with Frontend_Init's error in *outError when building the menu fails. */
static bool FrontendMainLoop_ContinueAfterSession(uint32_t *outError)
{
  if (!FrontendMainLoop_SelectCampaignSuccessorLevel() && (g_FrontendScenarioPathScratchUtf16 == 0)) {
    return FrontendMainLoop_RebuildMenu
                     (FRONTEND_ROM_RECORD_SCENARIO_SELECTION,FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE,outError);
  }
  return FrontendMainLoop_EnterMissionBriefing(outError);
}

/* Frontend_MainLoop: performs the pending g_FrontendPendingPageAction. Page actions open their page (the wait
   pages only once the peers are ready, otherwise they stay pending); every other action tears the frontend down,
   runs a session if requested and rebuilds the menu. Returns false with Frontend_Init's error in *outError when
   rebuilding the menu fails. */
static bool FrontendMainLoop_PerformPageAction(RomRecordId frontendEntryRecordId,uint32_t *outError)
{
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) {
    FrontendNetworkSetupPage_InitializeBackendMode((FrontendUiImage *)g_FrontendRootNode);
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) {
    FrontendGameplaySettingsPage_InitializeFromPersistentSettings((UiRootNode *)g_FrontendRootNode);
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE) {
    FrontendMainLoop_PollScenarioSelectionPage();
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE) {
    FrontendScenarioTransfer_ProcessReceivedAsset();
    if (FrontendMainLoop_AllPlayersHaveRoleState(FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT)) {
      FrontendMainLoop_ReleaseHostTransfer();
      FrontendTaskAssignmentPage_Initialize((FrontendTaskAssignmentPageInitView *)g_FrontendRootNode);
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    }
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) {
    FrontendScenarioTransfer_ProcessReceivedAsset();
    if (FrontendMainLoop_AllPlayersHaveRoleState(FRONTEND_PLAYER_STATE_LEVEL_READY_MASK)) {
      FrontendMainLoop_ReleaseHostTransfer();
      FrontendMissionBriefingPage_Initialize((UiRootNode *)g_FrontendRootNode);
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    }
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_CREDITS) {
    CreditsScreen_Open((FrontendCreditsUiStateView *)g_FrontendRootNode);
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE) {
    FrontendSession_ShowQuitConfirmPage((FrontendUiImage *)g_FrontendRootNode);
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  /* every remaining action leaves the menu: tear the frontend down first */
  FrontendRuntime_ShutdownAndReleaseResources();
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_START_SESSION) {
    PersistentSettings_Flush();
    FrontendMainLoop_RunSession(0);
    return FrontendMainLoop_ContinueAfterSession(outError);
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_RESUME_SAVED_SESSION) {
    /* unlike FRONTEND_PAGE_ACTION_START_SESSION the settings are not flushed before the session */
    FrontendMainLoop_RunSession(1);
    return FrontendMainLoop_ContinueAfterSession(outError);
  }
  /* any other action: rebuild the menu at the entry record */
  return FrontendMainLoop_RebuildMenu(frontendEntryRecordId,FRONTEND_PAGE_ACTION_NONE,outError);
}

/* Address: 0x00546BD0.
   The frontend (main menu) state machine, run from Game_Run until the player quits. It builds the menu at
   frontendEntryRecordId, jumps straight into the host/client/map flow when -HOST, -CLIENT= or -KARTE= is on the
   command line, then presents UI frames until g_FrontendPendingPageAction (FRONTEND_PAGE_ACTION_*) is set and
   performs it: open a menu page, wait for the network peers (scenario catalogue, task assignment, level
   transfer), or tear the frontend down to run a session. After a session a campaign continues with the
   successor level chosen by the end movie (menu rebuilt at FRONTEND_ROM_RECORD_MISSION_BRIEFING), otherwise
   the menu is rebuilt at the scenario selection or the entry record. Returns true when the UI root stack
   empties (quit); false with Frontend_Init's error in *outError when building the menu fails.
*/
bool Frontend_MainLoop(RomRecordId frontendEntryRecordId,uint32_t *outError)

{
  uint32_t initError;

  g_FrontendNetworkState = 0;
  if (Frontend_Init(frontendEntryRecordId,&initError)) {
    /* -HOST and -CLIENT= activate entry 3 of the entry menu's action table, -KARTE= (map) entry 0, without the
       click sound, and let the started camera transition end at once. */
    if ((g_CommandLineFindOption(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 26) != NULL) || /* "HOST" */
        (g_CommandLineFindOption(8,s_NAME__CLIENT__KARTE___00545e91 + 6) != NULL)) {       /* "CLIENT=" */
      FrontendRomActionTable_ExecuteRecord(0,0,1,3);
      FrontendRomTransition_RequestStop();
    }
    else if (g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 14) != NULL) { /* "KARTE=" */
      FrontendRomActionTable_ExecuteRecord(0,0,1,0);
      FrontendRomTransition_RequestStop();
    }
    do {
      if (!FrontendMainLoop_PresentFramesUntilPageAction()) {
        /* the last UI root was popped: the player quit the game */
        FrontendRuntime_ShutdownAndReleaseResources();
        return true;
      }
    } while (FrontendMainLoop_PerformPageAction(frontendEntryRecordId,&initError));
  }
  FrontendRuntime_ShutdownAndReleaseResources();
  *outError = initError;
  return false;
}


/* Address: 0x0050C380.
   Pointer-move handler of the model pointer context (pointerMove of g_FrontendModelPointerContextVtable): stores
   the best model hit under the pointer, then returns the cursor frame. While a non-right button is held
   (ROUTE_TO_SECONDARY_CALLBACK) heldButtonCursorCallback decides it, with no button hoverCursorCallback;
   while the right button drags the camera (ROUTE_TO_BUILTIN_ACTION_RESOLUTION) the frame shows the camera
   motion FrontendModelPointerContext_DispatchWorldCameraPointerInput will perform for the camera scheme bits,
   the left button and the modifier keys (1 move, 0x0F pitch, 0x10 heading and pitch, 0x11 distance, 0x25
   heading, 0x0E heading and distance, 0x12..0x14 the variants of scheme 0x8000).
*/
GraphicsCursorFrameIndex
FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
          (int pointerY,int pointerX,FrontendModelPointerHitContext *context)

{
  uint32_t callbackResult;
  GraphicsCursorFrameIndex cursorFrameIndex;
  uint64_t bestHit;

  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget(pointerY,pointerX,context);
  context->selectedHitMetric = (int)bestHit;
  context->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK) != 0) {
    if (context->heldButtonCursorCallback != NULL)
    {
      callbackResult = context->heldButtonCursorCallback
                        (context->surfaceHitDepth,context->surfaceHitWorldY,
                         context->surfaceHitWorldX,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION) ==
      0) {
    if (context->hoverCursorCallback != NULL)
    {
      callbackResult = context->hoverCursorCallback
                        (context->surfaceHitDepth,context->surfaceHitWorldY,
                         context->surfaceHitWorldX,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION) !=
      0) {
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT) != 0)
  {
    if ((g_CursorButtonState & LEFT) != 0) {
      return 17;
    }
    return 16;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN) != 0)
  {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      if ((g_CursorButtonState & LEFT) != 0) {
        return 17;
      }
      return 18;
    }
    if ((g_CursorButtonState & LEFT) != 0) {
      return 20;
    }
    return 19;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE) == 0)
  {
    return 0;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    return 15;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) == 0) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      return 37;
    }
    if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_HIDE_PANEL) ==
        0) {
      if ((g_CursorButtonState & LEFT) == 0) {
        return 1;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
          != 0) {
        return 14;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
          == 0) {
        return 37;
      }
    }
    else {
      if ((g_CursorButtonState & LEFT) != 0) {
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
            != 0) {
          return 15;
        }
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
            == 0) {
          return 0;
        }
        return 17;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
          != 0) {
        return 14;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
          == 0) {
        return 37;
      }
    }
    cursorFrameIndex = 16;
  }
  else {
    cursorFrameIndex = 17;
  }
  return cursorFrameIndex;
}

/* Address: 0x0050C5A0.
   Press of a non-right button on the model pointer context (nonRightPress of
   g_FrontendModelPointerContextVtable): remembers the press point (corner of the drag frame), stores the best
   model hit, routes the following pointer moves to heldButtonCursorCallback and reports the press to
   buttonPressCallback.
*/
void FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->dragFrameStartX = pointerX;
  callbackContext->dragFrameStartY = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerHitContext *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags | FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->buttonPressCallback !=
      NULL) {
    callbackContext->buttonPressCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerHitContext *)callbackContext);
  }
  return;
}


/* Address: 0x0050C610.
   Release of a non-right button on the model pointer context (nonRightRelease of
   g_FrontendModelPointerContextVtable): stores the best model hit, routes pointer moves back to the hover
   callback and reports the release to buttonReleaseCallback.
*/
void FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerHitContext *callbackContext)

{
  uint64_t bestHit;
  
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags & ~FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->buttonReleaseCallback !=
      NULL) {
    callbackContext->buttonReleaseCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
  return;
}


/* Address: 0x0050C670.
   Drag with a non-right button on the model pointer context (nonRightDrag of
   g_FrontendModelPointerContextVtable): remembers the current point (the other corner of the drag frame),
   stores the best model hit and reports the drag to buttonDragCallback.
*/
void FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->dragFrameEndX = pointerX;
  callbackContext->dragFrameEndY = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerHitContext *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  if (callbackContext->buttonDragCallback !=
      NULL) {
    callbackContext->buttonDragCallback
              (callbackContext->surfaceHitDepth,callbackContext->surfaceHitWorldY,
               callbackContext->surfaceHitWorldX,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerHitContext *)callbackContext);
  }
  return;
}


/* Address: 0x00549B40.
   Handler of action 0x2044 (slot 68 of g_FrontendUiActionHandlersPage20.handlers00_54), the colour buttons of
   the faction setup page: finds the row of the pressed button in the factionControls offset table and cycles
   that faction's colour (FrontendFactionSetup_CycleFactionColour directly in a local game,
   FRONTEND_COMMAND_CYCLE_FACTION_COLOUR in a network game).
*/
void FrontendFactionSetupAction_CycleFactionColour(UiNodeBase *factionControl)

{
  CommandPayload rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)factionControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_CycleFactionColour(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CYCLE_FACTION_COLOUR,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x00549BC0.
   Handler of action 0x2045 (slot 69 of g_FrontendUiActionHandlersPage20.handlers00_54), the mode buttons of
   the faction setup page: finds the row of the pressed button in the playerControls offset table and toggles
   whether that faction takes part (FrontendFactionSetup_ToggleFactionActive directly in a local game,
   FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE in a network game).
*/
void FrontendFactionSetupAction_ToggleFactionActive(UiNodeBase *playerControl)

{
  CommandPayload rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)playerControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndex]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_ToggleFactionActive(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x00549C40.
   Handler of action 0x2046 (slot 70 of g_FrontendUiActionHandlersPage20.handlers00_54), the "play" checkboxes
   of the faction setup page: finds the row of the pressed checkbox in the selectionRows offset table and makes
   that faction the local player's (FrontendFactionSetup_ChooseFaction directly in a local game,
   FRONTEND_COMMAND_CHOOSE_FACTION in a network game).
*/
void FrontendFactionSetupAction_ChooseFaction(UiNodeBase *selectionRowControl)

{
  CommandPayload rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)selectionRowControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_ChooseFaction(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CHOOSE_FACTION,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x0050BB80.
   Relocate method of the model pointer context (relocate of g_FrontendModelPointerContextVtable), run when the
   control is built from its template: puts the camera target at the origin, gives camera limits the template
   left at 0 their defaults, starts with no candidate models and no overlay entity, then relocates the children.
*/
void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContext *control)

{
  control->targetPositionXQ12 = 0;
  control->targetPositionYQ12 = 0;
  control->targetPositionZQ12 = 0;
  /* pitch -90..+90 degrees */
  if (control->minimumPitchAngle == 0) {
    control->minimumPitchAngle = (uint32_t)-FIXED_ANGLE16_QUARTER_TURN;
  }
  if (control->maximumPitchAngle == 0) {
    control->maximumPitchAngle = FIXED_ANGLE16_QUARTER_TURN;
  }
  /* camera distance 0.25..127 (Q12) */
  if (control->minimumDistanceQ12 == 0) {
    control->minimumDistanceQ12 = Q12_ONE / 4;
  }
  if (control->maximumDistanceOrSurfaceLimitQ12 == 0) {
    control->maximumDistanceOrSurfaceLimitQ12 = 127 * Q12_ONE;
  }
  control->worldObjectArray = 0;
  control->worldObjectCount = 0;
  control->candidateModelListHead = NULL;
  control->selectedOverlayEntity = NULL;
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x0050BC30.
   Layout method of the model pointer context (layout of g_FrontendModelPointerContextVtable): a new size
   invalidates the reusable terrain projection (WorldRuntime_ClearFieldGridDirtyFlag clears
   TERRAIN_RENDER_REUSE_PROJECTION), then the children are laid out.
*/
void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext)

{
  WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  UiContainer_LayoutChildren((UiNodeBase *)callbackContext);
  return;
}


/* Hierarchy renderer for the model passes of FrontendModelPointerContext_RenderWorldViewQueuesClipped. */
static void (*FrontendModelPointerContext_SelectRenderHierarchyProc(const FrontendModelPointerContext *control))
          (ModelRuntimeNode *)
{
  if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
    return ModelRuntime_RenderHierarchyRecursiveAlternatePath;
  }
  return ModelRuntime_CullAndRenderHierarchyRecursive;
}

/* End of one render pass: sorts and draws the active primitive queue into the clip rectangle and adds its
   primitive count to renderedPrimitiveCount. */
static void FrontendModelPointerContext_DrawActiveQueue
          (FrontendModelPointerContext *control,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft)
{
  uint32_t queuedPrimitiveCount;

  PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844
            (control->base.nodeFlags & 8,control->activePrimitiveQueue);
  g_GraphicsDrawPrimitiveQueue
            (clipBottom,clipRight,clipTop,clipLeft,control->activePrimitiveQueue);
  queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
  control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
}

/* Address: 0x0050BC60.
   Draw method of the model pointer context (drawClipped of g_FrontendModelPointerContextVtable), the 3D view of
   the menu room and of the in-game world: clamps the clip rectangle to the control, sets up camera, projection
   and (optionally) the sound listener, then renders the candidate models in up to four primitive-queue passes
   (models with flag 0x200, the terrain, the shading pass of models with flag 0x100, the remaining models),
   releasing and re-acquiring the render spin lock between passes. Afterwards the enabled selection
   overlays are drawn and the child controls on top. Nothing is drawn while FRONTEND_MENU_ROOM_RENDER_SUPPRESSED
   is set (a dialog page covers the room).
*/
void FrontendModelPointerContext_RenderWorldViewQueuesClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FrontendModelPointerContext *control)

{
  UiPixelCoordinate cursorOverrideX;
  UiPixelCoordinate cursorOverrideY;
  GraphicsWorldCoordinateQ12 listenerY;
  GraphicsWorldCoordinateQ12 listenerZ;
  void (*renderHierarchyProc)(ModelRuntimeNode *);
  ModelRuntimeNode *modelNode;
  GraphicsPrimitiveQueue *frameQueue;
  /* The original would jump straight to the end when a primitive-queue reset failed, handing the selection
     overlays a stale rectangle; GraphicsPrimitiveQueue_ResetGlobal never fails, so the overlays always get the
     clipped rectangle. */

  if ((control->contextFlags & FRONTEND_MENU_ROOM_RENDER_SUPPRESSED) != 0) {
    return;
  }
  if (clipLeft < control->base.left) {
    clipLeft = control->base.left;
  }
  if (control->base.right < clipRight) {
    clipRight = control->base.right;
  }
  if (clipTop < control->base.top) {
    clipTop = control->base.top;
  }
  if (control->base.bottom < clipBottom) {
    clipBottom = control->base.bottom;
  }
  g_GraphicsSetViewportAndClearDepth(clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockAcquire(control->renderSpinLock);
  cursorOverrideY = g_CursorOverrideY;
  cursorOverrideX = g_CursorOverrideX;
  control->selectedModelNode = NULL;
  control->selectedHitMetric = WORLD_POINTER_NO_HIT;
  control->surfaceHitWorldX = WORLD_POINTER_NO_HIT;
  control->surfaceHitWorldY = WORLD_POINTER_NO_HIT;
  control->surfaceHitDepth = WORLD_POINTER_NO_HIT;
  control->cursorWorldXQ12 = cursorOverrideX << 12;
  control->cursorWorldYQ12 = cursorOverrideY << 12;
  control->renderedPrimitiveCount = 0;
  Graphics_SetProjectionClipRect(clipBottom,clipRight,clipTop,clipLeft);
  Graphics_SetViewProjectionParameters
            (control->projectionShift,control->viewAngle1,control->viewAngle0,
             control->projectionScale,control->hitReferenceWorldZQ12,control->hitReferenceWorldYQ12,
             control->hitReferenceWorldXQ12);
  if ((control->contextFlags & WORLD_RUNTIME_FLAG_SOUND_LISTENER) != 0) {
    listenerY = control->targetPositionYQ12;
    listenerZ = ((int)control->committedDistanceOrSoundZOffset >> 2) + control->targetPositionZQ12;
    SpatialSound_RebuildListenerTransformFromPose
              (control->viewAngle1,control->viewAngle0,listenerZ,listenerY,control->targetPositionXQ12);
  }
  Graphics_SetProjectionViewport
            (control->base.bottom,control->base.right,control->base.top,control->base.left);
  Graphics_SetAuxiliaryOrientation
            (control->auxiliaryOrientationAngle1,control->auxiliaryOrientationAngle0);
  Graphics_SetSceneBoundsAndColors
            (control->sceneBound7,control->sceneBound6,control->sceneBound5,control->sceneBound4,
             control->sceneBound3,control->sceneBound2,control->sceneBound1,control->sceneBound0);
  Graphics_RebuildFrustumPlanes();
  g_GraphicsBeginScene();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  GraphicsShadingRuntime_RebuildCompactLightingRecords();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
  Graphics_SetActivePrimitiveQueue(frameQueue);
  control->activePrimitiveQueue = frameQueue;
  if (control->renderPhaseCallback != NULL) {
    control->renderPhaseCallback(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
  }
  renderHierarchyProc = FrontendModelPointerContext_SelectRenderHierarchyProc(control);
  for (modelNode = control->candidateModelListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (((modelNode->runtimeFlags & MODEL_NODE_FLAG_HIDDEN) == 0) &&
        ((modelNode->runtimeFlags & MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN) != 0)) {
      modelNode->runtimeFlags = modelNode->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
      if ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0) {
        renderHierarchyProc(modelNode);
      }
    }
  }
  if (control->renderPhaseCallback != NULL) {
    control->renderPhaseCallback(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
  }
  FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if (((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN) != 0) && (control->fieldGrid != NULL)) {
    frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
    Graphics_SetActivePrimitiveQueue(frameQueue);
    control->activePrimitiveQueue = frameQueue;
    TerrainProjectedGrid_TransformShadeAndQueue(control->fieldGrid,control);
    FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  }
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if ((control->contextFlags & WORLD_RUNTIME_FLAG_SHADING_ENABLED) != 0) {
    modelNode = control->candidateModelListHead;
    frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
    Graphics_SetActivePrimitiveQueue(frameQueue);
    control->activePrimitiveQueue = frameQueue;
    if (modelNode != NULL) {
      GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
      for (; modelNode != NULL; modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
        if (((modelNode->runtimeFlags & MODEL_NODE_FLAG_HIDDEN) == 0) &&
            ((modelNode->runtimeFlags & MODEL_NODE_FLAG_SHADING_PASS) != 0) &&
            ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0)) {
          GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
                    (modelNode,(GeneratedTextureRenderContextView *)control);
        }
      }
      GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
    }
    FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  }
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  frameQueue = GraphicsPrimitiveQueue_ResetGlobal();
  Graphics_SetActivePrimitiveQueue(frameQueue);
  control->activePrimitiveQueue = frameQueue;
  if (control->renderPhaseCallback != NULL) {
    control->renderPhaseCallback(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
  }
  renderHierarchyProc = FrontendModelPointerContext_SelectRenderHierarchyProc(control);
  for (modelNode = control->candidateModelListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if ((modelNode->runtimeFlags & (MODEL_NODE_FLAG_DRAW_BEFORE_TERRAIN | MODEL_NODE_FLAG_HIDDEN)) == 0) {
      modelNode->runtimeFlags = modelNode->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
      if ((modelNode->tintArgb & ARGB8888_ALPHA_MASK) != 0) {
        renderHierarchyProc(modelNode);
      }
    }
  }
  if (control->renderPhaseCallback != NULL) {
    control->renderPhaseCallback(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
  }
  FrontendModelPointerContext_DrawActiveQueue(control,clipBottom,clipRight,clipTop,clipLeft);
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  if ((((clipLeft == control->base.left) && (clipRight == control->base.right)) &&
      (clipTop == control->base.top)) && (clipBottom == control->base.bottom)) {
    /* the whole view was drawn: the next terrain pass may reuse this projection */
    control->contextFlags = control->contextFlags | TERRAIN_RENDER_REUSE_PROJECTION;
  }
  g_GraphicsEndScene();
  g_RenderedFrameCountSinceDebugRefresh++;
  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  }
  else {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitHalfSourceRgb;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_OVERLAYS) == 0) {
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS) != 0) {
      SelectionOverlay_RenderSelectedArmyMetrics(clipBottom,clipRight,clipTop,clipLeft);
      if ((control->selectedOverlayEntity != NULL) &&
          SelectionInfo_IsEntryAbsent(control->selectedOverlayEntity)) {
        SelectionOverlay_RenderArmyMetricsForEntity
                  (clipBottom,clipRight,clipTop,clipLeft,control->selectedOverlayEntity);
      }
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
      SelectionOverlay_DrawBoundsFrame
                (clipBottom,clipRight,clipTop,clipLeft,control->dragFrameEndY,
                 control->dragFrameEndX,control->dragFrameStartY,
                 control->dragFrameStartX);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS) != 0) {
      SelectionOverlay_DrawTerrainPointMarkers
                (clipBottom,clipRight,clipTop,clipLeft,control->terrainMarkerPointCount,
                 control->terrainMarkerCoordinatePairs,control->fieldGrid);
    }
    if (((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER) != 0) && (control->surfaceHitDepth != WORLD_POINTER_NO_HIT)) {
      SelectionOverlay_DrawWorldPointMarker
                (clipBottom,clipRight,clipTop,clipLeft,
                 (uint32_t)((g_UiCommandModeGColorVariantLimit & ARGB8888_ALPHA_MASK) != 0),
                 control->surfaceHitWorldY,control->surfaceHitWorldX,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS) != 0) {
      SelectionOverlay_DrawGridVertexMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY) != 0) {
      SelectionOverlay_DrawFluidExclusionMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS) != 0) {
      SelectionOverlay_DrawResourceCellMarkers
                (clipBottom,clipRight,clipTop,clipLeft,(uint8_t)control->selectedResourceMarkerIndex,
                 control->fieldGrid);
    }
    if ((((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN) != 0) && (control->fieldGrid != NULL))
       && ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_DRAW_DEBUG_CELL_MARKERS) != 0)) {
      SelectionOverlay_DrawDebugMarkedCellMarkers(clipBottom,clipRight,clipTop,clipLeft,control->fieldGrid);
    }
  }
  g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
  g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  if ((control->selectedModelNode != NULL) &&
     ((int)control->surfaceHitDepth < control->selectedHitMetric)) {
    control->selectedModelNode = NULL;
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  return;
}


/* Address: 0x0050C6E0.
   Right-button press on the model pointer context (rightPress of g_FrontendModelPointerContextVtable): starts a
   camera drag. Remembers the press point (the pointer is put back there after every drag step), routes pointer
   moves to the camera cursor resolution, restarts the held-tick counter (rightButtonHeldTicks, counted by
   FrontendModelPointerContext_Tick) and pins the drawn cursor.
*/
void FrontendModelPointerContext_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  callbackContext->capturedPointerX = pointerX;
  callbackContext->capturedPointerY = pointerY;
  callbackContext->capturedWheelDelta = wheelDelta;
  callbackContext->contextFlags =
       callbackContext->contextFlags |
       FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION;
  callbackContext->rightButtonHeldTicks = 0;
  g_CursorUseOverridePosition++;
  return;
}


/* Address: 0x0050C730.
   Right-button release on the model pointer context (rightRelease of g_FrontendModelPointerContextVtable): ends
   the camera drag and unpins the cursor. A release within 7 ticks of the press counts as a click and is
   reported to rightClickCallback (the menu room stops its camera flight with it).
*/
void FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContext *callbackContext)

{
  g_CursorUseOverridePosition = 0;
  /* clears ROUTE_TO_BUILTIN_ACTION_RESOLUTION and the camera motion bits 0..3 */
  callbackContext->contextFlags = callbackContext->contextFlags &
       ~(FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION | FRONTEND_CAMERA_MOTION_MASK);
  if ((callbackContext->rightButtonHeldTicks < 7) &&
     (callbackContext->rightClickCallback != NULL)) {
    callbackContext->rightClickCallback(callbackContext);
  }
  return;
}


/* Address: 0x0050CC80.
   Right-button drag on the model pointer context (rightDrag of g_FrontendModelPointerContextVtable): moves the
   camera by the pointer's offset from the press point, then puts the pointer back there, snapshots the camera
   state and calls the view's clearTransientStateCallback. The motion depends on the camera scheme bit of the
   view (0x100, 0x8000 or 0x200), the left button and the modifier keys: move, heading, pitch, distance or a
   combination; 0x10 blocks camera input. FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
   shows the matching cursor.
*/
void FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  InGameWorldTransientStateClearCallbackProc *clearTransientCallback;
  uint32_t pointerDeltaX;
  AngleTurn32 pointerDeltaY;

  /* The low four runtimeFlags bits record the motion in progress: 1 move, 2 heading, 4 distance, 8 pitch.
     The pointer's X offset drives the heading, its Y offset pitch, distance and moves. */
  pointerDeltaX = pointerX - callbackContext->pointerCaptureX;
  pointerDeltaY = pointerY - callbackContext->pointerCaptureY;
  if ((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0) {
    return;
  }
  if ((callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT) != 0) {
    if ((g_CursorButtonState & LEFT) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | (FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
  }
  else if ((callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN) != 0) {
    if ((g_CursorButtonState & LEFT) == 0) {
      if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
        WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
                  (pointerDeltaY,pointerDeltaX,callbackContext);
        WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
                  (pointerDeltaY,callbackContext);
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE);
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | (FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
        WorldMotion_AdjustHeadingAndClearFieldGridDirty(pointerDeltaX,callbackContext);
        /* EDX: the pointer Y delta, as in the other pitch branches (the decompile lost it). */
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(pointerDeltaY,callbackContext);
      }
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
      WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustPositionMagnitudeClamp(pointerDeltaY,callbackContext);
    }
  }
  else {
    if ((callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE) == 0) {
      return;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_HEADING;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
    }
    else if (((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_HIDE_PANEL) != 0) && ((g_CursorButtonState & LEFT) != 0)) {
      /* Flag 0x4000000 with the button held: 0x40000000 selects pitch, 0x80000000 distance. */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~FRONTEND_CAMERA_MOTION_MASK;
      if ((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
    else if (((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_HIDE_PANEL) == 0) && ((g_CursorButtonState & LEFT) == 0)) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_MOVE;
      WorldRuntime_TranslateCameraByScreenDelta(pointerDeltaY,pointerDeltaX,callbackContext);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(callbackContext);
    }
    else {
      /* Heading drag (button state opposite to flag 0x4000000); afterwards 0x40000000 adds a distance step and
         0x80000000 a pitch step (flags re-read after the heading call). */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_DISTANCE | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_HEADING;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      if ((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
  }
  g_PointerSetPosition(callbackContext->pointerCaptureY,callbackContext->pointerCaptureX);
  clearTransientCallback = callbackContext->fieldRegion.clearTransientStateCallback;
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (clearTransientCallback != NULL) {
    clearTransientCallback(callbackContext);
  }
  return;
}


/* Address: 0x0050CED0.
   Wheel handler of the model pointer context (pointerWheel of g_FrontendModelPointerContextVtable): unless
   camera input is blocked (0x10) or the view has no camera scheme (0x100/0x200/0x8000), the scaled wheel
   delta changes the camera distance, with Ctrl the pitch, and the camera state is snapshotted.
*/
void FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  int scaledWheelDelta;
  
  if (((callbackContext->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) &&
     ((callbackContext->runtimeFlags &
      (FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN | FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE |
       FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT)) != 0)) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_PITCH);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_DISTANCE;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(scaledWheelDelta,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
    else {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & ~(FRONTEND_CAMERA_MOTION_MOVE | FRONTEND_CAMERA_MOTION_HEADING | FRONTEND_CAMERA_MOTION_DISTANCE);
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | FRONTEND_CAMERA_MOTION_PITCH;
      WorldMotion_AdjustPitchClampAndRecomputePosition(scaledWheelDelta,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
  }
  return;
}


/* Address: 0x0050CF50.
   Keyboard handler of the model pointer context (keyboardEvent of g_FrontendModelPointerContextVtable): offers
   the key to the view's keyboardFallback first; when there is none or it returns true, the default handling
   (UiNode_DefaultKeyboardEventMoveFocusNext) decides and its result is returned.
*/
bool FrontendModelPointerContext_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerHitContext *control)

{
  if (control->keyboardFallback != NULL &&
      !control->keyboardFallback(keyboardStateMask,keyCode,(UiRootNode *)control)) {
    return false;
  }
  return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
}


/* Address: 0x0050CF90.
   Tick method of the model pointer context (tick of g_FrontendModelPointerContextVtable): counts the ticks the
   right button is held (rightButtonHeldTicks, read by FrontendModelPointerContext_RightRelease) and, in a view
   without camera scheme 0x100/0x8000, camera input block (0x10) and WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA, eases
   the camera distance by one convergence step per tick until it is within 15/16..17/16 of the clamped committed
   distance, placing the camera behind the target.
*/
void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext)

{
  uint32_t *callbackStateCounter;
  UQ12 targetDistance;
  UQ12 convergenceStep;
  UQ12 clampedCommittedDistance;
  FixedDirection cameraOffset;
  
  /* 0x40: right button held (ROUTE_TO_BUILTIN_ACTION_RESOLUTION); +0x11C is rightButtonHeldTicks of the
     pointer-context view of this record */
  if ((callbackContext->runtimeFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION) != 0) {
    callbackStateCounter = &callbackContext->selection.rightButtonHoldTicks;
    *callbackStateCounter = *callbackStateCounter + 1;
  }
  if ((callbackContext->runtimeFlags &
       (WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA | FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN |
        FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT | WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO)) == 0) {
    clampedCommittedDistance = callbackContext->motion.committedDistanceQ12;
    if ((int)clampedCommittedDistance < (int)callbackContext->minimumCameraDistanceQ12) {
      clampedCommittedDistance = callbackContext->minimumCameraDistanceQ12;
    }
    if ((int)callbackContext->maximumCameraDistanceQ12 < (int)clampedCommittedDistance) {
      clampedCommittedDistance = callbackContext->maximumCameraDistanceQ12;
    }
    targetDistance = callbackContext->motion.targetDistanceQ12;
    convergenceStep = g_WorldMotionTargetDistanceConvergenceStepQ12;
    if ((int)(clampedCommittedDistance * 15) >> 4 <= (int)targetDistance) {
      if ((int)targetDistance <= (int)(clampedCommittedDistance * 17) >> 4) {
        return;
      }
      convergenceStep = -g_WorldMotionTargetDistanceConvergenceStepQ12;
    }
    callbackContext->motion.targetDistanceQ12 = targetDistance + convergenceStep;
    /* heading ^ 0x8000 turns half round: the camera sits behind the target */
    cameraOffset = FixedMath_DirectionFromAnglesScaled
                      (-callbackContext->motion.pitchAngle,
                       callbackContext->motion.headingAngle ^ FIXED_ANGLE16_HALF_TURN,targetDistance + convergenceStep);
    callbackContext->motion.positionXQ12 =
         cameraOffset.x + callbackContext->motion.targetPositionXQ12;
    callbackContext->motion.positionYQ12 =
         cameraOffset.y + callbackContext->motion.targetPositionYQ12;
    callbackContext->motion.positionZQ12 =
         cameraOffset.z + callbackContext->motion.targetPositionZQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  }
  return;
}


/* Address: 0x00514E40.
   Refreshes the HUD resource numbers of the active faction in the in-game root: Xenite and Tritium
   (current / storage limit), Energy demand / generation capacity, and baseline Energy supply plus the Tritium
   extraction rate. Q4 amounts are shown as whole units (>> 4); the Xenite amount is also formatted as text.
*/
void FrontendRuntime_UpdateCurrentFactionMetricCache(void)

{
  XeniteAmountQ4 xeniteStorageLimit;
  TritiumAmountQ4 tritiumStorageLimit;
  int xeniteCurrentDisplay;
  FactionProgressAmountQ4 baselineEnergySupplyQ4;
  FactionProgressAmountQ4 energyGenerationCapacityQ4;
  FactionArmyContributionValue tritiumExtractionRate;
  InGameRuntimeRoot *runtimeRoot;
  int activeFactionIndex;
  
  runtimeRoot = g_InGameRuntimeRoot;
  activeFactionIndex = g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex;
  xeniteStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteStorageLimitQ4;
  xeniteCurrentDisplay = (int)g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteCurrentQ4 >> 4;
  g_InGameRuntimeRoot->primaryResourceDisplayCurrent = xeniteCurrentDisplay;
  runtimeRoot->primaryResourceDisplayLimit = (int)xeniteStorageLimit >> 4;
  /* decimal, no fraction digits */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,xeniteCurrentDisplay,
             (uint16_t *)&g_FrontendCurrentFactionPrimaryResourceTextUtf16);
  tritiumStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumStorageLimitQ4;
  baselineEnergySupplyQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].baselineEnergySupplyQ4;
  runtimeRoot->secondaryResourceDisplayCurrent =
       (int)g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumCurrentQ4 >> 4;
  runtimeRoot->secondaryResourceDisplayLimit = (int)tritiumStorageLimit >> 4;
  energyGenerationCapacityQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].energyGenerationCapacityQ4;
  tritiumExtractionRate =
       g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumExtractionRateQ4PerTick;
  runtimeRoot->energyDemandDisplay =
       (int)(g_GameFactionRuntimeImage.records[activeFactionIndex].suppliedEnergyDemandQ4 +
            g_GameFactionRuntimeImage.records[activeFactionIndex].unpoweredEnergyDemandQ4) >> 4;
  runtimeRoot->energyCapacityDisplay = (int)energyGenerationCapacityQ4 >> 4;
  /* the extraction rate is added unshifted, as in the original */
  runtimeRoot->baselineEnergySupplyDisplay =
       ((int)baselineEnergySupplyQ4 >> 4) + tritiumExtractionRate;
  return;
}


/* Address: 0x00547620.
   Periodic timer callback of the frontend (80 Hz): counts g_FrontendTimerCountdownTicks down to zero.
   Frontend_StateTick uses the countdown to pace its network polling.
*/
void __cdecl FrontendRuntime_TimerCountdownTick(void)

{
  if (g_FrontendTimerCountdownTicks != 0) {
    g_FrontendTimerCountdownTicks--;
  }
  return;
}

/* Address: 0x00547FB0.
   Periodic timer callback of the frontend (256 Hz): advances the clock of the menu camera flight while a ROM
   transition is pending; the flight's spline is evaluated at g_FrontendRomTransitionElapsedTicks.
*/
void __cdecl FrontendRomTransition_AdvanceElapsedTicks(void)

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks++;
  }
  return;
}

/* Address: 0x00548030.
   Keyboard fallback of the menu room's pointer context: looks the key up in the frontend hotkey table
   (commandCode + required modifier class, see KEYBOARD_STATE_*). Alt+Q and Alt+key 0x20004 leave the
   current menu: back to the main page, a network session is closed first; Ctrl+key 0x20001 on the faction
   setup page toggles bit 0 of the local player's colourCycleFlags (an eighth entry in the faction cycle,
   FrontendFactionSetup_CycleFactionColour). Returns true (CF set) when the key is not in the table.
*/
bool FrontendRuntime_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime)

{
  /* Rewritten from the assembly (0x00548030-0x0054813A and the continuations 0x00548140 /
     0x00548190-0x005483BA, which Ghidra does not assign to any function). The decompiled version
     jumped into the original machine code. EBX is g_FrontendRootNode. CF set = not handled. */
  UiCommandDispatchRecord *record = g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30;
  uint8_t *root = (uint8_t *)g_FrontendRootNode;
  uint32_t target = 0;

  (void)frontendRuntime;
  for (;; record++) {
    uint32_t flags = record->modifierClassFlags; /* the modifier classes the entry requires */
    if (record->commandCode == 0) {
      return true;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & KEYBOARD_STATE_ANY_MODIFIER) != 0) continue;
    }
    else {
      if ((flags & KEYBOARD_STATE_SHIFT) != 0) {
        if ((modifierFlags & KEYBOARD_STATE_SHIFT) == 0) continue;
      }
      else if ((modifierFlags & KEYBOARD_STATE_SHIFT) != 0) {
        continue;
      }
      if ((flags & KEYBOARD_STATE_ALT) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
      }
      else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
      else {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x548140:
    if (UiPageStack_ActivePageIndex((UiPageStackControl *)FRONTEND_UI(root,frontendPageStack)) ==
        FRONTEND_PAGE_FACTION_SETUP) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_XOR_PLAYER_STATE,0,0,1);
      }
      else {
        FrontendPlayerRuntime_XorStateMaskByPlayerId(g_LocalPlayerRuntimeId,0,0,1);
      }
    }
    break;
  case 0x548190: {
    FrontendPlayerRuntimeRecord *player;
    uint16_t *text;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0) {
      /* local game with no campaign or scenario loaded: only queue UI action 0 */
      if ((g_FrontendLoadedCampaignAsset == 0) && (g_FrontendScenarioInitializationCount == 0)) {
        UiActionQueue_Enqueue(0,root);
        break;
      }
      Resource_Release((void *)(uintptr_t)g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = 0;
      g_FrontendScenarioInitializationCount = 0;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      ((FrontendModelPointerContext *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      break;
    }
    /* Leaving a network session. The transcription names bit 0 the host (0x00548320) and bit 1 the client
       (0x00548250), but SESSION_NETWORK_ROLE_CLIENT is bit 0; the variable follows the enum. */
    {
      int wasClient = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != 0;
      g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_FrontendScenarioInitializationCount = 0;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      ((FrontendModelPointerContext *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      player = g_FrontendPlayerRuntimeBlocks;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      /* chat history notice with the first player's name */
      text = TextResource_Resolve(wasClient ? TEXT_ID_NETWORK_SESSION_LEFT : TEXT_ID_NETWORK_SESSION_CLOSED);
      RichTextCommandStream_PatchPayloadBySelector(0,&player->playerName,text);
      FrontendRecentTextHistory_InsertAndRebuild5(text);
      if (!wasClient) {
        /* the player record becomes a fresh local one (same fields as the client path of
           FrontendNetworkSetupPage_InitializeBackendMode) */
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        *(uint32_t *)&player->playerName = 0; /* first two code units */
        player->playerRuntimeId = 0;
        player->factionAssignment.roleStateFlags = 0;
        player->colourCycleFlags = 0;
        player->snapshotTransferFlags = 0;
      }
    }
    break;
  }
  default:
    Thandor_Log("Frontend dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}


/* Address: 0x00548700.
   Runs one record of the frontend ROM action table locally, with the activation sound (the direct-call form of
   the FRONTEND_COMMAND_EXECUTE_ROM_ACTION command handler).
*/
void FrontendState_DispatchCode(FrontendStatusCode romRecordIndex)

{
  FrontendRomActionTable_ExecuteRecord(0,0,false,romRecordIndex);
  return;
}


/* Address: 0x00548910.
   Hover handler of the menu room's pointer context (hoverCursorCallback/108). On the main page (not on a
   network client) an object of the room that has a usable ROM action record starts a camera flight towards
   the record's keyframe and makes the pointer cursor frame 7; the record's hint text (text id 0x2000 + hint)
   is shown in the hint box, hint 1 while a page action is still being processed, none otherwise.
   Actions 3, 4, 9 and negative ones are not offered in a network session, the network page (2) not without
   a network backend (the same rule as FrontendRomActionTable_ExecuteRecord). Returns the cursor frame index.
*/
uint32_t FrontendRuntime_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          void *pointedModelNode,FrontendPointerSceneRuntimeView *frontendRuntime)

{
  uint16_t *previousCommandStream;
  UiNodeBase *control;
  RomAssetRecordPrefix *pointedRomRecord;
  int *transitionRecord; /* keyframe channels 0..5, [6] hint text, [8] FRONTEND_PAGE_ACTION_* */
  uint32_t resultCode;
  uint16_t *commandStream;
  RomRecordId recordId;
  int hintValue;
  int keyframeChannel3;
  int keyframeChannel4;
  int keyframeChannel5;
  int halfWidth;
  int halfHeight;
  int frameWidth;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize windowTextureSize;

  resultCode = 0;
  hintValue = 0;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
     (UiPageStack_ActivePageIndex(&frontendRuntime->activePageStack) == 0)) {
    pointedRomRecord = RomRegistry_FindRecordBySlotValue((RomRegistrySlotValue)pointedModelNode);
    recordId = FRONTEND_ROM_RECORD_ID_NONE;
    if (pointedRomRecord != NULL) {
      recordId = pointedRomRecord->recordId;
    }
    transitionRecord = RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecord);
    /* Skip records without a transition, network-only pages (3/4/9/negative) in a networked session and the
       network page (2) when no backend exists. */
    if ((transitionRecord != NULL) &&
       ((((transitionRecord[8] != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          (transitionRecord[8] != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE)) &&
         (transitionRecord[8] != FRONTEND_PAGE_ACTION_CREDITS)) &&
         (transitionRecord[8] >= 0)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL)) &&
       ((transitionRecord[8] != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) || (g_NetworkBackendInstanceCount != 0))) {
      keyframeChannel3 = transitionRecord[3];
      keyframeChannel4 = transitionRecord[4];
      keyframeChannel5 = transitionRecord[5];
      /* Rebuild the camera spline only when the target keyframe changed. (When channels 0-2 already match
         the original skips storing them; storing the equal values here is equivalent.) */
      if ((((*transitionRecord != g_FrontendRomTransitionKeyframe1Channel0Q12) ||
           (transitionRecord[1] != g_FrontendRomTransitionKeyframe1Channel1Q12)) ||
          (transitionRecord[2] != g_FrontendRomTransitionKeyframe1Channel2Q12)) ||
         (((keyframeChannel3 != g_FrontendRomTransitionKeyframe1Channel3Q12) ||
          (keyframeChannel4 != g_FrontendRomTransitionKeyframe1Channel4Q12)) ||
          (keyframeChannel5 != g_FrontendRomTransitionKeyframe1Channel5Q12))) {
        /* fly from the current camera (keyframe 0) to the record's camera (keyframe 1) in 192 ticks of
           FrontendRomTransition_AdvanceElapsedTicks; pending -1 = no record to activate at the end */
        g_FrontendRomTransitionKeyframe1Channel0Q12 = *transitionRecord;
        g_FrontendRomTransitionKeyframe1Channel1Q12 = transitionRecord[1];
        g_FrontendRomTransitionKeyframe1Channel2Q12 = transitionRecord[2];
        g_FrontendRomTransitionKeyframe0Channel0Q12 = frontendRuntime->hitReferenceWorldXQ12;
        g_FrontendRomTransitionKeyframe0Channel1Q12 = frontendRuntime->hitReferenceWorldYQ12;
        g_FrontendRomTransitionKeyframe0Channel2Q12 = frontendRuntime->hitReferenceWorldZQ12;
        g_FrontendRomTransitionKeyframe0Channel3Q12 = frontendRuntime->projectionScale;
        g_FrontendRomTransitionKeyframe0Channel4Q12 = frontendRuntime->viewAngle0;
        g_FrontendRomTransitionKeyframe0Channel5Q12 = frontendRuntime->viewAngle1;
        g_FrontendRomTransitionKeyframe0TimeQ12 = 0;
        g_FrontendRomTransitionKeyframe1TimeQ12 = 192;
        g_FrontendRomTransitionElapsedTicks = 0;
        g_FrontendRomTransitionTargetRecordId = FRONTEND_ROM_TRANSITION_NO_TARGET;
        g_FrontendRomTransitionSplineKeyframeCount = 2;
        g_FrontendRomTransitionSplineKeyframes = &g_FrontendRomTransitionKeyframe0Channel0Q12;
        g_FrontendRomTransitionKeyframe1Channel3Q12 = keyframeChannel3;
        g_FrontendRomTransitionKeyframe1Channel4Q12 = keyframeChannel4;
        g_FrontendRomTransitionKeyframe1Channel5Q12 = keyframeChannel5;
        WorldMotionSpline_BuildSixChannelCurves
                  (2,(WorldMotionSplineKeyframe *)&g_FrontendRomTransitionKeyframe0Channel0Q12);
      }
      hintValue = transitionRecord[6];
      resultCode = 7;
    }
  }
  if (hintValue == 0) {
    if (g_FrontendPendingPageActionDepth == 0) {
      frontendRuntime->hintBox.hintActive = 0;
      frontendRuntime->hintBox.commandStream = NULL;
      return resultCode;
    }
    hintValue = 1;
  }
  /* only when the text changed: size the hint box around it plus the window frame (texture frame UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT) */
  previousCommandStream = frontendRuntime->hintBox.commandStream;
  commandStream = TextResource_Resolve(hintValue + TEXT_ID_MENU_HINT_BASE);
  if (commandStream != previousCommandStream) {
    frontendRuntime->hintBox.commandStream = commandStream;
    textExtent = RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,commandStream);
    halfWidth = (int)(textExtent.widthPixels + 1) >> 1;
    halfHeight = (int)(textExtent.heightPixels + 1) >> 1;
    frontendRuntime->hintBox.base.leftOffset = halfWidth;
    frontendRuntime->hintBox.base.topOffset = halfHeight;
    frontendRuntime->hintBox.base.right = -halfWidth;
    frontendRuntime->hintBox.base.bottom = -halfHeight;
    windowTextureSize = g_GraphicsTextureSourceGetLogicalSize(UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource);
    frameWidth = windowTextureSize.logicalWidthPixels + 3;
    control = frontendRuntime->hintBox.base.nextSibling;
    frontendRuntime->hintBox.hintActive = 1;
    frontendRuntime->hintBox.base.leftOffset = frontendRuntime->hintBox.base.leftOffset + frameWidth;
    frontendRuntime->hintBox.base.topOffset =
         frontendRuntime->hintBox.base.topOffset + windowTextureSize.logicalHeightPixels;
    frontendRuntime->hintBox.base.right = frontendRuntime->hintBox.base.right - frameWidth;
    frontendRuntime->hintBox.base.bottom =
         frontendRuntime->hintBox.base.bottom - windowTextureSize.logicalHeightPixels;
    control->vtable->layout(control);
  }
  return resultCode;
}


/* Address: 0x00548BE0.
   Button-press handler of the menu room's pointer context (buttonPressCallback): the frontend does nothing
   on press, it acts on release (FrontendMenuRoom_ExecuteClickedRomAction).
*/
void FrontendMenuRoom_PressNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
  return;
}

/* Address: 0x00548BF0.
   Drag handler of the menu room's pointer context (buttonDragCallback); dragging does nothing in the
   frontend.
*/
void FrontendMenuRoom_DragNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
  return;
}

/* Address: 0x00548C00.
   Button-release handler of the menu room's pointer context (buttonReleaseCallback): clicking an object of
   the menu room runs the ROM action record that belongs to it (FrontendRomActionTable_ExecuteRecord). In a
   network game the host sends it as a frontend command so every player follows; clients ignore clicks.
*/
void FrontendMenuRoom_ExecuteClickedRomAction
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          FrontendCallbackArgument5 pointedModelNode,uint32_t pointerContext)

{
  RomAssetRecordPrefix *slotRecord;
  RomRecordTableIndex recordIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    slotRecord = RomRegistry_FindRecordBySlotValue(pointedModelNode);
    if (slotRecord != NULL) {
      recordIndex = RomRecordTable_FindIndexById(slotRecord->recordId,g_FrontendActiveRomRecord);
      if (-1 < (int)recordIndex) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendRomActionTable_ExecuteRecord(g_LocalPlayerRuntimeId,0,0,recordIndex);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_EXECUTE_ROM_ACTION,0,0,recordIndex);
        }
      }
    }
  }
  return;
}


/* Address: 0x00548C70.
   Right-button release handler of the menu room's pointer context (rightClickCallback): stops the running
   camera flight (ScenarioCatalog_RequestRomTransitionStopCallback), in a network game as a frontend command
   sent by the host; clients ignore it.
*/
void FrontendMenuRoom_StopCameraFlight(uint32_t pointerContext)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RequestRomTransitionStopCallback(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_STOP_ROM_TRANSITION,0,0,0);
    }
  }
  return;
}

/* Address: 0x00548CB0.
   Adds a chat line to the shared recent-text history and rebuilds the frontend chat history box from its five
   newest entries.
*/
void FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text)

{
  RecentTextHistoryPointerList *output;
  
  /* lineCount + textLines of the chat history box form the pointer list. */
  output = (RecentTextHistoryPointerList *)
           &((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(5,output); /* the box shows five lines */
  return;
}


/* Address: 0x00549100.
   Handler of action 0x2043 (slot 67 of g_FrontendUiActionHandlersPage20.handlers00_54), the mission briefing's
   "Back" button: applies the game speed and returns to the main page with ROM action record 0
   (FRONTEND_COMMAND_APPLY_GAME_SPEED in a network game).
*/
void FrontendSessionAction_ApplyGameSpeedAndReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_APPLY_GAME_SPEED,0,0,0);
  }
  return;
}


/* Address: 0x00549140.
   Handler of action 0x204F (slot 79 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Exit" button of
   the in-game variant of the mission briefing: releases the loaded campaign and returns to the main page
   (FRONTEND_COMMAND_RELEASE_CAMPAIGN in a network game).
*/
void FrontendSessionAction_ReleaseCampaignAndReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReleaseSelectedResourceAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RELEASE_CAMPAIGN,0,0,0);
  }
  return;
}


/* Address: 0x00549180.
   Handler of action 0x2050 (slot 80 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Save" button of
   the in-game variant of the mission briefing: does nothing.
*/
void FrontendCallback_NoOpArg1(void *source)

{
  return;
}


/* Address: 0x00549AB0.
   Handler of action 0x2040 (slot 64 of g_FrontendUiActionHandlersPage20.handlers00_54), the faction setup
   page's "Back" button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE
   in a network game).
*/
void FrontendFactionSetupAction_ReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}


/* Address: 0x0054A5A0.
   Handler of action 0x2034 (slot 52 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Choose game"
   page's "Cancel" button: returns to the main page with ROM action record 0 in a local game and with record 4
   (as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE) in a network game.
*/
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

{
  /* the inner tests repeat the outer one, so only the local-direct and network-queued paths are reachable */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,4);
  }
  return;
}


/* Address: 0x0054A7D0.
   Handler of action 0x2033 (slot 51 of g_FrontendUiActionHandlersPage20.handlers00_54), the quit dialog's "no"
   button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a
   network game).
*/
void FrontendQuitDialogAction_ReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}


/* Address: 0x0054AAD0.
   Handler of action 0x2010 (slot 16 of g_FrontendUiActionHandlersPage20.handlers00_54), shared by the options
   page's "Ok" button and the display settings page's "Back" button: "Ok" returns to the main page (ROM action
   record 0, FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a network game), "Back" reopens the options page.
*/
void FrontendOptionsAction_ReturnToMainOrOptionsPage(UiNodeBase *sourceNode)

{
  FrontendModelPointerContextFlags *compactLayoutFlags;
  int parentNodeAddress;
  FrontendRootPageState *frontendRootPage;
  
  parentNodeAddress = (int)sourceNode->parent;
  frontendRootPage = (FrontendRootPageState *)sourceNode;
  while ((UiNodeBase *)parentNodeAddress != UI_NODE_NONE) {
    frontendRootPage = (FrontendRootPageState *)(frontendRootPage->rootNode).parent;
    parentNodeAddress = (int)(frontendRootPage->rootNode).parent;
  }
  if (sourceNode == &frontendRootPage->returnToMainActionControl) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
    return;
  }
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,&frontendRootPage->primaryPageStack);
  return;
}


/* Inserts value into the ascending list candidates[0..candidateCount-1] (UI_DISPLAY_MODE_NONE marks empty
   slots) unless it is already listed; the largest entry falls off the end. */
static void FrontendDisplayModeCandidates_InsertSortedUnique
          (uint32_t *candidates,uint32_t candidateCount,uint32_t value)
{
  uint32_t candidateIndex;
  uint32_t displacedValue;

  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value == candidates[candidateIndex]) {
      return;
    }
  }
  for (candidateIndex = 0; candidateIndex < candidateCount; candidateIndex++) {
    if (value < candidates[candidateIndex]) {
      displacedValue = candidates[candidateIndex];
      candidates[candidateIndex] = value;
      value = displacedValue;
    }
  }
}

/* Adapter row adapterIndex of the display settings page: driver description and device name (the software
   device gets its text resource name). */
static void FrontendDisplaySettingsPage_FillAdapterRow
          (FrontendDisplaySettingsPageOptionState *source,uint32_t adapterIndex)
{
  source->adapterRows.rows[adapterIndex].adapterDescriptionUtf16 =
       g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16;
  if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
    source->adapterRows.rows[adapterIndex].deviceNameUtf16 =
         TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
  }
  else {
    source->adapterRows.rows[adapterIndex].deviceNameUtf16 = g_GraphicsAdapters[adapterIndex].deviceNameUtf16;
  }
}

/* Address: 0x0054AB70.
   Handler of action 0x2011 (slot 17 of g_FrontendUiActionHandlersPage20.handlers00_54), the options page's
   "Graphics" button: opens the display settings page and fills its choices: the four smallest distinct colour
   depths and the ten smallest distinct resolutions (width << 16 | height) of g_GraphicsDisplayModes, each
   collected by an insertion into a sorted list with 0xFFFFFFFF as the empty mark, and the name and device of
   up to five adapters. The saved adapter, resolution and colour depth become the current selection.
*/
void FrontendDisplaySettingsAction_OpenPageAndListModes(FrontendDisplaySettingsPageOptionState *source)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  uint32_t *candidates;
  uint32_t adapterIndex;
  uint32_t rowIndex;
  GraphicsDisplayModeCount remainingModes;
  GraphicsDisplayMode *displayMode;

  candidates = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues;
  /* source is the frontend template's graphicsSettingsButton */
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,
             (UiPageStackControl *)FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),
                                               frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContext *)
           FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),menuRoomModelView))->
         contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  /* the four smallest distinct colour depths */
  for (rowIndex = 0; rowIndex < 4; rowIndex++) {
    candidates[rowIndex] = UI_DISPLAY_MODE_NONE;
  }
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    FrontendDisplayModeCandidates_InsertSortedUnique(candidates,4,displayMode->bitsPerPixel);
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  for (rowIndex = 0; rowIndex < 4; rowIndex++) {
    source->colorDepthRows.rows[rowIndex].bitsPerPixel = candidates[rowIndex];
  }
  /* name and device of up to five adapters (the first one is always listed) */
  FrontendDisplaySettingsPage_FillAdapterRow(source,0);
  for (adapterIndex = 1; (adapterIndex < 5) && (adapterIndex < g_GraphicsAdapterCount); adapterIndex++) {
    FrontendDisplaySettingsPage_FillAdapterRow(source,adapterIndex);
  }
  /* the ten smallest distinct resolutions */
  for (rowIndex = 0; rowIndex < 10; rowIndex++) {
    candidates[rowIndex] = UI_DISPLAY_MODE_NONE;
  }
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    FrontendDisplayModeCandidates_InsertSortedUnique
              (candidates,10,displayMode->width * UI_DISPLAY_MODE_WIDTH_SCALE + displayMode->height);
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  for (rowIndex = 0; rowIndex < 10; rowIndex++) {
    source->resolutionRows.rows[rowIndex].width = candidates[rowIndex] >> 16;
    source->resolutionRows.rows[rowIndex].height = candidates[rowIndex] & UI_DISPLAY_MODE_HEIGHT_MASK;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  adapterIndex = PersistentSettings_Read(1,PERSISTENT_SETTING_ADAPTER_INDEX);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
  FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)source);
  return;
}


/* Address: 0x0054BA30.
   Handler of actions 0x202C..0x2030 (slots 44..48 of g_FrontendUiActionHandlersPage20.handlers00_54), the five
   adapter choices of the display settings page: selects the adapter whose button was pressed (identified by
   its offset in the parent container, 0x68 bytes apart) and refreshes which modes can be chosen.
*/
void FrontendDisplaySettingsAction_SelectAdapter(UiNodeBase *sourceNode)

{
  int controlOffsetFromParent;
  FrontendDisplayAdapterIndex adapterIndex;

  controlOffsetFromParent = (int)sourceNode - (int)sourceNode->parent;
  /* any button other than options 1..4 selects adapter 4 */
  if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption1)) {
    adapterIndex = 0;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption2)) {
    adapterIndex = 1;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption3)) {
    adapterIndex = 2;
  }
  else if (controlOffsetFromParent == FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(displayAdapterOption4)) {
    adapterIndex = 3;
  }
  else {
    adapterIndex = 4;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.adapterIndex = adapterIndex;
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(sourceNode->parent);
  return;
}


/* Address: 0x0054D3F0.
   Handler of action 0x200C (slot 12 of g_FrontendUiActionHandlersPage20.handlers00_54), a selection change in
   the host lobby's player list: the Kick button (FRONTEND_ACTION_KICK_PLAYER) is hidden while the first row,
   the host itself, is selected and shown for any other player.
*/
void FrontendHostLobby_UpdateKickButtonForSelection(UiPointerListControl *playerListControl)

{
  UiPointerListControl *frontendRoot;
  UiNodeBase *parentCursor;
  
  parentCursor = playerListControl->base.parent;
  frontendRoot = playerListControl;
  while (parentCursor != UI_NODE_NONE) {
    frontendRoot = (UiPointerListControl *)(frontendRoot->base).parent;
    parentCursor = frontendRoot->base.parent;
  }
  if (playerListControl->selectedRowSlot == playerListControl->rowSlots) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendRoot->base);
    return;
  }
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendRoot->base);
  return;
}


/* Address: 0x0054D460.
   Handler of action 0x200E (slot 14 of g_FrontendUiActionHandlersPage20.handlers00_54), a click on the chat
   strip at the top left: drops the oldest chat lines until four are left, then one more (a click removes the
   oldest line shown), and rebuilds the strip's pointer list of at most five lines.
*/
void FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source)

{
  uint32_t currentEntryCount;
  
  for (currentEntryCount = ((UiConditionalActionControl *)source)->lineCount; 4 < currentEntryCount;
      currentEntryCount--) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)source)->lineCount);
  return;
}


/* Address: 0x0054D4A0.
   Handler of action 0x200F (slot 15 of g_FrontendUiActionHandlersPage20.handlers00_54), a choice in the network
   game page's protocol list: closes the current backend and opens the chosen one on NETWORK_GAME_UDP_PORT. On
   success the local endpoint is copied to g_FrontendNetworkEndpointScratch and formatted into
   g_FrontendNetworkEndpointTextUtf16, the session list is emptied, Join hidden and a discovery probe sent. A failure is reported and the backend opened once more without a report; if that
   fails too, the menu returns to the main page and the random generator to the primary stream.
*/
void FrontendNetworkSetup_OpenSelectedBackend(FrontendNetworkSetupPageBackendListPtr backendList)

{
  UiListRowIndex selectedBackendIndex;
  int remainingDwords;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  uint32_t backendError; /* 0 or a FATAL_ERROR_NETWORK_* code */

  selectedBackendIndex = UiPointerList_GetSelectedIndex(backendList);
  if (g_NetworkBackendInstanceCount <= selectedBackendIndex) {
    return;
  }
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  backendError = g_NetworkBackendSlot0(selectedBackendIndex);
  FatalError_ReportIfFailed(backendError,backendError != 0); /* reports and returns: the flag is ours */
  if (backendError == 0) {
    backendError = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    FatalError_ReportIfFailed(backendError,backendError != 0);
    if (backendError == 0) {
      endpointSourceDwordCursor = (uint32_t *)&g_NetworkLocalEndpoint;
      endpointDestinationDwordCursor = (uint32_t *)&g_FrontendNetworkEndpointScratch;
      for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor++;
        endpointDestinationDwordCursor++;
      }
      g_NetworkBackendSlot7
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->rootNode);
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->sessionList);
      UiTransfer_SendDiscoveryProbe();
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  if (g_NetworkBackendSlot0(selectedBackendIndex) == 0) {
    if (g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT) == 0) {
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  Random_SelectPrimaryStream();
  return;
}


/* Campaign level records (CampaignLevelRecord) as in OldUnitRuntime_RebuildScenarioReplayTables: finds the
   current level's record and points g_EndMoviePath at flm\endeNNNN.flm with its end movie number for the
   outcome g_EndMovieSelectionIndex (separate numbers for a nonzero / zero variant index). */
static void FrontendEndMovie_SelectCampaignMoviePath(CampaignAsset *campaign)
{
  int remainingRecords;
  CampaignLevelRecord *levelRecord;
  int32_t endMovieNumber;

  remainingRecords = campaign->levelRecordCount;
  levelRecord = campaign->levels;
  do {
    if (campaign->currentLevelId == levelRecord->levelId) {
      if (g_EndMovieVariantIndex == 0) {
        endMovieNumber = levelRecord->endMovieNumbers[g_EndMovieSelectionIndex];
      }
      else {
        endMovieNumber = levelRecord->endMovieNumbersVariant[g_EndMovieSelectionIndex];
      }
      /* four zero-padded digits over the "0000" of flm\ende0000.flm */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,endMovieNumber,(uint16_t *)(u_flm_ende0000_flm_0050df4a + 8));
      g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df4a;
      return;
    }
    levelRecord++;
    remainingRecords--;
  } while (remainingRecords != 0);
}

/* Fills the whole framebuffer with opaque black and presents it (skipped when the buffer cannot be
   accessed). */
static void FrontendEndMovie_ClearAndPresentBlackFrame(void)
{
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
               ,0,UI_ARGB_OPAQUE_BLACK,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
  }
}

/* Results page after the end movie: scores of every faction that took part, elapsed time and level title;
   then draws frames until a results button sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED. Nothing is shown
   when no faction took part. */
static void FrontendEndMovie_ShowResultsPage(InGameRuntimeRoot *runtimeRoot)
{
  int activeFactionCount;
  int factionIndex;
  uint64_t elapsedTimeUnits;
  uint16_t *resultsText;
  TextResourceId levelTitleResourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendResultsEightColumnTemplate *firstChart;
  uint32_t previousColumnCount;
  int remainingColumns;
  uint32_t *copySource;
  uint32_t *copyDestination;

  /* scores of every faction 1..7 that took part (lifecycle state not 0) */
  activeFactionCount = 0;
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      activeFactionCount++;
      GameFactionRuntime_RecomputeProgressAndScoreMetrics(factionIndex,&runtimeRoot->worldRuntime);
    }
  }
  if (activeFactionCount == 0) {
    return;
  }
  /* row counts of the three results lists */
  firstChart = (FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart1);
  firstChart->rowCount = activeFactionCount;
  ((FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart2))->rowCount = activeFactionCount;
  ((FrontendResultsEightColumnTemplate *)INGAME_UI(runtimeRoot,resultsChart3))->rowCount = activeFactionCount;
  /* elapsed minutes of the 80 Hz clock, rounded up, shown as hours and minutes */
  elapsedTimeUnits = (uint64_t)(g_GameFactionRuntimeImage.tail.periodicClockTick + 4799) / 4800;
  g_LocaleFormatTimeFieldsUtf16
            ((uint32_t)(elapsedTimeUnits / 60),(uint32_t)(elapsedTimeUnits % 60),
             (uint16_t *)&g_EndGameElapsedTimeScratchUtf16);
  resultsText = TextResource_Resolve(TEXT_ID_RESULTS_TITLE_TEMPLATE);
  levelTitleResourceId = g_InGameLevelTitleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE;
  RichTextCommandStream_PatchPayloadBySelector(1,&g_EndGameElapsedTimeScratchUtf16,resultsText);
  RichTextCommandStream_PatchPayloadBySelector(0,TextResource_Resolve(levelTitleResourceId),resultsText);
  /* the continue button; 0x1025 is the second results button, local games hide it */
  UiNodeList_UnsuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_SECONDARY_EXIT,(UiNodeBase *)runtimeRoot);
  }
  /* a host with other players waits for them instead of offering continue */
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
     && (1 < g_FrontendPlayerRuntimeBlockCount)) {
    UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
  }
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    playerBlock->factionAssignment.readyOrWaitState = 0;
    remainingPlayerBlocks--;
    playerBlock++;
  } while (remainingPlayerBlocks != 0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    /* local game: remove column 2 (the third) of resultsChart1; the shift copies the count - 3 later ones */
    previousColumnCount = firstChart->columnTypeCount;
    firstChart->columnTypeCount = firstChart->columnTypeCount - 1;
    remainingColumns = previousColumnCount - 3;
    if (2 < previousColumnCount && remainingColumns != 0) {
      copySource = &firstChart->columnTypes[3];
      copyDestination = &firstChart->columnTypes[2];
      for (; remainingColumns != 0; remainingColumns--) {
        *copyDestination = *copySource;
        copySource++;
        copyDestination++;
      }
    }
  }
  do {
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(0xffffffff);
    }
  } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED) == 0);
}

/* Address: 0x00565A30.
   End of a mission: plays the end movie chosen by the current scenario's record in the loaded campaign
   (flm\endeNNNN.flm for the outcome g_EndMovieSelectionIndex and variant g_EndMovieVariantIndex) on the
   in-game root, then shows the results page (per-faction scores, elapsed time, level title) until a results
   button sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED. Without an in-game root or end movie path, or when the
   movie cannot be opened, it only installs the results-screen callbacks.
*/
void Frontend_PlaySelectedEndMovie(void)

{
  UiRootCallbacks *rootCallbacks;
  InGameRuntimeRoot *runtimeRoot;
  uint32_t playbackRateHz;
  bool movieOpened;

  runtimeRoot = g_InGameRuntimeRoot;
  g_GraphicsCursorSetFrame(0);
  g_CursorVisibilityToken--;
  if ((runtimeRoot != NULL) && (g_EndMoviePath != NULL)) {
    rootCallbacks = runtimeRoot->rootUi.callbacks;
    rootCallbacks->keyboardFallback = EndMovieUiRuntime_DispatchCommandByFlags;
    rootCallbacks->frameUpdate = EndMovieUiRuntime_HandleModeTransition;
    if (g_FrontendLoadedCampaignAsset != 0) {
      FrontendEndMovie_SelectCampaignMoviePath((CampaignAsset *)g_FrontendLoadedCampaignAsset);
    }
    Movie_Close();
    /* clear both buffers to black */
    FrontendEndMovie_ClearAndPresentBlackFrame();
    FrontendEndMovie_ClearAndPresentBlackFrame();
    movieOpened = Movie_Open(1,g_EndMoviePath,&playbackRateHz,NULL);
    runtimeRoot = g_InGameRuntimeRoot;
    if (movieOpened) {
      g_EndMoviePendingTicks = 0;
      g_TimerRegisterPeriodic(playbackRateHz,FrontendSession_PeriodicTick);
      UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(runtimeRoot,primaryPageStack));
      if (Movie_AdvanceFrame(&runtimeRoot->activeEndMovieRuntime,NULL)) {
        runtimeRoot->endMoviePlaybackState = 0;
        g_EndMoviePendingTicks = 0;
        /* one movie frame per timer tick until the movie ends (or the end-movie flag is cleared elsewhere) */
        do {
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks--;
            if (!Movie_AdvanceFrame(NULL,NULL)) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)runtimeRoot);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0);
      }
      g_CursorVisibilityToken++;
      UiPageStack_SetActiveIndex(1,&runtimeRoot->endMoviePageStack);
      FrontendEndMovie_ShowResultsPage(runtimeRoot);
      g_TimerUnregisterPeriodic(FrontendSession_PeriodicTick);
      Movie_Close();
    }
    else {
      g_CursorVisibilityToken++;
    }
  }
  else {
    g_CursorVisibilityToken++;
  }
  rootCallbacks = g_InGameRuntimeRoot->rootUi.callbacks;
  rootCallbacks->keyboardFallback = InGameHotkeys_DispatchCommandByFlags;
  rootCallbacks->frameUpdate = InGameUiRoot_UpdateFrame;
  return;
}


/* Menu sounds of Frontend_Init: counts the two digits of "sound\menue01.sam" from 01 up to 99 into the
   voice-set table slots 1..99 and stops at the first file that does not exist. Returns 0, or the voice-set
   creation error. */
static uint32_t FrontendInit_LoadMenuSounds(void)
{
  uint32_t *voiceSetSlot;
  SoundSampleAsset *loadedSample;
  DirectSoundVoiceSet *menuVoiceSet;
  uint32_t voiceSetError;

  u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] = L'0';
  u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] = L'1';
  voiceSetSlot = &g_FrontendMenuSoundVoiceSetLoadBaseEntry1;
  while ((uint16_t)u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] < L'9' + 1) {
    while ((uint16_t)u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] < L'9' + 1) {
      if (!Resource_Load((uint16_t *)u_sound_menue01_sam_00545b54,(void **)&loadedSample,NULL,NULL)) {
        return 0;
      }
      voiceSetError = g_SoundCreateSampleVoiceSet(loadedSample,&menuVoiceSet);
      if (voiceSetError != 0) {
        Resource_Release(loadedSample);
        return voiceSetError;
      }
      *voiceSetSlot = (uint32_t)menuVoiceSet;
      Resource_Release(loadedSample);
      u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] =
           u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] + 1;
      voiceSetSlot++;
    }
    u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] =
         u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_TENS_DIGIT] + 1;
    u_sound_menue01_sam_00545b54[FRONTEND_MENU_SOUND_PATH_ONES_DIGIT] = L'0';
  }
  return 0;
}

/* Menu music of Frontend_Init: when music is enabled, loads sound\music00.sam and plays it looping at the
   saved music gain (g_FrontendMusicVoiceSet / g_FrontendMusicActiveBuffer). Failures leave the menu silent. */
static void FrontendInit_StartMenuMusic(void)
{
  uint32_t soundOptions;
  uint32_t musicGain;
  SoundSampleAsset *loadedSample;
  DirectSoundVoiceSet *musicVoiceSet;
  IDirectSoundBuffer *musicBuffer;

  soundOptions = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  musicBuffer = (IDirectSoundBuffer *)g_FrontendMusicActiveBuffer;
  if (((soundOptions & PERSISTENT_SOUND_OPTION_MUSIC) != 0) &&
      Resource_Load((uint16_t *)u_sound_music00_sam_00545c4e,(void **)&loadedSample,NULL,NULL)) {
    if (g_SoundCreateSampleVoiceSet(loadedSample,&musicVoiceSet) != 0) {
      Resource_Release(loadedSample);
    }
    else {
      g_FrontendMusicVoiceSet = (uint32_t)musicVoiceSet;
      Resource_Release(loadedSample);
      musicGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
      if (!g_SoundPlayLooping(musicGain,musicGain,musicVoiceSet,&musicBuffer)) {
        g_SoundReleaseSampleVoiceSet(musicVoiceSet);
        g_FrontendMusicVoiceSet = 0;
        musicBuffer = (IDirectSoundBuffer *)g_FrontendMusicActiveBuffer;
      }
    }
  }
  g_FrontendMusicActiveBuffer = (uint32_t)musicBuffer;
}

/* Fills the frontend's network-backend list with the backends' display names (0x100 bytes apart); the row
   pointer table is networkBackendNameRows (0x005434EC), right after the control offset tables. */
static void FrontendInit_FillNetworkBackendList(FrontendRootResourceSlots *frontendUiState)
{
  uint32_t backendCount;
  uint32_t backendIndex;
  uint16_t *backendDisplayName;

  backendCount = g_NetworkBackendInstanceCount;
  if (g_NetworkBackendInstanceCount == 0) {
    return;
  }
  backendDisplayName = g_NetworkBackendInstanceTable->displayNameUtf16;
  for (backendIndex = 0; backendIndex < backendCount; backendIndex++) {
    g_FrontendTaskAssignmentControlOffsets.networkBackendNameRows[backendIndex] = (uint32_t)backendDisplayName;
    backendDisplayName = backendDisplayName + 128; /* 0x100 bytes */
  }
  UiPointerList_InitializeMeasuredTextRows
            (backendCount,(void **)g_FrontendTaskAssignmentControlOffsets.networkBackendNameRows,
             (UiPointerListControl *)FRONTEND_UI(frontendUiState,networkProtocolList));
}

/* Installs the menu room's handlers on its 3D pointer-context control (root+0x368); they do not all match the
   generic callback field types, hence the casts. */
static void FrontendInit_InstallMenuRoomPointerCallbacks(FrontendModelPointerContext *pointerContext)
{
  typedef uint32_t FrontendModelPointerResolvedActionProc
          (uint32_t,uint32_t,uint32_t,int,struct ModelRuntimeNode *,struct FrontendModelPointerHitContext *);

  pointerContext->keyboardFallback =
       (bool (*)(UiKeyboardStateMask,UiActionId,struct UiRootNode *))
       FrontendRuntime_DispatchCommandByCodeAndModifierFlags;
  pointerContext->hoverCursorCallback =
       (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneView;
  pointerContext->heldButtonCursorCallback =
       (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneView;
  pointerContext->buttonPressCallback =
       (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_PressNoOp;
  pointerContext->buttonDragCallback =
       (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_DragNoOp;
  pointerContext->buttonReleaseCallback =
       (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_ExecuteClickedRomAction;
  pointerContext->clearTransientStateCallback = 0;
  pointerContext->rightClickCallback =
       (void (*)(FrontendModelPointerContext *))FrontendMenuRoom_StopCameraFlight;
  pointerContext->renderSpinLock = (RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock;
  pointerContext->renderSpinLockReleaseCallback = Frontend_StateTick;
}

/* Copies one saved name (PERSISTENT_SETTINGS_NAME_BYTES, 10 dwords) dword by dword. */
static void FrontendInit_CopyNameDwords(uint32_t *destination,const uint32_t *source)
{
  int remainingDwords;

  for (remainingDwords = PERSISTENT_SETTINGS_NAME_BYTES / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *destination = *source;
    source++;
    destination++;
  }
}

/* Address: 0x00546700.
   Builds the frontend (menu) at the ROM record initialRomRecordId: clears the screen, loads the central
   texture set, palette, menu sounds (sound\menueNN.sam until the first missing one), engine\zentrale.rom and
   the menu music, creates the 0x5954-byte frontend root from its template and pushes it on the UI root stack,
   installs the 3D menu-room callbacks and activates the record's camera transition. It then reports this
   player ready and draws frames until every player is ready (network sessions wait here for the peers).
   Returns true on success (the root is g_FrontendRootNode); false with the failing call's error in *outError.
   FrontendRuntime_ShutdownAndReleaseResources undoes it.
*/
bool Frontend_Init(RomRecordId initialRomRecordId,uint32_t *outError)

{
  uint32_t settingValue;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingBlockCount;
  GraphicsTextureSet *centralTextureSet;
  GraphicsPaletteAsset *centralPaletteAsset;
  uint32_t centralResourceErrorCode;
  uint32_t error;
  void *centralRomAsset;
  uint32_t romLoadErrorCode;
  void *allocPayload;
  uint32_t *zeroCursor;
  uint32_t *templateDwords;
  uint32_t *rootDwords;
  int remainingDwords;
  FrontendRootResourceSlots *frontendUiState;
  WorldRuntimeContext *worldRuntime;
  uint32_t *savedPlayerName;

  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = settingValue >> 1;
  /* network session: every player starts not ready */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
    do {
      playerBlock->factionAssignment.readyOrWaitState = 0;
      playerBlock->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      playerBlock++;
      remainingBlockCount--;
    } while (remainingBlockCount != 0);
  }
  if (!g_GraphicsFramebufferBeginAccess()) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,UI_ARGB_OPAQUE_BLACK,g_FramebufferAccess); /* opaque black */
    g_GraphicsFramebufferEndAccess();
  }
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  g_FrontendRomTransitionTargetRecordId = 0;
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
  g_FrontendRuntimeFlags = FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS;
  g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
  g_FrontendStateTickSpinLock = 0;
  g_TimerRegisterPeriodic(FRONTEND_PERIODIC_TIMER_HZ,FrontendRuntime_TimerCountdownTick);
  UiRuntime_SetSynchronizationHooks(Frontend_StateTick,&g_FrontendStateTickSpinLock);
  centralTextureSet = g_GraphicsTextureSetLoadPackage((uint16_t *)u_gfx_texturen_zentrale_gfx_00545acc,
                                                      &centralResourceErrorCode);
  if (centralTextureSet == NULL) {
    *outError = centralResourceErrorCode;
    return false;
  }
  g_FrontendCentralTextureSet = (uint32_t)centralTextureSet;
  centralPaletteAsset = g_GraphicsPaletteAssetLoadPackage((uint16_t *)u_gfx_texturen_zentrale_pal_00545b00,
                                                          &centralResourceErrorCode);
  if (centralPaletteAsset == NULL) {
    *outError = centralResourceErrorCode;
    return false;
  }
  g_FrontendCentralPaletteAsset = (uint32_t)centralPaletteAsset;
  RichTextCommandStream_PatchPayloadBySelector
            (0,&g_FrontendNetworkEndpointTextUtf16,TextResource_Resolve(TEXT_ID_NETWORK_ADDRESS_TEMPLATE));
  error = FrontendInit_LoadMenuSounds();
  if (error != 0) {
    *outError = error;
    return false;
  }
  centralRomAsset = Package_LoadEntry((uint16_t *)u_engine_zentrale_rom_00545aa4,&romLoadErrorCode);
  if (centralRomAsset == NULL) {
    *outError = romLoadErrorCode;
    return false;
  }
  g_FrontendCentralRomAsset = (uint32_t)centralRomAsset;
  error = RomAsset_PrepareRecords((RomAssetHeader *)centralRomAsset);
  if (error != 0) {
    *outError = error;
    return false;
  }
  error = g_MemoryApi.alloc(FRONTEND_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),&allocPayload);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* the world object records of the 3D menu room, zeroed */
  g_FrontendWorldObjectRecords = (WorldObjectRecord *)allocPayload;
  zeroCursor = (uint32_t *)allocPayload;
  for (remainingDwords = FRONTEND_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingDwords != 0;
       remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  error = g_MemoryApi.alloc(sizeof(FrontendUiImage),&allocPayload);
  if (error != 0) {
    *outError = error;
    return false;
  }
  frontendUiState = (FrontendRootResourceSlots *)allocPayload;
  worldRuntime = (WorldRuntimeContext *)FRONTEND_UI(frontendUiState,menuRoomModelView);
  templateDwords = (uint32_t *)&g_FrontendRootInitializationTemplate;
  g_FrontendRootNode = (uint32_t)frontendUiState;
  rootDwords = (uint32_t *)frontendUiState;
  for (remainingDwords = sizeof(FrontendUiImage) / 4; remainingDwords != 0; remainingDwords--) {
    *rootDwords = *templateDwords;
    templateDwords++;
    rootDwords++;
  }
  FrontendMenu_BindSharedResources(frontendUiState);
  UiRootStack_Push(&g_UiRootCallbacks_0053DA70,(UiRootNode *)frontendUiState);
  FrontendInit_StartMenuMusic();
  FrontendInit_FillNetworkBackendList(frontendUiState);
  /* the 3D pointer-context control of the menu room is the world runtime at root+0x368 */
  FrontendInit_InstallMenuRoomPointerCallbacks((FrontendModelPointerContext *)worldRuntime);
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)
               &((UiConditionalActionControl *)FRONTEND_UI(frontendUiState,chatMessageHistory))->lineCount);
  WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
  WorldRuntime_AttachObjectArray(FRONTEND_WORLD_OBJECT_RECORD_COUNT,g_FrontendWorldObjectRecords,worldRuntime);
  if (RomRuntime_BuildAllRegistryNodeTrees(worldRuntime)) {
    /* The only failure source of RomRuntime_BuildAllRegistryNodeTrees is WorldObjectArray_AllocateFreeRecord's
       FATAL_ERROR_GENERAL_FAILURE (object array full), passed up unchanged through
       RomRuntime_BuildNodeTreeRecursive. */
    *outError = FATAL_ERROR_GENERAL_FAILURE;
    return false;
  }
  error = FrontendRomTransition_ActivateRecordById(initialRomRecordId,worldRuntime);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* Saved player name into the name field and g_FrontendLocalPlayerNameUtf16 (which is also the fallback),
     saved game name into the game-name field (10 dwords = 0x28 bytes each). */
  savedPlayerName = PersistentSettings_GetRegionOrFallback
                      (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,PERSISTENT_SETTING_PLAYER_NAME);
  FrontendInit_CopyNameDwords
            ((uint32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(frontendUiState,playerNameEdit))->textBuffer,
             savedPlayerName);
  FrontendInit_CopyNameDwords((uint32_t *)(void *)g_FrontendLocalPlayerNameUtf16,savedPlayerName);
  FrontendInit_CopyNameDwords
            ((uint32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(frontendUiState,gameNameEdit))->textBuffer,
             PersistentSettings_GetRegionOrFallback
                       (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,PERSISTENT_SETTING_GAME_NAME));
  settingValue = PersistentSettings_Read(4,PERSISTENT_SETTING_NETWORK_PLAYER_COUNT);
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUiState,maxPlayersSlider))->value = settingValue;
  UiFrame_FlushInputAndResetPendingTicks();
  g_SpinLockAcquire(&g_FrontendStateTickSpinLock);
  WorldMotionSpline_ClearCachedDerivatives();
  g_TimerRegisterPeriodic(FRONTEND_ROM_TRANSITION_TIMER_HZ,FrontendRomTransition_AdvanceElapsedTicks);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    /* the same "ready" report, sent through the network command queue */
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_PLAYER_READY,0,0,0);
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  do {
    UiNode_InvalidateRoot((UiNodeBase *)frontendUiState);
    UiFrame_Update(0);
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    Frontend_StateTick();
  } while ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  UiFrame_FlushInputAndResetPendingTicks();
  return true;
}


/* Address: 0x00547630.
   Network work of the frontend, run under the frontend tick spin lock (skipped while the lock is busy); it is
   also installed as the menu room's render-lock release callback. According to g_FrontendNetworkState
   it sends the periodic packets of the state and hands every received packet to the state's handler, at most
   once per FRONTEND_TIMER_TICKS_PER_NETWORK_TICK timer ticks (the session start states faster).
*/
void Frontend_StateTick(void)

{
  uint32_t frontendRoot; /* passed to the packet handlers */
  uint32_t previousTickCounter;
  bool callResult;
  void *packet;
  void *packetEndpoint;

  callResult = g_SpinLockTryAcquire(&g_FrontendStateTickSpinLock);
  previousTickCounter = g_FrontendNetworkTickCounter;
  frontendRoot = g_FrontendRootNode;
  if (callResult) {
    return;
  }
  switch(g_FrontendNetworkState) {
  case FRONTEND_NETWORK_STATE_IDLE:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    break;
  case FRONTEND_NETWORK_STATE_BROWSING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      /* the packet goes out on 15 of every 16 ticks */
      if ((previousTickCounter & 15) != 0) {
        UiTransfer_SendDiscoveryProbe();
      }
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOSTING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_JOINED:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      if ((previousTickCounter & 15) != 0) {
        FrontendTransfer_SendCapabilityHeartbeat();
      }
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                 frontendRoot);
    }
    callResult = FrontendNetwork_HostTickCommandAndSnapshotTransfer(frontendRoot);
    if (callResult) {
      /* transfer still running: next tick at once */
      g_FrontendTimerCountdownTicks = 1;
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    /* no pacing: works whenever a packet of this session has arrived */
    callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
    if (!callResult) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    do {
      if (!UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) break;
      callResult = FrontendTransfer_HandleGameplayCommandAndRosterPackets
                        ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                         frontendRoot);
    } while (!callResult);
    callResult = FrontendTransfer_ConsumeProcessedFlagForMenuTick();
    if (callResult) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00543B70.
   Called by Frontend_Init for the freshly copied frontend UI: loads gfx\panel\menue.gfx as the texture of the
   menu panels (also kept in g_FrontendMenuTextureSource) and gives the buttons their click sounds, button
   sound voice sets 3 to 6 by control kind. Nothing is bound when the texture cannot be loaded.
*/
void FrontendMenu_BindSharedResources(FrontendRootResourceSlots *frontendUiState)

{
  DirectSoundVoiceSet *buttonVoiceSet;
  DirectSoundVoiceSet *buttonVoiceSet5;
  GraphicsTextureSourceAsset *menuTexture;
  int controlIndex;

  menuTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_menue_gfx_00545b78,NULL);
  if (menuTexture != NULL) {
    g_FrontendMenuTextureSource = menuTexture;
    frontendUiState->menuTextureSource_485C = menuTexture;
    frontendUiState->menuTextureSource_4F30 = menuTexture;
    frontendUiState->menuTextureSource_53D8 = menuTexture;
    frontendUiState->menuTextureSource_5720 = menuTexture;
    frontendUiState->menuTextureSource_2670 = menuTexture;
    frontendUiState->menuTextureSource_2D0C = menuTexture;
    frontendUiState->menuTextureSource_3738 = menuTexture;
    frontendUiState->menuTextureSource_3E64 = menuTexture;
    frontendUiState->menuTextureSource_24F8 = menuTexture;
    frontendUiState->menuTextureSource_1C8C = menuTexture;
    frontendUiState->menuTextureSource_0AE4 = menuTexture;
    frontendUiState->menuTextureSource_05E0 = menuTexture;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0644 = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0764 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_06A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0704 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0B48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0BA8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0C68 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1CF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1D50 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1DB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E10 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_255C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_25BC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_26D4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2790 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_27F0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2850 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2D70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2DD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_379C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_3EC8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_491C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_497C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_49DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_4FF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5050 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5498 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_54F8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5558 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_57E0 = buttonVoiceSet;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2AE0 = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2B40 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2BF4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2C54 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2CB4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2EE0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2F48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2FB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3018 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3080 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_313C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_31A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_320C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3274 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_32DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3344 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_33AC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3414 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_347C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_34E4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_35A0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3608 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3670 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_36D8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3858 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_390C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3974 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_39DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3A44 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3AAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3B14 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3D4C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3DAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3E0C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3F84 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3FE4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_4044 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_28B0 = buttonVoiceSet;
    controlIndex = 7;
    /* the seven faction, player and selection-row controls of the faction setup page (entries 1..7) */
    do {
      ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[controlIndex - 1]))->activationSound =
           buttonVoiceSet;
      ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[controlIndex - 1]))->activationSound =
           buttonVoiceSet;
      ((UiTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[controlIndex - 1]))->activationSound =
           buttonVoiceSet;
      buttonVoiceSet5 = g_UiButtonSoundVoiceSets7[5];
      controlIndex--;
    } while (controlIndex != 0);
    frontendUiState->buttonVoiceSet5_3C98 = g_UiButtonSoundVoiceSets7[5];
    frontendUiState->buttonVoiceSet5_41C0 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_433C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_44B8 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_4634 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_514C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_5210 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_0A8C = buttonVoiceSet5;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_2024 = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_21EC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_23CC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4AD4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4BD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_5654 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4DC4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4EB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_50BC = buttonVoiceSet;
  }
  return;
}


/* Address: 0x005445A0.
   Handler of frontend command FRONTEND_COMMAND_CYCLE_FACTION_COLOUR (0x650), called directly by
   FrontendFactionSetupAction_CycleFactionColour in a local game: advances the colour of faction row rowIndex + 1 (0-based index)
   by one, wrapping after 7 colours (8 when the requesting player has colourCycleFlags bit 0); the row's caption
   (faction colour name) and the level player slot's colour index (the field typed aiClassOrMode) move together.
   Nothing happens for an unknown player id.
*/
void FrontendFactionSetup_CycleFactionColour
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  int *levelCycleCounterField;
  LevelPlayerSlotByteOffset32 playerSlotOffset;
  FrontendLoadedLevelAsset *loadedLevelAsset;
  uint32_t nextSelectionTextId;
  uint32_t playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  UiFramedTextButtonControl *selectionControl;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerRuntimeId == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->colourCycleFlags & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      playerSlotOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowIndex];
      selectionControl = (UiFramedTextButtonControl *)
           (g_FrontendRootNode + g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex]);
      nextSelectionTextId = selectionControl->textResourceId + 1;
      selectionCycleCounterField =
           (int *)&((LevelPlayerSlotRecord *)((uint8_t *)g_FrontendLoadedLevelAsset->playerSlots + playerSlotOffset))->aiClassOrMode;
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + (TEXT_ID_FACTION_NAME_BASE + 1U) <= nextSelectionTextId) {
        nextSelectionTextId = TEXT_ID_FACTION_NAME_BASE + 1;
        levelCycleCounterField = (int *)&((LevelPlayerSlotRecord *)((uint8_t *)loadedLevelAsset->playerSlots + playerSlotOffset))->aiClassOrMode;
        *levelCycleCounterField = *levelCycleCounterField - selectionTextCycleLength;
      }
      selectionControl->textResourceId = nextSelectionTextId;
      return;
    }
    playerRecordCursor++;
    playerRecordsRemaining--;
  } while (playerRecordsRemaining != 0);
  return;
}


/* Address: 0x00544640.
   Handler of frontend command FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE (0x6F0), called directly by
   FrontendFactionSetupAction_ToggleFactionActive in a local game: unless a player has chosen faction rowIndex + 1, toggles
   whether that faction takes part (FACTION_RUNTIME_LIFECYCLE_ACTIVE: computer or nobody) and refreshes the
   faction setup page.
*/
void FrontendFactionSetup_ToggleFactionActive
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  FactionRuntimeLifecycleObservedState *lifecycleState;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (rowIndex + 1 == playerBlock->factionAssignment.factionAssignmentIndex) {
      return;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates + rowIndex + 1;
  *lifecycleState = *lifecycleState ^ FACTION_RUNTIME_LIFECYCLE_ACTIVE;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
  return;
}


/* Address: 0x005446A0.
   Handler of frontend command FRONTEND_COMMAND_CHOOSE_FACTION (0x750), called directly by
   FrontendFactionSetupAction_ChooseFaction in a local game: unless the row is inactive (FRONTEND_CONTROL_INACTIVE), the
   player chooses faction rowIndex + 1. For the local player the row's checkbox becomes the only one checked.
   The player's record (the first one in a local game) gets the faction and the next ready-state generation,
   which orders the choices, then the faction setup page is refreshed.
*/
void FrontendFactionSetup_ChooseFaction
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  FrontendPlayerRuntimeBlockCount remainingBlockCount;
  uint32_t readyStateGeneration;
  UiNodeBase *selectedControl;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeRecord *matchedPlayerBlock;

  selectedControl =
       THANDOR_UI_AT(g_FrontendRootNode,
                     g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex]);
  if ((((UiSelectableControl *)selectedControl)->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      UiSelectableGroup_SelectExclusive(7,selectedControl,
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[0]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[1]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[2]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[3]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[4]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[5]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[6]));
    }
    readyStateGeneration = g_FrontendFactionAssignmentReadyStateGeneration;
    /* network game: search the player's record; when the count runs out first, the first record is used
       (as it is in a local game) */
    matchedPlayerBlock = g_FrontendPlayerRuntimeBlocks;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
      remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
      playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
      do {
        if (playerRuntimeId == playerBlockCursor->playerRuntimeId) {
          matchedPlayerBlock = playerBlockCursor;
          break;
        }
        remainingBlockCount--;
        playerBlockCursor++;
      } while (remainingBlockCount != 0);
    }
    matchedPlayerBlock->factionAssignment.factionAssignmentIndex = rowIndex + 1;
    matchedPlayerBlock->factionAssignment.readyOrWaitState = readyStateGeneration;
    g_FrontendFactionAssignmentReadyStateGeneration++;
    FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
  }
  return;
}


/* Address: 0x00546190.
   Fills the frontend debug overlay texts: every 20th call the frames rendered since the last refresh and the
   draw calls, texture binds and texture reloads per frame (then all four counters restart), and on every call
   the menu camera's position and orientation, the cursor override position and the free arena bytes.
*/
void FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void)

{
  uint32_t freeArenaBytes;
  WideNumberDenominator32 denominator;
  WorldRuntimeContext *world;
  WorldCameraPosition worldVector0;
  WorldCameraOrientation worldVector1;
  
  denominator = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown--;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 20;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    /* per-frame averages with two decimals */
    if (denominator == 0) {
      denominator = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_TextureDeviceReloadCount,g_FrontendDebugOverlayTextSlot03Utf16);
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
    g_TextureBindStateChangeCount = 0;
    g_TextureDeviceReloadCount = 0;
  }
  world = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  worldVector0 = WorldRuntime_GetCameraPosition(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  worldVector1 = WorldRuntime_GetCameraOrientation(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideX,
             g_FrontendDebugOverlayTextSlot10Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideY,
             g_FrontendDebugOverlayTextSlot11Utf16);
  freeArenaBytes = g_MemoryApi.queryFreeBytes();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,freeArenaBytes,
             g_FrontendDebugOverlayTextSlot12Utf16); /* hexadecimal despite radix 10 */
  g_FrontendDebugOverlayTextSlot13Utf16[0] = 0;
  return;
}


/* Address: 0x005474E0.
   Tears down what Frontend_Init built, before a session starts, before the menu is rebuilt and when the game
   quits: removes the frame hooks and frontend timers, saves the root's state snapshot and pops/frees the
   frontend root, releases the ROM registry, world objects, central ROM, textures, palette, menu sounds and
   music, and flushes pending input.
*/
void FrontendRuntime_ShutdownAndReleaseResources(void)

{
  UiRootNode *root;
  int voiceSetsRemaining;
  uint32_t *voiceSetCursor;

  UiRuntime_SetSynchronizationHooks(NULL,NULL);
  g_TimerUnregisterPeriodic(FrontendRuntime_TimerCountdownTick);
  g_TimerUnregisterPeriodic(FrontendRomTransition_AdvanceElapsedTicks);
  root = g_FrontendRootNode;
  g_CursorVisibilityToken--;
  if (g_FrontendRootNode != NULL) {
    FrontendTeardown_SaveStatusTextAndHostAddress(g_FrontendRootNode);
    UiRootStack_Pop(root);
    g_MemoryApi.free(root);
    g_FrontendRootNode = NULL;
  }
  FrontendRomRegistry_ClearAndReleaseNestedResources();
  g_MemoryApi.free(g_FrontendWorldObjectRecords);
  g_FrontendWorldObjectRecords = NULL;
  Resource_Release(g_FrontendCentralRomAsset);
  g_FrontendCentralRomAsset = NULL;
  GraphicsShadingRuntime_ClearRecordTable();
  g_GraphicsTextureSetReleasePackage(g_FrontendCentralTextureSet);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_FrontendCentralPaletteAsset);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_FrontendMenuTextureSource);
  g_FrontendCentralTextureSet = NULL;
  g_FrontendCentralPaletteAsset = NULL;
  g_FrontendMenuTextureSource = NULL;
  /* all 100 menu sound slots (Frontend_Init fills 1..99) */
  voiceSetCursor = &g_FrontendMenuSoundVoiceSetTable100;
  voiceSetsRemaining = 100;
  do {
    if ((DirectSoundVoiceSet *)*voiceSetCursor != NULL) {
      g_SoundReleaseSampleVoiceSet((DirectSoundVoiceSet *)*voiceSetCursor);
    }
    *voiceSetCursor = 0;
    voiceSetCursor++;
    voiceSetsRemaining--;
  } while (voiceSetsRemaining != 0);
  g_SoundStopVoice(g_FrontendMusicActiveBuffer);
  g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
  g_FrontendMusicActiveBuffer = NULL;
  g_FrontendMusicVoiceSet = NULL;
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  return;
}


/* Address: 0x0050AD90.
   Finds the model under the pointer for the model pointer context's press, release, drag and move handlers:
   hit-tests every candidate model node with flag 2 that is a runtime model (and has flag 0x20 unless the
   context allows models without it). The winner is the nearest hit, or, unless the context compares by metric
   only, the hit whose model class has the highest priority, the nearer one on equal priority. Returns the
   node in the high 32 bits and its hit metric in the low 32 bits (NULL and WORLD_POINTER_NO_HIT without a hit).
*/
uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget
                (int pointerY,int pointerX,FrontendModelPointerHitContext *context)

{
  ModelRuntimeNode *modelNode;
  uint32_t bestHitMetric;
  ModelRuntimeNode *bestModelNode;
  uint32_t hitDistanceQ12;
  int candidatePriority;
  int bestPriority;

  bestModelNode = NULL;
  bestHitMetric = WORLD_POINTER_NO_HIT;
  for (modelNode = context->candidateModelListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if ((modelNode->runtimeFlags & MODEL_NODE_FLAG_RENDERED) != 0 && modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL &&
        ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_NON_FACTION_MODELS) != 0 ||
         (modelNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED) != 0)) {
      if (!ModelRuntimeNode_HitTestProjectedBoundsAndChildren
                        (pointerY,pointerX,modelNode,context,&hitDistanceQ12)) continue;
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
        if ((int)bestHitMetric <= (int)hitDistanceQ12) continue;
      }
      else if (bestModelNode != NULL) {
        /* Higher model-class priority wins; equal priority falls back to the smaller hit metric. */
        candidatePriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [modelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId];
        bestPriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [bestModelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId];
        if (candidatePriority < bestPriority) continue;
        if ((candidatePriority == bestPriority) && ((int)bestHitMetric <= (int)hitDistanceQ12))
        continue;
      }
      bestHitMetric = hitDistanceQ12;
      bestModelNode = modelNode;
    }
  }
  return ((uint64_t)(uintptr_t)bestModelNode << 32) | (uint64_t)bestHitMetric;
}

