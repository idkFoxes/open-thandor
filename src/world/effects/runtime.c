#include <thandor/world/effects/runtime.h>

/* Implementation ownership: world/effects/runtime. */

/* Address: 0x0051E120.
   Ownership: world/effects/runtime.
   Purpose: Scans the fixed 256-pointer effect-definition registry for definitionId. A match returns
   EffectDefinition * with CF clear. Failure writes the requested identifier to the package error buffer and
   returns error 0x48 with CF set. The stock corpus contains 140 unique EffectDefinition ids; serialized ids remain
   distinct from relocated EffectDefinition pointers and consumer-specific union facets.
*/
EffectDefinition * EffectRuntime_FindDefinitionByIdCf(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *arg4;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinition *candidateDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  while( true ) {
    arg4 = *registryCursor;
    if ((arg4 != (EffectDefinition *)0x0) && (arg4->definitionId == definitionId)) break;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)arg4,g_PackageLastErrorPath);
      return (EffectDefinition *)0x48;
    }
  }
  return arg4;
}

/* Address: 0x0051E190.
   Ownership: world/effects/runtime.
   Purpose: CF set propagates a load or allocation failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], MoviePlayback_AdvanceScheduledFrameAndTick
   [movie/runtime/playback].
*/
void EffectRuntime_InitGraphicsResources(word *mutableBasePath)

{
  EffectRuntimeSlot *runtimeSlotCursor;
  dword arg0;
  int runtimeSlotsRemaining;
  dword arg1;
  undefined1 in_CF;
  undefined8 uVar1;
  
  WidePath_SetExtensionCode(0x786667,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  (*(code *)g_GraphicsTextureSetLoadPackageCf)(mutableBasePath);
  if (!(bool)in_CF) {
    uVar1 = MoviePlayback_AdvanceScheduledFrameAndTick();
    g_EffectTextureSet = (GraphicsTextureSet *)uVar1;
    WidePath_SetExtensionCode(0x6c6170,mutableBasePath);
    (*g_GraphicsPaletteAssetLoadPackage)(arg0,arg1,mutableBasePath);
    if (!(bool)in_CF) {
      uVar1 = MoviePlayback_AdvanceScheduledFrameAndTick();
      g_EffectPalette = (GraphicsPaletteAsset *)uVar1;
      runtimeSlotCursor = (*g_MemoryApi.alloc)(0x40000);
      if (!(bool)in_CF) {
        g_EffectRuntimeRebaseBaseMinusOne = (byte *)((int)&runtimeSlotCursor[-1].effectAgeTicks + 3)
        ;
        g_EffectRuntimeSlots = runtimeSlotCursor;
        for (runtimeSlotsRemaining = 0x10000; runtimeSlotsRemaining != 0;
            runtimeSlotsRemaining = runtimeSlotsRemaining + -1) {
          (runtimeSlotCursor->definitionOrSavedId).definition = (EffectDefinition *)0x0;
          runtimeSlotCursor = (EffectRuntimeSlot *)&runtimeSlotCursor->modelNodeOrSavedOffset;
        }
      }
    }
  }
  return;
}

/* Address: 0x0051E210.
   Ownership: world/effects/runtime.
   Purpose: Releases the effect runtime pool and graphics resources, releases owned definition resources, clears
   the registry, and has no semantic normal return.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void EffectRuntime_ShutdownGraphicsResources(void)

{
  int registrySlotsRemaining;
  int extraout_ECX;
  EffectDefinition **registryCursor;
  EffectDefinition *currentDefinition;
  
  (*g_MemoryApi.free)(g_EffectRuntimeSlots);
  g_EffectRuntimeSlots = (EffectRuntimeSlot *)0x0;
  if (g_EffectTextureSet != (GraphicsTextureSet *)0x0) {
    (*(code *)g_GraphicsTextureSetReleasePackageCf)(g_EffectTextureSet);
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
      registrySlotsRemaining = extraout_ECX;
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
void __cdecl EffectRuntime_RebaseSlotsAfterLoad(void)

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
undefined8 __fastcall
EffectRuntimePool_CreateInstanceFromDefinitionCf
          (undefined4 param_1,undefined4 param_2,EffectRuntimeCompletionAction completionAction,
          EffectRuntimeOwnerReference4 ownerRuntime,AngleTurn32 orientationAngle0,
          AngleTurn32 orientationAngle1,AngleTurn32 orientationAngle2,Q12 worldZQ12,Q12 worldXQ12,
          Q12 worldYQ12,EffectDefinition *effectDefinition,WorldRuntimeContext *worldRuntime)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  Q12 QVar2;
  GameEntityRuntime *pGVar3;
  DefinitionReferencePresentFlag DVar4;
  DefinitionReferencePresentFlag DVar5;
  int iVar6;
  int iVar7;
  EffectShadingCountdownTicks EVar8;
  EffectShadingCountdownTicks EVar9;
  EffectRuntimeSlot *pEVar10;
  WorldRuntimeNode *node;
  ModelRuntimeNode *modelNodeRuntime;
  dword dVar11;
  GraphicsTextureSet *pGVar12;
  ModelPackedPointRecord *localPointRecord;
  uint uVar13;
  EffectRuntimeSlot *pEVar14;
  AngleTurn32 extraout_ECX;
  GraphicsPaletteAsset *pGVar15;
  undefined4 extraout_ECX_00;
  GraphicsWorldCoordinateQ12 worldYQ12_00;
  dword *extraout_ECX_01;
  dword *extraout_ECX_02;
  dword *extraout_ECX_03;
  dword *pdVar16;
  uint extraout_EDX;
  GraphicsFixedVec3 *worldPosition;
  ArmyRuntimeSlot *armySlot1;
  bool bVar17;
  longlong lVar18;
  ulonglong uVar19;
  undefined8 uVar20;
  char runtimeClassIndex;
  
  node = (WorldRuntimeNode *)0x14;
  pEVar14 = (EffectRuntimeSlot *)0x1000;
  armySlot1 = (ArmyRuntimeSlot *)g_EffectRuntimeSlots;
  pEVar10 = g_EffectRuntimeSlots;
  if (effectDefinition == (EffectDefinition *)0x0) {
EffectRuntimePool_CreateInstance_ReturnEffectSlotResult:
    return CONCAT44(param_2,armySlot1);
  }
  do {
    if (pEVar10 == (EffectRuntimeSlot *)0x0) {
EffectRuntimePool_CreateInstance_ReturnAllocationFailure:
      return CONCAT44(param_2,node);
    }
    bVar17 = false;
    if (((EffectModelNodeReferenceOrSavedOffset4 *)&armySlot1->modelNodeRuntime)->modelNode ==
        (ModelRuntimeNode *)0x0) {
      node = (WorldRuntimeNode *)WorldObjectArray_AllocateFreeRecordCf(worldRuntime);
      if (!bVar17) {
        modelNodeRuntime = (ModelRuntimeNode *)WorldRuntime_LinkNodeIntoOwnerListD8(node);
        ((EffectModelNodeReferenceOrSavedOffset4 *)&armySlot1->modelNodeRuntime)->modelNode =
             modelNodeRuntime;
        ((EffectDefinitionReferenceOrSavedId4 *)&armySlot1->definitionOrAsset)->definition =
             effectDefinition;
        modelNodeRuntime->ownerClassId = MODEL_RUNTIME_CLASS_02_TRACKED;
        (modelNodeRuntime->runtimePayload).armyRuntime = armySlot1;
        modelNodeRuntime->renderDepthBiasOrState = 0;
        (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
        (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
        (modelNodeRuntime->worldTransform).translation.z = worldZQ12;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_RANDOMIZE_ORIENTATION) != 0) {
          dVar11 = (*g_RandomGeneratorState.next)();
          orientationAngle0 = extraout_EDX & dVar11;
          orientationAngle1 = extraout_ECX;
        }
        (modelNodeRuntime->modelPayload).worldRotationAngle0 = orientationAngle2;
        (modelNodeRuntime->modelPayload).worldRotationAngle1 = orientationAngle1;
        (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle0;
        pGVar12 = g_EffectTextureSet;
        pGVar15 = g_EffectPalette;
        if ((effectDefinition->creationFlags & EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET) !=
            0) {
          pGVar12 = g_ArmyGraphicsBindings[0].textureSet;
          pGVar15 = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        pMVar1 = effectDefinition->ownedNestedResource;
        (modelNodeRuntime->modelPayload).textureSet = pGVar12;
        QVar2 = pMVar1->boundingRadiusQ12;
        (modelNodeRuntime->modelPayload).paletteAsset = pGVar15;
        modelNodeRuntime->subtreeBoundingRadiusQ12 = QVar2;
        (modelNodeRuntime->modelPayload).modelResource = pMVar1;
        pGVar3 = (GameEntityRuntime *)effectDefinition->animationFrameCount;
        DVar4 = effectDefinition->linkedEffectPresent;
        DVar5 = effectDefinition->linkedShotPresent;
        (modelNodeRuntime->modelPayload).meshGroupMask = 0xffffffff;
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 0x801;
        modelNodeRuntime->textureSubresourceBaseIndex = 0;
        modelNodeRuntime->modelRuntimeLinkOrSavedOffset = (void *)0x0;
        modelNodeRuntime->parentNode = (ModelRuntimeNode *)0x0;
        modelNodeRuntime->childCount = 0;
        armySlot1->linkedEntityRuntime = pGVar3;
        armySlot1->factionIndex = DVar4;
        (armySlot1->movementControl).movementAdvancePerTickQ12 = DVar5;
        iVar6 = effectDefinition->modelScaleStartQ12;
        iVar7 = effectDefinition->modelScaleEndQ12;
        modelNodeRuntime->modelScaleQ12 = iVar6;
        if ((iVar6 == 0x1000) && (iVar7 == 0x1000)) {
          modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & 0xfffff7ff;
        }
        EVar8 = effectDefinition->shadingStartCountdownTicks;
        EVar9 = effectDefinition->shadingStopCountdownTicks;
        armySlot1->commandCoordinate2Q12 = EVar8;
        armySlot1->commandModeFlags = EVar9;
        armySlot1->actionVector1Q12 = 0xffffff;
        armySlot1->actionVector2Q12 = 0;
        bVar17 = false;
        if ((EVar8 != 0) ||
           (localPointRecord =
                 (ModelPackedPointRecord *)
                 ModelLookupTable_ContainsPackedKeyCf(0,4,effectDefinition->ownedNestedResource),
           bVar17)) {
          modelNodeRuntime->shadingRecord = (GraphicsShadingRuntimeRecord *)0x0;
        }
        else {
          uVar20 = ModelNodeRuntime_TransformLocalPointRegs
                             (extraout_ECX_00,localPointRecord,modelNodeRuntime);
          lVar18 = GraphicsShadingRuntime_AllocateRecordRegs
                             (effectDefinition->shadingTransitionDurationTicks,
                              (effectDefinition->shadingColorArgb >> 0x18) << 8,
                              effectDefinition->shadingColorArgb,
                              (GraphicsWorldCoordinateQ12)((ulonglong)uVar20 >> 0x20),worldYQ12_00,
                              (GraphicsWorldCoordinateQ12)uVar20);
          modelNodeRuntime->shadingRecord = (GraphicsShadingRuntimeRecord *)lVar18;
        }
        dVar11 = effectDefinition->runtimeValue24;
        worldPosition = (GraphicsFixedVec3 *)effectDefinition->terrainGridMaskIndex;
        ((EffectRuntimeLifecycleState10 *)&(armySlot1->movementControl).turnVelocityAngle16)->
        runtimeState14 = 0;
        armySlot1->movementStateFlags = 0;
        ((EffectRuntimeOwnerAndDefinitionState8 *)&armySlot1->commandTargetArmyRuntime)->owner =
             ownerRuntime;
        armySlot1->commandCoordinate0Q12 = dVar11;
        runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
        armySlot1->commandCoordinate1Q12 = completionAction;
        uVar19 = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                           (0x1000,(modelNodeRuntime->worldTransform).translation.y,
                            (modelNodeRuntime->worldTransform).translation.x,worldRuntime->fieldGrid
                           );
        uVar13 = TerrainOccupancyMask_ResolveRuntimeClassFlags
                           (0x10,0,(FieldGridRegionMask)(uVar19 >> 0x20),runtimeClassIndex);
        armySlot1->actionVector0Q12 = (Q12)extraout_ECX_01;
        modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | uVar13 | 0x10;
        modelNodeRuntime->tintArgb = 0xffffff;
        pdVar16 = extraout_ECX_01;
        if ((((worldPosition != (GraphicsFixedVec3 *)0x0) &&
             (pdVar16 = worldRuntime->dwordArray,
             worldPosition < (GraphicsFixedVec3 *)worldRuntime->dwordArrayCount)) &&
            (pdVar16 != (dword *)0x0)) && (bVar17 = false, pdVar16[(int)worldPosition] != 0)) {
          uVar20 = TerrainGrid_TestProjectedCellMaskBits01Cf
                             ((modelNodeRuntime->worldTransform).translation.y,
                              (modelNodeRuntime->worldTransform).translation.x,worldRuntime);
          worldPosition = (GraphicsFixedVec3 *)((ulonglong)uVar20 >> 0x20);
          pdVar16 = extraout_ECX_02;
          if (!bVar17) {
            uVar20 = SpatialSound_PlayPositionedOneShot
                               (((EffectDefinitionReferenceOrSavedId4 *)
                                &armySlot1->definitionOrAsset)->definition->
                                positionedSoundMaximumDistanceQ12,
                                ((EffectDefinitionReferenceOrSavedId4 *)
                                &armySlot1->definitionOrAsset)->definition->positionedSoundGainQ15,
                                worldPosition,(DirectSoundVoiceSet **)uVar20);
            worldPosition = (GraphicsFixedVec3 *)((ulonglong)uVar20 >> 0x20);
            pdVar16 = extraout_ECX_03;
          }
        }
        ModelNodeRuntime_RebuildTransformsFromRoot(pdVar16,worldPosition,modelNodeRuntime);
        goto EffectRuntimePool_CreateInstance_ReturnEffectSlotResult;
      }
      goto EffectRuntimePool_CreateInstance_ReturnAllocationFailure;
    }
    pEVar14 = (EffectRuntimeSlot *)((int)&pEVar14[-1].effectAgeTicks + 3);
    armySlot1 = (ArmyRuntimeSlot *)&armySlot1->runtimeState40;
    pEVar10 = pEVar14;
  } while( true );
}
