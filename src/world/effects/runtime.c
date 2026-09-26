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
EffectDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectRuntime_FindDefinitionByIdCf(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *registryDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionLookupEaxCf5 failureResult;
  EffectDefinitionLookupEaxCf5 foundResult;
  EffectDefinition *candidateDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((registryDefinition = *registryCursor, registryDefinition == (EffectDefinition *)0x0 ||
         (registryDefinition->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)registryDefinition,g_PackageLastErrorPath);
      failureResult.carry = true;
      failureResult.definitionOrError = (EffectDefinition *)0x48;
      return failureResult;
    }
  }
  foundResult.carry = false;
  foundResult.definitionOrError = registryDefinition;
  return foundResult;
}


/* Address: 0x0051E190.
   Ownership: world/effects/runtime.
   Purpose: CF set propagates a load or allocation failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], MoviePlayback_AdvanceScheduledFrameAndTick
   [movie/runtime/playback].
*/
StatusValueEaxCf5 EffectRuntime_InitGraphicsResources(word *mutableBasePath)

{
  EffectRuntimeSlot *runtimeSlotCursor;
  int runtimeSlotsRemaining;
  ArenaAllocEaxCf5 loadOrAllocResult;
  StatusValueEaxCf5 statusResult;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadOrAllocResult = THANDOR_BITCAST(GraphicsTextureSetEaxCf5, ArenaAllocEaxCf5, (*g_GraphicsTextureSetLoadPackageCf)(mutableBasePath));
  if (!loadOrAllocResult.carry) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_EffectTextureSet = (GraphicsTextureSet *)loadOrAllocResult.eax;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    loadOrAllocResult = THANDOR_BITCAST(GraphicsPaletteAssetEaxCf5, ArenaAllocEaxCf5, (*g_GraphicsPaletteAssetLoadPackage)(mutableBasePath));
    if (!loadOrAllocResult.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_EffectPalette = (GraphicsPaletteAsset *)loadOrAllocResult.eax;
      loadOrAllocResult = (*g_MemoryApi.alloc)(0x40000);
      runtimeSlotCursor = (EffectRuntimeSlot *)loadOrAllocResult.eax;
      if (!loadOrAllocResult.carry) {
        g_EffectRuntimeRebaseBaseMinusOne = (byte *)((int)&runtimeSlotCursor[-1].effectAgeTicks + 3)
        ;
        g_EffectRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (EffectDefinition *)0x0;
          runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
        }
        loadOrAllocResult.eax = 0;
        loadOrAllocResult.carry = false;
      }
    }
  }
  statusResult.valueOrError = loadOrAllocResult.eax;
  statusResult.carry = loadOrAllocResult.carry;
  return statusResult;
}


/* Address: 0x0051E210.
   Ownership: world/effects/runtime.
   Purpose: Releases the effect runtime pool and graphics resources, releases owned definition resources, clears
   the registry, and has no semantic normal return.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx EffectRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinition *currentDefinition;
  
  (*g_MemoryApi.free)(g_EffectRuntimeSlots);
  g_EffectRuntimeSlots = (EffectRuntimeSlot *)0x0;
  if (g_EffectTextureSet != (GraphicsTextureSet *)0x0) {
    (*g_GraphicsTextureSetReleasePackageCf)(g_EffectTextureSet);
    g_EffectTextureSet = (GraphicsTextureSet *)0x0;
  }
  if (g_EffectPalette != (GraphicsPaletteAsset *)0x0) {
    (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_EffectPalette);
    g_EffectPalette = (GraphicsPaletteAsset *)0x0;
  }
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  do {
    currentDefinition = *registryCursor;
    if ((currentDefinition != (EffectDefinition *)0x0) &&
       (currentDefinition->ownedNestedResourcePresent != 0)) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = (EffectDefinition *)0x0;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  return;
}


/* Address: 0x0051E340.
   Ownership: world/effects/runtime.
   Purpose: Rebases all 4096 live effect slots after a serialized image is restored and resolves each saved
   definitionId through the effect registry. Serialized ids and relocated EffectDefinition pointers remain
   separate; consumer-specific union facets are not generalized.
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
  effectSlotsRemaining = 0x1000;
  g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
  do {
    slotCompletionAction = effectSlot->completionAction;
    ownerModelNode = (effectSlot->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
    if ((effectSlot->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      if (ownerModelNode != (ModelRuntimeNode *)0x0) {
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
      registrySlotsRemaining = 0x100;
      do {
        registryDefinition = *registryCursor;
        if ((registryDefinition != (EffectDefinition *)0x0) &&
           ((effectSlot->definitionOrSavedId).definition ==
            (EffectDefinition *)registryDefinition->definitionId))
        goto EffectRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance;
        registryCursor = registryCursor + 1;
        registrySlotsRemaining = registrySlotsRemaining + -1;
      } while (registrySlotsRemaining != 0);
      (effectSlot->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
EffectRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance:
      (effectSlot->definitionOrSavedId).definition = registryDefinition;
    }
    effectSlot = effectSlot + 1;
    effectSlotsRemaining = effectSlotsRemaining + -1;
    if (effectSlotsRemaining == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x0051E4A0.
   Ownership: world/effects/runtime.
   Purpose: Allocates one EffectRuntimeSlot and model node from a typed EffectDefinition; creationFlags select
   owner rebasing and graphics behavior. EAX/CF remain the nonstandard result channels. Allocates from the
   4096-slot effect pool; binds the effect mesh from effectDefinition->ownedNestedResource ->
   modelPayload.modelResource (SPR) — M7 real kit source; create-time rest euler = world (0, 0x4000, 0), same as
   army placement (W8). Unity def scales clear ModelRuntimeNode flag 0x800 here (M4, L123842). Role: Allocates and
   initializes one live effect instance from an EffectDefinition.
   Cross-module calls: WorldObjectArray_AllocateFreeRecordCf [world/runtime/core],
   WorldRuntime_LinkNodeIntoOwnerListD8 [world/runtime/core], ModelLookupTable_ContainsPackedKeyCf
   [assets/model/definitions], ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy],
   GraphicsShadingRuntime_AllocateRecordRegs [graphics/render/shading],
   TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint [world/terrain/occupancy].
*/
EffectRuntimeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectRuntimePool_CreateInstanceFromDefinitionCf
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
  EffectRuntimeSlot *poolOrSlotsRemaining;
  EffectModelRuntimeNodeClassView100 *effectModelNode;
  dword randomOrRuntimeValue;
  GraphicsTextureSet *chosenTextureSet;
  EffectRuntimeSlot *slotsRemaining;
  GraphicsPaletteAsset *chosenPalette;
  EffectRuntimeSlot *effectRuntimeCursor;
  bool projectedCellMasked;
  WorldObjectRecordEaxCf5 recordAlloc;
  ModelLookupEntryEaxCf5 lookupEntry;
  GraphicsShadingRuntimeRecordEaxCf5 shadingAlloc;
  EffectRuntimeCreateEaxCf5 successResult;
  EffectRuntimeCreateEaxCf5 failureResult;
  ModelLocalPointRegs12 localPoint;
  TerrainOccupancyResolvedMasksRegs12 occupancyMasks;
  char runtimeClassIndex;
  uint soundTableIndex;
  
  effectModelNode = (EffectModelRuntimeNodeClassView100 *)0x14;
  slotsRemaining = (EffectRuntimeSlot *)0x1000;
  effectRuntimeCursor = g_EffectRuntimeSlots;
  poolOrSlotsRemaining = g_EffectRuntimeSlots;
  if (effectDefinition == (EffectDefinition *)0x0) {
EffectRuntimePool_CreateInstance_ReturnEffectSlotResult:
    successResult.carry = false;
    successResult.effectRuntime = effectRuntimeCursor;
    return successResult;
  }
  do {
    if (poolOrSlotsRemaining == (EffectRuntimeSlot *)0x0) {
EffectRuntimePool_CreateInstance_ReturnAllocationFailure:
      failureResult.carry = true;
      failureResult.effectRuntime = (EffectRuntimeSlot *)effectModelNode;
      return failureResult;
    }
    if ((effectRuntimeCursor->modelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) {
      recordAlloc = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
      effectModelNode = (EffectModelRuntimeNodeClassView100 *)recordAlloc.recordOrError;
      if (!recordAlloc.carry) {
        WorldRuntime_LinkNodeIntoOwnerListD8((WorldOwnerListNode100 *)effectModelNode);
        (effectRuntimeCursor->modelNodeOrSavedOffset).modelNode =
             (ModelRuntimeNode *)effectModelNode;
        (effectRuntimeCursor->definitionOrSavedId).definition = effectDefinition;
        effectModelNode->ownerClassId = MODEL_RUNTIME_CLASS_02_TRACKED;
        effectModelNode->effectRuntime = effectRuntimeCursor;
        effectModelNode->renderDepthBiasOrState = 0;
        (effectModelNode->worldTransform).translation.x = worldYQ12;
        (effectModelNode->worldTransform).translation.y = worldXQ12;
        (effectModelNode->worldTransform).translation.z = worldZQ12;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_RANDOMIZE_ORIENTATION) != 0) {
          randomOrRuntimeValue = (*g_RandomGeneratorState.next)();
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
        (effectModelNode->modelPayload).meshGroupMask = 0xffffffff;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | 0x801;
        effectModelNode->textureSubresourceBaseIndex = 0;
        effectModelNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
        effectModelNode->parentNode = (ModelRuntimeNode *)0x0;
        effectModelNode->childCount = 0;
        effectRuntimeCursor->animationFramesRemaining = frameCount;
        effectRuntimeCursor->linkedEffectPresent = effectLinkPresent;
        effectRuntimeCursor->linkedShotPresent = shotLinkPresent;
        scaleStartQ12 = effectDefinition->modelScaleStartQ12;
        scaleEndQ12 = effectDefinition->modelScaleEndQ12;
        effectModelNode->modelScaleQ12 = scaleStartQ12;
        if ((scaleStartQ12 == 0x1000) && (scaleEndQ12 == 0x1000)) {
          effectModelNode->runtimeFlags = effectModelNode->runtimeFlags & 0xfffff7ff;
        }
        shadingStartTicks = effectDefinition->shadingStartCountdownTicks;
        shadingStopTicks = effectDefinition->shadingStopCountdownTicks;
        effectRuntimeCursor->shadingStartCountdownTicksRemaining = shadingStartTicks;
        effectRuntimeCursor->shadingStopCountdownTicksRemaining = shadingStopTicks;
        effectRuntimeCursor->stateTintArgb = 0xffffff;
        effectRuntimeCursor->effectAgeTicks = 0;
        if (shadingStartTicks == 0) {
          lookupEntry = ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition->ownedNestedResource);
          if (lookupEntry.carry) goto LAB_0051e672;
          localPoint = ModelNodeRuntime_TransformLocalPointRegs
                             (lookupEntry.entry,(ModelRuntimeNode *)effectModelNode);
          shadingAlloc = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition->shadingTransitionDurationTicks,
                              (effectDefinition->shadingColorArgb >> 0x18) << 8,
                              effectDefinition->shadingColorArgb,localPoint.edx,localPoint.ecx,localPoint.eax);
          effectModelNode->shadingRecord = shadingAlloc.record;
        }
        else {
LAB_0051e672:
          effectModelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
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
                           (0x1000,(effectModelNode->worldTransform).translation.y,
                            (effectModelNode->worldTransform).translation.x,worldRuntime->fieldGrid)
        ;
        occupancyMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags(0x10,0,randomOrRuntimeValue,runtimeClassIndex);
        effectRuntimeCursor->terrainRuntimeClassState = occupancyMasks.primaryOccupancyMask;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | occupancyMasks.runtimeFlags | 0x10;
        effectModelNode->tintArgb = 0xffffff;
        if ((((soundTableIndex != 0) && (soundTableIndex < worldRuntime->dwordArrayCount)) &&
            (worldRuntime->dwordArray != (dword *)0x0)) &&
           (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[soundTableIndex],
           voiceSetRef != (DirectSoundVoiceSet **)0x0)) {
          worldPosition = &(effectModelNode->worldTransform).translation;
          projectedCellMasked = TerrainGrid_TestProjectedCellMaskBits01Cf
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
        goto EffectRuntimePool_CreateInstance_ReturnEffectSlotResult;
      }
      goto EffectRuntimePool_CreateInstance_ReturnAllocationFailure;
    }
    effectRuntimeCursor = effectRuntimeCursor + 1;
    slotsRemaining = (EffectRuntimeSlot *)((int)&slotsRemaining[-1].effectAgeTicks + 3);
    poolOrSlotsRemaining = slotsRemaining;
  } while( true );
}

