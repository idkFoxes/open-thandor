/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/projection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>


/* Implementation ownership: graphics/render/projection. */

/* Address: 0x00486940.
   Bounding-box rejection for a projected triangle: true (CF set) when the point lies strictly to one side of
   all three vertices in X or in Y, i.e. outside the triangle's bounding box; false when it is inside or on it.
   Nothing in the recovered code calls it (it is only listed in function_map.c).
*/
bool GraphicsProjectedTriangle_PointOutsideBounds
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  if ((pointX <= vertexCX || pointX <= vertexBX || pointX <= vertexAX) &&
      (vertexCX <= pointX || vertexBX <= pointX || vertexAX <= pointX)) {
    if ((pointY <= vertexCY || pointY <= vertexBY || pointY <= vertexAY) &&
        (vertexCY <= pointY || vertexBY <= pointY || vertexAY <= pointY)) {
      return false;
    }
  }
  return true;
}

/* Address: 0x00486B00.
   Renders a list of model hierarchies off screen into a new texture-source asset (used for the army preview,
   g_GraphicsOffscreenRenderModelListToTextureSource). The asset holds one direct-colour subresource of
   outputWidth x outputHeight ARGB pixels at +0x220; it is drawn with the software rasterizer's auxiliary
   family into a temporary depth buffer that replaces g_SoftwareDepthBuffer/g_SoftwareDepthEpoch for the call.
   Returns the asset (CF clear), or CF set with the allocation error.
*/
OffscreenRenderResult GraphicsOffscreen_RenderModelListToTextureSource
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputHeight,
          GraphicsPixelDimension outputWidth,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes)

{
  int32_t *savedDepthBuffer;
  int32_t savedDepthEpoch;
  int32_t *assetOrDepthBuffer;
  uint32_t packedTimeOrDate;
  int32_t *zeroCursorOrDepthBuffer;
  GraphicsPrimitiveQueue *queue;
  uint32_t dwordsLeft;
  uint32_t assetBytesOrPixelsLeft;
  int32_t *depthCursor;
  ArenaAllocResult textureAllocation;
  ArenaAllocResult depthAllocation;
  PrimitiveQueueResult queueResult;
  OffscreenRenderResult failureResult;
  
  /* 0x200-byte asset header, one 0x20-byte subresource entry, then the pixels */
  assetBytesOrPixelsLeft = outputWidth * outputHeight * 4 + 0x220;
  textureAllocation = g_MemoryApi.alloc(assetBytesOrPixelsLeft);
  assetOrDepthBuffer = (int32_t *)textureAllocation.payloadOrError;
  if (!textureAllocation.failed) {
    zeroCursorOrDepthBuffer = assetOrDepthBuffer;
    for (dwordsLeft = assetBytesOrPixelsLeft >> 2; dwordsLeft != 0; dwordsLeft--) {
      *zeroCursorOrDepthBuffer = 0;
      zeroCursorOrDepthBuffer++;
    }
    /* common asset prefix: magic, allocation size, format version 1, converter version 0 */
    *assetOrDepthBuffer = ASSET_MAGIC_GFX;
    assetOrDepthBuffer[1] = assetBytesOrPixelsLeft;
    assetOrDepthBuffer[2] = 1;
    assetOrDepthBuffer[3] = 0;
    /* three time/date pairs at +0x10..+0x27 and the two producer names at +0x30 and +0x70 */
    packedTimeOrDate = g_LocaleGetPackedCurrentTime();
    assetOrDepthBuffer[4] = packedTimeOrDate;
    assetOrDepthBuffer[6] = packedTimeOrDate;
    assetOrDepthBuffer[8] = packedTimeOrDate;
    packedTimeOrDate = g_LocaleGetPackedCurrentDate();
    assetOrDepthBuffer[5] = packedTimeOrDate;
    assetOrDepthBuffer[7] = packedTimeOrDate;
    assetOrDepthBuffer[9] = packedTimeOrDate;
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetOrDepthBuffer + 0xc));
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetOrDepthBuffer + 0x1c));
    *(uint8_t *)(assetOrDepthBuffer + 0x40) = 0;
    /* table descriptor at +0xB0: one subresource, no palette banks, entry table at +0x200 */
    assetOrDepthBuffer[0x2c] = 1;
    assetOrDepthBuffer[0x2d] = 0;
    assetOrDepthBuffer[0x2e] = 0x200;
    /* the entry: logical and pixel size, origin 0/0, paletteIndex -1 (ARGB texels), data at +0x220 */
    assetOrDepthBuffer[0x80] = outputWidth;
    assetOrDepthBuffer[0x81] = outputHeight;
    assetOrDepthBuffer[0x86] = outputWidth;
    assetOrDepthBuffer[0x87] = outputHeight;
    assetOrDepthBuffer[0x84] = 0;
    assetOrDepthBuffer[0x85] = 0;
    assetOrDepthBuffer[0x82] = -1;
    assetOrDepthBuffer[0x83] = 0x220;
    depthAllocation = g_MemoryApi.alloc(outputHeight * outputWidth * 4);
    zeroCursorOrDepthBuffer = (int32_t *)depthAllocation.payloadOrError;
    if (!depthAllocation.failed) {
      depthCursor = zeroCursorOrDepthBuffer;
      for (assetBytesOrPixelsLeft = outputHeight * outputWidth & 0x3fffffff; savedDepthEpoch = g_SoftwareDepthEpoch,
          savedDepthBuffer = g_SoftwareDepthBuffer, assetBytesOrPixelsLeft != 0; assetBytesOrPixelsLeft--) {
        *depthCursor = -1;
        depthCursor++;
      }
      /* the original swaps the depth epoch and buffer in with XCHG */
      LOCK();
      g_SoftwareDepthEpoch = -1;
      UNLOCK();
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = zeroCursorOrDepthBuffer;
      Graphics_SetProjectionClipRect(outputHeight,outputWidth,0,0);
      Graphics_SetViewProjectionParameters
                (viewParameters->projectionShift,viewParameters->viewAngle1,
                 viewParameters->viewAngle0,viewParameters->projectionScale,viewParameters->originZ,
                 viewParameters->originY,viewParameters->originX);
      Graphics_SetProjectionViewport(outputHeight,outputWidth,0,0);
      Graphics_SetAuxiliaryOrientation(auxiliaryOrientationAngles[1],*auxiliaryOrientationAngles);
      Graphics_SetSceneBounds
                (sceneExtents->verticalExtent,sceneExtents->horizontalExtent,
                 sceneExtents->verticalExtent,sceneExtents->horizontalExtent,0,0,0,0);
      Graphics_RebuildFrustumPlanes();
      g_GraphicsShadingCompactRecordCount = 0;
      queueResult = GraphicsPrimitiveQueue_ResetGlobal();
      queue = queueResult.queue;
      if (!queueResult.failed) {
        Graphics_SetActivePrimitiveQueue(queue);
        for (; modelCount != 0; modelCount--) {
          if (*modelNodes != NULL) {
            ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
          }
          modelNodes++;
        }
        GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue);
        /* the pixels start at +0x220 (int32 index 0x88); rows are outputWidth pixels long */
        SoftwareRenderer_DrawQueueAuxiliary(outputHeight,outputWidth,assetOrDepthBuffer + 0x88,queue);
      }
      /* restore the caller's depth buffer and epoch, free the temporary one */
      assetOrDepthBuffer = g_SoftwareDepthBuffer;
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = savedDepthBuffer;
      g_SoftwareDepthEpoch = savedDepthEpoch;
      g_MemoryApi.free(assetOrDepthBuffer);
      return THANDOR_BITCAST(uint64_t, OffscreenRenderResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, textureAllocation) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
    g_MemoryApi.free(assetOrDepthBuffer);
    assetOrDepthBuffer = zeroCursorOrDepthBuffer;
  }
  failureResult.failed = true;
  failureResult.allocation = assetOrDepthBuffer;
  return failureResult;
}


/* Address: 0x0050A6A0.
   Point-in-triangle test for the mouse pointer against a projected triangle, used by
   ModelRuntimeNode_HitTestProjectedBoundsAndChildren on the faces of a model's projected bounding box.
   Returns true (CF set) when the pointer is strictly inside. Each edge test is a 64-bit cross product; the
   winding is normalized first (vertex1/vertex2 swapped when the triangle's own cross product is not
   negative) so that "inside" means all three pointer cross products are negative.
*/
bool GraphicsProjectedPoint_IsInsideTriangle(int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2)

{
  int64_t crossPartB;
  int64_t crossPartA;
  GraphicsProjectedPoint2i *orderedVertex2;
  
  /* the sign of the 64-bit sum crossPartA + crossPartB is the sign of its high dword plus the carry */
  crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)vertex2->x +
          (int64_t)(vertex2->y - vertex0->y) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - vertex2->y) * (int64_t)vertex0->x;
  orderedVertex2 = vertex2;
  if (-1 < (int)((int)((uint64_t)crossPartB >> 0x20) + (int)((uint64_t)crossPartA >> 0x20) +
                (uint32_t)CARRY4((uint32_t)crossPartB,(uint32_t)crossPartA))) {
    orderedVertex2 = vertex1;
    vertex1 = vertex2;
  }
  crossPartA = (int64_t)(pointerY - vertex1->y) * (int64_t)orderedVertex2->x +
          (int64_t)(orderedVertex2->y - pointerY) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - orderedVertex2->y) * (int64_t)pointerX;
  if ((int)((int)((uint64_t)crossPartB >> 0x20) + (int)((uint64_t)crossPartA >> 0x20) +
           (uint32_t)CARRY4((uint32_t)crossPartB,(uint32_t)crossPartA)) < 0) {
    crossPartA = (int64_t)(vertex0->y - pointerY) * (int64_t)orderedVertex2->x +
            (int64_t)(orderedVertex2->y - vertex0->y) * (int64_t)pointerX;
    crossPartB = (int64_t)(pointerY - orderedVertex2->y) * (int64_t)vertex0->x;
    if ((int)((int)((uint64_t)crossPartB >> 0x20) + (int)((uint64_t)crossPartA >> 0x20) +
             (uint32_t)CARRY4((uint32_t)crossPartB,(uint32_t)crossPartA)) < 0) {
      crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)pointerX +
              (int64_t)(pointerY - vertex0->y) * (int64_t)vertex1->x;
      crossPartB = (int64_t)(vertex1->y - pointerY) * (int64_t)vertex0->x;
      if ((int)((int)((uint64_t)crossPartB >> 0x20) + (int)((uint64_t)crossPartA >> 0x20) +
               (uint32_t)CARRY4((uint32_t)crossPartB,(uint32_t)crossPartA)) < 0) {
        return true;
      }
    }
  }
  return false;
}

