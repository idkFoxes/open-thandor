/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/scenario_load.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/scenario_load.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(4) uintptr_t g_FrontendLoadedCampaignAsset = 0;

static uint16_t g_CampaignLevelDirectoryUtf16[6] = {'l', 'e', 'v', 'e', 'l', 0}; /* L"level" */

uint16_t g_ScenarioLevelDirectoryUtf16[6] = {'l', 'e', 'v', 'e', 'l', 0}; /* L"level" */

/* <exe dir>\<level>.fld / .pcx. open-thandor: THANDOR_PATH_CAPACITY units; the loaders then combine this absolute
   path with the executable directory once more (FileSystem_LoadWholeFileNearExecutable), see THANDOR_PATH_CAPACITY. */
uint16_t g_LevelResourcePathScratchUtf16[THANDOR_PATH_CAPACITY] = {0};

FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset = 0;

uint16_t g_FrontendScenarioPathScratchUtf16[256] = {0};

/* Implementation ownership: gameplay/session/scenario_load. */

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
  uintptr_t checkedValue;
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
      loadedEntry = FieldGrid_LoadValidated((uint16_t *)fieldGridPath,&loadErrorCode); /* the original: Package_LoadEntry, no size check */
      checkedValue = FatalError_ExitIfFailed
                          (loadedEntry != NULL ? (uintptr_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
      (clientLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
  loadedEntry = FieldGrid_LoadValidated((uint16_t *)fieldGridPath,&loadErrorCode); /* the original: Package_LoadEntry, no size check */
  checkedValue = FatalError_ExitIfFailed
                      (loadedEntry != NULL ? (uintptr_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
  sourceGrid = (FieldGridAsset *)checkedValue;
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = Thandor_PointerToU32(sourceGrid); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  encodedSourceDwords = (uint32_t *)g_PackageScratchBuffer;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    /* transfer image: the decoded size, then the encoded grid; copied into its own buffer */
    sourceImageSizeBytes = (sourceGrid->common).allocationSizeBytes;
    encodeDestination = g_PackageScratchBuffer + 4;
    *(PckDecodedByteCount *)g_PackageScratchBuffer = sourceImageSizeBytes;
    encodeOk = PckCodec_EncodeFieldGrid(PACKAGE_SCRATCH_BUFFER_BYTES - 4,encodeDestination,
                                        sourceImageSizeBytes,sourceGrid,&encodedByteCount,&encodeErrorCode);
    checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
    packetByteCount = (uint32_t)checkedValue + 4;
    allocationError = g_MemoryApi.alloc(packetByteCount,&allocationPayload);
    checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uintptr_t)allocationPayload,allocationError != 0);
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
  uintptr_t frontendRoot;
  CampaignAsset *campaignAsset;
  uint8_t *levelRecordCursor; /* campaign asset base advanced by whole CampaignLevelRecords */
  uint8_t *fieldGridPath;
  uint8_t *encodeCursor;
  uint8_t *transferBundleBytes;
  FrontendLoadedLevelAsset *source;
  uint32_t *transferCopyDestination;
  void *loadedEntry;
  uint32_t loadErrorCode;
  uintptr_t checkedValue;
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
                        (loadedEntry != NULL ? (uintptr_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
    campaignAsset = (CampaignAsset *)checkedValue;
    DebugHook_CampaignLoaded(campaignAsset);
    /* CampaignAsset: the first level becomes the current one; find its record. levelRecordCursor is the
       asset base advanced by whole CampaignLevelRecords, so its levels[0] is the record under the cursor. */
    levelRecordCursor = (uint8_t *)campaignAsset;
    campaignRecordsRemaining = campaignAsset->levelRecordCount;
    g_FrontendLoadedCampaignAsset = (uintptr_t)campaignAsset;
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
                        (loadedEntry != NULL ? (uintptr_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
    g_FrontendLoadedLevelAsset = (FrontendLoadedLevelAsset *)checkedValue;
    fieldGridPath = (uint8_t *)g_FrontendLoadedLevelAsset +
                    (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,(uint16_t *)fieldGridPath);
    WidePath_CombineDirectoryAndLeaf
              (g_LevelResourcePathScratchUtf16,(uint16_t *)fieldGridPath,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    /* the original does not check this load for failure */
    sourceGrid = FieldGrid_LoadValidated((uint16_t *)fieldGridPath,&loadErrorCode); /* the original: Package_LoadEntry, no size check */
    if (sourceGrid == NULL) {
      /* Original quirk: the error code is used as the grid */
      sourceGrid = (FieldGridAsset *)(uintptr_t)loadErrorCode;
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
      encodedLevelBytes = (uint32_t)checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->levelEncodedBytes = encodedLevelBytes;
      encodeCursor = encodeCursor + encodedLevelBytes;
      encodeOk = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes,encodeCursor,
                          campaignAsset->decodedSizeBytes,(uint8_t *)campaignAsset,&encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      encodedCampaignBytes = (uint32_t)checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->campaignEncodedBytes = encodedCampaignBytes;
      encodeOk = PckCodec_EncodeFieldGrid
                         ((PACKAGE_SCRATCH_BUFFER_BYTES - 24 - encodedLevelBytes) - encodedCampaignBytes,encodeCursor + encodedCampaignBytes,
                          (sourceGrid->common).allocationSizeBytes,sourceGrid,&encodedByteCount,&encodeErrorCode);
      checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
      encodedFieldGridBytes = (uint32_t)checkedValue;
      ((ScenarioCampaignBundleHeader *)transferBundleBytes)->fieldGridEncodedBytes = encodedFieldGridBytes;
      /* header, encoded level and campaign (up to encodeCursor + encodedCampaignBytes), then the grid */
      bundleByteCount = (uint32_t)(encodeCursor - transferBundleBytes) + encodedCampaignBytes + encodedFieldGridBytes;
      allocationError = g_MemoryApi.alloc(bundleByteCount,&allocationPayload);
      checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uintptr_t)allocationPayload,allocationError != 0);
      transferCopyDestination = (uint32_t *)checkedValue;
      for (dwordsRemaining = bundleByteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
        *transferCopyDestination = *(uint32_t *)transferBundleBytes;
        transferBundleBytes += 4;
        transferCopyDestination++;
      }
      UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)bundleByteCount,(uint32_t *)checkedValue);
    }
    (source->header).pathState.levelPathOffsetOrLoadedFieldGrid = Thandor_PointerToU32(sourceGrid); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
  uintptr_t frontendRoot;
  int otherPlayersRemaining;
  uint8_t *packedDestination;
  uint32_t *packedSourceDwords;
  uint32_t *outgoingDwordCursor;
  void *loadedEntry;
  uint32_t loadErrorCode;
  uintptr_t checkedValue;
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
                        (loadedEntry != NULL ? (uintptr_t)loadedEntry : loadErrorCode,loadedEntry == NULL);
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
      packetByteCount = (uint32_t)checkedValue + 4;
      allocationError = g_MemoryApi.alloc(packetByteCount,&allocationPayload);
      checkedValue = FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uintptr_t)allocationPayload,allocationError != 0);
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
    levelRecordOffset = (uint32_t)((uint8_t *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                   [selectedRowIndex] -
            (uint8_t *)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
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
            levelAsset = (FrontendLoadedLevelAsset *)Package_LoadEntry(g_FrontendScenarioPathScratchUtf16,NULL);
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
  levelRecordOffset = (uint32_t)((uint8_t *)((UiListControl *)FRONTEND_UI(g_FrontendRootNode,missionsList))->rowSlots
                                 [selectedRowIndex] -
          (uint8_t *)g_ScenarioCatalog) - g_ScenarioCatalog->levelRecordsOffset;
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
