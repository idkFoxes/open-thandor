/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/input/devices.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/input/devices.h>
#include <thandor/thandor.h>
#include <intrin.h>

/* Implementation ownership: platform/input/devices. */

/* Address: 0x00417280.
   Ownership: platform/input/devices.
   Purpose: Masks both arguments to 16 bits, converts ASCII a-z to A-Z, and leaves flags from comparing
   uppercase(secondValue) against uppercase(firstValue). The function preserves incoming EAX and EDX; callers
   consume condition flags rather than a scalar return. Keyboard case-insensitive comparison callback; EFLAGS carry
   the comparison and EAX/EDX are preserved. Typed parameters: p0 leftCodeUnit→KeyboardCharacterCode_V308, p1
   rightCodeUnit→KeyboardCharacterCode_V308. Nearby but non-identical semantic domains were explicitly deferred.
   Local calls: Keyboard_ToUpperAscii.
*/
bool __thandor_cf_preserve_eax_ecx_edx
Keyboard_CompareAsciiCaseInsensitiveFlags
          (KeyboardCharacterCode leftCodeUnit,KeyboardCharacterCode rightCodeUnit)

{
  uint32_t asciiCodeUnit;
  uint32_t upperRight;
  uint32_t upperLeft;
  
  asciiCodeUnit = leftCodeUnit & 0xffff;
  upperRight = Keyboard_ToUpperAscii(rightCodeUnit & 0xffff);
  upperLeft = Keyboard_ToUpperAscii(asciiCodeUnit);
  return upperRight < upperLeft;
}


/* Address: 0x00417230.
   Ownership: platform/input/devices.
   Purpose: Discards all queued keyboard events by copying g_KeyboardReadIndex into g_KeyboardWriteIndex.
*/
void __thandor_void_preserve_eax_ecx_edx Keyboard_FlushEvents(void)

{
  g_KeyboardWriteIndex = g_KeyboardReadIndex;
  return;
}


/* Address: 0x00417240.
   Ownership: platform/input/devices.
   Purpose: Returns one KeyboardInputEvent through EDX:EAX: EAX=keyCode and EDX=stateMask. CF clear means an event
   was returned; CF set means the ring was empty. The qword return type models the preserved register pair, not a
   source-level 64-bit API.
*/
KeyboardEventResult __thandor_eax_edx_cf_preserve_ecx Keyboard_ReadNextEventRegs(void)

{
  uint32_t nextReadIndex;
  KeyboardEventResult readEvent;
  KeyboardEventResult emptyResult;
  KeyboardInputEvent *eventRecord;
  
  nextReadIndex = g_KeyboardReadIndex + 1;
  if (g_KeyboardReadIndex != g_KeyboardWriteIndex) {
    if (0x3f < nextReadIndex) {
      nextReadIndex = 0;
    }
    eventRecord = g_KeyboardEvents + g_KeyboardReadIndex;
    g_KeyboardReadIndex = nextReadIndex;
    /* EAX = key code, EDX = state mask; the decompiled version filled an unused local instead. */
    readEvent.queueEmpty = false;
    readEvent.eventCode = eventRecord->keyCode00;
    readEvent.eventData = eventRecord->stateMask04;
    return readEvent;
  }
  emptyResult.eventData = nextReadIndex;
  emptyResult.eventCode = 0; /* EAX unchanged in the original; all callers read it only with CF clear */
  emptyResult.queueEmpty = true;
  return emptyResult;
}


/* Address: 0x004172D0.
   Ownership: platform/input/devices.
   Purpose: Converts ASCII A-Z to a-z and leaves all other values unchanged. Keyboard ASCII case-transform
   callback. Typed parameters: p0 asciiCodeUnit→KeyboardCharacterCode_V308. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
uint32_t __thandor_eax_preserve_ecx_edx Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit)

{
  if ((0x40 < asciiCodeUnit) && (asciiCodeUnit < 0x5b)) {
    asciiCodeUnit = asciiCodeUnit + 0x20;
  }
  return asciiCodeUnit;
}


/* Address: 0x00576CF0.
   Starts the mouse: binds DirectInputCreateA from the DLL, hides the Windows cursor, creates an exclusive
   foreground buffered DirectInput mouse, hooks display-mode changes, starts the cursor-animation (20 Hz)
   and mouse-poll (64 Hz) timers, loads the cursor images (engine\mouse.gfx) and frame table
   (engine\mouse.dat), and seeds the lock-key bits of g_KeyboardStateMask.
   CF clear on success; on failure EAX is FATAL_ERROR_DIRECTINPUT_SETUP (failed stage in
   g_PackageLastErrorPath) or the cursor asset load error.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx DirectInputMouse_Init(void)

{
  GraphicsSubresourceIndex copiedFrameField;
  uint16_t keyState;
  TH_LEGACY_HRESULT directInputResult;
  GraphicsTextureSourceAsset *cursorDataOrError;
  uint32_t remainingFrames;
  uint32_t subresourceIndex;
  uint32_t maxHeight;
  uint32_t maxWidth;
  DllLoadResult dllLoadResult;
  FatalErrorCheckResult fatalCheckResult;
  DynApiResolveResult procResolveResult;
  PackageLoadResult packageLoadResult;
  StatusResult successResult;
  StatusResult failureResult;
  ResourceLoadResult resourceLoadResult;
  TextureSizeResult logicalSize;
  int32_t initStage;
  
  initStage = 0;
  dllLoadResult = DynDLL_Load(dynapi_3);
  fatalCheckResult = FatalError_ExitIfFailed((uint32_t)dllLoadResult.moduleOrError,dllLoadResult.failed);
  procResolveResult = DynAPI_Resolve(&pDirectInputCreateA,(HINSTANCE)fatalCheckResult.valueOrError,dynapi_19);
  FatalError_ExitIfFailed((uint32_t)procResolveResult.procedureOrError,procResolveResult.failed);
  SetCursor(NULL);
  directInputResult = pDirectInputCreateA(g_hInstance,DIRECTINPUT_VERSION,&g_DirectInput,NULL);
  if (directInputResult == 0) {
    initStage = 1;
    directInputResult = g_DirectInput->lpVtbl->CreateDevice
                      (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,NULL);
    if (directInputResult == 0) {
      initStage = 2;
      directInputResult = g_MouseDevice->lpVtbl->SetDataFormat(g_MouseDevice,&MouseDataFormat);
      if (directInputResult == 0) {
        initStage = 3;
        directInputResult = g_MouseDevice->lpVtbl->SetCooperativeLevel
                          (g_MouseDevice,g_MainWindow,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
        if (directInputResult == 0) {
          initStage = 4;
          directInputResult = g_MouseDevice->lpVtbl->SetProperty
                            (g_MouseDevice,DIPROP_BUFFERSIZE,&MouseBufferProperty.diph);
          if (directInputResult == 0) {
            g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
            /* chain in front of the graphics display-mode switch (XCHG in the original) */
            g_DirectInputMouseChainedSetDisplayMode = g_GraphicsSetDisplayMode;
            LOCK();
            g_GraphicsSetDisplayMode = DirectInputMouse_SetDisplayMode;
            UNLOCK();
            TimerSystem_RegisterPeriodic(20,GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
            TimerSystem_RegisterPeriodic(64,DirectInputMouse_PollBufferedEvents);
            g_PointerFlushEvents = DirectInputMouse_FlushBufferedEvents;
            g_PointerSetPosition = DirectInputMouse_SetPosition;
            packageLoadResult = Package_LoadEntry(u_engine_mouse_gfx_00416864);
            cursorDataOrError = packageLoadResult.bufferOrError;
            if (!packageLoadResult.failed) {
              maxWidth = 0;
              maxHeight = 0;
              subresourceIndex = 0;
              g_CursorSourceAsset = cursorDataOrError;
              do {
                logicalSize = g_GraphicsTextureSourceGetLogicalSize(subresourceIndex,cursorDataOrError);
                subresourceIndex++;
                if ((int)maxWidth < (int)logicalSize.logicalWidthPixels) {
                  maxWidth = logicalSize.logicalWidthPixels;
                }
                if ((int)maxHeight < (int)logicalSize.logicalHeightPixels) {
                  maxHeight = logicalSize.logicalHeightPixels;
                }
              } while (subresourceIndex < (cursorDataOrError->tableDescriptor).subresourceCount);
              g_CursorMaxWidth = maxWidth;
              g_CursorMaxHeight = maxHeight;
              resourceLoadResult = Resource_Load(u_engine_mouse_dat_00416886);
              cursorDataOrError = (GraphicsTextureSourceAsset *)resourceLoadResult.bufferOrError;
              if (!resourceLoadResult.failed) {
                remainingFrames = resourceLoadResult.byteCount >> 5; /* 32-byte GraphicsCursorFrameRecord */
                g_CursorFrameRecords = (GraphicsCursorFrameRecord *)cursorDataOrError;
                g_CursorFrameCount = remainingFrames;
                /* Ghidra typed the frame cursor as GraphicsTextureSourceAsset; the accesses are the
                   frame-record offsets: idleSubresourceIndex (+0x18) = idleAnimationFirstSubresourceIndex
                   (+0x08) and activeSubresourceIndex (+0x1C) = activeAnimationFirstSubresourceIndex (+0x10),
                   so every cursor starts on the first frame of its animations. */
                do {
                  copiedFrameField = (cursorDataOrError->common).buildMetadata.timestamps.dateValue0;
                  (cursorDataOrError->common).buildMetadata.timestamps.dateValue1 = (cursorDataOrError->common).formatVersion;
                  (cursorDataOrError->common).buildMetadata.timestamps.timeValue1 = copiedFrameField;
                  cursorDataOrError = (GraphicsTextureSourceAsset *)
                         &(cursorDataOrError->common).buildMetadata.timestamps.dateValue2;
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
                /* EAX on success is the Caps Lock GetKeyState result */
                successResult.valueOrError = (uint32_t)(int)GetKeyState(VK_CAPITAL);
                if ((successResult.valueOrError & 1) != 0) {
                  g_KeyboardStateMask |= KEYBOARD_STATE_CAPS_LOCK;
                }
                successResult.failed = false;
                return successResult;
              }
            }
            /* cursor asset load failed: its error code */
            failureResult.failed = true;
            failureResult.valueOrError = (uint32_t)cursorDataOrError;
            return failureResult;
          }
        }
      }
    }
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,initStage,g_PackageLastErrorPath);
  cursorDataOrError = (GraphicsTextureSourceAsset *)FATAL_ERROR_DIRECTINPUT_SETUP;
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)cursorDataOrError;
  return failureResult;
}


/* Address: 0x00576F20.
   Periodic DirectInput watchdog that keeps the mouse usable after it was lost (e.g. on a task switch).
   The original recreated the device while no button was held; this version reacquires the existing
   device and only recreates one when there is none (see the deviations below). g_MousePollBusy keeps
   DirectInputMouse_PollBufferedEvents off the device meanwhile.
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_RefreshDeviceIfIdle(void)

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
                              (g_MouseDevice,g_MainWindow,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
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


/* Address: 0x00577000.
   Shuts the mouse down: releases the DirectInput device and object, stops the mouse-poll and
   cursor-animation timers and gives Windows back its arrow cursor.
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_Shutdown(void)

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


/* Address: 0x00577080.
   Mouse-poll timer callback (64 Hz): drains the buffered DirectInput mouse events, updates the mouse
   position (clamped to the framebuffer, the excess kept in g_CursorOverflow*), button mask and wheel
   delta, and appends one entry per motion, wheel or button event to the 256-entry g_CursorInputEvents
   ring. A lost device is reacquired; after 16 errors the poll gives up until the next tick. Skipped when
   a poll or DirectInputMouse_RefreshDeviceIfIdle is already running (g_MousePollBusy).
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_PollBufferedEvents(void)

{
  uint32_t wasBusyOrEventIndex;
  uint32_t clockValue;
  UiPointerWheelDelta wheelDelta;
  GraphicsCursorButtonState buttonState;
  TH_LEGACY_HRESULT directInputResult;
  GraphicsCursorEventType eventType;
  uint32_t clampedXPlusOne;
  int clampedXOrY;
  uint32_t nextWriteIndex;
  uint32_t clampedYPlusOne;
  int unclampedY;
  GraphicsCursorInputEvent18 *eventRecord;
  uint32_t errorAttempts;
  int processedCount;
  
  wasBusyOrEventIndex = g_MousePollBusy;
  processedCount = 0;
  LOCK();
  g_MousePollBusy = 1;
  UNLOCK();
  errorAttempts = 0;
  if (wasBusyOrEventIndex == 0) {
    /* read buffered events until the buffer is empty or 16 errors occurred */
    for (;;) {
      g_MouseDeviceDataCount = 1;
      directInputResult = g_MouseDevice->lpVtbl->GetDeviceData
                        (g_MouseDevice,sizeof(DIDEVICEOBJECTDATA_DX3),&g_MouseDeviceEvent,&g_MouseDeviceDataCount,0);
      wasBusyOrEventIndex = g_CursorInputWriteIndex;
      if (directInputResult == DI_OK) {
        if (g_MouseDeviceDataCount != 1) break;
        processedCount++;
        g_MouseWheelDelta = 0;
        /* axes carry a relative delta; buttons are down while bit 7 of dwData is set */
        if (g_MouseDeviceEvent.dwOfs == DIMOFS_X) {
          eventType = MOTION_OR_WHEEL;
          g_MouseX = g_MouseX + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == DIMOFS_Y) {
          eventType = MOTION_OR_WHEEL;
          g_MouseY = g_MouseY + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == DIMOFS_Z) {
          g_MouseWheelDelta = (int)g_MouseDeviceEvent.dwData / WHEEL_DELTA;
          eventType = MOTION_OR_WHEEL;
        }
        else if (g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON0) {
          eventType = LEFT_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            eventType = LEFT_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~LEFT;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | LEFT;
          }
        }
        else if ((g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON2) || (g_MouseDeviceEvent.dwOfs == DIMOFS_BUTTON3)) {
          eventType = MIDDLE_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            eventType = MIDDLE_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~MIDDLE;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | MIDDLE;
          }
        }
        else {
          if (g_MouseDeviceEvent.dwOfs != DIMOFS_BUTTON1) continue; /* other axes/buttons: ignored */
          eventType = RIGHT_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            eventType = RIGHT_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~RIGHT;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | RIGHT;
          }
        }
        nextWriteIndex = g_CursorInputWriteIndex + 1;
        if (255 < nextWriteIndex) {
          nextWriteIndex = 0;
        }
        eventRecord = g_CursorInputEvents + g_CursorInputWriteIndex;
        g_CursorInputWriteIndex = nextWriteIndex;
        eventRecord->eventType00 = eventType;
        buttonState = g_MouseButtonMask;
        wheelDelta = g_MouseWheelDelta;
        clockValue = g_CursorInputClockValue;
        g_CursorOverflowLeft = 0;
        g_CursorOverflowRight = 0;
        g_CursorOverflowTop = 0;
        g_CursorOverflowBottom = 0;
        clampedXPlusOne = g_MouseX + 1;
        clampedYPlusOne = g_MouseY + 1;
        if ((int)g_FramebufferWidth <= (int)clampedXPlusOne) {
          g_CursorOverflowRight = (clampedXPlusOne - g_FramebufferWidth) + 1;
          clampedXPlusOne = g_FramebufferWidth;
        }
        if ((int)g_FramebufferHeight <= (int)clampedYPlusOne) {
          g_CursorOverflowBottom = (clampedYPlusOne - g_FramebufferHeight) + 1;
          clampedYPlusOne = g_FramebufferHeight;
        }
        clampedXOrY = clampedXPlusOne - 1;
        unclampedY = clampedYPlusOne - 1;
        g_MouseX = clampedXOrY;
        if (clampedXOrY < 1) {
          g_MouseX = 0;
          g_CursorOverflowLeft = 1 - clampedXOrY;
        }
        clampedXOrY = unclampedY;
        if (unclampedY < 1) {
          clampedXOrY = 0;
          g_CursorOverflowTop = 1 - unclampedY;
        }
        g_MouseY = clampedXOrY;
        g_CursorInputEvents[wasBusyOrEventIndex].pointerX08 = g_MouseX;
        g_CursorInputEvents[wasBusyOrEventIndex].pointerY0C = clampedXOrY;
        g_CursorInputEvents[wasBusyOrEventIndex].wheelDelta10 = wheelDelta;
        g_CursorInputEvents[wasBusyOrEventIndex].clockValue14 = clockValue;
        g_CursorInputEvents[wasBusyOrEventIndex].buttonState04 = buttonState;
        continue;
      }
      if (directInputResult == DIERR_INPUTLOST) {
        /* reacquire and read again */
        directInputResult = g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
        if (directInputResult == DI_OK) continue;
      }
      errorAttempts++;
      if (15 < errorAttempts) break;
    }
    g_MouseEventsProcessed = g_MouseEventsProcessed + processedCount;
    g_MousePollBusy = 0;
  }
  return;
}


/* Address: 0x005772F0.
   Mouse hook in front of g_GraphicsSetDisplayMode (installed by DirectInputMouse_Init): frees the three
   cursor buffers, switches the mode through the chained setter, recreates the buffers in the new pixel
   format, converts the cursor palette, centres the mouse and reacquires the device. CF set when the mode
   switch fails. The original also fails on a failed buffer creation (CF of g_SoftwareFramebufferCreate);
   this C version drops that CF, so the later previousHookFailed tests never fire.
*/
DisplayModeResult __thandor_eax_cf_preserve_ecx_edx
DirectInputMouse_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth)

{
  SoftwareFramebufferAccess *primaryFramebuffer;
  SoftwareFramebufferAccess *newCursorFramebuffer;
  SoftwareFramebufferAccess *newCompositeFramebuffer;
  DisplayModeResult previousHookResult;
  DisplayModeResult successResult;
  bool previousHookFailed;

  g_GraphicsBackendAccessState = -1; /* blocks backend access (timer cursor drawing) during the switch */
  g_MemoryApi.free(g_CursorSavedBackground);
  g_MemoryApi.free(g_CursorCompositeBuffer);
  g_MemoryApi.free(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = NULL;
  g_CursorCompositeBuffer = NULL;
  g_CursorAlternateSavedBackground = NULL;
  previousHookResult = g_DirectInputMouseChainedSetDisplayMode
                    (adapterIndex,bitsPerPixel,framebufferHeight,framebufferWidth);
  primaryFramebuffer = g_FramebufferAccess;
  previousHookFailed = previousHookResult.failed;
  newCursorFramebuffer = (SoftwareFramebufferAccess *)previousHookResult.valueOrError;
  if (!previousHookFailed) {
    newCursorFramebuffer =
         g_SoftwareFramebufferCreate
                   (g_FramebufferAccess->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
    if (!previousHookFailed) {
      g_CursorSavedBackground = newCursorFramebuffer;
      newCompositeFramebuffer =
           g_SoftwareFramebufferCreate(primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
      newCursorFramebuffer = newCompositeFramebuffer;
      if (!previousHookFailed) {
        g_CursorCompositeBuffer = newCompositeFramebuffer;
        newCursorFramebuffer =
             g_SoftwareFramebufferCreate
                       (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
        if (!previousHookFailed) {
          g_CursorAlternateSavedBackground = newCursorFramebuffer;
          g_GraphicsTextureSourceConvertPaletteEntries
                    ((GraphicsPaletteTextureSourceAsset *)g_CursorSourceAsset);
          g_CursorOverrideX = framebufferWidth >> 1;
          g_CursorOverrideY = framebufferHeight >> 1;
          g_MouseX = g_CursorOverrideX;
          g_MouseY = g_CursorOverrideY;
          successResult.valueOrError = g_MouseDevice->lpVtbl->Acquire(g_MouseDevice);
          g_GraphicsBackendAccessState = 0;
          successResult.failed = false;
          return successResult;
        }
      }
    }
  }
  previousHookResult.failed = true;
  previousHookResult.valueOrError = (uint32_t)newCursorFramebuffer;
  return previousHookResult;
}


/* Address: 0x00577420.
   g_PointerSetPosition implementation: moves the mouse to (positionX, positionY) in both the published
   cursor state and the DirectInput position, and clears the wheel delta.
*/
void __thandor_void_preserve_eax_ecx
DirectInputMouse_SetPosition(Win32CursorCoordinate32 positionY,Win32CursorCoordinate32 positionX)

{
  g_CursorOverrideX = positionX;
  g_CursorOverrideY = positionY;
  g_CursorWheelDelta = 0;
  g_MouseX = positionX;
  g_MouseY = positionY;
  g_MouseWheelDelta = 0;
  return;
}


/* Address: 0x00577460.
   g_PointerFlushEvents implementation: publishes the current mouse position and wheel delta as the
   cursor state and discards the queued cursor events. Note that it moves the write index back to the
   read index (the keyboard flush moves the read index instead).
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_FlushBufferedEvents(void)

{
  g_CursorOverrideX = g_MouseX;
  g_CursorOverrideY = g_MouseY;
  g_CursorWheelDelta = g_MouseWheelDelta;
  g_CursorInputWriteIndex = g_CursorInputReadIndex;
  return;
}


/* Address: 0x005774A0.
   WM_KEYDOWN/WM_SYSKEYDOWN handler: Shift, Ctrl and Alt set their KEYBOARD_STATE_* bits, the lock keys toggle
   theirs once per press (g_KeyboardToggleLatchMask stops auto-repeat from toggling again), and every other
   mapped key is queued as a KEYBOARD_KEY_CODE_* event with the modifier state of the moment in the 64-entry
   keyboard ring (0x10000-family keys also mark g_KeyboardSpecialKeyDown). CF clear when an event was queued.
*/
void __thandor_void_preserve_eax_ecx_edx Keyboard_OnKeyDown(KeyboardVirtualKeyCode virtualKey)

{
  KeyboardInputEvent *eventRecord;
  KeyboardEventRingIndex writeIndex;
  uint32_t queuedStateMask;
  uint32_t mappedCodeOrMask;
  uint32_t nextWriteIndex;

  queuedStateMask = g_KeyboardStateMask;
  writeIndex = g_KeyboardWriteIndex;
  mappedCodeOrMask = KEYBOARD_STATE_LEFT_SHIFT;
  if (((((virtualKey == VK_LSHIFT) || (mappedCodeOrMask = KEYBOARD_STATE_SHIFT, virtualKey == VK_SHIFT)) || (mappedCodeOrMask = KEYBOARD_STATE_RIGHT_SHIFT, virtualKey == VK_RSHIFT)
       ) || (((mappedCodeOrMask = KEYBOARD_STATE_LEFT_ALT, virtualKey == VK_LMENU || (mappedCodeOrMask = KEYBOARD_STATE_ALT, virtualKey == VK_MENU)) ||
             ((mappedCodeOrMask = KEYBOARD_STATE_RIGHT_ALT, virtualKey == VK_RMENU ||
              ((mappedCodeOrMask = KEYBOARD_STATE_LEFT_CTRL, virtualKey == VK_LCONTROL || (mappedCodeOrMask = KEYBOARD_STATE_CTRL, virtualKey == VK_CONTROL)))))))) ||
     (mappedCodeOrMask = KEYBOARD_STATE_RIGHT_CTRL, virtualKey == VK_RCONTROL)) {
    g_KeyboardStateMask = g_KeyboardStateMask | mappedCodeOrMask;
  }
  else {
    mappedCodeOrMask = KEYBOARD_STATE_CAPS_LOCK;
    if (((virtualKey != VK_CAPITAL) && (mappedCodeOrMask = KEYBOARD_STATE_NUM_LOCK, virtualKey != VK_NUMLOCK)) &&
       (mappedCodeOrMask = KEYBOARD_STATE_SCROLL_LOCK, virtualKey != VK_SCROLL)) {
      mappedCodeOrMask = KEYBOARD_KEY_CODE_SPACE;
      if ((((virtualKey != VK_SPACE) && (mappedCodeOrMask = KEYBOARD_KEY_CODE_ESCAPE, virtualKey != VK_ESCAPE)) &&
          ((((mappedCodeOrMask = KEYBOARD_KEY_CODE_ENTER, virtualKey != VK_RETURN &&
             ((virtualKey != VK_SEPARATOR && (mappedCodeOrMask = KEYBOARD_KEY_CODE_TAB, virtualKey != VK_TAB)))) &&
            (mappedCodeOrMask = KEYBOARD_KEY_CODE_BACKSPACE, virtualKey != VK_BACK)) &&
           (((((mappedCodeOrMask = KEYBOARD_KEY_CODE_PRINT, virtualKey != VK_PRINT && (virtualKey != VK_SNAPSHOT)) &&
              (mappedCodeOrMask = KEYBOARD_KEY_CODE_PAUSE, virtualKey != VK_EXECUTE)) &&
             ((virtualKey != VK_PAUSE && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DELETE), virtualKey != VK_DELETE)))) &&
            ((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_INSERT), virtualKey != VK_INSERT &&
             ((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(1), virtualKey != VK_F1 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(2), virtualKey != VK_F2))))))))))
         && (((((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(3), virtualKey != VK_F3 &&
                ((((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(4), virtualKey != VK_F4 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(5), virtualKey != VK_F5)) &&
                  (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(6), virtualKey != VK_F6)) &&
                 (((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(7), virtualKey != VK_F7 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(8), virtualKey != VK_F8)) &&
                  ((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(9), virtualKey != VK_F9 &&
                   ((mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(10), virtualKey != VK_F10 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(11), virtualKey != VK_F11))))
                  )))))) && (mappedCodeOrMask = KEYBOARD_KEY_CODE_FUNCTION(12), virtualKey != VK_F12)) &&
              ((((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME), virtualKey != VK_HOME && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END), virtualKey != VK_END)) &&
                (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP), virtualKey != VK_PRIOR)) &&
               ((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN), virtualKey != VK_NEXT && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT), virtualKey != VK_LEFT))))))
             && (((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT), virtualKey != VK_RIGHT &&
                  ((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP), virtualKey != VK_UP && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN), virtualKey != VK_DOWN))))
                 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_NUMPAD_5, virtualKey != VK_SELECT)))))) {
        /* digits and letters: KEYBOARD_KEY_CODE_CHAR of the ASCII code, letters in lowercase (+ 0x20) */
        mappedCodeOrMask = virtualKey + 0x30000;
        if (virtualKey < '0') {
          return;
        }
        if ('9' < virtualKey) {
          if (virtualKey < 'A') {
            return;
          }
          mappedCodeOrMask = virtualKey + 0x30020;
          if (((('Z' < virtualKey) && (mappedCodeOrMask = '*', virtualKey != VK_MULTIPLY)) &&
              (mappedCodeOrMask = '/', virtualKey != VK_DIVIDE)) &&
             ((mappedCodeOrMask = '+', virtualKey != VK_ADD && (mappedCodeOrMask = '-', virtualKey != VK_SUBTRACT)))) {
            if (virtualKey < VK_NUMPAD0) {
              return;
            }
            if (VK_DECIMAL < virtualKey) {
              return;
            }
            mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_INSERT);
            if ((((virtualKey != VK_NUMPAD0) && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END), virtualKey != VK_NUMPAD1)) &&
                ((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN), virtualKey != VK_NUMPAD2 &&
                 (((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN), virtualKey != VK_NUMPAD3 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT), virtualKey != VK_NUMPAD4)) &&
                  (mappedCodeOrMask = KEYBOARD_KEY_CODE_NUMPAD_5, virtualKey != VK_NUMPAD5)))))) &&
               (((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT), virtualKey != VK_NUMPAD6 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME), virtualKey != VK_NUMPAD7)) &&
                ((mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP), virtualKey != VK_NUMPAD8 && (mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP), virtualKey != VK_NUMPAD9))))))
            {
              mappedCodeOrMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DELETE);
            }
          }
        }
      }
      eventRecord = g_KeyboardEvents + g_KeyboardWriteIndex;
      nextWriteIndex = g_KeyboardWriteIndex + 1;
      g_KeyboardWriteIndex = g_KeyboardWriteIndex + 1;
      eventRecord->keyCode00 = mappedCodeOrMask;
      g_KeyboardEvents[writeIndex].stateMask04 = queuedStateMask;
      /* KEYBOARD_KEY_CODE_SPECIAL family: remember the key as held */
      if ((mappedCodeOrMask & 0xffff0000) == 0x10000) {
        g_KeyboardSpecialKeyDown[mappedCodeOrMask & 0xffff] = 1;
      }
      /* wrap the 64-entry ring */
      if (0x3f < nextWriteIndex) {
        g_KeyboardWriteIndex = 0;
      }
      return;
    }
    if ((g_KeyboardToggleLatchMask & mappedCodeOrMask) == 0) {
      g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask | mappedCodeOrMask;
      g_KeyboardStateMask = g_KeyboardStateMask ^ mappedCodeOrMask;
    }
  }
  return;
}


/* Address: 0x00577880.
   WM_KEYUP/WM_SYSKEYUP handler: clears the modifier bits of Shift, Ctrl and Alt, re-arms the Num/Scroll Lock
   toggle, and releases the g_KeyboardSpecialKeyDown entry of a 0x10000-family key (the held state the in-game
   camera keys poll). Queues no event. CF clear when a non-modifier key was processed.
*/
void __thandor_void_preserve_eax_ecx_edx Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey)

{
  uint32_t mappedKeyStateCode;
  uint32_t mappedCodeOrToggleMask;

  /* releasing Shift also clears Caps Lock; VK_CAPITAL itself is not handled here, so its toggle latch is
     never re-armed by a key-up */
  mappedKeyStateCode = KEYBOARD_STATE_CAPS_LOCK | KEYBOARD_STATE_LEFT_SHIFT;
  if ((((((virtualKey == VK_LSHIFT) || (mappedKeyStateCode = KEYBOARD_STATE_CAPS_LOCK | KEYBOARD_STATE_SHIFT, virtualKey == VK_SHIFT)) ||
        (mappedKeyStateCode = KEYBOARD_STATE_CAPS_LOCK | KEYBOARD_STATE_RIGHT_SHIFT, virtualKey == VK_RSHIFT)) ||
       ((mappedKeyStateCode = KEYBOARD_STATE_LEFT_ALT, virtualKey == VK_LMENU ||
        (mappedKeyStateCode = KEYBOARD_STATE_ALT, virtualKey == VK_MENU)))) ||
      ((mappedKeyStateCode = KEYBOARD_STATE_RIGHT_ALT, virtualKey == VK_RMENU ||
       ((mappedKeyStateCode = KEYBOARD_STATE_LEFT_CTRL, virtualKey == VK_LCONTROL ||
        (mappedKeyStateCode = KEYBOARD_STATE_CTRL, virtualKey == VK_CONTROL)))))) ||
     (mappedKeyStateCode = KEYBOARD_STATE_RIGHT_CTRL, virtualKey == VK_RCONTROL)) {
    g_KeyboardStateMask = g_KeyboardStateMask & ~mappedKeyStateCode;
  }
  else {
    mappedCodeOrToggleMask = KEYBOARD_STATE_NUM_LOCK;
    if ((virtualKey != VK_NUMLOCK) && (mappedCodeOrToggleMask = KEYBOARD_STATE_SCROLL_LOCK, virtualKey != VK_SCROLL)) {
      mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_NOT_SPECIAL;
      if (((((virtualKey != VK_SPACE) &&
            (((virtualKey != VK_ESCAPE && (virtualKey != VK_RETURN)) && (virtualKey != VK_SEPARATOR)))) &&
           (((virtualKey != VK_TAB && (virtualKey != VK_BACK)) && (virtualKey != VK_PRINT)))) &&
          ((virtualKey != VK_SNAPSHOT && (virtualKey != VK_EXECUTE)))) &&
         (((virtualKey != VK_PAUSE &&
           (((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DELETE), virtualKey != VK_DELETE && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_INSERT), virtualKey != VK_INSERT)) &&
            (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME), virtualKey != VK_HOME)))) &&
          (((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END), virtualKey != VK_END && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP), virtualKey != VK_PRIOR)) &&
           ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN), virtualKey != VK_NEXT &&
            (((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT), virtualKey != VK_LEFT && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT), virtualKey != VK_RIGHT)) &&
             ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP), virtualKey != VK_UP &&
              ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN), virtualKey != VK_DOWN && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_NUMPAD_5, virtualKey != VK_SELECT)))))))))
           ))))) {
        mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_NOT_SPECIAL;
        if (virtualKey < '0') {
          return;
        }
        if ('9' < virtualKey) {
          if (virtualKey < 'A') {
            return;
          }
          if ('Z' < virtualKey) {
            if (virtualKey < VK_NUMPAD0) {
              return;
            }
            if (VK_F12 < virtualKey) {
              return;
            }
            mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_INSERT);
            if ((((((virtualKey != VK_NUMPAD0) && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_END), virtualKey != VK_NUMPAD1)) &&
                  (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DOWN), virtualKey != VK_NUMPAD2)) &&
                 ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_DOWN), virtualKey != VK_NUMPAD3 && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_LEFT), virtualKey != VK_NUMPAD4)))) &&
                (((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_NUMPAD_5, virtualKey != VK_NUMPAD5 &&
                  ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_RIGHT), virtualKey != VK_NUMPAD6 && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_HOME), virtualKey != VK_NUMPAD7))))
                 && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_UP), virtualKey != VK_NUMPAD8)))) &&
               ((mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_PAGE_UP), virtualKey != VK_NUMPAD9 && (mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_SPECIAL(KEYBOARD_SPECIAL_KEY_DELETE), virtualKey != VK_DECIMAL)))) {
              mappedCodeOrToggleMask = KEYBOARD_KEY_CODE_NOT_SPECIAL;
            }
          }
        }
      }
      if ((mappedCodeOrToggleMask & 0xffff0000) == 0x10000) {
        g_KeyboardSpecialKeyDown[mappedCodeOrToggleMask & 0xffff] = 0;
      }
      return;
    }
    g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask & ~mappedCodeOrToggleMask;
  }
  return;
}


/* Address: 0x00577B30.
   Ownership: platform/input/devices.
   Purpose: Handles WM_CHAR and WM_SYSCHAR by appending the low 16-bit character plus the current keyboard state.
   When either Ctrl bit is active, control characters 1-26 are normalized to lowercase ASCII a-z by adding 0x60.
*/
void __thandor_void_preserve_eax_ecx Keyboard_OnChar(KeyboardCharacterCode character)

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
  if (((g_KeyboardStateMask & 0xc) != 0) && (keyCode < 0x1b)) {
    keyCode = keyCode + 0x60;
  }
  g_KeyboardEvents[writeIndex].stateMask04 = g_KeyboardStateMask;
  eventRecord->keyCode00 = keyCode;
  if (0x3f < nextWriteIndex) {
    g_KeyboardWriteIndex = 0;
  }
  return;
}


/* Address: 0x004172B0.
   Ownership: platform/input/devices.
   Purpose: Converts ASCII a-z to A-Z and leaves all other values unchanged. Keyboard ASCII case-transform
   callback. Typed parameters: p0 asciiCodeUnit→KeyboardCharacterCode_V308. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
uint32_t __thandor_eax_preserve_ecx_edx Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit)

{
  if ((0x60 < asciiCodeUnit) && (asciiCodeUnit < 0x7b)) {
    asciiCodeUnit = asciiCodeUnit - 0x20;
  }
  return asciiCodeUnit;
}

