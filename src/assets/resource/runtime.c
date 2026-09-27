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
   Ownership: assets/resource/runtime.
   Purpose: Carry-flag success/failure semantics are preserved in the comment rather than fabricated as an ordinary
   return.
   Cross-module calls: FileSystem_WriteBufferToPath [platform/filesystem/win32], Package_Mount
   [assets/package/runtime].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
ResourceRegistration_OpenSource(void *packagePath)

{
  uint8_t *source;
  uint32_t packedTimeOrDate;
  int clearDwordsRemaining;
  uint8_t *clearCursor;
  StatusResult mountResult;
  
  source = g_PackageScratchBuffer;
  clearCursor = g_PackageScratchBuffer;
  for (clearDwordsRemaining = 0x80; clearDwordsRemaining != 0;
      clearDwordsRemaining = clearDwordsRemaining + -1) {
    clearCursor[0] = 0;
    clearCursor[1] = 0;
    clearCursor[2] = 0;
    clearCursor[3] = 0;
    clearCursor = clearCursor + 4;
  }
  source[0] = 0x70;
  source[1] = 99;
  source[2] = 0x6b;
  source[3] = 0;
  source[4] = 0;
  source[5] = 2;
  source[6] = 0;
  source[7] = 0;
  source[8] = 1;
  source[9] = 0;
  source[10] = 0;
  source[0xb] = 0;
  source[0xc] = 0;
  source[0xd] = 0;
  source[0xe] = 1;
  source[0xf] = 0;
  packedTimeOrDate = g_LocaleGetPackedCurrentTime();
  *(uint32_t *)(source + 0x10) = packedTimeOrDate;
  *(uint32_t *)(source + 0x18) = packedTimeOrDate;
  *(uint32_t *)(source + 0x20) = packedTimeOrDate;
  packedTimeOrDate = g_LocaleGetPackedCurrentDate();
  *(uint32_t *)(source + 0x14) = packedTimeOrDate;
  *(uint32_t *)(source + 0x1c) = packedTimeOrDate;
  *(uint32_t *)(source + 0x24) = packedTimeOrDate;
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(source + 0x30));
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(source + 0x70));
  source[0x100] = 0;
  source[0xb0] = 0;
  source[0xb1] = 0;
  source[0xb2] = 0;
  source[0xb3] = 0;
  FileSystem_WriteBufferToPath(0x200,source,packagePath);
  mountResult = Package_Mount(packagePath);
  return mountResult;
}


/* Address: 0x0040F000.
   Loads a whole resource into a fresh arena buffer and returns it with its byte count. A mounted package
   entry is decoded into the buffer; otherwise the loose file is read, first from the executable's directory,
   then from the path as given. On failure CF is set and EAX carries the file-system or out-of-memory code.
*/
ResourceLoadResult __thandor_eax_ecx_cf_preserve_edx Resource_Load(uint16_t *path)

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
void __thandor_void_preserve_eax_ecx_edx Resource_Release(void *resourceBuffer)

{
  g_MemoryApi.free(resourceBuffer);
}


/* Address: 0x0050E890.
   Ownership: assets/resource/runtime.
   Purpose: The verified EDX:EAX register pair is represented by an opaque eight-byte nominal return. EDX:EAX pair
   ABI: EAX is the runtime-image base pointer and EDX is the byte length; the nominal qword preserves the register
   return and avoids a hidden structure-return pointer. Native register-pair ABI: EAX carries
   ResourceRegistrationRuntimeImage* and EDX carries byteLength. The nominal qword return is retained to avoid a
   hidden structure-return pointer; CUSTOM_STORAGE binds the two physical registers. Domain indices 0, 1, and 2
   correspond to army/model, shot, and effect runtime image pairs.
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
      for (clearCountOrArmyDefinition = 0x40; clearCountOrArmyDefinition != 0; clearCountOrArmyDefinition = clearCountOrArmyDefinition + -1) {
        recordCursor->primarySavedIdOrOffset = 0;
        recordCursor = (ResourceRegistrationRecordSerializedScalarView100 *)
                 &recordCursor->secondarySavedIdOrOffset;
      }
      recordsRemainingOrCount = recordsRemainingOrCount - 1;
      if (recordsRemainingOrCount == 0) {
        tailRecord = runtimeImage->tailRecordD8;
        recordCursor = runtimeImage->records58;
        if (tailRecord != (ResourceRegistrationRecord100 *)0x0) {
          tailRecord = (ResourceRegistrationRecord100 *)
                   ((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        recordsRemainingOrCount = runtimeImage->recordCountAC;
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
    for (; secondaryOffsetOrNestedCount != 0; secondaryOffsetOrNestedCount = secondaryOffsetOrNestedCount - 1) {
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
    recordCursor->spriteAssetSavedIdOrOffset = *(uint32_t *)(recordCursor->spriteAssetSavedIdOrOffset + 0xb8);
    recordCursor = recordCursor + 1;
    recordsRemainingOrCount = recordsRemainingOrCount - 1;
  } while (recordsRemainingOrCount != 0);
  tailRecord = runtimeImage->tailRecordD8;
  recordCursor = runtimeImage->records58;
  if (tailRecord != (ResourceRegistrationRecord100 *)0x0) {
    tailRecord = (ResourceRegistrationRecord100 *)((int)tailRecord - (int)g_RuntimeObjectRebaseBaseMinusOne)
    ;
  }
  recordsRemainingOrCount = runtimeImage->recordCountAC;
  recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets13[0xc] = (uint32_t)tailRecord;
  return ((uint64_t)(uint32_t)(uintptr_t)recordCursor << 32) | (uint32_t)(recordsRemainingOrCount * 0x100);
}

/* Address: 0x00513020.
   Ownership: assets/resource/runtime.
   Purpose: The verified EDX:EAX register pair is represented by an opaque eight-byte nominal return. EDX:EAX pair
   ABI: EAX is the runtime-image base pointer and EDX is the byte length; the nominal qword preserves the register
   return and avoids a hidden structure-return pointer. Native register-pair ABI: EAX carries
   ResourceRegistrationRuntimeImage* and EDX carries byteLength. The nominal qword return is retained to avoid a
   hidden structure-return pointer; CUSTOM_STORAGE binds the two physical registers.
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
        armyAssetPointersRemaining != 0; armyAssetPointersRemaining = armyAssetPointersRemaining - 1
        ) {
      *armyAssetPointerCursor = *(uint32_t *)(*armyAssetPointerCursor + 8);
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
    }
    primaryArmyAssetPointerCursor = factionRecordCursor->records[0].primaryArmyAssetPointersOrIds;
    for (primaryArmyAssetPointersRemaining = factionRecordCursor->records[0].primaryArmyAssetCount;
        primaryArmyAssetPointersRemaining != 0;
        primaryArmyAssetPointersRemaining = primaryArmyAssetPointersRemaining - 1) {
      *primaryArmyAssetPointerCursor = *(uint32_t *)(*primaryArmyAssetPointerCursor + 8);
      primaryArmyAssetPointerCursor = primaryArmyAssetPointerCursor + 1;
    }
    runtimeMemberCursor = factionRecordCursor->records[0].runtimeGroupMembers8x32;
    runtimeMembersRemaining = 0x100;
    do {
      runtimeMember = *runtimeMemberCursor;
      if (runtimeMember != (ArmyRuntimeSlot *)0x0) {
        runtimeMember =
             (ArmyRuntimeSlot *)((int)runtimeMember - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      *runtimeMemberCursor = runtimeMember;
      runtimeMemberCursor = runtimeMemberCursor + 1;
      runtimeMembersRemaining = runtimeMembersRemaining + -1;
    } while (runtimeMembersRemaining != 0);
    factionRecordCursor = (GameFactionRuntimeImage *)(factionRecordCursor->records + 1);
    factionRecordsRemaining = factionRecordsRemaining + -1;
  } while (factionRecordsRemaining != 0);
  /* EDX:EAX = faction runtime image, byte size 0x3A20 */
  return ((uint64_t)(uint32_t)(uintptr_t)&g_GameFactionRuntimeImage << 32) | 0x3a20;
}

/* Address: 0x0051E2B0.
   Ownership: assets/resource/runtime.
   Purpose: The verified EDX:EAX register pair is represented by an opaque eight-byte nominal return. EDX:EAX pair
   ABI: EAX is the runtime-image base pointer and EDX is the byte length; the nominal qword preserves the register
   return and avoids a hidden structure-return pointer. Native register-pair ABI: EAX carries
   ResourceRegistrationRuntimeImage* and EDX carries byteLength. The nominal qword return is retained to avoid a
   hidden structure-return pointer; CUSTOM_STORAGE binds the two physical registers. Function-specific scalar
   serialized effect-slot view exposes direct definitionSavedId and modelNodeSavedOffset fields.
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
  
  runtimeSlotsRemaining = 0x1000;
  runtimeSlotCursor = g_EffectRuntimeSlots;
  do {
    while( true ) {
      slotCompletionAction = runtimeSlotCursor->completionAction;
      ownerModelNode =
           (runtimeSlotCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
      if ((runtimeSlotCursor->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) break;
      for (clearDwordsRemaining = 0x10; effectRuntimeSlotsBase = g_EffectRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining = clearDwordsRemaining + -1) {
        (runtimeSlotCursor->definitionOrSavedId).definition = (EffectDefinition *)0x0;
        runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
      }
      runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
      if (runtimeSlotsRemaining == 0) {
        g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
        return ((uint64_t)(uint32_t)(uintptr_t)effectRuntimeSlotsBase << 32) | 0x40000;
      }
    }
    if (ownerModelNode != (ModelRuntimeNode *)0x0) {
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
    runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_EffectRuntimeSlots << 32) | 0x40000;
}

/* Address: 0x0052B6D0.
   Ownership: assets/resource/runtime.
   Purpose: The verified EDX:EAX register pair is represented by an opaque eight-byte nominal return. EDX:EAX pair
   ABI: EAX is the runtime-image base pointer and EDX is the byte length; the nominal qword preserves the register
   return and avoids a hidden structure-return pointer. Native register-pair ABI: EAX carries
   ResourceRegistrationRuntimeImage* and EDX carries byteLength. The nominal qword return is retained to avoid a
   hidden structure-return pointer; CUSTOM_STORAGE binds the two physical registers. Function-specific scalar
   serialized shot-slot view exposes direct definition, model-node, and runtime-state saved fields; live
   runtimeStatePointer facet retained.
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
  
  runtimeSlotsRemaining = 0x1000;
  runtimeSlotCursor = g_ShotRuntimeSlots;
  do {
    while( true ) {
      runtimeStateRef = (runtimeSlotCursor->runtimeStateOrSavedOffset).runtimeStatePointer;
      ownerArmyRuntime = (runtimeSlotCursor->ownerAndTrajectory).ownerArmyRuntime;
      if ((runtimeSlotCursor->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) break;
      for (clearDwordsRemaining = 0x10; shotRuntimeSlotsBase = g_ShotRuntimeSlots,
          clearDwordsRemaining != 0; clearDwordsRemaining = clearDwordsRemaining + -1) {
        (runtimeSlotCursor->definitionOrSavedId).definition = (ShotDefinition *)0x0;
        runtimeSlotCursor = (ShotRuntimeSlot *)&runtimeSlotCursor->launchSpeedQ12;
      }
      runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
      if (runtimeSlotsRemaining == 0) {
        terminalToggleField =
             &(g_ShotRuntimeSlots->ownerAndTrajectory).secondaryEffectCountdownTicks;
        *terminalToggleField = ~*terminalToggleField;
        return ((uint64_t)(uint32_t)(uintptr_t)shotRuntimeSlotsBase << 32) | 0x40000;
      }
    }
    if (runtimeStateRef != (void *)0x0) {
      runtimeStateRef = (void *)((int)runtimeStateRef - g_ModelRuntimeRebaseDelta);
    }
    if (ownerArmyRuntime != (ArmyRuntimeSlot *)0x0) {
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
    runtimeSlotsRemaining = runtimeSlotsRemaining + -1;
  } while (runtimeSlotsRemaining != 0);
  return ((uint64_t)(uint32_t)(uintptr_t)g_ShotRuntimeSlots << 32) | 0x40000;
}

/* Address: 0x00532B00.
   Ownership: assets/resource/runtime.
   Purpose: Handles resource registration resolve runtime record.
   Cross-module calls: WorldRuntime_GetVector1Regs [world/runtime/core], WorldRuntime_GetVector0Regs
   [world/runtime/core].
*/

void __thandor_void_preserve_eax_ecx_edx
ResourceRegistration_ResolveRuntimeRecord(ResourceRegistrationRuntimeImage *runtimeImage)

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
       = cameraOrientation.headingAngle & 0xffff | cameraOrientation.pitchAngle << 0x10;
  cameraPosition = WorldRuntime_GetVector0Regs((WorldRuntimeContext *)runtimeImage);
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraXQ12 + playerSlotByteOffset) = cameraPosition.xQ12;
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraYQ12 + playerSlotByteOffset) = cameraPosition.yQ12;
  *(Q12 *)((int)&(levelConditionStorage->levelImage).playerSlots[0].startCameraZQ12 + playerSlotByteOffset) = cameraPosition.zQ12;
  return;
}

