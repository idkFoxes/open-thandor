/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/effects/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/effects/runtime. */

/* Address: 0x0051E120.
   Looks up an effect definition by its id in the 256-slot effect-definition registry (used by the effect
   catalog to reject duplicate ids). Returns the registered definition (never NULL), or NULL on a miss; a miss
   also writes a number into the package error text (the original returned FATAL_ERROR_EFFECT_ID_NOT_FOUND as
   its failure value).
*/
EffectDefinition *EffectRuntime_FindDefinitionById(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *registryDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;

  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  while (registryDefinition = *registryCursor, registryDefinition == NULL || registryDefinition->definitionId != definitionId) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      /* the original formats EAX, i.e. the last slot's pointer, not the requested id */
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registryDefinition,g_PackageLastErrorPath);
      return NULL;
    }
  }
  return registryDefinition;
}


/* Address: 0x0051E190.
   Level start: loads the shared effect texture set and palette ("<mutableBasePath>.gfx/.pal"; the extension is
   changed in place) and allocates and zeroes the 0x40000-byte effect runtime pool. The movie schedule is ticked
   between the steps. Returns true with *outError = 0 on success, or false with the load/allocation error in
   *outError (always written).
*/
bool EffectRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError)

{
  EffectRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  uint32_t loadOrAllocError;
  bool loadOrAllocFailed;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPaletteAsset *loadedPalette;

  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadedTextureSet = g_GraphicsTextureSetLoadPackage(mutableBasePath,&loadOrAllocError);
  loadOrAllocFailed = loadedTextureSet == NULL;
  if (!loadOrAllocFailed) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_EffectTextureSet = loadedTextureSet;
    WidePath_SetExtensionCode(ASSET_MAGIC_PAL,mutableBasePath);
    loadedPalette = g_GraphicsPaletteAssetLoadPackage(mutableBasePath,&loadOrAllocError);
    loadOrAllocFailed = loadedPalette == NULL;
    if (!loadOrAllocFailed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_EffectPalette = loadedPalette;
      loadOrAllocError = g_MemoryApi.alloc(EFFECT_RUNTIME_POOL_BYTES,(void **)&runtimeSlotCursor);
      loadOrAllocFailed = loadOrAllocError != 0;
      if (!loadOrAllocFailed) {
        /* pool base - 1 (the rebase value for saved offsets) */
        g_EffectRuntimeRebaseBaseMinusOne = (uint8_t *)runtimeSlotCursor - 1;
        g_EffectRuntimeSlots = runtimeSlotCursor;
        /* zero the pool dword by dword */
        for (runtimeSlotsRemaining = EFFECT_RUNTIME_POOL_BYTES / 4; runtimeSlotsRemaining != 0;
             runtimeSlotsRemaining--) {
          runtimeSlotCursor->definitionOrSavedId.definition = NULL;
          runtimeSlotCursor = (EffectRuntimeSlot *)((uint32_t *)runtimeSlotCursor + 1);
        }
      }
    }
  }
  *outError = loadOrAllocError;
  return !loadOrAllocFailed;
}


/* Address: 0x0051E210.
   Counterpart of EffectRuntime_InitGraphicsResources: frees the effect runtime pool, releases the effect
   texture set and palette, releases the nested resource owned by each registered effect definition and clears
   the definition registry.
*/
void EffectRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinition *currentDefinition;
  
  g_MemoryApi.free(g_EffectRuntimeSlots);
  g_EffectRuntimeSlots = NULL;
  if (g_EffectTextureSet != NULL) {
    g_GraphicsTextureSetReleasePackage(g_EffectTextureSet);
    g_EffectTextureSet = NULL;
  }
  if (g_EffectPalette != NULL) {
    g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_EffectPalette);
    g_EffectPalette = NULL;
  }
  registryCursor = g_EffectDefinitionRegistry;
  for (registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    currentDefinition = *registryCursor;
    if (currentDefinition != NULL && currentDefinition->ownedNestedResourcePresent != 0) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = NULL;
    registryCursor++;
  }
}


/* Address: 0x0051E340.
   After a savegame load: turns the saved offsets of every live effect slot back into pointers (the model node,
   and the owner as a model runtime or an army depending on the completion action) and replaces the saved
   definition id by the registered definition; an effect whose definition is no longer registered is dropped.
*/
void EffectRuntime_RebaseSlotsAfterLoad(void)

{
  EffectRuntimeCompletionAction slotCompletionAction;
  int registrySlotsRemaining;
  int effectSlotsRemaining;
  ModelRuntimeNode *ownerModelNode;
  EffectDefinition **registryCursor;
  EffectRuntimeSlot *effectSlot;
  EffectDefinition *registryDefinition;
  
  effectSlot = g_EffectRuntimeSlots;
  effectSlotsRemaining = EFFECT_RUNTIME_SLOT_COUNT;
  /* NOT [EDI+0x3C] in the original: inverts the age of slot 0 only, on every load */
  g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
  do {
    slotCompletionAction = effectSlot->completionAction;
    ownerModelNode = effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode;
    if (effectSlot->modelNodeOrSavedOffset.modelNode != NULL) {
      /* saved offset + pool base - 1 */
      if (ownerModelNode != NULL) {
        if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
          ownerModelNode = (ModelRuntimeNode *)((uint8_t *)ownerModelNode + g_ModelRuntimeRebaseDelta);
        }
        else if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
          ownerModelNode = (ModelRuntimeNode *)((int)g_ArmyRuntimeRebaseBaseMinusOne + (int)ownerModelNode);
        }
      }
      effectSlot->modelNodeOrSavedOffset.modelNode =
           (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + (int)effectSlot->modelNodeOrSavedOffset.modelNode);
      effectSlot->completionAction = slotCompletionAction;
      effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode = ownerModelNode;
      registryCursor = g_EffectDefinitionRegistry;
      registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
      while (registryDefinition = *registryCursor,
             (registryDefinition == NULL) ||
             (effectSlot->definitionOrSavedId.definition !=
              (EffectDefinition *)registryDefinition->definitionId)) {
        registryCursor++;
        registrySlotsRemaining--;
        if (registrySlotsRemaining == 0) {
          /* saved definition no longer registered: drop the effect (definition = last registry entry) */
          effectSlot->modelNodeOrSavedOffset.modelNode = NULL;
          break;
        }
      }
      effectSlot->definitionOrSavedId.definition = registryDefinition;
    }
    effectSlot++;
    effectSlotsRemaining--;
  } while (effectSlotsRemaining != 0);
}


/* Address: 0x0051E4A0.
   Spawns one effect: takes a free effect slot and a world object record, links the record into the world as an
   effect node showing the definition's sprite model (effect or army graphics per the definition's creation
   flags) at the given position and orientation, sets up animation, scale, the optional light (shading record
   at the model's lookup point (0,4)) and terrain class flags, and plays its positioned sound unless the spot
   is masked. Returns the effect slot, or NULL when there is no pool, no free slot or no free world object
   record (the original returned FATAL_ERROR_GENERAL_FAILURE as its failure value in all three cases). A NULL
   definition returns the pool base (g_EffectRuntimeSlots) and creates nothing.
*/
EffectRuntimeSlot *EffectRuntimePool_CreateInstanceFromDefinition
          (EffectRuntimeCompletionAction completionAction,EffectRuntimeOwnerReference ownerRuntime,
          AngleTurn32 orientationAngle0,AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,
          Q12 worldZQ12,Q12 worldXQ12,Q12 worldYQ12,EffectDefinition *effectDefinition,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelResource *nestedModelResource;
  Q12 resourceRadiusQ12;
  EffectAnimationFrameCount frameCount;
  DefinitionReferencePresentFlag effectLinkPresent;
  DefinitionReferencePresentFlag shotLinkPresent;
  int scaleStartQ12;
  int scaleEndQ12;
  EffectShadingCountdownTicks shadingStartTicks;
  EffectShadingCountdownTicks shadingStopTicks;
  DirectSoundVoiceSet **voiceSetRef;
  EffectModelRuntimeNode *effectModelNode;
  uint32_t randomOrRuntimeValue;
  GraphicsTextureSet *chosenTextureSet;
  int slotsRemaining;
  GraphicsPaletteAsset *chosenPalette;
  EffectRuntimeSlot *effectRuntimeCursor;
  bool projectedCellMasked;
  ModelPackedPointRecord *lightPoint;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasks occupancyMasks;
  char runtimeClassIndex;
  uint32_t soundTableIndex;
  
  effectRuntimeCursor = g_EffectRuntimeSlots;
  if (effectDefinition == NULL) {
    /* the original reports success with the pool base */
    return effectRuntimeCursor;
  }
  if (effectRuntimeCursor == NULL) {
    return NULL;
  }
  slotsRemaining = EFFECT_RUNTIME_SLOT_COUNT;
  do {
    if (effectRuntimeCursor->modelNodeOrSavedOffset.modelNode == NULL) {
      effectModelNode = (EffectModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldRuntime);
      if (effectModelNode != NULL) {
        WorldRuntime_LinkOwnerListNode((WorldOwnerListNode *)effectModelNode);
        effectRuntimeCursor->modelNodeOrSavedOffset.modelNode =
             (ModelRuntimeNode *)effectModelNode;
        effectRuntimeCursor->definitionOrSavedId.definition = effectDefinition;
        effectModelNode->ownerClassId = WORLD_OWNER_RUNTIME_EFFECT;
        effectModelNode->effectRuntime = effectRuntimeCursor;
        effectModelNode->renderDepthBiasOrState = 0;
        /* worldYQ12 goes to translation.x and worldXQ12 to translation.y, as in the original: the parameter
           names are swapped relative to the node fields */
        effectModelNode->worldTransform.translation.x = worldYQ12;
        effectModelNode->worldTransform.translation.y = worldXQ12;
        effectModelNode->worldTransform.translation.z = worldZQ12;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_RANDOMIZE_ORIENTATION) != 0) {
          randomOrRuntimeValue = g_RandomGeneratorState.next();
          orientationAngle0 = randomOrRuntimeValue & FIXED_ANGLE16_MASK;
        }
        effectModelNode->modelPayload.worldRotationAngle0 = orientationAngle2;
        effectModelNode->modelPayload.worldRotationAngle1 = orientationAngle1;
        effectModelNode->modelPayload.worldRotationAngle2 = orientationAngle0;
        chosenTextureSet = g_EffectTextureSet;
        chosenPalette = g_EffectPalette;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET) !=
            0) {
          chosenTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          chosenPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        nestedModelResource = effectDefinition->ownedNestedResource;
        effectModelNode->modelPayload.textureSet = chosenTextureSet;
        resourceRadiusQ12 = nestedModelResource->boundingRadiusQ12;
        effectModelNode->modelPayload.paletteAsset = chosenPalette;
        effectModelNode->subtreeBoundingRadiusQ12 = resourceRadiusQ12;
        effectModelNode->modelPayload.modelResource = nestedModelResource;
        frameCount = effectDefinition->animationFrameCount;
        effectLinkPresent = effectDefinition->linkedEffectPresent;
        shotLinkPresent = effectDefinition->linkedShotPresent;
        effectModelNode->modelPayload.meshGroupMask = UINT32_MAX; /* all mesh groups */
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | (MODEL_RUNTIME_FLAG_APPLY_SCALE | MODEL_NODE_FLAG_TRANSFORM_DIRTY);
        effectModelNode->textureSubresourceBaseIndex = 0;
        effectModelNode->modelRuntimeLinkOrSavedOffset = NULL;
        effectModelNode->parentNode = NULL;
        effectModelNode->childCount = 0;
        effectRuntimeCursor->animationFramesRemaining = frameCount;
        effectRuntimeCursor->linkedEffectPresent = effectLinkPresent;
        effectRuntimeCursor->linkedShotPresent = shotLinkPresent;
        scaleStartQ12 = effectDefinition->modelScaleStartQ12;
        scaleEndQ12 = effectDefinition->modelScaleEndQ12;
        effectModelNode->modelScaleQ12 = scaleStartQ12;
        /* a constant scale of 1.0 needs no per-vertex scaling */
        if ((scaleStartQ12 == Q12_ONE) && (scaleEndQ12 == Q12_ONE)) {
          effectModelNode->runtimeFlags = effectModelNode->runtimeFlags & ~MODEL_RUNTIME_FLAG_APPLY_SCALE;
        }
        shadingStartTicks = effectDefinition->shadingStartCountdownTicks;
        shadingStopTicks = effectDefinition->shadingStopCountdownTicks;
        effectRuntimeCursor->shadingStartCountdownTicksRemaining = shadingStartTicks;
        effectRuntimeCursor->shadingStopCountdownTicksRemaining = shadingStopTicks;
        effectRuntimeCursor->stateTintArgb = 0xffffff; /* white */
        effectRuntimeCursor->effectAgeTicks = 0;
        if (shadingStartTicks == 0) {
          if (ModelLookupTable_FindPackedPoint
                (0,MODEL_POINT_CLASS_LIGHT,effectDefinition->ownedNestedResource,&lightPoint)) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (lightPoint,(ModelRuntimeNode *)effectModelNode);
            /* the alpha byte of the shading colour is the radius in 1/16 world units */
            effectModelNode->shadingRecord = GraphicsShadingRuntime_AllocateRecord
                               (effectDefinition->shadingTransitionDurationTicks,
                                (effectDefinition->shadingColorArgb >> 24) << 8,
                                effectDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
          }
          else {
            effectModelNode->shadingRecord = NULL;
          }
        }
        else {
          effectModelNode->shadingRecord = NULL;
        }
        randomOrRuntimeValue = effectDefinition->completionCountdownTicks;
        soundTableIndex = effectDefinition->soundSlotIndex;
        effectRuntimeCursor->lifecycleOwnerAndDefinition.nextShotPointIndex = 0;
        effectRuntimeCursor->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 = 0;
        effectRuntimeCursor->lifecycleOwnerAndDefinition.ownerAndDefinition.owner = ownerRuntime;
        effectRuntimeCursor->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks =
             randomOrRuntimeValue;
        runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
        effectRuntimeCursor->completionAction = completionAction;
        randomOrRuntimeValue = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                           (Q12_ONE,effectModelNode->worldTransform.translation.y,
                            effectModelNode->worldTransform.translation.x,worldRuntime->fieldGrid)
        ;
        occupancyMasks =
             TerrainOccupancyMask_ResolveRuntimeClassFlags(TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED,0,randomOrRuntimeValue,runtimeClassIndex);
        effectRuntimeCursor->terrainRuntimeClassState = occupancyMasks.primaryOccupancyMask;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | occupancyMasks.runtimeFlags | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
        effectModelNode->tintArgb = 0xffffff;
        if (soundTableIndex != 0 && soundTableIndex < worldRuntime->dwordArrayCount &&
            worldRuntime->dwordArray != NULL &&
            (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundTableIndex], voiceSetRef != NULL)) {
          worldPosition = &effectModelNode->worldTransform.translation;
          projectedCellMasked = TerrainGrid_TestProjectedCellMaskBits01
                             (effectModelNode->worldTransform.translation.y,worldPosition->x,
                              worldRuntime);
          if (!projectedCellMasked) {
            SpatialSound_PlayPositionedOneShot
                      (effectRuntimeCursor->definitionOrSavedId.definition->
                       positionedSoundMaximumDistanceQ12,
                       effectRuntimeCursor->definitionOrSavedId.definition->
                       positionedSoundGainQ15,worldPosition,voiceSetRef);
          }
        }
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)effectModelNode);
        return effectRuntimeCursor;
      }
      break; /* record allocation failed */
    }
    effectRuntimeCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  return NULL;
}

