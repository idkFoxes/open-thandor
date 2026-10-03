/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/data.h
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_DATA_H
#define THANDOR_GRAPHICS_RESOURCES_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern MemoryApiTable g_MemoryApi;

extern LocaleGetPackedCurrentDateProc *g_LocaleGetPackedCurrentDate;

extern LocaleGetPackedCurrentTimeProc *g_LocaleGetPackedCurrentTime;

extern uint32_t g_TextureDownsampleShift;

extern GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet;

extern GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet;

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureColor;

extern SoftwareFramebufferAccess *g_CursorAlternateSavedBackground;

extern GraphicsTextureSourceTiledBlitProc *g_GraphicsTextureSourceBlitTiledHalfSourceRgb;

extern GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledSaturatedAddRgb; /* GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledSaturatedAddRgb (texture.c). */

extern GraphicsTextureSourceTiledSaturatedAddRgbProc *g_GraphicsTextureSourceBlitTiledHalfRgbSaturatedAdd; /* GraphicsTextureSourceTiledSaturatedAddRgbProc * hook slot, statically GraphicsTextureSource_BlitTiledHalfRgbSaturatedAdd (texture.c). */

extern GraphicsTextureSourceDecomposeSubresourceProc *g_GraphicsTextureSourceDecomposeSubresourceRegionsCf; /* GraphicsTextureSourceDecomposeSubresourceProc * hook slot, statically GraphicsTextureSource_DecomposeSubresourceRegions (texture.c). */

extern GraphicsTextureSourceConvertPaletteEntriesProc *g_GraphicsTextureSourceConvertPaletteEntries;

extern GraphicsTextureSourceResolveAllocationBaseProc *g_GraphicsTextureSourceResolveAllocationBase;

extern GraphicsPaletteAssetLifecycleCallbackTable g_GraphicsPaletteAssetLifecycleCallbacks3;

extern GraphicsPaletteAssetValidateProc *g_GraphicsPaletteAssetValidate;

extern GraphicsPaletteAssetResolveAllocationBaseProc *g_GraphicsPaletteAssetResolveAllocationBase;

extern uint32_t g_GraphicsPaletteBankSlots[512];

extern uint8_t g_GraphicsPaletteRemapBytes[256];

extern IDirectDrawSurface3 *g_PrimarySurface3;

extern IDirectDrawSurface3 *g_BackSurface3;

extern TH_LEGACY_RECT g_CurrentClearRect;

extern DDSURFACEDESC_DX6 g_SurfaceDesc;

extern DirectDrawPaletteEntry *g_TexturePaletteEntries;

#endif
