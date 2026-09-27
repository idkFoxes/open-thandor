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
   Ownership: graphics/resources/framebuffer.
   Purpose: Default software-framebuffer begin-access hook. It performs no work and returns success by clearing CF.
   ABI: CF clear means success. CF set means failure.
*/
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccessStub(void)

{
  return false;
}


/* Address: 0x004A9260.
   Ownership: graphics/resources/framebuffer.
   Purpose: Default software-framebuffer end-access hook. It performs no work.
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccessStub(void)

{
  return;
}


/* Address: 0x005796E0.
   Ownership: graphics/resources/framebuffer.
   Purpose: Presents the shared display framebuffer through DirectDraw or the active 3D backend. The function
   serializes access through g_GraphicsBackendAccessState and only accepts g_DisplayFramebufferAccess.
   Cross-module calls: Glide3_Framebuffer_Present [graphics/backend/glide], GraphicsCursor_ComposeBeforePresent
   [graphics/core/runtime], GraphicsCursor_RestoreAfterPresent [graphics/core/runtime].
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

  g_ThandorFrameHeartbeat = g_ThandorFrameHeartbeat + 1;
  previousAccessState = g_GraphicsBackendAccessState;
  savedCursorDrawY = g_CursorCurrentDrawY;
  savedCursorDrawX = g_CursorCurrentDrawX;
  savedVisibilityToken = g_CursorCurrentVisibilityToken;
  savedCursorBackground = g_CursorSavedBackground;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (previousAccessState == 0) {
    if (framebuffer == &g_DisplayFramebufferAccess) {
      adapterDeviceKind = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
      if (adapterDeviceKind == 1) {
        Glide3_Framebuffer_Present(&g_DisplayFramebufferAccess);
      }
      else if (adapterDeviceKind < 2) {
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        g_CurrentClearRect.x1 = 0;
        g_CurrentClearRect.y1 = 0;
        g_CurrentClearRect.x2 = g_FramebufferWidth;
        g_CurrentClearRect.y2 = g_FramebufferHeight;
        surfaceResult = (*g_PrimarySurface3->lpVtbl->IsLost)(g_PrimarySurface3);
        restoreResult = 0;
        if (surfaceResult != 0) {
          restoreResult = (*g_PrimarySurface3->lpVtbl->Restore)(g_PrimarySurface3);
        }
        if (restoreResult == 0) {
          (*g_PrimarySurface3->lpVtbl->BltFast)
                    (g_PrimarySurface3,0,0,g_BackSurface3,(TH_LEGACY_RECT *)&g_CurrentClearRect,0x10
                    );
        }
        GraphicsCursor_RestoreAfterPresent(g_BackSurface3);
      }
      else {
        LOCK();
        g_CursorSavedBackground = g_CursorAlternateSavedBackground;
        UNLOCK();
        LOCK();
        g_CursorCurrentVisibilityToken = g_CursorAlternateVisibilityToken;
        UNLOCK();
        LOCK();
        g_CursorCurrentDrawX = g_CursorAlternateDrawX;
        UNLOCK();
        LOCK();
        g_CursorCurrentDrawY = g_CursorAlternateDrawY;
        UNLOCK();
        g_CursorAlternateSavedBackground = savedCursorBackground;
        g_CursorAlternateVisibilityToken = savedVisibilityToken;
        g_CursorAlternateDrawX = savedCursorDrawX;
        g_CursorAlternateDrawY = savedCursorDrawY;
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        surfaceResult = (*g_PrimarySurface3->lpVtbl->IsLost)(g_PrimarySurface3);
        restoreResult = 0;
        if (surfaceResult != 0) {
          restoreResult = (*g_PrimarySurface3->lpVtbl->Restore)(g_PrimarySurface3);
        }
        if (restoreResult == 0) {
          surfaceResult = (*g_PrimarySurface3->lpVtbl->Flip)(g_PrimarySurface3,(IDirectDrawSurface3 *)0x0,1)
          ;
          savedCursorDrawY = g_CursorCurrentDrawY;
          savedCursorDrawX = g_CursorCurrentDrawX;
          savedVisibilityToken = g_CursorCurrentVisibilityToken;
          savedCursorBackground = g_CursorSavedBackground;
          if (surfaceResult != 0) {
            LOCK();
            g_CursorSavedBackground = g_CursorAlternateSavedBackground;
            UNLOCK();
            LOCK();
            g_CursorCurrentVisibilityToken = g_CursorAlternateVisibilityToken;
            UNLOCK();
            LOCK();
            g_CursorCurrentDrawX = g_CursorAlternateDrawX;
            UNLOCK();
            LOCK();
            g_CursorCurrentDrawY = g_CursorAlternateDrawY;
            UNLOCK();
            g_CursorAlternateSavedBackground = savedCursorBackground;
            g_CursorAlternateVisibilityToken = savedVisibilityToken;
            g_CursorAlternateDrawX = savedCursorDrawX;
            g_CursorAlternateDrawY = savedCursorDrawY;
          }
        }
      }
    }
    g_GraphicsBackendAccessState = g_GraphicsBackendAccessState + -1;
  }
  return;
}


/* Address: 0x005798A0.
   Ownership: graphics/resources/framebuffer.
   Purpose: Captures a two-byte framebuffer rectangle and expands it to opaque ARGB8888. Returns
   GraphicsCapturedTextureSourceAsset with a fixed 0x200-byte header, sourceEntry at 0x200, and argb8888Pixels at
   0x220. sourceEntry uses paletteIndex=-1, dataOffset=0x220, originX=originY=0, and logical/pixel dimensions equal
   to the capture dimensions. ABI: CF clear means success. CF set means failure.
   Cross-module calls: Glide3_Framebuffer_CaptureRegion [graphics/backend/glide], Memory_ZeroDwords
   [core/memory/allocator].
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
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    captureResult = Glide3_Framebuffer_CaptureRegion(captureHeight,captureWidth,sourceY,sourceX);
    return captureResult;
  }
  allocationSizeOrPixel = captureWidth * captureHeight * 4 + 0x220;
  allocResult = (*g_MemoryApi.alloc)(allocationSizeOrPixel);
  capturedAsset = (GraphicsCapturedTextureSourceAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    clearCursor = capturedAsset;
    for (remainingDwords = allocationSizeOrPixel >> 2; remainingDwords != 0; remainingDwords = remainingDwords - 1) {
      (clearCursor->common).magic = 0;
      clearCursor = (GraphicsCapturedTextureSourceAsset *)&(clearCursor->common).allocationSizeBytes;
    }
    (capturedAsset->common).magic = ASSET_MAGIC_GFX;
    (capturedAsset->common).allocationSizeBytes = allocationSizeOrPixel;
    (capturedAsset->common).formatVersion = 1;
    (capturedAsset->common).converterVersion = 0;
    packedTimestamp = (*g_LocaleGetPackedCurrentTime)();
    (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedTimestamp;
    packedTimestamp = (*g_LocaleGetPackedCurrentDate)();
    (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTimestamp;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.sourceName);
    capturedAsset->opaqueTablePayloadBC_1FF[0x44] = 0;
    (capturedAsset->tableDescriptor).subresourceCount = 1;
    (capturedAsset->tableDescriptor).paletteBankCount = 0;
    (capturedAsset->tableDescriptor).subresourceTableOffset = 0x200;
    (capturedAsset->sourceEntry).logicalWidth = captureWidth;
    (capturedAsset->sourceEntry).logicalHeight = captureHeight;
    (capturedAsset->sourceEntry).pixelWidth = captureWidth;
    (capturedAsset->sourceEntry).pixelHeight = captureHeight;
    (capturedAsset->sourceEntry).originX = 0;
    (capturedAsset->sourceEntry).originY = 0;
    (capturedAsset->sourceEntry).paletteIndex = -1;
    (capturedAsset->sourceEntry).dataOffset = 0x220;
    surfaceResult = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    restoreResultOrPixelOffset = 0;
    if (surfaceResult != 0) {
      restoreResultOrPixelOffset = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if (restoreResultOrPixelOffset == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      surfaceResult = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,0x11,
                         (TH_LEGACY_HANDLE)0x0);
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
            remainingColumns = remainingColumns - 1;
          } while (remainingColumns != 0);
          sourcePixel = (uint16_t *)((int)sourceRowStart + g_SurfaceDesc.lPitch);
          captureHeight = captureHeight - 1;
          remainingColumns = captureWidth;
          sourceRowStart = sourcePixel;
        } while (captureHeight != 0);
        (*g_BackSurface3->lpVtbl->Unlock)(g_BackSurface3,lockedSurfacePixels);
        return THANDOR_BITCAST(uint64_t, FramebufferCaptureResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
    (*g_MemoryApi.free)(capturedAsset);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,100,g_PackageLastErrorPath);
    capturedAsset = (GraphicsCapturedTextureSourceAsset *)&k_LowAddressLiteral0000001B;
  }
  captureResult.failed = true;
  captureResult.capture = capturedAsset;
  return captureResult;
}


/* Address: 0x00579B50.
   Ownership: graphics/resources/framebuffer.
   Purpose: Captures a four-byte framebuffer rectangle, preserves RGB, and forces alpha to 0xFF. Returns
   GraphicsCapturedTextureSourceAsset with a fixed 0x200-byte header, sourceEntry at 0x200, and argb8888Pixels at
   0x220. sourceEntry uses paletteIndex=-1, dataOffset=0x220, originX=originY=0, and logical/pixel dimensions equal
   to the capture dimensions. ABI: CF clear means success. CF set means failure.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
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
  
  allocationSizeOrPixel = captureWidth * captureHeight * 4 + 0x220;
  allocResult = (*g_MemoryApi.alloc)(allocationSizeOrPixel);
  capturedAsset = (GraphicsCapturedTextureSourceAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    clearCursor = capturedAsset;
    for (remainingDwords = allocationSizeOrPixel >> 2; remainingDwords != 0; remainingDwords = remainingDwords - 1) {
      (clearCursor->common).magic = 0;
      clearCursor = (GraphicsCapturedTextureSourceAsset *)&(clearCursor->common).allocationSizeBytes;
    }
    (capturedAsset->common).magic = ASSET_MAGIC_GFX;
    (capturedAsset->common).allocationSizeBytes = allocationSizeOrPixel;
    (capturedAsset->common).formatVersion = 1;
    (capturedAsset->common).converterVersion = 0;
    packedTimestamp = (*g_LocaleGetPackedCurrentTime)();
    (capturedAsset->common).buildMetadata.timestamps.dateValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.dateValue2 = packedTimestamp;
    packedTimestamp = (*g_LocaleGetPackedCurrentDate)();
    (capturedAsset->common).buildMetadata.timestamps.timeValue0 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue1 = packedTimestamp;
    (capturedAsset->common).buildMetadata.timestamps.timeValue2 = packedTimestamp;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((capturedAsset->common).buildMetadata.names.sourceName);
    capturedAsset->opaqueTablePayloadBC_1FF[0x44] = 0;
    (capturedAsset->tableDescriptor).subresourceCount = 1;
    (capturedAsset->tableDescriptor).paletteBankCount = 0;
    (capturedAsset->tableDescriptor).subresourceTableOffset = 0x200;
    (capturedAsset->sourceEntry).logicalWidth = captureWidth;
    (capturedAsset->sourceEntry).logicalHeight = captureHeight;
    (capturedAsset->sourceEntry).pixelWidth = captureWidth;
    (capturedAsset->sourceEntry).pixelHeight = captureHeight;
    (capturedAsset->sourceEntry).originX = 0;
    (capturedAsset->sourceEntry).originY = 0;
    (capturedAsset->sourceEntry).paletteIndex = -1;
    (capturedAsset->sourceEntry).dataOffset = 0x220;
    surfaceResult = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    restoreResult = 0;
    if (surfaceResult != 0) {
      restoreResult = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if (restoreResult == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      surfaceResult = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,0x11,
                         (TH_LEGACY_HANDLE)0x0);
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
          if (remainingColumns == 1) {
            *destinationPixel = *sourcePixel | 0xff000000;
            destinationPixel = destinationPair + 3;
          }
          sourcePixel = (uint32_t *)((int)sourceRowStart + g_SurfaceDesc.lPitch);
          captureHeight = captureHeight - 1;
          remainingColumns = captureWidth;
          sourceRowStart = sourcePixel;
        } while (captureHeight != 0);
        (*g_BackSurface3->lpVtbl->Unlock)(g_BackSurface3,lockedSurfacePixels);
        return THANDOR_BITCAST(uint64_t, FramebufferCaptureResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
    (*g_MemoryApi.free)(capturedAsset);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x65,g_PackageLastErrorPath);
    capturedAsset = (GraphicsCapturedTextureSourceAsset *)&k_LowAddressLiteral0000001B;
  }
  captureResult.failed = true;
  captureResult.capture = capturedAsset;
  return captureResult;
}


/* Address: 0x00579D90.
   Ownership: graphics/resources/framebuffer.
   Purpose: Dispatches framebuffer access by active backend. Glide3 uses Glide3_Framebuffer_BeginAccess; DirectDraw
   restores and locks the back surface, then updates g_GlideFramebufferAccess-compatible width and pixel fields.
   ABI: CF clear means success. CF set means failure.
   Cross-module calls: Glide3_Framebuffer_BeginAccess [graphics/backend/glide], Memory_ZeroDwords
   [core/memory/allocator].
*/
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccess(void)

{
  TH_LEGACY_HRESULT surfaceOperationResult;
  int surfaceRestoreResult;
  TH_LEGACY_HRESULT lockResult;
  bool glideAccessFailed;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
    glideAccessFailed = Glide3_Framebuffer_BeginAccess();
    return glideAccessFailed;
  }
  if (g_ActiveTextureUploads == 0) {
    surfaceOperationResult = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    surfaceRestoreResult = 0;
    if (surfaceOperationResult != 0) {
      surfaceRestoreResult = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if (surfaceRestoreResult == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      lockResult = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0
                        );
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
   Ownership: graphics/resources/framebuffer.
   Purpose: Dispatches framebuffer end-access by active backend. Glide3 releases its locked buffers; DirectDraw
   unlocks the back surface and clears the shared pixels pointer.
   Cross-module calls: Glide3_Framebuffer_EndAccess [graphics/backend/glide].
*/
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccess(void)

{
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
    Glide3_Framebuffer_EndAccess();
    return;
  }
  (*g_BackSurface3->lpVtbl->Unlock)(g_BackSurface3,g_DisplayFramebufferAccess.pixels);
  g_DisplayFramebufferAccess.pixels = (uint8_t *)0x0;
  return;
}

