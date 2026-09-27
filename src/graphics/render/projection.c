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
   Ownership: graphics/render/projection.
   Purpose: Tests whether a projected point lies outside the recovered triangle bounds contract.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedTriangle_PointOutsideBounds
          (GraphicsProjectedCoordinate vertexAY,GraphicsProjectedCoordinate vertexAX,
          GraphicsProjectedCoordinate vertexBY,GraphicsProjectedCoordinate vertexBX,
          GraphicsProjectedCoordinate vertexCY,GraphicsProjectedCoordinate vertexCX,
          GraphicsProjectedCoordinate pointY,GraphicsProjectedCoordinate pointX)

{
  if ((((pointX <= vertexCX) || (pointX <= vertexBX)) || (pointX <= vertexAX)) &&
     (((vertexCX <= pointX || (vertexBX <= pointX)) || (vertexAX <= pointX)))) {
    if ((((pointY <= vertexCY) || (pointY <= vertexBY)) || (pointY <= vertexAY)) &&
       (((vertexCY <= pointY || (vertexBY <= pointY)) || (vertexAY <= pointY)))) {
      return false;
    }
  }
  return true;
}

/* Address: 0x00486B00.
   Ownership: graphics/render/projection.
   Purpose: Handles graphics offscreen render model list to texture source carry-flag result.
   Cross-module calls: Graphics_SetProjectionClipRect [graphics/core/runtime], Graphics_SetViewProjectionParameters
   [graphics/core/runtime], Graphics_SetProjectionViewport [graphics/core/runtime],
   Graphics_SetAuxiliaryOrientation [graphics/core/runtime], Graphics_SetSceneBounds [graphics/core/runtime],
   Graphics_RebuildFrustumPlanes [graphics/core/runtime].
*/
OffscreenRenderResult __thandor_eax_cf_preserve_ecx_edx
GraphicsOffscreen_RenderModelListToTextureSource
          (GraphicsOffscreenSceneExtents *sceneExtents,AngleTurn32 *auxiliaryOrientationAngles,
          GraphicsOffscreenViewParameters *viewParameters,GraphicsPixelDimension outputWidth,
          GraphicsPixelDimension outputHeight,ModelRuntimeCount modelCount,
          ModelRuntimeNode **modelNodes)

{
  int32_t *savedDepthBuffer;
  int32_t savedDepthEpoch;
  int32_t *textureSourceOrDepthBuffer;
  uint32_t packedTimeOrDate;
  int32_t *zeroCursorOrDepthBuffer;
  GraphicsPrimitiveQueue *queue;
  uint32_t dwordsRemaining;
  uint32_t byteSizeOrPixelsRemaining;
  int32_t *depthCursor;
  ArenaAllocResult textureAllocation;
  ArenaAllocResult depthAllocation;
  PrimitiveQueueResult queueResult;
  OffscreenRenderResult failureResult;
  
  byteSizeOrPixelsRemaining = outputHeight * outputWidth * 4 + 0x220;
  textureAllocation = g_MemoryApi.alloc(byteSizeOrPixelsRemaining);
  textureSourceOrDepthBuffer = (int32_t *)textureAllocation.payloadOrError;
  if (!textureAllocation.failed) {
    zeroCursorOrDepthBuffer = textureSourceOrDepthBuffer;
    for (dwordsRemaining = byteSizeOrPixelsRemaining >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
      *zeroCursorOrDepthBuffer = 0;
      zeroCursorOrDepthBuffer = zeroCursorOrDepthBuffer + 1;
    }
    *textureSourceOrDepthBuffer = 0x786667;
    textureSourceOrDepthBuffer[1] = byteSizeOrPixelsRemaining;
    textureSourceOrDepthBuffer[2] = 1;
    textureSourceOrDepthBuffer[3] = 0;
    packedTimeOrDate = g_LocaleGetPackedCurrentTime();
    textureSourceOrDepthBuffer[4] = packedTimeOrDate;
    textureSourceOrDepthBuffer[6] = packedTimeOrDate;
    textureSourceOrDepthBuffer[8] = packedTimeOrDate;
    packedTimeOrDate = g_LocaleGetPackedCurrentDate();
    textureSourceOrDepthBuffer[5] = packedTimeOrDate;
    textureSourceOrDepthBuffer[7] = packedTimeOrDate;
    textureSourceOrDepthBuffer[9] = packedTimeOrDate;
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(textureSourceOrDepthBuffer + 0xc));
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(textureSourceOrDepthBuffer + 0x1c));
    *(uint8_t *)(textureSourceOrDepthBuffer + 0x40) = 0;
    textureSourceOrDepthBuffer[0x2c] = 1;
    textureSourceOrDepthBuffer[0x2d] = 0;
    textureSourceOrDepthBuffer[0x2e] = 0x200;
    textureSourceOrDepthBuffer[0x80] = outputHeight;
    textureSourceOrDepthBuffer[0x81] = outputWidth;
    textureSourceOrDepthBuffer[0x86] = outputHeight;
    textureSourceOrDepthBuffer[0x87] = outputWidth;
    textureSourceOrDepthBuffer[0x84] = 0;
    textureSourceOrDepthBuffer[0x85] = 0;
    textureSourceOrDepthBuffer[0x82] = -1;
    textureSourceOrDepthBuffer[0x83] = 0x220;
    depthAllocation = g_MemoryApi.alloc(outputWidth * outputHeight * 4);
    zeroCursorOrDepthBuffer = (int32_t *)depthAllocation.payloadOrError;
    if (!depthAllocation.failed) {
      depthCursor = zeroCursorOrDepthBuffer;
      for (byteSizeOrPixelsRemaining = outputWidth * outputHeight & 0x3fffffff; savedDepthEpoch = g_SoftwareDepthEpoch,
          savedDepthBuffer = g_SoftwareDepthBuffer, byteSizeOrPixelsRemaining != 0; byteSizeOrPixelsRemaining = byteSizeOrPixelsRemaining - 1) {
        *depthCursor = -1;
        depthCursor = depthCursor + 1;
      }
      LOCK();
      g_SoftwareDepthEpoch = -1;
      UNLOCK();
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = zeroCursorOrDepthBuffer;
      Graphics_SetProjectionClipRect(outputWidth,outputHeight,0,0);
      Graphics_SetViewProjectionParameters
                (viewParameters->projectionShift,viewParameters->viewAngle1,
                 viewParameters->viewAngle0,viewParameters->projectionScale,viewParameters->originZ,
                 viewParameters->originY,viewParameters->originX);
      Graphics_SetProjectionViewport(outputWidth,outputHeight,0,0);
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
        for (; modelCount != 0; modelCount = modelCount - 1) {
          if (*modelNodes != (ModelRuntimeNode *)0x0) {
            ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
          }
          modelNodes = modelNodes + 1;
        }
        GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue);
        SoftwareRenderer_DrawQueueAuxiliary(outputWidth,outputHeight,textureSourceOrDepthBuffer + 0x88,queue);
      }
      textureSourceOrDepthBuffer = g_SoftwareDepthBuffer;
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = savedDepthBuffer;
      g_SoftwareDepthEpoch = savedDepthEpoch;
      g_MemoryApi.free(textureSourceOrDepthBuffer);
      return THANDOR_BITCAST(uint64_t, OffscreenRenderResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, textureAllocation) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
    g_MemoryApi.free(textureSourceOrDepthBuffer);
    textureSourceOrDepthBuffer = zeroCursorOrDepthBuffer;
  }
  failureResult.failed = true;
  failureResult.allocation = textureSourceOrDepthBuffer;
  return failureResult;
}


/* Address: 0x0050A6A0.
   Ownership: graphics/render/projection.
   Purpose: CF set means inside; scalar registers are preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GraphicsProjectedPoint_IsInsideTriangle
          (int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2)

{
  int64_t crossPartB;
  int64_t crossPartA;
  GraphicsProjectedPoint2i *orderedVertex2;
  
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

