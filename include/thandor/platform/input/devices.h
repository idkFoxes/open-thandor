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
#define KEYBOARD_STATE_LEFT_SHIFT 0x01
#define KEYBOARD_STATE_RIGHT_SHIFT 0x02
#define KEYBOARD_STATE_LEFT_CTRL 0x04
#define KEYBOARD_STATE_RIGHT_CTRL 0x08
#define KEYBOARD_STATE_LEFT_ALT 0x10
#define KEYBOARD_STATE_RIGHT_ALT 0x20
/* Key codes of the events Keyboard_OnKeyDown queues (the commandCode of the keyboard dispatchers): digits and
   letters are 0x30000 + their ASCII code (letters lowercase), special keys use the 0x10000 family. */
#define KEYBOARD_KEY_CODE_CHAR(asciiCode) (0x30000 + (asciiCode))
#define KEYBOARD_KEY_CODE_SPACE 0x20
#define KEYBOARD_KEY_CODE_BACKSPACE 0x10003
#define KEYBOARD_KEY_CODE_ESCAPE 0x10000 /* VK_ESCAPE (Keyboard_OnKeyDown); Game_PlayIntroMovies skips all intros */
#define KEYBOARD_KEY_CODE_NUMPAD_5 0x10015 /* also VK_SELECT */
#define KEYBOARD_KEY_CODE_ENTER 0x10001 /* VK_RETURN and VK_SEPARATOR */
#define KEYBOARD_KEY_CODE_TAB 0x10002
#define KEYBOARD_KEY_CODE_PRINT 0x10004 /* VK_PRINT and VK_SNAPSHOT */
#define KEYBOARD_KEY_CODE_PAUSE 0x10005 /* VK_PAUSE and VK_EXECUTE */
#define KEYBOARD_KEY_CODE_SPECIAL(index) (0x10000 + (index)) /* index: KEYBOARD_SPECIAL_KEY_* */
#define KEYBOARD_KEY_CODE_FUNCTION(number) (0x20000 + (number)) /* F1..F12 */
/* Keyboard_OnKeyUp: any code outside the 0x10000 family, i.e. no g_KeyboardSpecialKeyDown entry to clear */
#define KEYBOARD_KEY_CODE_NOT_SPECIAL 0x20000
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
/* Full key codes of those special keys, as the UI keyboard handlers (text edits, lists) compare them. */
#define KEYBOARD_KEY_CODE_DELETE KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DELETE)
#define KEYBOARD_KEY_CODE_INSERT KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_INSERT)
#define KEYBOARD_KEY_CODE_HOME KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME)
#define KEYBOARD_KEY_CODE_UP KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP)
#define KEYBOARD_KEY_CODE_PAGE_UP KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP)
#define KEYBOARD_KEY_CODE_LEFT KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT)
#define KEYBOARD_KEY_CODE_RIGHT KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT)
#define KEYBOARD_KEY_CODE_END KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END)
#define KEYBOARD_KEY_CODE_DOWN KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN)
#define KEYBOARD_KEY_CODE_PAGE_DOWN KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN)
/* Entries of the g_KeyboardEvents ring; the read and write indices wrap to 0 after SIZE - 1. */
#define KEYBOARD_EVENT_RING_SIZE 64
/* Key code families: the high word selects the family (KEYBOARD_KEY_CODE_SPECIAL(0), _FUNCTION(0), _CHAR(0)),
   the low word of a special key is its g_KeyboardSpecialKeyDown index */
#define KEYBOARD_KEY_CODE_FAMILY_MASK 0xffff0000
#define KEYBOARD_KEY_CODE_INDEX_MASK 0xffff
/* Entries of the g_CursorInputEvents ring (DirectInputMouse_PollBufferedEvents); the write index wraps after
   SIZE - 1 */
#define CURSOR_INPUT_EVENT_RING_SIZE 256
/* DirectInputMouse_Init: rates of the cursor-animation and mouse-poll timers */
#define CURSOR_ANIMATION_TIMER_HZ 20
#define MOUSE_POLL_TIMER_HZ 64
/* DirectInputMouse_PollBufferedEvents: a button event's dwData has this bit set while the button is down */
#define DIRECTINPUT_BUTTON_DOWN_BIT 0x80
/* DirectInputMouse_PollBufferedEvents: device errors after which a poll gives up until the next tick */
#define MOUSE_POLL_MAX_ERRORS 16
/* Functions are grouped by semantic ownership. */

Bool8 Keyboard_CompareAsciiCaseInsensitiveFlags(KeyboardCharacterCode leftCodeUnit,KeyboardCharacterCode rightCodeUnit);

void Keyboard_FlushEvents(void);

Bool8 Keyboard_ReadNextEvent(uint32_t *outKeyCode, uint32_t *outStateMask);

uint32_t Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit);

Bool8 DirectInputMouse_Init(uint32_t *outError);

void DirectInputMouse_RefreshDeviceIfIdle(void);

void DirectInputMouse_Shutdown(void);

void DirectInputMouse_PollBufferedEvents(void);

Bool8 DirectInputMouse_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth,uint32_t *errorCode);

void DirectInputMouse_SetPosition(Win32CursorCoordinate32 positionY,Win32CursorCoordinate32 positionX);

void DirectInputMouse_FlushBufferedEvents(void);

void Keyboard_OnKeyDown(KeyboardVirtualKeyCode virtualKey);

void Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey);

void Keyboard_OnChar(KeyboardCharacterCode character);

uint32_t Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit);

extern uint32_t g_CursorUseOverridePosition;
extern UiPointerWheelDelta g_CursorWheelDelta;

extern KeyboardReadEventProc *g_KeyboardReadEvent;

extern uint32_t g_CursorInputWriteIndex;
extern PointerFlushEventsProc *g_PointerFlushEvents;
extern PointerSetPositionProc *g_PointerSetPosition;
extern uint32_t g_CursorOverflowLeft;
extern uint32_t g_CursorOverflowRight;
extern uint32_t g_CursorOverflowTop;
extern uint32_t g_CursorOverflowBottom;
extern IDirectInputDeviceA *g_MouseDevice;
extern UiPixelCoordinate g_MouseX;
extern UiPixelCoordinate g_MouseY;
extern GraphicsCursorButtonState g_MouseButtonMask;

extern KeyboardAsciiCaseTransformCallbackTable3 g_KeyboardAsciiCaseTransformCallbacks3;

extern uint32_t g_KeyboardStateMask;

extern uint8_t g_KeyboardSpecialKeyDown[32];
extern KeyboardFlushEventsProc *g_KeyboardFlushEvents;

extern UiPixelCoordinate g_CursorOverrideX;
extern UiPixelCoordinate g_CursorOverrideY;
extern int32_t g_CursorVisibilityToken;
extern uint32_t g_CursorButtonState;

#endif /* THANDOR_PLATFORM_INPUT_DEVICES_H */
