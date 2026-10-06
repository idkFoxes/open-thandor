/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_display_mode.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Software renderer display mode: the pixel format and channel pack tables of a mode, the display mode
   hook chain and the owned memory framebuffer. */

#include <thandor/graphics/backend/software_display_mode.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_raster.h"

/* Module data. */

THANDOR_ALIGN(8) SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate = &SoftwareFramebuffer_Create;

static SoftwareDisplayModeHookProc *g_SoftwareChainedSetDisplayMode = nullptr;

SoftwarePixelPackTables *g_SoftwarePixelPackTables = nullptr;

int32_t g_SoftwareColorScaleQ16 = 0x10000;

int32_t g_SoftwareColorBiasQ16 = 0;

SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig = {};

SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode = &SoftwarePixelFormat_BaseDisplayModeHook;

SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables = &SoftwarePixelFormat_BuildChannelPackTables;

/* Initial g_GraphicsSetDisplayMode hook of the software pixel format: allocates the
   SoftwarePixelPackTables once and rebuilds them through g_SoftwareBuildPixelPackTables with the current colour
   scale/bias. The original also derived the MMX pack/unpack constants of its 16-bit rasterizer and blits from
   g_SoftwarePixelFormatConfig (gone with 16-bit colour). The mode arguments are not used. Returns true on
   success; false with the arena error in *errorCode when the allocation fails.
*/
Bool8 SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t adapterIndex,uint32_t bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  SoftwarePixelPackTables *packTables;
  uint32_t tableAllocationError;

  packTables = g_SoftwarePixelPackTables;
  if (g_SoftwarePixelPackTables == nullptr) {
    void *tableBlock;
    tableAllocationError = g_MemoryApi.alloc(sizeof(SoftwarePixelPackTables),&tableBlock); /* 3 x 256 dwords */
    if (tableAllocationError != 0) {
      *errorCode = tableAllocationError;
      return false;
    }
    packTables = static_cast<SoftwarePixelPackTables *>(tableBlock);
  }
  g_SoftwarePixelPackTables = packTables;
  g_SoftwareBuildPixelPackTables(g_SoftwareColorScaleQ16,g_SoftwareColorBiasQ16);
  return true;
}

/* g_SoftwareFramebufferCreate: creates an in-memory framebuffer of width x height pixels in one
   allocation, the SoftwareFramebufferAccess header (width, height, bytesPerPixel, pixels) followed by
   the pixels, which are zeroed (one g_MemoryApi.free releases both). Used for the off-screen buffers of the display setup (platform/input/devices.cpp).
   Returns the framebuffer, or NULL when the allocation fails; then the allocator error is stored in
   *outError (the mouse display-mode hook passes it on as its own error value).
*/
SoftwareFramebufferAccess *SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width,uint32_t *outError)

{
  SoftwareFramebufferAccess *framebuffer;
  void *frameBlock;
  uint8_t *cursor;
  uint32_t pixelBytes;
  uint32_t dwordsLeft;
  uint32_t frameAllocationError;

  pixelBytes = width * height * bytesPerPixel;
  frameAllocationError = g_MemoryApi.alloc(pixelBytes + sizeof(SoftwareFramebufferAccess),&frameBlock);
  if (frameAllocationError != 0) {
    *outError = frameAllocationError;
    return nullptr;
  }
  framebuffer = static_cast<SoftwareFramebufferAccess *>(frameBlock);
  /* the pixels follow the header */
  framebuffer->bytesPerPixel = bytesPerPixel;
  framebuffer->width = width;
  framebuffer->height = height;
  cursor = static_cast<uint8_t *>(frameBlock) + sizeof(SoftwareFramebufferAccess);
  framebuffer->pixels = cursor;
  /* whole dwords only; the original also leaves up to 3 trailing bytes as allocated */
  for (dwordsLeft = pixelBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    Thandor_StoreU32(cursor, 0);
    cursor += 4;
  }
  return framebuffer;
}

/* g_SoftwareBuildPixelPackTables: rebuilds the blue, green and red tables that turn an 8-bit
   channel into its bits of a framebuffer pixel, applying the display settings' colour scale (contrast) and bias
   (brightness), both Q16: value = 64 + (channel - 64) * scale + bias, clamped to 0..255. Called by the
   display-mode hook and by the display settings dialog (ui/dialogs/display_settings.cpp), which is why it also stores the two
   values in g_SoftwareColorScaleQ16/g_SoftwareColorBiasQ16.
*/
void SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16)

{
  uint32_t transformedChannelValueQ16;
  uint32_t channelIndex;
  SoftwarePixelPackTables *packTables;
  
  channelIndex = 0;
  packTables = g_SoftwarePixelPackTables;
  do {
    /* 0x400000 = 64.0 in Q16, the pivot of the scale */
    transformedChannelValueQ16 = (channelIndex - 64) * colorScaleQ16 + (64 << 16) + colorBiasQ16;
    if ((int)transformedChannelValueQ16 < 0) {
      transformedChannelValueQ16 = 0;
    }
    else if ((256 << 16) - 1 < transformedChannelValueQ16) {
      /* 255.0; the original clamps to 0xFF0000, not 0xFFFFFF */
      transformedChannelValueQ16 = 255 << 16;
    }
    /* keep the channel's top `bits` bits of the Q16 value (a byte in bits 16..23) and shift them into place */
    packTables->blue[channelIndex] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.blueBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.blueShift & SHIFT_COUNT_MASK);
    packTables->green[channelIndex] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.greenBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.greenShift & SHIFT_COUNT_MASK);
    packTables->red[channelIndex] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.redBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.redShift & SHIFT_COUNT_MASK);
    channelIndex++;
  } while (channelIndex < 256);
  g_SoftwareColorBiasQ16 = colorBiasQ16;
  g_SoftwareColorScaleQ16 = colorScaleQ16;
}

/* Software hook in front of g_GraphicsSetDisplayMode (see SoftwareRenderer_InstallDisplayModeHook): after the
   chained mode switch succeeds it replaces the depth buffer
   with one of the new size (the original also rebuilt the MMX colour constants of its 16-bit paths). Returns true on
   success; false with the error in *errorCode when the chained hook or the depth-buffer allocation fails.
*/
Bool8 SoftwareRenderer_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  int32_t *previousDepthBuffer;
  uint32_t depthAllocationError;
  void *depthAllocationPayload;

  if (!g_SoftwareChainedSetDisplayMode(adapterIndex,bitsPerPixel,height,width,errorCode)) {
    return false;
  }
  /* not in the original: the framebuffer size, not width x height (the SDL3 backend's GPU UI scale N makes the
     framebuffer the mode divided by N; without it they are equal) */
  (void)height;
  (void)width;
  {
    g_SoftwareDepthRowStrideBytes = g_FramebufferWidth * 4; /* one int32 depth value per pixel */
    depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * g_FramebufferHeight,
                                             &depthAllocationPayload);
    previousDepthBuffer = g_SoftwareDepthBuffer;
    if (depthAllocationError != 0) {
      *errorCode = depthAllocationError;
      return false;
    }
    {
      g_SoftwareDepthBuffer = static_cast<int32_t *>(depthAllocationPayload);
      g_MemoryApi.free(previousDepthBuffer);
      g_SoftwareDepthEpoch = 0;
    }
  }
  return true;
}

/* Hooks the software renderer into the display-mode switch: chains SoftwareRenderer_SetDisplayMode in front of
   the current g_GraphicsSetDisplayMode and allocates the
   depth buffer (one int32 per pixel) for the current framebuffer size. Returns 0 on success, or the arena error
   when the allocation fails.
*/
uint32_t SoftwareRenderer_InstallDisplayModeHook()

{
  void *allocatedDepthBuffer;
  uint32_t depthAllocationError;

  g_SoftwareChainedSetDisplayMode = g_GraphicsSetDisplayMode;
  g_SoftwareDepthRowStrideBytes = g_FramebufferWidth * 4;
  g_GraphicsSetDisplayMode = SoftwareRenderer_SetDisplayMode;
  depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * g_FramebufferHeight,
                                           &allocatedDepthBuffer);
  if (depthAllocationError == 0) {
    g_SoftwareDepthBuffer = static_cast<int32_t *>(allocatedDepthBuffer);
    g_SoftwareDepthEpoch = 0;
    return 0;
  }
  return depthAllocationError;
}
