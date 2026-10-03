/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/graphics/resources/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 00402000 g_MemoryApi */
__declspec(align(16)) MemoryApiTable g_MemoryApi = {0};

/* 0040279C g_LocaleGetPackedCurrentDate */
__declspec(align(4)) LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate = 0;

/* 004027A8 g_LocaleGetPackedCurrentTime */
__declspec(align(8)) LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime = 0;

/* 00485814 g_TextureDownsampleShift */
__declspec(align(4)) uint32_t g_TextureDownsampleShift = 0;

/* 00485834 g_GraphicsCreateTextureSet */
__declspec(align(4)) GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet = (void *)GraphicsTextureSet_AllocateMetadata;

/* 00485838 g_GraphicsDestroyTextureSet */
__declspec(align(8)) GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet = (void *)GraphicsTextureSet_FreeMetadata;

/* 0048583C g_GraphicsRefreshTextureColor */
__declspec(align(4)) GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureColor = (void *)GraphicsTextureSet_RefreshNoOp;

/* 004A8E7C g_CursorAlternateSavedBackground */
__declspec(align(4)) SoftwareFramebufferAccess *g_CursorAlternateSavedBackground = 0;

/* 004A8F08 g_GraphicsTextureSourceBlitTiledHalfSourceRgb */
__declspec(align(8)) GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb = (void *)GraphicsTextureSource_BlitTiledHalfSourceRgb;

/* 004A8F20 g_GraphicsTextureSourceBlitTiledSaturatedAddRgb: GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledSaturatedAddRgb (texture.c). */
__declspec(align(16)) GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledSaturatedAddRgb = (void *)GraphicsTextureSource_BlitTiledSaturatedAddRgb;

/* 004A8F28 g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd: GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd (texture.c). */
__declspec(align(8)) GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd = (void *)GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd;

/* 004A8F2C g_GraphicsTextureSourceDecomposeSubresourceRegionsCf: GraphicsTextureSourceDecomposeSubresourceProc * hook slot, statically GraphicsTextureSource_DecomposeSubresourceRegions (texture.c). */
__declspec(align(4)) GraphicsTextureSourceDecomposeSubresourceProc *g_GraphicsTextureSourceDecomposeSubresourceRegionsCf = (void *)GraphicsTextureSource_DecomposeSubresourceRegions;

/* 004A8F4C g_GraphicsTextureSourceConvertPaletteEntries */
__declspec(align(4)) GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries = (void *)GraphicsTextureSource_ConvertPaletteEntries;

/* 004A8F50 g_GraphicsTextureSourceResolveAllocationBase */
__declspec(align(16)) GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase = (void *)GraphicsTextureSource_ResolveAllocationBase;

/* 004A8F58 g_GraphicsPaletteAssetLifecycleCallbacks3 */
__declspec(align(8)) GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3 = {
    .releasePackage = (void *)GraphicsPaletteAsset_ReleasePackage,
    .clone = (void *)GraphicsPaletteAsset_Clone,
    .releaseClone = (void *)GraphicsPaletteAsset_ReleaseClone};

/* 004A8F64 g_GraphicsPaletteAssetValidate */
__declspec(align(4)) GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate = (void *)GraphicsPaletteAsset_Validate;

/* 004A8F68 g_GraphicsPaletteAssetResolveAllocationBase */
__declspec(align(8)) GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase = (void *)GraphicsPaletteAsset_ResolveAllocationBase;

/* 004AD930 g_GraphicsPaletteBankSlots */
__declspec(align(16)) uint32_t g_GraphicsPaletteBankSlots[512] = {0};

/* 004AE130 g_GraphicsPaletteRemapBytes */
__declspec(align(16)) uint8_t g_GraphicsPaletteRemapBytes[256] = {0};

/* 00577C30 IID_IDirect3DTexture2_Local */
__declspec(align(16)) TH_LEGACY_GUID IID_IDirect3DTexture2_Local = {.Data1 = 0x93281502, .Data2 = 36088, .Data3 = 4560, .Data4 = {137, 171, 0, 160, 201, 5, 65, 41}};

/* 00577C4C g_PrimarySurface3 */
__declspec(align(4)) IDirectDrawSurface3 *g_PrimarySurface3 = 0;

/* 00577C54 g_BackSurface3 */
__declspec(align(4)) IDirectDrawSurface3 *g_BackSurface3 = 0;

/* 00577C70 g_CurrentClearRect */
__declspec(align(16)) D3DRECT_DX6 g_CurrentClearRect = {0};

/* 00577C80 g_SurfaceDesc */
__declspec(align(16)) DDSURFACEDESC_DX6 g_SurfaceDesc = {0};

/* 00577E44 g_CursorAlternateDrawX */
__declspec(align(4)) int32_t g_CursorAlternateDrawX = 0;

/* 00577E48 g_CursorAlternateDrawY */
__declspec(align(8)) int32_t g_CursorAlternateDrawY = 0;

/* 00578058 g_TexturePaletteEntries */
__declspec(align(8)) DirectDrawPaletteEntry *g_TexturePaletteEntries = 0;

/* 00578070 g_ActiveTextureUploads */
__declspec(align(16)) uint32_t g_ActiveTextureUploads = 0;
