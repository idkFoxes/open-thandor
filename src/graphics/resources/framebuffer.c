/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/resources/framebuffer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/resources/framebuffer.h>

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
  uint uVar1;
  SoftwareFramebufferAccess *pSVar2;
  sdword sVar3;
  sdword sVar4;
  sdword sVar5;
  sdword sVar6;
  TH_LEGACY_HRESULT TVar7;
  int iVar8;
  
  sVar6 = g_GraphicsBackendAccessState;
  sVar5 = g_CursorCurrentDrawY;
  sVar4 = g_CursorCurrentDrawX;
  sVar3 = g_CursorCurrentVisibilityToken;
  pSVar2 = g_CursorSavedBackground;
  LOCK();
  g_GraphicsBackendAccessState = 1;
  UNLOCK();
  if (sVar6 == 0) {
    if (framebuffer == &g_DisplayFramebufferAccess) {
      uVar1 = g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1;
      if (uVar1 == 1) {
        Glide3_Framebuffer_Present(&g_DisplayFramebufferAccess);
      }
      else if (uVar1 < 2) {
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        g_CurrentClearRect.x1 = 0;
        g_CurrentClearRect.y1 = 0;
        g_CurrentClearRect.x2 = g_FramebufferWidth;
        g_CurrentClearRect.y2 = g_FramebufferHeight;
        TVar7 = (*g_PrimarySurface3->lpVtbl->IsLost)(g_PrimarySurface3);
        iVar8 = 0;
        if (TVar7 != 0) {
          iVar8 = (*g_PrimarySurface3->lpVtbl->Restore)(g_PrimarySurface3);
        }
        if (iVar8 == 0) {
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
        g_CursorAlternateSavedBackground = pSVar2;
        g_CursorAlternateVisibilityToken = sVar3;
        g_CursorAlternateDrawX = sVar4;
        g_CursorAlternateDrawY = sVar5;
        GraphicsCursor_ComposeBeforePresent(g_BackSurface3);
        TVar7 = (*g_PrimarySurface3->lpVtbl->IsLost)(g_PrimarySurface3);
        iVar8 = 0;
        if (TVar7 != 0) {
          iVar8 = (*g_PrimarySurface3->lpVtbl->Restore)(g_PrimarySurface3);
        }
        if (iVar8 == 0) {
          TVar7 = (*g_PrimarySurface3->lpVtbl->Flip)(g_PrimarySurface3,(IDirectDrawSurface3 *)0x0,1)
          ;
          sVar5 = g_CursorCurrentDrawY;
          sVar4 = g_CursorCurrentDrawX;
          sVar3 = g_CursorCurrentVisibilityToken;
          pSVar2 = g_CursorSavedBackground;
          if (TVar7 != 0) {
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
            g_CursorAlternateSavedBackground = pSVar2;
            g_CursorAlternateVisibilityToken = sVar3;
            g_CursorAlternateDrawX = sVar4;
            g_CursorAlternateDrawY = sVar5;
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
GraphicsFramebufferCaptureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion16Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  TH_LEGACY_LPVOID arg1;
  dword dVar1;
  TH_LEGACY_HRESULT TVar2;
  int iVar3;
  uint uVar4;
  GraphicsCapturedTextureSourceAsset *memory;
  uint uVar5;
  GraphicsPixelDimension GVar6;
  undefined2 *puVar7;
  GraphicsCapturedTextureSourceAsset *pGVar8;
  dword *pdVar9;
  ArenaAllocEaxCf5 AVar10;
  GraphicsFramebufferCaptureEaxCf5 GVar11;
  undefined2 *puVar12;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].deviceGuid.Data1 == 1) {
    GVar11 = Glide3_Framebuffer_CaptureRegion(captureHeight,captureWidth,sourceY,sourceX);
    return GVar11;
  }
  uVar4 = captureWidth * captureHeight * 4 + 0x220;
  AVar10 = (*g_MemoryApi.alloc)(uVar4);
  memory = (GraphicsCapturedTextureSourceAsset *)AVar10.eax;
  if (!AVar10.carry) {
    pGVar8 = memory;
    for (uVar5 = uVar4 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      (pGVar8->common).magic = 0;
      pGVar8 = (GraphicsCapturedTextureSourceAsset *)&(pGVar8->common).allocationSizeBytes;
    }
    (memory->common).magic = ASSET_MAGIC_GFX;
    (memory->common).allocationSizeBytes = uVar4;
    (memory->common).formatVersion = 1;
    (memory->common).converterVersion = 0;
    dVar1 = (*g_LocaleGetPackedCurrentTime)();
    (memory->common).buildMetadata.timestamps.dateValue0 = dVar1;
    (memory->common).buildMetadata.timestamps.dateValue1 = dVar1;
    (memory->common).buildMetadata.timestamps.dateValue2 = dVar1;
    dVar1 = (*g_LocaleGetPackedCurrentDate)();
    (memory->common).buildMetadata.timestamps.timeValue0 = dVar1;
    (memory->common).buildMetadata.timestamps.timeValue1 = dVar1;
    (memory->common).buildMetadata.timestamps.timeValue2 = dVar1;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((memory->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((memory->common).buildMetadata.names.sourceName);
    memory->opaqueTablePayloadBC_1FF[0x44] = 0;
    (memory->tableDescriptor).subresourceCount = 1;
    (memory->tableDescriptor).paletteBankCount = 0;
    (memory->tableDescriptor).subresourceTableOffset = 0x200;
    (memory->sourceEntry).logicalWidth = captureWidth;
    (memory->sourceEntry).logicalHeight = captureHeight;
    (memory->sourceEntry).pixelWidth = captureWidth;
    (memory->sourceEntry).pixelHeight = captureHeight;
    (memory->sourceEntry).originX = 0;
    (memory->sourceEntry).originY = 0;
    (memory->sourceEntry).paletteIndex = -1;
    (memory->sourceEntry).dataOffset = 0x220;
    TVar2 = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    iVar3 = 0;
    if (TVar2 != 0) {
      iVar3 = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if (iVar3 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar2 = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,0x11,
                         (TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      if (TVar2 == 0) {
        pdVar9 = memory->argb8888Pixels;
        iVar3 = sourceY * g_FramebufferWidth + sourceX;
        puVar7 = (undefined2 *)((int)g_SurfaceDesc.lpSurface + iVar3 * 2);
        GVar6 = captureWidth;
        puVar12 = puVar7;
        do {
          do {
            uVar4 = CONCAT22((short)((uint)iVar3 >> 0x10),*puVar7);
            *(undefined1 *)((int)pdVar9 + 3) = 0xff;
            *(char *)((int)pdVar9 + 2) =
                 (char)(((uVar4 & g_SoftwarePixelFormatConfig.redMask) >>
                        ((byte)g_SoftwarePixelFormatConfig.redShift & 0x1f)) <<
                       (8U - (char)g_SoftwarePixelFormatConfig.redBitCount & 0x1f));
            *(char *)((int)pdVar9 + 1) =
                 (char)(((uVar4 & g_SoftwarePixelFormatConfig.greenMask) >>
                        ((byte)g_SoftwarePixelFormatConfig.greenShift & 0x1f)) <<
                       (8U - (char)g_SoftwarePixelFormatConfig.greenBitCount & 0x1f));
            iVar3 = ((uVar4 & g_SoftwarePixelFormatConfig.blueMask) >>
                    ((byte)g_SoftwarePixelFormatConfig.blueShift & 0x1f)) <<
                    (8U - (char)g_SoftwarePixelFormatConfig.blueBitCount & 0x1f);
            *(char *)pdVar9 = (char)iVar3;
            puVar7 = puVar7 + 1;
            pdVar9 = pdVar9 + 1;
            GVar6 = GVar6 - 1;
          } while (GVar6 != 0);
          puVar7 = (undefined2 *)((int)puVar12 + g_SurfaceDesc.lPitch);
          captureHeight = captureHeight - 1;
          GVar6 = captureWidth;
          puVar12 = puVar7;
        } while (captureHeight != 0);
        (*g_BackSurface3->lpVtbl->Unlock)(g_BackSurface3,arg1);
        return (GraphicsFramebufferCaptureEaxCf5)((uint5)AVar10 & 0xffffffff);
      }
    }
    (*g_MemoryApi.free)(memory);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,100,g_PackageLastErrorPath);
    memory = (GraphicsCapturedTextureSourceAsset *)&k_LowAddressLiteral0000001B;
  }
  GVar11.carry = true;
  GVar11.eax = memory;
  return GVar11;
}


/* Address: 0x00579B50.
   Ownership: graphics/resources/framebuffer.
   Purpose: Captures a four-byte framebuffer rectangle, preserves RGB, and forces alpha to 0xFF. Returns
   GraphicsCapturedTextureSourceAsset with a fixed 0x200-byte header, sourceEntry at 0x200, and argb8888Pixels at
   0x220. sourceEntry uses paletteIndex=-1, dataOffset=0x220, originX=originY=0, and logical/pixel dimensions equal
   to the capture dimensions. ABI: CF clear means success. CF set means failure.
   Cross-module calls: Memory_ZeroDwords [core/memory/allocator].
*/
GraphicsFramebufferCaptureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion32Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX)

{
  uint uVar1;
  TH_LEGACY_LPVOID arg1;
  dword dVar2;
  TH_LEGACY_HRESULT TVar3;
  int iVar4;
  GraphicsCapturedTextureSourceAsset *memory;
  uint uVar5;
  GraphicsPixelDimension GVar6;
  uint *puVar7;
  GraphicsCapturedTextureSourceAsset *pGVar8;
  dword *pdVar9;
  dword *pdVar10;
  ArenaAllocEaxCf5 AVar11;
  GraphicsFramebufferCaptureEaxCf5 GVar12;
  uint *puVar13;
  
  uVar1 = captureWidth * captureHeight * 4 + 0x220;
  AVar11 = (*g_MemoryApi.alloc)(uVar1);
  memory = (GraphicsCapturedTextureSourceAsset *)AVar11.eax;
  if (!AVar11.carry) {
    pGVar8 = memory;
    for (uVar5 = uVar1 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
      (pGVar8->common).magic = 0;
      pGVar8 = (GraphicsCapturedTextureSourceAsset *)&(pGVar8->common).allocationSizeBytes;
    }
    (memory->common).magic = ASSET_MAGIC_GFX;
    (memory->common).allocationSizeBytes = uVar1;
    (memory->common).formatVersion = 1;
    (memory->common).converterVersion = 0;
    dVar2 = (*g_LocaleGetPackedCurrentTime)();
    (memory->common).buildMetadata.timestamps.dateValue0 = dVar2;
    (memory->common).buildMetadata.timestamps.dateValue1 = dVar2;
    (memory->common).buildMetadata.timestamps.dateValue2 = dVar2;
    dVar2 = (*g_LocaleGetPackedCurrentDate)();
    (memory->common).buildMetadata.timestamps.timeValue0 = dVar2;
    (memory->common).buildMetadata.timestamps.timeValue1 = dVar2;
    (memory->common).buildMetadata.timestamps.timeValue2 = dVar2;
    (*g_LocaleCopyDefaultComputerLabelUtf16)((memory->common).buildMetadata.names.producerName);
    (*g_LocaleCopyDefaultComputerLabelUtf16)((memory->common).buildMetadata.names.sourceName);
    memory->opaqueTablePayloadBC_1FF[0x44] = 0;
    (memory->tableDescriptor).subresourceCount = 1;
    (memory->tableDescriptor).paletteBankCount = 0;
    (memory->tableDescriptor).subresourceTableOffset = 0x200;
    (memory->sourceEntry).logicalWidth = captureWidth;
    (memory->sourceEntry).logicalHeight = captureHeight;
    (memory->sourceEntry).pixelWidth = captureWidth;
    (memory->sourceEntry).pixelHeight = captureHeight;
    (memory->sourceEntry).originX = 0;
    (memory->sourceEntry).originY = 0;
    (memory->sourceEntry).paletteIndex = -1;
    (memory->sourceEntry).dataOffset = 0x220;
    TVar3 = (*g_BackSurface3->lpVtbl->IsLost)(g_BackSurface3);
    iVar4 = 0;
    if (TVar3 != 0) {
      iVar4 = (*g_BackSurface3->lpVtbl->Restore)(g_BackSurface3);
    }
    if (iVar4 == 0) {
      Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
      g_SurfaceDesc.dwSize = 0x6c;
      TVar3 = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,0x11,
                         (TH_LEGACY_HANDLE)0x0);
      arg1 = g_SurfaceDesc.lpSurface;
      if (TVar3 == 0) {
        puVar7 = (uint *)((int)g_SurfaceDesc.lpSurface +
                         (sourceY * g_FramebufferWidth + sourceX) * 4);
        GVar6 = captureWidth;
        pdVar10 = memory->argb8888Pixels;
        puVar13 = puVar7;
        do {
          do {
            pdVar9 = pdVar10;
            uVar1 = puVar7[1];
            GVar6 = GVar6 - 2;
            *pdVar9 = *puVar7 | 0xff000000;
            pdVar9[1] = uVar1 | 0xff000000;
            puVar7 = puVar7 + 2;
            pdVar10 = pdVar9 + 2;
          } while (1 < GVar6);
          if (GVar6 == 1) {
            *pdVar10 = *puVar7 | 0xff000000;
            pdVar10 = pdVar9 + 3;
          }
          puVar7 = (uint *)((int)puVar13 + g_SurfaceDesc.lPitch);
          captureHeight = captureHeight - 1;
          GVar6 = captureWidth;
          puVar13 = puVar7;
        } while (captureHeight != 0);
        (*g_BackSurface3->lpVtbl->Unlock)(g_BackSurface3,arg1);
        return (GraphicsFramebufferCaptureEaxCf5)((uint5)AVar11 & 0xffffffff);
      }
    }
    (*g_MemoryApi.free)(memory);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x65,g_PackageLastErrorPath);
    memory = (GraphicsCapturedTextureSourceAsset *)&k_LowAddressLiteral0000001B;
  }
  GVar12.carry = true;
  GVar12.eax = memory;
  return GVar12;
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
  TH_LEGACY_HRESULT TVar1;
  bool bVar2;
  
  if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
    bVar2 = Glide3_Framebuffer_BeginAccess();
    return bVar2;
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
      TVar1 = (*g_BackSurface3->lpVtbl->Lock)
                        (g_BackSurface3,(TH_LEGACY_RECT *)0x0,&g_SurfaceDesc,1,(TH_LEGACY_HANDLE)0x0
                        );
      if (TVar1 == 0) {
        g_FramebufferRowStrideBytes = g_SurfaceDesc.lPitch;
        if (g_DisplayFramebufferAccess.bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
          g_DisplayFramebufferAccess.width = (uint)g_SurfaceDesc.lPitch >> 1;
        }
        else {
          g_DisplayFramebufferAccess.width = (uint)g_SurfaceDesc.lPitch >> 2;
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
  g_DisplayFramebufferAccess.pixels = (byte *)0x0;
  return;
}

