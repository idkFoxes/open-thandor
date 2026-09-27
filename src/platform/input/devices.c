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
KeyboardEventEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx Keyboard_ReadNextEventRegs(void)

{
  uint32_t nextReadIndex;
  KeyboardEventEaxEdxCf9 readEvent;
  KeyboardEventEaxEdxCf9 emptyResult;
  KeyboardInputEvent *eventRecord;
  
  nextReadIndex = g_KeyboardReadIndex + 1;
  if (g_KeyboardReadIndex != g_KeyboardWriteIndex) {
    if (0x3f < nextReadIndex) {
      nextReadIndex = 0;
    }
    eventRecord = g_KeyboardEvents + g_KeyboardReadIndex;
    g_KeyboardReadIndex = nextReadIndex;
    /* EAX = key code, EDX = state mask; the decompiled version filled an unused local instead. */
    readEvent.carry = false;
    readEvent.eventCode = eventRecord->keyCode00;
    readEvent.eventData = eventRecord->stateMask04;
    return readEvent;
  }
  emptyResult.eventData = nextReadIndex;
  emptyResult.eventCode = 0; /* EAX unchanged in the original; all callers read it only with CF clear */
  emptyResult.carry = true;
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
   Ownership: platform/input/devices.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime],
   TimerSystem_RegisterPeriodic [platform/system/time_locale], Package_LoadEntry [assets/package/runtime],
   Resource_Load [assets/resource/runtime].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx DirectInputMouse_Init(void)

{
  GraphicsSubresourceIndex frameTimestampValue;
  uint16_t keyState;
  TH_LEGACY_HRESULT directInputResult;
  GraphicsTextureSourceAsset *cursorDataOrError;
  uint32_t remainingFrames;
  uint32_t subresourceIndex;
  uint32_t maxHeight;
  uint32_t maxWidth;
  DynDllLoadEaxCf5 dllLoadResult;
  FatalErrorEaxCf5 fatalCheckResult;
  DynApiResolveEaxCf5 procResolveResult;
  PackageLoadEntryEaxCf5 packageLoadResult;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  ResourceLoadEaxEcxCf9 resourceLoadResult;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  int32_t initStage;
  
  initStage = 0;
  dllLoadResult = DynDLL_Load(dynapi_3);
  fatalCheckResult = (*g_FatalErrorPrimaryDispatchCf)((uint32_t)dllLoadResult.moduleOrError,dllLoadResult.carry);
  procResolveResult = DynAPI_Resolve(&pDirectInputCreateA,(HINSTANCE)fatalCheckResult.eax,dynapi_19);
  (*g_FatalErrorPrimaryDispatchCf)((uint32_t)procResolveResult.procedureOrError,procResolveResult.carry);
  SetCursor((HCURSOR)0x0);
  directInputResult = (*pDirectInputCreateA)(g_hInstance,0x300,&g_DirectInput,(TH_LEGACY_LPVOID)0x0);
  if (directInputResult == 0) {
    initStage = 1;
    directInputResult = (*g_DirectInput->lpVtbl->CreateDevice)
                      (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,(TH_LEGACY_LPVOID)0x0);
    if (directInputResult == 0) {
      initStage = 2;
      directInputResult = (*g_MouseDevice->lpVtbl->SetDataFormat)(g_MouseDevice,&MouseDataFormat);
      if (directInputResult == 0) {
        initStage = 3;
        directInputResult = (*g_MouseDevice->lpVtbl->SetCooperativeLevel)(g_MouseDevice,g_MainWindow,5);
        if (directInputResult == 0) {
          initStage = 4;
          directInputResult = (*g_MouseDevice->lpVtbl->SetProperty)
                            (g_MouseDevice,(TH_LEGACY_GUID *)0x1,&MouseBufferProperty.diph);
          if (directInputResult == 0) {
            (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
            g_DirectInputMousePreviousDisplayModeHookCf = g_GraphicsDisplayModeHook;
            LOCK();
            g_GraphicsDisplayModeHook = DirectInputMouse_DisplayModeHookCf;
            UNLOCK();
            TimerSystem_RegisterPeriodic(0x14,GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer)
            ;
            TimerSystem_RegisterPeriodic(0x40,DirectInputMouse_PollBufferedEvents);
            g_PointerFlushEvents = DirectInputMouse_FlushBufferedEvents;
            g_PointerSetPosition = DirectInputMouse_SetPosition;
            packageLoadResult = Package_LoadEntry((uint16_t *)u_engine_mouse_gfx_00416864);
            cursorDataOrError = packageLoadResult.bufferOrError;
            if (!packageLoadResult.carry) {
              maxWidth = 0;
              maxHeight = 0;
              subresourceIndex = 0;
              g_CursorSourceAsset = cursorDataOrError;
              do {
                logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(subresourceIndex,cursorDataOrError);
                subresourceIndex = subresourceIndex + 1;
                if ((int)maxWidth < (int)logicalSize.logicalWidthPixels) {
                  maxWidth = logicalSize.logicalWidthPixels;
                }
                if ((int)maxHeight < (int)logicalSize.logicalHeightPixels) {
                  maxHeight = logicalSize.logicalHeightPixels;
                }
              } while (subresourceIndex < (cursorDataOrError->tableDescriptor).subresourceCount);
              g_CursorMaxWidth = maxWidth;
              g_CursorMaxHeight = maxHeight;
              resourceLoadResult = Resource_Load((uint16_t *)u_engine_mouse_dat_00416886);
              cursorDataOrError = (GraphicsTextureSourceAsset *)resourceLoadResult.eax;
              if (!resourceLoadResult.carry) {
                remainingFrames = resourceLoadResult.ecx >> 5;
                g_CursorFrameRecords = (GraphicsCursorFrameRecord *)cursorDataOrError;
                g_CursorFrameCount = remainingFrames;
                do {
                  frameTimestampValue = (cursorDataOrError->common).buildMetadata.timestamps.dateValue0;
                  (cursorDataOrError->common).buildMetadata.timestamps.dateValue1 = (cursorDataOrError->common).formatVersion;
                  (cursorDataOrError->common).buildMetadata.timestamps.timeValue1 = frameTimestampValue;
                  cursorDataOrError = (GraphicsTextureSourceAsset *)
                         &(cursorDataOrError->common).buildMetadata.timestamps.dateValue2;
                  remainingFrames = remainingFrames - 1;
                } while (remainingFrames != 0);
                keyState = GetKeyState(0x90);
                if ((keyState & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x10000;
                }
                keyState = GetKeyState(0x91);
                if ((keyState & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x20000;
                }
                /* EAX on success is the Caps Lock GetKeyState result */
                successResult.valueOrError = (uint32_t)(int)GetKeyState(0x14);
                if ((successResult.valueOrError & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x40000;
                }
                successResult.carry = false;
                return successResult;
              }
            }
            /* cursor asset load failed: its error code */
            failureResult.carry = true;
            failureResult.valueOrError = (uint32_t)cursorDataOrError;
            return failureResult;
          }
        }
      }
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,initStage,g_PackageLastErrorPath);
  cursorDataOrError = (GraphicsTextureSourceAsset *)0x25;
  failureResult.carry = true;
  failureResult.valueOrError = (uint32_t)cursorDataOrError;
  return failureResult;
}


/* Address: 0x00576F20.
   Ownership: platform/input/devices.
   Purpose: Periodic DirectInput watchdog. While no mouse button is held, releases the current mouse device,
   recreates it, reapplies data format, cooperative level, and buffer property, then reacquires it. g_MousePollBusy
   brackets the refresh.
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_RefreshDeviceIfIdle(void)

{
  TH_LEGACY_HRESULT mouseDeviceSetupResult;
  TH_LEGACY_HRESULT mouseDeviceOperationResult;
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
  if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
    (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
    g_MousePollBusy = 0;
    return;
  }
  if ((g_MouseButtonMask & LEFT_MIDDLE_RIGHT) == CURSOR_BUTTON_NONE) {
    /* The original created a new DirectInput object every refresh without releasing the old one,
       accumulating thousands per session in dinput's hook thread. */
    if (g_DirectInput != (IDirectInputA *)0x0) {
      (*g_DirectInput->lpVtbl->Release)(g_DirectInput);
      g_DirectInput = (IDirectInputA *)0x0;
    }
    mouseDeviceSetupResult =
         (*pDirectInputCreateA)(g_hInstance,0x300,&g_DirectInput,(TH_LEGACY_LPVOID)0x0);
    if (mouseDeviceSetupResult == 0) {
      mouseDeviceOperationResult =
           (*g_DirectInput->lpVtbl->CreateDevice)
                     (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,(TH_LEGACY_LPVOID)0x0);
      if (mouseDeviceOperationResult == 0) {
        deviceConfigResult = (*g_MouseDevice->lpVtbl->SetDataFormat)(g_MouseDevice,&MouseDataFormat);
        if (deviceConfigResult == 0) {
          deviceConfigResult = (*g_MouseDevice->lpVtbl->SetCooperativeLevel)(g_MouseDevice,g_MainWindow,5);
          if (deviceConfigResult == 0) {
            deviceConfigResult = (*g_MouseDevice->lpVtbl->SetProperty)
                              (g_MouseDevice,(TH_LEGACY_GUID *)0x1,&MouseBufferProperty.diph);
            if (deviceConfigResult == 0) {
              (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
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
   Ownership: platform/input/devices.
   Purpose: Handles direct input mouse shutdown.
   Cross-module calls: TimerSystem_UnregisterPeriodic [platform/system/time_locale].
*/
void __thandor_void_preserve_eax_ecx_edx DirectInputMouse_Shutdown(void)

{
  HCURSOR hCursor;
  
  if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
    (*g_MouseDevice->lpVtbl->Release)(g_MouseDevice);
    g_MouseDevice = (IDirectInputDeviceA *)0x0;
  }
  if (g_DirectInput != (IDirectInputA *)0x0) {
    (*g_DirectInput->lpVtbl->Release)(g_DirectInput);
    g_DirectInput = (IDirectInputA *)0x0;
  }
  TimerSystem_UnregisterPeriodic(DirectInputMouse_PollBufferedEvents);
  TimerSystem_UnregisterPeriodic(GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer);
  hCursor = LoadCursorA((HINSTANCE)0x0,&k_LowAddressLiteral00007F00);
  SetCursor(hCursor);
  return;
}


/* Address: 0x00577080.
   Ownership: platform/input/devices.
   Purpose: The routine retries acquisition after DIERR_INPUTLOST and limits repeated polling errors to 16
   attempts.
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
      directInputResult = (*g_MouseDevice->lpVtbl->GetDeviceData)
                        (g_MouseDevice,0x10,&g_MouseDeviceEvent,&g_MouseDeviceDataCount,0);
      wasBusyOrEventIndex = g_CursorInputWriteIndex;
      if (directInputResult == 0) {
        if (g_MouseDeviceDataCount != 1) break;
        processedCount = processedCount + 1;
        g_MouseWheelDelta = 0;
        if (g_MouseDeviceEvent.dwOfs == 0) {
          eventType = MOTION_OR_WHEEL;
          g_MouseX = g_MouseX + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == 4) {
          eventType = MOTION_OR_WHEEL;
          g_MouseY = g_MouseY + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == 8) {
          g_MouseWheelDelta = (int)g_MouseDeviceEvent.dwData / 0x78;
          eventType = MOTION_OR_WHEEL;
        }
        else if (g_MouseDeviceEvent.dwOfs == 0xc) {
          eventType = LEFT_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            eventType = LEFT_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~LEFT;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | LEFT;
          }
        }
        else if ((g_MouseDeviceEvent.dwOfs == 0xe) || (g_MouseDeviceEvent.dwOfs == 0xf)) {
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
          if (g_MouseDeviceEvent.dwOfs != 0xd) continue; /* other axes/buttons: ignored */
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
        if (0xff < nextWriteIndex) {
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
      if (directInputResult == -0x7ff8ffe2) {
        /* DIERR_INPUTLOST: reacquire and read again */
        directInputResult = (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
        if (directInputResult == 0) continue;
      }
      errorAttempts = errorAttempts + 1;
      if (0xf < errorAttempts) break;
    }
    g_MouseEventsProcessed = g_MouseEventsProcessed + processedCount;
    g_MousePollBusy = 0;
  }
  return;
}


/* Address: 0x005772F0.
   Ownership: platform/input/devices.
   Purpose: Display-mode hook that releases three cursor surfaces, invokes the previous graphics hook, recreates
   the cursor surfaces, updates half-width/half-height cursor coordinates, and reacquires the DirectInput device.
   CF reports failure. Typed parameters: p2 framebufferHeight→GraphicsPixelDimension_V302. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
DisplayModeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
DirectInputMouse_DisplayModeHookCf
          (DisplayModeHookArgument0 hookArg0,DisplayModeHookArgument1 hookArg1,
          GraphicsPixelDimension framebufferHeight,GraphicsPixelDimension framebufferWidth)

{
  SoftwareFramebufferAccess *primaryFramebuffer;
  SoftwareFramebufferAccess *newCursorFramebuffer;
  SoftwareFramebufferAccess *newCompositeFramebuffer;
  DisplayModeEaxCf5 previousHookResult;
  DisplayModeEaxCf5 successResult;
  bool previousHookFailed;
  
  g_GraphicsBackendAccessState = -1;
  (*g_MemoryApi.free)(g_CursorSavedBackground);
  (*g_MemoryApi.free)(g_CursorCompositeBuffer);
  (*g_MemoryApi.free)(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = (SoftwareFramebufferAccess *)0x0;
  g_CursorCompositeBuffer = (SoftwareFramebufferAccess *)0x0;
  g_CursorAlternateSavedBackground = (SoftwareFramebufferAccess *)0x0;
  previousHookResult = (*g_DirectInputMousePreviousDisplayModeHookCf)
                    (hookArg0,hookArg1,framebufferHeight,framebufferWidth);
  primaryFramebuffer = g_FramebufferAccess;
  previousHookFailed = previousHookResult.carry;
  newCursorFramebuffer = (SoftwareFramebufferAccess *)previousHookResult.eax;
  if (!previousHookFailed) {
    newCursorFramebuffer =
         (*g_SoftwareFramebufferCreate)
                   (g_FramebufferAccess->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
    if (!previousHookFailed) {
      g_CursorSavedBackground = newCursorFramebuffer;
      newCompositeFramebuffer =
           (*g_SoftwareFramebufferCreate)(primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
      newCursorFramebuffer = newCompositeFramebuffer;
      if (!previousHookFailed) {
        g_CursorCompositeBuffer = newCompositeFramebuffer;
        newCursorFramebuffer =
             (*g_SoftwareFramebufferCreate)
                       (primaryFramebuffer->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
        if (!previousHookFailed) {
          g_CursorAlternateSavedBackground = newCursorFramebuffer;
          (*g_GraphicsTextureSourceConvertPaletteEntries)
                    ((GraphicsPaletteTextureSourceAsset *)g_CursorSourceAsset);
          g_CursorOverrideX = framebufferWidth >> 1;
          g_CursorOverrideY = framebufferHeight >> 1;
          g_MouseX = g_CursorOverrideX;
          g_MouseY = g_CursorOverrideY;
          successResult.eax = (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
          g_GraphicsBackendAccessState = 0;
          successResult.carry = false;
          return successResult;
        }
      }
    }
  }
  previousHookResult.carry = true;
  previousHookResult.eax = (uint32_t)newCursorFramebuffer;
  return previousHookResult;
}


/* Address: 0x00577420.
   Ownership: platform/input/devices.
   Purpose: Stores the requested pointer position into both published and DirectInput mouse coordinates and clears
   the current wheel delta. The backend stack order is Y then X. Typed parameters: p0
   positionY→Win32CursorCoordinate32_V303, p1 positionX→Win32CursorCoordinate32_V303. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
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
   Ownership: platform/input/devices.
   Purpose: Copies the current DirectInput coordinates and wheel delta into the published cursor state, then drops
   pending cursor events by copying the write index to the read index.
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

