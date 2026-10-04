/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/device.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/core/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

SoftwareDisplayModeHookProc *g_GraphicsDisplayModeFinalize = 0;

int32_t g_GraphicsBackendAccessState = -0x1;

/* allocated by Graphics_Init but no longer read (see there) */
static DirectDrawPaletteEntry *g_TexturePaletteEntries = 0;

static DirectDrawEnumerateA *pDirectDrawEnumerateA = 0;

static char sz_DDRAW[6] = "DDRAW";

static char sz_DirectDrawCreate[17] = "DirectDrawCreate";

static char sz_DirectDrawEnumerateA[21] = "DirectDrawEnumerateA";

DirectDrawCreate *pDirectDrawCreate = 0;

static GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = 0;

static GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = 0;

/* Implementation ownership: graphics/core/device. */













/* Graphics_Init's first step, shared with the SDL3 backend (SdlVideo_Init): allocates and clears the texture-slot
   and palette tables and allocates the empty adapter and display-mode tables, in this order (the arena layout
   the texture-set sort depends on). Returns 0 or the allocator's error code. */
uint32_t Graphics_AllocateTables(void)

{
  int remainingDwords;
  void *allocation;
  uint32_t *zeroCursor;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsTextureSlots = (GraphicsTextureResource **)allocation;
  zeroCursor = (uint32_t *)allocation;
  for (remainingDwords = GRAPHICS_TEXTURE_SLOT_CAPACITY * sizeof(GraphicsTextureResource *) / 4; remainingDwords != 0;
       remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(256 * sizeof(DirectDrawPaletteEntry),&allocation); /* 256 palette entries */
  if (allocError != 0) {
    return allocError;
  }
  g_TexturePaletteEntries = (DirectDrawPaletteEntry *)allocation;
  zeroCursor = (uint32_t *)allocation;
  for (remainingDwords = 256; remainingDwords != 0; remainingDwords--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  allocError = g_MemoryApi.alloc(GRAPHICS_ADAPTER_CAPACITY * sizeof(GraphicsAdapterRecord),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsAdapterCount = 0;
  g_GraphicsAdapters = (GraphicsAdapterRecord *)allocation;
  allocError = g_MemoryApi.alloc(GRAPHICS_DISPLAY_MODE_CAPACITY * sizeof(GraphicsDisplayMode),&allocation);
  if (allocError != 0) {
    return allocError;
  }
  g_GraphicsDisplayModeCount = 0;
  g_GraphicsDisplayModes = (GraphicsDisplayMode *)allocation;
  return 0;
}


/* Allocates the texture-slot, palette, adapter and display-mode tables, enumerates the DirectDraw adapters
   (every one is a software renderer device) and their display modes and installs the DirectDraw surface
   backend in the g_Graphics* slots. The display-mode hook installed before (the software renderer's) is kept
   as g_GraphicsDisplayModeFinalize. Returns 0 on success, otherwise the error code of the failing step (never
   0). The palette table is no longer read (only the original's hardware texture upload used it); it is still
   allocated, like the texture slots, so the arena layout and with it the texture-set addresses that
   GraphicsPrimitiveQueue_RadixSortForRendering sorts opaque packets by stay as they were.
*/
uint32_t __cdecl Graphics_Init(void)

{
  TH_LEGACY_HRESULT hresult;
  SoftwareDisplayModeHookProc *displayModeHook;
  uint32_t remainingAdapters;
  uint32_t displayAdapterIndex;
  GraphicsAdapterRecord *adapter;
  struct TH_LEGACY_GUID *driverGuid;
  HINSTANCE ddrawModule;
  uint32_t allocError;
  uint32_t resolveError;
  IDirectDraw *directDraw;

  allocError = Graphics_AllocateTables();
  if (allocError != 0) {
    return allocError;
  }
  ddrawModule = DynDLL_Load(sz_DDRAW);
  if (ddrawModule == NULL) {
    return FATAL_ERROR_DLL_LOAD_FAILED;
  }
  resolveError = DynAPI_Resolve((void **)&pDirectDrawCreate,ddrawModule,sz_DirectDrawCreate);
  if (resolveError != 0) {
    return resolveError;
  }
  resolveError = DynAPI_Resolve((void **)&pDirectDrawEnumerateA,ddrawModule,sz_DirectDrawEnumerateA);
  if (resolveError != 0) {
    return resolveError;
  }
  hresult = pDirectDrawEnumerateA(DirectDraw_EnumAdapterCallback,NULL);
  if ((hresult != 0) || (g_GraphicsAdapterCount == 0)) {
    return FATAL_ERROR_DIRECTDRAW_NO_ADAPTER;
  }
  /* collect the display modes, tagged with the adapter index */
  displayAdapterIndex = 0;
  remainingAdapters = g_GraphicsAdapterCount;
  adapter = g_GraphicsAdapters;
  do {
    driverGuid = &adapter->adapterGuid;
    if ((adapter->adapterGuid).Data1 == 0) {
      driverGuid = NULL; /* primary display driver: NULL GUID */
    }
    hresult = pDirectDrawCreate(driverGuid,&directDraw,NULL);
    if (hresult == 0) {
      directDraw->lpVtbl->EnumDisplayModes
                (directDraw,0,NULL,displayAdapterIndex,
                 /* signature differs: the callback takes the context as FrontendDisplayAdapterIndex (int) */
                 (int32_t (__stdcall *)(DDSURFACEDESC_DX6 *,uint32_t))DirectDraw_EnumDisplayModeCallback);
      directDraw->lpVtbl->Release(directDraw);
    }
    displayAdapterIndex++;
    adapter++;
    remainingAdapters--;
  } while (remainingAdapters != 0);
  /* the display-mode hook installed before (the software renderer's) */
  displayModeHook = g_GraphicsSetDisplayMode;
  if (g_GraphicsDisplayModeCount == 0) {
    return FATAL_ERROR_DIRECTDRAW_NO_DISPLAY_MODE;
  }
  /* g_GraphicsSetViewportAndClearDepth, g_GraphicsDrawPrimitiveQueue, g_GraphicsBeginScene/EndScene and the
     texture refresh/rebuild slots keep their software renderer defaults */
  /* signature differs: the adapter index is FrontendDisplayAdapterIndex (int), not uint32_t */
  g_GraphicsSetDisplayMode = (SoftwareDisplayModeHookProc *)GraphicsDirectDraw_ApplyDisplayModeAndCreateResources;
  g_GraphicsFramebufferBeginAccess = GraphicsFramebuffer_BeginAccess;
  g_GraphicsFramebufferEndAccess = GraphicsFramebuffer_EndAccess;
  g_GraphicsCreateTextureSet = GraphicsTextureSet_Create;
  g_GraphicsDestroyTextureSet = GraphicsTextureSet_Destroy;
  g_GraphicsDisplayModeFinalize = displayModeHook;
  return 0;
}


/* Tears the graphics backend down at exit (Runtime_Shutdown): blocks the cursor timer, frees the software
   cursor buffers and releases every DirectDraw surface, primary surface last.
*/
void Graphics_Shutdown(void)

{
  /* nonzero: GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer draws nothing */
  g_GraphicsBackendAccessState = -1;
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
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
  return;
}

/* Publishes the display framebuffer of the new mode and selects the 16- or 32-bit software blitters (also used
   by the SDL3 backend, which then installs its own present and capture functions and the permanent pixels). */
void GraphicsDirectDraw_PublishFramebuffer
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
