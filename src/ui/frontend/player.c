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
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[76]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[76] (0x204C). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById, FrontendPlayerMessageBuffer_AppendTripleById,
   FrontendPlayerMessageBuffer_PublishTextById.
   Cross-module calls: UiTextControl_UpdateNonEmptyValidity [ui/controls/text],
   RichTextCommandStream_CopyToNarrow [assets/text/richtext], FrontendCommandQueue_EnqueueLocalPlayerCommand
   [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl)

{
  int remainingPairs;
  uint16_t *textCursor;
  
  UiTextControl_UpdateNonEmptyValidity(textEditControl);
  if ((textEditControl->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    RichTextCommandStream_CopyToNarrow
              (0x30,g_UiSevenSlotCommandPayloadText.textBytes,textEditControl->textPrefix6C);
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById(g_LocalPlayerRuntimeId,0,0,0xffffff00);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1540,0,0,0xffffff00);
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
                (0x15b0,g_UiSevenSlotCommandPayloadText.triples[0].payloadDword0C,
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
                (0x15b0,g_UiSevenSlotCommandPayloadText.triples[1].payloadDword0C,
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
                (0x15b0,g_UiSevenSlotCommandPayloadText.triples[2].payloadDword0C,
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
                (0x15b0,g_UiSevenSlotCommandPayloadText.triples[3].payloadDword0C,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword08,
                 g_UiSevenSlotCommandPayloadText.triples[3].payloadDword04);
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerMessageBuffer_PublishTextById(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1640,0,0,0);
    }
    textEditControl->cursorIndex = 0;
    textEditControl->selectionStart = 0;
    textEditControl->selectionEnd = 0;
    textCursor = textEditControl->textPrefix6C;
    for (remainingPairs = 0x18; remainingPairs != 0; remainingPairs = remainingPairs + -1) {
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
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken)

{
  InGameRuntimeRootImageC3E4 *inGameRoot;

  /* note the crossed bases: modelToken is an army-slot offset, armyToken one from g_ModelRuntimeRebaseDelta */
  if ((((modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne != 0) &&
       (armyToken + g_ModelRuntimeRebaseDelta != 0)) &&
      (*(int *)(modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne + 4) != 0)) &&
     (*(int *)(armyToken + g_ModelRuntimeRebaseDelta + 4) != 0)) {
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
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[66]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[66] (0x2042). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→FrontendConsensusSourceAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FrontendPlayerRuntime_SetConsensusValueAndRefresh.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source)

{
  uint32_t consensusValue;
  
  consensusValue = *(uint32_t *)(source + 0x4c) & 2;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_SetConsensusValueAndRefresh(g_LocalPlayerRuntimeId,0,0,consensusValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x820,0,0,consensusValue);
  }
  return;
}


/* Address: 0x0054D3A0.
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[11]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[11] (0x200B). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks.
*/

void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode)

{
  UiNodeBase *parentCursor;
  FrontendPlayerRuntimeRecord **selectedPlayerRuntimeSlot;
  
  parentCursor = (rootNode->base).parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    rootNode = *(UiRootNode **)
                (((FrontendNetworkListsRuntimeView5650 *)rootNode)->opaqueGap0000_4B67 + 8);
    parentCursor = *(UiNodeBase **)
              (((FrontendNetworkListsRuntimeView5650 *)rootNode)->opaqueGap0000_4B67 + 8);
  }
  selectedPlayerRuntimeSlot =
       (((FrontendNetworkListsRuntimeView5650 *)rootNode)->playerRuntimeList).selectedRowSlot;
  if (selectedPlayerRuntimeSlot !=
      (((FrontendNetworkListsRuntimeView5650 *)rootNode)->playerRuntimeList).rowSlots) {
    (*selectedPlayerRuntimeSlot)->heartbeatExpiryTicks = 1;
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks
              ((FrontendNetworkListsRuntimeView5650 *)rootNode);
  }
  return;
}


/* Address: 0x0054F540.
   Ownership: ui/frontend/player.
   Purpose: Compacts player blocks and paired frontend command slots after timeout, then sends
   g_FrontendPlayerRemovalPacket10007 for each removed player token.
   Local calls: FrontendPlayerRuntime_RecordReadyAndUpdateWaitState.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], FrontendRecentTextHistory_InsertAndRebuild5 [ui/frontend/runtime],
   UiTransfer_StagePacketAndSend [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void)

{
  FrontendHeartbeatTickCount *heartbeatTicks;
  FrontendPlayerRuntimeBlockCount sendRemaining;
  int copyRemaining;
  int scanRemaining;
  int removedCount;
  FrontendPlayerRuntimeRecord *sourceBlock;
  FrontendCommandPacketRecord *commandSource;
  FrontendPlayerRuntimeRecord *nextSourceBlock;
  UiTransferEndpointDescriptor *endpoint;
  FrontendPlayerRuntimeRecord *destBlock;
  FrontendPlayerRuntimeRecord *nextDestBlock;
  TextResolveResult removalText;
  FrontendCommandPacketRecord *removedTokenOrCommandDest;
  FrontendPlayerRemovalPacket10007 *commandDestOrPacket;
  FrontendCommandPacketRecord *commandCursor;
  
  removedCount = 0;
  commandCursor = g_FrontendPlayerCommandRecords;
  removedTokenOrCommandDest = g_FrontendPlayerCommandRecords;
  scanRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
  sourceBlock = g_FrontendPlayerRuntimeBlocks + 1;
  destBlock = g_FrontendPlayerRuntimeBlocks + 1;
  commandDestOrPacket = (FrontendPlayerRemovalPacket10007 *)removedTokenOrCommandDest;
  if (scanRemaining != 0 && 0 < (int)g_FrontendPlayerRuntimeBlockCount) {
    do {
      heartbeatTicks = &sourceBlock->heartbeatExpiryTicks;
      *heartbeatTicks = *heartbeatTicks - 1;
      if (*heartbeatTicks == 0) {
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        removalText = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&sourceBlock->playerName,removalText.text);
        FrontendRecentTextHistory_InsertAndRebuild5(removalText.text);
        removedTokenOrCommandDest = (FrontendCommandPacketRecord *)sourceBlock->playerRuntimeId;
        removedCount = removedCount + 1;
        nextSourceBlock = sourceBlock + 1;
        nextDestBlock = destBlock;
      }
      else {
        nextSourceBlock = sourceBlock + 1;
        nextDestBlock = destBlock + 1;
        removedTokenOrCommandDest = (FrontendCommandPacketRecord *)(commandDestOrPacket + 1);
        copyRemaining = 0x4ec;
        if (nextDestBlock != nextSourceBlock) {
          for (; nextDestBlock = destBlock, nextSourceBlock = sourceBlock, copyRemaining != 0; copyRemaining = copyRemaining + -1) {
            nextDestBlock->runtimeState00 = nextSourceBlock->runtimeState00;
            sourceBlock = (FrontendPlayerRuntimeRecord *)&nextSourceBlock->peerSequenceToken;
            destBlock = (FrontendPlayerRuntimeRecord *)&nextDestBlock->peerSequenceToken;
          }
          commandSource = commandCursor;
          for (copyRemaining = 8; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
            (commandDestOrPacket->header).packedTypeAndUnitCount = (commandSource->header).packedTypeAndUnitCount;
            commandSource = (FrontendCommandPacketRecord *)&(commandSource->header).sequenceToken;
            commandDestOrPacket = (FrontendPlayerRemovalPacket10007 *)&(commandDestOrPacket->header).sequenceToken;
          }
        }
      }
      commandCursor = commandCursor + 1;
      scanRemaining = scanRemaining + -1;
      sourceBlock = nextSourceBlock;
      destBlock = nextDestBlock;
      commandDestOrPacket = (FrontendPlayerRemovalPacket10007 *)removedTokenOrCommandDest;
    } while (scanRemaining != 0);
  }
  if (removedCount != 0) {
    do {
      endpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
      g_FrontendPlayerRemovalPacket10007.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_10007_PLAYER_REMOVAL;
      g_FrontendPlayerRemovalPacket10007.removedPlayerToken = (FrontendPlayerRuntimeId)removedTokenOrCommandDest;
      sendRemaining = g_FrontendPlayerRuntimeBlockCount;
      while (sendRemaining = sendRemaining - 1, sendRemaining != 0) {
        commandDestOrPacket = &g_FrontendPlayerRemovalPacket10007;
        UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPlayerRemovalPacket10007.header);
        endpoint = endpoint + 0x13b;
        removedTokenOrCommandDest = (FrontendCommandPacketRecord *)commandDestOrPacket;
      }
      removedCount = removedCount + -1;
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
bool __thandor_cf_preserve_eax_ecx_edx
FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
          (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  while ((playerBlock->playerRuntimeId == excludedPlayerId ||
         (g_SelectionPlayerRuntimeBlockPointers[playerBlock->playerRuntimeId]->assignmentToken80A0 !=
          assignmentToken))) {
    playerBlock = playerBlock + 1;
    remainingBlocks--;
    if (remainingBlocks == 0) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00514F60.
   Ownership: ui/frontend/player.
   Purpose: EAX and EDX remain preserved. Typed parameters: p0 assignmentToken→RuntimeToken. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken)

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
    playerBlockCursor = playerBlockCursor + 1;
    playerBlocksRemaining = playerBlocksRemaining - 1;
  } while (playerBlocksRemaining != 0);
  return;
}


/* Address: 0x00544130.
   Ownership: ui/frontend/player.
   Purpose: Four-argument frontend callback. In mode bit 1 it marks the matching player's +0x54 dword and clears
   UI_NODE_SUPPRESSED (0x08) on the briefingBeginButton when every player is ready; in mode bit 0 it suppresses
   that button for the local player. EAX is
   preserved. Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08
          (PlayerRuntimeId playerId,uint32_t argument2,uint32_t argument3,uint32_t argument4)

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
    FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags =
         FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags | UI_NODE_SUPPRESSED;
    return;
  }
  searchRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  hostFlagOrRemaining = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST;
  while( true ) {
    if (hostFlagOrRemaining == SESSION_NETWORK_ROLE_LOCAL) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    playerBlock = playerBlock + 1;
    searchRemaining = searchRemaining - SESSION_NETWORK_ROLE_CLIENT;
    hostFlagOrRemaining = searchRemaining;
  }
  (playerBlock->factionAssignment).readyOrWaitState = 1;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  do {
    remainingBlocks = remainingBlocks - 1;
    if (remainingBlocks == 0) {
      FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags =
           FRONTEND_UI(g_FrontendRootNode,briefingBeginButton)->nodeFlags & ~UI_NODE_SUPPRESSED;
      return;
    }
    nextBlock = playerBlock + 1;
    playerBlock = playerBlock + 1;
  } while ((nextBlock->factionAssignment).readyOrWaitState != 0);
  return;
}


/* Address: 0x005442B0.
   Sets FRONTEND_PLAYER_STATE_LEVEL_RECEIVED in the roleStateFlags of the player block with this player id (the level's field grid arrived).
   Command handler with four dword arguments (local command 0x360, run on every peer in a network session);
   only the player id is used.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag08ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      (playerBlock->factionAssignment).roleStateFlags |= FRONTEND_PLAYER_STATE_LEVEL_RECEIVED;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544300.
   Command handler FRONTEND_COMMAND_XOR_PLAYER_STATE (run on every peer): toggles the stateMask bits in
   runtimeState64 of the player block with this player id. The two middle command arguments are unused.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t stateMask)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      playerBlock->runtimeState64 = playerBlock->runtimeState64 ^ stateMask;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag04ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      (playerBlock->factionAssignment).roleStateFlags |= FRONTEND_PLAYER_STATE_LEVEL_LOADED;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag02ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      (playerBlock->factionAssignment).roleStateFlags |= FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT;
      return;
    }
    playerBlock++;
    remainingBlocks--;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544D50.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks with exact stride 0x13B0, compares playerId against dword
   +0x14, sets bit 0x01 at +0x60 on the first match, and stores the remaining callback values at +0x8C, +0x88, and
   +0x84. EAX is preserved. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag01AndStoreValuesById
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
      roleFlags = &(playerBlock->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | 1;
      playerBlock->scenarioAvailabilityMask0 = scenarioAvailabilityMask0;
      playerBlock->scenarioAvailabilityMask1 = scenarioAvailabilityMask1;
      playerBlock->scenarioAvailabilityMask2 = scenarioAvailabilityMask2;
      return;
    }
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00549190.
   Default faction line-up for a freshly loaded level (scenario catalogue, frontend main loop): marks factions
   1..active count as active and clears the slots above, then hands the players the assignable factions
   round-robin (player n gets faction (n mod assignable count) + 1) and clears their ready and consensus state.
   The original also computes the local player's zero-based faction in EDX but restores EDX before returning.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendPlayerRuntime_InitializeFactionAssignments(void)

{
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  uint32_t activeRemaining;
  uint32_t factionSlot;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  uint32_t assignableRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;

  loadedLevel = g_FrontendLoadedLevelAsset;
  factionSlot = 1; /* slot 0 is not a player faction */
  activeRemaining = (g_FrontendLoadedLevelAsset->runtimeTail2E0).activeFactionCount;
  assignableRemaining = (g_FrontendLoadedLevelAsset->runtimeTail2E0).assignableFactionCount;
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
    (playerBlock->factionAssignment).factionAssignmentIndex = factionSlot;
    (playerBlock->factionAssignment).readyOrWaitState = 0;
    (playerBlock->factionAssignment).consensusValue = 0;
    factionSlot++;
    playerBlock++;
    if ((loadedLevel->runtimeTail2E0).assignableFactionCount < factionSlot) {
      factionSlot = factionSlot - (loadedLevel->runtimeTail2E0).assignableFactionCount;
    }
    remainingBlocks--;
  } while (remainingBlocks != 0);
}


/* Address: 0x0054D000.
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[5]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[5] (0x2005). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void __thandor_preserve_eax FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *firstPlayerBlock;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  uint32_t sessionTickInterval;
  /* source is the frontend template's hostLobbyBackButton (+0x543C). */
  FrontendUiImage *frontendUi;

  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(source,-0x543c);
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  sessionTickInterval = g_SessionNetworkTickInterval;
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) | 0x2000;
  }
  g_FrontendNetworkState = 0;
  ((UiRangeSliderControl *)FRONTEND_UI(frontendUi,networkSpeedSlider))->value = sessionTickInterval >> 1;
  firstPlayerBlock = g_FrontendPlayerRuntimeBlocks;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  (localPlayerRecord->playerName).textUtf16[0] = 0;
  (localPlayerRecord->playerName).textUtf16[1] = 0;
  firstPlayerBlock->playerRuntimeId = 0;
  (firstPlayerBlock->factionAssignment).roleStateFlags = 0;
  firstPlayerBlock->runtimeState64 = 0;
  firstPlayerBlock->snapshotTransferFlags = 0;
  return;
}


/* Address: 0x0054D1B0.
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[77]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[77] (0x204D). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_CopyExpanded
   [assets/text/richtext].
*/
void __thandor_preserve_eax FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source)

{
  TextResolveResult labelText;
  
  g_SessionNetworkTickInterval = ((UiRangeSliderControl *)source)->value;
  labelText = TextResource_Resolve
                    ((TextResourceId)((int)&((UiNodeVtable *)(uintptr_t)g_SessionNetworkTickInterval)[0x75].rightDrag + 1 /* TODO: Ghidra read a constant as an address */));
  RichTextCommandStream_CopyExpanded
            (0x40,(uint16_t *)&g_FrontendNetworkPlayerCountLabelUtf16,labelText.text);
  g_SessionNetworkTickInterval = (UiNodeVtable *)((int)g_SessionNetworkTickInterval << 1);
  return;
}


/* Address: 0x0054D720.
   Host lobby: shows the start button (FRONTEND_ACTION_START_NETWORK_GAME) only while at least a third of the
   players report FRONTEND_CAPABILITY_CD, i.e. run the game from the CD; otherwise hides it. Called whenever a
   player joins or a heartbeat updates the capabilities.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction(void)

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
    playerBlock = playerBlock + 1;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_SetReadyFlagById
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(PlayerRuntimeId playerRuntimeId)

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
          (playerBlock->factionAssignment).readyOrWaitState = 1;
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
      } while ((playerBlock->factionAssignment).readyOrWaitState != 0);
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
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
        (playerBlock->factionAssignment).readyOrWaitState++;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerRuntimeId != 0) {
            return;
          }
          if ((playerBlock->factionAssignment).readyOrWaitState < 2) {
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
      if ((readyScanBlock->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining--;
      readyScanBlock++;
    } while (readyScanRemaining != 0);
    if ((g_FrontendPlayerRuntimeBlocks->factionAssignment).readyOrWaitState < 2) {
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
void __thandor_preserve_eax
FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0)

{
  if ((armyRuntimeOffset0 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != NULL)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset1 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != NULL)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset2 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != NULL)) {
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
void __thandor_preserve_eax
FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerRuntimeId,RuntimeToken armyRuntimeOffset2,
          RuntimeToken armyRuntimeOffset1,RuntimeToken armyRuntimeOffset0)

{
  if ((armyRuntimeOffset0 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common)
      .ownership.modelNode != NULL)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset1 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common)
      .ownership.modelNode != NULL)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset2 != 0) &&
     (armyRuntimeOffset2 = armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne,
     (((GameEntityRuntime *)armyRuntimeOffset2)->common).ownership.modelNode != NULL)) {
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
void __thandor_preserve_eax
FrontendPlayerSelection_ClearAndRefreshLocalPanels
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
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
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
      selectionEntry = (destCursor->selection).entries[0];
      if (selectionEntry != NULL) {
        memberOrEntryRemaining = SELECTION_GROUP_COUNT * SELECTION_GROUP_ENTRY_COUNT;
        groupMemberSlot = g_GameFactionRuntimeImage.records[factionIndex].runtimeGroupMembers8x32;
        do {
          if (selectionEntry == (GameEntityRuntime *)*groupMemberSlot) {
            *groupMemberSlot = NULL;
          }
          groupMemberSlot = groupMemberSlot + 1;
          memberOrEntryRemaining--;
        } while (memberOrEntryRemaining != 0);
      }
      entriesRemaining--;
      sourceCursor = (SelectionPlayerRuntimeBlock *)((destCursor->selection).entries + 1);
    } while (entriesRemaining != 0);
    /* back to the first selection entry (the typed expression is Ghidra's): selection -> group */
    sourceCursor = (SelectionPlayerRuntimeBlock *)&destCursor[-1].packedSelectionState809C;
    destCursor = groupOrScanCursor;
  }
  entriesRemaining = SELECTION_GROUP_ENTRY_COUNT;
  if ((transferModeFlags & SELECTION_TRANSFER_MERGE) == 0) {
    for (; entriesRemaining != 0; entriesRemaining--) {
      (destCursor->selection).entries[0] = (sourceCursor->selection).entries[0];
      sourceCursor = (SelectionPlayerRuntimeBlock *)((sourceCursor->selection).entries + 1);
      destCursor = (SelectionPlayerRuntimeBlock *)((destCursor->selection).entries + 1);
    }
  }
  else {
    memberOrEntryRemaining = SELECTION_GROUP_ENTRY_COUNT;
    do {
      selectionEntry = (sourceCursor->selection).entries[0];
      scanRemaining = entriesRemaining;
      groupOrScanCursor = destCursor;
      if (selectionEntry != NULL) {
        do {
          if (selectionEntry == (groupOrScanCursor->selection).entries[0]) break;
          scanRemaining--;
          groupOrScanCursor = (SelectionPlayerRuntimeBlock *)((groupOrScanCursor->selection).entries + 1);
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
            scanRemaining = scanRemaining + -1;
            nextScanCursor = (SelectionPlayerRuntimeBlock *)((groupOrScanCursor->selection).entries + 1);
            foundEmpty = (groupOrScanCursor->selection).entries[0] == NULL;
            groupOrScanCursor = nextScanCursor;
          } while (!foundEmpty);
          if (foundEmpty) {
            /* = nextScanCursor's previous entry, i.e. the empty slot found */
            *(GameEntityRuntime **)(nextScanCursor[-1].reserved80B0_8117 + 100) =
                 (sourceCursor->selection).entries[0];
          }
        }
      }
      sourceCursor = (SelectionPlayerRuntimeBlock *)((sourceCursor->selection).entries + 1);
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
                  ((node->worldRuntime0A30).motion.pitchAngle,
                   (node->worldRuntime0A30).motion.headingAngle,
                   (node->worldRuntime0A30).motion.committedDistanceQ12,averagePosition.worldZQ12,
                   averagePosition.worldYQ12,averagePosition.worldXQ12,&node->worldRuntime0A30);
      }
    }
  }
  return;
}


/* Address: 0x00560830.
   Ownership: ui/frontend/player.
   Purpose: Resolves an army token through g_ArmyRuntimeRebaseBaseMinusOne, clears the player army pointer, then
   restores captured army flag 0x80 or applies the requested technology operation. It is separate from
   FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset identifiers. Typed
   parameters: p3 modelOffset→ArmyRuntimeSavedOffset_V343. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged. Typed
   parameters: p2 technologyIndexOrRestore→TechnologyIndexOrRestoreCode_V344.
   Cross-module calls: Technology_ApplyRecordToEntity [gameplay/technology/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,
          TechnologyIndexOrRestoreCode technologyIndexOrRestore,ArmyRuntimeSavedOffset modelOffset)

{
  GameEntityRuntimeFlags *entityFlags;
  SelectionPlayerRuntimeBlock *playerBlock;
  GameEntityRuntime *entity;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (entity = (GameEntityRuntime *)(modelOffset + g_ModelRuntimeRebaseDelta),
     (entity->common).ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    playerBlock->assignmentToken80A0 = 0;
    if (technologyIndexOrRestore != 0) {
      if ((int)technologyIndexOrRestore < 0) {
        entityFlags = &(entity->common).runtimeFlags;
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
   Ownership: ui/frontend/player.
   Purpose: The exact four-argument callback cleanup and preserved EAX result remain intact.
*/
void __thandor_preserve_eax_edx
FrontendPlayerTextCommand_SetPackedState
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,
          FrontendPackedTextCommandState packedState)

{
  g_SelectionPlayerRuntimeBlockPointers[playerIndex]->packedSelectionState809C = packedState;
  return;
}


/* Address: 0x005608D0.
   Ownership: ui/frontend/player.
   Purpose: Uses the low byte of +0x809C as a byte offset into player +0x80C0, writes one three-dword command,
   advances by 0x0C, clamps the offset to 0x24, and preserves the high state bits. It is separate from
   FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset identifiers. Typed
   parameters: p1 value2→FrontendTextCommandValue2_V344, p2 value1→FrontendTextCommandValue1_V344, p3
   value0→FrontendTextCommandValue0_V344. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerTextCommand_AppendTripleClamped
          (FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
          FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint32_t writeOffset;
  uint32_t nextOffset;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  writeOffset = playerBlock->packedSelectionState809C & 0xff;
  *(FrontendTextCommandValue0 *)(playerBlock->reserved80B0_8117 + writeOffset + 0x10) = value0;
  *(FrontendTextCommandValue1 *)(playerBlock->reserved80B0_8117 + writeOffset + 0x14) = value1;
  nextOffset = writeOffset + 0xc;
  playerBlock->packedSelectionState809C = playerBlock->packedSelectionState809C & 0xffffff00;
  if (0x24 < nextOffset) {
    nextOffset = 0x24;
  }
  *(FrontendTextCommandValue2 *)(playerBlock->reserved80B0_8117 + writeOffset + 0x18) = value2;
  playerBlock->packedSelectionState809C = playerBlock->packedSelectionState809C | nextOffset;
  return;
}


/* Address: 0x00560940.
   Ownership: ui/frontend/player.
   Purpose: Tests two UI-selected packed-state bits and network mode, converts the staged narrow text at +0x80C0 to
   UTF-16, patches two rich-text selectors, and inserts the result into recent-text history. It is separate from
   FactionRuntimeIndex, PlayerRuntimeId, network-player identity, and PCK-backed asset identifiers.
   Cross-module calls: Text_CopyNarrowToUtf16 [core/text/string], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8
   [ui/ingame/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerTextCommand_PublishConditionalRichText
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
    Text_CopyNarrowToUtf16
              (0x60,(uint16_t *)&g_FrontendPlayerMessageScratchUtf16,playerBlock->reserved80B0_8117 + 0x10);
    messageText = TextResource_Resolve(0xff07);
    stream = messageText.text;
    RichTextCommandStream_PatchPayloadBySelector(0,playerBlock->reserved80B0_8117 + 0x40,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendPlayerMessageScratchUtf16,stream);
    InGameRecentTextHistory_InsertAndRebuild8(stream);
  }
  return;
}


/* Address: 0x00561F10.
   Ownership: ui/frontend/player.
   Purpose: Resolves one player selection entry and applies the callback to it, or applies the callback to every
   non-null entry when resolution fails. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers. Typed parameters: p3 selectionEntryToken→RuntimeToken. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_Contains [gameplay/selection/runtime],
   ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken selectionEntryToken)

{
  GameEntityRuntime *targetEntity;
  int remainingEntries;
  WorldRuntimeContext *worldRuntime;
  SelectionPlayerRuntimeBlock *array;
  bool isSelected;
  
  array = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  targetEntity = (GameEntityRuntime *)(selectionEntryToken + (int)g_ArmyRuntimeRebaseBaseMinusOne);
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  remainingEntries = 0x20;
  isSelected = SelectionPointerArray_Contains(targetEntity,&array->selection);
  if (isSelected) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,targetEntity);
    return;
  }
  do {
    targetEntity = (array->selection).entries[0];
    if (targetEntity != (GameEntityRuntime *)0x0) {
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,targetEntity);
    }
    array = (SelectionPlayerRuntimeBlock *)((array->selection).entries + 1);
    remainingEntries = remainingEntries + -1;
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
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
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
        (playerBlock->factionAssignment).readyOrWaitState++;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerId != 0) {
            return;
          }
          if ((playerBlock->factionAssignment).readyOrWaitState < 2) {
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
      if ((readyScanBlock->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining--;
      readyScanBlock++;
    } while (readyScanRemaining != 0);
    if ((g_FrontendPlayerRuntimeBlocks->factionAssignment).readyOrWaitState < 2) {
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
   Ownership: ui/frontend/player.
   Purpose: Four-argument frontend callback. EAX is preserved. Kept distinct from frontend slot indices, faction
   runtime indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p0
   playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
   Cross-module calls: FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls [ui/frontend/settings].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_SetConsensusValueAndRefresh
          (PlayerRuntimeId playerId,uint32_t argument2,uint32_t argument3,
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
      (playerBlock->factionAssignment).consensusValue = consensusValue;
      taskAssignmentRoot = g_FrontendRootNode;
      combinedConsensus = 0xffffffff;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      playerBlock = g_FrontendPlayerRuntimeBlocks;
      do {
        combinedConsensus = combinedConsensus & (playerBlock->factionAssignment).consensusValue;
        playerBlock = playerBlock + 1;
        remainingBlocks = remainingBlocks - 1;
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
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00545490.
   Ownership: ui/frontend/player.
   Purpose: Scans the active frontend player blocks at exact 0x13B0 stride and resets the matching 0x64-byte
   message record write offset to four. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t *messageBuffer;

  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = (uint32_t *)(uintptr_t)g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks = remainingBlocks - 1;
    playerBlock = playerBlock + 1;
    messageBuffer = messageBuffer + 0x19;
  }
  *messageBuffer = 4;
  return;
}


/* Address: 0x00545500.
   Ownership: ui/frontend/player.
   Purpose: Scans the active frontend player blocks at exact 0x13B0 stride, advances the matching message record
   write offset by 0x0C, and appends three dwords. Kept distinct from frontend slot indices, faction runtime
   indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters:
   p1 valueA→FrontendMessageValueA_V344, p2 valueB→FrontendMessageValueB_V344, p3
   valueC→FrontendMessageValueC_V344.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerMessageBuffer_AppendTripleById
          (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
          FrontendMessageValueC valueC)

{
  int writeOffset;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  int *messageBuffer;
  
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks = remainingBlocks - 1;
    playerBlock = playerBlock + 1;
    messageBuffer = messageBuffer + 0x19;
  }
  writeOffset = *messageBuffer;
  *messageBuffer = *messageBuffer + 0xc;
  *(FrontendMessageValueC *)(writeOffset + (int)messageBuffer) = valueC;
  *(FrontendMessageValueB *)(writeOffset + 4 + (int)messageBuffer) = valueB;
  *(FrontendMessageValueA *)(writeOffset + 8 + (int)messageBuffer) = valueA;
  return;
}


/* Address: 0x00545590.
   Ownership: ui/frontend/player.
   Purpose: Finds the player message record, converts up to 0x60 narrow bytes from record +0x04 to UTF-16, patches
   player name and text into resource 0xFF07, and inserts the result into recent history. Kept distinct from
   frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset identifiers. Typed
   parameters: p0 playerId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable
   data remain unchanged.
   Cross-module calls: Text_CopyNarrowToUtf16 [core/text/string], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], FrontendRecentTextHistory_InsertAndRebuild5
   [ui/frontend/runtime].
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerMessageBuffer_PublishTextById
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  int messageBuffer;
  uint16_t *stream;
  TextResolveResult messageText;
  
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  messageBuffer = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (remainingBlocks = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    remainingBlocks = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (remainingBlocks == 0) {
      return;
    }
    if (playerId == playerBlock->playerRuntimeId) break;
    remainingBlocks = remainingBlocks - 1;
    playerBlock = playerBlock + 1;
    messageBuffer = messageBuffer + 100;
  }
  Text_CopyNarrowToUtf16(0x60,(uint16_t *)&g_FrontendPlayerMessageScratchUtf16,(uint8_t *)(messageBuffer + 4));
  messageText = TextResource_Resolve(0xff07);
  stream = messageText.text;
  RichTextCommandStream_PatchPayloadBySelector(0,&playerBlock->playerName,stream);
  RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendPlayerMessageScratchUtf16,stream);
  FrontendRecentTextHistory_InsertAndRebuild5(stream);
  return;
}


/* Address: 0x0054EBD0.
   Ownership: ui/frontend/player.
   Purpose: Decrements remote player expiry counters, compacts exact 0x13B0-byte player blocks, adjusts selection
   state, formats the new count, and refreshes the player list. Preserved EDX:EAX is incidental.
   Cross-module calls: UiPointerList_RefreshSelectionAndQueueAction [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks
          (FrontendNetworkListsRuntimeView5650 *frontendRoot)

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
  
  initialRowCount = (frontendRoot->playerRuntimeList).rowCount;
  blocksRemaining = initialRowCount - 1;
  if (blocksRemaining != 0 && 0 < (int)initialRowCount) {
    rowSlotCursor = (frontendRoot->playerRuntimeList).rowSlots + 1;
    sourceBlock = g_FrontendPlayerRuntimeBlocks + 1;
    destBlock = g_FrontendPlayerRuntimeBlocks + 1;
    do {
      heartbeatTicks = &sourceBlock->heartbeatExpiryTicks;
      *heartbeatTicks = *heartbeatTicks - 1;
      nextDestBlock = destBlock;
      if (*heartbeatTicks == 0) {
        nextSourceBlock = sourceBlock + 1;
        rowCountField = &(frontendRoot->playerRuntimeList).rowCount;
        *rowCountField = *rowCountField - 1;
        g_FrontendPlayerRuntimeCount = g_FrontendPlayerRuntimeCount + -1;
        selectedSlot = (frontendRoot->playerRuntimeList).selectedRowSlot;
        if (rowSlotCursor == selectedSlot) {
          (frontendRoot->playerRuntimeList).selectedRowSlot =
               (frontendRoot->playerRuntimeList).rowSlots;
        }
        else if (rowSlotCursor <= selectedSlot) {
          selectedSlotField = &(frontendRoot->playerRuntimeList).selectedRowSlot;
          *selectedSlotField = *selectedSlotField + -1;
        }
      }
      else {
        nextSourceBlock = sourceBlock + 1;
        nextDestBlock = destBlock + 1;
        rowSlotCursor = rowSlotCursor + 1;
        if (nextDestBlock != nextSourceBlock) {
          nextSourceBlock = sourceBlock;
          nextDestBlock = destBlock;
          for (copyRemaining = 0x4ec; copyRemaining != 0; copyRemaining = copyRemaining + -1) {
            nextDestBlock->runtimeState00 = nextSourceBlock->runtimeState00;
            nextSourceBlock = (FrontendPlayerRuntimeRecord *)&nextSourceBlock->peerSequenceToken;
            nextDestBlock = (FrontendPlayerRuntimeRecord *)&nextDestBlock->peerSequenceToken;
          }
        }
      }
      blocksRemaining = blocksRemaining + -1;
      sourceBlock = nextSourceBlock;
      destBlock = nextDestBlock;
    } while (blocksRemaining != 0);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(frontendRoot->playerRuntimeList).rowCount,
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
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
          RuntimeToken armyRuntimeOffset)

{
  GameEntityRuntime *army;
  
  if (armyRuntimeOffset != 0) {
    army = (GameEntityRuntime *)(armyRuntimeOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    if ((army->common).ownership.modelNode != NULL) {
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
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,ArmyRuntimeSavedOffset modelOffset)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint32_t armyFlags;
  uint32_t armyAddress;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (armyAddress = modelOffset + g_ModelRuntimeRebaseDelta, *(int *)(armyAddress + 4) != 0)) {
    armyFlags = *(uint32_t *)(armyAddress + 0xec);
    playerBlock->assignmentToken80A0 = armyAddress;
    playerBlock->assignmentFlags80A4 = armyFlags & 0x80;
    *(uint32_t *)(armyAddress + 0xec) = *(uint32_t *)(armyAddress + 0xec) & ~0x80;
  }
  return;
}

