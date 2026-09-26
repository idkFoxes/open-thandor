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
   RichTextCommandStream_CopyToNarrowCf [assets/text/richtext], FrontendCommandQueue_EnqueueLocalPlayerCommand
   [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl)

{
  int remainingPairs;
  word *textCursor;
  
  UiTextControl_UpdateNonEmptyValidity(textEditControl);
  if ((textEditControl->editStateFlags & UI_TEXT_EDIT_VALUE_VALID) != 0) {
    RichTextCommandStream_CopyToNarrowCf
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
   Ownership: ui/frontend/player.
   Purpose: Validates a model token and an army token, assigns both to a player, and switches and resets the local
   technology panel for the local player. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers. Typed parameters: p2 armyToken→RuntimeToken, p3
   modelToken→RuntimeToken. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection,
   FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout],
   InGameTechnologyPanel_ResetAndSelectCurrentArea [ui/ingame/technology].
*/
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,dword reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken)

{
  InGameRuntimeRootImageC3E4 *inGameRoot;
  
  if ((((modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne != 0) &&
       (armyToken + g_ModelRuntimeRebaseDelta != 0)) &&
      (*(int *)(modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne + 4) != 0)) &&
     (*(int *)(armyToken + g_ModelRuntimeRebaseDelta + 4) != 0)) {
    FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection(playerIndex,0,0,modelToken);
    inGameRoot = g_InGameRuntimeRoot;
    if (playerIndex == g_LocalPlayerRuntimeId) {
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
  uint consensusValue;
  
  consensusValue = *(uint *)(source + 0x4c) & 2;
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
   UiTransfer_StagePacketAndSendCf [network/protocol/transfer].
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
  TextResourceResolveEaxCf5 removalText;
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
        RichTextCommandStream_PatchPayloadBySelector(0,&sourceBlock->playerName,removalText.eax);
        FrontendRecentTextHistory_InsertAndRebuild5(removalText.eax);
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
        UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPlayerRemovalPacket10007.header);
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
   Ownership: ui/frontend/player.
   Purpose: CF set reports another matching player; CF clear reports absence. EAX is preserved. Typed parameters:
   p1 excludedPlayerId→PlayerRuntimeId. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf
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
    remainingBlocks = remainingBlocks - 1;
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
  dword playerBlocksRemaining;
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
   root flag 0x08 when every player is ready; in mode bit 0 it sets flag 0x08 for the local player. EAX is
   preserved. Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08
          (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4)

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
    *(uint *)(g_FrontendRootNode + 0x750) = *(uint *)(g_FrontendRootNode + 0x750) | 8;
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
      *(uint *)(g_FrontendRootNode + 0x750) = *(uint *)(g_FrontendRootNode + 0x750) & 0xfffffff7;
      return;
    }
    nextBlock = playerBlock + 1;
    playerBlock = playerBlock + 1;
  } while ((nextBlock->factionAssignment).readyOrWaitState != 0);
  return;
}


/* Address: 0x005442B0.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks using exact stride 0x13B0, compares playerId against block
   dword +0x14, and ORs bit 0x08 into block dword +0x60 on the first match. The callback consumes four dword
   arguments; only playerId is used. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag08ById
          (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *roleFlags;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      roleFlags = &(playerBlock->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | 8;
      return;
    }
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544300.
   Ownership: ui/frontend/player.
   Purpose: Handles frontend player runtime xor state mask by player id.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,dword unusedArg1,dword unusedArg2,dword stateMask)

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
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544360.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks using exact stride 0x13B0, compares playerId against block
   dword +0x14, and ORs bit 0x04 into block dword +0x60 on the first match. The callback consumes four dword
   arguments; only playerId is used. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag04ById
          (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *roleFlags;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      roleFlags = &(playerBlock->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | 4;
      return;
    }
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
}


/* Address: 0x00544820.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks using exact stride 0x13B0, compares playerId against block
   dword +0x14, and ORs bit 0x02 into block dword +0x60 on the first match. The callback consumes four dword
   arguments; only playerId is used. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag02ById
          (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *roleFlags;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == playerBlock->playerRuntimeId) {
      roleFlags = &(playerBlock->factionAssignment).roleStateFlags;
      *roleFlags = *roleFlags | 2;
      return;
    }
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
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
   Ownership: ui/frontend/player.
   Purpose: Builds faction availability bytes from the active configuration, initializes every 0x13B0-byte player
   block's faction and state fields, and leaves the local zero-based faction slot in EDX while preserving EAX.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendPlayerRuntime_InitializeFactionAssignments(void)

{
  FrontendLoadedLevelRuntimeImage370 *loadedLevel;
  dword activeRemaining;
  uint factionSlot;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  dword assignableRemaining;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  loadedLevel = g_FrontendLoadedLevelAsset;
  factionSlot = 1;
  activeRemaining = (g_FrontendLoadedLevelAsset->runtimeTail2E0).activeFactionCount;
  assignableRemaining = (g_FrontendLoadedLevelAsset->runtimeTail2E0).assignableFactionCount;
  do {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    activeRemaining = activeRemaining - 1;
    factionSlot = factionSlot + 1;
    assignableRemaining = assignableRemaining - 1;
  } while (assignableRemaining != 0);
  for (; activeRemaining != 0; activeRemaining = activeRemaining - 1) {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    factionSlot = factionSlot + 1;
  }
  if (factionSlot < 7) {
    do {
      g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionSlot] = 0;
      factionSlot = factionSlot + 1;
    } while (factionSlot < 8);
  }
  factionSlot = 1;
  remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    (playerBlock->factionAssignment).factionAssignmentIndex = factionSlot;
    (playerBlock->factionAssignment).readyOrWaitState = 0;
    (playerBlock->factionAssignment).consensusValue = 0;
    factionSlot = factionSlot + 1;
    playerBlock = playerBlock + 1;
    if ((loadedLevel->runtimeTail2E0).assignableFactionCount < factionSlot) {
      factionSlot = factionSlot - (loadedLevel->runtimeTail2E0).assignableFactionCount;
    }
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  return;
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
  uint sessionTickInterval;
  
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&source[-0x10b].left);
  sessionTickInterval = g_SessionNetworkTickInterval;
  if ((int)g_FramebufferWidth < 0x281) {
    source[-0x110].rightAnchorQ31 = source[-0x110].rightAnchorQ31 | 0x2000;
  }
  g_FrontendNetworkState = 0;
  source[-8].rightOffset = sessionTickInterval >> 1;
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
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_CopyExpandedCf
   [assets/text/richtext].
*/
void __thandor_preserve_eax FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source)

{
  TextResourceResolveEaxCf5 labelText;
  
  g_SessionNetworkTickInterval = source[1].vtable;
  labelText = TextResource_Resolve
                    ((TextResourceId)((int)&((UiNodeVtable *)(uintptr_t)g_SessionNetworkTickInterval)[0x75].rightDrag + 1 /* TODO: Ghidra read a constant as an address */));
  RichTextCommandStream_CopyExpandedCf
            (0x40,(word *)&g_FrontendNetworkPlayerCountLabelUtf16,labelText.eax);
  g_SessionNetworkTickInterval = (UiNodeVtable *)((int)g_SessionNetworkTickInterval << 1);
  return;
}


/* Address: 0x0054D720.
   Ownership: ui/frontend/player.
   Purpose: Counts active player blocks with flag 0x100 using the exact 0x13B0 stride. Action 0x2006 is suppressed
   when three times the flagged count is below the total count and restored otherwise.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction(void)

{
  int flaggedCount;
  uint remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  flaggedCount = 0;
  remainingBlocks = g_FrontendPlayerRuntimeCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((playerBlock->capabilityFlags & 0x100) != 0) {
      flaggedCount = flaggedCount + 1;
    }
    playerBlock = playerBlock + 1;
    remainingBlocks = remainingBlocks - 1;
  } while (remainingBlocks != 0);
  if ((uint)(flaggedCount * 3) < g_FrontendPlayerRuntimeCount) {
    UiNodeList_SuppressActionId(0x2006,g_FrontendRootNode);
  }
  else {
    UiNodeList_UnsuppressActionId(0x2006,g_FrontendRootNode);
  }
  return;
}


/* Address: 0x0055F470.
   Ownership: ui/frontend/player.
   Purpose: Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p0 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body
   bytes, control flow, and executable data remain unchanged. Typed parameters: p3
   readyFlagMask→FrontendReadyFlagMask_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_SetReadyFlagById
          (PlayerRuntimeId playerRuntimeId,dword reservedArg04,dword reservedArg08,
          FrontendReadyFlagMask readyFlagMask)

{
  byte *readyFlagsField;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  readyFlagsField = (byte *)&playerRuntimeBlock->sessionFlags;
  *(uint *)readyFlagsField = *(uint *)readyFlagsField & 0xfffffffd;
  playerRuntimeBlock->sessionFlags = playerRuntimeBlock->sessionFlags | readyFlagMask;
  return;
}


/* Address: 0x0055F5A0.
   Ownership: ui/frontend/player.
   Purpose: Marks the matching frontend player block ready at field +0x54 and updates action 0x101B according to
   local/network mode and remaining unready player blocks. Kept distinct from frontend slot indices, faction
   runtime indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p2
   playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
   Cross-module calls: UiNodeList_UnsuppressActionId [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists].
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
        searchRemaining = searchRemaining - 1;
        playerBlock = playerBlock + 1;
        readyScanBlock = g_FrontendPlayerRuntimeBlocks;
        readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      } while (searchRemaining != 0);
      do {
        readyScanRemaining = readyScanRemaining - 1;
        if (readyScanRemaining == 0) {
          UiNodeList_UnsuppressActionId(0x101b,(UiNodeBase *)g_InGameRuntimeRoot);
          return;
        }
        playerBlock = readyScanBlock + 1;
        readyScanBlock = readyScanBlock + 1;
      } while ((playerBlock->factionAssignment).readyOrWaitState != 0);
    }
  }
  else if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    UiNodeList_SuppressActionId(0x101b,(UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055F680.
   Ownership: ui/frontend/player.
   Purpose: Increments the matching player ready counter, resolves all-player readiness, queues command 0x550 when
   required, and clears runtime flags 0x01 and 0x10 once the consensus threshold is reached. Kept distinct from
   frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset identifiers. Typed
   parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes, control flow, and
   executable data remain unchanged.
   Cross-module calls: InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (PlayerRuntimeId playerRuntimeId,dword reserved0,dword reserved1,dword reserved2)

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
        (playerBlock->factionAssignment).readyOrWaitState =
             (playerBlock->factionAssignment).readyOrWaitState + 1;
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
          goto 
          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus_ClearUiCommandReadyWaitFlagsAndReturn
          ;
        }
        break;
      }
      searchRemaining = searchRemaining - 1;
      readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      readyScanBlock = g_FrontendPlayerRuntimeBlocks;
      playerBlock = playerBlock + 1;
    } while (searchRemaining != 0);
    do {
      if ((readyScanBlock->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining = readyScanRemaining - 1;
      readyScanBlock = readyScanBlock + 1;
    } while (readyScanRemaining != 0);
    if ((g_FrontendPlayerRuntimeBlocks->factionAssignment).readyOrWaitState < 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
        return;
      }
      FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
  }
FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus_ClearUiCommandReadyWaitFlagsAndReturn:
  if ((g_UiCommandRuntimeFlags & 0x10) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffffee;
  }
  return;
}


/* Address: 0x0055FB90.
   Ownership: ui/frontend/player.
   Purpose: Inserts up to three resolved entries into the selected player array and rebuilds the local selection
   panels when the affected player is local. Kept distinct from frontend slot indices, faction runtime indices,
   network endpoint identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters:
   p3 armyRuntimeOffset2→ArmyRuntimeSavedOffset_V343, p4 armyRuntimeOffset1→ArmyRuntimeSavedOffset_V343, p5
   armyRuntimeOffset0→ArmyRuntimeSavedOffset_V343.
   Cross-module calls: SelectionPointerArray_InsertUniqueAndRecenter [gameplay/selection/runtime],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology].
*/
void __thandor_preserve_eax
FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0)

{
  if ((armyRuntimeOffset0 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset1 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              ((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if ((armyRuntimeOffset2 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
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
   Ownership: ui/frontend/player.
   Purpose: Removes up to three resolved entries from the selected player array and rebuilds the local selection
   panels when the affected player is local. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-
   player identity, and PCK-backed asset identifiers. Typed parameters: p1 selectionEntryToken2→RuntimeToken, p2
   selectionEntryToken1→RuntimeToken, p3 selectionEntryToken0→RuntimeToken. Calling convention, parameter storage,
   body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_RemoveFirstMatch [gameplay/selection/runtime],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology].
*/
void __thandor_preserve_eax
FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerIndex,RuntimeToken selectionEntryToken2,
          RuntimeToken selectionEntryToken1,RuntimeToken selectionEntryToken0)

{
  if ((selectionEntryToken0 != 0) &&
     ((((GameEntityRuntime *)(selectionEntryToken0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common)
      .ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(selectionEntryToken0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
  }
  if ((selectionEntryToken1 != 0) &&
     ((((GameEntityRuntime *)(selectionEntryToken1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common)
      .ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)(selectionEntryToken1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
  }
  if ((selectionEntryToken2 != 0) &&
     (selectionEntryToken2 = selectionEntryToken2 + (int)g_ArmyRuntimeRebaseBaseMinusOne,
     (((GameEntityRuntime *)selectionEntryToken2)->common).ownership.modelNode !=
     (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_RemoveFirstMatch
              ((GameEntityRuntime *)selectionEntryToken2,
               &g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
  }
  SelectionPointerArray_RemoveFirstMatch
            ((GameEntityRuntime *)selectionEntryToken2,
             &g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
  if (playerIndex == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FCD0.
   Ownership: ui/frontend/player.
   Purpose: Clears the selected player's 32-entry selection array and rebuilds selection-detail and catalog panels
   when the affected player is local. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers.
   Cross-module calls: SelectionPointerArray_Clear32 [gameplay/selection/runtime],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology].
*/
void __thandor_preserve_eax
FrontendPlayerSelection_ClearAndRefreshLocalPanels
          (FrontendPlayerIndex playerIndex,dword callbackArg1,dword callbackArg2,dword callbackArg3)

{
  SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
  if (playerIndex == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return;
}


/* Address: 0x0055FD10.
   Ownership: ui/frontend/player.
   Purpose: Transfers or merges one 32-pointer faction selection group according to mode bits, recenters offsets,
   refreshes local selection panels, and optionally repositions the selected world object. Typed parameters: p5
   selectionGroupIndex→FrontendFactionAssignmentIndex_V306. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p4
   transferModeFlags→FrontendSelectionTransferModeFlags_V343.
   Cross-module calls: SelectionPointerArray_RecenterOffsetsAroundAveragePosition [gameplay/selection/runtime],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology],
   SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf [gameplay/selection/runtime],
   WorldRuntime_SetPosition80AndRebuildPosition60FromAngles [world/runtime/core].
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
  WorldPositionEaxEcxEdxCf13 averagePosition;
  
  groupOrScanCursor = (SelectionPlayerRuntimeBlock *)
           (selectionGroupIndex * 0x80 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x2e0) + factionIndex * 0x740);
  sourceCursor = groupOrScanCursor;
  destCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  if ((transferModeFlags & 1) != 0) {
    entriesRemaining = 0x20;
    sourceCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    do {
      destCursor = sourceCursor;
      selectionEntry = (destCursor->selection).entries[0];
      if (selectionEntry != (GameEntityRuntime *)0x0) {
        memberOrEntryRemaining = 0x100;
        groupMemberSlot = g_GameFactionRuntimeImage.records[factionIndex].runtimeGroupMembers8x32;
        do {
          if (selectionEntry == (GameEntityRuntime *)*groupMemberSlot) {
            *groupMemberSlot = (ArmyRuntimeSlot *)0x0;
          }
          groupMemberSlot = groupMemberSlot + 1;
          memberOrEntryRemaining = memberOrEntryRemaining + -1;
        } while (memberOrEntryRemaining != 0);
      }
      entriesRemaining = entriesRemaining + -1;
      sourceCursor = (SelectionPlayerRuntimeBlock *)((destCursor->selection).entries + 1);
    } while (entriesRemaining != 0);
    sourceCursor = (SelectionPlayerRuntimeBlock *)&destCursor[-1].packedSelectionState809C;
    destCursor = groupOrScanCursor;
  }
  entriesRemaining = 0x20;
  if ((transferModeFlags & 2) == 0) {
    for (; entriesRemaining != 0; entriesRemaining = entriesRemaining + -1) {
      (destCursor->selection).entries[0] = (sourceCursor->selection).entries[0];
      sourceCursor = (SelectionPlayerRuntimeBlock *)((sourceCursor->selection).entries + 1);
      destCursor = (SelectionPlayerRuntimeBlock *)((destCursor->selection).entries + 1);
    }
  }
  else {
    memberOrEntryRemaining = 0x20;
    do {
      selectionEntry = (sourceCursor->selection).entries[0];
      scanRemaining = entriesRemaining;
      groupOrScanCursor = destCursor;
      if (selectionEntry != (GameEntityRuntime *)0x0) {
        do {
          if (selectionEntry == (groupOrScanCursor->selection).entries[0])
          goto 
          FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh_AdvanceAfterDuplicateOrAppendDecision
          ;
          scanRemaining = scanRemaining + -1;
          groupOrScanCursor = (SelectionPlayerRuntimeBlock *)((groupOrScanCursor->selection).entries + 1);
        } while (scanRemaining != 0);
        foundEmpty = true;
        scanRemaining = entriesRemaining;
        groupOrScanCursor = destCursor;
        do {
          nextScanCursor = groupOrScanCursor;
          if (scanRemaining == 0) break;
          scanRemaining = scanRemaining + -1;
          nextScanCursor = (SelectionPlayerRuntimeBlock *)((groupOrScanCursor->selection).entries + 1);
          foundEmpty = (groupOrScanCursor->selection).entries[0] == (GameEntityRuntime *)0x0;
          groupOrScanCursor = nextScanCursor;
        } while (!foundEmpty);
        if (foundEmpty) {
          *(GameEntityRuntime **)(nextScanCursor[-1].reserved80B0_8117 + 100) =
               (sourceCursor->selection).entries[0];
        }
      }
FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh_AdvanceAfterDuplicateOrAppendDecision
      :
      sourceCursor = (SelectionPlayerRuntimeBlock *)((sourceCursor->selection).entries + 1);
      memberOrEntryRemaining = memberOrEntryRemaining + -1;
    } while (memberOrEntryRemaining != 0);
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            (&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  node = g_InGameRuntimeRoot;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)node);
    if ((transferModeFlags & 4) != 0) {
      averagePosition = SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf();
      if (!averagePosition.carry) {
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
          (FrontendPlayerIndex playerIndex,dword unusedArg1,
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
          (FrontendPlayerIndex playerIndex,dword unusedArg1,dword unusedArg2,
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
  uint writeOffset;
  uint nextOffset;
  
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
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8
   [ui/ingame/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerTextCommand_PublishConditionalRichText
          (FrontendPlayerIndex playerIndex,dword unusedArg1,dword unusedArg2,dword unusedArg3)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  word *stream;
  TextResourceResolveEaxCf5 messageText;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((((playerBlock->packedSelectionState809C &
        1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex + 8U & 0x1f))
        != 0) ||
      ((playerBlock->packedSelectionState809C &
       1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId + 0x10U &
            0x1f)) != 0)) &&
     ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL)) {
    Text_CopyNarrowToUtf16Cf
              (0x60,(word *)&g_FrontendPlayerMessageScratchUtf16,playerBlock->reserved80B0_8117 + 0x10);
    messageText = TextResource_Resolve(0xff07);
    stream = messageText.eax;
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
   Cross-module calls: SelectionPointerArray_ContainsCf [gameplay/selection/runtime],
   ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,dword reservedZero0,dword reservedZero1,
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
  isSelected = SelectionPointerArray_ContainsCf(targetEntity,&array->selection);
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
   Ownership: ui/frontend/player.
   Purpose: Four-argument frontend callback. It scans exact 0x13B0-byte player blocks by ID, increments dword
   +0x54, evaluates the mode-specific ready condition, and clears global wait-state bit 0x10 and its companion
   state when the condition closes. EAX is preserved. Kept distinct from frontend slot indices, faction runtime
   indices, network endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
          (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4)

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
        (playerBlock->factionAssignment).readyOrWaitState =
             (playerBlock->factionAssignment).readyOrWaitState + 1;
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
          goto 
          FrontendPlayerRuntime_RecordReadyAndUpdateWaitState_ClearWaitFlagAndResetTickIfPendingThenReturn
          ;
        }
        break;
      }
      searchRemaining = searchRemaining - 1;
      readyScanRemaining = g_FrontendPlayerRuntimeBlockCount;
      readyScanBlock = g_FrontendPlayerRuntimeBlocks;
      playerBlock = playerBlock + 1;
    } while (searchRemaining != 0);
    do {
      if ((readyScanBlock->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      readyScanRemaining = readyScanRemaining - 1;
      readyScanBlock = readyScanBlock + 1;
    } while (readyScanRemaining != 0);
    if ((g_FrontendPlayerRuntimeBlocks->factionAssignment).readyOrWaitState < 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0xd0,0,0,0);
        return;
      }
      FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
      return;
    }
  }
FrontendPlayerRuntime_RecordReadyAndUpdateWaitState_ClearWaitFlagAndResetTickIfPendingThenReturn:
  if ((g_FrontendRuntimeFlags & 0x10) != 0) {
    g_FrontendNetworkTickCounter = 0;
    g_FrontendRuntimeFlags = g_FrontendRuntimeFlags & 0xffffffef;
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
          (PlayerRuntimeId playerId,dword argument2,dword argument3,
          FrontendConsensusValue consensusValue)

{
  UiAnchorFractionQ31 *rootAnchorFlags;
  UiRootNode *taskAssignmentRoot;
  uint combinedConsensus;
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
        rootAnchorFlags = &((UiRootNode *)(uintptr_t)g_FrontendRootNode)[0x21].base.bottomAnchorQ31;
        *rootAnchorFlags = *rootAnchorFlags | 8;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL
              ) {
        rootAnchorFlags = &((UiRootNode *)(uintptr_t)g_FrontendRootNode)[0x21].base.bottomAnchorQ31;
        *rootAnchorFlags = *rootAnchorFlags & 0xfffffff7;
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
          (PlayerRuntimeId playerId,dword unusedArg1,dword unusedArg2,dword unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  undefined4 *messageBuffer;
  
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
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], FrontendRecentTextHistory_InsertAndRebuild5
   [ui/frontend/runtime].
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerMessageBuffer_PublishTextById
          (PlayerRuntimeId playerId,dword unusedArg1,dword unusedArg2,dword unusedArg3)

{
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  int messageBuffer;
  word *stream;
  TextResourceResolveEaxCf5 messageText;
  
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
  Text_CopyNarrowToUtf16Cf(0x60,(word *)&g_FrontendPlayerMessageScratchUtf16,(byte *)(messageBuffer + 4));
  messageText = TextResource_Resolve(0xff07);
  stream = messageText.eax;
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
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(frontendRoot->playerRuntimeList).rowCount,
               (word *)&g_FrontendNetworkRuntimeCountTextUtf16);
    UiPointerList_RefreshSelectionAndQueueAction(&frontendRoot->playerRuntimeList);
  }
  return;
}


/* Address: 0x0055FAD0.
   Ownership: ui/frontend/player.
   Purpose: Resolves a model token, replaces the player selection array with that model, and refreshes the local
   selection panels when the player is local.
   Cross-module calls: SelectionPointerArray_Clear32 [gameplay/selection/runtime],
   SelectionPointerArray_InsertUniqueAndRecenter [gameplay/selection/runtime], InGameSelectionDetailPanel_Rebuild
   [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology].
*/
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerIndex,dword reservedZero0,dword reservedZero1,
          RuntimeToken modelToken)

{
  GameEntityRuntime *entityRuntime;
  
  if (modelToken != 0) {
    entityRuntime = (GameEntityRuntime *)(modelToken + (int)g_ArmyRuntimeRebaseBaseMinusOne);
    SelectionPointerArray_Clear32(&g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
    if ((entityRuntime->common).ownership.modelNode != (ModelRuntimeNode *)0x0) {
      SelectionPointerArray_InsertUniqueAndRecenter
                (entityRuntime,&g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
      if (playerIndex == g_LocalPlayerRuntimeId) {
        InGameSelectionDetailPanel_Rebuild();
        UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      }
    }
  }
  return;
}


/* Address: 0x005607E0.
   Ownership: ui/frontend/player.
   Purpose: Resolves an army token through g_ArmyRuntimeRebaseBaseMinusOne, stores the army pointer for the player,
   captures army flag 0x80, and clears that flag on the army record. It is separate from FactionRuntimeIndex,
   PlayerRuntimeId, network-player identity, and PCK-backed asset identifiers. Typed parameters: p3
   modelOffset→ArmyRuntimeSavedOffset_V343. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
          (FrontendPlayerIndex playerIndex,dword unusedArg1,dword unusedArg2,ArmyRuntimeSavedOffset modelOffset)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  uint armyFlags;
  dword armyAddress;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (armyAddress = modelOffset + g_ModelRuntimeRebaseDelta, *(int *)(armyAddress + 4) != 0)) {
    armyFlags = *(uint *)(armyAddress + 0xec);
    playerBlock->assignmentToken80A0 = armyAddress;
    playerBlock->assignmentFlags80A4 = armyFlags & 0x80;
    *(uint *)(armyAddress + 0xec) = *(uint *)(armyAddress + 0xec) & 0xffffff7f;
  }
  return;
}

