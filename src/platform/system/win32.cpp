/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/win32.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/system/win32.h>
#include <thandor/thandor.h>

/* The message pump slot the game calls every frame and the close counter. The original's Win32 message pump
   and window procedure are replaced by the SDL3 event pump (SdlPlatform_PumpEvents, platform/sdl3/platform.cpp),
   which SdlPlatform_InstallTimersAndPump installs in the slot. */

/* Module data. */

Win32PumpMessagesProc *g_Win32PumpMessages = 0;

/* nonzero once the window was asked to close: the pump then shuts the game down */
uint32_t g_WindowDestroyDepth = 0;
