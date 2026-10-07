/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/chat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_CHAT_H
#define THANDOR_UI_INGAME_CHAT_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Labels of the message window's recipient check boxes, one per active faction (selector 0 = faction name) */
inline constexpr int32_t TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE = 0x216D;

/* Chat recipient mask (INGAME_COMMAND_CHAT_SET_RECIPIENTS): bit 9 + n for faction box n, bit 16 + n for session
   player box n (the loops start at the base and double before the first box), everyone = all bits from 8 up */
inline constexpr int32_t INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE = 0x100;
inline constexpr int32_t INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE = 0x8000;
inline constexpr uint32_t INGAME_CHAT_RECIPIENT_EVERYONE = 0xffffff00u;

void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source);

void InGameChatInput_SendLineOrCheckCheatPhrase(InGameCommandTextEntryPageTextEditPtr commandTextEdit);

void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source);

void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source);

void InGameSelectionPage_ShowSubpage1(UiNodeBase *source);

void InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView);

void InGameRecentTextHistory_InsertAndRebuild8(uint16_t *text);

void InGameSevenSlotCommand_ClosePage(UiNodeBase *source);

void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source);

#endif /* THANDOR_UI_INGAME_CHAT_H */
