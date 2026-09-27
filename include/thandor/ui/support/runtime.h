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

/* 0x0050F220 */
void __thandor_void_preserve_eax_ecx_edx
RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output);

/* 0x0050F130 */
void __thandor_void_preserve_eax_ecx_edx RecentTextHistory_Insert(uint16_t *text);

/* 0x0050F2E0 */
void __thandor_void_preserve_eax_ecx_edx RecentTextHistory_RemoveOldest(void);

/* 0x00548EC0 */
void __thandor_void_preserve_eax_ecx_edx
CreditsScreen_Open(FrontendCreditsUiStateView *frontendCreditsView);

/* 0x0054D5D0 */
bool __thandor_cf_preserve_eax_ecx_edx
PcxPreview_Load64x64PaletteAndPixels(PcxPreview64 *outputPreview,uint16_t *sourcePath);

/* 0x0050F1A0 */
void __thandor_void_preserve_eax_ecx_edx
RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex);

#endif /* THANDOR_UI_SUPPORT_RUNTIME_H */
