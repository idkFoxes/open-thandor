#ifndef THANDOR_GRAPHICS_RENDER_PROJECTION_H
#define THANDOR_GRAPHICS_RENDER_PROJECTION_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/projection. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486B00 */
undefined8 __fastcall GraphicsOffscreen_RenderModelListToTextureSourceCf (undefined4 param_1,undefined4 param_2,GraphicsOffscreenSceneExtents *sceneExtents, undefined4 param_4,GraphicsOffscreenViewParameters *viewParameters, GraphicsPixelDimension outputWidth,GraphicsPixelDimension outputHeight, ModelRuntimeCount modelCount,ModelRuntimeNode **modelNodes);

/* 0x0050A6A0 */
void GraphicsProjectedPoint_IsInsideTriangleCf (int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0, GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2);

#endif /* THANDOR_GRAPHICS_RENDER_PROJECTION_H */
