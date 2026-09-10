# Call graph: `graphics/resources/palette`

[Source](../../../../src/graphics/resources/palette.c) · [Header](../../../../include/thandor/graphics/resources/palette.h) · [Graphviz](palette.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-graphics-resources-palette)

13 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004AE520` **[`GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices`](../../../../src/graphics/resources/palette.c#L5)** — local: [`GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank`](../../../../src/graphics/resources/palette.c#L496), [`GraphicsPaletteTextureSource_CountCombinedUsedColors`](../../../../src/graphics/resources/palette.c#L534), [`GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources`](../../../../src/graphics/resources/palette.c#L424), [`GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources`](../../../../src/graphics/resources/palette.c#L581)
- `0x004AD800` **[`GraphicsPaletteAsset_GetBankCountRegs`](../../../../src/graphics/resources/palette.c#L187)**
- `0x004AD820` **[`GraphicsPaletteAsset_LoadPackage`](../../../../src/graphics/resources/palette.c#L198)** — cross: [`Package_LoadEntry`](../../../../src/assets/package/runtime.c#L402) → [`assets/package/runtime`](../../assets/package/runtime.md), [`Resource_Release`](../../../../src/assets/resource/runtime.c#L134) → [`assets/resource/runtime`](../../assets/resource/runtime.md)
- `0x004AD860` **[`GraphicsPaletteAsset_ReleasePackage`](../../../../src/graphics/resources/palette.c#L224)** — cross: [`Resource_Release`](../../../../src/assets/resource/runtime.c#L134) → [`assets/resource/runtime`](../../assets/resource/runtime.md)
- `0x004AD880` **[`GraphicsPaletteAsset_Clone`](../../../../src/graphics/resources/palette.c#L241)**
- `0x004AD8D0` **[`GraphicsPaletteAsset_ReleaseClone`](../../../../src/graphics/resources/palette.c#L275)**
- `0x004AD8F0` **[`GraphicsPaletteAsset_Validate`](../../../../src/graphics/resources/palette.c#L291)**
- `0x004AD920` **[`GraphicsPaletteAsset_ResolveAllocationBase`](../../../../src/graphics/resources/palette.c#L305)**
- `0x004AE7E0` **[`GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets`](../../../../src/graphics/resources/palette.c#L317)**
- `0x004AE3F0` **[`GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources`](../../../../src/graphics/resources/palette.c#L424)** — local: [`GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources`](../../../../src/graphics/resources/palette.c#L581)
- `0x004AE2E0` **[`GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank`](../../../../src/graphics/resources/palette.c#L496)**
- `0x004AE370` **[`GraphicsPaletteTextureSource_CountCombinedUsedColors`](../../../../src/graphics/resources/palette.c#L534)**
- `0x004AE230` **[`GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources`](../../../../src/graphics/resources/palette.c#L581)**

## Called by

No cross-module callers are recorded.
