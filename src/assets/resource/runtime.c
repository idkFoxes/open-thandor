/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/resource/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/resource/runtime.h>

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
  dword dVar1;
  int clearDwordsRemaining;
  byte *clearCursor;
  StatusValueEaxCf5 SVar2;
  
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
  dVar1 = (*g_LocaleGetPackedCurrentTime)();
  *(dword *)(source + 0x10) = dVar1;
  *(dword *)(source + 0x18) = dVar1;
  *(dword *)(source + 0x20) = dVar1;
  dVar1 = (*g_LocaleGetPackedCurrentDate)();
  *(dword *)(source + 0x14) = dVar1;
  *(dword *)(source + 0x1c) = dVar1;
  *(dword *)(source + 0x24) = dVar1;
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(source + 0x30));
  (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(source + 0x70));
  source[0x100] = 0;
  source[0xb0] = 0;
  source[0xb1] = 0;
  source[0xb2] = 0;
  source[0xb3] = 0;
  FileSystem_WriteBufferToPathCf(0x200,source,packagePath);
  SVar2 = Package_Mount(packagePath);
  return SVar2;
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
  byte *pbVar1;
  byte *in_ECX;
  ArenaAllocEaxCf5 AVar2;
  PackageDecodeEaxCf5 PVar3;
  FileSystemOpenEaxCf5 FVar4;
  FileSystemSizeEaxCf5 FVar5;
  FileSystemReadEaxCf5 FVar6;
  PackageFindEntryEaxEbxCf9 PVar7;
  ResourceLoadEaxEcxCf9 RVar8;
  ResourceLoadEaxEcxCf9 RVar9;
  ResourceLoadEaxEcxCf9 RVar10;
  
  PVar7 = Package_FindEntryAcrossMounts(path);
  entry = (PckEntryHeader *)PVar7.eax;
  if (PVar7.carry) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    FVar4 = (*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    RVar8.eax = (byte *)FVar4.eax;
    if (FVar4.carry) {
      FVar4 = (*g_FileSystemOpenCf)(0,path);
      RVar8.eax = (byte *)FVar4.eax;
      if (FVar4.carry) goto Resource_Load_ReturnOpenAllocationOrDecodeResult;
    }
    FVar5 = (*g_FileSystemGetSizeCf)(RVar8.eax);
    bytes = (byte *)FVar5.eax;
    pbVar1 = bytes;
    if (!FVar5.carry) {
      AVar2 = (*g_MemoryApi.alloc)((dword)bytes);
      RVar9.eax = (void *)AVar2.eax;
      in_ECX = bytes;
      if (AVar2.carry) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)bytes,g_FatalErrorDetail1Utf16);
        pbVar1 = (byte *)0x5;
      }
      else {
        FVar6 = (*g_FileSystemReadExactCf)((FileIoByteCount)bytes,RVar9.eax,RVar8.eax);
        pbVar1 = (byte *)FVar6.eax;
        if (!FVar6.carry) {
          (*g_FileSystemClose)(RVar8.eax);
          RVar9.ecx = (dword)bytes;
          RVar9.carry = false;
          return RVar9;
        }
        (*g_MemoryApi.free)(RVar9.eax);
      }
    }
    (*g_FileSystemClose)(RVar8.eax);
    RVar8.eax = pbVar1;
  }
  else {
    RVar8.eax = (byte *)0x5;
    if (entry->packedSize < 0x800001) {
      AVar2 = (*g_MemoryApi.alloc)(entry->unpackedSize);
      RVar8.eax = (byte *)AVar2.eax;
      if (!AVar2.carry) {
        PVar3 = Package_DecodeEntryInto(RVar8.eax,entry,PVar7.ebx);
        if (!PVar3.carry) {
          RVar8.ecx = entry->unpackedSize;
          RVar8.carry = false;
          return RVar8;
        }
        pbVar1 = (byte *)PVar3.eax;
        (*g_MemoryApi.free)(RVar8.eax);
        RVar8.eax = pbVar1;
      }
    }
  }
Resource_Load_ReturnOpenAllocationOrDecodeResult:
  RVar10.ecx = (dword)in_ECX;
  RVar10.eax = (dword)RVar8.eax;
  RVar10.carry = true;
  return RVar10;
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
  dword dVar1;
  dword dVar2;
  int iVar3;
  ResourceRegistrationRecord100 *pRVar4;
  dword dVar5;
  dword dVar6;
  ResourceRegistrationRecordSerializedScalarView100 *pRVar7;
  ResourceRegistrationRecordSerializedScalarView100 *pRVar8;
  
  pRVar8 = runtimeImage->records58;
  dVar5 = runtimeImage->recordCountAC;
  do {
    while ((pRVar8->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
      for (iVar3 = 0x40; iVar3 != 0; iVar3 = iVar3 + -1) {
        pRVar8->primarySavedIdOrOffset = 0;
        pRVar8 = (ResourceRegistrationRecordSerializedScalarView100 *)
                 &pRVar8->secondarySavedIdOrOffset;
      }
      dVar5 = dVar5 - 1;
      if (dVar5 == 0) {
        pRVar4 = runtimeImage->tailRecordD8;
        pRVar8 = runtimeImage->records58;
        if (pRVar4 != (ResourceRegistrationRecord100 *)0x0) {
          pRVar4 = (ResourceRegistrationRecord100 *)
                   ((int)pRVar4 - (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        dVar5 = runtimeImage->recordCountAC;
        pRVar8[dVar5 - 1].nestedSavedOffsets13[0xc] = (dword)pRVar4;
        return CONCAT44(pRVar8,dVar5 * 0x100);
      }
    }
    dVar1 = pRVar8->primarySavedIdOrOffset;
    dVar2 = pRVar8->secondarySavedIdOrOffset;
    dVar6 = pRVar8->nestedBaseSavedOffset;
    if (dVar1 != 0) {
      dVar1 = dVar1 - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    if (dVar2 != 0) {
      dVar2 = dVar2 - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    if (dVar6 != 0) {
      dVar6 = dVar6 - (int)g_RuntimeObjectRebaseBaseMinusOne;
    }
    pRVar8->primarySavedIdOrOffset = dVar1;
    pRVar8->secondarySavedIdOrOffset = dVar2;
    pRVar8->nestedBaseSavedOffset = dVar6;
    pRVar8->ownerRuntimeSavedOffset = 0;
    dVar1 = pRVar8->auxiliarySavedIdOrOffset;
    dVar2 = pRVar8->nestedCountC8;
    if (dVar1 != 0) {
      dVar1 = dVar1 - 0x4bed4f;
    }
    pRVar8->auxiliarySavedIdOrOffset = dVar1;
    pRVar7 = pRVar8;
    for (; dVar2 != 0; dVar2 = dVar2 - 1) {
      if (pRVar7->nestedSavedOffsets13[0] != 0) {
        pRVar7->nestedSavedOffsets13[0] =
             pRVar7->nestedSavedOffsets13[0] - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      pRVar7 = (ResourceRegistrationRecordSerializedScalarView100 *)
               &pRVar7->secondarySavedIdOrOffset;
    }
    dVar1 = pRVar8->runtimePayloadSavedOffset;
                    
    switch(pRVar8->domainIndex) {
    case RESOURCE_DOMAIN_ARMY_RUNTIME:
      iVar3 = *(int *)(dVar1 + 8);
      pRVar8->paletteAssetSavedIdOrOffset = 0;
      dVar1 = dVar1 - g_ModelRuntimeRebaseDelta;
      pRVar8->textureSetSavedIdOrOffset = *(dword *)(iVar3 + 0xc);
      break;
    case RESOURCE_DOMAIN_SHOT_RUNTIME:
      dVar1 = dVar1 - (int)g_ShotRuntimeRebaseBaseMinusOne;
      pRVar8->textureSetSavedIdOrOffset = 0;
      pRVar8->paletteAssetSavedIdOrOffset = 0;
      break;
    case RESOURCE_DOMAIN_EFFECT_RUNTIME:
      dVar1 = dVar1 - (int)g_EffectRuntimeRebaseBaseMinusOne;
      pRVar8->textureSetSavedIdOrOffset = 0;
      pRVar8->paletteAssetSavedIdOrOffset = 0;
    }
    pRVar8->runtimePayloadSavedOffset = dVar1;
    pRVar8->spriteAssetSavedIdOrOffset = *(dword *)(pRVar8->spriteAssetSavedIdOrOffset + 0xb8);
    pRVar8 = pRVar8 + 1;
    dVar5 = dVar5 - 1;
  } while (dVar5 != 0);
  pRVar4 = runtimeImage->tailRecordD8;
  pRVar8 = runtimeImage->records58;
  if (pRVar4 != (ResourceRegistrationRecord100 *)0x0) {
    pRVar4 = (ResourceRegistrationRecord100 *)((int)pRVar4 - (int)g_RuntimeObjectRebaseBaseMinusOne)
    ;
  }
  dVar5 = runtimeImage->recordCountAC;
  pRVar8[dVar5 - 1].nestedSavedOffsets13[0xc] = (dword)pRVar4;
  return CONCAT44(pRVar8,dVar5 * 0x100);
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
  return 0x50f34000003a20;
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
  EffectRuntimeCompletionAction EVar1;
  EffectDefinitionReferenceOrSavedId4 EVar2;
  int clearDwordsRemaining;
  int runtimeSlotsRemaining;
  ModelRuntimeNode *ownerModelNode;
  EffectRuntimeSlot *runtimeSlotCursor;
  EffectRuntimeSlot *effectRuntimeSlotsBase;
  
  runtimeSlotsRemaining = 0x1000;
  runtimeSlotCursor = g_EffectRuntimeSlots;
  do {
    while( true ) {
      EVar1 = runtimeSlotCursor->completionAction;
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
      if (EVar1 == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
        ownerModelNode = (ModelRuntimeNode *)((int)ownerModelNode - g_ModelRuntimeRebaseDelta);
      }
      else if (EVar1 == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
        ownerModelNode =
             (ModelRuntimeNode *)((int)ownerModelNode - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    (runtimeSlotCursor->modelNodeOrSavedOffset).modelNode =
         (ModelRuntimeNode *)
         ((int)(runtimeSlotCursor->modelNodeOrSavedOffset).modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    runtimeSlotCursor->completionAction = EVar1;
    EVar2 = (EffectDefinitionReferenceOrSavedId4)
            ((runtimeSlotCursor->definitionOrSavedId).definition)->definitionId;
    (runtimeSlotCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode =
         ownerModelNode;
    runtimeSlotCursor->definitionOrSavedId = EVar2;
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
  ShotDefinitionReferenceOrSavedId4 SVar1;
  void *pvVar2;
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
      pvVar2 = (runtimeSlotCursor->runtimeStateOrSavedOffset).runtimeStatePointer;
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
    if (pvVar2 != (void *)0x0) {
      pvVar2 = (void *)((int)pvVar2 - g_ModelRuntimeRebaseDelta);
    }
    if (ownerArmyRuntime != (ArmyRuntimeSlot *)0x0) {
      ownerArmyRuntime =
           (ArmyRuntimeSlot *)((int)ownerArmyRuntime - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    (runtimeSlotCursor->modelNodeOrSavedOffset).modelNode =
         (ModelRuntimeNode *)
         ((int)(runtimeSlotCursor->modelNodeOrSavedOffset).modelNode -
         (int)g_RuntimeObjectRebaseBaseMinusOne);
    (runtimeSlotCursor->runtimeStateOrSavedOffset).runtimeStatePointer = pvVar2;
    SVar1 = (ShotDefinitionReferenceOrSavedId4)
            ((runtimeSlotCursor->definitionOrSavedId).definition)->definitionId;
    (runtimeSlotCursor->ownerAndTrajectory).ownerArmyRuntime = ownerArmyRuntime;
    runtimeSlotCursor->definitionOrSavedId = SVar1;
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
  LevelPlayerSlotByteOffset32 LVar1;
  InGameLevelConditionStorageView800 *pIVar2;
  WorldVector1EaxEcxEdx12 WVar3;
  WorldVector0EaxEcxEdx12 WVar4;
  
  pIVar2 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  LVar1 = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets
          [runtimeImage->levelRuntimeRecordIndex50 - 1];
  WVar3 = WorldRuntime_GetVector1Regs((WorldRuntimeContext *)runtimeImage);
  *(UQ12 *)((int)&(pIVar2->levelImage).playerSlots[0].startCameraMagnitudeQ12 + LVar1) =
       WVar3.magnitudeQ12;
  *(AngleTurn32 *)((int)&(pIVar2->levelImage).playerSlots[0].packedHeadingLow16PitchHigh16 + LVar1)
       = WVar3.headingAngle & 0xffff | WVar3.pitchAngle << 0x10;
  WVar4 = WorldRuntime_GetVector0Regs((WorldRuntimeContext *)runtimeImage);
  *(Q12 *)((int)&(pIVar2->levelImage).playerSlots[0].startCameraXQ12 + LVar1) = WVar4.xQ12;
  *(Q12 *)((int)&(pIVar2->levelImage).playerSlots[0].startCameraYQ12 + LVar1) = WVar4.yQ12;
  *(Q12 *)((int)&(pIVar2->levelImage).playerSlots[0].startCameraZQ12 + LVar1) = WVar4.zQ12;
  return;
}

