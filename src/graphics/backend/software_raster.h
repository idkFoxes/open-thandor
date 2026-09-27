/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_raster.h
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTER_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTER_H

/*
Shared helpers of the software triangle rasterizer (SoftwareRaster{16,Non16,Aux}_ModeNN in
software.c), and at the end those of the texture-source blits and rectangle fills. Internal to
graphics/backend/software.c; see docs/software_raster.md.

The original handlers are hand-written MMX. These helpers reproduce its arithmetic bit for bit in
plain C (16-bit lanes wrap like PADDW/PSUBW, PSRAW is an arithmetic shift, PACKUSWB saturates to
0..255, PMULHW keeps the high half of a signed 16 x 16 product). OPEN_THANDOR_SELFTEST=rastercmp
compares every handler with the original machine code; keep it identical when changing anything.

Fixed-point formats:
  screen X/Y        Q12 pixels, snapped to whole pixels by SoftwareRenderer_PrepareTrianglePacket
  depth             32-bit unsigned, smaller is nearer; a pixel is drawn when depth <= buffer
  colour lanes      blue, green, red, alpha; channel * 64 (Q6) in 16 bits (RasterColor)
  texture U/V       Q12 texels (the packet coordinates are rescaled to the texture size by
                    SoftwareRenderer_PrepareTrianglePacket); wrapped by the texture size
Attributes a mode does not use (colour of flat modes, U/V of untextured modes) are zero and
stepping them changes nothing.
*/

enum {
    RASTER_LANE_BLUE,
    RASTER_LANE_GREEN,
    RASTER_LANE_RED,
    RASTER_LANE_ALPHA,
    RASTER_LANE_COUNT
};

/* Four 16-bit colour lanes in MMX order (blue, green, red, alpha). */
typedef struct RasterColor {
    short lane[RASTER_LANE_COUNT];
} RasterColor;

/* Where a family draws: 16-bit framebuffer, 32-bit framebuffer or the auxiliary 32-bit target. */
typedef struct RasterTarget {
    byte *pixels;          /* row 0 of the colour target */
    int pixelBytes;        /* 2 or 4 */
    int pixelStride;       /* bytes per colour row */
    byte *depth;           /* row 0 of the depth buffer (one dword per pixel) */
    int depthStride;       /* bytes per depth row */
    int clipMinX;          /* clip rectangle in pixels, max exclusive */
    int clipMinY;
    int clipMaxX;
    int clipMaxY;
} RasterTarget;

/* Per-triangle gradients: change of the attributes per pixel step in X. */
typedef struct RasterGradients {
    int depthStepX;
    RasterColor colorStepX;
    int uStepX;
    int vStepX;
} RasterGradients;

/* How Raster_SetupTriangle derives the colour. */
typedef enum RasterShading {
    RASTER_SHADE_GOURAUD, /* interpolated vertex colours (modes 0..6, 16..22) */
    RASTER_SHADE_FLAT     /* colour of v0 for the whole triangle (modes 8..14, 24..30) */
} RasterShading;

/* A texture as the textured modes (16..30) sample it: nearest texel, wrapped. */
typedef struct RasterTexture {
    const byte *texels;    /* one byte per texel (paletted) or one dword (direct colour) */
    const byte *palette;   /* 256 entries of 8 bytes (only the first dword is used), NULL for direct colour */
    dword uMask;           /* (width - 1) << 12 */
    dword vMask;           /* (height - 1) << 12 */
    int widthLog2;
} RasterTexture;

/* Edge walker. The long edge runs from v0 to v2 (vertices sorted by Y) and carries the attribute
   values; the short edge is v0 -> v1 for the upper part and v1 -> v2 for the lower part. */
typedef struct RasterEdges {
    int longX;             /* Q12 */
    int longXStep;         /* per scanline */
    int shortX;
    int shortXStep;
    dword longDepth;
    int longDepthStep;
    RasterColor longColor;
    RasterColor longColorStep;
    int longU;
    int longUStep;
    int longV;
    int longVStep;
    int scanlineY;
} RasterEdges;

/* One horizontal run of pixels, handed to the mode's span function. The span is walked away from
   the long edge, so it runs left to right or right to left; the deltas already carry the sign. */
typedef struct RasterSpan {
    byte *pixel;           /* first pixel */
    dword *depth;          /* its depth buffer entry */
    int count;             /* pixels to draw, > 0 */
    int pixelStep;         /* +pixelBytes or -pixelBytes */
    int depthPointerStep;  /* +1 or -1 (dwords) */
    dword depthValue;      /* interpolated depth of the current pixel */
    dword depthDelta;      /* per pixel, in span direction */
    RasterColor color;     /* interpolated colour of the current pixel */
    RasterColor colorDelta;
    int u;                 /* interpolated texture coordinates of the current pixel */
    int v;
    int uDelta;
    int vDelta;
    const RasterTexture *texture; /* textured modes, else NULL */
} RasterSpan;

typedef void (*RasterSpanProc)(RasterSpan *span);

/* ---- arithmetic -------------------------------------------------------------------------- */

/* (a * b) >> shift of the 64-bit product, truncated to 32 bits (IMUL + SHLD/SHRD). */
static __inline int Raster_MulShift(int a, int b, int shift)
{
    return (int)(((long long)a * b) >> shift);
}

/* Difference of two 32-bit values with wrap-around (SUB), e.g. of depths. */
static __inline int Raster_Diff(int a, int b)
{
    return (int)((dword)a - (dword)b);
}

/* One byte channel (0 = blue .. 3 = alpha) of a packed ARGB colour. */
static __inline int Raster_Channel(PackedArgb32 argb, int lane)
{
    return (int)((argb >> (8 * lane)) & 0xff);
}

/* PACKUSWB of one lane: clamps a signed 16-bit value to 0..255. */
static __inline int Raster_SaturateByte(int value)
{
    return value < 0 ? 0 : (value > 255 ? 255 : value);
}

/* PMULHW of one lane: high half of the signed 16 x 16 product. */
static __inline short Raster_MulHigh(short a, short b)
{
    return (short)(((int)a * b) >> 16);
}

static __inline RasterColor RasterColor_Add(RasterColor a, RasterColor b)
{
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        a.lane[i] = (short)(a.lane[i] + b.lane[i]);
    }
    return a;
}

static __inline RasterColor RasterColor_Negate(RasterColor a)
{
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        a.lane[i] = (short)-a.lane[i];
    }
    return a;
}

/* PSRAW of every lane. */
static __inline RasterColor RasterColor_ShiftRight(RasterColor a, int shift)
{
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        a.lane[i] = (short)(a.lane[i] >> shift);
    }
    return a;
}

/* ---- pixel formats ----------------------------------------------------------------------- */

/* Unpacks a 16-bit framebuffer pixel into lanes of channel * 16 (Q4), using the runtime 565/555
   constants: mask the channel, scale it to the top of 16 bits (PMULLW), shift down by 4 (PSRLW). */
static __inline RasterColor Raster_Unpack16(word pixel)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const word masks[RASTER_LANE_COUNT] = {k->packedPixelMasks.blue, k->packedPixelMasks.green,
                                           k->packedPixelMasks.red, (word)k->packedPixelMasks.zero};
    const word scales[RASTER_LANE_COUNT] = {k->unpackScales.blue, k->unpackScales.green, k->unpackScales.red,
                                            (word)k->unpackScales.zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        word scaled = (word)((pixel & masks[i]) * scales[i]);
        result.lane[i] = (short)(scaled >> 4);
    }
    return result;
}

/* Unpacks a 32-bit pixel (blue in the low byte) into lanes of (c * 0x101) >> 4, about channel * 16
   (Q4): MOVD + PUNPCKLBW with itself + PSRLW 4. Used by the Non16 and Aux families. */
static __inline RasterColor Raster_Unpack32(dword pixel)
{
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)((Raster_Channel(pixel, i) * 0x101) >> 4);
    }
    return result;
}

/* Packs four channel bytes into a 16-bit framebuffer pixel with the runtime 565/555 constants:
   widen to 12 bits (PUNPCKLBW + PSRLW 4), keep the channel's top bits (PAND), move them into place
   with PMADDWD and add the two dword halves. */
static __inline word Raster_Pack16(const int channel[RASTER_LANE_COUNT])
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const word masks[RASTER_LANE_COUNT] = {k->quantizeMasksQ12.blue, k->quantizeMasksQ12.green,
                                           k->quantizeMasksQ12.red, (word)k->quantizeMasksQ12.zero};
    const word weights[RASTER_LANE_COUNT] = {k->packWeights.blue, k->packWeights.green, k->packWeights.red,
                                             (word)k->packWeights.zero};
    short q[RASTER_LANE_COUNT];
    dword low;
    dword high;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        q[i] = (short)(((channel[i] * 0x101) >> 4) & masks[i]);
    }
    low = (dword)(q[0] * (short)weights[0] + q[1] * (short)weights[1]);
    high = (dword)(q[2] * (short)weights[2] + q[3] * (short)weights[3]);
    return (word)((low >> 8) + (high >> 8));
}

/* Packs four channel bytes into a 32-bit pixel (blue in the low byte, PACKUSWB + MOVD). */
static __inline dword Raster_Pack32(const int channel[RASTER_LANE_COUNT])
{
    return (dword)channel[0] | ((dword)channel[1] << 8) | ((dword)channel[2] << 16) | ((dword)channel[3] << 24);
}

/* Lanes >> shift, saturated to bytes (PSRAW + PACKUSWB). */
static __inline void Raster_LanesToBytes(RasterColor color, int shift, int channel[RASTER_LANE_COUNT])
{
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        channel[i] = Raster_SaturateByte(color.lane[i] >> shift);
    }
}

/* Q6 shaded colour -> 16-bit pixel. */
static __inline word Raster_ShadeToPixel16(RasterColor color)
{
    int channel[RASTER_LANE_COUNT];
    Raster_LanesToBytes(color, 6, channel);
    return Raster_Pack16(channel);
}

/* ---- blending ---------------------------------------------------------------------------- */

/* Alpha blend of two Q4 colours: source * alpha + destination * (1 - alpha), both through the
   g_SoftwareBlendAlphaFactors / g_SoftwareBlendInverseAlphaFactors tables (PMULHW). The table index
   is the top 12 bits of the source alpha lane, so a negative or overflowed alpha reads past the
   256 entries into the image data behind them, as the original does. Result in Q4. */
static __inline RasterColor Raster_BlendAlpha(RasterColor sourceQ4, RasterColor destinationQ4)
{
    unsigned index = (word)sourceQ4.lane[RASTER_LANE_ALPHA] >> 4;
    const SoftwareRgbWordLanes *alpha = &g_SoftwareBlendAlphaFactors[0] + index;
    const SoftwareRgbWordLanes *inverse = &g_SoftwareBlendInverseAlphaFactors[0] + index;
    const short alphaLanes[RASTER_LANE_COUNT] = {(short)alpha->blue, (short)alpha->green, (short)alpha->red,
                                                 (short)alpha->zero};
    const short inverseLanes[RASTER_LANE_COUNT] = {(short)inverse->blue, (short)inverse->green, (short)inverse->red,
                                                   (short)inverse->zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        short source = (short)(sourceQ4.lane[i] << 2);
        short destination = (short)(destinationQ4.lane[i] << 2);
        result.lane[i] = (short)(Raster_MulHigh(source, alphaLanes[i]) + Raster_MulHigh(destination, inverseLanes[i]));
    }
    return result;
}

/* ---- triangle walking -------------------------------------------------------------------- */

/* d(attribute)/dx of the plane through the three vertices, scaled by invArea and shifted:
   ((a2 - a0) * (y1 - y0) - (a1 - a0) * (y2 - y0)) >> 12, times invArea, >> shift. */
static __inline int Raster_GradientX(const GraphicsPrimitivePacket *packet, int a0, int a1, int a2, int invArea,
                                     int shift)
{
    int dy10 = packet->vertices[1].screenY - packet->vertices[0].screenY;
    int dy20 = packet->vertices[2].screenY - packet->vertices[0].screenY;
    long long plane = (long long)Raster_Diff(a2, a0) * dy10 - (long long)Raster_Diff(a1, a0) * dy20;
    return Raster_MulShift((int)(plane >> 12), invArea, shift);
}

/* Sets up the long edge (position, depth, colour, texture coordinates) and the X gradients of a
   triangle. Flat shading takes v0's colour, widened like PUNPCKLBW + PSRLW 2 ((c * 0x101) >> 2),
   and never steps it; Gouraud shading starts at c << 6 and interpolates. Returns 0 when the
   triangle has no height or no area (nothing is drawn). */
static __inline int Raster_SetupTriangle(const GraphicsPrimitivePacket *packet, RasterShading shading, int textured,
                                         RasterEdges *edges, RasterGradients *gradients)
{
    const GraphicsPrimitiveVertexRaw *v0 = &packet->vertices[0];
    const GraphicsPrimitiveVertexRaw *v1 = &packet->vertices[1];
    const GraphicsPrimitiveVertexRaw *v2 = &packet->vertices[2];
    int height = v2->screenY - v0->screenY;
    int invHeight;
    long long cross;
    int doubleArea;
    int invArea;
    int i;

    if (height <= 0) {
        return 0;
    }
    memset(edges, 0, sizeof *edges);
    memset(gradients, 0, sizeof *gradients);
    invHeight = 0x1000000 / height; /* 1 / height in pixels, Q12 */
    edges->longX = v0->screenX;
    edges->shortX = v0->screenX;
    edges->longXStep = Raster_MulShift(v2->screenX - v0->screenX, invHeight, 12);
    edges->longDepth = (dword)v0->depth;
    edges->longDepthStep = Raster_MulShift(Raster_Diff(v2->depth, v0->depth), invHeight, 12);
    if (textured) {
        edges->longU = v0->textureU;
        edges->longUStep = Raster_MulShift(Raster_Diff(v2->textureU, v0->textureU), invHeight, 12);
        edges->longV = v0->textureV;
        edges->longVStep = Raster_MulShift(Raster_Diff(v2->textureV, v0->textureV), invHeight, 12);
    }
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        int c0 = Raster_Channel(v0->diffuseColor, i);
        int c2 = Raster_Channel(v2->diffuseColor, i);
        if (shading == RASTER_SHADE_FLAT) {
            edges->longColor.lane[i] = (short)((c0 * 0x101) >> 2);
        }
        else {
            edges->longColor.lane[i] = (short)(c0 << 6);
            edges->longColorStep.lane[i] = (short)Raster_MulShift(c2 - c0, invHeight, 6);
        }
    }

    cross = (long long)(v2->screenX - v0->screenX) * (v1->screenY - v0->screenY) -
            (long long)(v1->screenX - v0->screenX) * (v2->screenY - v0->screenY);
    doubleArea = (int)(cross >> 12);
    if (doubleArea == 0) {
        return 0;
    }
    invArea = (int)(0x1000000000LL / doubleArea);
    gradients->depthStepX = Raster_GradientX(packet, v0->depth, v1->depth, v2->depth, invArea, 24);
    if (textured) {
        gradients->uStepX = Raster_GradientX(packet, v0->textureU, v1->textureU, v2->textureU, invArea, 24);
        gradients->vStepX = Raster_GradientX(packet, v0->textureV, v1->textureV, v2->textureV, invArea, 24);
    }
    if (shading == RASTER_SHADE_GOURAUD) {
        for (i = 0; i < RASTER_LANE_COUNT; i++) {
            gradients->colorStepX.lane[i] = (short)Raster_GradientX(
                packet, Raster_Channel(v0->diffuseColor, i) << 12, Raster_Channel(v1->diffuseColor, i) << 12,
                Raster_Channel(v2->diffuseColor, i) << 12, invArea, 30);
        }
    }
    edges->scanlineY = v0->screenY >> 12;
    return 1;
}

/* Clips the current scanline between the two edges and hands it to `drawSpan`. The span starts
   at the long edge; depth and colour are prestepped from the long edge X to the first pixel. */
static __forceinline void Raster_DrawScanline(const RasterTarget *target, const RasterEdges *edges,
                                              const RasterGradients *gradients, const RasterTexture *texture,
                                              RasterSpanProc drawSpan)
{
    int longX = edges->longX >> 12;
    int shortX = edges->shortX >> 12;
    int first;
    int end;
    int prestep;
    short prestepLane;
    RasterSpan span;
    int i;

    if (shortX == longX) {
        return;
    }
    if (shortX > longX) {
        /* long edge on the left: draw from `first` rightwards */
        first = longX < target->clipMinX ? target->clipMinX : longX;
        end = shortX > target->clipMaxX ? target->clipMaxX : shortX;
        if (end <= first) {
            return;
        }
        span.count = end - first;
        span.pixelStep = target->pixelBytes;
        span.depthPointerStep = 1;
        span.depthDelta = (dword)gradients->depthStepX;
        span.colorDelta = gradients->colorStepX;
        span.uDelta = gradients->uStepX;
        span.vDelta = gradients->vStepX;
        prestep = ((first + 1) << 12) - edges->longX;
    }
    else {
        /* long edge on the right: draw from pixel `first - 1` leftwards */
        first = longX > target->clipMaxX ? target->clipMaxX : longX;
        end = shortX < target->clipMinX ? target->clipMinX : shortX;
        if (end >= first) {
            return;
        }
        span.count = first - end;
        span.pixelStep = -target->pixelBytes;
        span.depthPointerStep = -1;
        span.depthDelta = (dword)-gradients->depthStepX;
        span.colorDelta = RasterColor_Negate(gradients->colorStepX);
        span.uDelta = -gradients->uStepX;
        span.vDelta = -gradients->vStepX;
        prestep = (first << 12) - edges->longX;
        first--;
    }
    span.pixel = target->pixels + edges->scanlineY * target->pixelStride + first * target->pixelBytes;
    span.depth = (dword *)(target->depth + edges->scanlineY * target->depthStride) + first;
    span.depthValue = edges->longDepth + (dword)Raster_MulShift(prestep, gradients->depthStepX, 12);
    span.u = edges->longU + Raster_MulShift(prestep, gradients->uStepX, 12);
    span.v = edges->longV + Raster_MulShift(prestep, gradients->vStepX, 12);
    span.texture = texture;
    prestepLane = (short)(prestep >> 8);
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        span.color.lane[i] = (short)(edges->longColor.lane[i] +
                                     (short)((gradients->colorStepX.lane[i] * prestepLane) >> 4));
    }
    drawSpan(&span);
}

/* Walks `rows` scanlines down both edges. */
static __forceinline void Raster_WalkRows(const RasterTarget *target, RasterEdges *edges,
                                          const RasterGradients *gradients, const RasterTexture *texture, int rows,
                                          RasterSpanProc drawSpan)
{
    for (; rows > 0; rows--) {
        if (edges->scanlineY >= target->clipMinY && edges->scanlineY < target->clipMaxY) {
            Raster_DrawScanline(target, edges, gradients, texture, drawSpan);
        }
        edges->longDepth += (dword)edges->longDepthStep;
        edges->longColor = RasterColor_Add(edges->longColor, edges->longColorStep);
        edges->longU += edges->longUStep;
        edges->longV += edges->longVStep;
        edges->longX += edges->longXStep;
        edges->shortX += edges->shortXStep;
        edges->scanlineY++;
    }
}

/* Upper part (v0 -> v1 as short edge), then lower part (v1 -> v2). `texture` is handed to the
   span function (NULL for untextured modes). */
static __forceinline void Raster_WalkTriangle(const RasterTarget *target, const GraphicsPrimitivePacket *packet,
                                              RasterEdges *edges, const RasterGradients *gradients,
                                              const RasterTexture *texture, RasterSpanProc drawSpan)
{
    const GraphicsPrimitiveVertexRaw *v0 = &packet->vertices[0];
    const GraphicsPrimitiveVertexRaw *v1 = &packet->vertices[1];
    const GraphicsPrimitiveVertexRaw *v2 = &packet->vertices[2];
    int upper = v1->screenY - v0->screenY;
    int lower = v2->screenY - v1->screenY;
    if (upper > 0) {
        edges->shortXStep = Raster_MulShift(0x1000000 / upper, v1->screenX - v0->screenX, 12);
        Raster_WalkRows(target, edges, gradients, texture, upper >> 12, drawSpan);
    }
    edges->shortX = v1->screenX;
    if (lower > 0) {
        edges->shortXStep = Raster_MulShift(0x1000000 / lower, v2->screenX - v1->screenX, 12);
        Raster_WalkRows(target, edges, gradients, texture, lower >> 12, drawSpan);
    }
}

/* Target of the 16-bit and 32-bit framebuffer families (g_FramebufferAccess + depth buffer). */
static __inline RasterTarget Raster_FramebufferTarget(int pixelBytes, int clipMaxY, int clipMaxX, int clipMinY,
                                                      int clipMinX)
{
    RasterTarget target;
    target.pixels = g_FramebufferAccess->pixels;
    target.pixelBytes = pixelBytes;
    target.pixelStride = (int)g_FramebufferRowStrideBytes;
    target.depth = (byte *)g_SoftwareDepthBuffer;
    target.depthStride = (int)g_SoftwareDepthRowStrideBytes;
    target.clipMinX = clipMinX;
    target.clipMinY = clipMinY;
    target.clipMaxX = clipMaxX;
    target.clipMaxY = clipMaxY;
    return target;
}

/* Target of the auxiliary family: a 32-bit image of clipMaxX pixels per row; the depth buffer uses
   the same row length (not g_SoftwareDepthRowStrideBytes). */
static __inline RasterTarget Raster_AuxiliaryTarget(int clipMaxY, int clipMaxX, int clipMinY, int clipMinX)
{
    RasterTarget target;
    target.pixels = (byte *)g_SoftwareAuxiliaryTargetBase;
    target.pixelBytes = 4;
    target.pixelStride = clipMaxX * 4;
    target.depth = (byte *)g_SoftwareDepthBuffer;
    target.depthStride = clipMaxX * 4;
    target.clipMinX = clipMinX;
    target.clipMinY = clipMinY;
    target.clipMaxX = clipMaxX;
    target.clipMaxY = clipMaxY;
    return target;
}

/* Advances a span to its next pixel. */
static __inline void RasterSpan_Next(RasterSpan *span)
{
    span->pixel += span->pixelStep;
    span->depth += span->depthPointerStep;
    span->depthValue += span->depthDelta;
    span->color = RasterColor_Add(span->color, span->colorDelta);
    span->u += span->uDelta;
    span->v += span->vDelta;
}

/* ---- textures ---------------------------------------------------------------------------- */

/* Texture of a textured packet: the source entry's texels inside the source asset, and for
   paletted textures (paletteIndex >= 0) palette bank paletteIndex (0x800 bytes each, after a
   0x200-byte header). */
static __inline void Raster_SetupTexture(const GraphicsPrimitivePacket *packet, RasterTexture *texture)
{
    const GraphicsTextureSetEntry *entry = packet->textureEntry;
    const byte *asset = (const byte *)entry->sourceAsset;
    const GraphicsTextureSourceEntry *source = entry->sourceEntry;
    int paletteIndex = (int)source->paletteIndex;
    texture->widthLog2 = (int)entry->widthLog2;
    texture->uMask = ((1u << (entry->widthLog2 & 31)) - 1) << 12;
    texture->vMask = ((1u << (entry->heightLog2 & 31)) - 1) << 12;
    texture->texels = asset + source->dataOffset;
    texture->palette = paletteIndex < 0 ? NULL : asset + 0x200 + (dword)paletteIndex * 0x800;
}

/* The ARGB texel at (u, v), both wrapped to the texture (nearest texel, no filtering). */
static __inline dword Raster_FetchTexel(const RasterTexture *texture, int u, int v)
{
    dword index = (((dword)u & texture->uMask) >> 12) +
                  (dword)(((unsigned long long)((dword)v & texture->vMask) << 32) >> (44 - texture->widthLog2));
    if (texture->palette != NULL) {
        return *(const dword *)(texture->palette + texture->texels[index] * 8u);
    }
    return ((const dword *)texture->texels)[index];
}

/* An ARGB texel as lanes of (c * 0x101) >> 2 (PUNPCKLBW + PSRLW 2), ready for Raster_Modulate. */
static __inline RasterColor Raster_TexelLanes(dword argb)
{
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)((Raster_Channel(argb, i) * 0x101) >> 2);
    }
    return result;
}

/* Shaded colour (Q6) times texel lanes (PMULHW); the result is Q4 (channel * 16). */
static __inline RasterColor Raster_Modulate(RasterColor color, RasterColor texel)
{
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        color.lane[i] = Raster_MulHigh(color.lane[i], texel.lane[i]);
    }
    return color;
}

/* ---- Alpha-tested depth write (shared by all families) -------------------------------------------- */

/* Depth rule of the alpha-blended modes that write depth (4/6/12/14 and the textured 20/22/28/30):
   depth is written when the source alpha lane (Q4, as handed to Raster_BlendAlpha) is >= 128, i.e.
   the blend index (word)alpha >> 4 is > 0x7f. The lane is read unsigned, so a negative lane also
   passes. The textured modes test exactly this in the original; the untextured ones test a stale
   MM2 instead (see docs/software_raster.md), and the C keeps this rule for them. */
static __inline int Raster_AlphaWritesDepth(RasterColor sourceQ4)
{
    return (word)sourceQ4.lane[RASTER_LANE_ALPHA] >= 0x800;
}

/* ---- Texture-source blits and rectangle fills --------------------------------------------- */
/*
SoftwareTextureSource_Blit*, SoftwareFramebuffer_FillRectArgb* (see docs/software_raster.md,
"Blits"). OPEN_THANDOR_SELFTEST=blitcmp compares them with the original machine code.

Texture source asset: "gfx" magic, tableDescriptor at +0xB0 (subresourceCount, paletteBankCount,
subresourceTableOffset), palette banks of 256 * 8 bytes at +0x200, and a table of 32-byte
GraphicsTextureSourceEntry records. A palette entry holds two dwords: the ARGB colour at +0 and,
at +4, the colour converted to the 16-bit framebuffer format with the alpha in its top byte. The
16-bit blits test the alpha of +4, write its low word and blend +0; the 32-bit blits use +4 for
everything (they treat it as ARGB). An entry with paletteIndex -1 stores ARGB dwords instead of
8-bit indices.

A source colour whose alpha is 0 is skipped, alpha 0xFF is written as is (converted), anything in
between is blended through g_SoftwareBlendAlphaFactors / g_SoftwareBlendInverseAlphaFactors. The
original tests the whole dword (< 0x1000000, >= 0xFF000000); Blit_IsTransparent / Blit_IsOpaque do
the same.
*/

/* One clipped image in the framebuffer, and where its first texel is. */
typedef struct BlitRegion {
    const byte *texels;    /* texel of the top-left drawn pixel */
    int texelBytes;        /* 1 (palette index) or 4 (ARGB) */
    int texelStride;       /* bytes per source row */
    const byte *palette;   /* palette bank of a paletted image (256 entries of 8 bytes), else NULL */
    byte *pixels;          /* top-left drawn pixel */
    int pixelBytes;        /* 2 or 4 */
    int pixelStride;       /* bytes per framebuffer row (framebuffer->width pixels) */
    int width;             /* drawn size in pixels, both > 0 */
    int height;
} BlitRegion;

static __inline int Blit_IsTransparent(dword argb)
{
    return argb < 0x1000000u;
}

static __inline int Blit_IsOpaque(dword argb)
{
    return argb >= 0xff000000u;
}

/* The two dwords of a palette entry: +0 ARGB colour, +4 converted (16-bit) pixel with the alpha on top. */
static __inline dword Blit_PaletteColor(const BlitRegion *region, byte index)
{
    return *(const dword *)(region->palette + index * 8u);
}

static __inline dword Blit_PalettePixel(const BlitRegion *region, byte index)
{
    return *(const dword *)(region->palette + index * 8u + 4u);
}

/* ARGB -> framebuffer pixel through the g_SoftwarePixelPackTables channel tables. The alpha byte is
   added on top; a 16-bit framebuffer keeps the low word. */
static __inline dword Blit_ConvertArgb(dword argb)
{
    const SoftwarePixelPackTables *tables = g_SoftwarePixelPackTables;
    return tables->blue[argb & 0xff] + (argb & 0xff000000u) + tables->green[(argb >> 8) & 0xff] +
           tables->red[(argb >> 16) & 0xff];
}

/* ARGB (or a 32-bit pixel) as lanes of (c * 0x101) >> shift (PUNPCKLBW with itself + PSRLW). */
static __inline RasterColor Blit_ArgbLanes(dword argb, int shift)
{
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)((Raster_Channel(argb, i) * 0x101) >> shift);
    }
    return result;
}

/* A 16-bit framebuffer pixel as lanes: masked channel scaled to the top of 16 bits (PAND + PMULLW
   with the 565/555 constants), then >> 2 (PSRLW). Same scale as Blit_ArgbLanes(argb, 2). */
static __inline RasterColor Blit_Unpack16(word pixel)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const word masks[RASTER_LANE_COUNT] = {k->packedPixelMasks.blue, k->packedPixelMasks.green,
                                           k->packedPixelMasks.red, (word)k->packedPixelMasks.zero};
    const word scales[RASTER_LANE_COUNT] = {k->unpackScales.blue, k->unpackScales.green, k->unpackScales.red,
                                            (word)k->unpackScales.zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        word scaled = (word)((pixel & masks[i]) * scales[i]);
        result.lane[i] = (short)(scaled >> 2);
    }
    return result;
}

/* source * alpha + destination * (1 - alpha) through the blend factor tables (two PMULHW + PADDW).
   alpha is the source alpha byte (1..254 for the blits). */
static __inline RasterColor Blit_BlendLanes(RasterColor source, RasterColor destination, unsigned alpha)
{
    const SoftwareRgbWordLanes *factor = &g_SoftwareBlendAlphaFactors[alpha];
    const SoftwareRgbWordLanes *inverse = &g_SoftwareBlendInverseAlphaFactors[alpha];
    const short factorLanes[RASTER_LANE_COUNT] = {(short)factor->blue, (short)factor->green, (short)factor->red,
                                                  (short)factor->zero};
    const short inverseLanes[RASTER_LANE_COUNT] = {(short)inverse->blue, (short)inverse->green,
                                                   (short)inverse->red, (short)inverse->zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)(Raster_MulHigh(source.lane[i], factorLanes[i]) +
                                 Raster_MulHigh(destination.lane[i], inverseLanes[i]));
    }
    return result;
}

/* Blended lanes (channel << 4, 12 bits) -> 16-bit pixel: PAND with the quantize masks, PMADDWD with
   the pack weights, and the word sum of bits 8..23 of both dword halves (PSRLQ 8 / 40 + PADDW). */
static __inline word Blit_PackLanes16(RasterColor lanes)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const word masks[RASTER_LANE_COUNT] = {k->quantizeMasksQ12.blue, k->quantizeMasksQ12.green,
                                           k->quantizeMasksQ12.red, (word)k->quantizeMasksQ12.zero};
    const word weights[RASTER_LANE_COUNT] = {k->packWeights.blue, k->packWeights.green, k->packWeights.red,
                                             (word)k->packWeights.zero};
    short q[RASTER_LANE_COUNT];
    dword low;
    dword high;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        q[i] = (short)(lanes.lane[i] & masks[i]);
    }
    low = (dword)(q[0] * (short)weights[0] + q[1] * (short)weights[1]);
    high = (dword)(q[2] * (short)weights[2] + q[3] * (short)weights[3]);
    return (word)((low >> 8) + (high >> 8));
}

/* Blended lanes -> 32-bit pixel: PSRLW 4 (logical) + PACKUSWB, alpha lane included. */
static __inline dword Blit_PackLanes32(RasterColor lanes)
{
    int channel[RASTER_LANE_COUNT];
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        channel[i] = Raster_SaturateByte((word)lanes.lane[i] >> 4);
    }
    return Raster_Pack32(channel);
}

/* Source-alpha blend of an ARGB colour over a 16-bit / 32-bit pixel, alpha = the colour's top byte. */
static __inline word Blit_BlendArgb16(dword argb, word destination)
{
    return Blit_PackLanes16(Blit_BlendLanes(Blit_ArgbLanes(argb, 2), Blit_Unpack16(destination), argb >> 24));
}

static __inline dword Blit_BlendArgb32(dword argb, dword destination)
{
    return Blit_PackLanes32(Blit_BlendLanes(Blit_ArgbLanes(argb, 2), Blit_ArgbLanes(destination, 2), argb >> 24));
}

/* Intersects [left, right) x [top, bottom) with the framebuffer and the clip rectangle, in the
   original's order: rectangle clamped to 0 and to the framebuffer size, then to the clip
   rectangle (all compares signed). Returns 0 when nothing is left. */
static __inline int Blit_ClipRect(const SoftwareFramebufferAccess *framebuffer, int clipMaxY, int clipMaxX,
                                  int clipMinY, int clipMinX, int *left, int *top, int *right, int *bottom)
{
    if (*left < 0) {
        *left = 0;
    }
    if (*top < 0) {
        *top = 0;
    }
    if (*right > (int)framebuffer->width) {
        *right = (int)framebuffer->width;
    }
    if (*bottom > (int)framebuffer->height) {
        *bottom = (int)framebuffer->height;
    }
    if (*left < clipMinX) {
        *left = clipMinX;
    }
    if (*top < clipMinY) {
        *top = clipMinY;
    }
    if (*right > clipMaxX) {
        *right = clipMaxX;
    }
    if (*bottom > clipMaxY) {
        *bottom = clipMaxY;
    }
    return *right > *left && *bottom > *top; /* SUB + JLE */
}

/* Common setup of the clipped blits: validates the asset (magic, subresource index), the framebuffer
   pixel size and the entry's palette bank, places the subresource at (drawX, drawY) + its origin and
   clips it. Returns 0 when nothing is drawn. */
static __inline int Blit_SetupSubresource(const GraphicsTextureSourceAsset *sourceAsset,
                                          GraphicsSubresourceIndex subresourceIndex,
                                          SoftwareFramebufferAccess *framebuffer, int pixelBytes, int drawX, int drawY,
                                          int clipMaxY, int clipMaxX, int clipMinY, int clipMinX, BlitRegion *region)
{
    const byte *asset = (const byte *)sourceAsset;
    const GraphicsTextureSourceEntry *entry;
    int left;
    int top;
    int right;
    int bottom;

    if (sourceAsset->common.magic != ASSET_MAGIC_GFX ||
        subresourceIndex >= sourceAsset->tableDescriptor.subresourceCount ||
        (int)framebuffer->bytesPerPixel != pixelBytes) {
        return 0;
    }
    entry = (const GraphicsTextureSourceEntry *)(asset + sourceAsset->tableDescriptor.subresourceTableOffset) +
            subresourceIndex;
    if (entry->paletteIndex == -1) {
        region->texelBytes = 4;
        region->palette = NULL;
    }
    else if ((dword)entry->paletteIndex < sourceAsset->tableDescriptor.paletteBankCount) {
        region->texelBytes = 1;
        region->palette = asset + 0x200 + (dword)entry->paletteIndex * 0x800;
    }
    else {
        return 0;
    }
    left = drawX + entry->originX;
    top = drawY + entry->originY;
    right = left + (int)entry->pixelWidth;
    bottom = top + (int)entry->pixelHeight;
    if (!Blit_ClipRect(framebuffer, clipMaxY, clipMaxX, clipMinY, clipMinX, &left, &top, &right, &bottom)) {
        return 0;
    }
    region->width = right - left;
    region->height = bottom - top;
    region->texelStride = (int)entry->pixelWidth * region->texelBytes;
    region->texels = asset + entry->dataOffset +
                     ((top - drawY - entry->originY) * (int)entry->pixelWidth + (left - drawX - entry->originX)) *
                         region->texelBytes;
    region->pixelBytes = pixelBytes;
    region->pixelStride = (int)framebuffer->width * pixelBytes;
    region->pixels = framebuffer->pixels + (top * (int)framebuffer->width + left) * pixelBytes;
    return 1;
}

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTER_H */
