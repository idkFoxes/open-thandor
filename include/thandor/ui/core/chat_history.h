/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/chat_history.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_CHAT_HISTORY_H
#define THANDOR_UI_CORE_CHAT_HISTORY_H

#include <thandor/ui/core/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/core/chat_history. */

/* Recent-text (chat message) history: 8 slots, each stamped with g_RecentTextSerialCounter when inserted.
   The counter advances with every RecentTextHistory_SortAndBuildPointerList call (once per frame in game),
   and a message older than RECENT_TEXT_HISTORY_LIFETIME calls is dropped. */
#define RECENT_TEXT_HISTORY_SLOT_COUNT 8
#define RECENT_TEXT_HISTORY_LIFETIME 0x100

/* Functions are grouped by semantic ownership. */

void RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output);

void RecentTextHistory_Insert(uint16_t *text);

void RecentTextHistory_RemoveOldest(void);

void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex);

extern RecentTextHistorySlot *g_RecentTextSlotStorage;

#endif /* THANDOR_UI_CORE_CHAT_HISTORY_H */
