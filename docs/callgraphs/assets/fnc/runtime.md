# Call graph: `assets/fnc/runtime`

[Source](../../../../src/assets/fnc/runtime.c) · [Header](../../../../include/thandor/assets/fnc/runtime.h) · [Graphviz](runtime.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-assets-fnc-runtime) · [Full changelog](../../../../CHANGELOG_FULL.md#module-assets-fnc-runtime)

3 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0041A610` **[`FncModule_GetBindingModeCf`](../../../../src/assets/fnc/runtime.c#L5)**
- `0x0041A640` **[`FncModule_LoadAndRelocateCf`](../../../../src/assets/fnc/runtime.c#L19)**
- `0x0041A710` **[`FncModule_GetExportByIndexCf`](../../../../src/assets/fnc/runtime.c#L76)**

## Called by

- [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md): [`Game_LoadCoreAssets`](../../../../src/platform/bootstrap/runtime.c#L593) → [`FncModule_LoadAndRelocateCf`](../../../../src/assets/fnc/runtime.c#L19); [`Game_LoadCoreAssets`](../../../../src/platform/bootstrap/runtime.c#L593) → [`FncModule_GetExportByIndexCf`](../../../../src/assets/fnc/runtime.c#L76)
