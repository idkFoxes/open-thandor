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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004A9250 */
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccessStub(void);

/* 0x004A9260 */
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccessStub(void);

/* 0x005796E0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsFramebuffer_Present(SoftwareFramebufferAccess *framebuffer);

/* 0x005798A0 */
GraphicsFramebufferCaptureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion16Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x00579B50 */
GraphicsFramebufferCaptureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsFramebuffer_CaptureRegion32Bit
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x00579D90 */
bool __thandor_cf_preserve_eax_ecx_edx GraphicsFramebuffer_BeginAccess(void);

/* 0x00579E60 */
void __thandor_void_preserve_eax_ecx_edx GraphicsFramebuffer_EndAccess(void);

#endif /* THANDOR_GRAPHICS_RESOURCES_FRAMEBUFFER_H */
