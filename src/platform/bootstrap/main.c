/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.c
 */

#include <stdlib.h>
#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry (0x00585D40), which
ends in ExitProcess. The rebuilt executable keeps the MSVC CRT (the Ghidra helpers use memcpy),
maps the original data image (see image.h) and enters ProcessEntry from WinMain.
*/

/* Diagnostics: OPEN_THANDOR_SELFTEST=codec round-trips synthetic save-sized data through the PCK
   encoder/decoder tables, checks guard bytes behind the output and logs the result. */
static void Thandor_SelfTestCodec(void)
{
    static const unsigned sizes[3] = {0x400000, 0x48000, 0x40000};
    unsigned t;
    for (t = 0; t < 3 * 2; t++) {
        unsigned size = sizes[t % 3];
        int noisy = t >= 3;
        unsigned capacity = 0x7ffc00;
        unsigned guard = 0x10000;
        byte *source = (byte *)malloc(size);
        byte *packed = (byte *)malloc(capacity + guard);
        byte *unpacked = (byte *)malloc(size + guard);
        unsigned i;
        unsigned seed = 12345;
        PckCodecEaxCf5 enc;
        PckCodecEaxCf5 dec;
        int packedGuardOk = 1;
        int unpackedGuardOk = 1;
        int same;
        if (!source || !packed || !unpacked) {
            Thandor_Log("codec selftest: allocation failed");
            return;
        }
        for (i = 0; i < size; i++) {
            seed = seed * 1103515245u + 12345u;
            source[i] = (noisy || (i % 4096) < 300) ? (byte)(seed >> 16) : 0;
        }
        memset(packed, 0xCD, capacity + guard);
        memset(unpacked, 0xCD, size + guard);
        enc = (*g_PckEncoderTable[0])(capacity, packed, size, source);
        for (i = capacity; i < capacity + guard; i++) {
            if (packed[i] != 0xCD) { packedGuardOk = 0; break; }
        }
        Thandor_Log("codec selftest %u: size=%x noisy=%d encode carry=%d packed=%x guard=%s", t, size,
                    noisy, enc.carry, enc.eax, packedGuardOk ? "ok" : "OVERWRITTEN");
        if (!enc.carry) {
            dec = (*g_PckDecoderTable[0])(size, unpacked, enc.eax, packed);
            for (i = size; i < size + guard; i++) {
                if (unpacked[i] != 0xCD) { unpackedGuardOk = 0; break; }
            }
            same = memcmp(source, unpacked, size) == 0;
            Thandor_Log("codec selftest %u: decode carry=%d eax=%x roundtrip=%s guard=%s", t, dec.carry,
                        dec.eax, same ? "ok" : "MISMATCH", unpackedGuardOk ? "ok" : "OVERWRITTEN");
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
        word path[0x100];
        word leaf[0x100];
        word parent[0x100];
        char leafA[0x100];
        char parentA[0x100];
        int i;
        memset(path, 0, sizeof path);
        memset(leaf, 0xAB, sizeof leaf);
        memset(parent, 0xAB, sizeof parent);
        for (i = 0; cases[c][i] != 0; i++) path[i] = (word)cases[c][i];
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
    static dword asset[0x100];
    static dword target[8 * 4];
    dword framebuffer[4] = {8, 0, 4, 0};
    dword *entry;
    dword *pixels;
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
            pixels[y * 4 + x] = 0xff000000u | ((dword)(x * 85) << 16) | ((dword)(y * 255) << 8);
        }
    }
    framebuffer[3] = (dword)(uintptr_t)target;
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
typedef void (__stdcall *OriginalStretchProc)(dword, dword, dword, dword, dword, void *, void *);

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
        dword srcW = 16 + (run * 37) % 300;
        dword srcH = 9 + (run * 53) % 200;
        dword dstW = 2 * (8 + (run * 71) % 400);
        dword dstH = 4 + (run * 29) % 300;
        dword pitch = dstW + 8;
        dword assetBytes = 0x220 + (srcW * (srcH + 1) + 2) * 4;
        byte *asset = (byte *)calloc(1, assetBytes);
        byte *mine = (byte *)calloc(pitch * (dstH + 1), bytesPerPixel);
        byte *theirs = (byte *)calloc(pitch * (dstH + 1), bytesPerPixel);
        dword fbMine[4];
        dword fbTheirs[4];
        dword *entry;
        dword *pixels;
        dword i;
        size_t total = (size_t)pitch * (dstH + 1) * bytesPerPixel;
        if (!asset || !mine || !theirs) {
            Thandor_Log("stretchcmp: allocation failed");
            return;
        }
        *maskSlot = (bytesPerPixel == 2) ? quantize[layout][0] : savedMask;
        *weightSlot = (bytesPerPixel == 2) ? quantize[layout][1] : savedWeights;
        *(dword *)asset = 0x786667;
        *(dword *)(asset + 0xb0) = 1;
        *(dword *)(asset + 0xb8) = 0x200;
        entry = (dword *)(asset + 0x200);
        entry[2] = 0xffffffff;
        entry[3] = 0x220;
        entry[6] = srcW;
        entry[7] = srcH;
        pixels = (dword *)(asset + 0x220);
        for (i = 0; i < srcW * (srcH + 1) + 2; i++) {
            seed = seed * 1103515245u + 12345u;
            pixels[i] = seed ^ (seed >> 13);
        }
        fbMine[0] = pitch; fbMine[1] = 0; fbMine[2] = bytesPerPixel; fbMine[3] = (dword)(uintptr_t)mine;
        fbTheirs[0] = pitch; fbTheirs[1] = 0; fbTheirs[2] = bytesPerPixel; fbTheirs[3] = (dword)(uintptr_t)theirs;
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

int __stdcall WinMain(HINSTANCE instance, HINSTANCE previousInstance, char *commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
    int relaunch = Thandor_RelaunchWithReservedImage();
    if (relaunch != -1) {
        return relaunch;
    }
    Thandor_InstallCrashHandler();
    if (Thandor_MapOriginalImage() != 0) {
        return 1;
    }
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
        if (value != NULL && strcmp(value, "stretchcmp") == 0) {
            Thandor_SelfTestStretchCompare();
            return 0;
        }
        if (value != NULL && strcmp(value, "crash") == 0) {
            *(volatile int *)0 = 1; /* exercises the crash handler */
        }
    }
    ProcessEntry();
    return 0;
}
