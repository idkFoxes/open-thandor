/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_blit_helpers.h
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H

#include "software_raster.h"

/* ---- Texture-source blits and rectangle fills --------------------------------------------- */
/*
SoftwareTextureSource_Blit*, SoftwareFramebuffer_FillRectArgb* (see docs/software_raster.md,
"Blits"). The former blitcmp self-test confirmed them against the original machine code.

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
    const uint8_t *texels;    /* texel of the top-left drawn pixel */
    int texelBytes;        /* 1 (palette index) or 4 (ARGB) */
    int texelStride;       /* bytes per source row */
    const uint8_t *palette;   /* palette bank of a paletted image (256 entries of 8 bytes), else NULL */
    uint8_t *pixels;          /* top-left drawn pixel */
    int pixelBytes;        /* 2 or 4 */
    int pixelStride;       /* bytes per framebuffer row (framebuffer->width pixels) */
    int width;             /* drawn size in pixels, both > 0 */
    int height;
} BlitRegion;

static __inline int Blit_IsTransparent(uint32_t argb)
{
    return argb < 0x1000000u;
}

static __inline int Blit_IsOpaque(uint32_t argb)
{
    return argb >= 0xff000000u;
}

/* The two dwords of a palette entry: +0 ARGB colour, +4 converted (16-bit) pixel with the alpha on top. */
static __inline uint32_t Blit_PaletteColor(const BlitRegion *region, uint8_t index)
{
    return *(const uint32_t *)(region->palette + index * 8u);
}

static __inline uint32_t Blit_PalettePixel(const BlitRegion *region, uint8_t index)
{
    return *(const uint32_t *)(region->palette + index * 8u + 4u);
}

/* ARGB -> framebuffer pixel through the g_SoftwarePixelPackTables channel tables. The alpha byte is
   added on top; a 16-bit framebuffer keeps the low word. */
static __inline uint32_t Blit_ConvertArgb(uint32_t argb)
{
    const SoftwarePixelPackTables *tables = g_SoftwarePixelPackTables;
    return tables->blue[argb & 0xff] + (argb & 0xff000000u) + tables->green[(argb >> 8) & 0xff] +
           tables->red[(argb >> 16) & 0xff];
}

/* ARGB (or a 32-bit pixel) as lanes of (c * 0x101) >> shift (PUNPCKLBW with itself + PSRLW). */
static __inline RasterColor Blit_ArgbLanes(uint32_t argb, int shift)
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
static __inline RasterColor Blit_Unpack16(uint16_t pixel)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const uint16_t masks[RASTER_LANE_COUNT] = {k->packedPixelMasks.blue, k->packedPixelMasks.green,
                                           k->packedPixelMasks.red, (uint16_t)k->packedPixelMasks.zero};
    const uint16_t scales[RASTER_LANE_COUNT] = {k->unpackScales.blue, k->unpackScales.green, k->unpackScales.red,
                                            (uint16_t)k->unpackScales.zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        uint16_t scaled = (uint16_t)((pixel & masks[i]) * scales[i]);
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
static __inline uint16_t Blit_PackLanes16(RasterColor lanes)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const uint16_t masks[RASTER_LANE_COUNT] = {k->quantizeMasksQ12.blue, k->quantizeMasksQ12.green,
                                           k->quantizeMasksQ12.red, (uint16_t)k->quantizeMasksQ12.zero};
    const uint16_t weights[RASTER_LANE_COUNT] = {k->packWeights.blue, k->packWeights.green, k->packWeights.red,
                                             (uint16_t)k->packWeights.zero};
    short q[RASTER_LANE_COUNT];
    uint32_t low;
    uint32_t high;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        q[i] = (short)(lanes.lane[i] & masks[i]);
    }
    low = (uint32_t)(q[0] * (short)weights[0] + q[1] * (short)weights[1]);
    high = (uint32_t)(q[2] * (short)weights[2] + q[3] * (short)weights[3]);
    return (uint16_t)((low >> 8) + (high >> 8));
}

/* Blended lanes -> 32-bit pixel: PSRLW 4 (logical) + PACKUSWB, alpha lane included. */
static __inline uint32_t Blit_PackLanes32(RasterColor lanes)
{
    int channel[RASTER_LANE_COUNT];
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        channel[i] = Raster_SaturateByte((uint16_t)lanes.lane[i] >> 4);
    }
    return Raster_Pack32(channel);
}

/* Source-alpha blend of an ARGB colour over a 16-bit / 32-bit pixel, alpha = the colour's top byte. */
static __inline uint16_t Blit_BlendArgb16(uint32_t argb, uint16_t destination)
{
    return Blit_PackLanes16(Blit_BlendLanes(Blit_ArgbLanes(argb, 2), Blit_Unpack16(destination), argb >> 24));
}

static __inline uint32_t Blit_BlendArgb32(uint32_t argb, uint32_t destination)
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
    return *right > *left && *bottom > *top; /* signed compares */
}

/* Common setup of the clipped blits: validates the asset (magic, subresource index), the framebuffer
   pixel size and the entry's palette bank, places the subresource at (drawX, drawY) + its origin and
   clips it. Returns 0 when nothing is drawn. */
static __inline int Blit_SetupSubresource(const GraphicsTextureSourceAsset *sourceAsset,
                                          GraphicsSubresourceIndex subresourceIndex,
                                          SoftwareFramebufferAccess *framebuffer, int pixelBytes, int drawX, int drawY,
                                          int clipMaxY, int clipMaxX, int clipMinY, int clipMinX, BlitRegion *region)
{
    const uint8_t *asset = (const uint8_t *)sourceAsset;
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
    else if ((uint32_t)entry->paletteIndex < sourceAsset->tableDescriptor.paletteBankCount) {
        region->texelBytes = 1;
        region->palette = asset + GFX_ASSET_HEADER_SIZE + (int32_t)((uint32_t)entry->paletteIndex * GFX_PALETTE_BANK_SIZE);
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

/* ---- B1: clipped blend variants (HalfSourceRgb, PaletteBank, Modulated) ------------------- */

/* The BlitModulatedSourceAlpha channel product, as the original's four IMULs: every channel of argb is
   multiplied by the matching channel of modulation. Blue keeps (b * mb) >> 8; green, red and alpha keep
   the high byte of their product ((c * mc) & 0xFF00) shifted into place. All four agree with
   (c * mc) >> 8, so even modulation 0xFFFFFFFF darkens by one step (0xFF * 0xFF >> 8 = 0xFE). In
   particular the modulated alpha is at most 0xFE: the opaque shortcut of the Modulated blits is dead
   code, and every visible texel goes through the blend. */
static __inline uint32_t Blit_Modulate(uint32_t argb, uint32_t modulation)
{
    uint32_t blue = ((argb & 0xff) * (modulation & 0xff)) >> 8;
    uint32_t green = (((argb >> 8) & 0xff) * ((modulation >> 8) & 0xff)) & 0xff00u;
    uint32_t red = (((argb >> 16) & 0xff) * ((modulation >> 16) & 0xff)) & 0xff00u;
    uint32_t alpha = ((argb >> 24) * (modulation >> 24)) & 0xff00u;
    return blue | green | (red << 8) | (alpha << 16);
}

/* ---- B2: saturated add (BlitSaturatedAddRgb, BlitHalfRgbSaturatedAdd) ------------------------ */

/* PADDUSW of one lane: unsigned 16-bit add, clamped at 0xFFFF. */
static __inline uint16_t Blit_AddSaturateWord(uint16_t a, uint16_t b)
{
    uint32_t sum = (uint32_t)a + b;
    return (uint16_t)(sum > 0xffffu ? 0xffffu : sum);
}

/* A 16-bit framebuffer pixel as lanes without Blit_Unpack16's final >> 2: the masked channel times
   the unpack scale, low 16 bits (PAND + PMULLW), i.e. the channel at the top of the word. */
static __inline RasterColor Blit_Unpack16Unshifted(uint16_t pixel)
{
    const SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    const uint16_t masks[RASTER_LANE_COUNT] = {k->packedPixelMasks.blue, k->packedPixelMasks.green,
                                           k->packedPixelMasks.red, (uint16_t)k->packedPixelMasks.zero};
    const uint16_t scales[RASTER_LANE_COUNT] = {k->unpackScales.blue, k->unpackScales.green, k->unpackScales.red,
                                            (uint16_t)k->unpackScales.zero};
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)(uint16_t)((pixel & masks[i]) * scales[i]);
    }
    return result;
}

/* Adds an ARGB colour to a 16-bit pixel: source lanes (c * 0x101) >> sourceShift, destination
   Blit_Unpack16Unshifted, PADDUSW, then PSRLW 4 down to the Q12 scale Blit_PackLanes16 expects.
   The alpha lane goes through the same steps; the pack constants' fourth lane decides whether it
   reaches the pixel. */
static __inline uint16_t Blit_AddArgb16(uint32_t argb, uint16_t destination, int sourceShift)
{
    RasterColor source = Blit_ArgbLanes(argb, sourceShift);
    RasterColor sum = Blit_Unpack16Unshifted(destination);
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        sum.lane[i] = (short)(Blit_AddSaturateWord((uint16_t)sum.lane[i], (uint16_t)source.lane[i]) >> 4);
    }
    return Blit_PackLanes16(sum);
}

/* Adds an ARGB colour to a 32-bit pixel: both as lanes c * 0x101 (the source >> sourceShift),
   PADDUSW, PSRLW 8 and PACKUSWB. All four bytes, alpha included, are summed and written. */
static __inline uint32_t Blit_AddArgb32(uint32_t argb, uint32_t destination, int sourceShift)
{
    RasterColor source = Blit_ArgbLanes(argb, sourceShift);
    RasterColor target = Blit_ArgbLanes(destination, 0);
    int channel[RASTER_LANE_COUNT];
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        channel[i] = Blit_AddSaturateWord((uint16_t)target.lane[i], (uint16_t)source.lane[i]) >> 8;
    }
    return Raster_Pack32(channel);
}

/* ---- B3: integer-scaled blit ---------------------------------------------------------------- */
/*
SoftwareTextureSource_BlitIntegerScaledSourceAlpha16/32 do not clip the source. They walk the whole
image, replicate every texel scale x scale times, and test each written pixel against the clip
rectangle, which is first clamped to [0, framebuffer size) (signed compares). The original's
destination pointer walks the unclipped image; a pixel at (x, y) is at pixels + (y * width + x) *
pixelBytes, which is what BlitScaled_Pixel computes, and only for pixels that pass the clip test.
*/
typedef struct BlitScaledImage {
    const uint8_t *texels;  /* first texel of the subresource */
    int texelBytes;      /* 1 (palette index) or 4 (ARGB) */
    const uint8_t *palette; /* palette bank of a paletted image, else NULL */
    uint32_t width;         /* source size in texels (the original's loop counters; 0 would mean 2^32) */
    uint32_t height;
    int left;            /* framebuffer position of the scaled image: draw + origin * scale */
    int top;
    int clipMinX;        /* clip rectangle clamped to the framebuffer */
    int clipMinY;
    int clipMaxX;
    int clipMaxY;
} BlitScaledImage;

/* Validates and places an integer-scaled subresource. Differences to Blit_SetupSubresource: any
   negative paletteIndex (a sign test), not only -1, means ARGB texels, and the origin is scaled. */
static __inline int Blit_SetupScaled(const GraphicsTextureSourceAsset *sourceAsset,
                                     GraphicsSubresourceIndex subresourceIndex,
                                     const SoftwareFramebufferAccess *framebuffer, int pixelBytes, uint32_t scale,
                                     int drawX, int drawY, int clipMaxY, int clipMaxX, int clipMinY, int clipMinX,
                                     BlitScaledImage *image)
{
    const uint8_t *asset = (const uint8_t *)sourceAsset;
    const GraphicsTextureSourceEntry *entry;

    if (sourceAsset->common.magic != ASSET_MAGIC_GFX ||
        subresourceIndex >= sourceAsset->tableDescriptor.subresourceCount ||
        (int)framebuffer->bytesPerPixel != pixelBytes) {
        return 0;
    }
    entry = (const GraphicsTextureSourceEntry *)(asset + sourceAsset->tableDescriptor.subresourceTableOffset) +
            subresourceIndex;
    if (entry->paletteIndex < 0) {
        image->texelBytes = 4;
        image->palette = NULL;
    }
    else if ((uint32_t)entry->paletteIndex < sourceAsset->tableDescriptor.paletteBankCount) {
        image->texelBytes = 1;
        image->palette = asset + GFX_ASSET_HEADER_SIZE + (int32_t)((uint32_t)entry->paletteIndex * GFX_PALETTE_BANK_SIZE);
    }
    else {
        return 0;
    }
    image->texels = asset + entry->dataOffset;
    image->width = entry->pixelWidth;
    image->height = entry->pixelHeight;
    image->left = (int)((uint32_t)drawX + (uint32_t)entry->originX * scale);
    image->top = (int)((uint32_t)drawY + (uint32_t)entry->originY * scale);
    image->clipMinX = clipMinX < 0 ? 0 : clipMinX;
    image->clipMinY = clipMinY < 0 ? 0 : clipMinY;
    image->clipMaxX = clipMaxX > (int)framebuffer->width ? (int)framebuffer->width : clipMaxX;
    image->clipMaxY = clipMaxY > (int)framebuffer->height ? (int)framebuffer->height : clipMaxY;
    return 1;
}

static __inline int BlitScaled_RowVisible(const BlitScaledImage *image, int y)
{
    return y >= image->clipMinY && y < image->clipMaxY;
}

static __inline int BlitScaled_ColumnVisible(const BlitScaledImage *image, int x)
{
    return x >= image->clipMinX && x < image->clipMaxX;
}

/* The texel's alpha-test colour and blend colour. A paletted texel tests the entry's converted pixel
   (+4) and blends the entry's ARGB colour (+0), in both depths; a direct texel is both. */
static __inline uint32_t BlitScaled_TexelColor(const BlitScaledImage *image, const uint8_t *texel, uint32_t *blendColor)
{
    if (image->palette != NULL) {
        *blendColor = *(const uint32_t *)(image->palette + *texel * 8u);
        return *(const uint32_t *)(image->palette + *texel * 8u + 4u);
    }
    *blendColor = *(const uint32_t *)texel;
    return *blendColor;
}

static __inline uint8_t *BlitScaled_Pixel(const SoftwareFramebufferAccess *framebuffer, int pixelBytes, int x, int y)
{
    return framebuffer->pixels + (y * (int)framebuffer->width + x) * pixelBytes;
}

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H */
