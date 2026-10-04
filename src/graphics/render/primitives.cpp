/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/primitives.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/primitives.h>
#include <thandor/thandor.h>

/* Module data. */

GraphicsPrimitiveQueueRadixSortProc *g_GraphicsPrimitiveQueueRadixSortProc = THANDOR_FN(GraphicsPrimitiveQueue_RadixSortForRendering);

GraphicsPrimitiveQueue *g_PrimitiveQueueStorage = 0;

static const uint64_t g_VertexColorAlphaPreserveMaskMMX = 0xFF000000ull;

static const uint64_t g_VertexColorRgbHalveMaskMMX = 0xFEFEFEull;

static uint32_t g_PrimitiveRadixBucketWords[256] = {0};

static uint32_t g_PrimitiveQueuePoolCapacity = 0;

/* Implementation ownership: graphics/render/primitives. */

/* Sort key of one queued packet for GraphicsPrimitiveQueue_RadixSortForRendering. */
static uint32_t GraphicsPrimitiveQueue_RenderSortKey(const GraphicsPrimitivePacket *packet)
{
  if ((packet->renderFlags & GRAPHICS_PRIMITIVE_BLEND_MASK) == GRAPHICS_PRIMITIVE_BLEND_OPAQUE) {
    /* the texture entry address (its low 32 bits on x64) groups the opaque packets by texture */
    return ((uint32_t)(uintptr_t)packet->textureEntry | GRAPHICS_PRIMITIVE_SORT_KEY_OPAQUE_BASE) -
           (packet->renderFlags & GRAPHICS_PRIMITIVE_SORT_KEY_FLAG_BITS);
  }
  return (packet->vertices[0].depth + packet->vertices[1].depth + packet->vertices[2].depth) &
         GRAPHICS_PRIMITIVE_SORT_KEY_DEPTH_MASK;
}

/* One stable radix pass of GraphicsPrimitiveQueue_RadixSortForRendering over the key byte at keyShift: counts
   the keys per bucket in g_PrimitiveRadixBucketWords, turns the counts into write cursors into destination
   (bucket 0xFF first, so the order is descending) and copies sortKey and packet of every source node, in source
   order, to its bucket's next slot. The links of the nodes are not copied. */
static void GraphicsPrimitiveQueue_RadixPass(const GraphicsPrimitiveQueueNode *source,
                                             GraphicsPrimitiveQueueNode *destination,uint32_t nodeCount,
                                             int keyShift)
{
  GraphicsPrimitiveRadixBucket *buckets;
  GraphicsPrimitiveQueueNode *bucketStart;
  GraphicsPrimitiveQueueNode *slot;
  uint32_t bucketCount;
  uint32_t nodeIndex;
  int bucketIndex;

  buckets = (GraphicsPrimitiveRadixBucket *)g_PrimitiveRadixBucketWords;
  for (bucketIndex = 0; bucketIndex < 256; bucketIndex++) {
    buckets[bucketIndex].count = 0;
  }
  for (nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
    buckets[(source[nodeIndex].sortKey >> keyShift) & 0xff].count++;
  }
  bucketStart = destination;
  for (bucketIndex = 255; bucketIndex >= 0; bucketIndex--) {
    bucketCount = buckets[bucketIndex].count;
    buckets[bucketIndex].writeCursor = bucketStart;
    bucketStart = bucketStart + bucketCount;
  }
  for (nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
    slot = buckets[(source[nodeIndex].sortKey >> keyShift) & 0xff].writeCursor++;
    slot->sortKey = source[nodeIndex].sortKey;
    slot->packet = source[nodeIndex].packet;
  }
}

/* Halves the RGB of the packet's three vertex colours and keeps their alpha (MMX). */
static void GraphicsPrimitivePacket_HalveVertexRgb(GraphicsPrimitivePacket *packet)
{
  uint64_t vertex0HalvedColor;
  uint64_t vertex1HalvedColor;
  uint64_t vertex2HalvedColor;

  vertex0HalvedColor =
       paddusb((packet->vertices[0].diffuseColor & g_VertexColorRgbHalveMaskMMX) >> 1,
               packet->vertices[0].diffuseColor & g_VertexColorAlphaPreserveMaskMMX);
  vertex1HalvedColor =
       paddusb((packet->vertices[1].diffuseColor & g_VertexColorRgbHalveMaskMMX) >> 1,
               packet->vertices[1].diffuseColor & g_VertexColorAlphaPreserveMaskMMX);
  vertex2HalvedColor =
       paddusb((packet->vertices[2].diffuseColor & g_VertexColorRgbHalveMaskMMX) >> 1,
               packet->vertices[2].diffuseColor & g_VertexColorAlphaPreserveMaskMMX);
  packet->vertices[0].diffuseColor = (int)vertex0HalvedColor;
  packet->vertices[1].diffuseColor = (int)vertex1HalvedColor;
  packet->vertices[2].diffuseColor = (int)vertex2HalvedColor;
}

/* Sorts a filled primitive queue for drawing and links the sorted nodes into the traversal list read by
   GraphicsPrimitiveQueue_Begin/Next. Each node gets a 32-bit key: blended packets (any blend-mode bit) the sum
   of their three vertex depths (below 0x80000000), opaque packets their texture entry with bits 0x30000000 of
   the render flags subtracted from 0xB0000000 (0x80000000 and above). Four stable byte-wise radix passes (low
   byte first) move the nodes between primaryNodes and radixScratchPool and back and order them by descending
   key: opaque packets first, grouped by texture, then the blended ones from the largest depth sum down. With
   halveVertexRgb set every packet's vertex RGB is halved (alpha kept, MMX).
   Installed in the graphics dispatch slot g_GraphicsPrimitiveQueueRadixSortProc (called
   by FrontendModelPointerContext_RenderWorldViewQueuesClipped with node flag 8) and called directly by the
   offscreen model renderer (graphics/render/projection.c).
*/
void GraphicsPrimitiveQueue_RadixSortForRendering(GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue)

{
  uint32_t nodeCount;
  uint32_t nodeIndex;
  GraphicsPrimitiveQueueNode *primaryNodes;
  GraphicsPrimitiveQueueNode *scratchNodes;
  GraphicsPrimitiveQueueNode *node;
  GraphicsPrimitiveQueueNode *previousNode;

  nodeCount = queue->count;
  if (nodeCount == 0) {
    return;
  }
  primaryNodes = queue->primaryNodes;
  scratchNodes = queue->radixScratchPool;
  if (nodeCount != 1) {
    for (nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
      primaryNodes[nodeIndex].sortKey = GraphicsPrimitiveQueue_RenderSortKey(primaryNodes[nodeIndex].packet);
    }
    /* key byte 0 to the scratch pool, byte 1 back, byte 2 to the scratch pool, byte 3 back to primaryNodes */
    GraphicsPrimitiveQueue_RadixPass(primaryNodes,scratchNodes,nodeCount,0);
    GraphicsPrimitiveQueue_RadixPass(scratchNodes,primaryNodes,nodeCount,8);
    GraphicsPrimitiveQueue_RadixPass(primaryNodes,scratchNodes,nodeCount,16);
    GraphicsPrimitiveQueue_RadixPass(scratchNodes,primaryNodes,nodeCount,24);
  }
  /* Link the sorted primaryNodes in array order */
  queue->traversalCursor = primaryNodes;
  previousNode = GRAPHICS_PRIMITIVE_QUEUE_END_NODE;
  for (nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
    node = &primaryNodes[nodeIndex];
    node->previous = previousNode;
    node->next = node + 1;
    if (halveVertexRgb != GRAPHICS_STATE_DISABLED) {
      GraphicsPrimitivePacket_HalveVertexRgb(node->packet);
    }
    previousNode = node;
  }
  previousNode->next = GRAPHICS_PRIMITIVE_QUEUE_END_NODE;
}


/* Allocates the global primitive queue pool for packetCapacity packets (header, two nodes and one packet
   each) and remembers the capacity for GraphicsPrimitiveQueue_ResetGlobal. Returns 0 on success (the pool is
   then in g_PrimitiveQueueStorage), or the arena error when the allocation fails; g_PrimitiveQueueStorage is
   then left unchanged.
*/
uint32_t GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity)

{
  GraphicsPrimitiveQueue *allocatedQueueStorage;
  uint32_t allocError;

  g_PrimitiveQueuePoolCapacity = packetCapacity;
  allocError = g_MemoryApi.alloc(packetCapacity * GRAPHICS_PRIMITIVE_QUEUE_BYTES_PER_PACKET +
                                 GRAPHICS_PRIMITIVE_QUEUE_HEADER_BYTES,(void **)&allocatedQueueStorage);
  if (allocError != 0) {
    return allocError;
  }
  g_PrimitiveQueueStorage = allocatedQueueStorage;
  return 0;
}


/* Empties the global primitive queue (g_PrimitiveQueueStorage) for a new frame and lays out its pool:
   primaryNodes right after the 0x20-byte header, then radixScratchPool, then the packets, each part sized for
   the capacity given to GraphicsPrimitiveQueue_AllocateGlobalPool. Never fails; returns the queue.
   Called by the frontend 3D views (ui/frontend/runtime.c) and the offscreen model renderer.
*/
GraphicsPrimitiveQueue *GraphicsPrimitiveQueue_ResetGlobal(void)

{
  GraphicsPrimitiveQueue *globalQueue;
  uint32_t poolCapacity;
  
  poolCapacity = g_PrimitiveQueuePoolCapacity;
  globalQueue = g_PrimitiveQueueStorage;
  /* capacity is stored through the global again, as in the original */
  g_PrimitiveQueueStorage->capacity = g_PrimitiveQueuePoolCapacity;
  globalQueue->count = 0;
  globalQueue->radixScratchPool = globalQueue->primaryNodes + poolCapacity;
  globalQueue->packetPool =
       (GraphicsPrimitivePacket *)(globalQueue->primaryNodes + poolCapacity + poolCapacity);
  return globalQueue;
}


/* Returns the number of packets queued in queue. Used by FrontendModelPointerContext_RenderWorldViewQueuesClipped
   (ui/frontend/runtime.c) after each drawn pass.
*/
uint32_t GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue)

{
  return queue->count;
}


/* Starts walking a sorted primitive queue: returns the packet of the node at traversalCursor (set by
   GraphicsPrimitiveQueue_RadixSortForRendering) and advances the cursor to the next node. Returns NULL when
   the queue is empty (queued packets are never NULL); GraphicsPrimitiveQueue_Next continues the walk.
*/
GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *currentTraversalPacket;

  if (queue->count != 0) {
    currentTraversalPacket = queue->traversalCursor->packet;
    queue->traversalCursor = queue->traversalCursor->next;
    return currentTraversalPacket;
  }
  return NULL;
}


/* Continues a walk begun by GraphicsPrimitiveQueue_Begin: returns the packet of the node at traversalCursor
   and advances the cursor. Returns NULL once the cursor reaches GRAPHICS_PRIMITIVE_QUEUE_END_NODE.
*/
GraphicsPrimitivePacket *GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveQueueNode *currentTraversalNode;
  GraphicsPrimitivePacket *currentTraversalPacket;

  currentTraversalNode = queue->traversalCursor;
  if (currentTraversalNode != GRAPHICS_PRIMITIVE_QUEUE_END_NODE) {
    currentTraversalPacket = currentTraversalNode->packet;
    queue->traversalCursor = currentTraversalNode->next;
    return currentTraversalPacket;
  }
  return NULL;
}


/* Appends a triangle packet for the model renderer: copies screen position, backend coordinates and depth of
   the three projected vertices and the texture coordinates from triangle, and sets renderFlags. The colours,
   material and texture are filled in afterwards by GraphicsPrimitiveQueue_SetVertexColors/SetMaterial. Returns
   true when the queue is full; one slot is always left unused. Called by ModelRender_SubmitTriangle and
   ModelRender_PrepareProjectedVertexAlternatePath (graphics/render/model.c).
*/
Bool8 GraphicsPrimitiveQueue_AppendTriangle(GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle,
          GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1,
          GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveScreenCoordinate vertex1Or2ScreenY;
  GraphicsPrimitiveBackendCoordinate vertex1Or2BackendCoord1;
  GraphicsPrimitiveTextureCoordinateFixed vertex1TextureV;
  GraphicsPrimitiveTextureCoordinateFixed vertex2TextureV;
  GraphicsPrimitivePacket *destinationPacket;
  uint32_t destinationPacketIndex;
  GraphicsPrimitiveScreenCoordinate vertex0ScreenY;
  GraphicsPrimitiveBackendCoordinate vertex0BackendCoord1;
  GraphicsPrimitiveDepthFixed vertex1Depth;
  GraphicsPrimitiveDepthFixed vertex2Depth;
  GraphicsPrimitiveTextureCoordinateFixed vertex1TextureU;
  GraphicsPrimitiveTextureCoordinateFixed vertex2TextureU;
  
  destinationPacketIndex = queue->count;
  if (destinationPacketIndex + 1 < queue->capacity) {
    queue->count = destinationPacketIndex + 1;
    destinationPacket = queue->packetPool + destinationPacketIndex;
    queue->primaryNodes[destinationPacketIndex].packet = destinationPacket;
    vertex0ScreenY = vertex0->screenY;
    destinationPacket->vertices[0].screenX = vertex0->screenX;
    destinationPacket->vertices[0].screenY = vertex0ScreenY;
    vertex1Or2ScreenY = vertex1->screenY;
    destinationPacket->vertices[1].screenX = vertex1->screenX;
    destinationPacket->vertices[1].screenY = vertex1Or2ScreenY;
    vertex1Or2ScreenY = vertex2->screenY;
    destinationPacket->vertices[2].screenX = vertex2->screenX;
    destinationPacket->vertices[2].screenY = vertex1Or2ScreenY;
    vertex0BackendCoord1 = vertex0->backendCoord1;
    destinationPacket->vertices[0].backendCoord0 = vertex0->backendCoord0;
    destinationPacket->vertices[0].backendCoord1 = vertex0BackendCoord1;
    vertex1Or2BackendCoord1 = vertex1->backendCoord1;
    destinationPacket->vertices[1].backendCoord0 = vertex1->backendCoord0;
    destinationPacket->vertices[1].backendCoord1 = vertex1Or2BackendCoord1;
    vertex1Or2BackendCoord1 = vertex2->backendCoord1;
    destinationPacket->vertices[2].backendCoord0 = vertex2->backendCoord0;
    destinationPacket->vertices[2].backendCoord1 = vertex1Or2BackendCoord1;
    vertex1Depth = vertex1->depth;
    vertex2Depth = vertex2->depth;
    destinationPacket->vertices[0].depth = vertex0->depth;
    destinationPacket->vertices[1].depth = vertex1Depth;
    destinationPacket->vertices[2].depth = vertex2Depth;
    vertex1TextureU = triangle->textureU1;
    vertex2TextureU = triangle->textureU2;
    destinationPacket->vertices[0].textureU = triangle->textureU0;
    destinationPacket->vertices[1].textureU = vertex1TextureU;
    destinationPacket->vertices[2].textureU = vertex2TextureU;
    vertex1TextureV = triangle->textureV1;
    vertex2TextureV = triangle->textureV2;
    destinationPacket->vertices[0].textureV = triangle->textureV0;
    destinationPacket->vertices[1].textureV = vertex1TextureV;
    destinationPacket->vertices[2].textureV = vertex2TextureV;
    destinationPacket->renderFlags = renderFlags;
    return false;
  }
  return true;
}


/* Sets the three vertex colours of the packet appended last. When not all three colours are fully opaque
   (alpha 0xFF) and the packet's blend mode is opaque (0) or 4, the blend mode becomes 6 (the XOR clears the
   old mode). Called by the model renderer (graphics/render/model.c) after GraphicsPrimitiveQueue_AppendTriangle.
*/
void GraphicsPrimitiveQueue_SetVertexColors
          (PackedArgb32 vertex2Color,PackedArgb32 vertex1Color,PackedArgb32 vertex0Color,
          GraphicsPrimitiveQueue *queue)

{
  uint32_t existingBlendModeFlags;
  GraphicsPrimitivePacket *packetPool;
  uint32_t queuedPacketCount;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  packetPool[queuedPacketCount - 1].vertices[0].diffuseColor = vertex0Color;
  packetPool[queuedPacketCount - 1].vertices[1].diffuseColor = vertex1Color;
  packetPool[queuedPacketCount - 1].vertices[2].diffuseColor = vertex2Color;
  existingBlendModeFlags = packetPool[queuedPacketCount - 1].renderFlags & GRAPHICS_PRIMITIVE_BLEND_MASK;
  if ((vertex0Color & vertex1Color & vertex2Color & ARGB8888_ALPHA_MASK) != ARGB8888_ALPHA_MASK &&
      (existingBlendModeFlags == GRAPHICS_PRIMITIVE_BLEND_MODE_4 ||
       existingBlendModeFlags == GRAPHICS_PRIMITIVE_BLEND_OPAQUE)) {
    packetPool[queuedPacketCount - 1].renderFlags =
         packetPool[queuedPacketCount - 1].renderFlags ^ existingBlendModeFlags ^
         GRAPHICS_PRIMITIVE_BLEND_ALPHA_DEPTH_WRITE;
  }
  return;
}


/* Sets the modulation colour of the packet appended last and, when textureEntry is not NULL, its texture
   (marking the packet GRAPHICS_PRIMITIVE_FLAG_TEXTURED); otherwise the texture is cleared. Called by the model
   renderer (graphics/render/model.c) after GraphicsPrimitiveQueue_AppendTriangle.
*/
void GraphicsPrimitiveQueue_SetMaterial(PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry,
          GraphicsPrimitiveQueue *queue)

{
  uint32_t queuedPacketCount;
  GraphicsPrimitivePacket *packetPool;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  packetPool[queuedPacketCount - 1].modulationColor = modulationColor;
  packetPool[queuedPacketCount - 1].textureEntry = NULL;
  if (textureEntry != NULL) {
    packetPool[queuedPacketCount - 1].renderFlags =
         packetPool[queuedPacketCount - 1].renderFlags | GRAPHICS_PRIMITIVE_FLAG_TEXTURED;
    packetPool[queuedPacketCount - 1].textureEntry = textureEntry;
  }
  return;
}


/* Adds (deltaU, deltaV) to the texture coordinates of all three vertices of the packet appended last, for
   scrolling textures. Called by ModelRender_SubmitTriangle (graphics/render/model.c).
*/
void GraphicsPrimitiveQueue_OffsetTextureCoordinates(GraphicsPrimitiveTextureCoordinateFixed deltaV,
          GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateSlot;
  GraphicsPrimitiveTextureCoordinateFixed *vertex0TextureUSlot;
  uint32_t queuedPacketCount;
  GraphicsPrimitivePacket *packetPool;
  GraphicsPrimitiveTextureCoordinateFixed *vertex1TextureUSlot;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  vertex0TextureUSlot = &packetPool[queuedPacketCount - 1].vertices[0].textureU;
  *vertex0TextureUSlot = *vertex0TextureUSlot + deltaU;
  vertex1TextureUSlot = &packetPool[queuedPacketCount - 1].vertices[1].textureU;
  *vertex1TextureUSlot = *vertex1TextureUSlot + deltaU;
  textureCoordinateSlot = &packetPool[queuedPacketCount - 1].vertices[2].textureU;
  *textureCoordinateSlot = *textureCoordinateSlot + deltaU;
  textureCoordinateSlot = &packetPool[queuedPacketCount - 1].vertices[0].textureV;
  *textureCoordinateSlot = *textureCoordinateSlot + deltaV;
  textureCoordinateSlot = &packetPool[queuedPacketCount - 1].vertices[1].textureV;
  *textureCoordinateSlot = *textureCoordinateSlot + deltaV;
  textureCoordinateSlot = &packetPool[queuedPacketCount - 1].vertices[2].textureV;
  *textureCoordinateSlot = *textureCoordinateSlot + deltaV;
  return;
}




/* Broad-phase helper: returns a 32-bit mask with one bit per 1 << SPATIAL_BIN_SHIFT wide bin touched by
   the interval [center - radius, center + radius] on one world axis (Q12). Bin indices wrap modulo 32, so
   the mask is a coarse spatial hash used by the collision and target searches.
*/
DepthBinMask32 DepthInterval_BuildBinMask(DepthIntervalRadius32 radiusQ12,DepthIntervalCenter32 centerQ12)

{
  uint32_t binMask;
  int binIndex;
  uint32_t currentBinBit;

  binMask = 0;
  binIndex = (centerQ12 - radiusQ12) >> SPATIAL_BIN_SHIFT;
  currentBinBit = 1 << ((uint8_t)binIndex & SHIFT_COUNT_MASK);
  do {
    binMask = binMask | currentBinBit;
    binIndex = binIndex + 1;
    /* rotate left by one, so bin 31 wraps to bit 0 */
    currentBinBit = currentBinBit << 1 | currentBinBit >> 31;
  } while (binIndex <= ((centerQ12 - radiusQ12) + radiusQ12 * 2) >> SPATIAL_BIN_SHIFT);
  return binMask;
}


/* Broad-phase test for two objects' per-axis spatial bin masks (DepthInterval_BuildBinMask): true when
   axis 0 masks and axis 1 masks both share a bin, i.e. the objects may overlap.
*/
Bool8 DepthBinMasks_Overlap(DepthBinMask32 firstMaskAxis0,DepthBinMask32 firstMaskAxis1,DepthBinMask32 secondMaskAxis0,
          DepthBinMask32 secondMaskAxis1)

{
  if (((firstMaskAxis1 & secondMaskAxis1) != 0) && ((firstMaskAxis0 & secondMaskAxis0) != 0)) {
    return true;
  }
  return false;
}

