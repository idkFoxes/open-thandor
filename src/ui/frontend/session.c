/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/session.c
 * Reverse engineering by idkFoxes 2026
 */

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
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReleaseSelectedResourceAndReturnToMainPage
          (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3,
          dword argument4)

{
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  g_FrontendScenarioInitializationCount = 0;
  FrontendSession_ReturnToMainPage(callbackContext,0,0,2);
  return;
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
void __thandor_preserve_eax FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source)

{
  int parentNodeAddress;
  
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  Movie_Close();
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&source[1].vtable);
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
  return;
}


/* Address: 0x00549090.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[71]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[71] (0x2047). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendSession_ApplyGameSpeedAndReturnToMainPage.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08 [ui/frontend/player].
*/
void __thandor_preserve_eax FrontendSessionAction_ApplySpeedOrToggleReady(void *source)

{
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
  return;
}


/* Address: 0x0054C770.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[0]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[0] (0x2000). Return datatype is preserved for non-queue direct callers.
   Local calls: FrontendSession_ReturnToMainPage.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands],
   Random_SelectPrimaryStream [core/math/random].
*/
void __thandor_preserve_eax FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source)

{
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
  return;
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
void __thandor_void_preserve_eax_ecx
FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source)

{
  UiNodeBase *pUVar1;
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
  return;
}


/* Address: 0x00544250.
   Ownership: ui/frontend/session.
   Purpose: Stores the fourth callback argument into the active frontend runtime dword at +0xA80. EAX is preserved.
   Typed parameters: p3 gameSpeedPercent→GameSpeedPercent_V305. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_SetGameSpeedPercent
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
void __thandor_preserve_eax FrontendSession_ShowPage9WithCompactLayout(void *frontendRuntime)

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
void __thandor_preserve_eax
FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *pFVar1;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  
  g_FrontendNetworkState = 1;
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)&source[-0x116].vtable);
  if ((int)g_FramebufferWidth < 0x281) {
    source[-0x11b].topAnchorQ31 = source[-0x11b].topAnchorQ31 | 0x2000;
  }
  UiNodeList_SuppressActionId(0x2002,(UiNodeBase *)&source[-0x127].left);
  UiPointerList_InitializeColumnLayout
            (0,g_FrontendSessionListRows,(UiPointerListControl *)&source[-0x29].left);
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
  return;
}


/* Address: 0x0054E3A0.
   Ownership: ui/frontend/session.
   Purpose: Ticks the frontend session-list expiry values, compacts expired 0xB0-byte rows, repairs selection
   offsets, and refreshes the associated pointer list.
   Cross-module calls: UiPointerList_RefreshSelectionAndQueueAction [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSessionList_DecrementExpiryAndCompactRows
          (FrontendNetworkListsRuntimeView5650 *frontendRuntime)

{
  UiTransferPayloadByteCount *pUVar1;
  UiListRowCount *pUVar2;
  void ***pppvVar3;
  void **ppvVar4;
  FrontendSessionDiscoveryRecordB0 *pFVar5;
  FrontendSessionDiscoveryRecordB0 *pFVar6;
  UiListRowCount UVar7;
  int iVar8;
  void **ppvVar9;
  FrontendSessionDiscoveryRecordB0 **ppFVar10;
  dword *sourceDwordCursor;
  dword *destinationDwordCursor;
  
  ppvVar9 = (frontendRuntime->sessionDiscoveryList).rowSlots;
  pFVar5 = g_FrontendSessionDiscoveryRecords;
  pFVar6 = g_FrontendSessionDiscoveryRecords;
  ppFVar10 = g_FrontendSessionListRows;
  for (UVar7 = (frontendRuntime->sessionDiscoveryList).rowCount; UVar7 != 0; UVar7 = UVar7 - 1) {
    pUVar1 = &(pFVar5->advertisement).payloadByteCount;
    *pUVar1 = *pUVar1 - 1;
    destinationDwordCursor = (dword *)pFVar6;
    if (*pUVar1 == 0) {
      sourceDwordCursor = (dword *)(pFVar5 + 1);
      pUVar2 = &(frontendRuntime->sessionDiscoveryList).rowCount;
      *pUVar2 = *pUVar2 - 1;
      ppvVar4 = (frontendRuntime->sessionDiscoveryList).selectedRowSlot;
      if (ppvVar9 == ppvVar4) {
        (frontendRuntime->sessionDiscoveryList).selectedRowSlot =
             (frontendRuntime->sessionDiscoveryList).rowSlots;
      }
      else if (ppvVar9 <= ppvVar4) {
        pppvVar3 = &(frontendRuntime->sessionDiscoveryList).selectedRowSlot;
        *pppvVar3 = *pppvVar3 + -1;
      }
    }
    else {
      sourceDwordCursor = (dword *)(pFVar5 + 1);
      *ppFVar10 = pFVar6;
      destinationDwordCursor = (dword *)(pFVar6 + 1);
      ppFVar10 = ppFVar10 + 1;
      if (destinationDwordCursor != sourceDwordCursor) {
        sourceDwordCursor = (dword *)pFVar5;
        destinationDwordCursor = (dword *)pFVar6;
        for (iVar8 = 0x2c; iVar8 != 0; iVar8 = iVar8 + -1) {
          *destinationDwordCursor = *sourceDwordCursor;
          sourceDwordCursor = sourceDwordCursor + 1;
          destinationDwordCursor = destinationDwordCursor + 1;
        }
      }
    }
    ppvVar9 = ppvVar9 + 1;
    pFVar5 = (FrontendSessionDiscoveryRecordB0 *)sourceDwordCursor;
    pFVar6 = (FrontendSessionDiscoveryRecordB0 *)destinationDwordCursor;
  }
  UiPointerList_RefreshSelectionAndQueueAction(&frontendRuntime->sessionDiscoveryList);
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
void __thandor_void_preserve_eax_ecx_edx FrontendSession_PeriodicTick(void)

{
  InGameRuntimeRootImageC3E4 *pIVar1;
  bool bVar2;
  UiRuntimeRecordRingDiscardEaxEdxCf9 UVar3;
  
  bVar2 = (*g_SpinLockTryAcquire)(&g_InGameStateTickSpinLock);
  pIVar1 = g_InGameRuntimeRoot;
  if (bVar2) {
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
      else if ((g_SessionNetworkTickCounter % g_SessionNetworkTickInterval) * 2 ==
               g_SessionNetworkTickInterval) {
        while( true ) {
          UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
          if (UVar3.carryEmpty) break;
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)UVar3.edxEndpointOrReadIndex,
                     (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex);
        }
        bVar2 = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
        if (bVar2) {
          g_InGameNetworkTickCountdown = 1;
          goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
        }
      }
    }
  }
  else {
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      bVar2 = UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
      if (!bVar2) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
      do {
        UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
        if (UVar3.carryEmpty) break;
        bVar2 = FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
                          ((NetworkSessionContext *)UVar3.edxEndpointOrReadIndex,
                           (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex);
      } while (!bVar2);
      bVar2 = FrontendTransfer_ConsumeProcessedFlagCf();
      if (bVar2) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    }
    else if (g_InGameNetworkTickCountdown != 0)
    goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
  }
  g_SessionNetworkTickCounter = g_SessionNetworkTickCounter + 1;
  if (((g_UiCommandRuntimeFlags & 0x800) != 0) &&
     (pIVar1->activeEndMovieRuntime022C != (MovieRuntime *)0x0)) {
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
void __thandor_void_preserve_eax_ecx_edx
FrontendClientSession_DecrementTimeoutsAndCompactPlayers(void)

{
  FrontendHeartbeatTickCount *pFVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  FrontendPlayerRuntimeRecord *pFVar6;
  FrontendCommandPacketRecord *pFVar7;
  FrontendPlayerRuntimeRecord *pFVar8;
  UiTransferEndpointDescriptor *endpoint;
  FrontendPlayerRuntimeRecord *pFVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  TextResourceResolveEaxCf5 TVar11;
  FrontendCommandPacketRecord *pFVar12;
  FrontendPlayerRemovalPacket10007 *pFVar13;
  FrontendCommandPacketRecord *pFStack_20;
  
  iVar5 = 0;
  pFStack_20 = g_FrontendClientPlayerCommandRecords;
  pFVar12 = g_FrontendClientPlayerCommandRecords;
  iVar4 = g_FrontendPlayerRuntimeBlockCount - 1;
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
          TVar11 = TextResource_Resolve(0xff00);
          RichTextCommandStream_PatchPayloadBySelector(0,&pFVar6->playerName,TVar11.eax);
          InGameRecentTextHistory_InsertAndRebuild8(TVar11.eax);
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
      while (FVar2 = FVar2 - 1, FVar2 != 0) {
        pFVar13 = &g_FrontendClientPlayerRemovalPacket10007;
        UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendClientPlayerRemovalPacket10007.header);
        endpoint = endpoint + 0x13b;
        pFVar12 = (FrontendCommandPacketRecord *)pFVar13;
      }
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(0xffffffff,0,0,0);
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
void __thandor_void_preserve_eax_ecx FrontendHostSession_TickShutdownOrReadyConsensus(void)

{
  PlayerRuntimeId PVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  TextResourceResolveEaxCf5 TVar6;
  
  pIVar3 = g_InGameRuntimeRoot;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    (*g_NetworkBackendSlot3)();
    (*g_NetworkBackendSlot1)();
    TVar6 = TextResource_Resolve(0xff01);
    RichTextCommandStream_PatchPayloadBySelector(0,&pFVar5->playerName,TVar6.eax);
    InGameRecentTextHistory_InsertAndRebuild8(TVar6.eax);
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    PVar1 = g_LocalPlayerRuntimeId;
    FVar4 = g_FrontendPlayerRuntimeBlockCount;
    do {
      if ((pFVar5->factionAssignment).readyOrWaitState == 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0)
          ;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ApplyGameSpeedAndReturnToMainPage
          (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3,
          FrontendStatusCode stateCode)

{
  uint *puVar1;
  int iVar2;
  
  iVar2 = g_FrontendRootNode;
  Movie_Close();
  g_GameFactionRuntimeImage.tail.gameSpeedQ8 = (uint)(*(int *)(iVar2 + 0xa80) * 0x28f5c) >> 0x10;
  puVar1 = (uint *)(iVar2 + 0x8fc);
  *puVar1 = *puVar1 | 8;
  FrontendSession_ReturnToMainPage(callbackContext,0,0,stateCode);
  return;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReturnToMainPage
          (dword callbackContext,dword argument2,dword argument3,FrontendStatusCode stateCode)

{
  int frontendRootAddress;
  
  frontendRootAddress = g_FrontendRootNode;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(g_FrontendRootNode + 0x508));
  *(uint *)(frontendRootAddress + 0x3b4) = *(uint *)(frontendRootAddress + 0x3b4) & 0xffffdfff;
  FrontendState_DispatchCode(stateCode);
  return;
}

