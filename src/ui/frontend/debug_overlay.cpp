/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/debug_overlay.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/debug_overlay.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16] = {0};

THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16] = {0};

/* 32 units, the last slot owns the unnamed 0x20 bytes after its first 16 units in the original. It receives the locale-formatted elapsed time (hours, time separator, minutes, AM/PM designator),
   which can run past 16 units. */
THANDOR_ALIGN(4) uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32] = {0};

/* uint32_t: frames until the debug overlay counters refresh (reloaded with 20); ui/ingame and ui/frontend runtime */
uint32_t g_DebugOverlayCounterRefreshCountdown = 20;

uint32_t g_RenderedFrameCountSinceDebugRefresh = 0;

uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16] = {0};

uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16] = {0};

/* Fills the frontend debug overlay texts: every 20th call the frames rendered since the last refresh and the
   draw calls, texture binds and texture reloads per frame (then all four counters restart), and on every call
   the menu camera's position and orientation, the cursor override position and the free arena bytes.
*/
void FrontendDebugOverlay_RefreshCountersAndWorldCoordinates()

{
  uint32_t freeArenaBytes;
  WideNumberDenominator32 denominator;
  WorldRuntimeContext *world;
  WorldCameraPosition worldVector0;
  WorldCameraOrientation worldVector1;
  
  denominator = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown--;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 20;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    /* per-frame averages with two decimals */
    if (denominator == 0) {
      denominator = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    /* texture binds and texture reloads: the original's hardware counters; the software renderer has
       neither, so both print 0 */
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               0,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               0,g_FrontendDebugOverlayTextSlot03Utf16);
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
  }
  world = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  worldVector0 = WorldRuntime_GetCameraPosition(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  worldVector1 = WorldRuntime_GetCameraOrientation(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideX,
             g_FrontendDebugOverlayTextSlot10Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideY,
             g_FrontendDebugOverlayTextSlot11Utf16);
  freeArenaBytes = g_MemoryApi.queryFreeBytes();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,freeArenaBytes,
             g_FrontendDebugOverlayTextSlot12Utf16); /* hexadecimal despite radix 10 */
  g_FrontendDebugOverlayTextSlot13Utf16[0] = 0;
  return;
}
