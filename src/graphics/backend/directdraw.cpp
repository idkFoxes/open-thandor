/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/directdraw.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/directdraw.h>
#include <thandor/thandor.h>

/* The display-mode and adapter tables and the framebuffer publication of a mode switch. The original's
   DirectDraw adapter/mode enumeration and surface code is replaced by the SDL3 backend (platform/sdl3/video). */

/* Module data. */

static GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc *g_GraphicsTextureSourceBlitIntegerScaledSourceAlpha = 0;

static GraphicsTextureSourceBlitSourceAlphaPaletteBankProc *g_GraphicsTextureSourceBlitSourceAlphaPaletteBank = 0;

/* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
uint32_t g_ActiveGraphicsAdapterIndex = 4294967295u;

GraphicsDisplayMode *g_GraphicsDisplayModes = 0;

GraphicsDisplayModeCount g_GraphicsDisplayModeCount = 0;

GraphicsAdapterRecord *g_GraphicsAdapters = 0;

uint32_t g_GraphicsAdapterCount = 0;

/* Implementation ownership: graphics/backend/directdraw. */

/* Tells whether the display mode (width, height, bitsPerPixel, adapterIndex) was enumerated
   (g_GraphicsDisplayModes, filled by SdlVideo_Init): returns false when it was, true
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


/* Publishes the display framebuffer of the new mode and selects the 16- or 32-bit software blitters. Called by
   SdlVideo_ApplyDisplayMode, which then installs its present and capture functions and the framebuffer pixels. */
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
}
