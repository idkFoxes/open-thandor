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

void RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output);

void RecentTextHistory_Insert(uint16_t *text);

void RecentTextHistory_RemoveOldest(void);

void CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView);

bool PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath);

void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex);

#endif /* THANDOR_UI_SUPPORT_RUNTIME_H */
