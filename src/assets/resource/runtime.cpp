/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/resource/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/resource/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

uint8_t *g_EffectRuntimeRebaseBaseMinusOne = 0;

uint8_t *g_RuntimeObjectRebaseBaseMinusOne = 0;

/* Implementation ownership: assets/resource/runtime. */

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


/* Loads a whole resource into a fresh arena buffer. A mounted package entry is decoded into the buffer;
   otherwise the loose file is read, first from the executable's directory, then from the path as given.
   Returns true with the buffer in *outBuffer and its byte count in *outByteCount. On failure returns false
   with the file-system, decoder or out-of-memory code in *outErrorCode and leaves *outBuffer and
   *outByteCount unchanged. outByteCount and outErrorCode may be NULL.
*/
Bool8 Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckEntryHeader *entry;
  uint32_t fileSize;
  uint32_t errorCode;
  void *fileHandle;
  void *buffer;
  Bool8 gotSize;
  EngineFileHandle entryFileHandle;

  entry = Package_FindEntryAcrossMounts(path,&entryFileHandle);
  if (entry == NULL) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    errorCode = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16,&fileHandle);
    if (errorCode != 0) {
      errorCode = g_FileSystemOpen(0,path,&fileHandle);
    }
    if (errorCode == 0) {
      gotSize = g_FileSystemGetSize(fileHandle,&fileSize);
      errorCode = fileSize; /* 0 when the size query failed */
      if (gotSize) {
        if (g_MemoryApi.alloc(fileSize,&buffer) != 0) {
          /* the requested size becomes the detail line of the out-of-memory message */
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)fileSize,g_FatalErrorDetail1Utf16);
          errorCode = FATAL_ERROR_OUT_OF_MEMORY;
        }
        else {
          errorCode = g_FileSystemReadExact((FileIoByteCount)fileSize,buffer,fileHandle);
          if (errorCode == 0) {
            g_FileSystemClose(fileHandle);
            *outBuffer = buffer;
            if (outByteCount != NULL) {
              *outByteCount = fileSize;
            }
            return true;
          }
          g_MemoryApi.free(buffer);
        }
      }
      g_FileSystemClose(fileHandle);
    }
  }
  else {
    errorCode = FATAL_ERROR_OUT_OF_MEMORY;
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      errorCode = g_MemoryApi.alloc(entry->unpackedSize,&buffer);
      if (errorCode == 0) {
        if (Package_DecodeEntryInto((uint8_t *)buffer,entry,entryFileHandle,NULL,&errorCode)) {
          *outBuffer = buffer;
          if (outByteCount != NULL) {
            *outByteCount = entry->unpackedSize;
          }
          return true;
        }
        g_MemoryApi.free(buffer);
      }
    }
  }
  if (outErrorCode != NULL) {
    *outErrorCode = errorCode;
  }
  return false;
}


/* Frees a buffer returned by Resource_Load (or Package_LoadEntry) back to the arena heap.
*/
void Resource_Release(void *resourceBuffer)

{
  g_MemoryApi.free(resourceBuffer);
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

