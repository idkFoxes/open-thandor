/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/bootstrap/main.c
 */

#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/*
The original image has no C runtime: its PE entry point is ProcessEntry (0x00585D40), which
ends in ExitProcess. The rebuilt executable keeps the MSVC CRT (the Ghidra helpers use memcpy),
maps the original data image (see image.h) and enters ProcessEntry from WinMain.
*/
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
    ProcessEntry();
    return 0;
}
