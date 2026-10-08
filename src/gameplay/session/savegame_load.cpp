/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame_load.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame_load.h>
#include <algorithm>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* A runtime pool viewed as the raw bytes its save entry is loaded into. */
template <class T> static inline uint8_t *SavedLevel_PoolBytes(T *pool)
{
  return reinterpret_cast<uint8_t *>(pool);
}

/* Turns the saved form of the resource registration records (widget.hex) back into pointers, after a savegame
   load and after writing a savegame: the 1-based offsets become runtime-object, shading-record, army/shot/effect
   slot pointers, texture set and palette are re-selected per domain, and the sprite id is resolved again. Also
   restores the tail record pointer and the local player's faction assignment.
*/
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage)

{
  ResourceRegistrationRecord *registrationRecord;
  ResourceRegistrationRecord *tailRecord;
  uint32_t remainingRecords;
  uint32_t nestedCount;
  uint32_t nestedIndex;
  uint8_t *primaryPointer;
  uint8_t *secondaryPointer;
  uint8_t *nestedBasePointer;
  uint8_t *auxiliaryPointer;
  void *tailNestedPointer;
  GraphicsTextureSet *selectedTextureSet;
  GraphicsPaletteAsset *selectedPalette;
  SpriteAssetHeader *resolvedSprite;
  ArmyRuntimeSlot *payloadSlot;

  registrationRecord = runtimeImage->records;
  remainingRecords = runtimeImage->recordCount;
  /* Original quirk: the record loop tests its count only after the first record, so an image with recordCount 0
     would walk 2^32 records. */
  /* recordCount is always INGAME_WORLD_OBJECT_RECORD_COUNT (WorldRuntime_AttachObjectArray in startup.cpp), so >= 1 */
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = Thandor_U32ToPointer<uint8_t>((registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      secondaryPointer = static_cast<uint8_t *>((registrationRecord->secondaryPointerOrSavedOffset).runtimePointer);
      nestedBasePointer = static_cast<uint8_t *>((registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer);
      /* 1-based offsets from the runtime-object base; 0 stays NULL */
      if (primaryPointer != nullptr) {
        primaryPointer = primaryPointer + Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      if (secondaryPointer != nullptr) {
        secondaryPointer = secondaryPointer + Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      if (nestedBasePointer != nullptr) {
        nestedBasePointer = nestedBasePointer + Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      (registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset = Thandor_PointerToU32(primaryPointer); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer = secondaryPointer;
      (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer = nestedBasePointer;
      (registrationRecord->ownerRuntimeOrSavedOffset).runtimePointer = runtimeImage;
      auxiliaryPointer = Thandor_U32ToPointer<uint8_t>((registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      nestedCount = registrationRecord->nestedCount;
      if (auxiliaryPointer != nullptr) {
        /* 1-based offset from the shading records; 0 is null */
        auxiliaryPointer = reinterpret_cast<uint8_t *>(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + Thandor_PointerToI32(auxiliaryPointer)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      }
      (registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset = Thandor_PointerToU32(auxiliaryPointer); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      /* the nested pointers are 1-based offsets from the runtime-object base as well */
      for (nestedIndex = 0; nestedIndex < nestedCount; nestedIndex++) {
        if (registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer != nullptr) {
          registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer =
               Thandor_U32ToPointer<uint8_t>((int)registrationRecord->nestedPointersOrSavedOffsets[nestedIndex].runtimePointer +
                       Thandor_PointerToI32(g_RuntimeObjectRebaseBaseMinusOne)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        }
      }
      payloadSlot = (registrationRecord->runtimePayload).armyRuntime;
      switch(registrationRecord->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        /* textureSet holds the army graphics binding index until here */
        payloadSlot = reinterpret_cast<ArmyRuntimeSlot *>
                     (Thandor_PointerToI32(payloadSlot) + g_ModelRuntimeRebaseDelta); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset; /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet; /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        /* the payload is handled through its army view in every domain */
        payloadSlot = reinterpret_cast<ArmyRuntimeSlot *>
                     (g_ShotRuntimeRebaseBaseMinusOne + Thandor_PointerToI32(payloadSlot)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = g_ShotPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = reinterpret_cast<ArmyRuntimeSlot *>
                     (g_EffectRuntimeRebaseBaseMinusOne + Thandor_PointerToI32(payloadSlot)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        selectedTextureSet = g_EffectTextureSet;
        selectedPalette = g_EffectPalette;
        /* effects flagged 2 in their model runtime use the army graphics of binding 0 */
        if ((((payloadSlot->modelRuntimeOrSavedOffset).modelRuntime)->effectModelFlags
            & 2) != 0) {
          selectedTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          selectedPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        registrationRecord->textureSet = selectedTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
      }
      (registrationRecord->runtimePayload).armyRuntime = payloadSlot;
      resolvedSprite = SpriteAssetRegistry_FindById((SpriteAssetId)registrationRecord->spriteAsset); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      registrationRecord->spriteAsset = resolvedSprite;
    }
    registrationRecord++;
    remainingRecords--;
  } while (remainingRecords != 0);
  /* the last nested slot of the last record is the saved tail record */
  tailNestedPointer = runtimeImage->records[runtimeImage->recordCount - 1].nestedPointersOrSavedOffsets
           [12].runtimePointer;
  tailRecord = nullptr;
  if (tailNestedPointer != nullptr) {
    tailRecord = reinterpret_cast<ResourceRegistrationRecord *>(g_RuntimeObjectRebaseBaseMinusOne + Thandor_PointerToI32(tailNestedPointer)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
  }
  runtimeImage->tailRecord = tailRecord;
  (g_FrontendPlayerRuntimeBlocks->factionAssignment).factionAssignmentIndex = runtimeImage->factionAssignmentIndex;
}

/* True when a saved 1-based offset (0 = none) names a whole element of elementSize bytes inside a pool of
   poolBytes bytes. */
static bool SavedOffset_IsElementInPool(uint32_t savedOffset,uint32_t poolBytes,uint32_t elementSize)
{
  return savedOffset != 0 && savedOffset - 1 <= poolBytes - elementSize;
}

/* Not in the original: true when a saved child or parent offset (already known to lie in the world object pool)
   names the start of a record that the walks may enter: an allocated one, or a free one without children. The
   rebase turns only the child offsets of allocated records into pointers, so a free record's childCount and
   child slots stay raw file values. */
static bool ResourceRegistrationRuntime_SavedTargetValid
          (const ResourceRegistrationRuntimeImageSavedView *runtimeImage,uint32_t savedOffset)
{
  const ResourceRegistrationRecordSavedView *target;
  uint32_t recordIndex;

  if ((savedOffset - 1) % sizeof(WorldObjectRecord) != 0) {
    return false;
  }
  recordIndex = (savedOffset - 1) / sizeof(WorldObjectRecord);
  if (recordIndex >= runtimeImage->recordCount) {
    return false;
  }
  target = &runtimeImage->records[recordIndex];
  return (target->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0 || target->nestedCount == 0;
}

/* Not in the original: finds a cycle in the child links (nestedSavedOffsets) of the allocated records, which
   ResourceRegistrationRuntime_ValidateLoadedRecords has already checked record by record. Iterative depth-first
   walk with a state per record (static, so neither the arena nor the heap is touched); a child that is on the
   current path closes a cycle. Returns true and the record whose child closes it in *outRecordIndex when one
   exists. A child reachable twice without a cycle is accepted: the game writes such saves (see
   ResourceRegistrationRuntime_ValidateLoadedRecords). */
static bool ResourceRegistrationRuntime_FindLoadedRecordCycle
          (const ResourceRegistrationRuntimeImageSavedView *runtimeImage,uint32_t *outRecordIndex)
{
  enum class RecordState : uint8_t { Unvisited, OnPath, Done };
  struct PathEntry {
    uint32_t recordIndex;
    uint32_t nextNestedIndex;
  };
  /* every record is on the path at most once, so the path holds at most one entry per record */
  static RecordState s_recordState[INGAME_WORLD_OBJECT_RECORD_COUNT];
  static PathEntry s_path[INGAME_WORLD_OBJECT_RECORD_COUNT];
  const uint32_t recordCount = runtimeImage->recordCount < static_cast<uint32_t>(INGAME_WORLD_OBJECT_RECORD_COUNT)
                               ? runtimeImage->recordCount
                               : static_cast<uint32_t>(INGAME_WORLD_OBJECT_RECORD_COUNT);
  const ResourceRegistrationRecordSavedView *record;
  PathEntry *top;
  uint32_t pathLength;
  uint32_t rootIndex;
  uint32_t childOffset;
  uint32_t childIndex;

  std::fill_n(s_recordState,recordCount,RecordState::Unvisited);
  for (rootIndex = 0; rootIndex < recordCount; rootIndex++) {
    if (s_recordState[rootIndex] != RecordState::Unvisited ||
        (runtimeImage->records[rootIndex].flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
      continue;
    }
    s_recordState[rootIndex] = RecordState::OnPath;
    s_path[0] = PathEntry{rootIndex,0};
    pathLength = 1;
    while (pathLength != 0) {
      top = &s_path[pathLength - 1];
      record = &runtimeImage->records[top->recordIndex];
      if (top->nextNestedIndex >= record->nestedCount) {
        s_recordState[top->recordIndex] = RecordState::Done;
        pathLength--;
        continue;
      }
      childOffset = record->nestedSavedOffsets[top->nextNestedIndex];
      top->nextNestedIndex++;
      if (childOffset == 0) {
        continue;
      }
      /* checked by ResourceRegistrationRuntime_SavedTargetValid: a record start below recordCount */
      childIndex = (childOffset - 1) / sizeof(WorldObjectRecord);
      if (childIndex >= recordCount || s_recordState[childIndex] == RecordState::Done) {
        continue;
      }
      if (s_recordState[childIndex] == RecordState::OnPath) {
        *outRecordIndex = top->recordIndex;
        return true;
      }
      if ((runtimeImage->records[childIndex].flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) == 0) {
        s_recordState[childIndex] = RecordState::Done; /* a free target has no children (nestedCount 0) */
        continue;
      }
      s_recordState[childIndex] = RecordState::OnPath;
      s_path[pathLength] = PathEntry{childIndex,0};
      pathLength++;
    }
  }
  return false;
}

/* Checks the saved form of the resource registration records (widget.hex) before
   ResourceRegistrationRuntime_RebaseLoadedRecords turns it back into pointers. The original trusts the file: a
   nestedCount above the 13 nested slots writes past the record, the army texture-set field indexes the eight
   army graphics bindings unchecked and every saved offset is rebased without a range check; rejected here
   (FATAL_ERROR_LEVEL_ASSET_INVALID) because the save is untrusted input. Every save written by the game passes:
   the writer saves only offsets of live pool elements, at most 13 nested children (the model tree loaders bound
   childCount) and the owner army's faction index.
   The render, release, tint and transform walks follow the child (and parent) links of the records recursively,
   so the links are checked too: a child or parent offset must name a record start, and that record must be
   allocated or have no children (a free record's child data is not rebased); the child links must not form a
   cycle, which would make the walks recurse without end. Game-written saves pass: the pointers are record
   pointers; the writer zeroes every free record, so a free target has nestedCount 0; and the links are acyclic,
   because a node's children are always created after it (ModelNodeRuntime_CreateHierarchyRecursive, the
   attachment repair in ModelRuntimePool_RepairDeferredChild) and a record re-created later only gets links to
   newer records, so every link points from an older to a newer creation. Not rejected: a free target or a
   child reachable twice. Original quirk: a shot node (ShotRuntimePool_CreateProjectileFromDefinition) does not
   reset childCount, childNodes or parentNode of the record it reuses, so a shot keeps the stale child links of
   the record's earlier model node, which may name records that are free or children of other nodes by now; a
   game-written save can therefore hold both. */
static bool ResourceRegistrationRuntime_ValidateLoadedRecords
          (const ResourceRegistrationRuntimeImageSavedView *runtimeImage,uint32_t *outError)

{
  const uint32_t objectPoolBytes = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord);
  const ResourceRegistrationRecordSavedView *record;
  uint32_t recordIndex;
  uint32_t nestedIndex;
  uint32_t payloadPoolBytes;
  uint32_t payloadElementSize;
  const char *problem;

  for (recordIndex = 0; recordIndex < runtimeImage->recordCount; recordIndex++) {
    record = &runtimeImage->records[recordIndex];
    problem = nullptr;
    if ((record->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      if ((record->primarySavedIdOrOffset != 0 &&
           !SavedOffset_IsElementInPool(record->primarySavedIdOrOffset,objectPoolBytes,sizeof(WorldObjectRecord))) ||
          (record->secondarySavedIdOrOffset != 0 &&
           !SavedOffset_IsElementInPool(record->secondarySavedIdOrOffset,objectPoolBytes,
                                        sizeof(WorldObjectRecord))) ||
          (record->nestedBaseSavedOffset != 0 &&
           !SavedOffset_IsElementInPool(record->nestedBaseSavedOffset,objectPoolBytes,sizeof(WorldObjectRecord)))) {
        problem = "object offset";
      }
      else if (record->auxiliarySavedIdOrOffset != 0 &&
               !SavedOffset_IsElementInPool(record->auxiliarySavedIdOrOffset,
                                            sizeof(g_GraphicsShadingRuntimeRecords),
                                            sizeof(g_GraphicsShadingRuntimeRecords[0]))) {
        problem = "shading offset";
      }
      else if (record->nestedCount > sizeof(record->nestedSavedOffsets) / sizeof(record->nestedSavedOffsets[0])) {
        problem = "nested count";
      }
      else {
        for (nestedIndex = 0; nestedIndex < record->nestedCount; nestedIndex++) {
          if (record->nestedSavedOffsets[nestedIndex] != 0 &&
              !SavedOffset_IsElementInPool(record->nestedSavedOffsets[nestedIndex],objectPoolBytes,
                                           sizeof(WorldObjectRecord))) {
            problem = "nested offset";
            break;
          }
          if (record->nestedSavedOffsets[nestedIndex] != 0 &&
              !ResourceRegistrationRuntime_SavedTargetValid(runtimeImage,record->nestedSavedOffsets[nestedIndex])) {
            problem = "nested target";
            break;
          }
        }
        if (problem == nullptr && record->nestedBaseSavedOffset != 0 &&
            !ResourceRegistrationRuntime_SavedTargetValid(runtimeImage,record->nestedBaseSavedOffset)) {
          problem = "parent target";
        }
      }
      if (problem == nullptr) {
        payloadPoolBytes = 0;
        payloadElementSize = 0;
        switch(record->domainIndex) {
        case RESOURCE_DOMAIN_ARMY_RUNTIME:
          if (record->textureSetSavedIdOrOffset >= sizeof(g_ArmyGraphicsBindings) / sizeof(g_ArmyGraphicsBindings[0])) {
            problem = "army graphics binding";
          }
          payloadPoolBytes = MODEL_RUNTIME_POOL_BYTES;
          payloadElementSize = sizeof(ModelRuntimeSlot);
          break;
        case RESOURCE_DOMAIN_SHOT_RUNTIME:
          payloadPoolBytes = SHOT_RUNTIME_POOL_BYTES;
          payloadElementSize = sizeof(ShotRuntimeSlot);
          break;
        case RESOURCE_DOMAIN_EFFECT_RUNTIME:
          payloadPoolBytes = EFFECT_RUNTIME_POOL_BYTES;
          payloadElementSize = sizeof(EffectRuntimeSlot);
          break;
        }
        if (problem == nullptr && payloadElementSize != 0 &&
            !SavedOffset_IsElementInPool(record->runtimePayloadSavedOffset,payloadPoolBytes,payloadElementSize)) {
          problem = "payload offset";
        }
      }
    }
    /* the last nested slot of the last record holds the saved tail record */
    if (problem == nullptr && recordIndex == runtimeImage->recordCount - 1 && record->nestedSavedOffsets[12] != 0 &&
        !SavedOffset_IsElementInPool(record->nestedSavedOffsets[12],objectPoolBytes,sizeof(WorldObjectRecord))) {
      problem = "tail record offset";
    }
    if (problem != nullptr) {
      Thandor_Log("savegame load: widget.hex record %u: invalid %s, rejected",recordIndex,problem);
      return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_ASSET_INVALID);
    }
  }
  if (ResourceRegistrationRuntime_FindLoadedRecordCycle(runtimeImage,&recordIndex)) {
    Thandor_Log("savegame load: widget.hex record %u: invalid child cycle, rejected",recordIndex);
    return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_ASSET_INVALID);
  }
  return true;
}

/* Loads one saved runtime pool (a .hex entry of the save package) into its buffer; false with the load error in
   *outError, which stays unchanged on success. */
static bool SavedLevel_LoadRuntimePool
          (PckLoadCapacityFlags bufferCapacity,uint8_t *destination,uint16_t *path,uint32_t *outError)

{
  uint32_t loadResult; /* Package_LoadEntryIntoBuffer: byte count on success, error code on failure */

  if (!Package_LoadEntryIntoBuffer(bufferCapacity,destination,path,&loadResult)) {
    return NewLevel_Fail(outError,loadResult);
  }
  return true;
}

/* Loads the saved runtime pools (widget.hex, army.hex, modul.hex, effect.hex, shot.hex, light.hex) over the
   freshly initialised ones and rebases their pointers. */
bool SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  if (!SavedLevel_LoadRuntimePool(worldRuntime->objectCount * sizeof(WorldObjectRecord),
                                  static_cast<uint8_t *>(worldRuntime->objectArray),g_WidgetHexPathUtf16,
                                  outError) ||
      !SavedLevel_LoadRuntimePool(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),
                                  SavedLevel_PoolBytes(g_ArmyRuntimeSlots),
                                  g_ArmyHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(MODEL_RUNTIME_POOL_BYTES,SavedLevel_PoolBytes(g_ModelRuntimeSlots),
                                  g_ModulHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(EFFECT_RUNTIME_POOL_BYTES,SavedLevel_PoolBytes(g_EffectRuntimeSlots),
                                  g_EffectHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(SHOT_RUNTIME_POOL_BYTES,SavedLevel_PoolBytes(g_ShotRuntimeSlots),
                                  g_ShotHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(sizeof(g_GraphicsShadingRuntimeRecords),
                                  SavedLevel_PoolBytes(g_GraphicsShadingRuntimeRecords),g_LightHexPathUtf16,
                                  outError) ||
      !ResourceRegistrationRuntime_ValidateLoadedRecords
                 (reinterpret_cast<const ResourceRegistrationRuntimeImageSavedView *>(worldRuntime),outError)) {
    return false;
  }
  ArmyRuntimePool_RebaseAfterLoad();
  ModelRuntimePool_RebaseAfterLoad();
  ShotRuntime_RebaseSlotsAfterLoad();
  EffectRuntime_RebaseSlotsAfterLoad();
  /* the registration image is a view of the world runtime context's first 0xDC bytes */
  ResourceRegistrationRuntime_RebaseLoadedRecords(reinterpret_cast<ResourceRegistrationRuntimeImage *>(worldRuntime));
  RuntimeHexSegment_ToggleLightImageFlag();
  return true;
}

/* After a savegame load: turns the saved offsets in every used army slot (model node != 0) back into
   pointers, the counterpart of ArmyRuntimePool_ConvertPointersToOffsetsForSave. Model runtime
   (modelRuntimeOrSavedOffset) and model node (modelNodeRuntime) are rebased by their pools' deltas; the army
   references (commandTargetArmyRuntime, assignedTargetArmyRuntime) are saved as pointer - (pool base - 1), so 0
   stays NULL.
*/
void ArmyRuntimePool_RebaseAfterLoad()

{
  uint32_t savedAssignedTargetOffset;
  void *rebasedModelRuntime;
  int slotIndex;
  ArmyRuntimeSlot *rebasedCommandTarget;
  ArmyRuntimeSlot *slot;
  ArmyMovementRuntime *movementRuntime;
  int droppedWaypointQueueCount = 0;

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = &g_ArmyRuntimeSlots[slotIndex];
    if (slot->modelNodeRuntime == nullptr) {
      continue;
    }
    /* The original trusts the saved waypoint queue; bounded here because a queued count above
       ARMY_MOVEMENT_WAYPOINT_CAPACITY makes the waypoint markers and the move orders read and write past
       queuedWaypoints, and a count of 0 underflows when the next waypoint is popped. The game keeps the count in
       1..8 while ARMY_MOVEMENT_WAYPOINTS_QUEUED is set (it clears the flag at 0), so every save it writes passes;
       an invalid queue is dropped. */
    movementRuntime = ModelView_Cast<ArmyMovementRuntime>(slot);
    if (Any(movementRuntime->movementStateFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) &&
        movementRuntime->queuedWaypointCount - 1 >= static_cast<ArmyWaypointCount>(ARMY_MOVEMENT_WAYPOINT_CAPACITY)) {
      droppedWaypointQueueCount++;
      movementRuntime->movementStateFlags = movementRuntime->movementStateFlags & ~ARMY_MOVEMENT_WAYPOINTS_QUEUED;
      movementRuntime->queuedWaypointCount = 0;
    }
    /* modelRuntime + g_ModelRuntimeRebaseDelta */
    rebasedModelRuntime =
         static_cast<uint8_t *>((slot->modelRuntimeOrSavedOffset).modelRuntime) + g_ModelRuntimeRebaseDelta;
    rebasedCommandTarget = nullptr;
    if (slot->commandTargetArmyRuntime != nullptr) {
      rebasedCommandTarget = /* 32-bit format field: ArmyRuntimeSlot.commandTargetArmyRuntime (army.hex) */
           Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(slot->commandTargetArmyRuntime) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
    }
    /* modelNodeRuntime + g_RuntimeObjectRebaseBaseMinusOne */
    slot->modelNodeRuntime = /* 32-bit format field: ArmyRuntimeSlot.modelNodeRuntime (army.hex) */
         reinterpret_cast<ModelRuntimeNode *>(g_RuntimeObjectRebaseBaseMinusOne + (int)slot->modelNodeRuntime);
    savedAssignedTargetOffset = slot->assignedTargetArmyRuntime;
    (slot->modelRuntimeOrSavedOffset).modelRuntime = static_cast<ModelRuntimeSlot *>(rebasedModelRuntime);
    if (savedAssignedTargetOffset != 0) {
      savedAssignedTargetOffset = savedAssignedTargetOffset + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne); /* 32-bit format field: ArmyRuntimeSlot.assignedTargetArmyRuntime (army.hex) */
    }
    slot->commandTargetArmyRuntime = rebasedCommandTarget;
    slot->assignedTargetArmyRuntime = savedAssignedTargetOffset;
  }
  if (droppedWaypointQueueCount != 0) {
    Thandor_Log("savegame load: %d army waypoint queues with a count outside 1..%d dropped",
                droppedWaypointQueueCount,ARMY_MOVEMENT_WAYPOINT_CAPACITY);
  }
}

/* Turns the *assetCount saved army-asset ids of one list back into registry pointers, in place. The first
   unknown id empties the list (*assetCount = 0); the entries already converted stay pointers. */
static void GameFactionRuntime_ResolveLoadedArmyAssetIds(uint32_t *assetIds,FactionArmyAssetCount *assetCount)
{
  FactionArmyAssetCount assetsRemaining;
  uint32_t *assetIdCursor;
  ArmyAssetRecordPrefix *resolvedAsset;

  /* The original converts the saved count unchecked; bounded here because a count above the 64-entry list
     reads past it here and lets the queue code write past it later: emptied like a list with an unknown id. The
     game keeps both counts <= FACTION_ARMY_ASSET_LIST_CAPACITY, so every save it writes passes. */
  if (*assetCount > static_cast<FactionArmyAssetCount>(FACTION_ARMY_ASSET_LIST_CAPACITY)) {
    Thandor_Log("savegame load: faction army asset count %u above %d, list emptied",*assetCount,
                FACTION_ARMY_ASSET_LIST_CAPACITY);
    *assetCount = 0;
    return;
  }
  assetIdCursor = assetIds;
  for (assetsRemaining = *assetCount; assetsRemaining != 0; assetsRemaining--) {
    if (ArmyAssetRegistry_FindById(*assetIdCursor,&resolvedAsset) != 0) {
      *assetCount = 0;
      return;
    }
    *assetIdCursor = Thandor_PointerToU32(resolvedAsset); /* 32-bit format field: GameFactionRuntimeRecord.primary/secondaryArmyAssetPointersOrIds (daten.hex) */
    assetIdCursor++;
  }
}

/* After loading a save: turns the army-asset ids of both army-asset lists of all eight faction records back into
   registry pointers (an unknown id empties that list) and converts the 256 saved runtime-group member offsets
   into pointers again (offset + g_ArmyRuntimeRebaseBaseMinusOne; 0 stays NULL).
*/
void GameFactionRuntime_RebaseLoadedArmyReferences()

{
  int factionIndex;
  int groupSlotIndex;
  GameFactionRuntimeRecord *factionRecord;
  ArmyRuntimeSlot *savedSlotOffset;
  ArmyRuntimeSlot *rebasedSlot;

  for (factionIndex = 0; factionIndex < 8; factionIndex++) {
    factionRecord = &g_GameFactionRuntimeImage.records[factionIndex];
    GameFactionRuntime_ResolveLoadedArmyAssetIds(factionRecord->secondaryArmyAssetPointersOrIds,
                                                 &factionRecord->secondaryArmyAssetCount);
    GameFactionRuntime_ResolveLoadedArmyAssetIds(factionRecord->primaryArmyAssetPointersOrIds,
                                                 &factionRecord->primaryArmyAssetCount);
    for (groupSlotIndex = 0; groupSlotIndex < 256; groupSlotIndex++) {
      /* the slot holds the saved offset */
      savedSlotOffset = factionRecord->runtimeGroupMembers8x32[groupSlotIndex];
      rebasedSlot = nullptr;
      if (savedSlotOffset != nullptr) {
        rebasedSlot = Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(savedSlotOffset) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne)); /* 32-bit format field: GameFactionRuntimeRecord.runtimeGroupMembers8x32 (daten.hex) */
      }
      factionRecord->runtimeGroupMembers8x32[groupSlotIndex] = rebasedSlot;
    }
  }
}
