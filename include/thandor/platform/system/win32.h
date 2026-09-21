/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/system/win32.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_PLATFORM_SYSTEM_WIN32_H
#define THANDOR_PLATFORM_SYSTEM_WIN32_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/system/win32. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005868D0 */
void __thandor_void_preserve_eax_ecx_edx Win32_PumpMessages(void);

/* 0x00577B90 */
bool __thandor_void_preserve_eax_ecx Win32_ShouldTranslateMessageFlags(Win32Message32 *message);

#endif /* THANDOR_PLATFORM_SYSTEM_WIN32_H */
