/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/software.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_raster.h"

/* Implementation ownership: graphics/backend/software. */

/* Address: 0x00519320.
   One tick of the credits screen's reveal mask (called from the frontend tick in ui/frontend/scenario.c while
   the credits page is open): pixels already revealed brighten by 0x1F, and the tick's position in a 100-tick
   cycle grows one of the reveal shapes (circles, diagonal wipes, horizontal bands, or everything), step by
   step. At the start of each cycle the mask is cleared and the two pattern counters (capped at 13) advance.
*/
void SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t cycleTicks;
  int previousTick;
  uint32_t shapeStep;
  UiBooleanState32 reverseRows;
  GraphicsTextureLogicalSize logicalSize;
  
  previousTick = maskRuntime->tickCounter;
  maskRuntime->tickCounter++;
  if (maskRuntime->maskPixels != NULL) {
    SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(maskRuntime);
    cycleTicks = previousTick + 20;
    shapeStep = cycleTicks % 100;
    if (shapeStep == 0) {
      if (maskRuntime->incomingSubresource != 0) {
        maskRuntime->outgoingSubresource++;
      }
      maskRuntime->incomingSubresource++;
      SoftwareMaskBuffer_Clear(maskRuntime);
      if (13 < maskRuntime->outgoingSubresource) {
        maskRuntime->outgoingSubresource = 13;
      }
      if (13 < maskRuntime->incomingSubresource) {
        maskRuntime->incomingSubresource = 13;
      }
    }
    else {
      /* the shape of this cycle; centres are (y, x) in mask pixels */
      switch(cycleTicks / 100) {
      case 1:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,80,160,shapeStep,maskRuntime);
        break;
      case 2:
        logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
                  (logicalSize.logicalWidthPixels + logicalSize.logicalHeightPixels,shapeStep,maskRuntime);
        break;
      case 3:
      case 7:
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit(0,shapeStep,maskRuntime);
        break;
      case 4:
      case 9:
        /* any nonzero height selects the reversed band order */
        logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
        reverseRows = logicalSize.logicalHeightPixels;
        SoftwareMaskBuffer_ApplyHorizontalBandBit(reverseRows,shapeStep,maskRuntime);
        break;
      case 5:
        SoftwareMaskBuffer_ApplyHorizontalBandBit(0,shapeStep,maskRuntime);
        break;
      case 6:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,280,160,shapeStep,maskRuntime);
        break;
      case 8:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,32,320,shapeStep,maskRuntime);
        break;
      case 10:
      case 13:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,180,320,shapeStep,maskRuntime);
        break;
      case 11:
        SoftwareMaskBuffer_ApplyCircularRegionBit(1,180,320,shapeStep,maskRuntime);
        break;
      case 12:
        SoftwareMaskBuffer_SetAllPixelsBit(maskRuntime);
      }
    }
  }
  return;
}


/* Address: 0x00485FD0.
   g_GraphicsSetViewportAndClearDepth: fills the rectangle with opaque black and starts a
   new depth epoch instead of clearing a depth buffer.
*/
void SoftwareRenderer_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX)

{
  bool accessFailed;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    g_GraphicsFramebufferFillRectArgb
              (clipMaxY,clipMaxX,clipMinY,clipMinX,clipMaxY,clipMaxX,clipMinY,clipMinX,ARGB8888_ALPHA_MASK /* opaque black */,
               g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
  SoftwareRenderer_AdvanceDepthEpoch();
  return;
}


/* Address: 0x004D1560.
   Queue renderer for 16-bit framebuffers (installed in g_SoftwareDrawQueue by SoftwareRenderer_SetDisplayMode):
   prepares every packet of the queue and draws it with the 16-bit raster handler its render flags select.
*/
void SoftwareRenderer_DrawQueue16Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;

  packet = GraphicsPrimitiveQueue_Begin(queue);
  while (packet != NULL) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    /* handler index * 4 = byte offset into the handler table */
    (*(SoftwareRasterHandler **)((uint8_t *)g_SoftwareRasterHandlers16Bit +
                                  ((packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 10)))
              (clipMaxY,clipMaxX,clipMinY,clipMinX,packet);
    g_PrimitiveDrawCallCount++;
    packet = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x004D15D0.
   Queue renderer for every framebuffer that is not 16-bit, i.e. 32-bit (installed in g_SoftwareDrawQueue by
   SoftwareRenderer_SetDisplayMode): like SoftwareRenderer_DrawQueue16Bit, with g_SoftwareRasterHandlersNon16Bit.
*/
void SoftwareRenderer_DrawQueueNon16Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;

  packet = GraphicsPrimitiveQueue_Begin(queue);
  while (packet != NULL) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    /* handler index * 4 = byte offset into the handler table */
    (*(SoftwareRasterHandler **)((uint8_t *)g_SoftwareRasterHandlersNon16Bit +
                                  ((packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 10)))
              (clipMaxY,clipMaxX,clipMinY,clipMinX,packet);
    g_PrimitiveDrawCallCount++;
    packet = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x004D1640.
   Queue renderer for an off-screen 32-bit target (GraphicsOffscreen_RenderModelListToTextureSource): like
   SoftwareRenderer_DrawQueue16Bit, but it draws into targetBase, whose rows are clipMaxX pixels long, through
   g_SoftwareRasterHandlersAuxiliary with clip minima of 0. Textured packets whose texture is subresource 99 or
   113 of its source are skipped (which textures these are is not known).
*/
void SoftwareRenderer_DrawQueueAuxiliary
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,void *targetBase,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;

  g_SoftwareAuxiliaryTargetBase = targetBase;
  packet = GraphicsPrimitiveQueue_Begin(queue);
  while (packet != NULL) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    if (((packet->renderFlags & GRAPHICS_PRIMITIVE_FLAG_TEXTURED) == 0) ||
       ((packet->textureEntry->subresourceIndex != 99 &&
        (packet->textureEntry->subresourceIndex != 113)))) {
      /* handler index * 4 = byte offset into the handler table */
      (*(SoftwareRasterHandler **)((uint8_t *)g_SoftwareRasterHandlersAuxiliary +
                                    ((packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 10)))
                (clipMaxY,clipMaxX,0,0,packet);
      g_PrimitiveDrawCallCount++;
    }
    packet = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x00486020.
   g_GraphicsDrawPrimitiveQueue: locks the framebuffer and hands the queue to the queue
   renderer chosen for the current pixel depth (g_SoftwareDrawQueue); draws nothing when the lock fails.
*/
void SoftwareRenderer_DrawPrimitiveQueueBridge(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  bool accessFailed;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    g_SoftwareDrawQueue(clipMaxY,clipMaxX,clipMinY,clipMinX,queue);
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x00486050.
   Software backend of g_GraphicsBeginScene (slot 0x00485820): the software renderer needs no scene setup, so it
   only reports success (CLC; RET).
*/
void SoftwareGraphicsDispatch_SuccessNoOp(void)

{
  return;
}

/* Address: 0x00486060.
   Software backend of g_GraphicsEndScene (slot 0x00485824): nothing to finish (a bare RET).
*/
void SoftwareGraphicsDispatch_NoOp(void)

{
  return;
}


/* Address: 0x004A8F80.
   Initial g_GraphicsSetDisplayMode hook (slot 0x004A8ED0) of the software pixel format: allocates the
   SoftwarePixelPackTables once, rebuilds them through g_SoftwareBuildPixelPackTables with the current colour
   scale/bias, and derives the MMX pack/unpack constants (g_SoftwarePixelMmxConstants) of the 16-bit rasterizer
   and blits from g_SoftwarePixelFormatConfig. The mode arguments are not used. Returns true on success;
   false with the arena error in *errorCode when the allocation fails. (The original left the blue unpack
   scale in EAX on success, which no caller reads.)
*/
bool SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t adapterIndex,uint32_t bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  SoftwarePixelPackTables *packTables;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  uint32_t tableAllocationError;

  packTables = g_SoftwarePixelPackTables;
  if (g_SoftwarePixelPackTables == NULL) {
    tableAllocationError = g_MemoryApi.alloc(sizeof(SoftwarePixelPackTables),(void **)&packTables); /* 3 x 256 dwords */
    if (tableAllocationError != 0) {
      *errorCode = tableAllocationError;
      return false;
    }
  }
  g_SoftwarePixelPackTables = packTables;
  g_SoftwareBuildPixelPackTables(g_SoftwareColorScaleQ16,g_SoftwareColorBiasQ16);
  redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
  greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
  blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
  /* unpack scale 1 << (16 - shift - bits) moves a channel to the top of a 16-bit lane */
  blueUnpackScale = 1 << (((16 - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits) & SHIFT_COUNT_MASK);
  g_SoftwarePixelMmxConstants.packedPixelMasks.red =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.green =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
  g_SoftwarePixelMmxConstants.unpackScales.red =
       (SoftwareColorLaneFixed16)
       (1 << (((16 - (char)g_SoftwarePixelFormatConfig.redShift) - redBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.unpackScales.green =
       (SoftwareColorLaneFixed16)
       (1 << (((16 - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
  /* quantize mask: the channel's top `bits` bits of a Q12 lane */
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
       (SoftwareColorLaneFixed16)(((1 << (redBits & SHIFT_COUNT_MASK)) - 1) << ((12 - redBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
       (SoftwareColorLaneFixed16)(((1 << (greenBits & SHIFT_COUNT_MASK)) - 1) << ((12 - greenBits) & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
       (SoftwareColorLaneFixed16)(((1 << (blueBits & SHIFT_COUNT_MASK)) - 1) << ((12 - blueBits) & SHIFT_COUNT_MASK));
  /* PMADDWD weight 1 << (bits + shift - 4) moves the quantized lane to its place in the pixel */
  g_SoftwarePixelMmxConstants.packWeights.red =
       (SoftwareColorLaneFixed16)
       (1 << ((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4 & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.packWeights.green =
       (SoftwareColorLaneFixed16)
       (1 << ((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4 & SHIFT_COUNT_MASK));
  g_SoftwarePixelMmxConstants.packWeights.blue =
       (SoftwareColorLaneFixed16)
       (1 << ((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4 & SHIFT_COUNT_MASK));
  return true;
}


/* Address: 0x004A9110.
   g_SoftwareFramebufferCreate (slot 0x004A8ED8): creates an in-memory framebuffer of width x height pixels in one
   allocation, the 0x10-byte SoftwareFramebufferAccess header (width, height, bytesPerPixel, pixels) followed by
   the pixels, which are zeroed. Used for the off-screen buffers of the display setup (platform/input/devices.c).
   Returns the framebuffer, or NULL when the allocation fails; then the allocator error is stored in
   *outError (the mouse display-mode hook passes it on as its own error value).
*/
SoftwareFramebufferAccess *SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width,uint32_t *outError)

{
  SoftwareFramebufferAccess *framebuffer;
  uint32_t *cursor;
  uint32_t pixelBytes;
  uint32_t dwordsLeft;
  uint32_t frameAllocationError;

  pixelBytes = width * height * bytesPerPixel;
  frameAllocationError = g_MemoryApi.alloc(pixelBytes + sizeof(SoftwareFramebufferAccess),
                                           (void **)&framebuffer);
  if (frameAllocationError != 0) {
    *outError = frameAllocationError;
    return NULL;
  }
  /* the pixels follow the header */
  framebuffer->bytesPerPixel = bytesPerPixel;
  framebuffer->width = width;
  framebuffer->height = height;
  framebuffer->pixels = (uint8_t *)(framebuffer + 1);
  cursor = (uint32_t *)(framebuffer + 1);
  /* whole dwords only; the original also leaves up to 3 trailing bytes as allocated */
  for (dwordsLeft = pixelBytes >> 2; dwordsLeft != 0; dwordsLeft--) {
    *cursor = 0;
    cursor++;
  }
  return framebuffer;
}


/* Address: 0x004A9160.
   g_SoftwareFramebufferDestroy (slot 0x004A8EDC): frees a framebuffer made by SoftwareFramebuffer_Create
   (header and pixels are one allocation).
*/
void SoftwareFramebuffer_Destroy(SoftwareFramebufferAccess *framebuffer)

{
  g_MemoryApi.free(framebuffer);
  return;
}


/* Address: 0x004A9180.
   g_SoftwareBuildPixelPackTables (slot 0x004A8EE8): rebuilds the blue, green and red tables that turn an 8-bit
   channel into its bits of a framebuffer pixel, applying the display settings' colour scale (contrast) and bias
   (brightness), both Q16: value = 64 + (channel - 64) * scale + bias, clamped to 0..255. Called by the
   display-mode hook and by the display settings dialog (ui/controls/misc.c), which is why it also stores the two
   values in g_SoftwareColorScaleQ16/g_SoftwareColorBiasQ16.
*/
void SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16)

{
  uint32_t transformedChannelValueQ16;
  uint32_t channelIndex;
  SoftwarePixelPackTables *packTableCursor;
  
  channelIndex = 0;
  packTableCursor = g_SoftwarePixelPackTables;
  do {
    /* 0x400000 = 64.0 in Q16, the pivot of the scale */
    transformedChannelValueQ16 = (channelIndex - 64) * colorScaleQ16 + (64 << 16) + colorBiasQ16;
    if ((int)transformedChannelValueQ16 < 0) {
      transformedChannelValueQ16 = 0;
    }
    else if ((256 << 16) - 1 < transformedChannelValueQ16) {
      /* 255.0; the original clamps to 0xFF0000, not 0xFFFFFF */
      transformedChannelValueQ16 = 255 << 16;
    }
    /* keep the channel's top `bits` bits of the Q16 value (a byte in bits 16..23) and shift them into place */
    packTableCursor->blue[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.blueBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.blueShift & SHIFT_COUNT_MASK);
    packTableCursor->green[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.greenBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.greenShift & SHIFT_COUNT_MASK);
    packTableCursor->red[0] =
         (transformedChannelValueQ16 >>
         ((24U - (char)g_SoftwarePixelFormatConfig.redBitCount) & SHIFT_COUNT_MASK)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.redShift & SHIFT_COUNT_MASK);
    channelIndex++;
    /* next entry of all three tables */
    packTableCursor = (SoftwarePixelPackTables *)(packTableCursor->blue + 1);
  } while (channelIndex < 256);
  g_SoftwareColorBiasQ16 = colorBiasQ16;
  g_SoftwareColorScaleQ16 = colorScaleQ16;
  return;
}


/* Address: 0x004A93C0.
   Clips and draws one source subresource into a two-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is copied, anything else is blended. A
   paletted texel tests the alpha of the entry's converted pixel (+4) and writes its low word, but blends the
   entry's ARGB colour (+0) with the alpha of that colour. ABI: all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitSourceAlpha16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      if (region.palette != NULL) {
        uint32_t converted = Blit_PalettePixel(&region, *texel);
        if (Blit_IsTransparent(converted)) {
          continue;
        }
        *pixel = Blit_IsOpaque(converted) ? (uint16_t)converted
                                          : Blit_BlendArgb16(Blit_PaletteColor(&region, *texel), *pixel);
      }
      else {
        uint32_t argb = *(const uint32_t *)texel;
        if (Blit_IsTransparent(argb)) {
          continue;
        }
        *pixel = Blit_IsOpaque(argb) ? (uint16_t)Blit_ConvertArgb(argb) : Blit_BlendArgb16(argb, *pixel);
      }
    }
  }
  return false;
}


/* Address: 0x004A9710.
   Clips and draws one source subresource into a four-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is converted through
   g_SoftwarePixelPackTables and written, anything else is blended in 8-bit lanes. Unlike the 16-bit version, a
   paletted texel uses the entry's second dword (+4) for everything: the alpha test, the blend colour, and the
   opaque write, which converts it through the pack tables again. ABI: all registers are preserved and CF is
   cleared.
*/
bool SoftwareTextureSource_BlitSourceAlpha32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != NULL ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_IsOpaque(color) ? Blit_ConvertArgb(color) : Blit_BlendArgb32(color, *pixel);
    }
  }
  return false;
}


/* Address: 0x004A9B20.
   Clips and draws one source subresource into a two-byte framebuffer, with the source RGB at half
   strength (see docs/software_raster.md "Blits"). Alpha 0 is skipped; every other alpha, 0xFF included, blends
   (source lanes (c * 0x101) >> 3 instead of >> 2), so there is no opaque copy. Unlike BlitSourceAlpha16, a
   paletted texel uses the entry's ARGB colour (+0) for both the alpha test and the blend. ABI: all registers are
   preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitHalfSourceRgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != NULL ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(argb)) {
        continue;
      }
      *pixel = Blit_PackLanes16(Blit_BlendLanes(Blit_ArgbLanes(argb, 3), Blit_Unpack16(*pixel), argb >> 24));
    }
  }
  return false;
}


/* Address: 0x004A9E10.
   Clips and draws one source subresource into a four-byte framebuffer, with the source RGB at half
   strength (see docs/software_raster.md "Blits"). Alpha 0 is skipped; every other alpha, 0xFF included, blends,
   so there is no opaque copy. Quirks kept from the original: a paletted texel uses the entry's second dword (+4)
   as its colour, like the other 32-bit blits, and only the paletted path halves the source ((c * 0x101) >> 3);
   the direct-colour path uses >> 2, i.e. it is an ordinary source-alpha blend whose alpha 0xFF still goes
   through the blend tables. ABI: all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitHalfSourceRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int sourceShift;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  sourceShift = region.palette != NULL ? 3 : 2;
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != NULL ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_PackLanes32(
          Blit_BlendLanes(Blit_ArgbLanes(color, sourceShift), Blit_ArgbLanes(*pixel, 2), color >> 24));
    }
  }
  return false;
}


/* Address: 0x004AA170.
   Stretches one direct-color source subresource into a two-byte framebuffer using two-dimensional linear
   interpolation. The source entry must be direct color: paletteIndex == -1. The function computes 8-bit fractional
   source steps from (pixelWidth-1)/(destinationWidth-1) and (pixelHeight-1)/(destinationHeight-1). Four
   neighboring ARGB8888 pixels are blended horizontally and vertically through g_SoftwareBilinearForwardFactors and
   g_SoftwareBilinearInverseFactors. The routine performs no clipping and draws the destination width in
   pixel pairs (an odd last column is left out).
*/
void SoftwareTextureSource_StretchDirectColorBilinear16
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  /* Rewritten from the assembly (0x004AA170-0x004AA3E8) with the MMX lanes in plain C, like the
     32-bit variant. Two destination pixels per step; each blends four ARGB8888 neighbours through
     the word tables g_SoftwareBilinearInverseFactors (0x00420F20, weight of the left/upper neighbour) and
     g_SoftwareBilinearForwardFactors (0x0041FF20, weight of the right/lower one), then packs to 16 bits with the
     runtime quantize masks (0x0041F6E8) and PMADDWD weights (0x0041F6E0), which the display
     setup fills for 555 or 565. */
  const short *firstWeights = (const short *)g_SoftwareBilinearInverseFactors;
  const short *secondWeights = (const short *)g_SoftwareBilinearForwardFactors;
  const uint16_t *quantizeMask = (const uint16_t *)&g_SoftwarePixelMmxConstants.quantizeMasksQ12;
  const short *packWeights = (const short *)&g_SoftwarePixelMmxConstants.packWeights;
  uint8_t *asset = (uint8_t *)sourceAsset;
  GraphicsTextureSourceEntry *entry;
  uint8_t *sourceBase;
  uint8_t *sourceRow;
  uint16_t *destinationRow;
  uint32_t pitchPixels;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t fy;
  uint32_t row;
  uint32_t pair;

  /* only a "gfx" texture source; the entry table holds 0x20-byte GraphicsTextureSourceEntry records */
  if (((uint32_t)sourceAsset->common.magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= sourceAsset->tableDescriptor.subresourceCount)) {
    return;
  }
  entry = (GraphicsTextureSourceEntry *)(asset + sourceAsset->tableDescriptor.subresourceTableOffset +
                                         subresourceIndex * sizeof(GraphicsTextureSourceEntry));
  /* paletteIndex -1: ARGB8888 texels */
  if (((uint32_t)framebuffer->bytesPerPixel != 2) || (entry->paletteIndex != -1)) {
    return;
  }
  pitchPixels = framebuffer->width;
  destinationRow = (uint16_t *)framebuffer->pixels +
                   (destinationY * pitchPixels + destinationX);
  sourceWidth = entry->pixelWidth;
  sourceHeight = entry->pixelHeight;
  /* 8.8 fixed-point source steps */
  stepX = ((sourceWidth - 1) * 256) / (destinationWidth - 1);
  stepY = ((sourceHeight - 1) * 256) / (destinationHeight - 1);
  sourceBase = asset + entry->dataOffset;
  sourceRow = sourceBase;
  fy = 0;
  for (row = destinationHeight; row != 0; row--) {
    uint32_t fx = 0;
    uint32_t *out = (uint32_t *)destinationRow;
    for (pair = destinationWidth >> 1; pair != 0; pair--) {
      uint16_t packed[2];
      int half;
      for (half = 0; half < 2; half++) {
        uint32_t x = fx >> 8;
        const uint8_t *p00 = sourceRow + x * 4;
        const uint8_t *p10 = sourceRow + sourceWidth * 4 + x * 4;
        uint32_t wx = fx & 0xff;
        uint32_t wy = fy & 0xff;
        uint16_t lanes[4];
        int lane;
        unsigned long long madd;
        for (lane = 0; lane < 4; lane++) {
          int a = ((p00[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int b = ((p00[lane + 4] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int c = ((p10[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int d = ((p10[lane + 4] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          short top = (short)(((a * firstWeights[wx * 4 + lane]) >> 16) + ((b * secondWeights[wx * 4 + lane]) >> 16));
          short bottom = (short)(((c * firstWeights[wx * 4 + lane]) >> 16) + ((d * secondWeights[wx * 4 + lane]) >> 16));
          short mixed = (short)(((top * firstWeights[wy * 4 + lane]) >> 16) +
                                ((bottom * secondWeights[wy * 4 + lane]) >> 16));
          int value = (unsigned short)mixed >> 2;
          if (value > ARGB8888_CHANNEL_MAX) value = ARGB8888_CHANNEL_MAX;
          /* PUNPCKLBW x,x; PSLLW 4; PAND quantize mask */
          lanes[lane] = (uint16_t)(((value * COLOR_CHANNEL_TO_WORD_LANE) << 4) & quantizeMask[lane]);
        }
        /* PMADDWD: two signed dword sums, then the two shifted copies are added per word. */
        madd = (unsigned long long)(uint32_t)((short)lanes[0] * packWeights[0] + (short)lanes[1] * packWeights[1]) |
               ((unsigned long long)(uint32_t)((short)lanes[2] * packWeights[2] +
                                            (short)lanes[3] * packWeights[3]) << 32);
        packed[half] = (uint16_t)((uint16_t)(madd >> 8) + (uint16_t)(madd >> 40));
        fx = fx + stepX;
      }
      *out = (uint32_t)packed[0] | ((uint32_t)packed[1] << 16);
      out++;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}


/* Address: 0x004AA3F0.
   Stretches one direct-color source subresource into a four-byte framebuffer using two-dimensional linear
   interpolation. The source entry must be direct color: paletteIndex == -1. The function computes 8-bit fractional
   source steps from (pixelWidth-1)/(destinationWidth-1) and (pixelHeight-1)/(destinationHeight-1). Four
   neighboring ARGB8888 pixels are blended horizontally and vertically through g_SoftwareBilinearForwardFactors and
   g_SoftwareBilinearInverseFactors. The routine performs no clipping and draws the destination width in
   pixel pairs (an odd last column is left out).
*/
void SoftwareTextureSource_StretchDirectColorBilinear32
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  /* Rewritten from the assembly (0x004AA3F0-0x004AA616) with the MMX lanes in plain C. The
     decompiled version (300 lines of lane emulation) left the end-movie frames static. Two
     destination pixels per step; each blends four ARGB8888 neighbours through the word tables
     g_SoftwareBilinearInverseFactors (0x00420F20, weight of the left/upper neighbour) and
     g_SoftwareBilinearForwardFactors (0x0041FF20, weight of the right/lower one), as PMULHW does. */
  const short *firstWeights = (const short *)g_SoftwareBilinearInverseFactors;
  const short *secondWeights = (const short *)g_SoftwareBilinearForwardFactors;
  const unsigned long long clampMask = g_SoftwareBilinearPackedByteClampMask;
  uint8_t *asset = (uint8_t *)sourceAsset;
  GraphicsTextureSourceEntry *entry;
  uint8_t *sourceBase;
  uint8_t *sourceRow;
  uint32_t *destinationRow;
  uint32_t pitchPixels;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t fy;
  uint32_t row;
  uint32_t pair;

  /* only a "gfx" texture source; the entry table holds 0x20-byte GraphicsTextureSourceEntry records */
  if (((uint32_t)sourceAsset->common.magic != ASSET_MAGIC_GFX) ||
      (subresourceIndex >= sourceAsset->tableDescriptor.subresourceCount)) {
    return;
  }
  entry = (GraphicsTextureSourceEntry *)(asset + sourceAsset->tableDescriptor.subresourceTableOffset +
                                         subresourceIndex * sizeof(GraphicsTextureSourceEntry));
  /* paletteIndex -1: ARGB8888 texels */
  if (((uint32_t)framebuffer->bytesPerPixel != 4) || (entry->paletteIndex != -1)) {
    return;
  }
  pitchPixels = framebuffer->width;
  destinationRow = (uint32_t *)framebuffer->pixels +
                   (destinationY * pitchPixels + destinationX);
  sourceWidth = entry->pixelWidth;
  sourceHeight = entry->pixelHeight;
  /* 8.8 fixed-point source steps */
  stepX = ((sourceWidth - 1) * 256) / (destinationWidth - 1);
  stepY = ((sourceHeight - 1) * 256) / (destinationHeight - 1);
  sourceBase = asset + entry->dataOffset;
  sourceRow = sourceBase;
  fy = 0;
  for (row = destinationHeight; row != 0; row--) {
    uint32_t fx = 0;
    uint32_t *out = destinationRow;
    for (pair = destinationWidth >> 1; pair != 0; pair--) {
      uint32_t pixels[2];
      int half;
      for (half = 0; half < 2; half++) {
        uint32_t x = fx >> 8;
        const uint8_t *p00 = sourceRow + x * 4;
        const uint8_t *p10 = sourceRow + sourceWidth * 4 + x * 4;
        uint32_t wx = fx & 0xff;
        uint32_t wy = fy & 0xff;
        uint32_t pixel = 0;
        int lane;
        for (lane = 0; lane < 4; lane++) {
          int a = ((p00[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int b = ((p00[lane + 4] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int c = ((p10[lane] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          int d = ((p10[lane + 4] * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
          short top = (short)(((a * firstWeights[wx * 4 + lane]) >> 16) + ((b * secondWeights[wx * 4 + lane]) >> 16));
          short bottom = (short)(((c * firstWeights[wx * 4 + lane]) >> 16) + ((d * secondWeights[wx * 4 + lane]) >> 16));
          short mixed = (short)(((top * firstWeights[wy * 4 + lane]) >> 16) +
                                ((bottom * secondWeights[wy * 4 + lane]) >> 16));
          int value = (unsigned short)mixed >> 2;
          if (value > ARGB8888_CHANNEL_MAX) value = ARGB8888_CHANNEL_MAX;
          pixel |= (uint32_t)value << (lane * 8);
        }
        pixels[half] = pixel;
        fx = fx + stepX;
      }
      /* PACKUSWB MM0,MM0 duplicates the first pixel into both halves; PAND with the clamp mask,
         then POR with the second pixel shifted into the high half. */
      *(unsigned long long *)out =
           ((((unsigned long long)pixels[0] << 32) | pixels[0]) & clampMask) |
           ((unsigned long long)pixels[1] << 32);
      out += 2;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}


/* Address: 0x004AA630.
   Draws one source subresource into a two-byte framebuffer at an integer scale (see
   docs/software_raster.md "Blits"). Every texel becomes an integerScale x integerScale block, and the entry's
   origin is scaled too. Nothing is clipped at the source: the whole scaled image is walked and each pixel is
   tested against the clip rectangle, which is clamped to the framebuffer. Alpha 0 is skipped, alpha 0xFF is
   written, anything else is blended. A paletted texel tests the alpha of the entry's converted pixel (+4) and
   writes its low word, but blends the entry's ARGB colour (+0), as in BlitSourceAlpha16. Quirks kept: every
   negative paletteIndex means ARGB texels, and the counters are do-while loops, so a scale or image size of 0
   runs them 2^32 times. ABI: all registers are preserved and CF is cleared.
*/
void SoftwareTextureSource_BlitIntegerScaledSourceAlpha16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitScaledImage image;
  const uint8_t *sourceRow;
  uint32_t sourceRowsLeft;
  int y;

  if (!Blit_SetupScaled(sourceAsset, subresourceIndex, framebuffer, 2, integerScale, drawX, drawY, clipMaxY,
                        clipMaxX, clipMinY, clipMinX, &image)) {
    return;
  }
  sourceRow = image.texels;
  y = image.top;
  sourceRowsLeft = image.height;
  do {
    uint32_t repeatRowsLeft = integerScale;
    do {
      if (BlitScaled_RowVisible(&image, y)) {
        const uint8_t *texel = sourceRow;
        uint32_t columnsLeft = image.width;
        int x = image.left;
        do {
          uint32_t blendColor;
          uint32_t color = BlitScaled_TexelColor(&image, texel, &blendColor);
          if (Blit_IsTransparent(color)) {
            x += (int)integerScale;
          }
          else {
            uint32_t repeatColumnsLeft = integerScale;
            do {
              if (BlitScaled_ColumnVisible(&image, x)) {
                uint16_t *pixel = (uint16_t *)BlitScaled_Pixel(framebuffer, 2, x, y);
                if (!Blit_IsOpaque(color)) {
                  *pixel = Blit_BlendArgb16(blendColor, *pixel);
                }
                else {
                  *pixel = image.palette != NULL ? (uint16_t)color : (uint16_t)Blit_ConvertArgb(color);
                }
              }
              x++;
            } while (--repeatColumnsLeft != 0);
          }
          texel += image.texelBytes;
        } while (--columnsLeft != 0);
      }
      y++;
    } while (--repeatRowsLeft != 0);
    sourceRow += image.width * image.texelBytes;
  } while (--sourceRowsLeft != 0);
}


/* Address: 0x004AAA40.
   Four-byte framebuffer version of SoftwareTextureSource_BlitIntegerScaledSourceAlpha16 (integer-scaled,
   per-pixel clip test, alpha 0 skipped, 0xFF written, anything else blended in 8-bit lanes). Unlike
   BlitSourceAlpha32, a paletted texel here uses the 16-bit layout: it tests the alpha of the converted pixel
   (+4), blends the ARGB colour (+0), and writes +4 as it is (not converted again) when opaque. A direct texel is
   converted through g_SoftwarePixelPackTables when opaque. Same quirks as the 16-bit version. ABI: all registers
   are preserved and CF is cleared.
*/
void SoftwareTextureSource_BlitIntegerScaledSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitScaledImage image;
  const uint8_t *sourceRow;
  uint32_t sourceRowsLeft;
  int y;

  if (!Blit_SetupScaled(sourceAsset, subresourceIndex, framebuffer, 4, integerScale, drawX, drawY, clipMaxY,
                        clipMaxX, clipMinY, clipMinX, &image)) {
    return;
  }
  sourceRow = image.texels;
  y = image.top;
  sourceRowsLeft = image.height;
  do {
    uint32_t repeatRowsLeft = integerScale;
    do {
      if (BlitScaled_RowVisible(&image, y)) {
        const uint8_t *texel = sourceRow;
        uint32_t columnsLeft = image.width;
        int x = image.left;
        do {
          uint32_t blendColor;
          uint32_t color = BlitScaled_TexelColor(&image, texel, &blendColor);
          if (Blit_IsTransparent(color)) {
            x += (int)integerScale;
          }
          else {
            uint32_t repeatColumnsLeft = integerScale;
            do {
              if (BlitScaled_ColumnVisible(&image, x)) {
                uint32_t *pixel = (uint32_t *)BlitScaled_Pixel(framebuffer, 4, x, y);
                if (!Blit_IsOpaque(color)) {
                  *pixel = Blit_BlendArgb32(blendColor, *pixel);
                }
                else {
                  *pixel = image.palette != NULL ? color : Blit_ConvertArgb(color);
                }
              }
              x++;
            } while (--repeatColumnsLeft != 0);
          }
          texel += image.texelBytes;
        } while (--columnsLeft != 0);
      }
      y++;
    } while (--repeatRowsLeft != 0);
    sourceRow += image.width * image.texelBytes;
  } while (--sourceRowsLeft != 0);
}


/* Address: 0x004AADE0.
   BlitSourceAlpha16 with an explicit palette bank (see docs/software_raster.md "Blits"). A paletted
   subresource is drawn with paletteBankIndex instead of its own paletteIndex; the entry's paletteIndex must still
   be valid, and paletteBankIndex is only checked (unsigned, < paletteBankCount) after clipping. A direct-colour
   subresource ignores paletteBankIndex. The pixel operation is that of BlitSourceAlpha16, including the palette
   +0/+4 mix. ABI: all registers are preserved and CF is cleared.
*/
void SoftwareTextureSource_BlitSourceAlphaPaletteBank16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return;
  }
  if (region.palette != NULL) {
    if (paletteBankIndex >= sourceAsset->tableDescriptor.paletteBankCount) {
      return;
    }
    /* the palette banks follow the asset header, 256 entries of 8 bytes each */
    region.palette =
        (const uint8_t *)sourceAsset + GFX_ASSET_HEADER_SIZE + paletteBankIndex * GFX_PALETTE_BANK_SIZE;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      if (region.palette != NULL) {
        uint32_t converted = Blit_PalettePixel(&region, *texel);
        if (Blit_IsTransparent(converted)) {
          continue;
        }
        *pixel = Blit_IsOpaque(converted) ? (uint16_t)converted
                                          : Blit_BlendArgb16(Blit_PaletteColor(&region, *texel), *pixel);
      }
      else {
        uint32_t argb = *(const uint32_t *)texel;
        if (Blit_IsTransparent(argb)) {
          continue;
        }
        *pixel = Blit_IsOpaque(argb) ? (uint16_t)Blit_ConvertArgb(argb) : Blit_BlendArgb16(argb, *pixel);
      }
    }
  }
}


/* Address: 0x004AB150.
   BlitSourceAlpha32 with an explicit palette bank (see docs/software_raster.md "Blits"). A paletted
   subresource is drawn with paletteBankIndex instead of its own paletteIndex; the entry's paletteIndex must still
   be valid, and paletteBankIndex is only checked (unsigned, < paletteBankCount) after clipping. A direct-colour
   subresource ignores paletteBankIndex. The pixel operation is that of BlitSourceAlpha32 (the palette entry's +4
   dword used for everything). ABI: all registers are preserved and CF is cleared.
*/
void SoftwareTextureSource_BlitSourceAlphaPaletteBank32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return;
  }
  if (region.palette != NULL) {
    if (paletteBankIndex >= sourceAsset->tableDescriptor.paletteBankCount) {
      return;
    }
    /* the palette banks follow the asset header, 256 entries of 8 bytes each */
    region.palette =
        (const uint8_t *)sourceAsset + GFX_ASSET_HEADER_SIZE + paletteBankIndex * GFX_PALETTE_BANK_SIZE;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t color = region.palette != NULL ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if (Blit_IsTransparent(color)) {
        continue;
      }
      *pixel = Blit_IsOpaque(color) ? Blit_ConvertArgb(color) : Blit_BlendArgb32(color, *pixel);
    }
  }
}


/* Address: 0x004AB4A0.
   Clips and adds one source subresource onto a two-byte framebuffer (saturated add, see
   docs/software_raster.md "Blits"). A texel whose RGB is 0 is skipped whatever its alpha; every other one is
   added lane by lane with unsigned 16-bit saturation (Blit_AddArgb16) and the sum is packed back. Source alpha
   is not a blend factor. A paletted texel uses the entry's ARGB colour (+0), unlike the 32-bit version. ABI: all
   registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitSaturatedAddRgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != NULL ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb16(argb, *pixel, 0);
      }
    }
  }
  return false;
}


/* Address: 0x004AB750.
   Clips and adds one source subresource onto a four-byte framebuffer (saturated add). A texel whose RGB
   is 0 is skipped whatever its alpha; every other one is added byte by byte, clamped at 0xFF (Blit_AddArgb32).
   The alpha byte is summed and written as well. Quirk kept from the original: a paletted texel uses the entry's
   second dword (+4, the converted pixel), not its ARGB colour, both for the RGB-zero test and for the add. ABI:
   all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitSaturatedAddRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != NULL ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 0);
      }
    }
  }
  return false;
}


/* Address: 0x004ABA70.
   Same as SoftwareTextureSource_BlitSaturatedAddRgb16, but the source lanes are halved (PSRLW 1 of
   c * 0x101) before the saturated add. The RGB-zero test uses the unhalved colour. A paletted texel uses the
   entry's ARGB colour (+0). ABI: all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != NULL ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb16(argb, *pixel, 1);
      }
    }
  }
  return false;
}


/* Address: 0x004ABD20.
   Same as SoftwareTextureSource_BlitSaturatedAddRgb32, but the source lanes are halved (PSRLW 1 of
   c * 0x101) before the saturated add. The RGB-zero test uses the unhalved colour, and a paletted texel again
   uses the entry's second dword (+4). ABI: all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitHalfRgbSaturatedAdd32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = region.palette != NULL ? Blit_PalettePixel(&region, *texel) : *(const uint32_t *)texel;
      if ((argb & ARGB8888_RGB_MASK) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 1);
      }
    }
  }
  return false;
}


/* Address: 0x004AC040.
   Clips and draws one source subresource into a two-byte framebuffer, each source channel multiplied by
   the matching channel of modulationArgb8888 first (Blit_Modulate, see docs/software_raster.md "Blits"). The
   modulated colour then goes through the source-alpha rules: alpha 0 skipped, alpha 0xFF converted and written,
   anything else blended. Unlike BlitSourceAlpha16, a paletted texel uses the entry's ARGB colour (+0) for
   everything. Quirk: the modulated alpha is at most 0xFE, so the opaque branch is never taken. ABI: all
   registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitModulatedSourceAlpha16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 2, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint16_t *pixel = (uint16_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = Blit_Modulate(region.palette != NULL ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel,
                                 modulationArgb8888);
      if (Blit_IsTransparent(argb)) {
        continue;
      }
      *pixel = Blit_IsOpaque(argb) ? (uint16_t)Blit_ConvertArgb(argb) : Blit_BlendArgb16(argb, *pixel);
    }
  }
  return false;
}


/* Address: 0x004AC4C0.
   Four-byte framebuffer version of BlitModulatedSourceAlpha16: each source channel is multiplied by the
   matching channel of modulationArgb8888 (Blit_Modulate), then drawn with the source-alpha rules. Unlike
   BlitSourceAlpha32, a paletted texel uses the entry's ARGB colour (+0). Quirk: the modulated alpha is at most
   0xFE, so the opaque branch is never taken. ABI: all registers are preserved and CF is cleared.
*/
bool SoftwareTextureSource_BlitModulatedSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer)

{
  BlitRegion region;
  int x;
  int y;

  if (!Blit_SetupSubresource(sourceAsset, subresourceIndex, framebuffer, 4, drawX, drawY, clipMaxY, clipMaxX,
                             clipMinY, clipMinX, &region)) {
    return false;
  }
  for (y = 0; y < region.height; y++) {
    const uint8_t *texel = region.texels + y * region.texelStride;
    uint32_t *pixel = (uint32_t *)(region.pixels + y * region.pixelStride);
    for (x = 0; x < region.width; x++, texel += region.texelBytes, pixel++) {
      uint32_t argb = Blit_Modulate(region.palette != NULL ? Blit_PaletteColor(&region, *texel) : *(const uint32_t *)texel,
                                 modulationArgb8888);
      if (Blit_IsTransparent(argb)) {
        continue;
      }
      *pixel = Blit_IsOpaque(argb) ? Blit_ConvertArgb(argb) : Blit_BlendArgb32(argb, *pixel);
    }
  }
  return false;
}


/* Address: 0x004AD110.
   Fills the intersection of [rectMinX, rectMaxX) x [rectMinY, rectMaxY), the framebuffer and the clip
   rectangle of a two-byte framebuffer with argb8888: alpha 0 draws nothing, alpha 0xFF writes the colour
   converted through g_SoftwarePixelPackTables, anything else blends it over every pixel (see
   docs/software_raster.md "Blits"). ABI: all registers are preserved and CF is cleared.
*/
void SoftwareFramebuffer_FillRectArgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer)

{
  uint16_t opaque;
  int x;
  int y;

  if (framebuffer->bytesPerPixel != SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT ||
      !Blit_ClipRect(framebuffer, clipMaxY, clipMaxX, clipMinY, clipMinX, &rectMinX, &rectMinY, &rectMaxX,
                     &rectMaxY) ||
      Blit_IsTransparent(argb8888)) {
    return;
  }
  opaque = Blit_IsOpaque(argb8888) ? (uint16_t)Blit_ConvertArgb(argb8888) : 0;
  for (y = rectMinY; y < rectMaxY; y++) {
    uint16_t *pixel = (uint16_t *)framebuffer->pixels + y * (int)framebuffer->width + rectMinX;
    for (x = rectMinX; x < rectMaxX; x++, pixel++) {
      *pixel = Blit_IsOpaque(argb8888) ? opaque : Blit_BlendArgb16(argb8888, *pixel);
    }
  }
}


/* Address: 0x004AD2A0.
   Four-byte framebuffer version of SoftwareFramebuffer_FillRectArgb16: alpha 0 draws nothing, alpha
   0xFF writes the converted colour, anything else is blended in 8-bit lanes (alpha lane included). ABI: all
   registers are preserved and CF is cleared.
*/
void SoftwareFramebuffer_FillRectArgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer)

{
  uint32_t opaque;
  int x;
  int y;

  if (framebuffer->bytesPerPixel != SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_32BIT ||
      !Blit_ClipRect(framebuffer, clipMaxY, clipMaxX, clipMinY, clipMinX, &rectMinX, &rectMinY, &rectMaxX,
                     &rectMaxY) ||
      Blit_IsTransparent(argb8888)) {
    return;
  }
  opaque = Blit_IsOpaque(argb8888) ? Blit_ConvertArgb(argb8888) : 0;
  for (y = rectMinY; y < rectMaxY; y++) {
    uint32_t *pixel = (uint32_t *)framebuffer->pixels + y * (int)framebuffer->width + rectMinX;
    for (x = rectMinX; x < rectMaxX; x++, pixel++) {
      *pixel = Blit_IsOpaque(argb8888) ? opaque : Blit_BlendArgb32(argb8888, *pixel);
    }
  }
}


/* Copies rowCount rows (> 0) of rowBytes bytes (> 0) from sourceRow to destRow, stepping each by its stride.
   Rows whose byte length is a multiple of 4 are copied in dwords, others byte by byte; both copy forwards. */
static void SoftwareFramebuffer_CopyRows(uint8_t *destRow,int destStrideBytes,const uint8_t *sourceRow,
          int sourceStrideBytes,uint32_t rowBytes,uint32_t rowCount)
{
  uint32_t i;

  if ((rowBytes & 3) != 0) {
    for (; rowCount != 0; rowCount--) {
      for (i = 0; i < rowBytes; i++) {
        destRow[i] = sourceRow[i];
      }
      sourceRow = sourceRow + sourceStrideBytes;
      destRow = destRow + destStrideBytes;
    }
  }
  else {
    for (; rowCount != 0; rowCount--) {
      for (i = 0; i < rowBytes >> 2; i++) {
        ((uint32_t *)destRow)[i] = ((const uint32_t *)sourceRow)[i];
      }
      sourceRow = sourceRow + sourceStrideBytes;
      destRow = destRow + destStrideBytes;
    }
  }
}

/* Address: 0x004AD410.
   g_GraphicsFramebufferCopyRegionToOrigin (slot 0x004A8F34): copies the copyWidth x copyHeight rectangle at
   (sourceX, sourceY) of source to the top-left corner of destination. Nothing is copied unless both have the
   same pixel size and destination is at least copyWidth x copyHeight. The rectangle is clipped to source; a
   negative source coordinate moves the destination start instead, so the copy stays aligned. Rows are copied in
   dwords when their byte length allows.
*/
void SoftwareFramebuffer_CopyRegionToOrigin(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX,
          SoftwareFramebufferAccess *destination,SoftwareFramebufferAccess *source)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int sourcePixelOffset;
  int destPixelOffset;
  int sourceStrideBytes;
  int destStrideBytes;
  uint32_t rowBytes;

  pixelBytes = source->bytesPerPixel;
  if (pixelBytes != destination->bytesPerPixel || (int)copyWidth > (int)destination->width ||
      (int)copyHeight > (int)destination->height) {
    return;
  }
  if (sourceY < 0) {
    sourcePixelOffset = 0;
    destPixelOffset = destination->width * -sourceY;
    copyHeight = copyHeight + sourceY;
    sourceY = 0;
  }
  else {
    destPixelOffset = 0;
    sourcePixelOffset = source->width * sourceY;
  }
  if (sourceX < 0) {
    destPixelOffset = destPixelOffset - sourceX;
    copyWidth = copyWidth + sourceX;
    sourceX = 0;
  }
  else {
    sourcePixelOffset = sourcePixelOffset + sourceX;
  }
  if ((int)source->width < (int)(sourceX + copyWidth)) {
    copyWidth = copyWidth - ((sourceX + copyWidth) - source->width);
  }
  if ((int)source->height < (int)(sourceY + copyHeight)) {
    copyHeight = copyHeight - ((sourceY + copyHeight) - source->height);
  }
  sourceStrideBytes = source->width * pixelBytes;
  destStrideBytes = destination->width * pixelBytes;
  rowBytes = pixelBytes * copyWidth;
  if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
    SoftwareFramebuffer_CopyRows(destination->pixels + destPixelOffset * pixelBytes, destStrideBytes,
                                 source->pixels + sourcePixelOffset * pixelBytes, sourceStrideBytes, rowBytes,
                                 copyHeight);
  }
}


/* Address: 0x004AD520.
   g_GraphicsFramebufferCopyOriginToRegion (slot 0x004A8F38), the reverse of SoftwareFramebuffer_CopyRegionToOrigin:
   copies the copyWidth x copyHeight rectangle at the top-left corner of source to (destinationX, destinationY) of
   destination. Nothing is copied unless both have the same pixel size and source is at least copyWidth x
   copyHeight. The rectangle is clipped to destination; a negative destination coordinate moves the source start
   instead. Rows are copied in dwords when their byte length allows.
*/
void SoftwareFramebuffer_CopyOriginToRegion(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          SoftwareFramebufferAccess *source,SoftwareFramebufferAccess *destination)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int destPixelOffset;
  int sourcePixelOffset;
  int destStrideBytes;
  int sourceStrideBytes;
  uint32_t rowBytes;

  pixelBytes = destination->bytesPerPixel;
  if (pixelBytes != source->bytesPerPixel || (int)copyWidth > (int)source->width ||
      (int)copyHeight > (int)source->height) {
    return;
  }
  if (destinationY < 0) {
    destPixelOffset = 0;
    sourcePixelOffset = source->width * -destinationY;
    copyHeight = copyHeight + destinationY;
    destinationY = 0;
  }
  else {
    sourcePixelOffset = 0;
    destPixelOffset = destination->width * destinationY;
  }
  if (destinationX < 0) {
    sourcePixelOffset = sourcePixelOffset - destinationX;
    copyWidth = copyWidth + destinationX;
    destinationX = 0;
  }
  else {
    destPixelOffset = destPixelOffset + destinationX;
  }
  if ((int)destination->width < (int)(destinationX + copyWidth)) {
    copyWidth = copyWidth - ((destinationX + copyWidth) - destination->width);
  }
  if ((int)destination->height < (int)(destinationY + copyHeight)) {
    copyHeight = copyHeight - ((destinationY + copyHeight) - destination->height);
  }
  destStrideBytes = destination->width * pixelBytes;
  sourceStrideBytes = source->width * pixelBytes;
  rowBytes = pixelBytes * copyWidth;
  if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
    SoftwareFramebuffer_CopyRows(destination->pixels + destPixelOffset * pixelBytes, destStrideBytes,
                                 source->pixels + sourcePixelOffset * pixelBytes, sourceStrideBytes, rowBytes,
                                 copyHeight);
  }
}


/* Span of the textured opaque modes 16/24: the nearest texel (paletted or direct colour, wrapped) modulated by
   the interpolated colour; pixel and depth are written where the depth test passes. */
static void Raster16_SpanTexturedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_Modulate(span->color, texel), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004D1710.
   g_SoftwareRasterHandlers16Bit entry 16 (render mode 16): textured, Gouraud-shaded, depth-tested, opaque triangle
   on the 16-bit framebuffer (see Raster16_SpanTexturedOpaque). Handler ABI and the rasterizer as a whole:
   docs/software_raster.md.
*/
void SoftwareRaster16_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedOpaque);
  }
}

/* Texel modulated by the span colour: the Q4 source colour of the textured modes. */
static RasterColor Raster16_TexturedSource(const RasterSpan *span)
{
    RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
    return Raster_Modulate(span->color, texel);
}

/* Textured alpha blend (as mode 17) that also stores the depth when the modulated alpha is
   >= 128 (Raster_AlphaWritesDepth), so opaque parts of the texture occlude later triangles. */
static void Raster16_SpanTexturedAlphaTested(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = Raster16_TexturedSource(span);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
            if (Raster_AlphaWritesDepth(source)) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* Modes 20/22 (Gouraud) and 28/30 (flat); each pair is byte-identical in the original. */
static void Raster16_DrawTexturedAlphaTested(RasterShading shading, GraphicsScreenCoordinate clipMaxY,
                                             GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,
                                             GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet)
{
    RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
    RasterEdges edges;
    RasterGradients gradients;
    RasterTexture texture;

    if (Raster_SetupTriangle(packet, shading, 1, &edges, &gradients)) {
        Raster_SetupTexture(packet, &texture);
        Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedAlphaTested);
    }
}

/* Address: 0x004D2990.
   g_SoftwareRasterHandlers16Bit entry 22 (render mode 22): byte-identical to mode 20 in the original: textured,
   Gouraud-shaded, alpha-blended, with the alpha-tested depth write (Raster16_DrawTexturedAlphaTested).
*/
void SoftwareRaster16_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Span of the textured alpha-blended modes 17/25: the modulated texel is blended with the framebuffer by its
   alpha (g_SoftwareBlendAlphaFactors); the depth buffer is not written. */
static void Raster16_SpanTexturedAlphaBlend(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = Raster16_TexturedSource(span);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004D3E90.
   g_SoftwareRasterHandlers16Bit entries 17 and 48, 49, 52, 54 (render mode 17): textured, Gouraud-shaded,
   depth-tested, alpha-blended triangle without depth write (Raster16_SpanTexturedAlphaBlend).
*/
void SoftwareRaster16_Mode17
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedAlphaBlend);
  }
}

/* Span of the textured additive modes 18/26: the modulated texel is added to the framebuffer with saturation;
   the depth buffer is not written. */
static void Raster16_SpanTexturedAdd(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = Raster16_TexturedSource(span);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(RasterColor_Add(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004D5310.
   g_SoftwareRasterHandlers16Bit entries 18 and 50 (render mode 18): textured, Gouraud-shaded, depth-tested,
   additive triangle without depth write (Raster16_SpanTexturedAdd).
*/
void SoftwareRaster16_Mode18
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedAdd);
  }
}

/* Address: 0x004D6690.
   g_SoftwareRasterHandlers16Bit entry 20 (render mode 20): textured, Gouraud-shaded, depth-tested, alpha-blended
   triangle like mode 17 that also writes the depth where the modulated alpha is >= 128. Byte-identical to mode 22.
*/
void SoftwareRaster16_Mode20
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004D7B90.
   g_SoftwareRasterHandlers16Bit entry 24 (render mode 24): textured, flat-shaded (colour of v0), depth-tested,
   opaque triangle. Same pixel operation as mode 16.
*/
void SoftwareRaster16_Mode24
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedOpaque);
  }
}

/* Address: 0x004D8A90.
   g_SoftwareRasterHandlers16Bit entry 30 (render mode 30): byte-identical to mode 28 in the original: textured,
   flat-shaded, alpha-blended, with the alpha-tested depth write (Raster16_DrawTexturedAlphaTested).
*/
void SoftwareRaster16_Mode30
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004D9C10.
   g_SoftwareRasterHandlers16Bit entries 25 and 56, 57, 60, 62 (render mode 25): textured, flat-shaded (colour of
   v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 17.
*/
void SoftwareRaster16_Mode25
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedAlphaBlend);
  }
}

/* Address: 0x004DAD10.
   g_SoftwareRasterHandlers16Bit entries 26 and 58 (render mode 26): textured, flat-shaded (colour of v0),
   depth-tested, additive triangle. Same pixel operation as mode 18.
*/
void SoftwareRaster16_Mode26
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, Raster16_SpanTexturedAdd);
  }
}

/* Address: 0x004DBD10.
   g_SoftwareRasterHandlers16Bit entry 28 (render mode 28): textured, flat-shaded, depth-tested, alpha-blended
   triangle with the alpha-tested depth write of mode 20. Byte-identical to mode 30.
*/
void SoftwareRaster16_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Span of the untextured opaque modes 0/8: a pixel is written, and its depth stored, where the interpolated
   depth is <= the depth buffer. */
static void Raster16_SpanShadedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            *(uint16_t *)span->pixel = Raster_ShadeToPixel16(span->color);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004DCE90.
   g_SoftwareRasterHandlers16Bit entry 0 (render mode 0): Gouraud-shaded, depth-tested, opaque triangle on the
   16-bit framebuffer (Raster16_SpanShadedOpaque). Handler ABI: docs/software_raster.md (five stack arguments, ret
   0x14; the draw queue passes the prepared packet).
*/
void SoftwareRaster16_Mode00
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedOpaque);
  }
}

/* Alpha blend like mode 1 that also writes depth where the source alpha is >= 128 (the C depth rule
   of modes 4/6/12/14, see Raster_AlphaWritesDepth in software_raster.h). */
static void Raster16_SpanShadedAlphaBlendDepth(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
            if (Raster_AlphaWritesDepth(source)) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* Modes 4/6 (Gouraud) and 12/14 (flat); the original handlers of each pair are byte-identical. */
static void Raster16_DrawAlphaBlendDepth(RasterShading shading, GraphicsScreenCoordinate clipMaxY,
                                         GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,
                                         GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, shading, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedAlphaBlendDepth);
  }
}

/* Address: 0x004DD700.
   g_SoftwareRasterHandlers16Bit entry 6 (render mode 6): Gouraud-shaded, depth-tested, alpha-blended triangle that
   also writes depth where the interpolated alpha is >= 128. Byte-identical to mode 4 in the original, which tests
   a stale MM2 instead of the alpha (docs/software_raster.md, "Stale MM2"); the C keeps the alpha rule.
*/
void SoftwareRaster16_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Span of the untextured alpha-blended modes 1/9: visible pixels are blended with the framebuffer by the
   interpolated vertex alpha (g_SoftwareBlendAlphaFactors); the depth buffer is not written. */
static void Raster16_SpanShadedAlphaBlend(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004DE0C0.
   g_SoftwareRasterHandlers16Bit entries 1 and 32, 33, 36, 38 (render mode 1): Gouraud-shaded, depth-tested,
   alpha-blended triangle without depth write (Raster16_SpanShadedAlphaBlend).
*/
void SoftwareRaster16_Mode01
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedAlphaBlend);
  }
}

/* Span of the untextured additive modes 2/10: visible pixels get the interpolated colour added with
   saturation; the depth buffer is not written. */
static void Raster16_SpanShadedAdd(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack16(*(uint16_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(RasterColor_Add(source, destination), 4, channel);
            *(uint16_t *)span->pixel = Raster_Pack16(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004DEA40.
   g_SoftwareRasterHandlers16Bit entries 2 and 34 (render mode 2): Gouraud-shaded, depth-tested, additive triangle
   without depth write (Raster16_SpanShadedAdd).
*/
void SoftwareRaster16_Mode02
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedAdd);
  }
}

/* Address: 0x004DF2F0.
   g_SoftwareRasterHandlers16Bit entry 4 (render mode 4): same as mode 6 (see SoftwareRaster16_Mode06).
*/
void SoftwareRaster16_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004DFCB0.
   g_SoftwareRasterHandlers16Bit entry 8 (render mode 8): flat-shaded (colour of v0), depth-tested, opaque
   triangle. Same pixel operation as mode 0 with a constant colour.
*/
void SoftwareRaster16_Mode08
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedOpaque);
  }
}

/* Address: 0x004E0250.
   g_SoftwareRasterHandlers16Bit entry 14 (render mode 14): flat-shaded version of mode 6 (colour of v0).
   Byte-identical to mode 12 in the original; same stale-MM2 note as mode 6.
*/
void SoftwareRaster16_Mode14
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004E0940.
   g_SoftwareRasterHandlers16Bit entries 9 and 40, 41, 44, 46 (render mode 9): flat-shaded (colour of v0),
   depth-tested, alpha-blended triangle. Same pixel operation as mode 1 with a constant colour; the depth buffer is
   not written.
*/
void SoftwareRaster16_Mode09
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedAlphaBlend);
  }
}

/* Address: 0x004E0FF0.
   g_SoftwareRasterHandlers16Bit entries 10 and 42 (render mode 10): flat-shaded (colour of v0), depth-tested,
   additive triangle. Same pixel operation as mode 2 with a constant colour; the depth buffer is not written.
*/
void SoftwareRaster16_Mode10
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(2, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster16_SpanShadedAdd);
  }
}

/* Address: 0x004E15D0.
   g_SoftwareRasterHandlers16Bit entry 12 (render mode 12): same as mode 14 (see SoftwareRaster16_Mode14).
*/
void SoftwareRaster16_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* 32-bit span of the textured opaque modes 16/24 (Non16 family): the nearest texel modulated by the
   interpolated colour; pixel and depth are written where the depth test passes. */
static void Raster32_SpanTexturedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_Modulate(span->color, texel), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}

/* Draws a textured triangle into the 32-bit framebuffer with the given shading and span function.
   All textured Non16 handlers share this setup; they differ only in shading and pixel operation. */
static void RasterNon16_DrawTextured(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                     GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                     GraphicsPrimitivePacket *packet, RasterShading shading,
                                     RasterSpanProc drawSpan)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;
  RasterTexture texture;

  if (Raster_SetupTriangle(packet, shading, 1, &edges, &gradients)) {
    Raster_SetupTexture(packet, &texture);
    Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture, drawSpan);
  }
}

/* Address: 0x004E1CC0.
   g_SoftwareRasterHandlersNon16Bit entry 16 (render mode 16): textured, Gouraud-shaded, depth-tested, opaque
   triangle on the 32-bit framebuffer (see SoftwareRaster16_Mode16).
*/
void SoftwareRasterNon16_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedOpaque);
}

/* Span of the Non16 modes 20/22/28/30: visible pixels are blended with the framebuffer by the modulated
   texel alpha; the depth is written only when that alpha lane, as an unsigned word, is >= 0x800 (alpha >= 128,
   or a negative lane). */
static void Raster32_SpanTexturedAlphaTested(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            RasterColor source = Raster_Modulate(span->color, texel);
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            if ((uint16_t)source.lane[RASTER_LANE_ALPHA] >= (128 << 4)) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004E2E00.
   g_SoftwareRasterHandlersNon16Bit entry 22 (render mode 22): textured, Gouraud-shaded, depth-tested,
   alpha-blended triangle with the alpha-tested depth write (Raster32_SpanTexturedAlphaTested). Byte-identical to
   mode 20 in the original.
*/
void SoftwareRasterNon16_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaTested);
}

/* Span of the Non16 modes 17/25: visible pixels are blended with the framebuffer by the modulated texel
   alpha; the depth buffer is not written. */
static void Raster32_SpanTexturedAlphaBlend(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(Raster_Modulate(span->color, texel), destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004E4180.
   g_SoftwareRasterHandlersNon16Bit entries 17 and 48, 49, 52, 54 (render mode 17): textured, Gouraud-shaded,
   depth-tested, alpha-blended triangle without depth write (Raster32_SpanTexturedAlphaBlend).
*/
void SoftwareRasterNon16_Mode17
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaBlend);
}

/* Span of the Non16 modes 18/26: visible pixels get the modulated texel added with saturation; the depth
   buffer is not written. */
static void Raster32_SpanTexturedAdd(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(RasterColor_Add(Raster_Modulate(span->color, texel), destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004E5440.
   g_SoftwareRasterHandlersNon16Bit entries 18 and 50 (render mode 18): textured, Gouraud-shaded, depth-tested,
   additive triangle without depth write (Raster32_SpanTexturedAdd).
*/
void SoftwareRasterNon16_Mode18
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAdd);
}

/* Address: 0x004E65C0.
   g_SoftwareRasterHandlersNon16Bit entry 20 (render mode 20): byte-identical to mode 22 (see
   SoftwareRasterNon16_Mode22).
*/
void SoftwareRasterNon16_Mode20
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaTested);
}

/* Address: 0x004E7940.
   g_SoftwareRasterHandlersNon16Bit entry 24 (render mode 24): textured, flat-shaded (colour of v0), depth-tested,
   opaque triangle. Same pixel operation as mode 16.
*/
void SoftwareRasterNon16_Mode24
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedOpaque);
}

/* Address: 0x004E86D0.
   g_SoftwareRasterHandlersNon16Bit entry 30 (render mode 30): textured, flat-shaded (colour of v0), depth-tested,
   alpha-blended triangle with the alpha-tested depth write of modes 20/22. Byte-identical to mode 28 in the
   original.
*/
void SoftwareRasterNon16_Mode30
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaTested);
}

/* Address: 0x004E96D0.
   g_SoftwareRasterHandlersNon16Bit entries 25 and 56, 57, 60, 62 (render mode 25): textured, flat-shaded (colour
   of v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 17.
*/
void SoftwareRasterNon16_Mode25
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaBlend);
}

/* Address: 0x004EA610.
   g_SoftwareRasterHandlersNon16Bit entries 26 and 58 (render mode 26): textured, flat-shaded (colour of v0),
   depth-tested, additive triangle. Same pixel operation as mode 18.
*/
void SoftwareRasterNon16_Mode26
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAdd);
}

/* Address: 0x004EB3E0.
   g_SoftwareRasterHandlersNon16Bit entry 28 (render mode 28): byte-identical to mode 30 (see
   SoftwareRasterNon16_Mode30).
*/
void SoftwareRasterNon16_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaTested);
}

/* 32-bit span of the untextured opaque modes (Non16 0/8, Aux 0/8): pixel and depth are written where the
   depth test passes. */
static void Raster32_SpanShadedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(span->color, 6, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004EC3E0.
   g_SoftwareRasterHandlersNon16Bit entry 0 (render mode 0): Gouraud-shaded, depth-tested, opaque triangle on the
   32-bit framebuffer (see SoftwareRaster16_Mode00).
*/
void SoftwareRasterNon16_Mode00
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedOpaque);
  }
}

/* Span of the Non16 modes 4/6/12/14: alpha blend like mode 1 that also writes the depth of pixels whose
   source alpha is >= 128 (Raster_AlphaWritesDepth). The original tests a stale MM2 instead, which it never
   loads in these modes (see docs/software_raster.md); the C rule is deliberate. */
static void Raster32_SpanShadedAlphaBlendDepth(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            if (Raster_AlphaWritesDepth(source)) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* Modes 4 and 6 (0x004EE4A0 and 0x004ECB90, byte-identical in the original): Gouraud-shaded with
   Raster32_SpanShadedAlphaBlendDepth. */
static void RasterNon16_DrawShadedAlphaBlendDepth(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                                  GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                                  GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAlphaBlendDepth);
  }
}

/* Address: 0x004ECB90.
   g_SoftwareRasterHandlersNon16Bit entry 6 (render mode 6): Gouraud-shaded, depth-tested, alpha-blended triangle
   (like mode 1) that also writes depth where the source alpha is >= 128; see
   RasterNon16_DrawShadedAlphaBlendDepth.
*/
void SoftwareRasterNon16_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawShadedAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Span of the Non16 modes 1/9: blend with the framebuffer by the interpolated alpha; the depth buffer is
   not written. */
static void Raster32_SpanShadedAlphaBlend(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(Raster_BlendAlpha(source, destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004ED440.
   g_SoftwareRasterHandlersNon16Bit entries 1 and 32, 33, 36, 38 (render mode 1): Gouraud-shaded, depth-tested,
   alpha-blended triangle; the depth buffer is not written (see SoftwareRaster16_Mode01).
*/
void SoftwareRasterNon16_Mode01
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAlphaBlend);
  }
}

/* Span of the Non16 modes 2/10: the interpolated colour is added with saturation; the depth buffer is not
   written. */
static void Raster32_SpanShadedAdd(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            RasterColor destination = Raster_Unpack32(*(uint32_t *)span->pixel);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(RasterColor_Add(source, destination), 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004EDCB0.
   g_SoftwareRasterHandlersNon16Bit entries 2 and 34 (render mode 2): Gouraud-shaded, depth-tested, additive
   triangle; the depth buffer is not written (see SoftwareRaster16_Mode02).
*/
void SoftwareRasterNon16_Mode02
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAdd);
  }
}

/* Address: 0x004EE4A0.
   g_SoftwareRasterHandlersNon16Bit entry 4 (render mode 4): byte-identical to mode 6, see
   RasterNon16_DrawShadedAlphaBlendDepth.
*/
void SoftwareRasterNon16_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawShadedAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004EED50.
   g_SoftwareRasterHandlersNon16Bit entry 8 (render mode 8): flat-shaded (v0's colour), depth-tested, opaque
   triangle (flat version of SoftwareRasterNon16_Mode00).
*/
void SoftwareRasterNon16_Mode08
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedOpaque);
  }
}

/* Modes 12 and 14 (0x004F02D0 and 0x004EF230, byte-identical in the original): the flat-shaded (v0's
   colour) version of RasterNon16_DrawShadedAlphaBlendDepth, with the same replacement of the stale-MM2 test. */
static void RasterNon16_DrawFlatAlphaBlendDepth(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                                GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                                GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAlphaBlendDepth);
  }
}

/* Address: 0x004EF230.
   g_SoftwareRasterHandlersNon16Bit entry 14 (render mode 14): flat-shaded version of modes 4/6: alpha-blended,
   depth written where the source alpha is >= 128; see RasterNon16_DrawFlatAlphaBlendDepth.
*/
void SoftwareRasterNon16_Mode14
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawFlatAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004EF810.
   g_SoftwareRasterHandlersNon16Bit entries 9 and 40, 41, 44, 46 (render mode 9): flat-shaded (v0's colour),
   depth-tested, alpha-blended triangle; the depth buffer is not written (flat version of
   SoftwareRasterNon16_Mode01).
*/
void SoftwareRasterNon16_Mode09
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAlphaBlend);
  }
}

/* Address: 0x004EFDB0.
   g_SoftwareRasterHandlersNon16Bit entries 10 and 42 (render mode 10): flat-shaded (v0's colour), depth-tested,
   additive triangle; the depth buffer is not written (flat version of SoftwareRasterNon16_Mode02).
*/
void SoftwareRasterNon16_Mode10
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_FramebufferTarget(4, clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_FLAT, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedAdd);
  }
}

/* Address: 0x004F02D0.
   g_SoftwareRasterHandlersNon16Bit entry 12 (render mode 12): byte-identical to mode 14, see
   RasterNon16_DrawFlatAlphaBlendDepth.
*/
void SoftwareRasterNon16_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawFlatAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Texture of the auxiliary textured modes. The span function of modes 20/22/28/30 needs the long
   edge's current U (see RasterAux_SpanTexturedPrestepDepth), so the walker's edges travel along
   with the texture; span->texture points at `texture`, the first member. */
typedef struct RasterAuxTexture {
    RasterTexture texture;
    const RasterEdges *edges;
} RasterAuxTexture;

/* The textured pixel of the auxiliary family: nearest texel modulated by the colour, saturated to
   ARGB. */
static __inline uint32_t RasterAux_TexturedPixel(const RasterSpan *span)
{
    RasterColor texel = Raster_TexelLanes(Raster_FetchTexel(span->texture, span->u, span->v));
    int channel[RASTER_LANE_COUNT];
    Raster_LanesToBytes(Raster_Modulate(span->color, texel), 4, channel);
    return Raster_Pack32(channel);
}

/* Modes 16 and 24: depth-tested textured pixel, colour and depth written. */
static void RasterAux_SpanTexturedOpaque(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            *(uint32_t *)span->pixel = RasterAux_TexturedPixel(span);
            *span->depth = span->depthValue;
        }
        RasterSpan_Next(span);
    }
}

/* Modes 17/18 and 25/26: depth-tested textured pixel, depth not written. The original reads and
   unpacks the destination pixel but does not use it: the "blend" is a plain write. */
static void RasterAux_SpanTexturedNoDepthWrite(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            *(uint32_t *)span->pixel = RasterAux_TexturedPixel(span);
        }
        RasterSpan_Next(span);
    }
}

/* Modes 20/22 and 28/30: like 17, but the depth is written when the span's U prestep,
   (prestep * uStepX) >> 12, is >= 0x800 as an unsigned value. The original tests MM2 here, meant
   to be the modulated alpha, but MM2 still holds that prestep from the span setup; the decision
   is therefore the same for the whole span. The prestep is recovered as the span's first U minus
   the long edge's U. */
static void RasterAux_SpanTexturedPrestepDepth(RasterSpan *span)
{
    const RasterAuxTexture *texture = (const RasterAuxTexture *)span->texture;
    uint32_t uPrestep = (uint32_t)span->u - (uint32_t)texture->edges->longU;
    int writeDepth = uPrestep >= Q12_ONE / 2;

    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            *(uint32_t *)span->pixel = RasterAux_TexturedPixel(span);
            if (writeDepth) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* All auxiliary textured modes: set up the triangle and its texture, then walk it with
   `drawSpan`. Target and depth rows are clipMaxX pixels long. */
static void RasterAux_DrawTexturedTriangle(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                           GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                           const GraphicsPrimitivePacket *packet, RasterShading shading,
                                           RasterSpanProc drawSpan)
{
    RasterTarget target = Raster_AuxiliaryTarget(clipMaxY, clipMaxX, clipMinY, clipMinX);
    RasterEdges edges;
    RasterGradients gradients;
    RasterAuxTexture texture;

    if (Raster_SetupTriangle(packet, shading, 1, &edges, &gradients)) {
        Raster_SetupTexture(packet, &texture.texture);
        texture.edges = &edges;
        Raster_WalkTriangle(&target, packet, &edges, &gradients, &texture.texture, drawSpan);
    }
}

/* Address: 0x004F08B0.
   g_SoftwareRasterHandlersAuxiliary entry 16 (render mode 16): textured, Gouraud-shaded, depth-tested, opaque
   triangle into the off-screen target of SoftwareRenderer_DrawQueueAuxiliary (see SoftwareRaster16_Mode16).
*/
void SoftwareRasterAux_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedOpaque);
}

/* Address: 0x004F19E0.
   g_SoftwareRasterHandlersAuxiliary entry 22 (render mode 22): byte-identical to mode 20 (see
   SoftwareRasterAux_Mode20).
*/
void SoftwareRasterAux_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedPrestepDepth);
}

/* Address: 0x004F2BD0.
   g_SoftwareRasterHandlersAuxiliary entries 17 and 48, 49, 52, 54 (render mode 17): textured, Gouraud-shaded,
   depth-tested triangle without depth write. Unlike the framebuffer families there is no alpha blend: the pixel is
   overwritten.
*/
void SoftwareRasterAux_Mode17
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedNoDepthWrite);
}

/* Address: 0x004F3D40.
   g_SoftwareRasterHandlersAuxiliary entries 18 and 50 (render mode 18): byte-identical to mode 17 (no additive
   blend in this family; see SoftwareRasterAux_Mode17).
*/
void SoftwareRasterAux_Mode18
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedNoDepthWrite);
}

/* Address: 0x004F4EB0.
   g_SoftwareRasterHandlersAuxiliary entry 20 (render mode 20): textured, Gouraud-shaded, depth-tested triangle.
   The pixel is overwritten; the depth is written only for spans whose U prestep is >= 0x800 (see
   RasterAux_SpanTexturedPrestepDepth). Also used for mode 22.
*/
void SoftwareRasterAux_Mode20
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedPrestepDepth);
}

/* Address: 0x004F60A0.
   g_SoftwareRasterHandlersAuxiliary entry 24 (render mode 24): flat-shaded version of mode 16 (textured,
   depth-tested, opaque).
*/
void SoftwareRasterAux_Mode24
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                                 RasterAux_SpanTexturedOpaque);
}

/* Address: 0x004F6E20.
   g_SoftwareRasterHandlersAuxiliary entry 30 (render mode 30): byte-identical to mode 28 (see
   SoftwareRasterAux_Mode28).
*/
void SoftwareRasterAux_Mode30
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                                 RasterAux_SpanTexturedPrestepDepth);
}

/* Address: 0x004F7C70.
   g_SoftwareRasterHandlersAuxiliary entries 25 and 56, 57, 60, 62 (render mode 25): flat-shaded version of mode 17
   (textured, depth-tested, pixel overwritten, no depth write).
*/
void SoftwareRasterAux_Mode25
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                                 RasterAux_SpanTexturedNoDepthWrite);
}

/* Address: 0x004F8A30.
   g_SoftwareRasterHandlersAuxiliary entries 26 and 58 (render mode 26): byte-identical to mode 25 (see
   SoftwareRasterAux_Mode25).
*/
void SoftwareRasterAux_Mode26
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                                 RasterAux_SpanTexturedNoDepthWrite);
}

/* Address: 0x004F97F0.
   g_SoftwareRasterHandlersAuxiliary entry 28 (render mode 28): flat-shaded version of mode 20 (textured,
   depth-tested, pixel overwritten, depth written for spans whose U prestep is >= 0x800). Also used for mode 30.
*/
void SoftwareRasterAux_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                                 RasterAux_SpanTexturedPrestepDepth);
}

/* Address: 0x004FA640.
   g_SoftwareRasterHandlersAuxiliary entry 0 (render mode 0): Gouraud-shaded, depth-tested, opaque triangle into
   the off-screen target of SoftwareRenderer_DrawQueueAuxiliary (see SoftwareRaster16_Mode00). Target and depth
   rows are clipMaxX pixels long.
*/
void SoftwareRasterAux_Mode00
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterTarget target = Raster_AuxiliaryTarget(clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, RASTER_SHADE_GOURAUD, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, Raster32_SpanShadedOpaque);
  }
}

/* Draws an untextured triangle into the auxiliary target with the given span function. The Aux
   modes 1..14 differ only in shading and span. */
static __forceinline void RasterAux_DrawUntextured(GraphicsScreenCoordinate clipMaxY,
                                                   GraphicsScreenCoordinate clipMaxX,
                                                   GraphicsScreenCoordinate clipMinY,
                                                   GraphicsScreenCoordinate clipMinX,
                                                   GraphicsPrimitivePacket *packet, RasterShading shading,
                                                   RasterSpanProc drawSpan)
{
  RasterTarget target = Raster_AuxiliaryTarget(clipMaxY, clipMaxX, clipMinY, clipMinX);
  RasterEdges edges;
  RasterGradients gradients;

  if (Raster_SetupTriangle(packet, shading, 0, &edges, &gradients)) {
    Raster_WalkTriangle(&target, packet, &edges, &gradients, NULL, drawSpan);
  }
}

/* Span of the Aux modes 4/6/12/14: opaque shaded write; the depth buffer is written only where the
   alpha rule allows it (Raster_AlphaWritesDepth, the C replacement of the original's stale-MM2 test). */
static void Raster32_SpanShadedOpaqueAlphaDepth(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            RasterColor source = RasterColor_ShiftRight(span->color, 2);
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(source, 4, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
            if (Raster_AlphaWritesDepth(source)) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004FADD0.
   g_SoftwareRasterHandlersAuxiliary entry 6 (render mode 6): Gouraud-shaded, depth-tested, opaque colour write;
   the depth is written only by pixels that pass Raster_AlphaWritesDepth (the original tests a stale MM2 instead,
   see docs/software_raster.md). Byte-identical to mode 4.
*/
void SoftwareRasterAux_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Span of the Aux modes 1/2/9/10: depth-tested colour write without depth write. Unlike the framebuffer
   families there is no blending: the original loads the destination pixel but never uses it. */
static void Raster32_SpanShadedWrite(RasterSpan *span)
{
    for (; span->count > 0; span->count--) {
        if (span->depthValue <= *span->depth) {
            int channel[RASTER_LANE_COUNT];
            Raster_LanesToBytes(span->color, 6, channel);
            *(uint32_t *)span->pixel = Raster_Pack32(channel);
        }
        RasterSpan_Next(span);
    }
}

/* Address: 0x004FB5A0.
   g_SoftwareRasterHandlersAuxiliary entries 1 and 32, 33, 36, 38 (render mode 1): Gouraud-shaded, depth-tested
   colour write without depth write (Raster32_SpanShadedWrite). Byte-identical to mode 2.
*/
void SoftwareRasterAux_Mode01
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedWrite);
}

/* Address: 0x004FBD70.
   g_SoftwareRasterHandlersAuxiliary entries 2 and 34 (render mode 2): same code as SoftwareRasterAux_Mode01 (no
   additive blend in this family).
*/
void SoftwareRasterAux_Mode02
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedWrite);
}

/* Address: 0x004FC540.
   g_SoftwareRasterHandlersAuxiliary entry 4 (render mode 4): same code as SoftwareRasterAux_Mode06.
*/
void SoftwareRasterAux_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Address: 0x004FCD10.
   g_SoftwareRasterHandlersAuxiliary entry 8 (render mode 8): flat-shaded version of SoftwareRasterAux_Mode00
   (depth-tested, opaque, writes depth).
*/
void SoftwareRasterAux_Mode08
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedOpaque);
}

/* Address: 0x004FD1E0.
   g_SoftwareRasterHandlersAuxiliary entry 14 (render mode 14): flat-shaded version of SoftwareRasterAux_Mode06
   (opaque colour write, depth written by the alpha rule). Byte-identical to mode 12.
*/
void SoftwareRasterAux_Mode14
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Address: 0x004FD6F0.
   g_SoftwareRasterHandlersAuxiliary entries 9 and 40, 41, 44, 46 (render mode 9): flat-shaded version of
   SoftwareRasterAux_Mode01 (colour write, no depth write). Byte-identical to mode 10.
*/
void SoftwareRasterAux_Mode09
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedWrite);
}

/* Address: 0x004FDC00.
   g_SoftwareRasterHandlersAuxiliary entries 10 and 42 (render mode 10): same code as SoftwareRasterAux_Mode09.
*/
void SoftwareRasterAux_Mode10
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedWrite);
}

/* Address: 0x004FE110.
   g_SoftwareRasterHandlersAuxiliary entry 12 (render mode 12): same code as SoftwareRasterAux_Mode14.
*/
void SoftwareRasterAux_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Address: 0x004FE620.
   Software hook in front of g_GraphicsSetDisplayMode (see SoftwareRenderer_InstallDisplayModeHook): after the
   chained mode switch succeeds it picks the queue renderer for the new pixel depth, replaces the depth buffer
   with one of the new size and rebuilds the MMX colour constants from the new pixel format. Returns true on
   success; false with the error in *errorCode when the chained hook or the depth-buffer allocation fails.
   (The original left the blue unpack scale in EAX on success, which no caller reads.)
*/
bool SoftwareRenderer_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width,uint32_t *errorCode)

{
  int32_t *previousDepthBuffer;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  uint32_t depthAllocationError;
  void *depthAllocationPayload;

  if (!g_SoftwareChainedSetDisplayMode(adapterIndex,bitsPerPixel,height,width,errorCode)) {
    return false;
  }
  {
    g_SoftwareDepthRowStrideBytes = width * 4; /* one int32 depth value per pixel */
    if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
      g_SoftwareDrawQueue = SoftwareRenderer_DrawQueue16Bit;
    }
    else {
      g_SoftwareDrawQueue = SoftwareRenderer_DrawQueueNon16Bit;
    }
    depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * height,&depthAllocationPayload);
    previousDepthBuffer = g_SoftwareDepthBuffer;
    if (depthAllocationError != 0) {
      *errorCode = depthAllocationError;
      return false;
    }
    {
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = (int32_t *)depthAllocationPayload;
      g_MemoryApi.free(previousDepthBuffer);
      g_SoftwareDepthEpoch = 0;
      /* quantize mask: the top <bits> bits of a Q12 colour channel */
      redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
           (SoftwareColorLaneFixed16)(((1 << (redBits & SHIFT_COUNT_MASK)) - 1) << ((12 - redBits) & SHIFT_COUNT_MASK));
      greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
           (SoftwareColorLaneFixed16)(((1 << (greenBits & SHIFT_COUNT_MASK)) - 1) << ((12 - greenBits) & SHIFT_COUNT_MASK));
      blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
           (SoftwareColorLaneFixed16)(((1 << (blueBits & SHIFT_COUNT_MASK)) - 1) << ((12 - blueBits) & SHIFT_COUNT_MASK));
      /* pack weight: moves a quantized Q12 channel to its bit position in the native pixel */
      g_SoftwarePixelMmxConstants.packWeights.red =
           (SoftwareColorLaneFixed16)
           (1 << (((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packWeights.green =
           (SoftwareColorLaneFixed16)
           (1 << (((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packWeights.blue =
           (SoftwareColorLaneFixed16)
           (1 << (((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4) & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.packedPixelMasks.red =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.green =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
      /* unpack scale: moves a native channel to the top of a 16-bit word lane */
      g_SoftwarePixelMmxConstants.unpackScales.red =
           (SoftwareColorLaneFixed16)
           (1 << ((16 - (char)g_SoftwarePixelFormatConfig.redShift) - redBits & SHIFT_COUNT_MASK));
      g_SoftwarePixelMmxConstants.unpackScales.green =
           (SoftwareColorLaneFixed16)
           (1 << ((16 - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits & SHIFT_COUNT_MASK));
      blueUnpackScale = 1 << ((16 - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits & SHIFT_COUNT_MASK);
      g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
    }
  }
  return true;
}


/* Address: 0x004FE7D0.
   Hooks the software renderer into the display-mode switch: chains SoftwareRenderer_SetDisplayMode in front of
   the current g_GraphicsSetDisplayMode, picks the queue renderer for the current pixel depth and allocates the
   depth buffer (one int32 per pixel) for the current framebuffer size. Returns 0 on success, or the arena error
   when the allocation fails.
*/
uint32_t __cdecl SoftwareRenderer_InstallDisplayModeHook(void)

{
  int32_t *allocatedDepthBuffer;
  uint32_t depthAllocationError;

  g_SoftwareChainedSetDisplayMode = g_GraphicsSetDisplayMode;
  g_SoftwareDepthRowStrideBytes = g_FramebufferWidth * 4;
  LOCK();
  g_GraphicsSetDisplayMode = SoftwareRenderer_SetDisplayMode;
  UNLOCK();
  if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    g_SoftwareDrawQueue = SoftwareRenderer_DrawQueue16Bit;
  }
  else {
    g_SoftwareDrawQueue = SoftwareRenderer_DrawQueueNon16Bit;
  }
  depthAllocationError = g_MemoryApi.alloc(g_SoftwareDepthRowStrideBytes * g_FramebufferHeight,
                                           (void **)&allocatedDepthBuffer);
  if (depthAllocationError == 0) {
    g_SoftwareDepthBuffer = allocatedDepthBuffer;
    g_SoftwareDepthEpoch = 0;
    return 0;
  }
  return depthAllocationError;
}


/* One byte of the cross-fade in SoftwareTexture_BilinearBlendScaleSubresources: both images and the
   factor are widened to (c * 0x101) >> 2 (PUNPCKLBW + PSRLW 2), then
   (b * (unity - f) + a * f) >> 16 per product (PMULHW), >> 4 (PSRLW) and saturated (PACKUSWB). */
static uint8_t SoftwareTexture_CrossFadeByte(uint8_t a, uint8_t b, uint8_t factor, short unity)
{
    short wideA = (short)((a * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    short wideB = (short)((b * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    short wideFactor = (short)((factor * COLOR_CHANNEL_TO_WORD_LANE) >> 2);
    uint16_t sum = (uint16_t)(Raster_MulHigh(wideB, (short)(unity - wideFactor)) + Raster_MulHigh(wideA, wideFactor));
    sum = (uint16_t)(sum >> 4);
    return (uint8_t)(sum > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : sum);
}

/* Fills g_SoftwarePixelIntensityToNativeColorLut256 with native grey pixels of the current
   framebuffer format. The table runs from white down to black: entry i is intensity 255 - i. */
static void SoftwareTexture_BuildIntensityLut(void)
{
    const SoftwarePixelFormatConfig *format = &g_SoftwarePixelFormatConfig;
    uint32_t entry;
    for (entry = 0; entry < 256; entry++) {
        uint32_t intensity = 255 - entry;
        g_SoftwarePixelIntensityToNativeColorLut256[entry] =
            ((intensity >> ((8 - format->redBitCount) & 31)) << (format->redShift & 31)) |
            ((intensity >> ((8 - format->greenBitCount) & 31)) << (format->greenShift & 31)) |
            ((intensity >> ((8 - format->blueBitCount) & 31)) << (format->blueShift & 31));
    }
}

/* Bilinear sample of an 8-bit image at column xFixed (8.8 fixed point) between `row` and the row
   below it. Horizontal: the two neighbours, widened like SoftwareTexture_CrossFadeByte, weighted by
   g_SoftwareBilinearPackedInterpolationWeights256[fraction] (PMADDWD, high half kept). Vertical:
   the two results times the first lane of the row weights (PMULHW), summed, >> 2, clamped to 255. */
static uint32_t SoftwareTexture_SampleIntensity(const uint8_t *row, uint32_t sourceWidth, uint32_t xFixed,
                                                short upperWeight, short lowerWeight)
{
    const short *weights = g_SoftwareBilinearPackedInterpolationWeights256[xFixed & 0xff];
    const uint8_t *upper = row + (xFixed >> 8);
    const uint8_t *lower = upper + sourceWidth;
    uint32_t upperSum = (uint32_t)(((upper[0] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[0] + ((upper[1] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[1]);
    uint32_t lowerSum = (uint32_t)(((lower[0] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[0] + ((lower[1] * COLOR_CHANNEL_TO_WORD_LANE) >> 2) * weights[1]);
    uint16_t sum = (uint16_t)(Raster_MulHigh((short)(upperSum >> 16), upperWeight) +
                      Raster_MulHigh((short)(lowerSum >> 16), lowerWeight));
    uint32_t intensity = (uint32_t)(sum >> 2);
    return intensity > ARGB8888_CHANNEL_MAX ? ARGB8888_CHANNEL_MAX : intensity;
}

/* Address: 0x00518CE0.
   Draws the cross-fade of two 8-bit subresources of a texture source, scaled to
   destinationWidth x destinationHeight at (destinationLeft, destinationTop) of the software
   framebuffer (16 or 32 bit), as grey levels. Called by
   UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren (ui/controls/text.c).
   1. blendedSourcePixels = per-pixel cross-fade of B (sourceSubresourceIndexB) to A through the
      factor image blendFactorPixels, eight pixels per step (SoftwareTexture_CrossFadeByte).
   2. g_SoftwarePixelIntensityToNativeColorLut256 is rebuilt for the current pixel format.
   3. Each destination pixel is a bilinear sample of the blended image (8.8 fixed-point steps
      (size - 1) * 256 / (destinationSize - 1)), looked up in that table.
   Nothing is drawn unless the asset is a texture source, both indices are valid and both entries
   are paletted (paletteIndex >= 0). Original quirks kept: B's size is compared with itself, so A
   is assumed to be as large as B; the loops are do-while, so fewer than 8 source pixels or a zero
   destination size run 2^32 times, and a destination size of 1 divides by zero; the scale reads
   one row below the blended image.
*/
void SoftwareTexture_BilinearBlendScaleSubresources
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft,
          uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,
          GraphicsSubresourceIndex sourceSubresourceIndexB,int *graphicsTextureAsset,
          int *framebufferAccess)
{
  const GraphicsTextureSourceAsset *asset = (const GraphicsTextureSourceAsset *)graphicsTextureAsset;
  const SoftwareFramebufferAccess *framebuffer = (const SoftwareFramebufferAccess *)framebufferAccess;
  const short *unity = (const short *)&g_SoftwareBlendUnityWordLanesQ14;
  const GraphicsTextureSourceEntry *entries;
  const GraphicsTextureSourceEntry *entryA;
  const GraphicsTextureSourceEntry *entryB;
  const uint8_t *sourceA;
  const uint8_t *sourceB;
  const uint8_t *factor;
  uint8_t *blended;
  uint8_t *destinationRow;
  uint32_t sourceWidth;
  uint32_t sourceHeight;
  uint32_t blocks;
  uint32_t stepX;
  uint32_t stepY;
  uint32_t yFixed;
  uint32_t rowsLeft;
  int pixelBytes;
  int lane;

  if (asset == NULL || asset->common.magic != ASSET_MAGIC_GFX ||
      sourceSubresourceIndexB >= asset->tableDescriptor.subresourceCount ||
      sourceSubresourceIndexA >= asset->tableDescriptor.subresourceCount) {
    return;
  }
  entries = (const GraphicsTextureSourceEntry *)((const uint8_t *)asset +
                                                 asset->tableDescriptor.subresourceTableOffset);
  entryA = &entries[sourceSubresourceIndexA];
  entryB = &entries[sourceSubresourceIndexB];
  if (entryB->paletteIndex < 0 || entryA->paletteIndex < 0) {
    return;
  }
  sourceWidth = entryB->pixelWidth;
  sourceHeight = entryB->pixelHeight;

  /* 1. cross-fade B -> A */
  sourceA = (const uint8_t *)asset + entryA->dataOffset;
  sourceB = (const uint8_t *)asset + entryB->dataOffset;
  factor = (const uint8_t *)blendFactorPixels;
  blended = (uint8_t *)blendedSourcePixels;
  blocks = (sourceHeight * sourceWidth) >> 3;
  do {
    for (lane = 0; lane < 8; lane++) {
      blended[lane] = SoftwareTexture_CrossFadeByte(sourceA[lane], sourceB[lane], factor[lane], unity[lane & 3]);
    }
    sourceA += 8;
    sourceB += 8;
    factor += 8;
    blended += 8;
  } while (--blocks != 0);

  /* 2. grey levels of the current pixel format */
  SoftwareTexture_BuildIntensityLut();

  /* 3. bilinear scale into the framebuffer */
  stepX = (uint32_t)(((unsigned long long)(sourceWidth - 1) << 8) / (uint32_t)(destinationWidth - 1));
  stepY = (uint32_t)(((unsigned long long)(sourceHeight - 1) << 8) / (uint32_t)(destinationHeight - 1));
  /* framebuffer->width is the row pitch in pixels; anything but 2 bytes per pixel is drawn as 4 */
  pixelBytes = framebuffer->bytesPerPixel == 2 ? 2 : 4;
  destinationRow = framebuffer->pixels + (destinationTop * (int)framebuffer->width + destinationLeft) * pixelBytes;
  yFixed = 0;
  rowsLeft = destinationHeight;
  do {
    const uint8_t *row = (const uint8_t *)blendedSourcePixels + (yFixed >> 8) * sourceWidth;
    short upperWeight = (short)g_SoftwareBilinearInverseFactors[yFixed & 0xff].blue;
    short lowerWeight = (short)g_SoftwareBilinearForwardFactors[yFixed & 0xff].blue;
    uint32_t xFixed = 0;
    uint32_t column = 0;
    do {
      uint32_t color = g_SoftwarePixelIntensityToNativeColorLut256[
          SoftwareTexture_SampleIntensity(row, sourceWidth, xFixed, upperWeight, lowerWeight)];
      if (pixelBytes == 2) {
        ((uint16_t *)destinationRow)[column] = (uint16_t)color;
      }
      else {
        ((uint32_t *)destinationRow)[column] = color;
      }
      xFixed += stepX;
    } while (++column != destinationWidth);
    yFixed += stepY;
    destinationRow += (int)framebuffer->width * pixelBytes;
  } while (--rowsLeft != 0);
}


/* Address: 0x004D16D0.
   Starts a new depth epoch instead of clearing the depth buffer: the epoch in the top byte of every depth
   value drops by one (0x01000000), so every new depth is nearer than any value left from earlier epochs. Only
   when the epoch underflows or reaches zero is the width*height depth buffer really cleared (0xFFFFFFFF) and the
   epoch reset to 0xFF000000.
*/
void SoftwareRenderer_AdvanceDepthEpoch(void)

{
  int pixelsRemaining;
  int32_t *depthValueCursor;
  bool depthEpochWrapped;

  depthEpochWrapped = (uint32_t)g_SoftwareDepthEpoch < SOFTWARE_DEPTH_EPOCH_STEP;
  g_SoftwareDepthEpoch = g_SoftwareDepthEpoch - SOFTWARE_DEPTH_EPOCH_STEP;
  if (depthEpochWrapped || g_SoftwareDepthEpoch == 0) {
    depthValueCursor = g_SoftwareDepthBuffer;
    for (pixelsRemaining = g_FramebufferWidth * g_FramebufferHeight; pixelsRemaining != 0;
        pixelsRemaining--) {
      *depthValueCursor = -1;
      depthValueCursor++;
    }
    g_SoftwareDepthEpoch = -SOFTWARE_DEPTH_EPOCH_STEP;
  }
  return;
}


/* Address: 0x00519210.
   Zeroes the one-byte-per-pixel mask buffer of a software mask (if it has one), sized by the logical
   width x height of its texture source, 64 bytes per step (eight MMX qword stores).
*/
void SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl)

{
  uint64_t *maskQwordWriteCursor;
  uint32_t blocksRemaining;
  GraphicsTextureLogicalSize logicalSize;

  maskQwordWriteCursor = (uint64_t *)maskControl->maskPixels;
  if (maskQwordWriteCursor != NULL) {
    logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskControl->textureSource);
    /* 64-byte blocks; a mask of fewer than 64 pixels would wrap the count, as in the original */
    blocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 6;
    do {
      *maskQwordWriteCursor = 0;
      maskQwordWriteCursor[1] = 0;
      maskQwordWriteCursor[2] = 0;
      maskQwordWriteCursor[3] = 0;
      maskQwordWriteCursor[4] = 0;
      maskQwordWriteCursor[5] = 0;
      maskQwordWriteCursor[6] = 0;
      maskQwordWriteCursor[7] = 0;
      maskQwordWriteCursor = maskQwordWriteCursor + 8;
      blocksRemaining = blocksRemaining - 1;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x00519270.
   Called by SoftwareMaskBuffer_AdvancePatternByPercentTick once per tick: adds 0x1F to every nonzero byte of
   the software mask, saturating at 0xFF (PCMPEQB / PAND / PXOR / PADDUSB); zero bytes stay zero. The mask size
   is taken from g_GraphicsTextureSourceGetLogicalSize, and the buffer is processed in 32-byte blocks,
   width * height >> 5 of them (the remainder is left alone). Quirk kept:
   the block counter is a do-while loop, so fewer than 32 pixels means 2^32 blocks. Nothing happens when
   maskPixels is NULL. ABI: all registers are preserved.
*/
void SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime)

{
  uint8_t *mask;
  uint32_t blocksLeft;
  GraphicsTextureLogicalSize logicalSize;
  int i;

  mask = maskRuntime->maskPixels;
  if (mask == NULL) {
    return;
  }
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  blocksLeft = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 5;
  do {
    for (i = 0; i < 32; i++) {
      if (mask[i] != 0) {
        mask[i] = (uint8_t)(mask[i] < ARGB8888_CHANNEL_MAX - SOFTWARE_MASK_BRIGHTEN_STEP ? mask[i] + SOFTWARE_MASK_BRIGHTEN_STEP : ARGB8888_CHANNEL_MAX);
      }
    }
    mask += 32;
  } while (--blocksLeft != 0);
}


/* Address: 0x00519500.
   Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of every mask pixel inside the circle
   of radius radiusStep * 28 around (centerX, centerY), i.e. a circle growing with the step. With invertSelection
   the radius is (width + height) - radiusStep * 28 (at least 0) and the pixels outside it are set, a shrinking
   hole. Distances are compared squared and unsigned.
*/
void SoftwareMaskBuffer_ApplyCircularRegionBit(UiBooleanState32 invertSelection,GraphicsScreenCoordinate centerY,
          GraphicsScreenCoordinate centerX,SoftwareMaskRadiusStep radiusStep,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t maskWidth;
  int radiusPixels;
  uint32_t radiusSquared;
  int rowDistanceSquared;
  uint32_t distanceSquared;
  uint32_t rowsRemaining;
  uint32_t columnX;
  uint8_t *maskCursor;
  GraphicsTextureLogicalSize logicalSize;
  int rowY;
  bool selected;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  maskCursor = maskRuntime->maskPixels;
  if (invertSelection != 0) {
    radiusPixels = radiusStep * -28 + maskWidth + rowsRemaining;
    if (radiusPixels < 0) {
      radiusPixels = 0;
    }
  }
  else {
    radiusPixels = radiusStep * 28;
  }
  radiusSquared = (uint32_t)(radiusPixels * radiusPixels);
  /* do-whiles kept: a zero width still visits one pixel per row, a zero height wraps the row counter */
  rowY = 0;
  do {
    rowDistanceSquared = (rowY - centerY) * (rowY - centerY);
    columnX = 0;
    do {
      distanceSquared = (columnX - centerX) * (columnX - centerX) + rowDistanceSquared;
      /* both tests include the circle's edge */
      if (invertSelection == 0) {
        selected = distanceSquared <= radiusSquared;
      }
      else {
        selected = radiusSquared <= distanceSquared;
      }
      if (selected) {
        *maskCursor = *maskCursor | 1;
      }
      columnX++;
      maskCursor++;
    } while (columnX < maskWidth);
    rowY++;
    rowsRemaining--;
  } while (rowsRemaining != 0);
}


/* Address: 0x005195D0.
   Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: a diagonal wipe. Sets bit 0 of every mask pixel
   with x + y < thresholdStep * 40 (from the top-left corner), or with invertSelection every pixel with
   x + y > (width + height) - thresholdStep * 40 (from the bottom-right corner).
*/
void SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
          (UiBooleanState32 invertSelection,SoftwareMaskThresholdStep thresholdStep,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t maskWidth;
  uint32_t columnsRemaining;
  int thresholdSum;
  int rowY;
  uint32_t rowsRemaining;
  int diagonalSum;
  uint8_t *maskCursor;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  thresholdSum = thresholdStep * 40;
  maskCursor = maskRuntime->maskPixels;
  if (invertSelection != 0) {
    thresholdSum = thresholdStep * -40 + maskWidth + rowsRemaining;
  }
  /* diagonalSum is x + y of the current pixel */
  rowY = 0;
  columnsRemaining = maskWidth;
  diagonalSum = 0;
  if (invertSelection == 0) {
    do {
      do {
        if (diagonalSum < thresholdSum) {
          *maskCursor = *maskCursor | 1;
        }
        maskCursor++;
        columnsRemaining--;
        diagonalSum++;
      } while (columnsRemaining != 0);
      rowY++;
      rowsRemaining--;
      columnsRemaining = maskWidth;
      diagonalSum = rowY;
    } while (rowsRemaining != 0);
    return;
  }
  do {
    do {
      if (thresholdSum < diagonalSum) {
        *maskCursor = *maskCursor | 1;
      }
      maskCursor++;
      columnsRemaining--;
      diagonalSum++;
    } while (columnsRemaining != 0);
    rowY++;
    rowsRemaining--;
    columnsRemaining = maskWidth;
    diagonalSum = rowY;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00519670.
   Last reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of every mask pixel, 16 bytes
   per step (width * height >> 4 steps; the remainder is left alone). Quirk kept: a mask of fewer than 16 pixels
   makes the do-while counter wrap to 2^32 steps.
*/
void SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl)

{
  uint32_t maskBlocksRemaining;
  uint32_t *maskWordCursor;
  GraphicsTextureLogicalSize logicalSize;
  
  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskControl->textureSource);
  maskWordCursor = (uint32_t *)maskControl->maskPixels;
  maskBlocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 4;
  do {
    *maskWordCursor = *maskWordCursor | ARGB8888_CHANNEL_ONES;
    maskWordCursor[1] = maskWordCursor[1] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[2] = maskWordCursor[2] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[3] = maskWordCursor[3] | ARGB8888_CHANNEL_ONES;
    maskWordCursor = maskWordCursor + 4;
    maskBlocksRemaining--;
  } while (maskBlocksRemaining != 0);
  return;
}


/* Address: 0x005196C0.
   Reveal shape of SoftwareMaskBuffer_AdvancePatternByPercentTick: sets bit 0 of one horizontal band of 15 rows,
   band bandIndex - 1 from the top, or with reverseRows band 24 - bandIndex (from the bottom of 25 bands).
   bandIndex above 24, and bandIndex 0 top-down, set nothing. The band is filled in 16-byte steps
   (width * 15 >> 4 of them).
*/
void SoftwareMaskBuffer_ApplyHorizontalBandBit(UiBooleanState32 reverseRows,TerrainGridMaskIndex bandIndex,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t bandBytes;
  uint32_t blocksLeft;
  int bandRow;
  uint32_t *maskWordCursor;
  GraphicsTextureLogicalSize logicalSize;

  logicalSize = g_GraphicsTextureSourceGetLogicalSize(0,maskRuntime->textureSource);
  if ((uint32_t)bandIndex >= 25) {
    return;
  }
  bandBytes = logicalSize.logicalWidthPixels * 15;
  if (reverseRows == 0) {
    bandRow = bandIndex - 1;
    if (bandRow < 0) {
      return;
    }
  }
  else {
    bandRow = 24 - bandIndex;
  }
  maskWordCursor = (uint32_t *)(maskRuntime->maskPixels + bandRow * bandBytes);
  /* Original quirk: a do-while, so a band of fewer than 16 bytes wraps the counter to 2^32 steps */
  blocksLeft = bandBytes >> 4;
  do {
    *maskWordCursor = *maskWordCursor | ARGB8888_CHANNEL_ONES;
    maskWordCursor[1] = maskWordCursor[1] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[2] = maskWordCursor[2] | ARGB8888_CHANNEL_ONES;
    maskWordCursor[3] = maskWordCursor[3] | ARGB8888_CHANNEL_ONES;
    maskWordCursor = maskWordCursor + 4;
    blocksLeft--;
  } while (blocksLeft != 0);
}


/* Exchanges two whole 0x20-byte vertices. */
static void SoftwareRenderer_SwapVertices(GraphicsPrimitiveVertexRaw *first,GraphicsPrimitiveVertexRaw *second)
{
  GraphicsPrimitiveVertexRaw saved;

  saved = *first;
  *first = *second;
  *second = saved;
}

/* Sorts the three vertices of a packet by screenY (ascending; the comparisons decide ties exactly as the
   original's branch tree), with one swap or one three-way rotation. */
static void SoftwareRenderer_SortVerticesByScreenY(GraphicsPrimitiveVertexRaw *vertices)
{
  int screenY0;
  int screenY1;
  int screenY2;
  GraphicsPrimitiveVertexRaw saved;

  screenY0 = vertices[0].screenY;
  screenY1 = vertices[1].screenY;
  screenY2 = vertices[2].screenY;
  if (screenY1 < screenY0) {
    if (screenY1 <= screenY2) {
      if (screenY2 < screenY0) {
        /* y1 <= y2 < y0: new order 1, 2, 0 */
        saved = vertices[1];
        vertices[1] = vertices[2];
        vertices[2] = vertices[0];
        vertices[0] = saved;
      }
      else {
        SoftwareRenderer_SwapVertices(&vertices[1], &vertices[0]);
      }
    }
    else {
      SoftwareRenderer_SwapVertices(&vertices[0], &vertices[2]);
    }
  }
  else if (screenY2 < screenY0) {
    /* y2 < y0 <= y1: new order 2, 0, 1 */
    saved = vertices[2];
    vertices[2] = vertices[1];
    vertices[1] = vertices[0];
    vertices[0] = saved;
  }
  else if (screenY2 < screenY1) {
    SoftwareRenderer_SwapVertices(&vertices[1], &vertices[2]);
  }
}

/* Address: 0x004FE840.
   Readies one packet for the software raster handlers: sorts the three 0x20-byte vertices by screen Y, snaps
   screen X/Y to whole Q12 pixels, adds the current depth epoch to each depth, sets the flat-shaded flag when all
   vertex colours are equal and, for textured packets, scales U/V from a 256-texel range down to the texture's
   widthLog2/heightLog2 size.
*/
void SoftwareRenderer_PrepareTrianglePacket(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitiveVertexRaw *vertex;
  PackedArgb32 firstColor;
  PackedArgb32 secondColor;
  PackedArgb32 thirdColor;
  GraphicsTextureSetEntry *textureEntryRef;
  int32_t depthEpoch;
  uint8_t texelShift;
  int i;

  SoftwareRenderer_SortVerticesByScreenY(packet->vertices);
  depthEpoch = g_SoftwareDepthEpoch;
  firstColor = packet->vertices[0].diffuseColor;
  secondColor = packet->vertices[1].diffuseColor;
  thirdColor = packet->vertices[2].diffuseColor;
  for (i = 0; i < 3; i++) {
    vertex = &packet->vertices[i];
    /* 0xfffff000 drops the Q12 fraction: whole pixels */
    vertex->screenX = vertex->screenX & ~(uint32_t)Q12_FRACTION_MASK;
    vertex->screenY = vertex->screenY & ~(uint32_t)Q12_FRACTION_MASK;
    vertex->depth = vertex->depth + depthEpoch;
  }
  packet->renderFlags = packet->renderFlags & ~GRAPHICS_PRIMITIVE_FLAG_FLAT_SHADED;
  if ((firstColor == secondColor) && (firstColor == thirdColor)) {
    packet->renderFlags = packet->renderFlags | GRAPHICS_PRIMITIVE_FLAG_FLAT_SHADED;
  }
  if ((packet->renderFlags & GRAPHICS_PRIMITIVE_FLAG_TEXTURED) != 0) {
    textureEntryRef = packet->textureEntry;
    texelShift = 8 - (char)textureEntryRef->widthLog2;
    for (i = 0; i < 3; i++) {
      packet->vertices[i].textureU = packet->vertices[i].textureU >> (texelShift & SHIFT_COUNT_MASK);
    }
    texelShift = 8 - (char)textureEntryRef->heightLog2;
    for (i = 0; i < 3; i++) {
      packet->vertices[i].textureV = packet->vertices[i].textureV >> (texelShift & SHIFT_COUNT_MASK);
    }
  }
}

