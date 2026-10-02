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
bool GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

/* 0x0054B0E0 */
bool DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578080 */
int __stdcall DirectDraw_EnumAdapterCallback (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName, void *applicationContext);

/* 0x005780F0 */
int32_t __stdcall DirectDraw_EnumDisplayModeCallback (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578920 */
bool GraphicsDirectDraw_ApplyDisplayModeAndCreateResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width,uint32_t *errorCode);

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H */
