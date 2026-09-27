/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/win32_constants.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_WIN32_CONSTANTS_H
#define THANDOR_PLATFORM_WIN32_CONSTANTS_H

/* The Win32 constants the game passes to the API, with the values of the Windows SDK. The build does
   not include <windows.h> (the API is declared in generated/imports.h), so they are defined here;
   each is guarded in case a translation unit includes the SDK header after all. */

#ifndef MAKEINTRESOURCEA
#define MAKEINTRESOURCEA(id) ((LPSTR)(uintptr_t)(uint16_t)(id))
#endif

/* SetPriorityClass / SetThreadPriority */
#ifndef REALTIME_PRIORITY_CLASS
#define REALTIME_PRIORITY_CLASS 0x00000100
#endif
#ifndef THREAD_PRIORITY_NORMAL
#define THREAD_PRIORITY_NORMAL 0
#endif

/* GetSystemMetrics */
#ifndef SM_CXSCREEN
#define SM_CXSCREEN 0
#endif
#ifndef SM_CYSCREEN
#define SM_CYSCREEN 1
#endif

/* LoadCursorA */
#ifndef IDC_ARROW
#define IDC_ARROW MAKEINTRESOURCEA(32512)
#endif

/* CreateWindowExA */
#ifndef WS_EX_TOPMOST
#define WS_EX_TOPMOST 0x00000008L
#endif
#ifndef WS_POPUP
#define WS_POPUP 0x80000000L
#endif
#ifndef WS_SYSMENU
#define WS_SYSMENU 0x00080000L
#endif

/* ShowWindow */
#ifndef SW_SHOWNORMAL
#define SW_SHOWNORMAL 1
#endif

#endif /* THANDOR_PLATFORM_WIN32_CONSTANTS_H */
