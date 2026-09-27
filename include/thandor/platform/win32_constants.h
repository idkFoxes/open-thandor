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
#ifndef NORMAL_PRIORITY_CLASS
#define NORMAL_PRIORITY_CLASS 0x00000020
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

/* IDirectSound::SetCooperativeLevel, CreateSoundBuffer and IDirectSoundBuffer::Play/SetVolume/SetPan
   (DSBCAPS_CTRLPAN and DSBCAPS_CTRLVOLUME are in the DirectSoundBufferCaps enum of generated/types.h) */
#ifndef DSSCL_EXCLUSIVE
#define DSSCL_EXCLUSIVE 0x00000003
#endif
#ifndef DSBCAPS_PRIMARYBUFFER
#define DSBCAPS_PRIMARYBUFFER 0x00000001
#endif
#ifndef DSBPLAY_LOOPING
#define DSBPLAY_LOOPING 0x00000001
#endif
#ifndef DSBVOLUME_MAX
#define DSBVOLUME_MAX 0
#endif
#ifndef DSBPAN_CENTER
#define DSBPAN_CENTER 0
#endif

/* WSAStartup version request */
#ifndef MAKEWORD
#define MAKEWORD(low, high) ((uint16_t)(((uint8_t)(low)) | (((uint16_t)(uint8_t)(high)) << 8)))
#endif

/* GetLocaleInfoA (Locale_Init) */
#ifndef LOCALE_USER_DEFAULT
#define LOCALE_USER_DEFAULT 0x0400
#endif
#ifndef LOCALE_ILANGUAGE
#define LOCALE_ILANGUAGE 0x00000001
#endif
#ifndef LOCALE_SDECIMAL
#define LOCALE_SDECIMAL 0x0000000E
#endif
#ifndef LOCALE_STHOUSAND
#define LOCALE_STHOUSAND 0x0000000F
#endif
#ifndef LOCALE_SGROUPING
#define LOCALE_SGROUPING 0x00000010
#endif
#ifndef LOCALE_SDATE
#define LOCALE_SDATE 0x0000001D
#endif
#ifndef LOCALE_STIME
#define LOCALE_STIME 0x0000001E
#endif
#ifndef LOCALE_ILDATE
#define LOCALE_ILDATE 0x00000022
#endif
#ifndef LOCALE_ITIME
#define LOCALE_ITIME 0x00000023
#endif
#ifndef LOCALE_S1159
#define LOCALE_S1159 0x00000028
#endif
#ifndef LOCALE_S2359
#define LOCALE_S2359 0x00000029
#endif
#ifndef LOCALE_SNEGATIVESIGN
#define LOCALE_SNEGATIVESIGN 0x00000051
#endif

/* DirectInputCreateA and the mouse device (DirectInputMouse_Init) */
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0300
#endif
#ifndef DISCL_EXCLUSIVE
#define DISCL_EXCLUSIVE 0x00000001
#endif
#ifndef DISCL_FOREGROUND
#define DISCL_FOREGROUND 0x00000004
#endif
#ifndef DIPROP_BUFFERSIZE
#define DIPROP_BUFFERSIZE ((TH_LEGACY_GUID *)1) /* MAKEDIPROP(1) */
#endif

/* GetKeyState */
#ifndef VK_CAPITAL
#define VK_CAPITAL 0x14
#endif
#ifndef VK_NUMLOCK
#define VK_NUMLOCK 0x90
#endif
#ifndef VK_SCROLL
#define VK_SCROLL 0x91
#endif

#endif /* THANDOR_PLATFORM_WIN32_CONSTANTS_H */
