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
   package scratch buffer, writes it as the whole file and mounts it. Returns true and the mounted package's
   file handle in *outHandle, or false (outHandle untouched) when Package_Mount fails.
   Called directly by the save-game writer InGameSaveGame_WritePackage, which creates the
   save directory and retries when it fails.
*/
bool InGameSaveGame_CreatePackage(void *packagePath,EngineFileHandle *outHandle)

{
  uint8_t *header;
  uint32_t packedTimeOrDate;
  int clearDwordsRemaining;
  uint8_t *clearCursor;
  EngineFileHandle mountedHandle;

  header = g_PackageScratchBuffer;
  clearCursor = g_PackageScratchBuffer;
  for (clearDwordsRemaining = PCK_ENTRY_HEADER_BYTES / 4; clearDwordsRemaining != 0; clearDwordsRemaining--) {
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
  header[11] = 0;
  header[12] = 0;
  header[13] = 0;
  header[14] = 1;
  header[15] = 0;
  /* three time/date pairs all set to now */
  packedTimeOrDate = g_LocaleGetPackedCurrentTime();
  ((PckArchiveHeader *)header)->timeValue0 = packedTimeOrDate;
  ((PckArchiveHeader *)header)->timeValue1 = packedTimeOrDate;
  ((PckArchiveHeader *)header)->timeValue2 = packedTimeOrDate;
  packedTimeOrDate = g_LocaleGetPackedCurrentDate();
  ((PckArchiveHeader *)header)->dateValue0 = packedTimeOrDate;
  ((PckArchiveHeader *)header)->dateValue1 = packedTimeOrDate;
  ((PckArchiveHeader *)header)->dateValue2 = packedTimeOrDate;
  g_LocaleCopyDefaultComputerLabelUtf16(((PckArchiveHeader *)header)->producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(((PckArchiveHeader *)header)->sourceName);
  header[256] = 0; /* unusedText: empty */
  /* +0xB0 entryCount = 0 */
  header[176] = 0;
  header[177] = 0;
  header[178] = 0;
  header[179] = 0;
  FileSystem_WriteBufferToPath(PCK_ENTRY_HEADER_BYTES,header,packagePath);
  if (!Package_Mount(packagePath,&mountedHandle)) {
    return false;
  }
  *outHandle = mountedHandle;
  return true;
}


/* Address: 0x0040F000.
   Loads a whole resource into a fresh arena buffer. A mounted package entry is decoded into the buffer;
   otherwise the loose file is read, first from the executable's directory, then from the path as given.
   Returns true with the buffer in *outBuffer and its byte count in *outByteCount. On failure returns false
   with the file-system, decoder or out-of-memory code in *outErrorCode and leaves *outBuffer and
   *outByteCount unchanged. outByteCount and outErrorCode may be NULL.
*/
bool Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode)

{
  PckEntryHeader *entry;
  uint32_t fileSize;
  uint32_t errorCode;
  void *fileHandle;
  void *buffer;
  bool gotSize;
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
   Called directly by the save-game writer InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX, and the
   C caller splits the qword the same way, so the swap is internal to the C.
*/
ResourceRegistrationImagePair
InGameSaveGame_PrepareRegistrationRecords
          (ResourceRegistrationRuntimeImageSavedView *runtimeImage)

{
  uint32_t rebasedOffset;
  uint32_t secondaryOffsetOrNestedCount;
  int clearCountOrArmyDefinition;
  ResourceRegistrationRecord *tailRecord;
  uint32_t recordsRemainingOrCount;
  uint32_t nestedBaseOffset;
  ResourceRegistrationRecordSavedView *nestedOffsetCursor;
  ResourceRegistrationRecordSavedView *recordCursor;
  
  recordCursor = runtimeImage->records;
  recordsRemainingOrCount = runtimeImage->recordCount;
  do {
    while ((recordCursor->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
      /* free record: zero its 0x40 dwords, which also advances recordCursor to the next record */
      for (clearCountOrArmyDefinition = sizeof(ResourceRegistrationRecordSavedView) / 4;
           clearCountOrArmyDefinition != 0; clearCountOrArmyDefinition--) {
        recordCursor->primarySavedIdOrOffset = 0;
        recordCursor = (ResourceRegistrationRecordSavedView *)((uint32_t *)recordCursor + 1);
      }
      recordsRemainingOrCount--;
      if (recordsRemainingOrCount == 0) {
        tailRecord = runtimeImage->tailRecord;
        recordCursor = runtimeImage->records;
        if (tailRecord != NULL) {
          tailRecord = (ResourceRegistrationRecord *)
                   ((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        recordsRemainingOrCount = runtimeImage->recordCount;
        /* the saved tail-record offset goes into the last dword of the image (record array + size - 4) */
        recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets[12] = (uint32_t)tailRecord;
        return ((uint64_t)(uint32_t)(uintptr_t)recordCursor << 32) |
               (uint32_t)(recordsRemainingOrCount * sizeof(ResourceRegistrationRecordSavedView));
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
    secondaryOffsetOrNestedCount = recordCursor->nestedCount;
    if (rebasedOffset != 0) {
      rebasedOffset = rebasedOffset - THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1); /* 1-based offset, 0 = none */
    }
    recordCursor->auxiliarySavedIdOrOffset = rebasedOffset;
    nestedOffsetCursor = recordCursor;
    for (; secondaryOffsetOrNestedCount != 0; secondaryOffsetOrNestedCount--) {
      if (nestedOffsetCursor->nestedSavedOffsets[0] != 0) {
        nestedOffsetCursor->nestedSavedOffsets[0] =
             nestedOffsetCursor->nestedSavedOffsets[0] - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      /* next dword */
      nestedOffsetCursor = (ResourceRegistrationRecordSavedView *)((uint32_t *)nestedOffsetCursor + 1);
    }
    rebasedOffset = recordCursor->runtimePayloadSavedOffset;
                    
    switch(recordCursor->domainIndex) {
    case RESOURCE_DOMAIN_ARMY_RUNTIME:
      /* the payload is a model runtime: the faction of its owner army is saved as the texture set
         (army graphics binding) index */
      clearCountOrArmyDefinition =
           (int)((ModelRuntimeSlot *)rebasedOffset)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      recordCursor->paletteAssetSavedIdOrOffset = 0;
      rebasedOffset = rebasedOffset - g_ModelRuntimeRebaseDelta;
      recordCursor->textureSetSavedIdOrOffset = ((ArmyRuntimeSlot *)clearCountOrArmyDefinition)->factionIndex;
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
    /* the sprite asset pointer is replaced by the asset's registry id */
    recordCursor->spriteAssetSavedIdOrOffset =
         ((SpriteAssetHeader *)recordCursor->spriteAssetSavedIdOrOffset)->registryHeader.registryId;
    recordCursor = recordCursor + 1;
    recordsRemainingOrCount--;
  } while (recordsRemainingOrCount != 0);
  tailRecord = runtimeImage->tailRecord;
  recordCursor = runtimeImage->records;
  if (tailRecord != NULL) {
    tailRecord = (ResourceRegistrationRecord *)((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne);
  }
  recordsRemainingOrCount = runtimeImage->recordCount;
  recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets[12] = (uint32_t)tailRecord;
  return ((uint64_t)(uint32_t)(uintptr_t)recordCursor << 32) |
               (uint32_t)(recordsRemainingOrCount * sizeof(ResourceRegistrationRecordSavedView));
}

/* Address: 0x00513020.
   Save-game preparation of the faction runtime image (the "daten.hex" entry): for each of the 8 faction
   records the army asset pointers are replaced by the asset ids (+8 of each asset) and the 8x32 group member
   pointers by saved army-slot offsets. Returns the image with its byte size 0x3A20 for Package_UpsertEntry;
   GameFactionRuntime_RebaseLoadedArmyReferences undoes it. Called directly by the save-game writer
   InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareFactionImage(void)

{
  ArmyRuntimeSlot *runtimeMember;
  int factionRecordsRemaining;
  FactionArmyAssetCount armyAssetPointersRemaining;
  FactionArmyAssetCount primaryArmyAssetPointersRemaining;
  int runtimeMembersRemaining;
  GameFactionRuntimeRecord *factionRecordCursor;
  uint32_t *armyAssetPointerCursor;
  uint32_t *primaryArmyAssetPointerCursor;
  ArmyRuntimeSlot **runtimeMemberCursor;

  factionRecordCursor = g_GameFactionRuntimeImage.records;
  factionRecordsRemaining = 8;
  do {
    armyAssetPointerCursor = factionRecordCursor->secondaryArmyAssetPointersOrIds;
    for (armyAssetPointersRemaining = factionRecordCursor->secondaryArmyAssetCount;
        armyAssetPointersRemaining != 0; armyAssetPointersRemaining--) {
      *armyAssetPointerCursor = ((ArmyAssetRecordPrefix *)*armyAssetPointerCursor)->registryId;
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
    }
    primaryArmyAssetPointerCursor = factionRecordCursor->primaryArmyAssetPointersOrIds;
    for (primaryArmyAssetPointersRemaining = factionRecordCursor->primaryArmyAssetCount;
        primaryArmyAssetPointersRemaining != 0; primaryArmyAssetPointersRemaining--) {
      *primaryArmyAssetPointerCursor = ((ArmyAssetRecordPrefix *)*primaryArmyAssetPointerCursor)->registryId;
      primaryArmyAssetPointerCursor = primaryArmyAssetPointerCursor + 1;
    }
    runtimeMemberCursor = factionRecordCursor->runtimeGroupMembers8x32;
    runtimeMembersRemaining = 256;
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
    factionRecordCursor++;
    factionRecordsRemaining--;
  } while (factionRecordsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)&g_GameFactionRuntimeImage << 32) | sizeof(GameFactionRuntimeImage);
}

/* Address: 0x0051E2B0.
   Save-game preparation of the 0x1000 effect runtime slots (the "effect.hex" entry): in every used slot the
   model node and owner pointers become saved offsets (the owner is a model node or an army slot depending on
   the completion action) and the definition pointer becomes the definition id; free slots are zeroed.
   Returns the slot array with its byte size 0x40000; EffectRuntime_RebaseSlotsAfterLoad undoes it. Called
   directly by the save-game writer InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareEffectSlots(void)

{
  EffectRuntimeCompletionAction slotCompletionAction;
  EffectDefinitionReferenceOrSavedId serializedDefinitionId;
  int clearDwordsRemaining;
  int runtimeSlotsRemaining;
  ModelRuntimeNode *ownerModelNode;
  EffectRuntimeSlot *runtimeSlotCursor;
  EffectRuntimeSlot *effectRuntimeSlotsBase;
  
  runtimeSlotsRemaining = EFFECT_RUNTIME_SLOT_COUNT;
  runtimeSlotCursor = g_EffectRuntimeSlots;
  do {
    while (slotCompletionAction = runtimeSlotCursor->completionAction,
           ownerModelNode =
                runtimeSlotCursor->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode,
           runtimeSlotCursor->modelNodeOrSavedOffset.modelNode == NULL) {
      /* free slot: zero its 0x10 dwords, which also advances runtimeSlotCursor to the next slot */
      for (clearDwordsRemaining = 16; effectRuntimeSlotsBase = g_EffectRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining--) {
        runtimeSlotCursor->definitionOrSavedId.definition = NULL;
        runtimeSlotCursor = (EffectRuntimeSlot *)((uint32_t *)runtimeSlotCursor + 1);
      }
      runtimeSlotsRemaining--;
      if (runtimeSlotsRemaining == 0) {
        /* NOT [slot0 + 0x3C] only on this exit (last slot free); EffectRuntime_RebaseSlotsAfterLoad
           inverts it on every load */
        g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
        return ((uint64_t)(uint32_t)(uintptr_t)effectRuntimeSlotsBase << 32) |
               (EFFECT_RUNTIME_SLOT_COUNT * sizeof(EffectRuntimeSlot));
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
    runtimeSlotCursor->modelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)
         ((int)runtimeSlotCursor->modelNodeOrSavedOffset.modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeSlotCursor->completionAction = slotCompletionAction;
    serializedDefinitionId =
         THANDOR_BITCAST(PckEffectDefinitionIdCatalog, EffectDefinitionReferenceOrSavedId,
                         runtimeSlotCursor->definitionOrSavedId.definition->definitionId);
    runtimeSlotCursor->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode =
         ownerModelNode;
    runtimeSlotCursor->definitionOrSavedId = serializedDefinitionId;
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining--;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_EffectRuntimeSlots << 32) |
         (EFFECT_RUNTIME_SLOT_COUNT * sizeof(EffectRuntimeSlot));
}

/* Address: 0x0052B6D0.
   Save-game preparation of the 0x1000 shot runtime slots (the "shot.hex" entry): in every used slot the model
   node, runtime state and owner army pointers become saved offsets and the definition pointer becomes the
   definition id; free slots are zeroed. Returns the slot array with its byte size 0x40000;
   ShotRuntime_RebaseSlotsAfterLoad undoes it. Called directly by the save-game writer
   InGameSaveGame_WritePackage.
   Return value: (base << 32) | byteSize; the original returns the base in EAX and the size in EDX.
*/
ResourceRegistrationImagePair __cdecl InGameSaveGame_PrepareShotSlots(void)

{
  ShotDefinitionReferenceOrSavedId serializedDefinitionId;
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
    while (runtimeStateRef = runtimeSlotCursor->runtimeStateOrSavedOffset.runtimeStatePointer,
           ownerArmyRuntime = runtimeSlotCursor->ownerAndTrajectory.ownerArmyRuntime,
           runtimeSlotCursor->modelNodeOrSavedOffset.modelNode == NULL) {
      /* free slot: zero its 0x10 dwords, which also advances runtimeSlotCursor to the next slot */
      for (clearDwordsRemaining = 16; shotRuntimeSlotsBase = g_ShotRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining--) {
        runtimeSlotCursor->definitionOrSavedId.definition = NULL;
        runtimeSlotCursor = (ShotRuntimeSlot *)((uint32_t *)runtimeSlotCursor + 1);
      }
      runtimeSlotsRemaining--;
      if (runtimeSlotsRemaining == 0) {
        /* NOT [slot0 + 0x3C] only on this exit (last slot free); ShotRuntime_RebaseSlotsAfterLoad
           inverts it on every load */
        terminalToggleField =
             &g_ShotRuntimeSlots->ownerAndTrajectory.secondaryEffectCountdownTicks;
        *terminalToggleField = ~*terminalToggleField;
        return ((uint64_t)(uint32_t)(uintptr_t)shotRuntimeSlotsBase << 32) | SHOT_RUNTIME_POOL_BYTES;
      }
    }
    if (runtimeStateRef != NULL) {
      runtimeStateRef = (void *)((int)runtimeStateRef - g_ModelRuntimeRebaseDelta);
    }
    if (ownerArmyRuntime != NULL) {
      ownerArmyRuntime =
           (ArmyRuntimeSlot *)((int)ownerArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    runtimeSlotCursor->modelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)
         ((int)runtimeSlotCursor->modelNodeOrSavedOffset.modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeSlotCursor->runtimeStateOrSavedOffset.runtimeStatePointer = runtimeStateRef;
    serializedDefinitionId =
         THANDOR_BITCAST(PckShotDefinitionIdCatalog, ShotDefinitionReferenceOrSavedId,
                         runtimeSlotCursor->definitionOrSavedId.definition->definitionId);
    runtimeSlotCursor->ownerAndTrajectory.ownerArmyRuntime = ownerArmyRuntime;
    runtimeSlotCursor->definitionOrSavedId = serializedDefinitionId;
    runtimeSlotCursor = runtimeSlotCursor + 1;
    runtimeSlotsRemaining--;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_ShotRuntimeSlots << 32) | SHOT_RUNTIME_POOL_BYTES;
}

/* Address: 0x00532B00.
   Before the level image is saved: stores the current camera (orientation as magnitude plus packed
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

