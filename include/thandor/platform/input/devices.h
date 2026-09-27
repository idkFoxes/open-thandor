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
/* Modifier bits of g_KeyboardStateMask (Keyboard_OnKeyDown): left/right Shift 0x01/0x02, left/right Ctrl
   0x04/0x08, left/right Alt 0x10/0x20. */
#define KEYBOARD_STATE_SHIFT 0x03
#define KEYBOARD_STATE_CTRL 0x0C
#define KEYBOARD_STATE_ALT 0x30
#define KEYBOARD_STATE_ANY_MODIFIER 0x3F
/* Key codes of the events Keyboard_OnKeyDown queues (the commandCode of the keyboard dispatchers): digits and
   letters are 0x30000 + their ASCII code (letters lowercase), special keys use the 0x10000 family. */
#define KEYBOARD_KEY_CODE_CHAR(asciiCode) (0x30000 + (asciiCode))
#define KEYBOARD_KEY_CODE_SPACE 0x20
#define KEYBOARD_KEY_CODE_BACKSPACE 0x10003
#define KEYBOARD_KEY_CODE_NUMPAD_5 0x10015 /* also VK_SELECT */
/* Indices into g_KeyboardSpecialKeyDown: the low word of a 0x10000-family key code, 1 while the key is held
   (Keyboard_OnKeyDown); the numpad keys map to the same codes. Used by the in-game camera keys. */
#define KEYBOARD_SPECIAL_KEY_DELETE 0x06 /* also numpad decimal point */
#define KEYBOARD_SPECIAL_KEY_INSERT 0x07 /* also numpad 0 */
#define KEYBOARD_SPECIAL_KEY_HOME 0x10 /* also numpad 7 */
#define KEYBOARD_SPECIAL_KEY_UP 0x11 /* also numpad 8 */
#define KEYBOARD_SPECIAL_KEY_PAGE_UP 0x12 /* also numpad 9 */
#define KEYBOARD_SPECIAL_KEY_LEFT 0x14 /* also numpad 4 */
#define KEYBOARD_SPECIAL_KEY_RIGHT 0x16 /* also numpad 6 */
#define KEYBOARD_SPECIAL_KEY_END 0x18 /* also numpad 1 */
#define KEYBOARD_SPECIAL_KEY_DOWN 0x19 /* also numpad 2 */
#define KEYBOARD_SPECIAL_KEY_PAGE_DOWN 0x1A /* also numpad 3 */
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
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
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
