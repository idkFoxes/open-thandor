/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/framebuffer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H
#define THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/framebuffer. */

/* Layout of the one-image 'gfx' asset built by GraphicsFramebuffer_CaptureRegion*: the table of the single
   source entry starts at byte 0x200, the ARGB8888 pixels at byte 0x220. */
#define GRAPHICS_CAPTURE_SOURCE_ENTRY_OFFSET 0x200
#define GRAPHICS_CAPTURE_PIXELS_OFFSET 0x220
/* Stage numbers left in g_PackageLastErrorPath with FATAL_ERROR_DIRECTDRAW_CREATE_SURFACES when the back surface
   cannot be restored or locked for a capture */
#define GRAPHICS_CAPTURE_FAILED_STAGE_16BIT 100
#define GRAPHICS_CAPTURE_FAILED_STAGE_32BIT 101
/* Functions are grouped by semantic ownership. */

bool GraphicsFramebuffer_BeginAccessStub(void);

void GraphicsFramebuffer_EndAccessStub(void);

void GraphicsFramebuffer_Present(SoftwareFramebufferAccess *framebuffer);

GraphicsCapturedTextureSourceAsset *GraphicsFramebuffer_CaptureRegion16Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

GraphicsCapturedTextureSourceAsset *GraphicsFramebuffer_CaptureRegion32Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

bool GraphicsFramebuffer_BeginAccess(void);

void GraphicsFramebuffer_EndAccess(void);

extern SoftwareFramebufferAccess *g_FramebufferAccess;
extern uint32_t g_FramebufferRowStrideBytes;
extern uint32_t g_FramebufferHeight;
extern GraphicsFramebufferPresentProc *g_GraphicsFramebufferPresent;
extern GraphicsFramebufferBeginAccessProc *g_GraphicsFramebufferBeginAccess;
extern GraphicsFramebufferEndAccessProc *g_GraphicsFramebufferEndAccess;
extern GraphicsFramebufferFillRectArgbProc *g_GraphicsFramebufferFillRectArgb;

extern IDirectDrawSurface3 *g_BackSurface3;

extern GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion;
extern SoftwareFramebufferAccess g_DisplayFramebufferAccess;

extern uint32_t g_FramebufferWidth;

#endif /* THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H */
