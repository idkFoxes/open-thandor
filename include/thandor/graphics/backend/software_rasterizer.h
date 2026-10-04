/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software_rasterizer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTERIZER_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTERIZER_H

#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/core/contracts.h>

/* g_SoftwareDepthEpoch drops by one step (the top byte) per frame (SoftwareRenderer_AdvanceDepthEpoch) */
#define SOFTWARE_DEPTH_EPOCH_STEP 0x1000000

extern SoftwareRasterScanState g_SoftwareRasterScanState;

extern int32_t *g_SoftwareDepthBuffer;

extern uint32_t g_SoftwareDepthRowStrideBytes;

extern void *g_SoftwareAuxiliaryTargetBase;

extern int32_t g_SoftwareDepthEpoch;

void SoftwareRenderer_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

void SoftwareRenderer_DrawQueue32Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

void SoftwareRenderer_DrawQueueAuxiliary
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,void *targetBase,
          GraphicsPrimitiveQueue *queue);

void SoftwareRenderer_DrawPrimitiveQueueBridge(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

void SoftwareRaster32_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster32_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRenderer_AdvanceDepthEpoch(void);

void SoftwareRenderer_PrepareTrianglePacket(GraphicsPrimitivePacket *packet);

extern SoftwareRasterHandler *g_SoftwareRasterHandlers32Bit[64];

extern SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64];

extern SoftwareDrawQueueProc *g_SoftwareDrawQueue;

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTERIZER_H */
