/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/selftest_blendscale.c
 */

/*
OPEN_THANDOR_SELFTEST=blendscalecmp: differential test of SoftwareTexture_BilinearBlendScaleSubresources
(graphics/backend/software.c) against a copy of the original machine code (0x518CE0..0x51910C).

Each run builds a random texture source asset with 2..5 paletted 8-bit entries, a random blend
factor image, a random pixel format (565, 555, 888 or arbitrary bit counts and shifts), a 16-bit
or 32-bit framebuffer with random padding and position, and random destination sizes. Some runs
use an invalid asset, index or direct-colour entry to exercise the early returns. Both versions
run on identical copies; the blended image, the framebuffer (with guard bytes) and
g_SoftwarePixelIntensityToNativeColorLut256 are compared. Sizes that make the original loop 2^32
times or divide by zero (fewer than 8 source pixels, destination size 0 or 1) are not generated.

Needs the mapped-image build (-DTHANDOR_MAPPED_IMAGE=ON) and thandor_original.exe.
Environment: OPEN_THANDOR_BLENDSCALECMP_RUNS (default 500), OPEN_THANDOR_BLENDSCALECMP_SEED (default 1).
Log lines start with "blendscalecmp".
*/

#include <stdlib.h>
#include <string.h>
#include <excpt.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

void Thandor_SelfTestBlendScaleCompare(void);

#ifndef THANDOR_MAPPED_IMAGE

void Thandor_SelfTestBlendScaleCompare(void)
{
    Thandor_Log("blendscalecmp: needs a build with -DTHANDOR_MAPPED_IMAGE=ON");
}

#else

#define BLENDSCALE_CODE_START 0x518ce0u
#define BLENDSCALE_CODE_END 0x51910cu
#define BLENDSCALE_GUARD 64u
#define BLENDSCALE_HEADER 0x200u

typedef void (__stdcall *BlendScaleProc)(dword destinationHeight, dword destinationWidth, int destinationTop,
                                         int destinationLeft, qword *blended, qword *factor, dword indexA,
                                         dword indexB, int *asset, int *framebuffer);

static unsigned BlendScaleRandom(unsigned *state)
{
    unsigned x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static unsigned BlendScaleRange(unsigned *state, unsigned low, unsigned high)
{
    return low + BlendScaleRandom(state) % (high - low + 1);
}

static void BlendScaleFill(byte *data, unsigned size, unsigned *state)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        data[i] = (byte)(BlendScaleRandom(state) >> 11);
    }
}

static int BlendScaleFirstDifference(const byte *a, const byte *b, unsigned size)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return (int)i;
        }
    }
    return -1;
}

static void BlendScaleSetFormat(unsigned *state)
{
    SoftwarePixelFormatConfig *format = &g_SoftwarePixelFormatConfig;
    switch (BlendScaleRandom(state) % 4) {
    case 0:
        format->redBitCount = 5; format->greenBitCount = 6; format->blueBitCount = 5;
        format->redShift = 11; format->greenShift = 5; format->blueShift = 0;
        break;
    case 1:
        format->redBitCount = 5; format->greenBitCount = 5; format->blueBitCount = 5;
        format->redShift = 10; format->greenShift = 5; format->blueShift = 0;
        break;
    case 2:
        format->redBitCount = 8; format->greenBitCount = 8; format->blueBitCount = 8;
        format->redShift = 16; format->greenShift = 8; format->blueShift = 0;
        break;
    default:
        format->redBitCount = BlendScaleRange(state, 0, 8);
        format->greenBitCount = BlendScaleRange(state, 0, 8);
        format->blueBitCount = BlendScaleRange(state, 0, 8);
        format->redShift = BlendScaleRange(state, 0, 24);
        format->greenShift = BlendScaleRange(state, 0, 24);
        format->blueShift = BlendScaleRange(state, 0, 24);
        break;
    }
}

/* Returns nonzero when the original faulted. */
static int BlendScaleCallOriginal(BlendScaleProc original, dword height, dword width, int top, int left,
                                  qword *blended, qword *factor, dword indexA, dword indexB, int *asset,
                                  int *framebuffer, unsigned *exceptionCode)
{
    __try {
        original(height, width, top, left, blended, factor, indexA, indexB, asset, framebuffer);
        __asm emms
    }
    __except (*exceptionCode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
        __asm emms
        return 1;
    }
    return 0;
}

void Thandor_SelfTestBlendScaleCompare(void)
{
    const char *runsText = getenv("OPEN_THANDOR_BLENDSCALECMP_RUNS");
    const char *seedText = getenv("OPEN_THANDOR_BLENDSCALECMP_SEED");
    int runs = runsText != NULL ? atoi(runsText) : 500;
    unsigned baseSeed = seedText != NULL ? (unsigned)atoi(seedText) : 1u;
    BlendScaleProc original =
        (BlendScaleProc)Thandor_LoadOriginalCodeCopy(BLENDSCALE_CODE_START, BLENDSCALE_CODE_END - BLENDSCALE_CODE_START);
    SoftwarePixelFormatConfig savedFormat = g_SoftwarePixelFormatConfig;
    dword savedLut[256];
    int run;
    int failures = 0;
    int drawn = 0;

    if (original == NULL) {
        Thandor_Log("blendscalecmp: could not load the original code");
        return;
    }
    memcpy(savedLut, g_SoftwarePixelIntensityToNativeColorLut256, sizeof savedLut);
    Thandor_Log("blendscalecmp: %d runs, seed %u", runs, baseSeed);
    for (run = 0; run < runs; run++) {
        unsigned state = (baseSeed * 2654435761u) ^ (unsigned)(run * 40503 + 1);
        dword sourceWidth;
        dword sourceHeight;
        dword pixels;
        dword entryCount;
        dword entryBytes;
        dword assetBytes;
        dword indexA;
        dword indexB;
        dword destinationWidth;
        dword destinationHeight;
        dword pixelBytes;
        dword pitch;
        int top;
        int left;
        dword framebufferBytes;
        dword blendedBytes;
        byte *asset;
        byte *factor;
        byte *blendedMine;
        byte *blendedTheirs;
        byte *pixelsMine;
        byte *pixelsTheirs;
        dword lutBefore[256];
        dword lutMine[256];
        SoftwareFramebufferAccess framebufferMine;
        SoftwareFramebufferAccess framebufferTheirs;
        unsigned exceptionCode = 0;
        unsigned invalid;
        dword i;
        int difference;
        const char *where = NULL;

        while (BlendScaleRandom(&state) == 0) {
        }
        sourceWidth = BlendScaleRange(&state, 1, 200);
        sourceHeight = BlendScaleRange(&state, 1, 160);
        if (sourceWidth * sourceHeight < 8) {
            sourceHeight = 8;
        }
        pixels = sourceWidth * sourceHeight;
        entryCount = BlendScaleRange(&state, 2, 5);
        entryBytes = (pixels + 15) & ~7u;
        assetBytes = BLENDSCALE_HEADER + entryCount * 0x20 + entryCount * entryBytes;
        indexA = BlendScaleRange(&state, 0, entryCount - 1);
        indexB = BlendScaleRange(&state, 0, entryCount - 1);
        destinationWidth = BlendScaleRange(&state, 2, 320);
        destinationHeight = BlendScaleRange(&state, 2, 240);
        pixelBytes = (run & 1) ? 4 : 2;
        pitch = destinationWidth + BlendScaleRange(&state, 0, 16);
        top = (int)BlendScaleRange(&state, 0, 8);
        left = (int)BlendScaleRange(&state, 0, pitch - destinationWidth);
        framebufferBytes = (dword)(top + destinationHeight) * pitch * pixelBytes + 2 * BLENDSCALE_GUARD;
        blendedBytes = sourceWidth * (sourceHeight + 1) + 16;

        asset = (byte *)calloc(1, assetBytes);
        factor = (byte *)malloc(entryBytes);
        blendedMine = (byte *)malloc(blendedBytes);
        blendedTheirs = (byte *)malloc(blendedBytes);
        pixelsMine = (byte *)malloc(framebufferBytes);
        pixelsTheirs = (byte *)malloc(framebufferBytes);
        if (!asset || !factor || !blendedMine || !blendedTheirs || !pixelsMine || !pixelsTheirs) {
            Thandor_Log("blendscalecmp: allocation failed");
            return;
        }

        /* texture source: header, entry table at 0x200, 8-bit texels */
        ((GraphicsTextureSourceAsset *)asset)->common.magic = ASSET_MAGIC_GFX;
        ((GraphicsTextureSourceAsset *)asset)->tableDescriptor.subresourceCount = entryCount;
        ((GraphicsTextureSourceAsset *)asset)->tableDescriptor.subresourceTableOffset = BLENDSCALE_HEADER;
        for (i = 0; i < entryCount; i++) {
            GraphicsTextureSourceEntry *entry =
                (GraphicsTextureSourceEntry *)(asset + BLENDSCALE_HEADER) + i;
            entry->paletteIndex = (int)BlendScaleRange(&state, 0, 3);
            entry->dataOffset = BLENDSCALE_HEADER + entryCount * 0x20 + i * entryBytes;
            entry->pixelWidth = sourceWidth;
            entry->pixelHeight = sourceHeight;
            BlendScaleFill(asset + entry->dataOffset, entryBytes, &state);
        }
        /* one run in eight takes an early return */
        invalid = BlendScaleRandom(&state) % 32;
        if (invalid == 0) {
            ((GraphicsTextureSourceAsset *)asset)->common.magic = (enum AssetMagic)0x12345678;
        }
        else if (invalid == 1) {
            indexA = entryCount + BlendScaleRange(&state, 0, 3);
        }
        else if (invalid == 2) {
            indexB = entryCount;
        }
        else if (invalid == 3) {
            ((GraphicsTextureSourceEntry *)(asset + BLENDSCALE_HEADER) + ((run & 2) ? indexA : indexB))->paletteIndex = -1;
        }
        BlendScaleFill(factor, entryBytes, &state);
        BlendScaleFill(blendedMine, blendedBytes, &state);
        memcpy(blendedTheirs, blendedMine, blendedBytes);
        BlendScaleFill(pixelsMine, framebufferBytes, &state);
        memcpy(pixelsTheirs, pixelsMine, framebufferBytes);
        BlendScaleFill((byte *)lutBefore, sizeof lutBefore, &state);
        BlendScaleSetFormat(&state);

        framebufferMine.width = pitch;
        framebufferMine.height = (dword)top + destinationHeight;
        framebufferMine.bytesPerPixel = (enum SoftwareFramebufferPixelSize)pixelBytes;
        framebufferMine.pixels = pixelsMine + BLENDSCALE_GUARD;
        framebufferTheirs = framebufferMine;
        framebufferTheirs.pixels = pixelsTheirs + BLENDSCALE_GUARD;

        memcpy(g_SoftwarePixelIntensityToNativeColorLut256, lutBefore, sizeof lutBefore);
        SoftwareTexture_BilinearBlendScaleSubresources(destinationHeight, destinationWidth, top, left,
                                                       (qword *)blendedMine, (qword *)factor, indexA, indexB,
                                                       (int *)asset, (int *)&framebufferMine);
        memcpy(lutMine, g_SoftwarePixelIntensityToNativeColorLut256, sizeof lutMine);
        memcpy(g_SoftwarePixelIntensityToNativeColorLut256, lutBefore, sizeof lutBefore);
        if (BlendScaleCallOriginal(original, destinationHeight, destinationWidth, top, left, (qword *)blendedTheirs,
                                   (qword *)factor, indexA, indexB, (int *)asset, (int *)&framebufferTheirs,
                                   &exceptionCode)) {
            failures++;
            Thandor_Log("blendscalecmp run %d: original faulted (exception %08x)", run, exceptionCode);
        }
        else {
            if (memcmp(lutBefore, g_SoftwarePixelIntensityToNativeColorLut256, sizeof lutBefore) != 0) {
                drawn++;
            }
            difference = BlendScaleFirstDifference(blendedMine, blendedTheirs, blendedBytes);
            if (difference >= 0) {
                where = "blended image";
            }
            else if ((difference = BlendScaleFirstDifference(pixelsMine, pixelsTheirs, framebufferBytes)) >= 0) {
                where = "framebuffer";
            }
            else if ((difference = BlendScaleFirstDifference((const byte *)lutMine,
                                                             (const byte *)g_SoftwarePixelIntensityToNativeColorLut256,
                                                             sizeof lutMine)) >= 0) {
                where = "intensity table";
            }
            if (where != NULL) {
                failures++;
                Thandor_Log("blendscalecmp run %d: MISMATCH in %s at byte %d (%ux%u -> %ux%u at %d,%d, %u bpp, "
                            "A %u B %u, invalid %u)",
                            run, where, difference, sourceWidth, sourceHeight, destinationWidth, destinationHeight,
                            left, top, pixelBytes * 8, indexA, indexB, invalid < 4 ? invalid + 1 : 0);
            }
        }
        free(asset);
        free(factor);
        free(blendedMine);
        free(blendedTheirs);
        free(pixelsMine);
        free(pixelsTheirs);
    }
    g_SoftwarePixelFormatConfig = savedFormat;
    memcpy(g_SoftwarePixelIntensityToNativeColorLut256, savedLut, sizeof savedLut);
    Thandor_Log("blendscalecmp: %d runs, %d drew, %d differ", runs, drawn, failures);
}

#endif
