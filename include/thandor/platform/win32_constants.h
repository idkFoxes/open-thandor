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

/* MessageBoxA (FatalError_Exit) */
#ifndef MB_ICONEXCLAMATION
#define MB_ICONEXCLAMATION 0x00000030L
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
/* IDirectSoundBuffer::Lock / GetStatus (DirectSound voice sets) */
#ifndef DSBLOCK_ENTIREBUFFER
#define DSBLOCK_ENTIREBUFFER 0x00000002
#endif
#ifndef DSBSTATUS_PLAYING
#define DSBSTATUS_PLAYING 0x00000001
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

/* WinSock socket/bind/setsockopt/ioctlsocket/inet_addr (network backends) */
#ifndef AF_INET
#define AF_INET 2
#endif
#ifndef AF_IPX
#define AF_IPX 6
#endif
#ifndef SOCK_DGRAM
#define SOCK_DGRAM 2
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET 0xFFFFFFFF /* (SOCKET)(~0) */
#endif
#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF /* inet_addr: not a dotted address */
#endif
#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST 0xFFFFFFFF
#endif
#ifndef SOL_SOCKET
#define SOL_SOCKET 0xFFFF
#endif
#ifndef SO_BROADCAST
#define SO_BROADCAST 0x0020
#endif
#ifndef FIONBIO
#define FIONBIO 0x8004667E /* _IOW('f', 126, u_long) */
#endif

/* Win32 file layer (platform/filesystem/win32): CreateFileA, SetFilePointer, GetFileSize, FindFirstFileA,
   GetDriveTypeA, CopyFileA */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#endif
#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#endif
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif
#ifndef FILE_CURRENT
#define FILE_CURRENT 1
#endif
#ifndef GENERIC_READ
#define GENERIC_READ 0x80000000L
#endif
#ifndef GENERIC_WRITE
#define GENERIC_WRITE 0x40000000L
#endif
#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ 0x00000001
#endif
#ifndef FILE_SHARE_WRITE
#define FILE_SHARE_WRITE 0x00000002
#endif
#ifndef CREATE_ALWAYS
#define CREATE_ALWAYS 2
#endif
#ifndef OPEN_EXISTING
#define OPEN_EXISTING 3
#endif
#ifndef OPEN_ALWAYS
#define OPEN_ALWAYS 4
#endif
#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#endif
#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#endif
#ifndef FILE_FLAG_WRITE_THROUGH
#define FILE_FLAG_WRITE_THROUGH 0x80000000
#endif
#ifndef DRIVE_NO_ROOT_DIR
#define DRIVE_NO_ROOT_DIR 1
#endif
#ifndef DRIVE_REMOVABLE
#define DRIVE_REMOVABLE 2
#endif
#ifndef DRIVE_FIXED
#define DRIVE_FIXED 3
#endif
#ifndef DRIVE_REMOTE
#define DRIVE_REMOTE 4
#endif
#ifndef DRIVE_RAMDISK
#define DRIVE_RAMDISK 6
#endif

/* PeekMessageA (Win32_PumpMessages) */
#ifndef PM_REMOVE
#define PM_REMOVE 0x0001
#endif
#ifndef WM_QUIT
#define WM_QUIT 0x0012
#endif

/* timeSetEvent (TimerSystem_RegisterPeriodic) */
#ifndef TIME_PERIODIC
#define TIME_PERIODIC 0x0001
#endif

/* Primary language ids of GetUserDefaultLCID (Locale_GetDefaultTelephoneCountryCode) */
#ifndef LANG_GERMAN
#define LANG_GERMAN 0x07
#endif
#ifndef LANG_ENGLISH
#define LANG_ENGLISH 0x09
#endif
#ifndef LANG_SPANISH
#define LANG_SPANISH 0x0a
#endif
#ifndef LANG_FRENCH
#define LANG_FRENCH 0x0c
#endif
#ifndef LANG_ITALIAN
#define LANG_ITALIAN 0x10
#endif
#ifndef LANG_RUSSIAN
#define LANG_RUSSIAN 0x19
#endif

/* DirectInput mouse (IDirectInputDevice::GetDeviceData results and DIMOUSESTATE offsets) and the
   mouse-wheel step */
#ifndef DI_OK
#define DI_OK 0
#endif
#ifndef DIERR_INPUTLOST
#define DIERR_INPUTLOST ((TH_LEGACY_HRESULT)0x8007001EL)
#endif
#ifndef DIMOFS_X
#define DIMOFS_X 0x00
#endif
#ifndef DIMOFS_Y
#define DIMOFS_Y 0x04
#endif
#ifndef DIMOFS_Z
#define DIMOFS_Z 0x08
#endif
#ifndef DIMOFS_BUTTON0
#define DIMOFS_BUTTON0 0x0C
#endif
#ifndef DIMOFS_BUTTON1
#define DIMOFS_BUTTON1 0x0D
#endif
#ifndef DIMOFS_BUTTON2
#define DIMOFS_BUTTON2 0x0E
#endif
#ifndef DIMOFS_BUTTON3
#define DIMOFS_BUTTON3 0x0F
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

#endif /* THANDOR_PLATFORM_WIN32_CONSTANTS_H */
