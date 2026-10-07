/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/scenario_transfer.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/scenario_transfer.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

ScenarioTransferState g_FrontendScenarioTransferState = ScenarioTransferState::SCENARIO_TRANSFER_NONE;

/* Allocates byteCount bytes through g_MemoryApi; FatalError_ExitIfFailed does not return on failure. */
static uintptr_t FrontendScenarioTransfer_AllocateOrExit(uint32_t byteCount)
{
  uint32_t allocationError;
  void *allocationPayload;

  allocationError = g_MemoryApi.alloc(byteCount,&allocationPayload);
  return FatalError_ExitIfFailed(allocationError != 0 ? allocationError : reinterpret_cast<uintptr_t>(allocationPayload),
                                 allocationError != 0);
}

/* Decodes a received field grid (PckCodec_DecodeFieldGrid) into its decodedBytes buffer and validates it
   (FieldGrid_ValidateLoadedImage). The original ignores the decoder's result and trusts the grid; bounded here
   because it comes from the network host: the callers abort the transfer when this returns false. */
static bool FrontendScenarioTransfer_DecodeFieldGrid
          (uint32_t decodedBytes,FieldGridAsset *destinationGrid,uint32_t encodedBytes,uint8_t *encodedGrid)
{
  return PckCodec_DecodeFieldGrid(decodedBytes,destinationGrid,encodedBytes,encodedGrid,nullptr,nullptr) &&
         FieldGrid_ValidateLoadedImage(destinationGrid,decodedBytes);
}

/* Frees g_FrontendLoadedLevelAsset and the field grid attached to it: the level's path offset field holds
   the loaded field grid once one was attached (values above 0xFFFF are pointers). */
void FrontendScenarioTransfer_ReleaseLoadedLevelAsset()
{
  if ((g_FrontendLoadedLevelAsset != nullptr) &&
     (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)) {
    Resource_Release(Thandor_U32ToPointer<void>((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid)); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  }
  Resource_Release(g_FrontendLoadedLevelAsset);
  g_FrontendLoadedLevelAsset = nullptr;
}

/* Frees the received mailbox buffer and ends the transfer. */
static void FrontendScenarioTransfer_FinishReceive(uint32_t *receivedDwords)
{
  g_MemoryApi.free(receivedDwords);
  UiTransferMailbox_ClearReceivedState();
  g_FrontendScenarioTransferState = ScenarioTransferState::SCENARIO_TRANSFER_NONE;
}

/* The original unpacked every received asset unchecked; bounded here because sizes, offsets and packed streams
   come from the network host: a malformed transfer (sizes beyond the received bytes, a stream the decoder
   rejects, records or a path outside the decoded asset) is logged, the received buffer is freed and the
   transfer ends without reporting the asset as received. Valid transfers pass every check. */
static void FrontendScenarioTransfer_AbortReceive(uint32_t *receivedDwords,const char *assetName)
{
  Thandor_Log("FrontendScenarioTransfer: rejected malformed %s from the host",assetName);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
}

/* Tells whether the packed streams of a bundle (headerBytes, then the streams of the given sizes) fit into the
   receivedByteCount bytes of the transfer. */
static bool FrontendScenarioTransfer_StreamsFit
          (uint32_t receivedByteCount,uint32_t headerBytes,uint32_t firstStreamBytes,uint32_t secondStreamBytes,
           uint32_t thirdStreamBytes)
{
  return (uint64_t)headerBytes + firstStreamBytes + secondStreamBytes + thirdStreamBytes <= receivedByteCount;
}

/* Tells whether a decoded level of levelBytes bytes holds its header and a terminated path at its path offset
   (an offset, at most 0xFFFF, before a field grid is attached) whose extension WidePath_SetExtensionCode can
   replace inside the asset: it writes up to four code units from behind the last '.' of the final component,
   or appends '.' and four code units at the terminator when there is none. */
static bool FrontendScenarioTransfer_LevelPathFits(const FrontendLoadedLevelAsset *levelAsset,uint32_t levelBytes)
{
  uint32_t pathOffset;
  uint32_t pathCodeUnits;
  const uint16_t *path;
  uint32_t unitIndex;
  uint32_t lastWrittenUnit;
  bool hasExtension;

  if (levelBytes < sizeof(FrontendLoadedLevelAsset)) {
    return false;
  }
  pathOffset = (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
  if ((0xffff < pathOffset) || (levelBytes <= pathOffset)) {
    return false;
  }
  path = Asset_RecordAt<uint16_t>(levelAsset,pathOffset);
  pathCodeUnits = (levelBytes - pathOffset) / sizeof(uint16_t);
  hasExtension = false;
  lastWrittenUnit = 0;
  for (unitIndex = 0; unitIndex < pathCodeUnits; unitIndex++) {
    if (path[unitIndex] == 0) {
      if (!hasExtension) {
        lastWrittenUnit = unitIndex + 4; /* '.', three characters, terminator */
      }
      return lastWrittenUnit < pathCodeUnits;
    }
    if (path[unitIndex] == '\\') {
      hasExtension = false;
    }
    else if (path[unitIndex] == '.') {
      hasExtension = true;
      lastWrittenUnit = unitIndex + 4; /* three characters and the terminator behind the '.' */
    }
  }
  return false;
}

/* Tells whether a decoded catalog of catalogBytes bytes holds its header and the level, campaign and save
   records its header counts (each section is packed 0x100 bytes per record at its offset). */
static bool FrontendScenarioTransfer_CatalogFits(const ScenarioCatalogHeader *catalog,uint32_t catalogBytes)
{
  return (catalogBytes >= SCENARIO_CATALOG_HEADER_SIZE) &&
         ((uint64_t)catalog->levelRecordsOffset +
              (uint64_t)catalog->levelRecordCount * SCENARIO_CATALOG_RECORD_SIZE <= catalogBytes) &&
         ((uint64_t)catalog->campaignRecordsOffset +
              (uint64_t)catalog->campaignRecordCount * SCENARIO_CATALOG_RECORD_SIZE <= catalogBytes) &&
         ((uint64_t)catalog->saveRecordsOffset +
              (uint64_t)catalog->saveRecordCount * SCENARIO_CATALOG_RECORD_SIZE <= catalogBytes);
}

/* Tells whether a decoded campaign of campaignBytes bytes holds its header, at least one level record, all
   levelRecordCount records, and a record of its first level (the start of the campaign looks it up). */
static bool FrontendScenarioTransfer_CampaignFits(const CampaignAsset *campaignAsset,uint32_t campaignBytes)
{
  int32_t recordIndex;

  if ((campaignBytes < offsetof(CampaignAsset,levels)) || (campaignAsset->levelRecordCount < 1) ||
      ((uint64_t)campaignAsset->levelRecordCount * sizeof(CampaignLevelRecord) >
       campaignBytes - offsetof(CampaignAsset,levels))) {
    return false;
  }
  for (recordIndex = 0; recordIndex < campaignAsset->levelRecordCount; recordIndex++) {
    if (campaignAsset->levels[recordIndex].levelId == campaignAsset->firstLevelId) {
      return true;
    }
  }
  return false;
}

/* The level's relative path (asset base + path offset) becomes <exe dir>\<level>.fld in
   g_LevelResourcePathScratchUtf16, the path the game uses for the field grid. */
static void FrontendScenarioTransfer_SetFieldGridPathOfLevel(FrontendLoadedLevelAsset *levelAsset)
{
  uint16_t *levelPath;

  levelPath = Asset_RecordAt<uint16_t>(levelAsset,(levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid);
  WidePath_SetExtensionCode(ASSET_MAGIC_FLD,levelPath);
  WidePath_CombineDirectoryAndLeaf
            (g_LevelResourcePathScratchUtf16,levelPath,g_ExecutableDirectoryUtf16);
}

/* SCENARIO_TRANSFER_CATALOG, packet: unpacked size, packed catalog. Replaces g_ScenarioCatalog and reports
   which received level records (up to 96) the previous catalog did not contain. */
static void FrontendScenarioTransfer_ProcessReceivedCatalog()
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
  uint32_t previousCatalogUsedBytes;
  ScenarioCatalogRecordCount oldRecordCount;
  ScenarioCatalogRecordCount newRecordsRemaining;
  uint32_t *oldLevelRecords;
  uint32_t *receivedRecord;
  uint32_t maskBit;
  int maskSlot;

  receivedDwords = static_cast<uint32_t *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
  if (receivedDwords == nullptr) {
    return;
  }
  if ((receivedByteCount < sizeof(uint32_t)) || (*receivedDwords < SCENARIO_CATALOG_HEADER_SIZE)) {
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"scenario catalog");
    return;
  }
  payloadSizeBytes = *receivedDwords;
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  changedLevelMask[1] = 0;
  changedLevelMask[2] = 0;
  changedLevelMask[3] = 0;
  previousCatalog = g_ScenarioCatalog;
  previousCatalogUsedBytes = g_ScenarioCatalogUsedBytes;
  g_ScenarioCatalog = reinterpret_cast<ScenarioCatalogHeader *>(checkedValue); /* the block address as an integer */
  g_ScenarioCatalogUsedBytes = payloadSizeBytes;
  if (!PckCodec_DecodeHuffmanRle(payloadSizeBytes,reinterpret_cast<uint8_t *>(checkedValue),receivedByteCount - 4,
                                 reinterpret_cast<uint8_t *>(receivedDwords + 1),nullptr,nullptr) ||
      !FrontendScenarioTransfer_CatalogFits(g_ScenarioCatalog,payloadSizeBytes)) {
    /* keep the previous catalog */
    g_MemoryApi.free(g_ScenarioCatalog);
    g_ScenarioCatalog = previousCatalog;
    g_ScenarioCatalogUsedBytes = previousCatalogUsedBytes;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"scenario catalog");
    return;
  }
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  /* Mark every received level record (up to 96) that the previous catalog did not contain. */
  newRecordsRemaining = g_ScenarioCatalog->levelRecordCount;
  if (previousCatalog != nullptr) {
    oldRecordCount = previousCatalog->levelRecordCount;
    receivedRecord = Asset_RecordAt<uint32_t>(g_ScenarioCatalog,g_ScenarioCatalog->levelRecordsOffset);
    oldLevelRecords = Asset_RecordAt<uint32_t>(previousCatalog,previousCatalog->levelRecordsOffset);
    if ((newRecordsRemaining != 0) && (oldRecordCount != 0)) {
      maskBit = 1;
      maskSlot = 3;
      for (; newRecordsRemaining != 0; newRecordsRemaining--) {
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
      }
    }
  }
  g_MemoryApi.free(previousCatalog);
  FrontendCommandQueue_EnqueueLocalPlayerCommand
            (FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED,changedLevelMask[1],changedLevelMask[2],changedLevelMask[3]);
}

/* SCENARIO_TRANSFER_LEVEL, packet: unpacked size, packed level asset; replaces g_FrontendLoadedLevelAsset. */
static void FrontendScenarioTransfer_ProcessReceivedLevel()
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  uint32_t payloadSizeBytes;

  receivedDwords = static_cast<uint32_t *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
  if (receivedDwords == nullptr) {
    return;
  }
  if ((receivedByteCount < sizeof(uint32_t)) || (*receivedDwords < sizeof(FrontendLoadedLevelAsset))) {
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"level");
    return;
  }
  payloadSizeBytes = *receivedDwords;
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  g_FrontendLoadedLevelAsset = reinterpret_cast<FrontendLoadedLevelAsset *> /* the block address as an integer */
                             (FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes));
  if (!PckCodec_DecodeHuffmanRle
            (payloadSizeBytes,reinterpret_cast<uint8_t *>(g_FrontendLoadedLevelAsset),receivedByteCount - 4,
             reinterpret_cast<uint8_t *>(receivedDwords + 1),
             nullptr,nullptr) ||
      !FrontendScenarioTransfer_LevelPathFits(g_FrontendLoadedLevelAsset,payloadSizeBytes)) {
    /* the path offset field may hold anything: free the level alone */
    g_MemoryApi.free(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = nullptr;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"level");
    return;
  }
  FrontendCommand_Issue<FrontendPlayerRuntime_MarkTaskAssignmentReadyById>(0,0,0);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
}

/* SCENARIO_TRANSFER_FIELD_GRID, packet: unpacked size, packed field grid; it is attached to the already
   loaded level. */
static void FrontendScenarioTransfer_ProcessReceivedFieldGrid()
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  uint32_t payloadSizeBytes;
  uintptr_t checkedValue;
  uint32_t previousPathState;

  receivedDwords = static_cast<uint32_t *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
  if (receivedDwords == nullptr) {
    return;
  }
  if ((receivedByteCount < sizeof(uint32_t)) || (*receivedDwords == 0) || (g_FrontendLoadedLevelAsset == nullptr)) {
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"field grid");
    return;
  }
  payloadSizeBytes = *receivedDwords;
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(payloadSizeBytes);
  previousPathState = (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid;
  (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  if (!FrontendScenarioTransfer_DecodeFieldGrid
            (payloadSizeBytes,reinterpret_cast<FieldGridAsset *>(checkedValue),receivedByteCount - 4,
             reinterpret_cast<uint8_t *>(receivedDwords + 1))) {
    /* the level keeps its path offset and no grid */
    g_MemoryApi.free(reinterpret_cast<void *>(checkedValue));
    (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = previousPathState;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"field grid");
    return;
  }
  FrontendCommand_Issue<FrontendPlayerRuntime_MarkLevelReceivedById>(0,0,0);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
}

/* SCENARIO_TRANSFER_CAMPAIGN_BUNDLE, packet: ScenarioCampaignBundleHeader (unpacked sizes of level, campaign
   and field grid, then their packed sizes), then the three packed streams. Afterwards the campaign starts at
   its first level: its record gives level\<name>.lev in g_FrontendScenarioPathScratchUtf16. */
static void FrontendScenarioTransfer_ProcessReceivedCampaignBundle()
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
  bool decodeOk;

  receivedDwords = static_cast<uint32_t *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
  if (receivedDwords == nullptr) {
    return;
  }
  bundle = reinterpret_cast<ScenarioCampaignBundleHeader *>(receivedDwords); /* the buffer starts with the bundle header */
  if ((receivedByteCount < sizeof(ScenarioCampaignBundleHeader)) ||
      !FrontendScenarioTransfer_StreamsFit(receivedByteCount,sizeof(ScenarioCampaignBundleHeader),
                                           bundle->levelEncodedBytes,bundle->campaignEncodedBytes,
                                           bundle->fieldGridEncodedBytes) ||
      (bundle->levelDecodedBytes < sizeof(FrontendLoadedLevelAsset)) || (bundle->campaignDecodedBytes == 0) ||
      (bundle->fieldGridDecodedBytes == 0)) {
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"campaign bundle");
    return;
  }
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  g_FrontendLoadedLevelAsset =
       reinterpret_cast<FrontendLoadedLevelAsset *>(FrontendScenarioTransfer_AllocateOrExit(bundle->levelDecodedBytes));
  decodeOk = PckCodec_DecodeHuffmanRle
                 (bundle->levelDecodedBytes,reinterpret_cast<uint8_t *>(g_FrontendLoadedLevelAsset),bundle->levelEncodedBytes,
                  Asset_RecordAfter<uint8_t>(bundle),nullptr,nullptr) &&
             FrontendScenarioTransfer_LevelPathFits(g_FrontendLoadedLevelAsset,bundle->levelDecodedBytes);
  campaignStream = Asset_RecordAfter<uint8_t>(bundle) + bundle->levelEncodedBytes;
  g_FrontendLoadedCampaignAsset =
       reinterpret_cast<CampaignAsset *>(FrontendScenarioTransfer_AllocateOrExit(bundle->campaignDecodedBytes));
  decodeOk = decodeOk &&
             PckCodec_DecodeHuffmanRle(bundle->campaignDecodedBytes,reinterpret_cast<uint8_t *>(g_FrontendLoadedCampaignAsset),
                                       bundle->campaignEncodedBytes,campaignStream,nullptr,nullptr) &&
             FrontendScenarioTransfer_CampaignFits(g_FrontendLoadedCampaignAsset,
                                                   bundle->campaignDecodedBytes);
  if (!decodeOk) {
    /* the level's path offset field may hold anything: free the level alone */
    g_MemoryApi.free(g_FrontendLoadedLevelAsset);
    g_FrontendLoadedLevelAsset = nullptr;
    g_MemoryApi.free(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = nullptr;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"campaign bundle");
    return;
  }
  levelAsset = g_FrontendLoadedLevelAsset;
  fieldGridStream = campaignStream + bundle->campaignEncodedBytes;
  /* afterwards the level's path offset field holds the received field grid */
  FrontendScenarioTransfer_SetFieldGridPathOfLevel(g_FrontendLoadedLevelAsset);
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(bundle->fieldGridDecodedBytes);
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  if (!FrontendScenarioTransfer_DecodeFieldGrid(bundle->fieldGridDecodedBytes,reinterpret_cast<FieldGridAsset *>(checkedValue),
                                bundle->fieldGridEncodedBytes,fieldGridStream)) {
    FrontendScenarioTransfer_ReleaseLoadedLevelAsset(); /* the level and its grid */
    g_MemoryApi.free(g_FrontendLoadedCampaignAsset);
    g_FrontendLoadedCampaignAsset = nullptr;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"campaign bundle");
    return;
  }
  FrontendCommand_Issue<FrontendPlayerRuntime_MarkLevelLoadedById>(0,0,0);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  /* The campaign starts at its first level (stored as the current level): find that level's record
     and build level\<name>.lev. levelRecordCursor is the asset base advanced by whole
     CampaignLevelRecords, so its levels[0] is the record under the cursor. The original lets the cursor end
     behind the last record without a match; FrontendScenarioTransfer_CampaignFits rejected such a campaign. */
  campaignAsset = g_FrontendLoadedCampaignAsset;
  levelRecordCursor = reinterpret_cast<uint8_t *>(campaignAsset);
  levelRecordsRemaining = campaignAsset->levelRecordCount;
  campaignAsset->currentLevelId = campaignAsset->firstLevelId;
  do {
    if (campaignAsset->firstLevelId == reinterpret_cast<CampaignAsset *>(levelRecordCursor)->levels[0].levelId) break;
    levelRecordCursor = levelRecordCursor + sizeof(CampaignLevelRecord);
    levelRecordsRemaining--;
  } while (levelRecordsRemaining != 0);
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             reinterpret_cast<CampaignAsset *>(levelRecordCursor)->levels[0].levelFileName,
             g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(ASSET_MAGIC_LEV,g_FrontendScenarioPathScratchUtf16);
  FrontendPlayerRuntime_InitializeFactionAssignments();
}

/* SCENARIO_TRANSFER_LEVEL_BUNDLE, packet: ScenarioLevelBundleHeader (unpacked sizes of level and field grid,
   their packed sizes), then the two packed streams. */
static void FrontendScenarioTransfer_ProcessReceivedLevelBundle()
{
  uint32_t *receivedDwords;
  uint32_t receivedByteCount;
  ScenarioLevelBundleHeader *bundle;
  FrontendLoadedLevelAsset *levelAsset;
  uintptr_t checkedValue;

  receivedDwords = static_cast<uint32_t *>(UiTransferMailbox_GetReceivedBuffer(&receivedByteCount));
  if (receivedDwords == nullptr) {
    return;
  }
  bundle = reinterpret_cast<ScenarioLevelBundleHeader *>(receivedDwords); /* the buffer starts with the bundle header */
  if ((receivedByteCount < sizeof(ScenarioLevelBundleHeader)) ||
      !FrontendScenarioTransfer_StreamsFit(receivedByteCount,sizeof(ScenarioLevelBundleHeader),
                                           bundle->levelEncodedBytes,bundle->fieldGridEncodedBytes,0) ||
      (bundle->levelDecodedBytes < sizeof(FrontendLoadedLevelAsset)) || (bundle->fieldGridDecodedBytes == 0)) {
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"level bundle");
    return;
  }
  FrontendScenarioTransfer_ReleaseLoadedLevelAsset();
  levelAsset = reinterpret_cast<FrontendLoadedLevelAsset *>(FrontendScenarioTransfer_AllocateOrExit(bundle->levelDecodedBytes));
  g_FrontendLoadedLevelAsset = levelAsset;
  if (!PckCodec_DecodeHuffmanRle(bundle->levelDecodedBytes,reinterpret_cast<uint8_t *>(levelAsset),bundle->levelEncodedBytes,
                                 Asset_RecordAfter<uint8_t>(bundle),nullptr,nullptr) ||
      !FrontendScenarioTransfer_LevelPathFits(levelAsset,bundle->levelDecodedBytes)) {
    /* the path offset field may hold anything: free the level alone */
    g_MemoryApi.free(levelAsset);
    g_FrontendLoadedLevelAsset = nullptr;
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"level bundle");
    return;
  }
  FrontendScenarioTransfer_SetFieldGridPathOfLevel(levelAsset);
  checkedValue = FrontendScenarioTransfer_AllocateOrExit(bundle->fieldGridDecodedBytes);
  (levelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)checkedValue; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  if (!FrontendScenarioTransfer_DecodeFieldGrid
            (bundle->fieldGridDecodedBytes,reinterpret_cast<FieldGridAsset *>(checkedValue),bundle->fieldGridEncodedBytes,
             Asset_RecordAfter<uint8_t>(bundle) + bundle->levelEncodedBytes)) {
    FrontendScenarioTransfer_ReleaseLoadedLevelAsset(); /* the level and its grid */
    FrontendScenarioTransfer_AbortReceive(receivedDwords,"level bundle");
    return;
  }
  FrontendCommand_Issue<FrontendPlayerRuntime_MarkLevelLoadedById>(0,0,0);
  FrontendScenarioTransfer_FinishReceive(receivedDwords);
  FrontendPlayerRuntime_InitializeFactionAssignments();
}

/* Network client, once per frontend frame: when the asset announced in g_FrontendScenarioTransferState has
   arrived in the transfer mailbox, unpacks it (scenario catalog, level, field grid, or a level/campaign bundle),
   frees the mailbox buffer and reports the new state to the host through the frontend command queue (or
   directly when no network session runs). Every packet starts with the unpacked size(s), then the packed data.
*/
void FrontendScenarioTransfer_ProcessReceivedAsset()

{
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) &&
     (g_FrontendScenarioTransferState != ScenarioTransferState::SCENARIO_TRANSFER_NONE)) {
    if (g_FrontendScenarioTransferState == ScenarioTransferState::SCENARIO_TRANSFER_CATALOG) {
      FrontendScenarioTransfer_ProcessReceivedCatalog();
    }
    else if (g_FrontendScenarioTransferState < ScenarioTransferState::SCENARIO_TRANSFER_FIELD_GRID) {
      FrontendScenarioTransfer_ProcessReceivedLevel();
    }
    else if (g_FrontendScenarioTransferState == ScenarioTransferState::SCENARIO_TRANSFER_FIELD_GRID) {
      FrontendScenarioTransfer_ProcessReceivedFieldGrid();
    }
    else if (g_FrontendScenarioTransferState < ScenarioTransferState::SCENARIO_TRANSFER_LEVEL_BUNDLE) {
      FrontendScenarioTransfer_ProcessReceivedCampaignBundle();
    }
    else {
      FrontendScenarioTransfer_ProcessReceivedLevelBundle();
    }
  }
}

/* Tells whether recordArray (recordCount records of 0x40 dwords each) contains a record equal to
   candidateRecord. Inverted like all failure flags: false = found, true = not found.
   recordCount must be at least 1.
*/
bool DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord)

{
  int dwordsRemainingInRecord;
  uint32_t *candidateRecordCursor;
  bool dwordsEqual;

  do {
    /* compare the 0x40 dwords until the first difference (the count is nonzero, so dwordsEqual holds the
       last comparison) */
    dwordsRemainingInRecord = DWORD_BLOCK64_RECORD_DWORDS;
    candidateRecordCursor = candidateRecord;
    do {
      dwordsRemainingInRecord--;
      dwordsEqual = *recordArray == *candidateRecordCursor;
      recordArray++;
      candidateRecordCursor++;
    } while (dwordsEqual && (dwordsRemainingInRecord != 0));
    if (dwordsEqual) {
      return false;
    }
    recordArray = recordArray + dwordsRemainingInRecord; /* skip the rest of the mismatching record */
    recordCount--;
  } while (recordCount != 0);
  return true;
}
