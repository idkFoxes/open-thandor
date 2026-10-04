/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/offscreen.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_OFFSCREEN_H
#define THANDOR_GRAPHICS_RENDER_OFFSCREEN_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/offscreen. */
/* Functions are grouped by semantic ownership. */

GraphicsTextureSourceAsset *GraphicsOffscreen_RenderModelListToTextureSource
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputHeight,
          GraphicsPixelDimension outputWidth,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes);

extern GraphicsOffscreenRenderModelListToTextureSourceProc *g_GraphicsOffscreenRenderModelListToTextureSource;

#endif /* THANDOR_GRAPHICS_RENDER_OFFSCREEN_H */
