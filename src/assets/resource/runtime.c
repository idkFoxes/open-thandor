/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/resource/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/resource/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/resource/runtime. */

/* Address: 0x0040E2E0.
   Creates a new, empty PCK package at packagePath and mounts it: builds a fresh 0x200-byte archive header
   (magic "pck", timestamps of now, the computer label as producer and source name, no entries) in the
   package scratch buffer, writes it as the whole file and returns Package_Mount's result (CF set on failure).
   Called directly by the save-game writer InGameUiAction1210_ResourceRegistrationHelper, which creates the
   save directory and retries when it fails.
*/
StatusResult ResourceRegistration_OpenSource(void *packagePath)

{
  uint8_t *header;
  uint32_t packedTimeOrDate;
  int clearDwordsRemaining;
  uint8_t *clearCursor;
  StatusResult mountResult;

  header = g_PackageScratchBuffer;
  clearCursor = g_PackageScratchBuffer;
  for (clearDwordsRemaining = 0x80; clearDwordsRemaining != 0; clearDwordsRemaining--) {
    clearCursor[0] = 0;
    clearCursor[1] = 0;
    clearCursor[2] = 0;
    clearCursor[3] = 0;
    clearCursor = clearCursor + 4;
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
  header[0xb] = 0;
  header[0xc] = 0;
  header[0xd] = 0;
  header[0xe] = 1;
  header[0xf] = 0;
  /* three time/date pairs (+0x10..+0x27) all set to now; the time goes to the lower dword of each pair */
  packedTimeOrDate = g_LocaleGetPackedCurrentTime();
  *(uint32_t *)(header + 0x10) = packedTimeOrDate;
  *(uint32_t *)(header + 0x18) = packedTimeOrDate;
  *(uint32_t *)(header + 0x20) = packedTimeOrDate;
  packedTimeOrDate = g_LocaleGetPackedCurrentDate();
  *(uint32_t *)(header + 0x14) = packedTimeOrDate;
  *(uint32_t *)(header + 0x1c) = packedTimeOrDate;
  *(uint32_t *)(header + 0x24) = packedTimeOrDate;
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(header + 0x30)); /* producerName */
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(header + 0x70)); /* sourceName */
  header[0x100] = 0;
  /* +0xB0 entryCount = 0 */
  header[0xb0] = 0;
  header[0xb1] = 0;
  header[0xb2] = 0;
  header[0xb3] = 0;
  FileSystem_WriteBufferToPath(PCK_ENTRY_HEADER_BYTES,header,packagePath);
  mountResult = Package_Mount(packagePath);
  return mountResult;
}


/* Address: 0x0040F000.
   Loads a whole resource into a fresh arena buffer and returns it with its byte count. A mounted package
   entry is decoded into the buffer; otherwise the loose file is read, first from the executable's directory,
   then from the path as given. On failure CF is set and EAX carries the file-system or out-of-memory code.
*/
ResourceLoadResult Resource_Load(uint16_t *path)

{
  PckEntryHeader *entry;
  uint8_t *fileSize;
  uint8_t *sizeOrErrorCode;
  /* ECX on failure: the file size once it is known, else the caller's ECX (zero stands in for it). It is only
     meaningful on success (byte count); callers test CF. */
  uint32_t failureByteCount = 0;
  ArenaAllocResult allocResult;
  PackageDecodeResult decodeResult;
  FileSystemOpenResult openResult;
  FileSystemSizeResult sizeResult;
  FileSystemReadResult readResult;
  PackageEntryLookupResult findResult;
  ResourceLoadResult fileOrPackageResult;
  ResourceLoadResult fileLoadResult;
  ResourceLoadResult failureResult;
  
  findResult = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)findResult.entry;
  if (findResult.notFound) {
    WidePath_CombineDirectoryAndLeaf
              ((uint16_t *)&g_FileSystemCombinedPathScratchUtf16,path,
               (uint16_t *)&g_ExecutableDirectoryUtf16);
    openResult = g_FileSystemOpen(0,(uint16_t *)&g_FileSystemCombinedPathScratchUtf16);
    fileOrPackageResult.bufferOrError = (uint8_t *)openResult.handleOrError;
    if (openResult.failed) {
      openResult = g_FileSystemOpen(0,path);
      fileOrPackageResult.bufferOrError = (uint8_t *)openResult.handleOrError;
      if (openResult.failed) goto Resource_Load_ReturnOpenAllocationOrDecodeResult;
    }
    sizeResult = g_FileSystemGetSize(fileOrPackageResult.bufferOrError);
    fileSize = (uint8_t *)sizeResult.sizeOrError;
    sizeOrErrorCode = fileSize;
    if (!sizeResult.failed) {
      allocResult = g_MemoryApi.alloc((uint32_t)fileSize);
      fileLoadResult.bufferOrError = (void *)allocResult.payloadOrError;
      failureByteCount = (uint32_t)fileSize;
      if (allocResult.failed) {
        /* the requested size becomes the detail line of the out-of-memory message */
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)fileSize,g_FatalErrorDetail1Utf16);
        sizeOrErrorCode = (uint8_t *)FATAL_ERROR_OUT_OF_MEMORY;
      }
      else {
        readResult = g_FileSystemReadExact((FileIoByteCount)fileSize,fileLoadResult.bufferOrError,fileOrPackageResult.bufferOrError);
        sizeOrErrorCode = (uint8_t *)readResult.valueOrError;
        if (!readResult.failed) {
          g_FileSystemClose(fileOrPackageResult.bufferOrError);
          fileLoadResult.byteCount = (uint32_t)fileSize;
          fileLoadResult.failed = false;
          return fileLoadResult;
        }
        g_MemoryApi.free(fileLoadResult.bufferOrError);
      }
    }
    g_FileSystemClose(fileOrPackageResult.bufferOrError);
    fileOrPackageResult.bufferOrError = sizeOrErrorCode;
  }
  else {
    fileOrPackageResult.bufferOrError = (uint8_t *)FATAL_ERROR_OUT_OF_MEMORY;
    if (entry->packedSize < PACKAGE_SCRATCH_BUFFER_BYTES + 1) {
      allocResult = g_MemoryApi.alloc(entry->unpackedSize);
      fileOrPackageResult.bufferOrError = (uint8_t *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        decodeResult = Package_DecodeEntryInto(fileOrPackageResult.bufferOrError,entry,findResult.fileHandle);
        if (!decodeResult.failed) {
          fileOrPackageResult.byteCount = entry->unpackedSize;
          fileOrPackageResult.failed = false;
          return fileOrPackageResult;
        }
        sizeOrErrorCode = (uint8_t *)decodeResult.valueOrError;
        g_MemoryApi.free(fileOrPackageResult.bufferOrError);
        fileOrPackageResult.bufferOrError = sizeOrErrorCode;
      }
    }
  }
Resource_Load_ReturnOpenAllocationOrDecodeResult:
  failureResult.byteCount = failureByteCount;
  failureResult.bufferOrError = (uint32_t)fileOrPackageResult.bufferOrError;
  failureResult.failed = true;
  return failureResult;
}


/* Address: 0x0040F1D0.
   Frees a buffer returned by Resource_Load (or Package_LoadEntry) back to the arena heap. Unlike a direct
   g_MemoryApi.free call it keeps EAX, ECX and EDX, so register-convention callers need not save them.
*/
void Resource_Release(void *resourceBuffer)

{
  g_MemoryApi.free(resourceBuffer);
}


/* Address: 0x0050E890.
   Save-game preparation of the runtime registration records (the "widget.hex" entry): turns the pointers of
   every allocated 0x100-byte record into saved offsets or ids, zeroes the free records and returns the record
   array with its byte size (recordCount * 0x100) for Package_UpsertEntry. The payload pointer is rebased per
   domain (0 army/model, 1 shot, 2 effect). ResourceRegistrationRuntime_RebaseLoadedRecords undoes it.
   Called directly by the save-game writer InGameUiAction1210_ResourceRegistrationHelper.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX, and the
   C caller splits the qword the same way, so the swap is internal to the C.
*/
ResourceRegistrationImagePair
ResourceRegistration_SelectDomainPair
          (ResourceRegistrationRuntimeImageSerializedScalarViewDC *runtimeImage)

{
  uint32_t rebasedOffset;
  uint32_t secondaryOffsetOrNestedCount;
  int clearCountOrArmyDefinition;
  ResourceRegistrationRecord100 *tailRecord;
  uint32_t recordsRemainingOrCount;
  uint32_t nestedBaseOffset;
  ResourceRegistrationRecordSerializedScalarView100 *nestedOffsetCursor;
  ResourceRegistrationRecordSerializedScalarView100 *recordCursor;
  
  recordCursor = runtimeImage->records58;
  recordsRemainingOrCount = runtimeImage->recordCountAC;
  do {
    while ((recordCursor->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
      /* free record: zero its 0x40 dwords, which also advances recordCursor to the next record */
      for (clearCountOrArmyDefinition = 0x40; clearCountOrArmyDefinition != 0; clearCountOrArmyDefinition--) {
        recordCursor->primarySavedIdOrOffset = 0;
        recordCursor = (ResourceRegistrationRecordSerializedScalarView100 *)
                 &recordCursor->secondarySavedIdOrOffset;
      }
      recordsRemainingOrCount--;
      if (recordsRemainingOrCount == 0) {
        tailRecord = runtimeImage->tailRecordD8;
        recordCursor = runtimeImage->records58;
        if (tailRecord != NULL) {
          tailRecord = (ResourceRegistrationRecord100 *)
                   ((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        recordsRemainingOrCount = runtimeImage->recordCountAC;
        /* the saved tail-record offset goes into the last dword of the image (record array + size - 4) */
        recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets13[0xc] = (uint32_t)tailRecord;
        return ((uint64_t)(uint32_t)(uintptr_t)recordCursor << 32) | (uint32_t)(recordsRemainingOrCount * 0x100);
      }
    }
    rebasedOffset = recordCursor->primarySavedIdOrOffset;
    secondaryOffsetOrNestedCount = recordCursor->secondarySavedIdOrOffset;
    nestedBaseOffset = recordCursor->nestedBaseSavedOffset;
    if (rebasedOffset != 0) {
      rebasedOffset = rebasedOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    if (secondaryOffsetOrNestedCount != 0) {
      secondaryOffsetOrNestedCount = secondaryOffsetOrNestedCount - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    if (nestedBaseOffset != 0) {
      nestedBaseOffset = nestedBaseOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    recordCursor->primarySavedIdOrOffset = rebasedOffset;
    recordCursor->secondarySavedIdOrOffset = secondaryOffsetOrNestedCount;
    recordCursor->nestedBaseSavedOffset = nestedBaseOffset;
    recordCursor->ownerRuntimeSavedOffset = 0;
    rebasedOffset = recordCursor->auxiliarySavedIdOrOffset;
    secondaryOffsetOrNestedCount = recordCursor->nestedCountC8;
    if (rebasedOffset != 0) {
      rebasedOffset = rebasedOffset - THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1);
    }
    recordCursor->auxiliarySavedIdOrOffset = rebasedOffset;
    nestedOffsetCursor = recordCursor;
    for (; secondaryOffsetOrNestedCount != 0; secondaryOffsetOrNestedCount--) {
      if (nestedOffsetCursor->nestedSavedOffsets13[0] != 0) {
        nestedOffsetCursor->nestedSavedOffsets13[0] =
             nestedOffsetCursor->nestedSavedOffsets13[0] - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      nestedOffsetCursor = (ResourceRegistrationRecordSerializedScalarView100 *)
               &nestedOffsetCursor->secondarySavedIdOrOffset;
    }
    rebasedOffset = recordCursor->runtimePayloadSavedOffset;
                    
    switch(recordCursor->domainIndex) {
    case RESOURCE_DOMAIN_ARMY_RUNTIME:
      /* the payload's +8 is the army definition; its +0xC is the saved texture set id */
      clearCountOrArmyDefinition = *(int *)(rebasedOffset + 8);
      recordCursor->paletteAssetSavedIdOrOffset = 0;
      rebasedOffset = rebasedOffset - g_ModelRuntimeRebaseDelta;
      recordCursor->textureSetSavedIdOrOffset = *(uint32_t *)(clearCountOrArmyDefinition + 0xc);
      break;
    case RESOURCE_DOMAIN_SHOT_RUNTIME:
      rebasedOffset = rebasedOffset - (int)g_ShotRuntimeRebaseBaseMinusOne;
      recordCursor->textureSetSavedIdOrOffset = 0;
      recordCursor->paletteAssetSavedIdOrOffset = 0;
      break;
    case RESOURCE_DOMAIN_EFFECT_RUNTIME:
      rebasedOffset = rebasedOffset - (int)g_EffectRuntimeRebaseBaseMinusOne;
      recordCursor->textureSetSavedIdOrOffset = 0;
      recordCursor->paletteAssetSavedIdOrOffset = 0;
    }
    recordCursor->runtimePayloadSavedOffset = rebasedOffset;
    /* the sprite asset pointer is replaced by the id stored at +0xB8 of the asset */
    recordCursor->spriteAssetSavedIdOrOffset = *(uint32_t *)(recordCursor->spriteAssetSavedIdOrOffset + 0xb8);
    recordCursor = recordCursor + 1;
    recordsRemainingOrCount--;
  } while (recordsRemainingOrCount != 0);
  tailRecord = runtimeImage->tailRecordD8;
  recordCursor = runtimeImage->records58;
  if (tailRecord != NULL) {
    tailRecord = (ResourceRegistrationRecord100 *)((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne);
  }
  recordsRemainingOrCount = runtimeImage->recordCountAC;
  recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets13[0xc] = (uint32_t)tailRecord;
  return ((uint64_t)(uint32_t)(uintptr_t)recordCursor << 32) | (uint32_t)(recordsRemainingOrCount * 0x100);
}

/* Address: 0x00513020.
   Save-game preparation of the faction runtime image (the "daten.hex" entry): for each of the 8 faction
   records the army asset pointers are replaced by the asset ids (+8 of each asset) and the 8x32 group member
   pointers by saved army-slot offsets. Returns the image with its byte size 0x3A20 for Package_UpsertEntry;
   GameFactionRuntime_RebaseLoadedArmyReferences undoes it. Called directly by the save-game writer
   InGameUiAction1210_ResourceRegistrationHelper.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain0Pair(void)

{
  ArmyRuntimeSlot *runtimeMember;
  int factionRecordsRemaining;
  FactionArmyAssetCount armyAssetPointersRemaining;
  FactionArmyAssetCount primaryArmyAssetPointersRemaining;
  int runtimeMembersRemaining;
  GameFactionRuntimeImage *factionRecordCursor;
  uint32_t *armyAssetPointerCursor;
  uint32_t *primaryArmyAssetPointerCursor;
  ArmyRuntimeSlot **runtimeMemberCursor;

  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionRecordsRemaining = 8;
  do {
    armyAssetPointerCursor = factionRecordCursor->records[0].secondaryArmyAssetPointersOrIds;
    for (armyAssetPointersRemaining = factionRecordCursor->records[0].secondaryArmyAssetCount;
        armyAssetPointersRemaining != 0; armyAssetPointersRemaining--) {
      *armyAssetPointerCursor = *(uint32_t *)(*armyAssetPointerCursor + 8);
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
    }
    primaryArmyAssetPointerCursor = factionRecordCursor->records[0].primaryArmyAssetPointersOrIds;
    for (primaryArmyAssetPointersRemaining = factionRecordCursor->records[0].primaryArmyAssetCount;
        primaryArmyAssetPointersRemaining != 0; primaryArmyAssetPointersRemaining--) {
      *primaryArmyAssetPointerCursor = *(uint32_t *)(*primaryArmyAssetPointerCursor + 8);
      primaryArmyAssetPointerCursor = primaryArmyAssetPointerCursor + 1;
    }
    runtimeMemberCursor = factionRecordCursor->records[0].runtimeGroupMembers8x32;
    runtimeMembersRemaining = 0x100;
    do {
      runtimeMember = *runtimeMemberCursor;
      if (runtimeMember != NULL) {
        runtimeMember =
             (ArmyRuntimeSlot *)((int)runtimeMember - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      *runtimeMemberCursor = runtimeMember;
      runtimeMemberCursor = runtimeMemberCursor + 1;
      runtimeMembersRemaining--;
    } while (runtimeMembersRemaining != 0);
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    factionRecordsRemaining--;
  } while (factionRecordsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)&g_GameFactionRuntimeImage << 32) | 0x3a20;
}

/* Address: 0x0051E2B0.
   Save-game preparation of the 0x1000 effect runtime slots (the "effect.hex" entry): in every used slot the
   model node and owner pointers become saved offsets (the owner is a model node or an army slot depending on
   the completion action) and the definition pointer becomes the definition id; free slots are zeroed.
   Returns the slot array with its byte size 0x40000; EffectRuntime_RebaseSlotsAfterLoad undoes it. Called
   directly by the save-game writer InGameUiAction1210_ResourceRegistrationHelper.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain1Pair(void)

{
  EffectRuntimeCompletionAction slotCompletionAction;
  EffectDefinitionReferenceOrSavedId4 serializedDefinitionId;
  int clearDwordsRemaining;
  int runtimeSlotsRemaining;
  ModelRuntimeNode *ownerModelNode;
  EffectRuntimeSlot *runtimeSlotCursor;
  EffectRuntimeSlot *effectRuntimeSlotsBase;
  
  runtimeSlotsRemaining = EFFECT_RUNTIME_SLOT_COUNT;
  runtimeSlotCursor = g_EffectRuntimeSlots;
  do {
    while( true ) {
      slotCompletionAction = runtimeSlotCursor->completionAction;
      ownerModelNode =
           (runtimeSlotCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
      if ((runtimeSlotCursor->modelNodeOrSavedOffset).modelNode != NULL) break;
      /* free slot: zero its 0x10 dwords, which also advances runtimeSlotCursor to the next slot */
      for (clearDwordsRemaining = 0x10; effectRuntimeSlotsBase = g_EffectRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining--) {
        (runtimeSlotCursor->definitionOrSavedId).definition = NULL;
        runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
      }
      runtimeSlotsRemaining--;
      if (runtimeSlotsRemaining == 0) {
        /* NOT [slot0 + 0x3C] only on this exit (last slot free); EffectRuntime_RebaseSlotsAfterLoad
           inverts it on every load */
        g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
        return ((uint64_t)(uint32_t)(uintptr_t)effectRuntimeSlotsBase << 32) | 0x40000;
      }
    }
    if (ownerModelNode != NULL) {
      if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
        ownerModelNode = (ModelRuntimeNode *)((int)ownerModelNode - g_ModelRuntimeRebaseDelta);
      }
      else if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
        ownerModelNode =
             (ModelRuntimeNode *)((int)ownerModelNode - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    (runtimeSlotCursor->modelNodeOrSavedOffset).modelNode =
         (ModelRuntimeNode *)
         ((int)(runtimeSlotCursor->modelNodeOrSavedOffset).modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeSlotCursor->completionAction = slotCompletionAction;
    serializedDefinitionId = THANDOR_BITCAST(PckEffectDefinitionIdCatalog, EffectDefinitionReferenceOrSavedId4, ((runtimeSlotCursor->definitionOrSavedId).definition)->definitionId);
    (runtimeSlotCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode =
         ownerModelNode;
    runtimeSlotCursor->definitionOrSavedId = serializedDefinitionId;
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining--;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_EffectRuntimeSlots << 32) | 0x40000;
}

/* Address: 0x0052B6D0.
   Save-game preparation of the 0x1000 shot runtime slots (the "shot.hex" entry): in every used slot the model
   node, runtime state and owner army pointers become saved offsets and the definition pointer becomes the
   definition id; free slots are zeroed. Returns the slot array with its byte size 0x40000;
   ShotRuntime_RebaseSlotsAfterLoad undoes it. Called directly by the save-game writer
   InGameUiAction1210_ResourceRegistrationHelper.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl ResourceRegistration_QueryDomain2Pair(void)

{
  ShotDefinitionReferenceOrSavedId4 serializedDefinitionId;
  void *runtimeStateRef;
  int clearDwordsRemaining;
  int runtimeSlotsRemaining;
  ArmyRuntimeSlot *ownerArmyRuntime;
  ShotRuntimeSlot *runtimeSlotCursor;
  uint32_t *terminalToggleField;
  ShotRuntimeSlot *shotRuntimeSlotsBase;
  
  runtimeSlotsRemaining = SHOT_RUNTIME_SLOT_COUNT;
  runtimeSlotCursor = g_ShotRuntimeSlots;
  do {
    while( true ) {
      runtimeStateRef = (runtimeSlotCursor->runtimeStateOrSavedOffset).runtimeStatePointer;
      ownerArmyRuntime = (runtimeSlotCursor->ownerAndTrajectory).ownerArmyRuntime;
      if ((runtimeSlotCursor->modelNodeOrSavedOffset).modelNode != NULL) break;
      /* free slot: zero its 0x10 dwords, which also advances runtimeSlotCursor to the next slot */
      for (clearDwordsRemaining = 0x10; shotRuntimeSlotsBase = g_ShotRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining--) {
        (runtimeSlotCursor->definitionOrSavedId).definition = NULL;
        runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
      }
      runtimeSlotsRemaining--;
      if (runtimeSlotsRemaining == 0) {
        /* NOT [slot0 + 0x3C] only on this exit (last slot free); ShotRuntime_RebaseSlotsAfterLoad
           inverts it on every load */
        terminalToggleField =
             &(g_ShotRuntimeSlots->ownerAndTrajectory).secondaryEffectCountdownTicks;
        *terminalToggleField = ~*terminalToggleField;
        return ((uint64_t)(uint32_t)(uintptr_t)shotRuntimeSlotsBase << 32) | 0x40000;
      }
    }
    if (runtimeStateRef != NULL) {
      runtimeStateRef = (void *)((int)runtimeStateRef - g_ModelRuntimeRebaseDelta);
    }
    if (ownerArmyRuntime != NULL) {
      ownerArmyRuntime =
           (ArmyRuntimeSlot *)((int)ownerArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    (runtimeSlotCursor->modelNodeOrSavedOffset).modelNode =
         (ModelRuntimeNode *)
         ((int)(runtimeSlotCursor->modelNodeOrSavedOffset).modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    (runtimeSlotCursor->runtimeStateOrSavedOffset).runtimeStatePointer = runtimeStateRef;
    serializedDefinitionId = THANDOR_BITCAST(PckShotDefinitionIdCatalog, ShotDefinitionReferenceOrSavedId4, ((runtimeSlotCursor->definitionOrSavedId).definition)->definitionId);
    (runtimeSlotCursor->ownerAndTrajectory).ownerArmyRuntime = ownerArmyRuntime;
    runtimeSlotCursor->definitionOrSavedId = serializedDefinitionId;
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining--;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_ShotRuntimeSlots << 32) | 0x40000;
}

/* Address: 0x00532B00.
   Before the level image is saved: stores the current camera (orientation as magnitude plus packed
   heading/pitch, and position) into the start-camera fields of the level player slot selected by
   runtimeImage->levelRuntimeRecordIndex50, so a loaded game starts with the camera where it was.
   Called directly by the save-game writer InGameUiAction1210_ResourceRegistrationHelper.
*/

void ResourceRegistration_ResolveRuntimeRecord(ResourceRegistrationRuntimeImage *runtimeImage)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorageView800 *levelConditionStorage;
  WorldVector1EaxEcxEdx12 cameraOrientation;
  WorldVector0EaxEcxEdx12 cameraPosition;
  
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets
          [runtimeImage->levelRuntimeRecordIndex50 - 1];
  cameraOrientation = WorldRuntime_GetVector1Regs((WorldRuntimeContext *)runtimeImage);
  *(UQ12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraMagnitudeQ12 + playerSlotByteOffset) =
       cameraOrientation.magnitudeQ12;
  *(AngleTurn32 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].packedHeadingLow16PitchHigh16 + playerSlotByteOffset)
       = cameraOrientation.headingAngle & 0xffff | cameraOrientation.pitchAngle << 16;
  cameraPosition = WorldRuntime_GetVector0Regs((WorldRuntimeContext *)runtimeImage);
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraXQ12 + playerSlotByteOffset) = cameraPosition.xQ12;
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraYQ12 + playerSlotByteOffset) = cameraPosition.yQ12;
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraZQ12 + playerSlotByteOffset) = cameraPosition.zQ12;
}

