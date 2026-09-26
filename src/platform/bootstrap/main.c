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
        if (value != NULL && strcmp(value, "crash") == 0) {
            *(volatile int *)0 = 1; /* exercises the crash handler */
        }
    }
    ProcessEntry();
    return 0;
}
