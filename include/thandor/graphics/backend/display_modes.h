/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/display_modes.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_DISPLAY_MODES_H
#define THANDOR_GRAPHICS_BACKEND_DISPLAY_MODES_H

#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/core/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

Bool8 GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

Bool8 DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex);

extern uint32_t g_ActiveGraphicsAdapterIndex; /* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
extern GraphicsDisplayMode *g_GraphicsDisplayModes;
extern GraphicsDisplayModeCount g_GraphicsDisplayModeCount;
extern GraphicsAdapterRecord *g_GraphicsAdapters;
extern uint32_t g_GraphicsAdapterCount;

#endif /* THANDOR_GRAPHICS_BACKEND_DISPLAY_MODES_H */
