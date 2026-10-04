/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_rasterizer.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Software triangle rasterizer: the primitive queue walkers per pixel family (32-bit framebuffer,
   auxiliary target), the 64-entry raster mode handler tables, the triangle packet set-up and the depth
   epoch. The original's 16-bit family (RGB565 framebuffer) is gone with 16-bit colour. */

#include <thandor/graphics/backend/software_rasterizer.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_raster.h"

/* Texture of the auxiliary textured modes. The span function of modes 20/22/28/30 needs the long
   edge's current U (see RasterAux_SpanTexturedPrestepDepth), so the walker's edges travel along
   with the texture; span->texture points at `texture`, the first member. */
typedef struct RasterAuxTexture {
    RasterTexture texture;
    const RasterEdges *edges;
} RasterAuxTexture;

/* Module data. */

THANDOR_ALIGN(16) SoftwareRasterScanState g_SoftwareRasterScanState = {0};

int32_t *g_SoftwareDepthBuffer = 0;

SoftwareDrawQueueProc *g_SoftwareDrawQueue = 0;

uint32_t g_SoftwareDepthRowStrideBytes = 0;

void *g_SoftwareAuxiliaryTargetBase = 0;

int32_t g_SoftwareDepthEpoch = 0;

/* g_GraphicsSetViewportAndClearDepth: fills the rectangle with opaque black and starts a
   new depth epoch instead of clearing a depth buffer.
*/
void SoftwareRenderer_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX)

{
  Bool8 accessFailed;
  
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

/* Queue renderer for the 32-bit framebuffer (installed in g_SoftwareDrawQueue by SoftwareRenderer_SetDisplayMode):
   prepares every packet of the queue and draws it with the raster handler its render flags select.
*/
void SoftwareRenderer_DrawQueue32Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  GraphicsPrimitivePacket *packet;

  packet = GraphicsPrimitiveQueue_Begin(queue);
  while (packet != NULL) {
    SoftwareRenderer_PrepareTrianglePacket(packet);
    /* the handler index is bits 12..17 of the flags */
    (*g_SoftwareRasterHandlers32Bit[(packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 12])
              (clipMaxY,clipMaxX,clipMinY,clipMinX,packet);
    g_PrimitiveDrawCallCount++;
    packet = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}

/* Queue renderer for an off-screen 32-bit target (GraphicsOffscreen_RenderModelListToTextureSource): like
   SoftwareRenderer_DrawQueue32Bit, but it draws into targetBase, whose rows are clipMaxX pixels long, through
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
      /* the handler index is bits 12..17 of the flags */
      (*g_SoftwareRasterHandlersAuxiliary[(packet->renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 12])
                (clipMaxY,clipMaxX,0,0,packet);
      g_PrimitiveDrawCallCount++;
    }
    packet = GraphicsPrimitiveQueue_Next(queue);
  }
  return;
}

/* g_GraphicsDrawPrimitiveQueue: locks the framebuffer and hands the queue to the queue
   renderer (g_SoftwareDrawQueue); draws nothing when the lock fails.
*/
void SoftwareRenderer_DrawPrimitiveQueueBridge(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue)

{
  Bool8 accessFailed;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    g_SoftwareDrawQueue(clipMaxY,clipMaxX,clipMinY,clipMinX,queue);
    g_GraphicsFramebufferEndAccess();
  }
  return;
}

/* 32-bit span of the textured opaque modes 16/24: the nearest texel modulated by the
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
   All textured 32-bit handlers share this setup; they differ only in shading and pixel operation. */
static void Raster32_DrawTextured(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
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

/* g_SoftwareRasterHandlers32Bit entry 16 (render mode 16): textured, Gouraud-shaded, depth-tested, opaque
   triangle on the 32-bit framebuffer.
*/
void SoftwareRaster32_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedOpaque);
}

/* Span of the 32-bit modes 20/22/28/30: visible pixels are blended with the framebuffer by the modulated
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

/* g_SoftwareRasterHandlers32Bit entry 22 (render mode 22): textured, Gouraud-shaded, depth-tested,
   alpha-blended triangle with the alpha-tested depth write (Raster32_SpanTexturedAlphaTested). Byte-identical to
   mode 20 in the original.
*/
void SoftwareRaster32_Mode22
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaTested);
}

/* Span of the 32-bit modes 17/25: visible pixels are blended with the framebuffer by the modulated texel
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

/* g_SoftwareRasterHandlers32Bit entries 17 and 48, 49, 52, 54 (render mode 17): textured, Gouraud-shaded,
   depth-tested, alpha-blended triangle without depth write (Raster32_SpanTexturedAlphaBlend).
*/
void SoftwareRaster32_Mode17
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaBlend);
}

/* Span of the 32-bit modes 18/26: visible pixels get the modulated texel added with saturation; the depth
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

/* g_SoftwareRasterHandlers32Bit entries 18 and 50 (render mode 18): textured, Gouraud-shaded, depth-tested,
   additive triangle without depth write (Raster32_SpanTexturedAdd).
*/
void SoftwareRaster32_Mode18
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAdd);
}

/* g_SoftwareRasterHandlers32Bit entry 20 (render mode 20): byte-identical to mode 22 (see
   SoftwareRaster32_Mode22).
*/
void SoftwareRaster32_Mode20
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanTexturedAlphaTested);
}

/* g_SoftwareRasterHandlers32Bit entry 24 (render mode 24): textured, flat-shaded (colour of v0), depth-tested,
   opaque triangle. Same pixel operation as mode 16.
*/
void SoftwareRaster32_Mode24
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedOpaque);
}

/* g_SoftwareRasterHandlers32Bit entry 30 (render mode 30): textured, flat-shaded (colour of v0), depth-tested,
   alpha-blended triangle with the alpha-tested depth write of modes 20/22. Byte-identical to mode 28 in the
   original.
*/
void SoftwareRaster32_Mode30
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaTested);
}

/* g_SoftwareRasterHandlers32Bit entries 25 and 56, 57, 60, 62 (render mode 25): textured, flat-shaded (colour
   of v0), depth-tested, alpha-blended triangle. Same pixel operation as mode 17.
*/
void SoftwareRaster32_Mode25
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaBlend);
}

/* g_SoftwareRasterHandlers32Bit entries 26 and 58 (render mode 26): textured, flat-shaded (colour of v0),
   depth-tested, additive triangle. Same pixel operation as mode 18.
*/
void SoftwareRaster32_Mode26
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAdd);
}

/* g_SoftwareRasterHandlers32Bit entry 28 (render mode 28): byte-identical to mode 30 (see
   SoftwareRaster32_Mode30).
*/
void SoftwareRaster32_Mode28
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawTextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanTexturedAlphaTested);
}

/* 32-bit span of the untextured opaque modes (32-bit 0/8, Aux 0/8): pixel and depth are written where the
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

/* g_SoftwareRasterHandlers32Bit entry 0 (render mode 0): Gouraud-shaded, depth-tested, opaque triangle on the
   32-bit framebuffer.
*/
void SoftwareRaster32_Mode00
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

/* Span of the 32-bit modes 4/6/12/14: alpha blend like mode 1 that also writes the depth of pixels whose
   source alpha is >= 128 (Raster_AlphaWritesDepth). The original tests a stale value instead, which it never
   sets in these modes (see docs/software_raster.md); the C rule is deliberate. */
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

/* Modes 4 and 6 (identical code in the original): Gouraud-shaded with
   Raster32_SpanShadedAlphaBlendDepth. */
static void Raster32_DrawShadedAlphaBlendDepth(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
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

/* g_SoftwareRasterHandlers32Bit entry 6 (render mode 6): Gouraud-shaded, depth-tested, alpha-blended triangle
   (like mode 1) that also writes depth where the source alpha is >= 128; see
   Raster32_DrawShadedAlphaBlendDepth.
*/
void SoftwareRaster32_Mode06
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawShadedAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* Span of the 32-bit modes 1/9: blend with the framebuffer by the interpolated alpha; the depth buffer is
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

/* g_SoftwareRasterHandlers32Bit entries 1 and 32, 33, 36, 38 (render mode 1): Gouraud-shaded, depth-tested,
   alpha-blended triangle; the depth buffer is not written.
*/
void SoftwareRaster32_Mode01
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

/* Span of the 32-bit modes 2/10: the interpolated colour is added with saturation; the depth buffer is not
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

/* g_SoftwareRasterHandlers32Bit entries 2 and 34 (render mode 2): Gouraud-shaded, depth-tested, additive
   triangle; the depth buffer is not written.
*/
void SoftwareRaster32_Mode02
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

/* g_SoftwareRasterHandlers32Bit entry 4 (render mode 4): byte-identical to mode 6, see
   Raster32_DrawShadedAlphaBlendDepth.
*/
void SoftwareRaster32_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawShadedAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* g_SoftwareRasterHandlers32Bit entry 8 (render mode 8): flat-shaded (v0's colour), depth-tested, opaque
   triangle (flat version of SoftwareRaster32_Mode00).
*/
void SoftwareRaster32_Mode08
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

/* Modes 12 and 14 (identical code in the original): the flat-shaded (v0's
   colour) version of Raster32_DrawShadedAlphaBlendDepth, with the same replacement of the stale-value test. */
static void Raster32_DrawFlatAlphaBlendDepth(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
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

/* g_SoftwareRasterHandlers32Bit entry 14 (render mode 14): flat-shaded version of modes 4/6: alpha-blended,
   depth written where the source alpha is >= 128; see Raster32_DrawFlatAlphaBlendDepth.
*/
void SoftwareRaster32_Mode14
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawFlatAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

/* g_SoftwareRasterHandlers32Bit entries 9 and 40, 41, 44, 46 (render mode 9): flat-shaded (v0's colour),
   depth-tested, alpha-blended triangle; the depth buffer is not written (flat version of
   SoftwareRaster32_Mode01).
*/
void SoftwareRaster32_Mode09
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

/* g_SoftwareRasterHandlers32Bit entries 10 and 42 (render mode 10): flat-shaded (v0's colour), depth-tested,
   additive triangle; the depth buffer is not written (flat version of SoftwareRaster32_Mode02).
*/
void SoftwareRaster32_Mode10
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

/* g_SoftwareRasterHandlers32Bit entry 12 (render mode 12): byte-identical to mode 14, see
   Raster32_DrawFlatAlphaBlendDepth.
*/
void SoftwareRaster32_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  Raster32_DrawFlatAlphaBlendDepth(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
}

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
   (prestep * uStepX) >> 12, is >= 0x800 as an unsigned value. The original tests a value here meant
   to be the modulated alpha, but it still holds that prestep from the span setup; the decision
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

/* g_SoftwareRasterHandlersAuxiliary entry 16 (render mode 16): textured, Gouraud-shaded, depth-tested, opaque
   triangle into the off-screen target of SoftwareRenderer_DrawQueueAuxiliary.
*/
void SoftwareRasterAux_Mode16
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawTexturedTriangle(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                                 RasterAux_SpanTexturedOpaque);
}

/* g_SoftwareRasterHandlersAuxiliary entry 22 (render mode 22): byte-identical to mode 20 (see
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

/* g_SoftwareRasterHandlersAuxiliary entries 17 and 48, 49, 52, 54 (render mode 17): textured, Gouraud-shaded,
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

/* g_SoftwareRasterHandlersAuxiliary entries 18 and 50 (render mode 18): byte-identical to mode 17 (no additive
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

/* g_SoftwareRasterHandlersAuxiliary entry 20 (render mode 20): textured, Gouraud-shaded, depth-tested triangle.
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

/* g_SoftwareRasterHandlersAuxiliary entry 24 (render mode 24): flat-shaded version of mode 16 (textured,
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

/* g_SoftwareRasterHandlersAuxiliary entry 30 (render mode 30): byte-identical to mode 28 (see
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

/* g_SoftwareRasterHandlersAuxiliary entries 25 and 56, 57, 60, 62 (render mode 25): flat-shaded version of mode 17
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

/* g_SoftwareRasterHandlersAuxiliary entries 26 and 58 (render mode 26): byte-identical to mode 25 (see
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

/* g_SoftwareRasterHandlersAuxiliary entry 28 (render mode 28): flat-shaded version of mode 20 (textured,
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

/* g_SoftwareRasterHandlersAuxiliary entry 0 (render mode 0): Gouraud-shaded, depth-tested, opaque triangle into
   the off-screen target of SoftwareRenderer_DrawQueueAuxiliary. Target and depth
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
   alpha rule allows it (Raster_AlphaWritesDepth, the C replacement of the original's stale-value test). */
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

/* g_SoftwareRasterHandlersAuxiliary entry 6 (render mode 6): Gouraud-shaded, depth-tested, opaque colour write;
   the depth is written only by pixels that pass Raster_AlphaWritesDepth (the original tests a stale value instead,
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

/* g_SoftwareRasterHandlersAuxiliary entries 1 and 32, 33, 36, 38 (render mode 1): Gouraud-shaded, depth-tested
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

/* g_SoftwareRasterHandlersAuxiliary entries 2 and 34 (render mode 2): same code as SoftwareRasterAux_Mode01 (no
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

/* g_SoftwareRasterHandlersAuxiliary entry 4 (render mode 4): same code as SoftwareRasterAux_Mode06.
*/
void SoftwareRasterAux_Mode04
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_GOURAUD,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* g_SoftwareRasterHandlersAuxiliary entry 8 (render mode 8): flat-shaded version of SoftwareRasterAux_Mode00
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

/* g_SoftwareRasterHandlersAuxiliary entry 14 (render mode 14): flat-shaded version of SoftwareRasterAux_Mode06
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

/* g_SoftwareRasterHandlersAuxiliary entries 9 and 40, 41, 44, 46 (render mode 9): flat-shaded version of
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

/* g_SoftwareRasterHandlersAuxiliary entries 10 and 42 (render mode 10): same code as SoftwareRasterAux_Mode09.
*/
void SoftwareRasterAux_Mode10
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedWrite);
}

/* g_SoftwareRasterHandlersAuxiliary entry 12 (render mode 12): same code as SoftwareRasterAux_Mode14.
*/
void SoftwareRasterAux_Mode12
               (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
               GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
               GraphicsPrimitivePacket *packet)
{
  RasterAux_DrawUntextured(clipMaxY, clipMaxX, clipMinY, clipMinX, packet, RASTER_SHADE_FLAT,
                           Raster32_SpanShadedOpaqueAlphaDepth);
}

/* Starts a new depth epoch instead of clearing the depth buffer: the epoch in the top byte of every depth
   value drops by one (0x01000000), so every new depth is nearer than any value left from earlier epochs. Only
   when the epoch underflows or reaches zero is the width*height depth buffer really cleared (0xFFFFFFFF) and the
   epoch reset to 0xFF000000.
*/
void SoftwareRenderer_AdvanceDepthEpoch(void)

{
  int pixelsRemaining;
  int32_t *depthValueCursor;
  Bool8 depthEpochWrapped;

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

/* Readies one packet for the software raster handlers: sorts the three 0x20-byte vertices by screen Y, snaps
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

SoftwareRasterHandler *g_SoftwareRasterHandlers32Bit[64] = {
    /*  0 */ THANDOR_FN(SoftwareRaster32_Mode00),
    /*  1 */ THANDOR_FN(SoftwareRaster32_Mode01),
    /*  2 */ THANDOR_FN(SoftwareRaster32_Mode02),
    /*  3 */ 0,
    /*  4 */ THANDOR_FN(SoftwareRaster32_Mode04),
    /*  5 */ 0,
    /*  6 */ THANDOR_FN(SoftwareRaster32_Mode06),
    /*  7 */ 0,
    /*  8 */ THANDOR_FN(SoftwareRaster32_Mode08),
    /*  9 */ THANDOR_FN(SoftwareRaster32_Mode09),
    /* 10 */ THANDOR_FN(SoftwareRaster32_Mode10),
    /* 11 */ 0,
    /* 12 */ THANDOR_FN(SoftwareRaster32_Mode12),
    /* 13 */ 0,
    /* 14 */ THANDOR_FN(SoftwareRaster32_Mode14),
    /* 15 */ 0,
    /* 16 */ THANDOR_FN(SoftwareRaster32_Mode16),
    /* 17 */ THANDOR_FN(SoftwareRaster32_Mode17),
    /* 18 */ THANDOR_FN(SoftwareRaster32_Mode18),
    /* 19 */ 0,
    /* 20 */ THANDOR_FN(SoftwareRaster32_Mode20),
    /* 21 */ 0,
    /* 22 */ THANDOR_FN(SoftwareRaster32_Mode22),
    /* 23 */ 0,
    /* 24 */ THANDOR_FN(SoftwareRaster32_Mode24),
    /* 25 */ THANDOR_FN(SoftwareRaster32_Mode25),
    /* 26 */ THANDOR_FN(SoftwareRaster32_Mode26),
    /* 27 */ 0,
    /* 28 */ THANDOR_FN(SoftwareRaster32_Mode28),
    /* 29 */ 0,
    /* 30 */ THANDOR_FN(SoftwareRaster32_Mode30),
    /* 31 */ 0,
    /* 32 */ THANDOR_FN(SoftwareRaster32_Mode01),
    /* 33 */ THANDOR_FN(SoftwareRaster32_Mode01),
    /* 34 */ THANDOR_FN(SoftwareRaster32_Mode02),
    /* 35 */ 0,
    /* 36 */ THANDOR_FN(SoftwareRaster32_Mode01),
    /* 37 */ 0,
    /* 38 */ THANDOR_FN(SoftwareRaster32_Mode01),
    /* 39 */ 0,
    /* 40 */ THANDOR_FN(SoftwareRaster32_Mode09),
    /* 41 */ THANDOR_FN(SoftwareRaster32_Mode09),
    /* 42 */ THANDOR_FN(SoftwareRaster32_Mode10),
    /* 43 */ 0,
    /* 44 */ THANDOR_FN(SoftwareRaster32_Mode09),
    /* 45 */ 0,
    /* 46 */ THANDOR_FN(SoftwareRaster32_Mode09),
    /* 47 */ 0,
    /* 48 */ THANDOR_FN(SoftwareRaster32_Mode17),
    /* 49 */ THANDOR_FN(SoftwareRaster32_Mode17),
    /* 50 */ THANDOR_FN(SoftwareRaster32_Mode18),
    /* 51 */ 0,
    /* 52 */ THANDOR_FN(SoftwareRaster32_Mode17),
    /* 53 */ 0,
    /* 54 */ THANDOR_FN(SoftwareRaster32_Mode17),
    /* 55 */ 0,
    /* 56 */ THANDOR_FN(SoftwareRaster32_Mode25),
    /* 57 */ THANDOR_FN(SoftwareRaster32_Mode25),
    /* 58 */ THANDOR_FN(SoftwareRaster32_Mode26),
    /* 59 */ 0,
    /* 60 */ THANDOR_FN(SoftwareRaster32_Mode25),
    /* 61 */ 0,
    /* 62 */ THANDOR_FN(SoftwareRaster32_Mode25)};

SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64] = {
    /*  0 */ THANDOR_FN(SoftwareRasterAux_Mode00),
    /*  1 */ THANDOR_FN(SoftwareRasterAux_Mode01),
    /*  2 */ THANDOR_FN(SoftwareRasterAux_Mode02),
    /*  3 */ 0,
    /*  4 */ THANDOR_FN(SoftwareRasterAux_Mode04),
    /*  5 */ 0,
    /*  6 */ THANDOR_FN(SoftwareRasterAux_Mode06),
    /*  7 */ 0,
    /*  8 */ THANDOR_FN(SoftwareRasterAux_Mode08),
    /*  9 */ THANDOR_FN(SoftwareRasterAux_Mode09),
    /* 10 */ THANDOR_FN(SoftwareRasterAux_Mode10),
    /* 11 */ 0,
    /* 12 */ THANDOR_FN(SoftwareRasterAux_Mode12),
    /* 13 */ 0,
    /* 14 */ THANDOR_FN(SoftwareRasterAux_Mode14),
    /* 15 */ 0,
    /* 16 */ THANDOR_FN(SoftwareRasterAux_Mode16),
    /* 17 */ THANDOR_FN(SoftwareRasterAux_Mode17),
    /* 18 */ THANDOR_FN(SoftwareRasterAux_Mode18),
    /* 19 */ 0,
    /* 20 */ THANDOR_FN(SoftwareRasterAux_Mode20),
    /* 21 */ 0,
    /* 22 */ THANDOR_FN(SoftwareRasterAux_Mode22),
    /* 23 */ 0,
    /* 24 */ THANDOR_FN(SoftwareRasterAux_Mode24),
    /* 25 */ THANDOR_FN(SoftwareRasterAux_Mode25),
    /* 26 */ THANDOR_FN(SoftwareRasterAux_Mode26),
    /* 27 */ 0,
    /* 28 */ THANDOR_FN(SoftwareRasterAux_Mode28),
    /* 29 */ 0,
    /* 30 */ THANDOR_FN(SoftwareRasterAux_Mode30),
    /* 31 */ 0,
    /* 32 */ THANDOR_FN(SoftwareRasterAux_Mode01),
    /* 33 */ THANDOR_FN(SoftwareRasterAux_Mode01),
    /* 34 */ THANDOR_FN(SoftwareRasterAux_Mode02),
    /* 35 */ 0,
    /* 36 */ THANDOR_FN(SoftwareRasterAux_Mode01),
    /* 37 */ 0,
    /* 38 */ THANDOR_FN(SoftwareRasterAux_Mode01),
    /* 39 */ 0,
    /* 40 */ THANDOR_FN(SoftwareRasterAux_Mode09),
    /* 41 */ THANDOR_FN(SoftwareRasterAux_Mode09),
    /* 42 */ THANDOR_FN(SoftwareRasterAux_Mode10),
    /* 43 */ 0,
    /* 44 */ THANDOR_FN(SoftwareRasterAux_Mode09),
    /* 45 */ 0,
    /* 46 */ THANDOR_FN(SoftwareRasterAux_Mode09),
    /* 47 */ 0,
    /* 48 */ THANDOR_FN(SoftwareRasterAux_Mode17),
    /* 49 */ THANDOR_FN(SoftwareRasterAux_Mode17),
    /* 50 */ THANDOR_FN(SoftwareRasterAux_Mode18),
    /* 51 */ 0,
    /* 52 */ THANDOR_FN(SoftwareRasterAux_Mode17),
    /* 53 */ 0,
    /* 54 */ THANDOR_FN(SoftwareRasterAux_Mode17),
    /* 55 */ 0,
    /* 56 */ THANDOR_FN(SoftwareRasterAux_Mode25),
    /* 57 */ THANDOR_FN(SoftwareRasterAux_Mode25),
    /* 58 */ THANDOR_FN(SoftwareRasterAux_Mode26),
    /* 59 */ 0,
    /* 60 */ THANDOR_FN(SoftwareRasterAux_Mode25),
    /* 61 */ 0,
    /* 62 */ THANDOR_FN(SoftwareRasterAux_Mode25)};
