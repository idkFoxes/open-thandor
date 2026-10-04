/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/model_draw.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_MODEL_DRAW_H
#define THANDOR_GRAPHICS_RENDER_MODEL_DRAW_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

extern int32_t g_ModelLodDepthThresholdQ8;

extern GraphicsFixedVec3 g_ModelCullViewRelative;

void ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime);

void ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode);

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_DRAW_H */
