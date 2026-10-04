/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/framebuffer.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/framebuffer.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(8) uint32_t g_FramebufferWidth = 0;

GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion = nullptr;

SoftwareFramebufferAccess g_DisplayFramebufferAccess = {};

SoftwareFramebufferAccess *g_FramebufferAccess = nullptr;

uint32_t g_FramebufferRowStrideBytes = 0;

uint32_t g_FramebufferHeight = 0;

GraphicsFramebufferPresentProc *g_GraphicsFramebufferPresent = nullptr;

GraphicsFramebufferBeginAccessProc *g_GraphicsFramebufferBeginAccess = &GraphicsFramebuffer_BeginAccessStub;

GraphicsFramebufferEndAccessProc *g_GraphicsFramebufferEndAccess = &GraphicsFramebuffer_EndAccessStub;

GraphicsFramebufferFillRectArgbProc *g_GraphicsFramebufferFillRectArgb = nullptr;

/* g_GraphicsFramebufferBeginAccess hook: the in-memory software framebuffer (the SDL3 backend's) needs no lock,
   so it only reports success (returns false). The original locked its DirectDraw back surface here.
*/
Bool8 GraphicsFramebuffer_BeginAccessStub()

{
  return false;
}


/* Default g_GraphicsFramebufferEndAccess hook, the counterpart of GraphicsFramebuffer_BeginAccessStub: nothing
   to unlock.
*/
void GraphicsFramebuffer_EndAccessStub()

{
}


/* Clears a freshly allocated capture asset (dword by dword, as the original) and fills its 'gfx'
   header and its single source entry for a captureWidth x captureHeight ARGB8888 image. */
void GraphicsFramebuffer_InitCaptureAsset
          (GraphicsCapturedTextureSourceAsset *capturedAsset,uint32_t allocationSize,
          GraphicsPixelDimension captureWidth,GraphicsPixelDimension captureHeight)

{
  uint32_t *clearDword;
  uint32_t remainingDwords;
  uint32_t packedTimestamp;

  clearDword = (uint32_t *)capturedAsset;
  for (remainingDwords = allocationSize >> 2; remainingDwords != 0; remainingDwords--) {
    *clearDword = 0;
    clearDword++;
  }
  (capturedAsset->common).magic = ASSET_MAGIC_GFX;
  (capturedAsset->common).allocationSizeBytes = allocationSize;
  (capturedAsset->common).formatVersion = 1;
  (capturedAsset->common).converterVersion = 0;
  packedTimestamp = g_LocaleGetPackedCurrentTime();
  (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTimestamp;
  (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTimestamp;
  (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTimestamp;
  packedTimestamp = g_LocaleGetPackedCurrentDate();
  (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedTimestamp;
  (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedTimestamp;
  (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedTimestamp;
  g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.sourceName);
  capturedAsset->unusedText[0] = 0;
  (capturedAsset->tableDescriptor).subresourceCount = 1;
  (capturedAsset->tableDescriptor).paletteBankCount = 0;
  (capturedAsset->tableDescriptor).subresourceTableOffset = GRAPHICS_CAPTURE_SOURCE_ENTRY_OFFSET;
  (capturedAsset->sourceEntry).logicalWidth = captureWidth;
  (capturedAsset->sourceEntry).logicalHeight = captureHeight;
  (capturedAsset->sourceEntry).pixelWidth = captureWidth;
  (capturedAsset->sourceEntry).pixelHeight = captureHeight;
  (capturedAsset->sourceEntry).originX = 0;
  (capturedAsset->sourceEntry).originY = 0;
  (capturedAsset->sourceEntry).paletteIndex = -1;
  (capturedAsset->sourceEntry).dataOffset = GRAPHICS_CAPTURE_PIXELS_OFFSET;
}

