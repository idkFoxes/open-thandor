#ifndef THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H
#define THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/resources/framebuffer. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004A9250 */
void __cdecl GraphicsFramebuffer_BeginAccessStub(void);

/* 0x004A9260 */
void __cdecl GraphicsFramebuffer_EndAccessStub(void);

/* 0x005796E0 */
void GraphicsFramebuffer_Present(SoftwareFramebufferAccess *framebuffer);

/* 0x005798A0 */
GraphicsCapturedTextureSourceAsset * GraphicsFramebuffer_CaptureRegion16Bit (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth, GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x00579B50 */
GraphicsCapturedTextureSourceAsset * GraphicsFramebuffer_CaptureRegion32Bit (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth, GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x00579D90 */
void __cdecl GraphicsFramebuffer_BeginAccess(void);

/* 0x00579E60 */
void __cdecl GraphicsFramebuffer_EndAccess(void);

#endif /* THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H */
