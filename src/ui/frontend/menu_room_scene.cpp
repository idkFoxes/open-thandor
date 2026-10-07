/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/menu_room_scene.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/menu_room_scene.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/core/bytes.h>

/* Module data. */

FrontendPageAction g_FrontendRomTransitionPageAction = FRONTEND_PAGE_ACTION_NONE;

RomRecord *g_FrontendActiveRomRecord = nullptr;

std::atomic<uint32_t> g_FrontendRomTransitionElapsedTicks{0};

WorldMotionSplineKeyframe *g_FrontendRomTransitionSplineKeyframes = nullptr;

uint32_t g_FrontendRomTransitionSplineKeyframeCount = 0;

std::atomic<uint32_t> g_FrontendRomTransitionTargetRecordId{0};

/* the 100 frontend menu sound slots (slot 0 unused, Frontend_Init
   loads sound\menueNN.sam into slots 1..99; ROM action records select one by activationSoundIndex) */
SoundVoiceSet *g_FrontendMenuSoundVoiceSets[100] = {};

/* The whole RomRecord behind a registry record (RomAssetRecordPrefix is its first 12 bytes): the record's header
   read with its camera pose, visibleRecordMask, lights and entry count. */
static inline RomRecord *RomRecord_Of(RomAssetRecordPrefix *record)
{
  return reinterpret_cast<RomRecord *>(record);
}

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
  FrontendModelPointerContext *menuRoomView;
  WorldRuntimeContext *menuRoomCamera;
  Bool8 visibilityLookupFailed;
  FrontendPageAction pageAction;
  RomRecordId targetRecordId;

  if (recordIndex >= g_FrontendActiveRomRecord->entryCount) {
    return;
  }
  entry = Thandor_At<FrontendRomActionEntry>
            (g_FrontendActiveRomRecord,recordIndex * FRONTEND_ROM_ACTION_ENTRY_SIZE + FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE);
  menuRoomView = &g_FrontendRootNode->menuRoomModelView;
  targetRecordId = entry->targetRecordId;
  pageAction = static_cast<FrontendPageAction>(entry->pageAction); /* ROM int32_t, negative: close */
  /* gameplay settings, quit confirmation, credits and closing are refused in network sessions, network
     setup without a network backend */
  if (!(((pageAction != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          pageAction != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE &&
          pageAction != FRONTEND_PAGE_ACTION_CREDITS && -1 < (int)pageAction) ||
         (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) &&
        (pageAction != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE || g_NetworkBackendInstanceCount != 0))) {
    return;
  }
  /* The original indexed g_FrontendMenuSoundVoiceSets and keyframes with the entry's values unchecked; bounded
     here because both come from the ROM action table: a sound index beyond the 100 slots plays nothing and a
     flight with more than 14 keyframes is not started (each logged). */
  if (entry->activationSoundIndex >= sizeof(g_FrontendMenuSoundVoiceSets) / sizeof(g_FrontendMenuSoundVoiceSets[0])) {
    Thandor_Log("FrontendRomActionTable_ExecuteRecord: entry %u sound index %u out of range",recordIndex,
                entry->activationSoundIndex);
  }
  else if (entry->activationSoundIndex != 0 && suppressActivationSound == 0 &&
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
  if (entry->keyframeCount > sizeof(entry->keyframes) / sizeof(entry->keyframes[0])) {
    Thandor_Log("FrontendRomActionTable_ExecuteRecord: entry %u keyframe count %u out of range",recordIndex,
                entry->keyframeCount);
    return;
  }
  lastKeyframeIndex = entry->keyframeCount - 1;
  /* Keyframe 0 = snapshot of the menu room camera (WorldRuntimeContext.motion of menuRoomModelView):
     position X/Y/Z, magnitude, heading, pitch; its timeQ12 is 0. */
  menuRoomCamera = FrontendModelPointerContext_AsWorldRuntime(menuRoomView);
  entry->keyframes[0].channel0Q12 = menuRoomCamera->motion.positionXQ12;
  entry->keyframes[0].channel1Q12 = menuRoomCamera->motion.positionYQ12;
  entry->keyframes[0].channel2Q12 = menuRoomCamera->motion.positionZQ12;
  entry->keyframes[0].channel3Q12 = menuRoomCamera->motion.positionMagnitudeQ12;
  entry->keyframes[0].channel4Q12 = menuRoomCamera->motion.headingAngle;
  entry->keyframes[0].channel5Q12 = menuRoomCamera->motion.pitchAngle;
  entry->keyframes[0].timeQ12 = 0;
  targetRecord = RomRecord_Of(RomRegistry_FindRecordById(targetRecordId));
  if (targetRecord == nullptr) {
    return;
  }
  pageAction = static_cast<FrontendPageAction>(entry->pageAction); /* ROM int32_t, negative: close */
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
                        (RomRecord_Of(slotRecord)->nodeTintArgb,
                         Thandor_U32ToPointer<RomSerializedNodeHeader>(slotRecord->rootNodeOffsetOrPointer),worldRuntime); /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
      if (modelNodeRuntime == nullptr) {
        return true;
      }
      slotCursor->runtimeRootNode = WorldNode_View<WorldRuntimeNode>(modelNodeRuntime);
      WorldRuntime_LinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(modelNodeRuntime));
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
void FrontendRomTransition_ProcessPendingRecord()

{
  RomRecordId pendingRecordId;
  WorldRuntimeContext *menuRoomView;
  Bool8 splineStillRunning;
  uint32_t activateError;

  const SpinLockGuard tickLock(&g_FrontendStateTickSpinLock);
  /* g_FrontendRomTransitionTargetRecordId holds the target record id of the running flight (-1 = none to
     activate, 0 = no flight). */
  pendingRecordId = g_FrontendRomTransitionTargetRecordId;
  menuRoomView = FrontendModelPointerContext_AsWorldRuntime(&g_FrontendRootNode->menuRoomModelView);
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
}

/* Skips a running menu-room camera flight: sets the elapsed ticks far past the last keyframe time, so the next
   FrontendRomTransition_ProcessPendingRecord finds the spline finished and activates the target record.
*/
void FrontendRomTransition_RequestStop()

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks = FRONTEND_ROM_TRANSITION_SKIP_TICKS;
  }
}

/* Shows a registered ROM record's runtime root node again (clears ROM_NODE_FLAG_HIDDEN) and creates all of its
   lights (FrontendRomTransition_ActivateRecordById, RomRuntime_UpdateRecordVisibilityAndDescriptors). */
static void RomRecord_ShowNodeAndCreateLights(RomAssetRecordPrefix *record,WorldRuntimeNode *rootNode)
{
  RomRecordTableIndex lightIndex;
  uint32_t lightCount;

  rootNode->runtimeFlags = rootNode->runtimeFlags & ~ROM_NODE_FLAG_HIDDEN;
  lightCount = RomRecord_Of(record)->lightCount;
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
  FrontendRomActionEntry *entryCursor;
  RomAssetRecordPrefix *record;
  WorldRuntimeNode *rootNode;
  FrontendPageAction transitionContextValue;
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
  g_FrontendActiveRomRecord = RomRecord_Of(activeRecord);
  /* mark the records linked from the entries (header + 0x200 * i) */
  entryCursor = Thandor_At<FrontendRomActionEntry>(activeRecord,FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE);
  for (entriesRemaining = RomRecord_Of(activeRecord)->entryCount; entriesRemaining != 0; entriesRemaining--) {
    if (RomRegistry_FindSlotValueByRecordId(entryCursor->linkedRecordId,
                                            &linkedRootNode)) {
      linkedRootNode->runtimeFlags = linkedRootNode->runtimeFlags | ROM_NODE_FLAG_ACTION_TARGET;
    }
    entryCursor = Thandor_At<FrontendRomActionEntry>(entryCursor,FRONTEND_ROM_ACTION_ENTRY_SIZE);
  }
  /* hide every record node, then show the active record and those in its visibleRecordMask */
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    record = slotCursor->record;
    rootNode = slotCursor->runtimeRootNode;
    if (record != nullptr) {
      rootNode->runtimeFlags = rootNode->runtimeFlags | ROM_NODE_FLAG_HIDDEN;
      if (activeRecord == record ||
          (RomRecord_Of(activeRecord)->visibleRecordMask[record->recordId >> 5] &
           1 << ((uint8_t)record->recordId & 31)) != 0) {
        RomRecord_ShowNodeAndCreateLights(record,rootNode);
      }
    }
    slotCursor = slotCursor + 1;
  }
  transitionContextValue = g_FrontendRomTransitionPageAction;
  WorldRuntime_SetCameraPositionKeepingTarget
            (RomRecord_Of(activeRecord)->cameraZQ12,
             RomRecord_Of(activeRecord)->cameraYQ12,
             RomRecord_Of(activeRecord)->cameraXQ12,worldRuntime);
  WorldRuntime_SetCameraAnglesAndMagnitudeClamped
            (2,RomRecord_Of(activeRecord)->cameraPitchAngle,
             RomRecord_Of(activeRecord)->cameraHeadingAngle,
             RomRecord_Of(activeRecord)->cameraMagnitudeQ12,worldRuntime);
  if (transitionContextValue != FRONTEND_PAGE_ACTION_NONE) {
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
      if (record == targetRecord || RomRecord_Of(record) == g_FrontendActiveRomRecord ||
          (1 << ((uint8_t)record->recordId & 31) &
           (RomRecord_Of(targetRecord)->visibleRecordMask[maskWordIndex] |
            g_FrontendActiveRomRecord->visibleRecordMask[maskWordIndex])) != 0) {
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

  lookupEntry = Thandor_At<ModelPackedPointRecord>(model,model->packedLookupTableRelativeOffset);
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

  newNode = WorldNode_View<ModelRuntimeNode>(WorldObjectArray_AllocateFreeRecord(worldObjectArray));
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
  *Thandor_Bytes(&newNode->textureSubresourceBaseIndex) = 0;
  *(Thandor_Bytes(&newNode->textureSubresourceBaseIndex) + 1) = 0;
  *(Thandor_Bytes(&newNode->textureSubresourceBaseIndex) + 2) = 0;
  *(Thandor_Bytes(&newNode->textureSubresourceBaseIndex) + 3) = 0;
  newNode->tintArgb = stateTintArgb;
  centralTextureSet = g_FrontendCentralTextureSet;
  spriteModelResource = romNodeRecord->spriteAssetReference.modelResource;
  newNode->modelPayload.paletteAsset = g_FrontendCentralPaletteAsset;
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
  g_FrontendRomTransitionSplineKeyframes = entry->keyframes;
  WorldMotionSpline_BuildSixChannelCurves
            (g_FrontendRomTransitionSplineKeyframeCount,
             g_FrontendRomTransitionSplineKeyframes);
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

  if (entryIndex < RomRecord_Of(record)->lightCount) {
    rootSprite = Thandor_U32ToPointer<RomSerializedNodeHeader>(record->rootNodeOffsetOrPointer)->spriteAssetReference.modelResource; /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
    light = &RomRecord_Of(record)->lights[entryIndex];
    descriptorCursor =
         Thandor_At<uint32_t>(rootSprite,static_cast<int>(rootSprite->packedLookupTableRelativeOffset));
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
}
