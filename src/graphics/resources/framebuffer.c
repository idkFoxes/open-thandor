/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/framebuffer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/framebuffer.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: graphics/resources/framebuffer. */

/* Address: 0x004A9250.
   Default g_GraphicsFramebufferBeginAccess hook: an in-memory software framebuffer needs no lock, so it only
   reports success (false, CF clear). Backends with a real surface install their own hook.
*/
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccessStub(void)

{
  return false;
}


/* Address: 0x004A9260.
   Default g_GraphicsFramebufferEndAccess hook, the counterpart of GraphicsFramebuffer_BeginAccessStub: nothing
   to unlock.
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccessStub(void)

{
  return;
}


/* Address: 0x005796E0.
   Shows the finished frame of g_DisplayFramebufferAccess (other framebuffers are ignored): the Glide adapter
   presents through Glide, the software renderer blits the back surface to the primary surface, a Direct3D
   device flips. The mouse cursor is drawn into the frame just before and, for the flip chain, its saved
   state is swapped with the one of the other buffer. Skipped while another thread holds
   g_GraphicsBackendAccessState.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsFramebuffer_Present(SoftwareFramebufferAccess *framebuffer)

{
  uint32_t adapterDeviceKind;
  SoftwareFramebufferAccess *savedCursorBackground;
  int32_t savedVisibilityToken;
  int32_t savedCursorDrawX;
  int32_t savedCursorDrawY;
  int32_t previousAccessState;
  TH_LEGACY_HRESULT surfaceResult;
  int restoreResult;

  g_ThandorFrameHeartbeat++;
  /* XCHG in the original: take the backend lock and learn whether it was already held */
  previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
  if (previousAccessState == 0) {
    if (framebuffer == &g_DisplayFramebufferAccess) {
      adapterDeviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
      if (adapterDeviceKind == GRAPHICS_DEVICE_GUID_GLIDE) {
        Glide3_Framebuffer_Present(&g_DisplayFramebufferAccess);
      }
      else if (adapterDeviceKind < 2) {
        /* GRAPHICS_DEVICE_GUID_SOFTWARE: copy the whole back surface to the primary surface */
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        g_CurrentClearRect.x1 = 0;
        g_CurrentClearRect.y1 = 0;
        g_CurrentClearRect.x2 = g_FramebufferWidth;
        g_CurrentClearRect.y2 = g_FramebufferHeight;
        surfaceResult = g_PrimarySurface3->lpVtbl->IsLost(g_PrimarySurface3);
        restoreResult = 0;
        if (surfaceResult != 0) {
          restoreResult = g_PrimarySurface3->lpVtbl->Restore(g_PrimarySurface3);
        }
        if (restoreResult == 0) {
          g_PrimarySurface3->lpVtbl->BltFast
                    (g_PrimarySurface3,0,0,g_BackSurface3,(TH_LEGACY_RECT *)&g_CurrentClearRect,DDBLTFAST_WAIT);
        }
        GraphicsCursor_RestoreAfterPresent(g_BackSurface3);
      }
      else {
        /* Direct3D flip chain: the back buffer becomes visible, so swap in the cursor state saved for it
           (XCHG in the original, 0x005797C7, which reads the current state only here, under the lock) */
        savedCursorBackground = (SoftwareFramebufferAccess *)(uintptr_t)
             THANDOR_ATOMIC_EXCHANGE(&g_CursorSavedBackground,g_CursorAlternateSavedBackground);
        savedVisibilityToken =
             (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentVisibilityToken,g_CursorAlternateVisibilityToken);
        savedCursorDrawX = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawX,g_CursorAlternateDrawX);
        savedCursorDrawY = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawY,g_CursorAlternateDrawY);
        g_CursorAlternateSavedBackground = savedCursorBackground;
        g_CursorAlternateVisibilityToken = savedVisibilityToken;
        g_CursorAlternateDrawX = savedCursorDrawX;
        g_CursorAlternateDrawY = savedCursorDrawY;
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        surfaceResult = g_PrimarySurface3->lpVtbl->IsLost(g_PrimarySurface3);
        restoreResult = 0;
        if (surfaceResult != 0) {
          restoreResult = g_PrimarySurface3->lpVtbl->Restore(g_PrimarySurface3);
        }
        if (restoreResult == 0) {
          surfaceResult = g_PrimarySurface3->lpVtbl->Flip(g_PrimarySurface3,NULL,DDFLIP_WAIT);
          if (surfaceResult != 0) {
            /* the flip failed: swap the cursor state back */
            savedCursorBackground = (SoftwareFramebufferAccess *)(uintptr_t)
                 THANDOR_ATOMIC_EXCHANGE(&g_CursorSavedBackground,g_CursorAlternateSavedBackground);
            savedVisibilityToken =
                 (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentVisibilityToken,g_CursorAlternateVisibilityToken);
            savedCursorDrawX = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawX,g_CursorAlternateDrawX);
            savedCursorDrawY = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_CursorCurrentDrawY,g_CursorAlternateDrawY);
            g_CursorAlternateSavedBackground = savedCursorBackground;
            g_CursorAlternateVisibilityToken = savedVisibilityToken;
            g_CursorAlternateDrawX = savedCursorDrawX;
            g_CursorAlternateDrawY = savedCursorDrawY;
          }
        }
      }
    }
    g_GraphicsBackendAccessState--;
  }
  return;
}


/* Address: 0x005798A0.
   g_GraphicsFramebufferCaptureRegion in 16-bit modes (callers grab the whole screen): copies a rectangle of
   the back surface into a newly allocated one-image 'gfx' asset in opaque ARGB8888, expanding each channel
   with the masks and shifts of g_SoftwarePixelFormatConfig. The Glide adapter has its own capture. CF set
   with the arena error, or with FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES when the back surface cannot be
   restored or locked.
*/
FramebufferCaptureResult __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion16Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  TH_LEGACY_LPVOID lockedSurfacePixels;
  uint32_t packedTimestamp;
  TH_LEGACY_HRESULT surfaceResult;
  int restoreResultOrPixelOffset;
  uint32_t allocationSizeOrPixel;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  uint32_t remainingDwords;
  GraphicsPixelDimension remainingColumns;
  uint16_t *sourcePixel;
  GraphicsCapturedTextureSourceAsset *clearCursor;
  uint32_t *destinationPixel;
  ArenaAllocResult allocResult;
  FramebufferCaptureResult captureResult;
  uint16_t *sourceRowStart;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_GLIDE) {
    captureResult = Glide3_Framebuffer_CaptureRegion(captureHeight,captureWidth,sourceY,sourceX);
    return captureResult;
  }
  allocationSizeOrPixel = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  allocResult = g_MemoryApi.alloc(allocationSizeOrPixel);
  capturedAsset = (GraphicsCapturedTextureSourceAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    /* dword clear of the whole asset (REP STOSD in the original) */
    clearCursor = capturedAsset;
    for (remainingDwords = allocationSizeOrPixel >> 2; remainingDwords != 0; remainingDwords--) {
      (clearCursor->common).magic = 0;
      clearCursor = (GraphicsCapturedTextureSourceAsset *)&(clearCursor->common).allocationSizeBytes;
    }
    (capturedAsset->common).magic = ASSET_MAGIC_GFX;
    (capturedAsset->common).allocationSizeBytes = allocationSizeOrPixel;
    (capturedAsset->common).formatVersion = 1;
    (capturedAsset->common).converterVersion = 0;
    packedTimestamp = g_LocaleGetPackedCurrentTime();
    (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedTimestamp;
    packedTimestamp = g_LocaleGetPackedCurrentDate();
    (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTimestamp;
    g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.producerName);
    g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.sourceName);
    capturedAsset->opaqueTablePayloadBC_1FF[0x44] = 0;
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
    surfaceResult = g_BackSurface3->lpVtbl->IsLost(g_BackSurface3);
    restoreResultOrPixelOffset = 0;
    if (surfaceResult != 0) {
      restoreResultOrPixelOffset = g_BackSurface3->lpVtbl->Restore(g_BackSurface3);
    }
    if (restoreResultOrPixelOffset == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      surfaceResult = g_BackSurface3->lpVtbl->Lock
                        (g_BackSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT | DDLOCK_READONLY,NULL);
      lockedSurfacePixels = g_SurfaceDesc.lpSurface;
      if (surfaceResult == 0) {
        destinationPixel = capturedAsset->argb8888Pixels;
        restoreResultOrPixelOffset = sourceY * g_FramebufferWidth + sourceX;
        sourcePixel = (uint16_t *)((int)g_SurfaceDesc.lpSurface + restoreResultOrPixelOffset * 2);
        remainingColumns = captureWidth;
        sourceRowStart = sourcePixel;
        do {
          do {
            /* The original loads the pixel into AX; the stale high word of EAX is masked away by the 16-bit
               channel masks. */
            allocationSizeOrPixel = ((uint32_t)restoreResultOrPixelOffset & 0xffff0000) | (uint32_t)*sourcePixel;
            *(uint8_t *)((int)destinationPixel + 3) = 0xff;
            *(char *)((int)destinationPixel + 2) =
                 (char)(((allocationSizeOrPixel & g_SoftwarePixelFormatConfig.redMask) >>
                        ((uint8_t)g_SoftwarePixelFormatConfig.redShift & 0x1f)) <<
                       (8U - (char)g_SoftwarePixelFormatConfig.redBitCount & 0x1f));
            *(char *)((int)destinationPixel + 1) =
                 (char)(((allocationSizeOrPixel & g_SoftwarePixelFormatConfig.greenMask) >>
                        ((uint8_t)g_SoftwarePixelFormatConfig.greenShift & 0x1f)) <<
                       (8U - (char)g_SoftwarePixelFormatConfig.greenBitCount & 0x1f));
            restoreResultOrPixelOffset = ((allocationSizeOrPixel & g_SoftwarePixelFormatConfig.blueMask) >>
                    ((uint8_t)g_SoftwarePixelFormatConfig.blueShift & 0x1f)) <<
                    (8U - (char)g_SoftwarePixelFormatConfig.blueBitCount & 0x1f);
            *(char *)destinationPixel = (char)restoreResultOrPixelOffset;
            sourcePixel = sourcePixel + 1;
            destinationPixel = destinationPixel + 1;
            remainingColumns--;
          } while (remainingColumns != 0);
          sourcePixel = (uint16_t *)((int)sourceRowStart + g_SurfaceDesc.lPitch);
          captureHeight--;
          remainingColumns = captureWidth;
          sourceRowStart = sourcePixel;
        } while (captureHeight != 0);
        g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,lockedSurfacePixels);
        return THANDOR_BITCAST(uint64_t, FramebufferCaptureResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
    g_MemoryApi.free(capturedAsset);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,GRAPHICS_CAPTURE_FAILED_STAGE_16BIT,
                            g_PackageLastErrorPath);
    capturedAsset = (GraphicsCapturedTextureSourceAsset *)FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
  }
  captureResult.failed = true;
  captureResult.capture = capturedAsset;
  return captureResult;
}


/* Address: 0x00579B50.
   32-bit counterpart of GraphicsFramebuffer_CaptureRegion16Bit: copies a rectangle of the back surface into a
   newly allocated one-image 'gfx' asset, keeping RGB and forcing alpha to 0xFF, two pixels per step. There is
   no Glide branch here. Callers pass the full (even) screen width. CF set with the arena error or
   FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES.
*/
FramebufferCaptureResult __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion32Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  uint32_t allocationSizeOrPixel;
  TH_LEGACY_LPVOID lockedSurfacePixels;
  uint32_t packedTimestamp;
  TH_LEGACY_HRESULT surfaceResult;
  int restoreResult;
  GraphicsCapturedTextureSourceAsset *capturedAsset;
  uint32_t remainingDwords;
  GraphicsPixelDimension remainingColumns;
  uint32_t *sourcePixel;
  GraphicsCapturedTextureSourceAsset *clearCursor;
  uint32_t *destinationPair;
  uint32_t *destinationPixel;
  ArenaAllocResult allocResult;
  FramebufferCaptureResult captureResult;
  uint32_t *sourceRowStart;
  
  allocationSizeOrPixel = captureWidth * captureHeight * 4 + GRAPHICS_CAPTURE_PIXELS_OFFSET;
  allocResult = g_MemoryApi.alloc(allocationSizeOrPixel);
  capturedAsset = (GraphicsCapturedTextureSourceAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    /* dword clear of the whole asset (REP STOSD in the original) */
    clearCursor = capturedAsset;
    for (remainingDwords = allocationSizeOrPixel >> 2; remainingDwords != 0; remainingDwords--) {
      (clearCursor->common).magic = 0;
      clearCursor = (GraphicsCapturedTextureSourceAsset *)&(clearCursor->common).allocationSizeBytes;
    }
    (capturedAsset->common).magic = ASSET_MAGIC_GFX;
    (capturedAsset->common).allocationSizeBytes = allocationSizeOrPixel;
    (capturedAsset->common).formatVersion = 1;
    (capturedAsset->common).converterVersion = 0;
    packedTimestamp = g_LocaleGetPackedCurrentTime();
    (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedTimestamp;
    packedTimestamp = g_LocaleGetPackedCurrentDate();
    (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTimestamp;
    g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.producerName);
    g_LocaleCopyDefaultComputerLabelUtf16((capturedAsset->common).buildMetadata.names.sourceName);
    capturedAsset->opaqueTablePayloadBC_1FF[0x44] = 0;
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
    surfaceResult = g_BackSurface3->lpVtbl->IsLost(g_BackSurface3);
    restoreResult = 0;
    if (surfaceResult != 0) {
      restoreResult = g_BackSurface3->lpVtbl->Restore(g_BackSurface3);
    }
    if (restoreResult == 0) {
      Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
      surfaceResult = g_BackSurface3->lpVtbl->Lock
                        (g_BackSurface3,NULL,&g_SurfaceDesc,DDLOCK_WAIT | DDLOCK_READONLY,NULL);
      lockedSurfacePixels = g_SurfaceDesc.lpSurface;
      if (surfaceResult == 0) {
        sourcePixel = (uint32_t *)((int)g_SurfaceDesc.lpSurface +
                         (sourceY * g_FramebufferWidth + sourceX) * 4);
        remainingColumns = captureWidth;
        destinationPixel = capturedAsset->argb8888Pixels;
        sourceRowStart = sourcePixel;
        do {
          do {
            destinationPair = destinationPixel;
            allocationSizeOrPixel = sourcePixel[1];
            remainingColumns = remainingColumns - 2;
            *destinationPair = *sourcePixel | 0xff000000;
            destinationPair[1] = allocationSizeOrPixel | 0xff000000;
            sourcePixel = sourcePixel + 2;
            destinationPixel = destinationPair + 2;
          } while (1 < remainingColumns);
          /* odd width: one pixel left. A width of 1 would not stop: the unsigned count wraps below 0 and the
             pair loop runs on (as in the original, SUB EBX,2; CMP EBX,1; JA) */
          if (remainingColumns == 1) {
            *destinationPixel = *sourcePixel | 0xff000000;
            destinationPixel = destinationPair + 3;
          }
          sourcePixel = (uint32_t *)((int)sourceRowStart + g_SurfaceDesc.lPitch);
          captureHeight--;
          remainingColumns = captureWidth;
          sourceRowStart = sourcePixel;
        } while (captureHeight != 0);
        g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,lockedSurfacePixels);
        return THANDOR_BITCAST(uint64_t, FramebufferCaptureResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
    g_MemoryApi.free(capturedAsset);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,GRAPHICS_CAPTURE_FAILED_STAGE_32BIT,
                            g_PackageLastErrorPath);
    capturedAsset = (GraphicsCapturedTextureSourceAsset *)FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
  }
  captureResult.failed = true;
  captureResult.capture = capturedAsset;
  return captureResult;
}


/* Address: 0x00579D90.
   Gives the CPU direct access to the frame being drawn: on the Glide adapter via
   Glide3_Framebuffer_BeginAccess, otherwise by restoring (if lost) and locking the DirectDraw back surface
   and publishing its pixels and width in pixels in g_DisplayFramebufferAccess. Fails (CF set) while
   texture uploads are active or when the restore or lock fails.
*/
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccess(void)

{
  TH_LEGACY_HRESULT isLostResult;
  int restoreResult;
  TH_LEGACY_HRESULT lockResult;

  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE) {
    return Glide3_Framebuffer_BeginAccess();
  }
  if (g_ActiveTextureUploads == 0) {
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
  }
  return true;
}


/* Address: 0x00579E60.
   Ends the CPU access begun by GraphicsFramebuffer_BeginAccess: Glide releases its locked buffers,
   DirectDraw unlocks the back surface and clears the published pixel pointer.
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccess(void)

{
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE) {
    Glide3_Framebuffer_EndAccess();
    return;
  }
  g_BackSurface3->lpVtbl->Unlock(g_BackSurface3,g_DisplayFramebufferAccess.pixels);
  g_DisplayFramebufferAccess.pixels = NULL;
}

