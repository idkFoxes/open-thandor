#include <thandor/ui/frontend/session.h>

/* Implementation ownership: ui/frontend/session. */

/* Address: 0x00544270.
   Ownership: ui/frontend/session.
   Purpose: Releases the selected frontend package resource, clears its pointer and companion state dword, then
   returns to the main page with state code 2. EAX is preserved. Typed parameters: p0
   callbackContext→FrontendReturnCallbackContext32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
undefined4
FrontendSession_ReleaseSelectedResourceAndReturnToMainPage
          (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3,
          dword argument4)

{
  undefined4 in_EAX;
  
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  g_FrontendScenarioInitializationCount = 0;
  FrontendSession_ReturnToMainPage(callbackContext,0,0,2);
  return in_EAX;
}

/* Address: 0x00548FE0.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[72]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[72] (0x2048). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: Movie_Close [movie/runtime/playback], UiPageStack_SetActiveIndex [ui/controls/layout],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source)

{
  undefined4 in_EAX;
  UiPageStackControl *stack;
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  Movie_Close();
  UiPageStack_SetActiveIndex(0,stack);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)
            ((GraphicsTextureSourceAsset *)source[7].left);
  (*g_MemoryApi.free)((void *)source[7].leftOffset);
  (*g_MemoryApi.free)((void *)source[7].topOffset);
  source[7].left = 0;
  source[7].leftOffset = 0;
  source[7].topOffset = 0;
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
  }
  return in_EAX;
}

/* Address: 0x00549090.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[71]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[71] (0x2047). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendSession_ApplyGameSpeedAndReturnToMainPage.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08 [ui/frontend/player].
*/
undefined4 FrontendSessionAction_ApplySpeedOrToggleReady(void *source)

{
  undefined4 in_EAX;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,1);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x2c0,0,0,1);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1e0,0,0,0);
  }
  return in_EAX;
}

/* Address: 0x0054C770.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[0]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[0] (0x2000). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands],
   Random_SelectPrimaryStream [core/math/random].
*/
undefined4 FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source)

{
  undefined4 in_EAX;
  
  (*g_NetworkBackendSlot3)();
  g_FrontendNetworkState = 0;
  (*g_NetworkBackendSlot1)();
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
  }
  Random_SelectPrimaryStream();
  return in_EAX;
}

/* Address: 0x0054D0B0.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[6]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[6] (0x2006). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: Random_NextPrimary [core/math/random], Random_SetBothSeeds [core/math/random],
   Random_SelectSecondaryStream [core/math/random], FrontendCommandQueue_EnqueueLocalPlayerCommand
   [network/protocol/commands].
*/
undefined4 FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source)

{
  UiNodeBase *pUVar1;
  undefined4 in_EAX;
  dword seed;
  int iVar2;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  
  pUVar1 = source->parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    pUVar1 = source->parent;
  }
  g_FrontendPlayerRuntimeBlockCount = source[0x122].rightOffset;
  g_FrontendExpectedPlayerRuntimeBlockCount = 0;
  g_FrontendPendingSessionPlayerCount = g_FrontendPlayerRuntimeBlockCount;
  seed = Random_NextPrimary();
  Random_SetBothSeeds(seed);
  Random_SelectSecondaryStream();
  iVar2 = 8;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    (playerRecordCursor->factionAssignment).roleStateFlags = 0;
    playerRecordCursor->runtimeState64 = 0;
    playerRecordCursor = playerRecordCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  g_FrontendHostSnapshotTransferCountdown = 4;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,1);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,1);
  }
  return in_EAX;
}

/* Address: 0x00544250.
   Ownership: ui/frontend/session.
   Purpose: Stores the fourth callback argument into the active frontend runtime dword at +0xA80. EAX is preserved.
   Typed parameters: p3 gameSpeedPercent→GameSpeedPercent_V305. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void FrontendSession_SetGameSpeedPercent
               (dword argument1,dword argument2,dword argument3,GameSpeedPercent gameSpeedPercent)

{
  *(GameSpeedPercent *)(g_FrontendRootNode + 0xa80) = gameSpeedPercent;
  return;
}

/* Address: 0x0054A790.
   Ownership: ui/frontend/session.
   Purpose: Selects frontend page index 9 and sets runtime flag 0x2000 at +0x3B4 when the framebuffer width is at
   most 640 pixels. EAX is preserved.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void FrontendSession_ShowPage9WithCompactLayout(void *frontendRuntime)

{
  UiPageStack_SetActiveIndex(9,(UiPageStackControl *)((int)frontendRuntime + 0x508));
  if ((int)g_FramebufferWidth < 0x281) {
    *(uint *)((int)frontendRuntime + 0x3b4) = *(uint *)((int)frontendRuntime + 0x3b4) | 0x2000;
  }
  return;
}

/* Address: 0x0054D2E0.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[10]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[10] (0x200A). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiTransferMailbox_RandomizeSequenceToken [network/protocol/transfer], UiTransfer_SendPacketType10000Value2931Cf
   [network/protocol/transfer].
*/
undefined4 FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *pFVar1;
  undefined4 in_EAX;
  UiPointerListControl *control;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  
  g_FrontendNetworkState = 1;
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&source[-0x116].vtable);
  if ((int)g_FramebufferWidth < 0x281) {
    source[-0x11b].topAnchorQ31 = source[-0x11b].topAnchorQ31 | 0x2000;
  }
  UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&source[-0x127].left);
  UiPointerList_InitializeColumnLayout(0,g_FrontendSessionListRows,control);
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  UiTransferMailbox_RandomizeSequenceToken();
  UiTransfer_SendPacketType10000Value2931Cf();
  pFVar1 = g_FrontendPlayerRuntimeBlocks;
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

/* Address: 0x0054E3A0.
   Ownership: ui/frontend/session.
   Purpose: Ticks the frontend session-list expiry values, compacts expired 0xB0-byte rows, repairs selection
   offsets, and refreshes the associated pointer list.
   Cross-module calls: UiPointerList_RefreshSelectionAndQueueAction [ui/controls/lists].
*/
void FrontendSessionList_DecrementExpiryAndCompactRows(void *frontendRuntime)

{
  UiTransferPayloadByteCount *pUVar1;
  FrontendSessionDiscoveryRecordB0 *pFVar2;
  FrontendSessionDiscoveryRecordB0 *pFVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  FrontendSessionDiscoveryRecordB0 **ppFVar7;
  dword *sourceDwordCursor;
  dword *destinationDwordCursor;
  
  uVar6 = *(uint *)((int)frontendRuntime + 0x4bb8);
  pFVar2 = g_FrontendSessionDiscoveryRecords;
  pFVar3 = g_FrontendSessionDiscoveryRecords;
  ppFVar7 = g_FrontendSessionListRows;
  for (iVar4 = *(int *)((int)frontendRuntime + 0x4bbc); iVar4 != 0; iVar4 = iVar4 + -1) {
    pUVar1 = &(pFVar2->advertisement).payloadByteCount;
    *pUVar1 = *pUVar1 - 1;
    destinationDwordCursor = (dword *)pFVar3;
    if (*pUVar1 == 0) {
      sourceDwordCursor = (dword *)(pFVar2 + 1);
      *(int *)((int)frontendRuntime + 0x4bbc) = *(int *)((int)frontendRuntime + 0x4bbc) + -1;
      if (uVar6 == *(uint *)((int)frontendRuntime + 0x4bc8)) {
        *(undefined4 *)((int)frontendRuntime + 0x4bc8) =
             *(undefined4 *)((int)frontendRuntime + 0x4bb8);
      }
      else if (uVar6 <= *(uint *)((int)frontendRuntime + 0x4bc8)) {
        *(int *)((int)frontendRuntime + 0x4bc8) = *(int *)((int)frontendRuntime + 0x4bc8) + -4;
      }
    }
    else {
      sourceDwordCursor = (dword *)(pFVar2 + 1);
      *ppFVar7 = pFVar3;
      destinationDwordCursor = (dword *)(pFVar3 + 1);
      ppFVar7 = ppFVar7 + 1;
      if (destinationDwordCursor != sourceDwordCursor) {
        sourceDwordCursor = (dword *)pFVar2;
        destinationDwordCursor = (dword *)pFVar3;
        for (iVar5 = 0x2c; iVar5 != 0; iVar5 = iVar5 + -1) {
          *destinationDwordCursor = *sourceDwordCursor;
          sourceDwordCursor = sourceDwordCursor + 1;
          destinationDwordCursor = destinationDwordCursor + 1;
        }
      }
    }
    uVar6 = uVar6 + 4;
    pFVar2 = (FrontendSessionDiscoveryRecordB0 *)sourceDwordCursor;
    pFVar3 = (FrontendSessionDiscoveryRecordB0 *)destinationDwordCursor;
  }
  UiPointerList_RefreshSelectionAndQueueAction
            ((UiPointerListControl *)((int)frontendRuntime + 0x4b68));
  return;
}

/* Address: 0x00565670.
   Ownership: ui/frontend/session.
   Purpose: Periodic frontend/session callback recovered from a stale data block. It advances network or local
   simulation timing and increments g_EndMoviePendingTicks when ending-movie playback is active. The broader
   session behavior is documented without forcing a movie-only name.
   Cross-module calls: FrontendTransfer_DispatchStagedCommandRecords [network/protocol/transfer],
   UiRuntimeRecordRing_DiscardOldestCf [ui/core/runtime], FrontendTransfer_HandleSyncRequest10021AndReply10023
   [network/protocol/transfer], FrontendTransfer_BroadcastPendingCommandBatchAndSyncState
   [network/protocol/transfer], UiRuntimeRecordRing_ContainsIdCf [ui/core/runtime],
   FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf [network/backend/runtime].
*/
void FrontendSession_PeriodicTick(void)

{
  uint uVar1;
  InGameRuntimeRootImageC3E4 *pIVar2;
  undefined1 in_CF;
  undefined1 uVar3;
  bool bVar4;
  undefined8 uVar5;
  
  (*g_SpinLockTryAcquire)(&g_InGameStateTickSpinLock);
  pIVar2 = g_InGameRuntimeRoot;
  if ((bool)in_CF) {
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_InGameNetworkTickCountdown != 0)
    goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        FrontendTransfer_DispatchStagedCommandRecords();
      }
      else {
        uVar1 = (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval) * 2;
        uVar3 = uVar1 < g_SessionNetworkTickInterval;
        if (uVar1 == g_SessionNetworkTickInterval) {
          while( true ) {
            uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
            if ((bool)uVar3) break;
            FrontendTransfer_HandleSyncRequest10021AndReply10023
                      ((NetworkSessionContext *)((ulonglong)uVar5 >> 0x20),
                       (FrontendTransferPacketUnion *)uVar5);
          }
          FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
          if ((bool)uVar3) {
            g_InGameNetworkTickCountdown = 1;
            goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
          }
        }
      }
    }
  }
  else {
    bVar4 = false;
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
      uVar3 = 1;
      if (!bVar4) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
      do {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        if ((bool)uVar3) break;
        FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
                  ((NetworkSessionContext *)((ulonglong)uVar5 >> 0x20),
                   (FrontendTransferPacketUnion *)uVar5);
      } while (!(bool)uVar3);
      FrontendTransfer_ConsumeProcessedFlagCf();
      if ((bool)uVar3) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    }
    else if (g_InGameNetworkTickCountdown != 0)
    goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
  }
  g_SessionNetworkTickCounter = g_SessionNetworkTickCounter + 1;
  if (((g_UiCommandRuntimeFlags & 0x800) != 0) &&
     (pIVar2->activeEndMovieRuntime022C != (MovieRuntime *)0x0)) {
    g_EndMoviePendingTicks = g_EndMoviePendingTicks + 1;
  }
FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn:
  (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
  return;
}

/* Address: 0x005725D0.
   Ownership: ui/frontend/session.
   Purpose: Compacts player blocks and paired command slots after timeout, then sends
   g_FrontendClientPlayerRemovalPacket10007 for each removed player token.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8 [ui/ingame/runtime],
   UiTransfer_StagePacketAndSendCf [network/protocol/transfer],
   FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus [ui/frontend/player].
*/
void FrontendClientSession_DecrementTimeoutsAndCompactPlayers(void)

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
  undefined8 uVar11;
  FrontendCommandPacketRecord *pFVar12;
  FrontendPlayerRemovalPacket10007 *pFVar13;
  FrontendCommandPacketRecord *pFStack_20;
  
  iVar5 = 0;
  pFStack_20 = g_FrontendClientPlayerCommandRecords;
  pFVar12 = g_FrontendClientPlayerCommandRecords;
  iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
  iVar3 = iVar4;
  pFVar6 = g_FrontendPlayerRuntimeBlocks + 1;
  pFVar9 = g_FrontendPlayerRuntimeBlocks + 1;
  pFVar13 = (FrontendPlayerRemovalPacket10007 *)pFVar12;
  if (iVar4 != 0 && 0 < (int)g_FrontendPlayerRuntimeBlockCount) {
    do {
      if (pFVar6->heartbeatExpiryTicks == 0) {
FrontendClientSession_DecrementTimeoutsAndCompactPlayers_RemoveExpiredPlayerAndPrepareCompaction:
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        pFVar12 = (FrontendCommandPacketRecord *)pFVar6->playerRuntimeId;
        iVar5 = iVar5 + 1;
        pFVar8 = pFVar6 + 1;
        pFVar10 = pFVar9;
      }
      else {
        pFVar1 = &pFVar6->heartbeatExpiryTicks;
        *pFVar1 = *pFVar1 - 1;
        if (*pFVar1 == 0) {
          stream = TextResource_Resolve(0xff00);
          RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
          InGameRecentTextHistory_InsertAndRebuild8();
          iVar4 = extraout_EDX;
          goto 
          FrontendClientSession_DecrementTimeoutsAndCompactPlayers_RemoveExpiredPlayerAndPrepareCompaction
          ;
        }
        pFVar8 = pFVar6 + 1;
        pFVar10 = pFVar9 + 1;
        pFVar12 = (FrontendCommandPacketRecord *)(pFVar13 + 1);
        iVar3 = 0x4ec;
        if (pFVar10 != pFVar8) {
          for (; pFVar10 = pFVar9, pFVar8 = pFVar6, iVar3 != 0; iVar3 = iVar3 + -1) {
            pFVar10->runtimeState00 = pFVar8->runtimeState00;
            pFVar6 = (FrontendPlayerRuntimeRecord *)&pFVar8->peerSequenceToken;
            pFVar9 = (FrontendPlayerRuntimeRecord *)&pFVar10->peerSequenceToken;
          }
          pFVar7 = pFStack_20;
          for (iVar3 = 8; iVar3 != 0; iVar3 = iVar3 + -1) {
            (pFVar13->header).packedTypeAndUnitCount = (pFVar7->header).packedTypeAndUnitCount;
            pFVar7 = (FrontendCommandPacketRecord *)&(pFVar7->header).sequenceToken;
            pFVar13 = (FrontendPlayerRemovalPacket10007 *)&(pFVar13->header).sequenceToken;
          }
        }
      }
      pFStack_20 = pFStack_20 + 1;
      iVar4 = iVar4 + -1;
      iVar3 = 0;
      pFVar6 = pFVar8;
      pFVar9 = pFVar10;
      pFVar13 = (FrontendPlayerRemovalPacket10007 *)pFVar12;
    } while (iVar4 != 0);
  }
  if (iVar5 != 0) {
    do {
      g_FrontendClientPlayerRemovalPacket10007.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_10007_PLAYER_REMOVAL;
      endpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
      g_FrontendClientPlayerRemovalPacket10007.removedPlayerToken = (FrontendPlayerRuntimeId)pFVar12
      ;
      FVar2 = g_FrontendPlayerRuntimeBlockCount;
      while (FVar2 != 1) {
        pFVar13 = &g_FrontendClientPlayerRemovalPacket10007;
        uVar11 = UiTransfer_StagePacketAndSendCf
                           (endpoint,&g_FrontendClientPlayerRemovalPacket10007.header);
        iVar3 = (int)((ulonglong)uVar11 >> 0x20);
        endpoint = endpoint + 0x13b;
        pFVar12 = (FrontendCommandPacketRecord *)pFVar13;
        FVar2 = extraout_ECX;
      }
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(0,iVar3,0xffffffff);
  }
  return;
}

/* Address: 0x00572960.
   Ownership: ui/frontend/session.
   Purpose: Ticks the host-session transition countdown, clears role and session flags on expiry, posts the
   localized transition text, and selects shutdown or ready-consensus handling.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8 [ui/ingame/runtime],
   FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void FrontendHostSession_TickShutdownOrReadyConsensus(void)

{
  PlayerRuntimeId PVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  word *stream;
  void *replacementPayload;
  FrontendPlayerRuntimeBlockCount FVar4;
  undefined4 extraout_EDX;
  FrontendPlayerRuntimeRecord *pFVar5;
  
  pIVar3 = g_InGameRuntimeRoot;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    (*g_NetworkBackendSlot3)();
    (*g_NetworkBackendSlot1)();
    stream = TextResource_Resolve(0xff01);
    RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
    InGameRecentTextHistory_InsertAndRebuild8();
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    PVar1 = g_LocalPlayerRuntimeId;
    FVar4 = g_FrontendPlayerRuntimeBlockCount;
    do {
      if ((pFVar5->factionAssignment).readyOrWaitState == 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                    (FVar4,extraout_EDX,g_LocalPlayerRuntimeId);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
        }
        pFVar5 = g_FrontendPlayerRuntimeBlocks;
        PVar1 = g_LocalPlayerRuntimeId;
        g_FrontendPlayerRuntimeBlockCount = 1;
        LOCK();
        g_LocalPlayerRuntimeId = 0;
        UNLOCK();
        (pIVar3->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
        g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerRuntimeBlockPointers[PVar1];
        (pFVar5->playerName).textUtf16[0] = 0;
        (pFVar5->playerName).textUtf16[1] = 0;
        pFVar5->playerRuntimeId = 0;
        (pFVar5->factionAssignment).roleStateFlags = 0;
        return;
      }
      pFVar5 = pFVar5 + 1;
      FVar4 = FVar4 - 1;
    } while (FVar4 != 0);
    g_FrontendPlayerRuntimeBlockCount = 1;
    LOCK();
    g_LocalPlayerRuntimeId = 0;
    UNLOCK();
    (pIVar3->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
    g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerRuntimeBlockPointers[PVar1];
    (pFVar2->playerName).textUtf16[0] = 0;
    (pFVar2->playerName).textUtf16[1] = 0;
    pFVar2->playerRuntimeId = 0;
    (pFVar2->factionAssignment).roleStateFlags = 0;
  }
  return;
}

/* Address: 0x00544210.
   Ownership: ui/frontend/session.
   Purpose: Closes the active movie, converts the frontend game-speed percentage at +0xA80 to the runtime Q8 value
   using multiplier 0x28F5C and shift 16, sets runtime flag 0x08 at +0x8FC, then returns to the main frontend page
   while forwarding the callback state code. EAX is preserved. Typed parameters: p3
   stateCode→FrontendStatusCode_V306. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: Movie_Close [movie/runtime/playback].
*/
undefined4
FrontendSession_ApplyGameSpeedAndReturnToMainPage
          (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3,
          FrontendStatusCode stateCode)

{
  uint *puVar1;
  int iVar2;
  undefined4 in_EAX;
  
  iVar2 = g_FrontendRootNode;
  Movie_Close();
  g_GameFactionRuntimeImage.tail.gameSpeedQ8 = (uint)(*(int *)(iVar2 + 0xa80) * 0x28f5c) >> 0x10;
  puVar1 = (uint *)(iVar2 + 0x8fc);
  *puVar1 = *puVar1 | 8;
  FrontendSession_ReturnToMainPage(callbackContext,0,0,stateCode);
  return in_EAX;
}

/* Address: 0x00544D10.
   Ownership: ui/frontend/session.
   Purpose: Selects frontend page index 0, clears compact-layout flag 0x2000 from runtime dword +0x3B4, and
   dispatches the fourth callback argument through the established frontend state service. EAX is preserved. Typed
   parameters: p3 stateCode→FrontendStatusCode_V306. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], FrontendState_DispatchCode
   [ui/frontend/runtime].
*/
undefined4
FrontendSession_ReturnToMainPage
          (dword callbackContext,dword argument2,dword argument3,FrontendStatusCode stateCode)

{
  undefined4 in_EAX;
  int frontendRootAddress;
  
  frontendRootAddress = g_FrontendRootNode;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(g_FrontendRootNode + 0x508));
  *(uint *)(frontendRootAddress + 0x3b4) = *(uint *)(frontendRootAddress + 0x3b4) & 0xffffdfff;
  FrontendState_DispatchCode(stateCode);
  return in_EAX;
}
