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
   Ownership: graphics/render/primitives.
   Purpose: Generates one sort key per primary node, performs a stable four-pass least-significant-byte radix sort
   between primaryNodes and radixScratchPool, then rebuilds next/previous traversal links. Textured/opaque packets
   use a texture/flag key; other packets use the summed vertex depth. When halveVertexRgb is nonzero, the function
   preserves alpha and halves each vertex RGB channel with MMX. Typed parameters: p0
   halveVertexRgb→GraphicsBooleanState_V307. Nearby but non-identical semantic domains were explicitly deferred.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_RadixSortForRendering
          (GraphicsBooleanState halveVertexRgb,GraphicsPrimitiveQueue *queue)

{
  int *bucketWordPtr;
  GraphicsPrimitivePacket *packetOrLinkNode;
  uint32_t *bucketWriteCursor;
  uint32_t bucketIndexOrOffset;
  GraphicsPrimitiveQueueNode *nodeCursor;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor1;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor2;
  GraphicsPrimitivePacket *previousLinkNode;
  GraphicsPrimitivePacket *primitivePacketCursor3;
  GraphicsPrimitivePacket *primitivePacketCursor2;
  uint32_t nodeSortKey;
  int bucketLoopOrVertexBase;
  uint32_t remainingOrBucketCount;
  uint32_t remainingNodeCount;
  GraphicsPrimitiveQueueNode *readNode;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor3;
  uint32_t *bucketWordCursor;
  uint32_t *sortWordCursor2;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor4;
  GraphicsPrimitivePacket *nextLinkNode;
  GraphicsPrimitivePacket *primitivePacketCursor4;
  uint64_t mm0PackedValue0;
  uint64_t mm1PackedValue0;
  uint64_t mm2PackedValue0;
  GraphicsPrimitivePacket *primitivePacketCursor1;
  uint32_t *sortWordCursor1;
  
  remainingNodeCount = queue->count;
  primitiveQueueNodeCursor4 = queue->radixScratchPool;
  primitiveQueueNodeCursor2 = queue->primaryNodes;
  if (remainingNodeCount != 0) {
    remainingOrBucketCount = remainingNodeCount;
    nodeCursor = primitiveQueueNodeCursor2;
    if (remainingNodeCount != 1) {
      do {
        primitivePacketCursor1 = nodeCursor->packet;
        if ((primitivePacketCursor1->renderFlags & 0x7000) == 0) {
          nodeSortKey = ((uint32_t)primitivePacketCursor1->textureEntry | 0xb0000000) -
                  (primitivePacketCursor1->renderFlags & 0x30000000);
        }
        else {
          nodeSortKey = primitivePacketCursor1->vertices[0].depth +
                  primitivePacketCursor1->vertices[1].depth +
                  primitivePacketCursor1->vertices[2].depth & 0x7fffffff;
        }
        nodeCursor->sortKey = nodeSortKey;
        nodeCursor = nodeCursor + 1;
        remainingOrBucketCount = remainingOrBucketCount - 1;
      } while (remainingOrBucketCount != 0);
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketLoopOrVertexBase = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primitiveQueueNodeCursor2, bucketLoopOrVertexBase != 0;
          bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1) {
        *bucketWordCursor = 0;
        bucketWordCursor = bucketWordCursor + 1;
      }
      do {
        g_PrimitiveRadixBucketWords[nodeCursor->sortKey & 0xff] =
             g_PrimitiveRadixBucketWords[nodeCursor->sortKey & 0xff] + 1;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        nodeCursor = nodeCursor + 1;
      } while (remainingOrBucketCount != 0);
      bucketLoopOrVertexBase = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      nodeCursor = primitiveQueueNodeCursor4;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)nodeCursor;
        bucketWordCursor = bucketWordCursor + -1;
        nodeCursor = nodeCursor + remainingOrBucketCount;
        bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1;
        remainingOrBucketCount = remainingNodeCount;
        readNode = primitiveQueueNodeCursor2;
      } while (bucketLoopOrVertexBase != 0);
      do {
        nodeSortKey = readNode->sortKey;
        packetOrLinkNode = readNode->packet;
        bucketIndexOrOffset = nodeSortKey & 0xff;
        bucketWriteCursor = (uint32_t *)g_PrimitiveRadixBucketWords[bucketIndexOrOffset];
        g_PrimitiveRadixBucketWords[bucketIndexOrOffset] = g_PrimitiveRadixBucketWords[bucketIndexOrOffset] + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrLinkNode;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        readNode = readNode + 1;
      } while (remainingOrBucketCount != 0);
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketLoopOrVertexBase = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primitiveQueueNodeCursor4, bucketLoopOrVertexBase != 0;
          bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1) {
        *bucketWordCursor = 0;
        bucketWordCursor = bucketWordCursor + 1;
      }
      do {
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff00) >> 6));
        *bucketWordPtr = *bucketWordPtr + 1;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        nodeCursor = nodeCursor + 1;
      } while (remainingOrBucketCount != 0);
      bucketLoopOrVertexBase = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      nodeCursor = primitiveQueueNodeCursor2;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)nodeCursor;
        bucketWordCursor = bucketWordCursor + -1;
        nodeCursor = nodeCursor + remainingOrBucketCount;
        bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1;
        remainingOrBucketCount = remainingNodeCount;
        readNode = primitiveQueueNodeCursor4;
      } while (bucketLoopOrVertexBase != 0);
      do {
        nodeSortKey = readNode->sortKey;
        packetOrLinkNode = readNode->packet;
        bucketIndexOrOffset = (nodeSortKey & 0xff00) >> 6;
        sortWordCursor1 = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketWordPtr = *bucketWordPtr + 0x10;
        *sortWordCursor1 = nodeSortKey;
        sortWordCursor1[1] = (uint32_t)packetOrLinkNode;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        readNode = readNode + 1;
      } while (remainingOrBucketCount != 0);
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketLoopOrVertexBase = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primitiveQueueNodeCursor2, bucketLoopOrVertexBase != 0;
          bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1) {
        *bucketWordCursor = 0;
        bucketWordCursor = bucketWordCursor + 1;
      }
      do {
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff0000) >> 0xe));
        *bucketWordPtr = *bucketWordPtr + 1;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        nodeCursor = nodeCursor + 1;
      } while (remainingOrBucketCount != 0);
      bucketLoopOrVertexBase = 0x100;
      bucketWordCursor = g_PrimitiveRadixBucketWords + 0xff;
      primitiveQueueNodeCursor1 = primitiveQueueNodeCursor4;
      do {
        remainingOrBucketCount = *bucketWordCursor;
        *bucketWordCursor = (uint32_t)primitiveQueueNodeCursor1;
        bucketWordCursor = bucketWordCursor + -1;
        primitiveQueueNodeCursor1 = primitiveQueueNodeCursor1 + remainingOrBucketCount;
        bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1;
        remainingOrBucketCount = remainingNodeCount;
        primitiveQueueNodeCursor3 = primitiveQueueNodeCursor2;
      } while (bucketLoopOrVertexBase != 0);
      do {
        nodeSortKey = primitiveQueueNodeCursor3->sortKey;
        packetOrLinkNode = primitiveQueueNodeCursor3->packet;
        primitiveQueueNodeCursor3 = primitiveQueueNodeCursor3 + 1;
        bucketIndexOrOffset = (nodeSortKey & 0xff0000) >> 0xe;
        bucketWriteCursor = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketWordPtr = *bucketWordPtr + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrLinkNode;
        remainingOrBucketCount = remainingOrBucketCount - 1;
      } while (remainingOrBucketCount != 0);
      bucketWordCursor = g_PrimitiveRadixBucketWords;
      for (bucketLoopOrVertexBase = 0x100; remainingOrBucketCount = remainingNodeCount, nodeCursor = primitiveQueueNodeCursor4, bucketLoopOrVertexBase != 0;
          bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1) {
        *bucketWordCursor = 0;
        bucketWordCursor = bucketWordCursor + 1;
      }
      do {
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + ((nodeCursor->sortKey & 0xff000000) >> 0x16)
                        );
        *bucketWordPtr = *bucketWordPtr + 1;
        remainingOrBucketCount = remainingOrBucketCount - 1;
        nodeCursor = nodeCursor + 1;
      } while (remainingOrBucketCount != 0);
      bucketLoopOrVertexBase = 0x100;
      sortWordCursor2 = g_PrimitiveRadixBucketWords + 0xff;
      do {
        remainingOrBucketCount = *sortWordCursor2;
        *sortWordCursor2 = (uint32_t)primitiveQueueNodeCursor2;
        sortWordCursor2 = sortWordCursor2 + -1;
        primitiveQueueNodeCursor2 = primitiveQueueNodeCursor2 + remainingOrBucketCount;
        bucketLoopOrVertexBase = bucketLoopOrVertexBase + -1;
        remainingOrBucketCount = remainingNodeCount;
      } while (bucketLoopOrVertexBase != 0);
      do {
        nodeSortKey = primitiveQueueNodeCursor4->sortKey;
        packetOrLinkNode = primitiveQueueNodeCursor4->packet;
        primitiveQueueNodeCursor4 = primitiveQueueNodeCursor4 + 1;
        bucketIndexOrOffset = (nodeSortKey & 0xff000000) >> 0x16;
        bucketWriteCursor = *(uint32_t **)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        bucketWordPtr = (int *)((int)g_PrimitiveRadixBucketWords + bucketIndexOrOffset);
        *bucketWordPtr = *bucketWordPtr + 0x10;
        *bucketWriteCursor = nodeSortKey;
        bucketWriteCursor[1] = (uint32_t)packetOrLinkNode;
        remainingOrBucketCount = remainingOrBucketCount - 1;
      } while (remainingOrBucketCount != 0);
    }
    packetOrLinkNode = (GraphicsPrimitivePacket *)queue->primaryNodes;
    queue->traversalCursor = (GraphicsPrimitiveQueueNode *)packetOrLinkNode;
    primitivePacketCursor3 = (GraphicsPrimitivePacket *)0xffffffff;
    previousLinkNode = (GraphicsPrimitivePacket *)0xffffffff;
    primitivePacketCursor4 = (GraphicsPrimitivePacket *)(queue + 1);
    nextLinkNode = (GraphicsPrimitivePacket *)(queue + 1);
    if (halveVertexRgb == GRAPHICS_STATE_DISABLED) {
      do {
        primitivePacketCursor2 = packetOrLinkNode;
        primitivePacketCursor2->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)previousLinkNode;
        primitivePacketCursor2->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)nextLinkNode;
        remainingNodeCount = remainingNodeCount - 1;
        previousLinkNode = primitivePacketCursor2;
        packetOrLinkNode = nextLinkNode;
        nextLinkNode = (GraphicsPrimitivePacket *)&nextLinkNode->vertices[0].depth;
      } while (remainingNodeCount != 0);
    }
    else {
      do {
        primitivePacketCursor2 = packetOrLinkNode;
        primitivePacketCursor2->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)primitivePacketCursor3;
        primitivePacketCursor2->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)primitivePacketCursor4;
        bucketLoopOrVertexBase = primitivePacketCursor2->vertices[0].screenY;
        mm0PackedValue0 =
             paddusb((*(uint32_t *)(bucketLoopOrVertexBase + 0x1c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketLoopOrVertexBase + 0x1c) & g_VertexColorAlphaPreserveMaskMMX);
        mm1PackedValue0 =
             paddusb((*(uint32_t *)(bucketLoopOrVertexBase + 0x3c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketLoopOrVertexBase + 0x3c) & g_VertexColorAlphaPreserveMaskMMX);
        mm2PackedValue0 =
             paddusb((*(uint32_t *)(bucketLoopOrVertexBase + 0x5c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint32_t *)(bucketLoopOrVertexBase + 0x5c) & g_VertexColorAlphaPreserveMaskMMX);
        *(int *)(bucketLoopOrVertexBase + 0x1c) = (int)mm0PackedValue0;
        *(int *)(bucketLoopOrVertexBase + 0x3c) = (int)mm1PackedValue0;
        *(int *)(bucketLoopOrVertexBase + 0x5c) = (int)mm2PackedValue0;
        remainingNodeCount = remainingNodeCount - 1;
        primitivePacketCursor3 = primitivePacketCursor2;
        packetOrLinkNode = primitivePacketCursor4;
        primitivePacketCursor4 =
             (GraphicsPrimitivePacket *)&primitivePacketCursor4->vertices[0].depth;
      } while (remainingNodeCount != 0);
    }
    primitivePacketCursor2->vertices[0].backendCoord0 = -1;
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
   Ownership: graphics/render/primitives.
   Purpose: Initializes the global variable-length queue: primaryNodes follow the 0x20-byte header,
   radixScratchPool follows primaryNodes, and packetPool follows the scratch nodes.
*/
PrimitiveQueueResult __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_ResetGlobal(void)

{
  GraphicsPrimitiveQueue *globalQueue;
  PrimitiveQueueResult resetResult;
  GraphicsPrimitiveQueue *queueStorage;
  uint32_t poolCapacity;
  
  poolCapacity = g_PrimitiveQueuePoolCapacity;
  globalQueue = g_PrimitiveQueueStorage;
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
   Ownership: graphics/render/primitives.
   Purpose: Frees a primitive queue allocation.
*/
void __thandor_preserve_eax GraphicsPrimitiveQueue_Free(GraphicsPrimitiveQueue *queue)

{
  g_MemoryApi.free(queue);
  return;
}


/* Address: 0x004D0A90.
   Ownership: graphics/render/primitives.
   Purpose: Returns queue->count.
*/
uint32_t __thandor_eax_preserve_ecx_edx GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue)

{
  return queue->count;
}


/* Address: 0x004D0AA0.
   Starts walking a sorted primitive queue: returns the packet of the node at traversalCursor (set by
   GraphicsPrimitiveQueue_RadixSortForRendering) and advances the cursor to the next node. CF set when the
   queue is empty; GraphicsPrimitiveQueue_Next continues the walk.
*/
PrimitivePacketResult __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue)

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
PrimitivePacketResult __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue)

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
   Ownership: graphics/render/primitives.
   Purpose: Allocates the next packet and stores its pointer in primaryNodes[count].packet. ABI: CF clear means
   success. CF set means failure or end of iteration. No individual bit meaning is promoted beyond the established
   mask role. Typed parameters: p0 renderFlags→GraphicsRenderFlagMask_V338.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_AppendTriangle
          (GraphicsRenderFlagMask renderFlags,GraphicsTriangleInput *triangle,
          GraphicsProjectedVertexSource *vertex2,GraphicsProjectedVertexSource *vertex1,
          GraphicsProjectedVertexSource *vertex0,GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveScreenCoordinate vertexScreenY;
  GraphicsPrimitiveBackendCoordinate vertexBackendCoord1;
  GraphicsPrimitiveTextureCoordinateFixed vertex1TextureV;
  GraphicsPrimitiveTextureCoordinateFixed vertex2TextureV;
  GraphicsPrimitivePacket *destinationPacket;
  uint32_t destinationPacketIndex;
  GraphicsPrimitiveScreenCoordinate copiedScreenY;
  GraphicsPrimitiveBackendCoordinate copiedBackendCoordinate1;
  GraphicsPrimitiveDepthFixed vertex1Depth;
  GraphicsPrimitiveDepthFixed vertex2Depth;
  GraphicsPrimitiveTextureCoordinateFixed vertex1TextureCoordinate;
  GraphicsPrimitiveTextureCoordinateFixed vertex2TextureCoordinate;
  
  destinationPacketIndex = queue->count;
  if (destinationPacketIndex + 1 < queue->capacity) {
    queue->count = destinationPacketIndex + 1;
    destinationPacket = queue->packetPool + destinationPacketIndex;
    queue->primaryNodes[destinationPacketIndex].packet = destinationPacket;
    copiedScreenY = vertex0->screenY;
    destinationPacket->vertices[0].screenX = vertex0->screenX;
    destinationPacket->vertices[0].screenY = copiedScreenY;
    vertexScreenY = vertex1->screenY;
    destinationPacket->vertices[1].screenX = vertex1->screenX;
    destinationPacket->vertices[1].screenY = vertexScreenY;
    vertexScreenY = vertex2->screenY;
    destinationPacket->vertices[2].screenX = vertex2->screenX;
    destinationPacket->vertices[2].screenY = vertexScreenY;
    copiedBackendCoordinate1 = vertex0->backendCoord1;
    destinationPacket->vertices[0].backendCoord0 = vertex0->backendCoord0;
    destinationPacket->vertices[0].backendCoord1 = copiedBackendCoordinate1;
    vertexBackendCoord1 = vertex1->backendCoord1;
    destinationPacket->vertices[1].backendCoord0 = vertex1->backendCoord0;
    destinationPacket->vertices[1].backendCoord1 = vertexBackendCoord1;
    vertexBackendCoord1 = vertex2->backendCoord1;
    destinationPacket->vertices[2].backendCoord0 = vertex2->backendCoord0;
    destinationPacket->vertices[2].backendCoord1 = vertexBackendCoord1;
    vertex1Depth = vertex1->depth;
    vertex2Depth = vertex2->depth;
    destinationPacket->vertices[0].depth = vertex0->depth;
    destinationPacket->vertices[1].depth = vertex1Depth;
    destinationPacket->vertices[2].depth = vertex2Depth;
    vertex1TextureCoordinate = triangle->textureU1;
    vertex2TextureCoordinate = triangle->textureU2;
    destinationPacket->vertices[0].textureU = triangle->textureU0;
    destinationPacket->vertices[1].textureU = vertex1TextureCoordinate;
    destinationPacket->vertices[2].textureU = vertex2TextureCoordinate;
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
   Ownership: graphics/render/primitives.
   Purpose: Writes three diffuse colors to the most recently appended packet and adjusts shading/alpha handler
   bits.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_SetVertexColors
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
  existingBlendModeFlags = packetPool[queuedPacketCount - 1].renderFlags & 0x7000;
  if (((vertex0Color & vertex1Color & vertex2Color & 0xff000000) != 0xff000000) &&
     ((existingBlendModeFlags == 0x4000 || (existingBlendModeFlags == 0)))) {
    packetPool[queuedPacketCount - 1].renderFlags =
         packetPool[queuedPacketCount - 1].renderFlags ^ existingBlendModeFlags ^ 0x6000;
  }
  return;
}


/* Address: 0x004D0D00.
   Ownership: graphics/render/primitives.
   Purpose: Writes modulationColor and an optional GraphicsTextureSetEntry pointer to the most recently appended
   packet.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_SetMaterial
          (PackedArgb32 modulationColor,GraphicsTextureSetEntry *textureEntry,
          GraphicsPrimitiveQueue *queue)

{
  uint32_t queuedPacketCount;
  GraphicsPrimitivePacket *packetPool;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  packetPool[queuedPacketCount - 1].modulationColor = modulationColor;
  packetPool[queuedPacketCount - 1].textureEntry = (GraphicsTextureSetEntry *)0x0;
  if (textureEntry != (GraphicsTextureSetEntry *)0x0) {
    packetPool[queuedPacketCount - 1].renderFlags =
         packetPool[queuedPacketCount - 1].renderFlags | 0x10000;
    packetPool[queuedPacketCount - 1].textureEntry = textureEntry;
  }
  return;
}


/* Address: 0x004D0D50.
   Ownership: graphics/render/primitives.
   Purpose: Offsets all three texture-coordinate pairs in the most recently appended packet.
*/
void __thandor_void_preserve_eax_ecx_edx
GraphicsPrimitiveQueue_OffsetTextureCoordinates
          (GraphicsPrimitiveTextureCoordinateFixed deltaV,
          GraphicsPrimitiveTextureCoordinateFixed deltaU,GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateSlot;
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateCursor;
  uint32_t queuedPacketCount;
  GraphicsPrimitivePacket *packetPool;
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateField;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  textureCoordinateCursor = &packetPool[queuedPacketCount - 1].vertices[0].textureU;
  *textureCoordinateCursor = *textureCoordinateCursor + deltaU;
  textureCoordinateField = &packetPool[queuedPacketCount - 1].vertices[1].textureU;
  *textureCoordinateField = *textureCoordinateField + deltaU;
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
   Ownership: graphics/render/primitives.
   Purpose: Handles graphics primitive queue append terrain textured triangle.
*/
PrimitivePacketResult __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangle
          (uint32_t *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
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
    packetIndexOrCoordinate = textureAndMaterialIndices[2];
    textureCoordinate = textureAndMaterialIndices[4];
    newPacket->vertices[0].textureU = *textureAndMaterialIndices;
    newPacket->vertices[1].textureU = packetIndexOrCoordinate;
    newPacket->vertices[2].textureU = textureCoordinate;
    packetIndexOrCoordinate = textureAndMaterialIndices[3];
    textureCoordinate = textureAndMaterialIndices[5];
    newPacket->vertices[0].textureV = textureAndMaterialIndices[1];
    newPacket->vertices[1].textureV = packetIndexOrCoordinate;
    newPacket->vertices[2].textureV = textureCoordinate;
    paletteModulationColor = 0;
    if (g_TerrainPrimaryPalette != (GraphicsPaletteAsset *)0x0) {
      paletteModulationColor = g_TerrainPrimaryPalette->paletteEntries[textureAndMaterialIndices[7]].
              alternateModulationColorArgb;
    }
    newPacket->renderFlags = 0x6000;
    newPacket->modulationColor = paletteModulationColor;
    terrainTextureSet = g_TerrainPrimaryTextureSet;
    textureEntryIndex = textureAndMaterialIndices[6];
    newPacket->textureEntry = (GraphicsTextureSetEntry *)0x0;
    if ((terrainTextureSet != (GraphicsTextureSet *)0x0) && (textureEntryIndex < terrainTextureSet->subresourceCount)) {
      newPacket->renderFlags = newPacket->renderFlags | 0x10000;
      newPacket->textureEntry = terrainTextureSet->entries + textureEntryIndex;
    }
    successResult.noPacket = false;
    successResult.packet = newPacket;
    return successResult;
  }
  failureResult.noPacket = true;
  /* Queue full: the original leaves EAX untouched; callers use the packet only with CF clear. */
  failureResult.packet = (GraphicsPrimitivePacket *)0;
  return failureResult;
}


/* Address: 0x004D0F20.
   Ownership: graphics/render/primitives.
   Purpose: Reserves one 0x80-byte primitive packet, copies three projected vertices and texture coordinates,
   resolves the texture and palette references, and returns the packet with carry clear or reports a full queue
   through carry. Typed parameters: p3 vertex2DiffuseColor→PackedArgb32, p4 vertex1DiffuseColor→PackedArgb32, p5
   vertex0DiffuseColor→PackedArgb32. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p6 vertex2Projected→GraphicsProjectedVertexSource *, p7
   vertex1Projected→GraphicsProjectedVertexSource *, p8 vertex0Projected→GraphicsProjectedVertexSource *.
*/
PrimitivePacketResult __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
          (uint32_t *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
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
  uint32_t *vertexFieldWriteCursor;
  PrimitivePacketResult successResult;
  PrimitivePacketResult failureResult;
  
  primitiveQueue = renderContext->activePrimitiveQueue;
  packetIndexOrAttribute = primitiveQueue->count;
  if (packetIndexOrAttribute + 1 < primitiveQueue->capacity) {
    primitiveQueue->count = packetIndexOrAttribute + 1;
    vertexFieldWriteCursor = (uint32_t *)(primitiveQueue->packetPool + packetIndexOrAttribute);
    primitiveQueue->primaryNodes[packetIndexOrAttribute].packet = (GraphicsPrimitivePacket *)vertexFieldWriteCursor;
    packetIndexOrAttribute = vertex0Projected->texturedPacketAttributes[1];
    *vertexFieldWriteCursor = vertex0Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[1] = packetIndexOrAttribute;
    vertexFieldWriteCursor[7] = vertex0DiffuseColor;
    packetIndexOrAttribute = vertex0Projected->texturedPacketAttributes[3];
    packetAttribute = vertex0Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[2] = vertex0Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[3] = packetIndexOrAttribute;
    vertexFieldWriteCursor[4] = packetAttribute;
    packetIndexOrAttribute = vertex1Projected->texturedPacketAttributes[1];
    vertexFieldWriteCursor[8] = vertex1Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[9] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0xf] = vertex1DiffuseColor;
    packetIndexOrAttribute = vertex1Projected->texturedPacketAttributes[3];
    packetAttribute = vertex1Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[10] = vertex1Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[0xb] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0xc] = packetAttribute;
    packetIndexOrAttribute = vertex2Projected->texturedPacketAttributes[1];
    vertexFieldWriteCursor[0x10] = vertex2Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[0x11] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0x17] = vertex2DiffuseColor;
    packetIndexOrAttribute = vertex2Projected->texturedPacketAttributes[3];
    packetAttribute = vertex2Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[0x12] = vertex2Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[0x13] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0x14] = packetAttribute;
    packetIndexOrAttribute = textureAndMaterialIndices[2];
    packetAttribute = textureAndMaterialIndices[4];
    vertexFieldWriteCursor[5] = *textureAndMaterialIndices;
    vertexFieldWriteCursor[0xd] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0x15] = packetAttribute;
    packetIndexOrAttribute = textureAndMaterialIndices[3];
    packetAttribute = textureAndMaterialIndices[5];
    vertexFieldWriteCursor[6] = textureAndMaterialIndices[1];
    vertexFieldWriteCursor[0xe] = packetIndexOrAttribute;
    vertexFieldWriteCursor[0x16] = packetAttribute;
    paletteModulationColor = 0;
    if (g_TerrainSecondaryPalette != (GraphicsPaletteAsset *)0x0) {
      paletteModulationColor = g_TerrainSecondaryPalette->paletteEntries[textureAndMaterialIndices[7]].
              alternateModulationColorArgb;
    }
    vertexFieldWriteCursor[0x18] = paletteModulationColor;
    materialTextureSet = g_TerrainMaterialTextureSets[textureAndMaterialIndices[6]];
    vertexFieldWriteCursor[0x1a] = g_UiCommandModeGColorVariantFlags;
    vertexFieldWriteCursor[0x19] = (uint32_t)materialTextureSet->entries;
    successResult.noPacket = false;
    successResult.packet = (GraphicsPrimitivePacket *)vertexFieldWriteCursor;
    return successResult;
  }
  failureResult.noPacket = true;
  /* Queue full: the original leaves EAX untouched; callers use the packet only with CF clear. */
  failureResult.packet = (GraphicsPrimitivePacket *)0;
  return failureResult;
}


/* Address: 0x004FFC10.
   Ownership: graphics/render/primitives.
   Purpose: Builds a contiguous 32-bit mask covering every signed Q14 bin between center-minus-radius and center-
   plus-radius. Typed parameters: p2 intervalRadius→DepthIntervalRadius32_V343, p3
   centerDepth→DepthIntervalCenter32_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
DepthBinMask32 __thandor_eax_preserve_ecx_edx
DepthInterval_BuildBinMask(DepthIntervalRadius32 intervalRadius,DepthIntervalCenter32 centerDepth)

{
  uint32_t binMask;
  int binIndex;
  uint32_t currentBinBit;
  
  binMask = 0;
  binIndex = centerDepth - intervalRadius >> 0xe;
  currentBinBit = 1 << ((uint8_t)binIndex & 0x1f);
  do {
    binMask = binMask | currentBinBit;
    binIndex = binIndex + 1;
    currentBinBit = currentBinBit * 2 + (uint32_t)CARRY4(currentBinBit,currentBinBit);
  } while (binIndex <= (centerDepth - intervalRadius) + intervalRadius * 2 >> 0xe);
  return binMask;
}


/* Address: 0x004FFC50.
   Ownership: graphics/render/primitives.
   Purpose: Tests two paired depth-bin masks. CF is set only when both corresponding mask pairs have at least one
   common bit; otherwise CF is clear. Low/high lanes remain distinct from counts and render flags. Typed
   parameters: p2 firstMaskLow→DepthBinMask32_V338, p3 firstMaskHigh→DepthBinMask32_V338, p4
   secondMaskLow→DepthBinMask32_V338, p5 secondMaskHigh→DepthBinMask32_V338. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
DepthBinMasks_Overlap
          (DepthBinMask32 firstMaskLow,DepthBinMask32 firstMaskHigh,DepthBinMask32 secondMaskLow,
          DepthBinMask32 secondMaskHigh)

{
  if (((firstMaskHigh & secondMaskHigh) != 0) && ((firstMaskLow & secondMaskLow) != 0)) {
    return true;
  }
  return false;
}

