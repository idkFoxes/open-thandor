/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/effect/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/effect/catalog.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

EffectDefinition *g_EffectDefinitionRegistry[256] = {};

/* Fixed name for the fatal-error box when an EFF asset has an invalid header (the asset's own path is not known
   here). */
static uint16_t s_EffectAssetErrorName[] = {'*', '.', 'e', 'f', 'f', 0}; /* L"*.eff" */

/* Registers every effect definition of a loaded EFF asset: checks the 'eff' magic and converter version
   0x40007, then hands each 0xC0-byte record after the 0x200-byte header to
   EffectDefinition_RegisterAndLoadSprite, stopping at the first failure. An invalid header leaves "*.eff" in
   g_PackageLastErrorPath and fails with FATAL_ERROR_EFFECT_ASSET_INVALID.
   Returns true on success; on failure returns false with the error code in *outError (left untouched on
   success). (The original's success return value, the last loaded sprite asset, was read by no caller.)
*/
bool EffectAsset_PrepareEntries(EffectAssetHeader *asset,uint32_t *outError)

{
  uint32_t registrationStatusCode;
  AssetRecordCount remainingEntryCount;
  EffectDefinition *definition;

  if ((asset->entryCountHeader.common.magic != ASSET_MAGIC_EFF) ||
     (asset->entryCountHeader.common.converterVersion != PCK_CONVERTER_EFF_00040007)) {
    /* The original passes the asset header itself as the error path; replaced by a fixed name here because the
       header words are no text (units >= 0x8000 become rich-text pointer codes in the fatal-error box). */
    Package_SetLastErrorPath(s_EffectAssetErrorName);
    *outError = FATAL_ERROR_EFFECT_ASSET_INVALID;
    return false;
  }
  registrationStatusCode = FATAL_ERROR_EFFECT_ASSET_INVALID;
  definition = Asset_RecordAfter<EffectDefinition>(asset);
  for (remainingEntryCount = asset->entryCountHeader.entryCount; remainingEntryCount != 0;
       remainingEntryCount--) {
    if (!EffectDefinition_RegisterAndLoadSprite(definition,&registrationStatusCode)) {
      *outError = registrationStatusCode;
      return false;
    }
    definition++;
  }
  return true;
}


/* Runs once all effect and shot assets are registered: replaces the linked effect and linked shot ids stored
   in every registered effect definition by pointers to those definitions. Returns 0 on success, or the
   lookup's error code (FATAL_ERROR_EFFECT_ID_NOT_FOUND / FATAL_ERROR_SHOT_ID_NOT_FOUND) as soon as an id is
   not registered. (The original's success return value, the last resolved definition, was read by no
   caller.)
*/
uint32_t EffectDefinitions_ResolveCrossReferences()

{
  uint32_t lookupError;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinition *linkedEffect;
  ShotDefinition *linkedShot;
  EffectDefinition *currentDefinition;

  registryCursor = g_EffectDefinitionRegistry;
  for (registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    currentDefinition = *registryCursor;
    if (currentDefinition != nullptr) {
      if (currentDefinition->linkedEffectPresent != 0) {
        lookupError = EffectDefinitionRegistry_FindById
                          ((PckEffectDefinitionIdCatalog)currentDefinition->linkedEffectDefinition,&linkedEffect); /* 5f-format: EffectDefinition.linkedEffectDefinition (+0x14, id on disk, pointer after resolve) */
        if (lookupError != 0) {
          return lookupError;
        }
        currentDefinition->linkedEffectDefinition = linkedEffect;
      }
      if (currentDefinition->linkedShotPresent != 0) {
        lookupError = ShotDefinitionRegistry_FindByIdWithError
                          ((PckShotDefinitionIdCatalog)currentDefinition->linkedShotDefinition,&linkedShot); /* 5f-format: EffectDefinition.linkedShotDefinition (+0x1C, id on disk, pointer after resolve) */
        if (lookupError != 0) {
          return lookupError;
        }
        currentDefinition->linkedShotDefinition = linkedShot;
      }
    }
    registryCursor++;
  }
  return 0;
}


/* Registers one 0xC0-byte effect definition in the first free registry slot, switches its resource path to
   .spr and loads the sprite asset, reusing an already registered sprite with the same id (the fresh load is
   then released). Returns true on success; returns false with the error code in *outError (untouched on
   success) on a duplicate id, a full registry or a path/package/sprite failure. (The original's success return
   value, the sprite asset, was read by no caller.)
*/
bool EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition,uint32_t *outError)

{
  SpriteAssetHeader *loadedSpriteAsset;
  SpriteAssetHeader *existingSpriteAsset;
  int registrySlotsRemaining;
  EffectDefinition **registrySlotCursor;
  uint32_t loadErrorCode;
  uint32_t spriteRegisterError;

  if (EffectRuntime_FindDefinitionById(definition->definitionId) != nullptr) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    *outError = FATAL_ERROR_EFFECT_ID_DUPLICATE;
    return false;
  }
  registrySlotCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  while (registrySlotsRemaining != 0 && *registrySlotCursor != nullptr) {
    registrySlotCursor++;
    registrySlotsRemaining--;
  }
  if (registrySlotsRemaining == 0) {
    /* the registry capacity goes to the error text */
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,EFFECT_DEFINITION_REGISTRY_SLOT_COUNT,
                            g_PackageLastErrorPath);
    *outError = FATAL_ERROR_EFFECT_REGISTRY_FULL;
    return false;
  }
  *registrySlotCursor = definition;
  if (WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16,
                                sizeof definition->resourcePathUtf16 / sizeof(uint16_t))) {
    /* Original quirk: a failure to set the .spr extension reports FATAL_ERROR_EFFECT_ID_NOT_FOUND (the
       definition stays registered) */
    *outError = FATAL_ERROR_EFFECT_ID_NOT_FOUND;
    return false;
  }
  loadedSpriteAsset = static_cast<SpriteAssetHeader *>(Package_LoadEntry(definition->resourcePathUtf16,&loadErrorCode));
  if (loadedSpriteAsset == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  existingSpriteAsset = SpriteAssetRegistry_FindById(loadedSpriteAsset->registryHeader.registryId);
  if (existingSpriteAsset == nullptr) {
    definition->ownedNestedResourcePresent++;
    definition->ownedNestedResource = loadedSpriteAsset;
    spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers(loadedSpriteAsset);
    if (spriteRegisterError != 0) {
      *outError = spriteRegisterError;
      return false;
    }
  }
  else {
    definition->ownedNestedResource = existingSpriteAsset;
    Resource_Release(loadedSpriteAsset);
  }
  return true;
}


/* The first registered effect definition with this id in the 256-slot registry, or NULL. No side effects. */
EffectDefinition *EffectDefinitionRegistry_LookupById(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;

  registryCursor = g_EffectDefinitionRegistry;
  for (registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT; registrySlotsRemaining != 0;
       registrySlotsRemaining--) {
    candidateDefinition = *registryCursor;
    if (candidateDefinition != nullptr && candidateDefinition->definitionId == definitionId) {
      return candidateDefinition;
    }
    registryCursor++;
  }
  return nullptr;
}

/* Looks up a registered effect definition by id, used to turn serialized effect ids into pointers. Id 0
   means "no effect" and yields NULL. Returns 0 with the definition (or NULL) in *outDefinition. An unknown id
   is written as decimal text to g_PackageLastErrorPath and FATAL_ERROR_EFFECT_ID_NOT_FOUND is returned;
   *outDefinition is then left unchanged.
*/
uint32_t EffectDefinitionRegistry_FindById(PckEffectDefinitionIdCatalog definitionId,EffectDefinition **outDefinition)

{
  EffectDefinition *candidateDefinition;

  if (definitionId == 0) {
    *outDefinition = nullptr;
    return 0;
  }
  candidateDefinition = EffectDefinitionRegistry_LookupById(definitionId);
  if (candidateDefinition != nullptr) {
    *outDefinition = candidateDefinition;
    return 0;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_EFFECT_ID_NOT_FOUND;
}

