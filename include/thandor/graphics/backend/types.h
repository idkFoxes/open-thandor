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

/* Types (split from generated/types.h by tools/dev/split_types.py). */

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
typedef Bool8 SoftwareDisplayModeHookProc(uint32_t adapterIndex, uint32_t bitsPerPixel, uint32_t height, uint32_t width, uint32_t *errorCode);
typedef void GraphicsFramebufferCopyRegionToOriginProc(int32_t copyHeight, int32_t copyWidth, int32_t sourceY, int32_t sourceX, SoftwareFramebufferAccess * destination, SoftwareFramebufferAccess * source);
typedef void GraphicsFramebufferCopyOriginToRegionProc(int32_t copyHeight, int32_t copyWidth, int32_t destinationY, int32_t destinationX, SoftwareFramebufferAccess * source, SoftwareFramebufferAccess * destination);
typedef void SoftwareFramebufferDestroyProc(SoftwareFramebufferAccess * framebuffer);

enum {
    SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT=4
};
typedef int SoftwareFramebufferPixelSize;

typedef uint32_t GraphicsPackedPixelMask;

typedef int SoftwareColorTransformQ16;

typedef uint32_t GraphicsDiagnosticCounter;

typedef uint32_t DisplayModeHookArgument0;

typedef uint32_t DisplayModeHookArgument1;

typedef uint32_t GraphicsDisplayModeCount;

typedef uint32_t PaletteBankIndex;

typedef uint32_t GraphicsPixelChannelBitShift;

typedef uint32_t GraphicsIntegerScale;

typedef uint32_t GraphicsPixelChannelBitCount;

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
typedef void GraphicsBeginSceneProc();
typedef void GraphicsDrawPrimitiveQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue);
typedef void GraphicsEndSceneProc();
typedef void GraphicsSetViewportProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX);
typedef void SoftwareBuildPixelPackTablesProc(int32_t colorScaleQ16, int32_t colorBiasQ16);
typedef void SoftwareDrawQueueProc(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitiveQueue * queue);
typedef SoftwareFramebufferAccess * SoftwareFramebufferCreateProc(uint32_t bytesPerPixel, uint32_t height, uint32_t width, uint32_t * outError);
typedef void SoftwareRasterHandler(int32_t clipMaxY, int32_t clipMaxX, int32_t clipMinY, int32_t clipMinX, GraphicsPrimitivePacket * packet);

#endif /* THANDOR_GRAPHICS_BACKEND_TYPES_H */
