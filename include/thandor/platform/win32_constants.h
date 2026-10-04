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

/* MessageBoxA (FatalError_Exit) */
#ifndef MB_ICONEXCLAMATION
#define MB_ICONEXCLAMATION 0x00000030L
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

/* Lock keys (g_KeyboardStateMask) */
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
#ifndef FALSE
#define FALSE 0
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

/* Virtual keys of the keyboard layer (the SDL3 backend maps SDL keys to them; platform/sdl3/input.cpp decides
   with them which key presses also produce characters) */
#ifndef VK_BACK
#define VK_BACK 0x08
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif
#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif
#ifndef VK_PAUSE
#define VK_PAUSE 0x13
#endif
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif
#ifndef VK_DELETE
#define VK_DELETE 0x2E
#endif
#ifndef VK_NUMPAD0
#define VK_NUMPAD0 0x60
#endif
#ifndef VK_F12
#define VK_F12 0x7B
#endif
/* Further virtual keys (Keyboard_OnKeyDown, Keyboard_OnKeyUp) */
#ifndef VK_SHIFT
#define VK_SHIFT 0x10
#endif
#ifndef VK_CONTROL
#define VK_CONTROL 0x11
#endif
#ifndef VK_MENU
#define VK_MENU 0x12
#endif
#ifndef VK_PRIOR
#define VK_PRIOR 0x21
#endif
#ifndef VK_NEXT
#define VK_NEXT 0x22
#endif
#ifndef VK_END
#define VK_END 0x23
#endif
#ifndef VK_HOME
#define VK_HOME 0x24
#endif
#ifndef VK_LEFT
#define VK_LEFT 0x25
#endif
#ifndef VK_UP
#define VK_UP 0x26
#endif
#ifndef VK_RIGHT
#define VK_RIGHT 0x27
#endif
#ifndef VK_DOWN
#define VK_DOWN 0x28
#endif
#ifndef VK_SELECT
#define VK_SELECT 0x29
#endif
#ifndef VK_PRINT
#define VK_PRINT 0x2A
#endif
#ifndef VK_EXECUTE
#define VK_EXECUTE 0x2B
#endif
#ifndef VK_SNAPSHOT
#define VK_SNAPSHOT 0x2C
#endif
#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif
#ifndef VK_NUMPAD1
#define VK_NUMPAD1 0x61
#endif
#ifndef VK_NUMPAD2
#define VK_NUMPAD2 0x62
#endif
#ifndef VK_NUMPAD3
#define VK_NUMPAD3 0x63
#endif
#ifndef VK_NUMPAD4
#define VK_NUMPAD4 0x64
#endif
#ifndef VK_NUMPAD5
#define VK_NUMPAD5 0x65
#endif
#ifndef VK_NUMPAD6
#define VK_NUMPAD6 0x66
#endif
#ifndef VK_NUMPAD7
#define VK_NUMPAD7 0x67
#endif
#ifndef VK_NUMPAD8
#define VK_NUMPAD8 0x68
#endif
#ifndef VK_NUMPAD9
#define VK_NUMPAD9 0x69
#endif
#ifndef VK_MULTIPLY
#define VK_MULTIPLY 0x6A
#endif
#ifndef VK_ADD
#define VK_ADD 0x6B
#endif
#ifndef VK_SEPARATOR
#define VK_SEPARATOR 0x6C
#endif
#ifndef VK_SUBTRACT
#define VK_SUBTRACT 0x6D
#endif
#ifndef VK_DECIMAL
#define VK_DECIMAL 0x6E
#endif
#ifndef VK_DIVIDE
#define VK_DIVIDE 0x6F
#endif
#ifndef VK_F1
#define VK_F1 0x70
#endif
#ifndef VK_F2
#define VK_F2 0x71
#endif
#ifndef VK_F3
#define VK_F3 0x72
#endif
#ifndef VK_F4
#define VK_F4 0x73
#endif
#ifndef VK_F5
#define VK_F5 0x74
#endif
#ifndef VK_F6
#define VK_F6 0x75
#endif
#ifndef VK_F7
#define VK_F7 0x76
#endif
#ifndef VK_F8
#define VK_F8 0x77
#endif
#ifndef VK_F9
#define VK_F9 0x78
#endif
#ifndef VK_F10
#define VK_F10 0x79
#endif
#ifndef VK_F11
#define VK_F11 0x7A
#endif
#ifndef VK_LSHIFT
#define VK_LSHIFT 0xA0
#endif
#ifndef VK_RSHIFT
#define VK_RSHIFT 0xA1
#endif
#ifndef VK_LCONTROL
#define VK_LCONTROL 0xA2
#endif
#ifndef VK_RCONTROL
#define VK_RCONTROL 0xA3
#endif
#ifndef VK_LMENU
#define VK_LMENU 0xA4
#endif
#ifndef VK_RMENU
#define VK_RMENU 0xA5
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

/* RegOpenKeyExA / RegQueryValueExA (Game_LoadCoreAssets: install directory); the bound registry procs take the
   key as a plain dword, so HKEY_LOCAL_MACHINE is defined without the SDK's (HKEY) cast */
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE 0x80000002
#endif
#ifndef KEY_READ
#define KEY_READ 0x00020019
#endif
#ifndef REG_SZ
#define REG_SZ 1
#endif
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0L
#endif

#endif /* THANDOR_PLATFORM_WIN32_CONSTANTS_H */
