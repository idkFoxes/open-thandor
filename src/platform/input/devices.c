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
   Ownership: platform/input/devices.
   Purpose: Handles WM_KEYDOWN and WM_SYSKEYDOWN virtual keys. Left/right Shift, Ctrl, and Alt update low state
   bits without queueing. Num Lock, Scroll Lock, and Caps Lock toggle high state bits once per physical press
   through g_KeyboardToggleLatchMask. Mapped non-modifier keys append an encoded KeyboardInputEvent. CF clear means
   an event was queued; CF set means no event was queued.
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
  mappedCodeOrMask = 1;
  if (((((virtualKey == 0xa0) || (mappedCodeOrMask = 3, virtualKey == 0x10)) || (mappedCodeOrMask = 2, virtualKey == 0xa1)
       ) || (((mappedCodeOrMask = 0x10, virtualKey == 0xa4 || (mappedCodeOrMask = 0x30, virtualKey == 0x12)) ||
             ((mappedCodeOrMask = 0x20, virtualKey == 0xa5 ||
              ((mappedCodeOrMask = 4, virtualKey == 0xa2 || (mappedCodeOrMask = 0xc, virtualKey == 0x11)))))))) ||
     (mappedCodeOrMask = 8, virtualKey == 0xa3)) {
    g_KeyboardStateMask = g_KeyboardStateMask | mappedCodeOrMask;
  }
  else {
    mappedCodeOrMask = 0x40000;
    if (((virtualKey != 0x14) && (mappedCodeOrMask = 0x10000, virtualKey != 0x90)) &&
       (mappedCodeOrMask = 0x20000, virtualKey != 0x91)) {
      mappedCodeOrMask = 0x20;
      if ((((virtualKey != 0x20) && (mappedCodeOrMask = 0x10000, virtualKey != 0x1b)) &&
          ((((mappedCodeOrMask = 0x10001, virtualKey != 0xd &&
             ((virtualKey != 0x6c && (mappedCodeOrMask = 0x10002, virtualKey != 9)))) &&
            (mappedCodeOrMask = 0x10003, virtualKey != 8)) &&
           (((((mappedCodeOrMask = 0x10004, virtualKey != 0x2a && (virtualKey != 0x2c)) &&
              (mappedCodeOrMask = 0x10005, virtualKey != 0x2b)) &&
             ((virtualKey != 0x13 && (mappedCodeOrMask = 0x10006, virtualKey != 0x2e)))) &&
            ((mappedCodeOrMask = 0x10007, virtualKey != 0x2d &&
             ((mappedCodeOrMask = 0x20001, virtualKey != 0x70 && (mappedCodeOrMask = 0x20002, virtualKey != 0x71))))))))))
         && (((((mappedCodeOrMask = 0x20003, virtualKey != 0x72 &&
                ((((mappedCodeOrMask = 0x20004, virtualKey != 0x73 && (mappedCodeOrMask = 0x20005, virtualKey != 0x74)) &&
                  (mappedCodeOrMask = 0x20006, virtualKey != 0x75)) &&
                 (((mappedCodeOrMask = 0x20007, virtualKey != 0x76 && (mappedCodeOrMask = 0x20008, virtualKey != 0x77)) &&
                  ((mappedCodeOrMask = 0x20009, virtualKey != 0x78 &&
                   ((mappedCodeOrMask = 0x2000a, virtualKey != 0x79 && (mappedCodeOrMask = 0x2000b, virtualKey != 0x7a))))
                  )))))) && (mappedCodeOrMask = 0x2000c, virtualKey != 0x7b)) &&
              ((((mappedCodeOrMask = 0x10010, virtualKey != 0x24 && (mappedCodeOrMask = 0x10018, virtualKey != 0x23)) &&
                (mappedCodeOrMask = 0x10012, virtualKey != 0x21)) &&
               ((mappedCodeOrMask = 0x1001a, virtualKey != 0x22 && (mappedCodeOrMask = 0x10014, virtualKey != 0x25))))))
             && (((mappedCodeOrMask = 0x10016, virtualKey != 0x27 &&
                  ((mappedCodeOrMask = 0x10011, virtualKey != 0x26 && (mappedCodeOrMask = 0x10019, virtualKey != 0x28))))
                 && (mappedCodeOrMask = 0x10015, virtualKey != 0x29)))))) {
        mappedCodeOrMask = virtualKey + 0x30000;
        if (virtualKey < 0x30) {
          return;
        }
        if (0x39 < virtualKey) {
          if (virtualKey < 0x41) {
            return;
          }
          mappedCodeOrMask = virtualKey + 0x30020;
          if ((((0x5a < virtualKey) && (mappedCodeOrMask = 0x2a, virtualKey != 0x6a)) &&
              (mappedCodeOrMask = 0x2f, virtualKey != 0x6f)) &&
             ((mappedCodeOrMask = 0x2b, virtualKey != 0x6b && (mappedCodeOrMask = 0x2d, virtualKey != 0x6d)))) {
            if (virtualKey < 0x60) {
              return;
            }
            if (0x6e < virtualKey) {
              return;
            }
            mappedCodeOrMask = 0x10007;
            if ((((virtualKey != 0x60) && (mappedCodeOrMask = 0x10018, virtualKey != 0x61)) &&
                ((mappedCodeOrMask = 0x10019, virtualKey != 0x62 &&
                 (((mappedCodeOrMask = 0x1001a, virtualKey != 99 && (mappedCodeOrMask = 0x10014, virtualKey != 100)) &&
                  (mappedCodeOrMask = 0x10015, virtualKey != 0x65)))))) &&
               (((mappedCodeOrMask = 0x10016, virtualKey != 0x66 && (mappedCodeOrMask = 0x10010, virtualKey != 0x67)) &&
                ((mappedCodeOrMask = 0x10011, virtualKey != 0x68 && (mappedCodeOrMask = 0x10012, virtualKey != 0x69))))))
            {
              mappedCodeOrMask = 0x10006;
            }
          }
        }
      }
      eventRecord = g_KeyboardEvents + g_KeyboardWriteIndex;
      nextWriteIndex = g_KeyboardWriteIndex + 1;
      g_KeyboardWriteIndex = g_KeyboardWriteIndex + 1;
      eventRecord->keyCode00 = mappedCodeOrMask;
      g_KeyboardEvents[writeIndex].stateMask04 = queuedStateMask;
      if ((mappedCodeOrMask & 0xffff0000) == 0x10000) {
        g_KeyboardSpecialKeyDown[mappedCodeOrMask & 0xffff] = 1;
      }
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
   Ownership: platform/input/devices.
   Purpose: Handles WM_KEYUP and WM_SYSKEYUP. Modifier bits and toggle latches are cleared directly. Engine
   special-key codes in family 0x0001 clear g_KeyboardSpecialKeyDown[lowWord]. No keyboard event is appended. CF
   clear means a recognized non-modifier key was processed; CF set covers modifier/toggle or unmapped keys.
*/
void __thandor_void_preserve_eax_ecx_edx Keyboard_OnKeyUp(KeyboardVirtualKeyCode virtualKey)

{
  uint32_t mappedKeyStateCode;
  uint32_t mappedCodeOrToggleMask;
  
  mappedKeyStateCode = 0x40001;
  if ((((((virtualKey == 0xa0) || (mappedKeyStateCode = 0x40003, virtualKey == 0x10)) ||
        (mappedKeyStateCode = 0x40002, virtualKey == 0xa1)) ||
       ((mappedKeyStateCode = 0x10, virtualKey == 0xa4 ||
        (mappedKeyStateCode = 0x30, virtualKey == 0x12)))) ||
      ((mappedKeyStateCode = 0x20, virtualKey == 0xa5 ||
       ((mappedKeyStateCode = 4, virtualKey == 0xa2 ||
        (mappedKeyStateCode = 0xc, virtualKey == 0x11)))))) ||
     (mappedKeyStateCode = 8, virtualKey == 0xa3)) {
    g_KeyboardStateMask = g_KeyboardStateMask & ~mappedKeyStateCode;
  }
  else {
    mappedCodeOrToggleMask = 0x10000;
    if ((virtualKey != 0x90) && (mappedCodeOrToggleMask = 0x20000, virtualKey != 0x91)) {
      mappedCodeOrToggleMask = 0x20000;
      if (((((virtualKey != 0x20) &&
            (((virtualKey != 0x1b && (virtualKey != 0xd)) && (virtualKey != 0x6c)))) &&
           (((virtualKey != 9 && (virtualKey != 8)) && (virtualKey != 0x2a)))) &&
          ((virtualKey != 0x2c && (virtualKey != 0x2b)))) &&
         (((virtualKey != 0x13 &&
           (((mappedCodeOrToggleMask = 0x10006, virtualKey != 0x2e && (mappedCodeOrToggleMask = 0x10007, virtualKey != 0x2d)) &&
            (mappedCodeOrToggleMask = 0x10010, virtualKey != 0x24)))) &&
          (((mappedCodeOrToggleMask = 0x10018, virtualKey != 0x23 && (mappedCodeOrToggleMask = 0x10012, virtualKey != 0x21)) &&
           ((mappedCodeOrToggleMask = 0x1001a, virtualKey != 0x22 &&
            (((mappedCodeOrToggleMask = 0x10014, virtualKey != 0x25 && (mappedCodeOrToggleMask = 0x10016, virtualKey != 0x27)) &&
             ((mappedCodeOrToggleMask = 0x10011, virtualKey != 0x26 &&
              ((mappedCodeOrToggleMask = 0x10019, virtualKey != 0x28 && (mappedCodeOrToggleMask = 0x10015, virtualKey != 0x29)))))))))
           ))))) {
        mappedCodeOrToggleMask = 0x20000;
        if (virtualKey < 0x30) {
          return;
        }
        if (0x39 < virtualKey) {
          if (virtualKey < 0x41) {
            return;
          }
          if (0x5a < virtualKey) {
            if (virtualKey < 0x60) {
              return;
            }
            if (0x7b < virtualKey) {
              return;
            }
            mappedCodeOrToggleMask = 0x10007;
            if ((((((virtualKey != 0x60) && (mappedCodeOrToggleMask = 0x10018, virtualKey != 0x61)) &&
                  (mappedCodeOrToggleMask = 0x10019, virtualKey != 0x62)) &&
                 ((mappedCodeOrToggleMask = 0x1001a, virtualKey != 99 && (mappedCodeOrToggleMask = 0x10014, virtualKey != 100)))) &&
                (((mappedCodeOrToggleMask = 0x10015, virtualKey != 0x65 &&
                  ((mappedCodeOrToggleMask = 0x10016, virtualKey != 0x66 && (mappedCodeOrToggleMask = 0x10010, virtualKey != 0x67))))
                 && (mappedCodeOrToggleMask = 0x10011, virtualKey != 0x68)))) &&
               ((mappedCodeOrToggleMask = 0x10012, virtualKey != 0x69 && (mappedCodeOrToggleMask = 0x10006, virtualKey != 0x6e)))) {
              mappedCodeOrToggleMask = 0x20000;
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

