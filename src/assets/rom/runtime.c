/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/rom/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/rom/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_FrontendRomTransitionPageAction = 0;

uint32_t g_FrontendActiveRomRecord = 0;

uint32_t g_FrontendRomTransitionElapsedTicks = 0;

uint32_t g_FrontendRomTransitionSplineKeyframes = 0;

uint32_t g_FrontendRomTransitionSplineKeyframeCount = 0;

uint32_t g_FrontendRomTransitionTargetRecordId = 0;

RomRegistrySlot *g_RomRegistrySlots = 0;

/* the 100 frontend menu sound slots (slot 0 unused, Frontend_Init
   loads sound\menueNN.sam into slots 1..99; ROM action records select one by activationSoundIndex) */
DirectSoundVoiceSet *g_FrontendMenuSoundVoiceSets[100] = {0};

uint16_t u_engine_zentrale_rom_00545aa4[20] = L"engine\\zentrale.rom";

/* Implementation ownership: assets/rom/runtime. */

/* Executes entry recordIndex of the active frontend ROM action table (a menu-room hotspot or a scripted entry
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
  RomRecord *targetRecord;
  int lastKeyframeIndex;
  void *menuRoomView;
  WorldRuntimeContext *menuRoomCamera;
  Bool8 visibilityLookupFailed;
  RomRecordId pageAction;
  RomRecordId targetRecordId;

  if (recordIndex >= ((RomRecord *)g_FrontendActiveRomRecord)->entryCount) {
    return;
  }
  entry = (FrontendRomActionEntry *)(recordIndex * FRONTEND_ROM_ACTION_ENTRY_SIZE +
                                     FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE + g_FrontendActiveRomRecord);
  menuRoomView = FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  targetRecordId = entry->targetRecordId;
  pageAction = entry->pageAction;
  /* gameplay settings, quit confirmation, credits and closing are refused in network sessions, network
     setup without a network backend */
  if (!(((pageAction != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          pageAction != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE &&
          pageAction != FRONTEND_PAGE_ACTION_CREDITS && -1 < (int)pageAction) ||
         (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) &&
        (pageAction != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE || g_NetworkBackendInstanceCount != 0))) {
    return;
  }
  if (entry->activationSoundIndex != 0 && suppressActivationSound == 0 &&
      g_FrontendMenuSoundVoiceSets[entry->activationSoundIndex] != NULL) {
    g_SoundPlayOneShot
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               g_FrontendMenuSoundVoiceSets[entry->activationSoundIndex],NULL);
  }
  if (targetRecordId == 0) {
    if ((int)pageAction < 0) {
      /* A negative action closes the menu-room view (only outside network sessions). */
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
        UiActionQueue_Enqueue(0,menuRoomView);
      }
    }
    else if (pageAction != FRONTEND_PAGE_ACTION_NONE) {
      g_FrontendPendingPageAction = pageAction;
    }
    return;
  }
  if ((int)entry->keyframeCount <= 1) {
    return;
  }
  lastKeyframeIndex = entry->keyframeCount - 1;
  /* Keyframe 0 = snapshot of the menu room camera (WorldRuntimeContext.motion of menuRoomModelView):
     position X/Y/Z, magnitude, heading, pitch; its timeQ12 is 0. */
  menuRoomCamera = (WorldRuntimeContext *)menuRoomView;
  entry->keyframes[0].channel0Q12 = menuRoomCamera->motion.positionXQ12;
  entry->keyframes[0].channel1Q12 = menuRoomCamera->motion.positionYQ12;
  entry->keyframes[0].channel2Q12 = menuRoomCamera->motion.positionZQ12;
  entry->keyframes[0].channel3Q12 = menuRoomCamera->motion.positionMagnitudeQ12;
  entry->keyframes[0].channel4Q12 = menuRoomCamera->motion.headingAngle;
  entry->keyframes[0].channel5Q12 = menuRoomCamera->motion.pitchAngle;
  entry->keyframes[0].timeQ12 = 0;
  targetRecord = (RomRecord *)RomRegistry_FindRecordById(targetRecordId);
  if (targetRecord == NULL) {
    return;
  }
  pageAction = entry->pageAction;
  /* The six channels of the last keyframe become the target record's camera pose; its timeQ12 comes
     from the action entry. */
  entry->keyframes[lastKeyframeIndex].channel0Q12 = targetRecord->cameraXQ12;
  entry->keyframes[lastKeyframeIndex].channel1Q12 = targetRecord->cameraYQ12;
  entry->keyframes[lastKeyframeIndex].channel2Q12 = targetRecord->cameraZQ12;
  entry->keyframes[lastKeyframeIndex].channel3Q12 = targetRecord->cameraMagnitudeQ12;
  entry->keyframes[lastKeyframeIndex].channel4Q12 = targetRecord->cameraHeadingAngle;
  entry->keyframes[lastKeyframeIndex].channel5Q12 = targetRecord->cameraPitchAngle;
  /* The first argument is stored as the pending transition value, i.e. the record to activate when
     the camera flight ends (see FrontendRomTransition_ProcessPendingRecord). */
  FrontendRomTransition_InitializeFromRecord(targetRecordId,entry);
  visibilityLookupFailed = RomRuntime_UpdateRecordVisibilityAndDescriptors(pageAction,targetRecordId);
  if (visibilityLookupFailed) {
    g_FrontendRomTransitionTargetRecordId = 0;
  }
}


/* Checks that the asset is a 'rom' of converter version 0x10005 and registers each of its variable-size
   records (from +0x200, each advanced by its leading byteSize) with RomAssetRecord_RegisterAndRelocate. An
   invalid header leaves "engine\zentrale.rom" in g_PackageLastErrorPath and fails with
   FATAL_ERROR_ROM_REGISTRY_FULL. Returns 0 on success, otherwise the error code (the original's success return
   value was never used by its caller).
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


/* Builds the runtime node tree of every registered ROM record (tinted with the record's nodeTintArgb), stores
   its root in the registry slot, links it into its world's owner list and computes its transforms. Returns
   true when a node allocation fails.
*/
Bool8 RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime)

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


/* Once per frontend frame: while a menu-room camera flight is pending, moves the camera along the flight
   spline for the elapsed ticks; when the spline has ended, clears the pending value and, if it is a record id
   (not negative), activates that ROM record. The elapsed ticks are advanced by the frontend timer callback
   FrontendRomTransition_AdvanceElapsedTicks; the body runs under the frontend tick spin lock.
*/
void FrontendRomTransition_ProcessPendingRecord(void)

{
  RomRecordId pendingRecordId;
  WorldRuntimeContext *menuRoomView;
  Bool8 splineStillRunning;
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
    if (!splineStillRunning) {
      g_FrontendRomTransitionTargetRecordId = 0;
      if (-1 < (int)pendingRecordId) {
        activateError = FrontendRomTransition_ActivateRecordById(pendingRecordId,menuRoomView);
        FatalError_ExitIfFailed(activateError,activateError != 0);
      }
    }
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  return;
}


/* Depth-first walk over a relocated sprite-node tree (RomSerializedNodeHeader), releasing every node's sprite
   asset, parents before children, with an explicit stack of {remaining, nextChild, node} frames. The same
   tree is walked by
   RomSerializedNodeTree_LoadSpritesAndRelocate. */
static void RomSerializedNodeTree_ReleaseSprites(RomSerializedNodeHeader *node)
{
  struct { RomSerializedNodeHeader *node; uint32_t nextChild; uint32_t remaining; } frames[64];
  int depth = 0;

  do {
    frames[depth].remaining = node->childCount;
    frames[depth].nextChild = 0;
    Resource_Release(node->spriteAssetReference.spriteAsset);
    frames[depth].node = node;
    depth++;
    /* pop the finished frames; stop at the first one with children left */
    while (depth != 0 && frames[depth - 1].remaining == 0) {
      depth--;
    }
    if (depth != 0) {
      node = frames[depth - 1].node->childReferences[frames[depth - 1].nextChild].node;
      frames[depth - 1].nextChild++;
      frames[depth - 1].remaining--;
    }
  } while (depth != 0);
}

/* Releases the sprite asset of every node of every registered ROM record and empties all 256 registry slots.
*/
void FrontendRomRegistry_ClearAndReleaseNestedResources(void)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomSerializedNodeHeader *rootNode;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->record != NULL) {
      rootNode = (RomSerializedNodeHeader *)slotCursor->record->rootNodeOffsetOrPointer;
      if (rootNode != NULL) {
        RomSerializedNodeTree_ReleaseSprites(rootNode);
      }
    }
    slotCursor->record = NULL;
    slotCursor->runtimeRootNode = NULL;
    slotCursor = slotCursor + 1;
  }
}


/* Skips a running menu-room camera flight: sets the elapsed ticks far past the last keyframe time, so the next
   FrontendRomTransition_ProcessPendingRecord finds the spline finished and activates the target record.
*/
void FrontendRomTransition_RequestStop(void)

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks = FRONTEND_ROM_TRANSITION_SKIP_TICKS;
  }
  return;
}


/* Reverse lookup in the ROM registry: returns the ROM record whose slot holds the given runtime root node, or
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


/* Returns the entry of a ROM record table (0x200-byte header with the entry count, then 0x200-byte entries)
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


/* Same scan as RomRecordTable_FindRecordById, but returns the zero-based entry index, or -1 when no entry of
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


/* Shows a registered ROM record's runtime root node again (clears ROM_NODE_FLAG_HIDDEN) and creates all of its
   lights (FrontendRomTransition_ActivateRecordById, RomRuntime_UpdateRecordVisibilityAndDescriptors). */
static void RomRecord_ShowNodeAndCreateLights(RomAssetRecordPrefix *record,WorldRuntimeNode *rootNode)
{
  RomRecordTableIndex lightIndex;
  uint32_t lightCount;

  rootNode->runtimeFlags = rootNode->runtimeFlags & ~ROM_NODE_FLAG_HIDDEN;
  lightCount = ((RomRecord *)record)->lightCount;
  for (lightIndex = 0; lightIndex < lightCount; lightIndex++) {
    RomRuntime_ApplyIndexedDescriptor(lightIndex,record);
  }
}

/* Makes a ROM record the active menu-room location: clears the shading lights, marks the runtime nodes of the
   records linked from its entries (ROM_NODE_FLAG_ACTION_TARGET), hides every record node except the active
   record and those in its visibleRecordMask (whose lights are created), then moves the camera to the record's
   pose. Afterwards the pending transition value (g_FrontendRomTransitionPageAction) is applied: negative
   closes the view, positive becomes the pending page action. Returns 0, or FATAL_ERROR_ROM_RECORD_NOT_REGISTERED
   (after only clearing the shading lights) when the record is not registered.
*/
uint32_t FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime)

{
  uint8_t *entryCursor;
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *rootNode;
  uint32_t transitionContextValue;
  RomRecordTableCount entriesRemaining;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomAssetRecordPrefix *activeRecord;
  WorldRuntimeNode *linkedRootNode;

  GraphicsShadingRuntime_ClearRecordTable();
  activeRecord = RomRegistry_FindRecordById(recordId);
  if (activeRecord == NULL) {
    return FATAL_ERROR_ROM_RECORD_NOT_REGISTERED;
  }
  g_FrontendActiveRomRecord = (uint32_t)activeRecord;
  /* mark the records linked from the entries (header + 0x200 * i) */
  entryCursor = (uint8_t *)activeRecord + FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE;
  for (entriesRemaining = ((RomRecord *)activeRecord)->entryCount; entriesRemaining != 0; entriesRemaining--) {
    if (RomRegistry_FindSlotValueByRecordId(((FrontendRomActionEntry *)entryCursor)->linkedRecordId,
                                            &linkedRootNode)) {
      linkedRootNode->runtimeFlags = linkedRootNode->runtimeFlags | ROM_NODE_FLAG_ACTION_TARGET;
    }
    entryCursor = entryCursor + FRONTEND_ROM_ACTION_ENTRY_SIZE;
  }
  /* hide every record node, then show the active record and those in its visibleRecordMask */
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    record = slotCursor->record;
    rootNode = slotCursor->runtimeRootNode;
    if (record != NULL) {
      rootNode->runtimeFlags = rootNode->runtimeFlags | ROM_NODE_FLAG_HIDDEN;
      if (activeRecord == record ||
          (((RomRecord *)activeRecord)->visibleRecordMask[record->recordId >> 5] &
           1 << ((uint8_t)record->recordId & 31)) != 0) {
        RomRecord_ShowNodeAndCreateLights(record,rootNode);
      }
    }
    slotCursor = slotCursor + 1;
  }
  transitionContextValue = g_FrontendRomTransitionPageAction;
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


/* Clears ROM_NODE_FLAG_ACTION_TARGET on every registry node, stores frontendValue as the pending transition
   value and, when recordId is registered, shows only the target record, the active record and the records in
   either one's visibleRecordMask, creating their lights. Returns true when recordId is not registered.
*/
Bool8 RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId)

{
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *rootNode;
  int slotsRemaining;
  uint32_t maskWordIndex;
  RomRegistrySlot *slotCursor;
  RomAssetRecordPrefix *targetRecord;

  g_FrontendRomTransitionPageAction = frontendValue;
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->runtimeRootNode != NULL) {
      slotCursor->runtimeRootNode->runtimeFlags =
           slotCursor->runtimeRootNode->runtimeFlags & ~ROM_NODE_FLAG_ACTION_TARGET;
    }
    slotCursor = slotCursor + 1;
  }
  targetRecord = RomRegistry_FindRecordById(recordId);
  if (targetRecord == NULL) {
    return true;
  }
  GraphicsShadingRuntime_ClearRecordTable();
  /* hide every record node, then show the target and active records and those in either visibleRecordMask */
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    record = slotCursor->record;
    rootNode = slotCursor->runtimeRootNode;
    if (record != NULL) {
      rootNode->runtimeFlags = rootNode->runtimeFlags | ROM_NODE_FLAG_HIDDEN;
      maskWordIndex = record->recordId >> 5;
      if (record == targetRecord || record == (RomAssetRecordPrefix *)g_FrontendActiveRomRecord ||
          (1 << ((uint8_t)record->recordId & 31) &
           (((RomRecord *)targetRecord)->visibleRecordMask[maskWordIndex] |
            ((RomRecord *)g_FrontendActiveRomRecord)->visibleRecordMask[maskWordIndex])) != 0) {
        RomRecord_ShowNodeAndCreateLights(record,rootNode);
      }
    }
    slotCursor = slotCursor + 1;
  }
  return false;
}


/* Loads the ".spr" sprite named after a serialized node header (UTF-16 file name) into the node, or reuses an
   already registered sprite with the same registry id. Returns true with the error in *outError when loading
   or registering fails. */
static Bool8 RomSerializedNode_LoadSprite(RomSerializedNodeHeader *node,uint32_t *outError)
{
  RomAssetHeader *asset;
  SpriteAssetHeader *existingSprite;
  uint32_t loadErrorCode;
  uint32_t spriteRegisterError;

  /* the sprite file name (UTF-16) follows the node header */
  /* cannot fail */
  WidePath_SetExtensionCode(ASSET_MAGIC_SPR,(uint16_t *)(node + 1));
  asset = Package_LoadEntry((uint16_t *)(node + 1),&loadErrorCode);
  if (asset == NULL) {
    *outError = loadErrorCode;
    return true;
  }
  existingSprite = SpriteAssetRegistry_FindById(((SpriteAssetHeader *)asset)->registryHeader.registryId);
  if (existingSprite != NULL) {
    node->spriteAssetReference.spriteAsset = existingSprite;
    Resource_Release(asset);
    return false;
  }
  /* set only for sprites this node loaded itself */
  node->ownedNestedResourcePresent++;
  node->spriteAssetReference.spriteAsset = (SpriteAssetHeader *)asset;
  spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers((SpriteAssetHeader *)asset);
  if (spriteRegisterError != 0) {
    *outError = spriteRegisterError;
    return true;
  }
  return false;
}

/* Depth-first walk over a serialized sprite-node tree, parents before children: loads every node's sprite
   (RomSerializedNode_LoadSprite) and relocates the child offsets (relative to assetBase) to pointers in place
   while walking, with an explicit stack of {node, nextChild, remaining} frames. Returns 0, or the first loader
   error. The same tree is walked by
   RomSerializedNodeTree_ReleaseSprites. */
static uint32_t RomSerializedNodeTree_LoadSpritesAndRelocate
          (RomSerializedNodeHeader *node,RomAssetHeader *assetBase)
{
  struct { RomSerializedNodeHeader *node; uint32_t nextChild; uint32_t remaining; } frames[64];
  int depth = 0;
  uint32_t loadError;
  uint32_t *child;

  do {
    if (RomSerializedNode_LoadSprite(node,&loadError)) {
      return loadError;
    }
    frames[depth].node = node;
    frames[depth].nextChild = 0;
    frames[depth].remaining = node->childCount;
    depth++;
    /* pop the finished frames; stop at the first one with children left */
    while (depth != 0 && frames[depth - 1].remaining == 0) {
      depth--;
    }
    if (depth != 0) {
      child = (uint32_t *)&frames[depth - 1].node->childReferences[frames[depth - 1].nextChild];
      *child = *child + (uint32_t)assetBase;
      frames[depth - 1].nextChild++;
      frames[depth - 1].remaining--;
      node = (RomSerializedNodeHeader *)*child;
    }
  } while (depth != 0);
  return 0;
}

/* Registers a ROM record in the first free slot of g_RomRegistrySlots and relocates its serialized node tree:
   child offsets become pointers, and every node's ".spr" sprite is loaded, or an already registered sprite with
   the same registry id is reused. Returns 0 on success, otherwise FATAL_ERROR_ROM_REGISTRY_FULL or the
   loader's error (the original's success return value, assetBase, was never used by its caller).
*/
uint32_t RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase)

{
  uint32_t rootNodeOffset;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

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
      return RomSerializedNodeTree_LoadSpritesAndRelocate
                       ((RomSerializedNodeHeader *)((uint8_t *)assetBase + rootNodeOffset),assetBase);
    }
    slotCursor++;
  }
  Package_SetLastErrorPath((uint16_t *)u_engine_zentrale_rom_00545aa4);
  return FATAL_ERROR_ROM_REGISTRY_FULL;
}


/* Returns the attachment point of child childIndex in a sprite's model: the first packed point with key class 0
   and key index childIndex, or NULL when the model has none. */
static ModelPackedPointRecord *RomModel_FindChildAttachmentPoint(ModelResource *model,uint32_t childIndex)
{
  ModelPackedLookupTableEntryCount lookupEntriesRemaining;
  ModelPackedPointRecord *lookupEntry;

  lookupEntry = (ModelPackedPointRecord *)((uint8_t *)model + model->packedLookupTableRelativeOffset);
  for (lookupEntriesRemaining = model->packedLookupTableEntryCount; lookupEntriesRemaining != 0;
       lookupEntriesRemaining--) {
    if ((lookupEntry->packedLookupKey & 0xf) == 0 && childIndex == lookupEntry->packedLookupKey >> 4) {
      return lookupEntry;
    }
    lookupEntry = lookupEntry + 1;
  }
  return NULL;
}

/* Allocates the runtime node for one serialized ROM sprite node and its children: the node takes the local
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
  GraphicsTextureSet *centralTextureSet;
  ModelRuntimeNode *newNode;
  ModelRuntimeNode *childNode;
  uint32_t childSlotsRemaining;
  uint32_t childIndex;
  ModelPackedPointRecord *attachmentPoint;

  newNode = (ModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldObjectArray);
  if (newNode == NULL) {
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
    attachmentPoint = RomModel_FindChildAttachmentPoint(spriteModelResource,childIndex);
    if (attachmentPoint == NULL) {
      /* No descriptor for this child: drop it and keep the index for the next child slot. */
      newNode->childCount = newNode->childCount - 1;
      continue;
    }
    childNode = RomRuntime_BuildNodeTreeRecursive
                       (stateTintArgb,romNodeRecord->childReferences[childIndex].node,
                        worldObjectArray);
    if (childNode == NULL) {
      return NULL;
    }
    newNode->childNodes[childIndex] = childNode;
    childNode->parentNode = newNode;
    childNode->modelPayload.localTranslationXQ12 = attachmentPoint->localPosition.x;
    childNode->modelPayload.localTranslationYQ12 = attachmentPoint->localPosition.y;
    childNode->modelPayload.localTranslationZQ12 = attachmentPoint->localPosition.z;
    childIndex = childIndex + 1;
  }
  return newNode;
}


/* Starts a camera flight along the keyframes of a frontend ROM action entry: resets the elapsed ticks, stores
   the keyframe count and keyframes and transitionEnabled (the record to activate at the end, see
   FrontendRomTransition_ProcessPendingRecord) and builds the spline curves.
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


/* Looks up the runtime root node registered for the ROM record with the given id: returns true and stores it
   (NULL while the record's tree is not built) in *outRootNode, or returns false when no registry slot holds
   such a record (the original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED was read by no caller).
*/
Bool8 RomRegistry_FindSlotValueByRecordId(RomRecordId recordId,WorldRuntimeNode **outRootNode)

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


/* Creates light entryIndex of a ROM record: finds the point-light descriptor with that index in the sprite of
   the record's root node and allocates a shading light at its world position, using the colour and radius the
   record stores for the entry (RomRecord.lights). Indices beyond the record's lightCount are ignored.
*/
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record)

{
  uint32_t descriptorsRemaining;
  uint32_t *descriptorCursor;
  RomRecordLight *light;
  ModelResource *rootSprite;

  if (entryIndex < ((RomRecord *)record)->lightCount) {
    rootSprite = ((RomSerializedNodeHeader *)record->rootNodeOffsetOrPointer)->spriteAssetReference.modelResource;
    light = &((RomRecord *)record)->lights[entryIndex];
    descriptorCursor =
         (uint32_t *)((uint8_t *)rootSprite + (int)rootSprite->packedLookupTableRelativeOffset);
    for (descriptorsRemaining = rootSprite->packedLookupTableEntryCount;
        descriptorsRemaining != 0; descriptorsRemaining--) {
      if (((*descriptorCursor & ROM_NODE_DESCRIPTOR_KIND_MASK) == ROM_NODE_DESCRIPTOR_KIND_LIGHT) &&
          (*descriptorCursor >> 4 == (entryIndex & 0xfffffff)))
      {
        /* descriptor dwords 1..3: world x, y, z (Q12) */
        GraphicsShadingRuntime_AllocateRecord
                  (0,light->radiusQ12,light->packedColorRgb,descriptorCursor[3],
                   descriptorCursor[2],descriptorCursor[1]);
        return;
      }
      descriptorCursor = descriptorCursor + 4;
    }
  }
  return;
}


/* Returns the registered ROM record with this record id, or NULL when no registry slot holds one (the
   original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED; FrontendRomTransition_ActivateRecordById
   reports it).
*/
RomAssetRecordPrefix * RomRegistry_FindRecordById(RomRecordId recordId)

{
  RomAssetRecordPrefix *slotRecord;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    slotRecord = slotCursor->record;
    if (slotRecord != NULL && recordId == slotRecord->recordId) {
      return slotRecord;
    }
    slotCursor = slotCursor + 1;
  }
  return NULL;
}

