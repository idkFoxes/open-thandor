/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/chat_history.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/chat_history.h>
#include <thandor/thandor.h>
#include <thandor/graphics/resources/pcx.h>
#include <string.h>

/* Module data. */

RecentTextHistorySlot *g_RecentTextSlotStorage = 0;

static RecentTextSerialCounter g_RecentTextSerialCounter = 1;

static uint32_t g_RecentTextEntrySerials[8] = {0};

/* Implementation ownership: ui/core/chat_history. */

/* Builds the list of chat messages to show (output, newest first, at most maxEntries) and ages the history:
   sorts the slots by serial, newest first, and lists them until an empty slot, an expired one (older than
   RECENT_TEXT_HISTORY_LIFETIME) or maxEntries is reached. In the last two cases the remaining slots are
   emptied, so messages beyond maxEntries are forgotten too. Finally the serial counter advances.
*/
void RecentTextHistory_SortAndBuildPointerList
          (RecentTextHistoryEntryLimit maxEntries,RecentTextHistoryPointerList *output)

{
  uint32_t currentSerial;
  RecentTextHistorySlot *slotCursor;
  UiListRowIndex firstIndex;
  UiListRowIndex secondIndex;
  uint32_t outputIndex;
  int minimumRetainedSerial;
  
  /* selection sort, newest (highest serial) first */
  secondIndex = 0;
  do {
    firstIndex = secondIndex + 1;
    currentSerial = g_RecentTextEntrySerials[secondIndex];
    do {
      if (currentSerial < g_RecentTextEntrySerials[firstIndex]) {
        RecentTextHistory_SwapSlots(firstIndex,secondIndex);
        currentSerial = g_RecentTextEntrySerials[secondIndex];
      }
      slotCursor = g_RecentTextSlotStorage;
      firstIndex++;
    } while (firstIndex < RECENT_TEXT_HISTORY_SLOT_COUNT);
    secondIndex++;
  } while (secondIndex < RECENT_TEXT_HISTORY_SLOT_COUNT - 1);
  outputIndex = 0;
  minimumRetainedSerial = g_RecentTextSerialCounter - RECENT_TEXT_HISTORY_LIFETIME;
  output->count = 0;
  do {
    if (g_RecentTextEntrySerials[outputIndex] == 0) { /* the rest is empty already */
      g_RecentTextSerialCounter++;
      return;
    }
    if ((int)g_RecentTextEntrySerials[outputIndex] < minimumRetainedSerial) break;
    output->entries[outputIndex] = slotCursor;
    output->count++;
    outputIndex++;
    slotCursor++;
    maxEntries--;
  } while (maxEntries != 0);
  for (; outputIndex < RECENT_TEXT_HISTORY_SLOT_COUNT; outputIndex++) {
    g_RecentTextEntrySerials[outputIndex] = 0;
  }
  g_RecentTextSerialCounter++;
  return;
}

/* Adds a chat message to the recent-text history: it replaces the oldest slot (lowest serial, an empty
   slot has serial 0; on ties the last one) and gets the current serial, the text being copied with its
   rich-text commands expanded, truncated to the slot's 256 bytes.
*/
void RecentTextHistory_Insert(uint16_t *text)

{
  uint32_t oldestSerial;
  int slotsRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;

  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = UINT32_MAX;
  slotsRemaining = RECENT_TEXT_HISTORY_SLOT_COUNT;
  currentIndex = 0;
  oldestIndex = -1;
  do {
    if (*serialCursor <= oldestSerial) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor++;
    currentIndex++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = g_RecentTextSerialCounter;
    RichTextCommandStream_CopyExpanded
              (sizeof g_RecentTextSlotStorage[0].text,g_RecentTextSlotStorage[oldestIndex].text,text,NULL);
  }
}

/* Drops the oldest chat message from the recent-text history by emptying its slot (serial 0; on ties the last
   one); the text itself stays. Used when the message lines are clicked away (InGameRecentText_TrimHistoryToThree,
   FrontendRecentText_TrimAndSortTopFive).
*/
void RecentTextHistory_RemoveOldest(void)

{
  uint32_t oldestSerial;
  int entriesRemaining;
  int currentIndex;
  int oldestIndex;
  uint32_t *serialCursor;

  serialCursor = g_RecentTextEntrySerials;
  oldestSerial = UINT32_MAX;
  currentIndex = 0;
  oldestIndex = -1;
  for (entriesRemaining = RECENT_TEXT_HISTORY_SLOT_COUNT; entriesRemaining != 0; entriesRemaining--) {
    if (*serialCursor != 0 && *serialCursor <= oldestSerial) {
      oldestSerial = *serialCursor;
      oldestIndex = currentIndex;
    }
    serialCursor++;
    currentIndex++;
  }
  if (-1 < oldestIndex) {
    g_RecentTextEntrySerials[oldestIndex] = 0;
  }
  return;
}

/* Swaps two entries of the recent-text history, for the sort in RecentTextHistory_SortAndBuildPointerList:
   their serials and their whole 256-byte text slots (in 32 steps of two dwords, swapped with atomic exchanges as in
   the original: the history is rebuilt on the main thread and on the timer thread, FrontendSession_PeriodicTick).
*/
void RecentTextHistory_SwapSlots(UiListRowIndex firstIndex,UiListRowIndex secondIndex)

{
  uint32_t secondSerial;
  uint32_t firstLowDword;
  uint32_t firstHighDword;
  uint32_t secondHighDword;
  int dwordPairsRemaining;
  uint32_t *firstSlotDwords;
  uint32_t *secondSlotDwords;

  secondSerial = g_RecentTextEntrySerials[secondIndex];
  g_RecentTextEntrySerials[secondIndex] = g_RecentTextEntrySerials[firstIndex];
  g_RecentTextEntrySerials[firstIndex] = secondSerial;
  secondSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + secondIndex);
  firstSlotDwords = (uint32_t *)(g_RecentTextSlotStorage + firstIndex);
  for (dwordPairsRemaining = 32; dwordPairsRemaining != 0; dwordPairsRemaining--) {
    secondHighDword = secondSlotDwords[1];
    firstLowDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords,*secondSlotDwords);
    firstHighDword = THANDOR_ATOMIC_EXCHANGE(firstSlotDwords + 1,secondHighDword);
    secondSlotDwords[0] = firstLowDword;
    secondSlotDwords[1] = firstHighDword;
    firstSlotDwords += 2;
    secondSlotDwords += 2;
  }
}
