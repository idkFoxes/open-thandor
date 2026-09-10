# Call graph: `assets/effect/catalog`

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051E0B0` **EffectAsset_PrepareEntries** — local: `EffectDefinition_RegisterAndLoadSpriteCf`; cross: `Package_SetLastErrorPath` → `assets/package/runtime`
- `0x0051E3E0` **EffectDefinitions_ResolveCrossReferences** — local: `EffectDefinitionRegistry_FindByIdWithErrorCf`; cross: `ShotDefinitionRegistry_FindByIdWithErrorCf` → `assets/shot/catalog`
- `0x0051DFD0` **EffectDefinition_RegisterAndLoadSpriteCf** — cross: `EffectRuntime_FindDefinitionByIdCf` → `world/effects/runtime`, `WidePath_SetExtensionCode` → `core/text/path`, `Package_LoadEntry` → `assets/package/runtime`, `SpriteAssetRegistry_FindById` → `assets/sprite/catalog`, `SpriteAsset_RegisterAndRelocatePointers` → `assets/sprite/catalog`, `Resource_Release` → `assets/resource/runtime`
- `0x0051E440` **EffectDefinitionRegistry_FindByIdWithErrorCf**
