/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/core/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_CORE_TYPES_H
#define THANDOR_GRAPHICS_CORE_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/core/flags.h> /* THANDOR_FLAG_ENUM: GraphicsCursorButtonState */
#include <thandor/ui/controls/types.h>

struct TH_LEGACY_GUID;
struct GraphicsAdapterRecord;
struct GraphicsCursorInputEvent18;
struct GraphicsCursorFrameRecord;
struct GraphicsPaletteEntry;
struct CursorPointerEvent;
struct GraphicsTextureSourceAsset;
struct SoftwareFramebufferAccess;

/* Callback/function-definition ABIs. */
using GraphicsCursorSetFrameProc = Bool8 (uint32_t frameIndex);

using TH_LEGACY_DWORD = uint32_t;

using TH_LEGACY_WORD = uint16_t;

using TH_LEGACY_BYTE = uint8_t;

struct TH_LEGACY_GUID {
    TH_LEGACY_DWORD Data1;
    TH_LEGACY_WORD Data2;
    TH_LEGACY_WORD Data3;
    TH_LEGACY_BYTE Data4[8];
};

/* One display adapter (SdlVideo_Init lists one; the original listed its DirectDraw adapters). */
struct GraphicsAdapterRecord {
    struct TH_LEGACY_GUID adapterGuid;
    uint16_t driverDescriptionUtf16[21];
    /* where the original kept the device GUID and name and the hardware renderer device data: keeps the record
       at 0x80 bytes, so the arena allocation of g_GraphicsAdapters (Graphics_AllocateTables) keeps its size */
    uint8_t reserved3A[0x46];
};

/* Mouse button state of the pointer events (g_MouseButtonMask, the g_CursorInputEvents ring, CursorPointerEvent,
   g_CursorButtonState). The enumerators are also visible unscoped (using enum below). */
enum class GraphicsCursorButtonState : uint32_t {
    CURSOR_BUTTON_NONE=0,
    LEFT=1,
    MIDDLE=2,
    LEFT_MIDDLE=3,
    RIGHT=4,
    LEFT_RIGHT=5,
    MIDDLE_RIGHT=6,
    LEFT_MIDDLE_RIGHT=7,
    /* GraphicsCursor_ConsumeNextInputEvent: bit 31 of a press's returned button state marks a double click
       (UI_POINTER_BUTTON_REPEAT_CLICK for the press dispatchers): the press comes less than 16 clock ticks after
       the release of the same button and within +-4 pixels of the previous press. */
    GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK=0x80000000u
};
THANDOR_FLAG_ENUM(GraphicsCursorButtonState);
using enum GraphicsCursorButtonState;

/* Type of a pointer event in the g_CursorInputEvents ring. */
enum class GraphicsCursorEventType : int {
    MOTION_OR_WHEEL=0,
    LEFT_PRESS=1,
    MIDDLE_PRESS=2,
    RIGHT_PRESS=3,
    CURSOR_EVENT_UNUSED_4=4, /* never queued; UiPointer_DispatchPendingEvents handles it like a left release */
    LEFT_RELEASE=5,
    MIDDLE_RELEASE=6,
    RIGHT_RELEASE=7
};
using enum GraphicsCursorEventType;

using GraphicsCursorClockValue = uint32_t;

using GraphicsCursorFrameCount = uint32_t;

using GraphicsBitsPerPixel = uint32_t;

using UiNumericCursorFrameIndex = uint32_t;

using ColorChannelByte = uint8_t;

using PaletteEntryFlagsByte = uint8_t;

struct GraphicsCursorInputEvent18 {
    GraphicsCursorEventType eventType;
    GraphicsCursorButtonState buttonState;
    UiPixelCoordinate pointerX;
    UiPixelCoordinate pointerY;
    UiPointerWheelDelta wheelDelta;
    GraphicsCursorClockValue clockValue;
};

struct GraphicsCursorFrameRecord {
    UiPixelOffset hotspotX; 
    UiPixelOffset hotspotY; 
    GraphicsSubresourceIndex idleAnimationFirstSubresourceIndex; 
    GraphicsSubresourceIndex idleAnimationLastSubresourceIndex; 
    GraphicsSubresourceIndex activeAnimationFirstSubresourceIndex; 
    GraphicsSubresourceIndex activeAnimationLastSubresourceIndex; 
    GraphicsSubresourceIndex idleSubresourceIndex; 
    GraphicsSubresourceIndex activeSubresourceIndex; 
};

struct GraphicsPaletteEntry {
    ColorChannelByte red; 
    ColorChannelByte green; 
    ColorChannelByte blue; 
    PaletteEntryFlagsByte flags; 
};

struct CursorPointerEvent {
    uint32_t eventType; // GraphicsCursorEventType of the consumed event
    GraphicsCursorButtonState buttonState; // button state, bit 31 set for a double click
    UiPixelCoordinate pointerX;
    UiPixelCoordinate pointerY;
    UiPointerWheelDelta wheelDelta;
};
using GraphicsCursorConsumeEventProc = Bool8 (CursorPointerEvent *outEvent);

/* True when the raw event type of a consumed event (CursorPointerEvent.eventType) is a button release (the
   types above RIGHT_PRESS). */
inline bool GraphicsCursorEventType_IsRelease(uint32_t eventType)
{
    return static_cast<uint32_t>(RIGHT_PRESS) < eventType;
}

#endif /* THANDOR_GRAPHICS_CORE_TYPES_H */
