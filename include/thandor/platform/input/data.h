/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/input/data.h
 */

#ifndef THANDOR_PLATFORM_INPUT_DATA_H
#define THANDOR_PLATFORM_INPUT_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_CursorInputWriteIndex;

extern uint32_t g_CursorMaxWidth;

extern uint32_t g_CursorMaxHeight;

extern PointerFlushEventsProc *g_PointerFlushEvents;

extern PointerSetPositionProc *g_PointerSetPosition;

extern uint32_t g_CursorOverflowLeft;

extern uint32_t g_CursorOverflowRight;

extern uint32_t g_CursorOverflowTop;

extern uint32_t g_CursorOverflowBottom;

extern uint16_t u_engine_mouse_gfx_00416864[17];

extern uint16_t u_engine_mouse_dat_00416886[17];

extern KeyboardInputEvent g_KeyboardEvents[256]; /* 004169E0 g_KeyboardEvents; the ring uses only the first 64 */

extern KeyboardEventRingIndex g_KeyboardWriteIndex;

extern KeyboardEventRingIndex g_KeyboardReadIndex;

extern uint32_t g_KeyboardToggleLatchMask;

extern KeyboardAsciiCaseTransformCallbackTable3 g_KeyboardAsciiCaseTransformCallbacks3;

extern SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate;

extern DirectInputCreateA *pDirectInputCreateA;

extern char dynapi_3[7];

extern char dynapi_19[19];

extern IDirectInputA *g_DirectInput;

extern IDirectInputDeviceA *g_MouseDevice;

extern TH_LEGACY_GUID GUID_SysMouse_Local;

extern TH_LEGACY_GUID GUID_XAxis_Local;

extern TH_LEGACY_GUID GUID_YAxis_Local;

extern TH_LEGACY_GUID GUID_ZAxis_Local;

extern DIDATAFORMAT MouseDataFormat;

extern DIOBJECTDATAFORMAT MouseObjectFormats[7];

extern DIPROPDWORD MouseBufferProperty;

extern uint32_t g_MouseDeviceDataCount;

extern uint32_t g_MousePollBusy;

extern DIDEVICEOBJECTDATA_DX3 g_MouseDeviceEvent;

extern UiPixelCoordinate g_MouseX;

extern UiPixelCoordinate g_MouseY;

extern UiPointerWheelDelta g_MouseWheelDelta;

extern GraphicsCursorButtonState g_MouseButtonMask;

extern SoftwareDisplayModeHookProc *g_DirectInputMouseChainedSetDisplayMode;

#endif
