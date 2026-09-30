/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/effect/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/effect/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/effect/catalog. */

/* Address: 0x0051E0B0.
   Registers every effect definition of a loaded EFF asset: checks the 'eff' magic and converter version
   0x40007, then hands each 0xC0-byte record after the 0x200-byte header to
   EffectDefinition_RegisterAndLoadSprite, stopping at the first failure. An invalid header leaves the asset
   path in g_PackageLastErrorPath and fails with FATAL_ERROR_EFFECT_ASSET_INVALID.
   Returns true on success; on failure returns false with the error code in *outError (left untouched on
   success). (The original's success EAX, the last loaded sprite asset, was read by no caller.)
*/
bool EffectAsset_PrepareEntries(EffectAssetHeader *asset,uint32_t *outError)

{
  uint32_t registrationStatusCode;
  AssetRecordCount remainingEntryCount;
  EffectDefinition *definition;

  registrationStatusCode = FATAL_ERROR_EFFECT_ASSET_INVALID;
  if ((asset->entryCountHeader.common.magic == ASSET_MAGIC_EFF) &&
     (asset->entryCountHeader.common.converterVersion == PCK_CONVERTER_EFF_00040007)) {
    definition = (EffectDefinition *)(asset + 1);
    for (remainingEntryCount = asset->entryCountHeader.entryCount; remainingEntryCount != 0;
         remainingEntryCount--) {
      if (!EffectDefinition_RegisterAndLoadSprite(definition,&registrationStatusCode)) {
        goto ReturnFailure;
      }
      definition++;
    }
    return true;
  }
  else {
    Package_SetLastErrorPath((uint16_t *)asset);
  }
ReturnFailure:
  *outError = registrationStatusCode;
  return false;
}


/* Address: 0x0051E3E0.
   Runs once all effect and shot assets are registered: replaces the linked effect and linked shot ids stored
   in every registered effect definition by pointers to those definitions. Returns 0 on success, or the
   lookup's error code (FATAL_ERROR_EFFECT_ID_NOT_FOUND / FATAL_ERROR_SHOT_ID_NOT_FOUND) as soon as an id is
   not registered. (The original's success EAX, the last resolved definition, was read by no caller.)
*/
uint32_t EffectDefinitions_ResolveCrossReferences(void)

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
    if (currentDefinition != NULL) {
      if (currentDefinition->linkedEffectPresent != 0) {
        lookupError = EffectDefinitionRegistry_FindById
                          ((PckEffectDefinitionIdCatalog)currentDefinition->linkedEffectDefinition,&linkedEffect);
        if (lookupError != 0) {
          return lookupError;
        }
        currentDefinition->linkedEffectDefinition = linkedEffect;
      }
      if (currentDefinition->linkedShotPresent != 0) {
        lookupError = ShotDefinitionRegistry_FindByIdWithError
                          ((PckShotDefinitionIdCatalog)currentDefinition->linkedShotDefinition,&linkedShot);
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


/* Address: 0x0051DFD0.
   Registers one 0xC0-byte effect definition in the first free registry slot, switches its resource path to
   .spr and loads the sprite asset, reusing an already registered sprite with the same id (the fresh load is
   then released). Returns true on success; returns false with the error code in *outError (untouched on
   success) on a duplicate id, a full registry or a path/package/sprite failure. (The original's success EAX,
   the sprite asset, was read by no caller.)
*/
bool EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition,uint32_t *outError)

{
  SpriteAssetHeader *assetOrError;
  SpriteAssetHeader *existingSpriteAsset;
  int registrySlotsRemaining;
  EffectDefinition **registrySlotCursor;
  bool extensionFailed;
  PackageLoadResult packageLoad;
  uint32_t spriteRegisterError;

  registrySlotCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  if (EffectRuntime_FindDefinitionById(definition->definitionId) == NULL) {
    /* Original quirk: the lookup's failure value is the error reported when the .spr extension cannot be set */
    assetOrError = (SpriteAssetHeader *)FATAL_ERROR_EFFECT_ID_NOT_FOUND;
    for (; registrySlotsRemaining != 0; registrySlotsRemaining--) {
      if (*registrySlotCursor == NULL) {
        *registrySlotCursor = definition;
        extensionFailed = WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16);
        if (extensionFailed) goto ReturnFailure;
        packageLoad = Package_LoadEntry(definition->resourcePathUtf16);
        assetOrError = packageLoad.bufferOrError;
        if (packageLoad.failed) goto ReturnFailure;
        existingSpriteAsset = SpriteAssetRegistry_FindById(assetOrError->registryHeader.registryId);
        if (existingSpriteAsset == NULL) {
          definition->ownedNestedResourcePresent++;
          definition->ownedNestedResource = assetOrError;
          spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers(assetOrError);
          if (spriteRegisterError != 0) {
            assetOrError = (SpriteAssetHeader *)spriteRegisterError;
            goto ReturnFailure;
          }
        }
        else {
          definition->ownedNestedResource = existingSpriteAsset;
          Resource_Release(assetOrError);
        }
        return true;
      }
      registrySlotCursor++;
    }
    /* the registry capacity goes to the error text */
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,EFFECT_DEFINITION_REGISTRY_SLOT_COUNT,
                            g_PackageLastErrorPath);
    assetOrError = (SpriteAssetHeader *)FATAL_ERROR_EFFECT_REGISTRY_FULL;
  }
  else {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    assetOrError = (SpriteAssetHeader *)FATAL_ERROR_EFFECT_ID_DUPLICATE;
  }
ReturnFailure:
  *outError = (uint32_t)assetOrError;
  return false;
}


/* Address: 0x0051E440.
   Looks up a registered effect definition by id, used to turn serialized effect ids into pointers. Id 0
   means "no effect" and yields NULL. Returns 0 with the definition (or NULL) in *outDefinition. An unknown id
   is written as decimal text to g_PackageLastErrorPath and FATAL_ERROR_EFFECT_ID_NOT_FOUND is returned;
   *outDefinition is then left unchanged.
*/
uint32_t EffectDefinitionRegistry_FindById(PckEffectDefinitionIdCatalog definitionId,EffectDefinition **outDefinition)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;

  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  candidateDefinition = NULL;
  if (definitionId != 0) {
    while (candidateDefinition = *registryCursor, candidateDefinition == NULL || candidateDefinition->definitionId != definitionId) {
      registryCursor++;
      registrySlotsRemaining--;
      if (registrySlotsRemaining == 0) {
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
        return FATAL_ERROR_EFFECT_ID_NOT_FOUND;
      }
    }
  }
  *outDefinition = candidateDefinition;
  return 0;
}

