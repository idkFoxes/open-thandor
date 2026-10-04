/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static uint16_t g_ResourceRegistrationDirectoryUtf16[256] = {0};

uint16_t g_CampagneHexPathUtf16[13] = {'c', 'a', 'm', 'p', 'a', 'g', 'n', 'e', '.', 'h', 'e', 'x', 0}; /* L"campagne.hex" */

uint16_t g_OldunitHexPathUtf16[12] = {'o', 'l', 'd', 'u', 'n', 'i', 't', '.', 'h', 'e', 'x', 0}; /* L"oldunit.hex" */

uint8_t g_InGameResourceRegistrationBusyCount = 0;

/* L"army.hex" */
THANDOR_ALIGN(4) uint16_t g_ArmyHexPathUtf16[9] = {'a', 'r', 'm', 'y', '.', 'h', 'e', 'x', 0};

/* L"effect.hex" */
uint16_t g_EffectHexPathUtf16[11] = {'e', 'f', 'f', 'e', 'c', 't', '.', 'h', 'e', 'x', 0};

/* L"shot.hex" */
uint16_t g_ShotHexPathUtf16[9] = {'s', 'h', 'o', 't', '.', 'h', 'e', 'x', 0};

/* L"modul.hex" */
uint16_t g_ModulHexPathUtf16[10] = {'m', 'o', 'd', 'u', 'l', '.', 'h', 'e', 'x', 0};

/* L"light.hex" */
uint16_t g_LightHexPathUtf16[10] = {'l', 'i', 'g', 'h', 't', '.', 'h', 'e', 'x', 0};

/* L"widget.hex" */
uint16_t g_WidgetHexPathUtf16[11] = {'w', 'i', 'd', 'g', 'e', 't', '.', 'h', 'e', 'x', 0};

uint8_t *g_EffectRuntimeRebaseBaseMinusOne = 0;

uint8_t *g_RuntimeObjectRebaseBaseMinusOne = 0;

/* Implementation ownership: gameplay/session/savegame. */

/* Creates the save package at savePath; when that fails, creates the package's directory and tries once more.
   Returns true when the package is open in *packageHandle. */
static Bool8 InGameSaveGame_OpenNewPackage(void *savePath,EngineFileHandle *packageHandle)

{
  if (InGameSaveGame_CreatePackage(savePath,packageHandle)) {
    return true;
  }
  WidePath_SplitParentAndLeaf((uint16_t *)g_PackageScratchBuffer,g_ResourceRegistrationDirectoryUtf16,
                              (uint16_t *)savePath);
  if (g_FileSystemCreateDirectoryRecursive
          (FILESYSTEM_CREATE_DIRECTORY_RECURSIVE,g_ResourceRegistrationDirectoryUtf16) != 0) {
    return false;
  }
  return InGameSaveGame_CreatePackage(savePath,packageHandle);
}

/* Writes the runtime segments army, modul, shot, effect, widget, light, field, level and daten and the campagne
   entry (deleted without a campaign). Pointer-holding images are converted to offsets for writing and rebased
   afterwards, also when the write failed. Returns true on success; stops at the first failed write. */
static Bool8 InGameSaveGame_WriteRuntimeEntries(void *worldView,EngineFileHandle packageHandle)

{
  InGameLevelConditionStorage *levelStorage;
  ResourceRegistrationImagePair domainImagePair;
  RuntimeHexSegmentImage segmentImage;
  Bool8 upsertOk;

  ArmyRuntimePool_ConvertPointersToOffsetsForSave();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (PckDecodedByteCount)(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot)),
                              (uint32_t *)g_ArmyRuntimeSlots,(uint16_t *)g_ArmyHexPathUtf16,packageHandle);
  ArmyRuntimePool_RebaseAfterLoad();
  if (!upsertOk) {
    return false;
  }
  /* the whole model runtime slot image (0x400000 bytes) */
  ModelRuntimePool_UnrebaseBeforeSave();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (PckDecodedByteCount)(MODEL_RUNTIME_SLOT_COUNT * sizeof(ModelRuntimeSlot)),
                              (uint32_t *)g_ModelRuntimeSlots,(uint16_t *)g_ModulHexPathUtf16,packageHandle);
  ModelRuntimePool_RebaseAfterLoad();
  if (!upsertOk) {
    return false;
  }
  domainImagePair = InGameSaveGame_PrepareShotSlots();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)g_ShotHexPathUtf16,packageHandle);
  ShotRuntime_RebaseSlotsAfterLoad();
  if (!upsertOk) {
    return false;
  }
  domainImagePair = InGameSaveGame_PrepareEffectSlots();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)g_EffectHexPathUtf16,packageHandle);
  EffectRuntime_RebaseSlotsAfterLoad();
  if (!upsertOk) {
    return false;
  }
  domainImagePair = InGameSaveGame_PrepareRegistrationRecords((ResourceRegistrationRuntimeImageSavedView *)worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)g_WidgetHexPathUtf16,packageHandle);
  ResourceRegistrationRuntime_RebaseLoadedRecords((ResourceRegistrationRuntimeImage *)worldView);
  if (!upsertOk) {
    return false;
  }
  segmentImage = RuntimeHexSegment_GetLightImageAndToggleFlag();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)segmentImage.byteSize,
                              segmentImage.image,(uint16_t *)g_LightHexPathUtf16,packageHandle);
  RuntimeHexSegment_ToggleLightImageFlag();
  if (!upsertOk) {
    return false;
  }
  segmentImage = RuntimeHexSegment_GetFieldImage((InGameFieldImageSaveContext58 *)worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)segmentImage.byteSize,
                              segmentImage.image,(uint16_t *)g_FieldHexPathUtf16,packageHandle);
  RuntimeHexSegment_AfterFieldImageNoOp((InGameFieldImageSaveContext58 *)worldView);
  levelStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if (!upsertOk) {
    return false;
  }
  InGameSaveGame_StoreCameraAsPlayerStart((ResourceRegistrationRuntimeImage *)worldView);
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                              (levelStorage->levelImage).header.resourceTables.
                              runtimePrefixByteSizeAndInitialArmyPlacementOffset,(uint32_t *)levelStorage,
                              (uint16_t *)g_LevelHexPathUtf16,packageHandle);
  if (!upsertOk) {
    return false;
  }
  domainImagePair = InGameSaveGame_PrepareFactionImage();
  upsertOk = Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,(PckDecodedByteCount)domainImagePair,
                              (uint32_t *)(domainImagePair >> 32),(uint16_t *)g_DatenHexPathUtf16,packageHandle);
  GameFactionRuntime_RebaseLoadedArmyReferences();
  if (!upsertOk) {
    return false;
  }
  if (g_FrontendLoadedCampaignAsset == 0) {
    Package_DeleteEntry((uint16_t *)g_CampagneHexPathUtf16,packageHandle,NULL);
    return true;
  }
  return Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,((uint32_t *)(uintptr_t)g_FrontendLoadedCampaignAsset)[1],
                             (uint32_t *)(uintptr_t)g_FrontendLoadedCampaignAsset,
                             (uint16_t *)g_CampagneHexPathUtf16,packageHandle);
}

/* Reads the 0x200-byte package header into g_PackageScratchBuffer, fills in the save name (file name of savePath;
   the directory lands behind the header), the packed date and time, the "date, time" text, the level title text
   id and the campaign index, and writes it back. Returns true on success. */
static Bool8 InGameSaveGame_WritePackageHeader(void *savePath,EngineFileHandle packageHandle)

{
  void *handle = (void *)(uintptr_t)packageHandle;
  InGameSavePackageHeader *header = (InGameSavePackageHeader *)g_PackageScratchBuffer;
  uint32_t dateTextByteLength;
  uint16_t *timeText;
  uint32_t campaignIndex;

  if (g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,handle) != 0 ||
      g_FileSystemReadExact(sizeof(InGameSavePackageHeader),header,handle) != 0) {
    return false;
  }
  WidePath_SplitParentAndLeaf(header->saveNameUtf16,(uint16_t *)(header + 1),(uint16_t *)savePath);
  header->packedDate = g_LocaleGetPackedCurrentDate();
  header->packedTime = g_LocaleGetPackedCurrentTime();
  dateTextByteLength = g_LocaleFormatCurrentDateUtf16(header->dateTimeTextUtf16);
  timeText = (uint16_t *)((uint8_t *)header->dateTimeTextUtf16 + dateTextByteLength + 4);
  timeText[-2] = L','; /* ", " between date and time */
  timeText[-1] = L' ';
  g_LocaleFormatCurrentTimeUtf16(timeText);
  if (g_FrontendLoadedCampaignAsset == 0) {
    campaignIndex = INGAME_SAVE_NO_CAMPAIGN;
  }
  else {
    campaignIndex = g_InGameLevelCampaignAssociationIndex;
  }
  header->levelTitleTextId = g_InGameLevelTitleTextResourceIndex;
  header->campaignIndex = campaignIndex;
  return g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,handle) == 0 &&
         g_FileSystemWriteExactOrFlush(sizeof(InGameSavePackageHeader),header,handle) == 0;
}

/* Creates the package and writes every entry and the header; on success the package is unmounted.
   Returns true on success; on failure the package stays as it is. */
static Bool8 InGameSaveGame_WritePackageContents(void *worldView,void *savePath)

{
  EngineFileHandle packageHandle;

  if (!InGameSaveGame_OpenNewPackage(savePath,&packageHandle)) {
    return false;
  }
  if (!InGameSaveGame_WriteRuntimeEntries(worldView,packageHandle)) {
    return false;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,GAME_STAT_TABLE_BYTES,(uint32_t *)g_GameStatTableImage,
                      (uint16_t *)g_StatHexPathUtf16,packageHandle);
  /* The oldunit entry is written when there are old-unit records or any secondary-table dword is set. */
  if (InGameSaveGame_OldUnitTablesAreEmpty()) {
    Package_DeleteEntry((uint16_t *)g_OldunitHexPathUtf16,packageHandle,NULL);
  }
  else if (!InGameSaveGame_WriteOldUnitEntry(packageHandle)) {
    return false;
  }
  if (!InGameSaveGame_WritePackageHeader(savePath,packageHandle)) {
    return false;
  }
  Package_Unmount(packageHandle);
  return true;
}

/* Writes a save game (called by InGameSaveGame_SaveSelectedOrTypedName with the world view): opens or creates
   the package at savePath (creating its directory if needed) and stores every runtime segment as a
   Huffman/RLE entry - army, modul, shot, effect, widget, light, field, level, daten, campagne (deleted without
   a campaign), stat and oldunit (deleted when empty). Pointer-holding images are converted to offsets for
   writing and rebased afterwards. Finally the 0x200-byte package header gets the save name, date and time
   and the level title and campaign index. Returns true on failure (an opened package is then not unmounted); the busy
   count is raised meanwhile.
*/
Bool8 InGameSaveGame_WritePackage(void *worldView,void *savePath)

{
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  Bool8 written;

  g_InGameResourceRegistrationBusyCount++;
  /* first hand every player's pending army asset back to its faction */
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount; remainingPlayerBlocks != 0;
       remainingPlayerBlocks--) {
    GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
              (playerBlock->playerRuntimeId,0,0,(playerBlock->factionAssignment).factionAssignmentIndex);
    playerBlock++;
  }
  written = InGameSaveGame_WritePackageContents(worldView,savePath);
  g_InGameResourceRegistrationBusyCount--;
  return !written;
}

/* Creates a new, empty PCK package at packagePath and mounts it: builds a fresh 0x200-byte archive header
   (magic "pck", timestamps of now, the computer label as producer and source name, no entries) in the
   package scratch buffer, writes it as the whole file and mounts it. Returns true and the mounted package's
   file handle in *outHandle, or false (outHandle untouched) when Package_Mount fails.
   Called directly by the save-game writer InGameSaveGame_WritePackage, which creates the
   save directory and retries when it fails.
*/
Bool8 InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle)

{
  uint8_t *header;
  uint32_t packedTime;
  uint32_t packedDate;
  int byteIndex;
  EngineFileHandle mountedHandle;

  header = g_PackageScratchBuffer;
  for (byteIndex = 0; byteIndex < PCK_ENTRY_HEADER_BYTES; byteIndex++) {
    header[byteIndex] = 0;
  }
  /* PckArchiveHeader, written byte by byte (the original stores the same values as dwords):
     +0x00 magic "pck\0", +0x04 archive size 0x200 (header only), +0x08 version 1, +0x0C format 0x10000 */
  header[0] = 'p';
  header[1] = 'c';
  header[2] = 'k';
  header[3] = 0;
  header[4] = 0;
  header[5] = 2;
  header[6] = 0;
  header[7] = 0;
  header[8] = 1;
  header[9] = 0;
  header[10] = 0;
  header[11] = 0;
  header[12] = 0;
  header[13] = 0;
  header[14] = 1;
  header[15] = 0;
  /* three time/date pairs all set to now */
  packedTime = g_LocaleGetPackedCurrentTime();
  ((PckArchiveHeader *)header)->timeValue0 = packedTime;
  ((PckArchiveHeader *)header)->timeValue1 = packedTime;
  ((PckArchiveHeader *)header)->timeValue2 = packedTime;
  packedDate = g_LocaleGetPackedCurrentDate();
  ((PckArchiveHeader *)header)->dateValue0 = packedDate;
  ((PckArchiveHeader *)header)->dateValue1 = packedDate;
  ((PckArchiveHeader *)header)->dateValue2 = packedDate;
  g_LocaleCopyDefaultComputerLabelUtf16(((PckArchiveHeader *)header)->producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(((PckArchiveHeader *)header)->sourceName);
  header[256] = 0; /* unusedText: empty */
  /* +0xB0 entryCount = 0 */
  header[176] = 0;
  header[177] = 0;
  header[178] = 0;
  header[179] = 0;
  FileSystem_WriteBufferToPath(PCK_ENTRY_HEADER_BYTES,header,(uint16_t *)packagePath);
  if (!Package_Mount((uint16_t *)packagePath,&mountedHandle)) {
    return false;
  }
  *outHandle = mountedHandle;
  return true;
}

/* Save-game preparation of the runtime registration records (the "widget.hex" entry): turns the pointers of
   every allocated 0x100-byte record into saved offsets or ids, zeroes the free records and returns the record
   array with its byte size (recordCount * 0x100) for Package_UpsertEntry. The payload pointer is rebased per
   domain (0 army/model, 1 shot, 2 effect). ResourceRegistrationRuntime_RebaseLoadedRecords undoes it.
   Called directly by the save-game writer InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize; the C caller splits the qword the same way.
*/
ResourceRegistrationImagePair
InGameSaveGame_PrepareRegistrationRecords
          (ResourceRegistrationRuntimeImageSavedView *runtimeImage)

{
  uint32_t primaryOffset;
  uint32_t secondaryOffset;
  uint32_t nestedBaseOffset;
  uint32_t auxiliaryOffset;
  uint32_t nestedCount;
  uint32_t payloadOffset;
  uint32_t clearCount;
  uint32_t *recordDword;
  uint32_t *nestedOffset;
  ArmyRuntimeSlot *ownerArmy;
  ResourceRegistrationRecord *tailRecord;
  uint32_t recordsRemaining;
  uint32_t recordCount;
  ResourceRegistrationRecordSavedView *records;
  ResourceRegistrationRecordSavedView *recordCursor;

  recordCursor = runtimeImage->records;
  recordsRemaining = runtimeImage->recordCount;
  /* Original quirk: a do-while, so a record count of 0 still processes the first record and then wraps
     the counter. */
  do {
    if ((recordCursor->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
      /* free record: zero its 0x40 dwords */
      recordDword = (uint32_t *)recordCursor;
      for (clearCount = sizeof(ResourceRegistrationRecordSavedView) / 4; clearCount != 0; clearCount--) {
        *recordDword = 0;
        recordDword++;
      }
    }
    else {
      primaryOffset = recordCursor->primarySavedIdOrOffset;
      secondaryOffset = recordCursor->secondarySavedIdOrOffset;
      nestedBaseOffset = recordCursor->nestedBaseSavedOffset;
      if (primaryOffset != 0) {
        primaryOffset = primaryOffset - (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.primarySavedIdOrOffset */
      }
      if (secondaryOffset != 0) {
        secondaryOffset = secondaryOffset - (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.secondarySavedIdOrOffset */
      }
      if (nestedBaseOffset != 0) {
        nestedBaseOffset = nestedBaseOffset - (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.nestedBaseSavedOffset */
      }
      recordCursor->primarySavedIdOrOffset = primaryOffset;
      recordCursor->secondarySavedIdOrOffset = secondaryOffset;
      recordCursor->nestedBaseSavedOffset = nestedBaseOffset;
      recordCursor->ownerRuntimeSavedOffset = 0;
      auxiliaryOffset = recordCursor->auxiliarySavedIdOrOffset;
      nestedCount = recordCursor->nestedCount;
      if (auxiliaryOffset != 0) {
        auxiliaryOffset = auxiliaryOffset - THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1); /* 1-based offset, 0 = none */
      }
      recordCursor->auxiliarySavedIdOrOffset = auxiliaryOffset;
      /* nestedCount is not clamped to the 13 array entries: walk the dwords from the array start */
      nestedOffset = recordCursor->nestedSavedOffsets;
      for (; nestedCount != 0; nestedCount--) {
        if (*nestedOffset != 0) {
          *nestedOffset = *nestedOffset - (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.nestedSavedOffsets */
        }
        nestedOffset++;
      }
      payloadOffset = recordCursor->runtimePayloadSavedOffset;
      switch(recordCursor->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        /* the payload is a model runtime: the faction of its owner army is saved as the texture set
           (army graphics binding) index */
        ownerArmy = ((ModelRuntimeSlot *)payloadOffset)->ownerArmyRuntimeOrSavedOffset.armyRuntime; /* 5f-format: ResourceRegistrationRecordSavedView.runtimePayloadSavedOffset */
        recordCursor->paletteAssetSavedIdOrOffset = 0;
        payloadOffset = payloadOffset - g_ModelRuntimeRebaseDelta;
        recordCursor->textureSetSavedIdOrOffset = ownerArmy->factionIndex;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadOffset = payloadOffset - (int)g_ShotRuntimeRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.runtimePayloadSavedOffset */
        recordCursor->textureSetSavedIdOrOffset = 0;
        recordCursor->paletteAssetSavedIdOrOffset = 0;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadOffset = payloadOffset - (int)g_EffectRuntimeRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecordSavedView.runtimePayloadSavedOffset */
        recordCursor->textureSetSavedIdOrOffset = 0;
        recordCursor->paletteAssetSavedIdOrOffset = 0;
      }
      recordCursor->runtimePayloadSavedOffset = payloadOffset;
      /* the sprite asset pointer is replaced by the asset's registry id */
      recordCursor->spriteAssetSavedIdOrOffset =
           ((SpriteAssetHeader *)recordCursor->spriteAssetSavedIdOrOffset)->registryHeader.registryId; /* 5f-format: ResourceRegistrationRecordSavedView.spriteAssetSavedIdOrOffset */
    }
    recordCursor = recordCursor + 1;
    recordsRemaining--;
  } while (recordsRemaining != 0);
  tailRecord = runtimeImage->tailRecord;
  records = runtimeImage->records;
  if (tailRecord != NULL) {
    tailRecord = (ResourceRegistrationRecord *)((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne); /* 5f-format: ResourceRegistrationImage.tailRecord (saved offset) */
  }
  recordCount = runtimeImage->recordCount;
  /* the saved tail-record offset goes into the last dword of the image (record array + size - 4) */
  records[recordCount - 1].nestedSavedOffsets[12] = (uint32_t)tailRecord; /* 5f-format: ResourceRegistrationRecordSavedView.nestedSavedOffsets[12] (saved tail record) */
  return ((uint64_t)(uint32_t)(uintptr_t)records << 32) |
         (uint32_t)(recordCount * sizeof(ResourceRegistrationRecordSavedView));
}

/* Save-game preparation of the faction runtime image (the "daten.hex" entry): for each of the 8 faction
   records the army asset pointers are replaced by the asset ids (registryId of each asset) and the 8x32 group member
   pointers by saved army-slot offsets. Returns the image with its byte size 0x3A20 for Package_UpsertEntry;
   GameFactionRuntime_RebaseLoadedArmyReferences undoes it. Called directly by the save-game writer
   InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareFactionImage(void)

{
  ArmyRuntimeSlot *runtimeMember;
  int factionIndex;
  FactionArmyAssetCount armyAssetPointersRemaining;
  FactionArmyAssetCount primaryArmyAssetPointersRemaining;
  int memberIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *armyAssetPointerCursor;
  uint32_t *primaryArmyAssetPointerCursor;
  Ptr32<ArmyRuntimeSlot> *runtimeMembers;

  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    armyAssetPointerCursor = factionRecord->secondaryArmyAssetPointersOrIds;
    for (armyAssetPointersRemaining = factionRecord->secondaryArmyAssetCount;
        armyAssetPointersRemaining != 0; armyAssetPointersRemaining--) {
      *armyAssetPointerCursor = ((ArmyAssetRecordPrefix *)*armyAssetPointerCursor)->registryId; /* 5f-format: GameFactionRuntimeRecord.secondaryArmyAssetPointersOrIds */
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
    }
    primaryArmyAssetPointerCursor = factionRecord->primaryArmyAssetPointersOrIds;
    for (primaryArmyAssetPointersRemaining = factionRecord->primaryArmyAssetCount;
        primaryArmyAssetPointersRemaining != 0; primaryArmyAssetPointersRemaining--) {
      *primaryArmyAssetPointerCursor = ((ArmyAssetRecordPrefix *)*primaryArmyAssetPointerCursor)->registryId; /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      primaryArmyAssetPointerCursor = primaryArmyAssetPointerCursor + 1;
    }
    /* the 8x32 group member pointers become saved army-slot offsets (0 stays 0) */
    runtimeMembers = factionRecord->runtimeGroupMembers8x32;
    for (memberIndex = 0; memberIndex < 256; memberIndex++) {
      runtimeMember = runtimeMembers[memberIndex];
      if (runtimeMember != NULL) {
        runtimeMember =
             (ArmyRuntimeSlot *)((int)runtimeMember - (int)g_ArmyRuntimeRebaseBaseMinusOne); /* 5f-format: GameFactionRuntimeRecord.runtimeGroupMembers8x32 */
      }
      runtimeMembers[memberIndex] = runtimeMember;
    }
  }
  return ((uint64_t)(uint32_t)(uintptr_t)&g_GameFactionRuntimeImage << 32) | sizeof(GameFactionRuntimeImage);
}

/* Save-game preparation of the 0x1000 effect runtime slots (the "effect.hex" entry): in every used slot the
   model node and owner pointers become saved offsets (the owner is a model node or an army slot depending on
   the completion action) and the definition pointer becomes the definition id; free slots are zeroed.
   Returns the slot array with its byte size 0x40000; EffectRuntime_RebaseSlotsAfterLoad undoes it. Called
   directly by the save-game writer InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareEffectSlots(void)

{
  EffectRuntimeCompletionAction slotCompletionAction;
  EffectDefinitionReferenceOrSavedId serializedDefinitionId;
  int slotIndex;
  int wordIndex;
  ModelRuntimeNode *ownerModelNode;
  EffectRuntimeSlot *slot;
  uint32_t *slotWords;

  for (slotIndex = 0; slotIndex < EFFECT_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = g_EffectRuntimeSlots + slotIndex;
    if (slot->modelNodeOrSavedOffset.modelNode == NULL) {
      /* free slot: zero its 0x10 dwords */
      slotWords = (uint32_t *)slot;
      for (wordIndex = 0; wordIndex < 16; wordIndex++) {
        slotWords[wordIndex] = 0;
      }
      if (slotIndex == EFFECT_RUNTIME_SLOT_COUNT - 1) {
        /* Original quirk: slot 0's effectAgeTicks is inverted only on this exit (last slot free);
           EffectRuntime_RebaseSlotsAfterLoad inverts it on every load */
        g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
      }
      continue;
    }
    slotCompletionAction = slot->completionAction;
    ownerModelNode = slot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode;
    if (ownerModelNode != NULL) {
      if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
        ownerModelNode = (ModelRuntimeNode *)((int)ownerModelNode - g_ModelRuntimeRebaseDelta); /* 5f-format: EffectRuntimeSlot.lifecycleOwnerAndDefinition.owner */
      }
      else if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
        ownerModelNode =
             (ModelRuntimeNode *)((int)ownerModelNode - (int)g_ArmyRuntimeRebaseBaseMinusOne); /* 5f-format: EffectRuntimeSlot.lifecycleOwnerAndDefinition.owner */
      }
    }
    slot->modelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)
         ((int)slot->modelNodeOrSavedOffset.modelNode - (int)g_RuntimeObjectRebaseBaseMinusOne); /* 5f-format: EffectRuntimeSlot.modelNodeOrSavedOffset */
    serializedDefinitionId.savedId = slot->definitionOrSavedId.definition->definitionId;
    slot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode = ownerModelNode;
    slot->definitionOrSavedId = serializedDefinitionId;
  }
  return ((uint64_t)(uint32_t)(uintptr_t)g_EffectRuntimeSlots << 32) |
         (EFFECT_RUNTIME_SLOT_COUNT * sizeof(EffectRuntimeSlot));
}

/* Save-game preparation of the 0x1000 shot runtime slots (the "shot.hex" entry): in every used slot the model
   node, runtime state and owner army pointers become saved offsets and the definition pointer becomes the
   definition id; free slots are zeroed. Returns the slot array with its byte size 0x40000;
   ShotRuntime_RebaseSlotsAfterLoad undoes it. Called directly by the save-game writer
   InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareShotSlots(void)

{
  ShotDefinitionReferenceOrSavedId serializedDefinitionId;
  void *runtimeStateRef;
  int slotIndex;
  int wordIndex;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ShotRuntimeSlot *slot;
  uint32_t *slotWords;
  uint32_t *terminalToggleField;

  for (slotIndex = 0; slotIndex < SHOT_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = g_ShotRuntimeSlots + slotIndex;
    if (slot->modelNodeOrSavedOffset.modelNode == NULL) {
      /* free slot: zero its 0x10 dwords */
      slotWords = (uint32_t *)slot;
      for (wordIndex = 0; wordIndex < 16; wordIndex++) {
        slotWords[wordIndex] = 0;
      }
      if (slotIndex == SHOT_RUNTIME_SLOT_COUNT - 1) {
        /* Original quirk: slot 0's secondaryEffectCountdownTicks is inverted only on this exit (last slot free);
           ShotRuntime_RebaseSlotsAfterLoad inverts it on every load */
        terminalToggleField =
             &g_ShotRuntimeSlots->ownerAndTrajectory.secondaryEffectCountdownTicks;
        *terminalToggleField = ~*terminalToggleField;
      }
      continue;
    }
    runtimeStateRef = slot->runtimeStateOrSavedOffset.runtimeStatePointer;
    ownerArmyRuntime = slot->ownerAndTrajectory.ownerArmyRuntime;
    if (runtimeStateRef != NULL) {
      runtimeStateRef = (void *)((int)runtimeStateRef - g_ModelRuntimeRebaseDelta); /* 5f-format: ShotRuntimeSlot.runtimeStateOrSavedOffset */
    }
    if (ownerArmyRuntime != NULL) {
      ownerArmyRuntime =
           (ArmyRuntimeSlot *)((int)ownerArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne); /* 5f-format: ShotRuntimeSlot.ownerAndTrajectory.ownerArmyRuntime */
    }
    slot->modelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)
         ((int)slot->modelNodeOrSavedOffset.modelNode - (int)g_RuntimeObjectRebaseBaseMinusOne); /* 5f-format: ShotRuntimeSlot.modelNodeOrSavedOffset */
    slot->runtimeStateOrSavedOffset.runtimeStatePointer = runtimeStateRef;
    serializedDefinitionId.savedId = slot->definitionOrSavedId.definition->definitionId;
    slot->ownerAndTrajectory.ownerArmyRuntime = ownerArmyRuntime;
    slot->definitionOrSavedId = serializedDefinitionId;
  }
  return ((uint64_t)(uint32_t)(uintptr_t)g_ShotRuntimeSlots << 32) | SHOT_RUNTIME_POOL_BYTES;
}

/* Before the level image is saved: stores the current camera (orientation as magnitude plus packed
   heading/pitch, and position) into the start-camera fields of the level player slot selected by
   runtimeImage->factionAssignmentIndex, so a loaded game starts with the camera where it was.
   Called directly by the save-game writer InGameSaveGame_WritePackage.
*/

void InGameSaveGame_StoreCameraAsPlayerStart(ResourceRegistrationRuntimeImage *runtimeImage)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorage *levelConditionStorage;
  WorldCameraOrientation cameraOrientation;
  WorldCameraPosition cameraPosition;
  LevelPlayerSlotRecord *playerSlot;
  
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets
          [runtimeImage->factionAssignmentIndex - 1];
  cameraOrientation = WorldRuntime_GetCameraOrientation((WorldRuntimeContext *)runtimeImage);
  playerSlot = (LevelPlayerSlotRecord *)((uint8_t *)levelConditionStorage->levelImage.playerSlots +
                                         playerSlotByteOffset);
  playerSlot->startCameraMagnitudeQ12 = cameraOrientation.magnitudeQ12;
  playerSlot->packedHeadingLow16PitchHigh16 =
       cameraOrientation.headingAngle & FIXED_ANGLE16_MASK | cameraOrientation.pitchAngle << 16;
  cameraPosition = WorldRuntime_GetCameraPosition((WorldRuntimeContext *)runtimeImage);
  playerSlot->startCameraXQ12 = cameraPosition.xQ12;
  playerSlot->startCameraYQ12 = cameraPosition.yQ12;
  playerSlot->startCameraZQ12 = cameraPosition.zQ12;
}

/* Savegame writing (called by the in-game save in ui/ingame/runtime): turns the four pointers of every used
   army slot (model runtime, model node, command target, assigned target) into offsets and zeroes the unused
   slots, so the pool can be written as it is (the caller then writes g_ArmyRuntimeSlots,
   ARMY_RUNTIME_SLOT_COUNT slots); ArmyRuntimePool_RebaseAfterLoad is the counterpart.
*/
void ArmyRuntimePool_ConvertPointersToOffsetsForSave(void)

{
  uint32_t assignedTargetOffset;
  ModelRuntimeSlot *savedModelRuntimeOffset;
  ArmyRuntimeSlot *savedTargetOffset;
  ArmyRuntimeSlot *slot;
  uint32_t *slotWords;
  int slotIndex;
  int wordIndex;

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = &g_ArmyRuntimeSlots[slotIndex];
    if (slot->modelNodeRuntime == NULL) {
      /* an unused slot is zeroed dword by dword */
      slotWords = (uint32_t *)slot;
      for (wordIndex = 0; wordIndex < (int)(sizeof(ArmyRuntimeSlot) / 4); wordIndex++) {
        slotWords[wordIndex] = 0;
      }
      continue;
    }
    savedModelRuntimeOffset = (ModelRuntimeSlot *) /* 5f-format: ArmyRuntimeSlot.modelRuntimeOrSavedOffset (army.hex) */
             ((int)(slot->modelRuntimeOrSavedOffset).modelRuntime - g_ModelRuntimeRebaseDelta);
    savedTargetOffset = slot->commandTargetArmyRuntime;
    if (savedTargetOffset != NULL) {
      savedTargetOffset = (ArmyRuntimeSlot *)((int)savedTargetOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne); /* 5f-format: ArmyRuntimeSlot.commandTargetArmyRuntime (army.hex) */
    }
    slot->modelNodeRuntime = /* 5f-format: ArmyRuntimeSlot.modelNodeRuntime (army.hex) */
         (ModelRuntimeNode *)((int)slot->modelNodeRuntime - (int)g_RuntimeObjectRebaseBaseMinusOne);
    assignedTargetOffset = slot->assignedTargetArmyRuntime;
    (slot->modelRuntimeOrSavedOffset).modelRuntime = savedModelRuntimeOffset;
    if (assignedTargetOffset != 0) {
      assignedTargetOffset = assignedTargetOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne; /* 5f-format: ArmyRuntimeSlot.assignedTargetArmyRuntime (army.hex) */
    }
    slot->commandTargetArmyRuntime = savedTargetOffset;
    slot->assignedTargetArmyRuntime = assignedTargetOffset;
  }
}

/* Pre-serializer provider of the light.hex save segment (called by
   InGameSaveGame_WritePackage): returns the shading runtime records and their byte
   size 0x4000, and inverts serializationToggleDword of record 0 so the saved image carries the
   inverted value; RuntimeHexSegment_ToggleLightImageFlag inverts it back after saving.
*/
RuntimeHexSegmentImage __cdecl RuntimeHexSegment_GetLightImageAndToggleFlag(void)

{
  RuntimeHexSegmentImage segment;

  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  segment.image = (uint32_t *)g_GraphicsShadingRuntimeRecords;
  segment.byteSize = sizeof(g_GraphicsShadingRuntimeRecords);
  return segment;
}

/* Post-serializer hook of the light.hex save segment: inverts serializationToggleDword of shading record 0
   back (RuntimeHexSegment_GetLightImageAndToggleFlag inverted it before), so the saved image carries the
   inverted value while the live one is unchanged. The caller keeps the serializer flags.
*/
void __cdecl RuntimeHexSegment_ToggleLightImageFlag(void)

{
  g_GraphicsShadingRuntimeRecords[0].serializationToggleDword =
       ~g_GraphicsShadingRuntimeRecords[0].serializationToggleDword;
  return;
}

/* Pre-serializer provider of the field.hex save segment (called by
   InGameSaveGame_WritePackage): returns the attached field grid (fieldGridAsset) and its whole
   allocation size (common.allocationSizeBytes), so the field image is saved as one block.
*/
RuntimeHexSegmentImage RuntimeHexSegment_GetFieldImage(InGameFieldImageSaveContext58 *fieldImageContext)

{
  RuntimeHexSegmentImage segment;

  segment.image = (uint32_t *)fieldImageContext->fieldGridAsset;
  segment.byteSize = (uint32_t)(fieldImageContext->fieldGridAsset->common).allocationSizeBytes;
  return segment;
}

/* Post-serializer hook of the field.hex save segment (called by InGameSaveGame_WritePackage):
   does nothing; the field image needs no restoring after saving. The caller keeps the serializer flags.
*/
void RuntimeHexSegment_AfterFieldImageNoOp(InGameFieldImageSaveContext58 *fieldImageContext)

{
  return;
}
