/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/primitives.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/primitives.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/render/primitives. */

/* Address: 0x00486080.
   Sorts a filled primitive queue for drawing and links the sorted nodes into the traversal list read by
   GraphicsPrimitiveQueue_Begin/Next. Each node gets a 32-bit key: blended packets (any blend-mode bit) the sum
   of their three vertex depths (below 0x80000000), opaque packets their texture entry with bits 0x30000000 of
   the render flags subtracted from 0xB0000000 (0x80000000 and above). Four stable byte-wise radix passes (low
   byte first) move the nodes between primaryNodes and radixScratchPool and back and order them by descending
   key: opaque packets first, grouped by texture, then the blended ones from the largest depth sum down. With
   halveVertexRgb set every packet's vertex RGB is halved (alpha kept, MMX).
   Installed in the graphics dispatch slot PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844 (called
   by FrontendModelPointerContext_RenderWorldViewQueuesClipped with node flag 8) and called directly by the
   offscreen model renderer (graphics/render/projection.c).
*/
void GraphicsPrimitiveQueue_RadixSortForRendering(GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue)

{
  int *bucketSlot;
  GraphicsPrimitivePacket *packetOrNode;
  uint32_t *bucketWriteCursor;
  uint32_t bucketIndexOrOffset;
  GraphicsPrimitiveQueueNode *nodeCursor;
  GraphicsPrimitiveQueueNode *pass3BucketStart;
  GraphicsPrimitiveQueueNode *primaryNodes;
  GraphicsPrimitivePacket *previousNode;
  GraphicsPrimitivePacket *previousNodeMmx;
  GraphicsPrimitivePacket *linkNode;
  uint32_t nodeSortKey;
  int bucketCountdownOrPacket;
  uint32_t remainingOrBucketCount;
  uint32_t remainingNodeCount;
  GraphicsPrimitiveQueueNode *readNode;
  GraphicsPrimitiveQueueNode *pass3ReadNode;
  uint32_t *bucketWordCursor;
  uint32_t *pass4BucketCursor;
  GraphicsPrimitiveQueueNode *scratchNodes;
  GraphicsPrimitivePacket *nextNode;
  GraphicsPrimitivePacket *nextNodeMmx;
  uint64_t vertex0HalvedColor;
  uint64_t vertex1HalvedColor;
  uint64_t vertex2HalvedColor;
  GraphicsPrimitivePacket *keyPacket;
  uint32_t *pass2WriteCursor;
  
  /* g_PrimitiveRadixBucketWords holds per pass first the bucket counts, then each bucket's write cursor
     (bucket 0xFF gets the first place, so the result is in descending key order) */
  remainingNodeCount = queue->count;
  scratchNodes = queue->radixScratchPool;
  primaryNodes = queue->primaryNodes;
  if (remainingNodeCount != 0) {
    remainingOrBucketCount = remainingNodeCount;
    nodeCursor = primaryNodes;
    if (remainingNodeCount != 1) {
      do {
        keyPacket = nodeCursor->packet;
        if ((keyPacket->renderFlags & GRAPHICS_PRIMITIVE_BLEND_MASK) == GRAPHICS_PRIMITIVE_BLEND_OPAQUE) {
          nodeSortKey = ((uint32_t)keyPacket->textureEntry | 0xb0000000) -
                  (keyPacket->renderFlags & 0x30000000);
        }
        else {
          nodeSortKey = keyPacket->vertices[0].depth +
                  keyPacket->vertices[1].depth +
                  keyPacket->vertices[2].depth & 0x7fffffff;
        }
        nodeCursor->sortKey = nodeSortKey;
        nodeCursor++;
        remainingOrBucketCount--;
      } while (remainingOrBucketCount != 0);
      /* pass 1: key bits 0..7, primaryNodes -> radixScratchPool */
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketCountdownOrPacket = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primaryNodes, bucketCountdownOrPacket != 0;
          bucketCountdownOrPacket--) {
        *bucketWordCursor = 0;
        bucketWordCursor++;
      }
      do {
        g_PrimitiveRadixBucketWords[nodeCursor->sortKey & 0xff] =
             g_PrimitiveRadixBucketWords[nodeCursor->sortKey & 0xff] + 1;
        remainingOrBucketCount--;
        nodeCursor++;
      } while (remainingOrBucketCount != 0);
      bucketCountdownOrPacket = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      nodeCursor = scratchNodes;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)nodeCursor;
        bucketWordCursor--;
        nodeCursor = nodeCursor + remainingOrBucketCount;
        bucketCountdownOrPacket--;
        remainingOrBucketCount = remainingNodeCount;
        readNode = primaryNodes;
      } while (bucketCountdownOrPacket != 0);
      do {
        nodeSortKey = readNode->sortKey;
        packetOrNode = readNode->packet;
        bucketIndexOrOffset = nodeSortKey & 0xff;
        bucketWriteCursor = (uint32_t *)g_PrimitiveRadixBucketWords[bucketIndexOrOffset];
        g_PrimitiveRadixBucketWords[bucketIndexOrOffset] = g_PrimitiveRadixBucketWords[bucketIndexOrOffset] + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrNode;
        remainingOrBucketCount--;
        readNode++;
      } while (remainingOrBucketCount != 0);
      /* pass 2: key bits 8..15 (shifted straight to a byte offset into the bucket words), back to primaryNodes */
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketCountdownOrPacket = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = scratchNodes, bucketCountdownOrPacket != 0;
          bucketCountdownOrPacket--) {
        *bucketWordCursor = 0;
        bucketWordCursor++;
      }
      do {
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff00) >> 6));
        *bucketSlot = *bucketSlot + 1;
        remainingOrBucketCount--;
        nodeCursor++;
      } while (remainingOrBucketCount != 0);
      bucketCountdownOrPacket = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      nodeCursor = primaryNodes;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)nodeCursor;
        bucketWordCursor--;
        nodeCursor = nodeCursor + remainingOrBucketCount;
        bucketCountdownOrPacket--;
        remainingOrBucketCount = remainingNodeCount;
        readNode = scratchNodes;
      } while (bucketCountdownOrPacket != 0);
      do {
        nodeSortKey = readNode->sortKey;
        packetOrNode = readNode->packet;
        bucketIndexOrOffset = (nodeSortKey & 0xff00) >> 6;
        pass2WriteCursor = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketSlot = *bucketSlot + 0x10;
        *pass2WriteCursor = nodeSortKey;
        pass2WriteCursor[1] = (uint32_t)packetOrNode;
        remainingOrBucketCount--;
        readNode++;
      } while (remainingOrBucketCount != 0);
      /* pass 3: key bits 16..23, primaryNodes -> radixScratchPool */
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketCountdownOrPacket = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primaryNodes, bucketCountdownOrPacket != 0;
          bucketCountdownOrPacket--) {
        *bucketWordCursor = 0;
        bucketWordCursor++;
      }
      do {
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff0000) >> 14));
        *bucketSlot = *bucketSlot + 1;
        remainingOrBucketCount--;
        nodeCursor++;
      } while (remainingOrBucketCount != 0);
      bucketCountdownOrPacket = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      pass3BucketStart = scratchNodes;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)pass3BucketStart;
        bucketWordCursor--;
        pass3BucketStart = pass3BucketStart + remainingOrBucketCount;
        bucketCountdownOrPacket--;
        remainingOrBucketCount = remainingNodeCount;
        pass3ReadNode = primaryNodes;
      } while (bucketCountdownOrPacket != 0);
      do {
        nodeSortKey = pass3ReadNode->sortKey;
        packetOrNode = pass3ReadNode->packet;
        pass3ReadNode++;
        bucketIndexOrOffset = (nodeSortKey & 0xff0000) >> 14;
        bucketWriteCursor = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketSlot = *bucketSlot + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrNode;
        remainingOrBucketCount--;
      } while (remainingOrBucketCount != 0);
      /* pass 4: key bits 24..31, back to primaryNodes */
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketCountdownOrPacket = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = scratchNodes, bucketCountdownOrPacket != 0;
          bucketCountdownOrPacket--) {
        *bucketWordCursor = 0;
        bucketWordCursor++;
      }
      do {
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff000000) >> 22));
        *bucketSlot = *bucketSlot + 1;
        remainingOrBucketCount--;
        nodeCursor++;
      } while (remainingOrBucketCount != 0);
      bucketCountdownOrPacket = 0x100;
      pass4BucketCursor = g_PrimitiveRadixBucketWords + 0xff;
      do {
        remainingOrBucketCount = *pass4BucketCursor;
        *pass4BucketCursor = (uint32_t)primaryNodes;
        pass4BucketCursor--;
        primaryNodes = primaryNodes + remainingOrBucketCount;
        bucketCountdownOrPacket--;
        remainingOrBucketCount = remainingNodeCount;
      } while (bucketCountdownOrPacket != 0);
      do {
        nodeSortKey = scratchNodes->sortKey;
        packetOrNode = scratchNodes->packet;
        scratchNodes++;
        bucketIndexOrOffset = (nodeSortKey & 0xff000000) >> 22;
        bucketWriteCursor = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketSlot = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketSlot = *bucketSlot + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrNode;
        remainingOrBucketCount--;
      } while (remainingOrBucketCount != 0);
    }
    /* Link the sorted primaryNodes in order. Ghidra types the nodes as packets here: vertices[0].backendCoord0
       is node->next (+0x08), backendCoord1 node->previous (+0x0C), vertices[0].screenY node->packet (+0x04),
       and &vertices[0].depth (+0x10) the following node; (queue + 1) is primaryNodes + 1. */
    packetOrNode = (GraphicsPrimitivePacket *)queue->primaryNodes;
    queue->traversalCursor = (GraphicsPrimitiveQueueNode *)packetOrNode;
    previousNodeMmx = (GraphicsPrimitivePacket *)0xffffffff;
    previousNode = (GraphicsPrimitivePacket *)0xffffffff;
    nextNodeMmx = (GraphicsPrimitivePacket *)(queue + 1);
    nextNode = (GraphicsPrimitivePacket *)(queue + 1);
    if (halveVertexRgb == GRAPHICS_STATE_DISABLED) {
      do {
        linkNode = packetOrNode;
        linkNode->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)previousNode;
        linkNode->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)nextNode;
        remainingNodeCount--;
        previousNode = linkNode;
        packetOrNode = nextNode;
        nextNode = (GraphicsPrimitivePacket *)&nextNode->vertices[0].depth;
      } while (remainingNodeCount != 0);
    }
    else {
      do {
        linkNode = packetOrNode;
        linkNode->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)previousNodeMmx;
        linkNode->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)nextNodeMmx;
        /* the node's packet; +0x1C/+0x3C/+0x5C are the three vertices' diffuseColor */
        bucketCountdownOrPacket = linkNode->vertices[0].screenY;
        vertex0HalvedColor =
             paddusb((*(uint32_t *)(bucketCountdownOrPacket + 0x1c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketCountdownOrPacket + 0x1c) & g_VertexColorAlphaPreserveMaskMMX);
        vertex1HalvedColor =
             paddusb((*(uint32_t *)(bucketCountdownOrPacket + 0x3c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketCountdownOrPacket + 0x3c) & g_VertexColorAlphaPreserveMaskMMX);
        vertex2HalvedColor =
             paddusb((*(uint32_t *)(bucketCountdownOrPacket + 0x5c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketCountdownOrPacket + 0x5c) & g_VertexColorAlphaPreserveMaskMMX);
        *(int *)(bucketCountdownOrPacket + 0x1c) = (int)vertex0HalvedColor;
        *(int *)(bucketCountdownOrPacket + 0x3c) = (int)vertex1HalvedColor;
        *(int *)(bucketCountdownOrPacket + 0x5c) = (int)vertex2HalvedColor;
        remainingNodeCount--;
        previousNodeMmx = linkNode;
        packetOrNode = nextNodeMmx;
        nextNodeMmx =
             (GraphicsPrimitivePacket *)&nextNodeMmx->vertices[0].depth;
      } while (remainingNodeCount != 0);
    }
    linkNode->vertices[0].backendCoord0 = -1; /* last node: next = GRAPHICS_PRIMITIVE_QUEUE_END_NODE */
  }
  return;
}


/* Address: 0x004D0A10.
   Allocates the global primitive queue pool for packetCapacity packets (header, two nodes and one packet
   each) and remembers the capacity for GraphicsPrimitiveQueue_ResetGlobal. CF set with the arena error
   when the allocation fails; g_PrimitiveQueueStorage is then left unchanged.
*/
StatusResult GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity)

{
  GraphicsPrimitiveQueue *allocatedQueueStorage;
  ArenaAllocResult allocResult;
  
  g_PrimitiveQueuePoolCapacity = packetCapacity;
  allocResult = g_MemoryApi.alloc(packetCapacity * GRAPHICS_PRIMITIVE_QUEUE_BYTES_PER_PACKET +
                                  GRAPHICS_PRIMITIVE_QUEUE_HEADER_BYTES);
  allocatedQueueStorage = (GraphicsPrimitiveQueue *)allocResult.payloadOrError;
  if (allocResult.failed) {
    return StatusValue_Fail(allocResult.payloadOrError);
  }
  g_PrimitiveQueueStorage = allocatedQueueStorage;
  return StatusValue_Ok(allocResult.payloadOrError);
}


/* Address: 0x004D0A40.
   Empties the global primitive queue (g_PrimitiveQueueStorage) for a new frame and lays out its pool:
   primaryNodes right after the 0x20-byte header, then radixScratchPool, then the packets, each part sized for
   the capacity given to GraphicsPrimitiveQueue_AllocateGlobalPool. Never fails (CF clear); returns the queue.
   Called by the frontend 3D views (ui/frontend/runtime.c) and the offscreen model renderer.
*/
PrimitiveQueueResult GraphicsPrimitiveQueue_ResetGlobal(void)

{
  GraphicsPrimitiveQueue *globalQueue;
  PrimitiveQueueResult resetResult;
  uint32_t poolCapacity;
  
  poolCapacity = g_PrimitiveQueuePoolCapacity;
  globalQueue = g_PrimitiveQueueStorage;
  /* capacity is stored through the global again, as in the original */
  g_PrimitiveQueueStorage->capacity = g_PrimitiveQueuePoolCapacity;
  globalQueue->count = 0;
  globalQueue->radixScratchPool = globalQueue->primaryNodes + poolCapacity;
  globalQueue->packetPool =
       (GraphicsPrimitivePacket *)(globalQueue->primaryNodes + poolCapacity + poolCapacity);
  resetResult.failed = false;
  resetResult.queue = globalQueue;
  return resetResult;
}


/* Address: 0x004D0A70.
   Frees a primitive queue allocation. No caller or table reference to this function is known.
*/
void GraphicsPrimitiveQueue_Free(GraphicsPrimitiveQueue *queue)

{
  g_MemoryApi.free(queue);
  return;
}


/* Address: 0x004D0A90.
   Returns the number of packets queued in queue. Used by FrontendModelPointerContext_RenderWorldViewQueuesClipped
   (ui/frontend/runtime.c) after each drawn pass.
*/
uint32_t GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue)

{
  return queue->count;
}


/* Address: 0x004D0AA0.
   Starts walking a sorted primitive queue: returns the packet of the node at traversalCursor (set by
   GraphicsPrimitiveQueue_RadixSortForRendering) and advances the cursor to the next node. CF set when the
   queue is empty; GraphicsPrimitiveQueue_Next continues the walk.
*/
PrimitivePacketResult GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue)

{
  PrimitivePacketResult successResult;
  PrimitivePacketResult failureResult;
  GraphicsPrimitivePacket *currentTraversalPacket;
  
  if (queue->count != 0) {
    currentTraversalPacket = queue->traversalCursor->packet;
    queue->traversalCursor = queue->traversalCursor->next;
    successResult.noPacket = false;
    successResult.packet = currentTraversalPacket;
    return successResult;
  }
  failureResult.noPacket = true;
  /* Empty queue: the original leaves EAX untouched; every caller stops on CF without reading it. */
  failureResult.packet = NULL;
  return failureResult;
}


/* Address: 0x004D0AE0.
   Continues a walk begun by GraphicsPrimitiveQueue_Begin: returns the packet of the node at traversalCursor
   and advances the cursor. CF set once the cursor reaches GRAPHICS_PRIMITIVE_QUEUE_END_NODE.
*/
PrimitivePacketResult GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue)

{
  PrimitivePacketResult successResult;
  PrimitivePacketResult failureResult;
  GraphicsPrimitiveQueueNode *currentTraversalNode;
  GraphicsPrimitivePacket *currentTraversalPacket;
  
  currentTraversalNode = queue->traversalCursor;
  if (currentTraversalNode != GRAPHICS_PRIMITIVE_QUEUE_END_NODE) {
    currentTraversalPacket = currentTraversalNode->packet;
    queue->traversalCursor = currentTraversalNode->next;
    successResult.noPacket = false;
    successResult.packet = currentTraversalPacket;
    return successResult;
  }
  failureResult.noPacket = true;
  /* EAX still holds the end-node marker */
  failureResult.packet = (GraphicsPrimitivePacket *)0xffffffff;
  return failureResult;
}


/* Address: 0x004D0B20.
   Appends a triangle packet for the model renderer: copies screen position, backend coordinates and depth of
   the three projected vertices and the texture coordinates from triangle, and sets renderFlags. The colours,
   material and texture are filled in afterwards by GraphicsPrimitiveQueue_SetVertexColors/SetMaterial. Returns
   true (CF set) when the queue is full; one slot is always left unused. Called by ModelRender_SubmitTriangle and
   ModelRender_PrepareProjectedVertexAlternatePath (graphics/render/model.c).
*/
bool GraphicsPrimitiveQueue_AppendTriangle(GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle,
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


/* Address: 0x004D0C80.
   Sets the three vertex colours of the packet appended last. When not all three colours are fully opaque
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
  if (((vertex0Color & vertex1Color & vertex2Color & 0xff000000) != 0xff000000) &&
     ((existingBlendModeFlags == 0x4000 || (existingBlendModeFlags == GRAPHICS_PRIMITIVE_BLEND_OPAQUE)))) {
    packetPool[queuedPacketCount - 1].renderFlags =
         packetPool[queuedPacketCount - 1].renderFlags ^ existingBlendModeFlags ^ 0x6000;
  }
  return;
}


/* Address: 0x004D0D00.
   Sets the modulation colour of the packet appended last and, when textureEntry is not NULL, its texture
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


/* Address: 0x004D0D50.
   Adds (deltaU, deltaV) to the texture coordinates of all three vertices of the packet appended last, for
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


/* Address: 0x004D0DA0.
   Terrain counterpart of GraphicsPrimitiveQueue_AppendTexturedTriangleRegs for the second projected surface:
   appends a packet from the terrain vertices' second screen/depth block (+0x2C..+0x3C), the per-vertex colours
   (masked with g_UiCommandModeGColorVariantLimit for vertices whose +0x4C is negative) and the texture
   coordinates of terrainPacketRecord (u0,v0,u1,v1,u2,v2, texture index, palette entry). Blend mode 6; textured
   with g_TerrainPrimaryTextureSet when the index is in range, modulated by g_TerrainPrimaryPalette. Returns the
   packet, CF set when the queue is full. Called by TerrainProjectedTriangle_ClipInterpolateAndQueueTextured
   (world/terrain/projection.c).
*/
PrimitivePacketResult GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveQueue *primitiveQueue;
  uint32_t packetIndexOrCoordinate;
  GraphicsPrimitiveScreenCoordinate sourceScreenX;
  GraphicsPrimitiveBackendCoordinate sourceBackendCoord1;
  GraphicsPrimitiveDepthFixed sourceDepth;
  uint32_t textureCoordinate;
  uint32_t textureEntryIndex;
  GraphicsTextureSet *terrainTextureSet;
  PackedArgb32 paletteModulationColor;
  GraphicsPrimitivePacket *newPacket;
  PrimitivePacketResult successResult;
  PrimitivePacketResult failureResult;
  
  primitiveQueue = renderContext->activePrimitiveQueue;
  packetIndexOrCoordinate = primitiveQueue->count;
  if (packetIndexOrCoordinate + 1 < primitiveQueue->capacity) {
    primitiveQueue->count = packetIndexOrCoordinate + 1;
    newPacket = primitiveQueue->packetPool + packetIndexOrCoordinate;
    primitiveQueue->primaryNodes[packetIndexOrCoordinate].packet = newPacket;
    /* the vertices are terrain vertices; the GraphicsProjectedVertexSource field names do not apply */
    sourceScreenX = vertex0Projected->screenX;
    if ((int)vertex0Projected[1].texturedPacketAttributes[2] < 0) {
      vertex0DiffuseColor = vertex0DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    newPacket->vertices[0].screenX = vertex0Projected->vertexColorArgb;
    newPacket->vertices[0].screenY = sourceScreenX;
    newPacket->vertices[0].diffuseColor = vertex0DiffuseColor;
    sourceBackendCoord1 = *(GraphicsPrimitiveBackendCoordinate *)vertex0Projected[1].reserved00_0B;
    sourceDepth = *(GraphicsPrimitiveDepthFixed *)(vertex0Projected[1].reserved00_0B + 4);
    newPacket->vertices[0].backendCoord0 = vertex0Projected->screenY;
    newPacket->vertices[0].backendCoord1 = sourceBackendCoord1;
    newPacket->vertices[0].depth = sourceDepth;
    sourceScreenX = vertex1Projected->screenX;
    if ((int)vertex1Projected[1].texturedPacketAttributes[2] < 0) {
      vertex1DiffuseColor = vertex1DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    newPacket->vertices[1].screenX = vertex1Projected->vertexColorArgb;
    newPacket->vertices[1].screenY = sourceScreenX;
    newPacket->vertices[1].diffuseColor = vertex1DiffuseColor;
    sourceBackendCoord1 = *(GraphicsPrimitiveBackendCoordinate *)vertex1Projected[1].reserved00_0B;
    sourceDepth = *(GraphicsPrimitiveDepthFixed *)(vertex1Projected[1].reserved00_0B + 4);
    newPacket->vertices[1].backendCoord0 = vertex1Projected->screenY;
    newPacket->vertices[1].backendCoord1 = sourceBackendCoord1;
    newPacket->vertices[1].depth = sourceDepth;
    sourceScreenX = vertex2Projected->screenX;
    if ((int)vertex2Projected[1].texturedPacketAttributes[2] < 0) {
      vertex2DiffuseColor = vertex2DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    newPacket->vertices[2].screenX = vertex2Projected->vertexColorArgb;
    newPacket->vertices[2].screenY = sourceScreenX;
    newPacket->vertices[2].diffuseColor = vertex2DiffuseColor;
    sourceBackendCoord1 = *(GraphicsPrimitiveBackendCoordinate *)vertex2Projected[1].reserved00_0B;
    sourceDepth = *(GraphicsPrimitiveDepthFixed *)(vertex2Projected[1].reserved00_0B + 4);
    newPacket->vertices[2].backendCoord0 = vertex2Projected->screenY;
    newPacket->vertices[2].backendCoord1 = sourceBackendCoord1;
    newPacket->vertices[2].depth = sourceDepth;
    packetIndexOrCoordinate = terrainPacketRecord[2];
    textureCoordinate = terrainPacketRecord[4];
    newPacket->vertices[0].textureU = *terrainPacketRecord;
    newPacket->vertices[1].textureU = packetIndexOrCoordinate;
    newPacket->vertices[2].textureU = textureCoordinate;
    packetIndexOrCoordinate = terrainPacketRecord[3];
    textureCoordinate = terrainPacketRecord[5];
    newPacket->vertices[0].textureV = terrainPacketRecord[1];
    newPacket->vertices[1].textureV = packetIndexOrCoordinate;
    newPacket->vertices[2].textureV = textureCoordinate;
    paletteModulationColor = 0;
    if (g_TerrainPrimaryPalette != NULL) {
      paletteModulationColor = g_TerrainPrimaryPalette->paletteEntries[terrainPacketRecord[7]].
              alternateModulationColorArgb;
    }
    newPacket->renderFlags = 0x6000;
    newPacket->modulationColor = paletteModulationColor;
    terrainTextureSet = g_TerrainPrimaryTextureSet;
    textureEntryIndex = terrainPacketRecord[6];
    newPacket->textureEntry = NULL;
    if ((terrainTextureSet != NULL) && (textureEntryIndex < terrainTextureSet->subresourceCount)) {
      newPacket->renderFlags = newPacket->renderFlags | GRAPHICS_PRIMITIVE_FLAG_TEXTURED;
      newPacket->textureEntry = terrainTextureSet->entries + textureEntryIndex;
    }
    successResult.noPacket = false;
    successResult.packet = newPacket;
    return successResult;
  }
  failureResult.noPacket = true;
  /* Queue full: the original leaves EAX untouched; callers use the packet only with CF clear. */
  failureResult.packet = NULL;
  return failureResult;
}


/* Address: 0x004D0F20.
   Appends a textured terrain triangle: copies each terrain vertex's screen position, backend coordinates and
   depth (+0x0C..+0x1C) and its colour, the texture coordinates of terrainPacketRecord (u0,v0,u1,v1,u2,v2,
   texture-set index, palette entry), the modulation colour from g_TerrainSecondaryPalette, the first texture
   of g_TerrainMaterialTextureSets[index] and the render flags g_UiCommandModeGColorVariantFlags. Returns the
   packet, CF set when the queue is full. Called by TerrainProjectedQuad_QueueAsTwoTrianglesRegs and
   TerrainProjectedTriangle_ClipInterpolateAndQueueTextured (world/terrain/projection.c).
*/
PrimitivePacketResult GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
          (uint32_t *terrainPacketRecord,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveQueue *primitiveQueue;
  uint32_t packetIndexOrAttribute;
  uint32_t packetAttribute;
  GraphicsTextureSet *materialTextureSet;
  PackedArgb32 paletteModulationColor;
  uint32_t *packetDwords;
  PrimitivePacketResult successResult;
  PrimitivePacketResult failureResult;
  
  primitiveQueue = renderContext->activePrimitiveQueue;
  packetIndexOrAttribute = primitiveQueue->count;
  if (packetIndexOrAttribute + 1 < primitiveQueue->capacity) {
    primitiveQueue->count = packetIndexOrAttribute + 1;
    packetDwords = (uint32_t *)(primitiveQueue->packetPool + packetIndexOrAttribute);
    primitiveQueue->primaryNodes[packetIndexOrAttribute].packet = (GraphicsPrimitivePacket *)packetDwords;
    /* packet dwords: 0..7 vertex 0 (screenX, screenY, backendCoord0, backendCoord1, depth, textureU, textureV,
       diffuseColor), 8..15 vertex 1, 16..23 vertex 2, 24 modulationColor, 25 textureEntry, 26 renderFlags */
    packetIndexOrAttribute = vertex0Projected->texturedPacketAttributes[1];
    *packetDwords = vertex0Projected->texturedPacketAttributes[0];
    packetDwords[1] = packetIndexOrAttribute;
    packetDwords[7] = vertex0DiffuseColor;
    packetIndexOrAttribute = vertex0Projected->texturedPacketAttributes[3];
    packetAttribute = vertex0Projected->texturedPacketAttributes[4];
    packetDwords[2] = vertex0Projected->texturedPacketAttributes[2];
    packetDwords[3] = packetIndexOrAttribute;
    packetDwords[4] = packetAttribute;
    packetIndexOrAttribute = vertex1Projected->texturedPacketAttributes[1];
    packetDwords[8] = vertex1Projected->texturedPacketAttributes[0];
    packetDwords[9] = packetIndexOrAttribute;
    packetDwords[0xf] = vertex1DiffuseColor;
    packetIndexOrAttribute = vertex1Projected->texturedPacketAttributes[3];
    packetAttribute = vertex1Projected->texturedPacketAttributes[4];
    packetDwords[10] = vertex1Projected->texturedPacketAttributes[2];
    packetDwords[0xb] = packetIndexOrAttribute;
    packetDwords[0xc] = packetAttribute;
    packetIndexOrAttribute = vertex2Projected->texturedPacketAttributes[1];
    packetDwords[0x10] = vertex2Projected->texturedPacketAttributes[0];
    packetDwords[0x11] = packetIndexOrAttribute;
    packetDwords[0x17] = vertex2DiffuseColor;
    packetIndexOrAttribute = vertex2Projected->texturedPacketAttributes[3];
    packetAttribute = vertex2Projected->texturedPacketAttributes[4];
    packetDwords[0x12] = vertex2Projected->texturedPacketAttributes[2];
    packetDwords[0x13] = packetIndexOrAttribute;
    packetDwords[0x14] = packetAttribute;
    packetIndexOrAttribute = terrainPacketRecord[2];
    packetAttribute = terrainPacketRecord[4];
    packetDwords[5] = *terrainPacketRecord;
    packetDwords[0xd] = packetIndexOrAttribute;
    packetDwords[0x15] = packetAttribute;
    packetIndexOrAttribute = terrainPacketRecord[3];
    packetAttribute = terrainPacketRecord[5];
    packetDwords[6] = terrainPacketRecord[1];
    packetDwords[0xe] = packetIndexOrAttribute;
    packetDwords[0x16] = packetAttribute;
    paletteModulationColor = 0;
    if (g_TerrainSecondaryPalette != NULL) {
      paletteModulationColor = g_TerrainSecondaryPalette->paletteEntries[terrainPacketRecord[7]].
              alternateModulationColorArgb;
    }
    packetDwords[0x18] = paletteModulationColor;
    materialTextureSet = g_TerrainMaterialTextureSets[terrainPacketRecord[6]];
    packetDwords[0x1a] = g_UiCommandModeGColorVariantFlags;
    packetDwords[0x19] = (uint32_t)materialTextureSet->entries;
    successResult.noPacket = false;
    successResult.packet = (GraphicsPrimitivePacket *)packetDwords;
    return successResult;
  }
  failureResult.noPacket = true;
  /* Queue full: the original leaves EAX untouched; callers use the packet only with CF clear. */
  failureResult.packet = NULL;
  return failureResult;
}


/* Address: 0x004FFC10.
   Broad-phase helper: returns a 32-bit mask with one bit per 1 << SPATIAL_BIN_SHIFT wide bin touched by
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
  currentBinBit = 1 << ((uint8_t)binIndex & 0x1f);
  do {
    binMask = binMask | currentBinBit;
    binIndex = binIndex + 1;
    /* ADD EBX,EBX / ADC EBX,0: rotate left by one, so bin 31 wraps to bit 0 */
    currentBinBit = currentBinBit * 2 + (uint32_t)CARRY4(currentBinBit,currentBinBit);
  } while (binIndex <= ((centerQ12 - radiusQ12) + radiusQ12 * 2) >> SPATIAL_BIN_SHIFT);
  return binMask;
}


/* Address: 0x004FFC50.
   Broad-phase test for two objects' per-axis spatial bin masks (DepthInterval_BuildBinMask): CF set when
   axis 0 masks and axis 1 masks both share a bin, i.e. the objects may overlap.
*/
bool DepthBinMasks_Overlap(DepthBinMask32 firstMaskAxis0,DepthBinMask32 firstMaskAxis1,DepthBinMask32 secondMaskAxis0,
          DepthBinMask32 secondMaskAxis1)

{
  if (((firstMaskAxis1 & secondMaskAxis1) != 0) && ((firstMaskAxis0 & secondMaskAxis0) != 0)) {
    return true;
  }
  return false;
}

