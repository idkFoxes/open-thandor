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
   Ownership: graphics/backend/software.
   Purpose: Handles software mask buffer advance pattern by percent tick.
   Local calls: SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31, SoftwareMaskBuffer_Clear,
   SoftwareMaskBuffer_ApplyCircularRegionBit, SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit,
   SoftwareMaskBuffer_ApplyHorizontalBandBit, SoftwareMaskBuffer_SetAllPixelsBit.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t phaseTicks;
  int previousTick;
  uint32_t radiusStep;
  UiBooleanState32 reverseRows;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  previousTick = maskRuntime->tickCounter;
  maskRuntime->tickCounter = maskRuntime->tickCounter + 1;
  if (maskRuntime->maskPixels != (uint8_t *)0x0) {
    SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(maskRuntime);
    phaseTicks = previousTick + 0x14;
    radiusStep = phaseTicks % 100;
    if (radiusStep == 0) {
      if (maskRuntime->patternState58 != 0) {
        maskRuntime->patternState54 = maskRuntime->patternState54 + 1;
      }
      maskRuntime->patternState58 = maskRuntime->patternState58 + 1;
      SoftwareMaskBuffer_Clear(maskRuntime);
      if (0xd < maskRuntime->patternState54) {
        maskRuntime->patternState54 = 0xd;
      }
      if (0xd < maskRuntime->patternState58) {
        maskRuntime->patternState58 = 0xd;
      }
    }
    else {
      switch(phaseTicks / 100) {
      case 1:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,0x50,0xa0,radiusStep,maskRuntime);
        break;
      case 2:
        logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
                  (logicalSize.logicalWidthPixels + logicalSize.logicalHeightPixels,radiusStep,maskRuntime);
        break;
      case 3:
      case 7:
        SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit(0,radiusStep,maskRuntime);
        break;
      case 4:
      case 9:
        logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
        reverseRows = logicalSize.logicalHeightPixels;
        SoftwareMaskBuffer_ApplyHorizontalBandBit(reverseRows,radiusStep,maskRuntime);
        break;
      case 5:
        SoftwareMaskBuffer_ApplyHorizontalBandBit(0,radiusStep,maskRuntime);
        break;
      case 6:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,0x118,0xa0,radiusStep,maskRuntime);
        break;
      case 8:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,0x20,0x140,radiusStep,maskRuntime);
        break;
      case 10:
      case 0xd:
        SoftwareMaskBuffer_ApplyCircularRegionBit(0,0xb4,0x140,radiusStep,maskRuntime);
        break;
      case 0xb:
        SoftwareMaskBuffer_ApplyCircularRegionBit(1,0xb4,0x140,radiusStep,maskRuntime);
        break;
      case 0xc:
        SoftwareMaskBuffer_SetAllPixelsBit(maskRuntime);
      }
    }
  }
  return;
}


/* Address: 0x00485FD0.
   Ownership: graphics/backend/software.
   Purpose: Clears the software viewport and advances the depth epoch. Typed parameters: p0
   clipMaxY→GraphicsScreenCoordinate_V307, p1 clipMaxX→GraphicsScreenCoordinate_V307, p2
   clipMinY→GraphicsScreenCoordinate_V307, p3 clipMinX→GraphicsScreenCoordinate_V307. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: SoftwareRenderer_AdvanceDepthEpoch.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_ClearViewport
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX)

{
  bool accessFailed;
  
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!accessFailed) {
    (*g_GraphicsFramebufferFillRectArgb)
              (clipMaxY,clipMaxX,clipMinY,clipMinX,clipMaxY,clipMaxX,clipMinY,clipMinX,0xff000000,
               g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  SoftwareRenderer_AdvanceDepthEpoch();
  return;
}


/* Address: 0x004D1560.
   Ownership: graphics/backend/software.
   Purpose: Prepares each packet and dispatches it through g_SoftwareRasterHandlers16Bit. Typed parameters: p0
   clipMaxY→GraphicsScreenCoordinate_V307, p1 clipMaxX→GraphicsScreenCoordinate_V307, p2
   clipMinY→GraphicsScreenCoordinate_V307, p3 clipMinX→GraphicsScreenCoordinate_V307. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: SoftwareRenderer_PrepareTrianglePacket.
   Cross-module calls: GraphicsPrimitiveQueue_Begin [graphics/render/primitives], GraphicsPrimitiveQueue_Next
   [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_DrawQueue16Bit
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;
  GraphicsPrimitivePacket *currentPacket;
  GraphicsPrimitivePacketEaxCf5 queueCursor;
  
  queueCursor = GraphicsPrimitiveQueue_Begin(queue);
  while (packet = queueCursor.packet, !queueCursor.carry) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    (**(code **)((int)g_SoftwareRasterHandlers16Bit + ((packet->renderFlags & 0x3f000) >> 10)))
              (clipMaxY,clipMaxX,clipMinY,clipMinX,packet);
    g_PrimitiveDrawCallCount = g_PrimitiveDrawCallCount + 1;
    queueCursor = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x004D15D0.
   Ownership: graphics/backend/software.
   Purpose: Prepares each packet and dispatches it through g_SoftwareRasterHandlersNon16Bit. Typed parameters: p0
   clipMaxY→GraphicsScreenCoordinate_V307, p1 clipMaxX→GraphicsScreenCoordinate_V307, p2
   clipMinY→GraphicsScreenCoordinate_V307, p3 clipMinX→GraphicsScreenCoordinate_V307. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: SoftwareRenderer_PrepareTrianglePacket.
   Cross-module calls: GraphicsPrimitiveQueue_Begin [graphics/render/primitives], GraphicsPrimitiveQueue_Next
   [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_DrawQueueNon16Bit
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;
  GraphicsPrimitivePacket *currentPacket;
  GraphicsPrimitivePacketEaxCf5 queueCursor;
  
  queueCursor = GraphicsPrimitiveQueue_Begin(queue);
  while (packet = queueCursor.packet, !queueCursor.carry) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    (**(code **)((int)g_SoftwareRasterHandlersNon16Bit + ((packet->renderFlags & 0x3f000) >> 10)))
              (clipMaxY,clipMaxX,clipMinY,clipMinX,packet);
    g_PrimitiveDrawCallCount = g_PrimitiveDrawCallCount + 1;
    queueCursor = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x004D1640.
   Ownership: graphics/backend/software.
   Purpose: Uses clip minima of zero, stores an explicit target base, and dispatches through
   g_SoftwareRasterHandlersAuxiliary. Typed parameters: p0 clipMaxY→GraphicsScreenCoordinate_V307, p1
   clipMaxX→GraphicsScreenCoordinate_V307. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: SoftwareRenderer_PrepareTrianglePacket.
   Cross-module calls: GraphicsPrimitiveQueue_Begin [graphics/render/primitives], GraphicsPrimitiveQueue_Next
   [graphics/render/primitives].
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_DrawQueueAuxiliary
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,void *targetBase,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;
  GraphicsPrimitivePacket *currentPacket;
  GraphicsPrimitivePacketEaxCf5 queueCursor;
  uint32_t textureSubresourceIndex;
  
  g_SoftwareAuxiliaryTargetBase = targetBase;
  queueCursor = GraphicsPrimitiveQueue_Begin(queue);
  while (packet = queueCursor.packet, !queueCursor.carry) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    if (((packet->renderFlags & 0x10000) == 0) ||
       ((packet->textureEntry->subresourceIndex != 99 &&
        (packet->textureEntry->subresourceIndex != 0x71)))) {
      (**(code **)((int)g_SoftwareRasterHandlersAuxiliary + ((packet->renderFlags & 0x3f000) >> 10))
      )(clipMaxY,clipMaxX,0,0,packet);
      g_PrimitiveDrawCallCount = g_PrimitiveDrawCallCount + 1;
    }
    queueCursor = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}


/* Address: 0x00486020.
   Ownership: graphics/backend/software.
   Purpose: Begins software-surface access, invokes the selected queue renderer, and ends access. Typed parameters:
   p0 clipMaxY→GraphicsScreenCoordinate_V307, p1 clipMaxX→GraphicsScreenCoordinate_V307, p2
   clipMinY→GraphicsScreenCoordinate_V307, p3 clipMinX→GraphicsScreenCoordinate_V307. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_DrawPrimitiveQueueBridge
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  bool accessFailed;
  
  accessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!accessFailed) {
    (*g_SoftwareDrawQueueProc)(clipMaxY,clipMaxX,clipMinY,clipMinX,queue);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x00486050.
   Ownership: graphics/backend/software.
   Purpose: Archived body is CLC; RET and is referenced from software graphics dispatch storage at 00485820.
*/
void SoftwareGraphicsDispatch_SuccessNoOp(void)

{
  return;
}

/* Address: 0x00486060.
   Ownership: graphics/backend/software.
   Purpose: Archived body is RET and is referenced from software graphics dispatch storage at 00485824.
*/
void __thandor_void_preserve_eax_ecx_edx SoftwareGraphicsDispatch_NoOp(void)

{
  return;
}


/* Address: 0x004A8F80.
   Ownership: graphics/backend/software.
   Purpose: Allocates the 0xC00 SoftwarePixelPackTables block when needed, rebuilds it through
   g_SoftwareBuildPixelPackTables, and derives the runtime MMX pack/unpack constants from
   g_SoftwarePixelFormatConfig. ABI: CF clear means success. CF set means failure.
*/
DisplayModeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t modeArg0,uint32_t modeArg1,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width)

{
  DisplayModeEaxCf5 hookResult;
  SoftwarePixelPackTables *packTables;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  ArenaAllocEaxCf5 tableAllocation;
  DisplayModeEaxCf5 failureResult;
  
  packTables = g_SoftwarePixelPackTables;
  if (g_SoftwarePixelPackTables == (SoftwarePixelPackTables *)0x0) {
    tableAllocation = (*g_MemoryApi.alloc)(0xc00);
    packTables = (SoftwarePixelPackTables *)tableAllocation.eax;
    if (tableAllocation.carry) {
      failureResult.eax = tableAllocation.eax;
      failureResult.carry = tableAllocation.carry;
      return failureResult;
    }
  }
  g_SoftwarePixelPackTables = packTables;
  (*g_SoftwareBuildPixelPackTables)(g_SoftwareColorScaleQ16,g_SoftwareColorBiasQ16);
  redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
  greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
  blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
  blueUnpackScale = 1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits & 0x1f);
  hookResult.carry = false;
  hookResult.eax = blueUnpackScale;
  g_SoftwarePixelMmxConstants.packedPixelMasks.red =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.green =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
  g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
       (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
  g_SoftwarePixelMmxConstants.unpackScales.red =
       (SoftwareColorLaneFixed16)
       (1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.redShift) - redBits & 0x1f));
  g_SoftwarePixelMmxConstants.unpackScales.green =
       (SoftwareColorLaneFixed16)
       (1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits & 0x1f));
  g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
       (SoftwareColorLaneFixed16)((1 << (redBits & 0x1f)) + -1 << (0xc - redBits & 0x1f));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
       (SoftwareColorLaneFixed16)((1 << (greenBits & 0x1f)) + -1 << (0xc - greenBits & 0x1f));
  g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
       (SoftwareColorLaneFixed16)((1 << (blueBits & 0x1f)) + -1 << (0xc - blueBits & 0x1f));
  g_SoftwarePixelMmxConstants.packWeights.red =
       (SoftwareColorLaneFixed16)
       (1 << ((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4 & 0x1f));
  g_SoftwarePixelMmxConstants.packWeights.green =
       (SoftwareColorLaneFixed16)
       (1 << ((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4 & 0x1f));
  g_SoftwarePixelMmxConstants.packWeights.blue =
       (SoftwareColorLaneFixed16)
       (1 << ((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4 & 0x1f));
  return hookResult;
}


/* Address: 0x004A9110.
   Ownership: graphics/backend/software.
   Purpose: Allocates 0x10 + width * height * bytesPerPixel bytes, stores the inline framebuffer header, points
   pixels at header + 0x10, and zeroes the complete pixel area. ABI: CF clear means success. CF set means failure.
*/
SoftwareFramebufferEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width)

{
  GraphicsPixelDimension *headerCursor;
  uint32_t pixelBytesOrWordsLeft;
  ArenaAllocEaxCf5 frameAllocation;
  SoftwareFramebufferEaxCf5 createResult;
  
  pixelBytesOrWordsLeft = width * height * bytesPerPixel;
  frameAllocation = (*g_MemoryApi.alloc)(pixelBytesOrWordsLeft + 0x10);
  headerCursor = (GraphicsPixelDimension *)frameAllocation.eax;
  if (!frameAllocation.carry) {
    headerCursor[2] = bytesPerPixel;
    *headerCursor = width;
    headerCursor[1] = height;
    headerCursor[3] = (GraphicsPixelDimension)(headerCursor + 4);
    headerCursor = headerCursor + 4;
    for (pixelBytesOrWordsLeft = pixelBytesOrWordsLeft >> 2; pixelBytesOrWordsLeft != 0; pixelBytesOrWordsLeft = pixelBytesOrWordsLeft - 1) {
      *headerCursor = 0;
      headerCursor = headerCursor + 1;
    }
    frameAllocation = THANDOR_BITCAST(uint64_t, ArenaAllocEaxCf5, ((THANDOR_BITCAST(ArenaAllocEaxCf5, uint64_t, frameAllocation) & 0xFFFFFFFFFFull) & 0xffffffff));
  }
  createResult.framebuffer = (SoftwareFramebufferAccess *)frameAllocation.eax;
  createResult.carry = frameAllocation.carry;
  return createResult;
}


/* Address: 0x004A9160.
   Ownership: graphics/backend/software.
   Purpose: Frees one inline SoftwareFramebufferAccess allocation.
*/
void __thandor_preserve_eax SoftwareFramebuffer_Destroy(SoftwareFramebufferAccess *framebuffer)

{
  (*g_MemoryApi.free)(framebuffer);
  return;
}


/* Address: 0x004A9180.
   Ownership: graphics/backend/software.
   Purpose: Builds the blue, green, and red 256-entry framebuffer packing tables from a signed Q16 linear color
   scale and bias.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16)

{
  uint32_t transformedChannelValueQ16;
  uint32_t channelIndex;
  SoftwarePixelPackTables *packTableCursor;
  
  channelIndex = 0;
  packTableCursor = g_SoftwarePixelPackTables;
  do {
    transformedChannelValueQ16 = (channelIndex - 0x40) * colorScaleQ16 + 0x400000 + colorBiasQ16;
    if ((int)transformedChannelValueQ16 < 0) {
      transformedChannelValueQ16 = 0;
    }
    else if (0xffffff < transformedChannelValueQ16) {
      transformedChannelValueQ16 = 0xff0000;
    }
    packTableCursor->blue[0] =
         (transformedChannelValueQ16 >>
         (0x18U - (char)g_SoftwarePixelFormatConfig.blueBitCount & 0x1f)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.blueShift & 0x1f);
    packTableCursor->green[0] =
         (transformedChannelValueQ16 >>
         (0x18U - (char)g_SoftwarePixelFormatConfig.greenBitCount & 0x1f)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.greenShift & 0x1f);
    packTableCursor->red[0] =
         (transformedChannelValueQ16 >>
         (0x18U - (char)g_SoftwarePixelFormatConfig.redBitCount & 0x1f)) <<
         ((uint8_t)g_SoftwarePixelFormatConfig.redShift & 0x1f);
    channelIndex = channelIndex + 1;
    packTableCursor = (SoftwarePixelPackTables *)(packTableCursor->blue + 1);
  } while (channelIndex < 0x100);
  g_SoftwareColorBiasQ16 = colorBiasQ16;
  g_SoftwareColorScaleQ16 = colorScaleQ16;
  return;
}


/* Address: 0x004A93C0.
   Ownership: graphics/backend/software.
   Purpose: Clips and draws one source subresource into a two-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is copied, anything else is blended. A
   paletted texel tests the alpha of the entry's converted pixel (+4) and writes its low word, but blends the
   entry's ARGB colour (+0) with the alpha of that colour. ABI: all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSourceAlpha16
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
   Ownership: graphics/backend/software.
   Purpose: Clips and draws one source subresource into a four-byte framebuffer (source-alpha blit, see
   docs/software_raster.md "Blits"). Alpha 0 is skipped, alpha 0xFF is converted through
   g_SoftwarePixelPackTables and written, anything else is blended in 8-bit lanes. Unlike the 16-bit version, a
   paletted texel uses the entry's second dword (+4) for everything: the alpha test, the blend colour, and the
   opaque write, which converts it through the pack tables again. ABI: all registers are preserved and CF is
   cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSourceAlpha32
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
   Ownership: graphics/backend/software.
   Purpose: Clips and draws one source subresource into a two-byte framebuffer, with the source RGB at half
   strength (see docs/software_raster.md "Blits"). Alpha 0 is skipped; every other alpha, 0xFF included, blends
   (source lanes (c * 0x101) >> 3 instead of >> 2), so there is no opaque copy. Unlike BlitSourceAlpha16, a
   paletted texel uses the entry's ARGB colour (+0) for both the alpha test and the blend. ABI: all registers are
   preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitHalfSourceRgb16
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
      if (Blit_IsTransparent(argb)) {
        continue;
      }
      *pixel = Blit_PackLanes16(Blit_BlendLanes(Blit_ArgbLanes(argb, 3), Blit_Unpack16(*pixel), argb >> 24));
    }
  }
  return false;
}


/* Address: 0x004A9E10.
   Ownership: graphics/backend/software.
   Purpose: Clips and draws one source subresource into a four-byte framebuffer, with the source RGB at half
   strength (see docs/software_raster.md "Blits"). Alpha 0 is skipped; every other alpha, 0xFF included, blends,
   so there is no opaque copy. Quirks kept from the original: a paletted texel uses the entry's second dword (+4)
   as its colour, like the other 32-bit blits, and only the paletted path halves the source ((c * 0x101) >> 3);
   the direct-colour path uses >> 2, i.e. it is an ordinary source-alpha blend whose alpha 0xFF still goes
   through the blend tables. ABI: all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitHalfSourceRgb32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
   Ownership: graphics/backend/software.
   Purpose: Stretches one direct-color source subresource into a two-byte framebuffer using two-dimensional linear
   interpolation. The source entry must be direct color: paletteIndex == -1. The function computes 8-bit fractional
   source steps from (pixelWidth-1)/(destinationWidth-1) and (pixelHeight-1)/(destinationHeight-1). Four
   neighboring ARGB8888 pixels are blended horizontally and vertically through g_SoftwareBilinearForwardFactors and
   g_SoftwareBilinearInverseFactors. The routine performs no clipping.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_StretchDirectColorBilinear16
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  /* Rewritten from the assembly (0x004AA170-0x004AA3E8) with the MMX lanes in plain C, like the
     32-bit variant. Two destination pixels per step; each blends four ARGB8888 neighbours through
     the forward (0x00420F20) and inverse (0x0041FF20) word tables, then packs to 16 bits with the
     runtime quantize masks (0x0041F6E8) and PMADDWD weights (0x0041F6E0), which the display
     setup fills for 555 or 565. */
  const short *forward = (const short *)(uintptr_t)THANDOR_ADDR(g_SoftwareBilinearInverseFactors,0);
  const short *inverse = (const short *)(uintptr_t)THANDOR_ADDR(g_SoftwareBilinearForwardFactors,0);
  const uint16_t *quantizeMask = (const uint16_t *)(uintptr_t)THANDOR_ADDR(g_SoftwarePixelMmxConstants,0x8);
  const short *packWeights = (const short *)(uintptr_t)THANDOR_ADDR(g_SoftwarePixelMmxConstants,0);
  uint8_t *asset = (uint8_t *)sourceAsset;
  uint8_t *entry;
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

  if ((*(uint32_t *)asset != 0x786667) || (subresourceIndex >= *(uint32_t *)(asset + 0xb0))) {
    return;
  }
  entry = asset + *(uint32_t *)(asset + 0xb8) + subresourceIndex * 0x20;
  if ((*(uint32_t *)((uint8_t *)framebuffer + 8) != 2) || (*(int32_t *)(entry + 8) != -1)) {
    return;
  }
  pitchPixels = *(uint32_t *)framebuffer;
  destinationRow = (uint16_t *)*(uint8_t **)((uint8_t *)framebuffer + 0xc) +
                   (destinationY * pitchPixels + destinationX);
  sourceWidth = *(uint32_t *)(entry + 0x18);
  sourceHeight = *(uint32_t *)(entry + 0x1c);
  stepX = ((sourceWidth - 1) * 0x100) / (destinationWidth - 1);
  stepY = ((sourceHeight - 1) * 0x100) / (destinationHeight - 1);
  sourceBase = asset + *(uint32_t *)(entry + 0xc);
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
          int a = ((p00[lane] * 0x101) >> 2);
          int b = ((p00[lane + 4] * 0x101) >> 2);
          int c = ((p10[lane] * 0x101) >> 2);
          int d = ((p10[lane + 4] * 0x101) >> 2);
          short top = (short)(((a * forward[wx * 4 + lane]) >> 16) + ((b * inverse[wx * 4 + lane]) >> 16));
          short bottom = (short)(((c * forward[wx * 4 + lane]) >> 16) + ((d * inverse[wx * 4 + lane]) >> 16));
          short mixed = (short)(((top * forward[wy * 4 + lane]) >> 16) +
                                ((bottom * inverse[wy * 4 + lane]) >> 16));
          int value = (unsigned short)mixed >> 2;
          if (value > 0xff) value = 0xff;
          /* PUNPCKLBW x,x; PSLLW 4; PAND quantize mask */
          lanes[lane] = (uint16_t)(((value * 0x101) << 4) & quantizeMask[lane]);
        }
        /* PMADDWD: two signed dword sums, then the two shifted copies are added per word. */
        madd = (unsigned long long)(uint32_t)((short)lanes[0] * packWeights[0] + (short)lanes[1] * packWeights[1]) |
               ((unsigned long long)(uint32_t)((short)lanes[2] * packWeights[2] +
                                            (short)lanes[3] * packWeights[3]) << 32);
        packed[half] = (uint16_t)((uint16_t)(madd >> 8) + (uint16_t)(madd >> 40));
        fx = fx + stepX;
      }
      *out = (uint32_t)packed[0] | ((uint32_t)packed[1] << 16);
      out = out + 1;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}


/* Address: 0x004AA3F0.
   Ownership: graphics/backend/software.
   Purpose: Stretches one direct-color source subresource into a four-byte framebuffer using two-dimensional linear
   interpolation. The source entry must be direct color: paletteIndex == -1. The function computes 8-bit fractional
   source steps from (pixelWidth-1)/(destinationWidth-1) and (pixelHeight-1)/(destinationHeight-1). Four
   neighboring ARGB8888 pixels are blended horizontally and vertically through g_SoftwareBilinearForwardFactors and
   g_SoftwareBilinearInverseFactors. The routine performs no clipping.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_StretchDirectColorBilinear32
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer)

{
  /* Rewritten from the assembly (0x004AA3F0-0x004AA616) with the MMX lanes in plain C. The
     decompiled version (300 lines of lane emulation) left the end-movie frames static. Two
     destination pixels per step; each blends four ARGB8888 neighbours through the forward
     (0x00420F20) and inverse (0x0041FF20) word tables, as PMULHW does. */
  const short *forward = (const short *)(uintptr_t)THANDOR_ADDR(g_SoftwareBilinearInverseFactors,0);
  const short *inverse = (const short *)(uintptr_t)THANDOR_ADDR(g_SoftwareBilinearForwardFactors,0);
  const unsigned long long clampMask = *(const unsigned long long *)(uintptr_t)THANDOR_ADDR(g_SoftwareBilinearPackedByteClampMask,0);
  uint8_t *asset = (uint8_t *)sourceAsset;
  uint8_t *entry;
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

  if ((*(uint32_t *)asset != 0x786667) || (subresourceIndex >= *(uint32_t *)(asset + 0xb0))) {
    return;
  }
  entry = asset + *(uint32_t *)(asset + 0xb8) + subresourceIndex * 0x20;
  if ((*(uint32_t *)((uint8_t *)framebuffer + 8) != 4) || (*(int32_t *)(entry + 8) != -1)) {
    return;
  }
  pitchPixels = *(uint32_t *)framebuffer;
  destinationRow = (uint32_t *)*(uint8_t **)((uint8_t *)framebuffer + 0xc) +
                   (destinationY * pitchPixels + destinationX);
  sourceWidth = *(uint32_t *)(entry + 0x18);
  sourceHeight = *(uint32_t *)(entry + 0x1c);
  stepX = ((sourceWidth - 1) * 0x100) / (destinationWidth - 1);
  stepY = ((sourceHeight - 1) * 0x100) / (destinationHeight - 1);
  sourceBase = asset + *(uint32_t *)(entry + 0xc);
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
          int a = ((p00[lane] * 0x101) >> 2);
          int b = ((p00[lane + 4] * 0x101) >> 2);
          int c = ((p10[lane] * 0x101) >> 2);
          int d = ((p10[lane + 4] * 0x101) >> 2);
          short top = (short)(((a * forward[wx * 4 + lane]) >> 16) + ((b * inverse[wx * 4 + lane]) >> 16));
          short bottom = (short)(((c * forward[wx * 4 + lane]) >> 16) + ((d * inverse[wx * 4 + lane]) >> 16));
          short mixed = (short)(((top * forward[wy * 4 + lane]) >> 16) +
                                ((bottom * inverse[wy * 4 + lane]) >> 16));
          int value = (unsigned short)mixed >> 2;
          if (value > 0xff) value = 0xff;
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
      out = out + 2;
    }
    destinationRow = destinationRow + pitchPixels;
    fy = fy + stepY;
    sourceRow = sourceBase + (fy >> 8) * sourceWidth * 4;
  }
}


/* Address: 0x004AA630.
   Ownership: graphics/backend/software.
   Purpose: Draws one source subresource into a two-byte framebuffer at an integer scale (see
   docs/software_raster.md "Blits"). Every texel becomes an integerScale x integerScale block, and the entry's
   origin is scaled too. Nothing is clipped at the source: the whole scaled image is walked and each pixel is
   tested against the clip rectangle, which is clamped to the framebuffer. Alpha 0 is skipped, alpha 0xFF is
   written, anything else is blended. A paletted texel tests the alpha of the entry's converted pixel (+4) and
   writes its low word, but blends the entry's ARGB colour (+0), as in BlitSourceAlpha16. Quirks kept: every
   negative paletteIndex means ARGB texels, and the counters are do-while loops, so a scale or image size of 0
   runs them 2^32 times. ABI: all registers are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_BlitIntegerScaledSourceAlpha16
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
   Ownership: graphics/backend/software.
   Purpose: Four-byte framebuffer version of SoftwareTextureSource_BlitIntegerScaledSourceAlpha16 (integer-scaled,
   per-pixel clip test, alpha 0 skipped, 0xFF written, anything else blended in 8-bit lanes). Unlike
   BlitSourceAlpha32, a paletted texel here uses the 16-bit layout: it tests the alpha of the converted pixel
   (+4), blends the ARGB colour (+0), and writes +4 as it is (not converted again) when opaque. A direct texel is
   converted through g_SoftwarePixelPackTables when opaque. Same quirks as the 16-bit version. ABI: all registers
   are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_BlitIntegerScaledSourceAlpha32
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
   Ownership: graphics/backend/software.
   Purpose: BlitSourceAlpha16 with an explicit palette bank (see docs/software_raster.md "Blits"). A paletted
   subresource is drawn with paletteBankIndex instead of its own paletteIndex; the entry's paletteIndex must still
   be valid, and paletteBankIndex is only checked (unsigned, < paletteBankCount) after clipping. A direct-colour
   subresource ignores paletteBankIndex. The pixel operation is that of BlitSourceAlpha16, including the palette
   +0/+4 mix. ABI: all registers are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSourceAlphaPaletteBank16
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
    region.palette = (const uint8_t *)sourceAsset + 0x200 + paletteBankIndex * 0x800;
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
   Ownership: graphics/backend/software.
   Purpose: BlitSourceAlpha32 with an explicit palette bank (see docs/software_raster.md "Blits"). A paletted
   subresource is drawn with paletteBankIndex instead of its own paletteIndex; the entry's paletteIndex must still
   be valid, and paletteBankIndex is only checked (unsigned, < paletteBankCount) after clipping. A direct-colour
   subresource ignores paletteBankIndex. The pixel operation is that of BlitSourceAlpha32 (the palette entry's +4
   dword used for everything). ABI: all registers are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSourceAlphaPaletteBank32
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
    region.palette = (const uint8_t *)sourceAsset + 0x200 + paletteBankIndex * 0x800;
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
   Ownership: graphics/backend/software.
   Purpose: Clips and adds one source subresource onto a two-byte framebuffer (saturated add, see
   docs/software_raster.md "Blits"). A texel whose RGB is 0 is skipped whatever its alpha; every other one is
   added lane by lane with unsigned 16-bit saturation (Blit_AddArgb16) and the sum is packed back. Source alpha
   is not a blend factor. A paletted texel uses the entry's ARGB colour (+0), unlike the 32-bit version. ABI: all
   registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSaturatedAddRgb16
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
      if ((argb & 0xffffff) != 0) {
        *pixel = Blit_AddArgb16(argb, *pixel, 0);
      }
    }
  }
  return false;
}


/* Address: 0x004AB750.
   Ownership: graphics/backend/software.
   Purpose: Clips and adds one source subresource onto a four-byte framebuffer (saturated add). A texel whose RGB
   is 0 is skipped whatever its alpha; every other one is added byte by byte, clamped at 0xFF (Blit_AddArgb32).
   The alpha byte is summed and written as well. Quirk kept from the original: a paletted texel uses the entry's
   second dword (+4, the converted pixel), not its ARGB colour, both for the RGB-zero test and for the add. ABI:
   all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitSaturatedAddRgb32
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
      if ((argb & 0xffffff) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 0);
      }
    }
  }
  return false;
}


/* Address: 0x004ABA70.
   Ownership: graphics/backend/software.
   Purpose: Same as SoftwareTextureSource_BlitSaturatedAddRgb16, but the source lanes are halved (PSRLW 1 of
   c * 0x101) before the saturated add. The RGB-zero test uses the unhalved colour. A paletted texel uses the
   entry's ARGB colour (+0). ABI: all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
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
      if ((argb & 0xffffff) != 0) {
        *pixel = Blit_AddArgb16(argb, *pixel, 1);
      }
    }
  }
  return false;
}


/* Address: 0x004ABD20.
   Ownership: graphics/backend/software.
   Purpose: Same as SoftwareTextureSource_BlitSaturatedAddRgb32, but the source lanes are halved (PSRLW 1 of
   c * 0x101) before the saturated add. The RGB-zero test uses the unhalved colour, and a paletted texel again
   uses the entry's second dword (+4). ABI: all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitHalfRgbSaturatedAdd32
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
      if ((argb & 0xffffff) != 0) {
        *pixel = Blit_AddArgb32(argb, *pixel, 1);
      }
    }
  }
  return false;
}


/* Address: 0x004AC040.
   Ownership: graphics/backend/software.
   Purpose: Clips and draws one source subresource into a two-byte framebuffer, each source channel multiplied by
   the matching channel of modulationArgb8888 first (Blit_Modulate, see docs/software_raster.md "Blits"). The
   modulated colour then goes through the source-alpha rules: alpha 0 skipped, alpha 0xFF converted and written,
   anything else blended. Unlike BlitSourceAlpha16, a paletted texel uses the entry's ARGB colour (+0) for
   everything. Quirk: the modulated alpha is at most 0xFE, so the opaque branch is never taken. ABI: all
   registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitModulatedSourceAlpha16
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
   Ownership: graphics/backend/software.
   Purpose: Four-byte framebuffer version of BlitModulatedSourceAlpha16: each source channel is multiplied by the
   matching channel of modulationArgb8888 (Blit_Modulate), then drawn with the source-alpha rules. Unlike
   BlitSourceAlpha32, a paletted texel uses the entry's ARGB colour (+0). Quirk: the modulated alpha is at most
   0xFE, so the opaque branch is never taken. ABI: all registers are preserved and CF is cleared.
*/
bool __thandor_cf_preserve_eax_ecx_edx
SoftwareTextureSource_BlitModulatedSourceAlpha32
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
   Ownership: graphics/backend/software.
   Purpose: Fills the intersection of [rectMinX, rectMaxX) x [rectMinY, rectMaxY), the framebuffer and the clip
   rectangle of a two-byte framebuffer with argb8888: alpha 0 draws nothing, alpha 0xFF writes the colour
   converted through g_SoftwarePixelPackTables, anything else blends it over every pixel (see
   docs/software_raster.md "Blits"). ABI: all registers are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareFramebuffer_FillRectArgb16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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
   Ownership: graphics/backend/software.
   Purpose: Four-byte framebuffer version of SoftwareFramebuffer_FillRectArgb16: alpha 0 draws nothing, alpha
   0xFF writes the converted colour, anything else is blended in 8-bit lanes (alpha lane included). ABI: all
   registers are preserved and CF is cleared.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareFramebuffer_FillRectArgb32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
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


/* Address: 0x004AD410.
   Ownership: graphics/backend/software.
   Purpose: Copies a rectangle beginning at sourceX/sourceY in source into destination beginning at (0,0). source
   and destination must have identical bytesPerPixel. destination must initially be at least copyWidth by
   copyHeight. The source coordinates are clipped against source bounds. Negative source coordinates shift the
   destination start so relative alignment is preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareFramebuffer_CopyRegionToOrigin
          (GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX,
          SoftwareFramebufferAccess *destination,SoftwareFramebufferAccess *source)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int sourceStrideBytes;
  int destPixelOffset;
  uint32_t bytesOrWordsLeft;
  int sourceOffsetOrDestStride;
  uint32_t rowBytes;
  uint8_t *sourceRow;
  uint8_t *destRowStartOrSourceCursor;
  uint8_t *destRow;
  uint8_t *sourceRowStartOrDestCursor;
  
  pixelBytes = source->bytesPerPixel;
  if (((pixelBytes == destination->bytesPerPixel) && ((int)copyWidth <= (int)destination->width)) &&
     ((int)copyHeight <= (int)destination->height)) {
    if (sourceY < 0) {
      sourceOffsetOrDestStride = 0;
      destPixelOffset = destination->width * -sourceY;
      copyHeight = copyHeight + sourceY;
      sourceY = 0;
    }
    else {
      destPixelOffset = 0;
      sourceOffsetOrDestStride = source->width * sourceY;
    }
    if (sourceX < 0) {
      destPixelOffset = destPixelOffset - sourceX;
      copyWidth = copyWidth + sourceX;
      sourceX = 0;
    }
    else {
      sourceOffsetOrDestStride = sourceOffsetOrDestStride + sourceX;
    }
    if ((int)source->width < (int)(sourceX + copyWidth)) {
      copyWidth = copyWidth - ((sourceX + copyWidth) - source->width);
    }
    if ((int)source->height < (int)(sourceY + copyHeight)) {
      copyHeight = copyHeight - ((sourceY + copyHeight) - source->height);
    }
    sourceStrideBytes = source->width * pixelBytes;
    sourceRow = source->pixels + sourceOffsetOrDestStride * pixelBytes;
    sourceOffsetOrDestStride = destination->width * pixelBytes;
    rowBytes = pixelBytes * copyWidth;
    destRow = destination->pixels + destPixelOffset * pixelBytes;
    if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
      bytesOrWordsLeft = rowBytes;
      destRowStartOrSourceCursor = destRow;
      sourceRowStartOrDestCursor = sourceRow;
      if ((rowBytes & 3) != 0) {
        do {
          for (; bytesOrWordsLeft != 0; bytesOrWordsLeft = bytesOrWordsLeft - 1) {
            *destRow = *sourceRow;
            sourceRow = sourceRow + 1;
            destRow = destRow + 1;
          }
          sourceRow = sourceRowStartOrDestCursor + sourceStrideBytes;
          destRow = destRowStartOrSourceCursor + sourceOffsetOrDestStride;
          copyHeight = copyHeight - 1;
          bytesOrWordsLeft = rowBytes;
          destRowStartOrSourceCursor = destRow;
          sourceRowStartOrDestCursor = sourceRow;
        } while (copyHeight != 0);
        return;
      }
      do {
        destRowStartOrSourceCursor = sourceRow;
        sourceRowStartOrDestCursor = destRow;
        for (bytesOrWordsLeft = rowBytes >> 2; bytesOrWordsLeft != 0; bytesOrWordsLeft = bytesOrWordsLeft - 1) {
          *(uint32_t *)sourceRowStartOrDestCursor = *(uint32_t *)destRowStartOrSourceCursor;
          destRowStartOrSourceCursor = destRowStartOrSourceCursor + 4;
          sourceRowStartOrDestCursor = sourceRowStartOrDestCursor + 4;
        }
        sourceRow = sourceRow + sourceStrideBytes;
        destRow = destRow + sourceOffsetOrDestStride;
        copyHeight = copyHeight - 1;
      } while (copyHeight != 0);
    }
  }
  return;
}


/* Address: 0x004AD520.
   Ownership: graphics/backend/software.
   Purpose: Copies a rectangle beginning at source (0,0) into destination beginning at destinationX/destinationY.
   source and destination must have identical bytesPerPixel. source must initially be at least copyWidth by
   copyHeight. The destination coordinates are clipped against destination bounds. Negative destination coordinates
   shift the source start so relative alignment is preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareFramebuffer_CopyOriginToRegion
          (GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          SoftwareFramebufferAccess *source,SoftwareFramebufferAccess *destination)

{
  SoftwareFramebufferPixelSize pixelBytes;
  int destStrideBytes;
  int sourcePixelOffset;
  uint32_t bytesOrWordsLeft;
  int destOffsetOrSourceStride;
  uint32_t rowBytes;
  uint8_t *sourceRow;
  uint8_t *destRowStartOrSourceCursor;
  uint8_t *destRow;
  uint8_t *sourceRowStartOrDestCursor;
  
  pixelBytes = destination->bytesPerPixel;
  if (((pixelBytes == source->bytesPerPixel) && ((int)copyWidth <= (int)source->width)) &&
     ((int)copyHeight <= (int)source->height)) {
    if (destinationY < 0) {
      destOffsetOrSourceStride = 0;
      sourcePixelOffset = source->width * -destinationY;
      copyHeight = copyHeight + destinationY;
      destinationY = 0;
    }
    else {
      sourcePixelOffset = 0;
      destOffsetOrSourceStride = destination->width * destinationY;
    }
    if (destinationX < 0) {
      sourcePixelOffset = sourcePixelOffset - destinationX;
      copyWidth = copyWidth + destinationX;
      destinationX = 0;
    }
    else {
      destOffsetOrSourceStride = destOffsetOrSourceStride + destinationX;
    }
    if ((int)destination->width < (int)(destinationX + copyWidth)) {
      copyWidth = copyWidth - ((destinationX + copyWidth) - destination->width);
    }
    if ((int)destination->height < (int)(destinationY + copyHeight)) {
      copyHeight = copyHeight - ((destinationY + copyHeight) - destination->height);
    }
    destStrideBytes = destination->width * pixelBytes;
    destRow = destination->pixels + destOffsetOrSourceStride * pixelBytes;
    destOffsetOrSourceStride = source->width * pixelBytes;
    rowBytes = pixelBytes * copyWidth;
    sourceRow = source->pixels + sourcePixelOffset * pixelBytes;
    if ((0 < (int)copyHeight) && (0 < (int)rowBytes)) {
      bytesOrWordsLeft = rowBytes;
      destRowStartOrSourceCursor = destRow;
      sourceRowStartOrDestCursor = sourceRow;
      if ((rowBytes & 3) != 0) {
        do {
          for (; bytesOrWordsLeft != 0; bytesOrWordsLeft = bytesOrWordsLeft - 1) {
            *destRow = *sourceRow;
            sourceRow = sourceRow + 1;
            destRow = destRow + 1;
          }
          destRow = destRowStartOrSourceCursor + destStrideBytes;
          sourceRow = sourceRowStartOrDestCursor + destOffsetOrSourceStride;
          copyHeight = copyHeight - 1;
          bytesOrWordsLeft = rowBytes;
          destRowStartOrSourceCursor = destRow;
          sourceRowStartOrDestCursor = sourceRow;
        } while (copyHeight != 0);
        return;
      }
      do {
        destRowStartOrSourceCursor = sourceRow;
        sourceRowStartOrDestCursor = destRow;
        for (bytesOrWordsLeft = rowBytes >> 2; bytesOrWordsLeft != 0; bytesOrWordsLeft = bytesOrWordsLeft - 1) {
          *(uint32_t *)sourceRowStartOrDestCursor = *(uint32_t *)destRowStartOrSourceCursor;
          destRowStartOrSourceCursor = destRowStartOrSourceCursor + 4;
          sourceRowStartOrDestCursor = sourceRowStartOrDestCursor + 4;
        }
        destRow = destRow + destStrideBytes;
        sourceRow = sourceRow + destOffsetOrSourceStride;
        copyHeight = copyHeight - 1;
      } while (copyHeight != 0);
    }
  }
  return;
}


/* Address: 0x004D1710.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 16: textured, Gouraud-shaded, depth-tested, opaque triangle.
   The nearest texel (paletted or direct colour, wrapped) is modulated by the interpolated colour; the
   pixel and its depth are written when the depth test passes.
*/
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

/* Address: 0x004D2990.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 22: byte-identical to mode 20 (textured, Gouraud-shaded,
   alpha-blended, alpha-tested depth write); see Raster16_DrawTexturedAlphaTested.
*/
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

void SoftwareRaster16_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004D3E90.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 17 (also table indices 48, 49, 52, 54): textured,
   Gouraud-shaded, depth-tested, alpha-blended triangle. The modulated texel is blended with the
   framebuffer by its alpha (g_SoftwareBlendAlphaFactors); the depth buffer is not written.
*/
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

/* Address: 0x004D5310.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 18 (also table index 50): textured, Gouraud-shaded,
   depth-tested, additive triangle. The modulated texel is added to the framebuffer with
   saturation; the depth buffer is not written.
*/
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 20 (byte-identical to mode 22): textured, Gouraud-shaded,
   depth-tested, alpha-blended triangle like mode 17, which also writes the depth where the
   modulated alpha is >= 128.
*/
void SoftwareRaster16_Mode20
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004D7B90.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 24: textured, flat-shaded (colour of v0), depth-tested,
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 30: byte-identical to mode 28 (textured, flat-shaded,
   alpha-blended, alpha-tested depth write); see Raster16_DrawTexturedAlphaTested.
*/
void SoftwareRaster16_Mode30
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004D9C10.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 25 (also table indices 56, 57, 60, 62): textured,
   flat-shaded (colour of v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 17.
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 26 (also table index 58): textured, flat-shaded,
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 28 (byte-identical to mode 30): textured, flat-shaded,
   depth-tested, alpha-blended triangle with the alpha-tested depth write of mode 20.
*/
void SoftwareRaster16_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawTexturedAlphaTested(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004DCE90.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 0: Gouraud-shaded, depth-tested, opaque triangle. A pixel is
   written (and its depth stored) when the interpolated depth is <= the depth buffer. Handler ABI: see
   docs/software_raster.md (five stack arguments, ret 0x14; the draw queue passes the prepared packet).
*/
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 6: Gouraud-shaded, depth-tested, alpha-blended triangle
   that also writes depth where the interpolated alpha is >= 128. Byte-identical to mode 4 in the
   original, which tests a stale MM2 instead of the alpha (see docs/software_raster.md, "Stale MM2");
   the C keeps the alpha rule.
*/
void SoftwareRaster16_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004DE0C0.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 1 (also table indices 32, 33, 36, 38): Gouraud-shaded,
   depth-tested, alpha-blended triangle. Visible pixels are blended with the framebuffer by the
   interpolated vertex alpha (g_SoftwareBlendAlphaFactors); the depth buffer is not written.
*/
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

/* Address: 0x004DEA40.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 2 (also table index 34): Gouraud-shaded, depth-tested,
   additive triangle. Visible pixels get the interpolated colour added with saturation; the depth
   buffer is not written.
*/
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 4: same as mode 6 (see SoftwareRaster16_Mode06).
*/
void SoftwareRaster16_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_GOURAUD, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004DFCB0.
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 8: flat-shaded (colour of v0), depth-tested, opaque
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 14: flat-shaded version of mode 6 (colour of v0).
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 9 (also table indices 40, 41, 44, 46): flat-shaded (colour of
   v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 1 with a constant colour; the
   depth buffer is not written.
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 10 (also table index 42): flat-shaded (colour of v0),
   depth-tested, additive triangle. Same pixel operation as mode 2 with a constant colour; the depth
   buffer is not written.
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
   Ownership: graphics/backend/software.
   Purpose: 16-bit framebuffer, render mode 12: same as mode 14 (see SoftwareRaster16_Mode14).
*/
void SoftwareRaster16_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster16_DrawAlphaBlendDepth(RASTER_SHADE_FLAT, clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004E1CC0.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 16: textured, Gouraud-shaded, depth-tested, opaque triangle
   (see SoftwareRaster16_Mode16). The nearest texel is modulated by the interpolated colour; the pixel and
   its depth are written when the depth test passes.
*/
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

void SoftwareRasterNon16_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedOpaque);
}

/* Address: 0x004E2E00.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render modes 20 and 22 (byte-identical in the original): textured,
   Gouraud-shaded, depth-tested, alpha-blended triangle with an alpha-tested depth write. Visible pixels
   are blended with the framebuffer by the modulated texel alpha; the depth is written only when that
   alpha lane, as an unsigned word, is >= 0x800 (alpha >= 128, or a negative lane).
*/
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
            if ((uint16_t)source.lane[RASTER_LANE_ALPHA] >= 0x800) {
                *span->depth = span->depthValue;
            }
        }
        RasterSpan_Next(span);
    }
}

void SoftwareRasterNon16_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaTested);
}

/* Address: 0x004E4180.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 17 (also table indices 48, 49, 52, 54): textured,
   Gouraud-shaded, depth-tested, alpha-blended triangle. Visible pixels are blended with the framebuffer
   by the modulated texel alpha; the depth buffer is not written.
*/
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

void SoftwareRasterNon16_Mode17
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaBlend);
}

/* Address: 0x004E5440.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 18 (also table index 50): textured, Gouraud-shaded,
   depth-tested, additive triangle. Visible pixels get the modulated texel added with saturation; the
   depth buffer is not written.
*/
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

void SoftwareRasterNon16_Mode18
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAdd);
}

/* Address: 0x004E65C0.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 20: byte-identical to mode 22 (see SoftwareRasterNon16_Mode22).
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 24: textured, flat-shaded (colour of v0), depth-tested,
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render modes 28 and 30 (byte-identical in the original): textured,
   flat-shaded (colour of v0), depth-tested, alpha-blended triangle with an alpha-tested depth write.
   Same pixel operation as modes 20/22.
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 25 (also table indices 56, 57, 60, 62): textured, flat-shaded
   (colour of v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 17.
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 26 (also table index 58): textured, flat-shaded (colour of v0),
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 28: byte-identical to mode 30 (see SoftwareRasterNon16_Mode30).
*/
void SoftwareRasterNon16_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaTested);
}

/* Address: 0x004EC3E0.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 0: Gouraud-shaded, depth-tested, opaque triangle (see
   SoftwareRaster16_Mode00).
*/
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

/* Address: 0x004ECB90 (Mode06) and 0x004EE4A0 (Mode04), byte-identical.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render modes 4 and 6: Gouraud-shaded, depth-tested, alpha-blended
   triangle (like SoftwareRasterNon16_Mode01) that also writes the depth of pixels whose source alpha
   is >= 128 (Raster_AlphaWritesDepth). The original tests a stale MM2 instead, which it never
   loads in these modes (see docs/software_raster.md); the C rule is deliberate.
*/
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

/* Render mode 6: see RasterNon16_DrawShadedAlphaBlendDepth. */
void SoftwareRasterNon16_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawShadedAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004ED440.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 1 (also table indices 32, 33, 36, 38): Gouraud-shaded,
   depth-tested, alpha-blended triangle; the depth buffer is not written (see
   SoftwareRaster16_Mode01).
*/
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

/* Address: 0x004EDCB0.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 2 (also table index 34): Gouraud-shaded, depth-tested,
   additive triangle; the depth buffer is not written (see SoftwareRaster16_Mode02).
*/
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 4: byte-identical to mode 6, see
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 8: flat-shaded (v0's colour), depth-tested, opaque
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

/* Address: 0x004EF230 (Mode14) and 0x004F02D0 (Mode12), byte-identical.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render modes 12 and 14: flat-shaded (v0's colour) version of modes
   4/6: alpha-blended, depth written when the source alpha is >= 128 (the original's stale-MM2 test
   is replaced as in RasterNon16_DrawShadedAlphaBlendDepth).
*/
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

/* Render mode 14: see RasterNon16_DrawFlatAlphaBlendDepth. */
void SoftwareRasterNon16_Mode14
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterNon16_DrawFlatAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Address: 0x004EF810.
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 9 (also table indices 40, 41, 44, 46): flat-shaded (v0's
   colour), depth-tested, alpha-blended triangle; the depth buffer is not written (flat version of
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 10 (also table index 42): flat-shaded (v0's colour),
   depth-tested, additive triangle; the depth buffer is not written (flat version of
   SoftwareRasterNon16_Mode02).
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
   Ownership: graphics/backend/software.
   Purpose: 32-bit framebuffer, render mode 12: byte-identical to mode 14, see
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
    int writeDepth = uPrestep >= 0x800;

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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 16: textured, Gouraud-shaded, depth-tested, opaque
   triangle (see SoftwareRaster16_Mode16).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 22, byte-identical to mode 20 (see
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 17: textured, Gouraud-shaded, depth-tested triangle
   without depth write. Unlike the framebuffer families there is no alpha blend: the pixel is
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 18, byte-identical to mode 17 (no additive blend in
   this family; see SoftwareRasterAux_Mode17).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 20: textured, Gouraud-shaded, depth-tested triangle.
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 24: flat-shaded version of mode 16 (textured,
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 30, byte-identical to mode 28 (see
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 25: flat-shaded version of mode 17 (textured,
   depth-tested, pixel overwritten, no depth write).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 26, byte-identical to mode 25 (see
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 28: flat-shaded version of mode 20 (textured,
   depth-tested, pixel overwritten, depth written for spans whose U prestep is >= 0x800). Also used
   for mode 30.
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target (SoftwareRenderer_DrawQueueAuxiliary), render mode 0: Gouraud-shaded,
   depth-tested, opaque triangle (see SoftwareRaster16_Mode00). Target and depth rows are clipMaxX
   pixels long.
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

/* Address: 0x004FADD0.
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 6 (byte-identical to mode 4): Gouraud-shaded,
   depth-tested, opaque colour write. The depth buffer is written only by pixels that pass
   Raster_AlphaWritesDepth (the original's stale-MM2 test, see software_raster.h).
*/

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

/* Opaque shaded write; the depth buffer is written only where the alpha rule allows it. */
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

void SoftwareRasterAux_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Address: 0x004FB5A0.
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 1 (also table indices 32, 33, 36, 38; byte-identical to
   mode 2): Gouraud-shaded, depth-tested colour write without depth write. Unlike the framebuffer
   families there is no blending: the original loads the destination pixel but never uses it.
*/
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

void SoftwareRasterAux_Mode01
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedWrite);
}

/* Address: 0x004FBD70.
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 2 (also table index 34): same code as
   SoftwareRasterAux_Mode01 (no additive blend in this family).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 4: same code as SoftwareRasterAux_Mode06.
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 8: flat-shaded version of SoftwareRasterAux_Mode00
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 14 (byte-identical to mode 12): flat-shaded version of
   SoftwareRasterAux_Mode06 (opaque colour write, depth written by the alpha rule).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 9 (also table indices 40, 41, 44, 46; byte-identical to
   mode 10): flat-shaded version of SoftwareRasterAux_Mode01 (colour write, no depth write).
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 10 (also table index 42): same code as
   SoftwareRasterAux_Mode09.
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
   Ownership: graphics/backend/software.
   Purpose: auxiliary 32-bit target, render mode 12: same code as SoftwareRasterAux_Mode14.
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
   Ownership: graphics/backend/software.
   Purpose: Calls the previous display-mode hook, updates the software depth buffer, selects the raster backend
   from g_FramebufferAccess->bytesPerPixel, and rebuilds the runtime MMX pixel constants. ABI: CF clear means
   success. CF set means failure. Typed parameters: p0 modeArg0→DisplayModeHookArgument0_V345, p1
   modeArg1→DisplayModeHookArgument1_V345. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
DisplayModeEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SoftwareRenderer_DisplayModeHook
          (DisplayModeHookArgument0 modeArg0,DisplayModeHookArgument1 modeArg1,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width)

{
  int32_t *memory;
  uint32_t blueUnpackScale;
  uint8_t redBits;
  uint8_t greenBits;
  uint8_t blueBits;
  DisplayModeEaxCf5 hookResult;
  
  hookResult = (*g_SoftwarePreviousDisplayModeHook)(modeArg0,modeArg1,height,width);
  if (!hookResult.carry) {
    g_SoftwareDepthRowStrideBytes = width * 4;
    if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
      g_SoftwareDrawQueueProc = SoftwareRenderer_DrawQueue16Bit;
    }
    else {
      g_SoftwareDrawQueueProc = SoftwareRenderer_DrawQueueNon16Bit;
    }
    hookResult = THANDOR_BITCAST(ArenaAllocEaxCf5, DisplayModeEaxCf5, (*g_MemoryApi.alloc)(g_SoftwareDepthRowStrideBytes * height));
    memory = g_SoftwareDepthBuffer;
    if (!hookResult.carry) {
      LOCK();
      UNLOCK();
      g_SoftwareDepthBuffer = (int32_t *)hookResult.eax;
      (*g_MemoryApi.free)(memory);
      g_SoftwareDepthEpoch = 0;
      redBits = (uint8_t)g_SoftwarePixelFormatConfig.redBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.red =
           (SoftwareColorLaneFixed16)((1 << (redBits & 0x1f)) + -1 << (0xc - redBits & 0x1f));
      greenBits = (uint8_t)g_SoftwarePixelFormatConfig.greenBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.green =
           (SoftwareColorLaneFixed16)((1 << (greenBits & 0x1f)) + -1 << (0xc - greenBits & 0x1f));
      blueBits = (uint8_t)g_SoftwarePixelFormatConfig.blueBitCount;
      g_SoftwarePixelMmxConstants.quantizeMasksQ12.blue =
           (SoftwareColorLaneFixed16)((1 << (blueBits & 0x1f)) + -1 << (0xc - blueBits & 0x1f));
      g_SoftwarePixelMmxConstants.packWeights.red =
           (SoftwareColorLaneFixed16)
           (1 << ((redBits + (char)g_SoftwarePixelFormatConfig.redShift) - 4 & 0x1f));
      g_SoftwarePixelMmxConstants.packWeights.green =
           (SoftwareColorLaneFixed16)
           (1 << ((greenBits + (char)g_SoftwarePixelFormatConfig.greenShift) - 4 & 0x1f));
      g_SoftwarePixelMmxConstants.packWeights.blue =
           (SoftwareColorLaneFixed16)
           (1 << ((blueBits + (char)g_SoftwarePixelFormatConfig.blueShift) - 4 & 0x1f));
      g_SoftwarePixelMmxConstants.packedPixelMasks.red =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.redMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.green =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.greenMask;
      g_SoftwarePixelMmxConstants.packedPixelMasks.blue =
           (SoftwareColorLaneFixed16)g_SoftwarePixelFormatConfig.blueMask;
      g_SoftwarePixelMmxConstants.unpackScales.red =
           (SoftwareColorLaneFixed16)
           (1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.redShift) - redBits & 0x1f));
      g_SoftwarePixelMmxConstants.unpackScales.green =
           (SoftwareColorLaneFixed16)
           (1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.greenShift) - greenBits & 0x1f));
      blueUnpackScale = 1 << (('\x10' - (char)g_SoftwarePixelFormatConfig.blueShift) - blueBits & 0x1f);
      g_SoftwarePixelMmxConstants.unpackScales.blue = (SoftwareColorLaneFixed16)blueUnpackScale;
      hookResult.carry = false;
      hookResult.eax = blueUnpackScale;
    }
  }
  return hookResult;
}


/* Address: 0x004FE7D0.
   Ownership: graphics/backend/software.
   Purpose: Installs SoftwareRenderer_DisplayModeHook after SoftwarePixelFormat_BaseDisplayModeHook and allocates
   the initial software depth buffer. ABI: CF clear means success. CF set means failure.
*/
StatusValueEaxCf5 __cdecl SoftwareRenderer_InstallDisplayModeHook(void)

{
  int32_t *allocatedDepthBuffer;
  bool framebufferPixelFormatTooNarrow;
  ArenaAllocEaxCf5 depthAllocation;
  
  g_SoftwarePreviousDisplayModeHook = g_GraphicsDisplayModeHook;
  g_SoftwareDepthRowStrideBytes = g_FramebufferWidth * 4;
  LOCK();
  g_GraphicsDisplayModeHook = SoftwareRenderer_DisplayModeHook;
  UNLOCK();
  if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    g_SoftwareDrawQueueProc = SoftwareRenderer_DrawQueue16Bit;
  }
  else {
    g_SoftwareDrawQueueProc = SoftwareRenderer_DrawQueueNon16Bit;
  }
  depthAllocation = (*g_MemoryApi.alloc)(g_SoftwareDepthRowStrideBytes * g_FramebufferHeight);
  allocatedDepthBuffer = (int32_t *)depthAllocation.eax;
  if (!depthAllocation.carry) {
    g_SoftwareDepthBuffer = allocatedDepthBuffer;
    g_SoftwareDepthEpoch = 0;
    return StatusValue_Ok(0);
  }
  return StatusValue_Fail(depthAllocation.eax);
}


/* One byte of the cross-fade in SoftwareTexture_BilinearBlendScaleSubresources: both images and the
   factor are widened to (c * 0x101) >> 2 (PUNPCKLBW + PSRLW 2), then
   (b * (unity - f) + a * f) >> 16 per product (PMULHW), >> 4 (PSRLW) and saturated (PACKUSWB). */
static uint8_t SoftwareTexture_CrossFadeByte(uint8_t a, uint8_t b, uint8_t factor, short unity)
{
    short wideA = (short)((a * 0x101) >> 2);
    short wideB = (short)((b * 0x101) >> 2);
    short wideFactor = (short)((factor * 0x101) >> 2);
    uint16_t sum = (uint16_t)(Raster_MulHigh(wideB, (short)(unity - wideFactor)) + Raster_MulHigh(wideA, wideFactor));
    sum = (uint16_t)(sum >> 4);
    return (uint8_t)(sum > 0xff ? 0xff : sum);
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
static uint32_t SoftwareTexture_SampleIntensity(const uint8_t *row, uint32_t sourceWidth, uint32_t xFixed, short upperWeight,
                                             short lowerWeight)
{
    const short *weights = (const short *)(&g_SoftwareBilinearPackedInterpolationWeights256 + (xFixed & 0xff) * 8);
    const uint8_t *upper = row + (xFixed >> 8);
    const uint8_t *lower = upper + sourceWidth;
    uint32_t upperSum = (uint32_t)(((upper[0] * 0x101) >> 2) * weights[0] + ((upper[1] * 0x101) >> 2) * weights[1]);
    uint32_t lowerSum = (uint32_t)(((lower[0] * 0x101) >> 2) * weights[0] + ((lower[1] * 0x101) >> 2) * weights[1]);
    uint16_t sum = (uint16_t)(Raster_MulHigh((short)(upperSum >> 16), upperWeight) +
                      Raster_MulHigh((short)(lowerSum >> 16), lowerWeight));
    uint32_t intensity = (uint32_t)(sum >> 2);
    return intensity > 0xff ? 0xff : intensity;
}

/* Address: 0x00518CE0.
   Ownership: graphics/backend/software.
   Purpose: Draws the cross-fade of two 8-bit subresources of a texture source, scaled to
   destinationWidth x destinationHeight at (destinationLeft, destinationTop) of the software
   framebuffer (16 or 32 bit), as grey levels. Used by UiSoftwareTexturePreviewControl.
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
void __thandor_void_preserve_eax_ecx_edx
SoftwareTexture_BilinearBlendScaleSubresources
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
   Ownership: graphics/backend/software.
   Purpose: Subtracts 0x01000000 from the depth epoch. On unsigned underflow or zero, fills the width*height depth
   buffer with 0xFFFFFFFF and resets the epoch to 0xFF000000.
*/
void __thandor_void_preserve_eax_ecx SoftwareRenderer_AdvanceDepthEpoch(void)

{
  int pixelsRemaining;
  int32_t *depthValueCursor;
  bool depthEpochWrapped;
  
  depthEpochWrapped = (uint32_t)g_SoftwareDepthEpoch < 0x1000000;
  g_SoftwareDepthEpoch = g_SoftwareDepthEpoch + -0x1000000;
  if (depthEpochWrapped || g_SoftwareDepthEpoch == 0) {
    depthValueCursor = g_SoftwareDepthBuffer;
    for (pixelsRemaining = g_FramebufferWidth * g_FramebufferHeight; pixelsRemaining != 0;
        pixelsRemaining = pixelsRemaining + -1) {
      *depthValueCursor = -1;
      depthValueCursor = depthValueCursor + 1;
    }
    g_SoftwareDepthEpoch = -0x1000000;
  }
  return;
}


/* Address: 0x00519210.
   Ownership: graphics/backend/software.
   Purpose: Clears the complete software mask buffer at runtime offset 0x60 using the logical dimensions of the
   associated texture source. Typed parameters: p2 maskControl→SoftwareMaskRuntimeAddress32_V345. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl)

{
  uint64_t *maskQwordWriteCursor;
  uint32_t qwordBlocksRemaining;
  uint64_t maskLogicalSizePair;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  maskQwordWriteCursor = (uint64_t *)maskControl->maskPixels;
  if (maskQwordWriteCursor != (uint64_t *)0x0) {
    logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskControl->textureSource);
    qwordBlocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 6;
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
      qwordBlocksRemaining = qwordBlocksRemaining - 1;
    } while (qwordBlocksRemaining != 0);
  }
  return;
}


/* Address: 0x00519270.
   Ownership: graphics/backend/software.
   Purpose: Adds 0x1F to every nonzero byte of the software mask, saturating at 0xFF (PCMPEQB / PAND / PXOR /
   PADDUSB); zero bytes stay zero. The mask size is taken from g_GraphicsTextureSourceGetLogicalSize, and the
   buffer is processed in 32-byte blocks, width * height >> 5 of them (the remainder is left alone). Quirk kept:
   the block counter is a do-while loop, so fewer than 32 pixels means 2^32 blocks. Nothing happens when
   maskPixels is NULL. ABI: all registers are preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime)

{
  uint8_t *mask;
  uint32_t blocksLeft;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  int i;

  mask = maskRuntime->maskPixels;
  if (mask == NULL) {
    return;
  }
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
  blocksLeft = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 5;
  do {
    for (i = 0; i < 32; i++) {
      if (mask[i] != 0) {
        mask[i] = (uint8_t)(mask[i] < 0xff - 0x1f ? mask[i] + 0x1f : 0xff);
      }
    }
    mask += 32;
  } while (--blocksLeft != 0);
}


/* Address: 0x00519500.
   Ownership: graphics/backend/software.
   Purpose: Typed parameters: p3 centerY→GraphicsScreenCoordinate_V307, p4 centerX→GraphicsScreenCoordinate_V307.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p2 invertSelection→UiBooleanState32_V342. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p5 radiusStep→SoftwareMaskRadiusStep_V344.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_ApplyCircularRegionBit
          (UiBooleanState32 invertSelection,GraphicsScreenCoordinate centerY,
          GraphicsScreenCoordinate centerX,SoftwareMaskRadiusStep radiusStep,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t maskWidth;
  int radiusPixels;
  int rowDistanceSquared;
  uint32_t rowsRemaining;
  uint32_t columnX;
  uint8_t *maskCursor;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  int rowY;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  radiusPixels = radiusStep * 0x1c;
  maskCursor = maskRuntime->maskPixels;
  if ((invertSelection != 0) && (radiusPixels = radiusStep * -0x1c + maskWidth + rowsRemaining, radiusPixels < 0)) {
    radiusPixels = 0;
  }
  columnX = 0;
  rowY = 0;
  rowDistanceSquared = -centerY * -centerY;
  if (invertSelection == 0) {
    do {
      do {
        if ((columnX - centerX) * (columnX - centerX) + rowDistanceSquared <= (uint32_t)(radiusPixels * radiusPixels)) {
          *maskCursor = *maskCursor | 1;
        }
        columnX = columnX + 1;
        maskCursor = maskCursor + 1;
      } while (columnX < maskWidth);
      rowY = rowY + 1;
      columnX = 0;
      rowDistanceSquared = (rowY - centerY) * (rowY - centerY);
      rowsRemaining = rowsRemaining - 1;
    } while (rowsRemaining != 0);
    return;
  }
  do {
    do {
      if ((uint32_t)(radiusPixels * radiusPixels) <= (columnX - centerX) * (columnX - centerX) + rowDistanceSquared) {
        *maskCursor = *maskCursor | 1;
      }
      columnX = columnX + 1;
      maskCursor = maskCursor + 1;
    } while (columnX < maskWidth);
    rowY = rowY + 1;
    columnX = 0;
    rowDistanceSquared = (rowY - centerY) * (rowY - centerY);
    rowsRemaining = rowsRemaining - 1;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x005195D0.
   Ownership: graphics/backend/software.
   Purpose: Sets bit zero on one side of a diagonal half-plane threshold across the complete mask, with the
   direction selected by the inversion flag. Typed parameters: p2 invertSelection→UiBooleanState32_V342. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p3 thresholdStep→SoftwareMaskThresholdStep_V344. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
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
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
  rowsRemaining = logicalSize.logicalHeightPixels;
  maskWidth = logicalSize.logicalWidthPixels;
  thresholdSum = thresholdStep * 0x28;
  maskCursor = maskRuntime->maskPixels;
  if (invertSelection != 0) {
    thresholdSum = thresholdStep * -0x28 + maskWidth + rowsRemaining;
  }
  rowY = 0;
  columnsRemaining = maskWidth;
  diagonalSum = 0;
  if (invertSelection == 0) {
    do {
      do {
        if (diagonalSum < thresholdSum) {
          *maskCursor = *maskCursor | 1;
        }
        maskCursor = maskCursor + 1;
        columnsRemaining = columnsRemaining - 1;
        diagonalSum = diagonalSum + 1;
      } while (columnsRemaining != 0);
      rowY = rowY + 1;
      rowsRemaining = rowsRemaining - 1;
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
      maskCursor = maskCursor + 1;
      columnsRemaining = columnsRemaining - 1;
      diagonalSum = diagonalSum + 1;
    } while (columnsRemaining != 0);
    rowY = rowY + 1;
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = maskWidth;
    diagonalSum = rowY;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00519670.
   Ownership: graphics/backend/software.
   Purpose: Sets bit zero for every pixel in the complete software mask buffer using its logical texture
   dimensions. Typed parameters: p2 maskControl→SoftwareMaskRuntimeAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_preserve_eax_edx
SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl)

{
  uint32_t maskBlocksRemaining;
  uint32_t *maskWordCursor;
  uint64_t maskLogicalSizePair;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskControl->textureSource);
  maskWordCursor = (uint32_t *)maskControl->maskPixels;
  maskBlocksRemaining = logicalSize.logicalHeightPixels * logicalSize.logicalWidthPixels >> 4;
  do {
    *maskWordCursor = *maskWordCursor | 0x1010101;
    maskWordCursor[1] = maskWordCursor[1] | 0x1010101;
    maskWordCursor[2] = maskWordCursor[2] | 0x1010101;
    maskWordCursor[3] = maskWordCursor[3] | 0x1010101;
    maskWordCursor = maskWordCursor + 4;
    maskBlocksRemaining = maskBlocksRemaining - 1;
  } while (maskBlocksRemaining != 0);
  return;
}


/* Address: 0x005196C0.
   Ownership: graphics/backend/software.
   Purpose: Sets bit zero in one 15-row horizontal band selected from 25 positions, with forward or reverse row
   ordering chosen by the direction flag. Typed parameters: p3 bandIndex→TerrainGridMaskIndex_V304. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged. Typed parameters: p2
   reverseRows→UiBooleanState32_V342.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareMaskBuffer_ApplyHorizontalBandBit
          (UiBooleanState32 reverseRows,TerrainGridMaskIndex bandIndex,
          SoftwareMaskRuntimeView *maskRuntime)

{
  uint32_t bandBytesOrBlocksLeft;
  int bandRow;
  uint32_t *maskWordCursor;
  GraphicsTextureSizeEaxEdxCf9 logicalSize;
  
  logicalSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,maskRuntime->textureSource);
  if ((uint32_t)bandIndex < 0x19) {
    bandBytesOrBlocksLeft = logicalSize.logicalWidthPixels * 0xf;
    if (reverseRows == 0) {
      bandRow = bandIndex + -1;
      if (bandRow < 0) {
        return;
      }
    }
    else {
      bandRow = 0x18 - bandIndex;
    }
    maskWordCursor = (uint32_t *)(maskRuntime->maskPixels + bandRow * bandBytesOrBlocksLeft);
    bandBytesOrBlocksLeft = bandBytesOrBlocksLeft >> 4;
    do {
      *maskWordCursor = *maskWordCursor | 0x1010101;
      maskWordCursor[1] = maskWordCursor[1] | 0x1010101;
      maskWordCursor[2] = maskWordCursor[2] | 0x1010101;
      maskWordCursor[3] = maskWordCursor[3] | 0x1010101;
      maskWordCursor = maskWordCursor + 4;
      bandBytesOrBlocksLeft = bandBytesOrBlocksLeft - 1;
    } while (bandBytesOrBlocksLeft != 0);
  }
  return;
}


/* Address: 0x004FE840.
   Ownership: graphics/backend/software.
   Purpose: Reorders the three 0x20-byte vertices by screen Y, truncates screen X/Y to Q12 pixel boundaries, adds
   the current depth epoch, updates flat-shading flags, and rescales texture coordinates from textureEntry
   widthLog2/heightLog2.
*/
void __thandor_void_preserve_eax_ecx_edx
SoftwareRenderer_PrepareTrianglePacket(GraphicsPrimitivePacket *packet)

{
  GraphicsPrimitivePacket *thirdVertexSlot;
  GraphicsPrimitiveScreenCoordinate *screenYField;
  GraphicsPrimitiveDepthFixed *depthField;
  GraphicsPrimitiveTextureCoordinateFixed *textureCoordField;
  int screenY0;
  int screenY1;
  int screenY2;
  PackedArgb32 movedColorOrFirstColor;
  PackedArgb32 savedColorOrSecondColor;
  PackedArgb32 thirdColor;
  GraphicsTextureSetEntry *textureEntryRef;
  GraphicsPrimitiveScreenCoordinate movedScreenY;
  GraphicsPrimitiveBackendCoordinate movedBackendCoord0;
  GraphicsPrimitiveBackendCoordinate movedBackendCoord1;
  GraphicsPrimitiveDepthFixed movedDepth;
  GraphicsPrimitiveTextureCoordinateFixed movedTextureU;
  GraphicsPrimitiveTextureCoordinateFixed movedTextureV;
  GraphicsPrimitiveScreenCoordinate savedScreenX;
  GraphicsPrimitiveScreenCoordinate savedScreenY;
  GraphicsPrimitiveBackendCoordinate savedBackendCoord0;
  GraphicsPrimitiveBackendCoordinate savedBackendCoord1;
  GraphicsPrimitiveDepthFixed savedDepth;
  GraphicsPrimitiveTextureCoordinateFixed savedTextureU;
  GraphicsPrimitiveTextureCoordinateFixed savedTextureV;
  int32_t depthEpoch;
  uint8_t texelShift;
  GraphicsPrimitivePacket *swapTarget;
  GraphicsPrimitivePacket *swapSource;
  GraphicsPrimitivePacket *rotateSource;
  
  screenY0 = packet->vertices[0].screenY;
  screenY1 = packet->vertices[1].screenY;
  screenY2 = packet->vertices[2].screenY;
  rotateSource = (GraphicsPrimitivePacket *)(packet->vertices + 1);
  thirdVertexSlot = (GraphicsPrimitivePacket *)(packet->vertices + 2);
  swapSource = thirdVertexSlot;
  if (screenY1 < screenY0) {
    swapTarget = packet;
    if ((screenY1 <= screenY2) &&
       (swapTarget = (GraphicsPrimitivePacket *)(packet->vertices + 1), swapSource = packet,
       rotateSource = thirdVertexSlot, screenY2 < screenY0))
    goto SoftwareRenderer_PrepareTrianglePacket_RotateThreeVerticesForScreenYOrdering;
  }
  else {
    swapTarget = thirdVertexSlot;
    if (screenY2 < screenY0) {
SoftwareRenderer_PrepareTrianglePacket_RotateThreeVerticesForScreenYOrdering:
      savedScreenX = swapTarget->vertices[0].screenX;
      savedScreenY = swapTarget->vertices[0].screenY;
      savedBackendCoord0 = swapTarget->vertices[0].backendCoord0;
      savedBackendCoord1 = swapTarget->vertices[0].backendCoord1;
      savedDepth = swapTarget->vertices[0].depth;
      savedTextureU = swapTarget->vertices[0].textureU;
      savedTextureV = swapTarget->vertices[0].textureV;
      savedColorOrSecondColor = swapTarget->vertices[0].diffuseColor;
      movedScreenY = rotateSource->vertices[0].screenY;
      movedBackendCoord0 = rotateSource->vertices[0].backendCoord0;
      movedBackendCoord1 = rotateSource->vertices[0].backendCoord1;
      movedDepth = rotateSource->vertices[0].depth;
      movedTextureU = rotateSource->vertices[0].textureU;
      movedTextureV = rotateSource->vertices[0].textureV;
      movedColorOrFirstColor = rotateSource->vertices[0].diffuseColor;
      swapTarget->vertices[0].screenX = rotateSource->vertices[0].screenX;
      swapTarget->vertices[0].screenY = movedScreenY;
      swapTarget->vertices[0].backendCoord0 = movedBackendCoord0;
      swapTarget->vertices[0].backendCoord1 = movedBackendCoord1;
      swapTarget->vertices[0].depth = movedDepth;
      swapTarget->vertices[0].textureU = movedTextureU;
      swapTarget->vertices[0].textureV = movedTextureV;
      swapTarget->vertices[0].diffuseColor = movedColorOrFirstColor;
      movedScreenY = packet->vertices[0].screenY;
      movedBackendCoord0 = packet->vertices[0].backendCoord0;
      movedBackendCoord1 = packet->vertices[0].backendCoord1;
      movedDepth = packet->vertices[0].depth;
      movedTextureU = packet->vertices[0].textureU;
      movedTextureV = packet->vertices[0].textureV;
      movedColorOrFirstColor = packet->vertices[0].diffuseColor;
      rotateSource->vertices[0].screenX = packet->vertices[0].screenX;
      rotateSource->vertices[0].screenY = movedScreenY;
      rotateSource->vertices[0].backendCoord0 = movedBackendCoord0;
      rotateSource->vertices[0].backendCoord1 = movedBackendCoord1;
      rotateSource->vertices[0].depth = movedDepth;
      rotateSource->vertices[0].textureU = movedTextureU;
      rotateSource->vertices[0].textureV = movedTextureV;
      rotateSource->vertices[0].diffuseColor = movedColorOrFirstColor;
      packet->vertices[0].screenX = savedScreenX;
      packet->vertices[0].screenY = savedScreenY;
      packet->vertices[0].backendCoord0 = savedBackendCoord0;
      packet->vertices[0].backendCoord1 = savedBackendCoord1;
      packet->vertices[0].depth = savedDepth;
      packet->vertices[0].textureU = savedTextureU;
      packet->vertices[0].textureV = savedTextureV;
      packet->vertices[0].diffuseColor = savedColorOrSecondColor;
      goto SoftwareRenderer_PrepareTrianglePacket_QuantizeOrderedVerticesAndPrepareFlags;
    }
    swapTarget = rotateSource;
    if (screenY1 <= screenY2)
    goto SoftwareRenderer_PrepareTrianglePacket_QuantizeOrderedVerticesAndPrepareFlags;
  }
  savedScreenX = swapTarget->vertices[0].screenX;
  savedScreenY = swapTarget->vertices[0].screenY;
  savedBackendCoord0 = swapTarget->vertices[0].backendCoord0;
  savedBackendCoord1 = swapTarget->vertices[0].backendCoord1;
  savedDepth = swapTarget->vertices[0].depth;
  savedTextureU = swapTarget->vertices[0].textureU;
  savedTextureV = swapTarget->vertices[0].textureV;
  savedColorOrSecondColor = swapTarget->vertices[0].diffuseColor;
  movedScreenY = swapSource->vertices[0].screenY;
  movedBackendCoord0 = swapSource->vertices[0].backendCoord0;
  movedBackendCoord1 = swapSource->vertices[0].backendCoord1;
  movedDepth = swapSource->vertices[0].depth;
  movedTextureU = swapSource->vertices[0].textureU;
  movedTextureV = swapSource->vertices[0].textureV;
  movedColorOrFirstColor = swapSource->vertices[0].diffuseColor;
  swapTarget->vertices[0].screenX = swapSource->vertices[0].screenX;
  swapTarget->vertices[0].screenY = movedScreenY;
  swapTarget->vertices[0].backendCoord0 = movedBackendCoord0;
  swapTarget->vertices[0].backendCoord1 = movedBackendCoord1;
  swapTarget->vertices[0].depth = movedDepth;
  swapTarget->vertices[0].textureU = movedTextureU;
  swapTarget->vertices[0].textureV = movedTextureV;
  swapTarget->vertices[0].diffuseColor = movedColorOrFirstColor;
  swapSource->vertices[0].screenX = savedScreenX;
  swapSource->vertices[0].screenY = savedScreenY;
  swapSource->vertices[0].backendCoord0 = savedBackendCoord0;
  swapSource->vertices[0].backendCoord1 = savedBackendCoord1;
  swapSource->vertices[0].depth = savedDepth;
  swapSource->vertices[0].textureU = savedTextureU;
  swapSource->vertices[0].textureV = savedTextureV;
  swapSource->vertices[0].diffuseColor = savedColorOrSecondColor;
SoftwareRenderer_PrepareTrianglePacket_QuantizeOrderedVerticesAndPrepareFlags:
  depthEpoch = g_SoftwareDepthEpoch;
  movedColorOrFirstColor = packet->vertices[0].diffuseColor;
  savedColorOrSecondColor = packet->vertices[1].diffuseColor;
  thirdColor = packet->vertices[2].diffuseColor;
  packet->vertices[0].screenX = packet->vertices[0].screenX & 0xfffff000;
  screenYField = &packet->vertices[0].screenY;
  *screenYField = *screenYField & 0xfffff000;
  depthField = &packet->vertices[0].depth;
  *depthField = *depthField + depthEpoch;
  packet->vertices[1].screenX = packet->vertices[1].screenX & 0xfffff000;
  screenYField = &packet->vertices[1].screenY;
  *screenYField = *screenYField & 0xfffff000;
  depthField = &packet->vertices[1].depth;
  *depthField = *depthField + depthEpoch;
  packet->vertices[2].screenX = packet->vertices[2].screenX & 0xfffff000;
  screenYField = &packet->vertices[2].screenY;
  *screenYField = *screenYField & 0xfffff000;
  depthField = &packet->vertices[2].depth;
  *depthField = *depthField + depthEpoch;
  packet->renderFlags = packet->renderFlags & 0xffff7fff;
  if ((movedColorOrFirstColor == savedColorOrSecondColor) && (movedColorOrFirstColor == thirdColor)) {
    packet->renderFlags = packet->renderFlags | 0x8000;
  }
  if ((packet->renderFlags & 0x10000) != 0) {
    textureEntryRef = packet->textureEntry;
    texelShift = 8 - (char)textureEntryRef->widthLog2;
    textureCoordField = &packet->vertices[0].textureU;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
    textureCoordField = &packet->vertices[1].textureU;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
    textureCoordField = &packet->vertices[2].textureU;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
    texelShift = 8 - (char)textureEntryRef->heightLog2;
    textureCoordField = &packet->vertices[0].textureV;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
    textureCoordField = &packet->vertices[1].textureV;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
    textureCoordField = &packet->vertices[2].textureV;
    *textureCoordField = *textureCoordField >> (texelShift & 0x1f);
  }
  return;
}

