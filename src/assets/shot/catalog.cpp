/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/shot/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/shot/catalog.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

ShotDefinition *g_ShotDefinitionRegistry[256] = {};

/* Implementation ownership: assets/shot/catalog. */

/* Registers every shot definition of a loaded SHT asset: checks the 'sht' magic and converter version
   0x60006, then hands each 0x2E0-byte record after the 0x200-byte header to
   ShotDefinition_RegisterAndResolveReferences, stopping at the first failure. An invalid header leaves the
   asset path in g_PackageLastErrorPath and fails with FATAL_ERROR_SHOT_ASSET_INVALID.
   Returns 0 on success, otherwise the error code (the original's success return value was never used by its
   callers).
*/
uint32_t ShotAsset_PrepareEntries(ShotAssetHeader *asset)

{
  uint32_t registrationError;
  AssetRecordCount entriesRemaining;
  ShotDefinition *definition;

  if ((asset->entryCountHeader.common.magic == ASSET_MAGIC_SHT) &&
     (asset->entryCountHeader.common.converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    definition = (ShotDefinition *)(asset + 1);
    for (entriesRemaining = asset->entryCountHeader.entryCount; entriesRemaining != 0; entriesRemaining--) {
      registrationError = ShotDefinition_RegisterAndResolveReferences(definition);
      if (registrationError != 0) {
        return registrationError;
      }
      definition++;
    }
    return 0;
  }
  Package_SetLastErrorPath((uint16_t *)asset);
  return FATAL_ERROR_SHOT_ASSET_INVALID;
}

/* Runs after the level's terrain materials are loaded: checks that each of the 31 terrain-material indices
   of every registered shot definition is negative (no material) or names a loaded material. Otherwise the
   registry index of the offending shot is written to g_PackageLastErrorPath and the check fails with
   FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID. Returns 0 on success, otherwise that error code (the original's
   success return value, the last index checked, was read by no caller).
*/
uint32_t ShotDefinitions_ValidateTerrainMaterialReferences()

{
  TerrainMaterialIndex materialIndex;
  int registrySlotsRemaining;
  int materialIndicesRemaining;
  ShotDefinition *definition;
  TerrainMaterialIndex *materialIndexCursor;
  ShotDefinition **registryCursor;

  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    definition = *registryCursor;
    if (definition != nullptr) {
      materialIndexCursor = definition->terrainMaterialIndices31;
      for (materialIndicesRemaining = SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT; materialIndicesRemaining != 0;
           materialIndicesRemaining--) {
        materialIndex = *materialIndexCursor;
        materialIndexCursor++;
        if (TERRAIN_MATERIAL_COUNT - 1 < materialIndex ||
            (-1 < materialIndex && g_TerrainMaterialTextureSets[materialIndex] == nullptr)) {
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                     SHOT_DEFINITION_REGISTRY_SLOT_COUNT - registrySlotsRemaining,g_PackageLastErrorPath);
          return FATAL_ERROR_SHOT_TERRAIN_MATERIAL_INVALID;
        }
      }
    }
    registryCursor++;
  }
  return 0;
}

/* The first registered shot definition with this id in the 256-slot registry, or NULL. No side effects. */
ShotDefinition *ShotDefinitionRegistry_LookupById(PckShotDefinitionIdCatalog definitionId)

{
  ShotDefinition *registeredDefinition;
  int registrySlotsRemaining;
  ShotDefinition **registryCursor;

  registryCursor = g_ShotDefinitionRegistry;
  for (registrySlotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    registeredDefinition = *registryCursor;
    if (registeredDefinition != nullptr && registeredDefinition->definitionId == definitionId) {
      return registeredDefinition;
    }
    registryCursor++;
  }
  return nullptr;
}

/* Looks up a registered shot definition by id: returns 0 and stores it in *outDefinition. On a miss the id is
   written as decimal text to g_PackageLastErrorPath for the fatal-error message, *outDefinition is left untouched
   and FATAL_ERROR_SHOT_ID_NOT_FOUND is returned.
*/
uint32_t ShotDefinitionRegistry_FindByIdWithError
          (PckShotDefinitionIdCatalog definitionId,ShotDefinition **outDefinition)

{
  ShotDefinition *registeredDefinition;

  registeredDefinition = ShotDefinitionRegistry_LookupById(definitionId);
  if (registeredDefinition != nullptr) {
    *outDefinition = registeredDefinition;
    return 0;
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_SHOT_ID_NOT_FOUND;
}

/* Loads the sprite of a freshly registered shot definition: switches the resource path to .spr, loads it and
   either registers it as owned by the definition or, when a sprite with the same id is already registered,
   reuses that one and releases the fresh load. Returns 0 or the error code. */
static uint32_t ShotDefinition_LoadSprite(ShotDefinition *definition)
{
  SpriteAssetHeader *loadedSprite;
  SpriteAssetHeader *existingSprite;
  uint32_t loadErrorCode;
  uint32_t spriteRegisterError;

  if (WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16)) {
    /* Original quirk: the original returns the leftover error code of the earlier duplicate-id lookup, so a
       failing .spr extension switch returns FATAL_ERROR_SHOT_ID_NOT_FOUND */
    return FATAL_ERROR_SHOT_ID_NOT_FOUND;
  }
  loadedSprite = (SpriteAssetHeader *)Package_LoadEntry(definition->resourcePathUtf16,&loadErrorCode);
  if (loadedSprite == nullptr) {
    return loadErrorCode;
  }
  existingSprite = SpriteAssetRegistry_FindById(loadedSprite->registryHeader.registryId);
  if (existingSprite == nullptr) {
    definition->ownedNestedResourcePresent++;
    definition->ownedNestedResource = loadedSprite;
    spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers(loadedSprite);
    if (spriteRegisterError != 0) {
      return spriteRegisterError;
    }
  }
  else {
    definition->ownedNestedResource = existingSprite;
    Resource_Release(loadedSprite);
  }
  return 0;
}

/* Replaces the effect definition id stored in *effectReference by the registered effect definition.
   On a failed lookup the field keeps the id and the lookup's error code is returned. */
static uint32_t ShotDefinition_ResolveEffectReference(Ptr32<EffectDefinition> *effectReference)
{
  uint32_t effectLookupError;
  EffectDefinition *resolvedEffect;

  effectLookupError = EffectDefinitionRegistry_FindById
                    ((PckEffectDefinitionIdCatalog)*effectReference,&resolvedEffect); /* 5f-format: ShotDefinition effect references (id on disk, pointer after resolve) */
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  *effectReference = resolvedEffect;
  return 0;
}

/* Resolves the launch, secondary and primary effects and the 31 terrain-impact and 8 target-class-impact
   effects of a shot definition (the effect pointer fields hold effect definition ids until then), stopping at
   the first failing lookup. Returns 0 or that lookup's error code. */
static uint32_t ShotDefinition_ResolveEffectReferences(ShotDefinition *definition)
{
  uint32_t effectLookupError;
  int referenceIndex;

  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->launchEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->secondaryEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  effectLookupError = ShotDefinition_ResolveEffectReference(&definition->primaryEffectDefinition);
  if (effectLookupError != 0) {
    return effectLookupError;
  }
  for (referenceIndex = 0; referenceIndex < SHOT_TERRAIN_MATERIAL_REFERENCE_COUNT; referenceIndex++) {
    effectLookupError =
         ShotDefinition_ResolveEffectReference(&definition->terrainImpactEffectDefinitions31[referenceIndex]);
    if (effectLookupError != 0) {
      return effectLookupError;
    }
  }
  for (referenceIndex = 0; referenceIndex < SHOT_TARGET_CLASS_IMPACT_COUNT; referenceIndex++) {
    effectLookupError =
         ShotDefinition_ResolveEffectReference(&definition->targetClassImpactEffectDefinitions8[referenceIndex]);
    if (effectLookupError != 0) {
      return effectLookupError;
    }
  }
  return 0;
}

/* Registers one 0x2E0-byte shot definition in the first free registry slot, loads its sprite (switching the
   resource path to .spr; an already registered sprite with the same id is reused and the fresh load released)
   and replaces the effect ids of the launch, secondary and primary effects and of the 31 terrain-impact and 8
   target-class-impact effects by their registered definitions. Returns 0 on success, otherwise the error code
   of a duplicate id, a full registry or the first failing load or lookup. (The original's success return value,
   the last resolved effect definition, is still in targetClassImpactEffectDefinitions8[7].)
   Original quirk: a failing sprite load or effect lookup leaves the definition registered in its slot.
*/
uint32_t ShotDefinition_RegisterAndResolveReferences(ShotDefinition *definition)

{
  int slotsRemaining;
  ShotDefinition **registrySlotCursor;
  uint32_t spriteError;

  /* The original accepts a lifetime of 0; rejected here because world/shots/flight.cpp divides the impact damage
     by it (the stock shots use 6 and more). */
  if (definition->projectileLifetimeTicks == 0) {
    Thandor_Log("ShotDefinition_RegisterAndResolveReferences: shot %u has projectile lifetime 0, rejected",
                (uint32_t)definition->definitionId);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_SHOT_ASSET_INVALID;
  }
  if (ShotRuntime_FindDefinitionById(definition->definitionId) != nullptr) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    return FATAL_ERROR_SHOT_ID_DUPLICATE;
  }
  registrySlotCursor = g_ShotDefinitionRegistry;
  for (slotsRemaining = SHOT_DEFINITION_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (*registrySlotCursor == nullptr) {
      *registrySlotCursor = definition;
      spriteError = ShotDefinition_LoadSprite(definition);
      if (spriteError != 0) {
        return spriteError;
      }
      return ShotDefinition_ResolveEffectReferences(definition);
    }
    registrySlotCursor++;
  }
  /* the registry capacity goes to the error text */
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,SHOT_DEFINITION_REGISTRY_SLOT_COUNT,
                          g_PackageLastErrorPath);
  return FATAL_ERROR_SHOT_REGISTRY_FULL;
}
