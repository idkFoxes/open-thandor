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
   Ownership: assets/effect/catalog.
   Purpose: Validates the 'eff' magic and converter version 0x00040007, then prepares entryCount fixed 0xC0-byte
   entries beginning at +0x200. Preparation stops on the first CF-set entry failure. Invalid headers are copied to
   the package last-error path. Payload fields remain opaque. Role: Walks the EFF asset table and registers every
   serialized effect definition.
   Local calls: EffectDefinition_RegisterAndLoadSprite.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusResult __thandor_void_preserve_ecx_edx
EffectAsset_PrepareEntries(EffectAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount remainingEntryCount;
  EffectAssetHeader *definition;
  EffectDefinition *definitionCursor;
  StatusResult registrationResult;
  StatusResult failureResult;
  
  registrationStatusCode = 0x47;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_EFF) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_EFF_00040007)) {
    remainingEntryCount = (asset->entryCountHeader).entryCount;
    definition = asset + 1;
    while( true ) {
      if (remainingEntryCount == 0) {
        registrationResult.failed = false;
        registrationResult.valueOrError = registrationStatusCode;
        return registrationResult;
      }
      registrationResult = EffectDefinition_RegisterAndLoadSprite((EffectDefinition *)definition);
      registrationStatusCode = registrationResult.valueOrError;
      if (registrationResult.failed) break;
      definition = (EffectAssetHeader *)(definition->reservedB4_1FF + 0xc);
      remainingEntryCount = remainingEntryCount - 1;
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
   Ownership: assets/effect/catalog.
   Purpose: Walks all effect definitions and resolves stored effect and shot definition identifiers into runtime
   pointers.
   Local calls: EffectDefinitionRegistry_FindByIdWithError.
   Cross-module calls: ShotDefinitionRegistry_FindByIdWithError [assets/shot/catalog].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx EffectDefinitions_ResolveCrossReferences(void)

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
  registrySlotsRemaining = 0x100;
  do {
    currentDefinition = *registryCursor;
    if (currentDefinition != (EffectDefinition *)0x0) {
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
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  successResult.failed = false;
  successResult.valueOrError = lastResolvedDefinition;
  return successResult;
}


/* Address: 0x0051DFD0.
   Ownership: assets/effect/catalog.
   Purpose: Registers one fixed 0xC0-byte effect definition in the 256-slot registry, rejects duplicate
   identifiers, changes its stored resource path to .spr, loads or reuses the sprite asset, and records ownership
   for later release. CF/EAX reports duplicate, capacity, path, package, or sprite registration failure. Role:
   Registers an EFF record, loads its referenced SPR resource and resolves effect/shot links. Inputs: Serialized
   EFF record and UTF-16 resource path; extension is normalized to .spr. Outputs: EffectDefinition with SpriteAsset
   and linked effect/shot pointers.
   Cross-module calls: EffectRuntime_FindDefinitionById [world/effects/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], SpriteAssetRegistry_FindById
   [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers [assets/sprite/catalog], Resource_Release
   [assets/resource/runtime].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
EffectDefinition_RegisterAndLoadSprite(EffectDefinition *definition)

{
  SpriteAssetHeader *asset;
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
  registrySlotsRemaining = 0x100;
  duplicateLookup = EffectRuntime_FindDefinitionById(definition->definitionId);
  asset = (SpriteAssetHeader *)duplicateLookup.definitionOrError;
  if (duplicateLookup.notFound) {
    do {
      if (*registrySlotCursor == (EffectDefinition *)0x0) {
        *registrySlotCursor = definition;
        extensionFailed = WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (extensionFailed) goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        packageLoad = Package_LoadEntry(definition->resourcePathUtf16);
        asset = packageLoad.bufferOrError;
        if (packageLoad.failed)
        goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        existingSpriteAsset = SpriteAssetRegistry_FindById((asset->registryHeader).registryId);
        if (existingSpriteAsset == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          spriteRegistration = SpriteAsset_RegisterAndRelocatePointers(asset);
          asset = spriteRegistration.assetOrError;
          if (spriteRegistration.failed)
          goto EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError;
        }
        else {
          definition->ownedNestedResource = existingSpriteAsset;
          Resource_Release(asset);
          asset = existingSpriteAsset;
        }
        successResult.failed = false;
        successResult.valueOrError = (uint32_t)asset;
        return successResult;
      }
      registrySlotCursor = registrySlotCursor + 1;
      registrySlotsRemaining = registrySlotsRemaining + -1;
    } while (registrySlotsRemaining != 0);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    asset = (SpriteAssetHeader *)0x49;
  }
  else {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    asset = (SpriteAssetHeader *)0x4e;
  }
EffectDefinition_RegisterAndLoadSprite_ReturnRegistryOrSpriteLoadError:
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)asset;
  return failureResult;
}


/* Address: 0x0051E440.
   Ownership: assets/effect/catalog.
   Purpose: Returns null for identifier zero, otherwise scans the 256-slot effect registry. On a miss it formats
   the identifier into g_PackageLastErrorPath and returns error 0x48 with CF set.
*/
EffectDefinitionResult __thandor_eax_cf_preserve_ecx_edx
EffectDefinitionRegistry_FindByIdWithError(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionResult missResult;
  EffectDefinitionResult foundResult;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  candidateDefinition = (EffectDefinition *)0x0;
  if (definitionId != 0) {
    while ((candidateDefinition = *registryCursor, candidateDefinition == (EffectDefinition *)0x0 ||
           (candidateDefinition->definitionId != definitionId))) {
      registryCursor = registryCursor + 1;
      registrySlotsRemaining = registrySlotsRemaining + -1;
      if (registrySlotsRemaining == 0) {
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
        missResult.notFound = true;
        missResult.definitionOrError = (EffectDefinition *)0x48;
        return missResult;
      }
    }
  }
  foundResult.notFound = false;
  foundResult.definitionOrError = candidateDefinition;
  return foundResult;
}

