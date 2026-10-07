/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_CORE_RUNTIME_H
#define THANDOR_GRAPHICS_CORE_RUNTIME_H

#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Capacity of g_GraphicsAdapters (Graphics_AllocateTables allocates 16 records of 0x80 bytes) and of
   g_GraphicsDisplayModes (the mode list stops at 256 modes). */
inline constexpr int GRAPHICS_ADAPTER_CAPACITY = 16;
inline constexpr int GRAPHICS_DISPLAY_MODE_CAPACITY = 256;
/* g_ActiveGraphicsAdapterIndex before the first display mode is set (and while the backend is being recreated). */
inline constexpr int GRAPHICS_ADAPTER_INDEX_NONE = -1;

/* Pixel formats shared by the graphics backends, textures and movies. ARGB8888 is the engine's 32-bit colour
   (gfx assets, palette entries, vertex colours): alpha in the top byte, blue in the low byte. */
inline constexpr uint32_t ARGB8888_ALPHA_MASK = 0xff000000;
inline constexpr int ARGB8888_RED_MASK = 0xff0000;
inline constexpr int ARGB8888_GREEN_MASK = 0xff00;
inline constexpr int ARGB8888_BLUE_MASK = 0xff;
inline constexpr int ARGB8888_RGB_MASK = 0xffffff;
inline constexpr uint32_t ARGB8888_OPAQUE_WHITE = 0xffffffff;
inline constexpr int ARGB8888_CHANNEL_MASK = 0xff; /* one channel shifted down to bit 0 */
inline constexpr int ARGB8888_CHANNEL_MAX = 0xff; /* one 8-bit channel at full intensity */
inline constexpr int ARGB8888_CHANNEL_ONES = 0x1010101; /* 1 in each of the four byte channels */
inline constexpr int ARGB8888_ALPHA_ONE = 0x1000000; /* alpha 1, the lowest alpha step */
/* c * 0x101 = c | c << 8: an 8-bit channel widened to a 16-bit lane (PUNPCKLBW of a value with itself) */
inline constexpr int COLOR_CHANNEL_TO_WORD_LANE = 0x101;
/* x86 shifts use only the low 5 bits of the count. The original masks some counts explicitly; the C keeps the
   mask where dropping it changes the generated code. */
inline constexpr int SHIFT_COUNT_MASK = 0x1f;
/* A dword count derived from its byte count ((count * 4) >> 2, as the original's dword fill/copy loops count): the
   top two bits drop */
inline constexpr int DWORD_COUNT_MASK = 0x3fffffff;

uint32_t Graphics_AllocateTables();

void Graphics_Shutdown();

extern SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize;
extern int32_t g_GraphicsBackendAccessState;

void GraphicsDisplay_PublishFramebuffer
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width);

#endif /* THANDOR_GRAPHICS_CORE_RUNTIME_H */
