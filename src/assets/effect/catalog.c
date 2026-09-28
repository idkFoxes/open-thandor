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
*/
StatusResult EffectAsset_PrepareEntries(EffectAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount remainingEntryCount;
  EffectDefinition *definition;
  StatusResult registrationResult;
  StatusResult failureResult;

  registrationStatusCode = FATAL_ERROR_EFFECT_ASSET_INVALID;
  if ((asset->entryCountHeader.common.magic == ASSET_MAGIC_EFF) &&
     (asset->entryCountHeader.common.converterVersion == PCK_CONVERTER_EFF_00040007)) {
    remainingEntryCount = asset->entryCountHeader.entryCount;
    definition = (EffectDefinition *)(asset + 1);
    while( true ) {
      if (remainingEntryCount == 0) {
        /* success hands back EAX as it was: the error code preset or the last registration result */
        registrationResult.failed = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = EffectDefinition_RegisterAndLoadSprite(definition);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.failed) break;
      definition++;
      remainingEntryCount--;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)asset);
  }
  failureResult.failed = true;
  failureResult.valueOrError = registrationStatusCode;
  return failureResult;
}


/* Address: 0x0051E3E0.
   Runs once all effect and shot assets are registered: replaces the linked effect and linked shot ids stored
   in every registered effect definition by pointers to those definitions. Fails with the lookup's error when
   an id is not registered.
*/
StatusResult EffectDefinitions_ResolveCrossReferences(void)

{
  StatusResult successResult;
  uint32_t lastResolvedDefinition = 0; /* EAX: success returns the last resolved definition (caller's EAX if none);
                                       callers test CF only */
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionResult effectLookup;
  ShotDefinitionResult shotLookup;
  EffectDefinition *currentDefinition;

  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  do {
    currentDefinition = *registryCursor;
    if (currentDefinition != NULL) {
      if (currentDefinition->linkedEffectPresent != 0) {
        effectLookup = EffectDefinitionRegistry_FindByIdWithError
                          ((PckEffectDefinitionIdCatalog)currentDefinition->linkedEffectDefinition);
        lastResolvedDefinition = (uint32_t)effectLookup.definitionOrError;
        if (effectLookup.notFound) {
          return THANDOR_BITCAST(EffectDefinitionResult, StatusResult, effectLookup);
        }
        currentDefinition->linkedEffectDefinition = effectLookup.definitionOrError;
      }
      if (currentDefinition->linkedShotPresent != 0) {
        shotLookup = ShotDefinitionRegistry_FindByIdWithError
                          ((PckShotDefinitionIdCatalog)currentDefinition->linkedShotDefinition);
        lastResolvedDefinition = (uint32_t)shotLookup.definitionOrError;
        if (shotLookup.notFound) {
          return THANDOR_BITCAST(ShotDefinitionResult, StatusResult, shotLookup);
        }
        currentDefinition->linkedShotDefinition = shotLookup.definitionOrError;
      }
    }
    registryCursor++;
    registrySlotsRemaining--;
  } while (registrySlotsRemaining != 0);
  successResult.failed = false;
  successResult.valueOrError = lastResolvedDefinition;
  return successResult;
}


/* Address: 0x0051DFD0.
   Registers one 0xC0-byte effect definition in the first free registry slot, switches its resource path to
   .spr and loads the sprite asset, reusing an already registered sprite with the same id (the fresh load is
   then released). CF/EAX report a duplicate id, a full registry or the path/package/sprite failure.
*/
StatusResult EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition)

{
  SpriteAssetHeader *assetOrError;
  SpriteAssetHeader *existingSpriteAsset;
  int registrySlotsRemaining;
  EffectDefinition **registrySlotCursor;
  bool extensionFailed;
  EffectDefinitionResult duplicateLookup;
  StatusResult failureResult;
  PackageLoadResult packageLoad;
  SpriteRegisterResult spriteRegistration;
  StatusResult successResult;
  
  registrySlotCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  duplicateLookup = EffectRuntime_FindDefinitionById(definition->definitionId);
  assetOrError = (SpriteAssetHeader *)duplicateLookup.definitionOrError;
  if (duplicateLookup.notFound) {
    do {
      if (*registrySlotCursor == NULL) {
        *registrySlotCursor = definition;
        extensionFailed = WidePath_SetExtensionCode(ASSET_MAGIC_SPR,definition->resourcePathUtf16);
        if (extensionFailed) goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        packageLoad = Package_LoadEntry(definition->resourcePathUtf16);
        assetOrError = packageLoad.bufferOrError;
        if (packageLoad.failed)
        goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        existingSpriteAsset = SpriteAssetRegistry_FindById(assetOrError->registryHeader.registryId);
        if (existingSpriteAsset == NULL) {
          definition->ownedNestedResourcePresent++;
          definition->ownedNestedResource = assetOrError;
          spriteRegistration = SpriteAsset_RegisterAndRelocatePointers(assetOrError);
          assetOrError = spriteRegistration.assetOrError;
          if (spriteRegistration.failed)
          goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        }
        else {
          definition->ownedNestedResource = existingSpriteAsset;
          Resource_Release(assetOrError);
          assetOrError = existingSpriteAsset;
        }
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)assetOrError;
        return successResult;
      }
      registrySlotCursor++;
      registrySlotsRemaining--;
    } while (registrySlotsRemaining != 0);
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
EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)assetOrError;
  return failureResult;
}


/* Address: 0x0051E440.
   Looks up a registered effect definition by id, used to turn serialized effect ids into pointers. Id 0
   means "no effect" and yields NULL. An unknown id is written as decimal text to g_PackageLastErrorPath and
   fails with FATAL_ERROR_EFFECT_ID_NOT_FOUND.
*/
EffectDefinitionResult EffectDefinitionRegistry_FindByIdWithError(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionResult missResult;
  EffectDefinitionResult foundResult;

  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = EFFECT_DEFINITION_REGISTRY_SLOT_COUNT;
  candidateDefinition = NULL;
  if (definitionId != 0) {
    while ((candidateDefinition = *registryCursor, candidateDefinition == NULL ||
           (candidateDefinition->definitionId != definitionId))) {
      registryCursor++;
      registrySlotsRemaining--;
      if (registrySlotsRemaining == 0) {
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
        missResult.notFound = true;
        missResult.definitionOrError = (EffectDefinition *)FATAL_ERROR_EFFECT_ID_NOT_FOUND;
        return missResult;
      }
    }
  }
  foundResult.notFound = false;
  foundResult.definitionOrError = candidateDefinition;
  return foundResult;
}

