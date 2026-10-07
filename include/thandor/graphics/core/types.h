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

enum {
    CURSOR_BUTTON_NONE=0,
    LEFT=1,
    MIDDLE=2,
    LEFT_MIDDLE=3,
    RIGHT=4,
    LEFT_RIGHT=5,
    MIDDLE_RIGHT=6,
    LEFT_MIDDLE_RIGHT=7
};
using GraphicsCursorButtonState = int;

enum {
    MOTION_OR_WHEEL=0,
    LEFT_PRESS=1,
    MIDDLE_PRESS=2,
    RIGHT_PRESS=3,
    LEFT_RELEASE=5,
    MIDDLE_RELEASE=6,
    RIGHT_RELEASE=7
};
using GraphicsCursorEventType = int;

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

#endif /* THANDOR_GRAPHICS_CORE_TYPES_H */
