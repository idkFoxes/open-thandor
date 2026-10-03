/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/data.h
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_DATA_H
#define THANDOR_GRAPHICS_RESOURCES_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern MemoryApiTable g_MemoryApi; /* 00402000 g_MemoryApi */

extern LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate; /* 0040279C g_LocaleGetPackedCurrentDate */

extern LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime; /* 004027A8 g_LocaleGetPackedCurrentTime */

extern uint32_t g_TextureDownsampleShift; /* 00485814 g_TextureDownsampleShift */

extern GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet; /* 00485834 g_GraphicsCreateTextureSet */

extern GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet; /* 00485838 g_GraphicsDestroyTextureSet */

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureColor; /* 0048583C g_GraphicsRefreshTextureColor */

extern SoftwareFramebufferAccess *g_CursorAlternateSavedBackground; /* 004A8E7C g_CursorAlternateSavedBackground */

extern GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb; /* 004A8F08 g_GraphicsTextureSourceBlitTiledHalfSourceRgb */

extern GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledSaturatedAddRgb; /* 004A8F20 g_GraphicsTextureSourceBlitTiledSaturatedAddRgb: GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledSaturatedAddRgb (texture.c). */

extern GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd; /* 004A8F28 g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd: GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd (texture.c). */

extern GraphicsTextureSourceDecomposeSubresourceProc *g_GraphicsTextureSourceDecomposeSubresourceRegionsCf; /* 004A8F2C g_GraphicsTextureSourceDecomposeSubresourceRegionsCf: GraphicsTextureSourceDecomposeSubresourceProc * hook slot, statically GraphicsTextureSource_DecomposeSubresourceRegions (texture.c). */

extern GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries; /* 004A8F4C g_GraphicsTextureSourceConvertPaletteEntries */

extern GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase; /* 004A8F50 g_GraphicsTextureSourceResolveAllocationBase */

extern GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3; /* 004A8F58 g_GraphicsPaletteAssetLifecycleCallbacks3 */

extern GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate; /* 004A8F64 g_GraphicsPaletteAssetValidate */

extern GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase; /* 004A8F68 g_GraphicsPaletteAssetResolveAllocationBase */

extern uint32_t g_GraphicsPaletteBankSlots[512]; /* 004AD930 g_GraphicsPaletteBankSlots */

extern uint8_t g_GraphicsPaletteRemapBytes[256]; /* 004AE130 g_GraphicsPaletteRemapBytes */

extern TH_LEGACY_GUID IID_IDirect3DTexture2_Local; /* 00577C30 IID_IDirect3DTexture2_Local */

extern IDirectDrawSurface3 *g_PrimarySurface3; /* 00577C4C g_PrimarySurface3 */

extern IDirectDrawSurface3 *g_BackSurface3; /* 00577C54 g_BackSurface3 */

extern D3DRECT_DX6 g_CurrentClearRect; /* 00577C70 g_CurrentClearRect */

extern DDSURFACEDESC_DX6 g_SurfaceDesc; /* 00577C80 g_SurfaceDesc */

extern int32_t g_CursorAlternateDrawX; /* 00577E44 g_CursorAlternateDrawX */

extern int32_t g_CursorAlternateDrawY; /* 00577E48 g_CursorAlternateDrawY */

extern DirectDrawPaletteEntry *g_TexturePaletteEntries; /* 00578058 g_TexturePaletteEntries */

extern uint32_t g_ActiveTextureUploads; /* 00578070 g_ActiveTextureUploads */

#endif
