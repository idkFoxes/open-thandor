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
__declspec(align(4)) uint16_t g_ArmyHexPathUtf16[9] = {'a', 'r', 'm', 'y', '.', 'h', 'e', 'x', 0};

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

/* Turns the saved form of the resource registration records (widget.hex) back into pointers, after a savegame
   load and after writing a savegame: the 1-based offsets become runtime-object, shading-record, army/shot/effect
   slot pointers, texture set and palette are re-selected per domain, and the sprite id is resolved again. Also
   restores the tail record pointer and the local player's faction assignment.
*/
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage)

{
  ResourceRegistrationRecord *registrationRecord;
  ResourceRegistrationRecord *tailRecord;
  uint32_t remainingRecords;
  uint32_t nestedCount;
  uint32_t nestedIndex;
  uint8_t *primaryPointer;
  uint8_t *secondaryPointer;
  uint8_t *nestedBasePointer;
  uint8_t *auxiliaryPointer;
  void *tailNestedPointer;
  GraphicsTextureSet *selectedTextureSet;
  GraphicsPaletteAsset *selectedPalette;
  SpriteAssetHeader *resolvedSprite;
  ArmyRuntimeSlot *payloadSlot;

  registrationRecord = runtimeImage->records;
  remainingRecords = runtimeImage->recordCount;
  /* Original quirk: the record loop tests its count only after the first record, so an image with recordCount 0
     would walk 2^32 records. */
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = (uint8_t *)(registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      secondaryPointer = (uint8_t *)(registrationRecord->secondaryPointerOrSavedOffset).runtimePointer;
      nestedBasePointer = (uint8_t *)(registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer;
      /* 1-based offsets from the runtime-object base; 0 stays NULL */
      if (primaryPointer != NULL) {
        primaryPointer = primaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      if (secondaryPointer != NULL) {
        secondaryPointer = secondaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      if (nestedBasePointer != NULL) {
        nestedBasePointer = nestedBasePointer + (int)g_RuntimeObjectRebaseBaseMinusOne; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      (registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)primaryPointer; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer = secondaryPointer;
      (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer = nestedBasePointer;
      (registrationRecord->ownerRuntimeOrSavedOffset).runtimePointer = runtimeImage;
      auxiliaryPointer = (uint8_t *)(registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      nestedCount = registrationRecord->nestedCount;
      if (auxiliaryPointer != NULL) {
        /* 1-based offset from the shading records; 0 is null */
        auxiliaryPointer = (uint8_t *)(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + (int)auxiliaryPointer); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      (registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)auxiliaryPointer; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      /* the nested pointers are 1-based offsets from the runtime-object base as well */
      for (nestedIndex = 0; nestedIndex < nestedCount; nestedIndex++) {
        if (registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer != NULL) {
          registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer =
               (uint8_t *)((int)registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer +
                       (int)g_RuntimeObjectRebaseBaseMinusOne); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        }
      }
      payloadSlot = (registrationRecord->runtimePayload).armyRuntime;
      switch(registrationRecord->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        /* textureSet holds the army graphics binding index until here */
        payloadSlot = (ArmyRuntimeSlot *)
                     ((int)payloadSlot + g_ModelRuntimeRebaseDelta); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet; /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + (int)payloadSlot); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = g_ShotPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_EffectRuntimeRebaseBaseMinusOne + (int)payloadSlot); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
        selectedTextureSet = g_EffectTextureSet;
        selectedPalette = g_EffectPalette;
        /* effects flagged 2 in their model runtime use the army graphics of binding 0 */
        if ((((payloadSlot->modelRuntimeOrSavedOffset).modelRuntime)->effectModelFlags
            & 2) != 0) {
          selectedTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          selectedPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        registrationRecord->textureSet = selectedTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
      }
      (registrationRecord->runtimePayload).armyRuntime = payloadSlot;
      resolvedSprite = SpriteAssetRegistry_FindById((SpriteAssetId)registrationRecord->spriteAsset); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
      registrationRecord->spriteAsset = resolvedSprite;
    }
    registrationRecord++;
    remainingRecords--;
  } while (remainingRecords != 0);
  /* the last nested slot of the last record is the saved tail record */
  tailNestedPointer = runtimeImage->records[runtimeImage->recordCount - 1].nestedPointersOrSavedOffsets
           [12].runtimePointer;
  tailRecord = NULL;
  if (tailNestedPointer != NULL) {
    tailRecord = (ResourceRegistrationRecord *)(g_RuntimeObjectRebaseBaseMinusOne + (int)tailNestedPointer); /* 5f-format: ResourceRegistrationRecord saved offsets (widget.hex) */
  }
  runtimeImage->tailRecord = tailRecord;
  (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex = runtimeImage->factionAssignmentIndex;
}

/* Loads one saved runtime pool (a .hex entry of the save package) into its buffer; false with the load error in
   *outError, which stays unchanged on success. */
static Bool8 SavedLevel_LoadRuntimePool
          (PckLoadCapacityFlags bufferCapacity,uint8_t *destination,uint16_t *path,uint32_t *outError)

{
  uint32_t loadResult; /* Package_LoadEntryIntoBuffer: byte count on success, error code on failure */

  if (!Package_LoadEntryIntoBuffer(bufferCapacity,destination,path,&loadResult)) {
    return NewLevel_Fail(outError,loadResult);
  }
  return true;
}

/* Loads the saved runtime pools (widget.hex, army.hex, modul.hex, effect.hex, shot.hex, light.hex) over the
   freshly initialised ones and rebases their pointers. */
Bool8 SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  if (!SavedLevel_LoadRuntimePool(worldRuntime->objectCount * sizeof(WorldObjectRecord),
                                  (uint8_t *)worldRuntime->objectArray,(uint16_t *)g_WidgetHexPathUtf16,
                                  outError) ||
      !SavedLevel_LoadRuntimePool(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),(uint8_t *)g_ArmyRuntimeSlots,
                                  (uint16_t *)g_ArmyHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(MODEL_RUNTIME_POOL_BYTES,(uint8_t *)g_ModelRuntimeSlots,
                                  (uint16_t *)g_ModulHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(EFFECT_RUNTIME_POOL_BYTES,(uint8_t *)g_EffectRuntimeSlots,
                                  (uint16_t *)g_EffectHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(SHOT_RUNTIME_POOL_BYTES,(uint8_t *)g_ShotRuntimeSlots,
                                  (uint16_t *)g_ShotHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(sizeof(g_GraphicsShadingRuntimeRecords),
                                  (uint8_t *)g_GraphicsShadingRuntimeRecords,(uint16_t *)g_LightHexPathUtf16,
                                  outError)) {
    return false;
  }
  ArmyRuntimePool_RebaseAfterLoad();
  ModelRuntimePool_RebaseAfterLoad();
  ShotRuntime_RebaseSlotsAfterLoad();
  EffectRuntime_RebaseSlotsAfterLoad();
  ResourceRegistrationRuntime_RebaseLoadedRecords((ResourceRegistrationRuntimeImage *)worldRuntime);
  RuntimeHexSegment_ToggleLightImageFlag();
  return true;
}
