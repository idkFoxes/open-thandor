/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/generated/image_data.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry (0x00585D40), which
ends in ExitProcess. The rebuilt executable keeps the MSVC CRT (the Ghidra helpers use memcpy),
maps the original data image (see image.h) and enters ProcessEntry from WinMain.
*/

/* Diagnostics: OPEN_THANDOR_SELFTEST=codec round-trips synthetic save-sized data through the PCK
   encoder/decoder tables, checks guard bytes behind the output and logs the result. */
static void Thandor_SelfTestCodec(void)
{
    static const unsigned sizes[3] = {MODEL_RUNTIME_POOL_BYTES, ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot), EFFECT_RUNTIME_POOL_BYTES};
    unsigned t;
    for (t = 0; t < 3 * 2; t++) {
        unsigned size = sizes[t % 3];
        int noisy = t >= 3;
        unsigned capacity = 0x7ffc00;
        unsigned guard = 0x10000;
        uint8_t *source = (uint8_t *)malloc(size);
        uint8_t *packed = (uint8_t *)malloc(capacity + guard);
        uint8_t *unpacked = (uint8_t *)malloc(size + guard);
        unsigned i;
        unsigned seed = 12345;
        PckCodecResult enc;
        PckCodecResult dec;
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
        memset(packed, 0xCD, capacity + guard);
        memset(unpacked, 0xCD, size + guard);
        enc = g_PckEncoderTable[0](capacity, packed, size, source);
        for (i = capacity; i < capacity + guard; i++) {
            if (packed[i] != 0xCD) { packedGuardOk = 0; break; }
        }
        Thandor_Log("codec selftest %u: size=%x noisy=%d encode carry=%d packed=%x guard=%s", t, size,
                    noisy, enc.failed, enc.byteCountOrError, packedGuardOk ? "ok" : "OVERWRITTEN");
        if (!enc.failed) {
            dec = g_PckDecoderTable[0](size, unpacked, enc.byteCountOrError, packed);
            for (i = size; i < size + guard; i++) {
                if (unpacked[i] != 0xCD) { unpackedGuardOk = 0; break; }
            }
            same = memcmp(source, unpacked, size) == 0;
            Thandor_Log("codec selftest %u: decode carry=%d eax=%x roundtrip=%s guard=%s", t, dec.failed,
                        dec.byteCountOrError, same ? "ok" : "MISMATCH", unpackedGuardOk ? "ok" : "OVERWRITTEN");
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
        uint16_t path[0x100];
        uint16_t leaf[0x100];
        uint16_t parent[0x100];
        char leafA[0x100];
        char parentA[0x100];
        int i;
        memset(path, 0, sizeof path);
        memset(leaf, 0xAB, sizeof leaf);
        memset(parent, 0xAB, sizeof parent);
        for (i = 0; cases[c][i] != 0; i++) path[i] = (uint16_t)cases[c][i];
        WidePath_SplitParentAndLeaf(leaf, parent, path);
        for (i = 0; i < 0xff && leaf[i] != 0; i++) leafA[i] = (char)leaf[i];
        leafA[i] = 0;
        for (i = 0; i < 0xff && parent[i] != 0; i++) parentA[i] = (char)parent[i];
        parentA[i] = 0;
        Thandor_Log("path selftest %d: parent=\"%s\" leaf=\"%s\"", c, parentA, leafA);
    }
}


static void Thandor_SelfTestStretch(void)
{
    /* 4x2 ARGB source with a horizontal red ramp, stretched to 8x4. */
    static uint32_t asset[0x100];
    static uint32_t target[8 * 4];
    uint32_t framebuffer[4] = {8, 0, 4, 0};
    uint32_t *entry;
    uint32_t *pixels;
    int x;
    int y;
    memset(asset, 0, sizeof asset);
    asset[0] = 0x786667;
    asset[0xb0 / 4] = 1;
    asset[0xb8 / 4] = 0x100;
    entry = asset + 0x100 / 4;
    entry[2] = 0xffffffff;
    entry[3] = 0x200;
    entry[6] = 4;
    entry[7] = 2;
    pixels = asset + 0x200 / 4;
    for (y = 0; y < 2; y++) {
        for (x = 0; x < 4; x++) {
            pixels[y * 4 + x] = 0xff000000u | ((uint32_t)(x * 85) << 16) | ((uint32_t)(y * 255) << 8);
        }
    }
    framebuffer[3] = (uint32_t)(uintptr_t)target;
    SoftwareTextureSource_StretchDirectColorBilinear32(4, 8, 0, 0, 0, (GraphicsTextureSourceAsset *)asset,
                                                       (SoftwareFramebufferAccess *)framebuffer);
    for (y = 0; y < 4; y++) {
        Thandor_Log("stretch selftest row %d: %08x %08x %08x %08x %08x %08x %08x %08x", y,
                    target[y * 8 + 0], target[y * 8 + 1], target[y * 8 + 2], target[y * 8 + 3],
                    target[y * 8 + 4], target[y * 8 + 5], target[y * 8 + 6], target[y * 8 + 7]);
    }
}

/* OPEN_THANDOR_SELFTEST=stretchcmp compares the C bilinear stretches against the original machine
   code (0x004AA170 16-bit, 0x004AA3F0 32-bit) on random sources, sizes and 555/565 constants. The
   mapped entries jump to the C versions, so the original bytes are copied from the file; both
   routines only use absolute data addresses and internal relative jumps. */
void Thandor_SelfTestRasterCompare(void);
void Thandor_SelfTestBlendScaleCompare(void);
void Thandor_SelfTestBlitCompare(void);
void Thandor_SelfTestRelaxCompare(void);

typedef void (__stdcall *OriginalStretchProc)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void *, void *);

static void Thandor_SelfTestStretchCompare(void)
{
    static const unsigned long long quantize[2][2] = {
        /* {quantize mask 0x41F6E8, PMADDWD weights 0x41F6E0}: 565 and 555 layouts */
        {0x0000f800fc00f800ull, 0x0000080000200100ull},
        {0x0000f800f800f800ull, 0x0000040000200080ull},
    };
    unsigned long long *maskSlot = (unsigned long long *)(uintptr_t)0x41f6e8;
    unsigned long long *weightSlot = (unsigned long long *)(uintptr_t)0x41f6e0;
    unsigned long long savedMask = *maskSlot;
    unsigned long long savedWeights = *weightSlot;
    unsigned seed = 4711;
    int run;
    int failures = 0;
    OriginalStretchProc original16 = (OriginalStretchProc)Thandor_LoadOriginalCodeCopy(0x4aa170, 0x4aa3eb - 0x4aa170);
    OriginalStretchProc original32 = (OriginalStretchProc)Thandor_LoadOriginalCodeCopy(0x4aa3f0, 0x4aa622 - 0x4aa3f0);
    if (original16 == NULL || original32 == NULL) {
        Thandor_Log("stretchcmp: could not load the original code");
        return;
    }
    for (run = 0; run < 24; run++) {
        int bytesPerPixel = (run & 1) ? 4 : 2;
        int layout = (run >> 1) & 1;
        uint32_t srcW = 16 + (run * 37) % 300;
        uint32_t srcH = 9 + (run * 53) % 200;
        uint32_t dstW = 2 * (8 + (run * 71) % 400);
        uint32_t dstH = 4 + (run * 29) % 300;
        uint32_t pitch = dstW + 8;
        uint32_t assetBytes = 0x220 + (srcW * (srcH + 1) + 2) * 4;
        uint8_t *asset = (uint8_t *)calloc(1, assetBytes);
        uint8_t *mine = (uint8_t *)calloc(pitch * (dstH + 1), bytesPerPixel);
        uint8_t *theirs = (uint8_t *)calloc(pitch * (dstH + 1), bytesPerPixel);
        uint32_t fbMine[4];
        uint32_t fbTheirs[4];
        uint32_t *entry;
        uint32_t *pixels;
        uint32_t i;
        size_t total = (size_t)pitch * (dstH + 1) * bytesPerPixel;
        if (!asset || !mine || !theirs) {
            Thandor_Log("stretchcmp: allocation failed");
            return;
        }
        *maskSlot = (bytesPerPixel == 2) ? quantize[layout][0] : savedMask;
        *weightSlot = (bytesPerPixel == 2) ? quantize[layout][1] : savedWeights;
        *(uint32_t *)asset = 0x786667;
        *(uint32_t *)(asset + 0xb0) = 1;
        *(uint32_t *)(asset + 0xb8) = 0x200;
        entry = (uint32_t *)(asset + 0x200);
        entry[2] = 0xffffffff;
        entry[3] = 0x220;
        entry[6] = srcW;
        entry[7] = srcH;
        pixels = (uint32_t *)(asset + 0x220);
        for (i = 0; i < srcW * (srcH + 1) + 2; i++) {
            seed = seed * 1103515245u + 12345u;
            pixels[i] = seed ^ (seed >> 13);
        }
        fbMine[0] = pitch; fbMine[1] = 0; fbMine[2] = bytesPerPixel; fbMine[3] = (uint32_t)(uintptr_t)mine;
        fbTheirs[0] = pitch; fbTheirs[1] = 0; fbTheirs[2] = bytesPerPixel; fbTheirs[3] = (uint32_t)(uintptr_t)theirs;
        if (bytesPerPixel == 2) {
            SoftwareTextureSource_StretchDirectColorBilinear16(dstH, dstW, 0, 4, 0,
                (GraphicsTextureSourceAsset *)asset, (SoftwareFramebufferAccess *)fbMine);
            original16(dstH, dstW, 0, 4, 0, asset, fbTheirs);
        }
        else {
            SoftwareTextureSource_StretchDirectColorBilinear32(dstH, dstW, 0, 4, 0,
                (GraphicsTextureSourceAsset *)asset, (SoftwareFramebufferAccess *)fbMine);
            original32(dstH, dstW, 0, 4, 0, asset, fbTheirs);
        }
        __asm emms
        for (i = 0; i < total && mine[i] == theirs[i]; i++) {
        }
        if (i < total) {
            failures++;
            Thandor_Log("stretchcmp run %d (%d bpp, %s, %ux%u -> %ux%u): MISMATCH at byte %u (pixel %u,%u): mine %02x theirs %02x",
                        run, bytesPerPixel * 8, layout ? "555" : "565", srcW, srcH, dstW, dstH, i,
                        (i / bytesPerPixel) % pitch, (i / bytesPerPixel) / pitch, mine[i], theirs[i]);
        }
        else {
            Thandor_Log("stretchcmp run %d (%d bpp, %s, %ux%u -> %ux%u): identical", run,
                        bytesPerPixel * 8, layout ? "555" : "565", srcW, srcH, dstW, dstH);
        }
        free(asset);
        free(mine);
        free(theirs);
    }
    *maskSlot = savedMask;
    *weightSlot = savedWeights;
    Thandor_Log("stretchcmp: %d of 24 runs differ", failures);
}

/* OPEN_THANDOR_SELFTEST=scanaddr decodes every entry of the packages next to the executable (all
   but FILME.PCK) and writes each aligned dword in the original image range 0x401000-0x58C000 to
   scanaddr.txt: package, entry path, type tag, offset, value. Used to find assets that store
   original code or data addresses. With OPEN_THANDOR_DUMPTEXT=<dir> it also writes every decoded
   text page (*.str, *.txt) to <dir>\<package>_<entry path>. */
/* The arena is set up by ProcessEntry; decoders called before that allocate through these. */
static ArenaAllocResult SelfTest_Alloc(uint32_t bytes)
{
    ArenaAllocResult result;
    result.payloadOrError = (uint32_t)(uintptr_t)malloc(bytes);
    result.failed = result.payloadOrError == 0;
    return result;
}

static ArenaFreeResult SelfTest_Free(void *memory)
{
    ArenaFreeResult result;
    memset(&result, 0, sizeof result);
    free(memory);
    return result;
}

static void Thandor_SelfTestScanAddresses(void)
{
    ArenaAllocResult (*savedAlloc)(uint32_t) = g_MemoryApi.alloc;
    ArenaFreeResult (*savedFree)(void *) = g_MemoryApi.free;
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
            PckCodecResult decoded;
            char name[247];
            int k;
            uint32_t i;
            if (fseek(pck, position, SEEK_SET) != 0 || fread(&header, sizeof header, 1, pck) != 1) {
                break;
            }
            if (header.packedSize == 0 || header.packedSize > PACKAGE_SCRATCH_BUFFER_BYTES || header.unpackedSize > 0x4000000 ||
                (uint32_t)header.compressionMethod > 3) {
                break;
            }
            for (k = 0; k < 246 && header.path[k] != 0; k++) {
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
                          (header.unpackedSize, unpacked, header.packedSize, packed);
            totalEntries++;
            if (decoded.failed) {
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
                    if ((value >= 0x401000 && value < 0x58c000) || (value >= 0x10000000 && value < 0x10300000)) {
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

/* OPEN_THANDOR_SELFTEST=imagecmp checks src/generated/image_data.c against the original file.
   Every converted pointer (g_ThandorImagePointers) is translated back - into a generated block ->
   original address, C function -> original entry - and must equal the recorded original value;
   every other byte must equal the original. Bytes of original code inside a block are zero in
   the generated data and counted separately. */
static uint32_t ImageCompare_ToOriginal(uint32_t generated, unsigned blocks)
{
    unsigned k;
    for (k = 0; k < blocks; k++) {
        const ThandorImageBlock *block = &g_ThandorImageBlocks[k];
        uint32_t base = (uint32_t)(uintptr_t)block->data;
        if (generated >= base && generated < base + (block->end - block->start)) {
            return block->start + (generated - base);
        }
    }
    for (k = 0; k < g_ThandorFunctionMapCount; k++) {
        if ((uint32_t)(uintptr_t)g_ThandorFunctionMap[k].function == generated) {
            return g_ThandorFunctionMap[k].originalAddress;
        }
    }
    return 0xFFFFFFFFu;
}

static void Thandor_SelfTestImageCompare(void)
{
    const uint8_t *original = (const uint8_t *)Thandor_LoadOriginalCodeCopy(0x401000, 0x18b000);
    unsigned blocks = sizeof g_ThandorImageBlocks / sizeof g_ThandorImageBlocks[0];
    unsigned pointerCount = sizeof g_ThandorImagePointers / sizeof g_ThandorImagePointers[0];
    unsigned b;
    unsigned p = 0;
    unsigned bytes = 0;
    unsigned pointerMismatches = 0;
    unsigned mismatches = 0;
    unsigned zeroedCode = 0;
    if (original == NULL) {
        Thandor_Log("imagecmp: could not read thandor_original.exe");
        return;
    }
    for (b = 0; b < blocks; b++) {
        const ThandorImageBlock *block = &g_ThandorImageBlocks[b];
        uint32_t address;
        for (address = block->start; address < block->end; address++) {
            uint8_t generated = block->data[address - block->start];
            uint8_t expected = original[address - 0x401000];
            while (p < pointerCount && g_ThandorImagePointers[p].location + 4 <= address) {
                p++;
            }
            if (p < pointerCount && g_ThandorImagePointers[p].location == address) {
                uint32_t value = *(const uint32_t *)(block->data + (address - block->start));
                uint32_t translated = ImageCompare_ToOriginal(value, blocks);
                if (translated != g_ThandorImagePointers[p].originalValue ||
                    translated != *(const uint32_t *)(original + address - 0x401000)) {
                    if (pointerMismatches++ < 10) {
                        Thandor_Log("imagecmp: pointer at %08X is %08X (as original %08X), original %08X",
                                    address, value, translated, g_ThandorImagePointers[p].originalValue);
                    }
                }
                bytes += 4;
                address += 3;
                continue;
            }
            bytes++;
            if (generated != expected) {
                if (generated == 0) {
                    zeroedCode++;
                }
                else if (mismatches++ < 10) {
                    Thandor_Log("imagecmp: byte at %08X is %02X, original %02X", address, generated, expected);
                }
            }
        }
    }
    Thandor_Log("imagecmp: %u blocks, %u bytes, %u pointers, %u pointer mismatches, %u byte mismatches, %u zeroed code bytes",
                blocks, bytes, pointerCount, pointerMismatches, mismatches, zeroedCode);
}

/* Program start (not part of the original): provides the original data image, computes the tables
   the original executable carried precomputed, runs a self-test when OPEN_THANDOR_SELFTEST names one,
   and otherwise enters the original entry point ProcessEntry. */
int __stdcall WinMain(HINSTANCE instance, HINSTANCE previousInstance, char *commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
#ifdef THANDOR_MAPPED_IMAGE
    int relaunch = Thandor_RelaunchWithReservedImage();
    if (relaunch != -1) {
        return relaunch;
    }
    Thandor_InstallCrashHandler();
    if (Thandor_MapOriginalImage() != 0) {
        return 1;
    }
#else
    /* The original data is compiled in (src/generated/image_data.c); nothing is mapped. */
    Thandor_InstallCrashHandler();
    Thandor_Log("open-thandor: generated image data, %u blocks",
                (unsigned)(sizeof g_ThandorImageBlocks / sizeof g_ThandorImageBlocks[0]));
#endif
    /* tables the original executable carried precomputed */
    FixedMath_BuildSinCosTables();
    Movie_BuildChromaLumaTable();
    GraphicsLighting_BuildPackedLookupTable();
    {
        const char *value = getenv("OPEN_THANDOR_SELFTEST");
        if (value != NULL && strcmp(value, "codec") == 0) {
            Thandor_SelfTestCodec();
            return 0;
        }
        if (value != NULL && strcmp(value, "path") == 0) {
            Thandor_SelfTestPathSplit();
            return 0;
        }
        if (value != NULL && strcmp(value, "stretch") == 0) {
            Thandor_SelfTestStretch();
            return 0;
        }
        if (value != NULL && strcmp(value, "imagecmp") == 0) {
            Thandor_SelfTestImageCompare();
            return 0;
        }
        if (value != NULL && strcmp(value, "scanaddr") == 0) {
            Thandor_SelfTestScanAddresses();
            return 0;
        }
        if (value != NULL && strcmp(value, "stretchcmp") == 0) {
            Thandor_SelfTestStretchCompare();
            return 0;
        }
        if (value != NULL && strcmp(value, "rastercmp") == 0) {
            Thandor_SelfTestRasterCompare(); /* selftest_raster.c */
            return 0;
        }
        if (value != NULL && strcmp(value, "blendscalecmp") == 0) {
            Thandor_SelfTestBlendScaleCompare(); /* selftest_blendscale.c */
            return 0;
        }
        if (value != NULL && strcmp(value, "blitcmp") == 0) {
            Thandor_SelfTestBlitCompare(); /* selftest_blit.c */
            return 0;
        }
        if (value != NULL && strcmp(value, "relaxcmp") == 0) {
            Thandor_SelfTestRelaxCompare(); /* selftest_relax.c */
            return 0;
        }
        if (value != NULL && strcmp(value, "crash") == 0) {
            *(volatile int *)0 = 1; /* exercises the crash handler */
        }
    }
    ProcessEntry();
    return 0;
}
