/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software_display_mode.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_DISPLAY_MODE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_DISPLAY_MODE_H

#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

extern SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate;

extern SoftwarePixelPackTables *g_SoftwarePixelPackTables;

extern int32_t g_SoftwareColorScaleQ16;

extern int32_t g_SoftwareColorBiasQ16;

extern SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig;

extern SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode;

extern SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables;

bool SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t adapterIndex,uint32_t bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,uint32_t *errorCode);

SoftwareFramebufferAccess *SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width,uint32_t *outError);

void SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16);

bool SoftwareRenderer_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width,uint32_t *errorCode);

uint32_t SoftwareRenderer_InstallDisplayModeHook();

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_DISPLAY_MODE_H */
