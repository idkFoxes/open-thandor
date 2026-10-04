/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/scenario_transfer.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/scenario_transfer.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

uint32_t g_FrontendScenarioTransferState = 0;

/* Implementation ownership: network/protocol/scenario_transfer. */

/* Allocates byteCount bytes through g_MemoryApi; FatalError_ExitIfFailed does not return on failure. */
static uintptr_t FrontendScenarioTransfer_AllocateOrExit(uint32_t byteCount)
{
  uint32_t allocationError;
  void *allocationPayload;

  allocationError = g_MemoryApi.alloc(byteCount,&allocationPayload);
  return FatalError_ExitIfFailed(allocationError != 0 ? allocationError : (uintptr_t)allocationPayload,
                                 allocationError != 0);
}

/* Frees g_FrontendLoadedLevelAsset and the field grid attached to it: the level's path offset field holds
   the loaded field grid once one was attached (values above 0xFFFF are pointers). */
void FrontendScenarioTransfer_ReleaseLoadedLevelAsset(void)
{
  if ((g_FrontendLoadedLevelAsset != NULL) &&
     (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
  uintptr_t checkedValue;
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
  uintptr_t checkedValue;

  receivedDwords = (uint32_t *)UiTransferMailbox_GetReceivedBuffer(&receivedByteCount);
  if (receivedDwords == NULL) {
    return;
  }
  payloadSizeBytes = *receivedDwords;
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
  uintptr_t checkedValue;

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
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
  uintptr_t checkedValue;

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
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
