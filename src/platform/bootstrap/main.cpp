/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.cpp
 */

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry, which
ends in ExitProcess. The rebuilt executable keeps the MSVC CRT (the code uses memcpy),
has the original data compiled in (the "Module data." sections of the module sources) and enters ProcessEntry from WinMain.
*/

/* Program start (not part of the original): installs the crash and hang reports, computes the tables
   the original executable carried precomputed, runs a self-test when OPEN_THANDOR_SELFTEST names one
   (developer tools only), and otherwise enters the original entry point ProcessEntry. */
extern "C" int __stdcall WinMain(HINSTANCE instance, HINSTANCE previousInstance, char *commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
    Thandor_InstallCrashHandler();
    /* tables the original executable carried precomputed */
    FixedMath_BuildSinCosTables();
    Movie_BuildChromaLumaTable();
    GraphicsLighting_BuildPackedLookupTable();
    UiScaler_BuildPixelWeightTables();
    SoftwareRenderer_BuildFactorTables();
    GraphicsShading_BuildIntensityScaleTable();
    if (DebugHook_RunSelfTest()) { /* platform/selftest/selftests.cpp */
        return 0;
    }
    ProcessEntry();
    return 0;
}
