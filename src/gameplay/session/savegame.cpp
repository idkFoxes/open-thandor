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

/* True when there is nothing to store in the oldunit entry: no old-unit records and every secondary-table
   dword zero. */
static Bool8 InGameSaveGame_OldUnitTablesAreEmpty(void)

{
  int index;

  if (g_OldUnitRecordCount != 0) {
    return false;
  }
  for (index = 0; index < OLD_UNIT_SECONDARY_TABLE_BYTES / 4; index++) {
    if (g_OldUnitSecondaryTable[index] != 0) {
      return false;
    }
  }
  return true;
}

/* Writes the oldunit entry: the record count followed by the primary and the secondary table, packed into a
   temporary allocation. Returns false only when that allocation fails. */
static Bool8 InGameSaveGame_WriteOldUnitEntry(EngineFileHandle packageHandle)

{
  uint32_t *oldUnitImage;
  uint32_t *destinationCursor;
  int index;

  if (g_MemoryApi.alloc(4 + OLD_UNIT_PRIMARY_TABLE_BYTES + OLD_UNIT_SECONDARY_TABLE_BYTES,(void **)&oldUnitImage) != 0) {
    return false;
  }
  oldUnitImage[0] = g_OldUnitRecordCount;
  destinationCursor = oldUnitImage + 1;
  for (index = 0; index < OLD_UNIT_PRIMARY_TABLE_BYTES / 4; index++) {
    *destinationCursor = g_OldUnitPrimaryTable[index];
    destinationCursor++;
  }
  for (index = 0; index < OLD_UNIT_SECONDARY_TABLE_BYTES / 4; index++) {
    *destinationCursor = g_OldUnitSecondaryTable[index];
    destinationCursor++;
  }
  Package_UpsertEntry(PCK_COMPRESSION_HUFFMAN_RLE,
                      (PckDecodedByteCount)((uint8_t *)destinationCursor - (uint8_t *)oldUnitImage),oldUnitImage,
                      (uint16_t *)g_OldunitHexPathUtf16,packageHandle);
  g_MemoryApi.free(oldUnitImage);
  return true;
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
