# Call graph: `assets/effect/catalog`

[Source](../../../../src/assets/effect/catalog.c) · [Header](../../../../include/thandor/assets/effect/catalog.h) · [Graphviz](catalog.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-assets-effect-catalog) · [Full changelog](../../../../CHANGELOG_FULL.md#module-assets-effect-catalog)

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051E0B0` **[`EffectAsset_PrepareEntries`](../../../../src/assets/effect/catalog.c#L5)** — local: [`EffectDefinition_RegisterAndLoadSprite`](../../../../src/assets/effect/catalog.c#L98); cross: [`Package_SetLastErrorPath`](../../../../src/assets/package/runtime.c#L737) → [`assets/package/runtime`](../package/runtime.md)
- `0x0051E3E0` **[`EffectDefinitions_ResolveCrossReferences`](../../../../src/assets/effect/catalog.c#L46)** — local: [`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165); cross: [`ShotDefinitionRegistry_FindByIdWithError`](../../../../src/assets/shot/catalog.c#L87) → [`assets/shot/catalog`](../shot/catalog.md)
- `0x0051DFD0` **[`EffectDefinition_RegisterAndLoadSprite`](../../../../src/assets/effect/catalog.c#L98)** — cross: [`EffectRuntime_FindDefinitionById`](../../../../src/world/effects/runtime.c#L5) → [`world/effects/runtime`](../../world/effects/runtime.md), [`WidePath_SetExtensionCode`](../../../../src/core/text/path.c#L36) → [`core/text/path`](../../core/text/path.md), [`Package_LoadEntry`](../../../../src/assets/package/runtime.c#L402) → [`assets/package/runtime`](../package/runtime.md), [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29) → [`assets/sprite/catalog`](../sprite/catalog.md), [`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50) → [`assets/sprite/catalog`](../sprite/catalog.md), [`Resource_Release`](../../../../src/assets/resource/runtime.c#L134) → [`assets/resource/runtime`](../resource/runtime.md)
- `0x0051E440` **[`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165)**

## Called by

- [`assets/model/definitions`](../model/definitions.md): [`ModelDefinition_RegisterAndResolveReferences`](../../../../src/assets/model/definitions.c#L485) → [`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165)
- [`assets/shot/catalog`](../shot/catalog.md): [`ShotDefinition_RegisterAndResolveReferences`](../../../../src/assets/shot/catalog.c#L235) → [`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165)
- [`gameplay/army/runtime`](../../gameplay/army/runtime.md): [`ArmyRuntime_ClassCommandHandlerGroupA`](../../../../src/gameplay/army/runtime.c#L1231) → [`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165)
- [`gameplay/selection/overlay`](../../gameplay/selection/overlay.md): [`InGameWorldOverlay_RebuildOrReleaseTransientMarkers`](../../../../src/gameplay/selection/overlay.c#L5) → [`EffectDefinitionRegistry_FindByIdWithError`](../../../../src/assets/effect/catalog.c#L165)
- [`gameplay/session/level`](../../gameplay/session/level.md): [`InGameLevelRuntime_LoadResourcesAfterDefaultReset`](../../../../src/gameplay/session/level.c#L42) → [`EffectAsset_PrepareEntries`](../../../../src/assets/effect/catalog.c#L5); [`InGameLevelRuntime_LoadResourcesAfterDefaultReset`](../../../../src/gameplay/session/level.c#L42) → [`EffectDefinitions_ResolveCrossReferences`](../../../../src/assets/effect/catalog.c#L46); [`InGameLevelRuntime_LoadResourcesAfterExternalTables`](../../../../src/gameplay/session/level.c#L917) → [`EffectAsset_PrepareEntries`](../../../../src/assets/effect/catalog.c#L5); [`InGameLevelRuntime_LoadResourcesAfterExternalTables`](../../../../src/gameplay/session/level.c#L917) → [`EffectDefinitions_ResolveCrossReferences`](../../../../src/assets/effect/catalog.c#L46)
