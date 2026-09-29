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

/* ModelRuntimeNode.runtimeFlags bits */
#ifndef MODEL_RUNTIME_FLAG_APPLY_SCALE
#define MODEL_RUNTIME_FLAG_APPLY_SCALE 0x800 /* ModelRender_PrepareProjectedVertex scales every vertex by
                                                modelScaleQ12 */
#endif
/* Set by the model class initializers (world/model/slots); the model renderer shifts the texture coordinates
   of triangles using the node's primary / secondary animated subresource by its texture offsets */
#define MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL 0x80
#define MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL 0x400
/* Model triangle renderFlags (triangle record +0x34), read by ModelRender_SubmitTriangle*,
   ModelRender_PrepareProjectedVertex* */
#define MODEL_TRIANGLE_PALETTE_BANK_MASK 0x1FF /* material colour: index into the node's palette asset */
#define MODEL_TRIANGLE_UNLIT 0x200 /* vertices take the node tint instead of lighting */
#define MODEL_TRIANGLE_DOUBLE_SIDED 0x400 /* drawn without the back-face test */
#define MODEL_TRIANGLE_LIGHTING_SCALED 0x800 /* lit by ModelRender_ComputeVertexIntensityScaledPath */
#define MODEL_TRIANGLE_FLAT_SHADED 0x8000 /* lit with the triangle normal; never reuses a cached vertex colour */
#define MODEL_TRIANGLE_VERTEX_CACHE_FLAGS 0x8E00 /* the bits a projected vertex's cached colour was computed for */
/* Marker in a mesh vertex's projected X (+0x30): not transformed and projected yet for this draw */
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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BDC90 */
void ModelRender_DrawMeshGroupsWithTemporaryTransform
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE1F0 */
void ModelRender_DrawMeshGroupsAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x0050A5C0 */
void ModelProjectedBounds_AccumulateHierarchyRecursive(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

/* 0x004BD4B0 */
void ModelRender_PrepareProjectedVertex
          (ModelRuntimeNode *modelNode,ModelMeshGroupAddress32 triangle,GraphicsFixedVec3 *vertex);

/* 0x004BD9B0 */
void ModelRender_SubmitTriangle(Q12 facingThresholdQ12,GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

/* 0x004BDC20 */
void ModelRender_SubmitMeshTriangles
          (Q12 facingThresholdQ12,ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BE180 */
void ModelRender_SubmitMeshTrianglesAlternatePath(ModelMeshGroupAddress32 meshGroup,ModelRuntimeNode *modelNode);

/* 0x004BD6B0 */
bool ModelRender_PrepareProjectedVertexAlternatePath
          (ModelRuntimeNode *modelNode,GraphicsTriangleInput *triangle,GraphicsFixedVec3 *vertex);

/* 0x004BDFB0 */
void ModelRender_SubmitTriangleAlternatePath(GraphicsTriangleInput *triangle,ModelRuntimeNode *modelNode);

/* 0x0050A4A0 */
void ModelProjectedBounds_AccumulateNode(ModelProjectedBoundsPixels *bounds,ModelRuntimeNode *modelNode);

/* 0x004BD7E0 */
int32_t ModelRender_ComputeFacingDotQ12(GraphicsTriangleInput *triangle);

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
void ModelProjectedBounds_ExpandWithCurrentScratchPoint(ModelProjectedBoundsPixels *bounds);

/* 0x004BD800 */
void ModelRender_PrepareViewDirections(ModelRuntimeNode *modelNodeRuntime);

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_H */
