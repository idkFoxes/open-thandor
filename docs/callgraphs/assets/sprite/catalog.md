# Call graph: `assets/sprite/catalog`

[Source](../../../../src/assets/sprite/catalog.c) · [Header](../../../../include/thandor/assets/sprite/catalog.h) · [Graphviz](catalog.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-assets-sprite-catalog)

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00486D60` **[`SpriteAsset_ValidateGroupCountCf`](../../../../src/assets/sprite/catalog.c#L5)**
- `0x004BE480` **[`SpriteAssetRegistry_Reset`](../../../../src/assets/sprite/catalog.c#L18)**
- `0x004BE490` **[`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29)**
- `0x004BE4D0` **[`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50)**

## Called by

- [`assets/effect/catalog`](../effect/catalog.md): [`EffectDefinition_RegisterAndLoadSpriteCf`](../../../../src/assets/effect/catalog.c#L98) → [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29); [`EffectDefinition_RegisterAndLoadSpriteCf`](../../../../src/assets/effect/catalog.c#L98) → [`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50)
- [`assets/model/definitions`](../model/definitions.md): [`ModelDefinition_RegisterAndResolveReferencesCf`](../../../../src/assets/model/definitions.c#L485) → [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29); [`ModelDefinition_RegisterAndResolveReferencesCf`](../../../../src/assets/model/definitions.c#L485) → [`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50)
- [`assets/rom/runtime`](../rom/runtime.md): [`RomAssetRecord_RegisterAndRelocate`](../../../../src/assets/rom/runtime.c#L555) → [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29); [`RomAssetRecord_RegisterAndRelocate`](../../../../src/assets/rom/runtime.c#L555) → [`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50)
- [`assets/shot/catalog`](../shot/catalog.md): [`ShotDefinition_RegisterAndResolveReferencesCf`](../../../../src/assets/shot/catalog.c#L235) → [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29); [`ShotDefinition_RegisterAndResolveReferencesCf`](../../../../src/assets/shot/catalog.c#L235) → [`SpriteAsset_RegisterAndRelocatePointers`](../../../../src/assets/sprite/catalog.c#L50)
- [`gameplay/session/runtime`](../../gameplay/session/runtime.md): [`InGameConditionRuntime_RebaseLoadedRecords`](../../../../src/gameplay/session/runtime.c#L450) → [`SpriteAssetRegistry_FindById`](../../../../src/assets/sprite/catalog.c#L29); [`InGameRuntime_ShutdownAndReleaseResources`](../../../../src/gameplay/session/runtime.c#L1444) → [`SpriteAssetRegistry_Reset`](../../../../src/assets/sprite/catalog.c#L18)
- [`ui/frontend/runtime`](../../ui/frontend/runtime.md): [`FrontendRuntime_ShutdownAndReleaseResourcesRegs`](../../../../src/ui/frontend/runtime.c#L3522) → [`SpriteAssetRegistry_Reset`](../../../../src/assets/sprite/catalog.c#L18)
