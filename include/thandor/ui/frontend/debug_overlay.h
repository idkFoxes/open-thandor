/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/debug_overlay.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_DEBUG_OVERLAY_H
#define THANDOR_UI_FRONTEND_DEBUG_OVERLAY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/debug_overlay. */

/* Functions are grouped by semantic ownership. */

void FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void);

extern uint32_t g_DebugOverlayCounterRefreshCountdown; /* uint32_t: frames until the debug overlay counters refresh (reloaded with 20); ui/ingame and ui/frontend runtime */

extern uint16_t g_FrontendDebugOverlayTextSlot00Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot01Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot02Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot03Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot04Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot05Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot06Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot07Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot08Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot09Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot12Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot13Utf16[32]; /* owns the unnamed 0x20 bytes after its first 16 units in the original (elapsed time can exceed 16 units) */

extern uint32_t g_RenderedFrameCountSinceDebugRefresh;
extern uint16_t g_FrontendDebugOverlayTextSlot10Utf16[16];
extern uint16_t g_FrontendDebugOverlayTextSlot11Utf16[16];

#endif /* THANDOR_UI_FRONTEND_DEBUG_OVERLAY_H */
