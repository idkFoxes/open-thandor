/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/session.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/session.h>
#include <thandor/thandor.h>

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
          (FrontendReturnCallbackContext32 callbackContext,uint32_t argument2,uint32_t argument3,
          uint32_t argument4)

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
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(source,frontendViewModeStack));
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage
            (FRONTEND_UI_FIELD(source,moviePlaybackView,0x50,GraphicsTextureSourceAsset *));
  g_MemoryApi.free(FRONTEND_UI_FIELD(source,moviePlaybackView,0x60,void *));
  g_MemoryApi.free(FRONTEND_UI_FIELD(source,moviePlaybackView,0x64,void *));
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x50,GraphicsTextureSourceAsset *) = (GraphicsTextureSourceAsset *)0x0;
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x60,void *) = (void *)0x0;
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x64,void *) = (void *)0x0;
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
  g_NetworkBackendSlot3();
  g_FrontendNetworkState = 0;
  g_NetworkBackendSlot1();
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
  UiNodeBase *parentNode;
  uint32_t seed;
  int recordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  
  parentNode = source->parent;
  while (parentNode != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentNode = source->parent;
  }
  g_FrontendPlayerRuntimeBlockCount = ((UiListControl *)FRONTEND_UI(source,hostLobbyPlayerList))->rowCount;
  g_FrontendExpectedPlayerRuntimeBlockCount = 0;
  g_FrontendPendingSessionPlayerCount = g_FrontendPlayerRuntimeBlockCount;
  seed = Random_NextPrimary();
  Random_SetBothSeeds(seed);
  Random_SelectSecondaryStream();
  recordsRemaining = 8;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    (playerRecordCursor->factionAssignment).roleStateFlags = 0;
    playerRecordCursor->runtimeState64 = 0;
    playerRecordCursor = playerRecordCursor + 1;
    recordsRemaining = recordsRemaining + -1;
  } while (recordsRemaining != 0);
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
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,GameSpeedPercent gameSpeedPercent)

{
  FRONTEND_UI_FIELD(g_FrontendRootNode,gameSpeedSlider,0x58,GameSpeedPercent) = gameSpeedPercent;
  return;
}


/* Address: 0x0054A790.
   Opens the "Exit programme" confirmation page (FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE from the main menu). At 640 pixels
   width or less the page covers the menu room, so the room's 3D rendering is switched off.
*/
void __thandor_preserve_eax FrontendSession_ShowQuitConfirmPage(FrontendUiImage *frontendUi)

{
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_QUIT_CONFIRM,
                             (UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,uint32_t) |= FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
}


/* Address: 0x0054D2E0.
   Ownership: ui/frontend/session.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[10]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[10] (0x200A). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiTransferMailbox_RandomizeSequenceToken [network/protocol/transfer], UiTransfer_SendPacketType10000Value2931
   [network/protocol/transfer].
*/
void __thandor_preserve_eax
FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  /* source is the frontend template's clientLobbyLeaveButton (+0x5784). */
  FrontendUiImage *frontendUi;

  g_FrontendNetworkState = 1;
  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(source,-0x5784);
  UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < 0x281) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) | 0x2000;
  }
  UiNodeList_SuppressActionId(0x2002,FRONTEND_UI(frontendUi,frontendRoot));
  UiPointerList_InitializeColumnLayout
            (0,g_FrontendSessionListRows,(UiPointerListControl *)FRONTEND_UI(frontendUi,sessionList));
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  UiTransferMailbox_RandomizeSequenceToken();
  UiTransfer_SendPacketType10000Value2931();
  firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  (localPlayerRecord->playerName).textUtf16[0] = 0;
  (localPlayerRecord->playerName).textUtf16[1] = 0;
  firstPlayerRecord->playerRuntimeId = 0;
  (firstPlayerRecord->factionAssignment).roleStateFlags = 0;
  firstPlayerRecord->runtimeState64 = 0;
  firstPlayerRecord->snapshotTransferFlags = 0;
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
  UiTransferPayloadByteCount *expiryTicks;
  UiListRowCount *rowCountField;
  void ***selectedSlotField;
  void **currentSelectedSlot;
  FrontendSessionDiscoveryRecordB0 *sourceRecord;
  FrontendSessionDiscoveryRecordB0 *destinationRecord;
  UiListRowCount rowsRemaining;
  int dwordsRemaining;
  void **rowSlotCursor;
  FrontendSessionDiscoveryRecordB0 **rowPointerCursor;
  uint32_t *sourceDwordCursor;
  uint32_t *destinationDwordCursor;
  
  rowSlotCursor = (frontendRuntime->sessionDiscoveryList).rowSlots;
  sourceRecord = g_FrontendSessionDiscoveryRecords;
  destinationRecord = g_FrontendSessionDiscoveryRecords;
  rowPointerCursor = g_FrontendSessionListRows;
  for (rowsRemaining = (frontendRuntime->sessionDiscoveryList).rowCount; rowsRemaining != 0; rowsRemaining = rowsRemaining - 1) {
    expiryTicks = &(sourceRecord->advertisement).payloadByteCount;
    *expiryTicks = *expiryTicks - 1;
    destinationDwordCursor = (uint32_t *)destinationRecord;
    if (*expiryTicks == 0) {
      sourceDwordCursor = (uint32_t *)(sourceRecord + 1);
      rowCountField = &(frontendRuntime->sessionDiscoveryList).rowCount;
      *rowCountField = *rowCountField - 1;
      currentSelectedSlot = (frontendRuntime->sessionDiscoveryList).selectedRowSlot;
      if (rowSlotCursor == currentSelectedSlot) {
        (frontendRuntime->sessionDiscoveryList).selectedRowSlot =
             (frontendRuntime->sessionDiscoveryList).rowSlots;
      }
      else if (rowSlotCursor <= currentSelectedSlot) {
        selectedSlotField = &(frontendRuntime->sessionDiscoveryList).selectedRowSlot;
        *selectedSlotField = *selectedSlotField + -1;
      }
    }
    else {
      sourceDwordCursor = (uint32_t *)(sourceRecord + 1);
      *rowPointerCursor = destinationRecord;
      destinationDwordCursor = (uint32_t *)(destinationRecord + 1);
      rowPointerCursor = rowPointerCursor + 1;
      if (destinationDwordCursor != sourceDwordCursor) {
        sourceDwordCursor = (uint32_t *)sourceRecord;
        destinationDwordCursor = (uint32_t *)destinationRecord;
        for (dwordsRemaining = 0x2c; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
          *destinationDwordCursor = *sourceDwordCursor;
          sourceDwordCursor = sourceDwordCursor + 1;
          destinationDwordCursor = destinationDwordCursor + 1;
        }
      }
    }
    rowSlotCursor = rowSlotCursor + 1;
    sourceRecord = (FrontendSessionDiscoveryRecordB0 *)sourceDwordCursor;
    destinationRecord = (FrontendSessionDiscoveryRecordB0 *)destinationDwordCursor;
  }
  UiPointerList_RefreshSelectionAndQueueAction(&frontendRuntime->sessionDiscoveryList);
  return;
}


/* Address: 0x00565670.
   Synchronization hook and movie-rate timer while the end movie plays (installed by
   InGameRuntime_RunSessionUntilExit and Frontend_PlaySelectedEndMovie): keeps the network lockstep of
   InGameRuntime_UpdateSimulationAndNetworkTick running without simulating, so peers do not time out, and counts
   one due movie frame in g_EndMoviePendingTicks per step. The host executes the staged command batch at each
   interval boundary and broadcasts the next one half an interval later; a client waits for the host's batch.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendSession_PeriodicTick(void)

{
  InGameRuntimeRootImageC3E4 *inGameRoot;
  bool callResult;
  RecordRingDiscardResult ringRecord;
  
  callResult = g_SpinLockTryAcquire(&g_InGameStateTickSpinLock);
  inGameRoot = g_InGameRuntimeRoot;
  if (callResult) {
    return;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_InGameNetworkTickCountdown != 0)
    goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        FrontendTransfer_DispatchStagedCommandRecords();
      }
      else if ((g_SessionNetworkTickCounter % g_SessionNetworkTickInterval) * 2 ==
               g_SessionNetworkTickInterval) {
        while( true ) {
          ringRecord = UiRuntimeRecordRing_DiscardOldest();
          if (ringRecord.empty) break;
          FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                    ((NetworkSessionContext *)ringRecord.endpointOrReadIndex,
                     (FrontendTransferPacketUnion *)ringRecord.payloadOrReadIndex);
        }
        callResult = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
        if (callResult) {
          /* not every peer has synced yet: retry on the next timer tick */
          g_InGameNetworkTickCountdown = 1;
          goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
        }
      }
    }
  }
  else {
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      /* client at an interval boundary: wait until the host's command batch has arrived */
      callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
      if (!callResult) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
      do {
        ringRecord = UiRuntimeRecordRing_DiscardOldest();
        if (ringRecord.empty) break;
        callResult = FrontendNetwork_HandleCommandBatchAndPlayerTimeout
                          ((NetworkSessionContext *)ringRecord.endpointOrReadIndex,
                           (FrontendTransferPacketUnion *)ringRecord.payloadOrReadIndex);
      } while (!callResult);
      callResult = FrontendTransfer_ConsumeProcessedFlag();
      if (callResult) goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    }
    else if (g_InGameNetworkTickCountdown != 0)
    goto FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn;
    g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  }
  g_SessionNetworkTickCounter++;
  if (((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0) &&
     (inGameRoot->activeEndMovieRuntime022C != NULL)) {
    g_EndMoviePendingTicks++;
  }
FrontendSession_PeriodicTick_ReleaseStateTickLockAndReturn:
  g_SpinLockRelease(&g_InGameStateTickSpinLock);
  return;
}


/* Address: 0x005725D0.
   In-game tick on the HOST (despite the name): counts down every client's heartbeat timeout, drops clients that
   ran out (a notice with the player's name is posted) and compacts the player blocks and their command records,
   then tells the remaining clients about each dropped player with a 0x10007 packet and re-evaluates the ready
   consensus.
   NOTE: this C differs from the original. The original pushes every dropped player id on the stack (one 0x10007
   per id) and keeps the command-record destination in a separate slot; here removedIdOrCommandCursor holds the
   last dropped id and is then copied into destinationCommandOrPacket, so a surviving client after a dropped one
   gets its command record written to the address "player id", and several drops send the same/garbage id.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendClientSession_DecrementTimeoutsAndCompactPlayers(void)

{
  FrontendHeartbeatTickCount *heartbeatTicks;
  FrontendPlayerRuntimeBlockCount recipientsRemaining;
  int copyCount;
  int playersRemaining;
  int removedCount;
  FrontendPlayerRuntimeRecord *sourcePlayer;
  FrontendCommandPacketRecord *commandCopyCursor;
  FrontendPlayerRuntimeRecord *nextSourcePlayer;
  UiTransferEndpointDescriptor *endpoint;
  FrontendPlayerRuntimeRecord *destinationPlayer;
  FrontendPlayerRuntimeRecord *nextDestinationPlayer;
  TextResolveResult timeoutText;
  FrontendCommandPacketRecord *removedIdOrCommandCursor;
  FrontendPlayerRemovalPacket10007 *destinationCommandOrPacket;
  FrontendCommandPacketRecord *sourceCommandRecord;
  
  removedCount = 0;
  sourceCommandRecord = g_FrontendClientPlayerCommandRecords;
  removedIdOrCommandCursor = g_FrontendClientPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount - 1;
  sourcePlayer = g_FrontendPlayerRuntimeBlocks + 1;
  destinationPlayer = g_FrontendPlayerRuntimeBlocks + 1;
  destinationCommandOrPacket = (FrontendPlayerRemovalPacket10007 *)removedIdOrCommandCursor;
  if (playersRemaining != 0 && 0 < (int)g_FrontendPlayerRuntimeBlockCount) {
    do {
      if (sourcePlayer->heartbeatExpiryTicks == 0) {
FrontendClientSession_RemoveExpiredPlayer:
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        removedIdOrCommandCursor = (FrontendCommandPacketRecord *)sourcePlayer->playerRuntimeId;
        removedCount = removedCount + 1;
        nextSourcePlayer = sourcePlayer + 1;
        nextDestinationPlayer = destinationPlayer;
      }
      else {
        heartbeatTicks = &sourcePlayer->heartbeatExpiryTicks;
        *heartbeatTicks = *heartbeatTicks - 1;
        if (*heartbeatTicks == 0) {
          timeoutText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
          RichTextCommandStream_PatchPayloadBySelector(0,&sourcePlayer->playerName,timeoutText.text);
          InGameRecentTextHistory_InsertAndRebuild8(timeoutText.text);
          goto FrontendClientSession_RemoveExpiredPlayer;
        }
        nextSourcePlayer = sourcePlayer + 1;
        nextDestinationPlayer = destinationPlayer + 1;
        removedIdOrCommandCursor = (FrontendCommandPacketRecord *)(destinationCommandOrPacket + 1);
        copyCount = sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t);
        if (nextDestinationPlayer != nextSourcePlayer) {
          for (; nextDestinationPlayer = destinationPlayer, nextSourcePlayer = sourcePlayer, copyCount != 0; copyCount--) {
            nextDestinationPlayer->runtimeState00 = nextSourcePlayer->runtimeState00;
            sourcePlayer = (FrontendPlayerRuntimeRecord *)&nextSourcePlayer->peerSequenceToken;
            destinationPlayer = (FrontendPlayerRuntimeRecord *)&nextDestinationPlayer->peerSequenceToken;
          }
          commandCopyCursor = sourceCommandRecord;
          for (copyCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); copyCount != 0; copyCount--) {
            (destinationCommandOrPacket->header).packedTypeAndUnitCount = (commandCopyCursor->header).packedTypeAndUnitCount;
            commandCopyCursor = (FrontendCommandPacketRecord *)&(commandCopyCursor->header).sequenceToken;
            destinationCommandOrPacket = (FrontendPlayerRemovalPacket10007 *)&(destinationCommandOrPacket->header).sequenceToken;
          }
        }
      }
      sourceCommandRecord = sourceCommandRecord + 1;
      playersRemaining--;
      sourcePlayer = nextSourcePlayer;
      destinationPlayer = nextDestinationPlayer;
      destinationCommandOrPacket = (FrontendPlayerRemovalPacket10007 *)removedIdOrCommandCursor;
    } while (playersRemaining != 0);
  }
  if (removedCount != 0) {
    do {
      g_FrontendClientPlayerRemovalPacket10007.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_10007_PLAYER_REMOVAL;
      endpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
      g_FrontendClientPlayerRemovalPacket10007.removedPlayerToken = (FrontendPlayerRuntimeId)removedIdOrCommandCursor
      ;
      recipientsRemaining = g_FrontendPlayerRuntimeBlockCount;
      while (recipientsRemaining = recipientsRemaining - 1, recipientsRemaining != 0) {
        destinationCommandOrPacket = &g_FrontendClientPlayerRemovalPacket10007;
        UiTransfer_StagePacketAndSend(endpoint,&g_FrontendClientPlayerRemovalPacket10007.header);
        endpoint = endpoint + sizeof(FrontendPlayerRuntimeRecord) / sizeof(UiTransferEndpointDescriptor);
        removedIdOrCommandCursor = (FrontendCommandPacketRecord *)destinationCommandOrPacket;
      }
      removedCount--;
    } while (removedCount != 0);
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(0xffffffff,0,0,0); /* no player: only re-check */
  }
  return;
}


/* Address: 0x00572960.
   In-game tick on a CLIENT (despite the name): counts down the host timeout. When it runs out the session falls
   back to a local game: the network role is cleared, the socket closed, TEXT_ID_NETWORK_HOST_LOST posted, a
   pending ready vote is submitted if some player has not voted yet, and the local player becomes the only
   player, with id 0.
*/
void __thandor_void_preserve_eax_ecx FrontendHostSession_TickShutdownOrReadyConsensus(void)

{
  PlayerRuntimeId previousLocalPlayerId;
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResolveResult shutdownText;
  
  inGameRoot = g_InGameRuntimeRoot;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    g_NetworkBackendSlot3(); /* close */
    g_NetworkBackendSlot1(); /* cleanup */
    shutdownText = TextResource_Resolve(TEXT_ID_NETWORK_HOST_LOST);
    RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,shutdownText.text);
    InGameRecentTextHistory_InsertAndRebuild8(shutdownText.text);
    firstPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    previousLocalPlayerId = g_LocalPlayerRuntimeId;
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    do {
      if ((playerRecord->factionAssignment).readyOrWaitState == 0) {
        /* always true here, the role was cleared above */
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
        }
        playerRecord = g_FrontendPlayerRuntimeBlocks;
        previousLocalPlayerId = g_LocalPlayerRuntimeId;
        g_FrontendPlayerRuntimeBlockCount = 1;
        LOCK();
        g_LocalPlayerRuntimeId = 0;
        UNLOCK();
        (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
        g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerRuntimeBlockPointers[previousLocalPlayerId];
        (playerRecord->playerName).textUtf16[0] = 0;
        (playerRecord->playerName).textUtf16[1] = 0;
        playerRecord->playerRuntimeId = 0;
        (playerRecord->factionAssignment).roleStateFlags = 0;
        return;
      }
      playerRecord++;
      playersRemaining--;
    } while (playersRemaining != 0);
    g_FrontendPlayerRuntimeBlockCount = 1;
    LOCK();
    g_LocalPlayerRuntimeId = 0;
    UNLOCK();
    (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
    g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerRuntimeBlockPointers[previousLocalPlayerId];
    (firstPlayerRecord->playerName).textUtf16[0] = 0;
    (firstPlayerRecord->playerName).textUtf16[1] = 0;
    firstPlayerRecord->playerRuntimeId = 0;
    (firstPlayerRecord->factionAssignment).roleStateFlags = 0;
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
          (FrontendReturnCallbackContext32 callbackContext,uint32_t argument2,uint32_t argument3,
          FrontendStatusCode stateCode)

{
  uint32_t *runtimeFlags;
  int frontendRootAddress;
  
  frontendRootAddress = g_FrontendRootNode;
  Movie_Close();
  g_GameFactionRuntimeImage.tail.gameSpeedQ8 =
       (uint32_t)(FRONTEND_UI_FIELD(frontendRootAddress,gameSpeedSlider,0x58,int) * 0x28f5c) >> 0x10;
  runtimeFlags = &((UiImageActionControl *)FRONTEND_UI(frontendRootAddress,briefingImage))->displayFlags;
  *runtimeFlags = *runtimeFlags | 8;
  FrontendSession_ReturnToMainPage(callbackContext,0,0,stateCode);
  return;
}


/* Address: 0x00544D10.
   Closes the dialog pages (back to FRONTEND_PAGE_MAIN, the menu room renders again) and runs entry
   romActionIndex of the frontend ROM action table (FrontendState_DispatchCode ->
   FrontendRomActionTable_ExecuteRecord). Command handler with four dword arguments (frontend command 0xDC0 in a
   network game); the player id and the two middle arguments are not used.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReturnToMainPage
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex)

{
  int frontendRootAddress;

  frontendRootAddress = g_FrontendRootNode;
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(g_FrontendRootNode,frontendPageStack));
  FRONTEND_UI_FIELD(frontendRootAddress,menuRoomModelView,0x4C,uint32_t) &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  FrontendState_DispatchCode(romActionIndex);
  return;
}

