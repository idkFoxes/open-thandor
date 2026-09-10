#include <thandor/ui/frontend/player.h>

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
undefined8 FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl)

{
  undefined4 in_EAX;
  int iVar1;
  undefined4 in_EDX;
  word *pwVar2;
  
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
    pwVar2 = textEditControl->textPrefix6C;
    for (iVar1 = 0x18; iVar1 != 0; iVar1 = iVar1 + -1) {
      pwVar2[0] = 0;
      pwVar2[1] = 0;
      pwVar2 = pwVar2 + 2;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
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
void FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
               (FrontendPlayerIndex playerIndex,undefined4 param_2,RuntimeToken armyToken,
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
      UiPageStack_SetActiveIndex(2,&g_InGameRuntimeRoot->technologyPageStack0BD0);
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
undefined4 FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source)

{
  undefined4 in_EAX;
  uint consensusValue;
  
  consensusValue = *(uint *)(source + 0x4c) & 2;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_SetConsensusValueAndRefresh(g_LocalPlayerRuntimeId,0,0,consensusValue);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x820,0,0,consensusValue);
  }
  return in_EAX;
}

/* Address: 0x0054D3A0.
   Ownership: ui/frontend/player.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[11]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[11] (0x200B). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks.
*/
undefined8 FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode)

{
  UiNodeBase *pUVar1;
  undefined4 in_EAX;
  undefined4 in_EDX;
  
  pUVar1 = (rootNode->base).parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    rootNode = (UiRootNode *)(rootNode->base).parent;
    pUVar1 = (rootNode->base).parent;
  }
  pUVar1 = rootNode[0xfb].base.firstChild;
  if (pUVar1 != (UiNodeBase *)rootNode[0xfa].rootFlags) {
    pUVar1->nextSibling->left = 1;
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(rootNode);
  }
  return CONCAT44(in_EDX,in_EAX);
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
void __cdecl FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void)

{
  FrontendHeartbeatTickCount *pFVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  word *stream;
  void *replacementPayload;
  int iVar3;
  FrontendPlayerRuntimeBlockCount extraout_ECX;
  int iVar4;
  int extraout_EDX;
  int iVar5;
  FrontendPlayerRuntimeRecord *pFVar6;
  FrontendCommandPacketRecord *pFVar7;
  FrontendPlayerRuntimeRecord *pFVar8;
  UiTransferEndpointDescriptor *endpoint;
  FrontendPlayerRuntimeRecord *pFVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  FrontendCommandPacketRecord *pFVar11;
  FrontendPlayerRemovalPacket10007 *pFVar12;
  FrontendCommandPacketRecord *pFStack_20;
  
  iVar5 = 0;
  pFStack_20 = g_FrontendPlayerCommandRecords;
  pFVar11 = g_FrontendPlayerCommandRecords;
  iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
  pFVar6 = g_FrontendPlayerRuntimeBlocks + 1;
  pFVar9 = g_FrontendPlayerRuntimeBlocks + 1;
  pFVar12 = (FrontendPlayerRemovalPacket10007 *)pFVar11;
  if (iVar4 != 0 && 0 < (int)g_FrontendPlayerRuntimeBlockCount) {
    do {
      pFVar1 = &pFVar6->heartbeatExpiryTicks;
      *pFVar1 = *pFVar1 - 1;
      if (*pFVar1 == 0) {
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        stream = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
        FrontendRecentTextHistory_InsertAndRebuild5();
        pFVar11 = (FrontendCommandPacketRecord *)pFVar6->playerRuntimeId;
        iVar5 = iVar5 + 1;
        pFVar8 = pFVar6 + 1;
        iVar4 = extraout_EDX;
        pFVar10 = pFVar9;
      }
      else {
        pFVar8 = pFVar6 + 1;
        pFVar10 = pFVar9 + 1;
        pFVar11 = (FrontendCommandPacketRecord *)(pFVar12 + 1);
        iVar3 = 0x4ec;
        if (pFVar10 != pFVar8) {
          for (; pFVar10 = pFVar9, pFVar8 = pFVar6, iVar3 != 0; iVar3 = iVar3 + -1) {
            pFVar10->runtimeState00 = pFVar8->runtimeState00;
            pFVar6 = (FrontendPlayerRuntimeRecord *)&pFVar8->peerSequenceToken;
            pFVar9 = (FrontendPlayerRuntimeRecord *)&pFVar10->peerSequenceToken;
          }
          pFVar7 = pFStack_20;
          for (iVar3 = 8; iVar3 != 0; iVar3 = iVar3 + -1) {
            (pFVar12->header).packedTypeAndUnitCount = (pFVar7->header).packedTypeAndUnitCount;
            pFVar7 = (FrontendCommandPacketRecord *)&(pFVar7->header).sequenceToken;
            pFVar12 = (FrontendPlayerRemovalPacket10007 *)&(pFVar12->header).sequenceToken;
          }
        }
      }
      pFStack_20 = pFStack_20 + 1;
      iVar4 = iVar4 + -1;
      pFVar6 = pFVar8;
      pFVar9 = pFVar10;
      pFVar12 = (FrontendPlayerRemovalPacket10007 *)pFVar11;
    } while (iVar4 != 0);
  }
  if (iVar5 != 0) {
    do {
      endpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
      g_FrontendPlayerRemovalPacket10007.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_10007_PLAYER_REMOVAL;
      g_FrontendPlayerRemovalPacket10007.removedPlayerToken = (FrontendPlayerRuntimeId)pFVar11;
      FVar2 = g_FrontendPlayerRuntimeBlockCount;
      while (FVar2 != 1) {
        pFVar12 = &g_FrontendPlayerRemovalPacket10007;
        UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPlayerRemovalPacket10007.header);
        endpoint = endpoint + 0x13b;
        pFVar11 = (FrontendCommandPacketRecord *)pFVar12;
        FVar2 = extraout_ECX;
      }
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
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
undefined8
FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf
          (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId)

{
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar1;
  undefined4 in_EDX;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  while ((pFVar2->playerRuntimeId == excludedPlayerId ||
         (g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->assignmentToken80A0 !=
          assignmentToken))) {
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
    if (FVar1 == 0) {
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00514F60.
   Ownership: ui/frontend/player.
   Purpose: EAX and EDX remain preserved. Typed parameters: p0 assignmentToken→RuntimeToken. Calling convention,
   parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken)

{
  undefined4 in_EAX;
  dword playerBlocksRemaining;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00544130.
   Ownership: ui/frontend/player.
   Purpose: Four-argument frontend callback. In mode bit 1 it marks the matching player's +0x54 dword and clears
   root flag 0x08 when every player is ready; in mode bit 0 it sets flag 0x08 for the local player. EAX is
   preserved. Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK
   asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId.
*/
void FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08
               (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  SessionNetworkRoleFlags SVar3;
  SessionNetworkRoleFlags SVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    if (playerId != g_LocalPlayerRuntimeId) {
      return;
    }
    *(uint *)(g_FrontendRootNode + 0x750) = *(uint *)(g_FrontendRootNode + 0x750) | 8;
    return;
  }
  SVar4 = g_FrontendPlayerRuntimeBlockCount;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  SVar3 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST;
  while( true ) {
    if (SVar3 == SESSION_NETWORK_ROLE_LOCAL) {
      return;
    }
    if (playerId == pFVar5->playerRuntimeId) break;
    pFVar5 = pFVar5 + 1;
    SVar4 = SVar4 - SESSION_NETWORK_ROLE_CLIENT;
    SVar3 = SVar4;
  }
  (pFVar5->factionAssignment).readyOrWaitState = 1;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  do {
    FVar1 = FVar1 - 1;
    if (FVar1 == 0) {
      *(uint *)(g_FrontendRootNode + 0x750) = *(uint *)(g_FrontendRootNode + 0x750) & 0xfffffff7;
      return;
    }
    pFVar2 = pFVar5 + 1;
    pFVar5 = pFVar5 + 1;
  } while ((pFVar2->factionAssignment).readyOrWaitState != 0);
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
void FrontendPlayerRuntime_MarkFlag08ById
               (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *pFVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == pFVar3->playerRuntimeId) {
      pFVar1 = &(pFVar3->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 8;
      return;
    }
    pFVar3 = pFVar3 + 1;
    FVar2 = FVar2 - 1;
  } while (FVar2 != 0);
  return;
}

/* Address: 0x00544300.
   Ownership: ui/frontend/player.
   Purpose: Handles frontend player runtime xor state mask by player id.
*/
undefined4
FrontendPlayerRuntime_XorStateMaskByPlayerId
          (int param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (param_1 == pFVar2->playerRuntimeId) {
      pFVar2->runtimeState64 = pFVar2->runtimeState64 ^ param_4;
      return in_EAX;
    }
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
  } while (FVar1 != 0);
  return in_EAX;
}

/* Address: 0x00544360.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks using exact stride 0x13B0, compares playerId against block
   dword +0x14, and ORs bit 0x04 into block dword +0x60 on the first match. The callback consumes four dword
   arguments; only playerId is used. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
undefined4
FrontendPlayerRuntime_MarkFlag04ById
          (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *pFVar1;
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == pFVar3->playerRuntimeId) {
      pFVar1 = &(pFVar3->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 4;
      return in_EAX;
    }
    pFVar3 = pFVar3 + 1;
    FVar2 = FVar2 - 1;
  } while (FVar2 != 0);
  return in_EAX;
}

/* Address: 0x00544820.
   Ownership: ui/frontend/player.
   Purpose: Scans the configured frontend player blocks using exact stride 0x13B0, compares playerId against block
   dword +0x14, and ORs bit 0x02 into block dword +0x60 on the first match. The callback consumes four dword
   arguments; only playerId is used. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void FrontendPlayerRuntime_MarkFlag02ById
               (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3)

{
  FrontendRoleStateFlags *pFVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == pFVar3->playerRuntimeId) {
      pFVar1 = &(pFVar3->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 2;
      return;
    }
    pFVar3 = pFVar3 + 1;
    FVar2 = FVar2 - 1;
  } while (FVar2 != 0);
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
undefined4
FrontendPlayerRuntime_MarkFlag01AndStoreValuesById
          (PlayerRuntimeId playerId,FrontendPlayerValue8C value8C,FrontendPlayerValue88 value88,
          FrontendPlayerValue84 value84)

{
  FrontendRoleStateFlags *pFVar1;
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == pFVar3->playerRuntimeId) {
      pFVar1 = &(pFVar3->factionAssignment).roleStateFlags;
      *pFVar1 = *pFVar1 | 1;
      pFVar3->callbackValue84 = value84;
      pFVar3->callbackValue88 = value88;
      pFVar3->callbackValue8C = value8C;
      return in_EAX;
    }
    pFVar3 = pFVar3 + 1;
    FVar2 = FVar2 - 1;
  } while (FVar2 != 0);
  return in_EAX;
}

/* Address: 0x00549190.
   Ownership: ui/frontend/player.
   Purpose: Builds faction availability bytes from the active configuration, initializes every 0x13B0-byte player
   block's faction and state fields, and leaves the local zero-based faction slot in EDX while preserving EAX.
*/
void __fastcall
FrontendPlayerRuntime_InitializeFactionAssignments(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  int iVar5;
  FrontendPlayerRuntimeRecord *pFVar6;
  
  iVar1 = g_FrontendLoadedLevelAsset;
  uVar3 = 1;
  iVar2 = *(int *)(g_FrontendLoadedLevelAsset + 0x314);
  iVar5 = *(int *)(g_FrontendLoadedLevelAsset + 0x310);
  do {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar3] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    iVar2 = iVar2 + -1;
    uVar3 = uVar3 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  for (; iVar2 != 0; iVar2 = iVar2 + -1) {
    g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar3] = FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    uVar3 = uVar3 + 1;
  }
  if (uVar3 < 7) {
    do {
      g_GameFactionRuntimeImage.tail.factionLifecycleStates[uVar3] = 0;
      uVar3 = uVar3 + 1;
    } while (uVar3 < 8);
  }
  uVar3 = 1;
  FVar4 = g_FrontendPlayerRuntimeBlockCount;
  pFVar6 = g_FrontendPlayerRuntimeBlocks;
  do {
    (pFVar6->factionAssignment).factionAssignmentIndex = uVar3;
    (pFVar6->factionAssignment).readyOrWaitState = 0;
    (pFVar6->factionAssignment).consensusValue = 0;
    uVar3 = uVar3 + 1;
    pFVar6 = pFVar6 + 1;
    if (*(uint *)(iVar1 + 0x310) < uVar3) {
      uVar3 = uVar3 - *(int *)(iVar1 + 0x310);
    }
    FVar4 = FVar4 - 1;
  } while (FVar4 != 0);
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
undefined4 FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *pFVar1;
  undefined4 in_EAX;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  uint sessionTickInterval;
  
  UiPageStack_SetActiveIndex(2,(UiPageStackControl *)&source[-0x10b].left);
  sessionTickInterval = g_SessionNetworkTickInterval;
  if ((int)g_FramebufferWidth < 0x281) {
    source[-0x110].rightAnchorQ31 = source[-0x110].rightAnchorQ31 | 0x2000;
  }
  g_FrontendNetworkState = 0;
  source[-8].rightOffset = sessionTickInterval >> 1;
  pFVar1 = g_FrontendPlayerRuntimeBlocks;
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  (localPlayerRecord->playerName).textUtf16[0] = 0;
  (localPlayerRecord->playerName).textUtf16[1] = 0;
  pFVar1->playerRuntimeId = 0;
  (pFVar1->factionAssignment).roleStateFlags = 0;
  pFVar1->runtimeState64 = 0;
  pFVar1->snapshotTransferFlags = 0;
  return in_EAX;
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
undefined4 FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source)

{
  undefined4 in_EAX;
  word *source_00;
  
  g_SessionNetworkTickInterval = source[1].vtable;
  source_00 = TextResource_Resolve
                        ((TextResourceId)((int)&g_SessionNetworkTickInterval[0x75].rightDrag + 1));
  RichTextCommandStream_CopyExpandedCf
            (0x40,(word *)&g_FrontendNetworkPlayerCountLabelUtf16,source_00);
  g_SessionNetworkTickInterval = (UiNodeVtable *)((int)g_SessionNetworkTickInterval << 1);
  return in_EAX;
}

/* Address: 0x0054D720.
   Ownership: ui/frontend/player.
   Purpose: Counts active player blocks with flag 0x100 using the exact 0x13B0 stride. Action 0x2006 is suppressed
   when three times the flagged count is below the total count and restored otherwise.
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists].
*/
void __cdecl FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction(void)

{
  int iVar1;
  uint uVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  iVar1 = 0;
  uVar2 = g_FrontendPlayerRuntimeCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((pFVar3->capabilityFlags & 0x100) != 0) {
      iVar1 = iVar1 + 1;
    }
    pFVar3 = pFVar3 + 1;
    uVar2 = uVar2 - 1;
  } while (uVar2 != 0);
  if ((uint)(iVar1 * 3) < g_FrontendPlayerRuntimeCount) {
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
void FrontendPlayerRuntime_SetReadyFlagById
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
void __fastcall
FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    FVar3 = g_FrontendPlayerRuntimeBlockCount;
    pFVar4 = g_FrontendPlayerRuntimeBlocks;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      do {
        if (playerRuntimeId == pFVar4->playerRuntimeId) {
          (pFVar4->factionAssignment).readyOrWaitState = 1;
          pFVar2 = g_FrontendPlayerRuntimeBlocks;
          FVar1 = g_FrontendPlayerRuntimeBlockCount;
          break;
        }
        FVar3 = FVar3 - 1;
        pFVar4 = pFVar4 + 1;
        pFVar2 = g_FrontendPlayerRuntimeBlocks;
        FVar1 = g_FrontendPlayerRuntimeBlockCount;
      } while (FVar3 != 0);
      do {
        FVar1 = FVar1 - 1;
        if (FVar1 == 0) {
          UiNodeList_UnsuppressActionId(0x101b,(UiNodeBase *)g_InGameRuntimeRoot);
          return;
        }
        pFVar4 = pFVar2 + 1;
        pFVar2 = pFVar2 + 1;
      } while ((pFVar4->factionAssignment).readyOrWaitState != 0);
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
void __fastcall
FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if (playerRuntimeId == pFVar3->playerRuntimeId) {
        (pFVar3->factionAssignment).readyOrWaitState =
             (pFVar3->factionAssignment).readyOrWaitState + 1;
        FVar2 = g_FrontendPlayerRuntimeBlockCount;
        pFVar4 = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerRuntimeId != 0) {
            return;
          }
          if ((pFVar3->factionAssignment).readyOrWaitState < 2) {
            return;
          }
          goto 
          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus_ClearUiCommandReadyWaitFlagsAndReturn
          ;
        }
        break;
      }
      FVar1 = FVar1 - 1;
      FVar2 = g_FrontendPlayerRuntimeBlockCount;
      pFVar4 = g_FrontendPlayerRuntimeBlocks;
      pFVar3 = pFVar3 + 1;
    } while (FVar1 != 0);
    do {
      if ((pFVar4->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      FVar2 = FVar2 - 1;
      pFVar4 = pFVar4 + 1;
    } while (FVar2 != 0);
    if ((g_FrontendPlayerRuntimeBlocks->factionAssignment).readyOrWaitState < 2) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
        return;
      }
      FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(0,param_2,g_LocalPlayerRuntimeId)
      ;
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
undefined4 __fastcall
FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId,
          ArmyRuntimeSavedOffset armyRuntimeOffset2,ArmyRuntimeSavedOffset armyRuntimeOffset1,
          ArmyRuntimeSavedOffset armyRuntimeOffset0)

{
  undefined4 in_EAX;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  
  if ((armyRuntimeOffset0 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              (param_1,param_2,
               (GameEntityRuntime *)(armyRuntimeOffset0 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    param_1 = extraout_ECX;
    param_2 = extraout_EDX;
  }
  if ((armyRuntimeOffset1 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              (param_1,param_2,
               (GameEntityRuntime *)(armyRuntimeOffset1 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
    param_1 = extraout_ECX_00;
    param_2 = extraout_EDX_00;
  }
  if ((armyRuntimeOffset2 != 0) &&
     ((((GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne))->common).
      ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    SelectionPointerArray_InsertUniqueAndRecenter
              (param_1,param_2,
               (GameEntityRuntime *)(armyRuntimeOffset2 + (int)g_ArmyRuntimeRebaseBaseMinusOne),
               &g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  }
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
  }
  return in_EAX;
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
undefined4
FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerIndex,RuntimeToken selectionEntryToken2,
          RuntimeToken selectionEntryToken1,RuntimeToken selectionEntryToken0)

{
  undefined4 in_EAX;
  
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
    selectionEntryToken2 =
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
  return in_EAX;
}

/* Address: 0x0055FCD0.
   Ownership: ui/frontend/player.
   Purpose: Clears the selected player's 32-entry selection array and rebuilds selection-detail and catalog panels
   when the affected player is local. It is separate from FactionRuntimeIndex, PlayerRuntimeId, network-player
   identity, and PCK-backed asset identifiers.
   Cross-module calls: SelectionPointerArray_Clear32 [gameplay/selection/runtime],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiCatalogGroup48_RebuildGrid [ui/ingame/technology].
*/
void FrontendPlayerSelection_ClearAndRefreshLocalPanels
               (FrontendPlayerIndex playerIndex,dword callbackArg1,dword callbackArg2,
               dword callbackArg3)

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
void FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
               (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex,
               FrontendSelectionTransferModeFlags transferModeFlags,
               FrontendFactionAssignmentIndex selectionGroupIndex)

{
  GameEntityRuntime *pGVar1;
  InGameRuntimeRootImageC3E4 *node;
  int iVar2;
  int iVar3;
  Q12 originY;
  int iVar4;
  ArmyRuntimeSlot **ppAVar5;
  SelectionPlayerRuntimeBlock *pSVar6;
  SelectionPlayerRuntimeBlock *pSVar7;
  SelectionPlayerRuntimeBlock *pSVar8;
  SelectionPlayerRuntimeBlock *pSVar9;
  bool bVar10;
  undefined8 uVar11;
  
  pSVar7 = (SelectionPlayerRuntimeBlock *)
           (selectionGroupIndex * 0x80 + 0x50f620 + factionIndex * 0x740);
  pSVar6 = pSVar7;
  pSVar9 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  if ((transferModeFlags & 1) != 0) {
    iVar4 = 0x20;
    pSVar6 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
    do {
      pSVar9 = pSVar6;
      pGVar1 = (pSVar9->selection).entries[0];
      if (pGVar1 != (GameEntityRuntime *)0x0) {
        iVar2 = 0x100;
        ppAVar5 = g_GameFactionRuntimeImage.records[factionIndex].runtimeGroupMembers8x32;
        do {
          if (pGVar1 == (GameEntityRuntime *)*ppAVar5) {
            *ppAVar5 = (ArmyRuntimeSlot *)0x0;
          }
          ppAVar5 = ppAVar5 + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
      }
      iVar4 = iVar4 + -1;
      pSVar6 = (SelectionPlayerRuntimeBlock *)((pSVar9->selection).entries + 1);
    } while (iVar4 != 0);
    pSVar6 = (SelectionPlayerRuntimeBlock *)&pSVar9[-1].packedSelectionState809C;
    pSVar9 = pSVar7;
  }
  iVar4 = 0x20;
  if ((transferModeFlags & 2) == 0) {
    for (; iVar4 != 0; iVar4 = iVar4 + -1) {
      (pSVar9->selection).entries[0] = (pSVar6->selection).entries[0];
      pSVar6 = (SelectionPlayerRuntimeBlock *)((pSVar6->selection).entries + 1);
      pSVar9 = (SelectionPlayerRuntimeBlock *)((pSVar9->selection).entries + 1);
    }
  }
  else {
    iVar2 = 0x20;
    do {
      pGVar1 = (pSVar6->selection).entries[0];
      iVar3 = iVar4;
      pSVar7 = pSVar9;
      if (pGVar1 != (GameEntityRuntime *)0x0) {
        do {
          if (pGVar1 == (pSVar7->selection).entries[0])
          goto 
          FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh_AdvanceAfterDuplicateOrAppendDecision
          ;
          iVar3 = iVar3 + -1;
          pSVar7 = (SelectionPlayerRuntimeBlock *)((pSVar7->selection).entries + 1);
        } while (iVar3 != 0);
        bVar10 = true;
        iVar3 = iVar4;
        pSVar7 = pSVar9;
        do {
          pSVar8 = pSVar7;
          if (iVar3 == 0) break;
          iVar3 = iVar3 + -1;
          pSVar8 = (SelectionPlayerRuntimeBlock *)((pSVar7->selection).entries + 1);
          bVar10 = (pSVar7->selection).entries[0] == (GameEntityRuntime *)0x0;
          pSVar7 = pSVar8;
        } while (!bVar10);
        if (bVar10) {
          *(GameEntityRuntime **)(pSVar8[-1].reserved80B0_8117 + 100) =
               (pSVar6->selection).entries[0];
        }
      }
FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh_AdvanceAfterDuplicateOrAppendDecision
      :
      pSVar6 = (SelectionPlayerRuntimeBlock *)((pSVar6->selection).entries + 1);
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
  }
  SelectionPointerArray_RecenterOffsetsAroundAveragePosition
            (&g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->selection);
  node = g_InGameRuntimeRoot;
  if (playerRuntimeId == g_LocalPlayerRuntimeId) {
    InGameSelectionDetailPanel_Rebuild();
    UiCatalogGroup48_RebuildGrid((UiNodeBase *)node);
    bVar10 = false;
    if ((transferModeFlags & 4) != 0) {
      uVar11 = SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf();
      if (!bVar10) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((node->worldRuntime0A30).motion.pitchAngle,
                   (node->worldRuntime0A30).motion.headingAngle,
                   (node->worldRuntime0A30).motion.committedDistanceQ12,
                   (Q12)((ulonglong)uVar11 >> 0x20),originY,(Q12)uVar11,&node->worldRuntime0A30);
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
void FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
               (FrontendPlayerIndex playerIndex,dword arg1,
               TechnologyIndexOrRestoreCode technologyIndexOrRestore,
               ArmyRuntimeSavedOffset modelOffset)

{
  GameEntityRuntimeFlags *pGVar1;
  SelectionPlayerRuntimeBlock *pSVar2;
  GameEntityRuntime *entity;
  
  pSVar2 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (entity = (GameEntityRuntime *)(modelOffset + g_ModelRuntimeRebaseDelta),
     (entity->common).ownership.modelNode != (ModelRuntimeNode *)0x0)) {
    pSVar2->assignmentToken80A0 = 0;
    if (technologyIndexOrRestore != 0) {
      if ((int)technologyIndexOrRestore < 0) {
        pGVar1 = &(entity->common).runtimeFlags;
        *pGVar1 = *pGVar1 | pSVar2->assignmentFlags80A4;
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
void FrontendPlayerTextCommand_SetPackedState
               (FrontendPlayerIndex playerIndex,dword arg1,dword arg2,
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
void FrontendPlayerTextCommand_AppendTripleClamped
               (FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
               FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  uint uVar2;
  uint uVar3;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  uVar2 = pSVar1->packedSelectionState809C & 0xff;
  *(FrontendTextCommandValue0 *)(pSVar1->reserved80B0_8117 + uVar2 + 0x10) = value0;
  *(FrontendTextCommandValue1 *)(pSVar1->reserved80B0_8117 + uVar2 + 0x14) = value1;
  uVar3 = uVar2 + 0xc;
  pSVar1->packedSelectionState809C = pSVar1->packedSelectionState809C & 0xffffff00;
  if (0x24 < uVar3) {
    uVar3 = 0x24;
  }
  *(FrontendTextCommandValue2 *)(pSVar1->reserved80B0_8117 + uVar2 + 0x18) = value2;
  pSVar1->packedSelectionState809C = pSVar1->packedSelectionState809C | uVar3;
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
void FrontendPlayerTextCommand_PublishConditionalRichText
               (FrontendPlayerIndex playerIndex,dword arg1,dword arg2,dword arg3)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  word *stream;
  undefined8 uVar2;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((((pSVar1->packedSelectionState809C &
        1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex + 8U & 0x1f))
        != 0) ||
      ((pSVar1->packedSelectionState809C &
       1 << ((char)(g_InGameRuntimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId + 0x10U &
            0x1f)) != 0)) &&
     ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL)) {
    Text_CopyNarrowToUtf16Cf
              (0x60,(word *)&g_FrontendPlayerMessageScratchUtf16,pSVar1->reserved80B0_8117 + 0x10);
    stream = TextResource_Resolve(0xff07);
    uVar2 = RichTextCommandStream_PatchPayloadBySelector(0,pSVar1->reserved80B0_8117 + 0x40,stream);
    RichTextCommandStream_PatchPayloadBySelector
              (1,&g_FrontendPlayerMessageScratchUtf16,(word *)uVar2);
    InGameRecentTextHistory_InsertAndRebuild8();
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
void FrontendPlayerSelection_ApplyEntryOrAll
               (FrontendPlayerIndex playerIndex,undefined4 param_2,undefined4 param_3,
               RuntimeToken selectionEntryToken)

{
  GameEntityRuntime *pGVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  WorldRuntimeContext *worldRuntime;
  SelectionPlayerRuntimeBlock *array;
  bool bVar3;
  
  array = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  bVar3 = (InGameRuntimeRootImageC3E4 *)0xfffff5cf < g_InGameRuntimeRoot;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  pGVar1 = (GameEntityRuntime *)
           SelectionPointerArray_ContainsCf
                     ((GameEntityRuntime *)
                      (selectionEntryToken + (int)g_ArmyRuntimeRebaseBaseMinusOne),&array->selection
                     );
  iVar2 = extraout_ECX;
  if (bVar3) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,pGVar1);
    return;
  }
  do {
    pGVar1 = (array->selection).entries[0];
    if (pGVar1 != (GameEntityRuntime *)0x0) {
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,pGVar1);
      iVar2 = extraout_ECX_00;
    }
    array = (SelectionPlayerRuntimeBlock *)((array->selection).entries + 1);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
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
void FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
               (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
      SESSION_NETWORK_ROLE_LOCAL) {
    do {
      if (playerId == pFVar3->playerRuntimeId) {
        (pFVar3->factionAssignment).readyOrWaitState =
             (pFVar3->factionAssignment).readyOrWaitState + 1;
        FVar2 = g_FrontendPlayerRuntimeBlockCount;
        pFVar4 = g_FrontendPlayerRuntimeBlocks;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL)
        {
          if (playerId != 0) {
            return;
          }
          if ((pFVar3->factionAssignment).readyOrWaitState < 2) {
            return;
          }
          goto 
          FrontendPlayerRuntime_RecordReadyAndUpdateWaitState_ClearWaitFlagAndResetTickIfPendingThenReturn
          ;
        }
        break;
      }
      FVar1 = FVar1 - 1;
      FVar2 = g_FrontendPlayerRuntimeBlockCount;
      pFVar4 = g_FrontendPlayerRuntimeBlocks;
      pFVar3 = pFVar3 + 1;
    } while (FVar1 != 0);
    do {
      if ((pFVar4->factionAssignment).readyOrWaitState == 0) {
        return;
      }
      FVar2 = FVar2 - 1;
      pFVar4 = pFVar4 + 1;
    } while (FVar2 != 0);
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
void FrontendPlayerRuntime_SetConsensusValueAndRefresh
               (PlayerRuntimeId playerId,dword argument2,dword argument3,
               FrontendConsensusValue consensusValue)

{
  UiAnchorFractionQ31 *pUVar1;
  UiRootNode *taskAssignmentRoot;
  uint uVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
  pFVar4 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerId == pFVar4->playerRuntimeId) {
      (pFVar4->factionAssignment).consensusValue = consensusValue;
      taskAssignmentRoot = g_FrontendRootNode;
      uVar2 = 0xffffffff;
      FVar3 = g_FrontendPlayerRuntimeBlockCount;
      pFVar4 = g_FrontendPlayerRuntimeBlocks;
      do {
        uVar2 = uVar2 & (pFVar4->factionAssignment).consensusValue;
        pFVar4 = pFVar4 + 1;
        FVar3 = FVar3 - 1;
      } while (FVar3 != 0);
      if (uVar2 == 0) {
        pUVar1 = &g_FrontendRootNode[0x21].base.bottomAnchorQ31;
        *pUVar1 = *pUVar1 | 8;
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL
              ) {
        pUVar1 = &g_FrontendRootNode[0x21].base.bottomAnchorQ31;
        *pUVar1 = *pUVar1 & 0xfffffff7;
      }
      FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(0,playerId,taskAssignmentRoot);
      return;
    }
    pFVar4 = pFVar4 + 1;
    FVar3 = FVar3 - 1;
  } while (FVar3 != 0);
  return;
}

/* Address: 0x00545490.
   Ownership: ui/frontend/player.
   Purpose: Scans the active frontend player blocks at exact 0x13B0 stride and resets the matching 0x64-byte
   message record write offset to four. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p0 playerId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged.
*/
void FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
               (PlayerRuntimeId playerId,dword arg1,dword arg2,dword arg3)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  undefined4 *puVar3;
  
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  puVar3 = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (FVar1 = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    FVar1 = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (FVar1 == 0) {
      return;
    }
    if (playerId == pFVar2->playerRuntimeId) break;
    FVar1 = FVar1 - 1;
    pFVar2 = pFVar2 + 1;
    puVar3 = puVar3 + 0x19;
  }
  *puVar3 = 4;
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
void FrontendPlayerMessageBuffer_AppendTripleById
               (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
               FrontendMessageValueC valueC)

{
  int iVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  int *piVar4;
  
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  piVar4 = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (FVar2 = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    FVar2 = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (FVar2 == 0) {
      return;
    }
    if (playerId == pFVar3->playerRuntimeId) break;
    FVar2 = FVar2 - 1;
    pFVar3 = pFVar3 + 1;
    piVar4 = piVar4 + 0x19;
  }
  iVar1 = *piVar4;
  *piVar4 = *piVar4 + 0xc;
  *(FrontendMessageValueC *)(iVar1 + (int)piVar4) = valueC;
  *(FrontendMessageValueB *)(iVar1 + 4 + (int)piVar4) = valueB;
  *(FrontendMessageValueA *)(iVar1 + 8 + (int)piVar4) = valueA;
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
void FrontendPlayerMessageBuffer_PublishTextById
               (PlayerRuntimeId playerId,dword arg1,dword arg2,dword arg3)

{
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  int iVar3;
  word *stream;
  undefined8 uVar4;
  
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  iVar3 = g_FrontendPlayerMessageBuffers;
  if ((g_FrontendNetworkState == 3) ||
     (FVar1 = g_FrontendPlayerRuntimeBlockCount, g_FrontendNetworkState == 2)) {
    FVar1 = g_FrontendPlayerRuntimeCount;
  }
  while( true ) {
    if (FVar1 == 0) {
      return;
    }
    if (playerId == pFVar2->playerRuntimeId) break;
    FVar1 = FVar1 - 1;
    pFVar2 = pFVar2 + 1;
    iVar3 = iVar3 + 100;
  }
  Text_CopyNarrowToUtf16Cf(0x60,(word *)&g_FrontendPlayerMessageScratchUtf16,(byte *)(iVar3 + 4));
  stream = TextResource_Resolve(0xff07);
  uVar4 = RichTextCommandStream_PatchPayloadBySelector(0,&pFVar2->playerName,stream);
  RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendPlayerMessageScratchUtf16,(word *)uVar4)
  ;
  FrontendRecentTextHistory_InsertAndRebuild5();
  return;
}

/* Address: 0x0054EBD0.
   Ownership: ui/frontend/player.
   Purpose: Decrements remote player expiry counters, compacts exact 0x13B0-byte player blocks, adjusts selection
   state, formats the new count, and refreshes the player list. Preserved EDX:EAX is incidental.
   Cross-module calls: UiPointerList_RefreshSelectionAndQueueAction [ui/controls/lists].
*/
void FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(UiRootNode *frontendRoot)

{
  FrontendHeartbeatTickCount *pFVar1;
  UiNodeBase **ppUVar2;
  UiNodeBase *pUVar3;
  UiNodeBase *pUVar4;
  int iVar5;
  undefined1 *puVar6;
  UiPointerListControl *control;
  FrontendPlayerRuntimeRecord *pFVar7;
  FrontendPlayerRuntimeRecord *pFVar8;
  FrontendPlayerRuntimeRecord *pFVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  
  puVar6 = (undefined1 *)((int)&frontendRoot[0xfa].callbacks[-1].keyboardFallbackCf + 3);
  if (puVar6 != (undefined1 *)0x0 && 0 < (int)frontendRoot[0xfa].callbacks) {
    pUVar4 = (UiNodeBase *)(frontendRoot[0xfa].rootFlags + 4);
    pFVar7 = g_FrontendPlayerRuntimeBlocks + 1;
    pFVar9 = g_FrontendPlayerRuntimeBlocks + 1;
    do {
      pFVar1 = &pFVar7->heartbeatExpiryTicks;
      *pFVar1 = *pFVar1 - 1;
      pFVar10 = pFVar9;
      if (*pFVar1 == 0) {
        pFVar8 = pFVar7 + 1;
        frontendRoot[0xfa].callbacks =
             (UiRootCallbacks *)((int)&frontendRoot[0xfa].callbacks[-1].keyboardFallbackCf + 3);
        g_FrontendPlayerRuntimeCount = g_FrontendPlayerRuntimeCount + -1;
        pUVar3 = frontendRoot[0xfb].base.firstChild;
        if (pUVar4 == pUVar3) {
          frontendRoot[0xfb].base.firstChild = (UiNodeBase *)frontendRoot[0xfa].rootFlags;
        }
        else if (pUVar4 <= pUVar3) {
          ppUVar2 = &frontendRoot[0xfb].base.firstChild;
          *ppUVar2 = (UiNodeBase *)&(*ppUVar2)[-1].nodeFlags;
        }
      }
      else {
        pFVar8 = pFVar7 + 1;
        pFVar10 = pFVar9 + 1;
        pUVar4 = (UiNodeBase *)&pUVar4->firstChild;
        if (pFVar10 != pFVar8) {
          pFVar8 = pFVar7;
          pFVar10 = pFVar9;
          for (iVar5 = 0x4ec; iVar5 != 0; iVar5 = iVar5 + -1) {
            pFVar10->runtimeState00 = pFVar8->runtimeState00;
            pFVar8 = (FrontendPlayerRuntimeRecord *)&pFVar8->peerSequenceToken;
            pFVar10 = (FrontendPlayerRuntimeRecord *)&pFVar10->peerSequenceToken;
          }
        }
      }
      puVar6 = puVar6 + -1;
      pFVar7 = pFVar8;
      pFVar9 = pFVar10;
    } while (puVar6 != (undefined1 *)0x0);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)frontendRoot[0xfa].callbacks,
               (word *)&g_FrontendNetworkRuntimeCountTextUtf16);
    UiPointerList_RefreshSelectionAndQueueAction(control);
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
void FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
               (FactionRuntimeIndex playerIndex,undefined4 param_2,undefined4 param_3,
               RuntimeToken modelToken)

{
  GameEntityRuntime *entityRuntime;
  undefined4 extraout_ECX;
  undefined4 extraout_EDX;
  
  if (modelToken != 0) {
    entityRuntime =
         (GameEntityRuntime *)
         SelectionPointerArray_Clear32
                   (&g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
    if ((entityRuntime->common).ownership.modelNode != (ModelRuntimeNode *)0x0) {
      SelectionPointerArray_InsertUniqueAndRecenter
                (extraout_ECX,extraout_EDX,entityRuntime,
                 &g_SelectionPlayerRuntimeBlockPointers[playerIndex]->selection);
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
void FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
               (FrontendPlayerIndex playerIndex,dword arg1,dword arg2,
               ArmyRuntimeSavedOffset modelOffset)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  uint uVar2;
  dword dVar3;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerIndex];
  if ((modelOffset != 0) &&
     (dVar3 = modelOffset + g_ModelRuntimeRebaseDelta, *(int *)(dVar3 + 4) != 0)) {
    uVar2 = *(uint *)(dVar3 + 0xec);
    pSVar1->assignmentToken80A0 = dVar3;
    pSVar1->assignmentFlags80A4 = uVar2 & 0x80;
    *(uint *)(dVar3 + 0xec) = *(uint *)(dVar3 + 0xec) & 0xffffff7f;
  }
  return;
}
