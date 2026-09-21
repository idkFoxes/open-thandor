/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/directdraw.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H
#define THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/directdraw. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00423CF0 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsDisplayMode_IsEnumeratedCf
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

/* 0x0054B0E0 */
bool __thandor_cf_preserve_eax_ecx_edx
DisplayModeTable_ContainsExactModeCf
          (FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578080 */
int DirectDraw_EnumAdapterCallback (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName, void *applicationContext);

/* 0x005780F0 */
sdword DirectDraw_EnumDisplayModeCallback (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578920 */
DisplayModeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width);

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H */
