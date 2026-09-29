/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.c
 */

#include <stdlib.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/generated/image_data.h>
#include <thandor/platform/selftest/selftest.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry (0x00585D40), which
ends in ExitProcess. The rebuilt executable keeps the MSVC CRT (the Ghidra helpers use memcpy),
maps the original data image (see image.h) and enters ProcessEntry from WinMain.
*/

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
    if (SelfTest_Run(getenv("OPEN_THANDOR_SELFTEST"))) { /* platform/selftest/selftests.c */
        return 0;
    }
    ProcessEntry();
    return 0;
}
