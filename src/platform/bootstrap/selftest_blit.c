/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/selftest_blit.c
 */

/*
OPEN_THANDOR_SELFTEST=blitcmp: differential test of the software texture-source blits, the ARGB
rectangle fills and the mask-buffer step (graphics/backend/software.c) against a copy of the ORIGINAL
machine code:
  SoftwareTextureSource_Blit{SourceAlpha,HalfSourceRgb,IntegerScaledSourceAlpha,SourceAlphaPaletteBank,
  SaturatedAddRgb,HalfRgbSaturatedAdd,ModulatedSourceAlpha}{16,32}, SoftwareFramebuffer_FillRectArgb{16,32}
  (0x4A93C0..0x4AD410) and SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31 (0x519270..0x519318).

Inputs per run (deterministic from the case and run number):
  - a texture source asset ("gfx" magic, 1..4 palette banks of 256 * 8 bytes at 0x200, 1..4
    subresources of 1..48 x 1..40 texels with origins -24..24). Each subresource is paletted
    (8-bit indices) or direct 32-bit ARGB (paletteIndex -1); some get an invalid palette index.
    Palette dwords (+0 ARGB, +4 native pixel with alpha) and direct texels get alpha 0, 0xFF or
    random, and sometimes RGB 0, so every branch of the pixel loops is taken,
  - sometimes an invalid magic, subresource index or framebuffer pixel size (early returns),
  - a 16-bit or 32-bit framebuffer of 1..160 x 1..120 pixels with random contents,
  - a clip rectangle (partly outside, sometimes empty or negative) and a draw position that places the
    image inside, partly outside or completely outside the framebuffer,
  - 565 or 555 pixel constants (g_SoftwarePixelMmxConstants), a random g_SoftwarePixelPackTables,
  - integer scale 1..4, palette bank (sometimes out of range), modulation colour, fill colour.
Both versions run on identical copies; the framebuffer (with 1 KB guards on both sides), the asset,
the mask buffer (with guards) and the returned carry flag are compared.

Needs the mapped-image build (-DTHANDOR_MAPPED_IMAGE=ON) and thandor_original.exe next to the exe.
Environment:
  OPEN_THANDOR_BLITCMP_RUNS    runs per function (default 500)
  OPEN_THANDOR_BLITCMP_SEED    base seed (default 1)
  OPEN_THANDOR_BLITCMP_FILTER  only functions whose name contains this text
Log lines start with "blitcmp".
*/

#include <stdlib.h>
#include <string.h>
#include <excpt.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

void Thandor_SelfTestBlitCompare(void);

#ifndef THANDOR_MAPPED_IMAGE

void Thandor_SelfTestBlitCompare(void)
{
    Thandor_Log("blitcmp: needs a build with -DTHANDOR_MAPPED_IMAGE=ON");
}

#else

#define BLIT_CODE_START 0x4a93c0u
#define BLIT_CODE_END 0x4ad410u
#define MASK_CODE_START 0x519270u
#define MASK_CODE_END 0x519318u

#define BLIT_GUARD 1024u
#define BLIT_ASSET_SLACK 0x10000u
#define BLIT_HEADER 0x200u
#define BLIT_BANK_BYTES 0x800u

enum BlitKind {
    BLIT_KIND_PLAIN,       /* (clip..., drawY, drawX, index, asset, framebuffer) */
    BLIT_KIND_EXTRA,       /* (clip..., drawY, drawX, extra, index, asset, framebuffer) */
    BLIT_KIND_FILL,        /* (clip..., rectMaxY, rectMaxX, rectMinY, rectMinX, argb, framebuffer) */
    BLIT_KIND_MASK         /* (maskRuntime) */
};

enum BlitExtra {
    BLIT_EXTRA_NONE,
    BLIT_EXTRA_SCALE,
    BLIT_EXTRA_BANK,
    BLIT_EXTRA_MODULATION
};

typedef bool (*BlitPlainProc)(GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                              GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                              GraphicsSubresourceIndex, GraphicsTextureSourceAsset *, SoftwareFramebufferAccess *);
typedef void (*BlitScaleProc)(GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                              GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                              GraphicsIntegerScale, GraphicsSubresourceIndex, GraphicsTextureSourceAsset *,
                              SoftwareFramebufferAccess *);
typedef void (*BlitBankProc)(GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                             GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                             PaletteBankIndex, GraphicsSubresourceIndex, GraphicsTextureSourceAsset *,
                             SoftwareFramebufferAccess *);
typedef bool (*BlitModulatedProc)(GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                                  GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                                  PackedArgb32, GraphicsSubresourceIndex, GraphicsTextureSourceAsset *,
                                  SoftwareFramebufferAccess *);
typedef void (*BlitFillProc)(GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                             GraphicsScreenCoordinate, GraphicsScreenCoordinate, GraphicsScreenCoordinate,
                             GraphicsScreenCoordinate, GraphicsScreenCoordinate, PackedArgb32,
                             SoftwareFramebufferAccess *);
typedef void (*BlitMaskProc)(SoftwareMaskRuntimeView *);

typedef struct BlitCase {
    const char *name;
    int kind;
    int extra;
    int pixelBytes;
    unsigned originalAddress;
    void *function;
} BlitCase;

static const BlitCase s_BlitCases[] = {
    {"SoftwareTextureSource_BlitSourceAlpha16", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 2, 0x4a93c0,
     (void *)SoftwareTextureSource_BlitSourceAlpha16},
    {"SoftwareTextureSource_BlitSourceAlpha32", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 4, 0x4a9710,
     (void *)SoftwareTextureSource_BlitSourceAlpha32},
    {"SoftwareTextureSource_BlitHalfSourceRgb16", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 2, 0x4a9b20,
     (void *)SoftwareTextureSource_BlitHalfSourceRgb16},
    {"SoftwareTextureSource_BlitHalfSourceRgb32", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 4, 0x4a9e10,
     (void *)SoftwareTextureSource_BlitHalfSourceRgb32},
    {"SoftwareTextureSource_BlitIntegerScaledSourceAlpha16", BLIT_KIND_EXTRA, BLIT_EXTRA_SCALE, 2, 0x4aa630,
     (void *)SoftwareTextureSource_BlitIntegerScaledSourceAlpha16},
    {"SoftwareTextureSource_BlitIntegerScaledSourceAlpha32", BLIT_KIND_EXTRA, BLIT_EXTRA_SCALE, 4, 0x4aaa40,
     (void *)SoftwareTextureSource_BlitIntegerScaledSourceAlpha32},
    {"SoftwareTextureSource_BlitSourceAlphaPaletteBank16", BLIT_KIND_EXTRA, BLIT_EXTRA_BANK, 2, 0x4aade0,
     (void *)SoftwareTextureSource_BlitSourceAlphaPaletteBank16},
    {"SoftwareTextureSource_BlitSourceAlphaPaletteBank32", BLIT_KIND_EXTRA, BLIT_EXTRA_BANK, 4, 0x4ab150,
     (void *)SoftwareTextureSource_BlitSourceAlphaPaletteBank32},
    {"SoftwareTextureSource_BlitSaturatedAddRgb16", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 2, 0x4ab4a0,
     (void *)SoftwareTextureSource_BlitSaturatedAddRgb16},
    {"SoftwareTextureSource_BlitSaturatedAddRgb32", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 4, 0x4ab750,
     (void *)SoftwareTextureSource_BlitSaturatedAddRgb32},
    {"SoftwareTextureSource_BlitHalfRgbSaturatedAdd16", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 2, 0x4aba70,
     (void *)SoftwareTextureSource_BlitHalfRgbSaturatedAdd16},
    {"SoftwareTextureSource_BlitHalfRgbSaturatedAdd32", BLIT_KIND_PLAIN, BLIT_EXTRA_NONE, 4, 0x4abd20,
     (void *)SoftwareTextureSource_BlitHalfRgbSaturatedAdd32},
    {"SoftwareTextureSource_BlitModulatedSourceAlpha16", BLIT_KIND_EXTRA, BLIT_EXTRA_MODULATION, 2, 0x4ac040,
     (void *)SoftwareTextureSource_BlitModulatedSourceAlpha16},
    {"SoftwareTextureSource_BlitModulatedSourceAlpha32", BLIT_KIND_EXTRA, BLIT_EXTRA_MODULATION, 4, 0x4ac4c0,
     (void *)SoftwareTextureSource_BlitModulatedSourceAlpha32},
    {"SoftwareFramebuffer_FillRectArgb16", BLIT_KIND_FILL, BLIT_EXTRA_NONE, 2, 0x4ad110,
     (void *)SoftwareFramebuffer_FillRectArgb16},
    {"SoftwareFramebuffer_FillRectArgb32", BLIT_KIND_FILL, BLIT_EXTRA_NONE, 4, 0x4ad2a0,
     (void *)SoftwareFramebuffer_FillRectArgb32},
    {"SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31", BLIT_KIND_MASK, BLIT_EXTRA_NONE, 1, 0x519270,
     (void *)SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31},
};

static unsigned BlitRandom(unsigned *state)
{
    unsigned x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static int BlitRange(unsigned *state, int low, int high)
{
    return low + (int)(BlitRandom(state) % (unsigned)(high - low + 1));
}

static void BlitFill(uint8_t *data, unsigned size, unsigned *state)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        data[i] = (uint8_t)(BlitRandom(state) >> 11);
    }
}

/* ARGB with alpha 0 / 0xFF / random and sometimes RGB 0, so each pixel branch is taken. */
static uint32_t BlitRandomArgb(unsigned *state)
{
    uint32_t value = BlitRandom(state);
    switch (BlitRandom(state) % 8) {
    case 0:
    case 1:
        value &= 0x00ffffffu;
        break;
    case 2:
    case 3:
        value |= 0xff000000u;
        break;
    case 4:
        value &= 0xff000000u;
        break;
    default:
        break;
    }
    return value;
}

static int BlitFirstDifference(const uint8_t *a, const uint8_t *b, unsigned size)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return (int)i;
        }
    }
    return -1;
}

static void BlitSetPixelConstants(int layout555)
{
    /* SoftwarePixelFormat_BaseDisplayModeHook for R5G6B5 / X1R5G5B5 */
    SoftwarePixelMmxConstants *k = &g_SoftwarePixelMmxConstants;
    k->quantizeMasksQ12.blue = 0xf80;
    k->quantizeMasksQ12.green = layout555 ? 0xf80 : 0xfc0;
    k->quantizeMasksQ12.red = 0xf80;
    k->quantizeMasksQ12.zero = 0;
    k->packWeights.blue = 2;
    k->packWeights.green = layout555 ? 0x40 : 0x80;
    k->packWeights.red = layout555 ? 0x800 : 0x1000;
    k->packWeights.zero = 0;
    k->packedPixelMasks.blue = 0x1f;
    k->packedPixelMasks.green = layout555 ? 0x3e0 : 0x7e0;
    k->packedPixelMasks.red = (SoftwareColorLaneFixed16)(layout555 ? 0x7c00 : 0xf800);
    k->packedPixelMasks.zero = 0;
    k->unpackScales.blue = 0x800;
    k->unpackScales.green = layout555 ? 0x40 : 0x20;
    k->unpackScales.red = layout555 ? 2 : 1;
    k->unpackScales.zero = 0;
}

/* g_GraphicsTextureSourceGetLogicalSize for the mask test: the C code calls it as a C function that
   returns the struct, the original expects width in EAX, height in EDX, CF clear and ECX preserved. */
static uint32_t s_BlitMaskWidth;
static uint32_t s_BlitMaskHeight;

static TextureSizeResult BlitMaskSizeForC(uint32_t reserved, GraphicsTextureSourceAsset *asset)
{
    TextureSizeResult size;
    (void)reserved;
    (void)asset;
    size.logicalWidthPixels = s_BlitMaskWidth;
    size.logicalHeightPixels = s_BlitMaskHeight;
    size.carry = false;
    return size;
}

static __declspec(naked) void BlitMaskSizeForOriginal(void)
{
    __asm {
        mov eax, s_BlitMaskWidth
        mov edx, s_BlitMaskHeight
        clc
        ret 8
    }
}

/* Pushes count dwords (args[0] = first parameter) and calls the stdcall original; returns CF. */
static unsigned BlitCallOriginal(void *entry, const uint32_t *args, int count)
{
    unsigned carry;
    __asm {
        push ebx
        push esi
        push edi
        mov ecx, count
        mov esi, args
    push_next:
        push uint32_t ptr [esi + ecx * 4 - 4]
        dec ecx
        jnz push_next
        cld
        mov eax, entry
        call eax
        setc al
        movzx eax, al
        mov carry, eax
        pop edi
        pop esi
        pop ebx
        emms
    }
    return carry;
}

static unsigned BlitCallC(const BlitCase *testCase, const uint32_t *a)
{
    unsigned carry = 0;
    switch (testCase->kind) {
    case BLIT_KIND_PLAIN:
        carry = ((BlitPlainProc)testCase->function)((int)a[0], (int)a[1], (int)a[2], (int)a[3], (int)a[4], (int)a[5],
                                                    a[6], (GraphicsTextureSourceAsset *)(uintptr_t)a[7],
                                                    (SoftwareFramebufferAccess *)(uintptr_t)a[8]);
        break;
    case BLIT_KIND_EXTRA:
        if (testCase->extra == BLIT_EXTRA_SCALE) {
            ((BlitScaleProc)testCase->function)((int)a[0], (int)a[1], (int)a[2], (int)a[3], (int)a[4], (int)a[5],
                                                (GraphicsIntegerScale)a[6], a[7],
                                                (GraphicsTextureSourceAsset *)(uintptr_t)a[8],
                                                (SoftwareFramebufferAccess *)(uintptr_t)a[9]);
        }
        else if (testCase->extra == BLIT_EXTRA_BANK) {
            ((BlitBankProc)testCase->function)((int)a[0], (int)a[1], (int)a[2], (int)a[3], (int)a[4], (int)a[5],
                                               (PaletteBankIndex)a[6], a[7],
                                               (GraphicsTextureSourceAsset *)(uintptr_t)a[8],
                                               (SoftwareFramebufferAccess *)(uintptr_t)a[9]);
        }
        else {
            carry = ((BlitModulatedProc)testCase->function)((int)a[0], (int)a[1], (int)a[2], (int)a[3], (int)a[4],
                                                            (int)a[5], a[6], a[7],
                                                            (GraphicsTextureSourceAsset *)(uintptr_t)a[8],
                                                            (SoftwareFramebufferAccess *)(uintptr_t)a[9]);
        }
        break;
    case BLIT_KIND_FILL:
        ((BlitFillProc)testCase->function)((int)a[0], (int)a[1], (int)a[2], (int)a[3], (int)a[4], (int)a[5],
                                           (int)a[6], (int)a[7], a[8], (SoftwareFramebufferAccess *)(uintptr_t)a[9]);
        break;
    default:
        ((BlitMaskProc)testCase->function)((SoftwareMaskRuntimeView *)(uintptr_t)a[0]);
        break;
    }
    return carry;
}

/* Returns nonzero when the call faulted. */
static int BlitRunGuarded(const BlitCase *testCase, void *originalEntry, const uint32_t *args, int count,
                          unsigned *carry, unsigned *exceptionCode)
{
    __try {
        if (originalEntry != NULL) {
            *carry = BlitCallOriginal(originalEntry, args, count);
        }
        else {
            *carry = BlitCallC(testCase, args);
            __asm emms
        }
    }
    __except (*exceptionCode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
        __asm emms
        return 1;
    }
    return 0;
}

typedef struct BlitBuffers {
    uint8_t *pixels;   /* framebuffer with BLIT_GUARD before and after */
    uint8_t *asset;
    uint8_t *mask;     /* mask with BLIT_GUARD before and after */
} BlitBuffers;

typedef struct BlitSetup {
    int width;
    int height;
    int pixelBytes;          /* framebuffer->bytesPerPixel as passed */
    unsigned pixelBytesTotal;
    unsigned assetBytes;
    unsigned maskBytes;
    int subresourceCount;
    int bankCount;
    int layout555;
    unsigned invalid;        /* which early return is exercised (0 = none) */
    char describe[160];
} BlitSetup;

/* Builds the asset: header, bankCount palettes at 0x200, subresource table, texel data. */
static void BlitBuildAsset(BlitSetup *setup, uint8_t *asset, unsigned *state)
{
    GraphicsTextureSourceAsset *header = (GraphicsTextureSourceAsset *)asset;
    unsigned tableOffset = BLIT_HEADER + (unsigned)setup->bankCount * BLIT_BANK_BYTES;
    unsigned dataOffset = tableOffset + (unsigned)setup->subresourceCount * 0x20u;
    unsigned i;
    int s;

    memset(asset, 0, setup->assetBytes);
    BlitFill(asset, BLIT_HEADER, state);
    header->common.magic = ASSET_MAGIC_GFX;
    header->tableDescriptor.subresourceCount = (uint32_t)setup->subresourceCount;
    header->tableDescriptor.paletteBankCount = (uint32_t)setup->bankCount;
    header->tableDescriptor.subresourceTableOffset = tableOffset;
    for (i = 0; i < (unsigned)setup->bankCount * 256u * 2u; i++) {
        ((uint32_t *)(asset + BLIT_HEADER))[i] = BlitRandomArgb(state);
    }
    for (s = 0; s < setup->subresourceCount; s++) {
        GraphicsTextureSourceEntry *entry = (GraphicsTextureSourceEntry *)(asset + tableOffset) + s;
        int direct = (BlitRandom(state) % 3) == 0;
        unsigned texels;
        entry->pixelWidth = (uint32_t)BlitRange(state, 1, 48);
        entry->pixelHeight = (uint32_t)BlitRange(state, 1, 40);
        entry->logicalWidth = entry->pixelWidth + (uint32_t)BlitRange(state, 0, 8);
        entry->logicalHeight = entry->pixelHeight + (uint32_t)BlitRange(state, 0, 8);
        entry->originX = BlitRange(state, -24, 24);
        entry->originY = BlitRange(state, -24, 24);
        entry->paletteIndex = direct ? -1 : BlitRange(state, 0, setup->bankCount - 1);
        if (!direct && (BlitRandom(state) % 16) == 0) {
            entry->paletteIndex = setup->bankCount + BlitRange(state, 0, 2); /* invalid bank */
        }
        entry->dataOffset = dataOffset;
        texels = entry->pixelWidth * entry->pixelHeight;
        if (direct) {
            for (i = 0; i < texels; i++) {
                ((uint32_t *)(asset + dataOffset))[i] = BlitRandomArgb(state);
            }
            dataOffset += texels * 4u;
        }
        else {
            BlitFill(asset + dataOffset, texels, state);
            dataOffset += (texels + 3u) & ~3u;
        }
    }
}

static unsigned BlitAssetBytes(int bankCount, int subresourceCount)
{
    return BLIT_HEADER + (unsigned)bankCount * BLIT_BANK_BYTES + (unsigned)subresourceCount * 0x20u +
           (unsigned)subresourceCount * 48u * 40u * 4u + BLIT_ASSET_SLACK;
}

/* Builds one run's inputs and arguments. Returns the argument count. */
static int BlitBuildRun(const BlitCase *testCase, unsigned seed, BlitSetup *setup, BlitBuffers *input,
                        SoftwareFramebufferAccess *framebuffer, SoftwareMaskRuntimeView *mask, uint32_t *args)
{
    unsigned rng = seed;
    int clipMinX;
    int clipMinY;
    int clipMaxX;
    int clipMaxY;
    int drawX;
    int drawY;
    unsigned subresourceIndex;
    int scale;
    int n = 0;

    memset(setup, 0, sizeof *setup);
    setup->width = BlitRange(&rng, 1, 160);
    setup->height = BlitRange(&rng, 1, 120);
    setup->pixelBytes = testCase->pixelBytes;
    setup->layout555 = (int)(BlitRandom(&rng) & 1);
    setup->bankCount = BlitRange(&rng, 1, 4);
    setup->subresourceCount = BlitRange(&rng, 1, 4);
    setup->invalid = (BlitRandom(&rng) % 24);
    if (setup->invalid > 3) {
        setup->invalid = 0;
    }
    BlitSetPixelConstants(setup->layout555);

    if (testCase->kind == BLIT_KIND_MASK) {
        /* mask: w*h >= 32 (the original loops 2^32 times for fewer than 32 pixels) */
        unsigned i;
        s_BlitMaskWidth = (uint32_t)BlitRange(&rng, 1, 200);
        s_BlitMaskHeight = (uint32_t)BlitRange(&rng, 1, 150);
        if (s_BlitMaskWidth * s_BlitMaskHeight < 32) {
            s_BlitMaskHeight = 32;
        }
        setup->maskBytes = s_BlitMaskWidth * s_BlitMaskHeight;
        BlitFill(input->mask, setup->maskBytes + 2 * BLIT_GUARD, &rng);
        for (i = 0; i < setup->maskBytes; i++) {
            uint8_t *p = input->mask + BLIT_GUARD + i;
            switch (BlitRandom(&rng) % 4) {
            case 0:
                *p = 0;
                break;
            case 1:
                *p = (uint8_t)BlitRange(&rng, 0xd8, 0xff);
                break;
            case 2:
                *p = (uint8_t)BlitRange(&rng, 1, 0x40);
                break;
            default:
                break;
            }
        }
        memset(mask, 0, sizeof *mask);
        mask->textureSource = (GraphicsTextureSourceAsset *)0x12345678;
        mask->maskPixels = (setup->invalid == 1) ? NULL : input->mask + BLIT_GUARD;
        sprintf_s(setup->describe, sizeof setup->describe, "mask %ux%u%s", s_BlitMaskWidth, s_BlitMaskHeight,
                  mask->maskPixels == NULL ? " (NULL)" : "");
        args[n++] = (uint32_t)(uintptr_t)mask;
        return n;
    }

    /* framebuffer (the stride is width * bytes per pixel) */
    setup->pixelBytesTotal = (unsigned)(setup->width * setup->height * setup->pixelBytes);
    BlitFill(input->pixels, setup->pixelBytesTotal + 2 * BLIT_GUARD, &rng);
    framebuffer->width = (GraphicsPixelDimension)setup->width;
    framebuffer->height = (GraphicsPixelDimension)setup->height;
    framebuffer->bytesPerPixel = (enum SoftwareFramebufferPixelSize)setup->pixelBytes;
    if (setup->invalid == 3) {
        framebuffer->bytesPerPixel = (enum SoftwareFramebufferPixelSize)(6 - setup->pixelBytes);
    }
    framebuffer->pixels = input->pixels + BLIT_GUARD;

    /* clip rectangle: mostly inside, sometimes beyond the framebuffer, empty or negative */
    clipMinX = BlitRange(&rng, -8, setup->width / 3);
    clipMinY = BlitRange(&rng, -8, setup->height / 3);
    clipMaxX = BlitRange(&rng, setup->width - setup->width / 3, setup->width + 8);
    clipMaxY = BlitRange(&rng, setup->height - setup->height / 3, setup->height + 8);
    if ((BlitRandom(&rng) % 16) == 0) {
        int t = clipMinX;
        clipMinX = clipMaxX;
        clipMaxX = t;
    }
    drawX = BlitRange(&rng, -60, setup->width + 20);
    drawY = BlitRange(&rng, -50, setup->height + 20);

    if (testCase->kind == BLIT_KIND_FILL) {
        int rectMinX = BlitRange(&rng, -20, setup->width - 1);
        int rectMinY = BlitRange(&rng, -20, setup->height - 1);
        int rectMaxX = rectMinX + BlitRange(&rng, -2, setup->width + 20);
        int rectMaxY = rectMinY + BlitRange(&rng, -2, setup->height + 20);
        uint32_t argb = BlitRandomArgb(&rng);
        args[n++] = (uint32_t)clipMaxY;
        args[n++] = (uint32_t)clipMaxX;
        args[n++] = (uint32_t)clipMinY;
        args[n++] = (uint32_t)clipMinX;
        args[n++] = (uint32_t)rectMaxY;
        args[n++] = (uint32_t)rectMaxX;
        args[n++] = (uint32_t)rectMinY;
        args[n++] = (uint32_t)rectMinX;
        args[n++] = argb;
        args[n++] = (uint32_t)(uintptr_t)framebuffer;
        sprintf_s(setup->describe, sizeof setup->describe,
                  "fb %dx%d %s clip %d,%d-%d,%d rect %d,%d-%d,%d argb %08x", setup->width, setup->height,
                  setup->layout555 ? "555" : "565", clipMinX, clipMinY, clipMaxX, clipMaxY, rectMinX, rectMinY,
                  rectMaxX, rectMaxY, argb);
        return n;
    }

    setup->assetBytes = BlitAssetBytes(setup->bankCount, setup->subresourceCount);
    BlitBuildAsset(setup, input->asset, &rng);
    subresourceIndex = (unsigned)BlitRange(&rng, 0, setup->subresourceCount - 1);
    if (setup->invalid == 1) {
        ((GraphicsTextureSourceAsset *)input->asset)->common.magic = (enum AssetMagic)0x12345678;
    }
    else if (setup->invalid == 2) {
        subresourceIndex = (unsigned)(setup->subresourceCount + BlitRange(&rng, 0, 2));
    }

    /* three runs in four place the image so that it overlaps the framebuffer (scaled for
       IntegerScaled: the image starts at draw + origin * scale and is size * scale large) */
    scale = testCase->extra == BLIT_EXTRA_SCALE ? BlitRange(&rng, 1, 4) : 1;
    if (subresourceIndex < (unsigned)setup->subresourceCount && (BlitRandom(&rng) % 4) != 0) {
        const GraphicsTextureSourceEntry *entry =
            (const GraphicsTextureSourceEntry *)(input->asset + BLIT_HEADER +
                                                 (unsigned)setup->bankCount * BLIT_BANK_BYTES) + subresourceIndex;
        int w = (int)entry->pixelWidth * scale;
        int h = (int)entry->pixelHeight * scale;
        drawX = BlitRange(&rng, -w + 1, setup->width - 1) - entry->originX * scale;
        drawY = BlitRange(&rng, -h + 1, setup->height - 1) - entry->originY * scale;
    }
    args[n++] = (uint32_t)clipMaxY;
    args[n++] = (uint32_t)clipMaxX;
    args[n++] = (uint32_t)clipMinY;
    args[n++] = (uint32_t)clipMinX;
    args[n++] = (uint32_t)drawY;
    args[n++] = (uint32_t)drawX;
    if (testCase->kind == BLIT_KIND_EXTRA) {
        uint32_t extra;
        if (testCase->extra == BLIT_EXTRA_SCALE) {
            extra = (uint32_t)scale;
        }
        else if (testCase->extra == BLIT_EXTRA_BANK) {
            extra = (uint32_t)BlitRange(&rng, 0, setup->bankCount - 1);
            if ((BlitRandom(&rng) % 12) == 0) {
                extra = (uint32_t)(setup->bankCount + BlitRange(&rng, 0, 3));
            }
        }
        else {
            extra = BlitRandom(&rng);
            switch (BlitRandom(&rng) % 4) {
            case 0:
                extra |= 0xff000000u;
                break;
            case 1:
                extra = 0xffffffffu;
                break;
            default:
                break;
            }
        }
        args[n++] = extra;
    }
    args[n++] = subresourceIndex;
    args[n++] = (uint32_t)(uintptr_t)input->asset;
    args[n++] = (uint32_t)(uintptr_t)framebuffer;
    {
        const GraphicsTextureSourceEntry *entry =
            (subresourceIndex < (unsigned)setup->subresourceCount)
                ? (const GraphicsTextureSourceEntry *)(input->asset + BLIT_HEADER +
                                                       (unsigned)setup->bankCount * BLIT_BANK_BYTES) + subresourceIndex
                : NULL;
        sprintf_s(setup->describe, sizeof setup->describe,
                  "fb %dx%d %s clip %d,%d-%d,%d draw %d,%d extra %08x sub %u (%dx%d at %d,%d pal %d) invalid %u",
                  setup->width, setup->height, setup->layout555 ? "555" : "565", clipMinX, clipMinY, clipMaxX,
                  clipMaxY, (int)args[5], (int)args[4], testCase->kind == BLIT_KIND_EXTRA ? args[6] : 0,
                  subresourceIndex, entry ? (int)entry->pixelWidth : 0, entry ? (int)entry->pixelHeight : 0,
                  entry ? entry->originX : 0, entry ? entry->originY : 0, entry ? entry->paletteIndex : 0,
                  setup->invalid);
    }
    return n;
}

void Thandor_SelfTestBlitCompare(void)
{
    const char *runsText = getenv("OPEN_THANDOR_BLITCMP_RUNS");
    const char *seedText = getenv("OPEN_THANDOR_BLITCMP_SEED");
    const char *filter = getenv("OPEN_THANDOR_BLITCMP_FILTER");
    int runs = (runsText != NULL && runsText[0] != 0) ? atoi(runsText) : 500;
    unsigned baseSeed = (seedText != NULL && seedText[0] != 0) ? (unsigned)strtoul(seedText, NULL, 0) : 1u;
    uint8_t *blitCode = (uint8_t *)Thandor_LoadOriginalCodeCopy(BLIT_CODE_START, BLIT_CODE_END - BLIT_CODE_START);
    uint8_t *maskCode = (uint8_t *)Thandor_LoadOriginalCodeCopy(MASK_CODE_START, MASK_CODE_END - MASK_CODE_START);
    SoftwarePixelMmxConstants savedConstants = g_SoftwarePixelMmxConstants;
    SoftwarePixelPackTables *savedPackTables = g_SoftwarePixelPackTables;
    GraphicsTextureSourceGetLogicalSizeProc *savedGetSize = g_GraphicsTextureSourceGetLogicalSize;
    SoftwarePixelPackTables *packTables = (SoftwarePixelPackTables *)malloc(sizeof(SoftwarePixelPackTables));
    const unsigned maxPixels = 160u * 120u * 4u + 2u * BLIT_GUARD;
    const unsigned maxAsset = BlitAssetBytes(4, 4);
    const unsigned maxMask = 200u * 150u + 2u * BLIT_GUARD;
    BlitBuffers input;
    BlitBuffers mine;
    BlitBuffers theirs;
    SoftwareFramebufferAccess framebuffer;
    SoftwareMaskRuntimeView maskRuntime;
    int caseIndex;
    int casesRun = 0;
    int casesFailed = 0;

    if (blitCode == NULL || maskCode == NULL) {
        Thandor_Log("blitcmp: could not load the original code (thandor_original.exe missing?)");
        return;
    }
    input.pixels = (uint8_t *)malloc(maxPixels);
    input.asset = (uint8_t *)malloc(maxAsset);
    input.mask = (uint8_t *)malloc(maxMask);
    mine.pixels = (uint8_t *)malloc(maxPixels);
    mine.asset = (uint8_t *)malloc(maxAsset);
    mine.mask = (uint8_t *)malloc(maxMask);
    theirs.pixels = (uint8_t *)malloc(maxPixels);
    theirs.asset = (uint8_t *)malloc(maxAsset);
    theirs.mask = (uint8_t *)malloc(maxMask);
    if (!packTables || !input.pixels || !input.asset || !input.mask || !mine.pixels || !mine.asset || !mine.mask ||
        !theirs.pixels || !theirs.asset || !theirs.mask) {
        Thandor_Log("blitcmp: allocation failed");
        return;
    }
    {
        unsigned packState = 0x9e3779b9u ^ baseSeed;
        BlitFill((uint8_t *)packTables, sizeof *packTables, &packState);
    }
    g_SoftwarePixelPackTables = packTables;
    Thandor_Log("blitcmp: %d runs per function, seed %u, filter \"%s\"", runs, baseSeed, filter ? filter : "");

    for (caseIndex = 0; caseIndex < (int)(sizeof s_BlitCases / sizeof s_BlitCases[0]); caseIndex++) {
        const BlitCase *testCase = &s_BlitCases[caseIndex];
        void *originalEntry = testCase->kind == BLIT_KIND_MASK
                                  ? (void *)(maskCode + (testCase->originalAddress - MASK_CODE_START))
                                  : (void *)(blitCode + (testCase->originalAddress - BLIT_CODE_START));
        int run;
        int mismatches = 0;
        int faults = 0;
        int drew = 0;
        unsigned unitsWritten = 0;
        if (filter != NULL && filter[0] != 0 && strstr(testCase->name, filter) == NULL) {
            continue;
        }
        casesRun++;
        for (run = 0; run < runs; run++) {
            unsigned seed = (baseSeed * 2654435761u) ^ ((unsigned)caseIndex * 0x9e3779b9u) ^
                            ((unsigned)run * 0x85ebca6bu) ^ 0x7654321u;
            BlitSetup setup;
            uint32_t args[10];
            int count;
            unsigned mineCarry = 0;
            unsigned theirsCarry = 0;
            unsigned mineFault = 0;
            unsigned theirsFault = 0;
            int mineFaulted;
            int theirsFaulted;
            const char *where = NULL;
            int difference = -1;
            const uint8_t *a = NULL;
            const uint8_t *b = NULL;
            if (seed == 0) {
                seed = 1;
            }
            count = BlitBuildRun(testCase, seed, &setup, &input, &framebuffer, &maskRuntime, args);

            /* C version on "mine" */
            memcpy(mine.pixels, input.pixels, setup.pixelBytesTotal + 2 * BLIT_GUARD);
            memcpy(mine.asset, input.asset, setup.assetBytes);
            memcpy(mine.mask, input.mask, setup.maskBytes + 2 * BLIT_GUARD);
            framebuffer.pixels = mine.pixels + BLIT_GUARD;
            if (testCase->kind == BLIT_KIND_PLAIN || testCase->kind == BLIT_KIND_EXTRA) {
                args[count - 2] = (uint32_t)(uintptr_t)mine.asset; /* asset is the second-to-last argument */
            }
            if (testCase->kind == BLIT_KIND_MASK && maskRuntime.maskPixels != NULL) {
                maskRuntime.maskPixels = mine.mask + BLIT_GUARD;
            }
            g_GraphicsTextureSourceGetLogicalSize = BlitMaskSizeForC;
            mineFaulted = BlitRunGuarded(testCase, NULL, args, count, &mineCarry, &mineFault);

            /* original on "theirs" */
            memcpy(theirs.pixels, input.pixels, setup.pixelBytesTotal + 2 * BLIT_GUARD);
            memcpy(theirs.asset, input.asset, setup.assetBytes);
            memcpy(theirs.mask, input.mask, setup.maskBytes + 2 * BLIT_GUARD);
            framebuffer.pixels = theirs.pixels + BLIT_GUARD;
            if (testCase->kind == BLIT_KIND_PLAIN || testCase->kind == BLIT_KIND_EXTRA) {
                args[count - 2] = (uint32_t)(uintptr_t)theirs.asset;
            }
            if (testCase->kind == BLIT_KIND_MASK && maskRuntime.maskPixels != NULL) {
                maskRuntime.maskPixels = theirs.mask + BLIT_GUARD;
            }
            g_GraphicsTextureSourceGetLogicalSize = (GraphicsTextureSourceGetLogicalSizeProc *)BlitMaskSizeForOriginal;
            theirsFaulted = BlitRunGuarded(testCase, originalEntry, args, count, &theirsCarry, &theirsFault);

            if (mineFaulted || theirsFaulted) {
                faults++;
                if (faults <= 3) {
                    Thandor_Log("blitcmp %s run %d: FAULT mine %08x theirs %08x (%s)", testCase->name, run,
                                mineFaulted ? mineFault : 0, theirsFaulted ? theirsFault : 0, setup.describe);
                }
                continue;
            }
            if (testCase->kind == BLIT_KIND_MASK) {
                unsigned changed = 0;
                unsigned i;
                for (i = 0; i < setup.maskBytes; i++) {
                    changed += theirs.mask[BLIT_GUARD + i] != input.mask[BLIT_GUARD + i];
                }
                unitsWritten += changed;
                drew += changed != 0;
            }
            else {
                unsigned changed = 0;
                unsigned i;
                for (i = 0; i < setup.pixelBytesTotal; i += (unsigned)testCase->pixelBytes) {
                    changed += memcmp(theirs.pixels + BLIT_GUARD + i, input.pixels + BLIT_GUARD + i,
                                      (size_t)testCase->pixelBytes) != 0;
                }
                unitsWritten += changed;
                drew += changed != 0;
            }
            if ((difference = BlitFirstDifference(mine.pixels, theirs.pixels, setup.pixelBytesTotal + 2 * BLIT_GUARD)) >= 0) {
                where = "framebuffer";
                a = mine.pixels;
                b = theirs.pixels;
            }
            else if ((difference = BlitFirstDifference(mine.mask, theirs.mask, setup.maskBytes + 2 * BLIT_GUARD)) >= 0) {
                where = "mask";
                a = mine.mask;
                b = theirs.mask;
            }
            else if ((difference = BlitFirstDifference(mine.asset, theirs.asset, setup.assetBytes)) >= 0) {
                where = "asset";
                a = mine.asset;
                b = theirs.asset;
            }
            else if (mineCarry != theirsCarry) {
                where = "carry flag";
            }
            if (where != NULL) {
                mismatches++;
                if (mismatches <= 3) {
                    int inside = difference - (int)BLIT_GUARD;
                    int unit = testCase->kind == BLIT_KIND_MASK ? 1 : testCase->pixelBytes;
                    int stride = testCase->kind == BLIT_KIND_MASK ? (int)s_BlitMaskWidth : setup.width * unit;
                    if (a != NULL && where[0] != 'a') {
                        Thandor_Log("blitcmp %s run %d: MISMATCH %s byte %d (pixel %d,%d): mine %02x theirs %02x (%s)",
                                    testCase->name, run, where, inside,
                                    inside >= 0 ? (inside % stride) / unit : -1, inside >= 0 ? inside / stride : -1,
                                    a[difference], b[difference], setup.describe);
                    }
                    else if (a != NULL) {
                        Thandor_Log("blitcmp %s run %d: MISMATCH asset byte %d: mine %02x theirs %02x (%s)",
                                    testCase->name, run, difference, a[difference], b[difference], setup.describe);
                    }
                    else {
                        Thandor_Log("blitcmp %s run %d: MISMATCH carry mine %u theirs %u (%s)", testCase->name, run,
                                    mineCarry, theirsCarry, setup.describe);
                    }
                }
            }
        }
        if (mismatches > 0 || faults > 0) {
            casesFailed++;
        }
        Thandor_Log("blitcmp %s: %s (%d runs, %d drew, %d mismatches, %d faults; original changed %u %s)",
                    testCase->name, (mismatches || faults) ? "DIFFERS" : "identical", runs, drew, mismatches, faults,
                    unitsWritten, testCase->kind == BLIT_KIND_MASK ? "mask bytes" : "pixels");
    }

    g_SoftwarePixelMmxConstants = savedConstants;
    g_SoftwarePixelPackTables = savedPackTables;
    g_GraphicsTextureSourceGetLogicalSize = savedGetSize;
    free(packTables);
    free(input.pixels);
    free(input.asset);
    free(input.mask);
    free(mine.pixels);
    free(mine.asset);
    free(mine.mask);
    free(theirs.pixels);
    free(theirs.asset);
    free(theirs.mask);
    Thandor_Log("blitcmp: %d functions, %d with mismatches/faults", casesRun, casesFailed);
}

#endif
