/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/directdraw.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/directdraw.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: graphics/backend/directdraw. */

/* Windowed test aid (OPEN_THANDOR_WINDOWED, not in the original): the few IDirectDrawClipper methods it
   uses, in vtable order (the generated types only know the clipper as an opaque pointer). */
typedef struct TestAidDirectDrawClipper TestAidDirectDrawClipper;
typedef struct TestAidDirectDrawClipperVtbl {
  void *QueryInterface;
  void *AddRef;
  TH_LEGACY_ULONG (__stdcall *Release)(TestAidDirectDrawClipper *);
  void *GetClipList;
  void *GetHWnd;
  void *Initialize;
  void *IsClipListChanged;
  void *SetClipList;
  TH_LEGACY_HRESULT (__stdcall *SetHWnd)(TestAidDirectDrawClipper *, TH_LEGACY_DWORD, void *);
} TestAidDirectDrawClipperVtbl;
struct TestAidDirectDrawClipper {
  TestAidDirectDrawClipperVtbl *lpVtbl;
};

/* Windowed test aid: a Direct3D or Glide adapter cannot run in a window here, so fall back to a plain
   DirectDraw record of the primary display driver (software renderer). Returns the adapter to use. */
static FrontendDisplayAdapterIndex
GraphicsDirectDraw_TestAidWindowedAdapter(FrontendDisplayAdapterIndex adapterIndex)
{
  FrontendDisplayAdapterIndex index;

  if ((g_GraphicsAdapters[adapterIndex].adapterGuid.Data1 != GRAPHICS_ADAPTER_GUID_GLIDE) &&
      (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE)) {
    return adapterIndex;
  }
  for (index = 0; (uint32_t)index < g_GraphicsAdapterCount; index++) {
    if ((g_GraphicsAdapters[index].adapterGuid.Data1 == 0) &&
        (g_GraphicsAdapters[index].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE)) {
      Thandor_Log("test aid: windowed mode supports only the software renderer: adapter %u instead of %u",
                  (unsigned)index, (unsigned)adapterIndex);
      return index;
    }
  }
  Thandor_Log("test aid: windowed mode supports only the software renderer, but no software adapter "
              "exists; keeping adapter %u", (unsigned)adapterIndex);
  return adapterIndex;
}

/* Windowed test aid: sizes the window to the mode and clips the primary surface to it. */
static TH_LEGACY_HRESULT GraphicsDirectDraw_TestAidAttachWindowClipper(uint32_t width,uint32_t height)
{
  TestAidDirectDrawClipper *clipper;
  TH_LEGACY_HRESULT result;

  Thandor_TestAidSetWindowClientSize(g_MainWindow,width,height);
  clipper = NULL;
  result = g_DirectDraw2->lpVtbl->CreateClipper(g_DirectDraw2,0,(TH_LEGACY_LPVOID *)&clipper,NULL);
  if (result == 0) {
    result = clipper->lpVtbl->SetHWnd(clipper,0,g_MainWindow);
    if (result == 0) {
      result = g_PrimarySurface3->lpVtbl->SetClipper(g_PrimarySurface3,clipper);
    }
    clipper->lpVtbl->Release(clipper); /* the primary surface keeps its own reference */
  }
  if (result != 0) {
    Thandor_Log("test aid: windowed mode could not attach a clipper (%08x)", (unsigned)result);
  }
  return result;
}

/* Address: 0x00423CF0.
   Tells whether the display mode (width, height, bitsPerPixel, adapterIndex) was enumerated
   (g_GraphicsDisplayModes, filled by DirectDraw_EnumDisplayModeCallback): CF clear (false) when it was, CF set
   (true) when not. Used by UiDisplayModeSelection_RefreshEnumeratedOptions (ui/controls/misc.c) to offer only
   available modes. The table is assumed non-empty: the first entry is compared before the count is checked.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsDisplayMode_IsEnumerated
          (FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width)

{
  GraphicsDisplayModeCount modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  while ((((width != modeCursor->width || (height != modeCursor->height)) ||
          (bitsPerPixel != modeCursor->bitsPerPixel)) || (adapterIndex != modeCursor->adapterIndex))) {
    modeCursor++;
    modesRemaining--;
    if (modesRemaining == 0) {
      return true;
    }
  }
  return false;
}


/* Address: 0x0054B0E0.
   Same test as GraphicsDisplayMode_IsEnumerated with the parameters in a different order: CF clear (false) when
   the mode was enumerated. Used by FrontendDisplaySettingsPage_UpdateModeActionAvailability
   (ui/frontend/settings.c).
*/
bool __thandor_cf_preserve_eax_ecx_edx
DisplayModeTable_ContainsExactMode
          (FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex)

{
  uint32_t modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  while ((((width != modeCursor->width || (height != modeCursor->height)) ||
          (bitsPerPixel != modeCursor->bitsPerPixel)) || (adapterIndex != modeCursor->adapterIndex))
        ) {
    modeCursor++;
    modesRemaining--;
    if (modesRemaining == 0) {
      return true;
    }
  }
  return false;
}


/* Address: 0x00578080.
   DirectDrawEnumerateA callback: appends a zeroed adapter record with the driver's GUID (all zero for the
   primary display driver, which DirectDraw passes as NULL) and its description, while fewer than 16 adapters are
   known. Always continues the enumeration (returns 1).
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

  if (g_GraphicsAdapterCount < GRAPHICS_ADAPTER_CAPACITY) {
    adapterRecord = g_GraphicsAdapters + g_GraphicsAdapterCount;
    zeroCursor = adapterRecord;
    for (dwordsRemaining = sizeof(GraphicsAdapterRecord) / sizeof(uint32_t); dwordsRemaining != 0;
         dwordsRemaining--) {
      (zeroCursor->adapterGuid).Data1 = 0;
      zeroCursor = (GraphicsAdapterRecord *)&(zeroCursor->adapterGuid).Data2;
    }
    if (adapterGuid != NULL) {
      adapterRecordCursor = adapterRecord;
      for (guidDwordsRemaining = sizeof(TH_LEGACY_GUID) / sizeof(uint32_t); guidDwordsRemaining != 0;
           guidDwordsRemaining--) {
        (adapterRecordCursor->adapterGuid).Data1 = adapterGuid->Data1;
        adapterGuid = (TH_LEGACY_GUID *)&adapterGuid->Data2;
        adapterRecordCursor = (GraphicsAdapterRecord *)&(adapterRecordCursor->adapterGuid).Data2;
      }
    }
    Text_CopyNarrowToUtf16(40,adapterRecord->driverDescriptionUtf16,(uint8_t *)driverDescription);
    g_GraphicsAdapterCount++;
  }
  return 1;
}

/* Address: 0x005780F0.
   IDirectDraw2::EnumDisplayModes callback (the context is the adapter index): records every plain RGB mode of at
   least 640x480 while fewer than 256 modes are known. Without a Direct3D device the adapter takes 16 and 32 bits;
   with one, only depths the hardware (or else the software) device can render to. Always continues (returns 1).
*/
int32_t __stdcall DirectDraw_EnumDisplayModeCallback
                 (DDSURFACEDESC_DX6 *surfaceDesc,FrontendDisplayAdapterIndex adapterIndex)

{
  uint32_t pixelFormatFlags;
  uint32_t modeHeight;
  FrontendColorDepthBits modeBitsPerPixel;
  GraphicsDisplayMode *modeSlot;
  
  pixelFormatFlags = (surfaceDesc->ddpfPixelFormat).dwFlags;
  modeBitsPerPixel = (surfaceDesc->ddpfPixelFormat).dwRGBBitCount;
  modeHeight = surfaceDesc->dwHeight;
  /* no palette, alpha, Z or YUV format, only RGB */
  if (((((pixelFormatFlags & (DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
                              DDPF_PALETTEINDEXED4)) == 0) &&
        ((pixelFormatFlags & (DDPF_ZPIXELS | DDPF_ZBUFFER | DDPF_ALPHA | DDPF_ALPHAPIXELS)) == 0)) &&
       ((pixelFormatFlags & DDPF_YUV) == 0)) &&
     (((pixelFormatFlags & DDPF_RGB) != 0 && (g_GraphicsDisplayModeCount < GRAPHICS_DISPLAY_MODE_CAPACITY)))) {
    if ((640 - 1 < surfaceDesc->dwWidth) && (480 - 1 < modeHeight)) {
      modeSlot = g_GraphicsDisplayModes + g_GraphicsDisplayModeCount;
      if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == 0) {
        if ((modeBitsPerPixel != 16) && (modeBitsPerPixel != 32)) {
          return 1;
        }
      }
      else if (modeBitsPerPixel == 16) {
        if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwFlags & D3DDD_DEVICERENDERBITDEPTH) == 0) {
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwFlags & D3DDD_DEVICERENDERBITDEPTH) == 0) {
            return 1;
          }
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwDeviceRenderBitDepth & DDBD_16) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 16;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & DDBD_16) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 16;
        }
      }
      else {
        if (modeBitsPerPixel != 32) {
          return 1;
        }
        if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwFlags & D3DDD_DEVICERENDERBITDEPTH) == 0) {
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwFlags & D3DDD_DEVICERENDERBITDEPTH) == 0) {
            return 1;
          }
          if (((g_GraphicsAdapters[adapterIndex].softwareDesc)->dwDeviceRenderBitDepth & DDBD_32) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 32;
        }
        else {
          if (((g_GraphicsAdapters[adapterIndex].hardwareDesc)->dwDeviceRenderBitDepth & DDBD_32) == 0
             ) {
            return 1;
          }
          modeBitsPerPixel = 32;
        }
      }
      modeSlot->width = surfaceDesc->dwWidth;
      modeSlot->height = modeHeight;
      modeSlot->bitsPerPixel = modeBitsPerPixel;
      modeSlot->adapterIndex = adapterIndex;
      g_GraphicsDisplayModeCount++;
    }
  }
  return 1;
}

/* Address: 0x00578920.
   g_GraphicsSetDisplayMode for DirectDraw/Direct3D (and the switch to Glide): releases the surfaces and devices of
   the current mode (and DirectDraw itself when the adapter changes), creates DirectDraw, the primary and back
   surfaces and, for a Direct3D adapter, the Z-buffer, device, viewport, texture formats and render states, then
   publishes the framebuffer, selects the 16- or 32-bit software blitters, calls the chained finalize hook and
   recreates the textures. A failing step returns its FATAL_ERROR_DIRECTDRAW_... or FATAL_ERROR_DIRECT3D_... code with CF
   set and leaves the number of completed steps as text in g_PackageLastErrorPath.
*/
DisplayModeResult __thandor_eax_cf_preserve_ecx_edx
GraphicsDirectDraw_ApplyDisplayModeAndCreateResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)

{
  D3DDEVICEDESC_DX6 *hardwareDeviceDesc;
  GraphicsAdapterRecord *adapterRecord;
  TH_LEGACY_HRESULT comResult;
  int32_t renderStateResult;
  int stageOrLoopCounter;
  uint32_t errorCodeOrCullMode;
  uint32_t *formatSource;
  GraphicsAdapterRecord *requestedGuidCursor;
  uint32_t *formatDest;
  GraphicsTextureResource **textureSlotCursor;
  bool guidMatchOrCarry;
  DisplayModeResult displayModeResult;
  DisplayModeResult exitResult;
  DisplayModeResult failureResult;
  int completedStages;

  completedStages = 0;
  if (Thandor_TestAidWindowed()) {
    adapterIndex = GraphicsDirectDraw_TestAidWindowedAdapter(adapterIndex);
  }
  /* EAX as the Glide path returns it. GraphicsGlide3_ApplyDisplayModeAndInitializeResources preserves EAX, so the
     original hands back whatever EAX held before the call (the caller's EAX on the first call, otherwise the last
     COM Release result). The caller only reads it with CF set; 0x1a is the mode error the Glide callee computes
     but discards, which gives a meaningful message. */
  displayModeResult.valueOrError = FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE;
  if ((g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE) &&
     (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE)) {
    /* Leaving the Glide backend: shut it down and create the new backend from scratch. */
    Glide3_Shutdown();
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
  }
  if (g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE) {
    /* DirectDraw backend already active: release every surface and device object, then keep the DirectDraw
       objects only if the requested adapter GUID matches the active one. */
    stageOrLoopCounter = 0x1000; /* texture slots */
    textureSlotCursor = g_GraphicsTextureSlots;
    do {
      if (*textureSlotCursor != NULL) {
        GraphicsTexture_ReleaseObjects(*textureSlotCursor);
      }
      textureSlotCursor = textureSlotCursor + 1;
      stageOrLoopCounter--;
    } while (stageOrLoopCounter != 0);
    g_LastViewportRect.x1 = 0;
    g_LastViewportRect.y1 = 0;
    g_LastViewportRect.x2 = 0;
    g_LastViewportRect.y2 = 0;
    if (g_Direct3DViewport2 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_Direct3DViewport2->lpVtbl->Release(g_Direct3DViewport2);
      g_Direct3DViewport2 = NULL;
    }
    if (g_ZSurface3 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_ZSurface3->lpVtbl->Release(g_ZSurface3);
      g_ZSurface3 = NULL;
    }
    if (g_ZSurfaceBase != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_ZSurfaceBase->lpVtbl->Release(g_ZSurfaceBase);
      g_ZSurfaceBase = NULL;
    }
    if (g_Direct3DDevice2 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_Direct3DDevice2->lpVtbl->Release(g_Direct3DDevice2);
      g_Direct3DDevice2 = NULL;
    }
    if (g_Direct3D2 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_Direct3D2->lpVtbl->Release(g_Direct3D2);
      g_Direct3D2 = NULL;
    }
    g_CursorCurrentVisibilityToken = -1;
    g_CursorAlternateVisibilityToken = -1;
    if (g_BackSurface3 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_BackSurface3->lpVtbl->Release(g_BackSurface3);
      g_BackSurface3 = NULL;
    }
    if (g_BackSurfaceBase != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_BackSurfaceBase->lpVtbl->Release(g_BackSurfaceBase);
      g_BackSurfaceBase = NULL;
    }
    if (g_PrimarySurface3 != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_PrimarySurface3->lpVtbl->Release(g_PrimarySurface3);
      g_PrimarySurface3 = NULL;
    }
    if (g_PrimarySurfaceBase != NULL) {
      displayModeResult.valueOrError = (uint32_t)g_PrimarySurfaceBase->lpVtbl->Release(g_PrimarySurfaceBase);
      g_PrimarySurfaceBase = NULL;
    }
    /* REPE CMPSD over the 16-byte adapter GUIDs. */
    guidMatchOrCarry = g_GraphicsAdapters + adapterIndex == NULL;
    stageOrLoopCounter = sizeof(TH_LEGACY_GUID) / sizeof(uint32_t);
    adapterRecord = g_GraphicsAdapters + g_ActiveGraphicsAdapterIndex;
    requestedGuidCursor = g_GraphicsAdapters + adapterIndex;
    do {
      if (stageOrLoopCounter == 0) break;
      stageOrLoopCounter--;
      guidMatchOrCarry = (adapterRecord->adapterGuid).Data1 == (requestedGuidCursor->adapterGuid).Data1;
      adapterRecord = (GraphicsAdapterRecord *)&(adapterRecord->adapterGuid).Data2;
      requestedGuidCursor = (GraphicsAdapterRecord *)&(requestedGuidCursor->adapterGuid).Data2;
    } while (guidMatchOrCarry);
    if (!guidMatchOrCarry) {
      g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
      if (g_DirectDraw2 != NULL) {
        displayModeResult.valueOrError = (uint32_t)g_DirectDraw2->lpVtbl->Release(g_DirectDraw2);
        g_DirectDraw2 = NULL;
      }
      if (g_DirectDraw != NULL) {
        displayModeResult.valueOrError = (uint32_t)g_DirectDraw->lpVtbl->Release(g_DirectDraw);
        g_DirectDraw = NULL;
      }
    }
  }
  if (g_ActiveGraphicsAdapterIndex == GRAPHICS_ADAPTER_INDEX_NONE) {
    /* No backend (first call, or switched away from Glide / to another adapter): create it. */
    if (g_GraphicsAdapters[adapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE) {
      guidMatchOrCarry = GraphicsGlide3_ApplyDisplayModeAndInitializeResources
                         (adapterIndex,bitsPerPixel,height,width);
      exitResult.failed = guidMatchOrCarry;
      exitResult.valueOrError = displayModeResult.valueOrError;
      return exitResult;
    }
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
    adapterRecord = g_GraphicsAdapters + adapterIndex;
    if ((adapterRecord->adapterGuid).Data1 == 0) {
      adapterRecord = NULL; /* the primary display driver */
    }
    comResult = pDirectDrawCreate(&adapterRecord->adapterGuid,&g_DirectDraw,NULL);
    errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE;
    stageOrLoopCounter = completedStages;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = g_DirectDraw->lpVtbl->SetCooperativeLevel(g_DirectDraw,g_MainWindow,DDSCL_NORMAL);
    errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE;
    stageOrLoopCounter = 1;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = g_DirectDraw->lpVtbl->QueryInterface
                      (g_DirectDraw,&IID_IDirectDraw2_Local,&g_DirectDraw2);
    errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE;
    stageOrLoopCounter = 2;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    comResult = g_DirectDraw2->lpVtbl->SetCooperativeLevel(g_DirectDraw2,g_MainWindow,
                                                        Thandor_TestAidWindowed() ? DDSCL_NORMAL :
                                                        DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
    errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE;
    stageOrLoopCounter = 3;
    if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
    completedStages = 4;
    g_ActiveGraphicsAdapterIndex = adapterIndex;
  }
  if (Thandor_TestAidWindowed()) {
    comResult = 0; /* windowed test aid: the desktop keeps its mode (and colour depth) */
  }
  else {
    comResult = g_DirectDraw2->lpVtbl->SetDisplayMode(g_DirectDraw2,width,height,bitsPerPixel,0,0);
  }
  adapterRecord = g_GraphicsAdapters;
  errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE;
  stageOrLoopCounter = completedStages;
  if (comResult == 0) {
    Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
    g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
    g_SurfaceDesc.dwFlags = DDSD_CAPS;
    g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    if (adapterRecord[adapterIndex].deviceGuid.Data1 != 0) {
      /* Direct3D: flipping primary chain with one back buffer as the render target */
      g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
      g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
      g_SurfaceDesc.dwBackBufferCount = 1;
    }
    comResult = g_DirectDraw2->lpVtbl->CreateSurface
                      (g_DirectDraw2,&g_SurfaceDesc,&g_PrimarySurfaceBase,NULL);
    errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
    stageOrLoopCounter = completedStages + 1;
    if (comResult == 0) {
      comResult = g_PrimarySurfaceBase->lpVtbl->QueryInterface
                        (g_PrimarySurfaceBase,&IID_IDirectDrawSurface3_Local,&g_PrimarySurface3);
      adapterRecord = g_GraphicsAdapters;
      errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
      stageOrLoopCounter = completedStages + 2;
      if (comResult == 0) {
        if (Thandor_TestAidWindowed()) {
          GraphicsDirectDraw_TestAidAttachWindowClipper(width,height);
        }
        Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
        stageOrLoopCounter = completedStages + 3;
        if (adapterRecord[adapterIndex].deviceGuid.Data1 == 0) {
          g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
          /* no Direct3D: the back buffer is a system-memory offscreen surface */
          g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
          g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
          g_SurfaceDesc.dwWidth = width;
          g_SurfaceDesc.dwHeight = height;
          comResult = g_DirectDraw2->lpVtbl->CreateSurface
                            (g_DirectDraw2,&g_SurfaceDesc,&g_BackSurfaceBase,NULL);
          errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          comResult = g_BackSurfaceBase->lpVtbl->QueryInterface
                            (g_BackSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_BackSurface3);
          errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
          stageOrLoopCounter = completedStages + 4;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          completedStages = completedStages + 5;
        }
        else {
          g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
          comResult = g_PrimarySurface3->lpVtbl->GetAttachedSurface
                            (g_PrimarySurface3,&g_SurfaceDesc.ddsCaps,&g_BackSurface3);
          errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES;
          if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          completedStages = completedStages + 4;
        }
        Memory_ZeroDwords(sizeof g_SurfaceDesc.ddpfPixelFormat,&g_SurfaceDesc.ddpfPixelFormat);
        g_SurfaceDesc.ddpfPixelFormat.dwSize = sizeof g_SurfaceDesc.ddpfPixelFormat;
        comResult = g_PrimarySurface3->lpVtbl->GetPixelFormat
                          (g_PrimarySurface3,&g_SurfaceDesc.ddpfPixelFormat);
        errorCodeOrCullMode = FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT;
        stageOrLoopCounter = completedStages;
        if ((((comResult == 0) && (stageOrLoopCounter = completedStages + 1, g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0)
             ) && (stageOrLoopCounter = completedStages + 2, g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0)) &&
           (stageOrLoopCounter = completedStages + 3, g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0)) {
          if (Thandor_TestAidWindowed() && (bitsPerPixel != g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount)) {
            /* windowed test aid: the surfaces have the desktop's depth, and the blitters chosen below, the
               renderer's queue (bytesPerPixel) and the pixel packing must all follow the surfaces */
            Thandor_Log("test aid: windowed %ux%u uses the desktop depth of %u bits instead of %u",
                        (unsigned)width,(unsigned)height,(unsigned)g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount,
                        (unsigned)bitsPerPixel);
            bitsPerPixel = g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
          }
          g_SoftwarePixelFormatConfig.redMask = g_SurfaceDesc.ddpfPixelFormat.dwRBitMask;
          g_SoftwarePixelFormatConfig.greenMask = g_SurfaceDesc.ddpfPixelFormat.dwGBitMask;
          g_SoftwarePixelFormatConfig.blueMask = g_SurfaceDesc.ddpfPixelFormat.dwBBitMask;
          /* shift = lowest set bit, bit count = highest set bit + 1 - shift */
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
            comResult = g_DirectDraw2->lpVtbl->QueryInterface
                              (g_DirectDraw2,&IID_IDirect3D2_Local,&g_Direct3D2);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 4;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            hardwareDeviceDesc = adapterRecord->hardwareDesc;
            Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
            g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
            g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
            g_SurfaceDesc.dwWidth = width;
            g_SurfaceDesc.dwHeight = height;
            g_SurfaceDesc.dwMipMapCount = 16; /* dwZBufferBitDepth in the SDK's union */
            /* a device without a hardware colour model is a software rasterizer: Z-buffer in system memory */
            if (hardwareDeviceDesc->dcmColorModel == 0) {
              g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
            }
            else {
              g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
            }
            comResult = g_DirectDraw2->lpVtbl->CreateSurface
                              (g_DirectDraw2,&g_SurfaceDesc,&g_ZSurfaceBase,NULL);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_ZBUFFER;
            stageOrLoopCounter = completedStages + 5;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_ZSurfaceBase->lpVtbl->QueryInterface
                              (g_ZSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_ZSurface3);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_ZBUFFER;
            stageOrLoopCounter = completedStages + 6;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_BackSurface3->lpVtbl->AddAttachedSurface(g_BackSurface3,g_ZSurface3);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_ZBUFFER;
            stageOrLoopCounter = completedStages + 7;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_Direct3D2->lpVtbl->CreateDevice
                              (g_Direct3D2,&g_GraphicsAdapters[adapterIndex].deviceGuid,
                               (IDirectDrawSurface *)g_BackSurface3,&g_Direct3DDevice2);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_CREATE_DEVICE;
            stageOrLoopCounter = completedStages + 8;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_Direct3D2->lpVtbl->CreateViewport
                              (g_Direct3D2,&g_Direct3DViewport2,NULL);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_VIEWPORT;
            stageOrLoopCounter = completedStages + 9;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_Direct3DDevice2->lpVtbl->AddViewport(g_Direct3DDevice2,g_Direct3DViewport2)
            ;
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_VIEWPORT;
            stageOrLoopCounter = completedStages + 10;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            comResult = g_Direct3DDevice2->lpVtbl->SetCurrentViewport
                              (g_Direct3DDevice2,g_Direct3DViewport2);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_VIEWPORT;
            stageOrLoopCounter = completedStages + 0xb;
            if (comResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            Memory_ZeroDwords(sizeof(DDPIXELFORMAT),(void *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0));
            Memory_ZeroDwords(sizeof(DDPIXELFORMAT),(void *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0));
            comResult = g_Direct3DDevice2->lpVtbl->EnumTextureFormats
                              (g_Direct3DDevice2,
                               GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback,
                               NULL);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_TEXTURE_FORMAT;
            stageOrLoopCounter = completedStages + 0xc;
            if (((comResult != 0) ||
                (stageOrLoopCounter = completedStages + 0xd, g_Direct3DOpaqueTextureFormatBitsPerPixel == 0)) ||
               (stageOrLoopCounter = completedStages + 0xe, g_Direct3DAlphaTextureFormatBitsPerPixel == 0))
            goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            formatSource = (uint32_t *)THANDOR_ADDR(g_Direct3DOpaqueTextureFormat,0);
            formatDest = (uint32_t *)THANDOR_ADDR(g_Direct3DSelectedOpaqueTextureFormat,0);
            for (stageOrLoopCounter = sizeof(DDPIXELFORMAT) / sizeof(uint32_t); stageOrLoopCounter != 0;
                 stageOrLoopCounter--) {
              *formatDest = *formatSource;
              formatSource = formatSource + 1;
              formatDest = formatDest + 1;
            }
            formatSource = (uint32_t *)THANDOR_ADDR(g_Direct3DAlphaTextureFormat,0);
            formatDest = (uint32_t *)THANDOR_ADDR(g_Direct3DSelectedAlphaTextureFormat,0);
            for (stageOrLoopCounter = sizeof(DDPIXELFORMAT) / sizeof(uint32_t); stageOrLoopCounter != 0;
                 stageOrLoopCounter--) {
              *formatDest = *formatSource;
              formatSource = formatSource + 1;
              formatDest = formatDest + 1;
            }
            g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_MONOENABLE,0);
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0xf;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_SPECULARENABLE,0);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x10;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x11;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            g_Direct3DDevice2->lpVtbl->GetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,&g_ImmediateVertexCount);
            /* the cull-mode read-back picks the immediate vertex count (3 when culling is off, else 4) */
            errorCodeOrCullMode = g_ImmediateVertexCount;
            g_ImmediateVertexCount = 3;
            if (errorCodeOrCullMode != D3DCULL_NONE) {
              g_ImmediateVertexCount = 4;
            }
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_ZENABLE,D3DZB_TRUE);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x12;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x13;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x14;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x15;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_ANTIALIAS,g_Direct3DAntialiasMode);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x16;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMAG,
                               g_Direct3DTextureFilterMode);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x17;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREMIN,
                               g_Direct3DTextureFilterMode);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x18;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREPERSPECTIVE,
                               g_Direct3DTexturePerspectiveEnabled);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x19;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            g_Direct3DDevice2->lpVtbl->SetRenderState
                      (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,
                       g_PrimitiveRenderStateCache.zWriteEnable);
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_ALPHABLENDENABLE,
                               g_PrimitiveRenderStateCache.alphaBlendEnable);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x1a;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_SRCBLEND,
                               g_PrimitiveRenderStateCache.sourceBlend);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x1b;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_DESTBLEND,
                               g_PrimitiveRenderStateCache.destinationBlend);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x1c;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
            g_BoundTextureHandle = 0;
            renderStateResult = g_Direct3DDevice2->lpVtbl->SetRenderState
                              (g_Direct3DDevice2,D3DRENDERSTATE_TEXTUREHANDLE,0);
            errorCodeOrCullMode = FATAL_ERROR_DIRECT3D_SETUP;
            stageOrLoopCounter = completedStages + 0x1d;
            if (renderStateResult != 0) goto GraphicsDirectDraw_ReleasePartialInitializationAfterFailure;
          }
          g_FramebufferWidth = width;
          g_FramebufferHeight = height;
          g_ActiveGraphicsAdapterIndex = adapterIndex;
          g_DisplayFramebufferAccess.width = width;
          g_DisplayFramebufferAccess.height = height;
          g_DisplayFramebufferAccess.pixels = NULL;
          g_FramebufferAccess = &g_DisplayFramebufferAccess;
          if (bitsPerPixel < 16 + 1) {
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
          displayModeResult = g_GraphicsDisplayModeFinalize(adapterIndex,bitsPerPixel,height,width);
          if (displayModeResult.failed) {
            displayModeResult.failed = true;
            return displayModeResult;
          }
          stageOrLoopCounter = 0x1000; /* texture slots */
          textureSlotCursor = g_GraphicsTextureSlots;
          do {
            if (*textureSlotCursor != NULL) {
              displayModeResult.valueOrError = (uint32_t)GraphicsTexture_CreateStagingTexture(*textureSlotCursor);
            }
            textureSlotCursor = textureSlotCursor + 1;
            stageOrLoopCounter--;
          } while (stageOrLoopCounter != 0);
          exitResult.failed = false;
          exitResult.valueOrError = displayModeResult.valueOrError;
          return exitResult;
        }
      }
    }
  }
GraphicsDirectDraw_ReleasePartialInitializationAfterFailure:
  completedStages = stageOrLoopCounter;
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,completedStages,g_PackageLastErrorPath);
  failureResult.failed = true;
  failureResult.valueOrError = errorCodeOrCullMode;
  return failureResult;
}

