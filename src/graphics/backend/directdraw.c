/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/directdraw.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/directdraw.h>
#include <thandor/thandor.h>
#ifdef THANDOR_TEST_AIDS
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>
#endif

/* Implementation ownership: graphics/backend/directdraw. */
#ifdef THANDOR_TEST_AIDS

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
#endif

/* Address: 0x00423CF0.
   Tells whether the display mode (width, height, bitsPerPixel, adapterIndex) was enumerated
   (g_GraphicsDisplayModes, filled by DirectDraw_EnumDisplayModeCallback): CF clear (false) when it was, CF set
   (true) when not. Used by UiDisplayModeSelection_RefreshEnumeratedOptions (ui/controls/misc.c) to offer only
   available modes. The table is assumed non-empty: the first entry is compared before the count is checked.
*/
bool GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
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
bool DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
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
  uint32_t *zeroCursor;
  uint32_t *adapterRecordCursor;

  if (g_GraphicsAdapterCount < GRAPHICS_ADAPTER_CAPACITY) {
    adapterRecord = g_GraphicsAdapters + g_GraphicsAdapterCount;
    zeroCursor = (uint32_t *)adapterRecord;
    for (dwordsRemaining = sizeof(GraphicsAdapterRecord) / sizeof(uint32_t); dwordsRemaining != 0;
         dwordsRemaining--) {
      *zeroCursor = 0;
      zeroCursor++;
    }
    if (adapterGuid != NULL) {
      adapterRecordCursor = (uint32_t *)&adapterRecord->adapterGuid;
      for (guidDwordsRemaining = sizeof(TH_LEGACY_GUID) / sizeof(uint32_t); guidDwordsRemaining != 0;
           guidDwordsRemaining--) {
        *adapterRecordCursor = *(uint32_t *)adapterGuid;
        adapterGuid = (TH_LEGACY_GUID *)((uint32_t *)adapterGuid + 1);
        adapterRecordCursor++;
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

/* Failure exit of GraphicsDirectDraw_ApplyDisplayModeAndCreateResources: leaves the number of completed setup steps
   as text in g_PackageLastErrorPath, stores the failing step's error code in *errorCode and returns false. */
static bool GraphicsDirectDraw_FailSetupStep(uint32_t failureCode,int completedStages,uint32_t *errorCode)
{
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,completedStages,g_PackageLastErrorPath);
  *errorCode = failureCode;
  return false;
}

/* Compares two adapter GUIDs dword by dword (the original uses REPE CMPSD). */
static bool GraphicsDirectDraw_AdapterGuidsMatch
          (const struct TH_LEGACY_GUID *activeGuid,const struct TH_LEGACY_GUID *requestedGuid)
{
  const uint32_t *activeWords;
  const uint32_t *requestedWords;
  int wordIndex;

  activeWords = (const uint32_t *)activeGuid;
  requestedWords = (const uint32_t *)requestedGuid;
  for (wordIndex = 0; wordIndex < (int)(sizeof(struct TH_LEGACY_GUID) / sizeof(uint32_t)); wordIndex++) {
    if (activeWords[wordIndex] != requestedWords[wordIndex]) {
      return false;
    }
  }
  return true;
}

/* Releases the textures' device objects and every surface and Direct3D object of the current DirectDraw mode; when
   the requested adapter's GUID differs from the active one, also releases DirectDraw and marks the backend inactive.
   Returns the result of the last COM Release (lastReleaseResult when nothing was released). */
static uint32_t GraphicsDirectDraw_ReleaseModeObjects
          (FrontendDisplayAdapterIndex adapterIndex,uint32_t lastReleaseResult)
{
  int slotIndex;

  for (slotIndex = 0; slotIndex < GRAPHICS_TEXTURE_SLOT_CAPACITY; slotIndex++) {
    if (g_GraphicsTextureSlots[slotIndex] != NULL) {
      GraphicsTexture_ReleaseObjects(g_GraphicsTextureSlots[slotIndex]);
    }
  }
  g_LastViewportRect.x1 = 0;
  g_LastViewportRect.y1 = 0;
  g_LastViewportRect.x2 = 0;
  g_LastViewportRect.y2 = 0;
  if (g_Direct3DViewport2 != NULL) {
    lastReleaseResult = (uint32_t)g_Direct3DViewport2->lpVtbl->Release(g_Direct3DViewport2);
    g_Direct3DViewport2 = NULL;
  }
  if (g_ZSurface3 != NULL) {
    lastReleaseResult = (uint32_t)g_ZSurface3->lpVtbl->Release(g_ZSurface3);
    g_ZSurface3 = NULL;
  }
  if (g_ZSurfaceBase != NULL) {
    lastReleaseResult = (uint32_t)g_ZSurfaceBase->lpVtbl->Release(g_ZSurfaceBase);
    g_ZSurfaceBase = NULL;
  }
  if (g_Direct3DDevice2 != NULL) {
    lastReleaseResult = (uint32_t)g_Direct3DDevice2->lpVtbl->Release(g_Direct3DDevice2);
    g_Direct3DDevice2 = NULL;
  }
  if (g_Direct3D2 != NULL) {
    lastReleaseResult = (uint32_t)g_Direct3D2->lpVtbl->Release(g_Direct3D2);
    g_Direct3D2 = NULL;
  }
  g_CursorCurrentVisibilityToken = -1;
  g_CursorAlternateVisibilityToken = -1;
  if (g_BackSurface3 != NULL) {
    lastReleaseResult = (uint32_t)g_BackSurface3->lpVtbl->Release(g_BackSurface3);
    g_BackSurface3 = NULL;
  }
  if (g_BackSurfaceBase != NULL) {
    lastReleaseResult = (uint32_t)g_BackSurfaceBase->lpVtbl->Release(g_BackSurfaceBase);
    g_BackSurfaceBase = NULL;
  }
  if (g_PrimarySurface3 != NULL) {
    lastReleaseResult = (uint32_t)g_PrimarySurface3->lpVtbl->Release(g_PrimarySurface3);
    g_PrimarySurface3 = NULL;
  }
  if (g_PrimarySurfaceBase != NULL) {
    lastReleaseResult = (uint32_t)g_PrimarySurfaceBase->lpVtbl->Release(g_PrimarySurfaceBase);
    g_PrimarySurfaceBase = NULL;
  }
  if (!GraphicsDirectDraw_AdapterGuidsMatch(&g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid,
                                            &g_GraphicsAdapters[adapterIndex].adapterGuid)) {
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
    if (g_DirectDraw2 != NULL) {
      lastReleaseResult = (uint32_t)g_DirectDraw2->lpVtbl->Release(g_DirectDraw2);
      g_DirectDraw2 = NULL;
    }
    if (g_DirectDraw != NULL) {
      lastReleaseResult = (uint32_t)g_DirectDraw->lpVtbl->Release(g_DirectDraw);
      g_DirectDraw = NULL;
    }
  }
  return lastReleaseResult;
}

/* Creates DirectDraw for the adapter (the primary display driver when its GUID is zero), queries IDirectDraw2 and
   sets the cooperative levels (setup steps 0 to 3). Returns false after reporting a failing step. */
static bool GraphicsDirectDraw_CreateDirectDraw(FrontendDisplayAdapterIndex adapterIndex,uint32_t *errorCode)
{
  GraphicsAdapterRecord *adapterRecord;
  TH_LEGACY_HRESULT comResult;

  adapterRecord = g_GraphicsAdapters + adapterIndex;
  if ((adapterRecord->adapterGuid).Data1 == 0) {
    adapterRecord = NULL; /* the primary display driver */
  }
  comResult = pDirectDrawCreate(&adapterRecord->adapterGuid,&g_DirectDraw,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE,0,errorCode);
  }
  comResult = g_DirectDraw->lpVtbl->SetCooperativeLevel(g_DirectDraw,g_MainWindow,DDSCL_NORMAL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE,1,errorCode);
  }
  comResult = g_DirectDraw->lpVtbl->QueryInterface(g_DirectDraw,&IID_IDirectDraw2_Local,&g_DirectDraw2);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE,2,errorCode);
  }
#ifdef THANDOR_TEST_AIDS
  comResult = g_DirectDraw2->lpVtbl->SetCooperativeLevel(g_DirectDraw2,g_MainWindow,
                                                      Thandor_TestAidWindowed() ? DDSCL_NORMAL :
                                                      DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
#else
  comResult = g_DirectDraw2->lpVtbl->SetCooperativeLevel(g_DirectDraw2,g_MainWindow,
                                                      DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
#endif
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE,3,errorCode);
  }
  return true;
}

/* Creates the primary surface (for Direct3D a flipping chain with one back buffer) and the back buffer (for Direct3D
   the attached one, otherwise a system-memory offscreen surface) with their IDirectDrawSurface3 interfaces, and adds
   the completed steps to *completedStages. Returns false after reporting a failing step. */
static bool GraphicsDirectDraw_CreateSurfaces
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsPixelDimension height,GraphicsPixelDimension width,
          int *completedStages,uint32_t *errorCode)
{
  TH_LEGACY_HRESULT comResult;

  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  g_SurfaceDesc.dwFlags = DDSD_CAPS;
  g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 != 0) {
    /* Direct3D: flipping primary chain with one back buffer as the render target */
    g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    g_SurfaceDesc.dwBackBufferCount = 1;
  }
  comResult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,&g_PrimarySurfaceBase,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 1,errorCode);
  }
  comResult = g_PrimarySurfaceBase->lpVtbl->QueryInterface
                    (g_PrimarySurfaceBase,&IID_IDirectDrawSurface3_Local,&g_PrimarySurface3);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 2,errorCode);
  }
#ifdef THANDOR_TEST_AIDS
  if (Thandor_TestAidWindowed()) {
    GraphicsDirectDraw_TestAidAttachWindowClipper(width,height);
  }
#endif
  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 == 0) {
    g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
    /* no Direct3D: the back buffer is a system-memory offscreen surface */
    g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    g_SurfaceDesc.dwWidth = width;
    g_SurfaceDesc.dwHeight = height;
    comResult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,&g_BackSurfaceBase,NULL);
    if (comResult != 0) {
      return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 3,
                                              errorCode);
    }
    comResult = g_BackSurfaceBase->lpVtbl->QueryInterface
                      (g_BackSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_BackSurface3);
    if (comResult != 0) {
      return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 4,
                                              errorCode);
    }
    *completedStages = *completedStages + 5;
  }
  else {
    g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    comResult = g_PrimarySurface3->lpVtbl->GetAttachedSurface
                      (g_PrimarySurface3,&g_SurfaceDesc.ddsCaps,&g_BackSurface3);
    if (comResult != 0) {
      return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 3,
                                              errorCode);
    }
    *completedStages = *completedStages + 4;
  }
  return true;
}

/* Reads the primary surface's pixel format into g_SurfaceDesc and checks that it has red, green and blue masks.
   Returns false after reporting a failing step. */
static bool GraphicsDirectDraw_ReadPrimaryPixelFormat(int completedStages,uint32_t *errorCode)
{
  TH_LEGACY_HRESULT comResult;

  Memory_ZeroDwords(sizeof g_SurfaceDesc.ddpfPixelFormat,&g_SurfaceDesc.ddpfPixelFormat);
  g_SurfaceDesc.ddpfPixelFormat.dwSize = sizeof g_SurfaceDesc.ddpfPixelFormat;
  comResult = g_PrimarySurface3->lpVtbl->GetPixelFormat(g_PrimarySurface3,&g_SurfaceDesc.ddpfPixelFormat);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT,completedStages,errorCode);
  }
  if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask == 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT,completedStages + 1,errorCode);
  }
  if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask == 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT,completedStages + 2,errorCode);
  }
  if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask == 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_PIXEL_FORMAT,completedStages + 3,errorCode);
  }
  return true;
}

/* Stores the primary surface's channel masks in g_SoftwarePixelFormatConfig with each channel's shift (lowest set
   bit) and bit count (highest set bit + 1 - shift). */
static void GraphicsDirectDraw_StoreSoftwarePixelFormat(void)
{
  int highestBit;

  g_SoftwarePixelFormatConfig.redMask = g_SurfaceDesc.ddpfPixelFormat.dwRBitMask;
  g_SoftwarePixelFormatConfig.greenMask = g_SurfaceDesc.ddpfPixelFormat.dwGBitMask;
  g_SoftwarePixelFormatConfig.blueMask = g_SurfaceDesc.ddpfPixelFormat.dwBBitMask;
  g_SoftwarePixelFormatConfig.redShift = 0;
  if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
    for (; (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >> g_SoftwarePixelFormatConfig.redShift & 1) == 0;
         g_SoftwarePixelFormatConfig.redShift++) {
    }
  }
  g_SoftwarePixelFormatConfig.greenShift = 0;
  if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
    for (; (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >> g_SoftwarePixelFormatConfig.greenShift & 1) == 0;
         g_SoftwarePixelFormatConfig.greenShift++) {
    }
  }
  g_SoftwarePixelFormatConfig.blueShift = 0;
  if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
    for (; (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >> g_SoftwarePixelFormatConfig.blueShift & 1) == 0;
         g_SoftwarePixelFormatConfig.blueShift++) {
    }
  }
  highestBit = 31; /* from the top bit of the channel mask */
  if (g_SurfaceDesc.ddpfPixelFormat.dwRBitMask != 0) {
    for (; g_SurfaceDesc.ddpfPixelFormat.dwRBitMask >> highestBit == 0; highestBit--) {
    }
  }
  g_SoftwarePixelFormatConfig.redBitCount = (highestBit + 1) - g_SoftwarePixelFormatConfig.redShift;
  highestBit = 31; /* from the top bit of the channel mask */
  if (g_SurfaceDesc.ddpfPixelFormat.dwGBitMask != 0) {
    for (; g_SurfaceDesc.ddpfPixelFormat.dwGBitMask >> highestBit == 0; highestBit--) {
    }
  }
  g_SoftwarePixelFormatConfig.greenBitCount = (highestBit + 1) - g_SoftwarePixelFormatConfig.greenShift;
  highestBit = 31; /* from the top bit of the channel mask */
  if (g_SurfaceDesc.ddpfPixelFormat.dwBBitMask != 0) {
    for (; g_SurfaceDesc.ddpfPixelFormat.dwBBitMask >> highestBit == 0; highestBit--) {
    }
  }
  g_SoftwarePixelFormatConfig.blueBitCount = (highestBit + 1) - g_SoftwarePixelFormatConfig.blueShift;
}

/* Creates IDirect3D2, the Z-buffer (attached to the back buffer), the device and its viewport, and selects the
   opaque and alpha texture formats (setup steps completedStages + 4 to + 14). Returns false after reporting a
   failing step. */
static bool GraphicsDirect3D_CreateDeviceAndViewport
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsPixelDimension height,GraphicsPixelDimension width,
          int completedStages,uint32_t *errorCode)
{
  GraphicsAdapterRecord *adapterRecord;
  D3DDEVICEDESC_DX6 *hardwareDeviceDesc;
  TH_LEGACY_HRESULT comResult;

  adapterRecord = g_GraphicsAdapters + adapterIndex;
  comResult = g_DirectDraw2->lpVtbl->QueryInterface(g_DirectDraw2,&IID_IDirect3D2_Local,&g_Direct3D2);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_SETUP,completedStages + 4,errorCode);
  }
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
  comResult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,&g_ZSurfaceBase,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_ZBUFFER,completedStages + 5,errorCode);
  }
  comResult = g_ZSurfaceBase->lpVtbl->QueryInterface(g_ZSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_ZSurface3);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_ZBUFFER,completedStages + 6,errorCode);
  }
  comResult = g_BackSurface3->lpVtbl->AddAttachedSurface(g_BackSurface3,g_ZSurface3);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_ZBUFFER,completedStages + 7,errorCode);
  }
  comResult = g_Direct3D2->lpVtbl->CreateDevice
                    (g_Direct3D2,&g_GraphicsAdapters[adapterIndex].deviceGuid,
                     (IDirectDrawSurface *)g_BackSurface3,&g_Direct3DDevice2);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_CREATE_DEVICE,completedStages + 8,errorCode);
  }
  comResult = g_Direct3D2->lpVtbl->CreateViewport(g_Direct3D2,&g_Direct3DViewport2,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_VIEWPORT,completedStages + 9,errorCode);
  }
  comResult = g_Direct3DDevice2->lpVtbl->AddViewport(g_Direct3DDevice2,g_Direct3DViewport2);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_VIEWPORT,completedStages + 10,errorCode);
  }
  comResult = g_Direct3DDevice2->lpVtbl->SetCurrentViewport(g_Direct3DDevice2,g_Direct3DViewport2);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_VIEWPORT,completedStages + 11,errorCode);
  }
  Memory_ZeroDwords(sizeof(DDPIXELFORMAT),&g_Direct3DOpaqueTextureFormat);
  Memory_ZeroDwords(sizeof(DDPIXELFORMAT),&g_Direct3DAlphaTextureFormat);
  comResult = g_Direct3DDevice2->lpVtbl->EnumTextureFormats
                    (g_Direct3DDevice2,GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_TEXTURE_FORMAT,completedStages + 12,errorCode);
  }
  if (g_Direct3DOpaqueTextureFormat.dwRGBBitCount == 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_TEXTURE_FORMAT,completedStages + 13,errorCode);
  }
  if (g_Direct3DAlphaTextureFormat.dwRGBBitCount == 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_TEXTURE_FORMAT,completedStages + 14,errorCode);
  }
  g_Direct3DSelectedOpaqueTextureFormat = g_Direct3DOpaqueTextureFormat;
  g_Direct3DSelectedAlphaTextureFormat = g_Direct3DAlphaTextureFormat;
  return true;
}

/* One checked render state of the Direct3D setup: a failure is reported as FATAL_ERROR_DIRECT3D_SETUP at
   failureStage. */
static bool GraphicsDirect3D_SetSetupRenderState
          (D3DRENDERSTATETYPE_DX6 renderState,uint32_t value,int failureStage,uint32_t *errorCode)
{
  if (g_Direct3DDevice2->lpVtbl->SetRenderState(g_Direct3DDevice2,renderState,value) != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECT3D_SETUP,failureStage,errorCode);
  }
  return true;
}

/* Sets the device's initial render states (setup steps completedStages + 15 to + 29; the mono-enable and
   z-write-enable results are not checked) and picks the immediate vertex count from the cull mode read back.
   Returns false after reporting a failing step. */
static bool GraphicsDirect3D_ApplyInitialRenderStates(int completedStages,uint32_t *errorCode)
{
  uint32_t cullMode;

  g_Direct3DDevice2->lpVtbl->SetRenderState(g_Direct3DDevice2,D3DRENDERSTATE_MONOENABLE,0);
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD,completedStages + 15,
                                            errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_SPECULARENABLE,0,completedStages + 16,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE,completedStages + 17,
                                            errorCode)) {
    return false;
  }
  g_Direct3DDevice2->lpVtbl->GetRenderState(g_Direct3DDevice2,D3DRENDERSTATE_CULLMODE,&g_ImmediateVertexCount);
  /* the cull-mode read-back picks the immediate vertex count (3 when culling is off, else 4) */
  cullMode = g_ImmediateVertexCount;
  g_ImmediateVertexCount = 3;
  if (cullMode != D3DCULL_NONE) {
    g_ImmediateVertexCount = 4;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_TRUE,completedStages + 18,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_ZFUNC,D3DCMP_LESSEQUAL,completedStages + 19,
                                            errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID,completedStages + 20,
                                            errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA,
                                            completedStages + 21,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_ANTIALIAS,g_Direct3DAntialiasMode,completedStages + 22,
                                            errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_TEXTUREMAG,g_Direct3DTextureFilterMode,
                                            completedStages + 23,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_TEXTUREMIN,g_Direct3DTextureFilterMode,
                                            completedStages + 24,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,g_Direct3DTexturePerspectiveEnabled,
                                            completedStages + 25,errorCode)) {
    return false;
  }
  g_Direct3DDevice2->lpVtbl->SetRenderState
            (g_Direct3DDevice2,D3DRENDERSTATE_ZWRITEENABLE,g_PrimitiveRenderStateCache.zWriteEnable);
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                            g_PrimitiveRenderStateCache.alphaBlendEnable,completedStages + 26,
                                            errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_SRCBLEND,g_PrimitiveRenderStateCache.sourceBlend,
                                            completedStages + 27,errorCode)) {
    return false;
  }
  if (!GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_DESTBLEND,g_PrimitiveRenderStateCache.destinationBlend,
                                            completedStages + 28,errorCode)) {
    return false;
  }
  g_BoundTextureHandle = 0;
  return GraphicsDirect3D_SetSetupRenderState(D3DRENDERSTATE_TEXTUREHANDLE,0,completedStages + 29,errorCode);
}

/* Publishes the display framebuffer of the new mode and selects the 16- or 32-bit software blitters. */
static void GraphicsDirectDraw_PublishFramebuffer
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width)
{
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
    g_GraphicsTextureSourceStretchDirectColorBilinear = SoftwareTextureSource_StretchDirectColorBilinear16;
    g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = SoftwareTextureSource_BlitIntegerScaledSourceAlpha16;
    g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = SoftwareTextureSource_BlitSourceAlphaPaletteBank16;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha = SoftwareTextureSource_BlitModulatedSourceAlpha16;
    g_GraphicsTextureSourceBlitSaturatedAddRgb = SoftwareTextureSource_BlitSaturatedAddRgb16;
    g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd = SoftwareTextureSource_BlitHalfRgbSaturatedAdd16;
    g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb16;
  }
  else {
    g_DisplayFramebufferAccess.bytesPerPixel = SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT;
    g_GraphicsFramebufferCaptureRegion = GraphicsFramebuffer_CaptureRegion32Bit;
    g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha32;
    g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb32;
    g_GraphicsTextureSourceStretchDirectColorBilinear = SoftwareTextureSource_StretchDirectColorBilinear32;
    g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = SoftwareTextureSource_BlitIntegerScaledSourceAlpha32;
    g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = SoftwareTextureSource_BlitSourceAlphaPaletteBank32;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha = SoftwareTextureSource_BlitModulatedSourceAlpha32;
    g_GraphicsTextureSourceBlitSaturatedAddRgb = SoftwareTextureSource_BlitSaturatedAddRgb32;
    g_GraphicsTextureSourceBlitHalfRgbSaturatedAdd = SoftwareTextureSource_BlitHalfRgbSaturatedAdd32;
    g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb32;
  }
  g_GraphicsFramebufferPresent = GraphicsFramebuffer_Present;
}

/* Address: 0x00578920.
   g_GraphicsSetDisplayMode for DirectDraw/Direct3D (and the switch to Glide): releases the surfaces and devices of
   the current mode (and DirectDraw itself when the adapter changes), creates DirectDraw, the primary and back
   surfaces and, for a Direct3D adapter, the Z-buffer, device, viewport, texture formats and render states, then
   publishes the framebuffer, selects the 16- or 32-bit software blitters, calls the chained finalize hook and
   recreates the textures. Returns true on success. A failing step returns false with its
   FATAL_ERROR_DIRECTDRAW_... or FATAL_ERROR_DIRECT3D_... code in *errorCode and leaves the number of completed
   steps as text in g_PackageLastErrorPath; a failing finalize hook passes its own error through.
*/
bool GraphicsDirectDraw_ApplyDisplayModeAndCreateResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width,uint32_t *errorCode)

{
  TH_LEGACY_HRESULT comResult;
  bool glideFailed;
  uint32_t glideFailureValue; /* the value the Glide path hands back on failure, see the quirk below */
  int completedStages;
  int slotIndex;

  completedStages = 0;
#ifdef THANDOR_TEST_AIDS
  if (Thandor_TestAidWindowed()) {
    adapterIndex = GraphicsDirectDraw_TestAidWindowedAdapter(adapterIndex);
  }
#endif
  /* Original quirk: error code of a failing Glide switch. GraphicsGlide3_ApplyDisplayModeAndInitializeResources
     preserves EAX, so the original hands back whatever EAX held before the call (the caller's EAX on the first
     call, otherwise the last COM Release result). 0x1a is the mode error the Glide callee computes but discards,
     which gives a meaningful message. */
  glideFailureValue = FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE;
  if ((g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE) &&
     (g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE)) {
    /* Leaving the Glide backend: shut it down and create the new backend from scratch. */
    Glide3_Shutdown();
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
  }
  if (g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE) {
    /* DirectDraw backend already active: release every surface and device object, then keep the DirectDraw
       objects only if the requested adapter GUID matches the active one. */
    glideFailureValue = GraphicsDirectDraw_ReleaseModeObjects(adapterIndex,glideFailureValue);
  }
  if (g_ActiveGraphicsAdapterIndex == GRAPHICS_ADAPTER_INDEX_NONE) {
    /* No backend (first call, or switched away from Glide / to another adapter): create it. */
    if (g_GraphicsAdapters[adapterIndex].adapterGuid.Data1 == GRAPHICS_ADAPTER_GUID_GLIDE) {
      glideFailed = GraphicsGlide3_ApplyDisplayModeAndInitializeResources(adapterIndex,bitsPerPixel,height,width);
      if (glideFailed) {
        *errorCode = glideFailureValue;
        return false;
      }
      return true;
    }
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
    if (!GraphicsDirectDraw_CreateDirectDraw(adapterIndex,errorCode)) {
      return false;
    }
    completedStages = 4;
    g_ActiveGraphicsAdapterIndex = adapterIndex;
  }
#ifdef THANDOR_TEST_AIDS
  if (Thandor_TestAidWindowed()) {
    comResult = 0; /* windowed test aid: the desktop keeps its mode (and colour depth) */
  }
  else {
    comResult = g_DirectDraw2->lpVtbl->SetDisplayMode(g_DirectDraw2,width,height,bitsPerPixel,0,0);
  }
#else
  comResult = g_DirectDraw2->lpVtbl->SetDisplayMode(g_DirectDraw2,width,height,bitsPerPixel,0,0);
#endif
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE,completedStages,errorCode);
  }
  if (!GraphicsDirectDraw_CreateSurfaces(adapterIndex,height,width,&completedStages,errorCode)) {
    return false;
  }
  if (!GraphicsDirectDraw_ReadPrimaryPixelFormat(completedStages,errorCode)) {
    return false;
  }
#ifdef THANDOR_TEST_AIDS
  if (Thandor_TestAidWindowed() && (bitsPerPixel != g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount)) {
    /* windowed test aid: the surfaces have the desktop's depth, and the blitters chosen below, the
       renderer's queue (bytesPerPixel) and the pixel packing must all follow the surfaces */
    Thandor_Log("test aid: windowed %ux%u uses the desktop depth of %u bits instead of %u",
                (unsigned)width,(unsigned)height,(unsigned)g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount,
                (unsigned)bitsPerPixel);
    bitsPerPixel = g_SurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
  }
#endif
  GraphicsDirectDraw_StoreSoftwarePixelFormat();
  if (g_GraphicsAdapters[adapterIndex].deviceGuid.Data1 != 0) {
    if (!GraphicsDirect3D_CreateDeviceAndViewport(adapterIndex,height,width,completedStages,errorCode)) {
      return false;
    }
    if (!GraphicsDirect3D_ApplyInitialRenderStates(completedStages,errorCode)) {
      return false;
    }
  }
  GraphicsDirectDraw_PublishFramebuffer(adapterIndex,bitsPerPixel,height,width);
  if (!g_GraphicsDisplayModeFinalize(adapterIndex,bitsPerPixel,height,width,errorCode)) {
    return false;
  }
  for (slotIndex = 0; slotIndex < GRAPHICS_TEXTURE_SLOT_CAPACITY; slotIndex++) {
    if (g_GraphicsTextureSlots[slotIndex] != NULL) {
      GraphicsTexture_CreateStagingTexture(g_GraphicsTextureSlots[slotIndex]);
    }
  }
  return true;
}

