/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/input/devices.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_INPUT_DEVICES_H
#define THANDOR_PLATFORM_INPUT_DEVICES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/input/devices. */

/* Lock-key bits of g_KeyboardStateMask, seeded from GetKeyState (DirectInputMouse_Init at startup
   and again on WM_ACTIVATEAPP in MainWindowProc). */
#define KEYBOARD_STATE_NUM_LOCK 0x10000
#define KEYBOARD_STATE_SCROLL_LOCK 0x20000
#define KEYBOARD_STATE_CAPS_LOCK 0x40000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00417280 */
bool __thandor_cf_preserve_eax_ecx_edx
Keyboard_CompareAsciiCaseInsensitiveFlags
          (KeyboardCharacterCode leftCodeUnit,KeyboardCharacterCode rightCodeUnit);

/* 0x00417230 */
void __thandor_void_preserve_eax_ecx_edx Keyboard_FlushEvents(void);

/* 0x00417240 */
KeyboardEventResult __thandor_eax_edx_cf_preserve_ecx Keyboard_ReadNextEventRegs(void);

/* 0x004172D0 */
uint32_t __thandor_eax_preserve_ecx_edx Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit);

/* 0x00576CF0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx DirectInputMouse_Init(void);

/* 0x00576F20 */
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_RefreshDeviceIfIdle(void);

/* 0x00577000 */
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_Shutdown(void);

/* 0x00577080 */
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_PollBufferedEvents(void);

/* 0x005772F0 */
DisplayModeResult __thandor_eax_cf_preserve_ecx_edx
DirectInputMouse_SetDisplayMode
          (DisplayModeHookArgument0 hookArg0,DisplayModeHookArgument1 hookArg1,
          GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth);

/* 0x00577420 */
void __thandor_void_preserve_eax_ecx
DirectInputMouse_SetPosition(Win32CursorCoordinate32 positionY,Win32CursorCoordinate32 positionX);

/* 0x00577460 */
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_FlushBufferedEvents(void);

/* 0x005774A0 */
void __thandor_void_preserve_eax_ecx_edx Keyboard_OnKeyDown(KeyboardVirtualKeyCode virtualKey);

/* 0x00577880 */
void __thandor_void_preserve_eax_ecx_edx Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey);

/* 0x00577B30 */
void __thandor_void_preserve_eax_ecx Keyboard_OnChar(KeyboardCharacterCode character);

/* 0x004172B0 */
uint32_t __thandor_eax_preserve_ecx_edx Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit);

#endif /* THANDOR_PLATFORM_INPUT_DEVICES_H */
