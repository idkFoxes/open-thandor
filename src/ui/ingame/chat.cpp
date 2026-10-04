/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/chat.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/chat.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static int32_t g_UiSevenSlotSelectionControlOffsets[7] = {8420, 8516, 8612, 8708, 8804, 8900, 8996};

static uint16_t g_DeveloperChatPhraseUtf16[32] = {'O', 'h', ' ', 'g', 'r', 'o', 's', 's', 'e', 'r', ' ', 'T', 'h', 'o', 'm', 'a', 's', ',', ' ', 'e', 'r', 'l', 0xF6, 's', 'e', ' ', 'm', 'i', 'c', 'h', '!', 0}; /* L"Oh grosser Thomas, erl\366se mich!" */

static uint16_t g_HmmNaGutChatPhraseUtf16[20] = {'H', 'm', 'm', 'm', ',', ' ', 'n', 'a', ' ', 'g', 'u', 't', '.', '.', '.', ' ', ';', '-', ')', 0}; /* L"Hmmm, na gut... ;-)" */

/* Implementation ownership: ui/ingame/chat. */

/* UI action 0x1005 (g_InGameUiActionHandlersPage10[5], InGameUiImage.messageSendAndCloseButton): sends the
   typed message to the chosen recipients (InGameSevenSlotCommand_SubmitTextAndSelectionMask) and closes the
   message window.
*/
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source)

{
  InGameSevenSlotCommand_SubmitTextAndSelectionMask(source);
  InGameSevenSlotCommand_ClosePage(source);
  return;
}

/* True when the first 32 UTF-16 units of text equal the cheat phrase g_DeveloperChatPhraseUtf16 (compared as
   16 dwords, like the original REPE CMPSD). */
static Bool8 InGameChatInput_MatchesCheatPhrase(const uint16_t *text)

{
  const int *phraseDwords;
  const int *textDwords;
  int dwordIndex;

  phraseDwords = (const int *)THANDOR_ADDR(g_DeveloperChatPhraseUtf16,0);
  textDwords = (const int *)text;
  for (dwordIndex = 0; dwordIndex < 16; dwordIndex++) {
    if (phraseDwords[dwordIndex] != textDwords[dwordIndex]) {
      return false;
    }
  }
  return true;
}

/* Recipient bits of the message window's seven check boxes (at g_UiSevenSlotSelectionControlOffsets from the
   in-game UI root uiRoot): bit n+1 above baseBit is set when box n is ticked. Shared by the chat line and the
   message window. */
static CommandPayload InGameChatInput_CollectTickedSlotBits(UiNodeBase *uiRoot,uint32_t baseBit)

{
  uint32_t slotIndex;
  uint32_t slotBit;
  CommandPayload recipientMask;
  Bool8 isSelected;

  recipientMask = 0;
  slotBit = baseBit;
  for (slotIndex = 0; slotIndex < 7; slotIndex++) {
    slotBit = slotBit * 2;
    isSelected = (Bool8)UiSelectableControl_IsSelected
                            ((UiSelectableControl *)THANDOR_UI_AT(uiRoot,g_UiSevenSlotSelectionControlOffsets[slotIndex]));
    if (isSelected) {
      recipientMask = recipientMask | slotBit;
    }
  }
  return recipientMask;
}

/* Sends the narrowed text in g_UiSevenSlotCommandPayloadText to the recipients: the recipient mask, the text as
   four 12-byte chat commands, then the publish command (directly in a local game, else through the command
   queue). */
static void InGameChatInput_SendPayloadText(CommandPayload recipientMask)

{
  uint32_t tripleIndex;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_SetPackedState(g_LocalPlayerRuntimeId,0,0,recipientMask);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_SET_RECIPIENTS,0,0,recipientMask);
  }
  for (tripleIndex = 0; tripleIndex < 4; tripleIndex++) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerTextCommand_AppendTripleClamped
                (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload1,
                 g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload2,
                 g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload3);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload1,
                 g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload2,
                 g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload3);
    }
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerTextCommand_PublishConditionalRichText(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CHAT_PUBLISH,0,0,0);
  }
}

/* UI action 0x1024 (g_InGameUiActionHandlersPage10[36]): Enter in the in-game chat line. In a local game the
   line is only compared with the cheat phrase g_DeveloperChatPhraseUtf16, which toggles the cheats and answers
   with a message. In a network game the text is sent to the recipients chosen in the message window (all, the
   ticked factions or the ticked session players) and the line is cleared. Either way the command page closes.
*/
void InGameChatInput_SendLineOrCheckCheatPhrase(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  int tabOffset;
  CommandPayload recipientMask;
  uint32_t unitIndex;
  UiNodeBase *recipientTab;

  UiTextControl_UpdateNonEmptyValidity((UiTextEditControl *)commandTextEdit);
  if ((commandTextEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      if (InGameChatInput_MatchesCheatPhrase(commandTextEdit->textBuffer)) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED;
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_CHEAT_PHRASE_ENTERED;
        InGameRecentTextHistory_InsertAndRebuild8((uint16_t *)g_HmmNaGutChatPhraseUtf16);
      }
    }
    else {
      RichTextCommandStream_CopyToNarrow
                (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,
                 commandTextEdit->textBuffer);
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientAllTab),
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientGroupsTab),
      THANDOR_UI_SIBLING(commandTextEdit,InGameUiImage,chatInputTextEdit,messageRecipientPlayersTab));
      /* Recipient mask: bit 9+n when box n of the faction tab is ticked (messageRecipientPlayersTab), bit 16+n
         for box n of the session player tab (messageRecipientGroupsTab), 0xFFFFFF00 for everyone. */
      tabOffset = (int)((uintptr_t)recipientTab - (uintptr_t)commandTextEdit);
      if (tabOffset ==
          (int)offsetof(InGameUiImage,messageRecipientPlayersTab) - (int)offsetof(InGameUiImage,chatInputTextEdit)) {
        recipientMask = InGameChatInput_CollectTickedSlotBits
                                  (THANDOR_UI_AT(commandTextEdit,-(int)offsetof(InGameUiImage,chatInputTextEdit)),
                                   INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE);
      }
      else if (tabOffset ==
               (int)offsetof(InGameUiImage,messageRecipientGroupsTab) - (int)offsetof(InGameUiImage,chatInputTextEdit)) {
        recipientMask = InGameChatInput_CollectTickedSlotBits
                                  (THANDOR_UI_AT(commandTextEdit,-(int)offsetof(InGameUiImage,chatInputTextEdit)),
                                   INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE);
      }
      else {
        recipientMask = INGAME_CHAT_RECIPIENT_EVERYONE;
      }
      InGameChatInput_SendPayloadText(recipientMask);
      commandTextEdit->cursorIndex = 0;
      commandTextEdit->selectionStart = 0;
      commandTextEdit->selectionEnd = 0;
      for (unitIndex = 0; unitIndex < 48; unitIndex++) {
        commandTextEdit->textBuffer[unitIndex] = 0;
      }
    }
  }
  UiPageStack_SetActiveIndex(0,&THANDOR_CONTAINER_OF(commandTextEdit, InGameCommandTextEntryPage2320, commandTextEdit)->commandPageStack);
  return;
}

/* UI action 0x1006 (g_InGameUiActionHandlersPage10[6]): the recipient tab InGameUiImage.messageRecipientPlayersTab
   of the message window. Selects the tab, shows the check box page and gives one check box to each faction
   still in the game (factions 1..7),
   labelled with the faction name (label texts 0x216D.. patched with name text 0x2173 + name index); the list
   is sized to the used rows and the unused check boxes are hidden.
*/
void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source)

{
  uint32_t *controlFlags;
  UiNodeBase *uiRootNode;
  int factionNameIndex;
  uint16_t *stream;
  TextResourceId resourceId;
  uint32_t factionIndexCursor;
  uintptr_t factionRecordAddress;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(uiRootNode,messageRecipientAllTab),
      INGAME_UI(uiRootNode,messageRecipientGroupsTab),
      INGAME_UI(uiRootNode,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(uiRootNode,messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  factionIndexCursor = 1;
  /* a faction is named after its colour: colorIndex of the faction record selects the name text */
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,sizeof(GameFactionRuntimeRecord));
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexCursor] != 0) {
      resolvedText = TextResource_Resolve(resourceId);
      stream = resolvedText;
      factionNameIndex = ((GameFactionRuntimeRecord *)factionRecordAddress)->colorIndex;
      resourceId++;
      /* nodeFlags of the check box: g_UiSevenSlotSelectionControlOffsets is relative to the root */
      controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      filledSlotCount++;
      resolvedText = TextResource_Resolve(factionNameIndex + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText,stream);
    }
    factionIndexCursor++;
    factionRecordAddress = factionRecordAddress + sizeof(GameFactionRuntimeRecord);
  } while (factionIndexCursor < 8);
  INGAME_UI(uiRootNode,messageRecipientList)->bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  return;
}

/* UI action 0x1007 (g_InGameUiActionHandlersPage10[7]): the recipient tab InGameUiImage.messageRecipientGroupsTab
   of the message window. Like action 0x1006, but gives one check box to each of the first seven session
   players (g_FrontendPlayerRuntimeBlocks), labelled with the player name from the player's selection block.
*/
void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  uint32_t *controlFlags;
  UiNodeBase *uiRootNode;
  SelectionPlayerRuntimeBlock *selectionBlock;
  TextResourceId resourceId;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(uiRootNode,messageRecipientAllTab),
      INGAME_UI(uiRootNode,messageRecipientGroupsTab),
      INGAME_UI(uiRootNode,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(uiRootNode,messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  do {
    resolvedText = TextResource_Resolve(resourceId);
    selectionBlock = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[filledSlotCount].playerRuntimeId];
    resourceId++;
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    filledSlotCount++;
    RichTextCommandStream_PatchPayloadBySelector(0,selectionBlock->playerNameUtf16,resolvedText);
    if (6 < filledSlotCount) break;
  } while (filledSlotCount < g_FrontendPlayerRuntimeBlockCount);
  INGAME_UI(uiRootNode,messageRecipientList)->bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            ((UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,(UiScrollableControl *)INGAME_UI(uiRootNode,messageRecipientScroll));
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = (uint32_t *)&THANDOR_UI_AT(uiRootNode,g_UiSevenSlotSelectionControlOffsets[filledSlotCount])->nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
  return;
}

/* UI action 0x1008 (g_InGameUiActionHandlersPage10[8]): the recipient tab InGameUiImage.messageRecipientAllTab
   of the message window (send to everyone). Selects the tab and shows page 1 of the recipient page stack,
   which has no check boxes.
*/
void InGameSelectionPage_ShowSubpage1(UiNodeBase *source)

{
  UiNodeBase *rootNodeCursor;

  rootNodeCursor = source;
  while (rootNodeCursor->parent != UI_NODE_NONE) {
    rootNodeCursor = rootNodeCursor->parent;
  }
  UiSelectableGroup_SelectExclusive(3,source,
      INGAME_UI(rootNodeCursor,messageRecipientAllTab),
      INGAME_UI(rootNodeCursor,messageRecipientGroupsTab),
      INGAME_UI(rootNodeCursor,messageRecipientPlayersTab));
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rootNodeCursor,messageRecipientPageStack));
  return;
}

/* UI action 0x100F (g_InGameUiActionHandlersPage10[15], InGameUiImage.messageHistoryPanel): a click on the
   message lines drops the oldest ones so that at most three remain - and always at least one, even when
   three or fewer are shown - then rebuilds the eight-line list.
*/
void InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView)

{
  uint32_t currentEntryCount;
  
  for (currentEntryCount = (historyView->recentTextPointerList).count; 4 < currentEntryCount;
      currentEntryCount = currentEntryCount - 1) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList(8,&historyView->recentTextPointerList);
  return;
}

/* Shows an in-game message line (chat, player departure, network notices): adds the UTF-16 text to the
   recent-text history and rebuilds the eight-line pointer list the in-game UI displays
   (g_InGameRuntimeRoot->recentTextHistory).
*/
void InGameRecentTextHistory_InsertAndRebuild8(uint16_t *text)

{
  RecentTextHistoryPointerList *messageList;

  messageList = &g_InGameRuntimeRoot->recentTextHistory;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(8,messageList);
  return;
}

/* UI action 0x1002 (g_InGameUiActionHandlersPage10[2], InGameUiImage.messageCancelButton): closes the message
   window - shows the world view again and switches the game window page stack back to page 0.
*/
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  INGAME_UI(source,worldView)->nodeFlags = INGAME_UI(source,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,(UiPageStackControl *)INGAME_UI(source,gameWindowPageStack));
  return;
}

/* UI action 0x1004 (g_InGameUiActionHandlersPage10[4], InGameUiImage.messageSendButton; also called by
   InGameSevenSlotCommand_SubmitAndClosePage): sends the text of the message window. The text is narrowed to
   48 bytes, the recipient mask is built from the selected tab and its check boxes (as in
   InGameChatInput_SendLineOrCheckCheatPhrase), then the mask, the text as four 12-byte chat commands and the publish command
   are issued, and the text field is cleared.
*/
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source)

{
  UiTextEditControl *messageTextEdit;
  CommandPayload recipientMask;
  uint32_t unitIndex;
  UiNodeBase *recipientTab;

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  messageTextEdit = (UiTextEditControl *)INGAME_UI(source,messageTextEdit);
  RichTextCommandStream_CopyToNarrow
            (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,messageTextEdit->textBuffer);
  /* Original quirk: the result is not tested; with no tab selected this is the last tab */
  UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,
      INGAME_UI(source,messageRecipientAllTab),
      INGAME_UI(source,messageRecipientGroupsTab),
      INGAME_UI(source,messageRecipientPlayersTab));
  if (recipientTab == (UiNodeBase *)INGAME_UI(source,messageRecipientPlayersTab)) {
    recipientMask = InGameChatInput_CollectTickedSlotBits(source,INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE);
  }
  else if (recipientTab == (UiNodeBase *)INGAME_UI(source,messageRecipientGroupsTab)) {
    recipientMask = InGameChatInput_CollectTickedSlotBits(source,INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE);
  }
  else {
    recipientMask = INGAME_CHAT_RECIPIENT_EVERYONE;
  }
  InGameChatInput_SendPayloadText(recipientMask);
  messageTextEdit->cursorIndex = 0;
  messageTextEdit->selectionStart = 0;
  messageTextEdit->selectionEnd = 0;
  for (unitIndex = 0; unitIndex < 48; unitIndex++) {
    messageTextEdit->textBuffer[unitIndex] = 0;
  }
  return;
}
