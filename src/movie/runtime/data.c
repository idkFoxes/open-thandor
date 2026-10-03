/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/movie/runtime/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/movie/runtime/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(4)) LocaleCopyDefaultComputerLabelUtf16Proc *g_LocaleCopyDefaultComputerLabelUtf16 = 0;

__declspec(align(4)) FileSystemGetPositionProc *g_FileSystemGetPosition = 0;

/* filled at startup by Movie_BuildChromaLumaTable */
__declspec(align(16)) uint32_t g_MovieChromaLumaToArgb[1024][32] = {0};

__declspec(align(16)) uint64_t g_MovieDeltaRgbHighNibbleMask2Pixels = 0xF0F0F000F0F0F0ull;

__declspec(align(8)) MovieRuntime *g_ActiveMovie = 0;

__declspec(align(4)) uint32_t g_MoviePlaybackCurrentFrame = 0;

/* 2 command records and the terminator record (commandCode 0) at 005658D8
   that ends the dispatcher's scan. The scan also reads the terminator's modifierClassFlags (0x90909090, NOP
   fill). Original quirk: the original's terminator was only 8 bytes long, its code followed at 005658E0, so the
   terminator's continuationEntryAddress is not original data (never read; the table entry ends at 005658E0). */
__declspec(align(16)) UiCommandDispatchRecord g_EndMovieCommandDispatchRecords[3] = {
    /* 0 */ {.commandCode = 0x71, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x565990},
    /* 1 */ {.commandCode = 0x70, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x5658F0},
    /* 2 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090}};
