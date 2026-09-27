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
   Ownership: world/effects/runtime.
   Purpose: Scans the fixed 256-pointer effect-definition registry for definitionId. A match returns
   EffectDefinition * with CF clear. Failure writes the requested identifier to the package error buffer and
   returns error 0x48 with CF set. The stock corpus contains 140 unique EffectDefinition ids; serialized ids remain
   distinct from relocated EffectDefinition pointers and consumer-specific union facets.
*/
EffectDefinitionResult __thandor_eax_cf_preserve_ecx_edx
EffectRuntime_FindDefinitionById(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *registryDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionResult failureResult;
  EffectDefinitionResult foundResult;
  EffectDefinition *candidateDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((registryDefinition = *registryCursor, registryDefinition == (EffectDefinition *)0x0 ||
         (registryDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)registryDefinition,g_PackageLastErrorPath);
      failureResult.notFound = true;
      failureResult.definitionOrError = (EffectDefinition *)0x48;
      return failureResult;
    }
  }
  foundResult.notFound = false;
  foundResult.definitionOrError = registryDefinition;
  return foundResult;
}


/* Address: 0x0051E190.
   Level start: loads the shared effect texture set and palette ("<mutableBasePath>.gfx/.pal"; the extension is
   changed in place) and allocates and zeroes the 0x40000-byte effect runtime pool. The movie schedule is ticked
   between the steps. Returns 0, or the load/allocation error with CF set.
*/
StatusResult EffectRuntime_InitGraphicsResources(uint16_t *mutableBasePath)

{
  EffectRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  ArenaAllocResult loadOrAllocResult;
  StatusResult statusResult;
  
  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadOrAllocResult = THANDOR_BITCAST(TextureSetResult, ArenaAllocResult, g_GraphicsTextureSetLoadPackage(mutableBasePath));
  if (!loadOrAllocResult.failed) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_EffectTextureSet = (GraphicsTextureSet *)loadOrAllocResult.payloadOrError;
    WidePath_SetExtensionCode(ASSET_MAGIC_PAL,mutableBasePath);
    loadOrAllocResult = THANDOR_BITCAST(PaletteAssetResult, ArenaAllocResult, g_GraphicsPaletteAssetLoadPackage(mutableBasePath));
    if (!loadOrAllocResult.failed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_EffectPalette = (GraphicsPaletteAsset *)loadOrAllocResult.payloadOrError;
      loadOrAllocResult = g_MemoryApi.alloc(0x40000);
      runtimeSlotCursor = (EffectRuntimeSlot *)loadOrAllocResult.payloadOrError;
      if (!loadOrAllocResult.failed) {
        /* pool base - 1 (the rebase value for saved offsets) */
        g_EffectRuntimeRebaseBaseMinusOne = (uint8_t *)((int)&runtimeSlotCursor[-1].effectAgeTicks + 3);
        g_EffectRuntimeSlots = runtimeSlotCursor;
        /* 0x10000 dwords */
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0; runtimeSlotsRemaining--) {
          (runtimeSlotCursor->definitionOrSavedId).definition = NULL;
          runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
        }
        loadOrAllocResult.payloadOrError = 0;
        loadOrAllocResult.failed = false;
      }
    }
  }
  statusResult.valueOrError = loadOrAllocResult.payloadOrError;
  statusResult.failed = loadOrAllocResult.failed;
  return statusResult;
}


/* Address: 0x0051E210.
   Counterpart of EffectRuntime_InitGraphicsResources: frees the effect runtime pool, releases the effect
   texture set and palette, releases the nested resource owned by each registered effect definition and clears
   the definition registry.
*/
void __thandor_void_preserve_eax_ecx EffectRuntime_ShutdownGraphicsResources(void)

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
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  do {
    currentDefinition = *registryCursor;
    if ((currentDefinition != NULL) &&
       (currentDefinition->ownedNestedResourcePresent != 0)) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = NULL;
    registryCursor++;
    registrySlotsRemaining--;
  } while (registrySlotsRemaining != 0);
}


/* Address: 0x0051E340.
   After a savegame load: turns the saved offsets of every live effect slot back into pointers (the model node,
   and the owner as a model runtime or an army depending on the completion action) and replaces the saved
   definition id by the registered definition; an effect whose definition is no longer registered is dropped.
*/
void __thandor_void_preserve_eax_ecx_edx EffectRuntime_RebaseSlotsAfterLoad(void)

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
    ownerModelNode = (effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
    if ((effectSlot->modelNodeOrSavedOffset).modelNode != NULL) {
      /* Ghidra's field arithmetic below: owner + g_ModelRuntimeRebaseDelta, owner + g_ArmyRuntimeRebaseBaseMinusOne
         and modelNode + g_RuntimeObjectRebaseBaseMinusOne */
      if (ownerModelNode != NULL) {
        if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
          ownerModelNode = (ModelRuntimeNode *)
                       ((ownerModelNode->modelPayload).reserved2C_33 + g_ModelRuntimeRebaseDelta + -0x38
                       );
        }
        else if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
          ownerModelNode = (ModelRuntimeNode *)
                       ((int)g_ArmyRuntimeRebaseBaseMinusOne +
                       (int)(&ownerModelNode->modelPayload + -1) + 0x30);
        }
      }
      (effectSlot->modelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((effectSlot->modelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30);
      effectSlot->completionAction = slotCompletionAction;
      (effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode = ownerModelNode;
      registryCursor = g_EffectDefinitionRegistry;
      registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
      for (;;) {
        registryDefinition = *registryCursor;
        if ((registryDefinition != NULL) &&
           ((effectSlot->definitionOrSavedId).definition ==
            (EffectDefinition *)registryDefinition->definitionId)) break;
        registryCursor++;
        registrySlotsRemaining--;
        if (registrySlotsRemaining == 0) {
          /* saved definition no longer registered: drop the effect (definition = last registry entry) */
          (effectSlot->modelNodeOrSavedOffset).modelNode = NULL;
          break;
        }
      }
      (effectSlot->definitionOrSavedId).definition = registryDefinition;
    }
    effectSlot++;
    effectSlotsRemaining--;
    if (effectSlotsRemaining == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x0051E4A0.
   Spawns one effect: takes a free effect slot and a world object record, links the record into the world as an
   effect node showing the definition's sprite model (effect or army graphics per the definition's creation
   flags) at the given position and orientation, sets up animation, scale, the optional light (shading record
   at the model's lookup point (0,4)) and terrain class flags, and plays its positioned sound unless the spot
   is masked. Returns the effect slot; FATAL_ERROR_GENERAL_FAILURE (no pool or no free slot) or the record
   allocation error with CF set. A NULL definition returns the pool base with CF clear.
*/
EffectCreateResult __thandor_eax_cf_preserve_ecx_edx
EffectRuntimePool_CreateInstanceFromDefinition
          (EffectRuntimeCompletionAction completionAction,EffectRuntimeOwnerReference4 ownerRuntime,
          AngleTurn32 orientationAngle0,AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,
          Q12 worldZQ12,Q12 worldXQ12,Q12 worldYQ12,EffectDefinition *effectDefinition,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *worldPosition;
  ModelResourceHitTestAndRenderView210 *nestedModelResource;
  Q12 resourceRadiusQ12;
  EffectAnimationFrameCount frameCount;
  DefinitionReferencePresentFlag effectLinkPresent;
  DefinitionReferencePresentFlag shotLinkPresent;
  int scaleStartQ12;
  int scaleEndQ12;
  EffectShadingCountdownTicks shadingStartTicks;
  EffectShadingCountdownTicks shadingStopTicks;
  DirectSoundVoiceSet **voiceSetRef;
  EffectModelRuntimeNodeClassView100 *effectModelNode;
  uint32_t randomOrRuntimeValue;
  GraphicsTextureSet *chosenTextureSet;
  int slotsRemaining;
  GraphicsPaletteAsset *chosenPalette;
  EffectRuntimeSlot *effectRuntimeCursor;
  bool projectedCellMasked;
  WorldObjectAllocResult recordAlloc;
  ModelLookupEntryResult lookupEntry;
  ShadingRecordResult shadingAlloc;
  EffectCreateResult successResult;
  EffectCreateResult failureResult;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasksRegs12 occupancyMasks;
  char runtimeClassIndex;
  uint32_t soundTableIndex;
  
  effectModelNode = (EffectModelRuntimeNodeClassView100 *)FATAL_ERROR_GENERAL_FAILURE; /* error code: no free slot */
  effectRuntimeCursor = g_EffectRuntimeSlots;
  if (effectDefinition == NULL) {
    /* the original returns CF clear with EAX = the pool base */
    successResult.failed = false;
    successResult.effectRuntime = effectRuntimeCursor;
    return successResult;
  }
  failureResult.failed = true;
  if (effectRuntimeCursor == NULL) {
    failureResult.effectRuntime = (EffectRuntimeSlot *)effectModelNode;
    return failureResult;
  }
  slotsRemaining = EFFECT_RUNTIME_SLOT_COUNT;
  do {
    if ((effectRuntimeCursor->modelNodeOrSavedOffset).modelNode == NULL) {
      recordAlloc = WorldObjectArray_AllocateFreeRecord(worldRuntime);
      effectModelNode = (EffectModelRuntimeNodeClassView100 *)recordAlloc.recordOrError;
      if (!recordAlloc.failed) {
        WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)effectModelNode);
        (effectRuntimeCursor->modelNodeOrSavedOffset).modelNode =
             (ModelRuntimeNode *)effectModelNode;
        (effectRuntimeCursor->definitionOrSavedId).definition = effectDefinition;
        effectModelNode->ownerClassId = WORLD_OWNER_RUNTIME_EFFECT;
        effectModelNode->effectRuntime = effectRuntimeCursor;
        effectModelNode->renderDepthBiasOrState = 0;
        /* worldYQ12 goes to translation.x and worldXQ12 to translation.y, as in the original: the parameter
           names are swapped relative to the node fields */
        (effectModelNode->worldTransform).translation.x = worldYQ12;
        (effectModelNode->worldTransform).translation.y = worldXQ12;
        (effectModelNode->worldTransform).translation.z = worldZQ12;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_RANDOMIZE_ORIENTATION) != 0) {
          randomOrRuntimeValue = g_RandomGeneratorState.next();
          orientationAngle0 = randomOrRuntimeValue & 0xffff;
        }
        (effectModelNode->modelPayload).worldRotationAngle0 = orientationAngle2;
        (effectModelNode->modelPayload).worldRotationAngle1 = orientationAngle1;
        (effectModelNode->modelPayload).worldRotationAngle2 = orientationAngle0;
        chosenTextureSet = g_EffectTextureSet;
        chosenPalette = g_EffectPalette;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET) !=
            0) {
          chosenTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          chosenPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        nestedModelResource = effectDefinition->ownedNestedResource;
        (effectModelNode->modelPayload).textureSet = chosenTextureSet;
        resourceRadiusQ12 = nestedModelResource->boundingRadiusQ12;
        (effectModelNode->modelPayload).paletteAsset = chosenPalette;
        effectModelNode->subtreeBoundingRadiusQ12 = resourceRadiusQ12;
        (effectModelNode->modelPayload).modelResource = nestedModelResource;
        frameCount = effectDefinition->animationFrameCount;
        effectLinkPresent = effectDefinition->linkedEffectPresent;
        shotLinkPresent = effectDefinition->linkedShotPresent;
        (effectModelNode->modelPayload).meshGroupMask = 0xffffffff; /* all mesh groups */
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | (MODEL_RUNTIME_FLAG_APPLY_SCALE | 0x1);
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
          lookupEntry = ModelLookupTable_ContainsPackedKey(0,4,effectDefinition->ownedNestedResource);
          if (!lookupEntry.notFound) {
            localPoint = ModelNodeRuntime_TransformLocalPointRegs
                               (lookupEntry.entry,(ModelRuntimeNode *)effectModelNode);
            shadingAlloc = GraphicsShadingRuntime_AllocateRecordRegs
                               (effectDefinition->shadingTransitionDurationTicks,
                                (effectDefinition->shadingColorArgb >> 0x18) << 8,
                                effectDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
            effectModelNode->shadingRecord = shadingAlloc.record;
          }
          else {
            effectModelNode->shadingRecord = NULL;
          }
        }
        else {
          effectModelNode->shadingRecord = NULL;
        }
        randomOrRuntimeValue = effectDefinition->runtimeValue24;
        soundTableIndex = effectDefinition->terrainGridMaskIndex;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).runtimeState14 = 0;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 = 0;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner = ownerRuntime;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24 =
             randomOrRuntimeValue;
        runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
        effectRuntimeCursor->completionAction = completionAction;
        randomOrRuntimeValue = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                           (Q12_ONE,(effectModelNode->worldTransform).translation.y,
                            (effectModelNode->worldTransform).translation.x,worldRuntime->fieldGrid)
        ;
        occupancyMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags(0x10,0,randomOrRuntimeValue,runtimeClassIndex);
        effectRuntimeCursor->terrainRuntimeClassState = occupancyMasks.primaryOccupancyMask;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | occupancyMasks.runtimeFlags | 0x10;
        effectModelNode->tintArgb = 0xffffff;
        if ((((soundTableIndex != 0) && (soundTableIndex < worldRuntime->dwordArrayCount)) &&
            (worldRuntime->dwordArray != NULL)) &&
           (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundTableIndex],
           voiceSetRef != NULL)) {
          worldPosition = &(effectModelNode->worldTransform).translation;
          projectedCellMasked = TerrainGrid_TestProjectedCellMaskBits01
                             ((effectModelNode->worldTransform).translation.y,worldPosition->x,
                              worldRuntime);
          if (!projectedCellMasked) {
            SpatialSound_PlayPositionedOneShot
                      (((effectRuntimeCursor->definitionOrSavedId).definition)->
                       positionedSoundMaximumDistanceQ12,
                       ((effectRuntimeCursor->definitionOrSavedId).definition)->
                       positionedSoundGainQ15,worldPosition,voiceSetRef);
          }
        }
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)effectModelNode);
        successResult.failed = false;
        successResult.effectRuntime = effectRuntimeCursor;
        return successResult;
      }
      break; /* record allocation failed: its error code */
    }
    effectRuntimeCursor++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  failureResult.effectRuntime = (EffectRuntimeSlot *)effectModelNode;
  return failureResult;
}

