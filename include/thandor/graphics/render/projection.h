/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/projection.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_PROJECTION_H
#define THANDOR_GRAPHICS_RENDER_PROJECTION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/projection. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486B00 */
GraphicsOffscreenAllocationEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsOffscreen_RenderModelListToTextureSourceCf
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputWidth,
          GraphicsPixelDimension outputHeight,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes);

/* 0x0050A6A0 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedPoint_IsInsideTriangleCf
          (int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2);


/* 0x00486940 */
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedTriangle_PointOutsideBoundsCf
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX);

#endif /* THANDOR_GRAPHICS_RENDER_PROJECTION_H */
