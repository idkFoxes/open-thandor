/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/main_loop.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/bytes.h>
#include <thandor/ui/frontend/main_loop.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Views this file needs (genuine reinterpretations, in one place). */
/* Some page initialisers take the frontend UI image (g_FrontendRootNode) through their own function-specific view
   of it (FrontendScenarioSelectionPageView, FrontendTaskAssignmentPageInitView, FrontendCreditsUiStateView: byte
   spans around the nodes they touch, starting at the image start). */
template <class View> static inline View *FrontendMainLoop_RootView()
{
  return reinterpret_cast<View *>(g_FrontendRootNode);
}
/* The loaded level image as the level runtime prefix the session runs on (two descriptions of the same LEV image). */
static inline LevelAssetRuntimePrefix *FrontendMainLoop_LevelAsRuntimePrefix(FrontendLoadedLevelAsset *levelAsset)
{
  return reinterpret_cast<LevelAssetRuntimePrefix *>(levelAsset);
}
/* The campaign's level record cursor one CampaignLevelRecord further on, again viewed as a CampaignAsset so that
   ->levels[0] is the record (see FrontendMainLoop_SelectCampaignSuccessorLevel). */
static inline CampaignAsset *FrontendMainLoop_NextLevelRecordView(CampaignAsset *levelRecordView)
{
  return reinterpret_cast<CampaignAsset *>(reinterpret_cast<CampaignLevelRecord *>(levelRecordView) + 1);
}

/* Module data. */

FrontendPageAction g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;

uint32_t g_FrontendScenarioInitializationCount = 0;

uint32_t g_EndMovieSelectionIndex = 0;

uint32_t g_FrontendPendingPageActionDepth = 0;

/* Frontend_MainLoop: presents UI frames until a page action is pending, then flushes the input and counts the
   action depth. Returns false instead when no action is pending and the UI root stack is empty (the player
   quit the game). */
static bool FrontendMainLoop_PresentFramesUntilPageAction()
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
  uint32_t remainingDecodedBytes;

  /* The original decoded the host's size into the scratch buffer unchecked and read a record per player
     whatever was decoded; bounded here because size and stream come from the host: a transfer shorter than its
     size dword, a size beyond the scratch buffer or a stream the decoder rejects is dropped, and the merge
     stops (logged) where the decoded records end. Valid tables decode and merge as before. */
  if ((receivedByteCount < sizeof(PckDecodedByteCount)) || (*receivedBuffer > PACKAGE_SCRATCH_BUFFER_BYTES) ||
      !PckCodec_DecodeHuffmanRle
            (*receivedBuffer,g_PackageScratchBuffer,receivedByteCount - 4,Thandor_Bytes(receivedBuffer + 1),nullptr,nullptr)) {
    Thandor_Log("FrontendMainLoop_TakeReceivedSnapshots: rejected malformed snapshot table (%u bytes received)",
                receivedByteCount);
    UiTransferMailbox_ClearReceivedState();
    return;
  }
  remainingDecodedBytes = *receivedBuffer;
  /* the decoded table: per player its transfer flags dword, then the payload dwords when complete */
  receivedFlagsCursor = reinterpret_cast<FrontendSnapshotTransferFlags *>(g_PackageScratchBuffer);
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  /* The original looped do-while, a block count of 0 ran it 2^32 times; bounded here because a client's count
     is 0 while the lobby rebuilds the roster (players leaving / a new session start): no blocks, nothing merged. */
  for (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount; remainingPlayerBlocks != 0; remainingPlayerBlocks--) {
    if ((remainingDecodedBytes < sizeof(FrontendSnapshotTransferFlags)) ||
        (Any(*receivedFlagsCursor & FrontendSnapshotTransferFlags::FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) &&
         (remainingDecodedBytes - sizeof(FrontendSnapshotTransferFlags) < FRONTEND_SNAPSHOT_PAYLOAD_BYTES))) {
      Thandor_Log("FrontendMainLoop_TakeReceivedSnapshots: snapshot table ends early (%u bytes decoded)",
                  *receivedBuffer);
      break;
    }
    remainingDecodedBytes = remainingDecodedBytes - sizeof(FrontendSnapshotTransferFlags);
    if (Any(*receivedFlagsCursor & FrontendSnapshotTransferFlags::FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE)) {
      remainingDecodedBytes = remainingDecodedBytes - FRONTEND_SNAPSHOT_PAYLOAD_BYTES;
    }
    receivedTransferFlags = *receivedFlagsCursor;
    playerBlock->snapshotTransferFlags = playerBlock->snapshotTransferFlags | receivedTransferFlags;
    receivedFlagsCursor++;
    if (Any(receivedTransferFlags & FrontendSnapshotTransferFlags::FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE)) {
      /* the payload bytes are copied dword by dword */
      payloadCursor = reinterpret_cast<FrontendSnapshotTransferFlags *>(playerBlock->snapshotPayload);
      for (remainingPayloadDwords = FRONTEND_SNAPSHOT_PAYLOAD_BYTES / sizeof(uint32_t); remainingPayloadDwords != 0;
           remainingPayloadDwords--) {
        *payloadCursor = *receivedFlagsCursor;
        receivedFlagsCursor++;
        payloadCursor++;
      }
    }
    playerBlock++;
  }
  FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SNAPSHOTS_RECEIVED,0,0,0);
  UiTransferMailbox_ClearReceivedState();
}

/* Frontend_MainLoop, scenario catalogue exchange: processes a received scenario asset; unless the local player
   already has the catalogue, marks it, rebuilds the catalogue and either offers it to the clients (host: the
   catalogue is compressed into its own buffer right behind the used bytes, prefixed with the uncompressed size)
   or starts waiting for the host's (client). */
static void FrontendMainLoop_ExchangeScenarioCatalog()
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
  if (Any(localPlayerBlock->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_SCENARIO_CATALOG)) {
    return;
  }
  localRoleStateFlags = &localPlayerBlock->factionAssignment.roleStateFlags;
  *localRoleStateFlags = *localRoleStateFlags | FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
  ScenarioCatalog_Rebuild();
  catalogUsedBytes = g_ScenarioCatalogUsedBytes;
  scenarioCatalog = g_ScenarioCatalog;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_MarkUnavailable();
    g_FrontendScenarioTransferState = ScenarioTransferState::SCENARIO_TRANSFER_CATALOG;
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    encodedCatalog = Thandor_Bytes(g_ScenarioCatalog) + g_ScenarioCatalogUsedBytes + sizeof(uint32_t);
    destinationCapacityBytes = (int)(SCENARIO_CATALOG_CAPACITY - sizeof(uint32_t)) - g_ScenarioCatalogUsedBytes;
    /* the uncompressed size, as the dword in front of the packed catalogue */
    *reinterpret_cast<uint32_t *>(encodedCatalog - 4) = g_ScenarioCatalogUsedBytes;
    encodeOk = PckCodec_EncodeHuffmanRle
                   (destinationCapacityBytes,encodedCatalog,catalogUsedBytes,Thandor_Bytes(scenarioCatalog),
                    &encodedByteCount,&encodeErrorCode);
    checkedValue = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
    UiTransferMailbox_SetOutgoingBuffer(checkedValue + 4,encodedCatalog - 4);
  }
}

/* Frontend_MainLoop, FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE (checked once per frame): in a network session
   first wait until every player's snapshot is published; a client takes the snapshot table the host sends
   meanwhile. Then wait for the scenario catalogue exchange (the host sends its catalogue, a client receives it)
   before the page opens. */
static void FrontendMainLoop_PollScenarioSelectionPage()
{
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendRoleStateFlags *roleStateFlags;
  PckDecodedByteCount *receivedBuffer; /* unpacked size, then the packed snapshot flags */
  uint32_t receivedByteCount;

  /* The original's do-while loops below ran 2^32 times for a block count of 0; bounded here because a client's
     count is 0 while the lobby rebuilds the roster (players leaving / a new session start): keep waiting until
     player blocks exist. Both loops below run with a count >= 1. */
  if (g_FrontendPlayerRuntimeBlockCount == 0) {
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
    do {
      if (!Any(playerBlock->snapshotTransferFlags & FrontendSnapshotTransferFlags::FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY)) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
          receivedBuffer = static_cast<PckDecodedByteCount *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
          if (receivedBuffer != nullptr) {
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
    if (!Any(*roleStateFlags)) {
      FrontendMainLoop_ExchangeScenarioCatalog();
      return;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_SetOutgoingBuffer(0,nullptr);
  }
  FrontendScenarioSelectionPage_InitializeAndApplyMapOption
            (FrontendMainLoop_RootView<FrontendScenarioSelectionPageView>());
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
}

/* Frontend_MainLoop: true when every player block has one of the roleStateFlags bits in stateMask. */
static bool FrontendMainLoop_AllPlayersHaveRoleState(FrontendRoleStateFlags stateMask)
{
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;

  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  /* The original's do-while ran 2^32 times for a block count of 0; bounded here because a client's count is 0
     while the lobby rebuilds the roster (players leaving / a new session start): not ready, keep waiting. */
  if (remainingPlayerBlocks == 0) {
    return false;
  }
  do {
    if (!Any(playerBlock->factionAssignment.roleStateFlags & stateMask)) {
      return false;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  return true;
}

/* Frontend_MainLoop, task assignment and mission briefing pages: the host releases its outgoing transfer before
   the page opens. */
static void FrontendMainLoop_ReleaseHostTransfer()
{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
    UiTransferMailbox_SetOutgoingBuffer(0,nullptr);
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
         (FrontendMainLoop_LevelAsRuntimePrefix(g_FrontendLoadedLevelAsset),loadExistingSession,
          g_FrontendScenarioPathScratchUtf16,&sessionError)) {
    FatalError_ExitIfFailed(sessionError,true);
  }
  PersistentSettings_Flush();
  UiFrame_FlushInputAndResetPendingTicks();
  g_FrontendScenarioInitializationCount = 0;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  /* The original looped do-while, a block count of 0 ran it 2^32 times; bounded here because the count can drop
     to 0 while the session runs (players leaving): no blocks, nothing to clear. */
  for (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount; remainingPlayerBlocks != 0; remainingPlayerBlocks--) {
    playerBlock->factionAssignment.roleStateFlags = FrontendRoleStateFlags{};
    playerBlock++;
  }
}

/* Frontend_MainLoop, after a session in a campaign: follows the successor of the campaign's current level chosen
   by the end movie selection and writes its level path (level\<name>.lev) to g_FrontendScenarioPathScratchUtf16.
   Returns true when a successor level was selected. A successor id missing from the campaign finishes it (the
   campaign asset is released); a negative successor id or a current level without a record leaves it loaded.
   The record cursors start at the asset base and advance by one CampaignLevelRecord, so level record i is
   levels[0] of the cursor viewed as a CampaignAsset. */
static bool FrontendMainLoop_SelectCampaignSuccessorLevel()
{
  CampaignAsset *campaign;
  CampaignAsset *levelRecordView;
  int remainingLevelRecords;
  int successorLevelId;

  campaign = g_FrontendLoadedCampaignAsset;
  if (campaign == nullptr) {
    return false;
  }
  remainingLevelRecords = campaign->levelRecordCount;
  levelRecordView = campaign;
  while (campaign->currentLevelId != levelRecordView->levels[0].levelId) {
    levelRecordView = FrontendMainLoop_NextLevelRecordView(levelRecordView);
    remainingLevelRecords--;
    if (remainingLevelRecords == 0) {
      return false;
    }
  }
  /* The original indexed successorLevelIds with the level's end selection unchecked; bounded here because the
     selection comes from the level's end trigger (a byte): a selection beyond the table is logged and treated
     like a negative successor id. */
  if (g_EndMovieSelectionIndex >= sizeof(levelRecordView->levels[0].successorLevelIds) / sizeof(int32_t)) {
    Thandor_Log("FrontendMainLoop_SelectCampaignSuccessorLevel: end selection %u out of range",
                g_EndMovieSelectionIndex);
    return false;
  }
  successorLevelId = levelRecordView->levels[0].successorLevelIds[(int)g_EndMovieSelectionIndex];
  if (successorLevelId < 0) {
    return false;
  }
  remainingLevelRecords = campaign->levelRecordCount;
  campaign->currentLevelId = successorLevelId;
  levelRecordView = campaign;
  while (successorLevelId != levelRecordView->levels[0].levelId) {
    levelRecordView = FrontendMainLoop_NextLevelRecordView(levelRecordView);
    remainingLevelRecords--;
    if (remainingLevelRecords == 0) {
      /* Successor level missing: the campaign is finished. */
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = nullptr;
      g_FrontendScenarioInitializationCount = 0;
      return false;
    }
  }
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,levelRecordView->levels[0].levelFileName,
             g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
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

  /* the transfer is built in the package scratch buffer: the header, then the packed images */
  bundleHeader = reinterpret_cast<ScenarioLevelBundleHeader *>(g_PackageScratchBuffer);
  fieldGridAllocationSize = fieldGrid->common.allocationSizeBytes;
  bundleHeader->levelDecodedBytes = loadedLevelAsset->header.common.allocationSizeBytes;
  bundleHeader->fieldGridDecodedBytes = fieldGridAllocationSize;
  encodedImages = Thandor_Bytes(bundleHeader + 1);
  /* capacity: the scratch buffer minus 24 bytes, the size of the campaign bundle header
     (ScenarioCampaignBundleHeader), although this header has 16 */
  encodeOk = PckCodec_EncodeHuffmanRle
                 (PACKAGE_SCRATCH_BUFFER_BYTES - 24,encodedImages,loadedLevelAsset->header.common.allocationSizeBytes,
                  Thandor_Bytes(loadedLevelAsset),&encodedByteCount,&encodeErrorCode);
  levelEncodedBytes = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
  bundleHeader->levelEncodedBytes = levelEncodedBytes;
  /* The original subtracts the encoded level size from the capacity unchecked; bounded here because a size
     beyond the buffer would wrap it to a huge capacity. Fails like a failed encode. */
  if ((uint64_t)levelEncodedBytes > (uint64_t)(PACKAGE_SCRATCH_BUFFER_BYTES - 24)) {
    Thandor_Log("FrontendMainLoop_OfferLevelToClients: encoded level %u bytes exceed the transfer buffer",
                levelEncodedBytes);
    encodeOk = false;
    encodeErrorCode = FATAL_ERROR_GENERAL_FAILURE;
  }
  else {
    encodeOk = PckCodec_EncodeFieldGrid
                   (PACKAGE_SCRATCH_BUFFER_BYTES - 24 - levelEncodedBytes,encodedImages + levelEncodedBytes,
                    fieldGrid->common.allocationSizeBytes,fieldGrid,&encodedByteCount,&encodeErrorCode);
  }
  fieldGridEncodedBytes = FatalError_ExitIfFailed(encodeOk ? encodedByteCount : encodeErrorCode,!encodeOk);
  bundleHeader->fieldGridEncodedBytes = fieldGridEncodedBytes;
  transferByteCount =
       (uint32_t)Thandor_ByteDistance(encodedImages,bundleHeader) + levelEncodedBytes + fieldGridEncodedBytes;
  allocError = g_MemoryApi.alloc(transferByteCount,&allocPayload);
  transferAllocation =
       reinterpret_cast<uint32_t *>(FatalError_ExitIfFailed(allocError != 0 ? allocError : reinterpret_cast<uintptr_t>(allocPayload),allocError != 0));
  transferDwordCursor = transferAllocation;
  /* copied dword by dword into the allocation */
  transferSourceDwords = reinterpret_cast<uint32_t *>(bundleHeader);
  for (remainingDwords = transferByteCount >> 2; remainingDwords != 0; remainingDwords--) {
    *transferDwordCursor = *transferSourceDwords;
    transferSourceDwords++;
    transferDwordCursor++;
  }
  UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)transferByteCount,transferAllocation);
}

/* Code units WidePath_SetExtensionCode writes from the path's terminator on when the path has no extension
   ('.', three characters, terminator); a path with an extension is rewritten within its old length. */
static constexpr uint32_t FRONTEND_LEVEL_PATH_EXTENSION_UNITS = 5;

/* True when the level's own path (header pathState.levelPathOffsetOrLoadedFieldGrid, still an offset here)
   starts inside the loadedByteCount bytes of the level and is terminated at least
   FRONTEND_LEVEL_PATH_EXTENSION_UNITS code units before their end, so the ".fld" rewrite and the path reads stay
   inside the level. Logs the rejection.
   The original trusts the offset (up to 0xFFFF) and the terminator; bounded here because a malformed level made
   the extension write (and the field grid path reads) run past the level's allocation. The 55 stock levels
   keep 298 or more code units after their path's terminator. */
static bool FrontendMainLoop_LevelPathFits(const FrontendLoadedLevelAsset *level,uint32_t loadedByteCount)
{
  const uint32_t pathOffset = level->header.pathState.levelPathOffsetOrLoadedFieldGrid;
  const uint32_t pathUnitsAvailable = (pathOffset < loadedByteCount) ?
                                      (loadedByteCount - pathOffset) / sizeof(uint16_t) : 0;
  const uint16_t *path = Thandor_At<uint16_t>(level,(pathUnitsAvailable != 0) ? pathOffset : 0);
  uint32_t terminatorIndex = 0;

  while ((terminatorIndex < pathUnitsAvailable) && (path[terminatorIndex] != 0)) {
    terminatorIndex++;
  }
  if (pathUnitsAvailable - terminatorIndex < FRONTEND_LEVEL_PATH_EXTENSION_UNITS) {
    Thandor_Log("FrontendMainLoop_LoadSelectedLevel: level \"%ls\" rejected, its path at 0x%X is not terminated %u "
                "units before the end (0x%X bytes)",reinterpret_cast<wchar_t *>(g_FrontendScenarioPathScratchUtf16),
                pathOffset,FRONTEND_LEVEL_PATH_EXTENSION_UNITS,loadedByteCount);
    return false;
  }
  return true;
}

/* Frontend_MainLoop, host and local game: replaces the loaded level by the one at
   g_FrontendScenarioPathScratchUtf16, loads its field grid (the level's path with the extension "fld", under the
   executable directory), offers both to the clients when hosting and assigns the factions. */
static void FrontendMainLoop_LoadSelectedLevel()
{
  void *loadedPackageEntry;
  uint32_t packageLoadErrorCode;
  uint32_t loadedByteCount;
  uintptr_t checkedValue;
  uint16_t *fieldGridPath;
  FieldGridAsset *fieldGrid;
  FrontendLoadedLevelAsset *loadedLevelAsset;

  /* levelPathOffsetOrLoadedFieldGrid holds the field grid pointer once loaded, an offset (<= 0xFFFF) into the
     level before */
  if ((g_FrontendLoadedLevelAsset != nullptr) &&
     (0xffff < g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release(Thandor_U32ToPointer<void>(g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid)); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = nullptr;
  /* Package_LoadEntryWithSize is the core of Package_LoadEntry (same allocation); the original used
     Package_LoadEntry and trusted the level header's size */
  loadedPackageEntry =
       Package_LoadEntryWithSize(g_FrontendScenarioPathScratchUtf16,&loadedByteCount,&packageLoadErrorCode);
  if (loadedPackageEntry == nullptr) {
    Thandor_Log("FrontendMainLoop_LoadSelectedLevel: loading \"%ls\" failed (error 0x%08X)",
                reinterpret_cast<wchar_t *>(g_FrontendScenarioPathScratchUtf16),packageLoadErrorCode);
  }
  /* The original took the level header's allocationSizeBytes as the image size (the host encodes that many
     bytes for the clients, NewLevel_ValidateImage bounds the level by it); bounded here because a level shorter
     than its header or than the size its header claims was read past its allocation: rejected like a level that
     fails to load. */
  else if ((loadedByteCount < sizeof(FrontendLoadedLevelAsset)) ||
           (static_cast<FrontendLoadedLevelAsset *>(loadedPackageEntry)->header.common.allocationSizeBytes >
            loadedByteCount)) {
    Thandor_Log("FrontendMainLoop_LoadSelectedLevel: level \"%ls\" rejected, header size 0x%X, decoded 0x%X bytes",
                reinterpret_cast<wchar_t *>(g_FrontendScenarioPathScratchUtf16),
                (loadedByteCount < sizeof(FrontendLoadedLevelAsset)) ? 0U :
                     static_cast<FrontendLoadedLevelAsset *>(loadedPackageEntry)->header.common.allocationSizeBytes,
                loadedByteCount);
    Resource_Release(loadedPackageEntry);
    loadedPackageEntry = nullptr;
    packageLoadErrorCode = FATAL_ERROR_LEVEL_ASSET_INVALID;
  }
  else if (!FrontendMainLoop_LevelPathFits(static_cast<FrontendLoadedLevelAsset *>(loadedPackageEntry),
                                           loadedByteCount)) {
    Resource_Release(loadedPackageEntry);
    loadedPackageEntry = nullptr;
    packageLoadErrorCode = FATAL_ERROR_LEVEL_ASSET_INVALID;
  }
  checkedValue = FatalError_ExitIfFailed
                      (loadedPackageEntry != nullptr ? reinterpret_cast<uintptr_t>(loadedPackageEntry) : packageLoadErrorCode,
                       loadedPackageEntry == nullptr);
  g_FrontendLoadedLevelAsset = reinterpret_cast<FrontendLoadedLevelAsset *>(checkedValue);
  fieldGridPath = Thandor_At<uint16_t>(g_FrontendLoadedLevelAsset,
                                       g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_FLD,fieldGridPath);
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,fieldGridPath,g_ExecutableDirectoryUtf16);
  fieldGrid = FieldGrid_LoadValidated(fieldGridPath,&packageLoadErrorCode); /* the original: Package_LoadEntry, no size check */
  if (fieldGrid == nullptr) {
    /* The original did not check the field grid load and used the error code as the grid (a host then read
       through it); handled here like a failed level load because the grid is required: a fatal error with the
       load's error code. */
    FatalError_ExitIfFailed(packageLoadErrorCode,true); /* does not return */
  }
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
    FrontendMainLoop_OfferLevelToClients(loadedLevelAsset,fieldGrid);
  }
  loadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid = Thandor_PointerToU32(fieldGrid); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
    g_FrontendScenarioTransferState = ScenarioTransferState::SCENARIO_TRANSFER_LEVEL_BUNDLE;
  }
  return true;
}

/* Frontend_MainLoop: rebuilds the menu at nextRomRecordId with nextPageAction pending (set even when the build
   fails). Returns false with Frontend_Init's error in *outError when building the menu fails. */
static bool FrontendMainLoop_RebuildMenu(RomRecordId nextRomRecordId,FrontendPageAction nextPageAction,uint32_t *outError)
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
  if (!FrontendMainLoop_SelectCampaignSuccessorLevel() && (g_FrontendScenarioPathScratchUtf16[0] == 0)) {
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
    FrontendNetworkSetupPage_InitializeBackendMode(g_FrontendRootNode);
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) {
    FrontendGameplaySettingsPage_InitializeFromPersistentSettings(&g_FrontendRootNode->frontendRoot.root);
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
      FrontendTaskAssignmentPage_Initialize(FrontendMainLoop_RootView<FrontendTaskAssignmentPageInitView>());
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    }
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) {
    FrontendScenarioTransfer_ProcessReceivedAsset();
    if (FrontendMainLoop_AllPlayersHaveRoleState(FRONTEND_PLAYER_STATE_LEVEL_READY_MASK)) {
      FrontendMainLoop_ReleaseHostTransfer();
      FrontendMissionBriefingPage_Initialize(&g_FrontendRootNode->frontendRoot.root);
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    }
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_CREDITS) {
    CreditsScreen_Open(FrontendMainLoop_RootView<FrontendCreditsUiStateView>());
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
    return true;
  }
  if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE) {
    FrontendSession_ShowQuitConfirmPage(g_FrontendRootNode);
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

/* The frontend (main menu) state machine, run from Game_Run until the player quits. It builds the menu at
   frontendEntryRecordId, jumps straight into the host/client/map flow when -HOST, -CLIENT= or -KARTE= is on the
   command line, then presents UI frames until g_FrontendPendingPageAction is set (a FRONTEND_PAGE_ACTION_* value) and
   performs it: open a menu page, wait for the network peers (scenario catalogue, task assignment, level
   transfer), or tear the frontend down to run a session. After a session a campaign continues with the
   successor level chosen by the end movie (menu rebuilt at FRONTEND_ROM_RECORD_MISSION_BRIEFING), otherwise
   the menu is rebuilt at the scenario selection or the entry record. Returns true when the UI root stack
   empties (quit); false with Frontend_Init's error in *outError when building the menu fails.
*/
bool Frontend_MainLoop(RomRecordId frontendEntryRecordId,uint32_t *outError)

{
  uint32_t initError;

  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  if (Frontend_Init(frontendEntryRecordId,&initError)) {
    /* -HOST and -CLIENT= activate entry 3 of the entry menu's action table, -KARTE= (map) entry 0, without the
       click sound, and let the started camera transition end at once. */
    if ((g_CommandLineFindOption(5,g_SpielerSpielNetzwerkHostKeywordsAscii + 26) != nullptr) || /* "HOST" */
        (g_CommandLineFindOption(8,g_NameClientKarteKeywordsAscii + 6) != nullptr)) {       /* "CLIENT=" */
      FrontendRomActionTable_ExecuteRecord(0,0,1,3);
      FrontendRomTransition_RequestStop();
    }
    else if (g_CommandLineFindOption(7,g_NameClientKarteKeywordsAscii + 14) != nullptr) { /* "KARTE=" */
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
