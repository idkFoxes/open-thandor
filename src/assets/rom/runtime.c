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
   Executes entry recordIndex of the active frontend ROM action table (a menu-room hotspot or a scripted entry
   from the frontend main loop): plays its click sound, then either posts its page action / a close request, or
   starts a camera flight from the current menu-room camera pose to the pose of its target ROM record.
   Gameplay settings, page 9, credits and closing are refused in network sessions, network setup without a
   network backend.
*/
void FrontendRomActionTable_ExecuteRecord
          (uint32_t reservedZero0,uint32_t reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex)

{
  /* Action entry layout (FRONTEND_ROM_ACTION_* offsets): viewed as RomAssetRecordPrefix[] (12-byte elements),
     record[2].recordId = +0x20 page action, record[3].byteSize = +0x24 keyframe count,
     record[3].rootNodeOffsetOrPointer = +0x28 target ROM record id, record[3].recordId = +0x2C sound index,
     +0x40 keyframes of 0x20 bytes each (record[5]..record[7] = keyframe 0). */
  RomAssetRecordPrefix *record;
  uint32_t copiedDword;
  RomRecordByteSize copiedByteSize;
  RomRecordId copiedRecordId;
  int frontendRootNode;
  RomAssetRecordPrefix *targetRecord;
  int lastKeyframeIndex;
  void *menuRoomView;
  bool visibilityLookupFailed;
  RomRecordResult targetLookup;
  RomRecordId pageActionOrCopiedDword;
  RomRecordId targetRecordId;

  frontendRootNode = g_FrontendRootNode;
  if (recordIndex < *(uint32_t *)(g_FrontendActiveRomRecordTable + FRONTEND_ROM_ACTION_TABLE_COUNT_OFFSET)) {
    record = (RomAssetRecordPrefix *)(recordIndex * FRONTEND_ROM_ACTION_ENTRY_SIZE + FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE +
                                      g_FrontendActiveRomRecordTable);
    menuRoomView = FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
    targetRecordId = record[3].rootNodeOffsetOrPointer;
    pageActionOrCopiedDword = record[2].recordId;
    if ((((((pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) &&
            (pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE)) &&
           (pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_CREDITS)) && (-1 < (int)pageActionOrCopiedDword)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
         SESSION_NETWORK_ROLE_LOCAL)) &&
       ((pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE || (g_NetworkBackendInstanceCount != 0)))) {
      if ((record[3].recordId != 0) &&
         ((suppressActivationSound == 0 &&
          ((DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId] != NULL)))) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[record[3].recordId]
                  );
      }
      if (targetRecordId == 0) {
        if ((int)pageActionOrCopiedDword < 0) {
          /* A negative action closes the menu-room view (only outside network sessions). */
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            UiActionQueue_Enqueue(0,menuRoomView);
          }
        }
        else if (pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_NONE) {
          g_FrontendPendingPageAction = pageActionOrCopiedDword;
        }
      }
      else if ((targetRecordId != 0) && (1 < (int)record[3].byteSize)) {
        lastKeyframeIndex = record[3].byteSize - 1;
        /* Keyframe 0 (record[5].rootNodeOffsetOrPointer = keyframes[0]) = snapshot of the menu room camera:
           WorldRuntimeContext.motion of menuRoomModelView, positionX/Y/Z, positionMagnitude, headingAngle,
           pitchAngle; its timeQ12 is 0. */
        pageActionOrCopiedDword =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionYQ12;
        record[5].rootNodeOffsetOrPointer =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionXQ12;
        record[5].recordId = pageActionOrCopiedDword;
        copiedDword =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionMagnitudeQ12;
        record[6].byteSize =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionZQ12;
        record[6].rootNodeOffsetOrPointer = copiedDword;
        copiedByteSize =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.pitchAngle;
        record[6].recordId =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.headingAngle;
        record[7].byteSize = copiedByteSize;
        record[7].rootNodeOffsetOrPointer = 0;
        targetLookup = RomRegistry_FindRecordById(targetRecordId);
        targetRecord = targetLookup.recordOrError;
        if (!targetLookup.notFound) {
          pageActionOrCopiedDword = record[2].recordId;
          /* The six channels of the last keyframe become the target record's camera pose (+0x20..+0x34); its
             timeQ12 comes from the action entry. */
          copiedByteSize = targetRecord[3].byteSize;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel0Q12 =
               targetRecord[2].recordId;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel1Q12 =
               copiedByteSize;
          copiedRecordId = targetRecord[3].recordId;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel2Q12 =
               targetRecord[3].rootNodeOffsetOrPointer;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel3Q12 =
               copiedRecordId;
          copiedDword = targetRecord[4].rootNodeOffsetOrPointer;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel4Q12 =
               targetRecord[4].byteSize;
          ((FrontendRomActionEntry *)record)->keyframes[lastKeyframeIndex].channel5Q12 = copiedDword;
          /* The first argument is stored as the pending transition value, i.e. the record to activate when
             the camera flight ends (see FrontendRomTransition_ProcessPendingRecord). */
          FrontendRomTransition_InitializeFromRecord(targetRecordId,record);
          visibilityLookupFailed =
               RomRuntime_UpdateRecordVisibilityAndDescriptors(pageActionOrCopiedDword,targetRecordId);
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
StatusResult RomAsset_PrepareRecords(RomAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  RomAssetHeader *record;
  StatusResult registerResult;
  StatusResult failureResult;
  
  registrationStatusCode = ROM_ASSET_REGISTRATION_FAILURE_SENTINEL_0x3B;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ROM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ROM_00010005)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (recordsRemaining == 0) {
        registerResult.failed = false;
        registerResult.valueOrError = registrationStatusCode;
        return registerResult;
      }
      registerResult = RomAssetRecord_RegisterAndRelocate((RomAssetRecordPrefix *)record,asset);
      registrationStatusCode = registerResult.valueOrError;
      if (registerResult.failed) break;
      record = (RomAssetHeader *)
               ((record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)u_engine_zentrale_rom_00545aa4);
  }
  failureResult.failed = true;
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
  ModelNodeCreateResult buildResult;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    slotRecord = slotCursor->record;
    if (slotRecord != (RomAssetRecordPrefix *)0x0) {
      buildResult = RomRuntime_BuildNodeTreeRecursive
                        (slotRecord[5].rootNodeOffsetOrPointer,
                         (RomSerializedNodeHeader34 *)slotRecord->rootNodeOffsetOrPointer,worldRuntime);
      modelNodeRuntime = buildResult.modelNode;
      if (buildResult.failed) {
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
   Once per frontend frame: while a menu-room camera flight is pending, moves the camera along the flight
   spline for the elapsed ticks; when the spline has ended, clears the pending value and, if it is a record id
   (not negative), activates that ROM record. The elapsed ticks are advanced by the frontend timer callback
   FrontendRomTransition_AdvanceElapsedTicks; the body runs under the frontend tick spin lock.
*/
void FrontendRomTransition_ProcessPendingRecord(void)

{
  RomRecordId pendingRecordId;
  WorldRuntimeContext *menuRoomView;
  bool splineStillRunning;
  StatusResult activateResult;

  g_SpinLockAcquire(&g_FrontendStateTickSpinLock);
  /* g_FrontendRomTransitionPendingCount holds the target record id of the running flight (-1 = none to
     activate, 0 = no flight). */
  pendingRecordId = g_FrontendRomTransitionPendingCount;
  menuRoomView = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  if (g_FrontendRomTransitionPendingCount != 0) {
    splineStillRunning = WorldMotionSpline_EvaluateAndApplyAtTime
                      (g_FrontendRomTransitionSplineKeyframeCount,
                       g_FrontendRomTransitionSplineKeyframes,g_FrontendRomTransitionElapsedTicks,
                       menuRoomView);
    if ((!splineStillRunning) && (g_FrontendRomTransitionPendingCount = 0, -1 < (int)pendingRecordId)) {
      activateResult = FrontendRomTransition_ActivateRecordById(pendingRecordId,menuRoomView);
      FatalError_ExitIfFailed(activateResult.valueOrError,activateResult.failed);
    }
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00547400.
   Ownership: assets/rom/runtime.
   Purpose: Walks exactly 256 ROM registry slots, releases nested resources reachable through each record root,
   then clears both dwords of every eight-byte slot. All saved registers and EAX are preserved.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void FrontendRomRegistry_ClearAndReleaseNestedResources(void)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  uint8_t *node;

  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  do {
    if ((slotCursor->record != (RomAssetRecordPrefix *)0x0) &&
       (node = (uint8_t *)slotCursor->record->rootNodeOffsetOrPointer, node != (uint8_t *)0x0)) {
      /* Rewritten from the assembly (0x00547432-0x00547474): depth-first walk over the relocated sprite-node
         tree (RomSerializedNodeHeader34), releasing every node's sprite asset. The original keeps
         {remaining, nextChild, node} frames on the machine stack (EBX = depth).
         The same tree is walked by RomAssetRecord_RegisterAndRelocate. */
      struct { uint8_t *node; uint32_t nextChild; uint32_t remaining; } frames[64];
      int depth = 0;
      for (;;) {
        frames[depth].remaining = ((RomSerializedNodeHeader34 *)node)->childCount;
        frames[depth].nextChild = 0;
        Resource_Release(((RomSerializedNodeHeader34 *)node)->spriteAssetReference.spriteAsset);
        frames[depth].node = node;
        depth++;
        while ((frames[depth - 1].remaining == 0) && (--depth != 0)) {
        }
        if (depth == 0) break;
        node = (uint8_t *)((RomSerializedNodeHeader34 *)frames[depth - 1].node)->
               childReferences[frames[depth - 1].nextChild].node;
        frames[depth - 1].nextChild++;
        frames[depth - 1].remaining--;
      }
    }
    slotCursor->record = (RomAssetRecordPrefix *)0x0;
    slotCursor->runtimeRootNode = (WorldRuntimeNode *)0x0;
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
}


/* Address: 0x00548720.
   Skips a running menu-room camera flight: sets the elapsed ticks far past the last keyframe time, so the next
   FrontendRomTransition_ProcessPendingRecord finds the spline finished and activates the target record.
*/
void FrontendRomTransition_RequestStop(void)

{
  if (g_FrontendRomTransitionPendingCount != 0) {
    g_FrontendRomTransitionElapsedTicks = FRONTEND_ROM_TRANSITION_SKIP_TICKS;
  }
  return;
}


/* Address: 0x005487F0.
   Reverse lookup in the ROM registry: returns the ROM record whose slot holds the given runtime root node, or
   NULL when no slot does.
*/
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  do {
    if ((WorldRuntimeNode *)slotValue == slotCursor->runtimeRootNode) {
      return slotCursor->record;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return NULL;
}


/* Address: 0x00548840.
   Returns the runtime root node registered for a ROM record in the 256-slot ROM registry, or 0 when the record
   is not registered (the reverse of RomRegistry_FindRecordBySlotValue). No caller or table slot referencing
   it was found in src/ or src/generated/image_data.c.
*/
uint32_t RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  do {
    if (record == slotCursor->record) {
      return (uint32_t)slotCursor->runtimeRootNode;
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return 0;
}

/* Address: 0x00548890.
   Returns the entry of a ROM record table (0x200-byte header with the entry count, then 0x200-byte entries)
   whose record id matches, or NULL. Used to find the target record of a frontend camera flight.
*/
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable)

{
  int recordsRemaining;

  recordsRemaining = *(int *)((int)recordTable + FRONTEND_ROM_ACTION_TABLE_COUNT_OFFSET);
  /* the cursor starts at the header, so the entry it tests lies one header size further on */
  while( true ) {
    if (recordsRemaining == 0) {
      return NULL;
    }
    if (recordId == *(RomRecordId *)((int)recordTable + (FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE +
                                                          ROM_RECORD_TABLE_ENTRY_ID_OFFSET))) break;
    recordsRemaining--;
    recordTable = (void *)((int)recordTable + FRONTEND_ROM_ACTION_ENTRY_SIZE);
  }
  return (void *)((int)recordTable + FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE);
}


/* Address: 0x005488D0.
   Same scan as RomRecordTable_FindRecordById, but returns the zero-based entry index, or -1 when no entry of
   the table has the record id.
*/
RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table)

{
  int recordIndex;
  int recordsRemaining;

  recordsRemaining = *(int *)((int)table + FRONTEND_ROM_ACTION_TABLE_COUNT_OFFSET);
  recordIndex = 0;
  while( true ) {
    if (recordsRemaining == 0) {
      return 0xffffffff;
    }
    if (recordId == *(RomRecordId *)((int)table + (FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE +
                                                    ROM_RECORD_TABLE_ENTRY_ID_OFFSET))) break;
    recordIndex++;
    recordsRemaining--;
    table = (void *)((int)table + FRONTEND_ROM_ACTION_ENTRY_SIZE);
  }
  return recordIndex;
}


/* Address: 0x005484D0.
   Ownership: assets/rom/runtime.
   Purpose: Finds a ROM record by ID, updates registry-node visibility and indexed descriptors against the selected
   record masks, applies world motion parameters, and returns status through CF. Typed parameters: p0
   recordId→RomRecordId_V308. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RomRegistry_FindRecordById, RomRegistry_FindSlotValueByRecordId,
   RomRuntime_ApplyIndexedDescriptor.
   Cross-module calls: GraphicsShadingRuntime_ClearRecordTable [graphics/render/shading],
   WorldRuntime_SetPosition60AndDistanceFromPosition80 [world/runtime/core],
   WorldRuntime_SetMotionParameters6CThrough78Clamped [world/runtime/core], UiActionQueue_Enqueue
   [ui/core/runtime].
*/
StatusResult FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime)

{
  uint32_t *slotNodeFlags;
  WorldRuntimeNodeFlags *nodeFlags;
  RomAssetRecordPrefix *recordCursor;
  WorldRuntimeNode *rootNode;
  uint32_t transitionContextValue;
  RomRecordTableIndex entryIndex;
  RomRecordByteSize companionsRemaining;
  RomRecordId descriptorsRemaining;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordResult recordLookup;
  StatusResult statusResult;
  StatusResult activeRecordResult;
  
  GraphicsShadingRuntime_ClearRecordTable();
  recordLookup = RomRegistry_FindRecordById(recordId);
  activeRecordResult.valueOrError = recordLookup.recordOrError;
  if (!recordLookup.notFound) {
    recordCursor = activeRecordResult.valueOrError;
    g_FrontendActiveRomRecordTable = activeRecordResult.valueOrError;
    slotCursor = g_RomRegistrySlots;
    for (companionsRemaining = ((RomAssetRecordPrefix *)(uintptr_t)activeRecordResult.valueOrError)[5].byteSize; g_RomRegistrySlots = slotCursor, companionsRemaining != 0;
        companionsRemaining = companionsRemaining - 1) {
      statusResult = RomRegistry_FindSlotValueByRecordId(recordCursor[0x2d].byteSize);
      if (!statusResult.failed) {
        slotNodeFlags = (uint32_t *)&((WorldRuntimeNode *)statusResult.valueOrError)->runtimeFlags;
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
              1 << ((uint8_t)recordCursor->recordId & 0x1f)) != 0)))) {
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
    statusResult.failed = false;
    statusResult.valueOrError = transitionContextValue;
    return statusResult;
  }
  activeRecordResult.failed = true;
  return activeRecordResult;
}


/* Address: 0x00548600.
   Ownership: assets/rom/runtime.
   Purpose: Clears runtime bit 0x20 across the exact 256-slot ROM registry, resolves the requested record, clears
   and rebuilds bit 0x40 visibility from the selected and companion record masks, and replays every indexed
   descriptor for visible records. CF clear reports success and CF set reports lookup failure. Typed parameters: p1
   recordId→RomRecordId_V308. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RomRegistry_FindRecordById, RomRuntime_ApplyIndexedDescriptor.
   Cross-module calls: GraphicsShadingRuntime_ClearRecordTable [graphics/render/shading].
*/
bool RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId)

{
  WorldRuntimeNodeFlags *nodeFlags;
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *rootNode;
  RomRecordTableIndex entryIndex;
  int slotsRemaining;
  uint32_t maskWordIndex;
  RomRecordId descriptorsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordResult recordLookup;
  
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
  recordLookup = RomRegistry_FindRecordById(recordId);
  if (!recordLookup.notFound) {
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
          (1 << ((uint8_t)record->recordId & 0x1f) &
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
   Registers a ROM record in the first free slot of g_RomRegistrySlots and relocates its serialized node tree:
   child offsets become pointers, and every node's ".spr" sprite is loaded, or an already registered sprite with
   the same registry id is reused. Fails with FATAL_ERROR_ROM_REGISTRY_FULL or the loader's error.
*/
StatusResult RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase)

{
  uint32_t rootNodeOffset;
  RomAssetHeader *asset;
  SpriteAssetHeader *existingSprite;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  uint8_t *rootSerializedNode;
  StatusResult failureResult;
  PackageLoadResult loadResult;
  SpriteRegisterResult registerResult;
  StatusResult successResult;
  
  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  do {
    if (slotCursor->record == NULL) {
      rootNodeOffset = record->rootNodeOffsetOrPointer;
      slotCursor->record = record;
      if (rootNodeOffset == 0) {
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)assetBase;
        return successResult;
      }
      record->rootNodeOffsetOrPointer =
           (uint32_t)((assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28
                  + (record->rootNodeOffsetOrPointer - 0x28));
      rootSerializedNode = (assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               (rootNodeOffset - 0x28);
      /* Rewritten from the assembly (0x005463A1-0x00546436): depth-first walk over the serialized
         sprite-node tree. The original keeps {node, nextChild, remaining} frames on the machine stack
         (EBX = depth), which Ghidra could only show as unaffected ESI/EBP registers. Child offsets are
         relative to assetBase and are relocated in place while walking. */
      {
        struct { uint8_t *node; uint32_t nextChild; uint32_t remaining; } frames[64];
        int depth = 0;
        uint8_t *node = rootSerializedNode;
        for (;;) {
          /* the sprite file name (UTF-16) follows the node header */
          WidePath_SetExtensionCode(0x727073,(uint16_t *)(((RomSerializedNodeHeader34 *)node) + 1)); /* ".spr"; never sets CF */
          loadResult = Package_LoadEntry((uint16_t *)(((RomSerializedNodeHeader34 *)node) + 1));
          if (loadResult.failed) {
            failureResult.failed = true;
            failureResult.valueOrError = (uint32_t)loadResult.bufferOrError;
            return failureResult;
          }
          asset = loadResult.bufferOrError;
          existingSprite = SpriteAssetRegistry_FindById(((SpriteAssetHeader *)asset)->registryHeader.registryId);
          if (existingSprite != NULL) {
            ((RomSerializedNodeHeader34 *)node)->spriteAssetReference.spriteAsset = existingSprite;
            Resource_Release(asset);
          }
          else {
            ((RomSerializedNodeHeader34 *)node)->ownedNestedResourcePresent++; /* set only for sprites this node loaded itself */
            ((RomSerializedNodeHeader34 *)node)->spriteAssetReference.spriteAsset = (SpriteAssetHeader *)asset;
            registerResult = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
            if (registerResult.failed) {
              failureResult.failed = true;
              failureResult.valueOrError = (uint32_t)registerResult.assetOrError;
              return failureResult;
            }
          }
          frames[depth].node = node;
          frames[depth].nextChild = 0;
          frames[depth].remaining = ((RomSerializedNodeHeader34 *)node)->childCount;
          depth++;
          while (frames[depth - 1].remaining == 0) {
            depth--;
            if (depth == 0) {
              successResult.failed = false;
              successResult.valueOrError = (uint32_t)assetBase;
              return successResult;
            }
          }
          {
            uint32_t *child = (uint32_t *)&((RomSerializedNodeHeader34 *)frames[depth - 1].node)->
                              childReferences[frames[depth - 1].nextChild];
            *child = *child + (uint32_t)assetBase;
            frames[depth - 1].nextChild++;
            frames[depth - 1].remaining--;
            node = (uint8_t *)*child;
          }
        }
      }
    }
    slotCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  Package_SetLastErrorPath((uint16_t *)u_engine_zentrale_rom_00545aa4);
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_ROM_REGISTRY_FULL;
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
   Cross-module calls: WorldObjectArray_AllocateFreeRecord [world/runtime/core].
*/
ModelNodeCreateResult RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader34 *romNodeRecord,
          WorldRuntimeContext *worldObjectArray)

{
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  ModelResourceHitTestAndRenderView210 *spriteModelResource;
  Q12 boundingRadius;
  uint32_t translationY;
  uint32_t translationZ;
  GraphicsTextureSet *centralTextureSet;
  ModelRuntimeNode *newNode;
  ModelRuntimeNode *resultOrChildNode;
  ModelPackedLookupTableEntryCount lookupEntriesRemaining;
  uint32_t childSlotsRemaining;
  uint32_t childIndex;
  uint8_t *lookupEntry;
  WorldObjectAllocResult allocResult;
  ModelNodeCreateResult createResult;
  
  allocResult = WorldObjectArray_AllocateFreeRecord(worldObjectArray);
  newNode = (ModelRuntimeNode *)allocResult.recordOrError;
  if (allocResult.failed) {
    createResult.failed = true;
    createResult.modelNode = newNode;
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
  /* four byte stores in this form: indexing the bytes changes the store order in the build */
  *(uint8_t *)&newNode->textureSubresourceBaseIndex = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 1) = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 2) = 0;
  *(uint8_t *)((int)&newNode->textureSubresourceBaseIndex + 3) = 0;
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
  newNode->runtimeStateA0 = (uint32_t)&spriteModelResource->firstMeshGroupRelativeOffset;
  childSlotsRemaining = romNodeRecord->childCount;
  spriteModelResource = (romNodeRecord->spriteAssetReference).modelResource;
  childIndex = 0;
  newNode->childCount = childSlotsRemaining;
  newNode->parentNode = (ModelRuntimeNode *)0x0;
  for (; childSlotsRemaining != 0; childSlotsRemaining = childSlotsRemaining - 1) {
    lookupEntry = spriteModelResource->reserved00_AF + spriteModelResource->packedLookupTableRelativeOffset;
    for (lookupEntriesRemaining = spriteModelResource->packedLookupTableEntryCount; lookupEntriesRemaining != 0; lookupEntriesRemaining = lookupEntriesRemaining - 1) {
      if (((((ModelPackedPointRecord *)lookupEntry)->packedLookupKey & 0xf) == 0) && (childIndex == ((ModelPackedPointRecord *)lookupEntry)->packedLookupKey >> 4)) break;
      lookupEntry = lookupEntry + sizeof(ModelPackedPointRecord);
    }
    if (lookupEntriesRemaining == 0) {
      /* No descriptor for this child: drop it and keep the index for the next child slot. */
      newNode->childCount = newNode->childCount - 1;
      continue;
    }
    createResult = RomRuntime_BuildNodeTreeRecursive
                       (stateTintArgb,romNodeRecord->childReferences[childIndex].node,
                        worldObjectArray);
    if (createResult.failed) {
      return createResult;
    }
    resultOrChildNode = createResult.modelNode;
    newNode->childNodes[childIndex] = resultOrChildNode;
    resultOrChildNode->parentNode = newNode;
    translationY = ((ModelPackedPointRecord *)lookupEntry)->localPosition.y;
    translationZ = ((ModelPackedPointRecord *)lookupEntry)->localPosition.z;
    (resultOrChildNode->modelPayload).localTranslationXQ12 = ((ModelPackedPointRecord *)lookupEntry)->localPosition.x;
    (resultOrChildNode->modelPayload).localTranslationYQ12 = translationY;
    (resultOrChildNode->modelPayload).localTranslationZQ12 = translationZ;
    childIndex = childIndex + 1;
  }
  createResult.failed = false;
  createResult.modelNode = newNode;
  return createResult;
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
void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,RomAssetRecordPrefix *record)

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
   Returns the runtime root node registered for the ROM record with the given id; fails with
   FATAL_ERROR_ROM_RECORD_NOT_REGISTERED when no registry slot holds such a record.
*/
StatusResult RomRegistry_FindSlotValueByRecordId(RomRecordId recordId)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  StatusResult foundResult;
  StatusResult missResult;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  while ((slotCursor->record == NULL ||
         (recordId != slotCursor->record->recordId))) {
    slotCursor++;
    slotsRemaining--;
    if (slotsRemaining == 0) {
      missResult.failed = true;
      missResult.valueOrError = FATAL_ERROR_ROM_RECORD_NOT_REGISTERED;
      return missResult;
    }
  }
  foundResult.failed = false;
  foundResult.valueOrError = (uint32_t)slotCursor->runtimeRootNode;
  return foundResult;
}


/* Address: 0x00548410.
   Creates light entryIndex of a ROM record: finds the point-light descriptor with that index in the sprite of
   the record's root node and allocates a shading light at its world position, using the colour and radius the
   record stores for the entry (0x10-byte entries from +0x50). Indices beyond the record's count (+0x38) are
   ignored.
*/
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record)

{
  uint32_t descriptorsRemaining;
  uint32_t *descriptorCursor;
  PackedRgb24 *entryColorAndRadius;
  int rootSpriteAddress;

  if (entryIndex < record[4].recordId) {
    rootSpriteAddress =
         (int)((RomSerializedNodeHeader34 *)record->rootNodeOffsetOrPointer)->spriteAssetReference.raw;
    entryColorAndRadius = (PackedRgb24 *)(entryIndex * 0x10 + 0x50 + (int)record);
    descriptorCursor =
         (uint32_t *)(rootSpriteAddress + (int)((ModelResourceHitTestAndRenderView210 *)rootSpriteAddress)->packedLookupTableRelativeOffset);
    for (descriptorsRemaining = ((ModelResourceHitTestAndRenderView210 *)rootSpriteAddress)->packedLookupTableEntryCount;
        descriptorsRemaining != 0; descriptorsRemaining--) {
      if (((*descriptorCursor & ROM_NODE_DESCRIPTOR_KIND_MASK) == ROM_NODE_DESCRIPTOR_KIND_LIGHT) &&
          (*descriptorCursor >> 4 == (entryIndex & 0xfffffff)))
      {
        /* descriptor dwords 1..3: world x, y, z (Q12) */
        GraphicsShadingRuntime_AllocateRecordRegs
                  (0,entryColorAndRadius[1],*entryColorAndRadius,descriptorCursor[3],
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
RomRecordResult RomRegistry_FindRecordById(RomRecordId recordId)

{
  RomAssetRecordPrefix *slotRecord;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomRecordResult foundResult;
  RomRecordResult missResult;
  RomAssetRecordPrefix *candidateRecord;
  
  slotsRemaining = 0x100;
  slotCursor = g_RomRegistrySlots;
  while ((slotRecord = slotCursor->record, slotRecord == (RomAssetRecordPrefix *)0x0 ||
         (recordId != slotRecord->recordId))) {
    slotCursor = slotCursor + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      missResult.notFound = true;
      missResult.recordOrError = (RomAssetRecordPrefix *)0x3c;
      return missResult;
    }
  }
  foundResult.notFound = false;
  foundResult.recordOrError = slotRecord;
  return foundResult;
}

