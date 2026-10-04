/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/pool.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/effects/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

GraphicsTextureSet *g_EffectTextureSet = 0;

GraphicsPaletteAsset *g_EffectPalette = 0;

EffectRuntimeSlot *g_EffectRuntimeSlots = 0;

/* Implementation ownership: world/effects/pool. */

/* Looks up an effect definition by its id in the 256-slot effect-definition registry (used by the effect
   catalog to reject duplicate ids). Returns the registered definition (never NULL), or NULL on a miss; a miss
   also writes a number into the package error text (the original returned FATAL_ERROR_EFFECT_ID_NOT_FOUND as
   its failure value).
*/
EffectDefinition *EffectRuntime_FindDefinitionById(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *registryDefinition;
  int registryIndex;

  registryDefinition = NULL;
  for (registryIndex = 0; registryIndex < EFFECT_DEFINITION_REGISTRY_SLOT_COUNT; registryIndex++) {
    registryDefinition = g_EffectDefinitionRegistry[registryIndex];
    if (registryDefinition != NULL && registryDefinition->definitionId == definitionId) {
      return registryDefinition;
    }
  }
  /* Original quirk: the error text gets the last registry slot's pointer, not the requested id */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)(intptr_t)registryDefinition,g_PackageLastErrorPath);
  return NULL;
}


/* Level start: loads the shared effect texture set and palette ("<mutableBasePath>.gfx/.pal"; the extension is
   changed in place) and allocates and zeroes the 0x40000-byte effect runtime pool. The movie schedule is ticked
   between the steps. Returns true with *outError = 0 on success, or false with the load/allocation error in
   *outError (always written).
*/
Bool8 EffectRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError)

{
  uint32_t error;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPaletteAsset *loadedPalette;
  void *poolMemory;
  uint32_t *poolDword;
  int poolDwordIndex;

  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadedTextureSet = g_GraphicsTextureSetLoadPackage(mutableBasePath,&error);
  if (loadedTextureSet == NULL) {
    *outError = error;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_EffectTextureSet = loadedTextureSet;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,mutableBasePath);
  loadedPalette = g_GraphicsPaletteAssetLoadPackage(mutableBasePath,&error);
  if (loadedPalette == NULL) {
    *outError = error;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_EffectPalette = loadedPalette;
  error = g_MemoryApi.alloc(EFFECT_RUNTIME_POOL_BYTES,&poolMemory);
  if (error != 0) {
    *outError = error;
    return false;
  }
  /* pool base - 1 (the rebase value for saved offsets) */
  g_EffectRuntimeRebaseBaseMinusOne = (uint8_t *)poolMemory - 1;
  g_EffectRuntimeSlots = (EffectRuntimeSlot *)poolMemory;
  /* zero the pool dword by dword */
  poolDword = (uint32_t *)poolMemory;
  for (poolDwordIndex = 0; poolDwordIndex < EFFECT_RUNTIME_POOL_BYTES / 4; poolDwordIndex++) {
    poolDword[poolDwordIndex] = 0;
  }
  *outError = 0;
  return true;
}


/* Counterpart of EffectRuntime_InitGraphicsResources: frees the effect runtime pool, releases the effect
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


/* After a savegame load: turns the saved offsets of every live effect slot back into pointers (the model node,
   and the owner as a model runtime or an army depending on the completion action) and replaces the saved
   definition id by the registered definition; an effect whose definition is no longer registered is dropped.
*/
void EffectRuntime_RebaseSlotsAfterLoad(void)

{
  EffectRuntimeCompletionAction slotCompletionAction;
  int registryIndex;
  int effectSlotIndex;
  ModelRuntimeNode *ownerModelNode;
  EffectRuntimeSlot *effectSlot;
  EffectDefinition *registryDefinition;

  /* Original quirk: inverts the age of slot 0 only, on every load */
  g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
  for (effectSlotIndex = 0; effectSlotIndex < EFFECT_RUNTIME_SLOT_COUNT; effectSlotIndex++) {
    effectSlot = &g_EffectRuntimeSlots[effectSlotIndex];
    if (effectSlot->modelNodeOrSavedOffset.modelNode == NULL) {
      continue;
    }
    /* saved offset + pool base - 1 */
    slotCompletionAction = effectSlot->completionAction;
    ownerModelNode = effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode;
    if (ownerModelNode != NULL) {
      if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
        ownerModelNode = (ModelRuntimeNode *)((uint8_t *)ownerModelNode + g_ModelRuntimeRebaseDelta);
      }
      else if (slotCompletionAction == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
        /* 5f-format: EffectRuntimeSlot.lifecycleOwnerAndDefinition.ownerAndDefinition.owner (saved offset) */
        ownerModelNode = (ModelRuntimeNode *)((int)g_ArmyRuntimeRebaseBaseMinusOne + (int)ownerModelNode);
      }
    }
    /* 5f-format: EffectRuntimeSlot.modelNodeOrSavedOffset */
    effectSlot->modelNodeOrSavedOffset.modelNode =
         (ModelRuntimeNode *)(g_RuntimeObjectRebaseBaseMinusOne + (int)effectSlot->modelNodeOrSavedOffset.modelNode);
    effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner.modelNode = ownerModelNode;
    /* the slot holds the saved definition id here */
    registryDefinition = NULL;
    for (registryIndex = 0; registryIndex < EFFECT_DEFINITION_REGISTRY_SLOT_COUNT; registryIndex++) {
      registryDefinition = g_EffectDefinitionRegistry[registryIndex];
      /* 5f-format: EffectRuntimeSlot.definitionOrSavedId */
      if (registryDefinition != NULL &&
          effectSlot->definitionOrSavedId.definition == (EffectDefinition *)registryDefinition->definitionId) {
        break;
      }
    }
    if (registryIndex == EFFECT_DEFINITION_REGISTRY_SLOT_COUNT) {
      /* saved definition no longer registered: drop the effect (definition = last registry entry) */
      effectSlot->modelNodeOrSavedOffset.modelNode = NULL;
    }
    effectSlot->definitionOrSavedId.definition = registryDefinition;
  }
}


/* Spawns one effect: takes a free effect slot and a world object record, links the record into the world as an
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
  uint32_t randomValue;
  uint32_t completionCountdownTicks;
  uint32_t neighborhoodClassBits;
  GraphicsTextureSet *chosenTextureSet;
  int slotIndex;
  GraphicsPaletteAsset *chosenPalette;
  EffectRuntimeSlot *effectSlot;
  Bool8 projectedCellMasked;
  ModelPackedPointRecord *lightPoint;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasks occupancyMasks;
  char runtimeClassIndex;
  uint32_t soundTableIndex;

  DebugHook_NoteOutsideStep("effect creation");
  if (effectDefinition == NULL) {
    /* the original reports success with the pool base */
    return g_EffectRuntimeSlots;
  }
  if (g_EffectRuntimeSlots == NULL) {
    return NULL;
  }
  /* first free slot */
  effectSlot = NULL;
  for (slotIndex = 0; slotIndex < EFFECT_RUNTIME_SLOT_COUNT; slotIndex++) {
    if (g_EffectRuntimeSlots[slotIndex].modelNodeOrSavedOffset.modelNode == NULL) {
      effectSlot = &g_EffectRuntimeSlots[slotIndex];
      break;
    }
  }
  if (effectSlot == NULL) {
    return NULL;
  }
  effectModelNode = (EffectModelRuntimeNode *)WorldObjectArray_AllocateFreeRecord(worldRuntime);
  if (effectModelNode == NULL) {
    return NULL;
  }
  WorldRuntime_LinkOwnerListNode((WorldOwnerListNode *)effectModelNode);
  effectSlot->modelNodeOrSavedOffset.modelNode = (ModelRuntimeNode *)effectModelNode;
  effectSlot->definitionOrSavedId.definition = effectDefinition;
  effectModelNode->ownerClassId = WORLD_OWNER_RUNTIME_EFFECT;
  effectModelNode->effectRuntime = effectSlot;
  effectModelNode->renderDepthBiasOrState = 0;
  /* worldYQ12 goes to translation.x and worldXQ12 to translation.y, as in the original: the parameter
     names are swapped relative to the node fields */
  effectModelNode->worldTransform.translation.x = worldYQ12;
  effectModelNode->worldTransform.translation.y = worldXQ12;
  effectModelNode->worldTransform.translation.z = worldZQ12;
  if ((effectDefinition->creationFlags & EFFECT_CREATION_RANDOMIZE_ORIENTATION) != 0) {
    randomValue = g_RandomGeneratorState.next();
    orientationAngle0 = randomValue & FIXED_ANGLE16_MASK;
  }
  effectModelNode->modelPayload.worldRotationAngle0 = orientationAngle2;
  effectModelNode->modelPayload.worldRotationAngle1 = orientationAngle1;
  effectModelNode->modelPayload.worldRotationAngle2 = orientationAngle0;
  chosenTextureSet = g_EffectTextureSet;
  chosenPalette = g_EffectPalette;
  if ((effectDefinition->creationFlags & EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET) != 0) {
    chosenTextureSet = g_ArmyGraphicsBindings[0].textureSet;
    chosenPalette = g_ArmyGraphicsBindings[0].paletteAsset;
  }
  nestedModelResource = (ModelResource *)effectDefinition->ownedNestedResource;
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
  effectSlot->animationFramesRemaining = frameCount;
  effectSlot->linkedEffectPresent = effectLinkPresent;
  effectSlot->linkedShotPresent = shotLinkPresent;
  scaleStartQ12 = effectDefinition->modelScaleStartQ12;
  scaleEndQ12 = effectDefinition->modelScaleEndQ12;
  effectModelNode->modelScaleQ12 = scaleStartQ12;
  /* a constant scale of 1.0 needs no per-vertex scaling */
  if ((scaleStartQ12 == Q12_ONE) && (scaleEndQ12 == Q12_ONE)) {
    effectModelNode->runtimeFlags = effectModelNode->runtimeFlags & ~MODEL_RUNTIME_FLAG_APPLY_SCALE;
  }
  shadingStartTicks = effectDefinition->shadingStartCountdownTicks;
  shadingStopTicks = effectDefinition->shadingStopCountdownTicks;
  effectSlot->shadingStartCountdownTicksRemaining = shadingStartTicks;
  effectSlot->shadingStopCountdownTicksRemaining = shadingStopTicks;
  effectSlot->stateTintArgb = 0xffffff; /* white */
  effectSlot->effectAgeTicks = 0;
  /* an immediate light at the model's light point, unless the shading starts later */
  if (shadingStartTicks == 0 &&
      ModelLookupTable_FindPackedPoint
            (0,MODEL_POINT_CLASS_LIGHT,(ModelResource *)effectDefinition->ownedNestedResource,&lightPoint)) {
    localPoint = ModelNodeRuntime_TransformLocalPoint(lightPoint,(ModelRuntimeNode *)effectModelNode);
    /* the alpha byte of the shading colour is the radius in 1/16 world units */
    effectModelNode->shadingRecord = GraphicsShadingRuntime_AllocateRecord
                       (effectDefinition->shadingTransitionDurationTicks,
                        (effectDefinition->shadingColorArgb >> 24) << 8,
                        effectDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
  }
  else {
    effectModelNode->shadingRecord = NULL;
  }
  completionCountdownTicks = effectDefinition->completionCountdownTicks;
  soundTableIndex = effectDefinition->soundSlotIndex;
  effectSlot->lifecycleOwnerAndDefinition.nextShotPointIndex = 0;
  effectSlot->lifecycleOwnerAndDefinition.animationFrameAccumulatorQ4 = 0;
  effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.owner = ownerRuntime;
  effectSlot->lifecycleOwnerAndDefinition.ownerAndDefinition.completionCountdownTicks = completionCountdownTicks;
  runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
  effectSlot->completionAction = completionAction;
  neighborhoodClassBits = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (Q12_ONE,effectModelNode->worldTransform.translation.y,
                      effectModelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  occupancyMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                     (TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED,0,neighborhoodClassBits,runtimeClassIndex);
  effectSlot->terrainRuntimeClassState = occupancyMasks.primaryOccupancyMask;
  effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | occupancyMasks.runtimeFlags | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
  effectModelNode->tintArgb = 0xffffff;
  if (soundTableIndex != 0 && soundTableIndex < worldRuntime->dwordArrayCount &&
      worldRuntime->dwordArray != NULL) {
    voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundTableIndex];
    if (voiceSetRef != NULL) {
      worldPosition = &effectModelNode->worldTransform.translation;
      projectedCellMasked = TerrainGrid_TestProjectedCellMaskBits01
                         (effectModelNode->worldTransform.translation.y,worldPosition->x,worldRuntime);
      if (!projectedCellMasked) {
        SpatialSound_PlayPositionedOneShot
                  (effectSlot->definitionOrSavedId.definition->positionedSoundMaximumDistanceQ12,
                   effectSlot->definitionOrSavedId.definition->positionedSoundGainQ15,worldPosition,voiceSetRef);
      }
    }
  }
  ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)effectModelNode);
  return effectSlot;
}

