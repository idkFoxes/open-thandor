/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/primitives.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_PRIMITIVES_H
#define THANDOR_GRAPHICS_RENDER_PRIMITIVES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/primitives. */

/* DepthInterval_BuildBinMask: world coordinates (Q12) are hashed into 32 wrapping bins of 1 << 14 units
   (4.0 world units) per axis */
#define SPATIAL_BIN_SHIFT 14

/* Primitive queue pool layout (GraphicsPrimitiveQueue_AllocateGlobalPool/ResetGlobal): a 0x20-byte header, then
   per packet one primary node and one radix scratch node (0x10 bytes each) and the 0x80-byte packet itself. */
#define GRAPHICS_PRIMITIVE_QUEUE_HEADER_BYTES 0x20
#define GRAPHICS_PRIMITIVE_QUEUE_BYTES_PER_PACKET 0xA0
/* GraphicsPrimitiveQueue_RadixSortForRendering ends the sorted traversal list with this node pointer */
#define GRAPHICS_PRIMITIVE_QUEUE_END_NODE ((GraphicsPrimitiveQueueNode *)(intptr_t)-1) /* 0xffffffff in the original */
/* GraphicsPrimitivePacket.renderFlags: bits 12..17 select the raster handler ((flags & 0x3f000) >> 12). Bit 16
   marks a textured packet, bits 12..14 the blend mode. */
#define GRAPHICS_PRIMITIVE_FLAG_TEXTURED 0x10000
#define GRAPHICS_PRIMITIVE_FLAG_FORCE_TRANSLUCENT 0x20000
/* bits 12..17: raster handler index; the software queue renderers shift by 10 to get its byte offset */
#define GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK 0x3f000
/* set by SoftwareRenderer_PrepareTrianglePacket when all three vertex colours are equal */
#define GRAPHICS_PRIMITIVE_FLAG_FLAT_SHADED 0x8000
#define GRAPHICS_PRIMITIVE_BLEND_MASK 0x7000
#define GRAPHICS_PRIMITIVE_BLEND_OPAQUE 0
#define GRAPHICS_PRIMITIVE_BLEND_TRANSLUCENT 0x1000
#define GRAPHICS_PRIMITIVE_BLEND_ADDITIVE 0x2000
#define GRAPHICS_PRIMITIVE_BLEND_MODE_4 0x4000 /* kept by GraphicsPrimitiveQueue_SetVertexColors like opaque */
#define GRAPHICS_PRIMITIVE_BLEND_ALPHA_DEPTH_WRITE 0x6000 /* blend mode 6: alpha-blended with depth writes */
/* GraphicsPrimitiveQueue_Sort keys: an opaque packet's key is its texture entry | 0xB0000000 minus render flag
   bits 28..29 (0x80000000 and above), a blended packet's the sum of its vertex depths below 0x80000000 */
#define GRAPHICS_PRIMITIVE_SORT_KEY_OPAQUE_BASE 0xb0000000
#define GRAPHICS_PRIMITIVE_SORT_KEY_FLAG_BITS 0x30000000
#define GRAPHICS_PRIMITIVE_SORT_KEY_DEPTH_MASK 0x7fffffff
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void GraphicsPrimitiveQueue_RadixSortForRendering(GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue);

uint32_t GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity);

GraphicsPrimitiveQueue *GraphicsPrimitiveQueue_ResetGlobal(void);

uint32_t GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue);

bool GraphicsPrimitiveQueue_AppendTriangle(GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle,
          GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1,
          GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_SetVertexColors
          (PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color,
          GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_SetMaterial(PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry,
          GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_OffsetTextureCoordinates(GraphicsPrimitiveTextureCoordinateFixed deltaV,
          GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue);

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

DepthBinMask32 DepthInterval_BuildBinMask(DepthIntervalRadius32 radiusQ12,DepthIntervalCenter32 centerQ12);

bool DepthBinMasks_Overlap(DepthBinMask32 firstMaskAxis0,DepthBinMask32 firstMaskAxis1,DepthBinMask32 secondMaskAxis0,
          DepthBinMask32 secondMaskAxis1);

#endif /* THANDOR_GRAPHICS_RENDER_PRIMITIVES_H */
