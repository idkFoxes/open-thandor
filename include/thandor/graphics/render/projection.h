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

GraphicsTextureSourceAsset *GraphicsOffscreen_RenderModelListToTextureSource
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputHeight,
          GraphicsPixelDimension outputWidth,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes);

bool GraphicsProjectedPoint_IsInsideTriangle(int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2);

#endif /* THANDOR_GRAPHICS_RENDER_PROJECTION_H */
