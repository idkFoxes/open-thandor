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
  FrontendRomActionEntry *entry;
  uint32_t copiedDword;
  RomRecordByteSize copiedByteSize;
  RomRecordId copiedRecordId;
  int frontendRootNode;
  RomRecord *targetRecord;
  int lastKeyframeIndex;
  void *menuRoomView;
  bool visibilityLookupFailed;
  RomRecordId pageActionOrCopiedDword;
  RomRecordId targetRecordId;

  frontendRootNode = g_FrontendRootNode;
  if (recordIndex < ((RomRecord *)g_FrontendActiveRomRecord)->entryCount) {
    entry = (FrontendRomActionEntry *)(recordIndex * FRONTEND_ROM_ACTION_ENTRY_SIZE +
                                       FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE + g_FrontendActiveRomRecord);
    menuRoomView = FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
    targetRecordId = entry->targetRecordId;
    pageActionOrCopiedDword = entry->pageAction;
    /* gameplay settings, quit confirmation, credits and closing are refused in network sessions, network
       setup without a network backend */
    if (((pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE &&
          pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_CREDITS && -1 < (int)pageActionOrCopiedDword) ||
         (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) &&
        (pageActionOrCopiedDword != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE || g_NetworkBackendInstanceCount != 0)) {
      if (entry->activationSoundIndex != 0 && suppressActivationSound == 0 &&
          (DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[entry->activationSoundIndex] != NULL) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)(&g_FrontendMenuSoundVoiceSetTable100)[entry->activationSoundIndex]);
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
      else if (targetRecordId != 0 && 1 < (int)entry->keyframeCount) {
        lastKeyframeIndex = entry->keyframeCount - 1;
        /* Keyframe 0 = snapshot of the menu room camera (WorldRuntimeContext.motion of menuRoomModelView):
           position X/Y/Z, magnitude, heading, pitch; its timeQ12 is 0. */
        pageActionOrCopiedDword =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionYQ12;
        entry->keyframes[0].channel0Q12 =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionXQ12;
        entry->keyframes[0].channel1Q12 = pageActionOrCopiedDword;
        copiedDword =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionMagnitudeQ12;
        entry->keyframes[0].channel2Q12 =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.positionZQ12;
        entry->keyframes[0].channel3Q12 = copiedDword;
        copiedByteSize =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.pitchAngle;
        entry->keyframes[0].channel4Q12 =
             ((WorldRuntimeContext *)FRONTEND_UI(frontendRootNode,menuRoomModelView))->motion.headingAngle;
        entry->keyframes[0].channel5Q12 = copiedByteSize;
        entry->keyframes[0].timeQ12 = 0;
        targetRecord = (RomRecord *)RomRegistry_FindRecordById(targetRecordId);
        if (targetRecord != NULL) {
          pageActionOrCopiedDword = entry->pageAction;
          /* The six channels of the last keyframe become the target record's camera pose; its timeQ12 comes
             from the action entry. */
          copiedByteSize = targetRecord->cameraYQ12;
          entry->keyframes[lastKeyframeIndex].channel0Q12 = targetRecord->cameraXQ12;
          entry->keyframes[lastKeyframeIndex].channel1Q12 = copiedByteSize;
          copiedRecordId = targetRecord->cameraMagnitudeQ12;
          entry->keyframes[lastKeyframeIndex].channel2Q12 = targetRecord->cameraZQ12;
          entry->keyframes[lastKeyframeIndex].channel3Q12 = copiedRecordId;
          copiedDword = targetRecord->cameraPitchAngle;
          entry->keyframes[lastKeyframeIndex].channel4Q12 = targetRecord->cameraHeadingAngle;
          entry->keyframes[lastKeyframeIndex].channel5Q12 = copiedDword;
          /* The first argument is stored as the pending transition value, i.e. the record to activate when
             the camera flight ends (see FrontendRomTransition_ProcessPendingRecord). */
          FrontendRomTransition_InitializeFromRecord(targetRecordId,entry);
          visibilityLookupFailed =
               RomRuntime_UpdateRecordVisibilityAndDescriptors(pageActionOrCopiedDword,targetRecordId);
          if (visibilityLookupFailed) {
            g_FrontendRomTransitionTargetRecordId = 0;
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x00546450.
   Checks that the asset is a 'rom' of converter version 0x10005 and registers each of its variable-size
   records (from +0x200, each advanced by its leading byteSize) with RomAssetRecord_RegisterAndRelocate. An
   invalid header leaves "engine\zentrale.rom" in g_PackageLastErrorPath and fails with
   FATAL_ERROR_ROM_REGISTRY_FULL. Returns 0 on success, otherwise the error code (the original's success EAX was
   never used by its caller).
*/
uint32_t RomAsset_PrepareRecords(RomAssetHeader *asset)

{
  uint32_t registrationError;
  AssetRecordCount recordsRemaining;
  RomAssetRecordPrefix *record;

  if (asset->recordCountHeader.common.magic == ASSET_MAGIC_ROM &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_ROM_00010005) {
    record = (RomAssetRecordPrefix *)(asset + 1);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      registrationError = RomAssetRecord_RegisterAndRelocate(record,asset);
      if (registrationError != 0) {
        return registrationError;
      }
      /* advance by the record's leading byte size */
      record = (RomAssetRecordPrefix *)((uint8_t *)record + record->byteSize);
    }
    return 0;
  }
  Package_SetLastErrorPath((uint16_t *)u_engine_zentrale_rom_00545aa4);
  /* an invalid header also fails with the registry-full code */
  return FATAL_ERROR_ROM_REGISTRY_FULL;
}


/* Address: 0x005466A0.
   Builds the runtime node tree of every registered ROM record (tinted with the record's nodeTintArgb), stores
   its root in the registry slot, links it into its world's owner list and computes its transforms. Returns
   true (CF) when a node allocation fails.
*/
bool RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime)

{
  RomAssetRecordPrefix *slotRecord;
  ModelRuntimeNode *modelNodeRuntime;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    slotRecord = slotCursor->record;
    if (slotRecord != NULL) {
      modelNodeRuntime = RomRuntime_BuildNodeTreeRecursive
                        (((RomRecord *)slotRecord)->nodeTintArgb,
                         (RomSerializedNodeHeader *)slotRecord->rootNodeOffsetOrPointer,worldRuntime);
      if (modelNodeRuntime == NULL) {
        return true;
      }
      slotCursor->runtimeRootNode = (WorldRuntimeNode *)modelNodeRuntime;
      WorldRuntime_LinkOwnerListNode((WorldOwnerListNode *)modelNodeRuntime);
      ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
    }
    slotCursor = slotCursor + 1;
  }
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
  uint32_t activateError;

  g_SpinLockAcquire(&g_FrontendStateTickSpinLock);
  /* g_FrontendRomTransitionTargetRecordId holds the target record id of the running flight (-1 = none to
     activate, 0 = no flight). */
  pendingRecordId = g_FrontendRomTransitionTargetRecordId;
  menuRoomView = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    splineStillRunning = WorldMotionSpline_EvaluateAndApplyAtTime
                      (g_FrontendRomTransitionSplineKeyframeCount,
                       g_FrontendRomTransitionSplineKeyframes,g_FrontendRomTransitionElapsedTicks,
                       menuRoomView);
    if ((!splineStillRunning) && (g_FrontendRomTransitionTargetRecordId = 0, -1 < (int)pendingRecordId)) {
      activateError = FrontendRomTransition_ActivateRecordById(pendingRecordId,menuRoomView);
      FatalError_ExitIfFailed(activateError,activateError != 0);
    }
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00547400.
   Releases the sprite asset of every node of every registered ROM record and empties all 256 registry slots.
   All saved registers and EAX are preserved.
*/
void FrontendRomRegistry_ClearAndReleaseNestedResources(void)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  uint8_t *node;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->record != NULL &&
        (node = (uint8_t *)slotCursor->record->rootNodeOffsetOrPointer, node != NULL)) {
      /* Rewritten from the assembly (0x00547432-0x00547474): depth-first walk over the relocated sprite-node
         tree (RomSerializedNodeHeader), releasing every node's sprite asset. The original keeps
         {remaining, nextChild, node} frames on the machine stack (EBX = depth).
         The same tree is walked by RomAssetRecord_RegisterAndRelocate. */
      struct { uint8_t *node; uint32_t nextChild; uint32_t remaining; } frames[64];
      int depth = 0;
      do {
        frames[depth].remaining = ((RomSerializedNodeHeader *)node)->childCount;
        frames[depth].nextChild = 0;
        Resource_Release(((RomSerializedNodeHeader *)node)->spriteAssetReference.spriteAsset);
        frames[depth].node = node;
        depth++;
        /* pop the finished frames; stop at the first one with children left */
        while (frames[depth - 1].remaining == 0 && --depth != 0) {
        }
        if (depth != 0) {
          node = (uint8_t *)((RomSerializedNodeHeader *)frames[depth - 1].node)->
                 childReferences[frames[depth - 1].nextChild].node;
          frames[depth - 1].nextChild++;
          frames[depth - 1].remaining--;
        }
      } while (depth != 0);
    }
    slotCursor->record = NULL;
    slotCursor->runtimeRootNode = NULL;
    slotCursor = slotCursor + 1;
  }
}


/* Address: 0x00548720.
   Skips a running menu-room camera flight: sets the elapsed ticks far past the last keyframe time, so the next
   FrontendRomTransition_ProcessPendingRecord finds the spline finished and activates the target record.
*/
void FrontendRomTransition_RequestStop(void)

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
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

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if ((WorldRuntimeNode *)slotValue == slotCursor->runtimeRootNode) {
      return slotCursor->record;
    }
    slotCursor++;
  }
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

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (record == slotCursor->record) {
      return (uint32_t)slotCursor->runtimeRootNode;
    }
    slotCursor++;
  }
  return 0;
}

/* Address: 0x00548890.
   Returns the entry of a ROM record table (0x200-byte header with the entry count, then 0x200-byte entries)
   whose record id matches, or NULL. Used to find the target record of a frontend camera flight.
*/
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable)

{
  int recordsRemaining;

  recordsRemaining = ((RomRecord *)recordTable)->entryCount;
  /* the cursor starts at the header, so the entry it tests lies one header size further on */
  while (recordsRemaining != 0) {
    if (recordId == ((FrontendRomActionEntry *)((RomRecord *)recordTable + 1))->linkedRecordId) {
      return (RomRecord *)recordTable + 1;
    }
    recordsRemaining--;
    recordTable = (uint8_t *)recordTable + FRONTEND_ROM_ACTION_ENTRY_SIZE;
  }
  return NULL;
}


/* Address: 0x005488D0.
   Same scan as RomRecordTable_FindRecordById, but returns the zero-based entry index, or -1 when no entry of
   the table has the record id.
*/
RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table)

{
  int recordIndex;
  int recordsRemaining;

  recordsRemaining = ((RomRecord *)table)->entryCount;
  recordIndex = 0;
  while (recordsRemaining != 0) {
    if (recordId == ((FrontendRomActionEntry *)((RomRecord *)table + 1))->linkedRecordId) {
      return recordIndex;
    }
    recordIndex++;
    recordsRemaining--;
    table = (uint8_t *)table + FRONTEND_ROM_ACTION_ENTRY_SIZE;
  }
  return UINT32_MAX;
}


/* Address: 0x005484D0.
   Makes a ROM record the active menu-room location: clears the shading lights, marks the runtime nodes of the
   records linked from its entries (ROM_NODE_FLAG_ACTION_TARGET), hides every record node except the active
   record and those in its visibleRecordMask (whose lights are created), then moves the camera to the record's
   pose. Afterwards the pending transition value (g_FrontendRomTransitionPageAction) is applied: negative
   closes the view, positive becomes the pending page action. Returns 0, or FATAL_ERROR_ROM_RECORD_NOT_REGISTERED
   (after only clearing the shading lights) when the record is not registered.
*/
uint32_t FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime)

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
  RomAssetRecordPrefix *activeRecord;
  WorldRuntimeNode *linkedRootNode;

  GraphicsShadingRuntime_ClearRecordTable();
  activeRecord = RomRegistry_FindRecordById(recordId);
  if (activeRecord != NULL) {
    recordCursor = activeRecord;
    g_FrontendActiveRomRecord = (uint32_t)activeRecord;
    slotCursor = g_RomRegistrySlots;
    /* mark the records linked from the entries (recordCursor walks the entries: header + 0x200 * i) */
    for (companionsRemaining = ((RomRecord *)activeRecord)->entryCount;
         g_RomRegistrySlots = slotCursor, companionsRemaining != 0; companionsRemaining--) {
      if (RomRegistry_FindSlotValueByRecordId
            (((FrontendRomActionEntry *)((RomRecord *)recordCursor + 1))->linkedRecordId,&linkedRootNode)) {
        slotNodeFlags = (uint32_t *)&linkedRootNode->runtimeFlags;
        *slotNodeFlags = *slotNodeFlags | ROM_NODE_FLAG_ACTION_TARGET;
      }
      recordCursor = (RomAssetRecordPrefix *)((uint8_t *)recordCursor + FRONTEND_ROM_ACTION_ENTRY_SIZE);
      slotCursor = g_RomRegistrySlots;
    }
    /* hide every record node, then show the active record and those in its visibleRecordMask */
    slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
    do {
      recordCursor = slotCursor->record;
      rootNode = slotCursor->runtimeRootNode;
      if (recordCursor != NULL &&
          (nodeFlags = &rootNode->runtimeFlags, *nodeFlags = *nodeFlags | ROM_NODE_FLAG_HIDDEN,
           activeRecord == recordCursor ||
           (((RomRecord *)activeRecord)->visibleRecordMask[recordCursor->recordId >> 5] &
            1 << ((uint8_t)recordCursor->recordId & 31)) != 0)) {
        nodeFlags = &rootNode->runtimeFlags;
        *nodeFlags = *nodeFlags & ~ROM_NODE_FLAG_HIDDEN;
        entryIndex = 0;
        for (descriptorsRemaining = ((RomRecord *)recordCursor)->lightCount; descriptorsRemaining != 0;
             descriptorsRemaining--) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,recordCursor);
          entryIndex = entryIndex + 1;
        }
      }
      transitionContextValue = g_FrontendRomTransitionPageAction;
      slotCursor = slotCursor + 1;
      slotsRemaining--;
    } while (slotsRemaining != 0);
    WorldRuntime_SetCameraPositionKeepingTarget
              (((RomRecord *)activeRecord)->cameraZQ12,
               ((RomRecord *)activeRecord)->cameraYQ12,
               ((RomRecord *)activeRecord)->cameraXQ12,worldRuntime);
    WorldRuntime_SetCameraAnglesAndMagnitudeClamped
              (2,((RomRecord *)activeRecord)->cameraPitchAngle,
               ((RomRecord *)activeRecord)->cameraHeadingAngle,
               ((RomRecord *)activeRecord)->cameraMagnitudeQ12,worldRuntime);
    if (transitionContextValue != 0) {
      if ((int)transitionContextValue < 0) {
        UiActionQueue_Enqueue(0,worldRuntime);
      }
      else {
        g_FrontendPendingPageAction = transitionContextValue;
      }
    }
    return 0;
  }
  return FATAL_ERROR_ROM_RECORD_NOT_REGISTERED;
}


/* Address: 0x00548600.
   Clears ROM_NODE_FLAG_ACTION_TARGET on every registry node, stores frontendValue as the pending transition
   value and, when recordId is registered, shows only the target record, the active record and the records in
   either one's visibleRecordMask, creating their lights. Returns true (CF) when recordId is not registered.
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
  RomAssetRecordPrefix *targetRecord;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  g_FrontendRomTransitionPageAction = frontendValue;
  slotCursor = g_RomRegistrySlots;
  do {
    if (slotCursor->runtimeRootNode != NULL) {
      nodeFlags = &slotCursor->runtimeRootNode->runtimeFlags;
      *nodeFlags = *nodeFlags & ~ROM_NODE_FLAG_ACTION_TARGET;
    }
    slotCursor = slotCursor + 1;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  targetRecord = RomRegistry_FindRecordById(recordId);
  if (targetRecord != NULL) {
    GraphicsShadingRuntime_ClearRecordTable();
    /* hide every record node, then show the target and active records and those in either visibleRecordMask */
    slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
    slotCursor = g_RomRegistrySlots;
    do {
      record = slotCursor->record;
      rootNode = slotCursor->runtimeRootNode;
      if (record != NULL &&
          (nodeFlags = &rootNode->runtimeFlags, *nodeFlags = *nodeFlags | ROM_NODE_FLAG_HIDDEN,
           record == targetRecord || record == g_FrontendActiveRomRecord ||
           (maskWordIndex = record->recordId >> 5,
            (1 << ((uint8_t)record->recordId & 31) &
             (((RomRecord *)targetRecord)->visibleRecordMask[maskWordIndex] |
              ((RomRecord *)g_FrontendActiveRomRecord)->visibleRecordMask[maskWordIndex])) != 0))) {
        nodeFlags = &rootNode->runtimeFlags;
        *nodeFlags = *nodeFlags & ~ROM_NODE_FLAG_HIDDEN;
        entryIndex = 0;
        for (descriptorsRemaining = ((RomRecord *)record)->lightCount; descriptorsRemaining != 0;
             descriptorsRemaining--) {
          RomRuntime_ApplyIndexedDescriptor(entryIndex,record);
          entryIndex = entryIndex + 1;
        }
      }
      slotCursor = slotCursor + 1;
      slotsRemaining--;
    } while (slotsRemaining != 0);
    return false;
  }
  return true;
}


/* Address: 0x00546330.
   Registers a ROM record in the first free slot of g_RomRegistrySlots and relocates its serialized node tree:
   child offsets become pointers, and every node's ".spr" sprite is loaded, or an already registered sprite with
   the same registry id is reused. Returns 0 on success, otherwise FATAL_ERROR_ROM_REGISTRY_FULL or the
   loader's error (the original's success EAX, assetBase, was never used by its caller).
*/
uint32_t RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase)

{
  uint32_t rootNodeOffset;
  RomAssetHeader *asset;
  SpriteAssetHeader *existingSprite;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  uint8_t *rootSerializedNode;
  PackageLoadResult loadResult;
  uint32_t spriteRegisterError;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->record == NULL) {
      rootNodeOffset = record->rootNodeOffsetOrPointer;
      slotCursor->record = record;
      if (rootNodeOffset == 0) {
        return 0;
      }
      /* asset start + serialized offset */
      record->rootNodeOffsetOrPointer = (uint32_t)((uint8_t *)assetBase + record->rootNodeOffsetOrPointer);
      rootSerializedNode = (uint8_t *)assetBase + rootNodeOffset;
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
          /* never sets CF */
          WidePath_SetExtensionCode(ASSET_MAGIC_SPR,(uint16_t *)((RomSerializedNodeHeader *)node + 1));
          loadResult = Package_LoadEntry((uint16_t *)((RomSerializedNodeHeader *)node + 1));
          if (loadResult.failed) {
            return (uint32_t)loadResult.bufferOrError;
          }
          asset = loadResult.bufferOrError;
          existingSprite = SpriteAssetRegistry_FindById(((SpriteAssetHeader *)asset)->registryHeader.registryId);
          if (existingSprite != NULL) {
            ((RomSerializedNodeHeader *)node)->spriteAssetReference.spriteAsset = existingSprite;
            Resource_Release(asset);
          }
          else {
            /* set only for sprites this node loaded itself */
            ((RomSerializedNodeHeader *)node)->ownedNestedResourcePresent++;
            ((RomSerializedNodeHeader *)node)->spriteAssetReference.spriteAsset = (SpriteAssetHeader *)asset;
            spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
            if (spriteRegisterError != 0) {
              return spriteRegisterError;
            }
          }
          frames[depth].node = node;
          frames[depth].nextChild = 0;
          frames[depth].remaining = ((RomSerializedNodeHeader *)node)->childCount;
          depth++;
          while (frames[depth - 1].remaining == 0) {
            depth--;
            if (depth == 0) {
              return 0;
            }
          }
          {
            uint32_t *child = (uint32_t *)&((RomSerializedNodeHeader *)frames[depth - 1].node)->
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
  }
  Package_SetLastErrorPath((uint16_t *)u_engine_zentrale_rom_00545aa4);
  return FATAL_ERROR_ROM_REGISTRY_FULL;
}


/* Address: 0x005464C0.
   Allocates the runtime node for one serialized ROM sprite node and its children: the node takes the local
   rotation of the serialized node, the central frontend palette and texture set, the given tint and the
   sprite's model resource; each child is placed at the matching attachment point (packed point key class 0,
   key index = child index) of the sprite, children without one are dropped. Returns the node, or NULL when no
   world object record is free (the only error, FATAL_ERROR_GENERAL_FAILURE).
*/
ModelRuntimeNode * RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader *romNodeRecord,
          WorldRuntimeContext *worldObjectArray)

{
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  ModelResource *spriteModelResource;
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

  allocResult = WorldObjectArray_AllocateFreeRecord(worldObjectArray);
  newNode = (ModelRuntimeNode *)allocResult.recordOrError;
  if (allocResult.failed) {
    return NULL;
  }
  newNode->modelPayload.localTranslationXQ12 = 0;
  newNode->modelPayload.localTranslationYQ12 = 0;
  newNode->modelPayload.localTranslationZQ12 = 0;
  newNode->worldTransform.translation.x = 0;
  newNode->worldTransform.translation.y = 0;
  newNode->worldTransform.translation.z = 0;
  rotationAngle0 = romNodeRecord->localRotationAngle0;
  rotationAngle1 = romNodeRecord->localRotationAngle1;
  rotationAngle2 = romNodeRecord->localRotationAngle2;
  newNode->modelPayload.localRotationAngle0 = rotationAngle0;
  newNode->modelPayload.localRotationAngle1 = rotationAngle1;
  newNode->modelPayload.localRotationAngle2 = rotationAngle2;
  newNode->modelPayload.worldRotationAngle0 = rotationAngle0;
  newNode->modelPayload.worldRotationAngle1 = rotationAngle1;
  newNode->modelPayload.worldRotationAngle2 = rotationAngle2;
  newNode->modelPayload.meshGroupMask = UINT32_MAX;
  newNode->runtimeFlags = newNode->runtimeFlags | 1;
  /* four byte stores in this form: indexing the bytes changes the store order in the build */
  *(uint8_t *)&newNode->textureSubresourceBaseIndex = 0;
  *((uint8_t *)&newNode->textureSubresourceBaseIndex + 1) = 0;
  *((uint8_t *)&newNode->textureSubresourceBaseIndex + 2) = 0;
  *((uint8_t *)&newNode->textureSubresourceBaseIndex + 3) = 0;
  newNode->tintArgb = stateTintArgb;
  centralTextureSet = g_FrontendCentralTextureSet;
  spriteModelResource = romNodeRecord->spriteAssetReference.modelResource;
  newNode->modelPayload.paletteAsset = g_FrontendCentralPaletteAsset;
  boundingRadius = spriteModelResource->boundingRadiusQ12;
  newNode->modelPayload.textureSet = centralTextureSet;
  newNode->subtreeBoundingRadiusQ12 = boundingRadius;
  newNode->modelPayload.modelResource = spriteModelResource;
  newNode->shadingRecord = NULL;
  newNode->modelRuntimeLinkOrSavedOffset = NULL;
  newNode->runtimeStateA0 = (uint32_t)&spriteModelResource->firstMeshGroupRelativeOffset;
  childSlotsRemaining = romNodeRecord->childCount;
  spriteModelResource = romNodeRecord->spriteAssetReference.modelResource;
  childIndex = 0;
  newNode->childCount = childSlotsRemaining;
  newNode->parentNode = NULL;
  for (; childSlotsRemaining != 0; childSlotsRemaining = childSlotsRemaining - 1) {
    /* the child's attachment point: the packed point with key class 0 and key index childIndex */
    lookupEntry = (uint8_t *)spriteModelResource + spriteModelResource->packedLookupTableRelativeOffset;
    for (lookupEntriesRemaining = spriteModelResource->packedLookupTableEntryCount; lookupEntriesRemaining != 0;
         lookupEntriesRemaining--) {
      if ((((ModelPackedPointRecord *)lookupEntry)->packedLookupKey & 0xf) == 0 &&
          childIndex == ((ModelPackedPointRecord *)lookupEntry)->packedLookupKey >> 4) break;
      lookupEntry = lookupEntry + sizeof(ModelPackedPointRecord);
    }
    if (lookupEntriesRemaining == 0) {
      /* No descriptor for this child: drop it and keep the index for the next child slot. */
      newNode->childCount = newNode->childCount - 1;
      continue;
    }
    resultOrChildNode = RomRuntime_BuildNodeTreeRecursive
                       (stateTintArgb,romNodeRecord->childReferences[childIndex].node,
                        worldObjectArray);
    if (resultOrChildNode == NULL) {
      return NULL;
    }
    newNode->childNodes[childIndex] = resultOrChildNode;
    resultOrChildNode->parentNode = newNode;
    translationY = ((ModelPackedPointRecord *)lookupEntry)->localPosition.y;
    translationZ = ((ModelPackedPointRecord *)lookupEntry)->localPosition.z;
    resultOrChildNode->modelPayload.localTranslationXQ12 = ((ModelPackedPointRecord *)lookupEntry)->localPosition.x;
    resultOrChildNode->modelPayload.localTranslationYQ12 = translationY;
    resultOrChildNode->modelPayload.localTranslationZQ12 = translationZ;
    childIndex = childIndex + 1;
  }
  return newNode;
}


/* Address: 0x005483C0.
   Starts a camera flight along the keyframes of a frontend ROM action entry: resets the elapsed ticks, stores
   the keyframe count and keyframes and transitionEnabled (the record to activate at the end, see
   FrontendRomTransition_ProcessPendingRecord) and builds the spline curves. EAX is preserved.
*/
void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,FrontendRomActionEntry *entry)

{
  g_FrontendRomTransitionElapsedTicks = 0;
  g_FrontendRomTransitionSplineKeyframeCount = entry->keyframeCount;
  g_FrontendRomTransitionTargetRecordId = transitionEnabled;
  g_FrontendRomTransitionSplineKeyframes = (uint32_t)entry->keyframes;
  WorldMotionSpline_BuildSixChannelCurves
            (g_FrontendRomTransitionSplineKeyframeCount,
             (WorldMotionSplineKeyframe *)g_FrontendRomTransitionSplineKeyframes);
  return;
}


/* Address: 0x005487A0.
   Looks up the runtime root node registered for the ROM record with the given id: returns true and stores it
   (NULL while the record's tree is not built) in *outRootNode, or returns false when no registry slot holds
   such a record (the original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED was read by no caller).
*/
bool RomRegistry_FindSlotValueByRecordId(RomRecordId recordId,WorldRuntimeNode **outRootNode)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  while (slotCursor->record == NULL || recordId != slotCursor->record->recordId) {
    slotCursor++;
    slotsRemaining--;
    if (slotsRemaining == 0) {
      return false;
    }
  }
  *outRootNode = slotCursor->runtimeRootNode;
  return true;
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
  RomRecordLight *light;
  int rootSpriteAddress;

  if (entryIndex < ((RomRecord *)record)->lightCount) {
    rootSpriteAddress =
         (int)((RomSerializedNodeHeader *)record->rootNodeOffsetOrPointer)->spriteAssetReference.raw;
    light = &((RomRecord *)record)->lights[entryIndex];
    descriptorCursor =
         (uint32_t *)(rootSpriteAddress + (int)((ModelResource *)rootSpriteAddress)->packedLookupTableRelativeOffset);
    for (descriptorsRemaining = ((ModelResource *)rootSpriteAddress)->packedLookupTableEntryCount;
        descriptorsRemaining != 0; descriptorsRemaining--) {
      if (((*descriptorCursor & ROM_NODE_DESCRIPTOR_KIND_MASK) == ROM_NODE_DESCRIPTOR_KIND_LIGHT) &&
          (*descriptorCursor >> 4 == (entryIndex & 0xfffffff)))
      {
        /* descriptor dwords 1..3: world x, y, z (Q12) */
        GraphicsShadingRuntime_AllocateRecordRegs
                  (0,light->radiusQ12,light->packedColorRgb,descriptorCursor[3],
                   descriptorCursor[2],descriptorCursor[1]);
        return;
      }
      descriptorCursor = descriptorCursor + 4;
    }
  }
  return;
}


/* Address: 0x00548740.
   Returns the registered ROM record with this record id, or NULL when no registry slot holds one (the
   original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED; FrontendRomTransition_ActivateRecordById
   reports it).
*/
RomAssetRecordPrefix * RomRegistry_FindRecordById(RomRecordId recordId)

{
  RomAssetRecordPrefix *slotRecord;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  while (slotRecord = slotCursor->record, slotRecord == NULL || recordId != slotRecord->recordId) {
    slotCursor = slotCursor + 1;
    slotsRemaining--;
    if (slotsRemaining == 0) {
      return NULL;
    }
  }
  return slotRecord;
}

