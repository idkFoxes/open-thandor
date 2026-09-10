#ifndef THANDOR_UI_SUPPORT_RUNTIME_H
#define THANDOR_UI_SUPPORT_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/support/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0050F220 */
void RecentTextHistory_SortAndBuildPointerList (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output);

/* 0x0050F130 */
void RecentTextHistory_Insert(word *text);

/* 0x0050F2E0 */
void __cdecl RecentTextHistory_RemoveOldest(void);

/* 0x00548EC0 */
void CreditsScreen_Open(void *creditsUiState);

/* 0x0054D5D0 */
void PcxPreview_Load64x64PaletteAndPixelsCf(PcxPreview64 *outputPreview,word *sourcePath);

/* 0x0050F1A0 */
void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex);

#endif /* THANDOR_UI_SUPPORT_RUNTIME_H */
