#ifndef THANDOR_GRAPHICS_RENDER_MODEL_H
#define THANDOR_GRAPHICS_RENDER_MODEL_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/model. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BDC90 */
void __fastcall ModelRender_DrawMeshGroupsWithTemporaryTransform (undefined4 param_1,undefined4 param_2,Q12 facingThresholdQ12, ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE1F0 */
undefined4 __fastcall ModelRender_DrawMeshGroupsAlternatePath (undefined4 param_1,undefined4 param_2,int param_3,ModelRuntimeNode *param_4);

/* 0x0050A5C0 */
undefined4 __fastcall ModelProjectedBounds_AccumulateHierarchyRecursive (undefined4 param_1,undefined4 param_2,int *param_3,int param_4);

/* 0x004BD4B0 */
undefined8 ModelRender_PrepareProjectedVertex (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 meshGroup,GraphicsFixedVec3 *vertex);

/* 0x004BD9B0 */
undefined8 __fastcall ModelRender_SubmitTriangle (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 facingThresholdQ12, GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

/* 0x004BDC20 */
void __fastcall ModelRender_SubmitMeshTriangles (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 facingThresholdQ12, ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE180 */
void __fastcall ModelRender_SubmitMeshTrianglesAlternatePath (undefined4 param_1,undefined4 param_2,int param_3,ModelRuntimeNode *param_4);

/* 0x004BD6B0 */
undefined8 ModelRender_PrepareProjectedVertexAlternatePath(int param_1,int param_2,GraphicsFixedVec3 *param_3);

/* 0x004BDFB0 */
undefined8 ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *param_1,int param_2);

/* 0x0050A4A0 */
void __fastcall ModelProjectedBounds_AccumulateNode(undefined4 param_1,undefined4 param_2,int *param_3,int param_4);

/* 0x004BD7E0 */
undefined8 ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *modelNodeRuntime);

/* 0x004CC710 */
PackedArgb32 ModelRender_ComputeVertexIntensityDefaultPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12, GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

/* 0x004CC820 */
PackedArgb32 ModelRender_ComputeVertexIntensityScaledPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

/* 0x004CC940 */
PackedArgb32 ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath (PackedArgb32 param_1,GraphicsFixedVec3 *param_2,PackedArgb32 param_3,PackedArgb32 param_4 ,GraphicsFixedVec3 *param_5);

/* 0x0050A430 */
undefined8 ModelProjectedBounds_ExpandWithCurrentScratchPoint(int *param_1);

/* 0x004BD800 */
void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime);

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_H */
