/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/savegame_load.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/savegame_load.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/session/savegame_load. */

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
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = Thandor_U32ToPointer<uint8_t>((registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
      secondaryPointer = (uint8_t *)(registrationRecord->secondaryPointerOrSavedOffset).runtimePointer;
      nestedBasePointer = (uint8_t *)(registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer;
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
        auxiliaryPointer = (uint8_t *)(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + Thandor_PointerToI32(auxiliaryPointer)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
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
        payloadSlot = (ArmyRuntimeSlot *)
                     (Thandor_PointerToI32(payloadSlot) + g_ModelRuntimeRebaseDelta); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset; /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet; /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + Thandor_PointerToI32(payloadSlot)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = g_ShotPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
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
    tailRecord = (ResourceRegistrationRecord *)(g_RuntimeObjectRebaseBaseMinusOne + Thandor_PointerToI32(tailNestedPointer)); /* 32-bit format field: ResourceRegistrationRecord saved offsets (widget.hex) */
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

/* Checks the saved form of the resource registration records (widget.hex) before
   ResourceRegistrationRuntime_RebaseLoadedRecords turns it back into pointers. The original trusts the file: a
   nestedCount above the 13 nested slots writes past the record, the army texture-set field indexes the eight
   army graphics bindings unchecked and every saved offset is rebased without a range check; rejected here
   (FATAL_ERROR_LEVEL_ASSET_INVALID) because the save is untrusted input. Every save written by the game passes:
   the writer saves only offsets of live pool elements, at most 13 nested children (the model tree loaders bound
   childCount) and the owner army's faction index. */
static Bool8 ResourceRegistrationRuntime_ValidateLoadedRecords
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
  return true;
}

/* Loads one saved runtime pool (a .hex entry of the save package) into its buffer; false with the load error in
   *outError, which stays unchanged on success. */
static Bool8 SavedLevel_LoadRuntimePool
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
Bool8 SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  if (!SavedLevel_LoadRuntimePool(worldRuntime->objectCount * sizeof(WorldObjectRecord),
                                  (uint8_t *)worldRuntime->objectArray,(uint16_t *)g_WidgetHexPathUtf16,
                                  outError) ||
      !SavedLevel_LoadRuntimePool(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),(uint8_t *)g_ArmyRuntimeSlots,
                                  (uint16_t *)g_ArmyHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(MODEL_RUNTIME_POOL_BYTES,(uint8_t *)g_ModelRuntimeSlots,
                                  (uint16_t *)g_ModulHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(EFFECT_RUNTIME_POOL_BYTES,(uint8_t *)g_EffectRuntimeSlots,
                                  (uint16_t *)g_EffectHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(SHOT_RUNTIME_POOL_BYTES,(uint8_t *)g_ShotRuntimeSlots,
                                  (uint16_t *)g_ShotHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(sizeof(g_GraphicsShadingRuntimeRecords),
                                  (uint8_t *)g_GraphicsShadingRuntimeRecords,(uint16_t *)g_LightHexPathUtf16,
                                  outError) ||
      !ResourceRegistrationRuntime_ValidateLoadedRecords
                 ((const ResourceRegistrationRuntimeImageSavedView *)worldRuntime,outError)) {
    return false;
  }
  ArmyRuntimePool_RebaseAfterLoad();
  ModelRuntimePool_RebaseAfterLoad();
  ShotRuntime_RebaseSlotsAfterLoad();
  EffectRuntime_RebaseSlotsAfterLoad();
  ResourceRegistrationRuntime_RebaseLoadedRecords((ResourceRegistrationRuntimeImage *)worldRuntime);
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

  for (slotIndex = 0; slotIndex < ARMY_RUNTIME_SLOT_COUNT; slotIndex++) {
    slot = &g_ArmyRuntimeSlots[slotIndex];
    if (slot->modelNodeRuntime == nullptr) {
      continue;
    }
    /* modelRuntime + g_ModelRuntimeRebaseDelta */
    rebasedModelRuntime = (uint8_t *)(slot->modelRuntimeOrSavedOffset).modelRuntime + g_ModelRuntimeRebaseDelta;
    rebasedCommandTarget = nullptr;
    if (slot->commandTargetArmyRuntime != nullptr) {
      rebasedCommandTarget = /* 32-bit format field: ArmyRuntimeSlot.commandTargetArmyRuntime (army.hex) */
           Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(slot->commandTargetArmyRuntime) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
    }
    /* modelNodeRuntime + g_RuntimeObjectRebaseBaseMinusOne */
    slot->modelNodeRuntime = /* 32-bit format field: ArmyRuntimeSlot.modelNodeRuntime (army.hex) */
         (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + (int)slot->modelNodeRuntime);
    savedAssignedTargetOffset = slot->assignedTargetArmyRuntime;
    (slot->modelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)rebasedModelRuntime;
    if (savedAssignedTargetOffset != 0) {
      savedAssignedTargetOffset = savedAssignedTargetOffset + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne); /* 32-bit format field: ArmyRuntimeSlot.assignedTargetArmyRuntime (army.hex) */
    }
    slot->commandTargetArmyRuntime = rebasedCommandTarget;
    slot->assignedTargetArmyRuntime = savedAssignedTargetOffset;
  }
}

/* Turns the *assetCount saved army-asset ids of one list back into registry pointers, in place. The first
   unknown id empties the list (*assetCount = 0); the entries already converted stay pointers. */
static void GameFactionRuntime_ResolveLoadedArmyAssetIds(uint32_t *assetIds,FactionArmyAssetCount *assetCount)
{
  FactionArmyAssetCount assetsRemaining;
  uint32_t *assetIdCursor;
  ArmyAssetRecordPrefix *resolvedAsset;

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
