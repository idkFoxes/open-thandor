/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/model/definitions.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/model/definitions.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

ModelDefinitionRecordPrefix *g_ModelDefinitionRegistry[768] = {};

/* Checks that the asset is an 'mdl' of converter version 0x8000A, then registers each of its variable-size
   model-definition records (starting at +0x200, each prefixed with its byte size) and resolves their
   references against the asset base. Returns true on success; returns false with the error code in *outError
   (untouched on success) for an invalid header or at the first record that fails. (The original's success
   return value, the last registration's value, was read by no caller.)
*/
Bool8 ModelAsset_PrepareRecords(ModelAssetHeader *asset,uint32_t *outError)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ModelDefinitionResolveView *definition;

  registrationStatusCode = FATAL_ERROR_MODEL_ASSET_INVALID;
  if (asset->recordCountHeader.common.magic == ASSET_MAGIC_MDL &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_MDL_0008000A) {
    definition = Asset_RecordAfter<ModelDefinitionResolveView>(asset);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      if (!ModelDefinition_RegisterAndResolveReferences(definition,asset,&registrationStatusCode)) break;
      /* advance by the record's leading byte size */
      definition = Asset_RecordAt<ModelDefinitionResolveView>(definition,definition->byteSize);
    }
    if (recordsRemaining == 0) {
      return true;
    }
  }
  *outError = registrationStatusCode;
  return false;
}

/* Looks up the model's packed point table (packedLookupTableEntryCount entries of ModelPackedPointRecord at
   packedLookupTableRelativeOffset) for the key (keyIndex << 4) | keyClass. Returns true when an entry matches
   and stores its localPosition in *outLocalPosition; returns false and stores (0, 0, 0) when no entry
   matches. outLocalPosition may be NULL when only presence matters. Called directly by
   ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters (a model class-init callback table slot) and by the
   army hangar-lowering step in gameplay/army/aircraft.cpp.
*/
Bool8 ModelLookupTable_GetPackedPointPosition
          (ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,GraphicsFixedVec3 *outLocalPosition)

{
  int entriesRemaining;
  ModelPackedPointRecord *entryCursor;

  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  entryCursor =
       Asset_RecordAt<ModelPackedPointRecord>(modelDefinition,modelDefinition->packedLookupTableRelativeOffset);
  for (; entriesRemaining != 0; entriesRemaining--) {
    if ((keyClass | keyIndex << 4) == entryCursor->packedLookupKey) {
      if (outLocalPosition != nullptr) {
        *outLocalPosition = entryCursor->localPosition;
      }
      return true;
    }
    entryCursor++; /* next 0x10-byte entry */
  }
  if (outLocalPosition != nullptr) {
    outLocalPosition->x = 0;
    outLocalPosition->y = 0;
    outLocalPosition->z = 0;
  }
  return false;
}

/* Looks up the model's packed point table (packedLookupTableEntryCount, packedLookupTableRelativeOffset) for the key
   (keyIndex << 4) | keyClass. Returns true and stores the matching entry in *outEntry when one matches;
   otherwise returns false and stores the address just past the table's last entry in *outEntry (one caller,
   ArmyPlacement_CanPlaceAnchoredModel, reads it anyway).
*/
Bool8 ModelLookupTable_FindPackedPoint(ModelLookupKeyIndex keyIndex,ModelLookupKeyClass keyClass,
          ModelResource *modelDefinition,ModelPackedPointRecord **outEntry)

{
  ModelPackedPointRecord *entryCursor;
  int entriesRemaining;

  entriesRemaining = modelDefinition->packedLookupTableEntryCount;
  entryCursor =
       Asset_RecordAt<ModelPackedPointRecord>(modelDefinition,modelDefinition->packedLookupTableRelativeOffset);
  for (; entriesRemaining != 0; entriesRemaining--) {
    if ((keyClass | keyIndex << 4) == entryCursor->packedLookupKey) {
      *outEntry = entryCursor;
      return true;
    }
    entryCursor++;
  }
  *outEntry = entryCursor;
  return false;
}

/* Looks a model definition up by id in the 768-slot registry and returns 0 with its build costs, the three
   fields buildEnergyLoadQ4 (*outEnergyLoadQ4), buildTicks (*outBuildTicks) and xeniteValueQ4 (*outXeniteCostQ4), which
   ArmyAssetRecord_RelocateModelTree adds to an army record. On a miss the id is formatted into
   g_PackageLastErrorPath, the out-parameters are left unchanged and FATAL_ERROR_MODEL_DEFINITION_MISSING is
   returned.
*/
uint32_t ModelDefinitionRegistry_FindBuildCostsById
          (PckModelDefinitionIdCatalog definitionId,uint32_t *outEnergyLoadQ4,uint32_t *outBuildTicks,
           uint32_t *outXeniteCostQ4)

{
  ModelDefinitionRecordPrefix *registeredDefinition;
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  for (; registrySlotsRemaining != 0; registrySlotsRemaining--) {
    registeredDefinition = *registryCursor;
    if (registeredDefinition != nullptr && registeredDefinition->definitionId == definitionId) {
      *outBuildTicks = ModelDefinition_FromPrefix(registeredDefinition)->buildTicks;
      *outEnergyLoadQ4 = ModelDefinition_FromPrefix(registeredDefinition)->buildEnergyLoadQ4;
      *outXeniteCostQ4 = ModelDefinition_FromPrefix(registeredDefinition)->xeniteValueQ4;
      return 0;
    }
    registryCursor++;
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_MODEL_DEFINITION_MISSING;
}

/* Returns the first registered model definition whose runtime class id (requiredTechnologyBit) equals
   runtimeClassId, or NULL. Called directly by the AI purchasing and technology code (gameplay/ai/purchasing.cpp, technology.cpp).
*/
ModelDefinitionRecordPrefix *
ModelDefinitionRegistry_FindByRuntimeClassId(ModelRuntimeClassId runtimeClassId)

{
  int registrySlotsRemaining;
  ModelDefinitionRecordPrefix **registryCursor;
  ModelDefinitionRecordPrefix *candidateDefinition;

  registryCursor = g_ModelDefinitionRegistry;
  registrySlotsRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
  while ((candidateDefinition = *registryCursor,
         candidateDefinition == nullptr ||
         (runtimeClassId != ModelDefinition_FromPrefix(candidateDefinition)->requiredTechnologyBit))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return nullptr;
    }
  }
  return candidateDefinition;
}

/* Serialized model node tree: nodeFlags (low nibble 0 = has a sprite), sprite path right after the header,
   spriteAssetReference, ownedNestedResourcePresent (owned-copy count), childCount, childSerializedOffsets
   (relative to the asset, relocated in place). Loads or reuses each node's sprite; returns true with *error
   on failure. The original trusts childCount and the nesting depth; bounded here because the walk follows file
   data: a node with more children than childSerializedOffsets holds or a tree deeper than
   MDL_NODE_TREE_MAX_DEPTH (a cyclic offset) fails with FATAL_ERROR_MODEL_ASSET_INVALID before its sprite is
   loaded. The stock models use at most 5 children and depth 5. */
#define MDL_NODE_TREE_MAX_DEPTH 64
static Bool8 ModelDefinition_ResolveNodeSprites(MdlSerializedNodeHeader *node,ModelAssetHeader *asset,uint32_t *error,
                                                uint32_t depth)
{
  uint32_t childIndex;
  if (depth >= MDL_NODE_TREE_MAX_DEPTH ||
      node->childCount > sizeof(node->childSerializedOffsets) / sizeof(node->childSerializedOffsets[0])) {
    Thandor_Log("ModelDefinition_RegisterAndResolveReferences: node with %u children at depth %u, rejected",
                (uint32_t)node->childCount,depth);
    *error = FATAL_ERROR_MODEL_ASSET_INVALID;
    return true;
  }
  if ((node->nodeFlags & 0xf) == 0) {
    uint16_t *spritePath = Asset_RecordAfter<uint16_t>(node); /* the node's sprite path text follows it */
    SpriteAssetHeader *loadedSprite;
    SpriteAssetHeader *registered;
    /* ".spr". The original checks this call for failure, but in the original WidePath_SetExtensionCode never
       fails, so that branch is dead (open-thandor: it fails only for a path longer than
       WIDE_PATH_MAX_CODE_UNITS units and then leaves it unchanged). On an error the original abandons the whole tree walk at once; returning up the
       recursion is equivalent. */
    WidePath_SetExtensionCode(ASSET_MAGIC_SPR,spritePath);
    loadedSprite = static_cast<SpriteAssetHeader *>(Package_LoadEntry(spritePath,error));
    if (loadedSprite == nullptr) {
      return true;
    }
    registered = SpriteAssetRegistry_FindById(loadedSprite->registryHeader.registryId);
    if (registered == nullptr) {
      /* first use of this sprite: the node owns the loaded copy and registers it */
      uint32_t relocateError;
      node->ownedNestedResourcePresent++;
      node->spriteAssetReference.spriteAsset = loadedSprite;
      relocateError = SpriteAsset_RegisterAndRelocatePointers(loadedSprite);
      if (relocateError != 0) {
        *error = relocateError;
        return true;
      }
    }
    else {
      /* already registered by another model: share it and drop the fresh copy */
      node->spriteAssetReference.spriteAsset = registered;
      Resource_Release(loadedSprite);
    }
  }
  for (childIndex = 0; childIndex < (uint32_t)node->childCount; childIndex++) {
    /* relocate the child offset to a pointer in place */
    node->childSerializedOffsets[childIndex] = node->childSerializedOffsets[childIndex] + (int)(uintptr_t)asset;
    if (ModelDefinition_ResolveNodeSprites
                  (Thandor_U32ToPointer<MdlSerializedNodeHeader>(node->childSerializedOffsets[childIndex]),asset,error,
                   depth + 1)) {
      return true;
    }
  }
  return false;
}

/* Puts the definition into the first free slot of the model registry. Returns 0, or the fatal error code
   for a duplicate id (the id is left in g_PackageLastErrorPath) or a full registry (the slot count is
   left there). */
static uint32_t ModelDefinition_ClaimRegistrySlot(ModelDefinitionResolveView *definition)
{
  int slotIndex;

  if (ModelDefinitionRegistry_FindById(definition->definitionId) != nullptr) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_MODEL_ID_DUPLICATE;
  }
  for (slotIndex = 0; slotIndex < MODEL_DEFINITION_REGISTRY_SLOT_COUNT; slotIndex++) {
    if (g_ModelDefinitionRegistry[slotIndex] == nullptr) {
      g_ModelDefinitionRegistry[slotIndex] = ModelDefinitionResolveView_Prefix(definition);
      return 0;
    }
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,MODEL_DEFINITION_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
  return FATAL_ERROR_MODEL_REGISTRY_FULL;
}

/* Replaces the serialized shot and effect ids of the definition by the registered definitions, in record
   order. Returns 0, or the error code of the first failed lookup (later fields keep their ids). */
static uint32_t ModelDefinition_ResolveShotAndEffectIds(ModelDefinitionResolveView *definition)
{
  /* each effect field holds its serialized id until the lookup replaces it by the definition */
  Ptr32<EffectDefinition> *const destructionEffects[] = {
    &definition->destructionEffect0,&definition->destructionEffect1,
    &definition->destructionEffect2,&definition->destructionEffect3,
    &definition->destructionEffect4,&definition->destructionEffect5,
    &definition->destructionEffect6,&definition->destructionEffect7
  };
  Ptr32<EffectDefinition> *const emitterAndRemovalEffects[] = {
    &definition->emitterEffectDefinitionReference,&definition->waterEmitterEffectDefinitionReference,
    &definition->removalEffectDefinitionReference,&definition->damageEffectDefinitionReference
  };
  uint32_t status;
  uint32_t fieldIndex;
  ShotDefinition *resolvedShot;
  ShotDefinition *resolvedEmitterShot;
  EffectDefinition *resolvedEffect;

  status = ShotDefinitionRegistry_FindByIdWithError
                     ((PckShotDefinitionIdCatalog)definition->shotDefinitionReference,&resolvedShot); /* 5f-format: ModelDefinition.shotDefinitionReference (id on disk, pointer after resolve) */
  if (status != 0) return status;
  definition->shotDefinitionReference = resolvedShot;
  for (fieldIndex = 0; fieldIndex < 8; fieldIndex++) {
    status = EffectDefinitionRegistry_FindById
                       ((PckEffectDefinitionIdCatalog)*destructionEffects[fieldIndex],&resolvedEffect); /* 5f-format: ModelDefinition destruction effect references (id on disk, pointer after resolve) */
    if (status != 0) return status;
    *destructionEffects[fieldIndex] = resolvedEffect;
  }
  /* -1: the definition has no emitter shot */
  if (definition->emitterShotDefinitionReference != Thandor_U32ToPointer<ShotDefinition>(-1)) {
    status = ShotDefinitionRegistry_FindByIdWithError
                       ((PckShotDefinitionIdCatalog)definition->emitterShotDefinitionReference,&resolvedEmitterShot); /* 5f-format: ModelDefinition.emitterShotDefinitionReference (id on disk, pointer after resolve) */
    if (status != 0) return status;
    definition->emitterShotDefinitionReference = resolvedEmitterShot;
  }
  for (fieldIndex = 0; fieldIndex < 4; fieldIndex++) {
    status = EffectDefinitionRegistry_FindById
                       ((PckEffectDefinitionIdCatalog)*emitterAndRemovalEffects[fieldIndex], /* 5f-format: ModelDefinition emitter/removal effect references (id on disk, pointer after resolve) */
                        &resolvedEffect);
    if (status != 0) return status;
    *emitterAndRemovalEffects[fieldIndex] = resolvedEffect;
  }
  return 0;
}

/* Copies the terrain-class dependent placement values from the grid tables. Negative classes keep the
   serialized values; placementContactKindIndex selects which grid tables terrainTraversalClass indexes
   (kind 4 from class 1, the fallback tables from class 4). */
static void ModelDefinition_CopyTerrainClassValues(ModelDefinitionResolveView *definition)
{
  uint32_t terrainClass;
  uint32_t footprintRadius;

  terrainClass = definition->terrainTraversalClass;
  if (-1 < (int)definition->footprintRadiusClass) {
    /* Original quirk: no upper bound on the class; the original would read past the 8 radius offsets into
       g_GridInfluenceSquaredThreshold for a class >= 8. The shipped model definitions only use -1..7. */
    footprintRadius =g_GridInfluenceRadiusOffset[definition->footprintRadiusClass];
    definition->footprintRadius = footprintRadius;
    definition->footprintRadiusCopy = footprintRadius;
  }
  if ((int)terrainClass < 0) return;
  /* All lookups index one threshold table from different entries; the class is not bounded, so a class
     outside the run of its entry reads the neighbouring entries (as in the original). */
  if (definition->placementContactKindIndex == 1) {
    uint32_t maxWaterSurfaceDelta =
         g_GridTerrainClassThresholds[(int32_t)(GRID_TERRAIN_THRESHOLD_BIT24_MAX_WATER_SURFACE_DELTA + terrainClass)];
    uint32_t maxNormalAngle =
         g_GridTerrainClassThresholds[(int32_t)(GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE + terrainClass)];
    ModelDefinition_FromResolveView(definition)->classParameterCC = maxWaterSurfaceDelta;
    definition->runtimeValue24 = maxNormalAngle;
  }
  else if (definition->placementContactKindIndex == 4) {
    uint32_t secondaryThreshold =
         g_GridTerrainClassThresholds[(int32_t)(GRID_TERRAIN_THRESHOLD_CLASS4_SECONDARY + (terrainClass - 1))];
    definition->runtimeValue24 =
         g_GridTerrainClassThresholds[(int32_t)(GRID_TERRAIN_THRESHOLD_BIT25_MAX_SELECTED_NORMAL_ANGLE + (terrainClass - 1))];
    definition->traversalSecondaryThreshold = secondaryThreshold;
  }
  else {
    int fallbackIndex = terrainClass - 4;
    uint32_t minWaterSurfaceDelta =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MIN_WATER_SURFACE_DELTA + fallbackIndex];
    uint32_t maxNormalAngle =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE + fallbackIndex];
    uint32_t secondaryThreshold =
         g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_FALLBACK_SECONDARY + fallbackIndex];
    definition->waterDamageThreshold = minWaterSurfaceDelta;
    definition->runtimeValue24 = maxNormalAngle;
    definition->traversalSecondaryThreshold = secondaryThreshold;
  }
}

/* Registers one MDL model definition in the first free slot of the 768-slot registry and turns its
   serialized references into runtime pointers: the node tree is relocated by the asset base and its
   sprites are loaded or reused, the shot and effect ids are resolved through their registries, and the
   terrain-class dependent placement values are copied from the grid tables. Returns true on success; a
   duplicate id, a full registry or any failed load/lookup returns false with its error code in *outError
   (untouched on success; the original's success return value was read by no caller).
*/
Bool8 ModelDefinition_RegisterAndResolveReferences
          (ModelDefinitionResolveView *definition,ModelAssetHeader *asset,uint32_t *outError)

{
  uint32_t status;
  uint32_t rootNodeOffset;

  /* The original accepts any target class; rejected here because the shot code indexes the 8 per-class impact
     effects and damages of a shot definition with it (world/shots/flight.cpp; the stock models use 0..7). */
  if (definition->targetClassIndex >= SHOT_TARGET_CLASS_IMPACT_COUNT) {
    Thandor_Log("ModelDefinition_RegisterAndResolveReferences: model %u has target class %u, rejected",
                (uint32_t)definition->definitionId,definition->targetClassIndex);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    *outError = FATAL_ERROR_MODEL_ASSET_INVALID;
    return false;
  }
  status = ModelDefinition_ClaimRegistrySlot(definition);
  if (status != 0) {
    *outError = status;
    return false;
  }
  rootNodeOffset = definition->rootNodeOffsetOrPointer;
  if (rootNodeOffset != 0) {
    /* asset start + serialized offset */
    definition->rootNodeOffsetOrPointer = Thandor_PointerToU32(Asset_RecordAt(asset,rootNodeOffset)); /* 5f-format: ModelDefinition.rootNodeOffsetOrPointer */
    /* The node tree walk is a recursion over every child (ModelDefinition_ResolveNodeSprites). */
    if (ModelDefinition_ResolveNodeSprites
                  (Asset_RecordAt<MdlSerializedNodeHeader>(asset,rootNodeOffset),asset,&status,0)) {
      *outError = status;
      return false;
    }
  }
  status = ModelDefinition_ResolveShotAndEffectIds(definition);
  if (status != 0) {
    *outError = status;
    return false;
  }
  ModelDefinition_CopyTerrainClassValues(definition);
  return true;
}

/* First registered model definition with the given id in the 768-slot registry, or NULL. No side effects
   (ModelDefinitionRegistry_FindById also reports a miss). */
ModelDefinitionRecordPrefix *ModelDefinitionRegistry_LookupById(PckModelDefinitionIdCatalog definitionId)

{
  int registryIndex;
  ModelDefinitionRecordPrefix *registeredDefinition;

  for (registryIndex = 0; registryIndex < MODEL_DEFINITION_REGISTRY_SLOT_COUNT; registryIndex++) {
    registeredDefinition = g_ModelDefinitionRegistry[registryIndex];
    if ((registeredDefinition != nullptr) && (registeredDefinition->definitionId == definitionId)) {
      return registeredDefinition;
    }
  }
  return nullptr;
}

/* Looks a model definition up by id in the 768-slot registry. On a miss it writes a number into
   g_PackageLastErrorPath for the error message and returns NULL (the original returned
   FATAL_ERROR_MODEL_DEFINITION_MISSING with a failure flag; callers that passed that code on now supply it
   themselves). A found definition is never NULL.
*/
ModelDefinitionRecordPrefix *ModelDefinitionRegistry_FindById(PckModelDefinitionIdCatalog definitionId)

{
  ModelDefinitionRecordPrefix *registeredDefinition;

  registeredDefinition = ModelDefinitionRegistry_LookupById(definitionId);
  if (registeredDefinition == nullptr) {
    /* Original quirk: the number formatted is the last registry slot's content (what the scan loaded last),
       not the missing id */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
               (int32_t)(intptr_t)g_ModelDefinitionRegistry[MODEL_DEFINITION_REGISTRY_SLOT_COUNT - 1],
               g_PackageLastErrorPath);
  }
  return registeredDefinition;
}
