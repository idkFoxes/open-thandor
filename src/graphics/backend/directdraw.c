/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/directdraw.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/directdraw.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/backend/directdraw. */

/* Address: 0x00423CF0.
   Ownership: graphics/backend/directdraw.
   Purpose: Scans the exact 0x10-byte GraphicsDisplayMode array and compares width, height, bitsPerPixel, and
   adapterIndex. CF clear means an exact tuple exists; CF set means absent; EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsDisplayMode_IsEnumeratedCf
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width)

{
  GraphicsDisplayModeCount modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  while ((((width != modeCursor->width || (height != modeCursor->height)) ||
          (bitsPerPixel != modeCursor->bitsPerPixel)) || (adapterIndex != modeCursor->adapterIndex))) {
    modeCursor = modeCursor + 1;
    modesRemaining = modesRemaining - 1;
    if (modesRemaining == 0) {
      return true;
    }
  }
  return false;
}


/* Address: 0x0054B0E0.
   Ownership: graphics/backend/directdraw.
   Purpose: Scans the enumerated display-mode table for an exact four-dword mode tuple. CF clear reports a match
   and CF set reports that no entry matched.
*/
bool __thandor_cf_preserve_eax_ecx_edx
DisplayModeTable_ContainsExactModeCf
          (FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex)

{
  dword modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  while ((((width != modeCursor->width || (height != modeCursor->height)) ||
          (bitsPerPixel != modeCursor->bitsPerPixel)) || (adapterIndex != modeCursor->adapterIndex))
        ) {
    modeCursor = modeCursor + 1;
    modesRemaining = modesRemaining - 1;
    if (modesRemaining == 0) {
      return true;
    }
  }
  return false;
}


/* Address: 0x00578080.
   Ownership: graphics/backend/directdraw.
   Purpose: DirectDrawEnumerateA callback. Appends one 0x80-byte GraphicsAdapterRecord.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
int __stdcall DirectDraw_EnumAdapterCallback
              (TH_LEGACY_GUID *adapterGuid,char *driverDescription,char *driverName,
              void *applicationContext)

{
  int dwordsRemaining;
  int guidDwordsRemaining;
  GraphicsAdapterRecord *adapterRecord;
  GraphicsAdapterRecord *zeroCursor;
  GraphicsAdapterRecord *adapterRecordCursor;
  
  if (g_GraphicsAdapterCount < 0x10) {
    adapterRecord = g_GraphicsAdapters + g_GraphicsAdapterCount;
    zeroCursor = adapterRecord;
    for (dwordsRemaining = 0x20; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
      (zeroCursor->adapterGuid).Data1 = 0;
      zeroCursor = (GraphicsAdapterRecord *)&(zeroCursor->adapterGuid).Data2;
    }
    if (adapterGuid != (TH_LEGACY_GUID *)0x0) {
      adapterRecordCursor = adapterRecord;
      for (guidDwordsRemaining = 4; guidDwordsRemaining != 0; guidDwordsRemaining = guidDwordsRemaining + -1) {
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
sdword __stdcall DirectDraw_EnumDisplayModeCallback
                 (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex)

{
  uint pixelFormatFlags;
  uint modeHeight;
  FrontendColorDepthBits modeBitsPerPixel;
  GraphicsDisplayMode *modeSlot;
  
  pixelFormatFlags = (surfaceDesc->ddpfPixelFormat).dwFlags;
  modeBitsPerPixel = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  modeHeight = surfaceDesc->dwHeight;
  if (((((pixelFormatFlags & 0x838) == 0) && ((pixelFormatFlags & 0x2403) == 0)) && ((pixelFormatFlags & 0x200) == 0)) &&
     (((pixelFormatFlags & 0x40) != 0 && (g_GraphicsDisplayModeCount < 0x100)))) {
    if ((0x27f < surfaceDesc->dwWidth) && (0x1df < modeHeight)) {
      modeSlot = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
      if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == 0) {
        if ((modeBitsPerPixel != 0x10) && (modeBitsPerPixel != 0x20)) {
          return 1;
        }
      }
      else if (modeBitsPerPixel == 0x10) {
        if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwFlags & 0x80) == 0) {
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwFlags & 0x80) == 0) {
            return 1;
          }
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwDeviceRenderBitDepth & 0x400) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 0x10;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & 0x400) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 0x10;
        }
      }
      else {
        if (modeBitsPerPixel != 0x20) {
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
          modeBitsPerPixel = 0x20;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & 0x100) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 0x20;
        }
      }
      modeSlot->width = surfaceDesc->dwWidth;
      modeSlot->height = modeHeight;
      modeSlot->bitsPerPixel = modeBitsPerPixel;
      modeSlot->adapterIndex = adapterIndex;
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
DisplayModeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)

{
  D3DDEVICEDESC_DX6 *hardwareDeviceDesc;
  GraphicsTextureResource *in_EAX;
  GraphicsAdapterRecord *adapterRecord;
  TH_LEGACY_HRESULT comResult;
  sdword renderStateResult;
  int stageOrLoopCounter;
  dword errorCodeOrCullMode;
  undefined4 *formatSource;
  GraphicsAdapterRecord *requestedGuidCursor;
  undefined4 *formatDest;
  GraphicsTextureResource **textureSlotCursor;
  bool guidMatchOrCarry;
  DisplayModeEaxCf5 displayModeResult;
  DisplayModeEaxCf5 exitResult;
  DisplayModeEaxCf5 failureResult;
  int completedStages;
  
  completedStages = 0;
  displayModeResult.eax = in_EAX;
  if (g_ActiveGraphicsAdapterIndex == -1) {
GraphicsDirectDraw_CreateOrSwitchBackend:
    if (g_GraphicsAdapters[adapterIndex].adapterGuid.Data1 == 1) {
      guidMatchOrCarry = GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf
                         (adapterIndex,bitsPerPixel,height,width);
      goto LAB_005794a3;
    }
    g_ActiveGraphicsAdapterIndex = -1;
    adapterRecord = g_GraphicsAdapters + adapterIndex;
    if ((adapterRecord->adapterGuid).Data1 == 0) {
      adapterRecord = (GraphicsAdapterRecord *)0x0;
    }
    comResult = (*pDirectDrawCreate)(&adapterRecord->adapterGuid,&g_DirectDraw,(TH_LEGACY_LPVOID)0x0);
    errorCodeOrCullMode = 0x19;
    stageOrLoopCounter = completedStages;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = (*g_DirectDraw->lpVtbl->SetCooperativeLevel)(g_DirectDraw,g_MainWindow,8);
    errorCodeOrCullMode = 0x19;
    stageOrLoopCounter = 1;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = (*g_DirectDraw->lpVtbl->QueryInterface)
                      (g_DirectDraw,&IID_IDirectDraw2_Local,&g_DirectDraw2);
    errorCodeOrCullMode = 0x19;
    stageOrLoopCounter = 2;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = (*g_DirectDraw2->lpVtbl->SetCooperativeLevel)(g_DirectDraw2,g_MainWindow,0x11);
    errorCodeOrCullMode = 0x19;
    stageOrLoopCounter = 3;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    completedStages = 4;
    g_ActiveGraphicsAdapterIndex = adapterIndex;
  }
  else {
    if (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == 1) {
      Glide3_Shutdown();
      g_ActiveGraphicsAdapterIndex = -1;
      goto GraphicsDirectDraw_CreateOrSwitchBackend;
    }
    stageOrLoopCounter = 0x1000;
    textureSlotCursor = g_GraphicsTextureSlots;
    do {
      if (*textureSlotCursor != (GraphicsTextureResource *)0x0) {
        GraphicsTexture_ReleaseObjects(*textureSlotCursor);
      }
      textureSlotCursor = textureSlotCursor + 1;
      stageOrLoopCounter = stageOrLoopCounter + -1;
    } while (stageOrLoopCounter != 0);
    g_LastViewportRect.x1 = 0;
    g_LastViewportRect.y1 = 0;
    g_LastViewportRect.x2 = 0;
    g_LastViewportRect.y2 = 0;
    if (g_Direct3DViewport2 != (IDirect3DViewport2 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)
                   (*g_Direct3DViewport2->lpVtbl->Release)(g_Direct3DViewport2);
      g_Direct3DViewport2 = (IDirect3DViewport2 *)0x0;
    }
    if (g_ZSurface3 != (IDirectDrawSurface3 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)(*g_ZSurface3->lpVtbl->Release)(g_ZSurface3);
      g_ZSurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_ZSurfaceBase != (IDirectDrawSurface *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)(*g_ZSurfaceBase->lpVtbl->Release)(g_ZSurfaceBase);
      g_ZSurfaceBase = (IDirectDrawSurface *)0x0;
    }
    if (g_Direct3DDevice2 != (IDirect3DDevice2 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)
                   (*g_Direct3DDevice2->lpVtbl->Release)(g_Direct3DDevice2);
      g_Direct3DDevice2 = (IDirect3DDevice2 *)0x0;
    }
    if (g_Direct3D2 != (IDirect3D2 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)(*g_Direct3D2->lpVtbl->Release)(g_Direct3D2);
      g_Direct3D2 = (IDirect3D2 *)0x0;
    }
    g_CursorCurrentVisibilityToken = -1;
    g_CursorAlternateVisibilityToken = -1;
    if (g_BackSurface3 != (IDirectDrawSurface3 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)(*g_BackSurface3->lpVtbl->Release)(g_BackSurface3);
      g_BackSurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_BackSurfaceBase != (IDirectDrawSurface *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)
                   (*g_BackSurfaceBase->lpVtbl->Release)(g_BackSurfaceBase);
      g_BackSurfaceBase = (IDirectDrawSurface *)0x0;
    }
    if (g_PrimarySurface3 != (IDirectDrawSurface3 *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)
                   (*g_PrimarySurface3->lpVtbl->Release)(g_PrimarySurface3);
      g_PrimarySurface3 = (IDirectDrawSurface3 *)0x0;
    }
    if (g_PrimarySurfaceBase != (IDirectDrawSurface *)0x0) {
      displayModeResult.eax = (GraphicsTextureResource *)
                   (*g_PrimarySurfaceBase->lpVtbl->Release)(g_PrimarySurfaceBase);
      g_PrimarySurfaceBase = (IDirectDrawSurface *)0x0;
    }
    guidMatchOrCarry = g_GraphicsAdapters + adapterIndex == (GraphicsAdapterRecord *)0x0;
    stageOrLoopCounter = 4;
    adapterRecord = g_GraphicsAdapters + g_ActiveGraphicsAdapterIndex;
    requestedGuidCursor = g_GraphicsAdapters + adapterIndex;
    do {
      if (stageOrLoopCounter == 0) break;
      stageOrLoopCounter = stageOrLoopCounter + -1;
      guidMatchOrCarry = (adapterRecord->adapterGuid).Data1 == (requestedGuidCursor->adapterGuid).Data1;
      adapterRecord = (GraphicsAdapterRecord *)&(adapterRecord->adapterGuid).Data2;
      requestedGuidCursor = (GraphicsAdapterRecord *)&(requestedGuidCursor->adapterGuid).Data2;
    } while (guidMatchOrCarry);
    if (!guidMatchOrCarry) {
      g_ActiveGraphicsAdapterIndex = -1;
      if (g_DirectDraw2 != (IDirectDraw2 *)0x0) {
        displayModeResult.eax = (GraphicsTextureResource *)(*g_DirectDraw2->lpVtbl->Release)(g_DirectDraw2);
        g_DirectDraw2 = (IDirectDraw2 *)0x0;
      }
      if (g_DirectDraw != (IDirectDraw *)0x0) {
        displayModeResult.eax = (GraphicsTextureResource *)(*g_DirectDraw->lpVtbl->Release)(g_DirectDraw);
        g_DirectDraw = (IDirectDraw *)0x0;
      }
      goto GraphicsDirectDraw_CreateOrSwitchBackend;
    }
  }
  comResult = (*g_DirectDraw2->lpVtbl->SetDisplayMode)(g_DirectDraw2,width,height,bitsPerPixel,0,0);
  adapterRecord = g_GraphicsAdapters;
  errorCodeOrCullMode = 0x1a;
  stageOrLoopCounter = completedStages;
  if (comResult == 0) {
    Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
    g_SurfaceDesc.dwSize = 0x6c;
    g_SurfaceDesc.dwFlags = 1;
    g_SurfaceDesc.ddsCaps.dwCaps = 0x200;
    if (adapterRecord[adapterIndex].deviceGuid.Data1 != 0) {
      g_SurfaceDesc.dwFlags = 0x21;
      g_SurfaceDesc.ddsCaps.dwCaps = 0x2218;
      g_SurfaceDesc.dwBackBufferCount = 1;
    }
    comResult = (*g_DirectDraw2->lpVtbl->CreateSurface)
                      (g_DirectDraw2,&g_SurfaceDesc,&g_PrimarySurfaceBase,(TH_LEGACY_LPVOID)0x0);
    errorCodeOrCullMode = 0x1b;
    stageOrLoopCounter = completedStages + 1;
    if (comResult == 0) {
      comResult = (*g_PrimarySurfaceBase->lpVtbl->QueryInterface)
                        (g_PrimarySurfaceBase,&IID_IDirectDrawSurface3_Local,&g_PrimarySurface3);
      adapterRecord = g_GraphicsAdapters;
      errorCodeOrCullMode = 0x1b;
      stageOrLoopCounter = completedStages + 2;
      if (comResult == 0) {
        Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
        stageOrLoopCounter = completedStages + 3;
        if (adapterRecord[adapterIndex].deviceGuid.Data1 == 0) {
          g_SurfaceDesc.dwSize = 0x6c;
          g_SurfaceDesc.dwFlags = 7;
          g_SurfaceDesc.ddsCaps.dwCaps = 0x840;
          g_SurfaceDesc.dwWidth = width;
          g_SurfaceDesc.dwHeight = height;
          comResult = (*g_DirectDraw2->lpVtbl->CreateSurface)
                            (g_DirectDraw2,&g_SurfaceDesc,&g_BackSurfaceBase,(TH_LEGACY_LPVOID)0x0);
          errorCodeOrCullMode = 0x1b;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          comResult = (*g_BackSurfaceBase->lpVtbl->QueryInterface)
                            (g_BackSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_BackSurface3);
          errorCodeOrCullMode = 0x1b;
          stageOrLoopCounter = completedStages + 4;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          completedStages = completedStages + 5;
        }
        else {
          g_SurfaceDesc.ddsCaps.dwCaps = 4;
          comResult = (*g_PrimarySurface3->lpVtbl->GetAttachedSurface)
                            (g_PrimarySurface3,&g_SurfaceDesc.ddsCaps,&g_BackSurface3);
          errorCodeOrCullMode = 0x1b;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          completedStages = completedStages + 4;
        }
        Memory_ZeroDwords(0x20,&g_SurfaceDesc.ddpfPixelFormat);
        g_SurfaceDesc.ddpfPixelFormat.dwSize = 0x20;
        comResult = (*g_PrimarySurface3->lpVtbl->GetPixelFormat)
                          (g_PrimarySurface3,&g_SurfaceDesc.ddpfPixelFormat);
        errorCodeOrCullMode = 0x1c;
        stageOrLoopCounter = completedStages;
        if ((((comResult == 0) && (stageOrLoopCounter = completedStages + 1, g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0)
             ) && (stageOrLoopCounter = completedStages + 2, g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0)) &&
           (stageOrLoopCounter = completedStages + 3, g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0)) {
          g_SoftwarePixelFormatConfig.redMask = g_SurfaceDesc.ddpfPixelFormat.dwRBitMask;
          g_SoftwarePixelFormatConfig.greenMask = g_SurfaceDesc.ddpfPixelFormat.dwGBitMask;
          g_SoftwarePixelFormatConfig.blueMask = g_SurfaceDesc.ddpfPixelFormat.dwBBitMask;
          g_SoftwarePixelFormatConfig.redShift = 0;
          if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
            for (; (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >> g_SoftwarePixelFormatConfig.redShift
                   & 1) == 0;
                g_SoftwarePixelFormatConfig.redShift = g_SoftwarePixelFormatConfig.redShift + 1) {
            }
          }
          g_SoftwarePixelFormatConfig.greenShift = 0;
          if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
            for (; (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >>
                    g_SoftwarePixelFormatConfig.greenShift & 1) == 0;
                g_SoftwarePixelFormatConfig.greenShift = g_SoftwarePixelFormatConfig.greenShift + 1)
            {
            }
          }
          g_SoftwarePixelFormatConfig.blueShift = 0;
          if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
            for (; (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >>
                    g_SoftwarePixelFormatConfig.blueShift & 1) == 0;
                g_SoftwarePixelFormatConfig.blueShift = g_SoftwarePixelFormatConfig.blueShift + 1) {
            }
          }
          stageOrLoopCounter = 0x1f;
          if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
            for (; g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >> stageOrLoopCounter == 0; stageOrLoopCounter = stageOrLoopCounter + -1) {
            }
          }
          g_SoftwarePixelFormatConfig.redBitCount =
               (stageOrLoopCounter + 1) - g_SoftwarePixelFormatConfig.redShift;
          stageOrLoopCounter = 0x1f;
          if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
            for (; g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >> stageOrLoopCounter == 0; stageOrLoopCounter = stageOrLoopCounter + -1) {
            }
          }
          g_SoftwarePixelFormatConfig.greenBitCount =
               (stageOrLoopCounter + 1) - g_SoftwarePixelFormatConfig.greenShift;
          stageOrLoopCounter = 0x1f;
          if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
            for (; g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >> stageOrLoopCounter == 0; stageOrLoopCounter = stageOrLoopCounter + -1) {
            }
          }
          g_SoftwarePixelFormatConfig.blueBitCount =
               (stageOrLoopCounter + 1) - g_SoftwarePixelFormatConfig.blueShift;
          adapterRecord = g_GraphicsAdapters + adapterIndex;
          if ((adapterRecord->deviceGuid).Data1 != 0) {
            comResult = (*g_DirectDraw2->lpVtbl->QueryInterface)
                              (g_DirectDraw2,&IID_IDirect3D2_Local,&g_Direct3D2);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 4;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            hardwareDeviceDesc = adapterRecord->hardwareDesc;
            Memory_ZeroDwords(0x6c,&g_SurfaceDesc);
            g_SurfaceDesc.dwSize = 0x6c;
            g_SurfaceDesc.dwFlags = 0x47;
            g_SurfaceDesc.dwWidth = width;
            g_SurfaceDesc.dwHeight = height;
            g_SurfaceDesc.dwMipMapCount = 0x10;
            if (hardwareDeviceDesc->dcmColorModel == 0) {
              g_SurfaceDesc.ddsCaps.dwCaps = 0x22800;
            }
            else {
              g_SurfaceDesc.ddsCaps.dwCaps = 0x26000;
            }
            comResult = (*g_DirectDraw2->lpVtbl->CreateSurface)
                              (g_DirectDraw2,&g_SurfaceDesc,&g_ZSurfaceBase,(TH_LEGACY_LPVOID)0x0);
            errorCodeOrCullMode = 0x1e;
            stageOrLoopCounter = completedStages + 5;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_ZSurfaceBase->lpVtbl->QueryInterface)
                              (g_ZSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_ZSurface3);
            errorCodeOrCullMode = 0x1e;
            stageOrLoopCounter = completedStages + 6;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_BackSurface3->lpVtbl->AddAttachedSurface)(g_BackSurface3,g_ZSurface3);
            errorCodeOrCullMode = 0x1e;
            stageOrLoopCounter = completedStages + 7;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_Direct3D2->lpVtbl->CreateDevice)
                              (g_Direct3D2,&g_GraphicsAdapters[adapterIndex].deviceGuid,
                               (IDirectDrawSurface *)g_BackSurface3,&g_Direct3DDevice2);
            errorCodeOrCullMode = 0x1f;
            stageOrLoopCounter = completedStages + 8;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_Direct3D2->lpVtbl->CreateViewport)
                              (g_Direct3D2,&g_Direct3DViewport2,(TH_LEGACY_LPVOID)0x0);
            errorCodeOrCullMode = 0x20;
            stageOrLoopCounter = completedStages + 9;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_Direct3DDevice2->lpVtbl->AddViewport)(g_Direct3DDevice2,g_Direct3DViewport2)
            ;
            errorCodeOrCullMode = 0x20;
            stageOrLoopCounter = completedStages + 10;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = (*g_Direct3DDevice2->lpVtbl->SetCurrentViewport)
                              (g_Direct3DDevice2,g_Direct3DViewport2);
            errorCodeOrCullMode = 0x20;
            stageOrLoopCounter = completedStages + 0xb;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            Memory_ZeroDwords(0x20,(void *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0));
            Memory_ZeroDwords(0x20,(void *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0));
            comResult = (*g_Direct3DDevice2->lpVtbl->EnumTextureFormats)
                              (g_Direct3DDevice2,
                               GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback,
                               (TH_LEGACY_LPVOID)0x0);
            errorCodeOrCullMode = 0x21;
            stageOrLoopCounter = completedStages + 0xc;
            if (((comResult != 0) ||
                (stageOrLoopCounter = completedStages + 0xd, g_Direct3DOpaqueTextureFormatBitsPerPixel == 0)) ||
               (stageOrLoopCounter = completedStages + 0xe, g_Direct3DAlphaTextureFormatBitsPerPixel == 0))
            goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            formatSource = (undefined4 *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
            formatDest = (undefined4 *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0);
            for (stageOrLoopCounter = 8; stageOrLoopCounter != 0; stageOrLoopCounter = stageOrLoopCounter + -1) {
              *formatDest = *formatSource;
              formatSource = formatSource + 1;
              formatDest = formatDest + 1;
            }
            formatSource = (undefined4 *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
            formatDest = (undefined4 *)THANDOR_ADDR(g_Direct3DSelectedAlphaTextureFormat,0);
            for (stageOrLoopCounter = 8; stageOrLoopCounter != 0; stageOrLoopCounter = stageOrLoopCounter + -1) {
              *formatDest = *formatSource;
              formatSource = formatSource + 1;
              formatDest = formatDest + 1;
            }
            (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_MONOENABLE,0);
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_SHADEMODE,2);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0xf;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_SPECULARENABLE,0);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x10;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,1);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x11;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            (*g_Direct3DDevice2->lpVtbl->GetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,&g_ImmediateVertexCount);
            errorCodeOrCullMode = g_ImmediateVertexCount;
            g_ImmediateVertexCount = 3;
            if (errorCodeOrCullMode != 1) {
              g_ImmediateVertexCount = 4;
            }
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_ZENABLE,1);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x12;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_ZFUNC,4);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x13;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_FILLMODE,3);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x14;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAPBLEND,4);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x15;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,g_Direct3DAntialiasMode);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x16;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,
                               g_Direct3DTextureFilterMode);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x17;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,
                               g_Direct3DTextureFilterMode);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x18;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,
                               g_Direct3DTexturePerspectiveEnabled);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x19;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                      (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                       g_PrimitiveRenderStateCache.zWriteEnable);
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                               g_PrimitiveRenderStateCache.alphaBlendEnable);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x1a;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                               g_PrimitiveRenderStateCache.sourceBlend);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x1b;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                               g_PrimitiveRenderStateCache.destinationBlend);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x1c;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            g_BoundTextureHandle = 0;
            renderStateResult = (*g_Direct3DDevice2->lpVtbl->SetRenderState)
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
            errorCodeOrCullMode = 0x1d;
            stageOrLoopCounter = completedStages + 0x1d;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          }
          g_FramebufferWidth = width;
          g_FramebufferHeight = height;
          g_ActiveGraphicsAdapterIndex = adapterIndex;
          g_DisplayFramebufferAccess.width = width;
          g_DisplayFramebufferAccess.height = height;
          g_DisplayFramebufferAccess.pixels = (byte *)0x0;
          g_FramebufferAccess = &g_DisplayFramebufferAccess;
          if (bitsPerPixel < 0x11) {
            g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT;
            g_GraphicsFramebufferCaptureRegion = GraphicsFramebuffer_CaptureRegion16Bit;
            g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha16;
            g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb16;
            g_GraphicsTextureSourceStretchDirectColorBilinear =
                 SoftwareTextureSource_StretchDirectColorBilinear16;
            g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha =
                 SoftwareTextureSource_BlitIntegerScaledSourceAlpha16;
            g_GraphicsTextureSourceBlitSourceAlphaPaletteBank =
                 SoftwareTextureSource_BlitSourceAlphaPaletteBank16;
            g_GraphicsTextureSourceBlitModulatedSourceAlpha =
                 SoftwareTextureSource_BlitModulatedSourceAlpha16;
            g_GraphicsTextureSourceBlitSaturatedAddRgb = SoftwareTextureSource_BlitSaturatedAddRgb16
            ;
            g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd =
                 SoftwareTextureSource_BlitHalfRgbSaturatedAdd16;
            g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb16;
          }
          else {
            g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
            g_GraphicsFramebufferCaptureRegion = GraphicsFramebuffer_CaptureRegion32Bit;
            g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha32;
            g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb32;
            g_GraphicsTextureSourceStretchDirectColorBilinear =
                 SoftwareTextureSource_StretchDirectColorBilinear32;
            g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha =
                 SoftwareTextureSource_BlitIntegerScaledSourceAlpha32;
            g_GraphicsTextureSourceBlitSourceAlphaPaletteBank =
                 SoftwareTextureSource_BlitSourceAlphaPaletteBank32;
            g_GraphicsTextureSourceBlitModulatedSourceAlpha =
                 SoftwareTextureSource_BlitModulatedSourceAlpha32;
            g_GraphicsTextureSourceBlitSaturatedAddRgb = SoftwareTextureSource_BlitSaturatedAddRgb32
            ;
            g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd =
                 SoftwareTextureSource_BlitHalfRgbSaturatedAdd32;
            g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb32;
          }
          g_GraphicsFramebufferPresent = GraphicsFramebuffer_Present;
          displayModeResult = (*g_GraphicsDisplayModeFinalizeCf)(adapterIndex,bitsPerPixel,height,width);
          if (displayModeResult.carry) {
            displayModeResult.carry = true;
            return displayModeResult;
          }
          stageOrLoopCounter = 0x1000;
          textureSlotCursor = g_GraphicsTextureSlots;
          do {
            if (*textureSlotCursor != (GraphicsTextureResource *)0x0) {
              displayModeResult.eax = GraphicsTexture_CreateStagingTexture(*textureSlotCursor);
            }
            textureSlotCursor = textureSlotCursor + 1;
            stageOrLoopCounter = stageOrLoopCounter + -1;
          } while (stageOrLoopCounter != 0);
          guidMatchOrCarry = false;
LAB_005794a3:
          exitResult.carry = guidMatchOrCarry;
          exitResult.eax = (dword)displayModeResult.eax;
          return exitResult;
        }
      }
    }
  }
GraphicsDirectDraw_ReleasePartialInitializationAfterFailure:
  completedStages = stageOrLoopCounter;
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,completedStages,g_PackageLastErrorPath);
  failureResult.carry = true;
  failureResult.eax = errorCodeOrCullMode;
  return failureResult;
}

