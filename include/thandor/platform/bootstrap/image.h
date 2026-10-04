/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/bootstrap/image.h
 */

#ifndef THANDOR_PLATFORM_BOOTSTRAP_IMAGE_H
#define THANDOR_PLATFORM_BOOTSTRAP_IMAGE_H

/*
Process support of the rebuilt executable: logging, crash and hang reports and small Win32 helpers.

thandor.exe keeps code and data in one writable .text section at fixed addresses. The rebuilt executable
compiles that data in as ordinary variables (the "Module data." sections of src/<area>/<module>/*.cpp); nothing of the original image
is mapped.
*/

/* Address range of the original thandor.exe's single RWX .text section (code and data); the scanaddr self-test
   looks for these values in assets */
#define ORIGINAL_TEXT_START 0x401000u
#define ORIGINAL_TEXT_END 0x58C000u
/* The rebuilt executable is linked at this fixed base (/BASE in CMakeLists.txt); its code starts one page in */
#define REBUILT_IMAGE_BASE 0x10000000u
#define REBUILT_IMAGE_CODE_START (REBUILT_IMAGE_BASE + 0x1000u)

/* Appends one line to thandor.log next to the executable. */
void Thandor_Log(const char *format, ...);

/* Appends `reason value` and the current call stack to thandor.log. */
void Thandor_LogStack(const char *reason, unsigned value);

/* Nonzero when [address, address + size) is committed, readable memory (diagnostics). */
int Thandor_IsReadable(const void *address, unsigned size);
unsigned Thandor_TickCount();
void Thandor_SleepMs(unsigned milliseconds);
/* Returns nonzero when the UTF-16 path names an existing directory. */
int Thandor_DirectoryExistsW(const unsigned short *path);
/* Full path of the running executable (ANSI), independent of how it was started. */
void Thandor_GetExecutablePathA(char *out, unsigned capacity);

/* Symbol name (+offset) of an address in this executable, for diagnostics. */
const char *Thandor_SymbolName(const void *address);

/* Writes crash.log next to the executable with a symbolized stack on unhandled exceptions, and
   hang.log when no frame has been presented for a few seconds (see g_ThandorFrameHeartbeat). */
void Thandor_InstallCrashHandler();

/* Incremented on every presented frame; the hang detector watches it. */
extern volatile long g_ThandorFrameHeartbeat;

#endif /* THANDOR_PLATFORM_BOOTSTRAP_IMAGE_H */
