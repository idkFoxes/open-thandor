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

/* Rows of g_ModelLightingMmxMultiplierRows (one table in the original). Both vertex
   lighting paths index it with a signed row relative to a base row, so negative indices read the rows before it. */
#define MODEL_LIGHTING_MMX_ROW_COUNT 819
/* row 0 = distance attenuation row -136 (former g_ModelDistanceAttenuationMmxNegativeRows) */
#define MODEL_DISTANCE_ATTENUATION_NEGATIVE_ROW0 0
/* ModelRender_ComputeVertexIntensityDefaultPath's base, indexed by the light-facing dot >> 21 */
#define MODEL_DISTANCE_ATTENUATION_ROW0 136
/* ModelRender_ComputeVertexIntensityScaledPath's base, indexed by (dot / lightingScaleQ12) >> 9 */
#define MODEL_LIGHTING_SCALE_ROW0 682

/* ModelRuntimeNode.runtimeFlags bits */
#ifndef MODEL_RUNTIME_FLAG_APPLY_SCALE
#define MODEL_RUNTIME_FLAG_APPLY_SCALE 0x800 /* ModelRender_PrepareProjectedVertex scales every vertex by
                                                modelScaleQ12 */
#endif
/* Set by the model class initializers (world/model/slots); the model renderer shifts the texture coordinates
   of triangles using the node's primary / secondary animated subresource by its texture offsets */
#define MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL 0x80
#define MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL 0x400
/* Model triangle renderFlags (GraphicsTriangleInput.renderFlags), read by ModelRender_SubmitTriangle*,
   ModelRender_PrepareProjectedVertex* */
#define MODEL_TRIANGLE_PALETTE_BANK_MASK 0x1FF /* material colour: index into the node's palette asset */
/* The palette bank mask ModelRender_SubmitTriangleAlternatePath uses: keeps bits 16..31 too */
#define MODEL_TRIANGLE_PALETTE_BANK_WIDE_MASK 0xffff01ff
#define MODEL_TRIANGLE_UNLIT 0x200 /* vertices take the node tint instead of lighting */
#define MODEL_TRIANGLE_DOUBLE_SIDED 0x400 /* drawn without the back-face test */
#define MODEL_TRIANGLE_LIGHTING_SCALED 0x800 /* lit by ModelRender_ComputeVertexIntensityScaledPath */
#define MODEL_TRIANGLE_FLAT_SHADED 0x8000 /* lit with the triangle normal; never reuses a cached vertex colour */
#define MODEL_TRIANGLE_VERTEX_CACHE_FLAGS 0x8E00 /* the bits a projected vertex's cached colour was computed for */
/* Marker in a mesh vertex's projected X (GraphicsProjectedVertexSource.screenX): not transformed and projected yet for this draw */
#define MODEL_VERTEX_NOT_PROJECTED ((int)0x80000000)
/* Marker the alternate renderer stores in the projected X of a vertex in front of the near plane */
#define MODEL_VERTEX_NEAR_CLIPPED 0x7fffffff
/* Stride of the vertex and triangle records of a mesh (both 0x40 bytes; GraphicsTriangleInput uses 0x38) */
#define MODEL_MESH_RECORD_SIZE 0x40
/* Offset of the shadow pass's projected XY (a GraphicsFixedVec2) in a mesh vertex record (graphics/render/shading.c) */
#define MODEL_MESH_VERTEX_SHADOW_XY_OFFSET 0x20
/* ModelMeshGroupHeader.groupFlags bits */
#define MODEL_MESH_GROUP_FACE_VIEWER 1 /* ModelNodeRuntime_BuildViewFacingRotation before the draw */
#define MODEL_MESH_GROUP_BILLBOARD 2   /* ModelNodeRuntime_BuildBillboardRotation before the draw */
/* ModelMeshHeader.flags bit */
#define MODEL_MESH_SOFT_SHADOW 1 /* shadow silhouette drawn before the blur (graphics/render/shading.c); clear: after */

/* Header of a model mesh group (ModelMeshGroupAddress32 of ModelRender_DrawMeshGroups*); the meshes follow
   at +0x20, each starting with its byte size and group mask. */
typedef struct ModelMeshGroupHeader {
    uint32_t unknown00;
    int meshCount;           /* +0x04 */
    uint32_t unknown08;
    uint32_t groupFlags;     /* +0x0C MODEL_MESH_GROUP_* */
    uint8_t unknown10_1F[16];
} ModelMeshGroupHeader;
/* Header of one mesh of a group (ModelRender_SubmitMeshTriangles*); the vertices follow at +0x20, then the
   GraphicsTriangleInput records. */
typedef struct ModelMeshHeader {
    int byteSize;            /* +0x00 distance to the next mesh */
    uint32_t groupMask;      /* +0x04 */
    int vertexCount;         /* +0x08 */
    int triangleCount;       /* +0x0C */
    uint32_t flags;          /* +0x10 MODEL_MESH_SOFT_SHADOW */
    uint8_t unknown14_1F[12];
} ModelMeshHeader;
/* Functions are grouped by semantic ownership. */

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

Bool8 ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex);

void ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

void ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

int32_t ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle);

PackedArgb32 ModelRender_ComputeVertexIntensityDefaultPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12, GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

PackedArgb32 ModelRender_ComputeVertexIntensityScaledPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

PackedArgb32
ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
          (PackedArgb32 vertexPackedColor,GraphicsFixedVec3 *vertexPositionQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12);

void ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds);

void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime);

extern GraphicsPrimitiveQueue *g_ActivePrimitiveQueue;
extern GraphicsFixedRect g_ProjectionClipRect;
extern GraphicsShadingRuntimeRecord g_GraphicsShadingNearbyRecords[256];
extern GraphicsShadingRecordCount g_GraphicsShadingNearbyRecordCount; /* GraphicsShadingRecordCount (4 bytes, 0 in the image): number of valid g_GraphicsShadingNearbyRecords, set by GraphicsShadingRuntime_CollectNearbyRecords, read by the model vertex lighting. Followed by 8 bytes of 0x90 filler and g_ModelLightingMmxMultiplierRows. */
extern SoftwareBgraWordLanes g_ModelLightingMmxMultiplierRows[819]; /* PMULHW multipliers (alpha lane 0x4000) of the model vertex lighting; rows 0..135 = distance attenuation rows -136..-1 (B/G/R 0x007F at -1 rising by 0x80 to 0x3F7F, then 0x3FFF), rows 136..681 = distance attenuation from MODEL_DISTANCE_ATTENUATION_ROW0 (ModelRender_ComputeVertexIntensityDefaultPath, signed dot >> 21), rows 682..818 = scaled-lighting multipliers from MODEL_LIGHTING_SCALE_ROW0 (ModelRender_ComputeVertexIntensityScaledPath, signed (dot / lightingScaleQ12) >> 9; B/G/R 0x1FFF falling by 0x80 to 0x007F, then 0). See graphics/render/model.h. */
extern GraphicsFixedVec3 g_ModelLightingVertexToLightVectorScratch;
extern GraphicsFixedVec3 g_ModelLightingTransformedSurfaceNormalScratch;
extern GraphicsFixedVec3 g_GraphicsTransformInputScratchVec3;
extern GraphicsFixedVec3 g_GraphicsTransformOutputScratchVec3;

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_H */
