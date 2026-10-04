/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/offscreen.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/offscreen.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

GraphicsOffscreenRenderModelListToTextureSourceProc *g_GraphicsOffscreenRenderModelListToTextureSource = &GraphicsOffscreen_RenderModelListToTextureSource;

/* Dword index of a field (GFX_SUBRESOURCE_*) of the first subresource record of a gfx asset */
#define GFX_SUBRESOURCE_DWORD(field) ((GFX_ASSET_HEADER_SIZE + GFX_SUBRESOURCE_##field) / 4)


/* Renders a list of model hierarchies off screen into a new texture-source asset (used for the army preview,
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
  int32_t *assetWords;
  int32_t *zeroCursor;
  int32_t *depthBuffer;
  int32_t *depthCursor;
  int32_t *usedDepthBuffer;
  uint32_t packedTime;
  uint32_t packedDate;
  GraphicsPrimitiveQueue *queue;
  uint32_t dwordsLeft;
  uint32_t assetBytes;
  uint32_t pixelsLeft;
  void *textureAllocationPayload;

  /* 0x200-byte asset header, one 0x20-byte subresource entry, then the pixels */
  assetBytes = outputWidth * outputHeight * 4 + GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET;
  if (g_MemoryApi.alloc(assetBytes,&textureAllocationPayload) != 0) {
    return nullptr;
  }
  assetWords = (int32_t *)textureAllocationPayload;
  zeroCursor = assetWords;
  for (dwordsLeft = assetBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    *zeroCursor = 0;
    zeroCursor++;
  }
  /* common asset prefix: magic, allocation size, format version 1, converter version 0 */
  *assetWords = ASSET_MAGIC_GFX;
  assetWords[1] = assetBytes;
  assetWords[2] = 1;
  assetWords[3] = 0;
  /* three time/date pairs at +0x10..+0x27 and the two producer names at +0x30 and +0x70 */
  packedTime = g_LocaleGetPackedCurrentTime();
  assetWords[4] = packedTime;
  assetWords[6] = packedTime;
  assetWords[8] = packedTime;
  packedDate = g_LocaleGetPackedCurrentDate();
  assetWords[5] = packedDate;
  assetWords[7] = packedDate;
  assetWords[9] = packedDate;
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetWords + 12));
  g_LocaleCopyDefaultComputerLabelUtf16((uint16_t *)(assetWords + 28));
  *(uint8_t *)(assetWords + offsetof(GraphicsTextureSourceAsset, unusedText) / 4) = 0; /* +0x100 unusedText: empty */
  /* table descriptor at +0xB0: one subresource, no palette banks, entry table at +0x200 */
  assetWords[offsetof(GraphicsTextureSourceAsset, tableDescriptor.subresourceCount) / 4] = 1;
  assetWords[offsetof(GraphicsTextureSourceAsset, tableDescriptor.paletteBankCount) / 4] = 0;
  assetWords[offsetof(GraphicsTextureSourceAsset, tableDescriptor.subresourceTableOffset) / 4] = GFX_ASSET_HEADER_SIZE;
  /* the entry: logical and pixel size, origin 0/0, paletteIndex -1 (ARGB texels), data at +0x220 */
  assetWords[GFX_SUBRESOURCE_DWORD(LOGICAL_WIDTH)] = outputWidth;
  assetWords[GFX_SUBRESOURCE_DWORD(LOGICAL_HEIGHT)] = outputHeight;
  assetWords[GFX_SUBRESOURCE_DWORD(PIXEL_WIDTH)] = outputWidth;
  assetWords[GFX_SUBRESOURCE_DWORD(PIXEL_HEIGHT)] = outputHeight;
  assetWords[GFX_SUBRESOURCE_DWORD(ORIGIN_X)] = 0;
  assetWords[GFX_SUBRESOURCE_DWORD(ORIGIN_Y)] = 0;
  assetWords[GFX_SUBRESOURCE_DWORD(PALETTE_INDEX)] = -1;
  assetWords[GFX_SUBRESOURCE_DWORD(PIXEL_OFFSET)] = GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET;
  if (g_MemoryApi.alloc(outputHeight * outputWidth * 4,(void **)&depthBuffer) != 0) {
    g_MemoryApi.free(assetWords);
    return nullptr;
  }
  depthCursor = depthBuffer;
  for (pixelsLeft = outputHeight * outputWidth & DWORD_COUNT_MASK; pixelsLeft != 0; pixelsLeft--) {
    *depthCursor = -1;
    depthCursor++;
  }
  savedDepthEpoch = g_SoftwareDepthEpoch;
  savedDepthBuffer = g_SoftwareDepthBuffer;
  /* the original swaps the depth epoch and buffer in with atomic exchanges (the LOCK/UNLOCK pairs) */
  LOCK();
  g_SoftwareDepthEpoch = -1;
  UNLOCK();
  LOCK();
  UNLOCK();
  g_SoftwareDepthBuffer = depthBuffer;
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
    if (*modelNodes != nullptr) {
      ModelRuntime_CullAndRenderHierarchyRecursive(*modelNodes);
    }
    modelNodes++;
  }
  GraphicsPrimitiveQueue_RadixSortForRendering(GRAPHICS_STATE_DISABLED,queue);
  /* the pixels start at +0x220 (int32 index 0x88); rows are outputWidth pixels long */
  SoftwareRenderer_DrawQueueAuxiliary(outputHeight,outputWidth,
                                  assetWords + GFX_SINGLE_SUBRESOURCE_PIXELS_OFFSET / 4,queue);
  /* restore the caller's depth buffer and epoch, free the temporary one (read back from the global) */
  usedDepthBuffer = g_SoftwareDepthBuffer;
  LOCK();
  UNLOCK();
  g_SoftwareDepthBuffer = savedDepthBuffer;
  g_SoftwareDepthEpoch = savedDepthEpoch;
  g_MemoryApi.free(usedDepthBuffer);
  return (GraphicsTextureSourceAsset *)textureAllocationPayload;
}


