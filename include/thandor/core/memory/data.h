/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/data.h
 */

#ifndef THANDOR_CORE_MEMORY_DATA_H
#define THANDOR_CORE_MEMORY_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint16_t g_ErrorTextHeapAllocationFailed[71]; /* 00407D84 g_ErrorTextHeapAllocationFailed */

extern ArenaState g_Arena; /* 00585708 g_Arena */

extern uint8_t g_ArenaLinearStorage[0x4C00]; /* 00587400 g_ArenaLinearStorage: uint8_t[0x4C00]: the linear (bump) region of g_Arena: linearCursor starts at [0] (00587400, Ghidra g_Arena_3), linearLimit is &[0x4000] (0058B400, Ghidra g_Arena_4); the 0xC00 bytes from the limit to the end of the original image (0058C000) are never handed out. One array so cursor and limit stay in the same object. */

#endif
