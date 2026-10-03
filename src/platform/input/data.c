/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/input/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/platform/input/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) uint32_t g_CursorInputWriteIndex = 0;

__declspec(align(16)) uint32_t g_CursorMaxWidth = 0;

__declspec(align(4)) uint32_t g_CursorMaxHeight = 0;

__declspec(align(4)) PointerFlushEventsProc *g_PointerFlushEvents = 0;

__declspec(align(4)) PointerSetPositionProc *g_PointerSetPosition = 0;

__declspec(align(4)) uint32_t g_CursorOverflowLeft = 0;

__declspec(align(8)) uint32_t g_CursorOverflowRight = 0;

__declspec(align(4)) uint32_t g_CursorOverflowTop = 0;

__declspec(align(16)) uint32_t g_CursorOverflowBottom = 0;

__declspec(align(4)) uint16_t u_engine_mouse_gfx_00416864[17] = L"engine\\mouse.gfx";

__declspec(align(4)) uint16_t u_engine_mouse_dat_00416886[17] = L"engine\\mouse.dat";

/* 004169E0 g_KeyboardEvents. Original quirk: the original reserves 256 events (0x800 bytes) for the ring, but
   the read and write indices wrap at KEYBOARD_EVENT_RING_SIZE (64), so entries 64-255 are never used. */
__declspec(align(16)) KeyboardInputEvent g_KeyboardEvents[256] = {0};

__declspec(align(16)) KeyboardEventRingIndex g_KeyboardWriteIndex = 0;

__declspec(align(4)) KeyboardEventRingIndex g_KeyboardReadIndex = 0;

__declspec(align(4)) uint32_t g_KeyboardToggleLatchMask = 0;

__declspec(align(8)) KeyboardAsciiCaseTransformCallbackTable3 g_KeyboardAsciiCaseTransformCallbacks3 = {
    .compareCaseInsensitiveFlags = (void *)Keyboard_CompareAsciiCaseInsensitiveFlags,
    .toUpper = (void *)Keyboard_ToUpperAscii,
    .toLower = (void *)Keyboard_ToLowerAscii};

__declspec(align(8)) SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate = (void *)SoftwareFramebuffer_Create;

__declspec(align(4)) DirectInputCreateA *pDirectInputCreateA = 0;

__declspec(align(4)) char dynapi_3[7] = "DINPUT";

__declspec(align(4)) char dynapi_19[19] = "DirectInputCreateA";

__declspec(align(16)) IDirectInputA *g_DirectInput = 0;

__declspec(align(4)) IDirectInputDeviceA *g_MouseDevice = 0;

__declspec(align(8)) TH_LEGACY_GUID GUID_SysMouse_Local = {.Data1 = 0x6F1D2B60, .Data2 = 54688, .Data3 = 4559, .Data4 = "\277\307DEST"};

__declspec(align(8)) TH_LEGACY_GUID GUID_XAxis_Local = {.Data1 = 0xA36D02E0, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

__declspec(align(8)) TH_LEGACY_GUID GUID_YAxis_Local = {.Data1 = 0xA36D02E1, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

__declspec(align(8)) TH_LEGACY_GUID GUID_ZAxis_Local = {.Data1 = 0xA36D02E2, .Data2 = 51699, .Data3 = 4559, .Data4 = "\277\307DEST"};

__declspec(align(8)) DIDATAFORMAT MouseDataFormat = {
    .dwSize = 24,
    .dwObjSize = 16,
    .dwFlags = 0x2,
    .dwDataSize = 16,
    .dwNumObjs = 7,
    .rgodf = (void *)&MouseObjectFormats};

__declspec(align(16)) DIOBJECTDATAFORMAT MouseObjectFormats[7] = {
    /* 0 */ {.pguid = (void *)&GUID_XAxis_Local, .dwType = 0xFFFF03},
    /* 1 */ {.pguid = (void *)&GUID_YAxis_Local, .dwOfs = 4, .dwType = 0xFFFF03},
    /* 2 */ {.pguid = (void *)&GUID_ZAxis_Local, .dwOfs = 8, .dwType = 0x80FFFF03},
    /* 3 */ {.dwOfs = 12, .dwType = 0xFFFF0C},
    /* 4 */ {.dwOfs = 13, .dwType = 0xFFFF0C},
    /* 5 */ {.dwOfs = 14, .dwType = 0x80FFFF0C},
    /* 6 */ {.dwOfs = 15, .dwType = 0x80FFFF0C}};

__declspec(align(16)) DIPROPDWORD MouseBufferProperty = {.diph = {.dwSize = 20, .dwHeaderSize = 16}, .dwData = 256};

__declspec(align(4)) uint32_t g_MouseDeviceDataCount = 0;

__declspec(align(8)) uint32_t g_MousePollBusy = 0;

__declspec(align(4)) DIDEVICEOBJECTDATA_DX3 g_MouseDeviceEvent = {0};

__declspec(align(4)) UiPixelCoordinate g_MouseX = 0;

__declspec(align(16)) UiPixelCoordinate g_MouseY = 0;

__declspec(align(4)) UiPointerWheelDelta g_MouseWheelDelta = 0;

__declspec(align(8)) GraphicsCursorButtonState g_MouseButtonMask = 0;

__declspec(align(16)) SoftwareDisplayModeHookProc *g_DirectInputMouseChainedSetDisplayMode = 0;
