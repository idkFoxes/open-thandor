/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software_blit_helpers.h
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H

#include <thandor/core/types.h>
#include <thandor/core/x86_emulation.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/resources/texture.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include "software_raster.h"

/* ---- Texture-source blits and rectangle fills --------------------------------------------- */
/*
SoftwareTextureSource_Blit*, SoftwareFramebuffer_FillRectArgb* (see docs/software_raster.md,
"Blits"). The former blitcmp self-test confirmed them against the original machine code.

Texture source asset: "gfx" magic, tableDescriptor at +0xB0 (subresourceCount, paletteBankCount,
subresourceTableOffset), palette banks of 256 * 8 bytes at +0x200, and a table of 32-byte
GraphicsTextureSourceEntry records. A palette entry holds two dwords: the ARGB colour at +0 and,
at +4, the colour converted to the framebuffer format (g_GraphicsTextureSourceConvertPaletteEntries)
with the alpha in its top byte. Most blits use +4 for everything (they treat it as ARGB); the
original's 16-bit blits (gone with 16-bit colour) tested the alpha of +4, wrote its low word and
blended +0. An entry with paletteIndex -1 stores ARGB dwords instead of
8-bit indices.

A source colour whose alpha is 0 is skipped, alpha 0xFF is written as is (converted), anything in
between is blended through g_SoftwareBlendAlphaFactors / g_SoftwareBlendInverseAlphaFactors. The
original tests the whole dword (< 0x1000000, >= 0xFF000000); Blit_IsTransparent / Blit_IsOpaque do
the same.
*/

/* One clipped image in the framebuffer, and where its first texel is. */
struct BlitRegion {
    const uint8_t *texels;    /* texel of the top-left drawn pixel */
    int texelBytes;        /* 1 (palette index) or 4 (ARGB) */
    int texelStride;       /* bytes per source row */
    const uint8_t *palette;   /* palette bank of a paletted image (256 entries of 8 bytes), else NULL */
    uint8_t *pixels;          /* top-left drawn pixel */
    int pixelBytes;        /* 4 */
    int pixelStride;       /* bytes per framebuffer row (framebuffer->width pixels) */
    int width;             /* drawn size in pixels, both > 0 */
    int height;
};

static inline int Blit_IsTransparent(uint32_t argb)
{
    return argb < 0x1000000u;
}

static inline int Blit_IsOpaque(uint32_t argb)
{
    return argb >= 0xff000000u;
}

/* The two dwords of a palette entry: +0 ARGB colour, +4 converted pixel with the alpha on top. */
static inline uint32_t Blit_PaletteColor(const BlitRegion *region, uint8_t index)
{
    return Thandor_LoadU32(region->palette + index * 8u);
}

static inline uint32_t Blit_PalettePixel(const BlitRegion *region, uint8_t index)
{
    return Thandor_LoadU32(region->palette + index * 8u + 4u);
}

/* ARGB -> framebuffer pixel through the g_SoftwarePixelPackTables channel tables. The alpha byte is
   added on top. */
static inline uint32_t Blit_ConvertArgb(uint32_t argb)
{
    const SoftwarePixelPackTables *tables = g_SoftwarePixelPackTables;
    return tables->blue[argb & 0xff] + (argb & 0xff000000u) + tables->green[(argb >> 8) & 0xff] +
           tables->red[(argb >> 16) & 0xff];
}

/* ARGB (or a 32-bit pixel) as lanes of (c * 0x101) >> shift (PUNPCKLBW with itself + PSRLW). */
static inline RasterColor Blit_ArgbLanes(uint32_t argb, int shift)
{
    RasterColor result;
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        result.lane[i] = (short)((Raster_Channel(argb, i) * 0x101) >> shift);
    }
    return result;
}

/* source * alpha + destination * (1 - alpha) through the blend factor tables (two PMULHW + PADDW).
   alpha is the source alpha byte (1..254 for the blits). */
static inline RasterColor Blit_BlendLanes(RasterColor source, RasterColor destination, unsigned alpha)
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

/* Blended lanes -> 32-bit pixel: PSRLW 4 (logical) + PACKUSWB, alpha lane included. */
static inline uint32_t Blit_PackLanes32(RasterColor lanes)
{
    int channel[RASTER_LANE_COUNT];
    int i;
    for (i = 0; i < RASTER_LANE_COUNT; i++) {
        channel[i] = Raster_SaturateByte((uint16_t)lanes.lane[i] >> 4);
    }
    return Raster_Pack32(channel);
}

/* Source-alpha blend of an ARGB colour over a 32-bit pixel, alpha = the colour's top byte. */
static inline uint32_t Blit_BlendArgb32(uint32_t argb, uint32_t destination)
{
    return Blit_PackLanes32(Blit_BlendLanes(Blit_ArgbLanes(argb, 2), Blit_ArgbLanes(destination, 2), argb >> 24));
}

/* Intersects [left, right) x [top, bottom) with the framebuffer and the clip rectangle, in the
   original's order: rectangle clamped to 0 and to the framebuffer size, then to the clip
   rectangle (all compares signed). Returns 0 when nothing is left. */
static inline int Blit_ClipRect(const SoftwareFramebufferAccess *framebuffer, int clipMaxY, int clipMaxX,
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
static inline int Blit_SetupSubresource(const GraphicsTextureSourceAsset *sourceAsset,
                                          GraphicsSubresourceIndex subresourceIndex,
                                          SoftwareFramebufferAccess *framebuffer, int pixelBytes, int drawX, int drawY,
                                          int clipMaxY, int clipMaxX, int clipMinY, int clipMinX, BlitRegion *region)
{
    const uint8_t *asset = GraphicsTextureSource_Bytes(sourceAsset);
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
    entry = GraphicsTextureSource_Entries(sourceAsset) + subresourceIndex;
    if (entry->paletteIndex == -1) {
        region->texelBytes = 4;
        region->palette = nullptr;
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

/* ---- B1: clipped blend variants (HalfSourceRgb, Modulated) -------------------------------- */

/* The BlitModulatedSourceAlpha channel product, as the original's four IMULs: every channel of argb is
   multiplied by the matching channel of modulation. Blue keeps (b * mb) >> 8; green, red and alpha keep
   the high byte of their product ((c * mc) & 0xFF00) shifted into place. All four agree with
   (c * mc) >> 8, so even modulation 0xFFFFFFFF darkens by one step (0xFF * 0xFF >> 8 = 0xFE). In
   particular the modulated alpha is at most 0xFE: the opaque shortcut of the Modulated blits is dead
   code, and every visible texel goes through the blend. */
static inline uint32_t Blit_Modulate(uint32_t argb, uint32_t modulation)
{
    uint32_t blue = ((argb & 0xff) * (modulation & 0xff)) >> 8;
    uint32_t green = (((argb >> 8) & 0xff) * ((modulation >> 8) & 0xff)) & 0xff00u;
    uint32_t red = (((argb >> 16) & 0xff) * ((modulation >> 16) & 0xff)) & 0xff00u;
    uint32_t alpha = ((argb >> 24) * (modulation >> 24)) & 0xff00u;
    return blue | green | (red << 8) | (alpha << 16);
}

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_BLIT_HELPERS_H */
