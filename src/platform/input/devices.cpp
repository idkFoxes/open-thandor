/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/input/devices.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/input/devices.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(8) UiPixelCoordinate g_CursorOverrideX = 0;

THANDOR_ALIGN(4) UiPixelCoordinate g_CursorOverrideY = 0;

THANDOR_ALIGN(4) int32_t g_CursorVisibilityToken = -1;

THANDOR_ALIGN(8) GraphicsCursorButtonState g_CursorButtonState = CURSOR_BUTTON_NONE;

THANDOR_ALIGN(16) uint8_t g_KeyboardSpecialKeyDown[32] = {};

THANDOR_ALIGN(16) KeyboardFlushEventsProc *g_KeyboardFlushEvents = &Keyboard_FlushEvents;

THANDOR_ALIGN(8) uint32_t g_KeyboardStateMask = 0;

static uint32_t g_CursorMaxWidth = 0;

static uint32_t g_CursorMaxHeight = 0;

static uint16_t g_EngineMouseGfxPathUtf16[17] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'm', 'o', 'u', 's', 'e', '.', 'g', 'f', 'x', 0}; /* L"engine\\mouse.gfx" */

static uint16_t g_EngineMouseDatPathUtf16[17] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'm', 'o', 'u', 's', 'e', '.', 'd', 'a', 't', 0}; /* L"engine\\mouse.dat" */

/* g_KeyboardEvents. Original quirk: the original reserves 256 events (0x800 bytes) for the ring, but
   the read and write indices wrap at KEYBOARD_EVENT_RING_SIZE (64), so entries 64-255 are never used. */
static KeyboardInputEvent g_KeyboardEvents[256] = {};

static KeyboardEventRingIndex g_KeyboardWriteIndex = 0;

static KeyboardEventRingIndex g_KeyboardReadIndex = 0;

static uint32_t g_KeyboardToggleLatchMask = 0;

uint32_t g_CursorInputWriteIndex = 0;

PointerFlushEventsProc *g_PointerFlushEvents = nullptr;

PointerSetPositionProc *g_PointerSetPosition = nullptr;

uint32_t g_CursorOverflowLeft = 0;

uint32_t g_CursorOverflowRight = 0;

uint32_t g_CursorOverflowTop = 0;

uint32_t g_CursorOverflowBottom = 0;

UiPixelCoordinate g_MouseX = 0;

UiPixelCoordinate g_MouseY = 0;

GraphicsCursorButtonState g_MouseButtonMask = CURSOR_BUTTON_NONE;

KeyboardReadEventProc *g_KeyboardReadEvent = &Keyboard_ReadNextEvent;

uint32_t g_CursorUseOverridePosition = 0;

UiPointerWheelDelta g_CursorWheelDelta = 0;

/* Case-insensitive character compare, reached through the compareCaseInsensitiveFlags slot of
   g_KeyboardAsciiCaseTransformCallbacks3. Both 16-bit code units are upper-cased; returns true when
   upper(right) < upper(left).
*/
Bool8 Keyboard_CompareAsciiCaseInsensitiveFlags(KeyboardCharacterCode leftCodeUnit,KeyboardCharacterCode rightCodeUnit)

{
  uint32_t leftLowWord;
  uint32_t upperRight;
  uint32_t upperLeft;

  leftLowWord = leftCodeUnit & 0xffff;
  upperRight = Keyboard_ToUpperAscii(rightCodeUnit & 0xffff);
  upperLeft = Keyboard_ToUpperAscii(leftLowWord);
  return upperRight < upperLeft;
}


/* Discards every queued keyboard event by moving the ring's write index back onto its read index.
   Reached through the g_KeyboardFlushEvents pointer.
*/
void Keyboard_FlushEvents()

{
  g_KeyboardWriteIndex = g_KeyboardReadIndex;
}


/* Takes the oldest event out of the keyboard ring, reached through the g_KeyboardReadEvent pointer.
   Returns true with the key code in *outKeyCode and the modifier state in *outStateMask, or
   false (outputs untouched) when the ring is empty.
*/
Bool8 Keyboard_ReadNextEvent(uint32_t *outKeyCode, uint32_t *outStateMask)

{
  uint32_t nextReadIndex;
  KeyboardInputEvent *eventRecord;

  nextReadIndex = g_KeyboardReadIndex + 1;
  if (g_KeyboardReadIndex != g_KeyboardWriteIndex) {
    if (KEYBOARD_EVENT_RING_SIZE - 1 < nextReadIndex) {
      nextReadIndex = 0;
    }
    eventRecord = g_KeyboardEvents + g_KeyboardReadIndex;
    g_KeyboardReadIndex = nextReadIndex;
    *outKeyCode = eventRecord->keyCode;
    *outStateMask = eventRecord->stateMask;
    return true;
  }
  return false;
}


/* Converts ASCII 'A'-'Z' to 'a'-'z' and returns every other value unchanged. Reached through the toLower
   slot of g_KeyboardAsciiCaseTransformCallbacks3.
*/
uint32_t Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit)

{
  if (('A' - 1 < asciiCodeUnit) && (asciiCodeUnit < 'Z' + 1)) {
    asciiCodeUnit = asciiCodeUnit + ('a' - 'A');
  }
  return asciiCodeUnit;
}


/* Step of the original's DirectInputMouse_Init, called by SdlInput_Init: loads the cursor images
   (engine\mouse.gfx; the largest image size sizes the cursor buffers) and the frame table (engine\mouse.dat) and
   starts every cursor on the first frame of its animations. Returns false with the load error in *outError. */
Bool8 GraphicsCursor_LoadAssets(uint32_t *outError)

{
  GraphicsSubresourceIndex activeFirstSubresource;
  GraphicsTextureSourceAsset *cursorAsset;
  GraphicsCursorFrameRecord *frameRecord;
  uint32_t remainingFrames;
  uint32_t subresourceIndex;
  uint32_t maxHeight;
  uint32_t maxWidth;
  void *cursorFrameData;
  uint32_t cursorFrameBytes;
  uint32_t cursorLoadErrorCode;
  GraphicsTextureLogicalSize logicalSize;

  /* cursor images: the largest image size sizes the cursor buffers */
  cursorAsset = static_cast<GraphicsTextureSourceAsset *>(Package_LoadEntry(g_EngineMouseGfxPathUtf16,&cursorLoadErrorCode));
  if (cursorAsset == nullptr) {
    *outError = cursorLoadErrorCode;
    return false;
  }
  maxWidth = 0;
  maxHeight = 0;
  subresourceIndex = 0;
  g_CursorSourceAsset = cursorAsset;
  do {
    logicalSize = g_GraphicsTextureSourceGetLogicalSize(subresourceIndex,cursorAsset);
    subresourceIndex++;
    if ((int)maxWidth < (int)logicalSize.logicalWidthPixels) {
      maxWidth = logicalSize.logicalWidthPixels;
    }
    if ((int)maxHeight < (int)logicalSize.logicalHeightPixels) {
      maxHeight = logicalSize.logicalHeightPixels;
    }
  } while (subresourceIndex < (cursorAsset->tableDescriptor).subresourceCount);
  g_CursorMaxWidth = maxWidth;
  g_CursorMaxHeight = maxHeight;

  /* cursor frame table */
  if (!Resource_Load(g_EngineMouseDatPathUtf16,&cursorFrameData,&cursorFrameBytes,&cursorLoadErrorCode)) {
    *outError = cursorLoadErrorCode;
    return false;
  }
  remainingFrames = cursorFrameBytes / sizeof(GraphicsCursorFrameRecord);
  frameRecord = static_cast<GraphicsCursorFrameRecord *>(cursorFrameData);
  g_CursorFrameRecords = frameRecord;
  g_CursorFrameCount = remainingFrames;
  /* every cursor starts on the first frame of its animations */
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    activeFirstSubresource = frameRecord->activeAnimationFirstSubresourceIndex;
    frameRecord->idleSubresourceIndex = frameRecord->idleAnimationFirstSubresourceIndex;
    frameRecord->activeSubresourceIndex = activeFirstSubresource;
    frameRecord++;
    remainingFrames--;
  } while (remainingFrames != 0);
  return true;
}


/* First half of the mouse display-mode hook (SdlInput_SetDisplayMode): blocks backend access (timer cursor
   drawing) for the switch and frees the three cursor buffers. */
void GraphicsCursor_FreeBuffers()

{
  g_GraphicsBackendAccessState = -1; /* blocks backend access (timer cursor drawing) during the switch */
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = nullptr;
  g_CursorCompositeBuffer = nullptr;
  g_CursorAlternateSavedBackground = nullptr;
}


/* Second half of the mouse display-mode hook (SdlInput_SetDisplayMode), after the mode switch: recreates
   the three cursor buffers in the new pixel format, converts the cursor palette and centres the mouse. Returns
   false with the allocator error in *errorCode when a buffer creation fails (the buffers created so far stay
   installed). */
Bool8 GraphicsCursor_CreateBuffersAndCenter
          (GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth,uint32_t *errorCode)

{
  SoftwareFramebufferAccess *primaryFramebuffer;
  SoftwareFramebufferAccess *newCursorFramebuffer;
  SoftwareFramebufferAccess *newCompositeFramebuffer;

  primaryFramebuffer = g_FramebufferAccess;
  /* a failing create stores its allocator error in *errorCode */
  newCursorFramebuffer = g_SoftwareFramebufferCreate
                   (g_FramebufferAccess->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCursorFramebuffer == nullptr) {
    return false;
  }
  g_CursorSavedBackground = newCursorFramebuffer;
  newCompositeFramebuffer = g_SoftwareFramebufferCreate
                   (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCompositeFramebuffer == nullptr) {
    return false;
  }
  g_CursorCompositeBuffer = newCompositeFramebuffer;
  newCursorFramebuffer = g_SoftwareFramebufferCreate
                   (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCursorFramebuffer == nullptr) {
    return false;
  }
  g_CursorAlternateSavedBackground = newCursorFramebuffer;
  g_GraphicsTextureSourceConvertPaletteEntries
            (reinterpret_cast<GraphicsPaletteTextureSourceAsset *>(g_CursorSourceAsset));
  g_CursorOverrideX = framebufferWidth >> 1;
  g_CursorOverrideY = framebufferHeight >> 1;
  g_MouseX = g_CursorOverrideX;
  g_MouseY = g_CursorOverrideY;
  return true;
}


/* Keyboard_OnKeyDown/OnKeyUp: the KEYBOARD_STATE_* bit of a Shift, Ctrl or Alt key (VK_SHIFT, VK_CONTROL
   and VK_MENU give both sides' bits), 0 for every other key. */
static uint32_t Keyboard_ModifierStateBits(KeyboardVirtualKeyCode virtualKey)
{
  switch (virtualKey) {
  case VK_LSHIFT:   return KEYBOARD_STATE_LEFT_SHIFT;
  case VK_SHIFT:    return KEYBOARD_STATE_SHIFT;
  case VK_RSHIFT:   return KEYBOARD_STATE_RIGHT_SHIFT;
  case VK_LMENU:    return KEYBOARD_STATE_LEFT_ALT;
  case VK_MENU:     return KEYBOARD_STATE_ALT;
  case VK_RMENU:    return KEYBOARD_STATE_RIGHT_ALT;
  case VK_LCONTROL: return KEYBOARD_STATE_LEFT_CTRL;
  case VK_CONTROL:  return KEYBOARD_STATE_CTRL;
  case VK_RCONTROL: return KEYBOARD_STATE_RIGHT_CTRL;
  default:          return 0;
  }
}


/* Keyboard_OnKeyDown/OnKeyUp: the KEYBOARD_STATE_* bit of a lock key, 0 for every other key. */
static uint32_t Keyboard_LockStateBit(KeyboardVirtualKeyCode virtualKey)
{
  switch (virtualKey) {
  case VK_CAPITAL: return KEYBOARD_STATE_CAPS_LOCK;
  case VK_NUMLOCK: return KEYBOARD_STATE_NUM_LOCK;
  case VK_SCROLL:  return KEYBOARD_STATE_SCROLL_LOCK;
  default:         return 0;
  }
}


/* Keyboard_OnKeyDown/OnKeyUp: the 0x10000-family code of a navigation key (the cursor block, VK_SELECT and
   the numpad digits and decimal point, which map to the same codes), 0 for every other key. */
static uint32_t Keyboard_NavigationKeyCode(KeyboardVirtualKeyCode virtualKey)
{
  switch (virtualKey) {
  case VK_DELETE:
  case VK_DECIMAL: return KEYBOARD_KEY_CODE_DELETE;
  case VK_INSERT:
  case VK_NUMPAD0: return KEYBOARD_KEY_CODE_INSERT;
  case VK_HOME:
  case VK_NUMPAD7: return KEYBOARD_KEY_CODE_HOME;
  case VK_END:
  case VK_NUMPAD1: return KEYBOARD_KEY_CODE_END;
  case VK_PRIOR:
  case VK_NUMPAD9: return KEYBOARD_KEY_CODE_PAGE_UP;
  case VK_NEXT:
  case VK_NUMPAD3: return KEYBOARD_KEY_CODE_PAGE_DOWN;
  case VK_LEFT:
  case VK_NUMPAD4: return KEYBOARD_KEY_CODE_LEFT;
  case VK_RIGHT:
  case VK_NUMPAD6: return KEYBOARD_KEY_CODE_RIGHT;
  case VK_UP:
  case VK_NUMPAD8: return KEYBOARD_KEY_CODE_UP;
  case VK_DOWN:
  case VK_NUMPAD2: return KEYBOARD_KEY_CODE_DOWN;
  case VK_SELECT:
  case VK_NUMPAD5: return KEYBOARD_KEY_CODE_NUMPAD_5;
  default:         return 0;
  }
}


/* Keyboard_OnKeyDown: the KEYBOARD_KEY_CODE_* event code of a non-modifier, non-lock key in *outKeyCode.
   Digits and letters give KEYBOARD_KEY_CODE_CHAR of their ASCII code (letters lowercase), the numpad
   operators their plain ASCII character. Returns false for keys that queue no event. */
static Bool8 Keyboard_MapKeyDownCode(KeyboardVirtualKeyCode virtualKey,uint32_t *outKeyCode)
{
  uint32_t navigationCode;

  switch (virtualKey) {
  case VK_SPACE:     *outKeyCode = KEYBOARD_KEY_CODE_SPACE; return true;
  case VK_ESCAPE:    *outKeyCode = KEYBOARD_KEY_CODE_ESCAPE; return true;
  case VK_RETURN:
  case VK_SEPARATOR: *outKeyCode = KEYBOARD_KEY_CODE_ENTER; return true;
  case VK_TAB:       *outKeyCode = KEYBOARD_KEY_CODE_TAB; return true;
  case VK_BACK:      *outKeyCode = KEYBOARD_KEY_CODE_BACKSPACE; return true;
  case VK_PRINT:
  case VK_SNAPSHOT:  *outKeyCode = KEYBOARD_KEY_CODE_PRINT; return true;
  case VK_EXECUTE:
  case VK_PAUSE:     *outKeyCode = KEYBOARD_KEY_CODE_PAUSE; return true;
  case VK_F1:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(1); return true;
  case VK_F2:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(2); return true;
  case VK_F3:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(3); return true;
  case VK_F4:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(4); return true;
  case VK_F5:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(5); return true;
  case VK_F6:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(6); return true;
  case VK_F7:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(7); return true;
  case VK_F8:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(8); return true;
  case VK_F9:        *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(9); return true;
  case VK_F10:       *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(10); return true;
  case VK_F11:       *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(11); return true;
  case VK_F12:       *outKeyCode = KEYBOARD_KEY_CODE_FUNCTION(12); return true;
  case VK_MULTIPLY:  *outKeyCode = '*'; return true;
  case VK_DIVIDE:    *outKeyCode = '/'; return true;
  case VK_ADD:       *outKeyCode = '+'; return true;
  case VK_SUBTRACT:  *outKeyCode = '-'; return true;
  default:           break;
  }
  if (('0' <= virtualKey) && (virtualKey <= '9')) {
    *outKeyCode = KEYBOARD_KEY_CODE_CHAR(virtualKey);
    return true;
  }
  if (('A' <= virtualKey) && (virtualKey <= 'Z')) {
    *outKeyCode = KEYBOARD_KEY_CODE_CHAR(virtualKey + ('a' - 'A'));
    return true;
  }
  navigationCode = Keyboard_NavigationKeyCode(virtualKey);
  if (navigationCode == 0) {
    return false;
  }
  *outKeyCode = navigationCode;
  return true;
}


/* WM_KEYDOWN/WM_SYSKEYDOWN handler: Shift, Ctrl and Alt set their KEYBOARD_STATE_* bits, the lock keys toggle
   theirs once per press (g_KeyboardToggleLatchMask stops auto-repeat from toggling again), and every other
   mapped key is queued as a KEYBOARD_KEY_CODE_* event with the modifier state of the moment in the 64-entry
   keyboard ring (0x10000-family keys also mark g_KeyboardSpecialKeyDown).
*/
void Keyboard_OnKeyDown(KeyboardVirtualKeyCode virtualKey)

{
  KeyboardInputEvent *eventRecord;
  KeyboardEventRingIndex writeIndex;
  uint32_t queuedStateMask;
  uint32_t modifierBits;
  uint32_t lockBit;
  uint32_t keyCode;
  uint32_t nextWriteIndex;

  queuedStateMask = g_KeyboardStateMask;
  modifierBits = Keyboard_ModifierStateBits(virtualKey);
  if (modifierBits != 0) {
    g_KeyboardStateMask = g_KeyboardStateMask | modifierBits;
    return;
  }
  lockBit = Keyboard_LockStateBit(virtualKey);
  if (lockBit != 0) {
    /* toggle once per press; auto-repeat finds the latch set */
    if ((g_KeyboardToggleLatchMask & lockBit) == 0) {
      g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask | lockBit;
      g_KeyboardStateMask = g_KeyboardStateMask ^ lockBit;
    }
    return;
  }
  if (!Keyboard_MapKeyDownCode(virtualKey,&keyCode)) {
    return;
  }
  writeIndex = g_KeyboardWriteIndex;
  eventRecord = g_KeyboardEvents + writeIndex;
  nextWriteIndex = writeIndex + 1;
  g_KeyboardWriteIndex = nextWriteIndex;
  eventRecord->keyCode = keyCode;
  eventRecord->stateMask = queuedStateMask;
  /* KEYBOARD_KEY_CODE_SPECIAL family: remember the key as held */
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == KEYBOARD_KEY_CODE_SPECIAL(0)) {
    g_KeyboardSpecialKeyDown[keyCode & KEYBOARD_KEY_CODE_INDEX_MASK] = 1;
  }
  /* wrap the ring */
  if (KEYBOARD_EVENT_RING_SIZE - 1 < nextWriteIndex) {
    g_KeyboardWriteIndex = 0;
  }
}


/* WM_KEYUP/WM_SYSKEYUP handler: clears the modifier bits of Shift, Ctrl and Alt, re-arms the Num/Scroll Lock
   toggle, and releases the g_KeyboardSpecialKeyDown entry of a navigation key (cursor block, VK_SELECT,
   numpad digits and decimal point; the held state the in-game camera keys poll). Original quirk: Escape,
   Enter, Tab, Backspace, Print and Pause also set their entry on key-down but are never released here. Queues no event.
*/
void Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey)

{
  uint32_t modifierBits;
  uint32_t navigationCode;

  modifierBits = Keyboard_ModifierStateBits(virtualKey);
  if (modifierBits != 0) {
    /* releasing Shift also clears Caps Lock */
    if ((virtualKey == VK_LSHIFT) || (virtualKey == VK_SHIFT) || (virtualKey == VK_RSHIFT)) {
      modifierBits = modifierBits | KEYBOARD_STATE_CAPS_LOCK;
    }
    g_KeyboardStateMask = g_KeyboardStateMask & ~modifierBits;
    return;
  }
  /* VK_CAPITAL is not handled here, so its toggle latch is never re-armed by a key-up */
  if ((virtualKey == VK_NUMLOCK) || (virtualKey == VK_SCROLL)) {
    g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask & ~Keyboard_LockStateBit(virtualKey);
    return;
  }
  /* only the navigation keys release a g_KeyboardSpecialKeyDown entry */
  navigationCode = Keyboard_NavigationKeyCode(virtualKey);
  if (navigationCode != 0) {
    g_KeyboardSpecialKeyDown[navigationCode & KEYBOARD_KEY_CODE_INDEX_MASK] = 0;
  }
}


/* WM_CHAR/WM_SYSCHAR handler (called from the main window procedure): queues the 16-bit character with
   the current modifier state in the keyboard ring. With Ctrl held, the control characters 1-26 that
   Windows delivers for Ctrl+A..Ctrl+Z are turned back into 'a'..'z'.
*/
void Keyboard_OnChar(KeyboardCharacterCode character)

{
  KeyboardInputEvent *eventRecord;
  KeyboardEventRingIndex writeIndex;
  UiKeyboardEventCode keyCode;
  uint32_t nextWriteIndex;
  
  writeIndex = g_KeyboardWriteIndex;
  eventRecord = g_KeyboardEvents + g_KeyboardWriteIndex;
  keyCode = character & 0xffff;
  nextWriteIndex = g_KeyboardWriteIndex + 1;
  g_KeyboardWriteIndex = g_KeyboardWriteIndex + 1;
  if (((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) && (keyCode < 26 + 1)) {
    keyCode = keyCode + ('a' - 1);
  }
  g_KeyboardEvents[writeIndex].stateMask = g_KeyboardStateMask;
  eventRecord->keyCode = keyCode;
  if (KEYBOARD_EVENT_RING_SIZE - 1 < nextWriteIndex) {
    g_KeyboardWriteIndex = 0;
  }
}


/* Converts ASCII 'a'-'z' to 'A'-'Z' and returns every other value unchanged. Reached through the toUpper
   slot of g_KeyboardAsciiCaseTransformCallbacks3; Keyboard_CompareAsciiCaseInsensitiveFlags
   also calls it directly.
*/
uint32_t Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit)

{
  if (('a' - 1 < asciiCodeUnit) && (asciiCodeUnit < 'z' + 1)) {
    asciiCodeUnit = asciiCodeUnit - ('a' - 'A');
  }
  return asciiCodeUnit;
}


/* Class vtables. */

KeyboardAsciiCaseTransformCallbackTable3 g_KeyboardAsciiCaseTransformCallbacks3 = {
    .compareCaseInsensitiveFlags = THANDOR_SLOT(Keyboard_CompareAsciiCaseInsensitiveFlags),
    .toUpper = THANDOR_SLOT(Keyboard_ToUpperAscii),
    .toLower = THANDOR_SLOT(Keyboard_ToLowerAscii)};
