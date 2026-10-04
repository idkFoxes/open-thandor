/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/selftests.cpp
 * Project code (not in the original game)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/platform/sdl3/window_icon.h>

#include <vector>

/* Self-test data */
#define SELFTEST_GUARD_BYTES 0x10000      /* codec: bytes behind each output buffer that must stay untouched */
#define SELFTEST_GUARD_FILL 0xCD          /* codec: fill byte of the output buffers and their guards */
#define SELFTEST_UNWRITTEN_FILL 0xAB      /* path split: fill byte that marks untouched output */
#define SCANADDR_MAX_UNPACKED_BYTES 0x4000000 /* scanaddr: entries claiming more are taken as the end of the package */
#define SCANADDR_REBUILT_IMAGE_SPAN 0x300000  /* scanaddr: dwords in [REBUILT_IMAGE_BASE, + this) are reported */

/* Diagnostics: OPEN_THANDOR_SELFTEST=codec round-trips synthetic save-sized data through the PCK
   encoder/decoder tables, checks guard bytes behind the output and logs the result. */
static void Thandor_SelfTestCodec(void)
{
    static const unsigned sizes[3] = {MODEL_RUNTIME_POOL_BYTES, ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot), EFFECT_RUNTIME_POOL_BYTES};
    unsigned t;
    for (t = 0; t < 3 * 2; t++) {
        unsigned size = sizes[t % 3];
        int noisy = t >= 3;
        unsigned capacity = PACKAGE_SCRATCH_BUFFER_BYTES - 2 * PCK_ENTRY_HEADER_BYTES; /* as Package_UpsertEntry */
        unsigned guard = SELFTEST_GUARD_BYTES;
        uint8_t *source = (uint8_t *)malloc(size);
        uint8_t *packed = (uint8_t *)malloc(capacity + guard);
        uint8_t *unpacked = (uint8_t *)malloc(size + guard);
        unsigned i;
        unsigned seed = 12345;
        Bool8 encodeOk;
        Bool8 decodeOk;
        uint32_t encodeValue = 0; /* packed size, or the error code on failure */
        uint32_t decodeValue = 0; /* reported byte count, or the error code on failure */
        uint32_t packedHash;
        int packedGuardOk = 1;
        int unpackedGuardOk = 1;
        int same;
        if (!source || !packed || !unpacked) {
            Thandor_Log("codec selftest: allocation failed");
            return;
        }
        for (i = 0; i < size; i++) {
            seed = seed * 1103515245u + 12345u;
            source[i] = (noisy || (i % 4096) < 300) ? (uint8_t)(seed >> 16) : 0;
        }
        memset(packed, SELFTEST_GUARD_FILL, capacity + guard);
        memset(unpacked, SELFTEST_GUARD_FILL, size + guard);
        encodeOk = g_PckEncoderTable[0](capacity, packed, size, source, &encodeValue, &encodeValue);
        for (i = capacity; i < capacity + guard; i++) {
            if (packed[i] != SELFTEST_GUARD_FILL) { packedGuardOk = 0; break; }
        }
        /* FNV-1a over the packed bytes, to compare the encoder output of two builds */
        packedHash = 2166136261u;
        for (i = 0; encodeOk && i < encodeValue && i < capacity; i++) {
            packedHash = (packedHash ^ packed[i]) * 16777619u;
        }
        Thandor_Log("codec selftest %u: size=%x noisy=%d encode ok=%d packed=%x hash=%08X guard=%s", t, size,
                    noisy, encodeOk, encodeValue, packedHash, packedGuardOk ? "ok" : "OVERWRITTEN");
        if (encodeOk) {
            decodeOk = g_PckDecoderTable[0](size, unpacked, encodeValue, packed, &decodeValue, &decodeValue);
            for (i = size; i < size + guard; i++) {
                if (unpacked[i] != SELFTEST_GUARD_FILL) { unpackedGuardOk = 0; break; }
            }
            same = memcmp(source, unpacked, size) == 0;
            Thandor_Log("codec selftest %u: decode ok=%d value=%x roundtrip=%s guard=%s", t, decodeOk,
                        decodeValue, same ? "ok" : "MISMATCH", unpackedGuardOk ? "ok" : "OVERWRITTEN");
        }
        free(source);
        free(packed);
        free(unpacked);
    }
}


static void Thandor_SelfTestPathSplit(void)
{
    static const wchar_t *cases[4] = {L"C:\\Games\\ot-run\\thandor.exe", L"thandor.exe",
                                       L"C:\\Games\\ot-run\\save\\Mission 1.sve", L"C:\\"};
    int c;
    for (c = 0; c < 4; c++) {
        uint16_t path[WIDE_PATH_MAX_CODE_UNITS];
        uint16_t leaf[WIDE_PATH_MAX_CODE_UNITS];
        uint16_t parent[WIDE_PATH_MAX_CODE_UNITS];
        char leafA[WIDE_PATH_MAX_CODE_UNITS];
        char parentA[WIDE_PATH_MAX_CODE_UNITS];
        int i;
        memset(path, 0, sizeof path);
        memset(leaf, SELFTEST_UNWRITTEN_FILL, sizeof leaf);
        memset(parent, SELFTEST_UNWRITTEN_FILL, sizeof parent);
        for (i = 0; cases[c][i] != 0; i++) path[i] = (uint16_t)cases[c][i];
        WidePath_SplitParentAndLeaf(leaf, parent, path);
        for (i = 0; i < WIDE_PATH_MAX_CODE_UNITS - 1 && leaf[i] != 0; i++) leafA[i] = (char)leaf[i];
        leafA[i] = 0;
        for (i = 0; i < WIDE_PATH_MAX_CODE_UNITS - 1 && parent[i] != 0; i++) parentA[i] = (char)parent[i];
        parentA[i] = 0;
        Thandor_Log("path selftest %d: parent=\"%s\" leaf=\"%s\"", c, parentA, leafA);
    }
}


static void Thandor_SelfTestStretch(void)
{
    /* 4x2 ARGB source with a horizontal red ramp, stretched to 8x4. */
    /* header, the subresource record in the header's unused text, pixels behind the header */
    static uint32_t asset[2 * GFX_ASSET_HEADER_SIZE / sizeof(uint32_t)];
    static uint32_t target[8 * 4];
    uint32_t framebuffer[4] = {8, 0, 4, 0};
    GraphicsTextureSourceAsset *header = (GraphicsTextureSourceAsset *)asset;
    GraphicsTextureSourceEntry *entry;
    uint32_t *pixels;
    int x;
    int y;
    memset(asset, 0, sizeof asset);
    header->common.magic = ASSET_MAGIC_GFX;
    header->tableDescriptor.subresourceCount = 1;
    header->tableDescriptor.subresourceTableOffset = offsetof(GraphicsTextureSourceAsset, unusedText);
    entry = (GraphicsTextureSourceEntry *)header->unusedText;
    entry->paletteIndex = -1; /* direct ARGB8888 pixels */
    entry->dataOffset = GFX_ASSET_HEADER_SIZE;
    entry->pixelWidth = 4;
    entry->pixelHeight = 2;
    pixels = asset + GFX_ASSET_HEADER_SIZE / sizeof(uint32_t);
    for (y = 0; y < 2; y++) {
        for (x = 0; x < 4; x++) {
            pixels[y * 4 + x] = ARGB8888_ALPHA_MASK | ((uint32_t)(x * 85) << 16) | ((uint32_t)(y * 255) << 8);
        }
    }
    framebuffer[3] = (uint32_t)(uintptr_t)target;
    SoftwareTextureSource_StretchDirectColorBilinear32(4, 8, 0, 0, 0, header,
                                                       (SoftwareFramebufferAccess *)framebuffer);
    for (y = 0; y < 4; y++) {
        Thandor_Log("stretch selftest row %d: %08x %08x %08x %08x %08x %08x %08x %08x", y,
                    target[y * 8 + 0], target[y * 8 + 1], target[y * 8 + 2], target[y * 8 + 3],
                    target[y * 8 + 4], target[y * 8 + 5], target[y * 8 + 6], target[y * 8 + 7]);
    }
}

/* OPEN_THANDOR_SELFTEST=scanaddr decodes every entry of the packages next to the executable (all
   but FILME.PCK) and writes each aligned dword in the original image range 0x401000-0x58C000 to
   scanaddr.txt: package, entry path, type tag, offset, value. Used to find assets that store
   original code or data addresses. With OPEN_THANDOR_DUMPTEXT=<dir> it also writes every decoded
   text page (*.str, *.txt) to <dir>\<package>_<entry path>. */
/* The arena is set up by ProcessEntry; decoders called before that allocate through these. */
static uint32_t SelfTest_Alloc(uint32_t bytes, void **outPayload)
{
    void *payload = malloc(bytes);
    if (payload == NULL) {
        return FATAL_ERROR_ARENA_EXHAUSTED; /* a failed alloc must report a nonzero code */
    }
    *outPayload = payload;
    return 0;
}

static uint32_t SelfTest_Free(void *memory)
{
    free(memory);
    return 0;
}

/* OPEN_THANDOR_SELFTEST=codec, after the round trips: malformed method-0 and method-2 inputs (truncated
   stream, output size 0, fewer than two symbols, empty source, grid larger than its buffers) must fail
   without a crash; a uniform source must encode and round-trip. Logs one line per case and a summary. */
static void Thandor_SelfTestCodecNegative(void)
{
    uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
    uint32_t (*savedFree)(void *) = g_MemoryApi.free;
    const unsigned gridBytes = FIELD_GRID_HEADER_BYTES + 4 * FIELD_GRID_CELL_DWORDS * 4; /* 2x2 cells */
    const unsigned compactBytes = FIELD_GRID_HEADER_BYTES + 4 * FIELD_GRID_COMPACT_CELL_BYTES;
    std::vector<uint8_t> source(0x1000);
    std::vector<uint8_t> packed(0x4000);
    std::vector<uint8_t> unpacked(0x1000 + 0x10);
    std::vector<uint32_t> grid(gridBytes / 4);
    std::vector<uint32_t> compact(compactBytes / 4);
    std::vector<uint32_t> decodedGrid(gridBytes / 4);
    uint32_t value = 0;
    uint32_t packedSize = 0;
    unsigned failures = 0;
    unsigned seed = 777;
    unsigned i;
    Bool8 ok;

    g_MemoryApi.alloc = SelfTest_Alloc;
    g_MemoryApi.free = SelfTest_Free;
    for (i = 0; i < source.size(); i++) {
        seed = seed * 1103515245u + 12345u;
        source[i] = (uint8_t)(seed >> 16);
    }
#define CODEC_NEG_EXPECT(name, condition) \
    do { \
        int passed_ = (condition) ? 1 : 0; \
        failures += passed_ ? 0 : 1; \
        Thandor_Log("codec negative: %s %s", name, passed_ ? "ok" : "FAILED"); \
    } while (0)
    ok = PckCodec_EncodeHuffmanRle((uint32_t)packed.size(), packed.data(), (uint32_t)source.size(), source.data(),
                                   &packedSize, &value);
    CODEC_NEG_EXPECT("noisy encode", ok);
    CODEC_NEG_EXPECT("truncated stream",
                     !PckCodec_DecodeHuffmanRle((uint32_t)source.size(), unpacked.data(), packedSize / 2,
                                                packed.data(), &value, &value));
    CODEC_NEG_EXPECT("source shorter than the table",
                     !PckCodec_DecodeHuffmanRle((uint32_t)source.size(), unpacked.data(), 0x80, packed.data(),
                                                &value, &value));
    CODEC_NEG_EXPECT("output size 0",
                     !PckCodec_DecodeHuffmanRle(0, unpacked.data(), packedSize, packed.data(), &value, &value));
    /* only symbol 0x41 weighted: the original takes the last leaf as the root */
    memset(packed.data(), 0, PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 0x40);
    packed[0x41] = 5;
    CODEC_NEG_EXPECT("single-symbol table",
                     !PckCodec_DecodeHuffmanRle(0x10, unpacked.data(), PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 0x40,
                                                packed.data(), &value, &value));
    memset(packed.data(), 0, PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 0x40);
    CODEC_NEG_EXPECT("empty table",
                     !PckCodec_DecodeHuffmanRle(0x10, unpacked.data(), PCK_HUFFMAN_FREQUENCY_TABLE_BYTES + 0x40,
                                                packed.data(), &value, &value));
    CODEC_NEG_EXPECT("empty encode",
                     !PckCodec_EncodeHuffmanRle((uint32_t)packed.size(), packed.data(), 0, source.data(), &value,
                                                &value));
    /* a uniform source has one distinct byte value: the encoder adds a dummy second symbol */
    memset(source.data(), 0x5A, source.size());
    ok = PckCodec_EncodeHuffmanRle((uint32_t)packed.size(), packed.data(), (uint32_t)source.size(), source.data(),
                                   &packedSize, &value);
    ok = ok && PckCodec_DecodeHuffmanRle((uint32_t)source.size(), unpacked.data(), packedSize, packed.data(),
                                         &value, &value);
    CODEC_NEG_EXPECT("uniform round trip", ok && memcmp(source.data(), unpacked.data(), source.size()) == 0);
    /* field grid 2x2 (header dwords 0x2E/0x2F are gridWidth/gridHeight) */
    for (i = 0; i < grid.size(); i++) {
        grid[i] = i * 0x9E3779B9u;
    }
    grid[46] = 2;
    grid[47] = 2;
    ok = PckCodec_EncodeFieldGrid((uint32_t)packed.size(), packed.data(), gridBytes, (FieldGridAsset *)grid.data(),
                                  &packedSize, &value);
    CODEC_NEG_EXPECT("grid encode", ok);
    CODEC_NEG_EXPECT("grid decode",
                     PckCodec_DecodeFieldGrid(gridBytes, (FieldGridAsset *)decodedGrid.data(), packedSize,
                                              packed.data(), &value, &value) &&
                     decodedGrid[46] == 2 && decodedGrid[47] == 2);
    CODEC_NEG_EXPECT("grid capacity too small",
                     !PckCodec_DecodeFieldGrid(gridBytes - 1, (FieldGridAsset *)decodedGrid.data(), packedSize,
                                               packed.data(), &value, &value));
    CODEC_NEG_EXPECT("grid source shorter than the prefix",
                     !PckCodec_DecodeFieldGrid(gridBytes, (FieldGridAsset *)decodedGrid.data(), 8, packed.data(),
                                               &value, &value));
    /* a compact image of 4 cells whose header claims 100x100, and one of 0 cells */
    for (i = 0; i < compact.size(); i++) {
        compact[i] = i * 0x85EBCA6Bu;
    }
    compact[46] = 100;
    compact[47] = 100;
    *(uint32_t *)packed.data() = compactBytes;
    ok = PckCodec_EncodeHuffmanRle((uint32_t)packed.size() - PCK_FIELD_GRID_PREFIX_BYTES,
                                   packed.data() + PCK_FIELD_GRID_PREFIX_BYTES, compactBytes,
                                   (uint8_t *)compact.data(), &packedSize, &value);
    CODEC_NEG_EXPECT("grid larger than its image",
                     ok && !PckCodec_DecodeFieldGrid(0x7FFFFFFF, (FieldGridAsset *)decodedGrid.data(),
                                                     packedSize + PCK_FIELD_GRID_PREFIX_BYTES, packed.data(),
                                                     &value, &value));
    compact[46] = 0;
    ok = PckCodec_EncodeHuffmanRle((uint32_t)packed.size() - PCK_FIELD_GRID_PREFIX_BYTES,
                                   packed.data() + PCK_FIELD_GRID_PREFIX_BYTES, compactBytes,
                                   (uint8_t *)compact.data(), &packedSize, &value);
    CODEC_NEG_EXPECT("grid of 0 cells",
                     ok && !PckCodec_DecodeFieldGrid(gridBytes, (FieldGridAsset *)decodedGrid.data(),
                                                     packedSize + PCK_FIELD_GRID_PREFIX_BYTES, packed.data(),
                                                     &value, &value));
#undef CODEC_NEG_EXPECT
    Thandor_Log("codec negative: %s", failures == 0 ? "all ok" : "FAILURES");
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
}

/* OPEN_THANDOR_SELFTEST=pcx decodes pcxtest.pcx (next to the executable) with Pcx_DecodeIndexed8 and logs
   width, height and an FNV-1a hash over the palette (0xFFRRGGBB dwords, little endian) and the pixels;
   tools/test/pcx_check.py writes the file and prints the expected line. */
static void Thandor_SelfTestPcx(void)
{
    uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
    uint32_t (*savedFree)(void *) = g_MemoryApi.free;
    FILE *file = fopen("pcxtest.pcx", "rb");
    static uint8_t bytes[1 << 20];
    uint32_t byteCount;
    PcxIndexedImage image;
    uint32_t hash = 2166136261u;
    uint32_t i;
    if (file == NULL) {
        Thandor_Log("pcx: pcxtest.pcx missing");
        return;
    }
    byteCount = (uint32_t)fread(bytes, 1, sizeof bytes, file);
    fclose(file);
    g_MemoryApi.alloc = SelfTest_Alloc;
    g_MemoryApi.free = SelfTest_Free;
    if (!Pcx_DecodeIndexed8(bytes, byteCount, &image)) {
        Thandor_Log("pcx: rejected (%u bytes)", byteCount);
    }
    else {
        for (i = 0; i < PCX_PALETTE_COLOR_COUNT * 4; i++) {
            hash = (hash ^ ((const uint8_t *)image.paletteColors)[i]) * 16777619u;
        }
        for (i = 0; i < image.width * image.height; i++) {
            hash = (hash ^ image.pixels[i]) * 16777619u;
        }
        Thandor_Log("pcx: %ux%u hash %08X", image.width, image.height, hash);
        Pcx_FreeIndexed8(&image);
    }
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
}

/* OPEN_THANDOR_SELFTEST=movieenc encodes synthetic 64x48 frames (gradients, noise, flat areas, black/white
   extremes) with Movie_EncodeFrame4x4Keyframe and then Movie_EncodeFrame4x4Delta for frames that change in
   parts, decodes each with Movie_DecodeFrame4x4Delta, and logs the byte counts and an FNV-1a hash over every
   encoded byte, the reference frame the delta encoder keeps and the decoded picture. Run it with two builds to
   check that a rewrite of the encoders or the decoder kept their output. */
#define MOVIEENC_WIDTH 64
#define MOVIEENC_HEIGHT 48
#define MOVIEENC_FRAMES 6
static uint32_t SelfTest_MovieEncodePixel(uint32_t frame, uint32_t x, uint32_t y, uint32_t *seed)
{
    uint32_t region = (x / 16 + (y / 16) * 4 + frame) % 6;
    *seed = *seed * 1103515245u + 12345u;
    if (frame > 0 && ((x / 8 + y / 8 + frame) & 3) != 0) {
        region = (x / 16 + (y / 16) * 4) % 6; /* most blocks keep the content of frame 0 */
    }
    switch (region) {
    case 0:
        return 0xFF000000u | (x * 4) << 16 | (y * 5) << 8 | ((x + y) * 2);
    case 1:
        return 0xFF000000u | (*seed >> 8 & 0xFFFFFFu);
    case 2:
        return 0xFF406080u;
    case 3:
        return ((x ^ y) & 1) != 0 ? 0xFFFFFFFFu : 0xFF000000u;
    case 4:
        return 0xFF000000u | ((*seed >> 16 & 15) + 120) * 0x010101u;
    default:
        return 0xFF000000u | (255 - x * 4) << 16 | (frame * 40 & 255) << 8 | (y * 5);
    }
}

static void Thandor_SelfTestMovieEncode(void)
{
    static uint32_t reference[MOVIEENC_WIDTH * MOVIEENC_HEIGHT];
    static uint32_t current[MOVIEENC_WIDTH * MOVIEENC_HEIGHT];
    static uint32_t encoded[MOVIEENC_WIDTH * MOVIEENC_HEIGHT * 2];
    static uint32_t decoded[MOVIEENC_WIDTH * MOVIEENC_HEIGHT];
    uint32_t consumed;
    uint32_t hash = 2166136261u;
    uint32_t seed = 1;
    uint32_t frame;
    uint32_t i;
    uint32_t byteCount;
    for (frame = 0; frame < MOVIEENC_FRAMES; frame++) {
        uint32_t *pixels = frame == 0 ? reference : current;
        for (i = 0; i < MOVIEENC_WIDTH * MOVIEENC_HEIGHT; i++) {
            pixels[i] = SelfTest_MovieEncodePixel(frame, i % MOVIEENC_WIDTH, i / MOVIEENC_WIDTH, &seed);
        }
        memset(encoded, 0xCD, sizeof encoded);
        if (frame == 0) {
            byteCount = Movie_EncodeFrame4x4Keyframe(MOVIEENC_HEIGHT, MOVIEENC_WIDTH, encoded, reference);
        }
        else {
            byteCount = Movie_EncodeFrame4x4Delta(MOVIEENC_HEIGHT, MOVIEENC_WIDTH, encoded, reference, current);
        }
        for (i = 0; i < byteCount && i < sizeof encoded; i++) {
            hash = (hash ^ ((const uint8_t *)encoded)[i]) * 16777619u;
        }
        for (i = 0; i < sizeof reference; i++) {
            hash = (hash ^ ((const uint8_t *)reference)[i]) * 16777619u;
        }
        /* decode the frame on top of the previous decoded picture, as the player does */
        consumed = Movie_DecodeFrame4x4Delta(MOVIEENC_HEIGHT, MOVIEENC_WIDTH, decoded, (uint8_t *)encoded);
        hash = (hash ^ consumed) * 16777619u;
        for (i = 0; i < sizeof decoded; i++) {
            hash = (hash ^ ((const uint8_t *)decoded)[i]) * 16777619u;
        }
        Thandor_Log("movieenc: frame %u %u bytes (decoder %u), hash so far %08X", frame, byteCount, consumed, hash);
    }
    Thandor_Log("movieenc: hash %08X", hash);
}

/* OPEN_THANDOR_SELFTEST=trianglesetup runs SoftwareRenderer_PrepareTrianglePacket (vertex sort by screen Y,
   pixel snapping, depth epoch, texture coordinate scaling) on 20000 random triangles - a quarter of them with
   equal Y values, so the tie cases of the sort are hit - and logs an FNV-1a hash over every prepared packet.
   Run it with two builds to check that a rewrite of the setup kept its output. */
static uint32_t SelfTest_TriangleRandom(uint32_t *seed)
{
    *seed = *seed * 1103515245u + 12345u;
    return *seed >> 8;
}

static void Thandor_SelfTestTriangleSetup(void)
{
    static GraphicsPrimitivePacket packet;
    uint32_t textureEntry[8];
    uint32_t seed = 4711;
    uint32_t hash = 2166136261u;
    uint32_t caseIndex;
    uint32_t i;
    int vertexIndex;
    for (caseIndex = 0; caseIndex < 20000; caseIndex++) {
        memset(textureEntry, 0, sizeof textureEntry);
        textureEntry[1] = 3 + SelfTest_TriangleRandom(&seed) % 6;
        textureEntry[2] = 3 + SelfTest_TriangleRandom(&seed) % 6;
        memset(&packet, 0, sizeof packet);
        for (vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
            GraphicsPrimitiveVertexRaw *v = &packet.vertices[vertexIndex];
            int y = (int)(SelfTest_TriangleRandom(&seed) % 900) - 100;
            if ((caseIndex & 3) == 0 && vertexIndex > 0 && (SelfTest_TriangleRandom(&seed) & 1) != 0) {
                v->screenY = packet.vertices[vertexIndex - 1].screenY; /* tie with the previous vertex */
            }
            else {
                v->screenY = (y << 12) | (int)(SelfTest_TriangleRandom(&seed) & 0xfff);
            }
            {
                /* two draws in one expression: their order is spelled out (operand order is unspecified) */
                int screenXWhole = (int)(SelfTest_TriangleRandom(&seed) % 1500) - 200;
                int screenXFraction = (int)(SelfTest_TriangleRandom(&seed) & 0xfff);
                v->screenX = (screenXWhole << 12) | screenXFraction;
            }
            v->depth = 0x20000000 + (int)(SelfTest_TriangleRandom(&seed) % 0x400000u) * 256;
            v->textureU = (int)(SelfTest_TriangleRandom(&seed) % 0x200000u) - 0x80000;
            v->textureV = (int)(SelfTest_TriangleRandom(&seed) % 0x200000u) - 0x80000;
            v->diffuseColor = SelfTest_TriangleRandom(&seed) * 257u;
        }
        packet.modulationColor = SelfTest_TriangleRandom(&seed);
        packet.textureEntry = (GraphicsTextureSetEntry *)textureEntry;
        packet.renderFlags = (GraphicsPrimitiveDispatchFlags)((SelfTest_TriangleRandom(&seed) % 32) << 12);
        g_SoftwareDepthEpoch = (int32_t)(SelfTest_TriangleRandom(&seed) % 0x1000000u);
        SoftwareRenderer_PrepareTrianglePacket(&packet);
        packet.textureEntry = NULL; /* the pointer differs between runs */
        for (i = 0; i < sizeof packet; i++) {
            hash = (hash ^ ((const uint8_t *)&packet)[i]) * 16777619u;
        }
    }
    Thandor_Log("trianglesetup: 20000 triangles, hash %08X", hash);
}

/* OPEN_THANDOR_SELFTEST=keymap sends every virtual key 0..255 through Keyboard_OnKeyDown / Keyboard_OnKeyUp,
   alone and with each modifier (Shift, Ctrl, Alt, Caps/Num/Scroll Lock) held, reads the queued events with
   Keyboard_ReadNextEvent and logs an FNV-1a hash over the event codes, their state masks, g_KeyboardStateMask and
   g_KeyboardSpecialKeyDown after every step. Run it with two builds to check that a rewrite of the key mapping
   kept it. */
static uint32_t SelfTest_KeymapDrain(uint32_t hash)
{
    uint32_t keyCode;
    uint32_t stateMask;
    uint32_t i;
    while (Keyboard_ReadNextEvent(&keyCode, &stateMask)) {
        hash = (hash ^ keyCode) * 16777619u;
        hash = (hash ^ stateMask) * 16777619u;
    }
    hash = (hash ^ g_KeyboardStateMask) * 16777619u;
    for (i = 0; i < sizeof g_KeyboardSpecialKeyDown; i++) {
        hash = (hash ^ g_KeyboardSpecialKeyDown[i]) * 16777619u;
    }
    return hash;
}

static void Thandor_SelfTestKeymap(void)
{
    static const uint32_t modifiers[] = {0, 0x10, 0x11, 0x12, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x14, 0x90, 0x91};
    uint32_t hash = 2166136261u;
    uint32_t modifierIndex;
    uint32_t virtualKey;
    for (modifierIndex = 0; modifierIndex < sizeof modifiers / sizeof modifiers[0]; modifierIndex++) {
        if (modifiers[modifierIndex] != 0) {
            Keyboard_OnKeyDown(modifiers[modifierIndex]);
            hash = SelfTest_KeymapDrain(hash);
        }
        for (virtualKey = 0; virtualKey < 256; virtualKey++) {
            Keyboard_OnKeyDown(virtualKey);
            hash = SelfTest_KeymapDrain(hash);
            Keyboard_OnKeyUp(virtualKey);
            hash = SelfTest_KeymapDrain(hash);
        }
        if (modifiers[modifierIndex] != 0) {
            Keyboard_OnKeyUp(modifiers[modifierIndex]);
            hash = SelfTest_KeymapDrain(hash);
        }
    }
    Thandor_Log("keymap: hash %08X", hash);
}

/* OPEN_THANDOR_SELFTEST=fixedmath feeds random and edge-case inputs to the fixed-point helpers the simulation uses
   (Atan2Angle16, SqrtQ12Approx, WriteDirectionScaled, InvertRigidQ28, SolveTriangleJointAngles, Length3,
   Vector2AngleAndLength) and logs an FNV-1a hash per function over all results. Run it with two builds to check
   that a rewrite kept every result bit. */
static uint32_t SelfTest_FixedRandom(uint32_t *seed)
{
    *seed = *seed * 1664525u + 1013904223u;
    return *seed;
}

static uint32_t SelfTest_HashBytes(uint32_t hash, const void *bytes, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++) {
        hash = (hash ^ ((const uint8_t *)bytes)[i]) * 16777619u;
    }
    return hash;
}

static void Thandor_SelfTestFixedMath(void)
{
    static const int32_t edges[] = {0, 1, -1, 2, -2, 0x7fffffff, (int32_t)0x80000000, 0x1000, -0x1000, 0x10000,
                                    0xffff, 0x7fff, -0x8000, 0x40000000, -0x40000000};
    uint32_t seed = 99;
    uint32_t hashAtan = 2166136261u, hashSqrt = 2166136261u, hashDirection = 2166136261u;
    uint32_t hashInvert = 2166136261u, hashTriangle = 2166136261u, hashLength = 2166136261u;
    uint32_t hashAngleLength = 2166136261u;
    uint32_t i;
    uint32_t j;
    for (i = 0; i < 200000; i++) {
        int32_t a = (int32_t)SelfTest_FixedRandom(&seed);
        int32_t b = (int32_t)SelfTest_FixedRandom(&seed);
        int32_t c = (int32_t)SelfTest_FixedRandom(&seed);
        uint32_t result;
        GraphicsFixedVec3 direction;
        FixedLengthAngle angleAndLength;
        if (i < 15 * 15) { /* every pair of edge values first */
            a = edges[i / 15];
            b = edges[i % 15];
        }
        else if ((i & 3) == 1) { /* small values, where rounding matters most */
            a >>= 16;
            b >>= 16;
            c >>= 16;
        }
        result = FixedMath_Atan2Angle16(a, b);
        hashAtan = SelfTest_HashBytes(hashAtan, &result, 4);
        result = FixedMath_SqrtQ12Approx((uint32_t)a);
        hashSqrt = SelfTest_HashBytes(hashSqrt, &result, 4);
        memset(&direction, 0, sizeof direction);
        FixedMath_WriteDirectionScaled(&direction, (AngleTurn32)a, (AngleTurn32)b, (FixedMathScale32)(c >> 4));
        hashDirection = SelfTest_HashBytes(hashDirection, &direction, sizeof direction);
        result = FixedMath_Length3(a >> 2, b >> 2, c >> 2);
        hashLength = SelfTest_HashBytes(hashLength, &result, 4);
        angleAndLength = FixedMath_Vector2AngleAndLength(a >> 1, b >> 1);
        hashAngleLength = SelfTest_HashBytes(hashAngleLength, &angleAndLength, sizeof angleAndLength);
        if ((i & 7) == 0) {
            GraphicsFixedMatrix3x4 input;
            GraphicsFixedMatrix3x4 output;
            for (j = 0; j < sizeof input / 4; j++) {
                ((int32_t *)&input)[j] = (int32_t)SelfTest_FixedRandom(&seed) >> (j % 3 == 0 ? 2 : 4);
            }
            memset(&output, 0, sizeof output);
            FixedTransform_InvertRigidQ28(&output, &input);
            hashInvert = SelfTest_HashBytes(hashInvert, &output, sizeof output);
        }
        if ((i & 3) == 0) {
            /* a valid triangle: two random sides and a third between their difference and their sum */
            uint32_t side0 = 0x1000 + SelfTest_FixedRandom(&seed) % 0x200000u;
            uint32_t side1 = 0x1000 + SelfTest_FixedRandom(&seed) % 0x200000u;
            uint32_t low = side0 > side1 ? side0 - side1 : side1 - side0;
            uint32_t side2 = low + 1 + SelfTest_FixedRandom(&seed) % (side0 + side1 - low - 1);
            FixedTriangleJointAngles angles = FixedGeometry_SolveTriangleJointAngles((Q12)side0, (Q12)side1, (Q12)side2);
            hashTriangle = SelfTest_HashBytes(hashTriangle, &angles, sizeof angles);
        }
    }
    Thandor_Log("fixedmath: atan2 hash %08X, sqrt hash %08X, direction hash %08X, invert hash %08X",
                hashAtan, hashSqrt, hashDirection, hashInvert);
    Thandor_Log("fixedmath: triangle hash %08X, length3 hash %08X, angle/length hash %08X",
                hashTriangle, hashLength, hashAngleLength);
}

/* OPEN_THANDOR_SELFTEST=numberformat formats random and edge values with WideNumber_FormatUtf16 under random flag
   combinations, digit counts and denominators (into a zeroed 256-unit buffer) and logs an FNV-1a hash over the
   returned lengths and every buffer. Run it with two builds to check that a rewrite kept the formatting. */
static void Thandor_SelfTestNumberFormat(void)
{
    static const int32_t edges[] = {0, 1, -1, 9, 10, 99, 100, 999, 1000, 1234, -1234, 12345, 999999, 1000000,
                                    0x7fffffff, (int32_t)0x80000000, 0x7fff, -0x8000};
    static uint16_t buffer[256];
    uint32_t seed = 7;
    uint32_t hash = 2166136261u;
    uint32_t i;
    for (i = 0; i < 40000; i++) {
        uint32_t flags;
        uint32_t fractionalDigits;
        uint32_t integerDigitLimit;
        uint32_t denominator;
        int32_t value;
        uint32_t written;
        flags = SelfTest_FixedRandom(&seed) & 0x7f;
        fractionalDigits = SelfTest_FixedRandom(&seed) % 5;
        integerDigitLimit = 1 + SelfTest_FixedRandom(&seed) % 10;
        denominator = (SelfTest_FixedRandom(&seed) & 3) == 0 ? 1 : 1 + SelfTest_FixedRandom(&seed) % 1000;
        if (i < 18 * 4) {
            value = edges[i % 18];
        }
        else {
            /* two draws: the shift count first (the order the C build evaluated them in) */
            uint32_t shift = SelfTest_FixedRandom(&seed) % 31;
            value = (int32_t)SelfTest_FixedRandom(&seed) >> shift;
        }
        memset(buffer, 0, sizeof buffer);
        written = WideNumber_FormatUtf16((WideNumberFormatFlags)flags, fractionalDigits, integerDigitLimit, denominator,
                                         value, buffer);
        hash = SelfTest_HashBytes(hash, &written, 4);
        hash = SelfTest_HashBytes(hash, buffer, sizeof buffer);
    }
    Thandor_Log("numberformat: 40000 numbers, hash %08X", hash);
}

/* settings: compares a reference image (loaded bytes = referenceMask) with an image parsed from ini text. Every
   dword the reference has must come back with the same value; a dword the ini leaves out must be zero there
   (reserved dwords are only written when nonzero). Logs the first difference; returns the number of them. */
static unsigned SelfTest_CompareSettingsImages(const char *what, const uint8_t *reference, uint64_t referenceMask,
                                               const uint8_t *parsed, uint64_t parsedMask)
{
    unsigned differences = 0;
    unsigned dword;
    for (dword = 0; dword < PERSISTENT_SETTINGS_IMAGE_BYTES / 4; dword++) {
        uint64_t bit = (uint64_t)1 << dword;
        uint32_t expected;
        uint32_t actual;
        memcpy(&expected, reference + dword * 4, 4);
        memcpy(&actual, parsed + dword * 4, 4);
        if ((referenceMask & bit) == 0) {
            if ((parsedMask & bit) != 0) {
                if (differences++ == 0) {
                    Thandor_Log("settings: %s: offset 0x%02X present, not in the reference", what, dword * 4);
                }
            }
            continue;
        }
        if ((parsedMask & bit) == 0 ? expected != 0 : expected != actual) {
            if (differences++ == 0) {
                Thandor_Log("settings: %s: offset 0x%02X %s %08X, expected %08X", what, dword * 4,
                            (parsedMask & bit) != 0 ? "is" : "missing,", actual, expected);
            }
        }
    }
    return differences;
}

/* OPEN_THANDOR_SELFTEST=settings: the thandor.ini format. Parses a fixed ini text (all value forms, unknown and
   missing keys) and checks the image; then, when thandor.dat is in the current directory, writes its image as
   ini text, parses that back and compares (the migration), and when thandor.ini is there too, compares it
   with thandor.dat (an ini the game wrote from that thandor.dat). Logs one line per check. */
static void Thandor_SelfTestSettings(void)
{
    static const char fixedText[] =
        "\xEF\xBB\xBF; comment\r\n[display]\r\nwidth = 1024\r\nheight=768 ; trailing comment\r\nrenderer = D3D12\r\n"
        "display_mode = 2\r\nunknown_key = 5\r\n[graphics]\r\nshading = off\r\ntexture_quality = low\r\n"
        "model_detail = -3\r\n[sound]\r\nmusic = false\r\neffects_volume = 50%\r\nmusic_volume = 0x4000\r\n"
        "movie_volume = 37.5 %\r\n[game]\r\nplayer_name = \"Ren\xC3\xA9 \xF0\x9F\x99\x82\"\r\n"
        "game_name = abcdefghijklmnopqrstuvwxyz\r\nmap_mouse_options = 0x5\r\nspeed_percent = oops\r\n"
        "[nosection]\r\nplayers = 7\r\n";
    uint8_t image[PERSISTENT_SETTINGS_IMAGE_BYTES];
    uint8_t expected[PERSISTENT_SETTINGS_IMAGE_BYTES];
    uint8_t parsed[PERSISTENT_SETTINGS_IMAGE_BYTES];
    uint64_t expectedMask = 0;
    uint64_t mask;
    static char text[0x4000];
    uint32_t length;
    unsigned differences;
    static const uint16_t playerName[] = {'R', 'e', 'n', 0xE9, ' ', 0xD83D, 0xDE42};
    FILE *file;

    memset(image, 0, sizeof image);
    memset(expected, 0, sizeof expected);
#define SELFTEST_SETTING(offset, value) do { uint32_t v_ = (uint32_t)(value); memcpy(expected + (offset), &v_, 4); \
        expectedMask |= (uint64_t)1 << ((offset) / 4); } while (0)
    SELFTEST_SETTING(PERSISTENT_SETTING_DISPLAY_WIDTH, 1024);
    SELFTEST_SETTING(PERSISTENT_SETTING_DISPLAY_HEIGHT, 768);
    SELFTEST_SETTING(PERSISTENT_SETTING_RENDERER, 1);
    SELFTEST_SETTING(PERSISTENT_SETTING_DISPLAY_MODE_KIND, 2);
    SELFTEST_SETTING(PERSISTENT_SETTING_SHADING_ENABLED, 0);
    SELFTEST_SETTING(PERSISTENT_SETTING_TEXTURE_QUALITY, TEXTURE_QUALITY_LOW);
    SELFTEST_SETTING(PERSISTENT_SETTING_MODEL_LOD_DEPTH_THRESHOLD, -3);
    SELFTEST_SETTING(PERSISTENT_SETTING_SOUND_OPTION_FLAGS, PERSISTENT_SOUND_OPTION_EFFECTS); /* default 3 minus music */
    SELFTEST_SETTING(PERSISTENT_SETTING_EFFECTS_GAIN, 0x4000);
    SELFTEST_SETTING(PERSISTENT_SETTING_MUSIC_GAIN, 0x4000);
    SELFTEST_SETTING(PERSISTENT_SETTING_MOVIE_DEFAULT_GAIN, 0x3000);
    SELFTEST_SETTING(PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS, 5);
#undef SELFTEST_SETTING
    memcpy(expected + PERSISTENT_SETTING_PLAYER_NAME, playerName, sizeof playerName);
    memcpy(expected + PERSISTENT_SETTING_GAME_NAME, "a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0", 40);
    expectedMask |= (((uint64_t)1 << 20) - 1) << (PERSISTENT_SETTING_PLAYER_NAME / 4);
    mask = PersistentSettings_ParseIni(fixedText, (uint32_t)(sizeof fixedText - 1), image);
    differences = SelfTest_CompareSettingsImages("fixed text", expected, expectedMask, image, mask);
    if (mask != expectedMask) {
        differences++;
    }
    /* and back: the ini written from it parses to the same image */
    length = PersistentSettings_FormatIni(image, mask, text, sizeof text);
    memset(parsed, 0, sizeof parsed);
    differences += SelfTest_CompareSettingsImages("fixed text round trip", image, mask, parsed,
                                                  PersistentSettings_ParseIni(text, length, parsed));
    Thandor_Log("settings: fixed text %s (present mask %016llX)", differences == 0 ? "ok" : "MISMATCH",
                (unsigned long long)mask);

    file = fopen("thandor.dat", "rb");
    if (file == NULL) {
        Thandor_Log("settings: no thandor.dat in the current directory, migration not checked");
        return;
    }
    {
        uint8_t datImage[PERSISTENT_SETTINGS_IMAGE_BYTES];
        size_t datBytes;
        uint64_t datMask = 0;
        unsigned dword;
        memset(datImage, 0, sizeof datImage);
        datBytes = fread(datImage, 1, sizeof datImage, file);
        fclose(file);
        for (dword = 0; (dword + 1) * 4 <= datBytes; dword++) {
            datMask |= (uint64_t)1 << dword;
        }
        length = PersistentSettings_FormatIni(datImage, datMask, text, sizeof text);
        memset(parsed, 0, sizeof parsed);
        differences = SelfTest_CompareSettingsImages("thandor.dat round trip", datImage, datMask, parsed,
                                                     PersistentSettings_ParseIni(text, length, parsed));
        Thandor_Log("settings: thandor.dat (%u bytes) -> ini (%u bytes) -> image: %s", (unsigned)datBytes,
                    length, differences == 0 ? "identical" : "MISMATCH");
        file = fopen("thandor.ini", "rb");
        if (file == NULL) {
            Thandor_Log("settings: no thandor.ini in the current directory");
            return;
        }
        length = (uint32_t)fread(text, 1, sizeof text, file);
        fclose(file);
        memset(parsed, 0, sizeof parsed);
        differences = SelfTest_CompareSettingsImages("thandor.ini vs thandor.dat", datImage, datMask, parsed,
                                                     PersistentSettings_ParseIni(text, length, parsed));
        Thandor_Log("settings: thandor.ini vs thandor.dat: %s", differences == 0 ? "identical" : "DIFFERENT");
    }
}

static void Thandor_SelfTestScanAddresses(void)
{
    uint32_t (*savedAlloc)(uint32_t, void **) = g_MemoryApi.alloc;
    uint32_t (*savedFree)(void *) = g_MemoryApi.free;
    static const char *packages[] = {"DATEN.PCK", "ENGINE.PCK", "GRAPHIK.PCK", "LEVEL.PCK",
                                     "MODELLE.PCK", "PATCH00.PCK", "PATCH01.PCK", "SOUND.PCK"};
    unsigned p;
    FILE *out = fopen("scanaddr.txt", "w");
    uint8_t *packed = (uint8_t *)malloc(PACKAGE_SCRATCH_BUFFER_BYTES);
    unsigned totalEntries = 0;
    unsigned totalHits = 0;
    if (out == NULL || packed == NULL) {
        Thandor_Log("scanaddr: setup failed");
        return;
    }
    g_MemoryApi.alloc = SelfTest_Alloc;
    g_MemoryApi.free = SelfTest_Free;
    /* OPEN_THANDOR_SCANFILES=a;b;... scans those package-format files (e.g. saves) instead. */
    const char *extra = getenv("OPEN_THANDOR_SCANFILES");
    static char extraNames[32][260];
    const char *list[32];
    unsigned listCount = 0;
    if (extra != NULL) {
        const char *cursor = extra;
        while (*cursor != 0 && listCount < 32) {
            unsigned n = 0;
            while (*cursor != 0 && *cursor != ';' && n < 259) extraNames[listCount][n++] = *cursor++;
            extraNames[listCount][n] = 0;
            if (*cursor == ';') cursor++;
            if (n != 0) { list[listCount] = extraNames[listCount]; listCount++; }
        }
    }
    else {
        for (p = 0; p < sizeof packages / sizeof packages[0]; p++) list[listCount++] = packages[p];
    }
    for (p = 0; p < listCount; p++) {
        FILE *pck;
        long position = PCK_ENTRY_HEADER_BYTES;
        pck = fopen(list[p], "rb");
        if (pck == NULL) {
            continue;
        }
        for (;;) {
            PckEntryHeader header;
            uint8_t *unpacked;
            Bool8 decoded;
            char name[PCK_ENTRY_PATH_UNITS + 1];
            int k;
            uint32_t i;
            if (fseek(pck, position, SEEK_SET) != 0 || fread(&header, sizeof header, 1, pck) != 1) {
                break;
            }
            if (header.packedSize == 0 || header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES || header.unpackedSize > SCANADDR_MAX_UNPACKED_BYTES ||
                (uint32_t)header.compressionMethod >= sizeof g_PckDecoderTable / sizeof g_PckDecoderTable[0]) {
                break;
            }
            for (k = 0; k < PCK_ENTRY_PATH_UNITS && header.path[k] != 0; k++) {
                name[k] = (char)header.path[k];
            }
            name[k] = 0;
            if (fread(packed, 1, header.packedSize, pck) != header.packedSize) {
                break;
            }
            unpacked = (uint8_t *)malloc(header.unpackedSize + 4);
            if (unpacked == NULL) {
                break;
            }
            if (g_PckDecoderTable[header.compressionMethod] == NULL) {
                fprintf(out, "%s %s NO-DECODER method %u\n", list[p], name, (uint32_t)header.compressionMethod);
                free(unpacked);
                position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
                continue;
            }
            decoded = g_PckDecoderTable[header.compressionMethod]
                          (header.unpackedSize, unpacked, header.packedSize, packed, NULL, NULL);
            totalEntries++;
            if (!decoded) {
                fprintf(out, "%s %s DECODE-FAILED\n", list[p], name);
            }
            else {
                const char *dumpDirectory = getenv("OPEN_THANDOR_DUMPTEXT");
                size_t nameLength = strlen(name);
                if (dumpDirectory != NULL && nameLength > 4 &&
                    (_stricmp(name + nameLength - 4, ".str") == 0 || _stricmp(name + nameLength - 4, ".txt") == 0)) {
                    /* <dir>\<package>_<entry path with '\' as '_'> holds the decoded entry */
                    char dumpPath[600];
                    FILE *dump;
                    int j;
                    sprintf(dumpPath, "%s\\%s_%s", dumpDirectory, list[p], name);
                    for (j = (int)strlen(dumpDirectory) + 1; dumpPath[j] != 0; j++) {
                        if (dumpPath[j] == '\\' || dumpPath[j] == '/') {
                            dumpPath[j] = '_';
                        }
                    }
                    dump = fopen(dumpPath, "wb");
                    if (dump != NULL) {
                        fwrite(unpacked, 1, header.unpackedSize, dump);
                        fclose(dump);
                    }
                }
                for (i = 0; i + 4 <= header.unpackedSize; i += 4) {
                    uint32_t value = *(uint32_t *)(unpacked + i);
                    if ((value >= ORIGINAL_TEXT_START && value < ORIGINAL_TEXT_END) ||
                        (value >= REBUILT_IMAGE_BASE && value < REBUILT_IMAGE_BASE + SCANADDR_REBUILT_IMAGE_SPAN)) {
                        fprintf(out, "%s %s %08x %x %08x\n", list[p], name, (uint32_t)header.typeTag, i, value);
                        totalHits++;
                    }
                }
            }
            free(unpacked);
            position += PCK_ENTRY_HEADER_BYTES + (long)header.packedSize;
        }
        fclose(pck);
    }
    fclose(out);
    free(packed);
    g_MemoryApi.alloc = savedAlloc;
    g_MemoryApi.free = savedFree;
    Thandor_Log("scanaddr: %u entries decoded, %u dwords in the original image range", totalEntries, totalHits);
}

/* OPEN_THANDOR_SELFTEST=icon: the .ico parser of the window icon (platform/sdl3/window_icon.cpp) on a synthetic
   icon file built here: 32-bit with alpha, 4-bit with palette and mask, 32-bit without alpha (the mask decides),
   24-bit 32x32, an 8-bit image of a size already present, a PNG entry and an entry outside the file; then the
   choice of the 100% image and of the alternate sizes, and two files that are no icons. Logs every failed check
   and a summary line ("icon: ok, N checks" or "icon: N of M checks FAILED"). */
namespace {

struct IconTestImage {
    int width;
    int height;
    int bitsPerPixel;
    std::vector<uint8_t> dib; /* BITMAPINFOHEADER, palette, XOR bitmap, AND mask */
};

void IconTest_PutU16(std::vector<uint8_t> &bytes, uint32_t value)
{
    bytes.push_back((uint8_t)value);
    bytes.push_back((uint8_t)(value >> 8));
}

void IconTest_PutU32(std::vector<uint8_t> &bytes, uint32_t value)
{
    IconTest_PutU16(bytes, value & 0xFFFF);
    IconTest_PutU16(bytes, value >> 16);
}

/* A DIB entry: the header (height doubled), the palette, then the given bitmap and mask rows (already bottom-up
   and padded). */
IconTestImage IconTest_Dib(int width, int height, int bitsPerPixel, const std::vector<uint32_t> &palette,
                           const std::vector<uint8_t> &colourRows, const std::vector<uint8_t> &maskRows)
{
    IconTestImage image = {width, height, bitsPerPixel, {}};
    IconTest_PutU32(image.dib, 40);
    IconTest_PutU32(image.dib, (uint32_t)width);
    IconTest_PutU32(image.dib, (uint32_t)height * 2);
    IconTest_PutU16(image.dib, 1);
    IconTest_PutU16(image.dib, (uint32_t)bitsPerPixel);
    IconTest_PutU32(image.dib, 0);
    IconTest_PutU32(image.dib, (uint32_t)(colourRows.size() + maskRows.size()));
    IconTest_PutU32(image.dib, 0);
    IconTest_PutU32(image.dib, 0);
    IconTest_PutU32(image.dib, (uint32_t)palette.size());
    IconTest_PutU32(image.dib, 0);
    for (uint32_t colour : palette) {
        IconTest_PutU32(image.dib, colour); /* B, G, R, 0 */
    }
    image.dib.insert(image.dib.end(), colourRows.begin(), colourRows.end());
    image.dib.insert(image.dib.end(), maskRows.begin(), maskRows.end());
    return image;
}

/* The icon file: header, directory, images; entry outsideEntry (if >= 0) points behind the end of the file. */
std::vector<std::byte> IconTest_File(const std::vector<IconTestImage> &images, int outsideEntry)
{
    std::vector<uint8_t> bytes;
    IconTest_PutU16(bytes, 0);
    IconTest_PutU16(bytes, 1);
    IconTest_PutU16(bytes, (uint32_t)images.size());
    uint32_t offset = (uint32_t)(6 + 16 * images.size());
    for (size_t index = 0; index < images.size(); index++) {
        const IconTestImage &image = images[index];
        bytes.push_back((uint8_t)image.width);
        bytes.push_back((uint8_t)image.height);
        bytes.push_back(0);
        bytes.push_back(0);
        IconTest_PutU16(bytes, 1);
        IconTest_PutU16(bytes, (uint32_t)image.bitsPerPixel);
        IconTest_PutU32(bytes, (uint32_t)image.dib.size());
        IconTest_PutU32(bytes, ((int)index == outsideEntry) ? 0x7FFFFFF0u : offset);
        offset += (uint32_t)image.dib.size();
    }
    for (const IconTestImage &image : images) {
        bytes.insert(bytes.end(), image.dib.begin(), image.dib.end());
    }
    std::vector<std::byte> file(bytes.size());
    memcpy(file.data(), bytes.data(), bytes.size());
    return file;
}

} // namespace

static void Thandor_SelfTestIcon(void)
{
    using thandor::sdl3::IconFile;
    unsigned checks = 0;
    unsigned failures = 0;
#define ICON_CHECK(condition, ...) do { checks++; if (!(condition)) { failures++; Thandor_Log("icon: " __VA_ARGS__); } } while (0)
    std::vector<IconTestImage> images;
    /* 0: 2x2 32-bit with alpha, top row (0x80112233, 0xFF445566), bottom row (0x00778899, 0x40AABBCC); the mask
       (all transparent) must not count */
    images.push_back(IconTest_Dib(2, 2, 32, {},
                                  {0x99, 0x88, 0x77, 0x00, 0xCC, 0xBB, 0xAA, 0x40,
                                   0x33, 0x22, 0x11, 0x80, 0x66, 0x55, 0x44, 0xFF},
                                  {0xC0, 0, 0, 0, 0xC0, 0, 0, 0}));
    /* 1: 3x2 4-bit, palette (0x000000FF blue, 0x0000FF00 green), top row 1 0 1, bottom row 0 1 0; mask: top
       row middle pixel transparent */
    images.push_back(IconTest_Dib(3, 2, 4, {0x000000FF, 0x0000FF00},
                                  {0x01, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00},
                                  {0x00, 0, 0, 0, 0x40, 0, 0, 0}));
    /* 2: 2x1 32-bit without alpha: the mask decides (left opaque, right transparent) */
    images.push_back(IconTest_Dib(2, 1, 32, {}, {0x10, 0x20, 0x30, 0x00, 0x40, 0x50, 0x60, 0x00},
                                  {0x40, 0, 0, 0}));
    /* 3: 32x32 24-bit, all 0x123456, no mask bits */
    {
        std::vector<uint8_t> rows;
        for (int pixel = 0; pixel < 32 * 32; pixel++) {
            rows.push_back(0x56);
            rows.push_back(0x34);
            rows.push_back(0x12);
        }
        images.push_back(IconTest_Dib(32, 32, 24, {}, rows, std::vector<uint8_t>(32 * 4, 0)));
    }
    /* 4: 2x2 8-bit (a size already present with 32 bits) */
    images.push_back(IconTest_Dib(2, 2, 8, {0x00FFFFFF}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}));
    /* 5: a PNG image (only the signature counts) */
    {
        IconTestImage png = {16, 16, 32, {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A, 0, 0, 0, 0}};
        images.push_back(png);
    }
    /* 6: an entry outside the file */
    images.push_back(IconTest_Dib(1, 1, 32, {}, {0, 0, 0, 0xFF}, {0, 0, 0, 0}));
    const std::vector<std::byte> file = IconTest_File(images, 6);

    IconFile icon;
    ICON_CHECK(thandor::sdl3::DecodeIcoFile(file, icon), "the synthetic file is not taken as an icon file");
    ICON_CHECK(icon.images.size() == 5, "%u images decoded, expected 5", (unsigned)icon.images.size());
    ICON_CHECK(icon.skippedPngImages == 1, "%d PNG images skipped, expected 1", icon.skippedPngImages);
    ICON_CHECK(icon.skippedInvalidImages == 1, "%d broken images skipped, expected 1", icon.skippedInvalidImages);
    if (icon.images.size() == 5) {
        static const uint32_t expected0[4] = {0x80112233, 0xFF445566, 0x00778899, 0x40AABBCC};
        static const uint32_t expected1[6] = {0xFF00FF00, 0x000000FF, 0xFF00FF00, 0xFF0000FF, 0xFF00FF00, 0xFF0000FF};
        static const uint32_t expected2[2] = {0xFF302010, 0x00605040};
        for (int pixel = 0; pixel < 4; pixel++) {
            ICON_CHECK(icon.images[0].pixels[pixel] == expected0[pixel], "32-bit pixel %d is %08X, expected %08X",
                       pixel, icon.images[0].pixels[pixel], expected0[pixel]);
        }
        ICON_CHECK((icon.images[1].width == 3) && (icon.images[1].height == 2), "4-bit image is %dx%d",
                   icon.images[1].width, icon.images[1].height);
        for (int pixel = 0; pixel < 6; pixel++) {
            ICON_CHECK(icon.images[1].pixels[pixel] == expected1[pixel], "4-bit pixel %d is %08X, expected %08X",
                       pixel, icon.images[1].pixels[pixel], expected1[pixel]);
        }
        for (int pixel = 0; pixel < 2; pixel++) {
            ICON_CHECK(icon.images[2].pixels[pixel] == expected2[pixel],
                       "32-bit pixel %d without alpha is %08X, expected %08X", pixel, icon.images[2].pixels[pixel],
                       expected2[pixel]);
        }
        ICON_CHECK((icon.images[3].width == 32) && (icon.images[3].pixels[32 * 32 - 1] == 0xFF123456),
                   "24-bit image is %dx%d, last pixel %08X", icon.images[3].width, icon.images[3].height,
                   icon.images[3].pixels.empty() ? 0u : icon.images[3].pixels.back());
        const size_t primary = thandor::sdl3::IconPrimaryImageIndex(icon);
        ICON_CHECK(primary == 3, "100%% image is %u, expected 3 (32x32)", (unsigned)primary);
        const std::vector<size_t> alternates = thandor::sdl3::IconAlternateImageIndices(icon, primary);
        ICON_CHECK((alternates.size() == 3) && (alternates[0] == 2) && (alternates[1] == 0) && (alternates[2] == 1),
                   "alternate images wrong (%u of them)", (unsigned)alternates.size());
    }
    /* no icon files: too short, and a cursor (type 2) */
    std::vector<std::byte> notIcon(file.begin(), file.begin() + 5);
    ICON_CHECK(!thandor::sdl3::DecodeIcoFile(notIcon, icon), "a 5-byte file is taken as an icon file");
    std::vector<std::byte> cursor = file;
    cursor[2] = std::byte{2};
    ICON_CHECK(!thandor::sdl3::DecodeIcoFile(cursor, icon), "a cursor file is taken as an icon file");
#undef ICON_CHECK
    if (failures == 0) {
        Thandor_Log("icon: ok, %u checks", checks);
    }
    else {
        Thandor_Log("icon: %u of %u checks FAILED", failures, checks);
    }
}

/* Runs the self-test that name (the value of OPEN_THANDOR_SELFTEST, may be NULL) selects; see selftest.h. */
int SelfTest_Run(const char *name)
{
    if (name != NULL && strcmp(name, "codec") == 0) {
        Thandor_SelfTestCodec();
        Thandor_SelfTestCodecNegative();
        return 1;
    }
    if (name != NULL && strcmp(name, "pcx") == 0) {
        Thandor_SelfTestPcx();
        return 1;
    }
    if (name != NULL && strcmp(name, "numberformat") == 0) {
        Thandor_SelfTestNumberFormat();
        return 1;
    }
    if (name != NULL && strcmp(name, "fixedmath") == 0) {
        Thandor_SelfTestFixedMath();
        return 1;
    }
    if (name != NULL && strcmp(name, "keymap") == 0) {
        Thandor_SelfTestKeymap();
        return 1;
    }
    if (name != NULL && strcmp(name, "trianglesetup") == 0) {
        Thandor_SelfTestTriangleSetup();
        return 1;
    }
    if (name != NULL && strcmp(name, "raster") == 0) {
        Thandor_SelfTestRaster();
        return 1;
    }
    if (name != NULL && strcmp(name, "movieenc") == 0) {
        Thandor_SelfTestMovieEncode();
        return 1;
    }
    if (name != NULL && strcmp(name, "settings") == 0) {
        Thandor_SelfTestSettings();
        return 1;
    }
    if (name != NULL && strcmp(name, "path") == 0) {
        Thandor_SelfTestPathSplit();
        return 1;
    }
    if (name != NULL && strcmp(name, "stretch") == 0) {
        Thandor_SelfTestStretch();
        return 1;
    }
    if (name != NULL && strcmp(name, "icon") == 0) {
        Thandor_SelfTestIcon();
        return 1;
    }
    if (name != NULL && strcmp(name, "scanaddr") == 0) {
        Thandor_SelfTestScanAddresses();
        return 1;
    }
    if (name != NULL && strcmp(name, "crash") == 0) {
        *(volatile int *)0 = 1; /* exercises the crash handler */
    }
    return 0;
}
