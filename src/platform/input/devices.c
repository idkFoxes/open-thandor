/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/input/devices.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/platform/input/devices.h>

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
  uint asciiCodeUnit;
  dword dVar1;
  dword dVar2;
  
  asciiCodeUnit = leftCodeUnit & 0xffff;
  dVar1 = Keyboard_ToUpperAscii(rightCodeUnit & 0xffff);
  dVar2 = Keyboard_ToUpperAscii(asciiCodeUnit);
  return dVar1 < dVar2;
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
  undefined4 in_EAX;
  uint nextReadIndex;
  qword eventRegisterPair;
  KeyboardEventEaxEdxCf9 KVar1;
  KeyboardEventEaxEdxCf9 KVar2;
  KeyboardInputEvent *eventRecord;
  
  nextReadIndex = g_KeyboardReadIndex + 1;
  if (g_KeyboardReadIndex != g_KeyboardWriteIndex) {
    if (0x3f < nextReadIndex) {
      nextReadIndex = 0;
    }
    eventRecord = g_KeyboardEvents + g_KeyboardReadIndex;
    g_KeyboardReadIndex = nextReadIndex;
    KVar1.carry = false;
    eventRegisterPair._0_4_ = eventRecord->keyCode00;
    eventRegisterPair._4_4_ = eventRecord->stateMask04;
    return KVar1;
  }
  KVar2.eventData = nextReadIndex;
  KVar2.eventCode = in_EAX;
  KVar2.carry = true;
  return KVar2;
}


/* Address: 0x004172D0.
   Ownership: platform/input/devices.
   Purpose: Converts ASCII A-Z to a-z and leaves all other values unchanged. Keyboard ASCII case-transform
   callback. Typed parameters: p0 asciiCodeUnit→KeyboardCharacterCode_V308. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
dword __thandor_eax_preserve_ecx_edx Keyboard_ToLowerAscii(KeyboardCharacterCode asciiCodeUnit)

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
  GraphicsSubresourceIndex GVar1;
  ushort uVar2;
  TH_LEGACY_HRESULT TVar3;
  undefined2 extraout_var;
  GraphicsTextureSourceAsset *arg1;
  uint uVar4;
  dword arg0;
  dword dVar5;
  dword dVar6;
  DynDllLoadEaxCf5 DVar7;
  FatalErrorEaxCf5 FVar8;
  DynApiResolveEaxCf5 DVar9;
  PackageLoadEntryEaxCf5 PVar10;
  StatusValueEaxCf5 SVar11;
  StatusValueEaxCf5 SVar12;
  ResourceLoadEaxEcxCf9 RVar13;
  GraphicsTextureSizeEaxEdxCf9 GVar14;
  sdword local_1c;
  
  local_1c = 0;
  DVar7 = DynDLL_Load(dynapi_3);
  FVar8 = (*g_FatalErrorPrimaryDispatchCf)((dword)DVar7.moduleOrError,DVar7.carry);
  DVar9 = DynAPI_Resolve(&pDirectInputCreateA,(HINSTANCE)FVar8.eax,dynapi_19);
  (*g_FatalErrorPrimaryDispatchCf)((dword)DVar9.procedureOrError,DVar9.carry);
  SetCursor((HCURSOR)0x0);
  TVar3 = (*pDirectInputCreateA)(g_hInstance,0x300,&g_DirectInput,(TH_LEGACY_LPVOID)0x0);
  if (TVar3 == 0) {
    local_1c = 1;
    TVar3 = (*g_DirectInput->lpVtbl->CreateDevice)
                      (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,(TH_LEGACY_LPVOID)0x0);
    if (TVar3 == 0) {
      local_1c = 2;
      TVar3 = (*g_MouseDevice->lpVtbl->SetDataFormat)(g_MouseDevice,&MouseDataFormat);
      if (TVar3 == 0) {
        local_1c = 3;
        TVar3 = (*g_MouseDevice->lpVtbl->SetCooperativeLevel)(g_MouseDevice,g_MainWindow,5);
        if (TVar3 == 0) {
          local_1c = 4;
          TVar3 = (*g_MouseDevice->lpVtbl->SetProperty)
                            (g_MouseDevice,(TH_LEGACY_GUID *)0x1,&MouseBufferProperty.diph);
          if (TVar3 == 0) {
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
            PVar10 = Package_LoadEntry((word *)u_engine_mouse_gfx_00416864);
            arg1 = PVar10.bufferOrError;
            if (!PVar10.carry) {
              dVar6 = 0;
              dVar5 = 0;
              arg0 = 0;
              g_CursorSourceAsset = arg1;
              do {
                GVar14 = (*g_GraphicsTextureSourceGetLogicalSize)(arg0,arg1);
                arg0 = arg0 + 1;
                if ((int)dVar6 < (int)GVar14.logicalWidthPixels) {
                  dVar6 = GVar14.logicalWidthPixels;
                }
                if ((int)dVar5 < (int)GVar14.logicalHeightPixels) {
                  dVar5 = GVar14.logicalHeightPixels;
                }
              } while (arg0 < (arg1->tableDescriptor).subresourceCount);
              g_CursorMaxWidth = dVar6;
              g_CursorMaxHeight = dVar5;
              RVar13 = Resource_Load((word *)u_engine_mouse_dat_00416886);
              arg1 = (GraphicsTextureSourceAsset *)RVar13.eax;
              if (!RVar13.carry) {
                uVar4 = RVar13.ecx >> 5;
                g_CursorFrameRecords = (GraphicsCursorFrameRecord *)arg1;
                g_CursorFrameCount = uVar4;
                do {
                  GVar1 = (arg1->common).buildMetadata.timestamps.dateValue0;
                  (arg1->common).buildMetadata.timestamps.dateValue1 = (arg1->common).formatVersion;
                  (arg1->common).buildMetadata.timestamps.timeValue1 = GVar1;
                  arg1 = (GraphicsTextureSourceAsset *)
                         &(arg1->common).buildMetadata.timestamps.dateValue2;
                  uVar4 = uVar4 - 1;
                } while (uVar4 != 0);
                uVar2 = GetKeyState(0x90);
                if ((uVar2 & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x10000;
                }
                uVar2 = GetKeyState(0x91);
                if ((uVar2 & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x20000;
                }
                SVar11.valueOrError._0_2_ = GetKeyState(0x14);
                SVar11.valueOrError._2_2_ = extraout_var;
                if (((ushort)SVar11.valueOrError & 1) != 0) {
                  g_KeyboardStateMask = g_KeyboardStateMask | 0x40000;
                }
                SVar11.carry = false;
                return SVar11;
              }
            }
            goto LAB_00576f08;
          }
        }
      }
    }
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,local_1c,g_PackageLastErrorPath);
  arg1 = (GraphicsTextureSourceAsset *)0x25;
LAB_00576f08:
  SVar12.carry = true;
  SVar12.valueOrError = (dword)arg1;
  return SVar12;
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
  TH_LEGACY_HRESULT TVar1;
  
  g_MousePollBusy = g_MousePollBusy + 1;
  if ((g_MouseButtonMask & LEFT_MIDDLE_RIGHT) == NONE) {
    if (g_MouseDevice != (IDirectInputDeviceA *)0x0) {
      (*g_MouseDevice->lpVtbl->Release)(g_MouseDevice);
      g_MouseDevice = (IDirectInputDeviceA *)0x0;
    }
    mouseDeviceSetupResult =
         (*pDirectInputCreateA)(g_hInstance,0x300,&g_DirectInput,(TH_LEGACY_LPVOID)0x0);
    if (mouseDeviceSetupResult == 0) {
      mouseDeviceOperationResult =
           (*g_DirectInput->lpVtbl->CreateDevice)
                     (g_DirectInput,&GUID_SysMouse_Local,&g_MouseDevice,(TH_LEGACY_LPVOID)0x0);
      if (mouseDeviceOperationResult == 0) {
        TVar1 = (*g_MouseDevice->lpVtbl->SetDataFormat)(g_MouseDevice,&MouseDataFormat);
        if (TVar1 == 0) {
          TVar1 = (*g_MouseDevice->lpVtbl->SetCooperativeLevel)(g_MouseDevice,g_MainWindow,5);
          if (TVar1 == 0) {
            TVar1 = (*g_MouseDevice->lpVtbl->SetProperty)
                              (g_MouseDevice,(TH_LEGACY_GUID *)0x1,&MouseBufferProperty.diph);
            if (TVar1 == 0) {
              (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
            }
          }
        }
      }
    }
  }
  g_MousePollBusy = g_MousePollBusy - 1;
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
  dword dVar1;
  dword dVar2;
  UiPointerWheelDelta UVar3;
  GraphicsCursorButtonState GVar4;
  TH_LEGACY_HRESULT TVar5;
  GraphicsCursorEventType GVar6;
  dword dVar7;
  int iVar8;
  uint uVar9;
  dword dVar10;
  int iVar11;
  GraphicsCursorInputEvent18 *pGVar12;
  uint local_24;
  int local_20;
  
  dVar1 = g_MousePollBusy;
  local_20 = 0;
  LOCK();
  g_MousePollBusy = 1;
  UNLOCK();
  local_24 = 0;
  if (dVar1 == 0) {
DirectInputMouse_PollBufferedEvents_ReadNextEventAfterProcessOrAcquireRetry:
    do {
      g_MouseDeviceDataCount = 1;
      TVar5 = (*g_MouseDevice->lpVtbl->GetDeviceData)
                        (g_MouseDevice,0x10,&g_MouseDeviceEvent,&g_MouseDeviceDataCount,0);
      dVar1 = g_CursorInputWriteIndex;
      if (TVar5 == 0) {
        if (g_MouseDeviceDataCount != 1) break;
        local_20 = local_20 + 1;
        g_MouseWheelDelta = 0;
        if (g_MouseDeviceEvent.dwOfs == 0) {
          GVar6 = MOTION_OR_WHEEL;
          g_MouseX = g_MouseX + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == 4) {
          GVar6 = MOTION_OR_WHEEL;
          g_MouseY = g_MouseY + g_MouseDeviceEvent.dwData;
        }
        else if (g_MouseDeviceEvent.dwOfs == 8) {
          g_MouseWheelDelta = (int)g_MouseDeviceEvent.dwData / 0x78;
          GVar6 = MOTION_OR_WHEEL;
        }
        else if (g_MouseDeviceEvent.dwOfs == 0xc) {
          GVar6 = LEFT_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            GVar6 = LEFT_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~LEFT;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | LEFT;
          }
        }
        else if ((g_MouseDeviceEvent.dwOfs == 0xe) || (g_MouseDeviceEvent.dwOfs == 0xf)) {
          GVar6 = MIDDLE_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            GVar6 = MIDDLE_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~MIDDLE;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | MIDDLE;
          }
        }
        else {
          if (g_MouseDeviceEvent.dwOfs != 0xd)
          goto DirectInputMouse_PollBufferedEvents_ReadNextEventAfterProcessOrAcquireRetry;
          GVar6 = RIGHT_PRESS;
          if ((g_MouseDeviceEvent.dwData & 0x80) == 0) {
            GVar6 = RIGHT_RELEASE;
            g_MouseButtonMask = g_MouseButtonMask & ~RIGHT;
          }
          else {
            g_MouseButtonMask = g_MouseButtonMask | RIGHT;
          }
        }
        uVar9 = g_CursorInputWriteIndex + 1;
        if (0xff < uVar9) {
          uVar9 = 0;
        }
        pGVar12 = g_CursorInputEvents + g_CursorInputWriteIndex;
        g_CursorInputWriteIndex = uVar9;
        pGVar12->eventType00 = GVar6;
        GVar4 = g_MouseButtonMask;
        UVar3 = g_MouseWheelDelta;
        dVar2 = g_CursorInputClockValue;
        g_CursorOverflowLeft = 0;
        g_CursorOverflowRight = 0;
        g_CursorOverflowTop = 0;
        g_CursorOverflowBottom = 0;
        dVar7 = g_MouseX + 1;
        dVar10 = g_MouseY + 1;
        if ((int)g_FramebufferWidth <= (int)dVar7) {
          g_CursorOverflowRight = (dVar7 - g_FramebufferWidth) + 1;
          dVar7 = g_FramebufferWidth;
        }
        if ((int)g_FramebufferHeight <= (int)dVar10) {
          g_CursorOverflowBottom = (dVar10 - g_FramebufferHeight) + 1;
          dVar10 = g_FramebufferHeight;
        }
        iVar8 = dVar7 - 1;
        iVar11 = dVar10 - 1;
        g_MouseX = iVar8;
        if (iVar8 < 1) {
          g_MouseX = 0;
          g_CursorOverflowLeft = 1 - iVar8;
        }
        iVar8 = iVar11;
        if (iVar11 < 1) {
          iVar8 = 0;
          g_CursorOverflowTop = 1 - iVar11;
        }
        g_MouseY = iVar8;
        g_CursorInputEvents[dVar1].pointerX08 = g_MouseX;
        g_CursorInputEvents[dVar1].pointerY0C = iVar8;
        g_CursorInputEvents[dVar1].wheelDelta10 = UVar3;
        g_CursorInputEvents[dVar1].clockValue14 = dVar2;
        g_CursorInputEvents[dVar1].buttonState04 = GVar4;
        goto DirectInputMouse_PollBufferedEvents_ReadNextEventAfterProcessOrAcquireRetry;
      }
      if (TVar5 == -0x7ff8ffe2) {
        TVar5 = (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
        if (TVar5 == 0)
        goto DirectInputMouse_PollBufferedEvents_ReadNextEventAfterProcessOrAcquireRetry;
      }
      local_24 = local_24 + 1;
    } while (local_24 < 0x10);
    g_MouseEventsProcessed = g_MouseEventsProcessed + local_20;
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
  SoftwareFramebufferAccess *pSVar1;
  SoftwareFramebufferAccess *newCursorFramebuffer;
  SoftwareFramebufferAccess *newCompositeFramebuffer;
  DisplayModeEaxCf5 DVar3;
  DisplayModeEaxCf5 DVar4;
  bool bVar2;
  
  g_GraphicsBackendAccessState = -1;
  (*g_MemoryApi.free)(g_CursorSavedBackground);
  (*g_MemoryApi.free)(g_CursorCompositeBuffer);
  (*g_MemoryApi.free)(g_CursorAlternateSavedBackground);
  g_CursorSavedBackground = (SoftwareFramebufferAccess *)0x0;
  g_CursorCompositeBuffer = (SoftwareFramebufferAccess *)0x0;
  g_CursorAlternateSavedBackground = (SoftwareFramebufferAccess *)0x0;
  DVar3 = (*g_DirectInputMousePreviousDisplayModeHookCf)
                    (hookArg0,hookArg1,framebufferHeight,framebufferWidth);
  pSVar1 = g_FramebufferAccess;
  bVar2 = DVar3.carry;
  newCursorFramebuffer = (SoftwareFramebufferAccess *)DVar3.eax;
  if (!bVar2) {
    newCursorFramebuffer =
         (*g_SoftwareFramebufferCreate)
                   (g_FramebufferAccess->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
    if (!bVar2) {
      g_CursorSavedBackground = newCursorFramebuffer;
      newCompositeFramebuffer =
           (*g_SoftwareFramebufferCreate)(pSVar1->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
      newCursorFramebuffer = newCompositeFramebuffer;
      if (!bVar2) {
        g_CursorCompositeBuffer = newCompositeFramebuffer;
        newCursorFramebuffer =
             (*g_SoftwareFramebufferCreate)
                       (pSVar1->bytesPerPixel,g_CursorMaxHeight,g_CursorMaxWidth);
        if (!bVar2) {
          g_CursorAlternateSavedBackground = newCursorFramebuffer;
          (*g_GraphicsTextureSourceConvertPaletteEntries)
                    ((GraphicsPaletteTextureSourceAsset *)g_CursorSourceAsset);
          g_CursorOverrideX = framebufferWidth >> 1;
          g_CursorOverrideY = framebufferHeight >> 1;
          g_MouseX = g_CursorOverrideX;
          g_MouseY = g_CursorOverrideY;
          DVar4.eax = (*g_MouseDevice->lpVtbl->Acquire)(g_MouseDevice);
          g_GraphicsBackendAccessState = 0;
          DVar4.carry = false;
          return DVar4;
        }
      }
    }
  }
  DVar3.carry = true;
  DVar3.eax = (dword)newCursorFramebuffer;
  return DVar3;
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
  KeyboardInputEvent *pKVar1;
  KeyboardEventRingIndex KVar2;
  dword dVar3;
  uint uVar4;
  uint uVar5;
  
  dVar3 = g_KeyboardStateMask;
  KVar2 = g_KeyboardWriteIndex;
  uVar4 = 1;
  if (((((virtualKey == 0xa0) || (uVar4 = 3, virtualKey == 0x10)) || (uVar4 = 2, virtualKey == 0xa1)
       ) || (((uVar4 = 0x10, virtualKey == 0xa4 || (uVar4 = 0x30, virtualKey == 0x12)) ||
             ((uVar4 = 0x20, virtualKey == 0xa5 ||
              ((uVar4 = 4, virtualKey == 0xa2 || (uVar4 = 0xc, virtualKey == 0x11)))))))) ||
     (uVar4 = 8, virtualKey == 0xa3)) {
    g_KeyboardStateMask = g_KeyboardStateMask | uVar4;
  }
  else {
    uVar4 = 0x40000;
    if (((virtualKey != 0x14) && (uVar4 = 0x10000, virtualKey != 0x90)) &&
       (uVar4 = 0x20000, virtualKey != 0x91)) {
      uVar4 = 0x20;
      if ((((virtualKey != 0x20) && (uVar4 = 0x10000, virtualKey != 0x1b)) &&
          ((((uVar4 = 0x10001, virtualKey != 0xd &&
             ((virtualKey != 0x6c && (uVar4 = 0x10002, virtualKey != 9)))) &&
            (uVar4 = 0x10003, virtualKey != 8)) &&
           (((((uVar4 = 0x10004, virtualKey != 0x2a && (virtualKey != 0x2c)) &&
              (uVar4 = 0x10005, virtualKey != 0x2b)) &&
             ((virtualKey != 0x13 && (uVar4 = 0x10006, virtualKey != 0x2e)))) &&
            ((uVar4 = 0x10007, virtualKey != 0x2d &&
             ((uVar4 = 0x20001, virtualKey != 0x70 && (uVar4 = 0x20002, virtualKey != 0x71))))))))))
         && (((((uVar4 = 0x20003, virtualKey != 0x72 &&
                ((((uVar4 = 0x20004, virtualKey != 0x73 && (uVar4 = 0x20005, virtualKey != 0x74)) &&
                  (uVar4 = 0x20006, virtualKey != 0x75)) &&
                 (((uVar4 = 0x20007, virtualKey != 0x76 && (uVar4 = 0x20008, virtualKey != 0x77)) &&
                  ((uVar4 = 0x20009, virtualKey != 0x78 &&
                   ((uVar4 = 0x2000a, virtualKey != 0x79 && (uVar4 = 0x2000b, virtualKey != 0x7a))))
                  )))))) && (uVar4 = 0x2000c, virtualKey != 0x7b)) &&
              ((((uVar4 = 0x10010, virtualKey != 0x24 && (uVar4 = 0x10018, virtualKey != 0x23)) &&
                (uVar4 = 0x10012, virtualKey != 0x21)) &&
               ((uVar4 = 0x1001a, virtualKey != 0x22 && (uVar4 = 0x10014, virtualKey != 0x25))))))
             && (((uVar4 = 0x10016, virtualKey != 0x27 &&
                  ((uVar4 = 0x10011, virtualKey != 0x26 && (uVar4 = 0x10019, virtualKey != 0x28))))
                 && (uVar4 = 0x10015, virtualKey != 0x29)))))) {
        uVar4 = virtualKey + 0x30000;
        if (virtualKey < 0x30) {
          return;
        }
        if (0x39 < virtualKey) {
          if (virtualKey < 0x41) {
            return;
          }
          uVar4 = virtualKey + 0x30020;
          if ((((0x5a < virtualKey) && (uVar4 = 0x2a, virtualKey != 0x6a)) &&
              (uVar4 = 0x2f, virtualKey != 0x6f)) &&
             ((uVar4 = 0x2b, virtualKey != 0x6b && (uVar4 = 0x2d, virtualKey != 0x6d)))) {
            if (virtualKey < 0x60) {
              return;
            }
            if (0x6e < virtualKey) {
              return;
            }
            uVar4 = 0x10007;
            if ((((virtualKey != 0x60) && (uVar4 = 0x10018, virtualKey != 0x61)) &&
                ((uVar4 = 0x10019, virtualKey != 0x62 &&
                 (((uVar4 = 0x1001a, virtualKey != 99 && (uVar4 = 0x10014, virtualKey != 100)) &&
                  (uVar4 = 0x10015, virtualKey != 0x65)))))) &&
               (((uVar4 = 0x10016, virtualKey != 0x66 && (uVar4 = 0x10010, virtualKey != 0x67)) &&
                ((uVar4 = 0x10011, virtualKey != 0x68 && (uVar4 = 0x10012, virtualKey != 0x69))))))
            {
              uVar4 = 0x10006;
            }
          }
        }
      }
      pKVar1 = g_KeyboardEvents + g_KeyboardWriteIndex;
      uVar5 = g_KeyboardWriteIndex + 1;
      g_KeyboardWriteIndex = g_KeyboardWriteIndex + 1;
      pKVar1->keyCode00 = uVar4;
      g_KeyboardEvents[KVar2].stateMask04 = dVar3;
      if ((uVar4 & 0xffff0000) == 0x10000) {
        g_KeyboardSpecialKeyDown[uVar4 & 0xffff] = 1;
      }
      if (0x3f < uVar5) {
        g_KeyboardWriteIndex = 0;
      }
      return;
    }
    if ((g_KeyboardToggleLatchMask & uVar4) == 0) {
      g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask | uVar4;
      g_KeyboardStateMask = g_KeyboardStateMask ^ uVar4;
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
  uint mappedKeyStateCode;
  uint uVar1;
  
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
    uVar1 = 0x10000;
    if ((virtualKey != 0x90) && (uVar1 = 0x20000, virtualKey != 0x91)) {
      uVar1 = 0x20000;
      if (((((virtualKey != 0x20) &&
            (((virtualKey != 0x1b && (virtualKey != 0xd)) && (virtualKey != 0x6c)))) &&
           (((virtualKey != 9 && (virtualKey != 8)) && (virtualKey != 0x2a)))) &&
          ((virtualKey != 0x2c && (virtualKey != 0x2b)))) &&
         (((virtualKey != 0x13 &&
           (((uVar1 = 0x10006, virtualKey != 0x2e && (uVar1 = 0x10007, virtualKey != 0x2d)) &&
            (uVar1 = 0x10010, virtualKey != 0x24)))) &&
          (((uVar1 = 0x10018, virtualKey != 0x23 && (uVar1 = 0x10012, virtualKey != 0x21)) &&
           ((uVar1 = 0x1001a, virtualKey != 0x22 &&
            (((uVar1 = 0x10014, virtualKey != 0x25 && (uVar1 = 0x10016, virtualKey != 0x27)) &&
             ((uVar1 = 0x10011, virtualKey != 0x26 &&
              ((uVar1 = 0x10019, virtualKey != 0x28 && (uVar1 = 0x10015, virtualKey != 0x29)))))))))
           ))))) {
        uVar1 = 0x20000;
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
            uVar1 = 0x10007;
            if ((((((virtualKey != 0x60) && (uVar1 = 0x10018, virtualKey != 0x61)) &&
                  (uVar1 = 0x10019, virtualKey != 0x62)) &&
                 ((uVar1 = 0x1001a, virtualKey != 99 && (uVar1 = 0x10014, virtualKey != 100)))) &&
                (((uVar1 = 0x10015, virtualKey != 0x65 &&
                  ((uVar1 = 0x10016, virtualKey != 0x66 && (uVar1 = 0x10010, virtualKey != 0x67))))
                 && (uVar1 = 0x10011, virtualKey != 0x68)))) &&
               ((uVar1 = 0x10012, virtualKey != 0x69 && (uVar1 = 0x10006, virtualKey != 0x6e)))) {
              uVar1 = 0x20000;
            }
          }
        }
      }
      if ((uVar1 & 0xffff0000) == 0x10000) {
        g_KeyboardSpecialKeyDown[uVar1 & 0xffff] = 0;
      }
      return;
    }
    g_KeyboardToggleLatchMask = g_KeyboardToggleLatchMask & ~uVar1;
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
  KeyboardInputEvent *pKVar1;
  KeyboardEventRingIndex KVar2;
  UiKeyboardEventCode UVar3;
  uint uVar4;
  
  KVar2 = g_KeyboardWriteIndex;
  pKVar1 = g_KeyboardEvents + g_KeyboardWriteIndex;
  UVar3 = character & 0xffff;
  uVar4 = g_KeyboardWriteIndex + 1;
  g_KeyboardWriteIndex = g_KeyboardWriteIndex + 1;
  if (((g_KeyboardStateMask & 0xc) != 0) && (UVar3 < 0x1b)) {
    UVar3 = UVar3 + 0x60;
  }
  g_KeyboardEvents[KVar2].stateMask04 = g_KeyboardStateMask;
  pKVar1->keyCode00 = UVar3;
  if (0x3f < uVar4) {
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
dword __thandor_eax_preserve_ecx_edx Keyboard_ToUpperAscii(KeyboardCharacterCode asciiCodeUnit)

{
  if ((0x60 < asciiCodeUnit) && (asciiCodeUnit < 0x7b)) {
    asciiCodeUnit = asciiCodeUnit - 0x20;
  }
  return asciiCodeUnit;
}

