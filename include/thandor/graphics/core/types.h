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

/* Types (split out by tools/dev/split_types.py). */

typedef struct TH_LEGACY_GUID TH_LEGACY_GUID, *PTH_LEGACY_GUID;
typedef struct GraphicsAdapterRecord GraphicsAdapterRecord, *PGraphicsAdapterRecord;
typedef struct GraphicsCursorInputEvent18 GraphicsCursorInputEvent18, *PGraphicsCursorInputEvent18;
typedef struct GraphicsCursorFrameRecord GraphicsCursorFrameRecord, *PGraphicsCursorFrameRecord;
typedef struct DirectDrawPaletteEntry DirectDrawPaletteEntry, *PDirectDrawPaletteEntry;
typedef struct CursorPointerEvent CursorPointerEvent, *PCursorPointerEvent;
typedef struct GraphicsTextureSourceAsset GraphicsTextureSourceAsset;
typedef struct SoftwareFramebufferAccess SoftwareFramebufferAccess;

/* Callback/function-definition ABIs. */
typedef Bool8 GraphicsCursorSetFrameProc(uint32_t frameIndex);
typedef void GraphicsTextureSourceBlitIntegerScaledSourceAlphaProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t integerScale, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);
typedef void GraphicsTextureSourceBlitSourceAlphaPaletteBankProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, int32_t drawY, int32_t drawX, uint32_t paletteBankIndex, uint32_t subresourceIndex, GraphicsTextureSourceAsset * sourceAsset, SoftwareFramebufferAccess * framebuffer);

typedef uint32_t TH_LEGACY_DWORD;

typedef uint16_t TH_LEGACY_WORD;

typedef uint8_t TH_LEGACY_BYTE;

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
typedef int GraphicsCursorButtonState;

enum {
    MOTION_OR_WHEEL=0,
    LEFT_PRESS=1,
    MIDDLE_PRESS=2,
    RIGHT_PRESS=3,
    LEFT_RELEASE=5,
    MIDDLE_RELEASE=6,
    RIGHT_RELEASE=7
};
typedef int GraphicsCursorEventType;

typedef uint32_t GraphicsCursorClockValue;

typedef uint32_t GraphicsCursorFrameCount;

typedef uint32_t GraphicsBitsPerPixel;

typedef uint32_t UiNumericCursorFrameIndex;

typedef uint8_t ColorChannelByte;

typedef uint8_t PaletteEntryFlagsByte;

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

struct DirectDrawPaletteEntry {
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
typedef Bool8 GraphicsCursorConsumeEventProc(CursorPointerEvent *outEvent);

#endif /* THANDOR_GRAPHICS_CORE_TYPES_H */
