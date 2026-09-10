# Call graph: `graphics/resources/palette`

13 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004AE520` **GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices** — local: `GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank`, `GraphicsPaletteTextureSource_CountCombinedUsedColors`, `GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources`, `GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources`
- `0x004AD800` **GraphicsPaletteAsset_GetBankCountRegs**
- `0x004AD820` **GraphicsPaletteAsset_LoadPackage** — cross: `Package_LoadEntry` → `assets/package/runtime`, `Resource_Release` → `assets/resource/runtime`
- `0x004AD860` **GraphicsPaletteAsset_ReleasePackage** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x004AD880` **GraphicsPaletteAsset_Clone**
- `0x004AD8D0` **GraphicsPaletteAsset_ReleaseClone**
- `0x004AD8F0` **GraphicsPaletteAsset_Validate**
- `0x004AD920` **GraphicsPaletteAsset_ResolveAllocationBase**
- `0x004AE7E0` **GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets**
- `0x004AE3F0` **GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources** — local: `GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources`
- `0x004AE2E0` **GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank**
- `0x004AE370` **GraphicsPaletteTextureSource_CountCombinedUsedColors**
- `0x004AE230` **GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources**
