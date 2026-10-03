/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/support/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/ui/support/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0050F0D0 g_RecentTextSerialCounter */
__declspec(align(16)) RecentTextSerialCounter g_RecentTextSerialCounter = 1;

/* 0050F0E0 g_RecentTextSlotStorage */
__declspec(align(16)) RecentTextHistorySlot *g_RecentTextSlotStorage = 0;

/* 0050F0E4 g_RecentTextEntrySerials */
__declspec(align(4)) uint32_t g_RecentTextEntrySerials[8] = {0};

/* 00545C22 g_CreditsTexturePathUtf16 */
__declspec(align(4)) uint16_t g_CreditsTexturePathUtf16[22] = L"gfx\\panel\\credits.gfx";
