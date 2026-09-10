#include <thandor/assets/resource/runtime.h>

/* Implementation ownership: assets/resource/runtime. */

/* Address: 0x0040E2E0.
   Ownership: assets/resource/runtime.
   Purpose: Carry-flag success/failure semantics are preserved in the comment rather than fabricated as an ordinary
   return.
   Cross-module calls: FileSystem_WriteBufferToPathCf [platform/filesystem/win32], Package_Mount
   [assets/package/runtime].
*/
dword ResourceRegistration_OpenSourceCf(void *packagePath)

{
  byte *source;
  dword dVar1;
  int clearDwordsRemaining;
  byte *clearCursor;
  
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
  dVar1 = Package_Mount(packagePath);
  return dVar1;
}

/* Address: 0x0040F000.
   Ownership: assets/resource/runtime.
   Purpose: Handles resource load.
   Cross-module calls: Package_FindEntryAcrossMounts [assets/package/runtime], WidePath_CombineDirectoryAndLeaf
   [core/text/path], Package_DecodeEntryInto [assets/package/runtime].
*/
undefined8 __fastcall Resource_Load(undefined4 param_1,undefined4 param_2,word *path)

{
  PckEntryHeader *entry;
  byte *destination;
  dword dVar1;
  void *destination_00;
  FileIoByteCount byteCount;
  byte *memory;
  undefined4 extraout_EDX;
  EngineFileHandle unaff_EBX;
  undefined1 in_CF;
  bool bVar2;
  
  entry = Package_FindEntryAcrossMounts(path);
  if ((bool)in_CF) {
    WidePath_CombineDirectoryAndLeaf
              ((word *)&g_FileSystemCombinedPathScratchUtf16,path,
               (word *)&g_ExecutableDirectoryUtf16);
    destination = (byte *)(*g_FileSystemOpenCf)(0,(word *)&g_FileSystemCombinedPathScratchUtf16);
    if ((bool)in_CF) {
      destination = (byte *)(*g_FileSystemOpenCf)(0,path);
      if ((bool)in_CF) goto Resource_Load_ReturnOpenAllocationOrDecodeResult;
    }
    bVar2 = false;
    dVar1 = (*g_FileSystemGetSizeCf)(destination);
    if (!bVar2) {
      destination_00 = (*g_MemoryApi.alloc)(dVar1);
      if (bVar2) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,byteCount,g_FatalErrorDetail1Utf16);
      }
      else {
        dVar1 = (*g_FileSystemReadExactCf)(byteCount,destination_00,destination);
        if (!bVar2) {
          (*g_FileSystemClose)(destination);
          return CONCAT44(param_2,extraout_EDX);
        }
        (*g_MemoryApi.free)((void *)((ulonglong)dVar1 >> 0x20));
      }
    }
    (*g_FileSystemClose)(destination);
  }
  else {
    destination = (byte *)0x5;
    bVar2 = entry->packedSize < 0x800000;
    if (entry->packedSize < 0x800001) {
      destination = (*g_MemoryApi.alloc)(entry->unpackedSize);
      if (!bVar2) {
        Package_DecodeEntryInto(destination,entry,unaff_EBX);
        if (!bVar2) {
          return CONCAT44(param_2,memory);
        }
        destination = memory;
        (*g_MemoryApi.free)(memory);
      }
    }
  }
Resource_Load_ReturnOpenAllocationOrDecodeResult:
  return CONCAT44(param_2,destination);
}

/* Address: 0x0040F1D0.
   Ownership: assets/resource/runtime.
   Purpose: Handles resource release.
*/
undefined4 Resource_Release(void *allocation)

{
  undefined4 in_EAX;
  
  (*g_MemoryApi.free)(allocation);
  return in_EAX;
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
void ResourceRegistration_ResolveRuntimeRecord(ResourceRegistrationRuntimeImage *runtimeImage)

{
  InGameConditionRuntime *pIVar1;
  InGameConditionRuntime *pIVar2;
  uint extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined8 uVar3;
  
  pIVar2 = g_InGameConditionRuntime;
  pIVar1 = (&g_InGameConditionRuntime)[runtimeImage->levelRuntimeRecordIndex50];
  uVar3 = WorldRuntime_GetVector1Regs((WorldRuntimeContext *)runtimeImage);
  *(int *)(pIVar2[2].reserved54_57 + (int)pIVar1) = (int)uVar3;
  *(uint *)(pIVar2[2].reserved54_57 + (int)(pIVar1->reserved00_4F + 4)) =
       extraout_ECX & 0xffff | (int)((ulonglong)uVar3 >> 0x20) << 0x10;
  uVar3 = WorldRuntime_GetVector0Regs((WorldRuntimeContext *)runtimeImage);
  *(int *)(pIVar2[2].reserved00_4F + (int)(pIVar1->reserved00_4F + 0x48)) = (int)uVar3;
  *(undefined4 *)(pIVar2[2].reserved00_4F + (int)(pIVar1->reserved00_4F + 0x4c)) = extraout_ECX_00;
  *(int *)(pIVar2[2].reserved54_57 + (int)&pIVar1[-1].tailRecordD8) =
       (int)((ulonglong)uVar3 >> 0x20);
  return;
}
