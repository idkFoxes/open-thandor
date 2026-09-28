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
#ifndef DISCL_NONEXCLUSIVE
#define DISCL_NONEXCLUSIVE 0x00000002 /* windowed test aid only */
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

/* PeekMessageA (Win32_PumpMessages) */
#ifndef PM_REMOVE
#define PM_REMOVE 0x0001
#endif
#ifndef WM_QUIT
#define WM_QUIT 0x0012
#endif

/* Keyboard messages and virtual keys (Win32_ShouldTranslateMessageFlags) */
#ifndef WM_KEYDOWN
#define WM_KEYDOWN 0x0100
#endif
#ifndef WM_CHAR
#define WM_CHAR 0x0102
#endif
#ifndef WM_DEADCHAR
#define WM_DEADCHAR 0x0103
#endif
#ifndef WM_SYSKEYUP
#define WM_SYSKEYUP 0x0105
#endif
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

/* DirectDraw (ddraw.h): SetCooperativeLevel flags, DDSURFACEDESC.dwFlags, DDSCAPS.dwCaps, DDPIXELFORMAT.dwFlags
   and the DDBD_* bit depths of D3DDEVICEDESC (GraphicsDirectDraw_ApplyDisplayModeAndCreateResources and the
   enumeration callbacks) */
#ifndef DDSCL_FULLSCREEN
#define DDSCL_FULLSCREEN 0x00000001
#endif
#ifndef DDSCL_NORMAL
#define DDSCL_NORMAL 0x00000008
#endif
#ifndef DDSCL_EXCLUSIVE
#define DDSCL_EXCLUSIVE 0x00000010
#endif
#ifndef DDSD_CAPS
#define DDSD_CAPS 0x00000001
#endif
#ifndef DDSD_HEIGHT
#define DDSD_HEIGHT 0x00000002
#endif
#ifndef DDSD_WIDTH
#define DDSD_WIDTH 0x00000004
#endif
#ifndef DDSD_BACKBUFFERCOUNT
#define DDSD_BACKBUFFERCOUNT 0x00000020
#endif
#ifndef DDSD_ZBUFFERBITDEPTH
#define DDSD_ZBUFFERBITDEPTH 0x00000040
#endif
#ifndef DDSCAPS_BACKBUFFER
#define DDSCAPS_BACKBUFFER 0x00000004
#endif
#ifndef DDSCAPS_COMPLEX
#define DDSCAPS_COMPLEX 0x00000008
#endif
#ifndef DDSCAPS_FLIP
#define DDSCAPS_FLIP 0x00000010
#endif
#ifndef DDSCAPS_OFFSCREENPLAIN
#define DDSCAPS_OFFSCREENPLAIN 0x00000040
#endif
#ifndef DDSCAPS_PRIMARYSURFACE
#define DDSCAPS_PRIMARYSURFACE 0x00000200
#endif
#ifndef DDSCAPS_SYSTEMMEMORY
#define DDSCAPS_SYSTEMMEMORY 0x00000800
#endif
#ifndef DDSCAPS_3DDEVICE
#define DDSCAPS_3DDEVICE 0x00002000
#endif
#ifndef DDSCAPS_VIDEOMEMORY
#define DDSCAPS_VIDEOMEMORY 0x00004000
#endif
#ifndef DDSCAPS_ZBUFFER
#define DDSCAPS_ZBUFFER 0x00020000
#endif
/* IDirectDrawSurface::Lock flag (GraphicsFramebuffer_BeginAccess) */
#ifndef DDLOCK_WAIT
#define DDLOCK_WAIT 0x00000001
#endif
/* Lock/BltFast/Flip flags of GraphicsFramebuffer_Present and the GraphicsFramebuffer_CaptureRegion* functions */
#ifndef DDLOCK_READONLY
#define DDLOCK_READONLY 0x00000010
#endif
#ifndef DDLOCK_WRITEONLY
#define DDLOCK_WRITEONLY 0x00000020
#endif
#ifndef DDBLTFAST_WAIT
#define DDBLTFAST_WAIT 0x00000010
#endif
#ifndef DDBLT_WAIT
#define DDBLT_WAIT 0x01000000 /* windowed test aid only (IDirectDrawSurface::Blt) */
#endif
#ifndef DDFLIP_WAIT
#define DDFLIP_WAIT 0x00000001
#endif
/* Texture surfaces (GraphicsTexture_CreateStagingTexture, GraphicsTexture_CreateDeviceTexture) */
#ifndef DDSD_PIXELFORMAT
#define DDSD_PIXELFORMAT 0x00001000
#endif
#ifndef DDSCAPS_TEXTURE
#define DDSCAPS_TEXTURE 0x00001000
#endif
#ifndef DDSCAPS_ALLOCONLOAD
#define DDSCAPS_ALLOCONLOAD 0x04000000
#endif
#ifndef DDERR_OUTOFVIDEOMEMORY
#define DDERR_OUTOFVIDEOMEMORY ((TH_LEGACY_HRESULT)0x8876017CL)
#endif
/* IDirectDraw2::CreatePalette flags (the 8-bit paths of GraphicsTexture_UploadColor_*) */
#ifndef DDPCAPS_8BIT
#define DDPCAPS_8BIT 0x00000004
#endif
#ifndef DDPCAPS_ALLOW256
#define DDPCAPS_ALLOW256 0x00000040
#endif
#ifndef DDPF_ALPHAPIXELS
#define DDPF_ALPHAPIXELS 0x00000001
#endif
#ifndef DDPF_ALPHA
#define DDPF_ALPHA 0x00000002
#endif
#ifndef DDPF_PALETTEINDEXED4
#define DDPF_PALETTEINDEXED4 0x00000008
#endif
#ifndef DDPF_PALETTEINDEXEDTO8
#define DDPF_PALETTEINDEXEDTO8 0x00000010
#endif
#ifndef DDPF_PALETTEINDEXED8
#define DDPF_PALETTEINDEXED8 0x00000020
#endif
#ifndef DDPF_RGB
#define DDPF_RGB 0x00000040
#endif
#ifndef DDPF_YUV
#define DDPF_YUV 0x00000200
#endif
#ifndef DDPF_ZBUFFER
#define DDPF_ZBUFFER 0x00000400
#endif
#ifndef DDPF_PALETTEINDEXED1
#define DDPF_PALETTEINDEXED1 0x00000800
#endif
#ifndef DDPF_ZPIXELS
#define DDPF_ZPIXELS 0x00002000
#endif
#ifndef DDBD_32
#define DDBD_32 0x00000100
#endif
#ifndef DDBD_16
#define DDBD_16 0x00000400
#endif

/* Direct3D (d3dcaps.h, d3dtypes.h): D3DDEVICEDESC.dwFlags validity bits, device and triangle caps
   (Direct3D_EnumDeviceCallback) and render-state values
   (GraphicsDirectDraw_ApplyDisplayModeAndCreateResources) */
#ifndef D3DDD_COLORMODEL
#define D3DDD_COLORMODEL 0x00000001
#endif
#ifndef D3DDD_DEVCAPS
#define D3DDD_DEVCAPS 0x00000002
#endif
#ifndef D3DDD_TRICAPS
#define D3DDD_TRICAPS 0x00000040
#endif
#ifndef D3DDD_DEVICERENDERBITDEPTH
#define D3DDD_DEVICERENDERBITDEPTH 0x00000080
#endif
#ifndef D3DDD_DEVICEZBUFFERBITDEPTH
#define D3DDD_DEVICEZBUFFERBITDEPTH 0x00000100
#endif
#ifndef D3DDEVCAPS_TLVERTEXSYSTEMMEMORY
#define D3DDEVCAPS_TLVERTEXSYSTEMMEMORY 0x00000040
#endif
#ifndef D3DDEVCAPS_TEXTUREVIDEOMEMORY
#define D3DDEVCAPS_TEXTUREVIDEOMEMORY 0x00000200
#endif
#ifndef D3DPRASTERCAPS_STIPPLE
#define D3DPRASTERCAPS_STIPPLE 0x00000200
#endif
#ifndef D3DPCMPCAPS_LESSEQUAL
#define D3DPCMPCAPS_LESSEQUAL 0x00000008
#endif
#ifndef D3DPBLENDCAPS_ZERO
#define D3DPBLENDCAPS_ZERO 0x00000001
#endif
#ifndef D3DPBLENDCAPS_ONE
#define D3DPBLENDCAPS_ONE 0x00000002
#endif
#ifndef D3DPBLENDCAPS_SRCALPHA
#define D3DPBLENDCAPS_SRCALPHA 0x00000010
#endif
#ifndef D3DPBLENDCAPS_INVSRCALPHA
#define D3DPBLENDCAPS_INVSRCALPHA 0x00000020
#endif
#ifndef D3DPSHADECAPS_COLORGOURAUDRGB
#define D3DPSHADECAPS_COLORGOURAUDRGB 0x00000008
#endif
#ifndef D3DPSHADECAPS_ALPHAGOURAUDBLEND
#define D3DPSHADECAPS_ALPHAGOURAUDBLEND 0x00004000
#endif
#ifndef D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED
#define D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED 0x00008000
#endif
#ifndef D3DPTBLENDCAPS_MODULATE
#define D3DPTBLENDCAPS_MODULATE 0x00000002
#endif
#ifndef D3DPTADDRESSCAPS_WRAP
#define D3DPTADDRESSCAPS_WRAP 0x00000001
#endif
#ifndef D3DSHADE_GOURAUD
#define D3DSHADE_GOURAUD 2
#endif
#ifndef D3DCULL_NONE
#define D3DCULL_NONE 1
#endif
#ifndef D3DZB_TRUE
#define D3DZB_TRUE 1
#endif
#ifndef D3DCMP_LESSEQUAL
#define D3DCMP_LESSEQUAL 4
#endif
#ifndef D3DFILL_SOLID
#define D3DFILL_SOLID 3
#endif
#ifndef D3DTBLEND_MODULATEALPHA
#define D3DTBLEND_MODULATEALPHA 4
#endif
/* IDirect3DDevice2::DrawPrimitive and IDirect3DViewport2::Clear (Graphics_DrawPrimitiveQueue,
   Graphics_SetViewportAndClearDepth) */
#ifndef D3DPT_TRIANGLEFAN
#define D3DPT_TRIANGLEFAN 6
#endif
#ifndef D3DVT_TLVERTEX
#define D3DVT_TLVERTEX 3
#endif
#ifndef D3DDP_DONOTUPDATEEXTENTS
#define D3DDP_DONOTUPDATEEXTENTS 0x00000008
#endif
#ifndef D3DCLEAR_ZBUFFER
#define D3DCLEAR_ZBUFFER 0x00000002
#endif
/* D3DCOLORMODEL (d3dtypes.h): D3DDEVICEDESC.dcmColorModel (Direct3D_PrimitiveHandler_*) */
#ifndef D3DCOLOR_RGB
#define D3DCOLOR_RGB 2
#endif

/* 3dfx Glide 3 (glide.h): grGet/grGetString selectors and GrScreenResolution_t values (Glide3_InitAndEnumerate) */
#ifndef GR_NUM_BOARDS
#define GR_NUM_BOARDS 0x0f
#endif
#ifndef GR_HARDWARE
#define GR_HARDWARE 0xa1
#endif
#ifndef GR_RENDERER
#define GR_RENDERER 0xa2
#endif
#ifndef GR_RESOLUTION_640x480
#define GR_RESOLUTION_640x480 0x7
#endif
#ifndef GR_RESOLUTION_800x600
#define GR_RESOLUTION_800x600 0x8
#endif
#ifndef GR_RESOLUTION_960x720
#define GR_RESOLUTION_960x720 0x9
#endif
#ifndef GR_RESOLUTION_1024x768
#define GR_RESOLUTION_1024x768 0xC
#endif
#ifndef GR_RESOLUTION_1280x1024
#define GR_RESOLUTION_1280x1024 0xD
#endif
#ifndef GR_RESOLUTION_1600x1200
#define GR_RESOLUTION_1600x1200 0xE
#endif

/* IDirect3DDevice2::EnumTextureFormats callback result (GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback)
   and the Glide 3 (glide.h) state values used by GraphicsGlide3_ApplyDisplayModeAndInitializeResources,
   Glide3_DrawPrimitiveQueue, Glide3_ClearViewport and Glide3_Framebuffer_Begin/EndAccess */
#ifndef D3DENUMRET_OK
#define D3DENUMRET_OK 1
#endif
#ifndef FXFALSE
#define FXFALSE 0
#endif
#ifndef FXTRUE
#define FXTRUE 1
#endif
#ifndef GR_NUM_TMU
#define GR_NUM_TMU 0x13
#endif
#ifndef GR_COLORFORMAT_ARGB
#define GR_COLORFORMAT_ARGB 0x0
#endif
#ifndef GR_ORIGIN_UPPER_LEFT
#define GR_ORIGIN_UPPER_LEFT 0x0
#endif
#ifndef GR_WINDOW_COORDS
#define GR_WINDOW_COORDS 0x00
#endif
#ifndef GR_PARAM_XY
#define GR_PARAM_XY 0x01
#endif
#ifndef GR_PARAM_Z
#define GR_PARAM_Z 0x02
#endif
#ifndef GR_PARAM_Q
#define GR_PARAM_Q 0x04
#endif
#ifndef GR_PARAM_PARGB
#define GR_PARAM_PARGB 0x30
#endif
#ifndef GR_PARAM_ST0
#define GR_PARAM_ST0 0x40
#endif
#ifndef GR_PARAM_ENABLE
#define GR_PARAM_ENABLE 0x01
#endif
#ifndef GR_CULL_DISABLE
#define GR_CULL_DISABLE 0x0
#endif
#ifndef GR_DEPTHBUFFER_ZBUFFER
#define GR_DEPTHBUFFER_ZBUFFER 0x1
#endif
#ifndef GR_CMP_GEQUAL
#define GR_CMP_GEQUAL 0x6
#endif
#ifndef GR_MIPMAP_DISABLE
#define GR_MIPMAP_DISABLE 0x0
#endif
#ifndef GR_MIPMAPLEVELMASK_BOTH
#define GR_MIPMAPLEVELMASK_BOTH 0x3
#endif
#ifndef GR_TEXTURECLAMP_WRAP
#define GR_TEXTURECLAMP_WRAP 0x0
#endif
#ifndef GR_TEXTUREFILTER_BILINEAR
#define GR_TEXTUREFILTER_BILINEAR 0x1
#endif
#ifndef GR_COMBINE_FUNCTION_LOCAL
#define GR_COMBINE_FUNCTION_LOCAL 0x1
#endif
#ifndef GR_COMBINE_FUNCTION_SCALE_OTHER
#define GR_COMBINE_FUNCTION_SCALE_OTHER 0x3
#endif
#ifndef GR_COMBINE_FACTOR_ZERO
#define GR_COMBINE_FACTOR_ZERO 0x0
#endif
#ifndef GR_COMBINE_FACTOR_LOCAL
#define GR_COMBINE_FACTOR_LOCAL 0x1
#endif
#ifndef GR_COMBINE_LOCAL_ITERATED
#define GR_COMBINE_LOCAL_ITERATED 0x0
#endif
#ifndef GR_COMBINE_OTHER_TEXTURE
#define GR_COMBINE_OTHER_TEXTURE 0x1
#endif
#ifndef GR_COMBINE_OTHER_CONSTANT
#define GR_COMBINE_OTHER_CONSTANT 0x2
#endif
#ifndef GR_BLEND_ZERO
#define GR_BLEND_ZERO 0x0
#endif
#ifndef GR_BLEND_SRC_ALPHA
#define GR_BLEND_SRC_ALPHA 0x1
#endif
#ifndef GR_BLEND_ONE
#define GR_BLEND_ONE 0x4
#endif
#ifndef GR_BLEND_ONE_MINUS_SRC_ALPHA
#define GR_BLEND_ONE_MINUS_SRC_ALPHA 0x5
#endif
#ifndef GR_BLEND_ONE_MINUS_DST_ALPHA
#define GR_BLEND_ONE_MINUS_DST_ALPHA 0x7
#endif
#ifndef GR_BUFFER_BACKBUFFER
#define GR_BUFFER_BACKBUFFER 0x1
#endif
#ifndef GR_LFB_READ_ONLY
#define GR_LFB_READ_ONLY 0x00
#endif
#ifndef GR_LFB_WRITE_ONLY
#define GR_LFB_WRITE_ONLY 0x01
#endif
#ifndef GR_LFB_NOIDLE
#define GR_LFB_NOIDLE 0x10
#endif
#ifndef GR_LFBWRITEMODE_565
#define GR_LFBWRITEMODE_565 0x0
#endif

/* Glide 3 (glide.h) GrTextureFormat_t values of the texture uploads (Glide3_TextureResource_Initialize) */
#ifndef GR_TEXFMT_RGB_565
#define GR_TEXFMT_RGB_565 0xa
#endif
#ifndef GR_TEXFMT_ARGB_4444
#define GR_TEXFMT_ARGB_4444 0xc
#endif

/* Window messages handled by MainWindowProc */
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef WM_CLOSE
#define WM_CLOSE 0x0010
#endif
#ifndef WM_ACTIVATEAPP
#define WM_ACTIVATEAPP 0x001C
#endif
#ifndef WM_SETCURSOR
#define WM_SETCURSOR 0x0020
#endif
#ifndef WM_KEYUP
#define WM_KEYUP 0x0101
#endif
#ifndef WM_SYSKEYDOWN
#define WM_SYSKEYDOWN 0x0104
#endif
#ifndef WM_SYSCHAR
#define WM_SYSCHAR 0x0106
#endif

#endif /* THANDOR_PLATFORM_WIN32_CONSTANTS_H */
