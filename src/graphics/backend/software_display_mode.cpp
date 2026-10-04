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

THANDOR_ALIGN(8) SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate = THANDOR_FN(SoftwareFramebuffer_Create);

static SoftwareDisplayModeHookProc *g_SoftwareChainedSetDisplayMode = 0;

SoftwarePixelPackTables *g_SoftwarePixelPackTables = 0;

int32_t g_SoftwareColorScaleQ16 = 0x10000;

int32_t g_SoftwareColorBiasQ16 = 0;

SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig = {0};

SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode = THANDOR_FN(SoftwarePixelFormat_BaseDisplayModeHook);

SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables = THANDOR_FN(SoftwarePixelFormat_BuildChannelPackTables);

/* Initial g_GraphicsSetDisplayMode hook of the software pixel format: allocates the
   SoftwarePixelPackTables once, rebuilds them through g_SoftwareBuildPixelPackTables with the current colour
   scale/bias, and derives the MMX pack/unpack constants (g_SoftwarePixelMmxConstants) of the 16-bit rasterizer
   and blits from g_SoftwarePixelFormatConfig. The mode arguments are not used. Returns true on success;
   false with the arena error in *errorCode when the allocation fails.
*/
Bool8 SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t adapterIndex,uint32_t bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  SoftwarePixelPackTables *packTables;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  uint32_t tableAllocationError;

  packTables = g_SoftwarePixelPackTables;
  if (g_SoftwarePixelPackTables == NULL) {
    tableAllocationError = g_MemoryApi.alloc(sizeof(SoftwarePixelPackTables),(void **)&packTables); /* 3 x 256 dwords */
    if (tableAllocationError != 0) {
      *errorCode = tableAllocationError;
      return false;
    }
  }
  g_SoftwarePixelPackTables = packTables;
  g_SoftwareBuildPixelPackTables(g_SoftwareColorScaleQ16,g_SoftwareColorBiasQ16);
  redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
  greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
  blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
  /* unpack scale 1 << (16 - shift - bits) moves a channel to the top of a 16-bit lane */
  blueUnpackScale = 1 << (((16 - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits) & SHIFT_COUNT_MASK);
  g_SoftwarePixelMmxConstants.packedPixelMasks.red =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.green =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
  g_SoftwarePixelMmxConstants.unpackScales.red =
       (SoftwareColorLaneFixed16)
       (1 << (((16 - (char)g_SoftwarePixelFormatConfig.redShift) - redBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.unpackScales.green =
       (SoftwareColorLaneFixed16)
       (1 << (((16 - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
  /* quantize mask: the channel's top `bits` bits of a Q12 lane */
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
       (SoftwareColorLaneFixed16)(((1 << (redBits & SHIFT_COUNT_MASK)) - 1) << ((12 - redBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
       (SoftwareColorLaneFixed16)(((1 << (greenBits & SHIFT_COUNT_MASK)) - 1) << ((12 - greenBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
       (SoftwareColorLaneFixed16)(((1 << (blueBits & SHIFT_COUNT_MASK)) - 1) << ((12 - blueBits) & SHIFT_COUNT_MASK));
  /* PMADDWD weight 1 << (bits + shift - 4) moves the quantized lane to its place in the pixel */
  g_SoftwarePixelMmxConstants.packWeights.red =
       (SoftwareColorLaneFixed16)
       (1 << ((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4 & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.packWeights.green =
       (SoftwareColorLaneFixed16)
       (1 << ((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4 & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.packWeights.blue =
       (SoftwareColorLaneFixed16)
       (1 << ((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4 & SHIFT_COUNT_MASK));
  return true;
}

/* g_SoftwareFramebufferCreate: creates an in-memory framebuffer of width x height pixels in one
   allocation, the 0x10-byte SoftwareFramebufferAccess header (width, height, bytesPerPixel, pixels) followed by
   the pixels, which are zeroed. Used for the off-screen buffers of the display setup (platform/input/devices.c).
   Returns the framebuffer, or NULL when the allocation fails; then the allocator error is stored in
   *outError (the mouse display-mode hook passes it on as its own error value).
*/
SoftwareFramebufferAccess *SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width,uint32_t *outError)

{
  SoftwareFramebufferAccess *framebuffer;
  uint32_t *cursor;
  uint32_t pixelBytes;
  uint32_t dwordsLeft;
  uint32_t frameAllocationError;

  pixelBytes = width * height * bytesPerPixel;
  frameAllocationError = g_MemoryApi.alloc(pixelBytes + sizeof(SoftwareFramebufferAccess),
                                           (void **)&framebuffer);
  if (frameAllocationError != 0) {
    *outError = frameAllocationError;
    return NULL;
  }
  /* the pixels follow the header */
  framebuffer->bytesPerPixel = bytesPerPixel;
  framebuffer->width = width;
  framebuffer->height = height;
  framebuffer->pixels = (uint8_t *)(framebuffer + 1);
  cursor = (uint32_t *)(framebuffer + 1);
  /* whole dwords only; the original also leaves up to 3 trailing bytes as allocated */
  for (dwordsLeft = pixelBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    *cursor = 0;
    cursor++;
  }
  return framebuffer;
}

/* g_SoftwareFramebufferDestroy: frees a framebuffer made by SoftwareFramebuffer_Create
   (header and pixels are one allocation).
*/
void SoftwareFramebuffer_Destroy(SoftwareFramebufferAccess *framebuffer)

{
  g_MemoryApi.free(framebuffer);
  return;
}

/* g_SoftwareBuildPixelPackTables: rebuilds the blue, green and red tables that turn an 8-bit
   channel into its bits of a framebuffer pixel, applying the display settings' colour scale (contrast) and bias
   (brightness), both Q16: value = 64 + (channel - 64) * scale + bias, clamped to 0..255. Called by the
   display-mode hook and by the display settings dialog (ui/controls/misc.c), which is why it also stores the two
   values in g_SoftwareColorScaleQ16/g_SoftwareColorBiasQ16.
*/
void SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16)

{
  uint32_t transformedChannelValueQ16;
  uint32_t channelIndex;
  SoftwarePixelPackTables *packTableCursor;
  
  channelIndex = 0;
  packTableCursor = g_SoftwarePixelPackTables;
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
    packTableCursor->blue[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.blueBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.blueShift & SHIFT_COUNT_MASK);
    packTableCursor->green[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.greenBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.greenShift & SHIFT_COUNT_MASK);
    packTableCursor->red[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.redBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.redShift & SHIFT_COUNT_MASK);
    channelIndex++;
    /* next entry of all three tables */
    packTableCursor = (SoftwarePixelPackTables *)(packTableCursor->blue + 1);
  } while (channelIndex < 256);
  g_SoftwareColorBiasQ16 = colorBiasQ16;
  g_SoftwareColorScaleQ16 = colorScaleQ16;
  return;
}

/* Software hook in front of g_GraphicsSetDisplayMode (see SoftwareRenderer_InstallDisplayModeHook): after the
   chained mode switch succeeds it picks the queue renderer for the new pixel depth, replaces the depth buffer
   with one of the new size and rebuilds the MMX colour constants from the new pixel format. Returns true on
   success; false with the error in *errorCode when the chained hook or the depth-buffer allocation fails.
*/
Bool8 SoftwareRenderer_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  int32_t *previousDepthBuffer;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  uint32_t depthAllocationError;
  void *depthAllocationPayload;

  if (!g_SoftwareChainedSetDisplayMode(adapterIndex,bitsPerPixel,height,width,errorCode)) {
    return false;
  }
  {
    g_SoftwareDepthRowStrideBytes = width * 4; /* one int32 depth value per pixel */
    if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
      g_SoftwareDrawQueue = SoftwareRenderer_DrawQueue16Bit;
    }
    else {
      g_SoftwareDrawQueue = SoftwareRenderer_DrawQueueNon16Bit;
    }
    depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * height,&depthAllocationPayload);
    previousDepthBuffer = g_SoftwareDepthBuffer;
    if (depthAllocationError != 0) {
      *errorCode = depthAllocationError;
      return false;
    }
    {
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = (int32_t *)depthAllocationPayload;
      g_MemoryApi.free(previousDepthBuffer);
      g_SoftwareDepthEpoch = 0;
      /* quantize mask: the top <bits> bits of a Q12 colour channel */
      redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
           (SoftwareColorLaneFixed16)(((1 << (redBits & SHIFT_COUNT_MASK)) - 1) << ((12 - redBits) & SHIFT_COUNT_MASK));
      greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
           (SoftwareColorLaneFixed16)(((1 << (greenBits & SHIFT_COUNT_MASK)) - 1) << ((12 - greenBits) & SHIFT_COUNT_MASK));
      blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
           (SoftwareColorLaneFixed16)(((1 << (blueBits & SHIFT_COUNT_MASK)) - 1) << ((12 - blueBits) & SHIFT_COUNT_MASK));
      /* pack weight: moves a quantized Q12 channel to its bit position in the native pixel */
      g_SoftwarePixelMmxConstants.packWeights.red =
           (SoftwareColorLaneFixed16)
           (1 << (((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packWeights.green =
           (SoftwareColorLaneFixed16)
           (1 << (((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packWeights.blue =
           (SoftwareColorLaneFixed16)
           (1 << (((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packedPixelMasks.red =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.green =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
      /* unpack scale: moves a native channel to the top of a 16-bit word lane */
      g_SoftwarePixelMmxConstants.unpackScales.red =
           (SoftwareColorLaneFixed16)
           (1 << ((16 - (char)g_SoftwarePixelFormatConfig.redShift) - redBits & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.unpackScales.green =
           (SoftwareColorLaneFixed16)
           (1 << ((16 - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits & SHIFT_COUNT_MASK));
      blueUnpackScale = 1 << ((16 - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits & SHIFT_COUNT_MASK);
      g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
    }
  }
  return true;
}

/* Hooks the software renderer into the display-mode switch: chains SoftwareRenderer_SetDisplayMode in front of
   the current g_GraphicsSetDisplayMode, picks the queue renderer for the current pixel depth and allocates the
   depth buffer (one int32 per pixel) for the current framebuffer size. Returns 0 on success, or the arena error
   when the allocation fails.
*/
uint32_t __cdecl SoftwareRenderer_InstallDisplayModeHook(void)

{
  int32_t *allocatedDepthBuffer;
  uint32_t depthAllocationError;

  g_SoftwareChainedSetDisplayMode = g_GraphicsSetDisplayMode;
  g_SoftwareDepthRowStrideBytes = g_FramebufferWidth * 4;
  LOCK();
  g_GraphicsSetDisplayMode = SoftwareRenderer_SetDisplayMode;
  UNLOCK();
  if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    g_SoftwareDrawQueue = SoftwareRenderer_DrawQueue16Bit;
  }
  else {
    g_SoftwareDrawQueue = SoftwareRenderer_DrawQueueNon16Bit;
  }
  depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * g_FramebufferHeight,
                                           (void **)&allocatedDepthBuffer);
  if (depthAllocationError == 0) {
    g_SoftwareDepthBuffer = allocatedDepthBuffer;
    g_SoftwareDepthEpoch = 0;
    return 0;
  }
  return depthAllocationError;
}
