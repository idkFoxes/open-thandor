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
dword EffectAsset_PrepareEntries(EffectAssetHeader *asset)

{
  dword registrationStatusCode;
  undefined4 extraout_EAX;
  int extraout_ECX;
  EffectDefinition *definitionCursor;
  bool bVar1;
  undefined8 uVar2;
  
  registrationStatusCode = 0x47;
  if (((asset->entryCountHeader).common.magic == ASSET_MAGIC_EFF) &&
     ((asset->entryCountHeader).common.converterVersion == PCK_CONVERTER_EFF_00040007)) {
    definitionCursor = (EffectDefinition *)(asset + 1);
    bVar1 = false;
    if ((asset->entryCountHeader).entryCount != 0) {
      do {
        uVar2 = EffectDefinition_RegisterAndLoadSpriteCf(definitionCursor);
        registrationStatusCode = (dword)uVar2;
        if (bVar1) {
          return registrationStatusCode;
        }
        bVar1 = (EffectDefinition *)0xffffff3f < definitionCursor;
        definitionCursor = definitionCursor + 1;
      } while (extraout_ECX != 1);
    }
    return registrationStatusCode;
  }
  Package_SetLastErrorPath((word *)asset);
  return extraout_EAX;
}

/* Address: 0x0051E3E0.
   Ownership: assets/effect/catalog.
   Purpose: Walks all effect definitions and resolves stored effect and shot definition identifiers into runtime
   pointers.
   Local calls: EffectDefinitionRegistry_FindByIdWithErrorCf.
   Cross-module calls: ShotDefinitionRegistry_FindByIdWithErrorCf [assets/shot/catalog].
*/
void __cdecl EffectDefinitions_ResolveCrossReferences(void)

{
  EffectDefinition *pEVar1;
  ShotDefinition *pSVar2;
  int registrySlotsRemaining;
  int extraout_ECX;
  int extraout_ECX_00;
  EffectDefinition **registryCursor;
  bool bVar3;
  EffectDefinition *currentDefinition;
  
  registryCursor = g_EffectDefinitionRegistry;
  registrySlotsRemaining = 0x100;
  do {
    currentDefinition = *registryCursor;
    if (currentDefinition != (EffectDefinition *)0x0) {
      bVar3 = false;
      if (currentDefinition->linkedEffectPresent != 0) {
        pEVar1 = EffectDefinitionRegistry_FindByIdWithErrorCf
                           ((PckEffectDefinitionIdCatalog)currentDefinition->linkedEffectDefinition)
        ;
        if (bVar3) {
          return;
        }
        currentDefinition->linkedEffectDefinition = pEVar1;
        registrySlotsRemaining = extraout_ECX;
      }
      bVar3 = false;
      if (currentDefinition->linkedShotPresent != 0) {
        pSVar2 = ShotDefinitionRegistry_FindByIdWithErrorCf
                           ((PckShotDefinitionIdCatalog)currentDefinition->linkedShotDefinition);
        if (bVar3) {
          return;
        }
        currentDefinition->linkedShotDefinition = pSVar2;
        registrySlotsRemaining = extraout_ECX_00;
      }
    }
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  return;
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
undefined8 EffectDefinition_RegisterAndLoadSpriteCf(EffectDefinition *definition)

{
  SpriteAssetHeader *pSVar1;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar2;
  SpriteAssetHeader *asset;
  EffectDefinition **registrySlotCursor;
  undefined1 in_CF;
  bool bVar3;
  
  registrySlotCursor = g_EffectDefinitionRegistry;
  EffectRuntime_FindDefinitionByIdCf(definition->definitionId);
  iVar2 = extraout_EDX;
  if ((bool)in_CF) {
    do {
      bVar3 = false;
      if (*registrySlotCursor == (EffectDefinition *)0x0) {
        *registrySlotCursor = definition;
        pSVar1 = (SpriteAssetHeader *)
                 WidePath_SetExtensionCode(0x727073,definition->resourcePathUtf16);
        if (bVar3) goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        pSVar1 = Package_LoadEntry(definition->resourcePathUtf16);
        if (bVar3) goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        pSVar1 = SpriteAssetRegistry_FindById((pSVar1->registryHeader).registryId);
        bVar3 = false;
        if (pSVar1 == (SpriteAssetHeader *)0x0) {
          definition->ownedNestedResourcePresent = definition->ownedNestedResourcePresent + 1;
          definition->ownedNestedResource = asset;
          pSVar1 = SpriteAsset_RegisterAndRelocatePointers(asset);
          if (bVar3) goto EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError;
        }
        else {
          definition->ownedNestedResource = pSVar1;
          pSVar1 = (SpriteAssetHeader *)Resource_Release(asset);
        }
        return CONCAT44(in_EDX,pSVar1);
      }
      registrySlotCursor = registrySlotCursor + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x100,g_PackageLastErrorPath);
    pSVar1 = (SpriteAssetHeader *)0x49;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,definition->definitionId,g_PackageLastErrorPath);
    pSVar1 = (SpriteAssetHeader *)0x4e;
  }
EffectDefinition_RegisterAndLoadSpriteCf_ReturnRegistryOrSpriteLoadError:
  return CONCAT44(in_EDX,pSVar1);
}

/* Address: 0x0051E440.
   Ownership: assets/effect/catalog.
   Purpose: Returns null for identifier zero, otherwise scans the 256-slot effect registry. On a miss it formats
   the identifier into g_PackageLastErrorPath and returns error 0x48 with CF set.
*/
EffectDefinition *
EffectDefinitionRegistry_FindByIdWithErrorCf(PckEffectDefinitionIdCatalog definitionId)

{
  EffectDefinition *candidateDefinition;
  int registrySlotsRemaining;
  EffectDefinition **registryCursor;
  
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
        return (EffectDefinition *)0x48;
      }
    }
  }
  return candidateDefinition;
}
