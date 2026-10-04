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
/* Functions are grouped by semantic ownership. */

Bool8 GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

Bool8 DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex);

int __stdcall DirectDraw_EnumAdapterCallback (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName, void *applicationContext);

int32_t __stdcall DirectDraw_EnumDisplayModeCallback (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex);

void GraphicsDirectDraw_PublishFramebuffer
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width);

Bool8 GraphicsDirectDraw_ApplyDisplayModeAndCreateResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width,uint32_t *errorCode);

extern IDirectDrawSurface3 *g_PrimarySurface3;
extern DDSURFACEDESC_DX6 g_SurfaceDesc;

extern uint32_t g_ActiveGraphicsAdapterIndex; /* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
extern GraphicsDisplayMode *g_GraphicsDisplayModes;
extern GraphicsDisplayModeCount g_GraphicsDisplayModeCount;
extern GraphicsAdapterRecord *g_GraphicsAdapters;
extern uint32_t g_GraphicsAdapterCount;
extern IDirectDraw2 *g_DirectDraw2;
extern IDirectDrawSurface *g_PrimarySurfaceBase;
extern IDirectDrawSurface *g_BackSurfaceBase;

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H */
