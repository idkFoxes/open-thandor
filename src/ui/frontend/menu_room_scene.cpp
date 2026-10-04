/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/menu_room_scene.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/menu_room_scene.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_FrontendRomTransitionPageAction = 0;

uintptr_t g_FrontendActiveRomRecord = 0;

std::atomic<uint32_t> g_FrontendRomTransitionElapsedTicks{0};

uintptr_t g_FrontendRomTransitionSplineKeyframes = 0;

uint32_t g_FrontendRomTransitionSplineKeyframeCount = 0;

std::atomic<uint32_t> g_FrontendRomTransitionTargetRecordId{0};

/* the 100 frontend menu sound slots (slot 0 unused, Frontend_Init
   loads sound\menueNN.sam into slots 1..99; ROM action records select one by activationSoundIndex) */
DirectSoundVoiceSet *g_FrontendMenuSoundVoiceSets[100] = {};

/* Implementation ownership: ui/frontend/menu_room_scene. */

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
      g_FrontendMenuSoundVoiceSets[entry->activationSoundIndex] != nullptr) {
    g_SoundPlayOneShot
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               g_FrontendMenuSoundVoiceSets[entry->activationSoundIndex],nullptr);
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
  if (targetRecord == nullptr) {
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
    if (slotRecord != nullptr) {
      modelNodeRuntime = RomRuntime_BuildNodeTreeRecursive
                        (((RomRecord *)slotRecord)->nodeTintArgb,
                         Thandor_U32ToPointer<RomSerializedNodeHeader>(slotRecord->rootNodeOffsetOrPointer),worldRuntime); /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
      if (modelNodeRuntime == nullptr) {
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

  const SpinLockGuard tickLock((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
  /* g_FrontendRomTransitionTargetRecordId holds the target record id of the running flight (-1 = none to
     activate, 0 = no flight). */
  pendingRecordId = g_FrontendRomTransitionTargetRecordId;
  menuRoomView = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    splineStillRunning = WorldMotionSpline_EvaluateAndApplyAtTime
                      (g_FrontendRomTransitionSplineKeyframeCount,
                       (WorldMotionSplineKeyframe *)g_FrontendRomTransitionSplineKeyframes,g_FrontendRomTransitionElapsedTicks,
                       menuRoomView);
    if (!splineStillRunning) {
      g_FrontendRomTransitionTargetRecordId = 0;
      if (-1 < (int)pendingRecordId) {
        activateError = FrontendRomTransition_ActivateRecordById(pendingRecordId,menuRoomView);
        FatalError_ExitIfFailed(activateError,activateError != 0);
      }
    }
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
  if (activeRecord == nullptr) {
    return FATAL_ERROR_ROM_RECORD_NOT_REGISTERED;
  }
  g_FrontendActiveRomRecord = (uintptr_t)activeRecord;
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
    if (record != nullptr) {
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
    if (slotCursor->runtimeRootNode != nullptr) {
      slotCursor->runtimeRootNode->runtimeFlags =
           slotCursor->runtimeRootNode->runtimeFlags & ~ROM_NODE_FLAG_ACTION_TARGET;
    }
    slotCursor = slotCursor + 1;
  }
  targetRecord = RomRegistry_FindRecordById(recordId);
  if (targetRecord == nullptr) {
    return true;
  }
  GraphicsShadingRuntime_ClearRecordTable();
  /* hide every record node, then show the target and active records and those in either visibleRecordMask */
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    record = slotCursor->record;
    rootNode = slotCursor->runtimeRootNode;
    if (record != nullptr) {
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
  return nullptr;
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
  if (newNode == nullptr) {
    return nullptr;
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
  centralTextureSet = (GraphicsTextureSet *)(uintptr_t)g_FrontendCentralTextureSet;
  spriteModelResource = romNodeRecord->spriteAssetReference.modelResource;
  newNode->modelPayload.paletteAsset = (GraphicsPaletteAsset *)(uintptr_t)g_FrontendCentralPaletteAsset;
  boundingRadius = spriteModelResource->boundingRadiusQ12;
  newNode->modelPayload.textureSet = centralTextureSet;
  newNode->subtreeBoundingRadiusQ12 = boundingRadius;
  newNode->modelPayload.modelResource = spriteModelResource;
  newNode->shadingRecord = nullptr;
  newNode->modelRuntimeLinkOrSavedOffset = nullptr;
  newNode->runtimeStateA0 = Thandor_PointerToU32(&spriteModelResource->firstMeshGroupRelativeOffset); /* 5f-format: ModelRuntimeNode.runtimeStateA0 (saved model runtime pool) */
  childSlotsRemaining = romNodeRecord->childCount;
  spriteModelResource = romNodeRecord->spriteAssetReference.modelResource;
  childIndex = 0;
  newNode->childCount = childSlotsRemaining;
  newNode->parentNode = nullptr;
  for (; childSlotsRemaining != 0; childSlotsRemaining = childSlotsRemaining - 1) {
    attachmentPoint = RomModel_FindChildAttachmentPoint(spriteModelResource,childIndex);
    if (attachmentPoint == nullptr) {
      /* No descriptor for this child: drop it and keep the index for the next child slot. */
      newNode->childCount = newNode->childCount - 1;
      continue;
    }
    childNode = RomRuntime_BuildNodeTreeRecursive
                       (stateTintArgb,romNodeRecord->childReferences[childIndex].node,
                        worldObjectArray);
    if (childNode == nullptr) {
      return nullptr;
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
  g_FrontendRomTransitionSplineKeyframes = (uintptr_t)entry->keyframes;
  WorldMotionSpline_BuildSixChannelCurves
            (g_FrontendRomTransitionSplineKeyframeCount,
             (WorldMotionSplineKeyframe *)g_FrontendRomTransitionSplineKeyframes);
  return;
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
    rootSprite = Thandor_U32ToPointer<RomSerializedNodeHeader>(record->rootNodeOffsetOrPointer)->spriteAssetReference.modelResource; /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
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
