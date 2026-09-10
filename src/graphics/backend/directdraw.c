#include <thandor/graphics/backend/directdraw.h>

/* Implementation ownership: graphics/backend/directdraw. */

/* Address: 0x00423CF0.
   Ownership: graphics/backend/directdraw.
   Purpose: Scans the exact 0x10-byte GraphicsDisplayMode array and compares width, height, bitsPerPixel, and
   adapterIndex. CF clear means an exact tuple exists; CF set means absent; EAX is preserved.
*/
undefined8
GraphicsDisplayMode_IsEnumeratedCf
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width)

{
  undefined4 in_EAX;
  GraphicsDisplayModeCount GVar1;
  undefined4 in_EDX;
  GraphicsDisplayMode *pGVar2;
  
  GVar1 = g_GraphicsDisplayModeCount;
  pGVar2 = g_GraphicsDisplayModes;
  while ((((width != pGVar2->width || (height != pGVar2->height)) ||
          (bitsPerPixel != pGVar2->bitsPerPixel)) || (adapterIndex != pGVar2->adapterIndex))) {
    pGVar2 = pGVar2 + 1;
    GVar1 = GVar1 - 1;
    if (GVar1 == 0) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0054B0E0.
   Ownership: graphics/backend/directdraw.
   Purpose: Scans the enumerated display-mode table for an exact four-dword mode tuple. CF clear reports a match
   and CF set reports that no entry matched.
*/
undefined8
DisplayModeTable_ContainsExactModeCf
          (FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex)

{
  undefined4 in_EAX;
  dword modesRemaining;
  undefined4 in_EDX;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  while ((((width != modeCursor->width || (height != modeCursor->height)) ||
          (bitsPerPixel != modeCursor->bitsPerPixel)) || (adapterIndex != modeCursor->adapterIndex))
        ) {
    modeCursor = modeCursor + 1;
    modesRemaining = modesRemaining - 1;
    if (modesRemaining == 0) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00578080.
   Ownership: graphics/backend/directdraw.
   Purpose: DirectDrawEnumerateA callback. Appends one 0x80-byte GraphicsAdapterRecord.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
int DirectDraw_EnumAdapterCallback
              (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName,
              void *applicationContext)

{
  int dwordsRemaining;
  int iVar1;
  GraphicsAdapterRecord *adapterRecord;
  GraphicsAdapterRecord *pGVar2;
  GraphicsAdapterRecord *adapterRecordCursor;
  
  if (g_GraphicsAdapterCount < 0x10) {
    adapterRecord = g_GraphicsAdapters + g_GraphicsAdapterCount;
    pGVar2 = adapterRecord;
    for (dwordsRemaining = 0x20; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
      (pGVar2->adapterGuid).Data1 = 0;
      pGVar2 = (GraphicsAdapterRecord *)&(pGVar2->adapterGuid).Data2;
    }
    if (adapterGuid != (TH_LEGACY_GUID *)0x0) {
      adapterRecordCursor = adapterRecord;
      for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
        (adapterRecordCursor->adapterGuid).Data1 = adapterGuid->Data1;
        adapterGuid = (TH_LEGACY_GUID *)&adapterGuid->Data2;
        adapterRecordCursor = (GraphicsAdapterRecord *)&(adapterRecordCursor->adapterGuid).Data2;
      }
    }
    Text_CopyNarrowToUtf16Cf(0x28,adapterRecord->driverDescriptionUtf16,(byte *)driverDescription);
    g_GraphicsAdapterCount = g_GraphicsAdapterCount + 1;
  }
  return 1;
}

/* Address: 0x005780F0.
   Ownership: graphics/backend/directdraw.
   Purpose: IDirectDraw::EnumDisplayModes callback. The context value is an integer adapter index. Typed
   parameters: p1 adapterIndex→FrontendDisplayAdapterIndex_V302. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
sdword DirectDraw_EnumDisplayModeCallback
                 (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex)

{
  uint uVar1;
  uint uVar2;
  FrontendColorDepthBits FVar3;
  GraphicsDisplayMode *pGVar4;
  
  uVar1 = (surfaceDesc->ddpfPixelFormat).dwFlags;
  FVar3 = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  uVar2 = surfaceDesc->dwHeight;
  if (((((uVar1 & 0x838) == 0) && ((uVar1 & 0x2403) == 0)) && ((uVar1 & 0x200) == 0)) &&
     (((uVar1 & 0x40) != 0 && (g_GraphicsDisplayModeCount < 0x100)))) {
    if ((0x27f < surfaceDesc->dwWidth) && (0x1df < uVar2)) {
      pGVar4 = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
      if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == 0) {
        if ((FVar3 != 0x10) && (FVar3 != 0x20)) {
          return 1;
        }
      }
      else if (FVar3 == 0x10) {
        if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwFlags & 0x80) == 0) {
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwFlags & 0x80) == 0) {
            return 1;
          }
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwDeviceRenderBitDepth & 0x400) == 0
             ) {
            return 1;
          }
          FVar3 = 0x10;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & 0x400) == 0
             ) {
            return 1;
          }
          FVar3 = 0x10;
        }
      }
      else {
        if (FVar3 != 0x20) {
          return 1;
        }
        if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwFlags & 0x80) == 0) {
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwFlags & 0x80) == 0) {
            return 1;
          }
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwDeviceRenderBitDepth & 0x100) == 0
             ) {
            return 1;
          }
          FVar3 = 0x20;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & 0x100) == 0
             ) {
            return 1;
          }
          FVar3 = 0x20;
        }
      }
      pGVar4->width = surfaceDesc->dwWidth;
      pGVar4->height = uVar2;
      pGVar4->bitsPerPixel = FVar3;
      pGVar4->adapterIndex = adapterIndex;
      g_GraphicsDisplayModeCount = g_GraphicsDisplayModeCount + 1;
    }
  }
  return 1;
}

/* Address: 0x00578920.
   Ownership: graphics/backend/directdraw.
   Purpose: ABI: __stdcall(adapterIndex,bitsPerPixel,height,width), RET 0x10. CF=0 success; CF=1 failure. EAX
   carries the stage-specific engine error code on formatted failures. Sequence: release old DirectDraw/Direct3D
   surfaces and texture objects; create DirectDraw and IDirectDraw2; SetCooperativeLevel; SetDisplayMode;
   create/query primary and back surfaces; derive pixel masks; create/attach Z surface; create Direct3D device and
   viewport; enumerate texture formats and render states; publish framebuffer width, height and bytes-per-pixel
   dispatch; call the previously chained display-mode hook; recreate texture resources. Every failed COM stage
   jumps to the shared formatted-error path, which reports the completed stage count and returns with carry set.
   Cross-module calls: GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf [graphics/backend/glide],
   Glide3_Shutdown [graphics/backend/glide], GraphicsTexture_ReleaseObjects [graphics/resources/texture],
   Memory_ZeroDwords [core/memory/allocator], GraphicsTexture_CreateStagingTexture [graphics/resources/texture].
*/
void GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf
               (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
               GraphicsPixelDimension height,GraphicsPixelDimension width)

{
  D3DDEVICEDESC_DX6 *pDVar1;
  dword dVar2;
  GraphicsAdapterRecord *pGVar3;
  TH_LEGACY_HRESULT TVar4;
  int extraout_EAX;
  int iVar5;
  sdword sVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 *puVar7;
  GraphicsAdapterRecord *pGVar8;
  undefined4 *puVar9;
  GraphicsTextureResource **ppGVar10;
  bool bVar11;
  int iVar12;
  
  iVar12 = 0;
  if (g_ActiveGraphicsAdapterIndex == -1) {
GraphicsDirectDraw_CreateOrSwitchBackend:
    if (g_GraphicsAdapters[adapterIndex].adapterGuid.Data1 == 1) {
      GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf
                (adapterIndex,bitsPerPixel,height,width);
      return;
    }
    g_ActiveGraphicsAdapterIndex = -1;
    pGVar3 = g_GraphicsAdapters + adapterIndex;
    if ((pGVar3->adapterGuid).Data1 == 0) {
      pGVar3 = (GraphicsAdapterRecord *)0x0;
    }
    TVar4 = (*pDirectDrawCreate)(&pGVar3->adapterGuid,&g_DirectDraw,(TH_LEGACY_LPVOID)0x0);
    if (TVar4 != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    iVar12 = iVar12 + 1;
    TVar4 = (*g_DirectDraw->lpVtbl->SetCooperativeLevel)(g_DirectDraw,g_MainWindow,8);
    if (TVar4 != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    iVar12 = iVar12 + 1;
    TVar4 = (*g_DirectDraw->lpVtbl->QueryInterface)
                      (g_DirectDraw,&IID_IDirectDraw2_Local,&g_DirectDraw2);
    if (TVar4 != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    iVar12 = iVar12 + 1;
    TVar4 = (*g_DirectDraw2->lpVtbl->SetCooperativeLevel)(g_DirectDraw2,g_MainWindow,0x11);
    if (TVar4 != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    iVar12 = iVar12 + 1;
    g_ActiveGraphicsAdapterIndex = adapterIndex;
  }
  else {
    if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
      Glide3_Shutdown();
      g_ActiveGraphicsAdapterIndex = -1;
      goto GraphicsDirectDraw_CreateOrSwitchBackend;
    }
    iVar5 = 0x1000;
    ppGVar10 = g_GraphicsTextureSlots;
    do {
      if (*ppGVar10 != (GraphicsTextureResource *)0x0) {
        GraphicsTexture_ReleaseObjects(*ppGVar10);
        iVar5 = extraout_ECX;
      }
      ppGVar10 = ppGVar10 + 1;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    g_LastViewportRect.x1 = 0;
    g_LastViewportRect.y1 = 0;
    g_LastViewportRect.x2 = 0;
    g_LastViewportRect.y2 = 0;
    if (g_Direct3DViewport2 != (IDirect3DViewport2 *)0x0) {
      (*g_Direct3DViewport2->lpVtbl->Release)(g_Direct3DViewport2);
      g_Direct3DViewport2 = (IDirect3DViewport2 *)0x0;
    }
    if (g_ZSurface3 != (IDirectDrawSurface3 *)0x0) {
      (*g_ZSurface3->lpVtbl->Release)(g_ZSurface3);
      g_ZSurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_ZSurfaceBase != (IDirectDrawSurface *)0x0) {
      (*g_ZSurfaceBase->lpVtbl->Release)(g_ZSurfaceBase);
      g_ZSurfaceBase = (IDirectDrawSurface *)0x0;
    }
    if (g_Direct3DDevice2 != (IDirect3DDevice2 *)0x0) {
      (*g_Direct3DDevice2->lpVtbl->Release)(g_Direct3DDevice2);
      g_Direct3DDevice2 = (IDirect3DDevice2 *)0x0;
    }
    if (g_Direct3D2 != (IDirect3D2 *)0x0) {
      (*g_Direct3D2->lpVtbl->Release)(g_Direct3D2);
      g_Direct3D2 = (IDirect3D2 *)0x0;
    }
    g_CursorCurrentVisibilityToken = -1;
    g_CursorAlternateVisibilityToken = -1;
    if (g_BackSurface3 != (IDirectDrawSurface3 *)0x0) {
      (*g_BackSurface3->lpVtbl->Release)(g_BackSurface3);
      g_BackSurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_BackSurfaceBase != (IDirectDrawSurface *)0x0) {
      (*g_BackSurfaceBase->lpVtbl->Release)(g_BackSurfaceBase);
      g_BackSurfaceBase = (IDirectDrawSurface *)0x0;
    }
    if (g_PrimarySurface3 != (IDirectDrawSurface3 *)0x0) {
      (*g_PrimarySurface3->lpVtbl->Release)(g_PrimarySurface3);
      g_PrimarySurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_PrimarySurfaceBase != (IDirectDrawSurface *)0x0) {
      (*g_PrimarySurfaceBase->lpVtbl->Release)(g_PrimarySurfaceBase);
      g_PrimarySurfaceBase = (IDirectDrawSurface *)0x0;
    }
    bVar11 = g_GraphicsAdapters + adapterIndex == (GraphicsAdapterRecord *)0x0;
    iVar5 = 4;
    pGVar3 = g_GraphicsAdapters + g_ActiveGraphicsAdapterIndex;
    pGVar8 = g_GraphicsAdapters + adapterIndex;
    do {
      if (iVar5 == 0) break;
      iVar5 = iVar5 + -1;
      bVar11 = (pGVar3->adapterGuid).Data1 == (pGVar8->adapterGuid).Data1;
      pGVar3 = (GraphicsAdapterRecord *)&(pGVar3->adapterGuid).Data2;
      pGVar8 = (GraphicsAdapterRecord *)&(pGVar8->adapterGuid).Data2;
    } while (bVar11);
    if (!bVar11) {
      g_ActiveGraphicsAdapterIndex = -1;
      if (g_DirectDraw2 != (IDirectDraw2 *)0x0) {
        (*g_DirectDraw2->lpVtbl->Release)(g_DirectDraw2);
        g_DirectDraw2 = (IDirectDraw2 *)0x0;
      }
      if (g_DirectDraw != (IDirectDraw *)0x0) {
        (*g_DirectDraw->lpVtbl->Release)(g_DirectDraw);
        g_DirectDraw = (IDirectDraw *)0x0;
      }
      goto GraphicsDirectDraw_CreateOrSwitchBackend;
    }
  }
  TVar4 = (*g_DirectDraw2->lpVtbl->SetDisplayMode)(g_DirectDraw2,width,height,bitsPerPixel);
  pGVar3 = g_GraphicsAdapters;
  if (TVar4 == 0) {
    iVar12 = iVar12 + 1;
    Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
    g_SurfaceDesc.dwSize = 0x6c;
    g_SurfaceDesc.dwFlags = 1;
    g_SurfaceDesc.ddsCaps.dwCaps = 0x200;
    if (*(int *)((pGVar3->deviceGuid).Data4 + extraout_EAX + -8) != 0) {
      g_SurfaceDesc.dwFlags = 0x21;
      g_SurfaceDesc.ddsCaps.dwCaps = 0x2218;
      g_SurfaceDesc.dwBackBufferCount = 1;
    }
    TVar4 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&g_PrimarySurfaceBase,(TH_LEGACY_LPVOID)0x0);
    if (TVar4 == 0) {
      iVar12 = iVar12 + 1;
      TVar4 = (*g_PrimarySurfaceBase->lpVtbl->QueryInterface)
                        (g_PrimarySurfaceBase,&IID_IDirectDrawSurface3_Local,&g_PrimarySurface3);
      pGVar3 = g_GraphicsAdapters;
      if (TVar4 == 0) {
        iVar12 = iVar12 + 1;
        Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
        if (*(int *)((pGVar3->deviceGuid).Data4 + extraout_ECX_00 + -8) == 0) {
          g_SurfaceDesc.dwSize = 0x6c;
          g_SurfaceDesc.dwFlags = 7;
          g_SurfaceDesc.ddsCaps.dwCaps = 0x840;
          g_SurfaceDesc.dwWidth = width;
          g_SurfaceDesc.dwHeight = height;
          TVar4 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                            (g_DirectDraw2,&g_SurfaceDesc,&g_BackSurfaceBase,(TH_LEGACY_LPVOID)0x0);
          if (TVar4 != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          iVar12 = iVar12 + 1;
          iVar5 = (*g_BackSurfaceBase->lpVtbl->QueryInterface)
                            (g_BackSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_BackSurface3);
        }
        else {
          g_SurfaceDesc.ddsCaps.dwCaps = 4;
          iVar5 = (*g_PrimarySurface3->lpVtbl->GetAttachedSurface)
                            (g_PrimarySurface3,(DDSCAPS *)0x577ce8,&g_BackSurface3);
        }
        if (iVar5 == 0) {
          iVar5 = iVar12 + 1;
          Memory_ZeroDwords(0x20,&g_SurfaceDesc.ddpfPixelFormat);
          g_SurfaceDesc.ddpfPixelFormat.dwSize = 0x20;
          TVar4 = (*g_PrimarySurface3->lpVtbl->GetPixelFormat)
                            (g_PrimarySurface3,(DDPIXELFORMAT *)0x577cc8);
          iVar12 = iVar5;
          if ((((TVar4 == 0) && (iVar12 = iVar5 + 1, g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0))
              && (iVar12 = iVar5 + 2, g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0)) &&
             (iVar12 = iVar5 + 3, g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0)) {
            iVar12 = iVar5 + 4;
            g_SoftwarePixelFormatConfig.redMask = g_SurfaceDesc.ddpfPixelFormat.dwRBitMask;
            g_SoftwarePixelFormatConfig.greenMask = g_SurfaceDesc.ddpfPixelFormat.dwGBitMask;
            g_SoftwarePixelFormatConfig.blueMask = g_SurfaceDesc.ddpfPixelFormat.dwBBitMask;
            g_SoftwarePixelFormatConfig.redShift = 0;
            if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
              for (; (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >>
                      g_SoftwarePixelFormatConfig.redShift & 1) == 0;
                  g_SoftwarePixelFormatConfig.redShift = g_SoftwarePixelFormatConfig.redShift + 1) {
              }
            }
            g_SoftwarePixelFormatConfig.greenShift = 0;
            if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
              for (; (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >>
                      g_SoftwarePixelFormatConfig.greenShift & 1) == 0;
                  g_SoftwarePixelFormatConfig.greenShift =
                       g_SoftwarePixelFormatConfig.greenShift + 1) {
              }
            }
            g_SoftwarePixelFormatConfig.blueShift = 0;
            if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
              for (; (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >>
                      g_SoftwarePixelFormatConfig.blueShift & 1) == 0;
                  g_SoftwarePixelFormatConfig.blueShift = g_SoftwarePixelFormatConfig.blueShift + 1)
              {
              }
            }
            iVar5 = 0x1f;
            if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
              for (; g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >> iVar5 == 0; iVar5 = iVar5 + -1) {
              }
            }
            g_SoftwarePixelFormatConfig.redBitCount =
                 (iVar5 + 1) - g_SoftwarePixelFormatConfig.redShift;
            iVar5 = 0x1f;
            if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
              for (; g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >> iVar5 == 0; iVar5 = iVar5 + -1) {
              }
            }
            g_SoftwarePixelFormatConfig.greenBitCount =
                 (iVar5 + 1) - g_SoftwarePixelFormatConfig.greenShift;
            iVar5 = 0x1f;
            if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
              for (; g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >> iVar5 == 0; iVar5 = iVar5 + -1) {
              }
            }
            g_SoftwarePixelFormatConfig.blueBitCount =
                 (iVar5 + 1) - g_SoftwarePixelFormatConfig.blueShift;
            pGVar3 = g_GraphicsAdapters + adapterIndex;
            if ((pGVar3->deviceGuid).Data1 == 0) {
GraphicsDirectDraw_CommitFramebufferAndRenderCallbacks:
              g_FramebufferWidth = width;
              g_FramebufferHeight = height;
              g_ActiveGraphicsAdapterIndex = adapterIndex;
              g_DisplayFramebufferAccess.width = width;
              g_DisplayFramebufferAccess.height = height;
              g_DisplayFramebufferAccess.pixels = (byte *)0x0;
              g_FramebufferAccess = &g_DisplayFramebufferAccess;
              bVar11 = bitsPerPixel < 0x10;
              if (bitsPerPixel < 0x11) {
                g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT;
                g_GraphicsFramebufferCaptureRegion = GraphicsFramebuffer_CaptureRegion16Bit;
                g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha16;
                g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb16
                ;
                g_GraphicsTextureSourceStretchDirectColorBilinear =
                     SoftwareTextureSource_StretchDirectColorBilinear16;
                g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha =
                     SoftwareTextureSource_BlitIntegerScaledSourceAlpha16;
                g_GraphicsTextureSourceBlitSourceAlphaPaletteBank =
                     SoftwareTextureSource_BlitSourceAlphaPaletteBank16;
                g_GraphicsTextureSourceBlitModulatedSourceAlpha =
                     SoftwareTextureSource_BlitModulatedSourceAlpha16;
                g_GraphicsTextureSourceBlitSaturatedAddRgb =
                     SoftwareTextureSource_BlitSaturatedAddRgb16;
                g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd =
                     SoftwareTextureSource_BlitHalfRgbSaturatedAdd16;
                g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb16;
              }
              else {
                g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
                g_GraphicsFramebufferCaptureRegion = GraphicsFramebuffer_CaptureRegion32Bit;
                g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha32;
                g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb32
                ;
                g_GraphicsTextureSourceStretchDirectColorBilinear =
                     SoftwareTextureSource_StretchDirectColorBilinear32;
                g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha =
                     SoftwareTextureSource_BlitIntegerScaledSourceAlpha32;
                g_GraphicsTextureSourceBlitSourceAlphaPaletteBank =
                     SoftwareTextureSource_BlitSourceAlphaPaletteBank32;
                g_GraphicsTextureSourceBlitModulatedSourceAlpha =
                     SoftwareTextureSource_BlitModulatedSourceAlpha32;
                g_GraphicsTextureSourceBlitSaturatedAddRgb =
                     SoftwareTextureSource_BlitSaturatedAddRgb32;
                g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd =
                     SoftwareTextureSource_BlitHalfRgbSaturatedAdd32;
                g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb32;
              }
              g_GraphicsFramebufferPresent = GraphicsFramebuffer_Present;
              (*(code *)g_GraphicsDisplayModeFinalizeCf)
                        (adapterIndex,bitsPerPixel,height,width,iVar12);
              if (!bVar11) {
                iVar12 = 0x1000;
                ppGVar10 = g_GraphicsTextureSlots;
                do {
                  if (*ppGVar10 != (GraphicsTextureResource *)0x0) {
                    GraphicsTexture_CreateStagingTexture(*ppGVar10);
                    iVar12 = extraout_ECX_01;
                  }
                  ppGVar10 = ppGVar10 + 1;
                  iVar12 = iVar12 + -1;
                } while (iVar12 != 0);
                return;
              }
              return;
            }
            TVar4 = (*g_DirectDraw2->lpVtbl->QueryInterface)
                              (g_DirectDraw2,&IID_IDirect3D2_Local,&g_Direct3D2);
            if (TVar4 == 0) {
              iVar12 = iVar12 + 1;
              pDVar1 = pGVar3->hardwareDesc;
              Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
              g_SurfaceDesc.dwSize = 0x6c;
              g_SurfaceDesc.dwFlags = 0x47;
              g_SurfaceDesc.dwWidth = width;
              g_SurfaceDesc.dwHeight = height;
              g_SurfaceDesc.dwMipMapCount = 0x10;
              if (pDVar1->dcmColorModel == 0) {
                g_SurfaceDesc.ddsCaps.dwCaps = 0x22800;
              }
              else {
                g_SurfaceDesc.ddsCaps.dwCaps = 0x26000;
              }
              TVar4 = (*g_DirectDraw2->lpVtbl->CreateSurface)
                                (g_DirectDraw2,&g_SurfaceDesc,&g_ZSurfaceBase,(TH_LEGACY_LPVOID)0x0)
              ;
              if (TVar4 == 0) {
                iVar12 = iVar12 + 1;
                TVar4 = (*g_ZSurfaceBase->lpVtbl->QueryInterface)
                                  (g_ZSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_ZSurface3);
                if (TVar4 == 0) {
                  iVar12 = iVar12 + 1;
                  TVar4 = (*g_BackSurface3->lpVtbl->AddAttachedSurface)(g_BackSurface3,g_ZSurface3);
                  if (TVar4 == 0) {
                    iVar12 = iVar12 + 1;
                    TVar4 = (*g_Direct3D2->lpVtbl->CreateDevice)
                                      (g_Direct3D2,&g_GraphicsAdapters[adapterIndex].deviceGuid,
                                       (IDirectDrawSurface *)g_BackSurface3,&g_Direct3DDevice2);
                    if (TVar4 == 0) {
                      iVar12 = iVar12 + 1;
                      TVar4 = (*g_Direct3D2->lpVtbl->CreateViewport)
                                        (g_Direct3D2,&g_Direct3DViewport2,(TH_LEGACY_LPVOID)0x0);
                      if (TVar4 == 0) {
                        iVar12 = iVar12 + 1;
                        TVar4 = (*g_Direct3DDevice2->lpVtbl->AddViewport)
                                          (g_Direct3DDevice2,g_Direct3DViewport2);
                        if (TVar4 == 0) {
                          iVar12 = iVar12 + 1;
                          TVar4 = (*g_Direct3DDevice2->lpVtbl->SetCurrentViewport)
                                            (g_Direct3DDevice2,g_Direct3DViewport2);
                          if (TVar4 == 0) {
                            iVar5 = iVar12 + 1;
                            Memory_ZeroDwords(0x20,(void *)0x577d90);
                            Memory_ZeroDwords(0x20,(void *)0x577db0);
                            TVar4 = (*g_Direct3DDevice2->lpVtbl->EnumTextureFormats)
                                              (g_Direct3DDevice2,
                                               GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
                                               ,(TH_LEGACY_LPVOID)0x0);
                            iVar12 = iVar5;
                            if (((TVar4 == 0) &&
                                (iVar12 = iVar5 + 1, g_Direct3DOpaqueTextureFormatBitsPerPixel != 0)
                                ) && (iVar12 = iVar5 + 2,
                                     g_Direct3DAlphaTextureFormatBitsPerPixel != 0)) {
                              iVar12 = iVar5 + 3;
                              puVar7 = (undefined4 *)0x577d90;
                              puVar9 = (undefined4 *)0x577dd0;
                              for (iVar5 = 8; iVar5 != 0; iVar5 = iVar5 + -1) {
                                *puVar9 = *puVar7;
                                puVar7 = puVar7 + 1;
                                puVar9 = puVar9 + 1;
                              }
                              puVar7 = (undefined4 *)0x577db0;
                              puVar9 = (undefined4 *)0x577df0;
                              for (iVar5 = 8; iVar5 != 0; iVar5 = iVar5 + -1) {
                                *puVar9 = *puVar7;
                                puVar7 = puVar7 + 1;
                                puVar9 = puVar9 + 1;
                              }
                              (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                        (g_Direct3DDevice2,D3DRENDERSTATE_MONOENABLE,0);
                              sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                (g_Direct3DDevice2,D3DRENDERSTATE_SHADEMODE,2);
                              if (sVar6 == 0) {
                                iVar12 = iVar12 + 1;
                                sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                  (g_Direct3DDevice2,D3DRENDERSTATE_SPECULARENABLE,0
                                                  );
                                if (sVar6 == 0) {
                                  iVar12 = iVar12 + 1;
                                  sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                    (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,1);
                                  if (sVar6 == 0) {
                                    iVar12 = iVar12 + 1;
                                    (*g_Direct3DDevice2->lpVtbl->GetRenderState)
                                              (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,
                                               &g_ImmediateVertexCount);
                                    dVar2 = g_ImmediateVertexCount;
                                    g_ImmediateVertexCount = 3;
                                    if (dVar2 != 1) {
                                      g_ImmediateVertexCount = 4;
                                    }
                                    sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                      (g_Direct3DDevice2,D3DRENDERSTATE_ZENABLE,1);
                                    if (sVar6 == 0) {
                                      iVar12 = iVar12 + 1;
                                      sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                        (g_Direct3DDevice2,D3DRENDERSTATE_ZFUNC,4);
                                      if (sVar6 == 0) {
                                        iVar12 = iVar12 + 1;
                                        sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                          (g_Direct3DDevice2,D3DRENDERSTATE_FILLMODE
                                                           ,3);
                                        if (sVar6 == 0) {
                                          iVar12 = iVar12 + 1;
                                          sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                            (g_Direct3DDevice2,
                                                             D3DRENDERSTATE_TEXTUREMAPBLEND,4);
                                          if (sVar6 == 0) {
                                            iVar12 = iVar12 + 1;
                                            sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                              (g_Direct3DDevice2,
                                                               D3DRENDERSTATE_ANTIALIAS,
                                                               g_Direct3DAntialiasMode);
                                            if (sVar6 == 0) {
                                              iVar12 = iVar12 + 1;
                                              sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                                (g_Direct3DDevice2,
                                                                 D3DRENDERSTATE_TEXTUREMAG,
                                                                 g_Direct3DTextureFilterMode);
                                              if (sVar6 == 0) {
                                                iVar12 = iVar12 + 1;
                                                sVar6 = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                                  (g_Direct3DDevice2,
                                                                   D3DRENDERSTATE_TEXTUREMIN,
                                                                   g_Direct3DTextureFilterMode);
                                                if (sVar6 == 0) {
                                                  iVar12 = iVar12 + 1;
                                                  sVar6 = (*g_Direct3DDevice2->lpVtbl->
                                                            SetRenderState)(g_Direct3DDevice2,
                                                                                                                                                        
                                                  D3DRENDERSTATE_TEXTUREPERSPECTIVE,
                                                  g_Direct3DTexturePerspectiveEnabled);
                                                  if (sVar6 == 0) {
                                                    iVar12 = iVar12 + 1;
                                                    (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                                                              (g_Direct3DDevice2,
                                                               D3DRENDERSTATE_ZWRITEENABLE,
                                                               g_PrimitiveRenderStateCache.
                                                               zWriteEnable);
                                                    sVar6 = (*g_Direct3DDevice2->lpVtbl->
                                                              SetRenderState)(g_Direct3DDevice2,
                                                                                                                                                            
                                                  D3DRENDERSTATE_ALPHABLENDENABLE,
                                                  g_PrimitiveRenderStateCache.alphaBlendEnable);
                                                  if (sVar6 == 0) {
                                                    iVar12 = iVar12 + 1;
                                                    sVar6 = (*g_Direct3DDevice2->lpVtbl->
                                                              SetRenderState)(g_Direct3DDevice2,
                                                                                                                                                            
                                                  D3DRENDERSTATE_SRCBLEND,
                                                  g_PrimitiveRenderStateCache.sourceBlend);
                                                  if (sVar6 == 0) {
                                                    iVar12 = iVar12 + 1;
                                                    sVar6 = (*g_Direct3DDevice2->lpVtbl->
                                                              SetRenderState)(g_Direct3DDevice2,
                                                                                                                                                            
                                                  D3DRENDERSTATE_DESTBLEND,
                                                  g_PrimitiveRenderStateCache.destinationBlend);
                                                  if (sVar6 == 0) {
                                                    iVar12 = iVar12 + 1;
                                                    g_BoundTextureHandle = 0;
                                                    sVar6 = (*g_Direct3DDevice2->lpVtbl->
                                                              SetRenderState)(g_Direct3DDevice2,
                                                                                                                                                            
                                                  D3DRENDERSTATE_TEXTUREHANDLE,0);
                                                  if (sVar6 == 0) {
                                                    iVar12 = iVar12 + 1;
                                                    goto 
                                                  GraphicsDirectDraw_CommitFramebufferAndRenderCallbacks
                                                  ;
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
GraphicsDirectDraw_ReleasePartialInitializationAfterFailure:
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar12,g_PackageLastErrorPath);
  return;
}
