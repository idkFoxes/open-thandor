/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/player.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/player.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/frontend/player. */

/* Address: 0x00548CD0.
   Handler of UI action 0x204C (slot 76 of g_UiActionPage20InitializedHandlers), the lobby's chatInputEdit:
   when the typed line is valid, converts it to 0x30 narrow bytes and sends it to every player as
   FRONTEND_COMMAND_CHAT_BEGIN, four FRONTEND_COMMAND_CHAT_APPEND (12 bytes each) and
   FRONTEND_COMMAND_CHAT_PUBLISH (without a network session the handlers are called directly), then empties
   the edit field. The in-game counterpart is InGameUiAction1024_Handler.
*/
void FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl)

{
  int remainingPairs;
  uint16_t *textCursor;
  
  UiTextControl_UpdateNonEmptyValidity(textEditControl);
  if ((textEditControl->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    RichTextCommandStream_CopyToNarrow
              (PLAYER_CHAT_TEXT_BYTES,g_UiSevenSlotCommandPayloadText.textBytes,textEditControl->textBuffer);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      /* the in-game "all recipients" mask; the lobby handler ignores it */
      FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById(g_LocalPlayerRuntimeId,0,0,PLAYER_CHAT_RECIPIENTS_ALL);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CHAT_BEGIN,0,0,PLAYER_CHAT_RECIPIENTS_ALL);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_AppendTripleById
                (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand
                (FRONTEND_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[0].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[0].payloadDword04);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_AppendTripleById
                (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand
                (FRONTEND_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[1].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[1].payloadDword04);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_AppendTripleById
                (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand
                (FRONTEND_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[2].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[2].payloadDword04);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_AppendTripleById
                (g_LocalPlayerRuntimeId,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand
                (FRONTEND_COMMAND_CHAT_APPEND,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_PublishTextById(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CHAT_PUBLISH,0,0,0);
    }
    textEditControl->cursorIndex = 0;
    textEditControl->selectionStart = 0;
    textEditControl->selectionEnd = 0;
    /* clear the 48 UTF-16 units of the text, two per step */
    textCursor = textEditControl->textBuffer;
    for (remainingPairs = 0x18; remainingPairs != 0; remainingPairs--) {
      textCursor[0] = 0;
      textCursor[1] = 0;
      textCursor = textCursor + 2;
    }
  }
  return;
}


/* Address: 0x00560750.
   Handler of INGAME_COMMAND_SELECT_MODEL_AND_ARMY: when both tokens still name live objects, selects the
   object for the player (FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection), shows the technology
   page of the game window for the local player and records the definition token
   (FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80). Tokens are pointer offsets so they can travel in
   network commands; dword +4 of the target is non-zero while it is alive.
*/
void FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken)

{
  InGameRuntimeRootImageC3E4 *inGameRoot;

  /* note the crossed bases: modelToken is an army-slot offset, armyToken one from g_ModelRuntimeRebaseDelta */
  if ((((modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne != 0) &&
       (armyToken + g_ModelRuntimeRebaseDelta != 0)) &&
      (((ArmyRuntimeSlot *)(modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne))->modelNodeRuntime != NULL)) &&
     ((ModelRuntimeSlot *)(armyToken + g_ModelRuntimeRebaseDelta))->rootModelNodeOrSavedOffset.modelNode != NULL) {
    FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection(playerIndex,0,0,modelToken);
    inGameRoot = g_InGameRuntimeRoot;
    if (playerIndex == g_LocalPlayerRuntimeId) {
      /* page 2 of the game window: the technology panel */
      UiPageStack_SetActiveIndex(2,&g_InGameRuntimeRoot->gameWindowPageStack0BD0);
      InGameTechnologyPanel_ResetAndSelectCurrentArea(&inGameRoot->rootUi0000);
    }
    FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80(playerIndex,0,0,armyToken);
  }
  return;
}


/* Address: 0x00549AF0.
   Handler of UI action 0x2042 (slot 66 of g_UiActionPage20InitializedHandlers), the factionSetupFinishButton
   check box of the faction setup page: sends its checked state (UI_SELECTABLE_SELECTED_OR_CHECKED or 0) as
   this player's consensus value with FRONTEND_COMMAND_SET_CONSENSUS_VALUE, or applies it directly without a
   network session.
*/
void FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source)

{
  uint32_t consensusValue;
  
  consensusValue = ((UiSelectableControl *)source)->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_SetConsensusValueAndRefresh(g_LocalPlayerRuntimeId,0,0,consensusValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SET_CONSENSUS_VALUE,0,0,consensusValue);
  }
  return;
}


/* Address: 0x0054D3A0.
   Handler of FRONTEND_ACTION_KICK_PLAYER (slot 11 of g_UiActionPage20InitializedHandlers), the host lobby's
   hostLobbyKickPlayerButton: sets the heartbeat expiry of the player selected in the player list to 1 and runs
   the lobby's expiry pass at once, which removes that player (and counts one extra tick for every other
   joined player). The first row, the host itself, cannot be kicked.
*/
void FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode)

{
  UiNodeBase *parentCursor;
  FrontendPlayerRuntimeRecord **selectedPlayerRuntimeSlot;
  
  /* rootNode starts as the kick button and walks up to the frontend root */
  parentCursor = rootNode->base.parent;
  while (parentCursor != UI_NODE_NONE) {
    rootNode = (UiRootNode *)rootNode->base.parent;
    parentCursor = rootNode->base.parent;
  }
  /* the list's row slots point at the player blocks */
  selectedPlayerRuntimeSlot = (FrontendPlayerRuntimeRecord **)
       ((FrontendNetworkListsRuntimeView *)rootNode)->playerRuntimeList.selectedRowSlot;
  if (selectedPlayerRuntimeSlot != (FrontendPlayerRuntimeRecord **)
      ((FrontendNetworkListsRuntimeView *)rootNode)->playerRuntimeList.rowSlots) {
    (*selectedPlayerRuntimeSlot)->heartbeatExpiryTicks = 1;
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks
              ((FrontendNetworkListsRuntimeView *)rootNode);
  }
  return;
}


/* Address: 0x0054F540.
   Host after the session start (FRONTEND_NETWORK_STATE_HOST_STARTING), from the frontend root's tick
   (FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState): counts down every client's heartbeat expiry. A
   client that timed out is announced with TEXT_ID_NETWORK_PLAYER_REMOVED and dropped by compacting the player
   blocks and their 0x20-byte command records. Every remaining client is then sent one
   FRONTEND_PACKET_10007_PLAYER_REMOVAL per removed player, and the ready wait is re-evaluated without a new
   report (player id -1).
   The command records start at g_FrontendPlayerCommandRecords[0] while the scan starts at player block 1, and
   the announcements go out in reverse removal order (the original pushes each removed id on the stack and pops
   one per round); both are kept from the original.
*/
void FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void)

{
  FrontendPlayerRuntimeBlockCount sendRemaining;
  int scanRemaining;
  int removedCount;
  FrontendPlayerRuntimeRecord *sourceBlock;
  FrontendPlayerRuntimeRecord *destBlock;
  FrontendCommandPacketRecord *commandSource;
  FrontendCommandPacketRecord *commandDest;
  UiTransferEndpointDescriptor *endpoint;
  TextResolveResult removalText;
  /* the original's PUSH/POP stack of removed player ids (at most 8 player blocks) */
  FrontendPlayerRuntimeId removedPlayerIds[8];
  
  removedCount = 0;
  sourceBlock = g_FrontendPlayerRuntimeBlocks + 1;
  destBlock = g_FrontendPlayerRuntimeBlocks + 1;
  commandSource = g_FrontendPlayerCommandRecords;
  commandDest = g_FrontendPlayerCommandRecords;
  scanRemaining = (int)g_FrontendPlayerRuntimeBlockCount - 1;
  if (0 < scanRemaining) {
    do {
      sourceBlock->heartbeatExpiryTicks = sourceBlock->heartbeatExpiryTicks - 1;
      if (sourceBlock->heartbeatExpiryTicks == 0) {
        /* remove: only the source cursors advance */
        g_FrontendPlayerRuntimeBlockCount--;
        removalText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
        RichTextCommandStream_PatchPayloadBySelector(0,&sourceBlock->playerName,removalText.text);
        FrontendRecentTextHistory_InsertAndRebuild5(removalText.text);
        removedPlayerIds[removedCount] = sourceBlock->playerRuntimeId;
        removedCount++;
      }
      else {
        /* keep: REP MOVSD of the 0x13B0-byte player block, then of its 8-dword command record */
        if (destBlock != sourceBlock) {
          *destBlock = *sourceBlock;
          *commandDest = *commandSource;
        }
        destBlock++;
        commandDest++;
      }
      sourceBlock++;
      commandSource++;
      scanRemaining--;
    } while (scanRemaining != 0);
  }
  if (removedCount != 0) {
    do {
      endpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
      g_FrontendPlayerRemovalPacket10007.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_10007_PLAYER_REMOVAL;
      /* POP: the last removed id first */
      g_FrontendPlayerRemovalPacket10007.removedPlayerToken = removedPlayerIds[removedCount - 1];
      sendRemaining = g_FrontendPlayerRuntimeBlockCount;
      while (sendRemaining = sendRemaining - 1, sendRemaining != 0) {
        UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPlayerRemovalPacket10007.header);
        endpoint = endpoint + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
      }
      removedCount--;
    } while (removedCount != 0);
    FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(0xffffffff,0,0,0);
  }
  return;
}


/* Address: 0x00514EF0.
   Tells whether any player other than excludedPlayerId has assignmentToken recorded in assignmentToken80A0
   (see FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80); the in-game HUD uses it to decide whether the
   technology window of a selected object is offered. CF set when such a player exists.
*/
bool FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
          (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  while ((playerBlock->playerRuntimeId == excludedPlayerId ||
         (g_SelectionPlayerRuntimeBlockPointers[playerBlock->playerRuntimeId]->assignmentToken80A0 !=
          assignmentToken))) {
    playerBlock++;
    remainingBlocks--;
    if (remainingBlocks == 0) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00514F60.
   Releases an assignment token: every frontend player whose selection player block (+0x80A0) still holds
   the token gets it cleared to 0. Assumes at least one frontend player block (do/while as in the original).
*/
void FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken)

{
  uint32_t playerBlocksRemaining;
  FrontendPlayerRuntimeRecord *playerBlockCursor;

  playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (assignmentToken ==
        g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId]->
        assignmentToken80A0) {
      g_SelectionPlayerRuntimeBlockPointers[playerBlockCursor->playerRuntimeId]->assignmentToken80A0
           = 0;
    }
    playerBlockCursor++;
    playerBlocksRemaining--;
  } while (playerBlocksRemaining != 0);
  return;
}


/* Address: 0x00544130.
   Handler of FRONTEND_COMMAND_BRIEFING_READY, which a client sends when it presses the mission briefing's
   "Begin" button (FrontendSessionAction_ApplySpeedOrToggleReady). On the client that pressed it, the button is
   switched off (UI_NODE_SUPPRESSED). On the host the player is marked ready (readyOrWaitState 1); once every
   client (player blocks 1..n-1) is ready, the host's own "Begin" button is switched on.
*/
void FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *nextBlock;
  SessionNetworkRoleFlags hostFlagOrRemaining;
  SessionNetworkRoleFlags searchRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    if (playerId != g_LocalPlayerRuntimeId) {
      return;
    }
    FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags |= UI_NODE_SUPPRESSED;
    return;
  }
  /* only the host keeps track; the first test reads the host flag, later ones the remaining count */
  searchRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  hostFlagOrRemaining = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST;
  for (;;) {
    if (hostFlagOrRemaining == SESSION_NETWORK_ROLE_LOCAL) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    playerBlock++;
    searchRemaining--;
    hostFlagOrRemaining = searchRemaining;
  }
  playerBlock->factionAssignment.readyOrWaitState = 1;
  /* are all clients ready? block 0 is the host */
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  do {
    remainingBlocks--;
    if (remainingBlocks == 0) {
      FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      return;
    }
    nextBlock = playerBlock + 1;
    playerBlock++;
  } while (nextBlock->factionAssignment.readyOrWaitState != 0);
  return;
}


/* Address: 0x005442B0.
   Sets FRONTEND_PLAYER_STATE_LEVEL_RECEIVED in the roleStateFlags of the player block with this player id (the level's field grid arrived).
   Command handler with four dword arguments (local command 0x360, run on every peer in a network session);
   only the player id is used.
*/
void FrontendPlayerRuntime_MarkLevelReceivedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->factionAssignment.roleStateFlags |= FRONTEND_PLAYER_STATE_LEVEL_RECEIVED;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544300.
   Command handler FRONTEND_COMMAND_XOR_PLAYER_STATE (run on every peer): toggles the stateMask bits in
   colourCycleFlags of the player block with this player id. The two middle command arguments are unused.
*/
void FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t stateMask)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->colourCycleFlags = playerBlock->colourCycleFlags ^ stateMask;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544360.
   Sets FRONTEND_PLAYER_STATE_LEVEL_LOADED in the roleStateFlags of the player block with this player id (the level package was loaded from disk).
   Command handler with four dword arguments (local command 0x410, run on every peer in a network session);
   only the player id is used.
*/
void FrontendPlayerRuntime_MarkLevelLoadedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->factionAssignment.roleStateFlags |= FRONTEND_PLAYER_STATE_LEVEL_LOADED;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544820.
   Sets FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT in the roleStateFlags of the player block with this player id (the level arrived, the player can go on to task assignment).
   Command handler with four dword arguments (local command 0x8D0, run on every peer in a network session);
   only the player id is used.
*/
void FrontendPlayerRuntime_MarkTaskAssignmentReadyById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->factionAssignment.roleStateFlags |= FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544D50.
   Handler of FRONTEND_COMMAND_SCENARIO_CATALOG_RECEIVED, which a client sends once it has unpacked the host's
   scenario catalogue (FrontendScenarioTransfer_ProcessReceivedAsset): sets FRONTEND_PLAYER_STATE_SCENARIO_CATALOG
   for the player and stores its 96-bit level mask (bit n = level record n, mask 0 holds records 0..31), which
   FrontendScenarioSession_LoadOrRequestLevelAsset consults later. The command sends the dwords high first.
   No handler is called directly: only network clients send it.
*/
void FrontendPlayerRuntime_MarkScenarioCatalogReceivedById
          (PlayerRuntimeId playerId,FrontendPlayerValue8C scenarioAvailabilityMask2,
          FrontendPlayerValue88 scenarioAvailabilityMask1,
          FrontendPlayerValue84 scenarioAvailabilityMask0)

{
  FrontendRoleStateFlags *roleFlags;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      roleFlags = &playerBlock->factionAssignment.roleStateFlags;
      *roleFlags = *roleFlags | FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
      playerBlock->scenarioAvailabilityMask0 = scenarioAvailabilityMask0;
      playerBlock->scenarioAvailabilityMask1 = scenarioAvailabilityMask1;
      playerBlock->scenarioAvailabilityMask2 = scenarioAvailabilityMask2;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00549190.
   Default faction line-up for a freshly loaded level (scenario catalogue, frontend main loop): marks factions
   1..active count as active and clears the slots above, then hands the players the assignable factions
   round-robin (player n gets faction (n mod assignable count) + 1) and clears their ready and consensus state.
   The original also computes the local player's zero-based faction in EDX but restores EDX before returning.
*/
void FrontendPlayerRuntime_InitializeFactionAssignments(void)

{
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  uint32_t activeRemaining;
  uint32_t factionSlot;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  uint32_t assignableRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;

  loadedLevel = g_FrontendLoadedLevelAsset;
  factionSlot = 1; /* slot 0 is not a player faction */
  activeRemaining = g_FrontendLoadedLevelAsset->runtimeTail2E0.activeFactionCount;
  assignableRemaining = g_FrontendLoadedLevelAsset->runtimeTail2E0.assignableFactionCount;
  /* the assignable factions come first, the remaining active (computer-only) ones follow */
  do {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    activeRemaining--;
    factionSlot++;
    assignableRemaining--;
  } while (assignableRemaining != 0);
  for (; activeRemaining != 0; activeRemaining--) {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    factionSlot++;
  }
  /* slot 7 is only cleared when fewer than six factions are active (factionSlot < 7), as in the original */
  if (factionSlot < 7) {
    do {
      g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = 0;
      factionSlot++;
    } while (factionSlot < 8);
  }
  factionSlot = 1;
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    playerBlock->factionAssignment.factionAssignmentIndex = factionSlot;
    playerBlock->factionAssignment.readyOrWaitState = 0;
    playerBlock->factionAssignment.consensusValue = 0;
    factionSlot++;
    playerBlock++;
    if (loadedLevel->runtimeTail2E0.assignableFactionCount < factionSlot) {
      factionSlot = factionSlot - loadedLevel->runtimeTail2E0.assignableFactionCount;
    }
    remainingBlocks--;
  } while (remainingBlocks != 0);
}


/* Address: 0x0054D000.
   Handler of UI action 0x2005 (slot 5 of g_UiActionPage20InitializedHandlers), the host lobby's "Back" button
   (hostLobbyBackButton): returns to the host game setup page, stops the menu room rendering on compact
   layouts, leaves the network session (local mode, FRONTEND_NETWORK_STATE_IDLE) and shrinks the roster to the
   local player alone (id 0, empty name, cleared state). The speed slider is reset from
   g_SessionNetworkTickInterval (the slider shows half the interval).
*/
void FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *firstPlayerBlock;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  uint32_t sessionTickInterval;
  /* source is the frontend template's hostLobbyBackButton (+0x543C). */
  FrontendUiImage *frontendUi;

  frontendUi = (FrontendUiImage *)((uint8_t *)source - offsetof(FrontendUiImage,hostLobbyBackButton));
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_HOST_GAME_SETUP,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  sessionTickInterval = g_SessionNetworkTickInterval;
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendUi,menuRoomModelView))->contextFlags |=
         FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,networkSpeedSlider))->value = sessionTickInterval >> 1;
  firstPlayerBlock = g_FrontendPlayerRuntimeBlocks;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  localPlayerRecord->playerName.textUtf16[0] = 0;
  localPlayerRecord->playerName.textUtf16[1] = 0;
  firstPlayerBlock->playerRuntimeId = 0;
  firstPlayerBlock->factionAssignment.roleStateFlags = 0;
  firstPlayerBlock->colourCycleFlags = 0;
  firstPlayerBlock->snapshotTransferFlags = 0;
  return;
}


/* Address: 0x0054D1B0.
   Handler of UI action 0x204D (slot 77 of g_UiActionPage20InitializedHandlers), the host game setup page's
   networkSpeedSlider (1..7): shows the speed's name (TEXT_ID_NETWORK_SPEED_BASE + value) in the label beside it
   and sets g_SessionNetworkTickInterval to twice the slider value. The label buffer is the one named
   g_FrontendNetworkPlayerCountLabelUtf16.
*/
void FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source)

{
  TextResolveResult labelText;
  
  g_SessionNetworkTickInterval = ((UiRangeSliderControl *)source)->value;
  labelText = TextResource_Resolve(g_SessionNetworkTickInterval + TEXT_ID_NETWORK_SPEED_BASE);
  RichTextCommandStream_CopyExpanded
            (0x40,(uint16_t *)&g_FrontendNetworkPlayerCountLabelUtf16,labelText.text);
  g_SessionNetworkTickInterval = g_SessionNetworkTickInterval << 1;
  return;
}


/* Address: 0x0054D720.
   Host lobby: shows the start button (FRONTEND_ACTION_START_NETWORK_GAME) only while at least a third of the
   players report FRONTEND_CAPABILITY_CD, i.e. run the game from the CD; otherwise hides it. Called whenever a
   player joins or a heartbeat updates the capabilities.
*/
void FrontendPlayerRuntime_UpdateStartButtonByCdShare(void)

{
  int cdPlayerCount;
  uint32_t remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;

  cdPlayerCount = 0;
  remainingBlocks = g_FrontendPlayerRuntimeCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((playerBlock->capabilityFlags & FRONTEND_CAPABILITY_CD) != 0) {
      cdPlayerCount++;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  if ((uint32_t)(cdPlayerCount * 3) < g_FrontendPlayerRuntimeCount) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_START_NETWORK_GAME,g_FrontendRootNode);
  }
  else {
    UiNodeList_UnsuppressActionId(FRONTEND_ACTION_START_NETWORK_GAME,g_FrontendRootNode);
  }
  return;
}


/* Address: 0x0055F470.
   Handler of INGAME_COMMAND_SET_SESSION_FLAGS: sets or clears PLAYER_SESSION_FLAG_SLOW_RENDERING of a player
   (slowRenderingFlag is that bit or 0), which the player roster shows as a highlighted "W". Other session
   flags are kept.
*/
void FrontendPlayerRuntime_SetSlowRenderingFlagById
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedArg04,uint32_t reservedArg08,
          FrontendReadyFlagMask slowRenderingFlag)

{
  uint8_t *readyFlagsField;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  readyFlagsField = (uint8_t *)&playerRuntimeBlock->sessionFlags;
  *(uint32_t *)readyFlagsField = *(uint32_t *)readyFlagsField & ~PLAYER_SESSION_FLAG_SLOW_RENDERING;
  playerRuntimeBlock->sessionFlags = playerRuntimeBlock->sessionFlags | slowRenderingFlag;
  return;
}


/* Address: 0x0055F5A0.
   Results screen of a network game: a client that pressed continue (command 0x470) is marked ready on the
   host, and the host's own continue button (INGAME_ACTION_RESULTS_CONTINUE) appears once every other player
   is ready; the host re-checks with player id 0xFFFFFFFF every frame. On a client the local player's own
   continue button disappears after pressing it (it then waits for the host).
*/
void FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(PlayerRuntimeId playerRuntimeId)

{
  FrontendPlayerRuntimeBlockCount readyScanRemaining;
  FrontendPlayerRuntimeRecord *readyScanBlock;
  FrontendPlayerRuntimeBlockCount searchRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    searchRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerBlock = g_FrontendPlayerRuntimeBlocks;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      do {
        if (playerRuntimeId == playerBlock->playerRuntimeId) {
          playerBlock->factionAssignment.readyOrWaitState = 1;
          readyScanBlock = g_FrontendPlayerRuntimeBlocks;
          readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
          break;
        }
        searchRemaining--;
        playerBlock++;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      } while (searchRemaining != 0);
      /* every block after the host's own (block 0) must be ready */
      do {
        readyScanRemaining--;
        if (readyScanRemaining == 0) {
          UiNodeList_UnsuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)g_InGameRuntimeRoot);
          return;
        }
        playerBlock = readyScanBlock + 1;
        readyScanBlock++;
      } while (playerBlock->factionAssignment.readyOrWaitState != 0);
    }
  }
  else if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055F680.
   Handler of INGAME_COMMAND_PLAYER_READY (a player has loaded the level): counts the report in the player's
   readyOrWaitState. When every player has reported, the host sends the command a second time; once the host's
   count reaches 2 (or at once in a local game), the session's start pause ends
   (UI_COMMAND_RUNTIME_FLAG_PAUSED and _WAITING_FOR_PLAYERS cleared). A client ends it when the host's
   (id 0) second report arrives.
*/
void FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount searchRemaining;
  FrontendPlayerRuntimeBlockCount readyScanRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeRecord *readyScanBlock;
  
  searchRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if (playerRuntimeId == playerBlock->playerRuntimeId) {
        playerBlock->factionAssignment.readyOrWaitState++;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerRuntimeId != 0) {
            return;
          }
          if (playerBlock->factionAssignment.readyOrWaitState < 2) {
            return;
          }
          /* Client: the host (id 0) reached consensus. */
          if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0) {
            g_UiCommandRuntimeFlags &=
                 ~(UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS);
          }
          return;
        }
        break;
      }
      searchRemaining--;
      readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      readyScanBlock = g_FrontendPlayerRuntimeBlocks;
      playerBlock++;
    } while (searchRemaining != 0);
    /* host: wait until every player has reported */
    do {
      if (readyScanBlock->factionAssignment.readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining--;
      readyScanBlock++;
    } while (readyScanRemaining != 0);
    if (g_FrontendPlayerRuntimeBlocks->factionAssignment.readyOrWaitState < 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
        return;
      }
      FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0) {
    g_UiCommandRuntimeFlags &=
         ~(UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS);
  }
  return;
}


/* Address: 0x0055FB90.
   Command handler INGAME_COMMAND_SELECTION_INSERT: adds up to three armies (saved offsets, 0 = none; armies
   without a model are skipped) to the player's selection and rebuilds the selection panels for the local player.
*/
void FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0)

{
  if (armyRuntimeOffset0 != 0 &&
      ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common.ownership.modelNode !=
      NULL) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if (armyRuntimeOffset1 != 0 &&
      ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common.ownership.modelNode !=
      NULL) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if (armyRuntimeOffset2 != 0 &&
      ((GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common.ownership.modelNode !=
      NULL) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FC30.
   Command handler INGAME_COMMAND_SELECTION_REMOVE: removes up to three armies (saved offsets, 0 = none) from the
   player's selection and rebuilds the selection panels for the local player. Like the original it calls the
   removal once more for the last argument after the three checks (see the comment there).
*/
void FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerRuntimeId,RuntimeToken armyRuntimeOffset2,
          RuntimeToken armyRuntimeOffset1,RuntimeToken armyRuntimeOffset0)

{
  if (armyRuntimeOffset0 != 0 &&
      ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common.ownership.modelNode !=
      NULL) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if (armyRuntimeOffset1 != 0 &&
      ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common.ownership.modelNode !=
      NULL) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset2 != 0) &&
     (armyRuntimeOffset2 = armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne,
     ((GameEntityRuntime *)armyRuntimeOffset2)->common.ownership.modelNode != NULL)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)armyRuntimeOffset2,
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  /* Original quirk (0x0055FCA1): a fourth, unconditional removal with the third argument as left in EAX: the
     rebased pointer, or the raw offset 0 when it was empty. */
  SelectionPointerArray_RemoveFirstMatch
            ((GameEntityRuntime *)armyRuntimeOffset2,
             &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FCD0.
   Command handler INGAME_COMMAND_SELECTION_CLEAR: empties the player's 32-entry selection and rebuilds the
   selection panels for the local player. Only the first command argument is used.
*/
void FrontendPlayerSelection_ClearAndRefreshLocalPanels
          (FrontendPlayerIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FD10.
   Selection groups (keys 1..8, command 0xBE0): copies or merges between the player's selection and one of the
   faction's 8 groups of 32 armies, direction and merge chosen by the SELECTION_TRANSFER_* flags. Storing a
   selection first removes its armies from all groups of the faction. For the local player it rebuilds the
   selection panels and, with SELECTION_TRANSFER_CENTER_VIEW, moves the camera to the selection's centre.
*/
void FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
          (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex,
          FrontendSelectionTransferModeFlags transferModeFlags,
          FrontendFactionAssignmentIndex selectionGroupIndex)

{
  GameEntityRuntime *selectionEntry;
  InGameRuntimeRootImageC3E4 *node;
  int memberOrEntryRemaining;
  int scanRemaining;
  int entriesRemaining;
  ArmyRuntimeSlot **groupMemberSlot;
  SelectionPlayerRuntimeBlock *sourceCursor;
  SelectionPlayerRuntimeBlock *groupOrScanCursor;
  SelectionPlayerRuntimeBlock *nextScanCursor;
  SelectionPlayerRuntimeBlock *destCursor;
  bool foundEmpty;
  WorldPositionResult averagePosition;
  
  /* &g_GameFactionRuntimeImage.records[factionIndex].runtimeGroupMembers8x32[selectionGroupIndex * 32]
     (faction records of 0x740 bytes, the groups at +0x2E0, 0x80 bytes each) */
  groupOrScanCursor = (SelectionPlayerRuntimeBlock *)
           (selectionGroupIndex * 0x80 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x2e0) + factionIndex * 0x740);
  sourceCursor = groupOrScanCursor;
  destCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  if ((transferModeFlags & SELECTION_TRANSFER_TO_GROUP) != 0) {
    entriesRemaining = SELECTION_GROUP_ENTRY_COUNT;
    sourceCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    do {
      destCursor = sourceCursor;
      selectionEntry = destCursor->selection.entries[0];
      if (selectionEntry != NULL) {
        memberOrEntryRemaining = SELECTION_GROUP_COUNT * SELECTION_GROUP_ENTRY_COUNT;
        groupMemberSlot = g_GameFactionRuntimeImage.records[factionIndex].runtimeGroupMembers8x32;
        do {
          if (selectionEntry == (GameEntityRuntime *)*groupMemberSlot) {
            *groupMemberSlot = NULL;
          }
          groupMemberSlot++;
          memberOrEntryRemaining--;
        } while (memberOrEntryRemaining != 0);
      }
      entriesRemaining--;
      sourceCursor = (SelectionPlayerRuntimeBlock *)(destCursor->selection.entries + 1);
    } while (entriesRemaining != 0);
    /* back to the first selection entry (the typed expression is Ghidra's): selection -> group */
    sourceCursor = (SelectionPlayerRuntimeBlock *)&destCursor[-1].packedSelectionState809C;
    destCursor = groupOrScanCursor;
  }
  entriesRemaining = SELECTION_GROUP_ENTRY_COUNT;
  if ((transferModeFlags & SELECTION_TRANSFER_MERGE) == 0) {
    for (; entriesRemaining != 0; entriesRemaining--) {
      destCursor->selection.entries[0] = sourceCursor->selection.entries[0];
      sourceCursor = (SelectionPlayerRuntimeBlock *)(sourceCursor->selection.entries + 1);
      destCursor = (SelectionPlayerRuntimeBlock *)(destCursor->selection.entries + 1);
    }
  }
  else {
    memberOrEntryRemaining = SELECTION_GROUP_ENTRY_COUNT;
    do {
      selectionEntry = sourceCursor->selection.entries[0];
      scanRemaining = entriesRemaining;
      groupOrScanCursor = destCursor;
      if (selectionEntry != NULL) {
        do {
          if (selectionEntry == groupOrScanCursor->selection.entries[0]) break;
          scanRemaining--;
          groupOrScanCursor = (SelectionPlayerRuntimeBlock *)(groupOrScanCursor->selection.entries + 1);
        } while (scanRemaining != 0);
        /* Not yet in the destination group (the scan ran out; entriesRemaining is 0x20 here): store it in
           the first empty slot. */
        if (scanRemaining == 0) {
          foundEmpty = true;
          scanRemaining = entriesRemaining;
          groupOrScanCursor = destCursor;
          do {
            nextScanCursor = groupOrScanCursor;
            if (scanRemaining == 0) break;
            scanRemaining--;
            nextScanCursor = (SelectionPlayerRuntimeBlock *)(groupOrScanCursor->selection.entries + 1);
            foundEmpty = groupOrScanCursor->selection.entries[0] == NULL;
            groupOrScanCursor = nextScanCursor;
          } while (!foundEmpty);
          if (foundEmpty) {
            /* = nextScanCursor's previous entry, i.e. the empty slot found */
            nextScanCursor->selection.entries[-1] =
                 sourceCursor->selection.entries[0];
          }
        }
      }
      sourceCursor = (SelectionPlayerRuntimeBlock *)(sourceCursor->selection.entries + 1);
      memberOrEntryRemaining--;
    } while (memberOrEntryRemaining != 0);
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            (&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  node = g_InGameRuntimeRoot;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)node);
    if ((transferModeFlags & SELECTION_TRANSFER_CENTER_VIEW) != 0) {
      averagePosition = SelectionInfoEntitySlots_ComputeAverageWorldPositionRegs();
      if (!averagePosition.unresolved) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (node->worldRuntime0A30.motion.pitchAngle,
                   node->worldRuntime0A30.motion.headingAngle,
                   node->worldRuntime0A30.motion.committedDistanceQ12,averagePosition.worldZQ12,
                   averagePosition.worldYQ12,averagePosition.worldXQ12,&node->worldRuntime0A30);
      }
    }
  }
  return;
}


/* Address: 0x00560830.
   Handler of INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE, sent when the technology page of a selected building closes:
   modelOffset is the building's record as an offset from g_ModelRuntimeRebaseDelta. While the record is live the
   player's assignmentToken80A0 is cleared, then a positive technologyIndexOrRestore starts that research
   (Technology_ApplyRecordToEntity, InGameTechnologyResearch_StartSelected), a negative one (cancel,
   InGameCommandAction_ClearSelectedArmyTokenAndClosePage) gives back the flag 0x80 that
   FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80 took away when the page opened, and 0 does neither.
*/
void FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,
          TechnologyIndexOrRestoreCode technologyIndexOrRestore,ArmyRuntimeSavedOffset modelOffset)

{
  GameEntityRuntimeFlags *entityFlags;
  SelectionPlayerRuntimeBlock *playerBlock;
  GameEntityRuntime *entity;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (entity = (GameEntityRuntime *)(modelOffset + g_ModelRuntimeRebaseDelta),
     entity->common.ownership.modelNode != NULL)) {
    playerBlock->assignmentToken80A0 = 0;
    if (technologyIndexOrRestore != 0) {
      if ((int)technologyIndexOrRestore < 0) {
        entityFlags = &entity->common.runtimeFlags;
        *entityFlags = *entityFlags | playerBlock->assignmentFlags80A4;
      }
      else {
        Technology_ApplyRecordToEntity(technologyIndexOrRestore,entity);
      }
    }
  }
  return;
}


/* Address: 0x005608A0.
   Handler of INGAME_COMMAND_CHAT_SET_RECIPIENTS, the first command of an in-game chat line
   (InGameUiAction1024_Handler): stores the recipient mask (bits 8+faction and 16+player, 0xFFFFFF00 for all)
   in the player's packedSelectionState809C and resets the staging write offset in its low byte to 0.
*/
void FrontendPlayerTextCommand_SetPackedState(FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,
          FrontendPackedTextCommandState packedState)

{
  g_SelectionPlayerRuntimeBlockPointers[playerIndex]->packedSelectionState809C = packedState;
  return;
}


/* Address: 0x005608D0.
   Handler of INGAME_COMMAND_CHAT_APPEND: writes 12 more bytes of the player's chat line (value0 first) into the
   staging text at +0x80C0, at the write offset kept in the low byte of packedSelectionState809C, and advances
   the offset. The offset stops at 0x24, the last of the four 12-byte pieces of the 0x30-byte line, so extra
   pieces overwrite it instead of running past the buffer.
*/
void FrontendPlayerTextCommand_AppendTripleClamped(FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
          FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint32_t writeOffset;
  uint32_t nextOffset;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  writeOffset = playerBlock->packedSelectionState809C & 0xff;
  *(FrontendTextCommandValue0 *)(playerBlock->chatStagingText80C0 + writeOffset) = value0;
  *(FrontendTextCommandValue1 *)(playerBlock->chatStagingText80C0 + writeOffset + 4) = value1;
  nextOffset = writeOffset + PLAYER_CHAT_PIECE_BYTES;
  playerBlock->packedSelectionState809C = playerBlock->packedSelectionState809C & 0xffffff00;
  if (PLAYER_CHAT_LAST_PIECE_OFFSET < nextOffset) {
    nextOffset = PLAYER_CHAT_LAST_PIECE_OFFSET;
  }
  *(FrontendTextCommandValue2 *)(playerBlock->chatStagingText80C0 + writeOffset + 8) = value2;
  playerBlock->packedSelectionState809C = playerBlock->packedSelectionState809C | nextOffset;
  return;
}


/* Address: 0x00560940.
   Handler of INGAME_COMMAND_CHAT_PUBLISH, the last command of an in-game chat line: in a network session, when
   the sender's recipient mask includes the local faction (bit 8 + faction) or the local player (bit 16 +
   player), shows "<sender>: <text>" (TEXT_ID_CHAT_MESSAGE) from the staged text in the in-game message
   history. The sender's name is the one kept at +0x80F0 of its block.
*/
void FrontendPlayerTextCommand_PublishConditionalRichText
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint16_t *stream;
  TextResolveResult messageText;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((((playerBlock->packedSelectionState809C &
        1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex + 8U & 0x1f))
        != 0) ||
      ((playerBlock->packedSelectionState809C &
       1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId + 0x10U &
            0x1f)) != 0)) &&
     ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL)) {
    /* +0x80C0: the 0x30 staged bytes, widened into a 0x60-byte buffer */
    Text_CopyNarrowToUtf16
              (0x60,(uint16_t *)&g_FrontendPlayerMessageScratchUtf16,playerBlock->chatStagingText80C0);
    messageText = TextResource_Resolve(TEXT_ID_CHAT_MESSAGE);
    stream = messageText.text;
    RichTextCommandStream_PatchPayloadBySelector(0,playerBlock->playerNameUtf16_80F0,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendPlayerMessageScratchUtf16,stream);
    InGameRecentTextHistory_InsertAndRebuild8(stream);
  }
  return;
}


/* Address: 0x00561F10.
   Handler of INGAME_COMMAND_DESTROY_ARMIES (army placement sub-mode 1, clicking an army on the map): an army
   outside the player's selection is destroyed alone; clicking one of the selected armies destroys every army
   of the 32-entry selection (ArmyRuntime_DestroyInstanceAndRefreshUi). The army is sent as a saved offset from
   g_ArmyRuntimeRebaseBaseMinusOne.
*/
void FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken armyRuntimeOffset)

{
  GameEntityRuntime *targetEntity;
  int remainingEntries;
  WorldRuntimeContext *worldRuntime;
  SelectionPlayerRuntimeBlock *selectionCursor;
  bool notInSelection;

  selectionCursor = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  targetEntity = (GameEntityRuntime *)(armyRuntimeOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  remainingEntries = 0x20;
  /* SelectionPointerArray_Contains sets CF (true) when the army is NOT in the selection */
  notInSelection = SelectionPointerArray_Contains(targetEntity,&selectionCursor->selection);
  if (notInSelection) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,targetEntity);
    return;
  }
  /* selectionCursor walks the 32 entries, one dword per step */
  do {
    targetEntity = selectionCursor->selection.entries[0];
    if (targetEntity != NULL) {
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,targetEntity);
    }
    selectionCursor = (SelectionPlayerRuntimeBlock *)(selectionCursor->selection.entries + 1);
    remainingEntries--;
  } while (remainingEntries != 0);
  return;
}


/* Address: 0x00544020.
   Handler of FRONTEND_COMMAND_PLAYER_READY: counts the player's report in its readyOrWaitState. When every
   player has reported, the host sends the command a second time; once the host's count reaches 2 (or at once
   in a local game), FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS is cleared, which ends Frontend_Init's wait loop.
   A client ends it when the host's (id 0) second report arrives. Same scheme as
   FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus.
*/
void FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount searchRemaining;
  FrontendPlayerRuntimeBlockCount readyScanRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;
  FrontendPlayerRuntimeRecord *readyScanBlock;
  
  searchRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if (playerId == playerBlock->playerRuntimeId) {
        playerBlock->factionAssignment.readyOrWaitState++;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerId != 0) {
            return;
          }
          if (playerBlock->factionAssignment.readyOrWaitState < 2) {
            return;
          }
          /* Client: the host (id 0) reached consensus. */
          if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0) {
            g_FrontendNetworkTickCounter = 0;
            g_FrontendRuntimeFlags &= ~FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS;
          }
          return;
        }
        break;
      }
      searchRemaining--;
      readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      readyScanBlock = g_FrontendPlayerRuntimeBlocks;
      playerBlock++;
    } while (searchRemaining != 0);
    /* host: wait until every player has reported */
    do {
      if (readyScanBlock->factionAssignment.readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining--;
      readyScanBlock++;
    } while (readyScanRemaining != 0);
    if (g_FrontendPlayerRuntimeBlocks->factionAssignment.readyOrWaitState < 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_PLAYER_READY,0,0,0);
        return;
      }
      FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0) {
    g_FrontendNetworkTickCounter = 0;
    g_FrontendRuntimeFlags &= ~FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS;
  }
  return;
}


/* Address: 0x00544770.
   Handler of FRONTEND_COMMAND_SET_CONSENSUS_VALUE (FrontendPlayerConsensus_SubmitSelectedValue): stores the
   player's "Finish" check box state on the faction setup page. When any player has it unchecked the "Next"
   button is switched off; when all have it checked the host's "Next" button is switched on. Then the page's
   faction and player controls are refreshed.
*/
void FrontendPlayerRuntime_SetConsensusValueAndRefresh
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendConsensusValue consensusValue)

{
  UiNodeFlags *nextButtonFlags;
  UiRootNode *taskAssignmentRoot;
  uint32_t combinedConsensus;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->factionAssignment.consensusValue = consensusValue;
      taskAssignmentRoot = (UiRootNode *)g_FrontendRootNode;
      combinedConsensus = 0xffffffff;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      playerBlock = g_FrontendPlayerRuntimeBlocks;
      do {
        combinedConsensus = combinedConsensus & playerBlock->factionAssignment.consensusValue;
        playerBlock++;
        remainingBlocks--;
      } while (remainingBlocks != 0);
      if (combinedConsensus == 0) {
        nextButtonFlags = &FRONTEND_UI(g_FrontendRootNode,factionSetupNextButton)->nodeFlags;
        *nextButtonFlags = *nextButtonFlags | UI_NODE_SUPPRESSED;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL
              ) {
        nextButtonFlags = &FRONTEND_UI(g_FrontendRootNode,factionSetupNextButton)->nodeFlags;
        *nextButtonFlags = *nextButtonFlags & ~UI_NODE_SUPPRESSED;
      }
      FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(taskAssignmentRoot);
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00545490.
   Handler of FRONTEND_COMMAND_CHAT_BEGIN, the first command of a lobby chat line
   (FrontendPlayerMessage_SubmitSevenSlotText): rewinds the sender's message record so the following
   FRONTEND_COMMAND_CHAT_APPEND pieces fill its text from the start. In the lobby states (hosting, joined) the
   players are counted with g_FrontendPlayerRuntimeCount, otherwise with g_FrontendPlayerRuntimeBlockCount.
*/
void FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t *messageBuffer;

  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = (uint32_t *)(uintptr_t)g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == FRONTEND_NETWORK_STATE_JOINED) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount,
     g_FrontendNetworkState == FRONTEND_NETWORK_STATE_HOSTING)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  for (;;) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks--;
    playerBlock++;
    messageBuffer = messageBuffer + FRONTEND_PLAYER_MESSAGE_RECORD_BYTES / 4;
  }
  *messageBuffer = FRONTEND_PLAYER_MESSAGE_TEXT_OFFSET;
  return;
}


/* Address: 0x00545500.
   Handler of FRONTEND_COMMAND_CHAT_APPEND: appends 12 bytes of a lobby chat line (valueC first) to the
   sender's message record and advances its write offset. Unlike the in-game FrontendPlayerTextCommand_
   AppendTripleClamped the offset is not clamped: a ninth piece would run past the 100-byte record.
*/
void FrontendPlayerMessageBuffer_AppendTripleById
          (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
          FrontendMessageValueC valueC)

{
  int writeOffset;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  int *messageBuffer;
  
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = (int *)g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == FRONTEND_NETWORK_STATE_JOINED) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount,
     g_FrontendNetworkState == FRONTEND_NETWORK_STATE_HOSTING)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  for (;;) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks--;
    playerBlock++;
    messageBuffer = messageBuffer + FRONTEND_PLAYER_MESSAGE_RECORD_BYTES / 4;
  }
  writeOffset = *messageBuffer;
  *messageBuffer = *messageBuffer + PLAYER_CHAT_PIECE_BYTES;
  *(FrontendMessageValueC *)((uint8_t *)messageBuffer + writeOffset) = valueC;
  *(FrontendMessageValueB *)((uint8_t *)messageBuffer + writeOffset + 4) = valueB;
  *(FrontendMessageValueA *)((uint8_t *)messageBuffer + writeOffset + 8) = valueA;
  return;
}


/* Address: 0x00545590.
   Handler of FRONTEND_COMMAND_CHAT_PUBLISH, the last command of a lobby chat line: widens the sender's collected
   text to UTF-16 and shows "<sender>: <text>" (TEXT_ID_CHAT_MESSAGE, the sender's player name) in the lobby's
   message history.
*/
void FrontendPlayerMessageBuffer_PublishTextById
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  int messageBuffer;
  uint16_t *stream;
  TextResolveResult messageText;
  
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == FRONTEND_NETWORK_STATE_JOINED) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount,
     g_FrontendNetworkState == FRONTEND_NETWORK_STATE_HOSTING)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  for (;;) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks--;
    playerBlock++;
    messageBuffer = messageBuffer + FRONTEND_PLAYER_MESSAGE_RECORD_BYTES;
  }
  /* the 0x30 text bytes, widened into a 0x60-byte buffer */
  Text_CopyNarrowToUtf16(0x60,(uint16_t *)&g_FrontendPlayerMessageScratchUtf16,
                         (uint8_t *)(messageBuffer + FRONTEND_PLAYER_MESSAGE_TEXT_OFFSET));
  messageText = TextResource_Resolve(TEXT_ID_CHAT_MESSAGE);
  stream = messageText.text;
  RichTextCommandStream_PatchPayloadBySelector(0,&playerBlock->playerName,stream);
  RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendPlayerMessageScratchUtf16,stream);
  FrontendRecentTextHistory_InsertAndRebuild5(stream);
  return;
}


/* Address: 0x0054EBD0.
   Host lobby (FRONTEND_NETWORK_STATE_HOSTING), from the frontend root's tick
   (FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState) and from the kick button
   (FrontendPlayerSetup_ExpireSelectedRuntimeBlock): counts down the heartbeat expiry of every joined
   player (rows 1..n-1; row 0 is the host) and drops those that reached 0 by compacting the 0x13B0-byte player
   blocks, keeping the list selection on the same player (or the host row when the selected one left). Then the
   player count text is rewritten and the list refreshed.
*/
void FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(FrontendNetworkListsRuntimeView *frontendRoot)

{
  FrontendHeartbeatTickCount *heartbeatTicks;
  UiListRowCount *rowCountField;
  void ***selectedSlotField;
  void **selectedSlot;
  UiListRowCount initialRowCount;
  void **rowSlotCursor;
  int copyRemaining;
  int blocksRemaining;
  FrontendPlayerRuntimeRecord *sourceBlock;
  FrontendPlayerRuntimeRecord *nextSourceBlock;
  FrontendPlayerRuntimeRecord *destBlock;
  FrontendPlayerRuntimeRecord *nextDestBlock;
  
  initialRowCount = frontendRoot->playerRuntimeList.rowCount;
  blocksRemaining = initialRowCount - 1;
  if (blocksRemaining != 0 && 0 < (int)initialRowCount) {
    rowSlotCursor = frontendRoot->playerRuntimeList.rowSlots + 1;
    sourceBlock = g_FrontendPlayerRuntimeBlocks + 1;
    destBlock = g_FrontendPlayerRuntimeBlocks + 1;
    do {
      heartbeatTicks = &sourceBlock->heartbeatExpiryTicks;
      *heartbeatTicks = *heartbeatTicks - 1;
      nextDestBlock = destBlock;
      if (*heartbeatTicks == 0) {
        nextSourceBlock = sourceBlock + 1;
        rowCountField = &frontendRoot->playerRuntimeList.rowCount;
        *rowCountField = *rowCountField - 1;
        g_FrontendPlayerRuntimeCount--;
        selectedSlot = frontendRoot->playerRuntimeList.selectedRowSlot;
        if (rowSlotCursor == selectedSlot) {
          frontendRoot->playerRuntimeList.selectedRowSlot =
               frontendRoot->playerRuntimeList.rowSlots;
        }
        else if (rowSlotCursor <= selectedSlot) {
          selectedSlotField = &frontendRoot->playerRuntimeList.selectedRowSlot;
          *selectedSlotField = *selectedSlotField - 1;
        }
      }
      else {
        nextSourceBlock = sourceBlock + 1;
        nextDestBlock = destBlock + 1;
        rowSlotCursor++;
        if (nextDestBlock != nextSourceBlock) {
          nextSourceBlock = sourceBlock;
          nextDestBlock = destBlock;
          /* REP MOVSD of one 0x13B0-byte player block */
          for (copyRemaining = sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t); copyRemaining != 0;
               copyRemaining--) {
            nextDestBlock->runtimeState00 = nextSourceBlock->runtimeState00;
            nextSourceBlock = (FrontendPlayerRuntimeRecord *)&nextSourceBlock->peerSequenceToken;
            nextDestBlock = (FrontendPlayerRuntimeRecord *)&nextDestBlock->peerSequenceToken;
          }
        }
      }
      blocksRemaining--;
      sourceBlock = nextSourceBlock;
      destBlock = nextDestBlock;
    } while (blocksRemaining != 0);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,frontendRoot->playerRuntimeList.rowCount,
               (uint16_t *)&g_FrontendNetworkRuntimeCountTextUtf16);
    UiPointerList_RefreshSelectionAndQueueAction(&frontendRoot->playerRuntimeList);
  }
  return;
}


/* Address: 0x0055FAD0.
   Command handler INGAME_COMMAND_SELECT_SINGLE_ARMY: replaces the player's selection with one army (saved offset;
   0 does nothing, an army without a model leaves the selection empty) and rebuilds the selection panels for the
   local player. The two middle command arguments are unused.
*/
void FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
          RuntimeToken armyRuntimeOffset)

{
  GameEntityRuntime *army;
  
  if (armyRuntimeOffset != 0) {
    army = (GameEntityRuntime *)(armyRuntimeOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    if (army->common.ownership.modelNode != NULL) {
      SelectionPointerArray_InsertUniqueAndRecenter
                (army,&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
      if (playerRuntimeId == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}


/* Address: 0x005607E0.
   Handler of INGAME_COMMAND_ASSIGN_ARMY_TOKEN: turns modelOffset (a definition record offset from
   g_ModelRuntimeRebaseDelta, sent when the technology page of a selected object opens) back into a pointer
   and, if the record is live (dword +4 non-zero), records it for the player in assignmentToken80A0. Bit 0x80
   of the record's flags at +0xEC is moved into assignmentFlags80A4 (and cleared on the record).
*/
void FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,ArmyRuntimeSavedOffset modelOffset)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint32_t armyFlags;
  ModelRuntimeSlot *army;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (army = (ModelRuntimeSlot *)(modelOffset + g_ModelRuntimeRebaseDelta),
      army->rootModelNodeOrSavedOffset.modelNode != NULL)) {
    armyFlags = army->classState.stateFlags;
    playerBlock->assignmentToken80A0 = (uint32_t)army;
    playerBlock->assignmentFlags80A4 = armyFlags & 0x80;
    army->classState.stateFlags = army->classState.stateFlags & ~0x80;
  }
  return;
}

