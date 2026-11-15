/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.cpp
 */

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>
#include <thandor/platform/bootstrap/low_memory.h>
#include <thandor/ui/core/key_binding_tables.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry, which
ends in ExitProcess. The rebuilt executable links the C/C++ runtime of its compiler (MinGW GCC),
has the original data compiled in (the "Module data." sections of the module sources) and enters ProcessEntry from WinMain.
*/

/* Program start (not part of the original): installs the crash and hang reports, computes the tables
   the original executable carried precomputed, hooks the key command tables to the key bindings, runs a
   self-test when OPEN_THANDOR_SELFTEST names one (developer tools only), and otherwise enters the original entry
   point ProcessEntry. */
extern "C" int __stdcall WinMain(HINSTANCE instance, HINSTANCE previousInstance, char *commandLine, int showCommand)
{
    (void)instance;
    (void)previousInstance;
    (void)commandLine;
    (void)showCommand;
    Thandor_InstallCrashHandler();
    Thandor_KeepTimerResolutionWhenHidden(); /* the 80 Hz clock keeps its rate while minimized and silent */
#ifdef THANDOR_LARGE_ADDRESS_AWARE
    /* before any other SDL call: SDL's allocations come from the pools below 2 GB as well */
    LowMemory_InstallSdlAllocator();
    LowMemory_LogState("at start (large-address-aware build)");
#endif
    /* tables the original executable carried precomputed */
    FixedMath_BuildSinCosTables();
    Movie_BuildChromaLumaTable();
    GraphicsLighting_BuildPackedLookupTable();
    SoftwareMinimap_BuildPixelWeightTables();
    SoftwareRenderer_BuildFactorTables();
    GraphicsShading_BuildIntensityScaleTable();
    /* the key command tables follow the key bindings from now on (also during the self-tests) */
    KeyBindingTables_Install();
    if (DebugHook_RunSelfTest()) { /* platform/selftest/selftests.cpp */
        return 0;
    }
    ProcessEntry();
    return 0;
}
