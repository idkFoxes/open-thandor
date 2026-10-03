/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/memory/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* its address doubles as the error code */
__declspec(align(4)) uint16_t g_ErrorTextHeapAllocationFailed[71] = L"error: HEAP: cannot allocate heap memory! Please check your swap-file.";

__declspec(align(8)) ArenaState g_Arena = {.linearCursor = &g_ArenaLinearStorage[0], .linearLimit = &g_ArenaLinearStorage[0x4000]};

/* linear region of g_Arena (g_Arena.linearCursor starts at [0], g_Arena.linearLimit is [0x4000]); the
   0xC00 bytes past the limit run to the end of the original image and are never handed out */
__declspec(align(16)) uint8_t g_ArenaLinearStorage[0x4C00] = {0};
