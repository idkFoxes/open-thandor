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
0..255, PMULHW keeps the high half of a signed 16 x 16 product). The former rastercmp
self-test confirmed every handler against the original machine code; keep it identical when changing anything.

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
    uint8_t *pixels;          /* row 0 of the colour target */
    int pixelBytes;        /* 2 or 4 */
    int pixelStride;       /* bytes per colour row */
    uint8_t *depth;           /* row 0 of the depth buffer (one dword per pixel) */
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
enum {
    RASTER_SHADE_GOURAUD, /* interpolated vertex colours (modes 0..6, 16..22) */
    RASTER_SHADE_FLAT     /* colour of v0 for the whole triangle (modes 8..14, 24..30) */
};
typedef int RasterShading;

/* A texture as the textured modes (16..30) sample it: nearest texel, wrapped. */
typedef struct RasterTexture {
    const uint8_t *texels;    /* one byte per texel (paletted) or one dword (direct colour) */
    const uint8_t *palette;   /* 256 entries of 8 bytes (only the first dword is used), NULL for direct colour */
    uint32_t uMask;           /* (width - 1) << 12 */
    uint32_t vMask;           /* (height - 1) << 12 */
    int widthLog2;
} RasterTexture;

/* Edge walker. The long edge runs from v0 to v2 (vertices sorted by Y) and carries the attribute
   values; the short edge is v0 -> v1 for the upper part and v1 -> v2 for the lower part. */
typedef struct RasterEdges {
    int longX;             /* Q12 */
    int longXStep;         /* per scanline */
    int shortX;
    int shortXStep;
    uint32_t longDepth;
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
    uint8_t *pixel;           /* first pixel */
    uint32_t *depth;          /* its depth buffer entry */
    int count;             /* pixels to draw, > 0 */
    int pixelStep;         /* +pixelBytes or -pixelBytes */
    int depthPointerStep;  /* +1 or -1 (dwords) */
    uint32_t depthValue;      /* interpolated depth of the current pixel */
    uint32_t depthDelta;      /* per pixel, in span direction */
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

/* (a * b) >> shift of the 64-bit product, truncated to 32 bits. */
static __inline int Raster_MulShift(int a, int b, int shift)
{
    return (int)(((long long)a * b) >> shift);
}

/* Difference of two 32-bit values with wrap-around, e.g. of depths. */
static __inline int Raster_Diff(int a, int b)
{
    return (int)((uint32_t)a - (uint32_t)b);
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
static __inline RasterColor Raster_Unpack16(uint16_t pixel)
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
        result.lane[i] = (short)(scaled >> 4);
    }
    return result;
}

/* Unpacks a 32-bit pixel (blue in the low byte) into lanes of (c * 0x101) >> 4, about channel * 16
   (Q4): MOVD + PUNPCKLBW with itself + PSRLW 4. Used by the Non16 and Aux families. */
static __inline RasterColor Raster_Unpack32(uint32_t pixel)
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
static __inline uint16_t Raster_Pack16(const int channel[RASTER_LANE_COUNT])
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
        q[i] = (short)(((channel[i] * 0x101) >> 4) & masks[i]);
    }
    low = (uint32_t)(q[0] * (short)weights[0] + q[1] * (short)weights[1]);
    high = (uint32_t)(q[2] * (short)weights[2] + q[3] * (short)weights[3]);
    return (uint16_t)((low >> 8) + (high >> 8));
}

/* Packs four channel bytes into a 32-bit pixel (blue in the low byte, PACKUSWB + MOVD). */
static __inline uint32_t Raster_Pack32(const int channel[RASTER_LANE_COUNT])
{
    return (uint32_t)channel[0] | ((uint32_t)channel[1] << 8) | ((uint32_t)channel[2] << 16) |
           ((uint32_t)channel[3] << 24);
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
static __inline uint16_t Raster_ShadeToPixel16(RasterColor color)
{
    int channel[RASTER_LANE_COUNT];
    Raster_LanesToBytes(color, 6, channel);
    return Raster_Pack16(channel);
}

/* ---- blending ---------------------------------------------------------------------------- */

/* Original addresses of the two blend factor tables (256 rows of 8 bytes each). */
#define RASTER_BLEND_ALPHA_FACTORS_ORIGINAL 0x00421720u
#define RASTER_BLEND_INVERSE_FACTORS_ORIGINAL 0x00421F20u

/* The original dwords at 0x00422720-0x00422F1F, as the inverse table rows 256..511 read them, with
   pointers as their original values (fixed image base). Static in the original: the parts below are
   never written at runtime, so constants give the same values.
     00422720-00422767  g_UiGraphicsAdapterTextButtonVtable (18 function pointers)
     00422768-004227A7  g_GraphicsAdapterFormatScratch0/1Utf16: written at runtime, read live from
                        today's variables (g_SoftwareBlendOverreadRanges); the zeros here are unused
     004227A8-0042299F  0x90 padding and the machine code of UiGraphicsAdapterTextButton_DrawFormattedAdapterText,
                        UiRootCallbacks_Free, UiDisplaySettingsRoot_RefreshModeSelection,
                        UiModalDialogRoot_BlockMissedPointerPress/Motion
     004229A0-004229B3  g_UiDisplaySettingsRootCallbacks
     004229B4-00422F1F  start of g_UiDisplaySettingsRootTemplate (only copied, never written) */
static const uint32_t g_SoftwareBlendOverreadOriginalDwords[512] = {
    /* 00422720 */ 0x004B2E40, 0x004B05A0, 0x004227B0, 0x004B0640, 0x004B31B0, 0x004B0760, 0x004B0770, 0x004B07C0,
    /* 00422740 */ 0x004B07D0, 0x004B07E0, 0x004B07F0, 0x004B0800, 0x004B32C0, 0x004B08E0, 0x004B26E0, 0x004B2710,
    /* 00422760 */ 0x004B09E0, 0x004B09F0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 00422780 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 004227A0 */ 0x00000000, 0x00000000, 0x90909090, 0x90909090, 0x57525350, 0x8BE58955, 0x43F7285D, 0x00000848,
    /* 004227C0 */ 0x1C850F00, 0xFF000001, 0x000054B3, 0xA60EE800, 0xD08BFFFF, 0x804C43F7, 0x0F000000, 0x00006F85,
    /* 004227E0 */ 0x4C43F700, 0x00000800, 0x00A2850F, 0x68680000, 0xFF004227, 0x016AF873, 0x006A0A6A, 0x15FF406A,
    /* 00422800 */ 0x00402628, 0x42278868, 0xF473FF00, 0x0A6A016A, 0x406A006A, 0x262815FF, 0x68520040, 0x00422768,
    /* 00422820 */ 0xD9E8006A, 0x52FFFF88, 0x42278868, 0xE8016A00, 0xFFFF88CC, 0x2475FF53, 0xFF2075FF, 0x75FF1C75,
    /* 00422840 */ 0x061AE818, 0xEC890009, 0x5B5A5F5D, 0x0014C258, 0x42276868, 0xF873FF00, 0x0A6A016A, 0x406A006A,
    /* 00422860 */ 0x262815FF, 0x68520040, 0x00422768, 0x8DE8006A, 0x53FFFF88, 0xFF2475FF, 0x75FF2075, 0x1875FF1C,
    /* 00422880 */ 0x0905DBE8, 0x5DEC8900, 0x585B5A5F, 0x900014C2, 0xC1F87B8B, 0x3D0307E7, 0x004A8EA4, 0x0020C781,
    /* 004228A0 */ 0x57520000, 0x55E8006A, 0x83FFFF88, 0xFFFFF0BF, 0x0E7500FF, 0x00011168, 0xA522E800, 0xF88BFFFF,
    /* 004228C0 */ 0xC78106EB, 0x0000002A, 0x016A5752, 0xFF882FE8, 0x75FF53FF, 0x2075FF24, 0xFF1C75FF, 0x7DE81875,
    /* 004228E0 */ 0x89000905, 0x5A5F5DEC, 0x14C2585B, 0x90909000, 0xE5895550, 0xFF0C75FF, 0x40200415, 0x5DEC8900,
    /* 00422900 */ 0x0004C258, 0x90909090, 0x90909090, 0x90909090, 0x52515350, 0x8BE58955, 0x838B185D, 0x00000AD0,
    /* 00422920 */ 0x0A10938B, 0x833B0000, 0x00000140, 0x933B0875, 0x00000144, 0x83893874, 0x00000140, 0x01449389,
    /* 00422940 */ 0x52500000, 0x8EE815FF, 0xFF53004A, 0x000130B3, 0x34B3FF00, 0xFF000001, 0x000138B3, 0x3CB3FF00,
    /* 00422960 */ 0xE8000001, 0x00001408, 0x1902E853, 0xEC890000, 0x5B595A5D, 0x0004C258, 0x90909090, 0x90909090,
    /* 00422980 */ 0x9090C3F9, 0x90909090, 0x90909090, 0x90909090, 0x000008B8, 0x9090C300, 0x90909090, 0x90909090,
    /* 004229A0 */ 0x004228F0, 0x00422910, 0x00422980, 0x00000000, 0x00422990, 0xFFFFFFFF, 0x00000078, 0xFFFFFFFF,
    /* 004229C0 */ 0x004B4CC0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFF28, 0xFFFFFF70, 0x000000D8,
    /* 004229E0 */ 0x00000090, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000021,
    /* 00422A00 */ 0x00000007, 0x00000000, 0x00000000, 0x00000108, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 00422A20 */ 0x00000000, 0x00000000, 0x00000000, 0x000000D4, 0xFFFFFFFF, 0x00000000, 0x004B1D80, 0x00000000,
    /* 00422A40 */ 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x000000E8, 0x00000070, 0x00000100, 0x00000000,
    /* 00422A60 */ 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000002, 0x00000008, 0x0000020E,
    /* 00422A80 */ 0x00000101, 0x00000000, 0x00000160, 0xFFFFFFFF, 0x00000000, 0x004B1D80, 0x00000000, 0x00000000,
    /* 00422AA0 */ 0x00000000, 0x00000000, 0x00000080, 0x000000E8, 0x000000F0, 0x00000100, 0x00000000, 0x00000000,
    /* 00422AC0 */ 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028, 0x00000004, 0x00000200, 0x00000100,
    /* 00422AE0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 00422B00 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000001BC, 0xFFFFFFFF, 0x00000000,
    /* 00422B20 */ 0x004B9530, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000000A0, 0x00000008, 0x000000F8,
    /* 00422B40 */ 0x0000001C, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    /* 00422B60 */ 0x00000000, 0x00000000, 0x0000010C, 0x00000000, 0x00000218, 0xFFFFFFFF, 0x00000000, 0x004B9530,
    /* 00422B80 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000018, 0x00000008, 0x00000078, 0x0000001C,
    /* 00422BA0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
    /* 00422BC0 */ 0x00000000, 0x0000010D, 0x00000000, 0x00000280, 0xFFFFFFFF, 0x00000000, 0x004B9530, 0x00000000,
    /* 00422BE0 */ 0x00000000, 0x00000000, 0x00000000, 0x00000018, 0x00000070, 0x00000078, 0x00000084, 0x00000000,
    /* 00422C00 */ 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    /* 00422C20 */ 0x0000010F, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000002E8, 0xFFFFFFFF, 0x00000000,
    /* 00422C40 */ 0x00422720, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x0000001C, 0x00000078,
    /* 00422C60 */ 0x00000030, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028,
    /* 00422C80 */ 0x00000480, 0x00000201, 0x00000106, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000350,
    /* 00422CA0 */ 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000010,
    /* 00422CC0 */ 0x00000030, 0x00000078, 0x00000044, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF,
    /* 00422CE0 */ 0xFFFFFFFF, 0x00000028, 0x00000480, 0x00000202, 0x00000106, 0x00000000, 0x00000000, 0x00000000,
    /* 00422D00 */ 0x00000000, 0x000003B8, 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000, 0x00000000, 0x00000000,
    /* 00422D20 */ 0x00000000, 0x00000010, 0x00000044, 0x00000078, 0x00000058, 0x00000000, 0x00000000, 0x00000000,
    /* 00422D40 */ 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028, 0x00000480, 0x00000203, 0x00000106, 0x00000000,
    /* 00422D60 */ 0x00000000, 0x00000000, 0x00000000, 0x00000420, 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000,
    /* 00422D80 */ 0x00000000, 0x00000000, 0x00000000, 0x00000010, 0x00000058, 0x00000078, 0x0000006C, 0x00000000,
    /* 00422DA0 */ 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028, 0x00000480, 0x00000204,
    /* 00422DC0 */ 0x00000106, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000488, 0xFFFFFFFF, 0x00000000,
    /* 00422DE0 */ 0x00422720, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000098, 0x0000001C, 0x000000F8,
    /* 00422E00 */ 0x00000030, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028,
    /* 00422E20 */ 0x00000400, 0x00000205, 0x00000107, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000004F0,
    /* 00422E40 */ 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000098,
    /* 00422E60 */ 0x00000030, 0x000000F8, 0x00000044, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF,
    /* 00422E80 */ 0xFFFFFFFF, 0x00000028, 0x00000400, 0x00000206, 0x00000107, 0x00000000, 0x00000000, 0x00000000,
    /* 00422EA0 */ 0x00000000, 0x00000558, 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000, 0x00000000, 0x00000000,
    /* 00422EC0 */ 0x00000000, 0x00000098, 0x00000044, 0x000000F8, 0x00000058, 0x00000000, 0x00000000, 0x00000000,
    /* 00422EE0 */ 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000028, 0x00000400, 0x00000207, 0x00000107, 0x00000000,
    /* 00422F00 */ 0x00000000, 0x00000000, 0x00000000, 0x000005C0, 0xFFFFFFFF, 0x00000000, 0x00422720, 0x00000000};

/* What lies at an original address in the range a blend factor row can be read from. */
typedef struct RasterOriginalRange {
    uint32_t start; /* original address */
    uint32_t end;   /* exclusive */
    const void *data;
} RasterOriginalRange;

static const RasterOriginalRange g_SoftwareBlendOverreadRanges[] = {
    {0x00421720, 0x00421F20, g_SoftwareBlendAlphaFactors},
    {0x00421F20, 0x00422720, g_SoftwareBlendInverseAlphaFactors},
    /* runtime scratch, read live; listed before the constant table that spans them */
    {0x00422768, 0x00422788, g_GraphicsAdapterFormatScratch0Utf16},
    {0x00422788, 0x004227A8, g_GraphicsAdapterFormatScratch1Utf16},
    {0x00422720, 0x00422F20, g_SoftwareBlendOverreadOriginalDwords},
    {0x004246A0, 0x004846A0, g_FixedSineQ28},
};

/* Original quirk: the blend index is the top 12 bits of the source alpha lane (PSRLQ 0x34) and is
   never clamped. The lane is a PSRAW / PMULHW result in -0x2000..0x1FFF, so the index is 0..0x1FF
   (an alpha that overshoots 255, e.g. 256 and 257 from interpolation at alpha 255) or 0xE00..0xFFF
   (a negative alpha). The original then reads its rows of 8 bytes beyond the 256-row tables:
     alpha table, index 256..511       -> g_SoftwareBlendInverseAlphaFactors
     inverse table, index 256..511     -> 0x422720-0x422F1F: the text button vtable, two scratch
                                          strings, code, the display settings callbacks and template
     both tables, index 0xE00..0xFFF   -> g_FixedSineQ28 (0x428720-0x429F1F)
   This returns the dword the original reads at such an address, from the first range of
   g_SoftwareBlendOverreadRanges that holds it: the variables that hold it today (tables, scratch
   strings, sine table) or the original constants g_SoftwareBlendOverreadOriginalDwords. */
static uint32_t Raster_OriginalBlendDword(uint32_t address)
{
    unsigned i;
    for (i = 0; i < sizeof g_SoftwareBlendOverreadRanges / sizeof g_SoftwareBlendOverreadRanges[0]; i++) {
        const RasterOriginalRange *range = &g_SoftwareBlendOverreadRanges[i];
        if (address >= range->start && address < range->end) {
            return *(const uint32_t *)((const uint8_t *)range->data + (address - range->start));
        }
    }
    return 0; /* not reachable with an index of 0..0x1FF or 0xE00..0xFFF */
}

/* The four word lanes of the 8-byte row at an original address (see Raster_OriginalBlendDword). */
static void Raster_OriginalBlendRow(uint32_t address, short lanes[RASTER_LANE_COUNT])
{
    uint32_t low = Raster_OriginalBlendDword(address);
    uint32_t high = Raster_OriginalBlendDword(address + 4);
    lanes[0] = (short)low;
    lanes[1] = (short)(low >> 16);
    lanes[2] = (short)high;
    lanes[3] = (short)(high >> 16);
}

/* Alpha blend of two Q4 colours: source * alpha + destination * (1 - alpha), both through the
   g_SoftwareBlendAlphaFactors / g_SoftwareBlendInverseAlphaFactors tables (PMULHW). The table index
   is the top 12 bits of the source alpha lane; an index past the 256 rows reads the original bytes
   behind the tables (Raster_OriginalBlendDword). Result in Q4. */
static __inline RasterColor Raster_BlendAlpha(RasterColor sourceQ4, RasterColor destinationQ4)
{
    unsigned index = (uint16_t)sourceQ4.lane[RASTER_LANE_ALPHA] >> 4;
    short alphaLanes[RASTER_LANE_COUNT];
    short inverseLanes[RASTER_LANE_COUNT];
    RasterColor result;
    int i;
    if (index < 256) {
        const SoftwareRgbWordLanes *alpha = &g_SoftwareBlendAlphaFactors[index];
        const SoftwareRgbWordLanes *inverse = &g_SoftwareBlendInverseAlphaFactors[index];
        alphaLanes[0] = (short)alpha->blue;
        alphaLanes[1] = (short)alpha->green;
        alphaLanes[2] = (short)alpha->red;
        alphaLanes[3] = (short)alpha->zero;
        inverseLanes[0] = (short)inverse->blue;
        inverseLanes[1] = (short)inverse->green;
        inverseLanes[2] = (short)inverse->red;
        inverseLanes[3] = (short)inverse->zero;
    } else {
        Raster_OriginalBlendRow(RASTER_BLEND_ALPHA_FACTORS_ORIGINAL + index * 8u, alphaLanes);
        Raster_OriginalBlendRow(RASTER_BLEND_INVERSE_FACTORS_ORIGINAL + index * 8u, inverseLanes);
    }
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
    edges->longDepth = (uint32_t)v0->depth;
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
        span.depthDelta = (uint32_t)gradients->depthStepX;
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
        span.depthDelta = (uint32_t)-gradients->depthStepX;
        span.colorDelta = RasterColor_Negate(gradients->colorStepX);
        span.uDelta = -gradients->uStepX;
        span.vDelta = -gradients->vStepX;
        prestep = (first << 12) - edges->longX;
        first--;
    }
    span.pixel = target->pixels + edges->scanlineY * target->pixelStride + first * target->pixelBytes;
    span.depth = (uint32_t *)(target->depth + edges->scanlineY * target->depthStride) + first;
    span.depthValue = edges->longDepth + (uint32_t)Raster_MulShift(prestep, gradients->depthStepX, 12);
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
        edges->longDepth += (uint32_t)edges->longDepthStep;
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
    target.depth = (uint8_t *)g_SoftwareDepthBuffer;
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
    target.pixels = (uint8_t *)g_SoftwareAuxiliaryTargetBase;
    target.pixelBytes = 4;
    target.pixelStride = clipMaxX * 4;
    target.depth = (uint8_t *)g_SoftwareDepthBuffer;
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
    const uint8_t *asset = (const uint8_t *)entry->sourceAsset;
    const GraphicsTextureSourceEntry *source = entry->sourceEntry;
    int paletteIndex = (int)source->paletteIndex;
    texture->widthLog2 = (int)entry->widthLog2;
    texture->uMask = ((1u << (entry->widthLog2 & 31)) - 1) << 12;
    texture->vMask = ((1u << (entry->heightLog2 & 31)) - 1) << 12;
    texture->texels = asset + source->dataOffset;
    texture->palette =
        paletteIndex < 0 ? NULL : asset + GFX_ASSET_HEADER_SIZE + (uint32_t)paletteIndex * GFX_PALETTE_BANK_SIZE;
}

/* The ARGB texel at (u, v), both wrapped to the texture (nearest texel, no filtering). */
static __inline uint32_t Raster_FetchTexel(const RasterTexture *texture, int u, int v)
{
    uint32_t index = (((uint32_t)u & texture->uMask) >> 12) +
                  (uint32_t)(((unsigned long long)((uint32_t)v & texture->vMask) << 32) >> (44 - texture->widthLog2));
    if (texture->palette != NULL) {
        return *(const uint32_t *)(texture->palette + texture->texels[index] * 8u);
    }
    return ((const uint32_t *)texture->texels)[index];
}

/* An ARGB texel as lanes of (c * 0x101) >> 2 (PUNPCKLBW + PSRLW 2), ready for Raster_Modulate. */
static __inline RasterColor Raster_TexelLanes(uint32_t argb)
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
   value instead (see docs/software_raster.md), and the C keeps this rule for them. */
static __inline int Raster_AlphaWritesDepth(RasterColor sourceQ4)
{
    return (uint16_t)sourceQ4.lane[RASTER_LANE_ALPHA] >= 0x800;
}

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
        region->palette = asset + GFX_ASSET_HEADER_SIZE + (uint32_t)entry->paletteIndex * GFX_PALETTE_BANK_SIZE;
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
        image->palette = asset + GFX_ASSET_HEADER_SIZE + (uint32_t)entry->paletteIndex * GFX_PALETTE_BANK_SIZE;
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

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_RASTER_H */
