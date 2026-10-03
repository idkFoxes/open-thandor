/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/input/devices.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/input/devices.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>
#include <intrin.h>

/* Module data. */

__declspec(align(8)) UiPixelCoordinate g_CursorOverrideX = 0;

__declspec(align(4)) UiPixelCoordinate g_CursorOverrideY = 0;

__declspec(align(4)) int32_t g_CursorVisibilityToken = -1;

__declspec(align(8)) uint32_t g_CursorButtonState = 0;

__declspec(align(16)) uint8_t g_KeyboardSpecialKeyDown[32] = {0};

__declspec(align(16)) KeyboardFlushEventsProc *g_KeyboardFlushEvents = (void *)Keyboard_FlushEvents;

__declspec(align(8)) uint32_t g_KeyboardStateMask = 0;

static TH_LEGACY_GUID GUID_SysMouse_Local = {.Data1 = 0x6F1D2B60, .Data2 = 54688, .Data3 = 4559, .Data4 = "\277\307DEST"};

static TH_LEGACY_GUID GUID_XAxis_Local = {.Data1 = 0xA36D02E0, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

static TH_LEGACY_GUID GUID_YAxis_Local = {.Data1 = 0xA36D02E1, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

static TH_LEGACY_GUID GUID_ZAxis_Local = {.Data1 = 0xA36D02E2, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

static DIOBJECTDATAFORMAT MouseObjectFormats[7] = {
    /* 0 */ {.pguid = (void *)&GUID_XAxis_Local, .dwType = 0xFFFF03},
    /* 1 */ {.pguid = (void *)&GUID_YAxis_Local, .dwOfs = 4, .dwType = 0xFFFF03},
    /* 2 */ {.pguid = (void *)&GUID_ZAxis_Local, .dwOfs = 8, .dwType = 0x80FFFF03},
    /* 3 */ {.dwOfs = 12, .dwType = 0xFFFF0C},
    /* 4 */ {.dwOfs = 13, .dwType = 0xFFFF0C},
    /* 5 */ {.dwOfs = 14, .dwType = 0x80FFFF0C},
    /* 6 */ {.dwOfs = 15, .dwType = 0x80FFFF0C}};

static uint32_t g_CursorMaxWidth = 0;

static uint32_t g_CursorMaxHeight = 0;

static uint16_t u_engine_mouse_gfx_00416864[17] = L"engine\\mouse.gfx";

static uint16_t u_engine_mouse_dat_00416886[17] = L"engine\\mouse.dat";

/* g_KeyboardEvents. Original quirk: the original reserves 256 events (0x800 bytes) for the ring, but
   the read and write indices wrap at KEYBOARD_EVENT_RING_SIZE (64), so entries 64-255 are never used. */
static KeyboardInputEvent g_KeyboardEvents[256] = {0};

static KeyboardEventRingIndex g_KeyboardWriteIndex = 0;

static KeyboardEventRingIndex g_KeyboardReadIndex = 0;

static uint32_t g_KeyboardToggleLatchMask = 0;

static DirectInputCreateA *pDirectInputCreateA = 0;

static char dynapi_3[7] = "DINPUT";

static char dynapi_19[19] = "DirectInputCreateA";

static IDirectInputA *g_DirectInput = 0;

static DIDATAFORMAT MouseDataFormat = {
    .dwSize = 24,
    .dwObjSize = 16,
    .dwFlags = 0x2,
    .dwDataSize = 16,
    .dwNumObjs = 7,
    .rgodf = (void *)&MouseObjectFormats};

static DIPROPDWORD MouseBufferProperty = {.diph = {.dwSize = 20, .dwHeaderSize = 16}, .dwData = 256};

static uint32_t g_MouseDeviceDataCount = 0;

static uint32_t g_MousePollBusy = 0;

static DIDEVICEOBJECTDATA_DX3 g_MouseDeviceEvent = {0};

static UiPointerWheelDelta g_MouseWheelDelta = 0;

static SoftwareDisplayModeHookProc *g_DirectInputMouseChainedSetDisplayMode = 0;

uint32_t g_CursorInputWriteIndex = 0;

PointerFlushEventsProc *g_PointerFlushEvents = 0;

PointerSetPositionProc *g_PointerSetPosition = 0;

uint32_t g_CursorOverflowLeft = 0;

uint32_t g_CursorOverflowRight = 0;

uint32_t g_CursorOverflowTop = 0;

uint32_t g_CursorOverflowBottom = 0;

IDirectInputDeviceA *g_MouseDevice = 0;

UiPixelCoordinate g_MouseX = 0;

UiPixelCoordinate g_MouseY = 0;

GraphicsCursorButtonState g_MouseButtonMask = 0;

KeyboardReadEventProc *g_KeyboardReadEvent = (void *)Keyboard_ReadNextEvent;

uint32_t g_CursorUseOverridePosition = 0;

UiPointerWheelDelta g_CursorWheelDelta = 0;

/* Implementation ownership: platform/input/devices. */

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
void Keyboard_FlushEvents(void)

{
  g_KeyboardWriteIndex = g_KeyboardReadIndex;
  return;
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


/* DirectInputMouse_Init failure exit for the DirectInput setup: records the failed stage (0 = DirectInput
   object, 1 = device, 2 = data format, 3 = cooperative level, 4 = buffer size) as text in
   g_PackageLastErrorPath and reports FATAL_ERROR_DIRECTINPUT_SETUP. */
static Bool8 DirectInputMouse_FailSetup(int32_t initStage,uint32_t *outError)
{
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,initStage,g_PackageLastErrorPath);
  *outError = FATAL_ERROR_DIRECTINPUT_SETUP;
  return false;
}


/* Starts the mouse: binds DirectInputCreateA from the DLL, hides the Windows cursor, creates an exclusive
   foreground buffered DirectInput mouse, hooks display-mode changes, starts the cursor-animation (20 Hz)
   and mouse-poll (64 Hz) timers, loads the cursor images (engine\mouse.gfx) and frame table
   (engine\mouse.dat), and seeds the lock-key bits of g_KeyboardStateMask.
   Returns true on success; on failure false with *outError set to FATAL_ERROR_DIRECTINPUT_SETUP (failed
   stage in g_PackageLastErrorPath) or the cursor asset load error. *outError is only written on failure.
*/
Bool8 DirectInputMouse_Init(uint32_t *outError)

{
  GraphicsSubresourceIndex activeFirstSubresource;
  uint16_t keyState;
  TH_LEGACY_HRESULT directInputResult;
  GraphicsTextureSourceAsset *cursorAsset;
  GraphicsCursorFrameRecord *frameRecord;
  uint32_t remainingFrames;
  uint32_t subresourceIndex;
  uint32_t maxHeight;
  uint32_t maxWidth;
  HINSTANCE directInputModule;
  uint32_t resolveError;
  void *cursorFrameData;
  uint32_t cursorFrameBytes;
  uint32_t cursorLoadErrorCode;
  GraphicsTextureLogicalSize logicalSize;

  directInputModule = DynDLL_Load(dynapi_3);
  if (directInputModule == NULL) {
    FatalError_ExitIfFailed(FATAL_ERROR_DLL_LOAD_FAILED,true); /* does not return */
  }
  resolveError = DynAPI_Resolve(&pDirectInputCreateA,directInputModule,dynapi_19);
  FatalError_ExitIfFailed(resolveError,resolveError != 0);
  SetCursor(NULL);
  directInputResult = pDirectInputCreateA(g_hInstance,DIRECTINPUT_VERSION,&g_DirectInput,NULL);
  if (directInputResult != 0) {
    return DirectInputMouse_FailSetup(0,outError);
  }
  directInputResult = g_DirectInput->lpVtbl->CreateDevice
                    (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,NULL);
  if (directInputResult != 0) {
    return DirectInputMouse_FailSetup(1,outError);
  }
  directInputResult = g_MouseDevice->lpVtbl->SetDataFormat(g_MouseDevice,&MouseDataFormat);
  if (directInputResult != 0) {
    return DirectInputMouse_FailSetup(2,outError);
  }
  directInputResult = g_MouseDevice->lpVtbl->SetCooperativeLevel
                    (g_MouseDevice,g_MainWindow,DebugHook_MouseCooperativeLevel(DISCL_EXCLUSIVE | DISCL_FOREGROUND));
  if (directInputResult != 0) {
    return DirectInputMouse_FailSetup(3,outError);
  }
  directInputResult = g_MouseDevice->lpVtbl->SetProperty
                    (g_MouseDevice,DIPROP_BUFFERSIZE,&MouseBufferProperty.diph);
  if (directInputResult != 0) {
    return DirectInputMouse_FailSetup(4,outError);
  }
  g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
  /* chain in front of the graphics display-mode switch (an atomic exchange in the original) */
  g_DirectInputMouseChainedSetDisplayMode = g_GraphicsSetDisplayMode;
  LOCK();
  g_GraphicsSetDisplayMode = DirectInputMouse_SetDisplayMode;
  UNLOCK();
  TimerSystem_RegisterPeriodic(CURSOR_ANIMATION_TIMER_HZ,GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
  TimerSystem_RegisterPeriodic(MOUSE_POLL_TIMER_HZ,DirectInputMouse_PollBufferedEvents);
  g_PointerFlushEvents = DirectInputMouse_FlushBufferedEvents;
  g_PointerSetPosition = DirectInputMouse_SetPosition;

  /* cursor images: the largest image size sizes the cursor buffers */
  cursorAsset = Package_LoadEntry(u_engine_mouse_gfx_00416864,&cursorLoadErrorCode);
  if (cursorAsset == NULL) {
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
  if (!Resource_Load(u_engine_mouse_dat_00416886,&cursorFrameData,&cursorFrameBytes,&cursorLoadErrorCode)) {
    *outError = cursorLoadErrorCode;
    return false;
  }
  remainingFrames = cursorFrameBytes / sizeof(GraphicsCursorFrameRecord);
  frameRecord = (GraphicsCursorFrameRecord *)cursorFrameData;
  g_CursorFrameRecords = frameRecord;
  g_CursorFrameCount = remainingFrames;
  /* every cursor starts on the first frame of its animations */
  do {
    activeFirstSubresource = frameRecord->activeAnimationFirstSubresourceIndex;
    frameRecord->idleSubresourceIndex = frameRecord->idleAnimationFirstSubresourceIndex;
    frameRecord->activeSubresourceIndex = activeFirstSubresource;
    frameRecord++;
    remainingFrames--;
  } while (remainingFrames != 0);

  keyState = GetKeyState(VK_NUMLOCK);
  if ((keyState & 1) != 0) {
    g_KeyboardStateMask |= KEYBOARD_STATE_NUM_LOCK;
  }
  keyState = GetKeyState(VK_SCROLL);
  if ((keyState & 1) != 0) {
    g_KeyboardStateMask |= KEYBOARD_STATE_SCROLL_LOCK;
  }
  keyState = GetKeyState(VK_CAPITAL);
  if ((keyState & 1) != 0) {
    g_KeyboardStateMask |= KEYBOARD_STATE_CAPS_LOCK;
  }
  return true;
}


/* Periodic DirectInput watchdog that keeps the mouse usable after it was lost (e.g. on a task switch).
   The original recreated the device while no button was held; this version reacquires the existing
   device and only recreates one when there is none (see the deviations below). g_MousePollBusy keeps
   DirectInputMouse_PollBufferedEvents off the device meanwhile.
*/
void DirectInputMouse_RefreshDeviceIfIdle(void)

{
  TH_LEGACY_HRESULT createInputResult;
  TH_LEGACY_HRESULT createDeviceResult;
  TH_LEGACY_HRESULT deviceConfigResult;
  
  /* The original only increments the busy counter here, so a poll already running on the timer
     thread could still be inside GetDeviceData on the device released below. Skip the refresh
     while a poll is active instead. */
  if (_InterlockedCompareExchange((volatile long *)&g_MousePollBusy,1,0) != 0) {
    return;
  }
  /* Deviation from the original: it released and recreated the mouse device (and a new
     DirectInput object) every 48 frames. On current Windows DirectInput services an exclusive
     mouse from a low-level hook thread, which crashed (DINPUT.DLL+0x8ADE, null device) when the
     device vanished under it. Reacquiring the existing device covers the lost-device case the
     refresh was for; the device is only (re)created when there is none. */
  if (g_MouseDevice != NULL) {
    g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
    g_MousePollBusy = 0;
    return;
  }
  if ((g_MouseButtonMask & LEFT_MIDDLE_RIGHT) == CURSOR_BUTTON_NONE) {
    /* The original created a new DirectInput object every refresh without releasing the old one,
       accumulating thousands per session in dinput's hook thread. */
    if (g_DirectInput != NULL) {
      g_DirectInput->lpVtbl->Release(g_DirectInput);
      g_DirectInput = NULL;
    }
    createInputResult = pDirectInputCreateA(g_hInstance,DIRECTINPUT_VERSION,&g_DirectInput,NULL);
    if (createInputResult == DI_OK) {
      createDeviceResult =
           g_DirectInput->lpVtbl->CreateDevice(g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,NULL);
      if (createDeviceResult == DI_OK) {
        deviceConfigResult = g_MouseDevice->lpVtbl->SetDataFormat(g_MouseDevice,&MouseDataFormat);
        if (deviceConfigResult == DI_OK) {
          deviceConfigResult = g_MouseDevice->lpVtbl->SetCooperativeLevel
                              (g_MouseDevice,g_MainWindow,
                               DebugHook_MouseCooperativeLevel(DISCL_EXCLUSIVE | DISCL_FOREGROUND));
          if (deviceConfigResult == DI_OK) {
            deviceConfigResult = g_MouseDevice->lpVtbl->SetProperty
                              (g_MouseDevice,DIPROP_BUFFERSIZE,&MouseBufferProperty.diph);
            if (deviceConfigResult == DI_OK) {
              g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
            }
          }
        }
      }
    }
  }
  g_MousePollBusy = 0;
  return;
}


/* Shuts the mouse down: releases the DirectInput device and object, stops the mouse-poll and
   cursor-animation timers and gives Windows back its arrow cursor.
*/
void DirectInputMouse_Shutdown(void)

{
  HCURSOR arrowCursor;

  if (g_MouseDevice != NULL) {
    g_MouseDevice->lpVtbl->Release(g_MouseDevice);
    g_MouseDevice = NULL;
  }
  if (g_DirectInput != NULL) {
    g_DirectInput->lpVtbl->Release(g_DirectInput);
    g_DirectInput = NULL;
  }
  TimerSystem_UnregisterPeriodic(DirectInputMouse_PollBufferedEvents);
  TimerSystem_UnregisterPeriodic(GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
  arrowCursor = LoadCursorA(NULL,IDC_ARROW);
  SetCursor(arrowCursor);
  return;
}


/* DirectInputMouse_PollBufferedEvents step: applies the buffered event in g_MouseDeviceEvent to the
   mouse position, wheel delta or button mask and returns its cursor event type in *outEventType. Axes
   carry a relative delta; buttons are down while bit 7 of dwData is set. Returns false (state untouched)
   for other axes and buttons, which are ignored. */
static Bool8 DirectInputMouse_ApplyDeviceEvent(GraphicsCursorEventType *outEventType)
{
  Bool8 buttonDown;

  buttonDown = (g_MouseDeviceEvent.dwData & DIRECTINPUT_BUTTON_DOWN_BIT) != 0;
  if (g_MouseDeviceEvent.dwOfs == DIMOFS_X) {
    g_MouseX = g_MouseX + g_MouseDeviceEvent.dwData;
    *outEventType = MOTION_OR_WHEEL;
  }
  else if (g_MouseDeviceEvent.dwOfs == DIMOFS_Y) {
    g_MouseY = g_MouseY + g_MouseDeviceEvent.dwData;
    *outEventType = MOTION_OR_WHEEL;
  }
  else if (g_MouseDeviceEvent.dwOfs == DIMOFS_Z) {
    g_MouseWheelDelta = (int)g_MouseDeviceEvent.dwData / WHEEL_DELTA;
    *outEventType = MOTION_OR_WHEEL;
  }
  else if (g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON0) {
    if (buttonDown) {
      g_MouseButtonMask = g_MouseButtonMask | LEFT;
      *outEventType = LEFT_PRESS;
    }
    else {
      g_MouseButtonMask = g_MouseButtonMask & ~LEFT;
      *outEventType = LEFT_RELEASE;
    }
  }
  else if ((g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON2) || (g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON3)) {
    if (buttonDown) {
      g_MouseButtonMask = g_MouseButtonMask | MIDDLE;
      *outEventType = MIDDLE_PRESS;
    }
    else {
      g_MouseButtonMask = g_MouseButtonMask & ~MIDDLE;
      *outEventType = MIDDLE_RELEASE;
    }
  }
  else if (g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON1) {
    if (buttonDown) {
      g_MouseButtonMask = g_MouseButtonMask | RIGHT;
      *outEventType = RIGHT_PRESS;
    }
    else {
      g_MouseButtonMask = g_MouseButtonMask & ~RIGHT;
      *outEventType = RIGHT_RELEASE;
    }
  }
  else {
    return false;
  }
  return true;
}


/* DirectInputMouse_PollBufferedEvents step: appends one eventType entry to the g_CursorInputEvents ring,
   clamping the mouse position to the framebuffer on the way (the distance beyond each edge, plus one,
   goes to g_CursorOverflow*). The entry records the clamped position, wheel delta, clock and buttons. */
static void DirectInputMouse_AppendCursorEvent(GraphicsCursorEventType eventType)
{
  uint32_t eventIndex;
  uint32_t nextWriteIndex;
  GraphicsCursorInputEvent18 *eventRecord;
  GraphicsCursorButtonState buttonState;
  UiPointerWheelDelta wheelDelta;
  uint32_t clockValue;
  uint32_t limitedXPlusOne;
  uint32_t limitedYPlusOne;
  int pointerX;
  int pointerY;
  int clampedY;

  eventIndex = g_CursorInputWriteIndex;
  nextWriteIndex = eventIndex + 1;
  if (CURSOR_INPUT_EVENT_RING_SIZE - 1 < nextWriteIndex) {
    nextWriteIndex = 0;
  }
  eventRecord = g_CursorInputEvents + eventIndex;
  g_CursorInputWriteIndex = nextWriteIndex;
  eventRecord->eventType = eventType;
  buttonState = g_MouseButtonMask;
  wheelDelta = g_MouseWheelDelta;
  clockValue = g_CursorInputClockValue;
  g_CursorOverflowLeft = 0;
  g_CursorOverflowRight = 0;
  g_CursorOverflowTop = 0;
  g_CursorOverflowBottom = 0;
  /* right and bottom edges: compared one past the position, as in the original */
  limitedXPlusOne = g_MouseX + 1;
  limitedYPlusOne = g_MouseY + 1;
  if ((int)g_FramebufferWidth <= (int)limitedXPlusOne) {
    g_CursorOverflowRight = (limitedXPlusOne - g_FramebufferWidth) + 1;
    limitedXPlusOne = g_FramebufferWidth;
  }
  if ((int)g_FramebufferHeight <= (int)limitedYPlusOne) {
    g_CursorOverflowBottom = (limitedYPlusOne - g_FramebufferHeight) + 1;
    limitedYPlusOne = g_FramebufferHeight;
  }
  pointerX = limitedXPlusOne - 1;
  pointerY = limitedYPlusOne - 1;
  /* left and top edges */
  g_MouseX = pointerX;
  if (pointerX < 1) {
    g_MouseX = 0;
    g_CursorOverflowLeft = 1 - pointerX;
  }
  clampedY = pointerY;
  if (pointerY < 1) {
    clampedY = 0;
    g_CursorOverflowTop = 1 - pointerY;
  }
  g_MouseY = clampedY;
  eventRecord->pointerX = g_MouseX;
  eventRecord->pointerY = clampedY;
  eventRecord->wheelDelta = wheelDelta;
  eventRecord->clockValue = clockValue;
  eventRecord->buttonState = buttonState;
}


/* Mouse-poll timer callback (64 Hz): drains the buffered DirectInput mouse events, updates the mouse
   position (clamped to the framebuffer, the excess kept in g_CursorOverflow*), button mask and wheel
   delta, and appends one entry per motion, wheel or button event to the 256-entry g_CursorInputEvents
   ring. A lost device is reacquired; after 16 errors the poll gives up until the next tick. Skipped when
   a poll or DirectInputMouse_RefreshDeviceIfIdle is already running (g_MousePollBusy).
*/
void DirectInputMouse_PollBufferedEvents(void)

{
  TH_LEGACY_HRESULT directInputResult;
  GraphicsCursorEventType eventType;
  uint32_t errorAttempts;
  int processedCount;

  processedCount = 0;
  /* developer tools (not in the original): while an input script (OPEN_THANDOR_SCRIPT) drives the game, the
     real mouse is ignored so it cannot move the cursor between a scripted move and click */
  if (DebugHook_IgnoreRealMouse()) {
    return;
  }
  /* atomic exchange: the timer callback and the main thread (UiPointer_DispatchPendingEvents) both come here */
  if (THANDOR_ATOMIC_EXCHANGE(&g_MousePollBusy,1) != 0) {
    return;
  }
  /* read buffered events until the buffer is empty or 16 errors occurred */
  errorAttempts = 0;
  while (errorAttempts < MOUSE_POLL_MAX_ERRORS) {
    g_MouseDeviceDataCount = 1;
    directInputResult = g_MouseDevice->lpVtbl->GetDeviceData
                      (g_MouseDevice,sizeof(DIDEVICEOBJECTDATA_DX3),&g_MouseDeviceEvent,&g_MouseDeviceDataCount,0);
    if (directInputResult == DI_OK) {
      if (g_MouseDeviceDataCount != 1) {
        break; /* buffer empty */
      }
      processedCount++;
      g_MouseWheelDelta = 0;
      if (DirectInputMouse_ApplyDeviceEvent(&eventType)) {
        DirectInputMouse_AppendCursorEvent(eventType);
      }
      continue;
    }
    /* a lost device is reacquired and read again */
    if ((directInputResult == DIERR_INPUTLOST) && (g_MouseDevice->lpVtbl->Acquire(g_MouseDevice) == DI_OK)) {
      continue;
    }
    errorAttempts++;
  }
  g_MouseEventsProcessed = g_MouseEventsProcessed + processedCount;
  g_MousePollBusy = 0;
  return;
}


/* Mouse hook in front of g_GraphicsSetDisplayMode (installed by DirectInputMouse_Init): frees the three
   cursor buffers, switches the mode through the chained setter, recreates the buffers in the new pixel
   format, converts the cursor palette, centres the mouse and reacquires the device. Returns true on
   success; false with the error in *errorCode when the mode switch fails or a buffer creation fails (checked after each of the three
   g_SoftwareFramebufferCreate calls; the failing create stores its allocator error there). On failure g_GraphicsBackendAccessState stays -1
   and the buffers created so far stay installed, as in the original.
*/
Bool8 DirectInputMouse_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth,uint32_t *errorCode)

{
  SoftwareFramebufferAccess *primaryFramebuffer;
  SoftwareFramebufferAccess *newCursorFramebuffer;
  SoftwareFramebufferAccess *newCompositeFramebuffer;

  g_GraphicsBackendAccessState = -1; /* blocks backend access (timer cursor drawing) during the switch */
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
  if (!g_DirectInputMouseChainedSetDisplayMode
                    (adapterIndex,bitsPerPixel,framebufferHeight,framebufferWidth,errorCode)) {
    return false; /* the chained hook's error is passed through */
  }
  primaryFramebuffer = g_FramebufferAccess;
  /* a failing create stores its allocator error in *errorCode */
  newCursorFramebuffer = g_SoftwareFramebufferCreate
                   (g_FramebufferAccess->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCursorFramebuffer == NULL) {
    return false;
  }
  g_CursorSavedBackground = newCursorFramebuffer;
  newCompositeFramebuffer = g_SoftwareFramebufferCreate
                   (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCompositeFramebuffer == NULL) {
    return false;
  }
  g_CursorCompositeBuffer = newCompositeFramebuffer;
  newCursorFramebuffer = g_SoftwareFramebufferCreate
                   (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth,errorCode);
  if (newCursorFramebuffer == NULL) {
    return false;
  }
  g_CursorAlternateSavedBackground = newCursorFramebuffer;
  g_GraphicsTextureSourceConvertPaletteEntries
            ((GraphicsPaletteTextureSourceAsset *)g_CursorSourceAsset);
  g_CursorOverrideX = framebufferWidth >> 1;
  g_CursorOverrideY = framebufferHeight >> 1;
  g_MouseX = g_CursorOverrideX;
  g_MouseY = g_CursorOverrideY;
  g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
  g_GraphicsBackendAccessState = 0;
  return true;
}


/* g_PointerSetPosition implementation: moves the mouse to (positionX, positionY) in both the published
   cursor state and the DirectInput position, and clears the wheel delta.
*/
void DirectInputMouse_SetPosition(Win32CursorCoordinate32 positionY,Win32CursorCoordinate32 positionX)

{
  g_CursorOverrideX = positionX;
  g_CursorOverrideY = positionY;
  g_CursorWheelDelta = 0;
  g_MouseX = positionX;
  g_MouseY = positionY;
  g_MouseWheelDelta = 0;
  return;
}


/* g_PointerFlushEvents implementation: publishes the current mouse position and wheel delta as the
   cursor state and discards the queued cursor events. Note that it moves the write index back to the
   read index (the keyboard flush moves the read index instead).
*/
void DirectInputMouse_FlushBufferedEvents(void)

{
  g_CursorOverrideX = g_MouseX;
  g_CursorOverrideY = g_MouseY;
  g_CursorWheelDelta = g_MouseWheelDelta;
  g_CursorInputWriteIndex = g_CursorInputReadIndex;
  return;
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
  return;
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
  return;
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
  return;
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
    .compareCaseInsensitiveFlags = (void *)Keyboard_CompareAsciiCaseInsensitiveFlags,
    .toUpper = (void *)Keyboard_ToUpperAscii,
    .toLower = (void *)Keyboard_ToLowerAscii};
