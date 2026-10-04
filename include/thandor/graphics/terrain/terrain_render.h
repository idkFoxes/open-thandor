/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/terrain/terrain_render.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_TERRAIN_TERRAIN_RENDER_H
#define THANDOR_GRAPHICS_TERRAIN_TERRAIN_RENDER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/terrain/terrain_render. */

/* FieldGridCell.flagsAndMaterial bits of TerrainProjectedVertex_TransformProjectAndShade (a field cell is
   also its TerrainProjectedVertexWorkRecord): set when point A (projectedPointA, the terrain point) or point B
   (projectedPointB, the offset secondary point) is not beyond the near plane and got no screen position. */
#define TERRAIN_VERTEX_POINT_A_NOT_PROJECTED 0x200000
#define TERRAIN_VERTEX_POINT_B_NOT_PROJECTED 0x4000000
/* The other projectionFlags bits of the terrain vertex pass: the low byte is the soil material (0xFF = no
   terrain at this vertex, nothing is drawn); per projected point one bit per side of g_ProjectionClipRect the
   point lies on the inner side of (x >= minX, y >= minY, x < maxX, y < maxY), so the OR over a triangle's
   vertices tells whether its screen bounding box can overlap the clip rectangle; SECONDARY_VISIBLE is set on
   the vertices of a queued secondary-surface triangle and makes TerrainProjectedVertex_ReshadeKeepingProjection
   project point B again. */
#define TERRAIN_VERTEX_MATERIAL_MASK 0xff
#define TERRAIN_VERTEX_MATERIAL_NONE 0xff
#define TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X 0x20000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MIN_Y 0x40000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MAX_X 0x80000
#define TERRAIN_VERTEX_POINT_A_INSIDE_MAX_Y 0x100000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X 0x400000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MIN_Y 0x800000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MAX_X 0x1000000
#define TERRAIN_VERTEX_POINT_B_INSIDE_MAX_Y 0x2000000
#define TERRAIN_VERTEX_SECONDARY_VISIBLE 0x10000000
/* The four side bits of a point, and those plus its NOT_PROJECTED bit (all five bits of the point). */
#define TERRAIN_VERTEX_POINT_A_SIDE_BITS                                                                   \
  (TERRAIN_VERTEX_POINT_A_INSIDE_MIN_X | TERRAIN_VERTEX_POINT_A_INSIDE_MIN_Y | TERRAIN_VERTEX_POINT_A_INSIDE_MAX_X | \
   TERRAIN_VERTEX_POINT_A_INSIDE_MAX_Y) /* 0x1e0000 */
#define TERRAIN_VERTEX_POINT_A_BITS (TERRAIN_VERTEX_POINT_A_SIDE_BITS | TERRAIN_VERTEX_POINT_A_NOT_PROJECTED)
#define TERRAIN_VERTEX_POINT_B_SIDE_BITS                                                                   \
  (TERRAIN_VERTEX_POINT_B_INSIDE_MIN_X | TERRAIN_VERTEX_POINT_B_INSIDE_MIN_Y | TERRAIN_VERTEX_POINT_B_INSIDE_MAX_X | \
   TERRAIN_VERTEX_POINT_B_INSIDE_MAX_Y) /* 0x3c00000 */
#define TERRAIN_VERTEX_POINT_B_BITS (TERRAIN_VERTEX_POINT_B_SIDE_BITS | TERRAIN_VERTEX_POINT_B_NOT_PROJECTED)
/* Render context flag of TerrainProjectedGrid_TransformShadeAndQueue. The context is the world runtime and this is
   the bit named WORLD_RUNTIME_FLAG_FIELD_GRID_DIRTY there: FrontendModelPointerContext_RenderWorldViewQueuesClipped
   sets it after a frame that covered the whole view, every camera change clears it. While it is set (and the field
   grid is unchanged) the row spans and point-A projections of the previous frame are reused. */
#define TERRAIN_RENDER_REUSE_PROJECTION 0x800
/* Soil packet table (g_TerrainSoilPacketTablePayload) of TerrainProjectedTriangle_ClipInterpolateAndQueueTextured: one block per
   material (projectionFlags bits 0..7), inside it one row per variant (projectionFlags bits 8..10, already the byte
   offset), inside that 0x20-byte packets: +0 the plain triangle, +0x20..+0xA0 the blend packets towards the
   other materials. The secondary surface table uses the same packet size. */
#define TERRAIN_SOIL_PACKET_MATERIAL_BYTES 0x800
#define TERRAIN_VERTEX_VARIANT_OFFSET_MASK 0x700 /* projectionFlags bits 8..10: variant row offset (0x100 each) */
#define TERRAIN_SURFACE_PACKET_BYTES 0x20
/* renderFlags of the queued material blend packets: translucent, and a sort layer bit (28 or 29,
   GRAPHICS_PRIMITIVE_SORT_KEY_FLAG_BITS) that lowers the sort key so they are drawn after the plain triangle;
   the second blend packet of a three-material triangle uses the higher layer */
#define TERRAIN_BLEND_PACKET_SORT_LAYER_1 0x10000000
#define TERRAIN_BLEND_PACKET_SORT_LAYER_2 0x20000000
#define TERRAIN_BLEND_PACKET_FIRST_LAYER_FLAGS \
  (GRAPHICS_PRIMITIVE_FLAG_FORCE_TRANSLUCENT | TERRAIN_BLEND_PACKET_SORT_LAYER_1)
#define TERRAIN_BLEND_PACKET_SECOND_LAYER_FLAGS \
  (GRAPHICS_PRIMITIVE_FLAG_FORCE_TRANSLUCENT | TERRAIN_BLEND_PACKET_SORT_LAYER_2)
/* Functions are grouped by semantic ownership. */

void TerrainProjectedGrid_TransformShadeAndQueue
          (FieldGridAsset *fieldGrid,FrontendModelPointerContext *renderContext);

void TerrainProjectedQuad_QueueAsTwoTriangles
          (uint32_t rowStrideBytes,TerrainProjectedVertexWorkRecord *topLeftVertex,
          FrontendModelPointerContext *renderContext);

void TerrainProjectedVertex_TransformProjectAndShade(TerrainProjectedVertexWorkRecord *vertex);

void TerrainProjectedVertex_ReshadeKeepingProjection(TerrainProjectedVertexWorkRecord *vertex);

void TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
          (int surfacePacketIndex,TerrainProjectedVertexWorkRecord *vertex2,
          TerrainProjectedVertexWorkRecord *vertex1,TerrainProjectedVertexWorkRecord *vertex0,
          FrontendModelPointerContext *renderContext);

void TerrainProjectedGrid_ClipRowSpansAgainstPlane(FieldGridAsset *fieldGrid,GraphicsFixedVec3 *planeNormal);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContext *renderContext);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_AppendTerrainTexturedTriangle
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContext *renderContext);

/* Weight 1.0 of Triangle2D_ComputeBarycentricWeightsQ12Packed's internal Q16 weights (returned >> 4 as Q12) */
#define TRIANGLE_BARYCENTRIC_WEIGHT_ONE_Q16 0x10000

TriangleBarycentricWeightsQ12
Triangle2D_ComputeBarycentricWeightsQ12Packed
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX);

/* Second result of Triangle2D_ComputeBarycentricWeightsQ12Packed: the point is outside the triangle. */
extern Bool8 g_Triangle2DBarycentricOutside;

#endif /* THANDOR_GRAPHICS_TERRAIN_TERRAIN_RENDER_H */
