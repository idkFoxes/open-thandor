/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/palette.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_PALETTE_H
#define THANDOR_GRAPHICS_RESOURCES_PALETTE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/palette. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004AE520 */
bool __thandor_cf_preserve_ecx_edx
GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(int textureSourceBase);

/* 0x004AD800 */
void GraphicsPaletteAsset_GetBankCountRegs(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD820 */
GraphicsPaletteAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteAsset_LoadPackage(word *pathUtf16);

/* 0x004AD860 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD880 */
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD8D0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD8F0 */
GraphicsPaletteAssetEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD920 */
GraphicsPaletteAsset * __thandor_eax_preserve_ecx_edx
GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset);

/* 0x004AE7E0 */
GraphicsPaletteTextureSourceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets
          (GraphicsPaletteTextureSourceAsset *appendedAsset,
          GraphicsPaletteTextureSourceAsset *baseAsset);

/* 0x004AE3F0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
          (GraphicsPaletteIndex sourcePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource);

/* 0x004AE2E0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
          (uint oldColorIndex,uint newColorIndex,GraphicsPaletteIndex paletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource);

/* 0x004AE370 */
uint __thandor_eax_preserve_ecx_edx
GraphicsPaletteTextureSource_CountCombinedUsedColors
          (GraphicsPaletteIndex candidatePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderViewBC *textureSource);

/* 0x004AE230 */
void __thandor_void_preserve_eax_ecx
GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
          (GraphicsPaletteIndex paletteIndex,GraphicsTextureSourceHeaderViewBC *textureSource);

#endif /* THANDOR_GRAPHICS_RESOURCES_PALETTE_H */
