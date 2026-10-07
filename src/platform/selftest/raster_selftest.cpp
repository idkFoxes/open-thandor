/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/raster_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=raster: golden hashes of the software renderer's output, a safety net for rewrites of the
   triangle handlers and the 2D blits (docs/software_raster.md). No game data is needed: the framebuffer, depth
   buffer and texture-source assets are synthetic and filled from a local seeded LCG (not the game's Random_*
   streams). The software functions are called directly (the handler tables, the Software* blits, the blit slots
   set to them), so the result does not depend on the renderer selection. Every group starts from the same noise
   framebuffer and depth buffer and has its own seed, so each line is independent of the others. One line per
   group: "raster <group>: <FNV-1a hash> (<n> pixels changed)", the hash over the colour target (and the depth
   buffer for the handlers, the mask for the mask step). Run it with two builds and compare the lines. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <vector>

constexpr auto RASTER_TEST_WIDTH = 320;
constexpr auto RASTER_TEST_HEIGHT = 200;
constexpr auto RASTER_TEST_AUX_WIDTH = 96;
constexpr auto RASTER_TEST_AUX_HEIGHT = 64;
constexpr auto RASTER_TEST_TRIANGLES = 48;
constexpr auto RASTER_TEST_BLITS = 48;
constexpr auto RASTER_TEST_DEPTH_EPOCH = 0x10000000;
constexpr auto RASTER_TEST_MAX_IMAGES = 8; /* the subresource table lives in the header's 256-byte unusedText */

struct RasterTestImage {
    int widthLog2; /* raster textures: width = 1 << widthLog2; blit images use width/height */
    int heightLog2;
    int width;
    int height;
    int paletteIndex; /* -1: ARGB texels */
    int originX;
    int originY;
    int logicalExtra; /* logical size = pixel size + this (tile spacing of the tiled blits) */
};

struct RasterTestAsset {
    std::vector<uint32_t> storage;
    GraphicsTextureSourceAsset *header;
    GraphicsTextureSourceEntry *entries;
};

struct RasterTestState {
    std::vector<uint32_t> pixels;
    std::vector<int32_t> depth;
    std::vector<uint32_t> auxPixels;
    std::vector<uint32_t> resetPixels; /* the targets after RasterTest_ResetTargets, to count changed pixels */
    std::vector<uint32_t> resetAuxPixels;
    SoftwareFramebufferAccess framebuffer;
};

/* 64-bit LCG (Knuth's MMIX constants); the high half is the result. */
static uint32_t RasterTest_Random(uint64_t *seed)
{
    *seed = *seed * 6364136223846793005ull + 1442695040888963407ull;
    return (uint32_t)(*seed >> 32);
}

/* Random in [low, high). */
static int RasterTest_Range(uint64_t *seed, int low, int high)
{
    return low + (int)(RasterTest_Random(seed) % (uint32_t)(high - low));
}

/* An ARGB colour whose alpha is 0, 0xFF or anything in between, so the transparent, opaque and blend paths
   are all hit. */
static uint32_t RasterTest_Argb(uint64_t *seed)
{
    uint32_t rgb = RasterTest_Random(seed) & 0xFFFFFFu;
    uint32_t pick = RasterTest_Random(seed) % 4;
    uint32_t alpha = pick == 0 ? 0 : (pick == 1 ? 0xFF : (RasterTest_Random(seed) & 0xFF));
    return (alpha << 24) | rgb;
}

static uint32_t RasterTest_Hash(uint32_t hash, const void *bytes, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++) {
        hash = (hash ^ static_cast<const uint8_t *>(bytes)[i]) * 16777619u;
    }
    return hash;
}

/* Builds a 'gfx' texture source: header, bankCount palette banks (ARGB at +0, the pixel converted by
   GraphicsTextureSource_ConvertPaletteEntries at +4), then each image's texels followed by a row and a few
   texels of padding (the stretch and the bilinear scale read one row and one texel past the image). */
static void RasterTest_BuildAsset(RasterTestAsset *asset, const RasterTestImage *images, int count, int bankCount,
                                  uint64_t seed)
{
    uint32_t offsets[RASTER_TEST_MAX_IMAGES];
    uint32_t size = GFX_ASSET_HEADER_SIZE + (uint32_t)bankCount * GFX_PALETTE_BANK_SIZE;
    uint8_t *base;
    uint32_t *palette;
    int i;
    for (i = 0; i < count; i++) {
        uint32_t texelBytes = images[i].paletteIndex < 0 ? 4 : 1;
        offsets[i] = size;
        size += ((uint32_t)(images[i].width * images[i].height + images[i].width + 4) * texelBytes + 15) & ~3u;
    }
    asset->storage.assign(size / 4, 0);
    base = reinterpret_cast<uint8_t *>(asset->storage.data());
    asset->header = reinterpret_cast<GraphicsTextureSourceAsset *>(base);
    asset->header->common.magic = ASSET_MAGIC_GFX;
    asset->header->tableDescriptor.subresourceCount = (uint32_t)count;
    asset->header->tableDescriptor.paletteBankCount = (uint32_t)bankCount;
    asset->header->tableDescriptor.subresourceTableOffset = offsetof(GraphicsTextureSourceAsset, unusedText);
    asset->entries = reinterpret_cast<GraphicsTextureSourceEntry *>(asset->header->unusedText);
    palette = Asset_RecordAt<uint32_t>(base, GFX_ASSET_HEADER_SIZE);
    for (i = 0; i < bankCount * 256; i++) {
        palette[i * 2] = RasterTest_Argb(&seed);
    }
    for (i = 0; i < count; i++) {
        GraphicsTextureSourceEntry *entry = &asset->entries[i];
        uint32_t texelCount = (uint32_t)(images[i].width * images[i].height + images[i].width + 4);
        uint32_t t;
        entry->logicalWidth = (uint32_t)(images[i].width + images[i].logicalExtra);
        entry->logicalHeight = (uint32_t)(images[i].height + images[i].logicalExtra);
        entry->paletteIndex = images[i].paletteIndex;
        entry->dataOffset = offsets[i];
        entry->originX = images[i].originX;
        entry->originY = images[i].originY;
        entry->pixelWidth = (uint32_t)images[i].width;
        entry->pixelHeight = (uint32_t)images[i].height;
        for (t = 0; t < texelCount; t++) {
            if (images[i].paletteIndex < 0) {
                Asset_RecordAt<uint32_t>(base, offsets[i])[t] = RasterTest_Argb(&seed);
            }
            else {
                base[offsets[i] + t] = (uint8_t)RasterTest_Random(&seed);
            }
        }
    }
    GraphicsTextureSource_ConvertPaletteEntries(reinterpret_cast<GraphicsPaletteTextureSourceAsset *>(base));
}

/* The same noise framebuffer, depth buffer and auxiliary target before every group. */
static void RasterTest_ResetTargets(RasterTestState *state)
{
    uint64_t seed = 20260601;
    size_t i;
    for (i = 0; i < state->pixels.size(); i++) {
        state->pixels[i] = RasterTest_Random(&seed);
    }
    for (i = 0; i < state->depth.size(); i++) {
        /* around the triangles' depths, so the depth test both passes and fails */
        state->depth[i] = 0x10000000 + (int32_t)(RasterTest_Random(&seed) % 0x50000000u);
    }
    for (i = 0; i < state->auxPixels.size(); i++) {
        state->auxPixels[i] = RasterTest_Random(&seed);
    }
    state->resetPixels = state->pixels;
    state->resetAuxPixels = state->auxPixels;
}

/* Pixels that differ from the reset state (a group that draws nothing would show 0). */
static unsigned RasterTest_ChangedPixels(const std::vector<uint32_t> &pixels, const std::vector<uint32_t> &reset)
{
    unsigned changed = 0;
    size_t i;
    for (i = 0; i < pixels.size(); i++) {
        changed += pixels[i] != reset[i];
    }
    return changed;
}

static void RasterTest_LogHash(const char *group, uint32_t hash, unsigned changedPixels)
{
    Thandor_Log("raster %s: %08X (%u pixels changed)", group, hash, changedPixels);
}

/* ---- triangle handlers ---- */

/* A random triangle for handler index `index` of the tables. Flat modes (bit 3) get three equal colours,
   the others different ones, because SoftwareRenderer_PrepareTrianglePacket recomputes the flat bit from
   them. Sizes from a few pixels to larger than the target; some vertices lie off-screen. */
static void RasterTest_MakeTriangle(uint64_t *seed, int index, int targetWidth, int targetHeight,
                                    GraphicsTextureSetEntry *textures, int textureCount,
                                    GraphicsPrimitivePacket *packet)
{
    static const int spreads[] = {12, 60, 160, 400};
    int spread = spreads[RasterTest_Random(seed) % 4];
    int centerX = RasterTest_Range(seed, -spread / 2, targetWidth + spread / 2);
    int centerY = RasterTest_Range(seed, -spread / 2, targetHeight + spread / 2);
    uint32_t flatColor = RasterTest_Random(seed);
    int v;
    memset(packet, 0, sizeof *packet);
    for (v = 0; v < 3; v++) {
        GraphicsPrimitiveVertexRaw *vertex = &packet->vertices[v];
        int x = centerX + RasterTest_Range(seed, -spread, spread + 1);
        int y = centerY + RasterTest_Range(seed, -spread, spread + 1);
        vertex->screenX = (x << 12) | (int)(RasterTest_Random(seed) & 0xFFF);
        vertex->screenY = (y << 12) | (int)(RasterTest_Random(seed) & 0xFFF);
        vertex->depth = 0x08000000 + (int)(RasterTest_Random(seed) % 0x40000000u);
        vertex->textureU = RasterTest_Range(seed, -0x200000, 0x200000);
        vertex->textureV = RasterTest_Range(seed, -0x200000, 0x200000);
        vertex->diffuseColor = (index & 8) != 0 ? flatColor : RasterTest_Random(seed);
    }
    if ((index & 8) == 0 && packet->vertices[0].diffuseColor == packet->vertices[1].diffuseColor &&
        packet->vertices[0].diffuseColor == packet->vertices[2].diffuseColor) {
        packet->vertices[2].diffuseColor ^= 1;
    }
    packet->modulationColor = RasterTest_Random(seed);
    packet->renderFlags = FromBits<GraphicsPrimitiveDispatchFlags>(index << 12);
    if ((index & 16) != 0) {
        packet->textureEntry = &textures[RasterTest_Random(seed) % (uint32_t)textureCount];
    }
}

static int RasterTest_Handlers(RasterTestState *state, GraphicsTextureSetEntry *textures, int textureCount,
                               int auxiliary)
{
    SoftwareRasterHandler **table = auxiliary ? g_SoftwareRasterHandlersAuxiliary : g_SoftwareRasterHandlers32Bit;
    int targetWidth = auxiliary ? RASTER_TEST_AUX_WIDTH : RASTER_TEST_WIDTH;
    int targetHeight = auxiliary ? RASTER_TEST_AUX_HEIGHT : RASTER_TEST_HEIGHT;
    int covered = 0;
    int index;
    for (index = 0; index < 64; index++) {
        static GraphicsPrimitivePacket packet;
        uint64_t seed = 0x5EED0000u + (uint64_t)(auxiliary * 64 + index);
        uint32_t hash = 2166136261u;
        char group[48];
        int t;
        if (table[index] == nullptr) {
            continue;
        }
        RasterTest_ResetTargets(state);
        if (auxiliary) {
            g_SoftwareAuxiliaryTargetBase = state->auxPixels.data();
        }
        for (t = 0; t < RASTER_TEST_TRIANGLES; t++) {
            int clipMinX = 0;
            int clipMinY = 0;
            int clipMaxX = targetWidth;
            int clipMaxY = targetHeight;
            RasterTest_MakeTriangle(&seed, index, targetWidth, targetHeight, textures, textureCount, &packet);
            if (!auxiliary && (t & 1) != 0) {
                /* a viewport inside the framebuffer */
                clipMinX = RasterTest_Range(&seed, 0, targetWidth / 4);
                clipMinY = RasterTest_Range(&seed, 0, targetHeight / 4);
                clipMaxX = RasterTest_Range(&seed, targetWidth * 3 / 4, targetWidth + 1);
                clipMaxY = RasterTest_Range(&seed, targetHeight * 3 / 4, targetHeight + 1);
            }
            /* as SoftwareRenderer_DrawQueue32Bit / SoftwareRenderer_DrawQueueAuxiliary do */
            SoftwareRenderer_PrepareTrianglePacket(&packet);
            table[ToBits(packet.renderFlags & GRAPHICS_PRIMITIVE_RASTER_HANDLER_MASK) >> 12](clipMaxY, clipMaxX, clipMinY,
                                                                                     clipMinX, &packet);
        }
        if (auxiliary) {
            hash = RasterTest_Hash(hash, state->auxPixels.data(), state->auxPixels.size() * 4);
            hash = RasterTest_Hash(hash, state->depth.data(),
                                   (size_t)RASTER_TEST_AUX_WIDTH * RASTER_TEST_AUX_HEIGHT * 4);
        }
        else {
            hash = RasterTest_Hash(hash, state->pixels.data(), state->pixels.size() * 4);
            hash = RasterTest_Hash(hash, state->depth.data(), state->depth.size() * 4);
        }
        snprintf(group, sizeof group, "%s mode %02d", auxiliary ? "aux" : "32bit", index);
        RasterTest_LogHash(group, hash,
                           auxiliary ? RasterTest_ChangedPixels(state->auxPixels, state->resetAuxPixels)
                                     : RasterTest_ChangedPixels(state->pixels, state->resetPixels));
        covered++;
    }
    return covered;
}

/* ---- blits ---- */

/* The values are the group numbers of the original list and seed the groups (0xB117000 + kind), so the hashes of
   the remaining groups stay comparable after the unused blits (saturated add, half RGB saturated add, palette bank,
   integer scaled, their tiled forms and the region copies) were removed. */
enum {
    RASTER_BLIT_SOURCE_ALPHA = 0,
    RASTER_BLIT_HALF_SOURCE_RGB = 1,
    RASTER_BLIT_MODULATED = 4,
    RASTER_BLIT_STRETCH = 7,
    RASTER_BLIT_FILL_RECT = 8,
    RASTER_BLIT_TILED_SOURCE_ALPHA = 9,
    RASTER_BLIT_TILED_HALF_SOURCE_RGB = 10,
    RASTER_BLIT_BILINEAR_BLEND_SCALE = 14,
    RASTER_BLIT_MASK_STEP = 15
};

static const struct {
    int kind;
    const char *name;
} g_RasterTestBlitGroups[] = {
    {RASTER_BLIT_SOURCE_ALPHA, "blit source-alpha"},
    {RASTER_BLIT_HALF_SOURCE_RGB, "blit half-source-rgb"},
    {RASTER_BLIT_MODULATED, "blit modulated"},
    {RASTER_BLIT_STRETCH, "blit stretch"},
    {RASTER_BLIT_FILL_RECT, "blit fill-rect"},
    {RASTER_BLIT_TILED_SOURCE_ALPHA, "blit tiled-source-alpha"},
    {RASTER_BLIT_TILED_HALF_SOURCE_RGB, "blit tiled-half-source-rgb"},
    {RASTER_BLIT_BILINEAR_BLEND_SCALE, "blit bilinear-blend-scale"},
    {RASTER_BLIT_MASK_STEP, "blit mask-step"},
};

#define RASTER_BLIT_GROUP_COUNT ((int)(sizeof g_RasterTestBlitGroups / sizeof g_RasterTestBlitGroups[0]))

/* Images of the blit asset (bank count 3). 0 is the mask step's size (40 x 30, logical size of subresource 0),
   5 and 6 the paletted pair of the bilinear blend scale, 7 has an invalid palette index (rejected). */
static const RasterTestImage g_RasterTestBlitImages[] = {
    {0, 0, 40, 30, -1, 0, 0, 0},  {0, 0, 17, 9, 0, -3, 2, 3},   {0, 0, 1, 1, -1, 0, 0, 2},
    {0, 0, 64, 48, 2, 5, -7, 0},  {0, 0, 33, 21, -1, -10, -4, 7}, {0, 0, 32, 16, 1, 0, 0, 0},
    {0, 0, 32, 16, 0, 0, 0, 1},   {0, 0, 8, 8, 3, 0, 0, 0},
};

#define RASTER_TEST_BLIT_IMAGE_COUNT ((int)(sizeof g_RasterTestBlitImages / sizeof g_RasterTestBlitImages[0]))

static void RasterTest_BlitOnce(int kind, uint64_t *seed, RasterTestState *state, RasterTestAsset *asset)
{
    SoftwareFramebufferAccess *fb = &state->framebuffer;
    GraphicsTextureSourceAsset *source = asset->header;
    uint32_t image = RasterTest_Random(seed) % (uint32_t)RASTER_TEST_BLIT_IMAGE_COUNT;
    /* clip rectangles partly outside the framebuffer and sometimes inverted */
    int clipMinX = RasterTest_Range(seed, -20, RASTER_TEST_WIDTH);
    int clipMinY = RasterTest_Range(seed, -20, RASTER_TEST_HEIGHT);
    int clipMaxX = RasterTest_Range(seed, clipMinX - 10, RASTER_TEST_WIDTH + 30);
    int clipMaxY = RasterTest_Range(seed, clipMinY - 10, RASTER_TEST_HEIGHT + 30);
    int drawX = RasterTest_Range(seed, -60, RASTER_TEST_WIDTH + 10);
    int drawY = RasterTest_Range(seed, -60, RASTER_TEST_HEIGHT + 10);
    if ((RasterTest_Random(seed) & 1) != 0) {
        /* the usual case: the whole framebuffer */
        clipMinX = 0;
        clipMinY = 0;
        clipMaxX = RASTER_TEST_WIDTH;
        clipMaxY = RASTER_TEST_HEIGHT;
    }
    switch (kind) {
    case RASTER_BLIT_SOURCE_ALPHA:
        g_GraphicsTextureSourceBlitSourceAlpha(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX, image, source, fb);
        break;
    case RASTER_BLIT_HALF_SOURCE_RGB:
        g_GraphicsTextureSourceBlitHalfSourceRgb(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX, image, source,
                                                 fb);
        break;
    case RASTER_BLIT_MODULATED:
        g_GraphicsTextureSourceBlitModulatedSourceAlpha(clipMaxY, clipMaxX, clipMinY, clipMinX, drawY, drawX,
                                                        RasterTest_Random(seed), image, source, fb);
        break;
    case RASTER_BLIT_STRETCH: {
        /* no clipping: the destination stays inside; sizes of at least 2 (the steps divide by size - 1) */
        int width = RasterTest_Range(seed, 2, 120);
        int height = RasterTest_Range(seed, 2, 90);
        int x = RasterTest_Range(seed, 0, RASTER_TEST_WIDTH - width + 1);
        int y = RasterTest_Range(seed, 0, RASTER_TEST_HEIGHT - height + 1);
        g_GraphicsTextureSourceStretchDirectColorBilinear((uint32_t)height, (uint32_t)width, y, x, image, source, fb);
        break;
    }
    case RASTER_BLIT_FILL_RECT: {
        int rectMinX = RasterTest_Range(seed, -40, RASTER_TEST_WIDTH);
        int rectMinY = RasterTest_Range(seed, -40, RASTER_TEST_HEIGHT);
        int rectMaxX = RasterTest_Range(seed, rectMinX - 5, RASTER_TEST_WIDTH + 40);
        int rectMaxY = RasterTest_Range(seed, rectMinY - 5, RASTER_TEST_HEIGHT + 40);
        g_GraphicsFramebufferFillRectArgb(clipMaxY, clipMaxX, clipMinY, clipMinX, rectMaxY, rectMaxX, rectMinY,
                                          rectMinX, RasterTest_Argb(seed), fb);
        break;
    }
    case RASTER_BLIT_TILED_SOURCE_ALPHA:
    case RASTER_BLIT_TILED_HALF_SOURCE_RGB: {
        int repeatEndX = (RasterTest_Random(seed) % 3) == 0 ? GRAPHICS_TILED_BLIT_ONE_TILE
                                                             : RasterTest_Range(seed, 0, RASTER_TEST_WIDTH + 30);
        int repeatEndY = (RasterTest_Random(seed) % 3) == 0 ? GRAPHICS_TILED_BLIT_ONE_TILE
                                                             : RasterTest_Range(seed, 0, RASTER_TEST_HEIGHT + 30);
        if (image == 2) {
            image = 4; /* the 1x1 image would make thousands of tiles */
        }
        if (kind == RASTER_BLIT_TILED_SOURCE_ALPHA) {
            GraphicsTextureSource_BlitTiledSourceAlpha(clipMaxY, clipMaxX, clipMinY, clipMinX, repeatEndY, repeatEndX,
                                                       drawY, drawX, image, source, fb);
        }
        else {
            GraphicsTextureSource_BlitTiledHalfSourceRgb(clipMaxY, clipMaxX, clipMinY, clipMinX, repeatEndY,
                                                         repeatEndX, drawY, drawX, image, source, fb);
        }
        break;
    }
    default:
        break;
    }
}

/* The cross-fade + bilinear scale of the texture preview (paletted images 5 and 6, 32 x 16). */
static void RasterTest_BilinearBlendScale(uint64_t *seed, RasterTestState *state, RasterTestAsset *asset)
{
    std::vector<uint64_t> blended((32 * 16 + 2 * 32 + 16) / 8);
    std::vector<uint64_t> factors(32 * 16 / 8);
    int i;
    for (i = 0; i < RASTER_TEST_BLITS / 4; i++) {
        int width = RasterTest_Range(seed, 2, 160);
        int height = RasterTest_Range(seed, 2, 120);
        int left = RasterTest_Range(seed, 0, RASTER_TEST_WIDTH - width + 1);
        int top = RasterTest_Range(seed, 0, RASTER_TEST_HEIGHT - height + 1);
        size_t f;
        for (f = 0; f < factors.size(); f++) {
            factors[f] = ((uint64_t)RasterTest_Random(seed) << 32) | RasterTest_Random(seed);
        }
        SoftwareTexture_BilinearBlendScaleSubresources((uint32_t)height, (uint32_t)width, top, left, blended.data(),
                                                       factors.data(), 6, 5, asset->header,
                                                       &state->framebuffer);
    }
}

/* The credits mask step on a 40 x 30 mask (the logical size of subresource 0): 37 blocks of 32 bytes. */
static uint32_t RasterTest_MaskStep(uint64_t *seed, RasterTestAsset *asset, uint32_t hash)
{
    static SoftwareMaskRuntimeView view;
    std::vector<uint8_t> mask(40 * 30);
    size_t i;
    int step;
    for (i = 0; i < mask.size(); i++) {
        uint32_t r = RasterTest_Random(seed);
        mask[i] = (r & 0x300) == 0 ? 0 : (uint8_t)r;
    }
    memset(&view, 0, sizeof view);
    view.textureSource = asset->header;
    view.maskPixels = mask.data();
    for (step = 0; step < 10; step++) {
        SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(&view);
        hash = RasterTest_Hash(hash, mask.data(), mask.size());
    }
    return hash;
}

static int RasterTest_Blits(RasterTestState *state, RasterTestAsset *asset)
{
    int group;
    for (group = 0; group < RASTER_BLIT_GROUP_COUNT; group++) {
        int kind = g_RasterTestBlitGroups[group].kind;
        uint64_t seed = 0xB117000u + (uint64_t)kind;
        uint32_t hash = 2166136261u;
        int i;
        RasterTest_ResetTargets(state);
        if (kind == RASTER_BLIT_BILINEAR_BLEND_SCALE) {
            RasterTest_BilinearBlendScale(&seed, state, asset);
        }
        else if (kind == RASTER_BLIT_MASK_STEP) {
            hash = RasterTest_MaskStep(&seed, asset, hash);
        }
        else {
            for (i = 0; i < RASTER_TEST_BLITS; i++) {
                RasterTest_BlitOnce(kind, &seed, state, asset);
            }
        }
        hash = RasterTest_Hash(hash, state->pixels.data(), state->pixels.size() * 4);
        RasterTest_LogHash(g_RasterTestBlitGroups[group].name, hash, RasterTest_ChangedPixels(state->pixels, state->resetPixels));
    }
    return RASTER_BLIT_GROUP_COUNT;
}

void Thandor_SelfTestRaster()
{
    /* power-of-two textures for the handlers: direct and paletted (banks 0..2), 1x1 to 256x256 */
    static const RasterTestImage textureImages[] = {
        {0, 0, 1, 1, -1, 0, 0, 0},     {3, 2, 8, 4, 0, 0, 0, 0},     {5, 5, 32, 32, -1, 0, 0, 0},
        {6, 4, 64, 16, 1, 0, 0, 0},    {7, 7, 128, 128, -1, 0, 0, 0}, {8, 8, 256, 256, 2, 0, 0, 0},
        {4, 6, 16, 64, -1, 0, 0, 0},
    };
    const int textureCount = (int)(sizeof textureImages / sizeof textureImages[0]);
    static RasterTestState state;
    static RasterTestAsset textureAsset;
    static RasterTestAsset blitAsset;
    GraphicsTextureSetEntry textures[sizeof textureImages / sizeof textureImages[0]];
    SoftwarePixelPackTables packTables;
    /* everything the test replaces, restored at the end */
    SoftwareFramebufferAccess *savedFramebuffer = g_FramebufferAccess;
    uint32_t savedRowStride = g_FramebufferRowStrideBytes;
    int32_t *savedDepthBuffer = g_SoftwareDepthBuffer;
    uint32_t savedDepthStride = g_SoftwareDepthRowStrideBytes;
    void *savedAuxBase = g_SoftwareAuxiliaryTargetBase;
    int32_t savedEpoch = g_SoftwareDepthEpoch;
    SoftwarePixelPackTables *savedPackTables = g_SoftwarePixelPackTables;
    SoftwarePixelFormatConfig savedFormat = g_SoftwarePixelFormatConfig;
    GraphicsTextureSourceBlitProc *savedSourceAlpha = g_GraphicsTextureSourceBlitSourceAlpha;
    GraphicsTextureSourceBlitProc *savedHalfSourceRgb = g_GraphicsTextureSourceBlitHalfSourceRgb;
    GraphicsTextureSourceBlitModulatedSourceAlphaProc *savedModulated = g_GraphicsTextureSourceBlitModulatedSourceAlpha;
    GraphicsTextureSourceStretchDirectColorBilinearProc *savedStretch = g_GraphicsTextureSourceStretchDirectColorBilinear;
    GraphicsFramebufferFillRectArgbProc *savedFill = g_GraphicsFramebufferFillRectArgb;
    int handlers32;
    int handlersAux;
    int blits;
    int i;

    /* the 32-bit ARGB pixel format of the SDL backend, neutral colour scale and bias */
    g_SoftwarePixelFormatConfig.redBitCount = 8;
    g_SoftwarePixelFormatConfig.greenBitCount = 8;
    g_SoftwarePixelFormatConfig.blueBitCount = 8;
    g_SoftwarePixelFormatConfig.redShift = 16;
    g_SoftwarePixelFormatConfig.greenShift = 8;
    g_SoftwarePixelFormatConfig.blueShift = 0;
    g_SoftwarePixelFormatConfig.redMask = 0xFF0000;
    g_SoftwarePixelFormatConfig.greenMask = 0xFF00;
    g_SoftwarePixelFormatConfig.blueMask = 0xFF;
    g_SoftwarePixelPackTables = &packTables;
    SoftwarePixelFormat_BuildChannelPackTables(0x10000, 0);
    g_GraphicsTextureSourceBlitSourceAlpha = SoftwareTextureSource_BlitSourceAlpha32;
    g_GraphicsTextureSourceBlitHalfSourceRgb = SoftwareTextureSource_BlitHalfSourceRgb32;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha = SoftwareTextureSource_BlitModulatedSourceAlpha32;
    g_GraphicsTextureSourceStretchDirectColorBilinear = SoftwareTextureSource_StretchDirectColorBilinear32;
    g_GraphicsFramebufferFillRectArgb = SoftwareFramebuffer_FillRectArgb32;

    state.pixels.assign((size_t)RASTER_TEST_WIDTH * RASTER_TEST_HEIGHT, 0);
    state.depth.assign((size_t)RASTER_TEST_WIDTH * RASTER_TEST_HEIGHT, 0);
    state.auxPixels.assign((size_t)RASTER_TEST_AUX_WIDTH * RASTER_TEST_AUX_HEIGHT, 0);
    state.framebuffer.width = RASTER_TEST_WIDTH;
    state.framebuffer.height = RASTER_TEST_HEIGHT;
    state.framebuffer.bytesPerPixel = 4;
    state.framebuffer.pixels = reinterpret_cast<uint8_t *>(state.pixels.data());
    g_FramebufferAccess = &state.framebuffer;
    g_FramebufferRowStrideBytes = RASTER_TEST_WIDTH * 4;
    g_SoftwareDepthBuffer = state.depth.data();
    g_SoftwareDepthRowStrideBytes = RASTER_TEST_WIDTH * 4;
    g_SoftwareDepthEpoch = RASTER_TEST_DEPTH_EPOCH;

    RasterTest_BuildAsset(&textureAsset, textureImages, textureCount, 3, 4711);
    for (i = 0; i < textureCount; i++) {
        memset(&textures[i], 0, sizeof textures[i]);
        textures[i].widthLog2 = (uint32_t)textureImages[i].widthLog2;
        textures[i].heightLog2 = (uint32_t)textureImages[i].heightLog2;
        textures[i].sourceAsset = textureAsset.header;
        textures[i].sourceEntry = &textureAsset.entries[i];
        textures[i].subresourceIndex = (uint32_t)i;
    }
    RasterTest_BuildAsset(&blitAsset, g_RasterTestBlitImages, RASTER_TEST_BLIT_IMAGE_COUNT, 3, 815);

    handlers32 = RasterTest_Handlers(&state, textures, textureCount, 0);
    handlersAux = RasterTest_Handlers(&state, textures, textureCount, 1);
    blits = RasterTest_Blits(&state, &blitAsset);
    Thandor_Log("raster: %d 32-bit and %d auxiliary handler entries, %d blit groups", handlers32, handlersAux,
                blits);

    g_FramebufferAccess = savedFramebuffer;
    g_FramebufferRowStrideBytes = savedRowStride;
    g_SoftwareDepthBuffer = savedDepthBuffer;
    g_SoftwareDepthRowStrideBytes = savedDepthStride;
    g_SoftwareAuxiliaryTargetBase = savedAuxBase;
    g_SoftwareDepthEpoch = savedEpoch;
    g_SoftwarePixelPackTables = savedPackTables;
    g_SoftwarePixelFormatConfig = savedFormat;
    g_GraphicsTextureSourceBlitSourceAlpha = savedSourceAlpha;
    g_GraphicsTextureSourceBlitHalfSourceRgb = savedHalfSourceRgb;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha = savedModulated;
    g_GraphicsTextureSourceStretchDirectColorBilinear = savedStretch;
    g_GraphicsFramebufferFillRectArgb = savedFill;
}
