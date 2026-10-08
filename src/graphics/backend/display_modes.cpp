/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/display_modes.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/display_modes.h>
#include <thandor/thandor.h>

/* The display-mode and adapter tables (the framebuffer publication of a mode switch is in graphics/core/device).
   The original's DirectDraw adapter/mode enumeration and surface code is replaced by the SDL3 backend
   (platform/sdl3/video). */

/* Module data. */

/* uint32_t index into g_GraphicsAdapters of the active graphics adapter; 0xFFFFFFFF (GRAPHICS_ADAPTER_INDEX_NONE) before a display mode is set. */
uint32_t g_ActiveGraphicsAdapterIndex = 4294967295u;

GraphicsDisplayMode *g_GraphicsDisplayModes = nullptr;

GraphicsDisplayModeCount g_GraphicsDisplayModeCount = 0;

GraphicsAdapterRecord *g_GraphicsAdapters = nullptr;

uint32_t g_GraphicsAdapterCount = 0;

/* Tells whether the display mode (width, height, bitsPerPixel, adapterIndex) was enumerated
   (g_GraphicsDisplayModes, filled by SdlVideo_Init): returns false when it was, true
   when not. Used by UiDisplayModeSelection_RefreshEnumeratedOptions (ui/dialogs/display_settings.cpp) to offer only
   available modes. An empty table (no enumerated mode) gives true (not enumerated).
*/
bool GraphicsDisplayMode_IsEnumerated(FrontendDisplayAdapterIndex adapterIndex,FrontendColorDepthBits bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width)

{
  GraphicsDisplayModeCount modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  /* The original compared the first entry before checking the count, so an empty table read entry 0 (a NULL
     table) and then counted down from 0 past the end; bounded here because the enumeration can find no mode. */
  if (modesRemaining == 0 || modeCursor == nullptr) {
    return true;
  }
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
   the mode was enumerated (true for an empty table). Used by FrontendDisplaySettingsPage_UpdateModeActionAvailability
   (ui/frontend/display_settings.cpp).
*/
bool DisplayModeTable_ContainsExactMode(FrontendColorDepthBits bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,FrontendDisplayAdapterIndex adapterIndex)

{
  uint32_t modesRemaining;
  GraphicsDisplayMode *modeCursor;
  
  modesRemaining = g_GraphicsDisplayModeCount;
  modeCursor = g_GraphicsDisplayModes;
  /* The original compared the first entry before checking the count, so an empty table read entry 0 (a NULL
     table) and then counted down from 0 past the end; bounded here because the enumeration can find no mode. */
  if (modesRemaining == 0 || modeCursor == nullptr) {
    return true;
  }
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
