/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/input/data.h
 */

#ifndef THANDOR_PLATFORM_INPUT_DATA_H
#define THANDOR_PLATFORM_INPUT_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_CursorInputWriteIndex; /* 004167F0 g_CursorInputWriteIndex */

extern uint32_t g_CursorMaxWidth; /* 00416820 g_CursorMaxWidth */

extern uint32_t g_CursorMaxHeight; /* 00416824 g_CursorMaxHeight */

extern PointerFlushEventsProc *g_PointerFlushEvents; /* 00416844 g_PointerFlushEvents */

extern PointerSetPositionProc *g_PointerSetPosition; /* 0041684C g_PointerSetPosition */

extern uint32_t g_CursorOverflowLeft; /* 00416854 g_CursorOverflowLeft */

extern uint32_t g_CursorOverflowRight; /* 00416858 g_CursorOverflowRight */

extern uint32_t g_CursorOverflowTop; /* 0041685C g_CursorOverflowTop */

extern uint32_t g_CursorOverflowBottom; /* 00416860 g_CursorOverflowBottom */

extern uint16_t u_engine_mouse_gfx_00416864[17]; /* 00416864 u_engine_mouse_gfx_00416864 */

extern uint16_t u_engine_mouse_dat_00416886[17]; /* 00416886 u_engine_mouse_dat_00416886 */

extern KeyboardInputEvent g_KeyboardEvents[256]; /* 004169E0 g_KeyboardEvents; the ring uses only the first 64 */

extern KeyboardEventRingIndex g_KeyboardWriteIndex; /* 004171E0 g_KeyboardWriteIndex */

extern KeyboardEventRingIndex g_KeyboardReadIndex; /* 004171E4 g_KeyboardReadIndex */

extern uint32_t g_KeyboardToggleLatchMask; /* 004171EC g_KeyboardToggleLatchMask */

extern KeyboardAsciiCaseTransformCallbackTable3 g_KeyboardAsciiCaseTransformCallbacks3; /* 00417218 g_KeyboardAsciiCaseTransformCallbacks3 */

extern SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate; /* 004A8ED8 g_SoftwareFramebufferCreate */

extern DirectInputCreateA *pDirectInputCreateA; /* 00573FC4 pDirectInputCreateA */

extern char dynapi_3[7]; /* 005744BA dynapi_3 */

extern char dynapi_19[19]; /* 00574586 dynapi_19 */

extern IDirectInputA *g_DirectInput; /* 00576B10 g_DirectInput */

extern IDirectInputDeviceA *g_MouseDevice; /* 00576B14 g_MouseDevice */

extern TH_LEGACY_GUID GUID_SysMouse_Local; /* 00576B18 GUID_SysMouse_Local */

extern TH_LEGACY_GUID GUID_XAxis_Local; /* 00576B28 GUID_XAxis_Local */

extern TH_LEGACY_GUID GUID_YAxis_Local; /* 00576B38 GUID_YAxis_Local */

extern TH_LEGACY_GUID GUID_ZAxis_Local; /* 00576B48 GUID_ZAxis_Local */

extern DIDATAFORMAT MouseDataFormat; /* 00576B58 MouseDataFormat */

extern DIOBJECTDATAFORMAT MouseObjectFormats[7]; /* 00576B70 MouseObjectFormats */

extern DIPROPDWORD MouseBufferProperty; /* 00576BE0 MouseBufferProperty */

extern uint32_t g_MouseDeviceDataCount; /* 00576BF4 g_MouseDeviceDataCount */

extern uint32_t g_MousePollBusy; /* 00576BF8 g_MousePollBusy */

extern DIDEVICEOBJECTDATA_DX3 g_MouseDeviceEvent; /* 00576BFC g_MouseDeviceEvent */

extern UiPixelCoordinate g_MouseX; /* 00576C0C g_MouseX */

extern UiPixelCoordinate g_MouseY; /* 00576C10 g_MouseY */

extern UiPointerWheelDelta g_MouseWheelDelta; /* 00576C14 g_MouseWheelDelta */

extern GraphicsCursorButtonState g_MouseButtonMask; /* 00576C18 g_MouseButtonMask */

extern SoftwareDisplayModeHookProc *g_DirectInputMouseChainedSetDisplayMode; /* 00576C20 g_DirectInputMouseChainedSetDisplayMode */

#endif
