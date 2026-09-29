/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/raster.c
 */

/*
OPEN_THANDOR_SELFTEST=rastercmp: differential test of the software triangle rasterizer.

Every SoftwareRaster{16,Non16,Aux}_ModeNN handler (graphics/backend/software.c) runs next to a copy
of the ORIGINAL machine code of the same handler on identical random input, and the memory the
handler writes is compared byte by byte:
  - the colour target (16-bit or 32-bit framebuffer, or the auxiliary 32-bit target),
  - the depth buffer,
  - guard bytes around both buffers (out-of-bounds writes),
  - g_SoftwareRasterScanState (reported separately as "state": a rewrite may keep the scan state
    in locals, so a state-only difference is not an output error).
The original handlers address globals absolutely, so this needs the mapped-image build
(-DTHANDOR_MAPPED_IMAGE=ON) and thandor_original.exe next to the executable.

Inputs per run (deterministic from the case and run number):
  - viewport size and clip rectangle (the Aux family always gets clipMin = 0 and uses
    clipMaxX * 4 as the row stride of its target and depth buffer, like DrawQueueAuxiliary),
  - framebuffer / depth row strides with random padding, random framebuffer and depth contents,
  - three random vertices (partly off screen), prepared by SoftwareRenderer_PrepareTrianglePacket
    exactly as the draw queue does (sorted by Y, snapped to pixels, depth epoch added),
  - 565 or 555 pixel constants (g_SoftwarePixelMmxConstants),
  - for textured modes (16..30) a texture of 1..256 x 1..256 texels, paletted (bank 0..3) or
    direct 32-bit, with random texels and palettes,
  - random scan state (the lanes the game keeps at zero stay zero).
Triangles whose area makes the original IDIV fault are skipped.

Environment:
  OPEN_THANDOR_RASTERCMP_RUNS    runs per handler (default 300)
  OPEN_THANDOR_RASTERCMP_FILTER  only handlers whose name contains this text (e.g. "Raster16_Mode0")
  OPEN_THANDOR_RASTERCMP_SEED    base seed (default 1)
  OPEN_THANDOR_RASTERCMP_STATE   set: log where the scan state differs (first two runs per handler)
Mode04/06/12/14 (all families) read MM2 without loading it; see RasterReadsStaleMm2.
Log lines start with "rastercmp" (thandor.log next to the executable).
*/

#include <stdlib.h>
#include <string.h>
#include <excpt.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

void Thandor_SelfTestRasterCompare(void);

#ifndef THANDOR_MAPPED_IMAGE

void Thandor_SelfTestRasterCompare(void)
{
    Thandor_Log("rastercmp: needs a build with -DTHANDOR_MAPPED_IMAGE=ON");
}

#else

typedef void (*RasterCompareProc)(GraphicsScreenCoordinate clipMaxY, GraphicsScreenCoordinate clipMaxX,
                                  GraphicsScreenCoordinate clipMinY, GraphicsScreenCoordinate clipMinX,
                                  GraphicsPrimitivePacket *packet);

typedef struct RasterCompareCase {
    const char *name;
    int family; /* 0 = 16-bit framebuffer, 1 = 32-bit framebuffer, 2 = auxiliary 32-bit target */
    int mode;   /* (renderFlags & 0x3f000) >> 12 */
    unsigned originalAddress;
    RasterCompareProc function;
} RasterCompareCase;

#define RASTER_CASE(family, name, mode, address, function) {name, family, mode, address, function}

static const RasterCompareCase s_RasterCases[] = {
    RASTER_CASE(0, "SoftwareRaster16_Mode00", 0, 0x4dce90, SoftwareRaster16_Mode00),
    RASTER_CASE(0, "SoftwareRaster16_Mode01", 1, 0x4de0c0, SoftwareRaster16_Mode01),
    RASTER_CASE(0, "SoftwareRaster16_Mode02", 2, 0x4dea40, SoftwareRaster16_Mode02),
    RASTER_CASE(0, "SoftwareRaster16_Mode04", 4, 0x4df2f0, SoftwareRaster16_Mode04),
    RASTER_CASE(0, "SoftwareRaster16_Mode06", 6, 0x4dd700, SoftwareRaster16_Mode06),
    RASTER_CASE(0, "SoftwareRaster16_Mode08", 8, 0x4dfcb0, SoftwareRaster16_Mode08),
    RASTER_CASE(0, "SoftwareRaster16_Mode09", 9, 0x4e0940, SoftwareRaster16_Mode09),
    RASTER_CASE(0, "SoftwareRaster16_Mode10", 10, 0x4e0ff0, SoftwareRaster16_Mode10),
    RASTER_CASE(0, "SoftwareRaster16_Mode12", 12, 0x4e15d0, SoftwareRaster16_Mode12),
    RASTER_CASE(0, "SoftwareRaster16_Mode14", 14, 0x4e0250, SoftwareRaster16_Mode14),
    RASTER_CASE(0, "SoftwareRaster16_Mode16", 16, 0x4d1710, SoftwareRaster16_Mode16),
    RASTER_CASE(0, "SoftwareRaster16_Mode17", 17, 0x4d3e90, SoftwareRaster16_Mode17),
    RASTER_CASE(0, "SoftwareRaster16_Mode18", 18, 0x4d5310, SoftwareRaster16_Mode18),
    RASTER_CASE(0, "SoftwareRaster16_Mode20", 20, 0x4d6690, SoftwareRaster16_Mode20),
    RASTER_CASE(0, "SoftwareRaster16_Mode22", 22, 0x4d2990, SoftwareRaster16_Mode22),
    RASTER_CASE(0, "SoftwareRaster16_Mode24", 24, 0x4d7b90, SoftwareRaster16_Mode24),
    RASTER_CASE(0, "SoftwareRaster16_Mode25", 25, 0x4d9c10, SoftwareRaster16_Mode25),
    RASTER_CASE(0, "SoftwareRaster16_Mode26", 26, 0x4dad10, SoftwareRaster16_Mode26),
    RASTER_CASE(0, "SoftwareRaster16_Mode28", 28, 0x4dbd10, SoftwareRaster16_Mode28),
    RASTER_CASE(0, "SoftwareRaster16_Mode30", 30, 0x4d8a90, SoftwareRaster16_Mode30),

    RASTER_CASE(1, "SoftwareRasterNon16_Mode00", 0, 0x4ec3e0, SoftwareRasterNon16_Mode00),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode01", 1, 0x4ed440, SoftwareRasterNon16_Mode01),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode02", 2, 0x4edcb0, SoftwareRasterNon16_Mode02),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode04", 4, 0x4ee4a0, SoftwareRasterNon16_Mode04),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode06", 6, 0x4ecb90, SoftwareRasterNon16_Mode06),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode08", 8, 0x4eed50, SoftwareRasterNon16_Mode08),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode09", 9, 0x4ef810, SoftwareRasterNon16_Mode09),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode10", 10, 0x4efdb0, SoftwareRasterNon16_Mode10),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode12", 12, 0x4f02d0, SoftwareRasterNon16_Mode12),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode14", 14, 0x4ef230, SoftwareRasterNon16_Mode14),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode16", 16, 0x4e1cc0, SoftwareRasterNon16_Mode16),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode17", 17, 0x4e4180, SoftwareRasterNon16_Mode17),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode18", 18, 0x4e5440, SoftwareRasterNon16_Mode18),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode20", 20, 0x4e65c0, SoftwareRasterNon16_Mode20),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode22", 22, 0x4e2e00, SoftwareRasterNon16_Mode22),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode24", 24, 0x4e7940, SoftwareRasterNon16_Mode24),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode25", 25, 0x4e96d0, SoftwareRasterNon16_Mode25),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode26", 26, 0x4ea610, SoftwareRasterNon16_Mode26),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode28", 28, 0x4eb3e0, SoftwareRasterNon16_Mode28),
    RASTER_CASE(1, "SoftwareRasterNon16_Mode30", 30, 0x4e86d0, SoftwareRasterNon16_Mode30),

    RASTER_CASE(2, "SoftwareRasterAux_Mode00", 0, 0x4fa640, SoftwareRasterAux_Mode00),
    RASTER_CASE(2, "SoftwareRasterAux_Mode01", 1, 0x4fb5a0, SoftwareRasterAux_Mode01),
    RASTER_CASE(2, "SoftwareRasterAux_Mode02", 2, 0x4fbd70, SoftwareRasterAux_Mode02),
    RASTER_CASE(2, "SoftwareRasterAux_Mode04", 4, 0x4fc540, SoftwareRasterAux_Mode04),
    RASTER_CASE(2, "SoftwareRasterAux_Mode06", 6, 0x4fadd0, SoftwareRasterAux_Mode06),
    RASTER_CASE(2, "SoftwareRasterAux_Mode08", 8, 0x4fcd10, SoftwareRasterAux_Mode08),
    RASTER_CASE(2, "SoftwareRasterAux_Mode09", 9, 0x4fd6f0, SoftwareRasterAux_Mode09),
    RASTER_CASE(2, "SoftwareRasterAux_Mode10", 10, 0x4fdc00, SoftwareRasterAux_Mode10),
    RASTER_CASE(2, "SoftwareRasterAux_Mode12", 12, 0x4fe110, SoftwareRasterAux_Mode12),
    RASTER_CASE(2, "SoftwareRasterAux_Mode14", 14, 0x4fd1e0, SoftwareRasterAux_Mode14),
    RASTER_CASE(2, "SoftwareRasterAux_Mode16", 16, 0x4f08b0, SoftwareRasterAux_Mode16),
    RASTER_CASE(2, "SoftwareRasterAux_Mode17", 17, 0x4f2bd0, SoftwareRasterAux_Mode17),
    RASTER_CASE(2, "SoftwareRasterAux_Mode18", 18, 0x4f3d40, SoftwareRasterAux_Mode18),
    RASTER_CASE(2, "SoftwareRasterAux_Mode20", 20, 0x4f4eb0, SoftwareRasterAux_Mode20),
    RASTER_CASE(2, "SoftwareRasterAux_Mode22", 22, 0x4f19e0, SoftwareRasterAux_Mode22),
    RASTER_CASE(2, "SoftwareRasterAux_Mode24", 24, 0x4f60a0, SoftwareRasterAux_Mode24),
    RASTER_CASE(2, "SoftwareRasterAux_Mode25", 25, 0x4f7c70, SoftwareRasterAux_Mode25),
    RASTER_CASE(2, "SoftwareRasterAux_Mode26", 26, 0x4f8a30, SoftwareRasterAux_Mode26),
    RASTER_CASE(2, "SoftwareRasterAux_Mode28", 28, 0x4f97f0, SoftwareRasterAux_Mode28),
    RASTER_CASE(2, "SoftwareRasterAux_Mode30", 30, 0x4f6e20, SoftwareRasterAux_Mode30),
};

/* All 60 handlers lie in one block of the original image, 0x004D1710 .. 0x004FE61C; they use
   only absolute data addresses and relative jumps inside the block, so one copy serves all. */
#define RASTER_CODE_START 0x4d1710u
#define RASTER_CODE_END 0x4fe620u

#define RASTER_GUARD_BYTES 256u
#define RASTER_GUARD_VALUE 0xa5
#define RASTER_TEXTURE_HEADER 0x2200u /* 0x200 header + 4 palette banks of 256 * 8 bytes */
#define RASTER_SCAN_STATE_BYTES ((unsigned)sizeof(SoftwareRasterScanState))

static unsigned RasterRandom(unsigned *state)
{
    unsigned x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static int RasterRandomRange(unsigned *state, int low, int high)
{
    return low + (int)(RasterRandom(state) % (unsigned)(high - low + 1));
}

static void RasterFillRandom(uint8_t *data, unsigned size, unsigned *state)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        data[i] = (uint8_t)(RasterRandom(state) >> 11);
    }
}

/* The original handlers are __stdcall (ret 0x14) and clobber EBX, ESI and EDI. staleMm2 is loaded
   into MM2: Mode04/06/12/14 test MM2 without ever loading it (see RasterReadsStaleMm2). */
static void RasterCallOriginal(void *entry, int clipMaxY, int clipMaxX, int clipMinY, int clipMinX, void *packet,
                               unsigned staleMm2)
{
    __asm {
        mov eax, staleMm2
        movd mm2, eax
        push ebx
        push esi
        push edi
        push packet
        push clipMinX
        push clipMinY
        push clipMaxX
        push clipMaxY
        mov eax, entry
        call eax
        pop edi
        pop esi
        pop ebx
        emms
    }
}

/* Returns nonzero when the handler faulted. */
static int RasterRunGuarded(const RasterCompareCase *testCase, void *originalEntry, int clipMaxY, int clipMaxX,
                            int clipMinY, int clipMinX, GraphicsPrimitivePacket *packet, unsigned staleMm2,
                            unsigned *exceptionCode)
{
    __try {
        if (originalEntry != NULL) {
            RasterCallOriginal(originalEntry, clipMaxY, clipMaxX, clipMinY, clipMinX, packet, staleMm2);
        }
        else {
            testCase->function(clipMaxY, clipMaxX, clipMinY, clipMinX, packet);
            __asm emms
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        *exceptionCode = GetExceptionCode();
        __asm emms
        return 1;
    }
    return 0;
}

typedef struct RasterBuffers {
    uint8_t *color;      /* includes RASTER_GUARD_BYTES before and after */
    uint8_t *depth;
    uint8_t *texture;    /* texture asset: header, palettes, texels */
    GraphicsPrimitivePacket packet;
    uint8_t state[sizeof(SoftwareRasterScanState)];
} RasterBuffers;

typedef struct RasterRunSetup {
    int width;
    int height;
    int clipMinX;
    int clipMinY;
    int clipMaxX;
    int clipMaxY;
    int bytesPerPixel;
    unsigned colorStride;
    unsigned depthStride;
    unsigned colorBytes;
    unsigned depthBytes;
    unsigned textureBytes;
    int layout555;
    int widthLog2;
    int heightLog2;
    int paletteIndex;
} RasterRunSetup;

static void RasterSetPixelConstants(int layout555)
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

/* Lanes the game never writes and which stay zero (they are part of 64-bit MMX loads). */
static void RasterClearZeroLanes(SoftwareRasterScanState *state)
{
    state->longEdgeDepth.unusedHighLane = 0;
    state->longEdgeDepthStepY.unusedHighLane = 0;
    state->depthStepX.unusedHighLane = 0;
    state->textureAddress.zeroAfterUMask = 0;
    state->textureAddress.zeroBeforeVMask = 0;
    state->textureAddress.zeroShiftHigh = 0;
}

/* Builds the inputs of one run into `input` (buffers already allocated with the sizes of setup). */
static int RasterBuildRun(const RasterCompareCase *testCase, unsigned seed, RasterRunSetup *setup,
                          RasterBuffers *input, uint32_t *textureEntry, uint32_t *sourceEntry)
{
    unsigned rng = seed;
    int i;
    int textured = testCase->mode >= 16;
    long long cross;
    int area;
    int sameColor;
    GraphicsPrimitivePacket *packet = &input->packet;

    RasterFillRandom(input->color, setup->colorBytes + 2 * RASTER_GUARD_BYTES, &rng);
    memset(input->color, RASTER_GUARD_VALUE, RASTER_GUARD_BYTES);
    memset(input->color + RASTER_GUARD_BYTES + setup->colorBytes, RASTER_GUARD_VALUE, RASTER_GUARD_BYTES);
    /* depth values in a band around the vertex depths, so about half of the pixels pass */
    for (i = 0; i < (int)(setup->depthBytes / 4); i++) {
        ((uint32_t *)(input->depth + RASTER_GUARD_BYTES))[i] = 0x20000000u + (RasterRandom(&rng) % 0x40000000u);
    }
    memset(input->depth, RASTER_GUARD_VALUE, RASTER_GUARD_BYTES);
    memset(input->depth + RASTER_GUARD_BYTES + setup->depthBytes, RASTER_GUARD_VALUE, RASTER_GUARD_BYTES);
    if (textured) {
        RasterFillRandom(input->texture, setup->textureBytes, &rng);
    }

    memset(textureEntry, 0, 8 * sizeof(uint32_t));
    memset(sourceEntry, 0, 8 * sizeof(uint32_t));
    textureEntry[1] = (uint32_t)setup->widthLog2;
    textureEntry[2] = (uint32_t)setup->heightLog2;
    textureEntry[3] = (uint32_t)(uintptr_t)input->texture;
    textureEntry[4] = (uint32_t)(uintptr_t)sourceEntry;
    sourceEntry[2] = (uint32_t)setup->paletteIndex;
    sourceEntry[3] = RASTER_TEXTURE_HEADER;

    memset(packet, 0, sizeof *packet);
    sameColor = (RasterRandom(&rng) & 7) == 0;
    for (i = 0; i < 3; i++) {
        GraphicsPrimitiveVertexRaw *v = &packet->vertices[i];
        int x = RasterRandomRange(&rng, -setup->width / 4, setup->width + setup->width / 4);
        int y = RasterRandomRange(&rng, -setup->height / 4, setup->height + setup->height / 4);
        v->screenX = (x << 12) | (int)(RasterRandom(&rng) & 0xfff);
        v->screenY = (y << 12) | (int)(RasterRandom(&rng) & 0xfff);
        v->depth = 0x20000000 + (int)(RasterRandom(&rng) % 0x40000000u);
        v->textureU = (int)(RasterRandom(&rng) % 0x200000u) - 0x80000;
        v->textureV = (int)(RasterRandom(&rng) % 0x200000u) - 0x80000;
        v->diffuseColor = (sameColor && i > 0) ? packet->vertices[0].diffuseColor : RasterRandom(&rng);
    }
    packet->modulationColor = RasterRandom(&rng);
    packet->textureEntry = (GraphicsTextureSetEntry *)textureEntry;
    packet->renderFlags = (GraphicsPrimitiveDispatchFlags)(testCase->mode << 12);
    g_SoftwareDepthEpoch = (int32_t)(RasterRandom(&rng) % 0x1000000u);
    SoftwareRenderer_PrepareTrianglePacket(packet);

    /* the original divides 0x1000000000 by the (Q12) doubled area with IDIV: skip overflowing cases */
    cross = (long long)(packet->vertices[2].screenX - packet->vertices[0].screenX) *
                (packet->vertices[1].screenY - packet->vertices[0].screenY) -
            (long long)(packet->vertices[1].screenX - packet->vertices[0].screenX) *
                (packet->vertices[2].screenY - packet->vertices[0].screenY);
    area = (int)(cross >> 12);
    if (area != 0 && area >= -64 && area <= 64) {
        return 0;
    }

    RasterFillRandom(input->state, sizeof input->state, &rng);
    RasterClearZeroLanes((SoftwareRasterScanState *)input->state);
    return 1;
}

/* Mode04/06/12/14 of every family write depth only when the low dword of MM2 is >= 0x800, but never
   load MM2: the original sees whatever the previous triangle left there (the textured Mode20/22/28/30
   load it with the shaded alpha lane). The C versions use the current pixel's alpha instead (alpha >=
   0x80). For these modes the original runs twice, with MM2 = 0 (depth never written) and MM2 = ~0
   (always written); the colour must match exactly and every depth dword must match one of the two. */
static int RasterReadsStaleMm2(const RasterCompareCase *testCase)
{
    return testCase->mode == 4 || testCase->mode == 6 || testCase->mode == 12 || testCase->mode == 14;
}

/* First depth dword (byte offset) that matches neither `never` nor `always`; guards must match `never`. */
static int RasterFirstDepthOutsideBoth(const uint8_t *mine, const uint8_t *never, const uint8_t *always, unsigned depthBytes)
{
    unsigned i;
    for (i = 0; i < RASTER_GUARD_BYTES; i++) {
        if (mine[i] != never[i]) {
            return (int)i;
        }
    }
    for (i = RASTER_GUARD_BYTES; i < RASTER_GUARD_BYTES + depthBytes; i += 4) {
        uint32_t m = *(const uint32_t *)(mine + i);
        if (m != *(const uint32_t *)(never + i) && m != *(const uint32_t *)(always + i)) {
            return (int)i;
        }
    }
    for (i = RASTER_GUARD_BYTES + depthBytes; i < 2 * RASTER_GUARD_BYTES + depthBytes; i++) {
        if (mine[i] != never[i]) {
            return (int)i;
        }
    }
    return -1;
}

/* Test coverage: number of `unit`-byte elements that differ. */
static unsigned RasterCountChangedUnits(const uint8_t *before, const uint8_t *after, unsigned size, unsigned unit)
{
    unsigned count = 0;
    unsigned i;
    for (i = 0; i + unit <= size; i += unit) {
        if (memcmp(before + i, after + i, unit) != 0) {
            count++;
        }
    }
    return count;
}

static int RasterFirstDifference(const uint8_t *a, const uint8_t *b, unsigned size)
{
    unsigned i;
    for (i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            return (int)i;
        }
    }
    return -1;
}

/* Runs one implementation (originalEntry NULL = the C function) on a copy of `input`. */
static int RasterExecute(const RasterCompareCase *testCase, void *originalEntry, const RasterRunSetup *setup,
                         const RasterBuffers *input, RasterBuffers *output, SoftwareFramebufferAccess *access,
                         unsigned staleMm2, unsigned *exceptionCode)
{
    int faulted;
    memcpy(output->color, input->color, setup->colorBytes + 2 * RASTER_GUARD_BYTES);
    memcpy(output->depth, input->depth, setup->depthBytes + 2 * RASTER_GUARD_BYTES);
    output->packet = input->packet;
    memcpy(&g_SoftwareRasterScanState, input->state, sizeof input->state);

    access->width = (GraphicsPixelDimension)setup->width;
    access->height = (GraphicsPixelDimension)setup->height;
    access->bytesPerPixel = (enum SoftwareFramebufferPixelSize)setup->bytesPerPixel;
    access->pixels = output->color + RASTER_GUARD_BYTES;
    g_FramebufferAccess = access;
    g_FramebufferRowStrideBytes = setup->colorStride;
    g_SoftwareDepthBuffer = (int32_t *)(output->depth + RASTER_GUARD_BYTES);
    g_SoftwareDepthRowStrideBytes = setup->depthStride;
    g_SoftwareAuxiliaryTargetBase = output->color + RASTER_GUARD_BYTES;
    RasterSetPixelConstants(setup->layout555);

    faulted = RasterRunGuarded(testCase, originalEntry, setup->clipMaxY, setup->clipMaxX, setup->clipMinY,
                               setup->clipMinX, &output->packet, staleMm2, exceptionCode);
    memcpy(output->state, &g_SoftwareRasterScanState, sizeof output->state);
    return faulted;
}

static void RasterDescribeDifference(const char *what, const RasterRunSetup *setup, int offset, const uint8_t *mine,
                                     const uint8_t *theirs, int *x, int *y)
{
    int inside = offset - (int)RASTER_GUARD_BYTES;
    unsigned stride = (what[0] == 'c') ? setup->colorStride : setup->depthStride;
    unsigned pixelBytes = (what[0] == 'c') ? (unsigned)setup->bytesPerPixel : 4u;
    (void)mine;
    (void)theirs;
    if (inside < 0 || inside >= (int)((what[0] == 'c') ? setup->colorBytes : setup->depthBytes)) {
        *x = -1;
        *y = -1;
        return;
    }
    *y = inside / (int)stride;
    *x = (inside % (int)stride) / (int)pixelBytes;
}

void Thandor_SelfTestRasterCompare(void)
{
    const char *runsText = getenv("OPEN_THANDOR_RASTERCMP_RUNS");
    const char *filter = getenv("OPEN_THANDOR_RASTERCMP_FILTER");
    const char *seedText = getenv("OPEN_THANDOR_RASTERCMP_SEED");
    int runs = runsText ? atoi(runsText) : 300;
    unsigned baseSeed = seedText ? (unsigned)strtoul(seedText, NULL, 0) : 1u;
    uint8_t *code = (uint8_t *)Thandor_LoadOriginalCodeCopy(RASTER_CODE_START, RASTER_CODE_END - RASTER_CODE_START);
    uint8_t savedState[sizeof(SoftwareRasterScanState)];
    SoftwarePixelMmxConstants savedConstants = g_SoftwarePixelMmxConstants;
    SoftwareFramebufferAccess *savedAccess = g_FramebufferAccess;
    uint32_t savedColorStride = g_FramebufferRowStrideBytes;
    int32_t *savedDepth = g_SoftwareDepthBuffer;
    uint32_t savedDepthStride = g_SoftwareDepthRowStrideBytes;
    void *savedAux = g_SoftwareAuxiliaryTargetBase;
    int32_t savedEpoch = g_SoftwareDepthEpoch;
    const unsigned maxColorBytes = (320u * 4u + 64u) * 240u;
    const unsigned maxDepthBytes = (320u * 4u + 64u) * 240u;
    const unsigned maxTextureBytes = RASTER_TEXTURE_HEADER + 256u * 256u * 4u;
    RasterBuffers input;
    RasterBuffers mine;
    RasterBuffers theirs;
    RasterBuffers theirsOtherState;
    RasterBuffers theirsAlways;
    static uint32_t textureEntry[8];
    static uint32_t sourceEntry[8];
    SoftwareFramebufferAccess access;
    int caseIndex;
    int casesRun = 0;
    int casesFailed = 0;
    int casesStateOnly = 0;
    int casesStateDependent = 0;

    if (code == NULL) {
        Thandor_Log("rastercmp: could not load the original code (thandor_original.exe missing?)");
        return;
    }
    memcpy(savedState, &g_SoftwareRasterScanState, sizeof savedState);
    input.color = (uint8_t *)malloc(maxColorBytes + 2 * RASTER_GUARD_BYTES);
    input.depth = (uint8_t *)malloc(maxDepthBytes + 2 * RASTER_GUARD_BYTES);
    input.texture = (uint8_t *)malloc(maxTextureBytes);
    mine.color = (uint8_t *)malloc(maxColorBytes + 2 * RASTER_GUARD_BYTES);
    mine.depth = (uint8_t *)malloc(maxDepthBytes + 2 * RASTER_GUARD_BYTES);
    theirs.color = (uint8_t *)malloc(maxColorBytes + 2 * RASTER_GUARD_BYTES);
    theirs.depth = (uint8_t *)malloc(maxDepthBytes + 2 * RASTER_GUARD_BYTES);
    theirsOtherState.color = (uint8_t *)malloc(maxColorBytes + 2 * RASTER_GUARD_BYTES);
    theirsOtherState.depth = (uint8_t *)malloc(maxDepthBytes + 2 * RASTER_GUARD_BYTES);
    theirsAlways.color = (uint8_t *)malloc(maxColorBytes + 2 * RASTER_GUARD_BYTES);
    theirsAlways.depth = (uint8_t *)malloc(maxDepthBytes + 2 * RASTER_GUARD_BYTES);
    if (!input.color || !input.depth || !input.texture || !mine.color || !mine.depth || !theirs.color ||
        !theirs.depth || !theirsOtherState.color || !theirsOtherState.depth || !theirsAlways.color ||
        !theirsAlways.depth) {
        Thandor_Log("rastercmp: allocation failed");
        return;
    }
    Thandor_Log("rastercmp: %d runs per handler, seed %u, filter \"%s\"", runs, baseSeed, filter ? filter : "");

    for (caseIndex = 0; caseIndex < (int)(sizeof s_RasterCases / sizeof s_RasterCases[0]); caseIndex++) {
        const RasterCompareCase *testCase = &s_RasterCases[caseIndex];
        void *originalEntry = code + (testCase->originalAddress - RASTER_CODE_START);
        int run;
        int outputDiffs = 0;
        int stateDiffs = 0;
        int stateDependent = 0;
        int skipped = 0;
        int faults = 0;
        int drawn = 0;
        unsigned pixelsWritten = 0;
        unsigned depthsWritten = 0;
        int staleMm2 = RasterReadsStaleMm2(testCase);
        if (filter != NULL && strstr(testCase->name, filter) == NULL) {
            continue;
        }
        casesRun++;
        for (run = 0; run < runs; run++) {
            unsigned seed = (baseSeed * 2654435761u) ^ ((unsigned)caseIndex * 0x9e3779b9u) ^
                            ((unsigned)run * 0x85ebca6bu) ^ 0x1234567u;
            unsigned rng;
            RasterRunSetup setup;
            unsigned mineFault = 0;
            unsigned theirsFault = 0;
            int mineFaulted;
            int theirsFaulted;
            int colorDiff;
            int depthDiff;
            int stateDiff;
            if (seed == 0) {
                seed = 1;
            }
            rng = seed;
            memset(&setup, 0, sizeof setup);
            setup.width = RasterRandomRange(&rng, 8, 320);
            setup.height = RasterRandomRange(&rng, 8, 240);
            setup.layout555 = (int)(RasterRandom(&rng) & 1);
            setup.widthLog2 = RasterRandomRange(&rng, 0, 8);
            setup.heightLog2 = RasterRandomRange(&rng, 0, 8);
            setup.paletteIndex = (RasterRandom(&rng) % 3 == 0) ? -1 : RasterRandomRange(&rng, 0, 3);
            if (testCase->family == 2) {
                setup.bytesPerPixel = 4;
                setup.clipMinX = 0;
                setup.clipMinY = 0;
                setup.clipMaxX = setup.width;
                setup.clipMaxY = setup.height;
                setup.colorStride = (unsigned)setup.width * 4u;
                setup.depthStride = (unsigned)setup.width * 4u;
            }
            else {
                setup.bytesPerPixel = testCase->family == 0 ? 2 : 4;
                setup.clipMinX = RasterRandomRange(&rng, 0, setup.width / 4);
                setup.clipMinY = RasterRandomRange(&rng, 0, setup.height / 4);
                setup.clipMaxX = setup.width - RasterRandomRange(&rng, 0, setup.width / 4);
                setup.clipMaxY = setup.height - RasterRandomRange(&rng, 0, setup.height / 4);
                setup.colorStride = (unsigned)(setup.width * setup.bytesPerPixel) + 4u * (RasterRandom(&rng) % 5u);
                setup.depthStride = (unsigned)setup.width * 4u + 4u * (RasterRandom(&rng) % 5u);
            }
            setup.colorBytes = setup.colorStride * (unsigned)setup.height;
            setup.depthBytes = setup.depthStride * (unsigned)setup.height;
            setup.textureBytes = RASTER_TEXTURE_HEADER + (4u << (setup.widthLog2 + setup.heightLog2));

            if (!RasterBuildRun(testCase, RasterRandom(&rng), &setup, &input, textureEntry, sourceEntry)) {
                skipped++;
                continue;
            }
            drawn++;
            mineFaulted = RasterExecute(testCase, NULL, &setup, &input, &mine, &access, 0, &mineFault);
            theirsFaulted = RasterExecute(testCase, originalEntry, &setup, &input, &theirs, &access, 0, &theirsFault);
            if (staleMm2 && !theirsFaulted) {
                theirsFaulted = RasterExecute(testCase, originalEntry, &setup, &input, &theirsAlways, &access,
                                              0xffffffffu, &theirsFault);
            }
            if (mineFaulted || theirsFaulted) {
                faults++;
                if (faults <= 3) {
                    Thandor_Log("rastercmp %s run %d: FAULT mine %08x theirs %08x", testCase->name, run,
                                mineFaulted ? mineFault : 0, theirsFaulted ? theirsFault : 0);
                }
                continue;
            }
            pixelsWritten += RasterCountChangedUnits(input.color + RASTER_GUARD_BYTES, theirs.color + RASTER_GUARD_BYTES,
                                                     setup.colorBytes, (unsigned)setup.bytesPerPixel);
            depthsWritten += RasterCountChangedUnits(input.depth + RASTER_GUARD_BYTES, theirs.depth + RASTER_GUARD_BYTES,
                                                     setup.depthBytes, 4u);
            colorDiff = RasterFirstDifference(mine.color, theirs.color, setup.colorBytes + 2 * RASTER_GUARD_BYTES);
            depthDiff = staleMm2
                            ? RasterFirstDepthOutsideBoth(mine.depth, theirs.depth, theirsAlways.depth, setup.depthBytes)
                            : RasterFirstDifference(mine.depth, theirs.depth, setup.depthBytes + 2 * RASTER_GUARD_BYTES);
            stateDiff = RasterFirstDifference(mine.state, theirs.state, sizeof mine.state);
            if (staleMm2 && RasterFirstDifference(theirs.color, theirsAlways.color,
                                                  setup.colorBytes + 2 * RASTER_GUARD_BYTES) >= 0) {
                Thandor_Log("rastercmp %s run %d: note: the ORIGINAL colour output depends on MM2", testCase->name, run);
            }
            if (colorDiff >= 0 || depthDiff >= 0) {
                outputDiffs++;
                if (outputDiffs <= 3) {
                    const char *what = colorDiff >= 0 ? "color" : "depth";
                    int offset = colorDiff >= 0 ? colorDiff : depthDiff;
                    const uint8_t *a = colorDiff >= 0 ? mine.color : mine.depth;
                    const uint8_t *b = colorDiff >= 0 ? theirs.color : theirs.depth;
                    int x;
                    int y;
                    RasterDescribeDifference(what, &setup, offset, a, b, &x, &y);
                    Thandor_Log("rastercmp %s run %d: MISMATCH %s byte %d (pixel %d,%d): mine %02x theirs %02x "
                                "(%dx%d clip %d,%d-%d,%d %s tex %dx%d pal %d; v0 %08x,%08x v1 %08x,%08x v2 %08x,%08x)",
                                testCase->name, run, what, offset - (int)RASTER_GUARD_BYTES, x, y, a[offset], b[offset],
                                setup.width, setup.height, setup.clipMinX, setup.clipMinY, setup.clipMaxX,
                                setup.clipMaxY, setup.layout555 ? "555" : "565", 1 << setup.widthLog2,
                                1 << setup.heightLog2, setup.paletteIndex,
                                input.packet.vertices[0].screenX, input.packet.vertices[0].screenY,
                                input.packet.vertices[1].screenX, input.packet.vertices[1].screenY,
                                input.packet.vertices[2].screenX, input.packet.vertices[2].screenY);
                }
            }
            else if (stateDiff >= 0) {
                stateDiffs++;
                /* expected once a handler keeps its scan state in locals; details on request */
                if (stateDiffs <= 2 && getenv("OPEN_THANDOR_RASTERCMP_STATE") != NULL) {
                    Thandor_Log("rastercmp %s run %d: state differs at +0x%02x (mine %02x theirs %02x)",
                                testCase->name, run, stateDiff, mine.state[stateDiff], theirs.state[stateDiff]);
                }
            }
            /* does the original output depend on the incoming scan state? (it should not) */
            if (!stateDependent && run < 64) {
                unsigned otherFault = 0;
                unsigned stateRng = seed ^ 0x5bd1e995u;
                RasterFillRandom(input.state, sizeof input.state, &stateRng);
                RasterClearZeroLanes((SoftwareRasterScanState *)input.state);
                if (!RasterExecute(testCase, originalEntry, &setup, &input, &theirsOtherState, &access, 0, &otherFault) &&
                    (RasterFirstDifference(theirs.color, theirsOtherState.color,
                                           setup.colorBytes + 2 * RASTER_GUARD_BYTES) >= 0 ||
                     RasterFirstDifference(theirs.depth, theirsOtherState.depth,
                                           setup.depthBytes + 2 * RASTER_GUARD_BYTES) >= 0)) {
                    stateDependent = 1;
                    Thandor_Log("rastercmp %s run %d: note: the ORIGINAL output depends on the incoming scan state",
                                testCase->name, run);
                }
            }
        }
        if (outputDiffs > 0 || faults > 0) {
            casesFailed++;
        }
        else if (stateDiffs > 0) {
            casesStateOnly++;
        }
        if (stateDependent) {
            casesStateDependent++;
        }
        Thandor_Log("rastercmp %s: %s (%d drawn, %d skipped, %d output mismatches, %d faults, %d state-only; "
                    "original wrote %u pixels / %u depths)%s",
                    testCase->name, (outputDiffs || faults) ? "DIFFERS" : (stateDiffs ? "identical output" : "identical"),
                    drawn, skipped, outputDiffs, faults, stateDiffs, pixelsWritten, depthsWritten,
                    staleMm2 ? " [depth writes checked against stale MM2 = 0 and ~0]" : "");
    }

    memcpy(&g_SoftwareRasterScanState, savedState, sizeof savedState);
    g_SoftwarePixelMmxConstants = savedConstants;
    g_FramebufferAccess = savedAccess;
    g_FramebufferRowStrideBytes = savedColorStride;
    g_SoftwareDepthBuffer = savedDepth;
    g_SoftwareDepthRowStrideBytes = savedDepthStride;
    g_SoftwareAuxiliaryTargetBase = savedAux;
    g_SoftwareDepthEpoch = savedEpoch;
    free(input.color);
    free(input.depth);
    free(input.texture);
    free(mine.color);
    free(mine.depth);
    free(theirs.color);
    free(theirs.depth);
    free(theirsOtherState.color);
    free(theirsOtherState.depth);
    free(theirsAlways.color);
    free(theirsAlways.depth);
    Thandor_Log("rastercmp: %d handlers, %d with output mismatches/faults, %d with state-only differences, "
                "%d whose original output depends on the incoming scan state",
                casesRun, casesFailed, casesStateOnly, casesStateDependent);
}

#endif
