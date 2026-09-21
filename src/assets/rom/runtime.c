/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/rom/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/rom/runtime.h>

/* Implementation ownership: assets/rom/runtime. */

/* Address: 0x005452A0.
   Ownership: assets/rom/runtime.
   Purpose: Executes one indexed frontend ROM action record, including optional activation sound, queued action
   dispatch, transition parameter copying, ROM record lookup, transition initialization, and visibility updates.
   Typed parameters: p5 recordIndex→RomRecordTableIndex_V331. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p4 suppressActivationSound→FrontendBooleanState32_V342.
   Local calls: RomRegistry_FindRecordByIdCf, FrontendRomTransition_InitializeFromRecord,
   RomRuntime_UpdateRecordVisibilityAndDescriptorsCf.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendRomActionTable_ExecuteRecord
          (dword reservedZero0,dword reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex)

{
  RomAssetRecordPrefix *record;
  dword dVar1;
  RomRecordByteSize RVar2;
  RomRecordId RVar3;
  int iVar4;
  RomAssetRecordPrefix *pRVar5;
  int iVar6;
  void *source;
  bool bVar7;
  RomRecordLookupEaxCf5 RVar8;
  RomRecordId RVar9;
  RomRecordId recordId;
  
  iVar4 = g_FrontendRootNode;
  if (recordIndex < *(uint *)(g_FrontendActiveRomRecordTable + 0x3c)) {
    record = (RomAssetRecordPrefix *)(recordIndex * 0x200 + 0x200 + g_FrontendActiveRomRecordTable);
    source = (void *)(g_FrontendRootNode + 0x368);
    recordId = record[3].rootNodeOffsetOrPointer;
    RVar9 = record[2].recordId;
    if ((((((RVar9 != 3) && (RVar9 != 4)) && (RVar9 != 9)) && (-1 < (int)RVar9)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
         SESSION_NETWORK_ROLE_LOCAL)) && ((RVar9 != 2 || (g_NetworkBackendInstanceCount != 0)))) {
      if ((record[3].recordId != 0) &&
         ((suppressActivationSound == 0 &&
          ((DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId] !=
           (DirectSoundVoiceSet *)0x0)))) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId]
                  );
      }
      if (recordId == 0) {
        if ((int)RVar9 < 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            UiActionQueue_Enqueue(0,source);
          }
        }
        else if (RVar9 != 0) {
          g_FrontendPendingPageAction = RVar9;
        }
      }
      else if ((recordId != 0) && (1 < (int)record[3].byteSize)) {
        iVar6 = record[3].byteSize - 1;
        RVar9 = *(RomRecordId *)(iVar4 + 0x3cc);
        record[5].rootNodeOffsetOrPointer = *(dword *)(iVar4 + 0x3c8);
        record[5].recordId = RVar9;
        dVar1 = *(dword *)(iVar4 + 0x3d4);
        record[6].byteSize = *(RomRecordByteSize *)(iVar4 + 0x3d0);
        record[6].rootNodeOffsetOrPointer = dVar1;
        RVar2 = *(RomRecordByteSize *)(iVar4 + 0x3dc);
        record[6].recordId = *(RomRecordId *)(iVar4 + 0x3d8);
        record[7].byteSize = RVar2;
        record[7].rootNodeOffsetOrPointer = 0;
        RVar8 = RomRegistry_FindRecordByIdCf(recordId);
        pRVar5 = RVar8.recordOrError;
        if (!RVar8.carry) {
          RVar9 = record[2].recordId;
          RVar2 = pRVar5[3].byteSize;
          *(RomRecordId *)((int)record + iVar6 * 0x20 + 0x40) = pRVar5[2].recordId;
          *(RomRecordByteSize *)((int)record + iVar6 * 0x20 + 0x44) = RVar2;
          RVar3 = pRVar5[3].recordId;
          *(dword *)((int)record + iVar6 * 0x20 + 0x48) = pRVar5[3].rootNodeOffsetOrPointer;
          *(RomRecordId *)((int)record + iVar6 * 0x20 + 0x4c) = RVar3;
          dVar1 = pRVar5[4].rootNodeOffsetOrPointer;
          *(RomRecordByteSize *)((int)record + iVar6 * 0x20 + 0x50) = pRVar5[4].byteSize;
          *(dword *)((int)record + iVar6 * 0x20 + 0x54) = dVar1;
          FrontendRomTransition_InitializeFromRecord(recordId,record);
          bVar7 = RomRuntime_UpdateRecordVisibilityAndDescriptorsCf(RVar9,recordId);
          if (bVar7) {
            g_FrontendRomTransitionPendingCount = 0;
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x00546450.
   Ownership: assets/rom/runtime.
   Purpose: Validates the 'rom' magic and converter version 0x00010005, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byteSize. Invalid headers update the
   package last-error path. CF and EAX status are preserved.
   Local calls: RomAssetRecord_RegisterAndRelocate.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx RomAsset_PrepareRecords(RomAssetHeader *asset)

{
  dword registrationStatusCode;
  AssetRecordCount AVar1;
  RomAssetHeader *record;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  registrationStatusCode = ROM_ASSET_REGISTRATION_FAILURE_SENTINEL_0x3B;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ROM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ROM_00010005)) {
    AVar1 = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (AVar1 == 0) {
        SVar2.carry = false;
        SVar2.valueOrError = registrationStatusCode;
        return SVar2;
      }
      SVar2 = RomAssetRecord_RegisterAndRelocate((RomAssetRecordPrefix *)record,asset);
      registrationStatusCode = SVar2.valueOrError;
      if (SVar2.carry) break;
      record = (RomAssetHeader *)
               ((record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      AVar1 = AVar1 - 1;
    }
  }
  else {
    Package_SetLastErrorPath((word *)u_engine_zentrale_rom_00545aa4);
  }
  SVar3.carry = true;
  SVar3.valueOrError = registrationStatusCode;
  return SVar3;
}


/* Address: 0x005466A0.
   Ownership: assets/rom/runtime.
   Purpose: Walks all 256 ROM registry slots, builds a runtime node tree for each populated record, stores the root
   in the paired registry slot, and links it into the owning world list.
   Local calls: RomRuntime_BuildNodeTreeRecursive.
   Cross-module calls: WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
bool RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime)

{
  RomAssetRecordPrefix *pRVar1;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar2;
  RomRegistrySlot *pRVar3;
  ModelNodeCreateEaxCf5 MVar4;
  
  iVar2 = 0x100;
  pRVar3 = g_RomRegistrySlots;
  do {
    pRVar1 = pRVar3->record;
    if (pRVar1 != (RomAssetRecordPrefix *)0x0) {
      MVar4 = RomRuntime_BuildNodeTreeRecursive
                        (pRVar1[5].rootNodeOffsetOrPointer,
                         (RomSerializedNodeHeader34 *)pRVar1->rootNodeOffsetOrPointer,worldRuntime);
      modelNodeRuntime = MVar4.modelNode;
      if (MVar4.carry) {
        return true;
      }
      pRVar3->runtimeRootNode = (WorldRuntimeNode *)modelNodeRuntime;
      WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
      ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    }
    pRVar3 = pRVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return false;
}


/* Address: 0x00547FC0.
   Ownership: assets/rom/runtime.
   Purpose: Under the frontend ROM-transition spin lock, completes a pending transition request, activates the
   returned record ID when successful, frees the pending allocation, and clears the pending count.
   Local calls: FrontendRomTransition_ActivateRecordByIdCf.
   Cross-module calls: WorldMotionSpline_EvaluateAndApplyAtTime [core/math/interpolation].
*/
void __thandor_void_preserve_eax_ecx FrontendRomTransition_ProcessPendingRecord(void)

{
  RomRecordId recordId;
  WorldRuntimeContext *worldRuntime;
  bool bVar1;
  StatusValueEaxCf5 SVar2;
  
  (*g_SpinLockAcquire)(&g_FrontendStateTickSpinLock);
  recordId = g_FrontendRomTransitionPendingCount;
  worldRuntime = (WorldRuntimeContext *)(g_FrontendRootNode + 0x368);
  if (g_FrontendRomTransitionPendingCount != 0) {
    bVar1 = WorldMotionSpline_EvaluateAndApplyAtTime
                      (g_FrontendRomTransitionSplineKeyframeCount,
                       g_FrontendRomTransitionSplineKeyframes,g_FrontendRomTransitionElapsedTicks,
                       worldRuntime);
    if ((!bVar1) && (g_FrontendRomTransitionPendingCount = 0, -1 < (int)recordId)) {
      SVar2 = FrontendRomTransition_ActivateRecordByIdCf(recordId,worldRuntime);
      (*g_FatalErrorPrimaryDispatchCf)(SVar2.valueOrError,SVar2.carry);
    }
  }
  (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00547400.
   Ownership: assets/rom/runtime.
   Purpose: Walks exactly 256 ROM registry slots, releases nested resources reachable through each record root,
   then clears both dwords of every eight-byte slot. All saved registers and EAX are preserved.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendRomRegistry_ClearAndReleaseNestedResources(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  RomRegistrySlot *pRVar4;
  dword dVar5;
  
  iVar2 = 0x100;
  pRVar4 = g_RomRegistrySlots;
  while( true ) {
    if ((pRVar4->record != (RomAssetRecordPrefix *)0x0) &&
       (dVar5 = pRVar4->record->rootNodeOffsetOrPointer, dVar5 != 0)) break;
FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance:
    pRVar4->record = (RomAssetRecordPrefix *)0x0;
    pRVar4->runtimeRootNode = (WorldRuntimeNode *)0x0;
    pRVar4 = pRVar4 + 1;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      return;
    }
  }
  iVar3 = 0;
  do {
    iVar1 = *(int *)(dVar5 + 0x10);
    Resource_Release(*(void **)(dVar5 + 0x2c));
    iVar3 = iVar3 + 1;
    while( true ) {
      if (iVar1 != 0) break;
      iVar3 = iVar3 + -1;
      if (iVar3 == 0)
      goto FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance;
    }
    dVar5 = *(dword *)(dVar5 + 0x14);
  } while( true );
}


/* Address: 0x00548720.
   Ownership: assets/rom/runtime.
   Purpose: If the ROM transition enable state is nonzero, publishes the exact transition state value 0x10000000.
   No registers or flags are deliberately replaced by the prototype.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendRomTransition_RequestStop(void)

{
  if (g_FrontendRomTransitionPendingCount != 0) {
    g_FrontendRomTransitionElapsedTicks = 0x10000000;
  }
  return;
}


/* Address: 0x005487F0.
   Ownership: assets/rom/runtime.
   Purpose: Typed parameters: p0 slotValue→RomRegistrySlotValue_V344. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
RomAssetRecordPrefix * __thandor_eax_preserve_ecx_edx
RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    if ((WorldRuntimeNode *)slotValue == slotCursor->runtimeRootNode) {
      return slotCursor->record;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return (RomAssetRecordPrefix *)0x0;
}


/* Address: 0x00548840.
   Ownership: assets/rom/runtime.
   Purpose: Reverse lookup for the ROM registry: returns the secondary slot value paired with a record pointer, or
   zero when the record is absent.
*/
dword RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    if (record == slotCursor->record) {
      return (dword)slotCursor->runtimeRootNode;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return 0;
}

/* Address: 0x00548890.
   Ownership: assets/rom/runtime.
   Purpose: Scans the active fixed-stride ROM record table and returns the first 0x200-byte record whose identifier
   at offset 0x1C matches recordId, or null. Typed parameters: p0 recordId→RomRecordId_V308. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
void * __thandor_eax_preserve_ecx_edx
RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable)

{
  int recordsRemaining;
  
  recordsRemaining = *(int *)((int)recordTable + 0x3c);
  while( true ) {
    if (recordsRemaining == 0) {
      return (void *)0x0;
    }
    if (recordId == *(RomRecordId *)((int)recordTable + 0x21c)) break;
    recordsRemaining = recordsRemaining + -1;
    recordTable = (void *)((int)recordTable + 0x200);
  }
  return (void *)((int)recordTable + 0x200);
}


/* Address: 0x005488D0.
   Ownership: assets/rom/runtime.
   Purpose: Scans the count at table offset +0x3C over exact 0x200-byte records, compares each record ID at +0x1C,
   and returns the zero-based index or -1 in EAX. EDX is preserved. Typed parameters: p0 recordId→RomRecordId_V308.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
RomRecordTableIndex __thandor_eax_preserve_ecx_edx
RomRecordTable_FindIndexById(RomRecordId recordId,void *table)

{
  int recordIndex;
  int recordsRemaining;
  
  recordsRemaining = *(int *)((int)table + 0x3c);
  recordIndex = 0;
  while( true ) {
    if (recordsRemaining == 0) {
      return 0xffffffff;
    }
    if (recordId == *(RomRecordId *)((int)table + 0x21c)) break;
    recordIndex = recordIndex + 1;
    recordsRemaining = recordsRemaining + -1;
    table = (void *)((int)table + 0x200);
  }
  return recordIndex;
}


/* Address: 0x005484D0.
   Ownership: assets/rom/runtime.
   Purpose: Finds a ROM record by ID, updates registry-node visibility and indexed descriptors against the selected
   record masks, applies world motion parameters, and returns status through CF. Typed parameters: p0
   recordId→RomRecordId_V308. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RomRegistry_FindRecordByIdCf, RomRegistry_FindSlotValueByRecordIdCf,
   RomRuntime_ApplyIndexedDescriptor.
   Cross-module calls: GraphicsShadingRuntime_ClearRecordTable [graphics/render/shading],
   WorldRuntime_SetPosition60AndDistanceFromPosition80 [world/runtime/core],
   WorldRuntime_SetMotionParameters6CThrough78Clamped [world/runtime/core], UiActionQueue_Enqueue
   [ui/core/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FrontendRomTransition_ActivateRecordByIdCf(RomRecordId recordId,WorldRuntimeContext *worldRuntime)

{
  uint *puVar1;
  WorldRuntimeNodeFlags *pWVar2;
  RomAssetRecordPrefix *pRVar3;
  WorldRuntimeNode *pWVar4;
  uint uVar5;
  RomRecordTableIndex entryIndex;
  RomRecordByteSize RVar6;
  RomRecordId RVar7;
  int iVar8;
  RomRegistrySlot *pRVar9;
  RomRecordLookupEaxCf5 RVar10;
  StatusValueEaxCf5 SVar11;
  StatusValueEaxCf5 SVar12;
  
  GraphicsShadingRuntime_ClearRecordTable();
  RVar10 = RomRegistry_FindRecordByIdCf(recordId);
  SVar12.valueOrError = RVar10.recordOrError;
  if (!RVar10.carry) {
    pRVar3 = SVar12.valueOrError;
    g_FrontendActiveRomRecordTable = SVar12.valueOrError;
    pRVar9 = g_RomRegistrySlots;
    for (RVar6 = SVar12.valueOrError[5].byteSize; g_RomRegistrySlots = pRVar9, RVar6 != 0;
        RVar6 = RVar6 - 1) {
      SVar11 = RomRegistry_FindSlotValueByRecordIdCf(pRVar3[0x2d].byteSize);
      if (!SVar11.carry) {
        puVar1 = (uint *)(SVar11.valueOrError + 0x4c);
        *puVar1 = *puVar1 | 0x20;
      }
      pRVar3 = (RomAssetRecordPrefix *)&pRVar3[0x2a].recordId;
      pRVar9 = g_RomRegistrySlots;
    }
    iVar8 = 0x100;
    do {
      pRVar3 = pRVar9->record;
      pWVar4 = pRVar9->runtimeRootNode;
      if ((pRVar3 != (RomAssetRecordPrefix *)0x0) &&
         ((pWVar2 = &pWVar4->runtimeFlags, *pWVar2 = *pWVar2 | 0x40, SVar12.valueOrError == pRVar3
          || (((&SVar12.valueOrError[1].rootNodeOffsetOrPointer)[pRVar3->recordId >> 5] &
              1 << ((byte)pRVar3->recordId & 0x1f)) != 0)))) {
        pWVar2 = &pWVar4->runtimeFlags;
        *pWVar2 = *pWVar2 & 0xffffffbf;
        entryIndex = 0;
        for (RVar7 = pRVar3[4].recordId; RVar7 != 0; RVar7 = RVar7 - 1) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,pRVar3);
          entryIndex = entryIndex + 1;
        }
      }
      uVar5 = g_FrontendRomTransitionContextValue;
      pRVar9 = pRVar9 + 1;
      iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (SVar12.valueOrError[3].rootNodeOffsetOrPointer,SVar12.valueOrError[3].byteSize,
               SVar12.valueOrError[2].recordId,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,SVar12.valueOrError[4].rootNodeOffsetOrPointer,SVar12.valueOrError[4].byteSize,
               SVar12.valueOrError[3].recordId,worldRuntime);
    if (uVar5 != 0) {
      if ((int)uVar5 < 0) {
        UiActionQueue_Enqueue(0,worldRuntime);
      }
      else {
        g_FrontendPendingPageAction = uVar5;
      }
    }
    SVar11.carry = false;
    SVar11.valueOrError = uVar5;
    return SVar11;
  }
  SVar12.carry = true;
  return SVar12;
}


/* Address: 0x00548600.
   Ownership: assets/rom/runtime.
   Purpose: Clears runtime bit 0x20 across the exact 256-slot ROM registry, resolves the requested record, clears
   and rebuilds bit 0x40 visibility from the selected and companion record masks, and replays every indexed
   descriptor for visible records. CF clear reports success and CF set reports lookup failure. Typed parameters: p1
   recordId→RomRecordId_V308. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RomRegistry_FindRecordByIdCf, RomRuntime_ApplyIndexedDescriptor.
   Cross-module calls: GraphicsShadingRuntime_ClearRecordTable [graphics/render/shading].
*/
bool __thandor_cf_preserve_eax_ecx_edx
RomRuntime_UpdateRecordVisibilityAndDescriptorsCf
          (RomVisibilityFrontendValue frontendValue,RomRecordId recordId)

{
  WorldRuntimeNodeFlags *pWVar1;
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *pWVar2;
  RomRecordTableIndex entryIndex;
  int iVar3;
  uint uVar4;
  RomRecordId RVar5;
  RomRegistrySlot *pRVar6;
  RomRecordLookupEaxCf5 RVar7;
  
  iVar3 = 0x100;
  g_FrontendRomTransitionContextValue = frontendValue;
  pRVar6 = g_RomRegistrySlots;
  do {
    if (pRVar6->runtimeRootNode != (WorldRuntimeNode *)0x0) {
      pWVar1 = &pRVar6->runtimeRootNode->runtimeFlags;
      *pWVar1 = *pWVar1 & 0xffffffdf;
    }
    pRVar6 = pRVar6 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  RVar7 = RomRegistry_FindRecordByIdCf(recordId);
  if (!RVar7.carry) {
    GraphicsShadingRuntime_ClearRecordTable();
    iVar3 = 0x100;
    pRVar6 = g_RomRegistrySlots;
    do {
      record = pRVar6->record;
      pWVar2 = pRVar6->runtimeRootNode;
      if ((record != (RomAssetRecordPrefix *)0x0) &&
         (((pWVar1 = &pWVar2->runtimeFlags, *pWVar1 = *pWVar1 | 0x40, record == RVar7.recordOrError
           || (record == g_FrontendActiveRomRecordTable)) ||
          (uVar4 = record->recordId >> 5,
          (1 << ((byte)record->recordId & 0x1f) &
          ((&RVar7.recordOrError[1].rootNodeOffsetOrPointer)[uVar4] |
          (&g_FrontendActiveRomRecordTable[1].rootNodeOffsetOrPointer)[uVar4])) != 0)))) {
        pWVar1 = &pWVar2->runtimeFlags;
        *pWVar1 = *pWVar1 & 0xffffffbf;
        entryIndex = 0;
        for (RVar5 = record[4].recordId; RVar5 != 0; RVar5 = RVar5 - 1) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,record);
          entryIndex = entryIndex + 1;
        }
      }
      pRVar6 = pRVar6 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    return false;
  }
  return true;
}


/* Address: 0x00546330.
   Ownership: assets/rom/runtime.
   Purpose: Inserts a ROM record into the first free slot of a 256-entry, 8-byte runtime registry; converts its
   root-node offset and recursive child offsets to pointers; loads each referenced sprite asset; reuses an existing
   sprite by registryId when possible; otherwise prepares and registers the newly loaded sprite. CF and EAX errors
   are preserved.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], SpriteAssetRegistry_FindById
   [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers [assets/sprite/catalog], Resource_Release
   [assets/resource/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase)

{
  int *piVar1;
  dword dVar2;
  RomAssetHeader *asset;
  SpriteAssetHeader *pSVar3;
  int iVar4;
  RomRegistrySlot *pRVar5;
  int unaff_EBP;
  int unaff_ESI;
  byte *pbVar6;
  bool bVar7;
  StatusValueEaxCf5 SVar8;
  PackageLoadEntryEaxCf5 PVar9;
  SpriteRegisterRelocateEaxCf5 SVar10;
  StatusValueEaxCf5 SVar11;
  
  iVar4 = 0x100;
  pRVar5 = g_RomRegistrySlots;
  do {
    if (pRVar5->record == (RomAssetRecordPrefix *)0x0) {
      dVar2 = record->rootNodeOffsetOrPointer;
      pRVar5->record = record;
      if (dVar2 == 0) {
RomAssetRecord_ReturnWithoutRootNode:
        SVar11.carry = false;
        SVar11.valueOrError = (dword)assetBase;
        return SVar11;
      }
      record->rootNodeOffsetOrPointer =
           (dword)((assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28
                  + (record->rootNodeOffsetOrPointer - 0x28));
      pbVar6 = (assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               (dVar2 - 0x28);
      iVar4 = 0;
      goto RomAssetRecord_LoadOrReuseSpriteReference;
    }
    pRVar5 = pRVar5 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  Package_SetLastErrorPath((word *)u_engine_zentrale_rom_00545aa4);
  asset = (RomAssetHeader *)&k_LowAddressLiteral0000003B;
RomAssetRecord_ReturnRegistrationResult:
  SVar8.carry = true;
  SVar8.valueOrError = (dword)asset;
  return SVar8;
RomAssetRecord_LoadOrReuseSpriteReference:
  bVar7 = WidePath_SetExtensionCode(0x727073,(word *)(pbVar6 + 0x34));
  asset = assetBase;
  if (bVar7) goto RomAssetRecord_ReturnRegistrationResult;
  PVar9 = Package_LoadEntry((word *)(pbVar6 + 0x34));
  asset = PVar9.bufferOrError;
  if (PVar9.carry) goto RomAssetRecord_ReturnRegistrationResult;
  pSVar3 = SpriteAssetRegistry_FindById(*(SpriteAssetId *)((int)asset->reservedB4_1FF + 4));
  if (pSVar3 == (SpriteAssetHeader *)0x0) {
    *(int *)(pbVar6 + 0x30) = *(int *)(pbVar6 + 0x30) + 1;
    *(RomAssetHeader **)(pbVar6 + 0x2c) = asset;
    SVar10 = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
    asset = (RomAssetHeader *)SVar10.assetOrError;
    if (SVar10.carry) goto RomAssetRecord_ReturnRegistrationResult;
  }
  else {
    *(SpriteAssetHeader **)(pbVar6 + 0x2c) = pSVar3;
    Resource_Release(asset);
  }
  iVar4 = iVar4 + 1;
  while (record == (RomAssetRecordPrefix *)0x0) {
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) goto RomAssetRecord_ReturnWithoutRootNode;
  }
  piVar1 = (int *)(unaff_ESI + 0x14 + unaff_EBP * 4);
  *piVar1 = (int)((assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 *piVar1 + -0x28);
  record = (RomAssetRecordPrefix *)((int)&record[-1].recordId + 3);
  pbVar6 = *(byte **)(unaff_ESI + 0x14 + unaff_EBP * 4);
  unaff_EBP = unaff_EBP + 1;
  goto RomAssetRecord_LoadOrReuseSpriteReference;
}


/* Address: 0x005464C0.
   Ownership: assets/rom/runtime.
   Purpose: Recursively allocates and initializes one runtime node from a ROM record tree, copies transform and
   descriptor fields, links child runtime nodes, and returns the new node in EAX. Typed parameters: p2
   stateTintArgb→PackedArgb32. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p3 romNodeRecord→RomRuntimeNodeRecordAddress32_V345, p4 worldObjectArray→WorldRuntimeContext
   *.
   Cross-module calls: WorldObjectArray_AllocateFreeRecordCf [world/runtime/core].
*/
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader34 *romNodeRecord,
          WorldRuntimeContext *worldObjectArray)

{
  AngleTurn32 AVar1;
  AngleTurn32 AVar2;
  AngleTurn32 AVar3;
  ModelResourceHitTestAndRenderView210 *pMVar4;
  Q12 QVar5;
  uint uVar6;
  uint uVar7;
  GraphicsTextureSet *pGVar8;
  ModelRuntimeNode *pMVar9;
  ModelRuntimeNode *pMVar10;
  ModelPackedLookupTableEntryCount MVar11;
  dword dVar12;
  uint uVar13;
  byte *pbVar14;
  WorldObjectRecordEaxCf5 WVar15;
  ModelNodeCreateEaxCf5 MVar16;
  
  WVar15 = WorldObjectArray_AllocateFreeRecordCf(worldObjectArray);
  pMVar9 = (ModelRuntimeNode *)WVar15.recordOrError;
  pMVar10 = pMVar9;
  if (WVar15.carry) {
RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet:
    MVar16.carry = true;
    MVar16.modelNode = pMVar10;
    return MVar16;
  }
  (pMVar9->modelPayload).localTranslationXQ12 = 0;
  (pMVar9->modelPayload).localTranslationYQ12 = 0;
  (pMVar9->modelPayload).localTranslationZQ12 = 0;
  (pMVar9->worldTransform).translation.x = 0;
  (pMVar9->worldTransform).translation.y = 0;
  (pMVar9->worldTransform).translation.z = 0;
  AVar1 = romNodeRecord->localRotationAngle0;
  AVar2 = romNodeRecord->localRotationAngle1;
  AVar3 = romNodeRecord->localRotationAngle2;
  (pMVar9->modelPayload).localRotationAngle0 = AVar1;
  (pMVar9->modelPayload).localRotationAngle1 = AVar2;
  (pMVar9->modelPayload).localRotationAngle2 = AVar3;
  (pMVar9->modelPayload).worldRotationAngle0 = AVar1;
  (pMVar9->modelPayload).worldRotationAngle1 = AVar2;
  (pMVar9->modelPayload).worldRotationAngle2 = AVar3;
  (pMVar9->modelPayload).meshGroupMask = 0xffffffff;
  pMVar9->runtimeFlags = pMVar9->runtimeFlags | 1;
  *(byte *)&pMVar9->textureSubresourceBaseIndex = 0;
  *(byte *)((int)&pMVar9->textureSubresourceBaseIndex + 1) = 0;
  *(byte *)((int)&pMVar9->textureSubresourceBaseIndex + 2) = 0;
  *(byte *)((int)&pMVar9->textureSubresourceBaseIndex + 3) = 0;
  pMVar9->tintArgb = stateTintArgb;
  pGVar8 = g_FrontendCentralTextureSet;
  pMVar4 = (romNodeRecord->spriteAssetReference).modelResource;
  (pMVar9->modelPayload).paletteAsset = g_FrontendCentralPaletteAsset;
  QVar5 = pMVar4->boundingRadiusQ12;
  (pMVar9->modelPayload).textureSet = pGVar8;
  pMVar9->subtreeBoundingRadiusQ12 = QVar5;
  (pMVar9->modelPayload).modelResource = pMVar4;
  pMVar9->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  pMVar9->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  pMVar9->runtimeStateA0 = (dword)&pMVar4->firstMeshGroupRelativeOffset;
  dVar12 = romNodeRecord->childCount;
  pMVar4 = (romNodeRecord->spriteAssetReference).modelResource;
  uVar13 = 0;
  pMVar9->childCount = dVar12;
  pMVar9->parentNode = (ModelRuntimeNode *)0x0;
  do {
    if (dVar12 == 0) {
      return (ModelNodeCreateEaxCf5)((uint5)WVar15 & 0xffffffff);
    }
    pbVar14 = pMVar4->reserved00_AF + pMVar4->packedLookupTableRelativeOffset;
    for (MVar11 = pMVar4->packedLookupTableEntryCount; MVar11 != 0; MVar11 = MVar11 - 1) {
      if (((*(uint *)pbVar14 & 0xf) == 0) && (uVar13 == *(uint *)pbVar14 >> 4)) {
        MVar16 = RomRuntime_BuildNodeTreeRecursive
                           (stateTintArgb,romNodeRecord->childReferences[uVar13].node,
                            worldObjectArray);
        pMVar10 = MVar16.modelNode;
        if (MVar16.carry)
        goto RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet;
        pMVar9->childNodes[uVar13] = pMVar10;
        pMVar10->parentNode = pMVar9;
        uVar6 = *(uint *)(pbVar14 + 8);
        uVar7 = *(uint *)(pbVar14 + 0xc);
        (pMVar10->modelPayload).localTranslationXQ12 = *(uint *)(pbVar14 + 4);
        (pMVar10->modelPayload).localTranslationYQ12 = uVar6;
        (pMVar10->modelPayload).localTranslationZQ12 = uVar7;
        goto RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor;
      }
      pbVar14 = pbVar14 + 0x10;
    }
    pMVar9->childCount = pMVar9->childCount - 1;
    uVar13 = uVar13 - 1;
RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor:
    uVar13 = uVar13 + 1;
    dVar12 = dVar12 - 1;
  } while( true );
}


/* Address: 0x005483C0.
   Ownership: assets/rom/runtime.
   Purpose: Publishes an exact ROM transition state: clears the result, stores the second argument as enable state,
   copies record dword +0x24, stores record+0x40, and invokes the existing transition initializer. EAX is
   preserved. Typed parameters: p0 transitionEnabled→FrontendBooleanState32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: WorldMotionSpline_BuildSixChannelCurves [core/math/interpolation].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendRomTransition_InitializeFromRecord
          (FrontendBooleanState32 transitionEnabled,RomAssetRecordPrefix *record)

{
  g_FrontendRomTransitionElapsedTicks = 0;
  g_FrontendRomTransitionSplineKeyframeCount = record[3].byteSize;
  g_FrontendRomTransitionPendingCount = transitionEnabled;
  g_FrontendRomTransitionSplineKeyframes = &record[5].rootNodeOffsetOrPointer;
  WorldMotionSpline_BuildSixChannelCurves
            (g_FrontendRomTransitionSplineKeyframeCount,
             (WorldMotionSplineKeyframe *)g_FrontendRomTransitionSplineKeyframes);
  return;
}


/* Address: 0x005487A0.
   Ownership: assets/rom/runtime.
   Purpose: Finds a ROM record by recordId and returns the second dword stored in the matching RomRegistrySlot.
   Error 0x3C is returned with CF set on a miss. Typed parameters: p0 recordId→RomRecordId_V308. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRegistry_FindSlotValueByRecordIdCf(RomRecordId recordId)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((slotCursor->record == (RomAssetRecordPrefix *)0x0 ||
         (recordId != slotCursor->record->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      SVar2.carry = true;
      SVar2.valueOrError = 0x3c;
      return SVar2;
    }
  }
  SVar1.carry = false;
  SVar1.valueOrError = (dword)slotCursor->runtimeRootNode;
  return SVar1;
}


/* Address: 0x00548410.
   Ownership: assets/rom/runtime.
   Purpose: Bounds-checks an entry index against record dword +0x38, scans the associated descriptor table for type
   4 with the same high index, and forwards the record and descriptor values to the existing graphics runtime
   helper. EAX is preserved. Typed parameters: p0 entryIndex→RomRecordTableIndex_V331. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: GraphicsShadingRuntime_AllocateRecordRegs [graphics/render/shading].
*/
void __thandor_void_preserve_eax_ecx_edx
RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record)

{
  uint descriptorsRemaining;
  uint *descriptorCursor;
  PackedRgb24 *descriptorColorPair;
  int modelRuntimeNodeBaseAddress;
  
  if (entryIndex < record[4].recordId) {
    modelRuntimeNodeBaseAddress = *(int *)(record->rootNodeOffsetOrPointer + 0x2c);
    descriptorColorPair = (PackedRgb24 *)(entryIndex * 0x10 + 0x50 + (int)record);
    descriptorCursor =
         (uint *)(modelRuntimeNodeBaseAddress + *(int *)(modelRuntimeNodeBaseAddress + 0xe4));
    for (descriptorsRemaining = *(uint *)(modelRuntimeNodeBaseAddress + 0xe8);
        descriptorsRemaining != 0; descriptorsRemaining = descriptorsRemaining - 1) {
      if (((*descriptorCursor & 0xf) == 4) && (*descriptorCursor >> 4 == (entryIndex & 0xfffffff)))
      {
        GraphicsShadingRuntime_AllocateRecordRegs
                  (0,descriptorColorPair[1],*descriptorColorPair,descriptorCursor[3],
                   descriptorCursor[2],descriptorCursor[1]);
        return;
      }
      descriptorCursor = descriptorCursor + 4;
    }
  }
  return;
}


/* Address: 0x00548740.
   Ownership: assets/rom/runtime.
   Purpose: Scans all 256 RomRegistrySlot records and returns the registered ROM record whose recordId matches.
   Error 0x3C is returned with CF set on a miss. Typed parameters: p0 recordId→RomRecordId_V308. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
RomRecordLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRegistry_FindRecordByIdCf(RomRecordId recordId)

{
  RomAssetRecordPrefix *pRVar1;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordLookupEaxCf5 RVar2;
  RomRecordLookupEaxCf5 RVar3;
  RomAssetRecordPrefix *candidateRecord;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((pRVar1 = slotCursor->record, pRVar1 == (RomAssetRecordPrefix *)0x0 ||
         (recordId != pRVar1->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      RVar3.carry = true;
      RVar3.recordOrError = (RomAssetRecordPrefix *)0x3c;
      return RVar3;
    }
  }
  RVar2.carry = false;
  RVar2.recordOrError = pRVar1;
  return RVar2;
}

