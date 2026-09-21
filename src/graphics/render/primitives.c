/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/primitives.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/primitives.h>

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
  int *piVar1;
  GraphicsPrimitivePacket *pGVar2;
  uint *puVar3;
  uint uVar4;
  GraphicsPrimitiveQueueNode *pGVar5;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor1;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor2;
  GraphicsPrimitivePacket *pGVar6;
  GraphicsPrimitivePacket *primitivePacketCursor3;
  GraphicsPrimitivePacket *primitivePacketCursor2;
  uint uVar7;
  int iVar8;
  dword dVar9;
  dword dVar10;
  GraphicsPrimitiveQueueNode *pGVar11;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor3;
  dword *pdVar12;
  dword *sortWordCursor2;
  GraphicsPrimitiveQueueNode *primitiveQueueNodeCursor4;
  GraphicsPrimitivePacket *pGVar13;
  GraphicsPrimitivePacket *primitivePacketCursor4;
  undefined8 mm0PackedValue0;
  undefined8 mm1PackedValue0;
  undefined8 mm2PackedValue0;
  GraphicsPrimitivePacket *primitivePacketCursor1;
  uint *sortWordCursor1;
  
  dVar10 = queue->count;
  primitiveQueueNodeCursor4 = queue->radixScratchPool;
  primitiveQueueNodeCursor2 = queue->primaryNodes;
  if (dVar10 != 0) {
    dVar9 = dVar10;
    pGVar5 = primitiveQueueNodeCursor2;
    if (dVar10 != 1) {
      do {
        primitivePacketCursor1 = pGVar5->packet;
        if ((primitivePacketCursor1->renderFlags & 0x7000) == 0) {
          uVar7 = ((uint)primitivePacketCursor1->textureEntry | 0xb0000000) -
                  (primitivePacketCursor1->renderFlags & 0x30000000);
        }
        else {
          uVar7 = primitivePacketCursor1->vertices[0].depth +
                  primitivePacketCursor1->vertices[1].depth +
                  primitivePacketCursor1->vertices[2].depth & 0x7fffffff;
        }
        pGVar5->sortKey = uVar7;
        pGVar5 = pGVar5 + 1;
        dVar9 = dVar9 - 1;
      } while (dVar9 != 0);
      pdVar12 = g_PrimitiveRadixBucketWords;
      for (iVar8 = 0x100; dVar9 = dVar10, pGVar5 = primitiveQueueNodeCursor2, iVar8 != 0;
          iVar8 = iVar8 + -1) {
        *pdVar12 = 0;
        pdVar12 = pdVar12 + 1;
      }
      do {
        g_PrimitiveRadixBucketWords[pGVar5->sortKey & 0xff] =
             g_PrimitiveRadixBucketWords[pGVar5->sortKey & 0xff] + 1;
        dVar9 = dVar9 - 1;
        pGVar5 = pGVar5 + 1;
      } while (dVar9 != 0);
      iVar8 = 0x100;
      pdVar12 = g_PrimitiveRadixBucketWords + 0xff;
      pGVar5 = primitiveQueueNodeCursor4;
      do {
        dVar9 = *pdVar12;
        *pdVar12 = (dword)pGVar5;
        pdVar12 = pdVar12 + -1;
        pGVar5 = pGVar5 + dVar9;
        iVar8 = iVar8 + -1;
        dVar9 = dVar10;
        pGVar11 = primitiveQueueNodeCursor2;
      } while (iVar8 != 0);
      do {
        uVar7 = pGVar11->sortKey;
        pGVar2 = pGVar11->packet;
        uVar4 = uVar7 & 0xff;
        puVar3 = (uint *)g_PrimitiveRadixBucketWords[uVar4];
        g_PrimitiveRadixBucketWords[uVar4] = g_PrimitiveRadixBucketWords[uVar4] + 0x10;
        *puVar3 = uVar7;
        puVar3[1] = (uint)pGVar2;
        dVar9 = dVar9 - 1;
        pGVar11 = pGVar11 + 1;
      } while (dVar9 != 0);
      pdVar12 = g_PrimitiveRadixBucketWords;
      for (iVar8 = 0x100; dVar9 = dVar10, pGVar5 = primitiveQueueNodeCursor4, iVar8 != 0;
          iVar8 = iVar8 + -1) {
        *pdVar12 = 0;
        pdVar12 = pdVar12 + 1;
      }
      do {
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + ((pGVar5->sortKey & 0xff00) >> 6));
        *piVar1 = *piVar1 + 1;
        dVar9 = dVar9 - 1;
        pGVar5 = pGVar5 + 1;
      } while (dVar9 != 0);
      iVar8 = 0x100;
      pdVar12 = g_PrimitiveRadixBucketWords + 0xff;
      pGVar5 = primitiveQueueNodeCursor2;
      do {
        dVar9 = *pdVar12;
        *pdVar12 = (dword)pGVar5;
        pdVar12 = pdVar12 + -1;
        pGVar5 = pGVar5 + dVar9;
        iVar8 = iVar8 + -1;
        dVar9 = dVar10;
        pGVar11 = primitiveQueueNodeCursor4;
      } while (iVar8 != 0);
      do {
        uVar7 = pGVar11->sortKey;
        pGVar2 = pGVar11->packet;
        uVar4 = (uVar7 & 0xff00) >> 6;
        sortWordCursor1 = *(uint **)((int)g_PrimitiveRadixBucketWords + uVar4);
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + uVar4);
        *piVar1 = *piVar1 + 0x10;
        *sortWordCursor1 = uVar7;
        sortWordCursor1[1] = (uint)pGVar2;
        dVar9 = dVar9 - 1;
        pGVar11 = pGVar11 + 1;
      } while (dVar9 != 0);
      pdVar12 = g_PrimitiveRadixBucketWords;
      for (iVar8 = 0x100; dVar9 = dVar10, pGVar5 = primitiveQueueNodeCursor2, iVar8 != 0;
          iVar8 = iVar8 + -1) {
        *pdVar12 = 0;
        pdVar12 = pdVar12 + 1;
      }
      do {
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + ((pGVar5->sortKey & 0xff0000) >> 0xe));
        *piVar1 = *piVar1 + 1;
        dVar9 = dVar9 - 1;
        pGVar5 = pGVar5 + 1;
      } while (dVar9 != 0);
      iVar8 = 0x100;
      pdVar12 = g_PrimitiveRadixBucketWords + 0xff;
      primitiveQueueNodeCursor1 = primitiveQueueNodeCursor4;
      do {
        dVar9 = *pdVar12;
        *pdVar12 = (dword)primitiveQueueNodeCursor1;
        pdVar12 = pdVar12 + -1;
        primitiveQueueNodeCursor1 = primitiveQueueNodeCursor1 + dVar9;
        iVar8 = iVar8 + -1;
        dVar9 = dVar10;
        primitiveQueueNodeCursor3 = primitiveQueueNodeCursor2;
      } while (iVar8 != 0);
      do {
        uVar7 = primitiveQueueNodeCursor3->sortKey;
        pGVar2 = primitiveQueueNodeCursor3->packet;
        primitiveQueueNodeCursor3 = primitiveQueueNodeCursor3 + 1;
        uVar4 = (uVar7 & 0xff0000) >> 0xe;
        puVar3 = *(uint **)((int)g_PrimitiveRadixBucketWords + uVar4);
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + uVar4);
        *piVar1 = *piVar1 + 0x10;
        *puVar3 = uVar7;
        puVar3[1] = (uint)pGVar2;
        dVar9 = dVar9 - 1;
      } while (dVar9 != 0);
      pdVar12 = g_PrimitiveRadixBucketWords;
      for (iVar8 = 0x100; dVar9 = dVar10, pGVar5 = primitiveQueueNodeCursor4, iVar8 != 0;
          iVar8 = iVar8 + -1) {
        *pdVar12 = 0;
        pdVar12 = pdVar12 + 1;
      }
      do {
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + ((pGVar5->sortKey & 0xff000000) >> 0x16)
                        );
        *piVar1 = *piVar1 + 1;
        dVar9 = dVar9 - 1;
        pGVar5 = pGVar5 + 1;
      } while (dVar9 != 0);
      iVar8 = 0x100;
      sortWordCursor2 = g_PrimitiveRadixBucketWords + 0xff;
      do {
        dVar9 = *sortWordCursor2;
        *sortWordCursor2 = (dword)primitiveQueueNodeCursor2;
        sortWordCursor2 = sortWordCursor2 + -1;
        primitiveQueueNodeCursor2 = primitiveQueueNodeCursor2 + dVar9;
        iVar8 = iVar8 + -1;
        dVar9 = dVar10;
      } while (iVar8 != 0);
      do {
        uVar7 = primitiveQueueNodeCursor4->sortKey;
        pGVar2 = primitiveQueueNodeCursor4->packet;
        primitiveQueueNodeCursor4 = primitiveQueueNodeCursor4 + 1;
        uVar4 = (uVar7 & 0xff000000) >> 0x16;
        puVar3 = *(uint **)((int)g_PrimitiveRadixBucketWords + uVar4);
        piVar1 = (int *)((int)g_PrimitiveRadixBucketWords + uVar4);
        *piVar1 = *piVar1 + 0x10;
        *puVar3 = uVar7;
        puVar3[1] = (uint)pGVar2;
        dVar9 = dVar9 - 1;
      } while (dVar9 != 0);
    }
    pGVar2 = (GraphicsPrimitivePacket *)queue->primaryNodes;
    queue->traversalCursor = (GraphicsPrimitiveQueueNode *)pGVar2;
    primitivePacketCursor3 = (GraphicsPrimitivePacket *)0xffffffff;
    pGVar6 = (GraphicsPrimitivePacket *)0xffffffff;
    primitivePacketCursor4 = (GraphicsPrimitivePacket *)(queue + 1);
    pGVar13 = (GraphicsPrimitivePacket *)(queue + 1);
    if (halveVertexRgb == GRAPHICS_STATE_DISABLED) {
      do {
        primitivePacketCursor2 = pGVar2;
        primitivePacketCursor2->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)pGVar6;
        primitivePacketCursor2->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)pGVar13;
        dVar10 = dVar10 - 1;
        pGVar6 = primitivePacketCursor2;
        pGVar2 = pGVar13;
        pGVar13 = (GraphicsPrimitivePacket *)&pGVar13->vertices[0].depth;
      } while (dVar10 != 0);
    }
    else {
      do {
        primitivePacketCursor2 = pGVar2;
        primitivePacketCursor2->vertices[0].backendCoord1 =
             (GraphicsPrimitiveBackendCoordinate)primitivePacketCursor3;
        primitivePacketCursor2->vertices[0].backendCoord0 =
             (GraphicsPrimitiveBackendCoordinate)primitivePacketCursor4;
        iVar8 = primitivePacketCursor2->vertices[0].screenY;
        mm0PackedValue0 =
             paddusb((*(uint *)(iVar8 + 0x1c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint *)(iVar8 + 0x1c) & g_VertexColorAlphaPreserveMaskMMX);
        mm1PackedValue0 =
             paddusb((*(uint *)(iVar8 + 0x3c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint *)(iVar8 + 0x3c) & g_VertexColorAlphaPreserveMaskMMX);
        mm2PackedValue0 =
             paddusb((*(uint *)(iVar8 + 0x5c) & g_VertexColorRgbHalveMaskMMX) >> 1,
                     *(uint *)(iVar8 + 0x5c) & g_VertexColorAlphaPreserveMaskMMX);
        *(int *)(iVar8 + 0x1c) = (int)mm0PackedValue0;
        *(int *)(iVar8 + 0x3c) = (int)mm1PackedValue0;
        *(int *)(iVar8 + 0x5c) = (int)mm2PackedValue0;
        dVar10 = dVar10 - 1;
        primitivePacketCursor3 = primitivePacketCursor2;
        pGVar2 = primitivePacketCursor4;
        primitivePacketCursor4 =
             (GraphicsPrimitivePacket *)&primitivePacketCursor4->vertices[0].depth;
      } while (dVar10 != 0);
    }
    primitivePacketCursor2->vertices[0].backendCoord0 = -1;
  }
  return;
}


/* Address: 0x004D0A10.
   Ownership: graphics/render/primitives.
   Purpose: Allocates 0x20 + capacity * 0xA0 bytes for the global primitive queue pool. ABI: CF clear means
   success. CF set means failure or end of iteration.
*/
void GraphicsPrimitiveQueue_AllocateGlobalPool(GraphicsPrimitiveQueueCapacity packetCapacity)

{
  GraphicsPrimitiveQueue *allocatedQueueStorage;
  bool allocationSizeOverflow;
  ArenaAllocEaxCf5 AVar1;
  
  g_PrimitiveQueuePoolCapacity = packetCapacity;
  AVar1 = (*g_MemoryApi.alloc)(packetCapacity * 0xa0 + 0x20);
  allocatedQueueStorage = (GraphicsPrimitiveQueue *)AVar1.eax;
  if (!AVar1.carry) {
    g_PrimitiveQueueStorage = allocatedQueueStorage;
  }
  return;
}


/* Address: 0x004D0A40.
   Ownership: graphics/render/primitives.
   Purpose: Initializes the global variable-length queue: primaryNodes follow the 0x20-byte header,
   radixScratchPool follows primaryNodes, and packetPool follows the scratch nodes.
*/
GraphicsPrimitiveQueueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_ResetGlobal(void)

{
  GraphicsPrimitiveQueue *pGVar1;
  GraphicsPrimitiveQueueEaxCf5 GVar2;
  GraphicsPrimitiveQueue *queueStorage;
  dword poolCapacity;
  
  poolCapacity = g_PrimitiveQueuePoolCapacity;
  pGVar1 = g_PrimitiveQueueStorage;
  g_PrimitiveQueueStorage->capacity = g_PrimitiveQueuePoolCapacity;
  pGVar1->count = 0;
  pGVar1->radixScratchPool = pGVar1->primaryNodes + poolCapacity;
  pGVar1->packetPool =
       (GraphicsPrimitivePacket *)(pGVar1->primaryNodes + poolCapacity + poolCapacity);
  GVar2.carry = false;
  GVar2.queue = pGVar1;
  return GVar2;
}


/* Address: 0x004D0A70.
   Ownership: graphics/render/primitives.
   Purpose: Frees a primitive queue allocation.
*/
void __thandor_preserve_eax GraphicsPrimitiveQueue_Free(GraphicsPrimitiveQueue *queue)

{
  (*g_MemoryApi.free)(queue);
  return;
}


/* Address: 0x004D0A90.
   Ownership: graphics/render/primitives.
   Purpose: Returns queue->count.
*/
dword __thandor_eax_preserve_ecx_edx GraphicsPrimitiveQueue_GetCount(GraphicsPrimitiveQueue *queue)

{
  return queue->count;
}


/* Address: 0x004D0AA0.
   Ownership: graphics/render/primitives.
   Purpose: Returns traversalCursor->packet and advances traversalCursor to node->next. ABI: CF clear means
   success. CF set means failure or end of iteration.
*/
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Begin(GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *in_EAX;
  GraphicsPrimitivePacketEaxCf5 GVar1;
  GraphicsPrimitivePacketEaxCf5 GVar2;
  GraphicsPrimitivePacket *currentTraversalPacket;
  
  if (queue->count != 0) {
    currentTraversalPacket = queue->traversalCursor->packet;
    queue->traversalCursor = queue->traversalCursor->next;
    GVar1.carry = false;
    GVar1.packet = currentTraversalPacket;
    return GVar1;
  }
  GVar2.carry = true;
  GVar2.packet = in_EAX;
  return GVar2;
}


/* Address: 0x004D0AE0.
   Ownership: graphics/render/primitives.
   Purpose: Returns traversalCursor->packet and advances until the 0xFFFFFFFF sentinel. ABI: CF clear means
   success. CF set means failure or end of iteration.
*/
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_Next(GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacketEaxCf5 GVar1;
  GraphicsPrimitivePacketEaxCf5 GVar2;
  GraphicsPrimitiveQueueNode *currentTraversalNode;
  GraphicsPrimitivePacket *currentTraversalPacket;
  
  currentTraversalNode = queue->traversalCursor;
  if (currentTraversalNode != (GraphicsPrimitiveQueueNode *)0xffffffff) {
    currentTraversalPacket = currentTraversalNode->packet;
    queue->traversalCursor = currentTraversalNode->next;
    GVar1.carry = false;
    GVar1.packet = currentTraversalPacket;
    return GVar1;
  }
  GVar2.carry = true;
  GVar2.packet = (GraphicsPrimitivePacket *)0xffffffff;
  return GVar2;
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
  GraphicsPrimitiveScreenCoordinate GVar1;
  GraphicsPrimitiveBackendCoordinate GVar2;
  GraphicsPrimitiveTextureCoordinateFixed GVar3;
  GraphicsPrimitiveTextureCoordinateFixed GVar4;
  GraphicsPrimitivePacket *destinationPacket;
  dword destinationPacketIndex;
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
    GVar1 = vertex1->screenY;
    destinationPacket->vertices[1].screenX = vertex1->screenX;
    destinationPacket->vertices[1].screenY = GVar1;
    GVar1 = vertex2->screenY;
    destinationPacket->vertices[2].screenX = vertex2->screenX;
    destinationPacket->vertices[2].screenY = GVar1;
    copiedBackendCoordinate1 = vertex0->backendCoord1;
    destinationPacket->vertices[0].backendCoord0 = vertex0->backendCoord0;
    destinationPacket->vertices[0].backendCoord1 = copiedBackendCoordinate1;
    GVar2 = vertex1->backendCoord1;
    destinationPacket->vertices[1].backendCoord0 = vertex1->backendCoord0;
    destinationPacket->vertices[1].backendCoord1 = GVar2;
    GVar2 = vertex2->backendCoord1;
    destinationPacket->vertices[2].backendCoord0 = vertex2->backendCoord0;
    destinationPacket->vertices[2].backendCoord1 = GVar2;
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
    GVar3 = triangle->textureV1;
    GVar4 = triangle->textureV2;
    destinationPacket->vertices[0].textureV = triangle->textureV0;
    destinationPacket->vertices[1].textureV = GVar3;
    destinationPacket->vertices[2].textureV = GVar4;
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
  uint existingBlendModeFlags;
  GraphicsPrimitivePacket *packetPool;
  dword queuedPacketCount;
  
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
  dword queuedPacketCount;
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
  GraphicsPrimitiveTextureCoordinateFixed *pGVar1;
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateCursor;
  dword queuedPacketCount;
  GraphicsPrimitivePacket *packetPool;
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordinateField;
  
  queuedPacketCount = queue->count;
  packetPool = queue->packetPool;
  textureCoordinateCursor = &packetPool[queuedPacketCount - 1].vertices[0].textureU;
  *textureCoordinateCursor = *textureCoordinateCursor + deltaU;
  textureCoordinateField = &packetPool[queuedPacketCount - 1].vertices[1].textureU;
  *textureCoordinateField = *textureCoordinateField + deltaU;
  pGVar1 = &packetPool[queuedPacketCount - 1].vertices[2].textureU;
  *pGVar1 = *pGVar1 + deltaU;
  pGVar1 = &packetPool[queuedPacketCount - 1].vertices[0].textureV;
  *pGVar1 = *pGVar1 + deltaV;
  pGVar1 = &packetPool[queuedPacketCount - 1].vertices[1].textureV;
  *pGVar1 = *pGVar1 + deltaV;
  pGVar1 = &packetPool[queuedPacketCount - 1].vertices[2].textureV;
  *pGVar1 = *pGVar1 + deltaV;
  return;
}


/* Address: 0x004D0DA0.
   Ownership: graphics/render/primitives.
   Purpose: Handles graphics primitive queue append terrain textured triangle.
*/
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTerrainSecondarySurfaceTriangleCf
          (dword *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveQueue *pGVar1;
  dword dVar2;
  GraphicsPrimitiveScreenCoordinate GVar3;
  GraphicsPrimitiveBackendCoordinate GVar4;
  GraphicsPrimitiveDepthFixed GVar5;
  dword dVar6;
  uint uVar7;
  GraphicsTextureSet *pGVar8;
  GraphicsPrimitivePacket *in_EAX;
  PackedArgb32 PVar9;
  GraphicsPrimitivePacket *pGVar10;
  GraphicsPrimitivePacketEaxCf5 GVar11;
  GraphicsPrimitivePacketEaxCf5 GVar12;
  
  pGVar1 = renderContext->activePrimitiveQueue;
  dVar2 = pGVar1->count;
  if (dVar2 + 1 < pGVar1->capacity) {
    pGVar1->count = dVar2 + 1;
    pGVar10 = pGVar1->packetPool + dVar2;
    pGVar1->primaryNodes[dVar2].packet = pGVar10;
    GVar3 = vertex0Projected->screenX;
    if ((int)vertex0Projected[1].texturedPacketAttributes[2] < 0) {
      vertex0DiffuseColor = vertex0DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    pGVar10->vertices[0].screenX = vertex0Projected->vertexColorArgb;
    pGVar10->vertices[0].screenY = GVar3;
    pGVar10->vertices[0].diffuseColor = vertex0DiffuseColor;
    GVar4 = *(GraphicsPrimitiveBackendCoordinate *)vertex0Projected[1].reserved00_0B;
    GVar5 = *(GraphicsPrimitiveDepthFixed *)(vertex0Projected[1].reserved00_0B + 4);
    pGVar10->vertices[0].backendCoord0 = vertex0Projected->screenY;
    pGVar10->vertices[0].backendCoord1 = GVar4;
    pGVar10->vertices[0].depth = GVar5;
    GVar3 = vertex1Projected->screenX;
    if ((int)vertex1Projected[1].texturedPacketAttributes[2] < 0) {
      vertex1DiffuseColor = vertex1DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    pGVar10->vertices[1].screenX = vertex1Projected->vertexColorArgb;
    pGVar10->vertices[1].screenY = GVar3;
    pGVar10->vertices[1].diffuseColor = vertex1DiffuseColor;
    GVar4 = *(GraphicsPrimitiveBackendCoordinate *)vertex1Projected[1].reserved00_0B;
    GVar5 = *(GraphicsPrimitiveDepthFixed *)(vertex1Projected[1].reserved00_0B + 4);
    pGVar10->vertices[1].backendCoord0 = vertex1Projected->screenY;
    pGVar10->vertices[1].backendCoord1 = GVar4;
    pGVar10->vertices[1].depth = GVar5;
    GVar3 = vertex2Projected->screenX;
    if ((int)vertex2Projected[1].texturedPacketAttributes[2] < 0) {
      vertex2DiffuseColor = vertex2DiffuseColor & g_UiCommandModeGColorVariantLimit;
    }
    pGVar10->vertices[2].screenX = vertex2Projected->vertexColorArgb;
    pGVar10->vertices[2].screenY = GVar3;
    pGVar10->vertices[2].diffuseColor = vertex2DiffuseColor;
    GVar4 = *(GraphicsPrimitiveBackendCoordinate *)vertex2Projected[1].reserved00_0B;
    GVar5 = *(GraphicsPrimitiveDepthFixed *)(vertex2Projected[1].reserved00_0B + 4);
    pGVar10->vertices[2].backendCoord0 = vertex2Projected->screenY;
    pGVar10->vertices[2].backendCoord1 = GVar4;
    pGVar10->vertices[2].depth = GVar5;
    dVar2 = textureAndMaterialIndices[2];
    dVar6 = textureAndMaterialIndices[4];
    pGVar10->vertices[0].textureU = *textureAndMaterialIndices;
    pGVar10->vertices[1].textureU = dVar2;
    pGVar10->vertices[2].textureU = dVar6;
    dVar2 = textureAndMaterialIndices[3];
    dVar6 = textureAndMaterialIndices[5];
    pGVar10->vertices[0].textureV = textureAndMaterialIndices[1];
    pGVar10->vertices[1].textureV = dVar2;
    pGVar10->vertices[2].textureV = dVar6;
    PVar9 = 0;
    if (g_TerrainPrimaryPalette != (GraphicsPaletteAsset *)0x0) {
      PVar9 = g_TerrainPrimaryPalette->paletteEntries[textureAndMaterialIndices[7]].
              alternateModulationColorArgb;
    }
    pGVar10->renderFlags = 0x6000;
    pGVar10->modulationColor = PVar9;
    pGVar8 = g_TerrainPrimaryTextureSet;
    uVar7 = textureAndMaterialIndices[6];
    pGVar10->textureEntry = (GraphicsTextureSetEntry *)0x0;
    if ((pGVar8 != (GraphicsTextureSet *)0x0) && (uVar7 < pGVar8->subresourceCount)) {
      pGVar10->renderFlags = pGVar10->renderFlags | 0x10000;
      pGVar10->textureEntry = pGVar8->entries + uVar7;
    }
    GVar11.carry = false;
    GVar11.packet = pGVar10;
    return GVar11;
  }
  GVar12.carry = true;
  GVar12.packet = in_EAX;
  return GVar12;
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
GraphicsPrimitivePacketEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsPrimitiveQueue_AppendTexturedTriangleRegs
          (dword *textureAndMaterialIndices,PackedArgb32 vertex2DiffuseColor,
          PackedArgb32 vertex1DiffuseColor,PackedArgb32 vertex0DiffuseColor,
          GraphicsProjectedVertexSource *vertex2Projected,
          GraphicsProjectedVertexSource *vertex1Projected,
          GraphicsProjectedVertexSource *vertex0Projected,
          FrontendModelPointerContextRuntimeState17C *renderContext)

{
  GraphicsPrimitiveQueue *pGVar1;
  dword dVar2;
  dword dVar3;
  GraphicsTextureSet *pGVar4;
  GraphicsPrimitivePacket *in_EAX;
  PackedArgb32 PVar5;
  dword *vertexFieldWriteCursor;
  GraphicsPrimitivePacketEaxCf5 GVar6;
  GraphicsPrimitivePacketEaxCf5 GVar7;
  
  pGVar1 = renderContext->activePrimitiveQueue;
  dVar2 = pGVar1->count;
  if (dVar2 + 1 < pGVar1->capacity) {
    pGVar1->count = dVar2 + 1;
    vertexFieldWriteCursor = (dword *)(pGVar1->packetPool + dVar2);
    pGVar1->primaryNodes[dVar2].packet = (GraphicsPrimitivePacket *)vertexFieldWriteCursor;
    dVar2 = vertex0Projected->texturedPacketAttributes[1];
    *vertexFieldWriteCursor = vertex0Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[1] = dVar2;
    vertexFieldWriteCursor[7] = vertex0DiffuseColor;
    dVar2 = vertex0Projected->texturedPacketAttributes[3];
    dVar3 = vertex0Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[2] = vertex0Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[3] = dVar2;
    vertexFieldWriteCursor[4] = dVar3;
    dVar2 = vertex1Projected->texturedPacketAttributes[1];
    vertexFieldWriteCursor[8] = vertex1Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[9] = dVar2;
    vertexFieldWriteCursor[0xf] = vertex1DiffuseColor;
    dVar2 = vertex1Projected->texturedPacketAttributes[3];
    dVar3 = vertex1Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[10] = vertex1Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[0xb] = dVar2;
    vertexFieldWriteCursor[0xc] = dVar3;
    dVar2 = vertex2Projected->texturedPacketAttributes[1];
    vertexFieldWriteCursor[0x10] = vertex2Projected->texturedPacketAttributes[0];
    vertexFieldWriteCursor[0x11] = dVar2;
    vertexFieldWriteCursor[0x17] = vertex2DiffuseColor;
    dVar2 = vertex2Projected->texturedPacketAttributes[3];
    dVar3 = vertex2Projected->texturedPacketAttributes[4];
    vertexFieldWriteCursor[0x12] = vertex2Projected->texturedPacketAttributes[2];
    vertexFieldWriteCursor[0x13] = dVar2;
    vertexFieldWriteCursor[0x14] = dVar3;
    dVar2 = textureAndMaterialIndices[2];
    dVar3 = textureAndMaterialIndices[4];
    vertexFieldWriteCursor[5] = *textureAndMaterialIndices;
    vertexFieldWriteCursor[0xd] = dVar2;
    vertexFieldWriteCursor[0x15] = dVar3;
    dVar2 = textureAndMaterialIndices[3];
    dVar3 = textureAndMaterialIndices[5];
    vertexFieldWriteCursor[6] = textureAndMaterialIndices[1];
    vertexFieldWriteCursor[0xe] = dVar2;
    vertexFieldWriteCursor[0x16] = dVar3;
    PVar5 = 0;
    if (g_TerrainSecondaryPalette != (GraphicsPaletteAsset *)0x0) {
      PVar5 = g_TerrainSecondaryPalette->paletteEntries[textureAndMaterialIndices[7]].
              alternateModulationColorArgb;
    }
    vertexFieldWriteCursor[0x18] = PVar5;
    pGVar4 = g_TerrainMaterialTextureSets[textureAndMaterialIndices[6]];
    vertexFieldWriteCursor[0x1a] = g_UiCommandModeGColorVariantFlags;
    vertexFieldWriteCursor[0x19] = (dword)pGVar4->entries;
    GVar6.carry = false;
    GVar6.packet = (GraphicsPrimitivePacket *)vertexFieldWriteCursor;
    return GVar6;
  }
  GVar7.carry = true;
  GVar7.packet = in_EAX;
  return GVar7;
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
  uint binMask;
  int binIndex;
  uint currentBinBit;
  
  binMask = 0;
  binIndex = centerDepth - intervalRadius >> 0xe;
  currentBinBit = 1 << ((byte)binIndex & 0x1f);
  do {
    binMask = binMask | currentBinBit;
    binIndex = binIndex + 1;
    currentBinBit = currentBinBit * 2 + (uint)CARRY4(currentBinBit,currentBinBit);
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
DepthBinMasks_OverlapCf
          (DepthBinMask32 firstMaskLow,DepthBinMask32 firstMaskHigh,DepthBinMask32 secondMaskLow,
          DepthBinMask32 secondMaskHigh)

{
  if (((firstMaskHigh & secondMaskHigh) != 0) && ((firstMaskLow & secondMaskLow) != 0)) {
    return true;
  }
  return false;
}

