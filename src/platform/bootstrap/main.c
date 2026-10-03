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
has the original data compiled in (src/generated/image_data.c) and enters ProcessEntry from WinMain.
*/

/* Program start (not part of the original): installs the crash and hang reports, computes the tables
   the original executable carried precomputed, runs a self-test when OPEN_THANDOR_SELFTEST names one,
   and otherwise enters the original entry point ProcessEntry. */
int __stdcall WinMain(HINSTANCE instance, HINSTANCE previousInstance, char *commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
    Thandor_InstallCrashHandler();
    Thandor_Log("open-thandor: generated image data, %u objects",
                (unsigned)(sizeof g_ThandorImageBlocks / sizeof g_ThandorImageBlocks[0]));
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
