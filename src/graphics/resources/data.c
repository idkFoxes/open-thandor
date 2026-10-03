/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/graphics/resources/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) MemoryApiTable g_MemoryApi = {0};

__declspec(align(4)) LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate = 0;

__declspec(align(8)) LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime = 0;

__declspec(align(4)) uint32_t g_TextureDownsampleShift = 0;

__declspec(align(4)) GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet = (void *)GraphicsTextureSet_AllocateMetadata;

__declspec(align(8)) GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet = (void *)GraphicsTextureSet_FreeMetadata;

__declspec(align(4)) GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureColor = (void *)GraphicsTextureSet_RefreshNoOp;

__declspec(align(4)) SoftwareFramebufferAccess *g_CursorAlternateSavedBackground = 0;

__declspec(align(8)) GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb = (void *)GraphicsTextureSource_BlitTiledHalfSourceRgb;

/* GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledSaturatedAddRgb (texture.c). */
__declspec(align(16)) GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledSaturatedAddRgb = (void *)GraphicsTextureSource_BlitTiledSaturatedAddRgb;

/* GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd (texture.c). */
__declspec(align(8)) GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd = (void *)GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd;

/* GraphicsTextureSourceDecomposeSubresourceProc * hook slot, statically GraphicsTextureSource_DecomposeSubresourceRegions (texture.c). */
__declspec(align(4)) GraphicsTextureSourceDecomposeSubresourceProc *g_GraphicsTextureSourceDecomposeSubresourceRegionsCf = (void *)GraphicsTextureSource_DecomposeSubresourceRegions;

__declspec(align(4)) GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries = (void *)GraphicsTextureSource_ConvertPaletteEntries;

__declspec(align(16)) GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase = (void *)GraphicsTextureSource_ResolveAllocationBase;

__declspec(align(8)) GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3 = {
    .releasePackage = (void *)GraphicsPaletteAsset_ReleasePackage,
    .clone = (void *)GraphicsPaletteAsset_Clone,
    .releaseClone = (void *)GraphicsPaletteAsset_ReleaseClone};

__declspec(align(4)) GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate = (void *)GraphicsPaletteAsset_Validate;

__declspec(align(8)) GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase = (void *)GraphicsPaletteAsset_ResolveAllocationBase;

__declspec(align(16)) uint32_t g_GraphicsPaletteBankSlots[512] = {0};

__declspec(align(16)) uint8_t g_GraphicsPaletteRemapBytes[256] = {0};

__declspec(align(4)) IDirectDrawSurface3 *g_PrimarySurface3 = 0;

__declspec(align(4)) IDirectDrawSurface3 *g_BackSurface3 = 0;

/* the source rectangle of the present blit (GraphicsFramebuffer_Present) */
__declspec(align(16)) TH_LEGACY_RECT g_CurrentClearRect = {0};

__declspec(align(16)) DDSURFACEDESC_DX6 g_SurfaceDesc = {0};

/* allocated by Graphics_Init but no longer read (see there) */
__declspec(align(8)) DirectDrawPaletteEntry *g_TexturePaletteEntries = 0;
