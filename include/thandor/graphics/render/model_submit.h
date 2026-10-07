/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/model_submit.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_MODEL_SUBMIT_H
#define THANDOR_GRAPHICS_RENDER_MODEL_SUBMIT_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

extern GraphicsFixedVec3 g_GraphicsTransformInputScratchVec3;

extern GraphicsFixedVec3 g_GraphicsTransformOutputScratchVec3;

extern GraphicsFixedMatrix3x4 g_GraphicsTransformScratchMatrix3x4;

void ModelRender_DrawMeshGroupsWithTemporaryTransform
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

void ModelRender_DrawMeshGroupsAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

void ModelProjectedBounds_AccumulateHierarchyRecursive(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

void ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 triangle,GraphicsFixedVec3 *vertex);

void ModelRender_SubmitTriangle(Q12 facingThresholdQ12,GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

void ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

void ModelRender_SubmitMeshTrianglesAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

bool ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex);

void ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

void ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

int32_t ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle);

void ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds);

void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime);

extern GraphicsFixedMatrix3x4 g_ModelViewCompositeTransform;

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_SUBMIT_H */
