/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/directdraw.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/directdraw.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

static GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = 0;

static GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = 0;

static TH_LEGACY_GUID IID_IDirectDraw2_Local = {.Data1 = 0xB3A6F3E0, .Data2 = 11075, .Data3 = 4559, .Data4 = {162, 222, 0, 170, 0, 185, 51, 86}};

static TH_LEGACY_GUID IID_IDirectDrawSurface3_Local = {.Data1 = 0xDA044E00, .Data2 = 27058, .Data3 = 4560, .Data4 = {161, 213, 0, 170, 0, 184, 223, 187}};

static IDirectDraw *g_DirectDraw = 0;

/* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
uint32_t g_ActiveGraphicsAdapterIndex = 4294967295u;

GraphicsDisplayMode *g_GraphicsDisplayModes = 0;

GraphicsDisplayModeCount g_GraphicsDisplayModeCount = 0;

GraphicsAdapterRecord *g_GraphicsAdapters = 0;

uint32_t g_GraphicsAdapterCount = 0;

IDirectDraw2 *g_DirectDraw2 = 0;

IDirectDrawSurface *g_PrimarySurfaceBase = 0;

IDirectDrawSurface *g_BackSurfaceBase = 0;

IDirectDrawSurface3 *g_PrimarySurface3 = 0;

DDSURFACEDESC_DX6 g_SurfaceDesc = {0};

/* Implementation ownership: graphics/backend/directdraw. */

/* Tells whether the display mode (width, height, bitsPerPixel, adapterIndex) was enumerated
   (g_GraphicsDisplayModes, filled by DirectDraw_EnumDisplayModeCallback): returns false when it was, true
   when not. Used by UiDisplayModeSelection_RefreshEnumeratedOptions (ui/controls/misc.c) to offer only
   available modes. The table is assumed non-empty: the first entry is compared before the count is checked.
*/
Bool8 GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
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


/* Same test as GraphicsDisplayMode_IsEnumerated with the parameters in a different order: returns false when
   the mode was enumerated. Used by FrontendDisplaySettingsPage_UpdateModeActionAvailability
   (ui/frontend/settings.c).
*/
Bool8 DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
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


/* DirectDrawEnumerateA callback: appends a zeroed adapter record with the driver's GUID (all zero for the
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

/* IDirectDraw2::EnumDisplayModes callback (the context is the adapter index): records every plain RGB mode of at
   least 640x480 with 16 or 32 bits per pixel (the depths the software renderer draws) while fewer than 256 modes
   are known. Always continues (returns 1).
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
      if ((modeBitsPerPixel != 16) && (modeBitsPerPixel != 32)) {
        return 1;
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
static Bool8 GraphicsDirectDraw_FailSetupStep(uint32_t failureCode,int completedStages,uint32_t *errorCode)
{
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,completedStages,g_PackageLastErrorPath);
  *errorCode = failureCode;
  return false;
}

/* Compares two adapter GUIDs dword by dword (the original uses REPE CMPSD). */
static Bool8 GraphicsDirectDraw_AdapterGuidsMatch
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

/* Releases every surface of the current DirectDraw mode; when the requested adapter's GUID differs from the active
   one, also releases DirectDraw and marks the backend inactive. */
static void GraphicsDirectDraw_ReleaseModeObjects(FrontendDisplayAdapterIndex adapterIndex)
{
  g_CursorCurrentVisibilityToken = -1;
  if (g_BackSurface3 != NULL) {
    g_BackSurface3->lpVtbl->Release(g_BackSurface3);
    g_BackSurface3 = NULL;
  }
  if (g_BackSurfaceBase != NULL) {
    g_BackSurfaceBase->lpVtbl->Release(g_BackSurfaceBase);
    g_BackSurfaceBase = NULL;
  }
  if (g_PrimarySurface3 != NULL) {
    g_PrimarySurface3->lpVtbl->Release(g_PrimarySurface3);
    g_PrimarySurface3 = NULL;
  }
  if (g_PrimarySurfaceBase != NULL) {
    g_PrimarySurfaceBase->lpVtbl->Release(g_PrimarySurfaceBase);
    g_PrimarySurfaceBase = NULL;
  }
  if (!GraphicsDirectDraw_AdapterGuidsMatch(&g_GraphicsAdapters[g_ActiveGraphicsAdapterIndex].adapterGuid,
                                            &g_GraphicsAdapters[adapterIndex].adapterGuid)) {
    g_ActiveGraphicsAdapterIndex = GRAPHICS_ADAPTER_INDEX_NONE;
    if (g_DirectDraw2 != NULL) {
      g_DirectDraw2->lpVtbl->Release(g_DirectDraw2);
      g_DirectDraw2 = NULL;
    }
    if (g_DirectDraw != NULL) {
      g_DirectDraw->lpVtbl->Release(g_DirectDraw);
      g_DirectDraw = NULL;
    }
  }
}

/* Creates DirectDraw for the adapter (the primary display driver when its GUID is zero), queries IDirectDraw2 and
   sets the cooperative levels (setup steps 0 to 3). Returns false after reporting a failing step. */
static Bool8 GraphicsDirectDraw_CreateDirectDraw(FrontendDisplayAdapterIndex adapterIndex,uint32_t *errorCode)
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
  comResult = g_DirectDraw2->lpVtbl->SetCooperativeLevel
                    (g_DirectDraw2,g_MainWindow,DebugHook_DirectDrawCooperativeLevel(DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE));
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE,3,errorCode);
  }
  return true;
}

/* Creates the primary surface and the back buffer (a system-memory offscreen surface the software renderer draws
   into) with their IDirectDrawSurface3 interfaces, and adds the completed steps to *completedStages. Returns false
   after reporting a failing step. */
static Bool8 GraphicsDirectDraw_CreateSurfaces
          (GraphicsPixelDimension height,GraphicsPixelDimension width,int *completedStages,uint32_t *errorCode)
{
  TH_LEGACY_HRESULT comResult;

  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  g_SurfaceDesc.dwFlags = DDSD_CAPS;
  g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  comResult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,&g_PrimarySurfaceBase,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 1,errorCode);
  }
  comResult = g_PrimarySurfaceBase->lpVtbl->QueryInterface
                    (g_PrimarySurfaceBase,&IID_IDirectDrawSurface3_Local,&g_PrimarySurface3);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 2,errorCode);
  }
  DebugHook_AfterPrimarySurfaceCreated(width,height);
  Memory_ZeroDwords(sizeof g_SurfaceDesc,&g_SurfaceDesc);
  g_SurfaceDesc.dwSize = sizeof g_SurfaceDesc;
  /* the back buffer is a system-memory offscreen surface */
  g_SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  g_SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  g_SurfaceDesc.dwWidth = width;
  g_SurfaceDesc.dwHeight = height;
  comResult = g_DirectDraw2->lpVtbl->CreateSurface(g_DirectDraw2,&g_SurfaceDesc,&g_BackSurfaceBase,NULL);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 3,errorCode);
  }
  comResult = g_BackSurfaceBase->lpVtbl->QueryInterface
                    (g_BackSurfaceBase,&IID_IDirectDrawSurface3_Local,&g_BackSurface3);
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES,*completedStages + 4,errorCode);
  }
  *completedStages = *completedStages + 5;
  return true;
}

/* Reads the primary surface's pixel format into g_SurfaceDesc and checks that it has red, green and blue masks.
   Returns false after reporting a failing step. */
static Bool8 GraphicsDirectDraw_ReadPrimaryPixelFormat(int completedStages,uint32_t *errorCode)
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

/* g_GraphicsSetDisplayMode for DirectDraw: releases the surfaces of the current mode (and DirectDraw itself when
   the adapter changes), creates DirectDraw and the primary and back surfaces, then publishes the framebuffer,
   selects the 16- or 32-bit software blitters and calls the chained finalize hook. Returns true on success. A
   failing step returns false with its FATAL_ERROR_DIRECTDRAW_... code in *errorCode and leaves the number of
   completed steps as text in g_PackageLastErrorPath; a failing finalize hook passes its own error through.
*/
Bool8 GraphicsDirectDraw_ApplyDisplayModeAndCreateResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width,uint32_t *errorCode)

{
  TH_LEGACY_HRESULT comResult;
  int completedStages;

  completedStages = 0;
  if (g_ActiveGraphicsAdapterIndex != GRAPHICS_ADAPTER_INDEX_NONE) {
    /* DirectDraw backend already active: release every surface, then keep the DirectDraw objects only if the
       requested adapter GUID matches the active one. */
    GraphicsDirectDraw_ReleaseModeObjects(adapterIndex);
  }
  if (g_ActiveGraphicsAdapterIndex == GRAPHICS_ADAPTER_INDEX_NONE) {
    /* No backend (first call, or switched to another adapter): create it. */
    if (!GraphicsDirectDraw_CreateDirectDraw(adapterIndex,errorCode)) {
      return false;
    }
    completedStages = 4;
    g_ActiveGraphicsAdapterIndex = adapterIndex;
  }
  if (DebugHook_Windowed()) {
    comResult = 0; /* windowed mode (developer tools): the desktop keeps its mode and colour depth */
  }
  else {
    comResult = g_DirectDraw2->lpVtbl->SetDisplayMode(g_DirectDraw2,width,height,bitsPerPixel,0,0);
  }
  if (comResult != 0) {
    return GraphicsDirectDraw_FailSetupStep(FATAL_ERROR_DIRECTDRAW_SET_DISPLAY_MODE,completedStages,errorCode);
  }
  if (!GraphicsDirectDraw_CreateSurfaces(height,width,&completedStages,errorCode)) {
    return false;
  }
  if (!GraphicsDirectDraw_ReadPrimaryPixelFormat(completedStages,errorCode)) {
    return false;
  }
  bitsPerPixel = DebugHook_SurfaceBitsPerPixel(bitsPerPixel,width,height);
  GraphicsDirectDraw_StoreSoftwarePixelFormat();
  GraphicsDirectDraw_PublishFramebuffer(adapterIndex,bitsPerPixel,height,width);
  return g_GraphicsDisplayModeFinalize(adapterIndex,bitsPerPixel,height,width,errorCode);
}

