/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/palette_optimizer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_PALETTE_OPTIMIZER_H
#define THANDOR_GRAPHICS_RESOURCES_PALETTE_OPTIMIZER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

Bool8 GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(intptr_t textureSourceBase);

Bool8 GraphicsPaletteAsset_GetBankCount(GraphicsPaletteAsset *paletteAsset,uint32_t *outBankCount);

GraphicsPaletteTextureSourceAsset * GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets
          (GraphicsPaletteTextureSourceAsset *appendedAsset,
          GraphicsPaletteTextureSourceAsset *baseAsset);

void GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources
          (GraphicsPaletteIndex sourcePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderView *textureSource);

void GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank
          (uint32_t oldColorIndex,uint32_t newColorIndex,GraphicsPaletteIndex paletteBank,
          GraphicsTextureSourceHeaderView *textureSource);

uint32_t GraphicsPaletteTextureSource_CountCombinedUsedColors
          (GraphicsPaletteIndex candidatePaletteBank,GraphicsPaletteIndex destinationPaletteBank,
          GraphicsTextureSourceHeaderView *textureSource);

void GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources
          (GraphicsPaletteIndex paletteIndex,GraphicsTextureSourceHeaderView *textureSource);

#endif /* THANDOR_GRAPHICS_RESOURCES_PALETTE_OPTIMIZER_H */
