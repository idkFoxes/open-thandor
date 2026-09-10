#ifndef THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H
#define THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/directdraw. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00423CF0 */
undefined8 GraphicsDisplayMode_IsEnumeratedCf (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel, FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

/* 0x0054B0E0 */
undefined8 DisplayModeTable_ContainsExactModeCf (FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height, FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578080 */
int DirectDraw_EnumAdapterCallback (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName, void *applicationContext);

/* 0x005780F0 */
sdword DirectDraw_EnumDisplayModeCallback (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex);

/* 0x00578920 */
void GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel, GraphicsPixelDimension height,GraphicsPixelDimension width);

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECTDRAW_H */
