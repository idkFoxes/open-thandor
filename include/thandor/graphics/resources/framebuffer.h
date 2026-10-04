/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/framebuffer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H
#define THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H

#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/platform/sdl3/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/framebuffer. */

/* Layout of the one-image 'gfx' asset built by the screen captures (SdlVideo_CaptureRegion*): the table of the single
   source entry starts at byte 0x200, the ARGB8888 pixels at byte 0x220. */
#define GRAPHICS_CAPTURE_SOURCE_ENTRY_OFFSET 0x200
#define GRAPHICS_CAPTURE_PIXELS_OFFSET 0x220
/* Functions are grouped by semantic ownership. */

Bool8 GraphicsFramebuffer_BeginAccessStub(void);

void GraphicsFramebuffer_EndAccessStub(void);

void GraphicsFramebuffer_InitCaptureAsset
          (GraphicsCapturedTextureSourceAsset *capturedAsset,uint32_t allocationSize,
          GraphicsPixelDimension captureWidth,GraphicsPixelDimension captureHeight);

uint8_t GraphicsFramebuffer_ExpandChannelTo8Bit
          (uint32_t pixel,GraphicsPackedPixelMask channelMask,GraphicsPixelChannelBitShift channelShift,
          GraphicsPixelChannelBitCount channelBitCount);

extern SoftwareFramebufferAccess *g_FramebufferAccess;
extern uint32_t g_FramebufferRowStrideBytes;
extern uint32_t g_FramebufferHeight;
extern GraphicsFramebufferPresentProc *g_GraphicsFramebufferPresent;
extern GraphicsFramebufferBeginAccessProc *g_GraphicsFramebufferBeginAccess;
extern GraphicsFramebufferEndAccessProc *g_GraphicsFramebufferEndAccess;
extern GraphicsFramebufferFillRectArgbProc *g_GraphicsFramebufferFillRectArgb;

extern GraphicsFramebufferCaptureRegionProc *g_GraphicsFramebufferCaptureRegion;
extern SoftwareFramebufferAccess g_DisplayFramebufferAccess;

extern uint32_t g_FramebufferWidth;

#endif /* THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H */
