/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/effects/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/effects/runtime.h>

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
  EffectDefinition *arg4;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionLookupEaxCf5 EVar1;
  EffectDefinitionLookupEaxCf5 EVar2;
  EffectDefinition *candidateDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while ((arg4 = *registryCursor, arg4 == (EffectDefinition *)0x0 ||
         (arg4->definitionId != definitionId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      EVar1.carry = true;
      EVar1.definitionOrError = (EffectDefinition *)0x48;
      return EVar1;
    }
  }
  EVar2.carry = false;
  EVar2.definitionOrError = arg4;
  return EVar2;
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
  ArenaAllocEaxCf5 AVar1;
  StatusValueEaxCf5 SVar2;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  AVar1 = (ArenaAllocEaxCf5)(*g_GraphicsTextureSetLoadPackageCf)(mutableBasePath);
  if (!AVar1.carry) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    g_EffectTextureSet = (GraphicsTextureSet *)AVar1.eax;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    AVar1 = (ArenaAllocEaxCf5)(*g_GraphicsPaletteAssetLoadPackage)(mutableBasePath);
    if (!AVar1.carry) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_EffectPalette = (GraphicsPaletteAsset *)AVar1.eax;
      AVar1 = (*g_MemoryApi.alloc)(0x40000);
      runtimeSlotCursor = (EffectRuntimeSlot *)AVar1.eax;
      if (!AVar1.carry) {
        g_EffectRuntimeRebaseBaseMinusOne = (byte *)((int)&runtimeSlotCursor[-1].effectAgeTicks + 3)
        ;
        g_EffectRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (EffectDefinition *)0x0;
          runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
        }
        AVar1.eax = 0;
        AVar1.carry = false;
      }
    }
  }
  SVar2.valueOrError = AVar1.eax;
  SVar2.carry = AVar1.carry;
  return SVar2;
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
  EffectRuntimeCompletionAction EVar1;
  int iVar2;
  int iVar3;
  ModelRuntimeNode *modelNode1;
  EffectDefinition **ppEVar4;
  EffectRuntimeSlot *pEVar5;
  EffectDefinition *effectDefinition1;
  
  pEVar5 = g_EffectRuntimeSlots;
  iVar3 = 0x1000;
  g_EffectRuntimeSlots->effectAgeTicks = ~g_EffectRuntimeSlots->effectAgeTicks;
  do {
    EVar1 = pEVar5->completionAction;
    modelNode1 = (pEVar5->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode;
    if ((pEVar5->modelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      if (modelNode1 != (ModelRuntimeNode *)0x0) {
        if (EVar1 == EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY) {
          modelNode1 = (ModelRuntimeNode *)
                       ((modelNode1->modelPayload).reserved2C_33 + g_ModelRuntimeRebaseDelta + -0x38
                       );
        }
        else if (EVar1 == EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL) {
          modelNode1 = (ModelRuntimeNode *)
                       ((int)g_ArmyRuntimeRebaseBaseMinusOne +
                       (int)(&modelNode1->modelPayload + -1) + 0x30);
        }
      }
      (pEVar5->modelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((pEVar5->modelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30);
      pEVar5->completionAction = EVar1;
      (pEVar5->lifecycleOwnerAndDefinition).ownerAndDefinition.owner.modelNode = modelNode1;
      ppEVar4 = g_EffectDefinitionRegistry;
      iVar2 = 0x100;
      do {
        effectDefinition1 = *ppEVar4;
        if ((effectDefinition1 != (EffectDefinition *)0x0) &&
           ((pEVar5->definitionOrSavedId).definition ==
            (EffectDefinition *)effectDefinition1->definitionId))
        goto EffectRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance;
        ppEVar4 = ppEVar4 + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      (pEVar5->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
EffectRuntime_RebaseSlotsAfterLoad_CommitResolvedDefinitionAndAdvance:
      (pEVar5->definitionOrSavedId).definition = effectDefinition1;
    }
    pEVar5 = pEVar5 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
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
  ModelResourceHitTestAndRenderView210 *pMVar1;
  Q12 QVar2;
  EffectAnimationFrameCount EVar3;
  DefinitionReferencePresentFlag DVar4;
  DefinitionReferencePresentFlag DVar5;
  int iVar6;
  int iVar7;
  EffectShadingCountdownTicks EVar8;
  EffectShadingCountdownTicks EVar9;
  DirectSoundVoiceSet **voiceSetRef;
  EffectRuntimeSlot *pEVar10;
  EffectModelRuntimeNodeClassView100 *effectModelNode;
  dword dVar11;
  GraphicsTextureSet *pGVar12;
  EffectRuntimeSlot *pEVar13;
  GraphicsPaletteAsset *pGVar14;
  EffectRuntimeSlot *effectRuntimeCursor;
  bool bVar15;
  WorldObjectRecordEaxCf5 WVar16;
  ModelLookupEntryEaxCf5 MVar17;
  GraphicsShadingRuntimeRecordEaxCf5 GVar18;
  EffectRuntimeCreateEaxCf5 EVar19;
  EffectRuntimeCreateEaxCf5 EVar20;
  ModelLocalPointRegs12 MVar21;
  TerrainOccupancyResolvedMasksRegs12 TVar22;
  char runtimeClassIndex;
  uint uVar23;
  
  effectModelNode = (EffectModelRuntimeNodeClassView100 *)0x14;
  pEVar13 = (EffectRuntimeSlot *)0x1000;
  effectRuntimeCursor = g_EffectRuntimeSlots;
  pEVar10 = g_EffectRuntimeSlots;
  if (effectDefinition == (EffectDefinition *)0x0) {
EffectRuntimePool_CreateInstance_ReturnEffectSlotResult:
    EVar19.carry = false;
    EVar19.effectRuntime = effectRuntimeCursor;
    return EVar19;
  }
  do {
    if (pEVar10 == (EffectRuntimeSlot *)0x0) {
EffectRuntimePool_CreateInstance_ReturnAllocationFailure:
      EVar20.carry = true;
      EVar20.effectRuntime = (EffectRuntimeSlot *)effectModelNode;
      return EVar20;
    }
    if ((effectRuntimeCursor->modelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) {
      WVar16 = WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
      effectModelNode = (EffectModelRuntimeNodeClassView100 *)WVar16.recordOrError;
      if (!WVar16.carry) {
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
          dVar11 = (*g_RandomGeneratorState.next)();
          orientationAngle0 = dVar11 & 0xffff;
        }
        (effectModelNode->modelPayload).worldRotationAngle0 = orientationAngle2;
        (effectModelNode->modelPayload).worldRotationAngle1 = orientationAngle1;
        (effectModelNode->modelPayload).worldRotationAngle2 = orientationAngle0;
        pGVar12 = g_EffectTextureSet;
        pGVar14 = g_EffectPalette;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET) !=
            0) {
          pGVar12 = g_ArmyGraphicsBindings[0].textureSet;
          pGVar14 = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        pMVar1 = effectDefinition->ownedNestedResource;
        (effectModelNode->modelPayload).textureSet = pGVar12;
        QVar2 = pMVar1->boundingRadiusQ12;
        (effectModelNode->modelPayload).paletteAsset = pGVar14;
        effectModelNode->subtreeBoundingRadiusQ12 = QVar2;
        (effectModelNode->modelPayload).modelResource = pMVar1;
        EVar3 = effectDefinition->animationFrameCount;
        DVar4 = effectDefinition->linkedEffectPresent;
        DVar5 = effectDefinition->linkedShotPresent;
        (effectModelNode->modelPayload).meshGroupMask = 0xffffffff;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | 0x801;
        effectModelNode->textureSubresourceBaseIndex = 0;
        effectModelNode->modelRuntimeLinkOrSavedOffset = (void *)0x0;
        effectModelNode->parentNode = (ModelRuntimeNode *)0x0;
        effectModelNode->childCount = 0;
        effectRuntimeCursor->animationFramesRemaining = EVar3;
        effectRuntimeCursor->linkedEffectPresent = DVar4;
        effectRuntimeCursor->linkedShotPresent = DVar5;
        iVar6 = effectDefinition->modelScaleStartQ12;
        iVar7 = effectDefinition->modelScaleEndQ12;
        effectModelNode->modelScaleQ12 = iVar6;
        if ((iVar6 == 0x1000) && (iVar7 == 0x1000)) {
          effectModelNode->runtimeFlags = effectModelNode->runtimeFlags & 0xfffff7ff;
        }
        EVar8 = effectDefinition->shadingStartCountdownTicks;
        EVar9 = effectDefinition->shadingStopCountdownTicks;
        effectRuntimeCursor->shadingStartCountdownTicksRemaining = EVar8;
        effectRuntimeCursor->shadingStopCountdownTicksRemaining = EVar9;
        effectRuntimeCursor->stateTintArgb = 0xffffff;
        effectRuntimeCursor->effectAgeTicks = 0;
        if (EVar8 == 0) {
          MVar17 = ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition->ownedNestedResource);
          if (MVar17.carry) goto LAB_0051e672;
          MVar21 = ModelNodeRuntime_TransformLocalPointRegs
                             (MVar17.entry,(ModelRuntimeNode *)effectModelNode);
          GVar18 = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition->shadingTransitionDurationTicks,
                              (effectDefinition->shadingColorArgb >> 0x18) << 8,
                              effectDefinition->shadingColorArgb,MVar21.edx,MVar21.ecx,MVar21.eax);
          effectModelNode->shadingRecord = GVar18.record;
        }
        else {
LAB_0051e672:
          effectModelNode->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
        }
        dVar11 = effectDefinition->runtimeValue24;
        uVar23 = effectDefinition->terrainGridMaskIndex;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).runtimeState14 = 0;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).animationFrameAccumulatorQ4 = 0;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.owner = ownerRuntime;
        (effectRuntimeCursor->lifecycleOwnerAndDefinition).ownerAndDefinition.runtimeValue24 =
             dVar11;
        runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
        effectRuntimeCursor->completionAction = completionAction;
        dVar11 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                           (0x1000,(effectModelNode->worldTransform).translation.y,
                            (effectModelNode->worldTransform).translation.x,worldRuntime->fieldGrid)
        ;
        TVar22 = TerrainOccupancyMask_ResolveRuntimeClassFlags(0x10,0,dVar11,runtimeClassIndex);
        effectRuntimeCursor->terrainRuntimeClassState = TVar22.primaryOccupancyMask;
        effectModelNode->runtimeFlags = effectModelNode->runtimeFlags | TVar22.runtimeFlags | 0x10;
        effectModelNode->tintArgb = 0xffffff;
        if ((((uVar23 != 0) && (uVar23 < worldRuntime->dwordArrayCount)) &&
            (worldRuntime->dwordArray != (dword *)0x0)) &&
           (voiceSetRef = (DirectSoundVoiceSet **)worldRuntime->dwordArray[uVar23],
           voiceSetRef != (DirectSoundVoiceSet **)0x0)) {
          worldPosition = &(effectModelNode->worldTransform).translation;
          bVar15 = TerrainGrid_TestProjectedCellMaskBits01Cf
                             ((effectModelNode->worldTransform).translation.y,worldPosition->x,
                              worldRuntime);
          if (!bVar15) {
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
    pEVar13 = (EffectRuntimeSlot *)((int)&pEVar13[-1].effectAgeTicks + 3);
    pEVar10 = pEVar13;
  } while( true );
}

