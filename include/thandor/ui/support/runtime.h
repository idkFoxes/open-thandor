/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/support/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_SUPPORT_RUNTIME_H
#define THANDOR_UI_SUPPORT_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/support/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Recent-text (chat message) history: 8 slots, each stamped with g_RecentTextSerialCounter when inserted.
   The counter advances with every RecentTextHistory_SortAndBuildPointerList call (once per frame in game),
   and a message older than RECENT_TEXT_HISTORY_LIFETIME calls is dropped. */
#define RECENT_TEXT_HISTORY_SLOT_COUNT 8
#define RECENT_TEXT_HISTORY_LIFETIME 0x100
/* Extension code for WidePath_SetExtensionCode (see WIDE_PATH_EXTENSION_* in core/text/path.h): ".pcx",
   the 64x64 player preview pictures of PcxPreview_Load64x64PaletteAndPixels. */
#define WIDE_PATH_EXTENSION_PCX 0x786370
/* Image record returned by the PCX decoder module (g_PcxFunctionExport2): the offset of the image header at
   +0xB8, the palette (8 bytes per colour) at +0x200; inside the image header the pixel offset, width, height */
#define PCX_DECODED_IMAGE_HEADER_OFFSET 0xB8
#define PCX_DECODED_PALETTE 0x200
#define PCX_IMAGE_HEADER_PIXEL_OFFSET 0x0C
#define PCX_IMAGE_HEADER_WIDTH 0x18
#define PCX_IMAGE_HEADER_HEIGHT 0x1C

/* 0x0050F220 */
void RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output);

/* 0x0050F130 */
void RecentTextHistory_Insert(uint16_t *text);

/* 0x0050F2E0 */
void RecentTextHistory_RemoveOldest(void);

/* 0x00548EC0 */
void CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView);

/* 0x0054D5D0 */
bool PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath);

/* 0x0050F1A0 */
void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex);

#endif /* THANDOR_UI_SUPPORT_RUNTIME_H */
