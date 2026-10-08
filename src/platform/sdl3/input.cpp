/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/sdl3/input.cpp
 * Project code (not in the original game)
 */

/* SDL3 backend: keyboard and mouse. Keys reach the game as the Windows virtual-key codes MainWindowProc passes to
   Keyboard_OnKeyDown/OnKeyUp (Shift, Ctrl and Alt as VK_SHIFT, VK_CONTROL, VK_MENU; AltGr as Ctrl+Alt, see
   KeyIsAltGr), text as the Windows-1252
   characters of WM_CHAR (Keyboard_OnChar), suppressed for the keys Win32_ShouldTranslateMessageFlags does not
   translate, with the control characters 1-26 of Ctrl+A..Ctrl+Z. Mouse events go into the g_CursorInputEvents
   ring exactly as DirectInputMouse_PollBufferedEvents appends them (position clamped to the framebuffer, the
   excess in g_CursorOverflow*). Fullscreen (borderless or exclusive) uses SDL's relative mouse mode, so the position moves by the raw
   device deltas as with the exclusive DirectInput mouse; a window (display mode kind "Fenster", or the developer
   tools' window) uses the absolute position mapped into framebuffer pixels. */

#include <thandor/platform/sdl3/sdl_objects.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <span>

#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

namespace thandor::sdl3 {
namespace {

/* Windows virtual-key codes the game does not define itself (thandor/platform/win32_constants.h has the rest). */
enum class VirtualKey : KeyboardVirtualKeyCode {
  None = 0,
  Clear = 0x0C,
  LeftWindows = 0x5B,
  RightWindows = 0x5C,
  Applications = 0x5D,
  NumpadDecimal = 0x6E,
  NumpadDivide = 0x6F,
  NumpadSubtract = 0x6D,
  F1 = 0x70,
  Semicolon = 0xBA,
  Plus = 0xBB,
  Comma = 0xBC,
  Minus = 0xBD,
  Period = 0xBE,
  Slash = 0xBF,
  Grave = 0xC0,
  LeftBracket = 0xDB,
  Backslash = 0xDC,
  RightBracket = 0xDD,
  Apostrophe = 0xDE,
  Oem102 = 0xE2,
};

constexpr KeyboardVirtualKeyCode ToCode(VirtualKey key) noexcept
{
  return static_cast<KeyboardVirtualKeyCode>(key);
}

/* the text input of the last key press may become characters (Win32_ShouldTranslateMessageFlags) */
bool s_translateText = true;
/* the pending wheel delta of the event being appended (DirectInput's g_MouseWheelDelta) */
UiPointerWheelDelta s_mouseWheelDelta = 0;
/* fractions of relative motion not yet moved */
float s_relativeRemainderX = 0.0f;
float s_relativeRemainderY = 0.0f;

/* Numpad key with Num Lock on (VK_NUMPAD0..9 / VK_DECIMAL) or off (the navigation key it doubles as). */
KeyboardVirtualKeyCode NumpadKey(bool numLock, KeyboardVirtualKeyCode withNumLock,
                                 KeyboardVirtualKeyCode withoutNumLock) noexcept
{
  return numLock ? withNumLock : withoutNumLock;
}

/* The Windows virtual-key code of an SDL key event, VirtualKey::None for keys the game never sees. Letters
   follow the keyboard layout (as Windows' virtual keys do), the other keys their position. */
KeyboardVirtualKeyCode VirtualKeyOf(const SDL_KeyboardEvent &event) noexcept
{
  const SDL_Scancode scancode = event.scancode;
  if ((event.key >= SDLK_A) && (event.key <= SDLK_Z)) {
    return static_cast<KeyboardVirtualKeyCode>('A' + (event.key - SDLK_A));
  }
  if ((scancode >= SDL_SCANCODE_A) && (scancode <= SDL_SCANCODE_Z)) {
    return static_cast<KeyboardVirtualKeyCode>('A' + (scancode - SDL_SCANCODE_A));
  }
  if ((scancode >= SDL_SCANCODE_1) && (scancode <= SDL_SCANCODE_9)) {
    return static_cast<KeyboardVirtualKeyCode>('1' + (scancode - SDL_SCANCODE_1));
  }
  if ((scancode >= SDL_SCANCODE_F1) && (scancode <= SDL_SCANCODE_F12)) {
    return ToCode(VirtualKey::F1) + static_cast<KeyboardVirtualKeyCode>(scancode - SDL_SCANCODE_F1);
  }
  const bool numLock = (event.mod & SDL_KMOD_NUM) != 0;
  switch (scancode) {
  case SDL_SCANCODE_0: return '0';
  case SDL_SCANCODE_RETURN: return VK_RETURN;
  case SDL_SCANCODE_ESCAPE: return VK_ESCAPE;
  case SDL_SCANCODE_BACKSPACE: return VK_BACK;
  case SDL_SCANCODE_TAB: return VK_TAB;
  case SDL_SCANCODE_SPACE: return VK_SPACE;
  case SDL_SCANCODE_MINUS: return ToCode(VirtualKey::Minus);
  case SDL_SCANCODE_EQUALS: return ToCode(VirtualKey::Plus);
  case SDL_SCANCODE_LEFTBRACKET: return ToCode(VirtualKey::LeftBracket);
  case SDL_SCANCODE_RIGHTBRACKET: return ToCode(VirtualKey::RightBracket);
  case SDL_SCANCODE_BACKSLASH: return ToCode(VirtualKey::Backslash);
  case SDL_SCANCODE_NONUSHASH: return ToCode(VirtualKey::Backslash);
  case SDL_SCANCODE_SEMICOLON: return ToCode(VirtualKey::Semicolon);
  case SDL_SCANCODE_APOSTROPHE: return ToCode(VirtualKey::Apostrophe);
  case SDL_SCANCODE_GRAVE: return ToCode(VirtualKey::Grave);
  case SDL_SCANCODE_COMMA: return ToCode(VirtualKey::Comma);
  case SDL_SCANCODE_PERIOD: return ToCode(VirtualKey::Period);
  case SDL_SCANCODE_SLASH: return ToCode(VirtualKey::Slash);
  case SDL_SCANCODE_NONUSBACKSLASH: return ToCode(VirtualKey::Oem102);
  case SDL_SCANCODE_CAPSLOCK: return VK_CAPITAL;
  case SDL_SCANCODE_PRINTSCREEN: return VK_SNAPSHOT;
  case SDL_SCANCODE_SCROLLLOCK: return VK_SCROLL;
  case SDL_SCANCODE_PAUSE: return VK_PAUSE;
  case SDL_SCANCODE_INSERT: return VK_INSERT;
  case SDL_SCANCODE_HOME: return VK_HOME;
  case SDL_SCANCODE_PAGEUP: return VK_PRIOR;
  case SDL_SCANCODE_DELETE: return VK_DELETE;
  case SDL_SCANCODE_END: return VK_END;
  case SDL_SCANCODE_PAGEDOWN: return VK_NEXT;
  case SDL_SCANCODE_RIGHT: return VK_RIGHT;
  case SDL_SCANCODE_LEFT: return VK_LEFT;
  case SDL_SCANCODE_DOWN: return VK_DOWN;
  case SDL_SCANCODE_UP: return VK_UP;
  case SDL_SCANCODE_NUMLOCKCLEAR: return VK_NUMLOCK;
  case SDL_SCANCODE_KP_DIVIDE: return ToCode(VirtualKey::NumpadDivide);
  case SDL_SCANCODE_KP_MULTIPLY: return VK_MULTIPLY;
  case SDL_SCANCODE_KP_MINUS: return ToCode(VirtualKey::NumpadSubtract);
  case SDL_SCANCODE_KP_PLUS: return VK_ADD;
  case SDL_SCANCODE_KP_ENTER: return VK_RETURN;
  case SDL_SCANCODE_KP_1: return NumpadKey(numLock, VK_NUMPAD1, VK_END);
  case SDL_SCANCODE_KP_2: return NumpadKey(numLock, VK_NUMPAD2, VK_DOWN);
  case SDL_SCANCODE_KP_3: return NumpadKey(numLock, VK_NUMPAD3, VK_NEXT);
  case SDL_SCANCODE_KP_4: return NumpadKey(numLock, VK_NUMPAD4, VK_LEFT);
  case SDL_SCANCODE_KP_5: return NumpadKey(numLock, VK_NUMPAD5, ToCode(VirtualKey::Clear));
  case SDL_SCANCODE_KP_6: return NumpadKey(numLock, VK_NUMPAD6, VK_RIGHT);
  case SDL_SCANCODE_KP_7: return NumpadKey(numLock, VK_NUMPAD7, VK_HOME);
  case SDL_SCANCODE_KP_8: return NumpadKey(numLock, VK_NUMPAD8, VK_UP);
  case SDL_SCANCODE_KP_9: return NumpadKey(numLock, VK_NUMPAD9, VK_PRIOR);
  case SDL_SCANCODE_KP_0: return NumpadKey(numLock, VK_NUMPAD0, VK_INSERT);
  case SDL_SCANCODE_KP_PERIOD: return NumpadKey(numLock, ToCode(VirtualKey::NumpadDecimal), VK_DELETE);
  case SDL_SCANCODE_APPLICATION: return ToCode(VirtualKey::Applications);
  case SDL_SCANCODE_SELECT: return VK_SELECT;
  case SDL_SCANCODE_EXECUTE: return VK_EXECUTE;
  case SDL_SCANCODE_LCTRL:
  case SDL_SCANCODE_RCTRL: return VK_CONTROL;
  case SDL_SCANCODE_LSHIFT:
  case SDL_SCANCODE_RSHIFT: return VK_SHIFT;
  case SDL_SCANCODE_LALT:
  case SDL_SCANCODE_RALT: return VK_MENU;
  case SDL_SCANCODE_LGUI: return ToCode(VirtualKey::LeftWindows);
  case SDL_SCANCODE_RGUI: return ToCode(VirtualKey::RightWindows);
  default: return ToCode(VirtualKey::None);
  }
}

/* Win32_ShouldTranslateMessageFlags for a key press: whether Windows would turn it into WM_CHAR. Backspace, Tab,
   Enter, Pause, Escape, Space through Delete, the numpad and F1-F12 give no character. */
bool KeyProducesText(KeyboardVirtualKeyCode virtualKey) noexcept
{
  return (virtualKey != VK_BACK) && (virtualKey != VK_TAB) && (virtualKey != VK_RETURN) &&
         (virtualKey != VK_PAUSE) && (virtualKey != VK_ESCAPE) &&
         ((virtualKey < VK_SPACE) || ((VK_DELETE < virtualKey) && ((virtualKey < VK_NUMPAD0) || (VK_F12 < virtualKey))));
}

/* True for the right Alt key of a keyboard layout with AltGr. Windows sends such an AltGr press as a left Ctrl
   followed by the right Alt, so the original game saw Ctrl+Alt (the Ctrl+Alt key commands such as the cheat keys
   Ctrl+Alt+X/E/Z/V, and AltGr characters such as '@' arriving with Ctrl+Alt rather than as Alt shortcuts like
   Alt+Q). SDL drops that left Ctrl, so the backend adds it back. A layout has AltGr when some character key gives
   a character with SDL_KMOD_MODE (SDL's keymap asks Windows for Ctrl+Alt there). */
bool KeyIsAltGr(const SDL_KeyboardEvent &event) noexcept
{
  if (event.scancode != SDL_SCANCODE_RALT) {
    return false;
  }
  /* the letter, digit and punctuation keys (not Enter, Escape, Backspace, Tab and Space between them) */
  for (int scancode = SDL_SCANCODE_A; scancode <= SDL_SCANCODE_SLASH; scancode++) {
    if ((scancode >= SDL_SCANCODE_RETURN) && (scancode <= SDL_SCANCODE_SPACE)) {
      continue;
    }
    if (SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode), SDL_KMOD_MODE, false) != SDLK_UNKNOWN) {
      return true;
    }
  }
  return SDL_GetKeyFromScancode(SDL_SCANCODE_NONUSBACKSLASH, SDL_KMOD_MODE, false) != SDLK_UNKNOWN;
}

/* Windows-1252 code of a Unicode code point, 0 when it has none. */
KeyboardCharacterCode Cp1252FromCodePoint(char32_t codePoint) noexcept
{
  /* 0x80..0x9F of Windows-1252 */
  static constexpr char32_t kCp1252High[32] = {
      0x20AC, 0,      0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0, 0x017D, 0,
      0,      0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0, 0x017E, 0x0178};
  if ((codePoint < 0x80) || ((codePoint >= 0xA0) && (codePoint <= 0xFF))) {
    return static_cast<KeyboardCharacterCode>(codePoint);
  }
  for (std::size_t index = 0; index < std::size(kCp1252High); index++) {
    if ((kCp1252High[index] != 0) && (kCp1252High[index] == codePoint)) {
      return static_cast<KeyboardCharacterCode>(0x80 + index);
    }
  }
  return 0;
}

/* Decodes one UTF-8 sequence from text (advancing it); U+FFFD for a malformed one. */
char32_t NextCodePoint(std::span<const unsigned char> &text) noexcept
{
  const unsigned char lead = text[0];
  std::size_t length = 1;
  char32_t codePoint = 0xFFFD;
  if (lead < 0x80) {
    codePoint = lead;
  }
  else if (((lead & 0xE0) == 0xC0) && (text.size() >= 2)) {
    length = 2;
    codePoint = (static_cast<char32_t>(lead & 0x1F) << 6) | (text[1] & 0x3F);
  }
  else if (((lead & 0xF0) == 0xE0) && (text.size() >= 3)) {
    length = 3;
    codePoint = (static_cast<char32_t>(lead & 0x0F) << 12) | (static_cast<char32_t>(text[1] & 0x3F) << 6) |
                (text[2] & 0x3F);
  }
  else if (((lead & 0xF8) == 0xF0) && (text.size() >= 4)) {
    length = 4;
    codePoint = (static_cast<char32_t>(lead & 0x07) << 18) | (static_cast<char32_t>(text[1] & 0x3F) << 12) |
                (static_cast<char32_t>(text[2] & 0x3F) << 6) | (text[3] & 0x3F);
  }
  text = text.subspan(length);
  return codePoint;
}

/* The lock-key bits of g_KeyboardStateMask from SDL's modifier state (GetKeyState's toggle bits). */
UiKeyboardStateMask LockKeyBits() noexcept
{
  const SDL_Keymod modifiers = SDL_GetModState();
  UiKeyboardStateMask bits = KEYBOARD_STATE_NONE;
  if ((modifiers & SDL_KMOD_NUM) != 0) {
    bits |= KEYBOARD_STATE_NUM_LOCK;
  }
  if ((modifiers & SDL_KMOD_SCROLL) != 0) {
    bits |= KEYBOARD_STATE_SCROLL_LOCK;
  }
  if ((modifiers & SDL_KMOD_CAPS) != 0) {
    bits |= KEYBOARD_STATE_CAPS_LOCK;
  }
  return bits;
}

/* DirectInputMouse_AppendCursorEvent: appends one eventType entry to the g_CursorInputEvents ring, clamping the
   mouse position to the framebuffer on the way (the distance beyond each edge, plus one, goes to
   g_CursorOverflow*). The entry records the clamped position, wheel delta, clock and buttons. */
void AppendCursorEvent(GraphicsCursorEventType eventType) noexcept
{
  const uint32_t eventIndex = g_CursorInputWriteIndex;
  uint32_t nextWriteIndex = eventIndex + 1;
  if (CURSOR_INPUT_EVENT_RING_SIZE - 1 < nextWriteIndex) {
    nextWriteIndex = 0;
  }
  GraphicsCursorInputEvent18 &eventRecord = g_CursorInputEvents[eventIndex];
  g_CursorInputWriteIndex = nextWriteIndex;
  eventRecord.eventType = eventType;
  const GraphicsCursorButtonState buttonState = g_MouseButtonMask;
  const UiPointerWheelDelta wheelDelta = s_mouseWheelDelta;
  const uint32_t clockValue = g_CursorInputClockValue;
  g_CursorOverflowLeft = 0;
  g_CursorOverflowRight = 0;
  g_CursorOverflowTop = 0;
  g_CursorOverflowBottom = 0;
  /* right and bottom edges: compared one past the position, as in the original */
  uint32_t limitedXPlusOne = static_cast<uint32_t>(g_MouseX + 1);
  uint32_t limitedYPlusOne = static_cast<uint32_t>(g_MouseY + 1);
  if (static_cast<int>(g_FramebufferWidth) <= static_cast<int>(limitedXPlusOne)) {
    g_CursorOverflowRight = (limitedXPlusOne - g_FramebufferWidth) + 1;
    limitedXPlusOne = g_FramebufferWidth;
  }
  if (static_cast<int>(g_FramebufferHeight) <= static_cast<int>(limitedYPlusOne)) {
    g_CursorOverflowBottom = (limitedYPlusOne - g_FramebufferHeight) + 1;
    limitedYPlusOne = g_FramebufferHeight;
  }
  const int pointerX = static_cast<int>(limitedXPlusOne) - 1;
  const int pointerY = static_cast<int>(limitedYPlusOne) - 1;
  /* left and top edges */
  g_MouseX = pointerX;
  if (pointerX < 1) {
    g_MouseX = 0;
    g_CursorOverflowLeft = static_cast<uint32_t>(1 - pointerX);
  }
  int clampedY = pointerY;
  if (pointerY < 1) {
    clampedY = 0;
    g_CursorOverflowTop = static_cast<uint32_t>(1 - pointerY);
  }
  g_MouseY = clampedY;
  eventRecord.pointerX = g_MouseX;
  eventRecord.pointerY = clampedY;
  eventRecord.wheelDelta = wheelDelta;
  eventRecord.clockValue = clockValue;
  eventRecord.buttonState = buttonState;
}

/* Absolute mode (window): the event position in framebuffer pixels. */
void MoveToEventPosition(const SDL_Event &event) noexcept
{
  float windowX = 0.0f;
  float windowY = 0.0f;
  if (event.type == SDL_EVENT_MOUSE_MOTION) {
    windowX = event.motion.x;
    windowY = event.motion.y;
  }
  else if ((event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) || (event.type == SDL_EVENT_MOUSE_BUTTON_UP)) {
    windowX = event.button.x;
    windowY = event.button.y;
  }
  else {
    return;
  }
  float x = 0.0f;
  float y = 0.0f;
  WindowToFramebuffer(windowX, windowY, x, y);
  g_MouseX = static_cast<UiPixelCoordinate>(std::floor(x));
  g_MouseY = static_cast<UiPixelCoordinate>(std::floor(y));
}

/* Relative mode (fullscreen): moves the position by the raw device delta, as DirectInput's relative axes. At UI
   scale N a framebuffer pixel is N display pixels, so the delta is divided by N (the pointer keeps its speed on
   the screen). */
void MoveByRelativeMotion(const SDL_MouseMotionEvent &motion) noexcept
{
  const auto scale = static_cast<float>(std::max(AppliedUiScale(), 1));
  s_relativeRemainderX += motion.xrel / scale;
  s_relativeRemainderY += motion.yrel / scale;
  const float wholeX = std::trunc(s_relativeRemainderX);
  const float wholeY = std::trunc(s_relativeRemainderY);
  s_relativeRemainderX -= wholeX;
  s_relativeRemainderY -= wholeY;
  g_MouseX += static_cast<UiPixelCoordinate>(wholeX);
  g_MouseY += static_cast<UiPixelCoordinate>(wholeY);
}

/* The button-mask bit and press/release event types of an SDL button (DirectInput: button 0 left, 1 right,
   2 and 3 middle); false for buttons the game ignores. */
bool ButtonOf(Uint8 button, GraphicsCursorButtonState &mask, GraphicsCursorEventType &press,
              GraphicsCursorEventType &release) noexcept
{
  switch (button) {
  case SDL_BUTTON_LEFT:
    mask = LEFT;
    press = LEFT_PRESS;
    release = LEFT_RELEASE;
    return true;
  case SDL_BUTTON_RIGHT:
    mask = RIGHT;
    press = RIGHT_PRESS;
    release = RIGHT_RELEASE;
    return true;
  case SDL_BUTTON_MIDDLE:
  case SDL_BUTTON_X1:
    mask = MIDDLE;
    press = MIDDLE_PRESS;
    release = MIDDLE_RELEASE;
    return true;
  default:
    return false;
  }
}

SoftwareDisplayModeHookProc *s_chainedSetDisplayMode = nullptr;

} // namespace

void HandleKeyDown(const SDL_KeyboardEvent &event)
{
  const KeyboardVirtualKeyCode virtualKey = VirtualKeyOf(event);
  if (virtualKey == ToCode(VirtualKey::None)) {
    s_translateText = true;
    return;
  }
  if (KeyIsAltGr(event)) {
    Keyboard_OnKeyDown(VK_CONTROL);
  }
  Keyboard_OnKeyDown(virtualKey);
  s_translateText = KeyProducesText(virtualKey);
  /* Ctrl+A..Ctrl+Z: Windows' WM_CHAR 1..26 (SDL sends no text for control characters); Keyboard_OnChar turns
     them back into 'a'..'z' */
  const bool ctrl = (event.mod & SDL_KMOD_CTRL) != 0;
  const bool alt = (event.mod & SDL_KMOD_ALT) != 0;
  if (ctrl && !alt && (virtualKey >= 'A') && (virtualKey <= 'Z')) {
    Keyboard_OnChar(virtualKey - 'A' + 1);
  }
}

void HandleKeyUp(const SDL_KeyboardEvent &event)
{
  const KeyboardVirtualKeyCode virtualKey = VirtualKeyOf(event);
  if (virtualKey != ToCode(VirtualKey::None)) {
    if (KeyIsAltGr(event)) {
      Keyboard_OnKeyUp(VK_CONTROL);
    }
    Keyboard_OnKeyUp(virtualKey);
  }
}

void HandleTextInput(const SDL_TextInputEvent &event)
{
  if (!s_translateText || (event.text == nullptr)) {
    return;
  }
  const auto *bytes = reinterpret_cast<const unsigned char *>(event.text);
  std::span<const unsigned char> text(bytes, SDL_strlen(event.text));
  while (!text.empty()) {
    const KeyboardCharacterCode character = Cp1252FromCodePoint(NextCodePoint(text));
    if (character != 0) {
      Keyboard_OnChar(character);
    }
  }
}

void HandleMouseEvent(const SDL_Event &event)
{
  /* developer tools: while an input script drives the game the real mouse is ignored */
  if (DebugHook_IgnoreRealMouse()) {
    return;
  }
  const bool relative = !AbsoluteMouse();
  GraphicsCursorEventType eventType = MOTION_OR_WHEEL;
  s_mouseWheelDelta = 0;
  switch (event.type) {
  case SDL_EVENT_MOUSE_MOTION:
    if (relative) {
      MoveByRelativeMotion(event.motion);
    }
    else {
      MoveToEventPosition(event);
    }
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  case SDL_EVENT_MOUSE_BUTTON_UP: {
    GraphicsCursorButtonState mask = CURSOR_BUTTON_NONE;
    GraphicsCursorEventType press = MOTION_OR_WHEEL;
    GraphicsCursorEventType release = MOTION_OR_WHEEL;
    if (!ButtonOf(event.button.button, mask, press, release)) {
      return;
    }
    if (!relative) {
      MoveToEventPosition(event);
    }
    if (event.button.down) {
      g_MouseButtonMask = g_MouseButtonMask | mask;
      eventType = press;
    }
    else {
      g_MouseButtonMask = g_MouseButtonMask & ~mask;
      eventType = release;
    }
    break;
  }
  case SDL_EVENT_MOUSE_WHEEL: {
    int notches = event.wheel.integer_y;
    if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
      notches = -notches;
    }
    if (notches == 0) {
      return;
    }
    s_mouseWheelDelta = notches;
    break;
  }
  default:
    return;
  }
  AppendCursorEvent(eventType);
}

void HandleFocusGained()
{
  /* MainWindowProc on WM_ACTIVATEAPP: back to the game's priority, lock keys reseeded (modifiers released),
     queued keys dropped. The original restored real-time priority; high here (see ProcessEntry). */
  SetPriorityClass(GetCurrentProcess(), DebugHook_ProcessPriorityClass(HIGH_PRIORITY_CLASS));
  g_KeyboardStateMask = LockKeyBits();
  g_KeyboardFlushEvents();
}

void HandleFocusLost()
{
  SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
}

void UpdateMouseMode() noexcept
{
  if (MainWindow() != nullptr) {
    SDL_SetWindowRelativeMouseMode(MainWindow(), !AbsoluteMouse());
  }
}

} // namespace thandor::sdl3

using namespace thandor::sdl3;

bool SdlInput_Init(uint32_t *outError)
{
  SDL_HideCursor();
  /* chain in front of the graphics display-mode switch */
  s_chainedSetDisplayMode = g_GraphicsSetDisplayMode;
  g_GraphicsSetDisplayMode = SdlInput_SetDisplayMode;
  /* the cursor animation and input clock; the mouse itself comes with the events of the pump */
  g_TimerRegisterPeriodic(CURSOR_ANIMATION_TIMER_HZ, GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
  g_PointerFlushEvents = SdlInput_FlushEvents;
  g_PointerSetPosition = SdlInput_SetPosition;
  if (!GraphicsCursor_LoadAssets(outError)) {
    return false;
  }
  g_KeyboardStateMask |= LockKeyBits();
  UpdateMouseMode();
  return true;
}

void SdlInput_Shutdown()
{
  /* The original called the unregister hook unconditionally; checked here because the hook is only installed
     by SdlPlatform_InstallTimersAndPump, so a fatal error before it (file system, locale, error system,
     DynAPI bootstrap) reached Runtime_Shutdown with a null pointer and crashed. */
  if (g_TimerUnregisterPeriodic != nullptr) {
    g_TimerUnregisterPeriodic(GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
  }
  if (MainWindow() != nullptr) {
    SDL_SetWindowRelativeMouseMode(MainWindow(), false);
  }
  SDL_ShowCursor();
}

bool SdlInput_SetDisplayMode(uint32_t adapterIndex,uint32_t bitsPerPixel,uint32_t height,uint32_t width,
                              uint32_t *errorCode)
{
  GraphicsCursor_FreeBuffers();
  if (!s_chainedSetDisplayMode(adapterIndex, bitsPerPixel, height, width, errorCode)) {
    return false; /* the chained hook's error is passed through */
  }
  /* the framebuffer size: at a GPU UI scale N > 1 the mode (height x width) is N x it */
  if (!GraphicsCursor_CreateBuffersAndCenter(g_FramebufferHeight, g_FramebufferWidth, errorCode)) {
    return false;
  }
  g_GraphicsBackendAccessState = 0;
  return true;
}

void SdlInput_SetPosition(int32_t positionY,int32_t positionX)
{
  g_CursorOverrideX = positionX;
  g_CursorOverrideY = positionY;
  g_CursorWheelDelta = 0;
  g_MouseX = positionX;
  g_MouseY = positionY;
  s_mouseWheelDelta = 0;
  /* the window follows the system mouse: move it along (not while a script drives the game, and only while it
     is over this window, so tests never move the desktop mouse) */
  if (AbsoluteMouse() && !DebugHook_IgnoreRealMouse() && (SDL_GetMouseFocus() == MainWindow())) {
    float windowX = 0.0f;
    float windowY = 0.0f;
    FramebufferToWindow(static_cast<float>(positionX) + 0.5f, static_cast<float>(positionY) + 0.5f, windowX, windowY);
    SDL_WarpMouseInWindow(MainWindow(), windowX, windowY);
  }
}

void SdlInput_FlushEvents()
{
  g_CursorOverrideX = g_MouseX;
  g_CursorOverrideY = g_MouseY;
  g_CursorWheelDelta = s_mouseWheelDelta;
  g_CursorInputWriteIndex = g_CursorInputReadIndex;
}
