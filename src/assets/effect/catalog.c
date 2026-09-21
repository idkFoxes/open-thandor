/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/effect/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/effect/catalog.h>

/* Implementation ownership: assets/effect/catalog. */

/* Address: 0x0051E0B0.
   Ownership: assets/effect/catalog.
   Purpose: Validates the 'eff' magic and converter version 0x00040007, then prepares entryCount fixed 0xC0-byte
   entries beginning at +0x200. Preparation stops on the first CF-set entry failure. Invalid headers are copied to
   the package last-error path. Payload fields remain opaque. Role: Walks the EFF asset table and registers every
   serialized effect definition.
   Local calls: EffectDefinition_RegisterAndLoadSpriteCf.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
EffectAsset_PrepareEntries(EffectAssetHeader *asset)

{
  dword registrationStatusCode;
  AssetRecordCount AVar1;
  EffectAssetHeader *definition;
  EffectDefinition *definitionCursor;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  registrationStatusCode = 0x47;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_EFF) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_EFF_00040007)) {
    AVar1 = (asset->entryCountHeader).entryCount;
    definition = asset + 1;
    while( true ) {
      if (AVar1 == 0) {
        SVar2.carry = false;
        SVar2.valueOrError = registrationStatusCode;
        return SVar2;
      }
      SVar2 = EffectDefinition_RegisterAndLoadSpriteCf((EffectDefinition *)definition);
      registrationStatusCode = SVar2.valueOrError;
      if (SVar2.carry) break;
      definition = (EffectAssetHeader *)(definition->reservedB4_1FF + 0xc);
      AVar1 = AVar1 - 1;
    }
  }
  else {
    Package_SetLastErrorPath((word *)asset);
  }
  SVar3.carry = true;
  SVar3.valueOrError = registrationStatusCode;
  return SVar3;
}


/* Address: 0x0051E3E0.
   Ownership: assets/effect/catalog.
   Purpose: Walks all effect definitions and resolves stored effect and shot definition identifiers into runtime
   pointers.
   Local calls: EffectDefinitionRegistry_FindByIdWithErrorCf.
   Cross-module calls: ShotDefinitionRegistry_FindByIdWithErrorCf [assets/shot/catalog].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx EffectDefinitions_ResolveCrossReferences(void)

{
  StatusValueEaxCf5 SVar1;
  ShotDefinition *in_EAX;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionLookupEaxCf5 EVar2;
  ShotDefinitionLookupEaxCf5 SVar3;
  EffectDefinition *currentDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  do {
    currentDefinition = *registryCursor;
    if (currentDefinition != (EffectDefinition *)0x0) {
      if (currentDefinition->linkedEffectPresent != 0) {
        EVar2 = EffectDefinitionRegistry_FindByIdWithErrorCf
                          ((PckEffectDefinitionIdCatalog)currentDefinition->linkedEffectDefinition);
        in_EAX = (ShotDefinition *)EVar2.definitionOrError;
        if (EVar2.carry) {
          return (StatusValueEaxCf5)EVar2;
        }
        currentDefinition->linkedEffectDefinition = (EffectDefinition *)in_EAX;
      }
      if (currentDefinition->linkedShotPresent != 0) {
        SVar3 = ShotDefinitionRegistry_FindByIdWithErrorCf
                          ((PckShotDefinitionIdCatalog)currentDefinition->linkedShotDefinition);
        in_EAX = SVar3.definitionOrError;
        if (SVar3.carry) {
          return (StatusValueEaxCf5)SVar3;
        }
        currentDefinition->linkedShotDefinition = in_EAX;
      }
    }
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  SVar1.carry = false;
  SVar1.valueOrError = (dword)in_EAX;
  return SVar1;
}


/* Address: 0x0051DFD0.
   Ownership: assets/effect/catalog.
   Purpose: Registers one fixed 0xC0-byte effect definition in the 256-slot registry, rejects duplicate
   identifiers, changes its stored resource path to .spr, loads or reuses the sprite asset, and records ownership
   for later release. CF/EAX reports duplicate, capacity, path, package, or sprite registration failure. Role:
   Registers an EFF record, loads its referenced SPR resource and resolves effect/shot links. Inputs: Serialized
   EFF record and UTF-16 resource path; extension is normalized to .spr. Outputs: EffectDefinition with SpriteAsset
   and linked effect/shot pointers.
   Cross-module calls: EffectRuntime_FindDefinitionByIdCf [world/effects/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], SpriteAssetRegistry_FindById
   [assets/sprite/catalog], SpriteAsset_RegisterAndRelocatePointers [assets/sprite/catalog], Resource_Release
   [assets/resource/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectDefinition_RegisterAndLoadSpriteCf(EffectDefinition *definition)

{
  SpriteAssetHeader *asset;
  SpriteAssetHeader *pSVar1;
  int iVar2;
  EffectDefinition **registrySlotCursor;
  bool bVar3;
  EffectDefinitionLookupEaxCf5 EVar4;
  StatusValueEaxCf5 SVar5;
  PackageLoadEntryEaxCf5 PVar6;
  SpriteRegisterRelocateEaxCf5 SVar7;
  StatusValueEaxCf5 SVar8;
  
  registrySlotCursor = g_EffectDefinitionRegistry;
  iVar2 = 0x100;
  EVar4 = EffectRuntime_FindDefinitionByIdCf(definition->definitionId);
  asset = (SpriteAssetHeader *)EVar4.definitionOrError;
  if (EVar4.carry) {
    do {
      if (*registrySlotCursor == (EffectDefinition *)0x0) {
        *registrySlotCursor = definition;
        bVar3 = WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (bVar3) goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        PVar6 = Package_LoadEntry(definition->resourcePathUtf16);
        asset = PVar6.bufferOrError;
        if (PVar6.carry)
        goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        pSVar1 = SpriteAssetRegistry_FindById((asset->registryHeader).registryId);
        if (pSVar1 == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          SVar7 = SpriteAsset_RegisterAndRelocatePointers(asset);
          asset = SVar7.assetOrError;
          if (SVar7.carry)
          goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        }
        else {
          definition->ownedNestedResource = pSVar1;
          Resource_Release(asset);
          asset = pSVar1;
        }
        SVar8.carry = false;
        SVar8.valueOrError = (dword)asset;
        return SVar8;
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    asset = (SpriteAssetHeader *)0x49;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    asset = (SpriteAssetHeader *)0x4e;
  }
EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError:
  SVar5.carry = true;
  SVar5.valueOrError = (dword)asset;
  return SVar5;
}


/* Address: 0x0051E440.
   Ownership: assets/effect/catalog.
   Purpose: Returns null for identifier zero, otherwise scans the 256-slot effect registry. On a miss it formats
   the identifier into g_PackageLastErrorPath and returns error 0x48 with CF set.
*/
EffectDefinitionLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
EffectDefinitionRegistry_FindByIdWithErrorCf(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  EffectDefinitionLookupEaxCf5 EVar1;
  EffectDefinitionLookupEaxCf5 EVar2;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  candidateDefinition = (EffectDefinition *)0x0;
  if (definitionId != 0) {
    while ((candidateDefinition = *registryCursor, candidateDefinition == (EffectDefinition *)0x0 ||
           (candidateDefinition->definitionId != definitionId))) {
      registryCursor = registryCursor + 1;
      registrySlotsRemaining = registrySlotsRemaining + -1;
      if (registrySlotsRemaining == 0) {
        (*g_WideNumberFormatUtf16)
                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definitionId,g_PackageLastErrorPath);
        EVar1.carry = true;
        EVar1.definitionOrError = (EffectDefinition *)0x48;
        return EVar1;
      }
    }
  }
  EVar2.carry = false;
  EVar2.definitionOrError = candidateDefinition;
  return EVar2;
}

