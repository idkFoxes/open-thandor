/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/projection.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/projection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Dword index of a field (GFX_SUBRESOURCE_*) of the first subresource record of a gfx asset */
#define GFX_SUBRESOURCE_DWORD(field) ((GFX_ASSET_HEADER_SIZE + GFX_SUBRESOURCE_##field) / 4)


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
   Returns the asset, or NULL when either allocation fails.
*/
GraphicsTextureSourceAsset *GraphicsOffscreen_RenderModelListToTextureSource
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
  uint32_t textureAllocationError;
  void *textureAllocationPayload;
  uint32_t depthAllocationError;

  /* 0x200-byte asset header, one 0x20-byte subresource entry, then the pixels */
  assetBytesOrPixelsLeft = outputWidth * outputHeight * 4 + GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET;
  textureAllocationError = g_MemoryApi.alloc(assetBytesOrPixelsLeft,&textureAllocationPayload);
  if (textureAllocationError == 0) {
    assetOrDepthBuffer = (int32_t *)textureAllocationPayload;
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
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetOrDepthBuffer + 12));
    g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetOrDepthBuffer + 28));
    *(uint8_t *)(assetOrDepthBuffer + offsetof(GraphicsTextureSourceAsset, unusedText) / 4) = 0; /* +0x100 unusedText: empty */
    /* table descriptor at +0xB0: one subresource, no palette banks, entry table at +0x200 */
    assetOrDepthBuffer[offsetof(GraphicsTextureSourceAsset, tableDescriptor.subresourceCount) / 4] = 1;
    assetOrDepthBuffer[offsetof(GraphicsTextureSourceAsset, tableDescriptor.paletteBankCount) / 4] = 0;
    assetOrDepthBuffer[offsetof(GraphicsTextureSourceAsset, tableDescriptor.subresourceTableOffset) / 4] = GFX_ASSET_HEADER_SIZE;
    /* the entry: logical and pixel size, origin 0/0, paletteIndex -1 (ARGB texels), data at +0x220 */
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(LOGICAL_WIDTH)] = outputWidth;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(LOGICAL_HEIGHT)] = outputHeight;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(PIXEL_WIDTH)] = outputWidth;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(PIXEL_HEIGHT)] = outputHeight;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(ORIGIN_X)] = 0;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(ORIGIN_Y)] = 0;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(PALETTE_INDEX)] = -1;
    assetOrDepthBuffer[GFX_SUBRESOURCE_DWORD(PIXEL_OFFSET)] = GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET;
    depthAllocationError = g_MemoryApi.alloc(outputHeight * outputWidth * 4,(void **)&zeroCursorOrDepthBuffer);
    if (depthAllocationError != 0) {
      zeroCursorOrDepthBuffer = (int32_t *)depthAllocationError;
    }
    else {
      depthCursor = zeroCursorOrDepthBuffer;
      for (assetBytesOrPixelsLeft = outputHeight * outputWidth & DWORD_COUNT_MASK; savedDepthEpoch = g_SoftwareDepthEpoch,
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
      Graphics_SetSceneBoundsAndColors
                (sceneExtents->verticalExtent,sceneExtents->horizontalExtent,
                 sceneExtents->verticalExtent,sceneExtents->horizontalExtent,0,0,0,0);
      Graphics_RebuildFrustumPlanes();
      g_GraphicsShadingCompactRecordCount = 0;
      queue = GraphicsPrimitiveQueue_ResetGlobal();
      Graphics_SetActivePrimitiveQueue(queue);
      for (; modelCount != 0; modelCount--) {
        if (*modelNodes != NULL) {
          ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
        }
        modelNodes++;
      }
      GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue);
      /* the pixels start at +0x220 (int32 index 0x88); rows are outputWidth pixels long */
      SoftwareRenderer_DrawQueueAuxiliary(outputHeight,outputWidth,
                                      assetOrDepthBuffer + GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET / 4,queue);
      /* restore the caller's depth buffer and epoch, free the temporary one */
      assetOrDepthBuffer = g_SoftwareDepthBuffer;
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = savedDepthBuffer;
      g_SoftwareDepthEpoch = savedDepthEpoch;
      g_MemoryApi.free(assetOrDepthBuffer);
      return (GraphicsTextureSourceAsset *)textureAllocationPayload;
    }
    g_MemoryApi.free(assetOrDepthBuffer);
  }
  return NULL;
}


/* Address: 0x0050A6A0.
   Point-in-triangle test for the mouse pointer against a projected triangle, used by
   ModelRuntimeNode_HitTestProjectedBoundsAndChildren on the faces of a model's projected bounding box.
   Returns true when the pointer is strictly inside. Each edge test is a 64-bit cross product; the
   winding is normalized first (vertex1/vertex2 swapped when the triangle's own cross product is not
   negative) so that "inside" means all three pointer cross products are negative.
*/
bool GraphicsProjectedPoint_IsInsideTriangle(int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2)

{
  int64_t crossPartB;
  int64_t crossPartA;
  GraphicsProjectedPoint2i *orderedVertex2;
  
  /* each test is the sign of the wrapping 64-bit sum crossPartA + crossPartB */
  crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)vertex2->x +
          (int64_t)(vertex2->y - vertex0->y) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - vertex2->y) * (int64_t)vertex0->x;
  orderedVertex2 = vertex2;
  if (0 <= (int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA)) {
    orderedVertex2 = vertex1;
    vertex1 = vertex2;
  }
  crossPartA = (int64_t)(pointerY - vertex1->y) * (int64_t)orderedVertex2->x +
          (int64_t)(orderedVertex2->y - pointerY) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - orderedVertex2->y) * (int64_t)pointerX;
  if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
    crossPartA = (int64_t)(vertex0->y - pointerY) * (int64_t)orderedVertex2->x +
            (int64_t)(orderedVertex2->y - vertex0->y) * (int64_t)pointerX;
    crossPartB = (int64_t)(pointerY - orderedVertex2->y) * (int64_t)vertex0->x;
    if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
      crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)pointerX +
              (int64_t)(pointerY - vertex0->y) * (int64_t)vertex1->x;
      crossPartB = (int64_t)(vertex1->y - pointerY) * (int64_t)vertex0->x;
      if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
        return true;
      }
    }
  }
  return false;
}

