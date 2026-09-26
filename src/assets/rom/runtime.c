/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/rom/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/rom/runtime.h>
#include <thandor/thandor.h>

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
  dword copiedDword;
  RomRecordByteSize copiedByteSize;
  RomRecordId copiedRecordId;
  int frontendRootNode;
  RomAssetRecordPrefix *targetRecord;
  int transitionSlotIndex;
  void *source;
  bool visibilityLookupFailed;
  RomRecordLookupEaxCf5 targetLookup;
  RomRecordId actionOrCopiedValue;
  RomRecordId recordId;
  
  frontendRootNode = g_FrontendRootNode;
  if (recordIndex < *(uint *)(g_FrontendActiveRomRecordTable + 0x3c)) {
    record = (RomAssetRecordPrefix *)(recordIndex * 0x200 + 0x200 + g_FrontendActiveRomRecordTable);
    source = (void *)(g_FrontendRootNode + 0x368);
    recordId = record[3].rootNodeOffsetOrPointer;
    actionOrCopiedValue = record[2].recordId;
    if ((((((actionOrCopiedValue != 3) && (actionOrCopiedValue != 4)) && (actionOrCopiedValue != 9)) && (-1 < (int)actionOrCopiedValue)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
         SESSION_NETWORK_ROLE_LOCAL)) && ((actionOrCopiedValue != 2 || (g_NetworkBackendInstanceCount != 0)))) {
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
        if ((int)actionOrCopiedValue < 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            UiActionQueue_Enqueue(0,source);
          }
        }
        else if (actionOrCopiedValue != 0) {
          g_FrontendPendingPageAction = actionOrCopiedValue;
        }
      }
      else if ((recordId != 0) && (1 < (int)record[3].byteSize)) {
        transitionSlotIndex = record[3].byteSize - 1;
        actionOrCopiedValue = *(RomRecordId *)(frontendRootNode + 0x3cc);
        record[5].rootNodeOffsetOrPointer = *(dword *)(frontendRootNode + 0x3c8);
        record[5].recordId = actionOrCopiedValue;
        copiedDword = *(dword *)(frontendRootNode + 0x3d4);
        record[6].byteSize = *(RomRecordByteSize *)(frontendRootNode + 0x3d0);
        record[6].rootNodeOffsetOrPointer = copiedDword;
        copiedByteSize = *(RomRecordByteSize *)(frontendRootNode + 0x3dc);
        record[6].recordId = *(RomRecordId *)(frontendRootNode + 0x3d8);
        record[7].byteSize = copiedByteSize;
        record[7].rootNodeOffsetOrPointer = 0;
        targetLookup = RomRegistry_FindRecordByIdCf(recordId);
        targetRecord = targetLookup.recordOrError;
        if (!targetLookup.carry) {
          actionOrCopiedValue = record[2].recordId;
          copiedByteSize = targetRecord[3].byteSize;
          *(RomRecordId *)((int)record + transitionSlotIndex * 0x20 + 0x40) = targetRecord[2].recordId;
          *(RomRecordByteSize *)((int)record + transitionSlotIndex * 0x20 + 0x44) = copiedByteSize;
          copiedRecordId = targetRecord[3].recordId;
          *(dword *)((int)record + transitionSlotIndex * 0x20 + 0x48) = targetRecord[3].rootNodeOffsetOrPointer;
          *(RomRecordId *)((int)record + transitionSlotIndex * 0x20 + 0x4c) = copiedRecordId;
          copiedDword = targetRecord[4].rootNodeOffsetOrPointer;
          *(RomRecordByteSize *)((int)record + transitionSlotIndex * 0x20 + 0x50) = targetRecord[4].byteSize;
          *(dword *)((int)record + transitionSlotIndex * 0x20 + 0x54) = copiedDword;
          FrontendRomTransition_InitializeFromRecord(recordId,record);
          visibilityLookupFailed = RomRuntime_UpdateRecordVisibilityAndDescriptorsCf(actionOrCopiedValue,recordId);
          if (visibilityLookupFailed) {
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
  AssetRecordCount recordsRemaining;
  RomAssetHeader *record;
  StatusValueEaxCf5 registerResult;
  StatusValueEaxCf5 failureResult;
  
  registrationStatusCode = ROM_ASSET_REGISTRATION_FAILURE_SENTINEL_0x3B;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ROM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ROM_00010005)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (recordsRemaining == 0) {
        registerResult.carry = false;
        registerResult.valueOrError = registrationStatusCode;
        return registerResult;
      }
      registerResult = RomAssetRecord_RegisterAndRelocate((RomAssetRecordPrefix *)record,asset);
      registrationStatusCode = registerResult.valueOrError;
      if (registerResult.carry) break;
      record = (RomAssetHeader *)
               ((record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  else {
    Package_SetLastErrorPath((word *)u_engine_zentrale_rom_00545aa4);
  }
  failureResult.carry = true;
  failureResult.valueOrError = registrationStatusCode;
  return failureResult;
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
  RomAssetRecordPrefix *slotRecord;
  ModelRuntimeNode *modelNodeRuntime;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  ModelNodeCreateEaxCf5 buildResult;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    slotRecord = slotCursor->record;
    if (slotRecord != (RomAssetRecordPrefix *)0x0) {
      buildResult = RomRuntime_BuildNodeTreeRecursive
                        (slotRecord[5].rootNodeOffsetOrPointer,
                         (RomSerializedNodeHeader34 *)slotRecord->rootNodeOffsetOrPointer,worldRuntime);
      modelNodeRuntime = buildResult.modelNode;
      if (buildResult.carry) {
        return true;
      }
      slotCursor->runtimeRootNode = (WorldRuntimeNode *)modelNodeRuntime;
      WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)modelNodeRuntime);
      ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
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
  bool splineStillRunning;
  StatusValueEaxCf5 activateResult;
  
  (*g_SpinLockAcquire)(&g_FrontendStateTickSpinLock);
  recordId = g_FrontendRomTransitionPendingCount;
  worldRuntime = (WorldRuntimeContext *)(g_FrontendRootNode + 0x368);
  if (g_FrontendRomTransitionPendingCount != 0) {
    splineStillRunning = WorldMotionSpline_EvaluateAndApplyAtTime
                      (g_FrontendRomTransitionSplineKeyframeCount,
                       g_FrontendRomTransitionSplineKeyframes,g_FrontendRomTransitionElapsedTicks,
                       worldRuntime);
    if ((!splineStillRunning) && (g_FrontendRomTransitionPendingCount = 0, -1 < (int)recordId)) {
      activateResult = FrontendRomTransition_ActivateRecordByIdCf(recordId,worldRuntime);
      (*g_FatalErrorPrimaryDispatchCf)(activateResult.valueOrError,activateResult.carry);
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
  int nodeChildCount;
  int slotsRemaining;
  int releaseDepth;
  RomRegistrySlot *slotCursor;
  dword nodeAddress;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while( true ) {
    if ((slotCursor->record != (RomAssetRecordPrefix *)0x0) &&
       (nodeAddress = slotCursor->record->rootNodeOffsetOrPointer, nodeAddress != 0)) break;
FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance:
    slotCursor->record = (RomAssetRecordPrefix *)0x0;
    slotCursor->runtimeRootNode = (WorldRuntimeNode *)0x0;
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      return;
    }
  }
  releaseDepth = 0;
  do {
    nodeChildCount = *(int *)(nodeAddress + 0x10);
    Resource_Release(*(void **)(nodeAddress + 0x2c));
    releaseDepth = releaseDepth + 1;
    while( true ) {
      if (nodeChildCount != 0) break;
      releaseDepth = releaseDepth + -1;
      if (releaseDepth == 0)
      goto FrontendRomRegistry_ClearAndReleaseNestedResources_ClearCurrentSlotAndAdvance;
    }
    nodeAddress = *(dword *)(nodeAddress + 0x14);
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
  uint *slotNodeFlags;
  WorldRuntimeNodeFlags *nodeFlags;
  RomAssetRecordPrefix *recordCursor;
  WorldRuntimeNode *rootNode;
  uint transitionContextValue;
  RomRecordTableIndex entryIndex;
  RomRecordByteSize companionsRemaining;
  RomRecordId descriptorsRemaining;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordLookupEaxCf5 recordLookup;
  StatusValueEaxCf5 statusResult;
  StatusValueEaxCf5 activeRecordResult;
  
  GraphicsShadingRuntime_ClearRecordTable();
  recordLookup = RomRegistry_FindRecordByIdCf(recordId);
  activeRecordResult.valueOrError = recordLookup.recordOrError;
  if (!recordLookup.carry) {
    recordCursor = activeRecordResult.valueOrError;
    g_FrontendActiveRomRecordTable = activeRecordResult.valueOrError;
    slotCursor = g_RomRegistrySlots;
    for (companionsRemaining = ((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[5].byteSize; g_RomRegistrySlots = slotCursor, companionsRemaining != 0;
        companionsRemaining = companionsRemaining - 1) {
      statusResult = RomRegistry_FindSlotValueByRecordIdCf(recordCursor[0x2d].byteSize);
      if (!statusResult.carry) {
        slotNodeFlags = (uint *)(statusResult.valueOrError + 0x4c);
        *slotNodeFlags = *slotNodeFlags | 0x20;
      }
      recordCursor = (RomAssetRecordPrefix *)&recordCursor[0x2a].recordId;
      slotCursor = g_RomRegistrySlots;
    }
    slotsRemaining = 0x100;
    do {
      recordCursor = slotCursor->record;
      rootNode = slotCursor->runtimeRootNode;
      if ((recordCursor != (RomAssetRecordPrefix *)0x0) &&
         ((nodeFlags = &rootNode->runtimeFlags, *nodeFlags = *nodeFlags | 0x40, activeRecordResult.valueOrError == recordCursor
          || (((&((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[1].rootNodeOffsetOrPointer)[recordCursor->recordId >> 5] &
              1 << ((byte)recordCursor->recordId & 0x1f)) != 0)))) {
        nodeFlags = &rootNode->runtimeFlags;
        *nodeFlags = *nodeFlags & 0xffffffbf;
        entryIndex = 0;
        for (descriptorsRemaining = recordCursor[4].recordId; descriptorsRemaining != 0; descriptorsRemaining = descriptorsRemaining - 1) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,recordCursor);
          entryIndex = entryIndex + 1;
        }
      }
      transitionContextValue = g_FrontendRomTransitionContextValue;
      slotCursor = slotCursor + 1;
      slotsRemaining = slotsRemaining + -1;
    } while (slotsRemaining != 0);
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[3].rootNodeOffsetOrPointer,((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[3].byteSize,
               ((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[2].recordId,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[4].rootNodeOffsetOrPointer,((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[4].byteSize,
               ((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[3].recordId,worldRuntime);
    if (transitionContextValue != 0) {
      if ((int)transitionContextValue < 0) {
        UiActionQueue_Enqueue(0,worldRuntime);
      }
      else {
        g_FrontendPendingPageAction = transitionContextValue;
      }
    }
    statusResult.carry = false;
    statusResult.valueOrError = transitionContextValue;
    return statusResult;
  }
  activeRecordResult.carry = true;
  return activeRecordResult;
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
  WorldRuntimeNodeFlags *nodeFlags;
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *rootNode;
  RomRecordTableIndex entryIndex;
  int slotsRemaining;
  uint maskWordIndex;
  RomRecordId descriptorsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordLookupEaxCf5 recordLookup;
  
  slotsRemaining = 0x100;
  g_FrontendRomTransitionContextValue = frontendValue;
  slotCursor = g_RomRegistrySlots;
  do {
    if (slotCursor->runtimeRootNode != (WorldRuntimeNode *)0x0) {
      nodeFlags = &slotCursor->runtimeRootNode->runtimeFlags;
      *nodeFlags = *nodeFlags & 0xffffffdf;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  recordLookup = RomRegistry_FindRecordByIdCf(recordId);
  if (!recordLookup.carry) {
    GraphicsShadingRuntime_ClearRecordTable();
    slotsRemaining = 0x100;
    slotCursor = g_RomRegistrySlots;
    do {
      record = slotCursor->record;
      rootNode = slotCursor->runtimeRootNode;
      if ((record != (RomAssetRecordPrefix *)0x0) &&
         (((nodeFlags = &rootNode->runtimeFlags, *nodeFlags = *nodeFlags | 0x40, record == recordLookup.recordOrError
           || (record == g_FrontendActiveRomRecordTable)) ||
          (maskWordIndex = record->recordId >> 5,
          (1 << ((byte)record->recordId & 0x1f) &
          ((&recordLookup.recordOrError[1].rootNodeOffsetOrPointer)[maskWordIndex] |
          (&((RomAssetRecordPrefix *)(uintptr_t)g_FrontendActiveRomRecordTable)[1].rootNodeOffsetOrPointer)[maskWordIndex])) != 0)))) {
        nodeFlags = &rootNode->runtimeFlags;
        *nodeFlags = *nodeFlags & 0xffffffbf;
        entryIndex = 0;
        for (descriptorsRemaining = record[4].recordId; descriptorsRemaining != 0; descriptorsRemaining = descriptorsRemaining - 1) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,record);
          entryIndex = entryIndex + 1;
        }
      }
      slotCursor = slotCursor + 1;
      slotsRemaining = slotsRemaining + -1;
    } while (slotsRemaining != 0);
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
  int *unusedCounter;
  dword rootNodeOffset;
  RomAssetHeader *asset;
  SpriteAssetHeader *existingSprite;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  byte *rootSerializedNode;
  bool unusedFlag;
  StatusValueEaxCf5 failureResult;
  PackageLoadEntryEaxCf5 loadResult;
  SpriteRegisterRelocateEaxCf5 registerResult;
  StatusValueEaxCf5 successResult;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    if (slotCursor->record == (RomAssetRecordPrefix *)0x0) {
      rootNodeOffset = record->rootNodeOffsetOrPointer;
      slotCursor->record = record;
      if (rootNodeOffset == 0) {
RomAssetRecord_ReturnWithoutRootNode:
        successResult.carry = false;
        successResult.valueOrError = (dword)assetBase;
        return successResult;
      }
      record->rootNodeOffsetOrPointer =
           (dword)((assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28
                  + (record->rootNodeOffsetOrPointer - 0x28));
      rootSerializedNode = (assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               (rootNodeOffset - 0x28);
      /* Rewritten from the assembly (0x005463A1-0x00546436): depth-first walk over the serialized
         sprite-node tree. The original keeps {node, nextChild, remaining} frames on the machine stack
         (EBX = depth), which Ghidra could only show as unaff_ESI/unaff_EBP. Child offsets are
         relative to assetBase and are relocated in place while walking. */
      {
        struct { byte *node; dword nextChild; dword remaining; } frames[64];
        int depth = 0;
        byte *node = rootSerializedNode;
        for (;;) {
          WidePath_SetExtensionCode(0x727073,(word *)(node + 0x34)); /* ".spr"; never sets CF */
          loadResult = Package_LoadEntry((word *)(node + 0x34));
          if (loadResult.carry) {
            failureResult.carry = true;
            failureResult.valueOrError = (dword)loadResult.bufferOrError;
            return failureResult;
          }
          asset = loadResult.bufferOrError;
          existingSprite = SpriteAssetRegistry_FindById(*(SpriteAssetId *)((byte *)asset + 0xb8));
          if (existingSprite != (SpriteAssetHeader *)0x0) {
            *(SpriteAssetHeader **)(node + 0x2c) = existingSprite;
            Resource_Release(asset);
          }
          else {
            *(int *)(node + 0x30) = *(int *)(node + 0x30) + 1;
            *(RomAssetHeader **)(node + 0x2c) = asset;
            registerResult = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
            if (registerResult.carry) {
              failureResult.carry = true;
              failureResult.valueOrError = (dword)registerResult.assetOrError;
              return failureResult;
            }
          }
          frames[depth].node = node;
          frames[depth].nextChild = 0;
          frames[depth].remaining = *(dword *)(node + 0x10);
          depth++;
          while (frames[depth - 1].remaining == 0) {
            depth--;
            if (depth == 0) goto RomAssetRecord_ReturnWithoutRootNode;
          }
          {
            dword *child = (dword *)(frames[depth - 1].node + 0x14) + frames[depth - 1].nextChild;
            *child = *child + (dword)assetBase;
            frames[depth - 1].nextChild++;
            frames[depth - 1].remaining--;
            node = (byte *)*child;
          }
        }
      }
    }
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  Package_SetLastErrorPath((word *)u_engine_zentrale_rom_00545aa4);
  asset = (RomAssetHeader *)&k_LowAddressLiteral0000003B;
RomAssetRecord_ReturnRegistrationResult:
  failureResult.carry = true;
  failureResult.valueOrError = (dword)asset;
  return failureResult;
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
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  ModelResourceHitTestAndRenderView210 *spriteModelResource;
  Q12 boundingRadius;
  uint translationY;
  uint translationZ;
  GraphicsTextureSet *centralTextureSet;
  ModelRuntimeNode *newNode;
  ModelRuntimeNode *resultOrChildNode;
  ModelPackedLookupTableEntryCount lookupEntriesRemaining;
  dword childSlotsRemaining;
  uint childIndex;
  byte *lookupEntry;
  WorldObjectRecordEaxCf5 allocResult;
  ModelNodeCreateEaxCf5 createResult;
  
  allocResult = WorldObjectArray_AllocateFreeRecordCf(worldObjectArray);
  newNode = (ModelRuntimeNode *)allocResult.recordOrError;
  resultOrChildNode = newNode;
  if (allocResult.carry) {
RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet:
    createResult.carry = true;
    createResult.modelNode = resultOrChildNode;
    return createResult;
  }
  (newNode->modelPayload).localTranslationXQ12 = 0;
  (newNode->modelPayload).localTranslationYQ12 = 0;
  (newNode->modelPayload).localTranslationZQ12 = 0;
  (newNode->worldTransform).translation.x = 0;
  (newNode->worldTransform).translation.y = 0;
  (newNode->worldTransform).translation.z = 0;
  rotationAngle0 = romNodeRecord->localRotationAngle0;
  rotationAngle1 = romNodeRecord->localRotationAngle1;
  rotationAngle2 = romNodeRecord->localRotationAngle2;
  (newNode->modelPayload).localRotationAngle0 = rotationAngle0;
  (newNode->modelPayload).localRotationAngle1 = rotationAngle1;
  (newNode->modelPayload).localRotationAngle2 = rotationAngle2;
  (newNode->modelPayload).worldRotationAngle0 = rotationAngle0;
  (newNode->modelPayload).worldRotationAngle1 = rotationAngle1;
  (newNode->modelPayload).worldRotationAngle2 = rotationAngle2;
  (newNode->modelPayload).meshGroupMask = 0xffffffff;
  newNode->runtimeFlags = newNode->runtimeFlags | 1;
  *(byte *)&newNode->textureSubresourceBaseIndex = 0;
  *(byte *)((int)&newNode->textureSubresourceBaseIndex + 1) = 0;
  *(byte *)((int)&newNode->textureSubresourceBaseIndex + 2) = 0;
  *(byte *)((int)&newNode->textureSubresourceBaseIndex + 3) = 0;
  newNode->tintArgb = stateTintArgb;
  centralTextureSet = g_FrontendCentralTextureSet;
  spriteModelResource = (romNodeRecord->spriteAssetReference).modelResource;
  (newNode->modelPayload).paletteAsset = g_FrontendCentralPaletteAsset;
  boundingRadius = spriteModelResource->boundingRadiusQ12;
  (newNode->modelPayload).textureSet = centralTextureSet;
  newNode->subtreeBoundingRadiusQ12 = boundingRadius;
  (newNode->modelPayload).modelResource = spriteModelResource;
  newNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
  newNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
  newNode->runtimeStateA0 = (dword)&spriteModelResource->firstMeshGroupRelativeOffset;
  childSlotsRemaining = romNodeRecord->childCount;
  spriteModelResource = (romNodeRecord->spriteAssetReference).modelResource;
  childIndex = 0;
  newNode->childCount = childSlotsRemaining;
  newNode->parentNode = (ModelRuntimeNode *)0x0;
  do {
    if (childSlotsRemaining == 0) {
      return THANDOR_BITCAST(qword, ModelNodeCreateEaxCf5, ((THANDOR_BITCAST(WorldObjectRecordEaxCf5, qword, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
    lookupEntry = spriteModelResource->reserved00_AF + spriteModelResource->packedLookupTableRelativeOffset;
    for (lookupEntriesRemaining = spriteModelResource->packedLookupTableEntryCount; lookupEntriesRemaining != 0; lookupEntriesRemaining = lookupEntriesRemaining - 1) {
      if (((*(uint *)lookupEntry & 0xf) == 0) && (childIndex == *(uint *)lookupEntry >> 4)) {
        createResult = RomRuntime_BuildNodeTreeRecursive
                           (stateTintArgb,romNodeRecord->childReferences[childIndex].node,
                            worldObjectArray);
        resultOrChildNode = createResult.modelNode;
        if (createResult.carry)
        goto RomRuntime_BuildNodeTreeRecursive_ReturnAllocationOrRecursiveChildFailureWithCarrySet;
        newNode->childNodes[childIndex] = resultOrChildNode;
        resultOrChildNode->parentNode = newNode;
        translationY = *(uint *)(lookupEntry + 8);
        translationZ = *(uint *)(lookupEntry + 0xc);
        (resultOrChildNode->modelPayload).localTranslationXQ12 = *(uint *)(lookupEntry + 4);
        (resultOrChildNode->modelPayload).localTranslationYQ12 = translationY;
        (resultOrChildNode->modelPayload).localTranslationZQ12 = translationZ;
        goto RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor;
      }
      lookupEntry = lookupEntry + 0x10;
    }
    newNode->childCount = newNode->childCount - 1;
    childIndex = childIndex - 1;
RomRuntime_BuildNodeTreeRecursive_AdvanceChildSlotAfterResolvedOrMissingDescriptor:
    childIndex = childIndex + 1;
    childSlotsRemaining = childSlotsRemaining - 1;
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
  StatusValueEaxCf5 foundResult;
  StatusValueEaxCf5 missResult;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((slotCursor->record == (RomAssetRecordPrefix *)0x0 ||
         (recordId != slotCursor->record->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      missResult.carry = true;
      missResult.valueOrError = 0x3c;
      return missResult;
    }
  }
  foundResult.carry = false;
  foundResult.valueOrError = (dword)slotCursor->runtimeRootNode;
  return foundResult;
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
  RomAssetRecordPrefix *slotRecord;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordLookupEaxCf5 foundResult;
  RomRecordLookupEaxCf5 missResult;
  RomAssetRecordPrefix *candidateRecord;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((slotRecord = slotCursor->record, slotRecord == (RomAssetRecordPrefix *)0x0 ||
         (recordId != slotRecord->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      missResult.carry = true;
      missResult.recordOrError = (RomAssetRecordPrefix *)0x3c;
      return missResult;
    }
  }
  foundResult.carry = false;
  foundResult.recordOrError = slotRecord;
  return foundResult;
}

