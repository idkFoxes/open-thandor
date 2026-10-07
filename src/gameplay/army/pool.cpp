/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/pool.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/pool.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) void *g_ArmyRuntimeRebaseBaseMinusOne = nullptr;

ArmyRuntimeSlot *g_ArmyRuntimeSlots = nullptr;

ArmyGraphicsBinding g_ArmyGraphicsBindings[8] = {};

/* uint32_t[24] depth-bin/occupancy class per model runtime class (0x88/0x90/0xA0/0xC0; 0x90 = structure), copied to ArmyRuntimeSlot.depthBinClass; gameplay/army runtime and placement */
const uint32_t g_ArmyRuntimeDepthBinClassByModelClass[24] = {
    /*  0 */ 136, 192, 192, 192, 144, 136, 136, 136, 136, 136, 136, 144, 136, 144, 144, 144,
    /* 16 */ 144, 192, 160, 192, 144, 192, 144, 144};

/* Level start: allocates and zeroes the 0x48000-byte army runtime pool, loads the army graphics (texture set and
   palette, "<graphicsBasePath><suffix>.gfx/.pal") of slot 0 and of every existing faction, and renders the two
   panel preview textures of every army asset that has a selection panel entry. The movie schedule is ticked in between, since this
   runs behind the level-loading movie. Returns true on success (*outError = 0); on an allocation or graphics
   load error returns false with that error in *outError. A failed preview render is skipped silently.
*/
Bool8 ArmyRuntime_InitializePoolAndGraphics(void *ownerContext,uint16_t *graphicsBasePath,uint32_t *outError)

{
  uint16_t pathChar;
  uint32_t factionGraphicsVariant;
  ArmyAssetRecordPrefix *armyAsset;
  ArmyRuntimeSlot *armyPool;
  uint32_t *poolDwordCursor;
  GraphicsPaletteAsset *paletteAsset;
  GraphicsTextureSourceAsset *textureSourceAsset;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPixelDimension previewHeight;
  int remainingCount;
  int factionSuffixChar;
  Bool8 loadFactionGraphics;
  int frontendPlayerRuntimeId;
  ArmyAssetRecordPrefix **registryCursor;
  uint16_t *pathCursor;
  uint16_t *pathEnd;
  uint32_t allocError;
  GraphicsTextureResource *previewTexture;

  allocError = g_MemoryApi.alloc(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),reinterpret_cast<void **>(&armyPool));
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  /* base - 1: the rebase value for saved offsets, see ArmyRuntimePool_RebaseAfterLoad */
  g_ArmyRuntimeRebaseBaseMinusOne = Thandor_Bytes(armyPool) - 1;
  g_ArmyRuntimeSlots = armyPool;
  poolDwordCursor = reinterpret_cast<uint32_t *>(armyPool);
  for (remainingCount = ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot) / 4; remainingCount != 0; remainingCount--) {
    *poolDwordCursor = 0;
    poolDwordCursor++;
  }
  /* find the end of graphicsBasePath (at most 32 code units); pathEnd ends up on the terminator (or on the
     last of the 32 code units) and the suffix digit is written there */
  pathCursor = graphicsBasePath;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    pathChar = *pathCursor;
    pathCursor++;
    if (pathChar == 0) {
      break;
    }
  }
  pathEnd = pathCursor - 1;
  g_MoviePlaybackScheduleSpan = 26;
  for (frontendPlayerRuntimeId = 0; frontendPlayerRuntimeId < ARMY_GRAPHICS_BINDING_COUNT;
       frontendPlayerRuntimeId++) {
    MoviePlayback_AdvanceScheduledFrameAndTick();
    factionSuffixChar = '0';
    /* Slot 0 always loads the "0" graphics; the other slots load theirs (suffix 0-9/A-Z from the faction's
       graphics variant) only while the faction exists. */
    loadFactionGraphics = frontendPlayerRuntimeId == 0;
    if (!loadFactionGraphics) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[frontendPlayerRuntimeId] != 0) {
        /* the faction's colour index selects its graphics */
        factionGraphicsVariant = g_GameFactionRuntimeImage.records[frontendPlayerRuntimeId].colorIndex;
        g_MoviePlaybackScheduleCounter--;
        if (factionGraphicsVariant < 10) {
          factionSuffixChar = factionGraphicsVariant + '0';
        }
        else {
          factionSuffixChar = factionGraphicsVariant + ('A' - 10);
        }
        loadFactionGraphics = true;
      }
    }
    if (loadFactionGraphics) {
      *reinterpret_cast<int *>(pathEnd) = factionSuffixChar; /* the suffix and a terminator in one dword */
      WidePath_SetExtensionCode(ASSET_MAGIC_GFX,graphicsBasePath);
      textureSourceAsset = static_cast<GraphicsTextureSourceAsset *>(Package_LoadEntry(graphicsBasePath,outError));
      if (textureSourceAsset == nullptr) {
        return false;
      }
      ArmyGraphics_CopyFrontendPlayerPaletteAndTexture
                (frontendPlayerRuntimeId,reinterpret_cast<ArmyGraphicsAssetAddress32>(textureSourceAsset));
      loadedTextureSet = g_GraphicsCreateTextureSet(textureSourceAsset,outError);
      if (loadedTextureSet == nullptr) {
        Resource_Release(textureSourceAsset);
        return false;
      }
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_ArmyGraphicsBindings[frontendPlayerRuntimeId].textureSet = loadedTextureSet;
      WidePath_SetExtensionCode(ASSET_MAGIC_PAL,graphicsBasePath);
      paletteAsset = g_GraphicsPaletteAssetLoadPackage(graphicsBasePath,outError);
      if (paletteAsset == nullptr) {
        return false;
      }
      g_ArmyGraphicsBindings[frontendPlayerRuntimeId].paletteAsset = paletteAsset;
    }
    MoviePlayback_AdvanceScheduledFrameAndTick();
  }
  pathEnd[0] = 0;
  pathEnd[1] = 0;
  /* Preview textures for army assets with a non-zero selection detail variant (bits 1-7 of
     armyAsset[1].selectionDetailTemplateVariantIndex): the panel size one (subresource 34) into
     armyAsset[1].rootNodeOffsetOrPointer, a third of the subresource-2 width into armyAsset[1].registryId. */
  registryCursor = g_ArmyAssetRecordRegistry;
  for (remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT; remainingCount != 0; remainingCount--) {
    armyAsset = *registryCursor;
    if ((armyAsset != nullptr) &&
       ((armyAsset[1].selectionDetailTemplateVariantIndex & ARMY_ASSET_FLAG_PRODUCTION_MASK) != 0)) {
      previewTexture = ArmyRuntime_RenderPreviewTexture
                         (g_InGamePanelTextureSubresource34Height,
                          g_InGamePanelTextureSubresource34Width,
                          static_cast<WorldRuntimeContext *>(ownerContext)->activeFactionRuntimeIndex,armyAsset->registryId,
                          static_cast<WorldRuntimeContext *>(ownerContext));
      if (previewTexture != nullptr) {
        armyAsset[1].rootNodeOffsetOrPointer = Thandor_PointerToU32(previewTexture); /* 32-bit format field: ArmyAssetRecord[1].rootNodeOffsetOrPointer (preview texture) */
        previewHeight =
             (GraphicsPixelDimension)
             ((uint64_t)(int64_t)g_InGamePanelTextureSubresource02Width / 3);
        previewTexture = ArmyRuntime_RenderPreviewTexture
                           (previewHeight,previewHeight,
                            static_cast<WorldRuntimeContext *>(ownerContext)->activeFactionRuntimeIndex,armyAsset->registryId,
                            static_cast<WorldRuntimeContext *>(ownerContext));
        if (previewTexture != nullptr) {
          armyAsset[1].registryId = Thandor_PointerToI32(previewTexture); /* 32-bit format field: ArmyAssetRecord[1].registryId (preview texture) */
        }
      }
    }
    registryCursor++;
  }
  *outError = 0;
  return true;
}

/* Counterpart of ArmyRuntime_InitializePoolAndGraphics: frees the army runtime pool, releases every faction's
   army texture set and palette, frees the two preview textures (armyAsset[1].rootNodeOffsetOrPointer and
   .registryId) of every registered army asset and clears the asset registry.
*/
void ArmyRuntime_ShutdownPoolAndGraphics()

{
  ArmyAssetRecordPrefix *armyAsset;
  ArmyGraphicsBinding *graphicsBinding;
  int bindingIndex;
  int registryIndex;

  g_MemoryApi.free(g_ArmyRuntimeSlots);
  g_ArmyRuntimeSlots = nullptr;
  for (bindingIndex = 0; bindingIndex < ARMY_GRAPHICS_BINDING_COUNT; bindingIndex++) {
    graphicsBinding = &g_ArmyGraphicsBindings[bindingIndex];
    if (graphicsBinding->textureSet != nullptr) {
      g_GraphicsTextureSetReleasePackage(graphicsBinding->textureSet);
      graphicsBinding->textureSet = nullptr;
    }
    if (graphicsBinding->paletteAsset != nullptr) {
      g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(graphicsBinding->paletteAsset);
      graphicsBinding->paletteAsset = nullptr;
    }
  }
  for (registryIndex = 0; registryIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; registryIndex++) {
    armyAsset = g_ArmyAssetRecordRegistry[registryIndex];
    if (armyAsset != nullptr) {
      /* the two preview textures stored in the record that follows the prefix */
      g_MemoryApi.free(Thandor_U32ToPointer<void>(armyAsset[1].rootNodeOffsetOrPointer)); /* 32-bit format field: ArmyAssetRecord[1].rootNodeOffsetOrPointer (preview texture) */
      g_MemoryApi.free(Thandor_U32ToPointer<void>(armyAsset[1].registryId)); /* 32-bit format field: ArmyAssetRecord[1].registryId (preview texture) */
      g_ArmyAssetRecordRegistry[registryIndex] = nullptr;
    }
  }
}

/* Removes an army for good: destroys its model hierarchy, drops every reference to it (player selections, the
   world selection, owned-model links of other nodes, each player's primary selection, the faction group
   tables), frees its runtime slot (model node = NULL) and rebuilds the in-game catalog grids and the
   selection detail panel.
*/
void ArmyRuntime_DestroyInstanceAndRefreshUi(WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *modelRuntime;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  SelectionPlayerRuntimeBlock *playerSelectionBlock;

  modelRuntime = (entityRuntime->common).ownership.modelRuntime();
  if (modelRuntime != nullptr) {
    (entityRuntime->common).ownership.definitionOrClassRecord = nullptr;
    ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime);
  }
  SelectionPlayerBlocks_RemovePointer(entityRuntime);
  if (entityRuntime == (worldRuntime->selection).selectedEntity) {
    (worldRuntime->selection).selectedEntity = nullptr;
  }
  WorldRuntime_ForEachOwnerListNode
            (entityRuntime,WorldRuntimeNode_ClearOwnedModelReferencesCallback,worldRuntime);
  /* drop it as each player's primary selection (the loop body runs at least once, as in the original) */
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    playerSelectionBlock = g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId];
    /* Original quirk: compares the entity address with the token, which is an offset (not a pointer) */
    if (reinterpret_cast<uintptr_t>(entityRuntime) == (uintptr_t)playerSelectionBlock->placedArmyToken) {
      playerSelectionBlock->placedArmyToken = 0;
    }
    playerBlockCursor++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  GameFactionRuntime_ClearRuntimeGroupMemberPointerFromAllFactionTables(entityRuntime);
  (entityRuntime->common).ownership.modelNode = nullptr; /* marks the army slot free */
  InGameBuildCatalog_RebuildGrid(&g_InGameRuntimeRoot->rootUi.base);
  InGameSpecialBuildCatalog_RebuildGrid(&g_InGameRuntimeRoot->rootUi.base);
  InGameSelectionDetailPanel_Rebuild();
}

/* The first free army slot (model node NULL), or NULL when there is no army pool or no free slot. */
static ArmyRuntimeSlot *ArmyRuntimePool_FindFreeSlot()

{
  ArmyRuntimeSlot *armyRuntime;
  uint32_t armySlotsRemaining;

  armyRuntime = g_ArmyRuntimeSlots;
  if (armyRuntime == nullptr) {
    return nullptr;
  }
  for (armySlotsRemaining = ARMY_RUNTIME_SLOT_COUNT; armySlotsRemaining != 0; armySlotsRemaining--) {
    if (armyRuntime->modelNodeRuntime == nullptr) {
      return armyRuntime;
    }
    armyRuntime++;
  }
  return nullptr;
}

/* Stores the error in *outError (when outError is not NULL) and returns the failure result NULL. */
static ArmyRuntimeSlot *ArmyRuntime_FailCreateInstance(uint32_t error,uint32_t *outError)

{
  if (outError != nullptr) {
    *outError = error;
  }
  return nullptr;
}

/* Creates an army (unit or building) of an army asset for a faction at a world point: takes the first free
   army slot, creates the faction's model (and its linked child models) with the faction's army graphics, links
   it into the world, places it on the terrain and initialises occupancy, tint and selection metrics. Returns
   the army slot (never NULL), or NULL on failure with the error in *outError (when outError is not NULL):
   FATAL_ERROR_GENERAL_FAILURE (no army pool or no free slot), FATAL_ERROR_ARMY_ID_NOT_FOUND (the id is left in
   g_PackageLastErrorPath) or the model creation error.
   Original quirk: when creating the linked child models fails, the error is the value of worldYQ12 (a stale
   value the original never replaced by an error code); kept.
*/
ArmyRuntimeSlot *ArmyRuntime_CreateInstanceFromAsset
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyAssetRecordPrefix *armyAssetRecord;
  ModelDefinitionRecordPrefix *selectedDefinition;
  GraphicsTextureSet *textureSet;
  GraphicsPaletteAsset *paletteAsset;
  uint32_t anchorScoreWeight;
  PckArmyAssetIdCatalog secondaryWorkspaceScoreWeight;
  GameEntityRuntime *linkedEntity;
  uint32_t rootNodeReference;
  PckModelDefinitionIdCatalog modelDefinitionId;
  uint32_t modelCreateError;
  ModelRuntimeSlot *createdModelRuntime;
  ModelRuntimeNode *modelNodeRuntime;
  Bool8 childCreateFailed;
  ModelDefinition *definition;

  armyRuntime = ArmyRuntimePool_FindFreeSlot();
  if (armyRuntime == nullptr) {
    return ArmyRuntime_FailCreateInstance(FATAL_ERROR_GENERAL_FAILURE,outError);
  }
  armyAssetRecord = ArmyAssetRegistry_FindRecordById(armyAssetId);
  if (armyAssetRecord == nullptr) {
    /* the asset id as decimal text (base 10, at least one digit) for the error message */
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,armyAssetId,g_PackageLastErrorPath);
    return ArmyRuntime_FailCreateInstance(FATAL_ERROR_ARMY_ID_NOT_FOUND,outError);
  }
  armyRuntime->armyAssetId = armyAssetId;
  if (((creationFlags & ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION) != 0) &&
      (factionIndex == worldRuntime->activeFactionRuntimeIndex)) {
    selectedDefinition = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                      (factionIndex,armyAssetRecord->rootNodeOffsetOrPointer); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
    ModelView_Cast<ModelDefinition>(selectedDefinition)->builtCount++;
  }
  /* the graphics bindings exist for faction slots 0-7 only */
  if (7 < (uint32_t)factionIndex) {
    factionIndex = 7;
  }
  armyRuntime->factionIndex = factionIndex;
  if ((creationFlags & ARMY_CREATE_UNLOCK_TECHNOLOGY) != 0) {
    ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology
              (factionIndex,reinterpret_cast<ModelDefinitionHierarchyNodeAddress32>(armyAssetRecord));
  }
  textureSet = g_ArmyGraphicsBindings[factionIndex].textureSet;
  paletteAsset = g_ArmyGraphicsBindings[factionIndex].paletteAsset;
  anchorScoreWeight = armyAssetRecord[7].selectionDetailTemplateVariantIndex;
  secondaryWorkspaceScoreWeight = armyAssetRecord[7].registryId;
  armyRuntime->aiSiteScoreWeight = armyAssetRecord[7].byteSize;
  armyRuntime->aiFactionAnchorScoreWeight = anchorScoreWeight;
  armyRuntime->aiSecondaryWorkspaceScoreWeight = secondaryWorkspaceScoreWeight;
  linkedEntity = Thandor_U32ToPointer<GameEntityRuntime>(armyAssetRecord[1].byteSize); /* 32-bit format field: ArmyAssetRecord[1].byteSize (linked entity) */
  armyRuntime->occupancyMarkRadius = 0;
  armyRuntime->visibilityRadius = 0;
  armyRuntime->visibilityHeightOffset = 0;
  armyRuntime->aiUnitFlags = AI_UNIT_FLAGS_NONE;
  rootNodeReference = armyAssetRecord->rootNodeOffsetOrPointer;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  armyRuntime->linkedEntityRuntime = linkedEntity;
  (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = nullptr;
  armyRuntime->aiUnitState = 0;
  modelDefinitionId = ModelDefinition_SelectFactionUnlockedLinkedId(factionIndex,rootNodeReference);
  modelCreateError = ModelRuntimePool_CreateInstanceByDefinitionId
                     (paletteAsset,textureSet,armyRuntime,modelDefinitionId,worldRuntime,
                      &createdModelRuntime);
  if (modelCreateError != 0) {
    return ArmyRuntime_FailCreateInstance(modelCreateError,outError);
  }
  modelNodeRuntime = createdModelRuntime->rootModelNodeOrSavedOffset.modelNode;
  (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime = createdModelRuntime;
  armyRuntime->modelNodeRuntime = modelNodeRuntime;
  /* worldYQ12 goes to translation.x and worldXQ12 to translation.y throughout, as in the original; the
     parameter names are swapped relative to the node fields */
  (modelNodeRuntime->worldTransform).translation.x = worldYQ12;
  (modelNodeRuntime->worldTransform).translation.y = worldXQ12;
  (modelNodeRuntime->worldTransform).translation.z = 0;
  armyRuntime->movementRetryCountdown = 0;
  armyRuntime->fallbackWorldYQ12 = worldYQ12;
  armyRuntime->fallbackWorldXQ12 = worldXQ12;
  armyRuntime->movementTarget0Q12 = worldYQ12;
  armyRuntime->movementTarget1Q12 = worldXQ12;
  (modelNodeRuntime->modelPayload).worldRotationAngle0 = 0;
  (modelNodeRuntime->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
  (modelNodeRuntime->modelPayload).worldRotationAngle2 = orientationAngle;
  armyRuntime->commandTargetArmyRuntime = nullptr;
  armyRuntime->assignedTargetArmyRuntime = 0;
  armyRuntime->commandCoordinate0Q12 = 0;
  armyRuntime->commandCoordinate1Q12 = 0;
  armyRuntime->commandCoordinate2Q12 = 0;
  armyRuntime->commandModeFlags = ARMY_COMMAND_MODE_NONE;
  armyRuntime->commandGeneration = 0;
  (armyRuntime->articulatedContact).fallbackPosition0Q12 = worldYQ12;
  (armyRuntime->articulatedContact).fallbackPosition1Q12 = worldXQ12;
  (armyRuntime->linkedChildOverloadedState).primaryCoordinateCommandOrHistory.coordinateOrTargetQ12 = worldYQ12;
  (armyRuntime->linkedChildOverloadedState).secondaryCoordinateCommandOrHistory.coordinateOrTargetQ12 = worldXQ12;
  armyRuntime->movementPosition0Q12 = worldYQ12;
  armyRuntime->movementPosition1Q12 = worldXQ12;
  armyRuntime->movementStateFlags = ARMY_MOVEMENT_NONE;
  armyRuntime->actionVector1Q12 = 0;
  armyRuntime->terrainOccupancyMask0 = 0;
  armyRuntime->terrainOccupancyMask1 = 0;
  armyRuntime->runtimeState40 = 0;
  childCreateFailed = ModelNodeRuntime_InstantiateLinkedChildrenRecursive
                    (factionIndex,paletteAsset,textureSet,
                     (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime,rootNodeReference,worldRuntime);
  if (childCreateFailed) {
    /* Original quirk: the error is worldYQ12 (see above). */
    return ArmyRuntime_FailCreateInstance((uint32_t)worldYQ12,outError);
  }
  WorldRuntime_LinkOwnerListNode(ModelView_Cast<WorldOwnerListNode>(modelNodeRuntime));
  ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
  /* terrain contact by the definition's contact kind; depth class by its model class; depth radius from the
     definition */
  definition = (ModelDefinition *)
               (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime->definitionOrSavedId.runtimeDefinition;
  g_ArmyPlacementContactKindDispatchTable.callbacks[definition->placementContactKindIndex]
            (definition->placementHeightOffsetQ12,(modelNodeRuntime->worldTransform).translation.y,
             (modelNodeRuntime->worldTransform).translation.x,modelNodeRuntime,worldRuntime);
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  armyRuntime->depthBinClass =
       (ModelRuntimeClassId)g_ArmyRuntimeDepthBinClassByModelClass[definition->runtimeClassId];
  ModelNodeRuntime_UpdateDepthBinMasks(definition->footprintRadius,modelNodeRuntime);
  ArmyRuntime_InitializeTerrainOccupancyFlags(worldRuntime,armyRuntime);
  ModelNodeRuntime_RefreshStateTint(modelNodeRuntime);
  ArmyRuntime_RebuildDerivedSelectionMetrics(armyRuntime);
  return armyRuntime;
}

/* Sets up the terrain occupancy of a newly placed army: classifies the field-grid neighbourhood of its model
   node (within the model definition's footprintRadius), lets TerrainOccupancyMask_ResolveRuntimeClassFlags
   derive the two occupancy masks and the node's occupancy flags (0x4, 0x8, 0x1000) from it, and forces node flag 0x1000
   when the model runtime has flag 0x200 set in classState.stateFlags.
*/
void ArmyRuntime_InitializeTerrainOccupancyFlags
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  uint32_t occupancyRuntimeFlags;
  TerrainOccupancyResolvedMasks resolvedMasks;
  ModelRuntimeNode *modelNode;
  ModelRuntimeSlot *modelRuntime;

  armyRuntime->terrainOccupancyMask0 =
       TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                 ((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionOrSavedId).runtimeDefinition->
                  footprintRadius,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.y,
                  (armyRuntime->modelNodeRuntime->worldTransform).translation.x,
                  worldRuntime->fieldGrid);
  modelNode = armyRuntime->modelNodeRuntime;
  resolvedMasks = TerrainOccupancyMask_ResolveRuntimeClassFlags
                    (modelNode->runtimeFlags,armyRuntime->terrainOccupancyMask1,
                     armyRuntime->terrainOccupancyMask0,
                     (char)worldRuntime->activeFactionRuntimeIndex);
  occupancyRuntimeFlags = resolvedMasks.runtimeFlags;
  /* replace node flags 0x4, 0x8 and 0x1000 by the resolved ones */
  modelNode->runtimeFlags = modelNode->runtimeFlags & ~(MODEL_NODE_FLAG_FORCE_TRANSPARENT | TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE | TERRAIN_OCCUPANCY_FLAG_PRESENT);
  armyRuntime->terrainOccupancyMask0 = resolvedMasks.primaryOccupancyMask;
  armyRuntime->terrainOccupancyMask1 = resolvedMasks.secondaryOccupancyMask;
  modelRuntime = (armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  modelNode->runtimeFlags = modelNode->runtimeFlags | FromBits<ModelRuntimeFlags>(occupancyRuntimeFlags);
  if (Any(modelRuntime->classState.stateFlags & ARMY_MODEL_STATE_DISMANTLED)) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | MODEL_NODE_FLAG_FORCE_TRANSPARENT;
  }
}

/* Replaces one image of a freshly loaded faction graphics ('gfx') asset with the image a frontend player sent in
   his snapshot payload, so the player's own picture shows in the game: the payload's 256 RGB palette entries
   become opaque ARGB entries (pure black stays transparent), followed by 0x1000 bytes of pixel data. Nothing
   changes when no player with a complete snapshot has frontendPlayerRuntimeId as faction assignment.
*/
void ArmyGraphics_CopyFrontendPlayerPaletteAndTexture(FrontendPlayerRuntimeId frontendPlayerRuntimeId,
          ArmyGraphicsAssetAddress32 armyGraphicsAsset)

{
  int pixelDataOffset;
  uint32_t paletteColor;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int remainingCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *payloadCursor;
  uint32_t *destinationCursor;
  GraphicsTextureSourceAsset *sourceAsset;

  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((frontendPlayerRuntimeId != (playerRecord->factionAssignment).factionAssignmentIndex ||
         !Any(playerRecord->snapshotTransferFlags & FrontendSnapshotTransferFlags::FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE))) {
    playerRecord++;
    remainingBlocks = remainingBlocks - 1;
    if (remainingBlocks == 0) {
      return;
    }
  }
  payloadCursor = playerRecord->snapshotPayload;
  /* The asset is a texture source asset; the replaced image is subresource 0x71 of its table (entry at table
     offset + 0xE20). Its paletteIndex selects one of the 0x800-byte palettes (256 8-byte entries) from asset
     +0x200, its dataOffset locates the pixel data. */
  sourceAsset = reinterpret_cast<GraphicsTextureSourceAsset *>(armyGraphicsAsset);
  pixelDataOffset =
       reinterpret_cast<GraphicsTextureSourceEntry *>
       (sourceAsset->tableDescriptor.subresourceTableOffset + armyGraphicsAsset)
       [ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE].dataOffset;
  destinationCursor = reinterpret_cast<uint32_t *>
                      (reinterpret_cast<GraphicsTextureSourceEntry *>
                       (sourceAsset->tableDescriptor.subresourceTableOffset + armyGraphicsAsset)
                       [ARMY_GRAPHICS_PLAYER_IMAGE_SUBRESOURCE].paletteIndex * ARMY_GRAPHICS_PALETTE_BYTES
                       + ARMY_GRAPHICS_PALETTE_TABLE_OFFSET + armyGraphicsAsset);
  for (remainingCount = 256; remainingCount != 0; remainingCount--) {
    /* reads four bytes of a three-byte entry; the fourth is replaced by the alpha */
    paletteColor = *reinterpret_cast<uint32_t *>(payloadCursor);
    if ((paletteColor & 0xffffff) == 0) {
      paletteColor = paletteColor & 0xffffff;
    }
    else {
      paletteColor = paletteColor | 0xff000000;
    }
    *destinationCursor = paletteColor;
    payloadCursor = payloadCursor + 3;
    destinationCursor = destinationCursor + 2;
  }
  destinationCursor = reinterpret_cast<uint32_t *>(pixelDataOffset + armyGraphicsAsset);
  for (remainingCount = ARMY_GRAPHICS_PLAYER_IMAGE_DWORDS; remainingCount != 0; remainingCount--) {
    *destinationCursor = *reinterpret_cast<uint32_t *>(payloadCursor);
    payloadCursor = payloadCursor + 4;
    destinationCursor++;
  }
}
