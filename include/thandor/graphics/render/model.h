/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/model.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_MODEL_H
#define THANDOR_GRAPHICS_RENDER_MODEL_H

#include <thandor/core/types.h>
#include <thandor/core/flags.h> /* THANDOR_FLAG_ENUM: ModelMeshGroupFlags, ModelMeshFlags */
#include <thandor/graphics/render/types.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/core/contracts.h>

/* Rows of g_ModelLightingMmxMultiplierRows (one table in the original). Both vertex
   lighting paths index it with a signed row relative to a base row, so negative indices read the rows before it. */
inline constexpr int MODEL_LIGHTING_MMX_ROW_COUNT = 819;
/* row 0 = distance attenuation row -136 (former g_ModelDistanceAttenuationMmxNegativeRows) */
inline constexpr int MODEL_DISTANCE_ATTENUATION_NEGATIVE_ROW0 = 0;
/* ModelRender_ComputeVertexIntensityDefaultPath's base, indexed by the light-facing dot >> 21 */
inline constexpr int MODEL_DISTANCE_ATTENUATION_ROW0 = 136;
/* ModelRender_ComputeVertexIntensityScaledPath's base, indexed by (dot / lightingScaleQ12) >> 9 */
inline constexpr int MODEL_LIGHTING_SCALE_ROW0 = 682;

/* ModelRuntimeNode.runtimeFlags bits */
inline constexpr int MODEL_RUNTIME_FLAG_APPLY_SCALE = 0x800; /* ModelRender_PrepareProjectedVertex scales every vertex by
                                                modelScaleQ12 */
/* Set by the model class initializers (world/model/slots); the model renderer shifts the texture coordinates
   of triangles using the node's primary / secondary animated subresource by its texture offsets */
inline constexpr int MODEL_RUNTIME_FLAG_PRIMARY_TEXTURE_SCROLL = 0x80;
inline constexpr int MODEL_RUNTIME_FLAG_SECONDARY_TEXTURE_SCROLL = 0x400;
/* Model triangle renderFlags (GraphicsTriangleInput.renderFlags): the MODEL_TRIANGLE_* enumerators of
   GraphicsPrimitiveDispatchFlags (graphics/render/types.h) */
/* Marker in a mesh vertex's projected X (GraphicsProjectedVertexSource.screenX): not transformed and projected yet for this draw */
inline constexpr int MODEL_VERTEX_NOT_PROJECTED = static_cast<int>(0x80000000);
/* Marker the alternate renderer stores in the projected X of a vertex in front of the near plane */
inline constexpr int MODEL_VERTEX_NEAR_CLIPPED = 0x7fffffff;
/* Stride of the vertex and triangle records of a mesh (both 0x40 bytes; GraphicsTriangleInput uses 0x38) */
inline constexpr int MODEL_MESH_RECORD_SIZE = 0x40;
/* Offset of the shadow pass's projected XY (a GraphicsFixedVec2) in a mesh vertex record (graphics/render/shadow_texture.cpp) */
inline constexpr int MODEL_MESH_VERTEX_SHADOW_XY_OFFSET = 0x20;
/* ModelMeshGroupHeader.groupFlags bits (MDL data; bits without an enumerator stay valid) */
enum class ModelMeshGroupFlags : uint32_t {
    MODEL_MESH_GROUP_FACE_VIEWER = 1, /* ModelNodeRuntime_BuildViewFacingRotation before the draw */
    MODEL_MESH_GROUP_BILLBOARD = 2 /* ModelNodeRuntime_BuildBillboardRotation before the draw */
};
THANDOR_FLAG_ENUM(ModelMeshGroupFlags);
using enum ModelMeshGroupFlags;
/* ModelMeshHeader.flags bits (MDL data) */
enum class ModelMeshFlags : uint32_t {
    MODEL_MESH_SOFT_SHADOW = 1 /* shadow silhouette drawn before the blur (graphics/render/shadow_texture.cpp); clear: after */
};
THANDOR_FLAG_ENUM(ModelMeshFlags);
using enum ModelMeshFlags;

/* Header of a model mesh group (ModelMeshGroupAddress32 of ModelRender_DrawMeshGroups*); the meshes follow
   at +0x20, each starting with its byte size and group mask. */
struct ModelMeshGroupHeader {
    uint32_t unknown00;
    int meshCount;           /* +0x04 */
    uint32_t unknown08;
    ModelMeshGroupFlags groupFlags; /* +0x0C MODEL_MESH_GROUP_* */
    uint8_t unknown10_1F[16];
};
/* Header of one mesh of a group (ModelRender_SubmitMeshTriangles*); the vertices follow at +0x20, then the
   GraphicsTriangleInput records. */
struct ModelMeshHeader {
    int byteSize;            /* +0x00 distance to the next mesh */
    uint32_t groupMask;      /* +0x04 */
    int vertexCount;         /* +0x08 */
    int triangleCount;       /* +0x0C */
    ModelMeshFlags flags;    /* +0x10 MODEL_MESH_SOFT_SHADOW */
    uint8_t unknown14_1F[12];
};

/* A mesh vertex record (MODEL_MESH_RECORD_SIZE bytes, no struct of its own) is walked as GraphicsFixedVec3 words:
   [0] local position, +0x10 normal, +0x1C packed colour, +0x20 view position (the shadow pass stores its projected
   XY there), +0x2C lit colour, +0x30/+0x34 projected X/Y, +0x38 lighting flags. These give the members that do
   not start on a GraphicsFixedVec3 boundary. */
static inline GraphicsFixedVec3 *ModelVertex_Words(GraphicsProjectedVertexSource *vertex)
{
    return reinterpret_cast<GraphicsFixedVec3 *>(vertex); /* the same vertex record, as the words above */
}
static inline GraphicsFixedVec3 *ModelVertex_Normal(GraphicsFixedVec3 *vertex)
{
    return reinterpret_cast<GraphicsFixedVec3 *>(&vertex[1].y);
}
static inline GraphicsFixedVec3 *ModelVertex_ViewPosition(GraphicsFixedVec3 *vertex)
{
    return reinterpret_cast<GraphicsFixedVec3 *>(&vertex[2].z);
}
static inline GraphicsFixedVec2 *ModelVertex_ShadowXY(GraphicsFixedVec3 *vertex)
{
    return reinterpret_cast<GraphicsFixedVec2 *>(&vertex[2].z);
}

/* The triangle's plane normal (planeNormalX/Y/ZQ12) as one vector. */
static inline GraphicsFixedVec3 *ModelTriangle_PlaneNormal(GraphicsTriangleInput *triangle)
{
    return reinterpret_cast<GraphicsFixedVec3 *>(&triangle->planeNormalXQ12);
}

PackedArgb32 ModelRender_ComputeVertexIntensityDefaultPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12, GraphicsDistanceAttenuationTableAddress32 distanceAttenuationTable, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

PackedArgb32 ModelRender_ComputeVertexIntensityScaledPath (PackedArgb32 vertexPackedColor,int *vertexPositionQ12,Q12 lightingScaleQ12, PackedArgb32 scenePackedColor0,PackedArgb32 scenePackedColor1, GraphicsFixedVec3 *lightDirectionQ12,PackedArgb32 materialPackedColor, GraphicsFixedVec3 *surfaceNormalQ12);

PackedArgb32
ModelRender_ComputeNearbyLightPackedVertexColorAlternatePath
          (PackedArgb32 vertexPackedColor,GraphicsFixedVec3 *vertexPositionQ12,
          PackedArgb32 scenePackedColor0,PackedArgb32 materialPackedColor,
          GraphicsFixedVec3 *surfaceNormalQ12);

extern SoftwareBgraWordLanes g_ModelLightingMmxMultiplierRows[819]; /* PMULHW multipliers (alpha lane 0x4000) of the model vertex lighting; rows 0..135 = distance attenuation rows -136..-1 (B/G/R 0x007F at -1 rising by 0x80 to 0x3F7F, then 0x3FFF), rows 136..681 = distance attenuation from MODEL_DISTANCE_ATTENUATION_ROW0 (ModelRender_ComputeVertexIntensityDefaultPath, signed dot >> 21), rows 682..818 = scaled-lighting multipliers from MODEL_LIGHTING_SCALE_ROW0 (ModelRender_ComputeVertexIntensityScaledPath, signed (dot / lightingScaleQ12) >> 9; B/G/R 0x1FFF falling by 0x80 to 0x007F, then 0). See graphics/render/model.h. */
extern GraphicsFixedVec3 g_ModelLightingVertexToLightVectorScratch;
extern GraphicsFixedVec3 g_ModelLightingTransformedSurfaceNormalScratch;

#endif /* THANDOR_GRAPHICS_RENDER_MODEL_H */
