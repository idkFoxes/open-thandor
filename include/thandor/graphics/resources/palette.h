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

/* Palette texture sources (GraphicsPaletteTextureSource_*): the palette banks start after the 0x200-byte
   header, each 256 entries of 8 bytes (colour, second dword). */
#define GRAPHICS_PALETTE_BANKS_OFFSET 0x200
#define GRAPHICS_PALETTE_BANK_BYTES 0x800
#define GRAPHICS_PALETTE_BANK_ENTRIES 0x100
/* GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices marks an entry unused by setting the low three
   bits of its blue, green and red bytes (the colours only use the upper five bits of each channel). */
#define GRAPHICS_PALETTE_ENTRY_UNUSED_MARK 0x70707
/* Entries of g_GraphicsPaletteBankSlots (used-colour count per bank); the optimiser handles at most this many banks */
#define GRAPHICS_PALETTE_BANK_SLOT_CAPACITY 0x200
/* Functions are grouped by semantic ownership. */

Bool8 GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(intptr_t textureSourceBase);

Bool8 GraphicsPaletteAsset_GetBankCount(GraphicsPaletteAsset *paletteAsset,uint32_t *outBankCount);

GraphicsPaletteAsset * GraphicsPaletteAsset_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode);

void GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset);

GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset);

void GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset);

GraphicsPaletteAsset * GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset,uint32_t *outErrorCode);

GraphicsPaletteAsset * GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset);

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

extern GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3;

extern GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage;

#endif /* THANDOR_GRAPHICS_RESOURCES_PALETTE_H */
