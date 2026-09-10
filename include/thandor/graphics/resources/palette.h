#ifndef THANDOR_GRAPHICS_RESOURCES_PALETTE_H
#define THANDOR_GRAPHICS_RESOURCES_PALETTE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/palette. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004AE520 */
void GraphicsPaletteTextureSource_OptimizePaletteBanksAndRemapIndices(int param_1);

/* 0x004AD800 */
void GraphicsPaletteAsset_GetBankCountRegs(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD820 */
GraphicsPaletteAsset * __fastcall GraphicsPaletteAsset_LoadPackage(dword packageContext0,dword packageContext1,word *pathUtf16);

/* 0x004AD860 */
void GraphicsPaletteAsset_ReleasePackage(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD880 */
GraphicsPaletteAsset * GraphicsPaletteAsset_Clone(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD8D0 */
void GraphicsPaletteAsset_ReleaseClone(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD8F0 */
GraphicsPaletteAsset * GraphicsPaletteAsset_Validate(GraphicsPaletteAsset *paletteAsset);

/* 0x004AD920 */
GraphicsPaletteAsset * GraphicsPaletteAsset_ResolveAllocationBase(GraphicsPaletteAsset *paletteAsset);

/* 0x004AE7E0 */
undefined8 GraphicsPaletteTextureSource_CombineAssetsAndRebaseOffsets(int param_1,undefined4 *param_2);

/* 0x004AE3F0 */
void __fastcall GraphicsPaletteTextureSource_MergePaletteBankAndRemapSubresources (undefined4 param_1,undefined4 param_2,int param_3,int param_4,int param_5);

/* 0x004AE2E0 */
undefined8 GraphicsPaletteTextureSource_RemapColorIndexForPaletteBank (int param_1,int param_2,int param_3,int param_4);

/* 0x004AE370 */
undefined8 GraphicsPaletteTextureSource_CountCombinedUsedColors(int param_1,int param_2,int param_3);

/* 0x004AE230 */
undefined4 GraphicsPaletteTextureSource_RemovePaletteBankAndRebaseSubresources(int param_1,int param_2);

#endif /* THANDOR_GRAPHICS_RESOURCES_PALETTE_H */
