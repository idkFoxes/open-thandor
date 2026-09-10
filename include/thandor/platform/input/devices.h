#ifndef THANDOR_PLATFORM_INPUT_DEVICES_H
#define THANDOR_PLATFORM_INPUT_DEVICES_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/input/devices. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00417280 */
void Keyboard_CompareAsciiCaseInsensitiveFlags (KeyboardCharacterCode leftCodeUnit,KeyboardCharacterCode rightCodeUnit);

/* 0x00417230 */
void __cdecl Keyboard_FlushEvents(void);

/* 0x00417240 */
qword __cdecl Keyboard_ReadNextEventRegs(void);

/* 0x004172D0 */
dword Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit);

/* 0x00576CF0 */
dword __cdecl DirectInputMouse_Init(void);

/* 0x00576F20 */
void __cdecl DirectInputMouse_RefreshDeviceIfIdle(void);

/* 0x00577000 */
void __cdecl DirectInputMouse_Shutdown(void);

/* 0x00577080 */
void __cdecl DirectInputMouse_PollBufferedEvents(void);

/* 0x005772F0 */
void DirectInputMouse_DisplayModeHookCf (DisplayModeHookArgument0 hookArg0,DisplayModeHookArgument1 hookArg1, GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth);

/* 0x00577420 */
void DirectInputMouse_SetPosition (Win32CursorCoordinate32 positionY,Win32CursorCoordinate32 positionX);

/* 0x00577460 */
void __cdecl DirectInputMouse_FlushBufferedEvents(void);

/* 0x005774A0 */
void Keyboard_OnKeyDown(KeyboardVirtualKeyCode virtualKey);

/* 0x00577880 */
void Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey);

/* 0x00577B30 */
void Keyboard_OnChar(KeyboardCharacterCode character);

/* 0x004172B0 */
dword Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit);

#endif /* THANDOR_PLATFORM_INPUT_DEVICES_H */
