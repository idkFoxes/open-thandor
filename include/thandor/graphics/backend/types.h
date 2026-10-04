/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_TYPES_H
#define THANDOR_GRAPHICS_BACKEND_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/frontend/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct GraphicsDisplayMode GraphicsDisplayMode, *PGraphicsDisplayMode;
typedef struct SoftwarePixelFormatConfig SoftwarePixelFormatConfig, *PSoftwarePixelFormatConfig;
typedef struct SoftwareRasterScalarMmxLane SoftwareRasterScalarMmxLane, *PSoftwareRasterScalarMmxLane;
typedef struct SoftwareFramebufferAccess SoftwareFramebufferAccess, *PSoftwareFramebufferAccess;
typedef struct SoftwarePixelPackTables SoftwarePixelPackTables, *PSoftwarePixelPackTables;
typedef struct SoftwareRasterTextureAddressState SoftwareRasterTextureAddressState, *PSoftwareRasterTextureAddressState;
typedef struct SoftwareRasterTexCoordFixed2 SoftwareRasterTexCoordFixed2, *PSoftwareRasterTexCoordFixed2;
typedef struct SoftwareRasterScanState SoftwareRasterScanState, *PSoftwareRasterScanState;
typedef struct SoftwareRasterColorFixed4 SoftwareRasterColorFixed4, *PSoftwareRasterColorFixed4;

/* Display-mode switch slot (g_GraphicsSetDisplayMode and its chained hooks): true on success; on failure
   returns false and stores the error code (or message) in *errorCode, which is left untouched on success. */
using SoftwareDisplayModeHookProc = Bool8 (uint32_t adapterIndex, uint32_t bitsPerPixel, uint32_t height, uint32_t width, uint32_t *errorCode);

enum {
    SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT=4
};
using SoftwareFramebufferPixelSize = int;

using GraphicsPackedPixelMask = uint32_t;

using SoftwareColorTransformQ16 = int;

using GraphicsDiagnosticCounter = uint32_t;

using DisplayModeHookArgument0 = uint32_t;

using DisplayModeHookArgument1 = uint32_t;

using GraphicsDisplayModeCount = uint32_t;


using GraphicsPixelChannelBitShift = uint32_t;

using GraphicsPixelChannelBitCount = uint32_t;

struct GraphicsDisplayMode {
    FrontendDisplayDimensionPixels width; 
    FrontendDisplayDimensionPixels height; 
    FrontendColorDepthBits bitsPerPixel; 
    FrontendDisplayAdapterIndex adapterIndex; 
};

struct SoftwarePixelFormatConfig {
    GraphicsPixelChannelBitCount redBitCount; 
    GraphicsPixelChannelBitCount greenBitCount; 
    GraphicsPixelChannelBitCount blueBitCount; 
    GraphicsPixelChannelBitShift redShift; 
    GraphicsPixelChannelBitShift greenShift; 
    GraphicsPixelChannelBitShift blueShift; 
    GraphicsPackedPixelMask redMask; 
    GraphicsPackedPixelMask greenMask; 
    GraphicsPackedPixelMask blueMask; 
};

struct SoftwareRasterScalarMmxLane {
    int32_t value; 
    int32_t unusedHighLane; 
};

struct SoftwareFramebufferAccess {
    GraphicsPixelDimension width; 
    GraphicsPixelDimension height; 
    SoftwareFramebufferPixelSize bytesPerPixel; 
    Ptr32<uint8_t> pixels; 
};

struct SoftwarePixelPackTables {
    uint32_t blue[256]; 
    uint32_t green[256]; 
    uint32_t red[256]; 
};

struct SoftwareRasterTextureAddressState {
    uint32_t uMaskQ12; 
    uint32_t zeroAfterUMask; 
    uint32_t zeroBeforeVMask; 
    uint32_t vMaskQ12; 
    uint32_t vRowAddressShift; 
    uint32_t zeroShiftHigh; 
};

struct SoftwareRasterTexCoordFixed2 {
    GraphicsPrimitiveTextureCoordinateFixed u; 
    GraphicsPrimitiveTextureCoordinateFixed v; 
};

struct SoftwareRasterColorFixed4 {
    SoftwareColorLaneFixed16 blue; 
    SoftwareColorLaneFixed16 green; 
    SoftwareColorLaneFixed16 red; 
    SoftwareColorLaneFixed16 alpha; 
};

struct SoftwareRasterScanState {
    struct SoftwareRasterColorFixed4 longEdgeColor; 
    struct SoftwareRasterColorFixed4 longEdgeColorStepY; 
    struct SoftwareRasterColorFixed4 colorStepX; 
    struct SoftwareRasterTexCoordFixed2 longEdgeTexCoord; 
    struct SoftwareRasterTexCoordFixed2 longEdgeTexCoordStepY; 
    struct SoftwareRasterTexCoordFixed2 texCoordStepX; 
    struct SoftwareRasterScalarMmxLane longEdgeDepth; 
    struct SoftwareRasterScalarMmxLane longEdgeDepthStepY; 
    struct SoftwareRasterScalarMmxLane depthStepX; 
    int32_t longEdgeXQ12; 
    int32_t shortEdgeXQ12; 
    int32_t longEdgeXStepYQ12; 
    int32_t shortEdgeXStepYQ12; 
    struct SoftwareRasterTextureAddressState textureAddress; 
    int32_t scanlineY; 
};
using GraphicsBeginSceneProc = void ();
using GraphicsDrawPrimitiveQueueProc = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue);
using GraphicsEndSceneProc = void ();
using GraphicsSetViewportProc = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX);
using SoftwareBuildPixelPackTablesProc = void (int32_t colorScaleQ16, int32_t colorBiasQ16);
using SoftwareFramebufferCreateProc = SoftwareFramebufferAccess * (SoftwareFramebufferPixelSize bytesPerPixel, GraphicsPixelDimension height, GraphicsPixelDimension width, uint32_t * outError);
using SoftwareRasterHandler = void (int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitivePacket * packet);

#endif /* THANDOR_GRAPHICS_BACKEND_TYPES_H */
