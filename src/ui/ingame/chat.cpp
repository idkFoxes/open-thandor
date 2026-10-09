/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/chat.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/chat.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

#include <cstring>
#include <span>

/* Module data. */

/* The message window's seven recipient check boxes (the original kept their offsets from the image start,
   8420 + 96 * n). */
static constexpr UiTextButtonControl InGameUiImage::*const g_UiSevenSlotSelectionControls[7] = {
    &InGameUiImage::messageRecipientCheckbox1, &InGameUiImage::messageRecipientCheckbox2,
    &InGameUiImage::messageRecipientCheckbox3, &InGameUiImage::messageRecipientCheckbox4,
    &InGameUiImage::messageRecipientCheckbox5, &InGameUiImage::messageRecipientCheckbox6,
    &InGameUiImage::messageRecipientCheckbox7};

static uint16_t g_DeveloperChatPhraseUtf16[32] = {'O', 'h', ' ', 'g', 'r', 'o', 's', 's', 'e', 'r', ' ', 'T', 'h', 'o', 'm', 'a', 's', ',', ' ', 'e', 'r', 'l', 0xF6, 's', 'e', ' ', 'm', 'i', 'c', 'h', '!', 0}; /* L"Oh grosser Thomas, erl\366se mich!" */

static uint16_t g_HmmNaGutChatPhraseUtf16[20] = {'H', 'm', 'm', 'm', ',', ' ', 'n', 'a', ' ', 'g', 'u', 't', '.', '.', '.', ' ', ';', '-', ')', 0}; /* L"Hmmm, na gut... ;-)" */

/* UI action 0x1005 (g_InGameUiActionHandlersPage10[5], InGameUiImage.messageSendAndCloseButton): sends the
   typed message to the chosen recipients (InGameSevenSlotCommand_SubmitTextAndSelectionMask) and closes the
   message window.
*/
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source)

{
  InGameSevenSlotCommand_SubmitTextAndSelectionMask(source);
  InGameSevenSlotCommand_ClosePage(source);
}

/* True when the first 32 UTF-16 units of the chat line equal the cheat phrase g_DeveloperChatPhraseUtf16. The
   original compares 16 dwords (REPE CMPSD) through int pointers; done here as a memcmp of exactly the phrase's
   64 bytes, which stays inside the 48-unit text buffer and needs no type-punned reads. */
static bool InGameChatInput_MatchesCheatPhrase(std::span<const uint16_t> text)

{
  static_assert(sizeof(g_DeveloperChatPhraseUtf16) == 64,"the cheat phrase compare reads 64 bytes of the chat line");
  /* the chat line holds 0x30 units (96 bytes), so the size test only fails for a shorter capacity */
  return text.size_bytes() >= sizeof(g_DeveloperChatPhraseUtf16) &&
         std::memcmp(g_DeveloperChatPhraseUtf16,text.data(),sizeof(g_DeveloperChatPhraseUtf16)) == 0;
}

/* Recipient bits of the message window's seven check boxes (g_UiSevenSlotSelectionControls of the
   in-game UI root uiRoot): bit n+1 above baseBit is set when box n is ticked. Shared by the chat line and the
   message window. */
static CommandPayload InGameChatInput_CollectTickedSlotBits(UiNodeBase *uiRoot,uint32_t baseBit)

{
  uint32_t slotIndex;
  uint32_t slotBit;
  CommandPayload recipientMask;
  bool isSelected;

  recipientMask = 0;
  slotBit = baseBit;
  for (slotIndex = 0; slotIndex < 7; slotIndex++) {
    slotBit = slotBit * 2;
    isSelected = UiSelectableControl_IsSelected
                            (&(InGameUi_Image(uiRoot)->*g_UiSevenSlotSelectionControls[slotIndex]).selectable);
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

  InGameCommand_Issue<FrontendPlayerTextCommand_SetPackedState>(0,0,recipientMask);
  for (tripleIndex = 0; tripleIndex < 4; tripleIndex++) {
    InGameCommand_Issue<FrontendPlayerTextCommand_AppendTripleClamped>
              (g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload1,
               g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload2,
               g_UiSevenSlotCommandPayloadText.triples[tripleIndex].payload3);
  }
  InGameCommand_Issue<FrontendPlayerTextCommand_PublishConditionalRichText>(0,0,0);
}

/* UI action 0x1024 (g_InGameUiActionHandlersPage10[36]): Enter in the in-game chat line. In a local game the
   line is only compared with the cheat phrase g_DeveloperChatPhraseUtf16, which toggles the cheats and answers
   with a message. In a network game the text is sent to the recipients chosen in the message window (all, the
   ticked factions or the ticked session players) and the line is cleared. Either way the command page closes.
*/
void InGameChatInput_SendLineOrCheckCheatPhrase(InGameCommandTextEntryPageTextEditPtr commandTextEdit)

{
  CommandPayload recipientMask;
  uint32_t unitIndex;
  UiNodeBase *recipientTab;
  /* commandTextEdit is chatInputTextEdit of the in-game UI template copy */
  InGameUiImage *image = THANDOR_CONTAINER_OF(commandTextEdit, InGameUiImage, chatInputTextEdit);

  UiTextControl_UpdateNonEmptyValidity(reinterpret_cast<UiTextEditControl *>(commandTextEdit));
  if (Any(commandTextEdit->editStateFlags & UI_TEXT_EDIT_VALUE_VALID)) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      if (InGameChatInput_MatchesCheatPhrase(UiTextEdit_Text(commandTextEdit))) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED;
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_CHEAT_PHRASE_ENTERED;
        InGameRecentTextHistory_InsertAndRebuild8(g_HmmNaGutChatPhraseUtf16);
      }
    }
    else {
      RichTextCommandStream_CopyToNarrow
                (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,
                 UiTextEdit_Text(commandTextEdit).data());
      /* Original quirk: the result is not tested; with no tab selected this is the last tab */
      UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,
      &image->messageRecipientAllTab.selectable.base,
      &image->messageRecipientGroupsTab.selectable.base,
      &image->messageRecipientPlayersTab.selectable.base);
      /* Recipient mask: bit 9+n when box n of the faction tab is ticked (messageRecipientPlayersTab), bit 16+n
         for box n of the session player tab (messageRecipientGroupsTab), 0xFFFFFF00 for everyone. */
      if (recipientTab == &image->messageRecipientPlayersTab.selectable.base) {
        recipientMask = InGameChatInput_CollectTickedSlotBits
                                  (&image->inGameRootPanel.root.base,INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE);
      }
      else if (recipientTab == &image->messageRecipientGroupsTab.selectable.base) {
        recipientMask = InGameChatInput_CollectTickedSlotBits
                                  (&image->inGameRootPanel.root.base,INGAME_CHAT_RECIPIENT_PLAYER_BITS_BASE);
      }
      else {
        recipientMask = INGAME_CHAT_RECIPIENT_EVERYONE;
      }
      InGameChatInput_SendPayloadText(recipientMask);
      commandTextEdit->cursorIndex = 0;
      commandTextEdit->selectionStart = 0;
      commandTextEdit->selectionEnd = 0;
      for (unitIndex = 0; unitIndex < 48; unitIndex++) {
        UiTextEdit_Text(commandTextEdit)[unitIndex] = 0;
      }
    }
  }
  UiPageStack_SetActiveIndex(0,&THANDOR_CONTAINER_OF(commandTextEdit, InGameCommandTextEntryPage2320, commandTextEdit)->commandPageStack);
}

/* UI action 0x1006 (g_InGameUiActionHandlersPage10[6]): the recipient tab InGameUiImage.messageRecipientPlayersTab
   of the message window. Selects the tab, shows the check box page and gives one check box to each faction
   still in the game (factions 1..7),
   labelled with the faction name (label texts 0x216D.. patched with name text 0x2173 + name index); the list
   is sized to the used rows and the unused check boxes are hidden.
*/
void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source)

{
  UiNodeFlags *controlFlags;
  UiNodeBase *uiRootNode;
  int factionNameIndex;
  uint16_t *stream;
  TextResourceId resourceId;
  uint32_t factionIndexCursor;
  const GameFactionRuntimeRecord *factionRecord;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  InGameUiImage *image = InGameUi_Image(uiRootNode);
  UiSelectableGroup_SelectExclusive(3,source,
      &image->messageRecipientAllTab.selectable.base,
      &image->messageRecipientGroupsTab.selectable.base,
      &image->messageRecipientPlayersTab.selectable.base);
  UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  factionIndexCursor = 1;
  /* a faction is named after its colour: colorIndex of the faction record selects the name text */
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  while (factionIndexCursor < 8) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndexCursor] != 0) {
      resolvedText = TextResource_Resolve(resourceId);
      stream = resolvedText;
      factionNameIndex = factionRecord->colorIndex;
      resourceId++;
      /* nodeFlags of the check box */
      controlFlags = &(image->*g_UiSevenSlotSelectionControls[filledSlotCount]).selectable.base.nodeFlags;
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      filledSlotCount++;
      resolvedText = TextResource_Resolve(factionNameIndex + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_PatchPayloadBySelector(0,resolvedText,stream);
    }
    factionIndexCursor++;
    factionRecord++;
  }
  image->messageRecipientList.root.base.bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            (&image->messageRecipientScroll);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,&image->messageRecipientScroll);
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = &(image->*g_UiSevenSlotSelectionControls[filledSlotCount]).selectable.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
}

/* UI action 0x1007 (g_InGameUiActionHandlersPage10[7]): the recipient tab InGameUiImage.messageRecipientGroupsTab
   of the message window. Like action 0x1006, but gives one check box to each of the first seven session
   players (g_FrontendPlayerRuntimeBlocks), labelled with the player name from the player's selection block.
*/
void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source)

{
  UiNodeFlags *controlFlags;
  UiNodeBase *uiRootNode;
  SelectionPlayerRuntimeBlock *selectionBlock;
  TextResourceId resourceId;
  uint32_t filledSlotCount;
  uint16_t *resolvedText;
  
  uiRootNode = source;
  while (uiRootNode->parent != UI_NODE_NONE) {
    uiRootNode = uiRootNode->parent;
  }
  InGameUiImage *image = InGameUi_Image(uiRootNode);
  UiSelectableGroup_SelectExclusive(3,source,
      &image->messageRecipientAllTab.selectable.base,
      &image->messageRecipientGroupsTab.selectable.base,
      &image->messageRecipientPlayersTab.selectable.base);
  UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&image->messageRecipientPageStack));
  resourceId = TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE;
  filledSlotCount = 0;
  /* The original is a do/while that fills one box even with a player count of 0 (reading an unset player
     block's selection pointer); bounded here because the count drops to 0 when the network session falls
     apart (players leaving, lobby reset). For a count >= 1 the loop runs exactly as before. */
  while ((filledSlotCount < g_FrontendPlayerRuntimeBlockCount) && (filledSlotCount < 7)) {
    resolvedText = TextResource_Resolve(resourceId);
    selectionBlock = g_SelectionPlayerRuntimeBlockPointers
             [g_FrontendPlayerRuntimeBlocks[filledSlotCount].playerRuntimeId];
    resourceId++;
    controlFlags = &(image->*g_UiSevenSlotSelectionControls[filledSlotCount]).selectable.base.nodeFlags;
    *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
    filledSlotCount++;
    RichTextCommandStream_PatchPayloadBySelector(0,selectionBlock->playerNameUtf16,resolvedText);
  }
  image->messageRecipientList.root.base.bottomOffset = filledSlotCount * 24; /* 24-pixel rows */
  UiScrollableControl_RebuildViewportAndScrollbars
            (&image->messageRecipientScroll);
  UiScrollableControl_ClampOffsetsToViewport
            (0,0,0,0,&image->messageRecipientScroll);
  for (; filledSlotCount < 7; filledSlotCount++) {
    controlFlags = &(image->*g_UiSevenSlotSelectionControls[filledSlotCount]).selectable.base.nodeFlags;
    *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
  }
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
  InGameUiImage *image = InGameUi_Image(rootNodeCursor);
  UiSelectableGroup_SelectExclusive(3,source,
      &image->messageRecipientAllTab.selectable.base,
      &image->messageRecipientGroupsTab.selectable.base,
      &image->messageRecipientPlayersTab.selectable.base);
  UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&image->messageRecipientPageStack));
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
}

/* UI action 0x1002 (g_InGameUiActionHandlersPage10[2], InGameUiImage.messageCancelButton): closes the message
   window - shows the world view again and switches the game window page stack back to page 0.
*/
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source)

{

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  InGameUiImage *image = InGameUi_Image(source);
  image->worldView.base.nodeFlags = image->worldView.base.nodeFlags & ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,UiLayoutContainerControl_AsPageStack(&image->gameWindowPageStack));
}

/* UI action 0x1004 (g_InGameUiActionHandlersPage10[4], InGameUiImage.messageSendButton; also called by
   InGameSevenSlotCommand_SubmitAndClosePage): sends the text of the message window. The text is narrowed to
   48 bytes, the recipient mask is built from the selected tab and its check boxes (as in
   InGameChatInput_SendLineOrCheckCheatPhrase), then the mask, the text as four 12-byte chat commands and the publish command
   are issued, and the text field is cleared.
*/
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source)

{
  InGameCommandTextEditControlCC *messageTextEdit;
  CommandPayload recipientMask;
  uint32_t unitIndex;
  UiNodeBase *recipientTab;

  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  InGameUiImage *image = InGameUi_Image(source);
  /* the 48-code-unit view of the message text edit (its text buffer runs on into the trailing template dwords) */
  messageTextEdit = reinterpret_cast<InGameCommandTextEditControlCC *>(&image->messageTextEdit);
  RichTextCommandStream_CopyToNarrow
            (sizeof(g_UiSevenSlotCommandPayloadText.textBytes),g_UiSevenSlotCommandPayloadText.textBytes,UiTextEdit_Text(messageTextEdit).data());
  /* Original quirk: the result is not tested; with no tab selected this is the last tab */
  UiSelectableGroup_FindVisibleSelected(&recipientTab,nullptr,3,
      &image->messageRecipientAllTab.selectable.base,
      &image->messageRecipientGroupsTab.selectable.base,
      &image->messageRecipientPlayersTab.selectable.base);
  if (recipientTab == &image->messageRecipientPlayersTab.selectable.base) {
    recipientMask = InGameChatInput_CollectTickedSlotBits(source,INGAME_CHAT_RECIPIENT_FACTION_BITS_BASE);
  }
  else if (recipientTab == &image->messageRecipientGroupsTab.selectable.base) {
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
    UiTextEdit_Text(messageTextEdit)[unitIndex] = 0;
  }
}
