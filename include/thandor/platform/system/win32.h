#ifndef THANDOR_PLATFORM_SYSTEM_WIN32_H
#define THANDOR_PLATFORM_SYSTEM_WIN32_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: platform/system/win32. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005868D0 */
void __cdecl Win32_PumpMessages(void);

/* 0x00577B90 */
void Win32_ShouldTranslateMessageFlags(Win32Message32 *message);

#endif /* THANDOR_PLATFORM_SYSTEM_WIN32_H */
