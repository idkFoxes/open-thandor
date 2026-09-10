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
void FrontendRomActionTable_ExecuteRecord
               (undefined4 param_1,undefined4 param_2,FrontendBooleanState32 suppressActivationSound
               ,RomRecordTableIndex recordIndex)

{
  RomAssetRecordPrefix *record;
  RomRecordByteSize RVar1;
  dword dVar2;
  RomRecordByteSize RVar3;
  int iVar4;
  RomAssetRecordPrefix *pRVar5;
  RomRecordId extraout_ECX;
  dword dVar6;
  RomRecordId transitionEnabled;
  RomRecordId extraout_EDX;
  RomRecordId RVar7;
  uint extraout_EDX_00;
  void *source;
  bool bVar8;
  undefined1 uVar9;
  RomRecordId RVar10;
  
  iVar4 = g_FrontendRootNode;
  if (recordIndex < *(uint *)(g_FrontendActiveRomRecordTable + 0x3c)) {
    record = (RomAssetRecordPrefix *)(recordIndex * 0x200 + 0x200 + g_FrontendActiveRomRecordTable);
    source = (void *)(g_FrontendRootNode + 0x368);
    dVar6 = record[3].rootNodeOffsetOrPointer;
    RVar7 = record[2].recordId;
    if ((((((RVar7 != 3) && (RVar7 != 4)) && (RVar7 != 9)) && (-1 < (int)RVar7)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
         SESSION_NETWORK_ROLE_LOCAL)) && ((RVar7 != 2 || (g_NetworkBackendInstanceCount != 0)))) {
      if ((record[3].recordId != 0) &&
         ((suppressActivationSound == 0 &&
          ((DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId] !=
           (DirectSoundVoiceSet *)0x0)))) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId]
                  );
        dVar6 = extraout_ECX;
        RVar7 = extraout_EDX;
      }
      if (dVar6 == 0) {
        if ((int)RVar7 < 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            UiActionQueue_Enqueue(0,source);
          }
        }
        else if (RVar7 != 0) {
          g_FrontendPendingPageAction = RVar7;
        }
      }
      else {
        RVar1 = record[3].byteSize;
        if ((dVar6 != 0) && (1 < (int)RVar1)) {
          RVar7 = *(RomRecordId *)(iVar4 + 0x3cc);
          record[5].rootNodeOffsetOrPointer = *(dword *)(iVar4 + 0x3c8);
          record[5].recordId = RVar7;
          dVar2 = *(dword *)(iVar4 + 0x3d4);
          record[6].byteSize = *(RomRecordByteSize *)(iVar4 + 0x3d0);
          record[6].rootNodeOffsetOrPointer = dVar2;
          RVar3 = *(RomRecordByteSize *)(iVar4 + 0x3dc);
          record[6].recordId = *(RomRecordId *)(iVar4 + 0x3d8);
          record[7].byteSize = RVar3;
          bVar8 = (int)((RVar1 - 1) * 0x10) < 0;
          record[7].rootNodeOffsetOrPointer = 0;
          pRVar5 = RomRegistry_FindRecordByIdCf(dVar6);
          if (!bVar8) {
            RVar7 = record[2].recordId;
            uVar9 = CARRY4((uint)record,extraout_EDX_00);
            RVar1 = pRVar5[3].byteSize;
            *(RomRecordId *)((int)&record[5].rootNodeOffsetOrPointer + extraout_EDX_00) =
                 pRVar5[2].recordId;
            *(RomRecordByteSize *)((int)&record[5].recordId + extraout_EDX_00) = RVar1;
            RVar10 = pRVar5[3].recordId;
            *(dword *)((int)&record[6].byteSize + extraout_EDX_00) =
                 pRVar5[3].rootNodeOffsetOrPointer;
            *(RomRecordId *)((int)&record[6].rootNodeOffsetOrPointer + extraout_EDX_00) = RVar10;
            dVar6 = pRVar5[4].rootNodeOffsetOrPointer;
            *(RomRecordByteSize *)((int)&record[6].recordId + extraout_EDX_00) = pRVar5[4].byteSize;
            *(dword *)((int)&record[7].byteSize + extraout_EDX_00) = dVar6;
            RVar10 = transitionEnabled;
            FrontendRomTransition_InitializeFromRecord(transitionEnabled,record);
            RomRuntime_UpdateRecordVisibilityAndDescriptorsCf(RVar7,RVar10);
            if ((bool)uVar9) {
              g_FrontendRomTransitionPendingCount = 0;
            }
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
dword RomAsset_PrepareRecords(RomAssetHeader *asset)

{
  dword registrationStatusCode;
  undefined4 extraout_EAX;
  int extraout_ECX;
  RomAssetHeader *record;
  bool bVar1;
  undefined8 uVar2;
  
  registrationStatusCode = ROM_ASSET_REGISTRATION_FAILURE_SENTINEL_0x3B;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ROM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ROM_00010005)) {
    record = asset + 1;
    bVar1 = false;
    if ((asset->recordCountHeader).recordCount != 0) {
      do {
        uVar2 = RomAssetRecord_RegisterAndRelocate((RomAssetRecordPrefix *)record,asset);
        registrationStatusCode = (dword)uVar2;
        if (bVar1) {
          return registrationStatusCode;
        }
        bVar1 = CARRY4((uint)record,(record->recordCountHeader).common.magic);
        record = (RomAssetHeader *)
                 ((record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((record->recordCountHeader).common.magic - 0x28));
      } while (extraout_ECX != 1);
    }
    return registrationStatusCode;
  }
  Package_SetLastErrorPath((word *)(u_Tengine_zentrale_rom_00545aa2 + 1));
  return extraout_EAX;
}

/* Address: 0x005466A0.
   Ownership: assets/rom/runtime.
   Purpose: Walks all 256 ROM registry slots, builds a runtime node tree for each populated record, stores the root
   in the paired registry slot, and links it into the owning world list.
   Local calls: RomRuntime_BuildNodeTreeRecursive.
   Cross-module calls: WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
void __fastcall
RomRuntime_BuildAllRegistryNodeTrees
          (undefined4 param_1,undefined4 param_2,WorldRuntimeContext *worldRuntime)

{
  RomAssetRecordPrefix *pRVar1;
  ModelRuntimeNode *modelNodeRuntime;
  int iVar2;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  undefined4 extraout_EDX;
  RomRegistrySlot *pRVar3;
  bool bVar4;
  undefined8 uVar5;
  
  iVar2 = 0x100;
  pRVar3 = g_RomRegistrySlots;
  do {
    pRVar1 = pRVar3->record;
    bVar4 = false;
    if (pRVar1 != (RomAssetRecordPrefix *)0x0) {
      uVar5 = RomRuntime_BuildNodeTreeRecursive
                        (iVar2,param_2,pRVar1[5].rootNodeOffsetOrPointer,
                         pRVar1->rootNodeOffsetOrPointer,worldRuntime);
      if (bVar4) {
        return;
      }
      pRVar3->runtimeRootNode = (WorldRuntimeNode *)uVar5;
      modelNodeRuntime =
           (ModelRuntimeNode *)WorldRuntime_LinkNodeIntoOwnerListD8((WorldRuntimeNode *)uVar5);
      uVar5 = ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX,extraout_EDX,modelNodeRuntime)
      ;
      param_2 = (undefined4)((ulonglong)uVar5 >> 0x20);
      iVar2 = extraout_ECX_00;
    }
    pRVar3 = pRVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}

/* Address: 0x00547FC0.
   Ownership: assets/rom/runtime.
   Purpose: Under the frontend ROM-transition spin lock, completes a pending transition request, activates the
   returned record ID when successful, frees the pending allocation, and clears the pending count.
   Local calls: FrontendRomTransition_ActivateRecordByIdCf.
   Cross-module calls: WorldMotionSpline_EvaluateAndApplyAtTime [core/math/interpolation].
*/
undefined4 __fastcall FrontendRomTransition_ProcessPendingRecord(undefined4 param_1)

{
  RomRecordId recordId;
  WorldRuntimeContext *worldRuntime;
  bool bVar1;
  
  (*g_SpinLockAcquire)(&g_FrontendStateTickSpinLock);
  worldRuntime = (WorldRuntimeContext *)(g_FrontendRootNode + 0x368);
  bVar1 = false;
  if (g_FrontendRomTransitionPendingCount != 0) {
    WorldMotionSpline_EvaluateAndApplyAtTime
              (g_FrontendRomTransitionSplineKeyframeCount,g_FrontendRomTransitionSplineKeyframes,
               g_FrontendRomTransitionElapsedTicks,worldRuntime);
    if ((!bVar1) && (g_FrontendRomTransitionPendingCount = 0, -1 < (int)recordId)) {
      FrontendRomTransition_ActivateRecordByIdCf(recordId,worldRuntime);
      (*g_FatalErrorPrimaryDispatchCf)();
    }
  }
  (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
  return param_1;
}

/* Address: 0x00547400.
   Ownership: assets/rom/runtime.
   Purpose: Walks exactly 256 ROM registry slots, releases nested resources reachable through each record root,
   then clears both dwords of every eight-byte slot. All saved registers and EAX are preserved.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __cdecl FrontendRomRegistry_ClearAndReleaseNestedResources(void)

{
  int iVar1;
  int extraout_ECX;
  int extraout_EDX;
  int iVar2;
  RomRegistrySlot *pRVar3;
  dword dVar4;
  
  iVar1 = 0x100;
  pRVar3 = g_RomRegistrySlots;
  while( true ) {
    if ((pRVar3->record != (RomAssetRecordPrefix *)0x0) &&
       (dVar4 = pRVar3->record->rootNodeOffsetOrPointer, dVar4 != 0)) break;
FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance:
    pRVar3->record = (RomAssetRecordPrefix *)0x0;
    pRVar3->runtimeRootNode = (WorldRuntimeNode *)0x0;
    pRVar3 = pRVar3 + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return;
    }
  }
  iVar2 = 0;
  do {
    Resource_Release(*(void **)(dVar4 + 0x2c));
    iVar2 = iVar2 + 1;
    while( true ) {
      if (extraout_ECX != 0) break;
      iVar2 = iVar2 + -1;
      if (iVar2 == 0)
      goto FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance;
    }
    dVar4 = *(dword *)(dVar4 + 0x14 + extraout_EDX * 4);
  } while( true );
}

/* Address: 0x00548720.
   Ownership: assets/rom/runtime.
   Purpose: If the ROM transition enable state is nonzero, publishes the exact transition state value 0x10000000.
   No registers or flags are deliberately replaced by the prototype.
*/
void __cdecl FrontendRomTransition_RequestStop(void)

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
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue)

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
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable)

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
undefined8 RomRecordTable_FindIndexById(RomRecordId recordId,void *table)

{
  int recordIndex;
  int recordsRemaining;
  undefined4 in_EDX;
  
  recordIndex = 0;
  recordsRemaining = *(int *)((int)table + 0x3c);
  for (; recordsRemaining != 0; recordsRemaining = recordsRemaining + -1) {
    if (recordId == *(RomRecordId *)((int)table + 0x21c))
    goto RomRecordTable_FindIndexById_ReturnFoundIndexOrMinusOne;
    recordIndex = recordIndex + 1;
    table = (void *)((int)table + 0x200);
  }
  recordIndex = -1;
RomRecordTable_FindIndexById_ReturnFoundIndexOrMinusOne:
  return CONCAT44(in_EDX,recordIndex);
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
void FrontendRomTransition_ActivateRecordByIdCf
               (RomRecordId recordId,WorldRuntimeContext *worldRuntime)

{
  WorldRuntimeNodeFlags *pWVar1;
  WorldRuntimeNode *pWVar2;
  RomAssetRecordPrefix *pRVar3;
  dword dVar4;
  RomRecordTableIndex entryIndex;
  int extraout_EAX;
  int extraout_EAX_00;
  int extraout_ECX;
  int extraout_ECX_00;
  RomAssetRecordPrefix *pRVar5;
  RomAssetRecordPrefix *extraout_EDX;
  int iVar6;
  RomRegistrySlot *pRVar7;
  undefined1 in_CF;
  bool bVar8;
  
  GraphicsShadingRuntime_ClearRecordTable();
  pRVar3 = RomRegistry_FindRecordByIdCf(recordId);
  if (!(bool)in_CF) {
    pRVar5 = pRVar3;
    g_FrontendActiveRomRecordTable = pRVar3;
    if (pRVar3[5].byteSize != 0) {
      do {
        bVar8 = (RomAssetRecordPrefix *)0xfffffdff < pRVar5;
        dVar4 = RomRegistry_FindSlotValueByRecordIdCf(pRVar5[0x2d].byteSize);
        if (!bVar8) {
          *(uint *)(dVar4 + 0x4c) = *(uint *)(dVar4 + 0x4c) | 0x20;
        }
        pRVar5 = (RomAssetRecordPrefix *)&pRVar5[0x2a].recordId;
      } while (extraout_ECX != 1);
    }
    iVar6 = 0x100;
    pRVar7 = g_RomRegistrySlots;
    do {
      pRVar5 = pRVar7->record;
      pWVar2 = pRVar7->runtimeRootNode;
      if ((pRVar5 != (RomAssetRecordPrefix *)0x0) &&
         ((pWVar1 = &pWVar2->runtimeFlags, *pWVar1 = *pWVar1 | 0x40, pRVar3 == pRVar5 ||
          (((&pRVar3[1].rootNodeOffsetOrPointer)[pRVar5->recordId >> 5] &
           1 << ((byte)pRVar5->recordId & 0x1f)) != 0)))) {
        pWVar1 = &pWVar2->runtimeFlags;
        *pWVar1 = *pWVar1 & 0xffffffbf;
        entryIndex = 0;
        if (pRVar5[4].recordId != 0) {
          do {
            RomRuntime_ApplyIndexedDescriptor(entryIndex,pRVar5);
            entryIndex = extraout_EAX + 1;
            pRVar5 = extraout_EDX;
          } while (extraout_ECX_00 != 1);
        }
      }
      pRVar7 = pRVar7 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (pRVar3[3].rootNodeOffsetOrPointer,pRVar3[3].byteSize,pRVar3[2].recordId,worldRuntime)
    ;
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,pRVar3[4].rootNodeOffsetOrPointer,pRVar3[4].byteSize,pRVar3[3].recordId,
               worldRuntime);
    iVar6 = g_FrontendPendingPageAction;
    if ((extraout_EAX_00 != 0) && (iVar6 = extraout_EAX_00, extraout_EAX_00 < 0)) {
      UiActionQueue_Enqueue(0,worldRuntime);
      iVar6 = g_FrontendPendingPageAction;
    }
    g_FrontendPendingPageAction = iVar6;
    return;
  }
  return;
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
void RomRuntime_UpdateRecordVisibilityAndDescriptorsCf
               (RomVisibilityFrontendValue frontendValue,RomRecordId recordId)

{
  WorldRuntimeNodeFlags *pWVar1;
  WorldRuntimeNode *pWVar2;
  RomAssetRecordPrefix *pRVar3;
  RomRecordTableIndex entryIndex;
  int extraout_EAX;
  int iVar4;
  uint uVar5;
  int extraout_ECX;
  RomAssetRecordPrefix *record;
  RomAssetRecordPrefix *extraout_EDX;
  RomRegistrySlot *pRVar6;
  bool bVar7;
  
  iVar4 = 0x100;
  g_FrontendRomTransitionContextValue = frontendValue;
  pRVar6 = g_RomRegistrySlots;
  do {
    if (pRVar6->runtimeRootNode != (WorldRuntimeNode *)0x0) {
      pWVar1 = &pRVar6->runtimeRootNode->runtimeFlags;
      *pWVar1 = *pWVar1 & 0xffffffdf;
    }
    bVar7 = (RomRegistrySlot *)0xfffffff7 < pRVar6;
    pRVar6 = pRVar6 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  pRVar3 = RomRegistry_FindRecordByIdCf(recordId);
  if (bVar7) {
    return;
  }
  GraphicsShadingRuntime_ClearRecordTable();
  iVar4 = 0x100;
  pRVar6 = g_RomRegistrySlots;
  do {
    record = pRVar6->record;
    pWVar2 = pRVar6->runtimeRootNode;
    if (record != (RomAssetRecordPrefix *)0x0) {
      pWVar1 = &pWVar2->runtimeFlags;
      *pWVar1 = *pWVar1 | 0x40;
      if ((record != pRVar3) && (record != g_FrontendActiveRomRecordTable)) {
        uVar5 = record->recordId >> 5;
        if ((1 << ((byte)record->recordId & 0x1f) &
            ((&pRVar3[1].rootNodeOffsetOrPointer)[uVar5] |
            (&g_FrontendActiveRomRecordTable[1].rootNodeOffsetOrPointer)[uVar5])) == 0)
        goto RomRuntime_UpdateRecordVisibilityAndDescriptorsCf_AdvanceAfterVisibilityDecision;
      }
      pWVar1 = &pWVar2->runtimeFlags;
      *pWVar1 = *pWVar1 & 0xffffffbf;
      entryIndex = 0;
      if (record[4].recordId != 0) {
        do {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,record);
          entryIndex = extraout_EAX + 1;
          record = extraout_EDX;
        } while (extraout_ECX != 1);
      }
    }
RomRuntime_UpdateRecordVisibilityAndDescriptorsCf_AdvanceAfterVisibilityDecision:
    pRVar6 = pRVar6 + 1;
    iVar4 = iVar4 + -1;
    if (iVar4 == 0) {
      return;
    }
  } while( true );
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
undefined8
RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase)

{
  int *piVar1;
  dword dVar2;
  SpriteAssetHeader *asset;
  SpriteAssetHeader *pSVar3;
  undefined4 in_EDX;
  int iVar4;
  RomRegistrySlot *pRVar5;
  int unaff_EBP;
  int unaff_ESI;
  byte *pbVar6;
  bool bVar7;
  
  iVar4 = 0x100;
  pRVar5 = g_RomRegistrySlots;
  do {
    if (pRVar5->record == (RomAssetRecordPrefix *)0x0) {
      dVar2 = record->rootNodeOffsetOrPointer;
      pRVar5->record = record;
      if (dVar2 == 0) {
RomAssetRecord_ReturnWithoutRootNode:
        return CONCAT44(in_EDX,assetBase);
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
  Package_SetLastErrorPath((word *)(u_Tengine_zentrale_rom_00545aa2 + 1));
  asset = (SpriteAssetHeader *)&k_LowAddressLiteral0000003B;
RomAssetRecord_ReturnRegistrationResult:
  return CONCAT44(in_EDX,asset);
RomAssetRecord_LoadOrReuseSpriteReference:
  bVar7 = false;
  asset = (SpriteAssetHeader *)WidePath_SetExtensionCode(0x727073,(word *)(pbVar6 + 0x34));
  if ((bVar7) || (asset = Package_LoadEntry((word *)(pbVar6 + 0x34)), bVar7))
  goto RomAssetRecord_ReturnRegistrationResult;
  pSVar3 = SpriteAssetRegistry_FindById((asset->registryHeader).registryId);
  bVar7 = false;
  if (pSVar3 == (SpriteAssetHeader *)0x0) {
    *(int *)(pbVar6 + 0x30) = *(int *)(pbVar6 + 0x30) + 1;
    *(SpriteAssetHeader **)(pbVar6 + 0x2c) = asset;
    asset = SpriteAsset_RegisterAndRelocatePointers(asset);
    if (bVar7) goto RomAssetRecord_ReturnRegistrationResult;
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
undefined8 __fastcall
RomRuntime_BuildNodeTreeRecursive
          (undefined4 param_1,undefined4 param_2,PackedArgb32 stateTintArgb,
          RomRuntimeNodeRecordAddress32 romNodeRecord,WorldRuntimeContext *worldObjectArray)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  uint *puVar11;
  bool bVar12;
  undefined8 uVar13;
  
  bVar12 = &stack0xffffffe8 < (undefined1 *)0x4;
  iVar6 = WorldObjectArray_AllocateFreeRecordCf(worldObjectArray);
  iVar7 = iVar6;
  if (bVar12) {
RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet:
    return CONCAT44(param_2,iVar7);
  }
  *(undefined4 *)(iVar6 + 0x18) = 0;
  *(undefined4 *)(iVar6 + 0x1c) = 0;
  *(undefined4 *)(iVar6 + 0x20) = 0;
  *(undefined4 *)(iVar6 + 0x94) = 0;
  *(undefined4 *)(iVar6 + 0x98) = 0;
  *(undefined4 *)(iVar6 + 0x9c) = 0;
  uVar1 = *(undefined4 *)(romNodeRecord + 4);
  uVar2 = *(undefined4 *)(romNodeRecord + 8);
  uVar3 = *(undefined4 *)(romNodeRecord + 0xc);
  *(undefined4 *)(iVar6 + 0x24) = uVar1;
  *(undefined4 *)(iVar6 + 0x28) = uVar2;
  *(undefined4 *)(iVar6 + 0x2c) = uVar3;
  *(undefined4 *)(iVar6 + 0xc) = uVar1;
  *(undefined4 *)(iVar6 + 0x10) = uVar2;
  *(undefined4 *)(iVar6 + 0x14) = uVar3;
  *(undefined4 *)(iVar6 + 0x44) = 0xffffffff;
  *(uint *)(iVar6 + 0x4c) = *(uint *)(iVar6 + 0x4c) | 1;
  *(undefined4 *)(iVar6 + 0x50) = 0;
  *(PackedArgb32 *)(iVar6 + 0x58) = stateTintArgb;
  uVar2 = g_FrontendCentralTextureSet;
  iVar7 = *(int *)(romNodeRecord + 0x2c);
  *(undefined4 *)(iVar6 + 0x30) = g_FrontendCentralPaletteAsset;
  uVar1 = *(undefined4 *)(iVar7 + 0xd8);
  *(undefined4 *)(iVar6 + 0x34) = uVar2;
  *(undefined4 *)(iVar6 + 0x54) = uVar1;
  *(int *)(iVar6 + 0x40) = iVar7;
  *(undefined4 *)(iVar6 + 0x5c) = 0;
  *(undefined4 *)(iVar6 + 0x60) = 0;
  *(int *)(iVar6 + 0xa0) = iVar7 + 0x200;
  iVar8 = *(int *)(romNodeRecord + 0x10);
  iVar4 = *(int *)(romNodeRecord + 0x2c);
  uVar9 = 0;
  *(int *)(iVar6 + 200) = iVar8;
  *(undefined4 *)(iVar6 + 0xc4) = 0;
  do {
    if (iVar8 == 0) {
      return CONCAT44(param_2,iVar6);
    }
    puVar11 = (uint *)(iVar4 + *(int *)(iVar4 + 0xe4));
    for (iVar7 = *(int *)(iVar4 + 0xe8); iVar7 != 0; iVar7 = iVar7 + -1) {
      if (((*puVar11 & 0xf) == 0) && (bVar12 = uVar9 < *puVar11 >> 4, uVar9 == *puVar11 >> 4)) {
        uVar13 = RomRuntime_BuildNodeTreeRecursive
                           (iVar7,uVar9,stateTintArgb,
                            *(RomRuntimeNodeRecordAddress32 *)(romNodeRecord + 0x14 + uVar9 * 4),
                            worldObjectArray);
        iVar10 = (int)((ulonglong)uVar13 >> 0x20);
        iVar7 = (int)uVar13;
        if (bVar12)
        goto RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet;
        *(int *)(iVar6 + 0xcc + iVar10 * 4) = iVar7;
        *(int *)(iVar7 + 0xc4) = iVar6;
        uVar9 = puVar11[2];
        uVar5 = puVar11[3];
        *(uint *)(iVar7 + 0x18) = puVar11[1];
        *(uint *)(iVar7 + 0x1c) = uVar9;
        *(uint *)(iVar7 + 0x20) = uVar5;
        goto RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor;
      }
      puVar11 = puVar11 + 4;
    }
    *(int *)(iVar6 + 200) = *(int *)(iVar6 + 200) + -1;
    iVar10 = uVar9 - 1;
RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor:
    uVar9 = iVar10 + 1;
    iVar8 = iVar8 + -1;
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
void FrontendRomTransition_InitializeFromRecord
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
dword RomRegistry_FindSlotValueByRecordIdCf(RomRecordId recordId)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((slotCursor->record == (RomAssetRecordPrefix *)0x0 ||
         (recordId != slotCursor->record->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      return 0x3c;
    }
  }
  return (dword)slotCursor->runtimeRootNode;
}

/* Address: 0x00548410.
   Ownership: assets/rom/runtime.
   Purpose: Bounds-checks an entry index against record dword +0x38, scans the associated descriptor table for type
   4 with the same high index, and forwards the record and descriptor values to the existing graphics runtime
   helper. EAX is preserved. Typed parameters: p0 entryIndex→RomRecordTableIndex_V331. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: GraphicsShadingRuntime_AllocateRecordRegs [graphics/render/shading].
*/
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record)

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
RomAssetRecordPrefix * RomRegistry_FindRecordByIdCf(RomRecordId recordId)

{
  RomAssetRecordPrefix *pRVar1;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomAssetRecordPrefix *candidateRecord;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while( true ) {
    pRVar1 = slotCursor->record;
    if ((pRVar1 != (RomAssetRecordPrefix *)0x0) && (recordId == pRVar1->recordId)) break;
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      return (RomAssetRecordPrefix *)0x3c;
    }
  }
  return pRVar1;
}
