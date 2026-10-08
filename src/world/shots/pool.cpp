/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/shots/pool.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/shots/runtime.h>
#include <algorithm>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

GraphicsTextureSet *g_ShotTextureSet = nullptr;

GraphicsPaletteAsset *g_ShotPalette = nullptr;

ShotRuntimeSlot *g_ShotRuntimeSlots = nullptr;

uint8_t *g_ShotRuntimeRebaseBaseMinusOne = nullptr;

/* Loads the shot graphics of a level (mutableBasePath with its extension replaced by .gfx and .pal) and
   allocates the zeroed shot runtime pool; g_ShotRuntimeRebaseBaseMinusOne is set for the 1-based saved slot
   offsets. The movie playback is advanced between the steps so that a running movie keeps going. Returns true
   with *outError = 0 on success, or false with the error code of the first failing load or allocation in
   *outError (always written).
*/
bool ShotRuntime_InitGraphicsResources(uint16_t *mutableBasePath,uint32_t *outError)

{
  ShotRuntimeSlot *pool;
  uint32_t *poolDword;
  uint32_t loadError;
  GraphicsTextureSet *loadedTextureSet;
  GraphicsPaletteAsset *loadedPalette;

  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,mutableBasePath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  loadedTextureSet = g_GraphicsTextureSetLoadPackage(mutableBasePath,&loadError);
  if (loadedTextureSet == nullptr) {
    *outError = loadError;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_ShotTextureSet = loadedTextureSet;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,mutableBasePath);
  loadedPalette = g_GraphicsPaletteAssetLoadPackage(mutableBasePath,&loadError);
  if (loadedPalette == nullptr) {
    *outError = loadError;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_ShotPalette = loadedPalette;
  loadError = g_MemoryApi.alloc(SHOT_RUNTIME_POOL_BYTES,reinterpret_cast<void **>(&pool)); /* the arena stores the block address through void ** */
  if (loadError != 0) {
    *outError = loadError;
    return false;
  }
  /* pool address - 1 */
  g_ShotRuntimeRebaseBaseMinusOne = reinterpret_cast<uint8_t *>(pool) - 1;
  g_ShotRuntimeSlots = pool;
  /* clears the pool dword by dword */
  poolDword = reinterpret_cast<uint32_t *>(pool); /* zeroed dword by dword, as the original */
  std::fill_n(poolDword,SHOT_RUNTIME_POOL_BYTES / 4,0);
  *outError = 0;
  return true;
}

/* Counterpart of ShotRuntime_InitGraphicsResources at level end: frees the shot runtime pool, releases the
   shot texture set and palette, releases the nested resource each shot definition owns and empties the shot
   definition registry.
*/
void ShotRuntime_ShutdownGraphicsResources()

{
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;
  ShotDefinition *currentDefinition;
  
  g_MemoryApi.free(g_ShotRuntimeSlots);
  g_ShotRuntimeSlots = nullptr;
  if (g_ShotTextureSet != nullptr) {
    g_GraphicsTextureSetReleasePackage(g_ShotTextureSet);
    g_ShotTextureSet = nullptr;
  }
  if (g_ShotPalette != nullptr) {
    g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_ShotPalette);
    g_ShotPalette = nullptr;
  }
  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    currentDefinition = *registryCursor;
    if (currentDefinition != nullptr && currentDefinition->ownedNestedResourcePresent != 0) {
      Resource_Release(currentDefinition->ownedNestedResource);
    }
    *registryCursor = nullptr;
    registryCursor++;
  }
}

/* Looks a shot definition up by id in the 256-slot registry (a second copy of
   ShotDefinitionRegistry_FindByIdWithError, used by ShotDefinition registration to reject duplicates).
   Returns the registered definition (never NULL), or NULL on a miss; then a number is also formatted into
   g_PackageLastErrorPath (the original returned FATAL_ERROR_SHOT_ID_NOT_FOUND as its error value).
*/
ShotDefinition *ShotRuntime_FindDefinitionById(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registryDefinition;

  registryDefinition = ShotDefinitionRegistry_LookupById(definitionId);
  if (registryDefinition == nullptr) {
    /* Original quirk: the original formats the last registry slot (what its scan loaded last), not the
       missing id */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
               (int32_t)(intptr_t)g_ShotDefinitionRegistry[SHOT_DEFINITION_REGISTRY_SLOT_COUNT - 1],
               g_PackageLastErrorPath);
  }
  return registryDefinition;
}

/* True when the 1-based saved byte offset savedOffset (1 = pool start) names a whole record of recordBytes
   inside a pool of poolBytes. */
static bool ShotRuntime_SavedOffsetInPool(uint32_t savedOffset,size_t poolBytes,size_t recordBytes)

{
  return static_cast<uint32_t>(savedOffset - 1U) <= poolBytes - recordBytes;
}

/* Turns the saved form of the shot slots back into pointers after a savegame load (and after writing one):
   for every live shot (non-zero model node) the 1-based model node, runtime state and owner army offsets are
   rebased and the saved definition id is replaced by the registered ShotDefinition. A shot whose id is no
   longer registered is dropped (model node cleared).
   The original adds the saved offsets to the pool bases unchecked; bounded here because a savegame is a user
   file: a shot whose model node offset lies outside the world object pool is dropped (model node cleared), a
   target model runtime or owner army offset outside its pool is cleared to none (0), one log line each.
*/
void ShotRuntime_RebaseSlotsAfterLoad()

{
  ShotSecondaryEffectCountdownTicks *firstSlotCountdown;
  void *rebasedRuntimeState;
  int registrySlotsRemaining;
  int shotSlotsRemaining;
  ArmyRuntimeSlot *rebasedOwnerArmy;
  ShotDefinition **registryCursor;
  ShotRuntimeSlot *shotSlot;
  ArmyRuntimeSlot *savedOwnerArmy;
  ShotDefinition *registryDefinition;
  
  shotSlot = g_ShotRuntimeSlots;
  /* Original quirk: the first slot's secondary effect countdown (slot0 + 0x3C) is inverted on every call,
     purpose unknown */
  firstSlotCountdown = &g_ShotRuntimeSlots->ownerAndTrajectory.secondaryEffectCountdownTicks;
  *firstSlotCountdown = ~*firstSlotCountdown;
  for (shotSlotsRemaining = SHOT_RUNTIME_SLOT_COUNT; shotSlotsRemaining != 0; shotSlotsRemaining--) {
    rebasedRuntimeState = shotSlot->runtimeStateOrSavedOffset.runtimeStatePointer;
    savedOwnerArmy = shotSlot->ownerAndTrajectory.ownerArmyRuntime;
    if (shotSlot->modelNodeOrSavedOffset.modelNode != nullptr &&
        !ShotRuntime_SavedOffsetInPool(shotSlot->modelNodeOrSavedOffset.raw,
                                       INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),
                                       sizeof(WorldObjectRecord))) {
      Thandor_Log("shot: saved slot %d has model node offset 0x%08X outside the world object pool, dropped",
                  SHOT_RUNTIME_SLOT_COUNT - shotSlotsRemaining,shotSlot->modelNodeOrSavedOffset.raw);
      shotSlot->modelNodeOrSavedOffset.modelNode = nullptr;
    }
    if (shotSlot->modelNodeOrSavedOffset.modelNode != nullptr) {
      if (rebasedRuntimeState != nullptr) {
        if (!ShotRuntime_SavedOffsetInPool(shotSlot->runtimeStateOrSavedOffset.raw,MODEL_RUNTIME_POOL_BYTES,
                                           sizeof(ModelRuntimeSlot))) {
          Thandor_Log("shot: saved slot %d has target offset 0x%08X outside the model runtime pool, cleared",
                      SHOT_RUNTIME_SLOT_COUNT - shotSlotsRemaining,shotSlot->runtimeStateOrSavedOffset.raw);
          rebasedRuntimeState = nullptr;
        }
        else {
          /* 5f-format: ShotRuntimeSlot.runtimeStateOrSavedOffset */
          rebasedRuntimeState = reinterpret_cast<void *>(Thandor_PointerToI32(rebasedRuntimeState) + g_ModelRuntimeRebaseDelta);
        }
      }
      rebasedOwnerArmy = nullptr;
      if (savedOwnerArmy != nullptr) {
        if (!ShotRuntime_SavedOffsetInPool(static_cast<uint32_t>(shotSlot->ownerAndTrajectory.ownerArmyRuntime),
                                           ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),sizeof(ArmyRuntimeSlot))) {
          Thandor_Log("shot: saved slot %d has owner army offset 0x%08X outside the army pool, cleared",
                      SHOT_RUNTIME_SLOT_COUNT - shotSlotsRemaining,
                      static_cast<uint32_t>(shotSlot->ownerAndTrajectory.ownerArmyRuntime));
        }
        else {
          /* 5f-format: ShotRuntimeSlot.ownerAndTrajectory.ownerArmyRuntime (saved offset) */
          rebasedOwnerArmy = Thandor_U32ToPointer<ArmyRuntimeSlot>(Thandor_PointerToI32(savedOwnerArmy) + Thandor_PointerToI32(g_ArmyRuntimeRebaseBaseMinusOne));
        }
      }
      /* saved model node offset + g_RuntimeObjectRebaseBaseMinusOne */
      /* 5f-format: ShotRuntimeSlot.modelNodeOrSavedOffset */
      shotSlot->modelNodeOrSavedOffset.modelNode =
           reinterpret_cast<ModelRuntimeNode *>(g_RuntimeObjectRebaseBaseMinusOne + (int)shotSlot->modelNodeOrSavedOffset.modelNode);
      shotSlot->runtimeStateOrSavedOffset.runtimeStatePointer = rebasedRuntimeState;
      shotSlot->ownerAndTrajectory.ownerArmyRuntime = rebasedOwnerArmy;
      /* the slot still holds the saved definition id in its definition field */
      registryCursor = g_ShotDefinitionRegistry;
      registryDefinition = nullptr;
      for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
           registrySlotsRemaining--) {
        registryDefinition = *registryCursor;
        /* 5f-format: ShotRuntimeSlot.definitionOrSavedId */
        if (registryDefinition != nullptr &&
            shotSlot->definitionOrSavedId.definition == Thandor_U32ToPointer<ShotDefinition>(registryDefinition->definitionId)) {
          break;
        }
        registryCursor++;
      }
      if (registrySlotsRemaining == 0) {
        /* saved definition no longer registered: drop the shot.
           Original quirk: the definition becomes the last registry entry */
        shotSlot->modelNodeOrSavedOffset.modelNode = nullptr;
      }
      shotSlot->definitionOrSavedId.definition = registryDefinition;
    }
    shotSlot++;
  }
}

/* Fires one projectile of shotDefinition from the launch point towards the target point: takes the first
   free slot of the shot pool and a world object record for its model node, links the node into the world's
   owner list and seeds position, launch angles, velocity (plus half the ballistic divisor upwards for
   ballistic shots), lifetime, animation, optional shading light, occupancy class flags and tint; a launch
   effect is spawned when the model has an effect point (MODEL_POINT_CLASS_EFFECT). Called by the army and model weapon
   code (ArmyRuntime_ResolveShotLaunchFromModelAttachment, ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate,
   ArmyRuntime_UpdateTimedShotAndEffectEmitters, ModelRuntime_EmitProjectilesFromAttachmentPoints) and by
   EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions. The original signals failure to the
   caller when no slot or record is free; this version just returns.
*/
void ShotRuntimePool_CreateProjectileFromDefinition
          (ShotTargetModelReference targetModelReference,ArmyRuntimeSlot *ownerArmyRuntime,
          Q12 targetWorldZQ12,Q12 targetWorldYQ12,Q12 targetWorldXQ12,Q12 launchWorldZQ12,
          Q12 launchWorldYQ12,Q12 launchWorldXQ12,ShotDefinition *shotDefinition,
          WorldRuntimeContext *worldRuntime)

{
  ModelResource *nestedModelResource;
  Q12 modelBoundingRadiusQ12;
  ShotAnimationFrameAccumulatorQ4 frameThresholdQ4;
  AngleTurn16Stored32 elevationOffsetAngle;
  ShotSecondaryEffectCountdownTicks secondaryEffectInterval;
  PackedArgb32 definitionTintArgb;
  GraphicsPaletteAsset *shotPalette;
  ShotModelRuntimeNode *shotModelNode;
  PackedArgb32 nodeTintArgb;
  int slotsRemaining;
  Q12 runtimeLaunchSpeedQ12;
  Q12 launchDirectionZQ12;
  uint32_t neighborhoodMask;
  ShotRuntimeSlot *shotRuntimeCursor;
  uint64_t tintProduct;
  ShotLaunchAngles launchAngles;
  ModelPackedPointRecord *packedPoint;
  FixedDirection launchDirection;
  ModelWorldPoint localPoint;
  TerrainOccupancyResolvedMasks resolvedMasks;
  char runtimeClassIndex;
  
  if (g_ShotRuntimeSlots == nullptr) {
    return; /* no shot pool */
  }
  shotRuntimeCursor = g_ShotRuntimeSlots;
  slotsRemaining = SHOT_RUNTIME_SLOT_COUNT;
  while (slotsRemaining != 0 && shotRuntimeCursor->modelNodeOrSavedOffset.modelNode != nullptr) {
    shotRuntimeCursor++;
    slotsRemaining--;
  }
  if (slotsRemaining == 0) {
    return; /* no free slot */
  }
  shotModelNode = WorldNode_View<ShotModelRuntimeNode>(WorldObjectArray_AllocateFreeRecord(worldRuntime));
  if (shotModelNode == nullptr) {
    return;
  }
  WorldRuntime_LinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(shotModelNode));
  shotRuntimeCursor->modelNodeOrSavedOffset.modelNode = WorldNode_View<ModelRuntimeNode>(shotModelNode);
  shotRuntimeCursor->definitionOrSavedId.definition = shotDefinition;
  shotModelNode->ownerClassId = WORLD_OWNER_RUNTIME_SHOT;
  shotModelNode->shotRuntime = shotRuntimeCursor;
  shotModelNode->worldTransform.translation.x = launchWorldXQ12;
  shotModelNode->worldTransform.translation.y = launchWorldYQ12;
  shotModelNode->worldTransform.translation.z = launchWorldZQ12;
  runtimeLaunchSpeedQ12 = shotDefinition->launchSpeedQ12;
  if (shotDefinition->trajectoryRampDurationTicks != 0) {
    runtimeLaunchSpeedQ12 = 0;
  }
  shotRuntimeCursor->lifetimeTicksRemaining = shotDefinition->projectileLifetimeTicks;
  shotRuntimeCursor->launchSpeedQ12 = runtimeLaunchSpeedQ12;
  shotRuntimeCursor->ownerAndTrajectory.ownerArmyRuntime = ownerArmyRuntime;
  launchAngles = ShotDefinition_ComputeLaunchAngles
                     (targetWorldZQ12,targetWorldYQ12,targetWorldXQ12,launchWorldZQ12,
                      launchWorldYQ12,launchWorldXQ12,shotDefinition);
  shotModelNode->modelPayload.worldRotationAngle0 = launchAngles.headingAngle;
  shotModelNode->modelPayload.worldRotationAngle1 = launchAngles.elevationAngle;
  shotModelNode->modelPayload.worldRotationAngle2 = launchAngles.headingAngle;
  launchDirection = FixedMath_DirectionFromAnglesScaled
                     (launchAngles.elevationAngle,launchAngles.headingAngle,shotDefinition->launchSpeedQ12);
  launchDirectionZQ12 = launchDirection.z;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent0Q12 = launchDirection.x;
  if (shotDefinition->trajectoryMode == SHOT_TRAJECTORY_BALLISTIC) {
    launchDirectionZQ12 = launchDirectionZQ12 + (shotDefinition->ballisticDivisorQ12 >> 1);
  }
  shotModelNode->renderDepthBiasOrState = 0;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent1Q12 = launchDirection.y;
  shotRuntimeCursor->ownerAndTrajectory.directionComponent2Q12 = launchDirectionZQ12;
  shotRuntimeCursor->projectileAgeTicks = 0;
  shotPalette = g_ShotPalette;
  nestedModelResource = static_cast<ModelResource *>(shotDefinition->ownedNestedResource.get());
  shotModelNode->modelPayload.textureSet = g_ShotTextureSet;
  modelBoundingRadiusQ12 = nestedModelResource->boundingRadiusQ12;
  shotModelNode->modelPayload.paletteAsset = shotPalette;
  shotModelNode->subtreeBoundingRadiusQ12 = modelBoundingRadiusQ12;
  shotModelNode->modelPayload.modelResource = nestedModelResource;
  frameThresholdQ4 = shotDefinition->animationFrameAdvanceThresholdQ4;
  elevationOffsetAngle = shotDefinition->elevationOffsetAngle16;
  shotRuntimeCursor->ownerAndTrajectory.animationFrameIndex = 0;
  shotRuntimeCursor->impactEffectEmissionFlags = {};
  shotRuntimeCursor->animationFrameAccumulatorQ4 = frameThresholdQ4;
  shotRuntimeCursor->runtimeStateOrSavedOffset.runtimeState = targetModelReference;
  shotRuntimeCursor->elevationOffsetAngle16 = elevationOffsetAngle;
  shotModelNode->modelPayload.meshGroupMask = UINT32_MAX;
  secondaryEffectInterval = shotDefinition->secondaryEffectIntervalTicks;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | MODEL_NODE_FLAG_TRANSFORM_DIRTY;
  shotRuntimeCursor->ownerAndTrajectory.secondaryEffectCountdownTicks = secondaryEffectInterval;
  shotModelNode->textureSubresourceBaseIndex = 0;
  shotModelNode->modelRuntimeLinkOrSavedOffset = nullptr;
  /* optional light point of the model: allocates a shading record there */
  if (!ModelLookupTable_FindPackedPoint
         (0,MODEL_POINT_CLASS_LIGHT,static_cast<ModelResource *>(shotDefinition->ownedNestedResource.get()),&packedPoint)) {
    shotModelNode->shadingRecord = nullptr;
  }
  else {
    localPoint = ModelNodeRuntime_TransformLocalPoint
                       (packedPoint,WorldNode_View<ModelRuntimeNode>(shotModelNode));
    shotModelNode->shadingRecord = GraphicsShadingRuntime_AllocateRecord
                       (shotDefinition->shadingTransitionDurationTicks,
                        (shotDefinition->shadingColorArgb >> 24) << 8, /* alpha byte = radius / 16 */
                        shotDefinition->shadingColorArgb,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12);
  }
  shotModelNode->parentNode = nullptr;
  shotModelNode->childCount = 0;
  runtimeClassIndex = (char)worldRuntime->activeFactionRuntimeIndex;
  neighborhoodMask = TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
                     (Q12_ONE,shotModelNode->worldTransform.translation.y,
                      shotModelNode->worldTransform.translation.x,worldRuntime->fieldGrid);
  resolvedMasks =
       TerrainOccupancyMask_ResolveRuntimeClassFlags(TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED,0,neighborhoodMask,runtimeClassIndex);
  shotRuntimeCursor->terrainRuntimeClassState = resolvedMasks.primaryOccupancyMask;
  shotModelNode->runtimeFlags = shotModelNode->runtimeFlags | FromBits<ModelRuntimeFlags>(resolvedMasks.runtimeFlags) | TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED;
  nodeTintArgb = ModelRuntimeNode_GetStateTintArgb(WorldNode_View<ModelRuntimeNode>(shotModelNode));
  definitionTintArgb = shotDefinition->stateTintArgb;
  /* PUNPCKLBW/PSRLW 4 both tints, PMULHW, PACKUSWB */
  tintProduct = pmulhw(ColorLanes_UnpackBytesShiftRight(nodeTintArgb,4),
                       ColorLanes_UnpackBytesShiftRight(definitionTintArgb,4));
  shotModelNode->tintArgb = ColorLanes_PackWordsUnsignedSaturate(tintProduct);
  ModelNodeRuntime_RebuildTransformsFromRoot(WorldNode_View<ModelRuntimeNode>(shotModelNode));
  ModelNodeRuntime_UpdateDepthBinMasks(0,WorldNode_View<ModelRuntimeNode>(shotModelNode));
  if (ModelLookupTable_FindPackedPoint
        (0,MODEL_POINT_CLASS_EFFECT,static_cast<ModelResource *>(shotDefinition->ownedNestedResource.get()),&packedPoint)) {
    localPoint = ModelNodeRuntime_TransformLocalPoint
                       (packedPoint,WorldNode_View<ModelRuntimeNode>(shotModelNode));
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },
               shotModelNode->modelPayload.worldRotationAngle2,
               shotModelNode->modelPayload.worldRotationAngle1,
               shotModelNode->modelPayload.worldRotationAngle0,localPoint.zQ12,localPoint.yQ12,localPoint.xQ12,
               shotDefinition->launchEffectDefinition,worldRuntime);
  }
}
