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
   Handler of frontend command FRONTEND_COMMAND_RELEASE_CAMPAIGN (0x320): releases the loaded campaign asset,
   resets the scenario initialisation count and returns to the main page with ROM action record 2. Called
   directly by FrontendCallback_ReleaseSelectedResourceOrDispatch0320 in a local game, through the command queue
   in a network game. Only the player id is forwarded; the other three arguments are unused.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReleaseSelectedResourceAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3)

{
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = NULL;
  g_FrontendScenarioInitializationCount = 0;
  FrontendSession_ReturnToMainPage(playerRuntimeId,0,0,2);
  return;
}


/* Address: 0x00548FE0.
   Handler of action 0x2048 (slot 72 of g_FrontendUiActionHandlersPage20.handlers00_54), a click on the movie
   view: closes the playing movie, switches the view-mode stack back to the menu room, frees the movie's texture
   source and its two frame buffers, shows the pointer cursor again and returns to the main page (directly in a
   local game, as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a network game).
*/
void __thandor_preserve_eax FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source)

{
  int parentNodeAddress;

  /* climb to the frontend root */
  parentNodeAddress = (int)source->parent;
  while ((UiNodeBase *)parentNodeAddress != UI_NODE_NONE) {
    source = source->parent;
    parentNodeAddress = (int)source->parent;
  }
  Movie_Close();
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(source,frontendViewModeStack));
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage
            (FRONTEND_UI_FIELD(source,moviePlaybackView,0x50,GraphicsTextureSourceAsset *));
  g_MemoryApi.free(FRONTEND_UI_FIELD(source,moviePlaybackView,0x60,void *));
  g_MemoryApi.free(FRONTEND_UI_FIELD(source,moviePlaybackView,0x64,void *));
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x50,GraphicsTextureSourceAsset *) = NULL;
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x60,void *) = NULL;
  FRONTEND_UI_FIELD(source,moviePlaybackView,0x64,void *) = NULL;
  g_CursorVisibilityToken++;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}


/* Address: 0x00549090.
   Handler of action 0x2047 (slot 71 of g_FrontendUiActionHandlersPage20.handlers00_54), the mission briefing's
   "Begin" button. A host or local player applies the game-speed slider and leaves with ROM action record 1
   (FRONTEND_COMMAND_APPLY_GAME_SPEED in a network game); a client only reports that it is ready
   (FRONTEND_COMMAND_BRIEFING_READY).
*/
void __thandor_preserve_eax FrontendSessionAction_ApplySpeedOrToggleReady(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,1);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_APPLY_GAME_SPEED,0,0,1);
    }
  }
  /* the client branch repeats the network test although a client is always networked */
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_BRIEFING_READY,0,0,0);
  }
  return;
}


/* Address: 0x0054C770.
   Handler of action 0x2000 (slot 0 of g_FrontendUiActionHandlersPage20.handlers00_54), leaving the network
   game page: closes and cleans up the network backend, sets the frontend network state back to idle, returns
   to the main page and switches the random generator back to the primary stream.
*/
void __thandor_preserve_eax FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source)

{
  g_NetworkBackendSlot3(); /* close */
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  g_NetworkBackendSlot1(); /* cleanup */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  Random_SelectPrimaryStream();
  return;
}


/* Address: 0x0054D0B0.
   Handler of action FRONTEND_ACTION_START_NETWORK_GAME (0x2006, slot 6 of
   g_FrontendUiActionHandlersPage20.handlers00_54), the host lobby's start button: takes the player count from
   the lobby list, reseeds both random streams from the primary one and selects the secondary stream, clears the handshake state of all eight player blocks, arms the player-snapshot transfer and returns to the
   main page with ROM action record 1.
*/
void __thandor_void_preserve_eax_ecx
FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source)

{
  UiNodeBase *parentNode;
  uint32_t seed;
  int recordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;

  /* climb to the frontend root */
  parentNode = source->parent;
  while (parentNode != UI_NODE_NONE) {
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
    recordsRemaining = recordsRemaining - 1;
  } while (recordsRemaining != 0);
  g_FrontendHostSnapshotTransferCountdown = 4;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,1);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,1);
  }
  return;
}


/* Address: 0x00544250.
   Handler of frontend command FRONTEND_COMMAND_SET_GAME_SPEED (0x300): stores the game-speed percent in the
   mission briefing's gameSpeedSlider value (+0x58). Called directly by
   FrontendGameplaySettings_SetGameSpeedPercent in a local game, through the command queue in a network game.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_SetGameSpeedPercent
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          GameSpeedPercent gameSpeedPercent)

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
   Handler of action 0x200A (slot 10 of g_FrontendUiActionHandlersPage20.handlers00_54), the client lobby's
   Leave button; FrontendTransfer_TickRequestTimeoutAndResetPage also calls it when the host stops answering.
   Reopens the network game page with an empty session list and the Join button hidden, leaves the network
   session, takes a new session identity and sends a fresh discovery probe; the local player becomes the only
   player again, with id 0.
*/
void __thandor_preserve_eax
FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source)

{
  FrontendPlayerRuntimeRecord *firstPlayerRecord;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  /* source is the frontend template's clientLobbyLeaveButton (+0x5784). */
  FrontendUiImage *frontendUi;

  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_BROWSING;
  frontendUi = (FrontendUiImage *)THANDOR_UI_AT(source,-0x5784);
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_NETWORK_GAME,(UiPageStackControl *)FRONTEND_UI(frontendUi,frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) =
         FRONTEND_UI_FIELD(frontendUi,menuRoomModelView,0x4C,int32_t) | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,FRONTEND_UI(frontendUi,frontendRoot));
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
   Network game page tick (FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState while g_FrontendNetworkState
   is FRONTEND_NETWORK_STATE_BROWSING): counts down the expiry of every discovered session, drops the sessions
   whose expiry ran out by compacting the 0xB0-byte records in place, rebuilds the row pointers,
   keeps the selection on the same session (row 0 when the selected one went away) and refreshes the list.
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
  /* the record field typed payloadByteCount holds the session's expiry ticks */
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
        *selectedSlotField = *selectedSlotField - 1;
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
        for (dwordsRemaining = sizeof(FrontendSessionDiscoveryRecordB0) / sizeof(uint32_t); dwordsRemaining != 0;
            dwordsRemaining = dwordsRemaining - 1) {
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
        g_FrontendPlayerRuntimeBlockCount = 1;
        /* XCHG: the timer thread reads the id (InGameCommandQueue_AppendLocalPlayerCommand) */
        previousLocalPlayerId = THANDOR_ATOMIC_EXCHANGE(&g_LocalPlayerRuntimeId,0);
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
    previousLocalPlayerId = THANDOR_ATOMIC_EXCHANGE(&g_LocalPlayerRuntimeId,0);
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
   Handler of frontend command FRONTEND_COMMAND_APPLY_GAME_SPEED (0x2C0), leaving the mission briefing: closes
   any playing movie, converts the gameSpeedSlider percent into the simulation's Q8 game speed, sets flag 0x08
   of the briefing image and returns to the main page with ROM action record romActionIndex. Called directly by
   FrontendSessionAction_ApplySpeedOrToggleReady and FrontendCallback_ApplyGameSpeedOrDispatch02C0 in a local
   game, through the command queue in a network game.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ApplyGameSpeedAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex)

{
  uint32_t *displayFlags;
  int frontendRootAddress;

  frontendRootAddress = g_FrontendRootNode;
  Movie_Close();
  /* percent * 0x28F5C >> 16 = percent * 256 / 100 */
  g_GameFactionRuntimeImage.tail.gameSpeedQ8 =
       (uint32_t)(FRONTEND_UI_FIELD(frontendRootAddress,gameSpeedSlider,0x58,int) * 0x28f5c) >> 16;
  displayFlags = &((UiImageActionControl *)FRONTEND_UI(frontendRootAddress,briefingImage))->displayFlags;
  *displayFlags = *displayFlags | 8;
  FrontendSession_ReturnToMainPage(playerRuntimeId,0,0,romActionIndex);
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

