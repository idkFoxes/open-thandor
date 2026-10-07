/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/primitives.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_PRIMITIVES_H
#define THANDOR_GRAPHICS_RENDER_PRIMITIVES_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/core/contracts.h>

/* DepthInterval_BuildBinMask: world coordinates (Q12) are hashed into 32 wrapping bins of 1 << 14 units
   (4.0 world units) per axis */
inline constexpr int SPATIAL_BIN_SHIFT = 14;

/* Primitive queue pool layout (GraphicsPrimitiveQueue_AllocateGlobalPool/ResetGlobal): a 0x20-byte header, then
   per packet one primary node and one radix scratch node (0x10 bytes each) and the 0x80-byte packet itself
   (on x64 the header and nodes are wider; the packet stays 0x80 bytes). */
inline constexpr uint32_t GRAPHICS_PRIMITIVE_QUEUE_HEADER_BYTES = offsetof(GraphicsPrimitiveQueue, primaryNodes);
inline constexpr uint32_t GRAPHICS_PRIMITIVE_QUEUE_BYTES_PER_PACKET =
    2 * sizeof(GraphicsPrimitiveQueueNode) + sizeof(GraphicsPrimitivePacket);
/* GraphicsPrimitiveQueue_RadixSortForRendering ends the sorted traversal list with this node pointer */
#define GRAPHICS_PRIMITIVE_QUEUE_END_NODE (Thandor_U32ToPointer<GraphicsPrimitiveQueueNode>(-1)) /* 0xffffffff in the original */
/* GraphicsPrimitivePacket.renderFlags: GraphicsPrimitiveDispatchFlags (graphics/render/types.h). */
/* GraphicsPrimitiveQueue_Sort keys: an opaque packet's key is its texture entry | 0xB0000000 minus render flag
   bits 28..29 (GRAPHICS_PRIMITIVE_SORT_KEY_FLAG_BITS; 0x80000000 and above), a blended packet's the sum of its
   vertex depths below 0x80000000 */
inline constexpr uint32_t GRAPHICS_PRIMITIVE_SORT_KEY_OPAQUE_BASE = 0xb0000000;
inline constexpr int GRAPHICS_PRIMITIVE_SORT_KEY_DEPTH_MASK = 0x7fffffff;

void GraphicsPrimitiveQueue_RadixSortForRendering(GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue);

uint32_t GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity);

GraphicsPrimitiveQueue *GraphicsPrimitiveQueue_ResetGlobal();

uint32_t GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue);

GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue);

bool GraphicsPrimitiveQueue_AppendTriangle(GraphicsPrimitiveDispatchFlags renderFlags,GraphicsTriangleInput *triangle,
          GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1,
          GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_SetVertexColors
          (PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color,
          GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_SetMaterial(PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry,
          GraphicsPrimitiveQueue *queue);

void GraphicsPrimitiveQueue_OffsetTextureCoordinates(GraphicsPrimitiveTextureCoordinateFixed deltaV,
          GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue);

DepthBinMask32 DepthInterval_BuildBinMask(DepthIntervalRadius32 radiusQ12,DepthIntervalCenter32 centerQ12);

bool DepthBinMasks_Overlap(DepthBinMask32 firstMaskAxis0,DepthBinMask32 firstMaskAxis1,DepthBinMask32 secondMaskAxis0,
          DepthBinMask32 secondMaskAxis1);

extern GraphicsPrimitiveQueueRadixSortProc *g_GraphicsPrimitiveQueueRadixSortProc;
extern GraphicsPrimitiveQueue *g_PrimitiveQueueStorage;

#endif /* THANDOR_GRAPHICS_RENDER_PRIMITIVES_H */
