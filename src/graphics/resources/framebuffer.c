/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/framebuffer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/framebuffer.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>

/* Implementation ownership: graphics/resources/framebuffer. */

/* Address: 0x004A9250.
   Default g_GraphicsFramebufferBeginAccess hook: an in-memory software framebuffer needs no lock, so it only
   reports success (false, CF clear). Backends with a real surface install their own hook.
*/
bool GraphicsFramebuffer_BeginAccessStub(void)

{
  return false;
}


/* Address: 0x004A9260.
   Default g_GraphicsFramebufferEndAccess hook, the counterpart of GraphicsFramebuffer_BeginAccessStub: nothing
   to unlock.
*/
void GraphicsFramebuffer_EndAccessStub(void)

{
  return;
}


/* Address: 0x005796E0.
   Shows the finished frame of g_DisplayFramebufferAccess (other framebuffers are ignored): the mouse cursor is
   drawn into the back surface, the whole back surface is blitted to the primary surface and the cursor is
   removed again. Skipped while another thread holds g_GraphicsBackendAccessState.
*/
void GraphicsFramebuffer_Present(SoftwareFramebufferAccess *framebuffer)

{
  int32_t previousAccessState;
  TH_LEGACY_HRESULT surfaceResult;
  int restoreResult;

  g_ThandorFrameHeartbeat++;
  /* XCHG in the original: take the backend lock and learn whether it was already held */
  previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
  if (previousAccessState == 0) {
    if (framebuffer == &g_DisplayFramebufferAccess) {
      /* copy the whole back surface to the primary surface */
      GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
      g_CurrentClearRect.left = 0;
      g_CurrentClearRect.top = 0;
      g_CurrentClearRect.right = g_FramebufferWidth;
      g_CurrentClearRect.bottom = g_FramebufferHeight;
      surfaceResult = g_PrimarySurface3->lpVtbl->IsLost(g_PrimarySurface3);
      restoreResult = 0;
      if (surfaceResult != 0) {
        restoreResult = g_PrimarySurface3->lpVtbl->Restore(g_PrimarySurface3);
      }
      if (restoreResult == 0) {
#ifdef THANDOR_TEST_AIDS
        if (Thandor_TestAidWindowed()) {
          /* windowed test aid (not in the original): the primary surface is the whole desktop, so blit
             into the client area; Blt (unlike BltFast) honours the window's clipper */
          TH_LEGACY_RECT windowRect;
          int clientX;
          int clientY;
          Thandor_TestAidClientOriginOnScreen(g_MainWindow,&clientX,&clientY);
          windowRect.left = clientX;
          windowRect.top = clientY;
          windowRect.right = clientX + (int)g_FramebufferWidth;
          windowRect.bottom = clientY + (int)g_FramebufferHeight;
          g_PrimarySurface3->lpVtbl->Blt
                    (g_PrimarySurface3,&windowRect,g_BackSurface3,&g_CurrentClearRect,DDBLT_WAIT,NULL);
        }
        else {
          g_PrimarySurface3->lpVtbl->BltFast
                    (g_PrimarySurface3,0,0,g_BackSurface3,&g_CurrentClearRect,DDBLTFAST_WAIT);
        }
#else
        g_PrimarySurface3->lpVtbl->BltFast
                  (g_PrimarySurface3,0,0,g_BackSurface3,&g_CurrentClearRect,DDBLTFAST_WAIT);
#endif
      }
      GraphicsCursor_RestoreAfterPresent(g_BackSurface3);
    }
    g_GraphicsBackendAccessState--;
  }
  return;
}


/* Clears a freshly allocated capture asset (dword by dword, REP STOSD in the original) and fills its 'gfx'
   header and its single source entry for a captureWidth x captureHeight ARGB8888 image. */
static void GraphicsFramebuffer_InitCaptureAsset
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


/* Restores the back surface if it was lost and locks it read-only into g_SurfaceDesc. Returns false when the
   restore or the lock fails (no lock is attempted after a failed restore). */
static bool GraphicsFramebuffer_LockBackSurfaceForCapture(void)

{
  TH_LEGACY_HRESULT isLostResult;
  int restoreResult;
  TH_LEGACY_HRESULT lockResult;

  isLostResult = g_BackSurface3->lpVtbl->IsLost(g_BackSurface3);
  restoreResult = 0;
  if (isLostResult != 0) {
    restoreResult = g_BackSurface3->lpVtbl->Restore(g_BackSurface3);
  }
  if (restoreResult != 0) {
    return false;
  }
  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  lockResult = g_BackSurface3->lpVtbl->Lock(g_BackSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT | DDLOCK_READONLY,NULL);
  return lockResult == 0;
}


/* Expands one channel of a packed pixel to 8 bits: isolates it with its mask, shifts it down to bit 0 and
   then up so its top bit lands on bit 7 (the low bits stay zero, no replication). */
static uint8_t GraphicsFramebuffer_ExpandChannelTo8Bit
          (uint32_t pixel,GraphicsPackedPixelMask channelMask,GraphicsPixelChannelBitShift channelShift,
          GraphicsPixelChannelBitCount channelBitCount)

{
  return (uint8_t)(((pixel & channelMask) >> ((uint8_t)channelShift & SHIFT_COUNT_MASK)) <<
                   (8U - (char)channelBitCount & SHIFT_COUNT_MASK));
}


/* Address: 0x005798A0.
   g_GraphicsFramebufferCaptureRegion in 16-bit modes (callers grab the whole screen): copies a rectangle of
   the back surface into a newly allocated one-image 'gfx' asset in opaque ARGB8888, expanding each channel
   with the masks and shifts of g_SoftwarePixelFormatConfig. Returns the asset, or NULL when the allocation fails or the back surface cannot be restored or locked (the
   original's error values, the arena error or FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES, were never read).
*/
GraphicsCapturedTextureSourceAsset *GraphicsFramebuffer_CaptureRegion16Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  TH_LEGACY_LPVOID lockedSurfacePixels;
  uint32_t allocationSize;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  int firstPixelOffset;
  uint8_t *sourceRow;
  uint16_t *sourcePixel;
  uint32_t *destinationPixel;
  GraphicsPixelDimension remainingColumns;
  uint32_t pixel;

  allocationSize = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  if (g_MemoryApi.alloc(allocationSize,(void **)&capturedAsset) != 0) {
    return NULL;
  }
  GraphicsFramebuffer_InitCaptureAsset(capturedAsset,allocationSize,captureWidth,captureHeight);
  if (!GraphicsFramebuffer_LockBackSurfaceForCapture()) {
    g_MemoryApi.free(capturedAsset);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,GRAPHICS_CAPTURE_FAILED_STAGE_16BIT,
                            g_PackageLastErrorPath);
    return NULL;
  }
  lockedSurfacePixels = g_SurfaceDesc.lpSurface;
  firstPixelOffset = sourceY * g_FramebufferWidth + sourceX;
  sourceRow = (uint8_t *)((uint16_t *)g_SurfaceDesc.lpSurface + firstPixelOffset);
  destinationPixel = capturedAsset->argb8888Pixels;
  /* Bottom-tested loops as in the original: a width or height of 0 would wrap the unsigned counters. */
  do {
    sourcePixel = (uint16_t *)sourceRow;
    remainingColumns = captureWidth;
    do {
      /* The original loads the pixel into AX and leaves a stale high word in EAX; the 16-bit channel masks
         remove it, so the zero-extended pixel gives the same channels. */
      pixel = *sourcePixel;
      ((uint8_t *)destinationPixel)[3] = ARGB8888_CHANNEL_MAX;
      ((uint8_t *)destinationPixel)[2] =
           GraphicsFramebuffer_ExpandChannelTo8Bit(pixel,g_SoftwarePixelFormatConfig.redMask,
                                                   g_SoftwarePixelFormatConfig.redShift,
                                                   g_SoftwarePixelFormatConfig.redBitCount);
      ((uint8_t *)destinationPixel)[1] =
           GraphicsFramebuffer_ExpandChannelTo8Bit(pixel,g_SoftwarePixelFormatConfig.greenMask,
                                                   g_SoftwarePixelFormatConfig.greenShift,
                                                   g_SoftwarePixelFormatConfig.greenBitCount);
      ((uint8_t *)destinationPixel)[0] =
           GraphicsFramebuffer_ExpandChannelTo8Bit(pixel,g_SoftwarePixelFormatConfig.blueMask,
                                                   g_SoftwarePixelFormatConfig.blueShift,
                                                   g_SoftwarePixelFormatConfig.blueBitCount);
      sourcePixel++;
      destinationPixel++;
      remainingColumns--;
    } while (remainingColumns != 0);
    sourceRow += g_SurfaceDesc.lPitch;
    captureHeight--;
  } while (captureHeight != 0);
  g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,lockedSurfacePixels);
  return capturedAsset;
}


/* Address: 0x00579B50.
   32-bit counterpart of GraphicsFramebuffer_CaptureRegion16Bit: copies a rectangle of the back surface into a
   newly allocated one-image 'gfx' asset, keeping RGB and forcing alpha to 0xFF, two pixels per step. Callers
   pass the full (even) screen width. Returns the asset, or NULL when the
   allocation fails or the back surface cannot be restored or locked.
*/
GraphicsCapturedTextureSourceAsset *GraphicsFramebuffer_CaptureRegion32Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  uint32_t allocationSize;
  TH_LEGACY_LPVOID lockedSurfacePixels;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  uint8_t *sourceRow;
  uint32_t *sourcePixel;
  uint32_t *destinationPixel;
  GraphicsPixelDimension remainingColumns;

  allocationSize = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  if (g_MemoryApi.alloc(allocationSize,(void **)&capturedAsset) != 0) {
    return NULL;
  }
  GraphicsFramebuffer_InitCaptureAsset(capturedAsset,allocationSize,captureWidth,captureHeight);
  if (!GraphicsFramebuffer_LockBackSurfaceForCapture()) {
    g_MemoryApi.free(capturedAsset);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,GRAPHICS_CAPTURE_FAILED_STAGE_32BIT,
                            g_PackageLastErrorPath);
    return NULL;
  }
  lockedSurfacePixels = g_SurfaceDesc.lpSurface;
  sourceRow = (uint8_t *)((uint32_t *)g_SurfaceDesc.lpSurface + (sourceY * g_FramebufferWidth + sourceX));
  destinationPixel = capturedAsset->argb8888Pixels;
  /* Bottom-tested loops as in the original: a height of 0 would wrap the unsigned counter. */
  do {
    sourcePixel = (uint32_t *)sourceRow;
    remainingColumns = captureWidth;
    do {
      remainingColumns -= 2;
      destinationPixel[0] = sourcePixel[0] | ARGB8888_ALPHA_MASK;
      destinationPixel[1] = sourcePixel[1] | ARGB8888_ALPHA_MASK;
      sourcePixel += 2;
      destinationPixel += 2;
    } while (1 < remainingColumns);
    /* odd width: one pixel left. A width of 1 would not stop: the unsigned count wraps below 0 and the
       pair loop runs on (as in the original, SUB EBX,2; CMP EBX,1; JA) */
    if (remainingColumns == 1) {
      *destinationPixel = *sourcePixel | ARGB8888_ALPHA_MASK;
      destinationPixel++;
    }
    sourceRow += g_SurfaceDesc.lPitch;
    captureHeight--;
  } while (captureHeight != 0);
  g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,lockedSurfacePixels);
  return capturedAsset;
}


/* Address: 0x00579D90.
   Gives the CPU direct access to the frame being drawn: restores (if lost) and locks the DirectDraw back surface
   and publishes its pixels and width in pixels in g_DisplayFramebufferAccess. Fails (CF set) when the restore or
   lock fails.
*/
bool GraphicsFramebuffer_BeginAccess(void)

{
  TH_LEGACY_HRESULT isLostResult;
  int restoreResult;
  TH_LEGACY_HRESULT lockResult;

  isLostResult = g_BackSurface3->lpVtbl->IsLost(g_BackSurface3);
  restoreResult = 0;
  if (isLostResult != 0) {
    restoreResult = g_BackSurface3->lpVtbl->Restore(g_BackSurface3);
  }
  if (restoreResult == 0) {
    Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
    g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
    lockResult = g_BackSurface3->lpVtbl->Lock
                      (g_BackSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT,NULL);
    if (lockResult == 0) {
      g_FramebufferRowStrideBytes = g_SurfaceDesc.lPitch;
      if (g_DisplayFramebufferAccess.bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
        g_DisplayFramebufferAccess.width = (uint32_t)g_SurfaceDesc.lPitch >> 1;
      }
      else {
        g_DisplayFramebufferAccess.width = (uint32_t)g_SurfaceDesc.lPitch >> 2;
      }
      g_DisplayFramebufferAccess.pixels = g_SurfaceDesc.lpSurface;
      return false;
    }
  }
  return true;
}


/* Address: 0x00579E60.
   Ends the CPU access begun by GraphicsFramebuffer_BeginAccess: unlocks the back surface and clears the
   published pixel pointer.
*/
void GraphicsFramebuffer_EndAccess(void)

{
  g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,g_DisplayFramebufferAccess.pixels);
  g_DisplayFramebufferAccess.pixels = NULL;
}

