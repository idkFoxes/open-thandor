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
   Cross-module calls: FileSystem_WriteBufferToPathCf [platform/filesystem/win32], Package_Mount
   [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ResourceRegistration_OpenSourceCf(void *packagePath)

{
  byte *source;
  dword packedTimeOrDate;
  int clearDwordsRemaining;
  byte *clearCursor;
  StatusValueEaxCf5 mountResult;
  
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
  packedTimeOrDate = (*g_LocaleGetPackedCurrentTime)();
  *(dword *)(source + 0x10) = packedTimeOrDate;
  *(dword *)(source + 0x18) = packedTimeOrDate;
  *(dword *)(source + 0x20) = packedTimeOrDate;
  packedTimeOrDate = (*g_LocaleGetPackedCurrentDate)();
  *(dword *)(source + 0x14) = packedTimeOrDate;
  *(dword *)(source + 0x1c) = packedTimeOrDate;
  *(dword *)(source + 0x24) = packedTimeOrDate;
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(source + 0x30));
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(source + 0x70));
  source[0x100] = 0;
  source[0xb0] = 0;
  source[0xb1] = 0;
  source[0xb2] = 0;
  source[0xb3] = 0;
  FileSystem_WriteBufferToPathCf(0x200,source,packagePath);
  mountResult = Package_Mount(packagePath);
  return mountResult;
}


/* Address: 0x0040F000.
   Ownership: assets/resource/runtime.
   Purpose: Handles resource load.
   Cross-module calls: Package_FindEntryAcrossMounts [assets/package/runtime], WidePath_CombineDirectoryAndLeaf
   [core/text/path], Package_DecodeEntryInto [assets/package/runtime].
*/
ResourceLoadEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx Resource_Load(word *path)

{
  PckEntryHeader *entry;
  byte *bytes;
  byte *sizeOrFailureCode;
  byte *in_ECX = (byte *)0; /* ECX is only meaningful on success (byte count); callers test CF */
  ArenaAllocEaxCf5 allocResult;
  PackageDecodeEaxCf5 decodeResult;
  FileSystemOpenEaxCf5 openResult;
  FileSystemSizeEaxCf5 sizeResult;
  FileSystemReadEaxCf5 readResult;
  PackageFindEntryEaxEbxCf9 findResult;
  ResourceLoadEaxEcxCf9 fileOrPackageResult;
  ResourceLoadEaxEcxCf9 fileLoadResult;
  ResourceLoadEaxEcxCf9 failureResult;
  
  findResult = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)findResult.eax;
  if (findResult.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    openResult = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    fileOrPackageResult.eax = (byte *)openResult.eax;
    if (openResult.carry) {
      openResult = (*g_FileSystemOpenCf)(0,path);
      fileOrPackageResult.eax = (byte *)openResult.eax;
      if (openResult.carry) goto Resource_Load_ReturnOpenAllocationOrDecodeResult;
    }
    sizeResult = (*g_FileSystemGetSizeCf)(fileOrPackageResult.eax);
    bytes = (byte *)sizeResult.eax;
    sizeOrFailureCode = bytes;
    if (!sizeResult.carry) {
      allocResult = (*g_MemoryApi.alloc)((dword)bytes);
      fileLoadResult.eax = (void *)allocResult.eax;
      in_ECX = bytes;
      if (allocResult.carry) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
        sizeOrFailureCode = (byte *)0x5;
      }
      else {
        readResult = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,fileLoadResult.eax,fileOrPackageResult.eax);
        sizeOrFailureCode = (byte *)readResult.eax;
        if (!readResult.carry) {
          (*g_FileSystemClose)(fileOrPackageResult.eax);
          fileLoadResult.ecx = (dword)bytes;
          fileLoadResult.carry = false;
          return fileLoadResult;
        }
        (*g_MemoryApi.free)(fileLoadResult.eax);
      }
    }
    (*g_FileSystemClose)(fileOrPackageResult.eax);
    fileOrPackageResult.eax = sizeOrFailureCode;
  }
  else {
    fileOrPackageResult.eax = (byte *)0x5;
    if (entry->packedSize < 0x800001) {
      allocResult = (*g_MemoryApi.alloc)(entry->unpackedSize);
      fileOrPackageResult.eax = (byte *)allocResult.eax;
      if (!allocResult.carry) {
        decodeResult = Package_DecodeEntryInto(fileOrPackageResult.eax,entry,findResult.ebx);
        if (!decodeResult.carry) {
          fileOrPackageResult.ecx = entry->unpackedSize;
          fileOrPackageResult.carry = false;
          return fileOrPackageResult;
        }
        sizeOrFailureCode = (byte *)decodeResult.eax;
        (*g_MemoryApi.free)(fileOrPackageResult.eax);
        fileOrPackageResult.eax = sizeOrFailureCode;
      }
    }
  }
Resource_Load_ReturnOpenAllocationOrDecodeResult:
  failureResult.ecx = (dword)in_ECX;
  failureResult.eax = (dword)fileOrPackageResult.eax;
  failureResult.carry = true;
  return failureResult;
}


/* Address: 0x0040F1D0.
   Ownership: assets/resource/runtime.
   Purpose: Handles resource release.
*/
void __thandor_void_preserve_eax_ecx_edx Resource_Release(void *allocation)

{
  (*g_MemoryApi.free)(allocation);
  return;
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
  dword rebasedOffset;
  dword secondaryOffsetOrNestedCount;
  int clearCountOrArmyDefinition;
  ResourceRegistrationRecord100 *tailRecord;
  dword recordsRemainingOrCount;
  dword nestedBaseOffset;
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
        recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets13[0xc] = (dword)tailRecord;
        return CONCAT44(recordCursor,recordsRemainingOrCount * 0x100);
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
      recordCursor->textureSetSavedIdOrOffset = *(dword *)(clearCountOrArmyDefinition + 0xc);
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
    recordCursor->spriteAssetSavedIdOrOffset = *(dword *)(recordCursor->spriteAssetSavedIdOrOffset + 0xb8);
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
  recordCursor[recordsRemainingOrCount - 1].nestedSavedOffsets13[0xc] = (dword)tailRecord;
  return CONCAT44(recordCursor,recordsRemainingOrCount * 0x100);
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
  dword *armyAssetPointerCursor;
  dword *primaryArmyAssetPointerCursor;
  ArmyRuntimeSlot **runtimeMemberCursor;
  
  factionRecordCursor = &g_GameFactionRuntimeImage;
  factionRecordsRemaining = 8;
  do {
    armyAssetPointerCursor = factionRecordCursor->records[0].secondaryArmyAssetPointersOrIds;
    for (armyAssetPointersRemaining = factionRecordCursor->records[0].secondaryArmyAssetCount;
        armyAssetPointersRemaining != 0; armyAssetPointersRemaining = armyAssetPointersRemaining - 1
        ) {
      *armyAssetPointerCursor = *(dword *)(*armyAssetPointerCursor + 8);
      armyAssetPointerCursor = armyAssetPointerCursor + 1;
    }
    primaryArmyAssetPointerCursor = factionRecordCursor->records[0].primaryArmyAssetPointersOrIds;
    for (primaryArmyAssetPointersRemaining = factionRecordCursor->records[0].primaryArmyAssetCount;
        primaryArmyAssetPointersRemaining != 0;
        primaryArmyAssetPointersRemaining = primaryArmyAssetPointersRemaining - 1) {
      *primaryArmyAssetPointerCursor = *(dword *)(*primaryArmyAssetPointerCursor + 8);
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
  return ((qword)(dword)(uintptr_t)&g_GameFactionRuntimeImage << 32) | 0x3a20;
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
        return CONCAT44(effectRuntimeSlotsBase,0x40000);
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
  return CONCAT44(g_EffectRuntimeSlots,0x40000);
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
  dword *terminalToggleField;
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
        return CONCAT44(shotRuntimeSlotsBase,0x40000);
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
  return CONCAT44(g_ShotRuntimeSlots,0x40000);
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

