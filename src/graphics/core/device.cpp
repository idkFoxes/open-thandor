/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/device.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/core/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize = nullptr;

int32_t g_GraphicsBackendAccessState = -0x1;

/* allocated by Graphics_AllocateTables but no longer read (see there) */
static GraphicsPaletteEntry *g_TexturePaletteEntries = nullptr;

/* The first step of the original's Graphics_Init, called by SdlVideo_Init: allocates and clears the texture-slot
   and palette tables and allocates the empty adapter and display-mode tables, in this order. The palette table is
   no longer read (only the original's hardware texture upload used it); it is still allocated, like the texture
   slots, so the arena layout and with it the texture-set addresses that GraphicsPrimitiveQueue_RadixSortForRendering
   sorts opaque packets by stay as they were. Returns 0 or the allocator's error code. */
uint32_t Graphics_AllocateTables()

{
  int remainingDwords;
  void *allocation;
  uint32_t *zeroCursor;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsTextureSlots = static_cast<GraphicsTextureResource **>(allocation);
  zeroCursor = static_cast<uint32_t *>(allocation);
  for (remainingDwords = GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *) / 4; remainingDwords != 0;
       remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(256 * sizeof(GraphicsPaletteEntry),&allocation); /* 256 palette entries */
  if (allocError != 0) {
    return allocError;
  }
  g_TexturePaletteEntries = static_cast<GraphicsPaletteEntry *>(allocation);
  zeroCursor = static_cast<uint32_t *>(allocation);
  for (remainingDwords = 256; remainingDwords != 0; remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(GRAPHICS_ADAPTER_CAPACITY * sizeof(GraphicsAdapterRecord),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsAdapterCount = 0;
  g_GraphicsAdapters = static_cast<GraphicsAdapterRecord *>(allocation);
  allocError = g_MemoryApi.alloc(GRAPHICS_DISPLAY_MODE_CAPACITY * sizeof(GraphicsDisplayMode),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsDisplayModeCount = 0;
  g_GraphicsDisplayModes = static_cast<GraphicsDisplayMode *>(allocation);
  return 0;
}


/* Tears the graphics backend down at exit (Runtime_Shutdown): marks the backend as not accessible (SdlVideo_Present
   then presents nothing) and frees the software cursor buffers (SdlVideo_Shutdown then releases the framebuffer
   and the window).
*/
void Graphics_Shutdown()

{
  /* nonzero: SdlVideo_Present skips the frame */
  g_GraphicsBackendAccessState = -1;
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = nullptr;
  g_CursorCompositeBuffer = nullptr;
  g_CursorAlternateSavedBackground = nullptr;
}

/* Publishes the display framebuffer of the new mode (32 bits per pixel) and installs the blitters of the 2D
   backend (graphics/core/draw2d.h; the software blitters unless the GPU records the frame).
   Called by SdlVideo_ApplyDisplayMode, which then installs its present and capture functions and the framebuffer
   pixels. The original chose the 16-bit (RGB565) blitters for a depth of 16 bits or less; 16-bit colour is gone,
   so bitsPerPixel is not looked at. */
void GraphicsDisplay_PublishFramebuffer
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)
{
  g_FramebufferWidth = width;
  g_FramebufferHeight = height;
  g_ActiveGraphicsAdapterIndex = adapterIndex;
  g_DisplayFramebufferAccess.width = width;
  g_DisplayFramebufferAccess.height = height;
  g_DisplayFramebufferAccess.pixels = nullptr;
  g_FramebufferAccess = &g_DisplayFramebufferAccess;
  (void)bitsPerPixel;
  g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
  /* the blit/fill slots of the current 2D backend (software: the software blitters themselves) */
  Draw2D_InstallSlots();
}
