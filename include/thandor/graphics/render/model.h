/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/model.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_MODEL_H
#define THANDOR_GRAPHICS_RENDER_MODEL_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/model. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BDC90 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_DrawMeshGroupsWithTemporaryTransform
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE1F0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_DrawMeshGroupsAlternatePath
          (ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x0050A5C0 */
void __thandor_void_preserve_eax_ecx
ModelProjectedBounds_AccumulateHierarchyRecursive
          (ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

/* 0x004BD4B0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 meshGroup,GraphicsFixedVec3 *vertex);

/* 0x004BD9B0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitTriangle
          (Q12 facingThresholdQ12,GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

/* 0x004BDC20 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE180 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitMeshTrianglesAlternatePath
          (ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BD6B0 */
bool __thandor_cf_preserve_eax_ecx_edx
ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex);

/* 0x004BDFB0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

/* 0x0050A4A0 */
void __thandor_void_preserve_eax_ecx_edx
ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

/* 0x004BD7E0 */
sdword __thandor_eax_preserve_ecx_edx
ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *modelNodeRuntime);

/* 0x004CC710 */
PackedArgb32 ModelRender_ComputeVertexIntensityDefaultPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12, GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

/* 0x004CC820 */
PackedArgb32 ModelRender_ComputeVertexIntensityScaledPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

/* 0x004CC940 */
PackedArgb32
ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
          (PackedArgb32 vertexPackedColor,GraphicsFixedVec3 *vertexPositionQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12);

/* 0x0050A430 */
void __thandor_preserve_eax_edx
ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds);

/* 0x004BD800 */
void __thandor_void_preserve_eax_ecx_edx
ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime);

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_H */
