/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: network/backend/runtime. */

/* Address: 0x0054EF60.
   Host-side packet handler of the frontend session, for packets from known players (matched by sequence token
   and IPv4 address): 0x10011 stores the player's next command record (and marks it pending when new), 0x10013
   only refreshes the player's timeout, 0x10004 answers with the requested player's 0x30005 snapshot, and 0x8000A
   stores one chunk of the player's snapshot payload and requests the next one with 0x10009.
*/
void FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  NetworkIpv4AddressNetworkOrder senderAddress;
  UiTransferSenderContext packetSenderContext;
  UiTransferJoinAvailability packetChunkOffset;
  uint32_t sequenceTokenOrPlayerIndex;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *sourceDword;
  FrontendCommandPacketRecord *commandRecord;
  uint32_t *snapshotCursor;
  uint8_t *payloadCursor;
  uint32_t *copySource;
  uint32_t *copyDestination;
  
  sequenceTokenOrPlayerIndex = packet->packet10000Handshake.header.sequenceToken;
  senderAddress = senderEndpoint->ipv4AddressNetworkOrder;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10011_LOBBY_COMMAND) {
    commandRecord = g_FrontendPlayerCommandRecords;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
      playerRecord = playerRecord + 1;
      commandRecord = commandRecord + 1;
      remainingPlayers = remainingPlayers - 1;
      if (remainingPlayers == 0) {
        return;
      }
    }
    packetSenderContext = packet->packet10000Handshake.header.senderContext;
    playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* a new sender context means a new command record: copy it and mark the player's command as submitted */
    if (packetSenderContext != commandRecord->header.senderContext) {
      playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      copySource = (uint32_t *)packet;
      copyDestination = (uint32_t *)commandRecord;
      for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
        *copyDestination = *copySource;
        copySource++;
        copyDestination++;
      }
    }
    return;
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10013_WAIT_ACK) {
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
      remainingPlayers = remainingPlayers - 1;
      playerRecord = playerRecord + 1;
      if (remainingPlayers == 0) {
        return;
      }
    }
    playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    return;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10004_SNAPSHOT_REQUEST) {
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
      remainingPlayers = remainingPlayers - 1;
      playerRecord = playerRecord + 1;
      if (remainingPlayers == 0) {
        return;
      }
    }
    sequenceTokenOrPlayerIndex = packet->packet10004PlayerSnapshotRequest.requestedPlayerIndex;
    if (sequenceTokenOrPlayerIndex < g_FrontendPlayerRuntimeBlockCount) {
      copySource = (uint32_t *)(g_FrontendPlayerRuntimeBlocks + sequenceTokenOrPlayerIndex);
      snapshotCursor = (uint32_t *)&g_FrontendPacket30005Buffer;
      /* the packet is filled from the first 0x60 bytes of the player record, then its header and fields are set */
      for (dwordCount = sizeof(FrontendPacket30005PlayerSnapshot) / sizeof(uint32_t); dwordCount != 0;
           dwordCount--) {
        *snapshotCursor = *copySource;
        copySource++;
        snapshotCursor++;
      }
      g_FrontendPacket30005Buffer.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_30005_PLAYER_SNAPSHOT;
      g_FrontendPacket30005Buffer.playerIndex = sequenceTokenOrPlayerIndex;
      g_FrontendPacket30005Buffer.secondaryRandomSeed = Random_GetSecondarySeed();
      UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket30005Buffer.header);
      return;
    }
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK) {
    return;
  }
  while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
         (senderAddress != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
    remainingPlayers = remainingPlayers - 1;
    playerRecord = playerRecord + 1;
    if (remainingPlayers == 0) {
      return;
    }
  }
  /* the dword at +0x14 of the 0x8000A packet is the chunk offset, the data follows at +0x18 (read through\n     unrelated union members) */
  packetChunkOffset = packet->packet50001SessionAdvertisement.joinAvailableFlag;
  if ((((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
      ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) &&
     (packetChunkOffset == playerRecord->snapshotChunkOffset)) {
    sourceDword = &packet->command10011Or10021.command.payload2;
    payloadCursor = playerRecord->snapshotPayload + packetChunkOffset;
    dwordCount = UI_TRANSFER_CHUNK_PAYLOAD_BYTES / sizeof(uint32_t);
    if (packetChunkOffset == FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
      dwordCount = FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES / sizeof(uint32_t);
      playerRecord->snapshotTransferFlags =
           playerRecord->snapshotTransferFlags | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
      playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES;
    }
    for (; dwordCount != 0; dwordCount--) {
      *(uint32_t *)payloadCursor = *sourceDword;
      sourceDword = sourceDword + 1;
      payloadCursor = payloadCursor + 4;
    }
    if (packetChunkOffset != FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
      g_FrontendPacket10009Buffer.snapshotChunkOffset = packetChunkOffset + UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
      playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
      g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST;
      UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10009Buffer.header);
      g_FrontendHostSnapshotTransferCountdown = FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS;
    }
  }
  return;
}


/* Address: 0x0054F240.
   Host tick of the frontend session. While a client's command record is missing it re-sends the last batch to
   clients without a new record and 0x10012 (wait) to the others and returns true (CF set). Otherwise it
   broadcasts all non-empty command records as one lobby command batch, executes the batch locally, and drives
   the snapshot exchange: re-requests a missing chunk, or once every snapshot is complete packs all of them,
   PCK-encodes the block into the outgoing transfer mailbox and queues FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE.
*/
bool FrontendNetwork_HostTickCommandAndSnapshotTransfer(uint32_t callbackArg)

{
  FrontendSnapshotTransferFlags playerFlags;
  uint32_t commandHandlerIndex;
  int loopCount;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  uint32_t packedCommandOrDwordCount;
  uint32_t commandCountOrBufferSize;
  FrontendPlayerRuntimeBlockCount remainingPlayerCount;
  FrontendSnapshotTransferFlags scratchSizeBytes;
  FrontendSnapshotTransferFlags sourceSizeBytes;
  FrontendCommandPacketRecord *commandRecord;
  UiTransferEndpointDescriptor *peerEndpoint;
  FrontendPlayerRuntimeRecord *transferPlayer;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *payloadCursor;
  uint32_t *scratchCursor;
  FrontendCommandPacketRecord *batchCursor;
  uint32_t *outgoingCursor;
  PckCodecResult encodeResult;
  ArenaAllocResult allocResult;
  
  loopCount = g_FrontendPlayerRuntimeBlockCount - 1;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (loopCount != 0) {
    do {
      if (playerRecord[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        /* peerEndpoint[1] is the dword after the endpoint, i.e. that player's commandSyncPending */
        peerEndpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
        while (remainingPlayerCount = remainingPlayerCount - 1, remainingPlayerCount != 0) {
          if (peerEndpoint[1].addressHeader.packedFamilyAndPort == 0) {
            UiTransfer_StagePacketAndSend(peerEndpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
          }
          else {
            g_FrontendPacket10012Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10012_WAIT;
            UiTransfer_StagePacketAndSend(peerEndpoint,&g_FrontendPacket10012Buffer.header);
          }
          peerEndpoint = peerEndpoint + sizeof(FrontendPlayerRuntimeRecord) / sizeof(UiTransferEndpointDescriptor);
        }
        return true;
      }
      loopCount = loopCount + -1;
      playerRecord = playerRecord + 1;
    } while (loopCount != 0);
    loopCount = g_FrontendPlayerRuntimeBlockCount - 1;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      playerRecord[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      loopCount = loopCount + -1;
      playerRecord = playerRecord + 1;
    } while (loopCount != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  commandCountOrBufferSize = 0;
  commandRecord = g_FrontendPlayerCommandRecords;
  batchCursor = g_FrontendCommandBatchPacketBuffer;
  remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
  /* Pack every non-empty 0x20-byte command record into the batch (the command code is in bits 8..31, the player
     id in the low byte). */
  do {
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecord = commandRecord + 1;
    }
    else {
      for (loopCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); loopCount != 0; loopCount--) {
        batchCursor->header.packedTypeAndUnitCount = commandRecord->header.packedTypeAndUnitCount;
        commandRecord = (FrontendCommandPacketRecord *)&commandRecord->header.sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
      }
      commandCountOrBufferSize = commandCountOrBufferSize + 1;
    }
    remainingPlayerCount = remainingPlayerCount - 1;
  } while (remainingPlayerCount != 0);
  if (commandCountOrBufferSize << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    /* Nothing pending: send the first record (the host's own) as a batch of one. */
    commandRecord = g_FrontendPlayerCommandRecords;
    for (loopCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); loopCount != 0; loopCount--) {
      batchCursor->header.packedTypeAndUnitCount = commandRecord->header.packedTypeAndUnitCount;
      commandRecord = (FrontendCommandPacketRecord *)&commandRecord->header.sequenceToken;
      batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
    }
    commandCountOrBufferSize = 1;
  }
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       commandCountOrBufferSize << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
  peerEndpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
  while (remainingPlayerCount = remainingPlayerCount - 1, remainingPlayerCount != 0) {
    UiTransfer_StagePacketAndSend(peerEndpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
    peerEndpoint = peerEndpoint + sizeof(FrontendPlayerRuntimeRecord) / sizeof(UiTransferEndpointDescriptor);
  }
  /* Execute the batch locally as well. */
  commandRecord = g_FrontendCommandBatchPacketBuffer;
  commandCountOrBufferSize = commandCountOrBufferSize & 0xffff;
  do {
    packedCommandOrDwordCount = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommandOrDwordCount >> 8;
    if (commandHandlerIndex != 0) {
      /* the handler (FRONTEND_COMMAND_CODE_BASE + code) must lie in the code section */
      CommandQueueHandlerProc *commandHandler =
           CommandDispatch_ResolveHandler
                     (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL) {
        (*commandHandler)
                  (packedCommandOrDwordCount & 0xff,commandRecord->command.payload1,
                   commandRecord->command.payload2,commandRecord->command.payload3);
      }
    }
    remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    commandRecord = commandRecord + 1;
    commandCountOrBufferSize = commandCountOrBufferSize - 1;
  } while (commandCountOrBufferSize != 0);
  /* snapshot exchange: each time the countdown runs out, re-request a missing chunk or publish all snapshots */
  if ((g_FrontendHostSnapshotTransferCountdown != 0) &&
     (g_FrontendHostSnapshotTransferCountdown = g_FrontendHostSnapshotTransferCountdown - 1,
     remainingPlayers = g_FrontendPlayerRuntimeBlockCount, transferPlayer = g_FrontendPlayerRuntimeBlocks,
     g_FrontendHostSnapshotTransferCountdown == 0)) {
    do {
      if (((transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
         ((transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) {
        g_FrontendPacket10009Buffer.snapshotChunkOffset = transferPlayer->snapshotChunkOffset;
        g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST;
        UiTransfer_StagePacketAndSend
                  (&transferPlayer->endpoint,&g_FrontendPacket10009Buffer.header);
        g_FrontendHostSnapshotTransferCountdown = FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS;
        return false;
      }
      remainingPlayers = remainingPlayers - 1;
      transferPlayer = transferPlayer + 1;
    } while (remainingPlayers != 0);
    g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags =
         g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags |
         FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
    scratchSizeBytes = 0;
    scratchCursor = (uint32_t *)g_PackageScratchBuffer;
    if (1 < remainingPlayerCount) {
      do {
        playerFlags = playerRecord->snapshotTransferFlags;
        *scratchCursor = playerFlags;
        sourceSizeBytes = scratchSizeBytes + FRONTEND_SNAPSHOT_FLAGS_BYTES;
        scratchCursor++;
        if ((playerFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
          payloadCursor = playerRecord->snapshotPayload;
          for (loopCount = FRONTEND_SNAPSHOT_PAYLOAD_BYTES / sizeof(uint32_t); loopCount != 0; loopCount--) {
            *scratchCursor = *(uint32_t *)payloadCursor;
            payloadCursor = payloadCursor + 4;
            scratchCursor++;
          }
          sourceSizeBytes = scratchSizeBytes + (FRONTEND_SNAPSHOT_FLAGS_BYTES + FRONTEND_SNAPSHOT_PAYLOAD_BYTES);
        }
        playerRecord = playerRecord + 1;
        remainingPlayerCount = remainingPlayerCount - 1;
        scratchSizeBytes = sourceSizeBytes;
      } while (remainingPlayerCount != 0);
      /* encoded behind the packed data, after a size dword; the size and the encoded bytes are then copied out */
      encodeResult = PckCodec_EncodeHuffmanRle
                         (PACKAGE_SCRATCH_BUFFER_BYTES - 4 - sourceSizeBytes,(uint8_t *)(scratchCursor + 1),sourceSizeBytes,
                          g_PackageScratchBuffer);
      if (!encodeResult.failed) {
        *scratchCursor = sourceSizeBytes;
        commandCountOrBufferSize = encodeResult.byteCountOrError + 4;
        allocResult = g_MemoryApi.alloc(commandCountOrBufferSize);
        if (!allocResult.failed) {
          outgoingCursor = (uint32_t *)allocResult.payloadOrError;
          for (packedCommandOrDwordCount = commandCountOrBufferSize >> 2; packedCommandOrDwordCount != 0;
               packedCommandOrDwordCount--) {
            *outgoingCursor = *scratchCursor;
            scratchCursor++;
            outgoingCursor++;
          }
          UiTransferMailbox_SetOutgoingBuffer
                    (commandCountOrBufferSize,(void *)allocResult.payloadOrError);
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE,0,0,0);
        }
      }
    }
  }
  return false;
}


/* Address: 0x0054FA10.
   Host timeout of a client while the session starts, called by
   FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState while g_FrontendNetworkState is
   FRONTEND_NETWORK_STATE_CLIENT_STARTING: when g_SessionTransferTimeoutTicks runs out the host is lost. The session is dropped to a
   local one (mailbox cleared, network role cleared, backend socket closed), the frontend returns to its
   first page if it was still waiting for players, the TEXT_ID_NETWORK_HOST_LOST notice with the host's name
   is shown and the player list collapses to the local player alone.
*/
void FrontendNetwork_TickDisconnectTimeoutAndResetSession(void)

{
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  int frontendRootBase;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint16_t *resolvedText;

  g_SessionTransferTimeoutTicks--;
  if (g_SessionTransferTimeoutTicks == 0) {
    UiTransferMailbox_ClearReceivedState();
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
    g_NetworkBackendSlot3(); /* close the socket */
    g_NetworkBackendSlot1(); /* backend cleanup */
    frontendRootBase = g_FrontendRootNode;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0) {
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(g_FrontendRootNode,frontendPageStack));
      ((FrontendModelPointerContext *)FRONTEND_UI(frontendRootBase,menuRoomModelView))->contextFlags &=
           ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(frontendRootBase,menuRoomModelView));
    }
    /* player block 0 is the host's while connected */
    resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_HOST_LOST);
    RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
    FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
    localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers--) {
      if (playerRecord->factionAssignment.readyOrWaitState == 0) {
        /* some player had not reported ready yet: report it for the local player, so a waiting
           Frontend_Init can finish (the role was cleared above, so the local branch is always taken) */
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_PLAYER_READY,0,0,0);
        }
        playerRecord = g_FrontendPlayerRuntimeBlocks;
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        playerRecord->playerName.textUtf16[0] = 0;
        playerRecord->playerName.textUtf16[1] = 0;
        playerRecord->playerRuntimeId = 0;
        playerRecord->factionAssignment.roleStateFlags = 0;
        playerRecord->colourCycleFlags = 0;
        playerRecord->snapshotTransferFlags = 0;
        return;
      }
      playerRecord = playerRecord + 1;
    }
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    playerRecord->playerName.textUtf16[0] = 0;
    playerRecord->playerName.textUtf16[1] = 0;
    localPlayerRecord->playerRuntimeId = 0;
    localPlayerRecord->factionAssignment.roleStateFlags = 0;
    localPlayerRecord->colourCycleFlags = 0;
    localPlayerRecord->snapshotTransferFlags = 0;
  }
  return;
}


/* Address: 0x00572710.
   Client side of the in-game command exchange, for one received packet from the host of this session. A
   new COMMAND_BATCH is executed and answered with the client's next COMMAND_SUBMIT (returns true, CF set);
   a repeated batch (same sender context) resends the last submit. COMMAND_WAIT is answered with
   COMMAND_WAIT_ACK, and a player-removal packet drops that player's record and shows a notice. Every
   packet from the host refreshes the session timeout. Commands are resolved to their handlers by
   CommandDispatch_ResolveHandler.
*/
bool FrontendNetwork_HandleCommandBatchAndPlayerTimeout
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSenderContext packetSenderContext;
  uint32_t commandHandlerIndex;
  uint32_t remainingCommands;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int dwordCount;
  uint32_t *nextPlayerRecord;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *recordDwordCursor;
  uint16_t *resolvedText;

  if ((((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
        FRONTEND_PACKET_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    packetSenderContext = packet->packet10000Handshake.header.senderContext;
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* g_FrontendSelectedPlayerToken holds the sender context of the last executed batch */
    if (packetSenderContext != g_FrontendSelectedPlayerToken) {
      remainingCommands =
           packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
      g_FrontendSelectedPlayerToken = packetSenderContext;
      /* the batch is an array of 0x20-byte command records; the first header is the batch header */
      do {
        commandHandlerIndex = packet->command10011Or10021.command.packedCommandAndPlayerId >> 8;
        if (commandHandlerIndex != 0) {
          CommandQueueHandlerProc *commandHandler =
               CommandDispatch_ResolveHandler
                         (INGAME_COMMAND_CODE_BASE,INGAME_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
          if (commandHandler != NULL) {
            (*commandHandler)
                      (packet->command10011Or10021.command.packedCommandAndPlayerId & 0xff,
                       packet->command10011Or10021.command.payload1,
                       packet->command10011Or10021.command.payload2,
                       packet->command10011Or10021.command.payload3);
          }
        }
        packet = (FrontendTransferPacketUnion *)(&packet->command10011Or10021 + 1);
        remainingCommands--;
      } while (remainingCommands != 0);
      FrontendTransfer_SendCommandSubmit();
      g_FrontendTransferResponsePending = 1;
      return true;
    }
    /* the host resent the batch, so it has not received our submit: send it again */
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
    return false;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_COMMAND_WAIT) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    g_FrontendPacket10023Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_WAIT_ACK;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10023Buffer.header);
    return false;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_10007_PLAYER_REMOVAL) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (packet->playerRemoval10007.removedPlayerToken == playerRecord->playerRuntimeId) {
        resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
        InGameRecentTextHistory_InsertAndRebuild8(resolvedText);
        if (remainingPlayers - 1 != 0) {
          /* close the gap: move the following records down by one (REP MOVSD) */
          nextPlayerRecord = (uint32_t *)(playerRecord + 1);
          recordDwordCursor = (uint32_t *)playerRecord;
          for (dwordCount = (remainingPlayers - 1) * (sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t));
               dwordCount != 0; dwordCount--) {
            *recordDwordCursor = *nextPlayerRecord;
            nextPlayerRecord++;
            recordDwordCursor++;
          }
        }
        g_FrontendPlayerRuntimeBlockCount--;
        return false;
      }
      playerRecord++;
      remainingPlayers--;
    } while (remainingPlayers != 0);
    return false;
  }
  return false;
}


/* Address: 0x00583D10.
   Stub in the network backend code that returns 0 and preserves the other registers; nothing references
   it (neither a call nor a table entry).
*/
uint32_t Unreferenced_ReturnZeroPreserveRegs_00583D10(void)

{
  return 0;
}

/* Address: 0x00583D30.
   Empty stub in the network backend code (a bare RET); nothing references it.
*/
void Unreferenced_NoOpPreserveRegs_00583D30(void)

{
  return;
}

/* Address: 0x00583D40.
   Stub in the network backend code that returns 0 and preserves the other registers; nothing references
   it.
*/
uint32_t Unreferenced_ReturnZeroPreserveRegs_00583D40(void)

{
  return 0;
}

/* Address: 0x00584080.
   Binds the 45 exports of wsock32.dll, starts WinSock 1.1 and installs the UDP fallback backend
   (NetworkFallback_*) as the only network backend instance. Returns 0 on success, otherwise the
   DynDLL/DynAPI error code or the WSAStartup error; the original returns with CF clear in every case,
   so a missing WinSock is not fatal there. Its first instruction jumps over 0x0058408D..0x0058495F,
   presumably the ws2_32 path that NETWORK_BACKEND_MODE_WS2_32 belongs to.
*/
uint32_t __cdecl Network_Init(void)

{
  HINSTANCE module;
  uint32_t startupError;
  DllLoadResult loadResult;
  uint32_t resolveError;
  
  loadResult = DynDLL_Load(s_Wsock32ModuleName);
  module = loadResult.moduleOrError;
  if (loadResult.failed) return (uint32_t)module;
  resolveError = DynAPI_Resolve(&g_WinSock_accept,module,s_Wsock32Export_accept);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_bind,module,s_Wsock32Export_bind);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_connect,module,s_Wsock32Export_connect);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getpeername,module,s_Wsock32Export_getpeername);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getsockname,module,s_Wsock32Export_getsockname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getsockopt,module,s_Wsock32Export_getsockopt);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_htonl,module,s_Wsock32Export_htonl);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_htons,module,s_Wsock32Export_htons);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_inet_ntoa,module,s_Wsock32Export_inet_ntoa);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_ioctlsocket,module,s_Wsock32Export_ioctlsocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_listen,module,s_Wsock32Export_listen);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_ntohl,module,s_Wsock32Export_ntohl);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_ntohs,module,s_Wsock32Export_ntohs);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_recv,module,s_Wsock32Export_recv);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_recvfrom,module,s_Wsock32Export_recvfrom);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_select,module,s_Wsock32Export_select);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_send,module,s_Wsock32Export_send);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_sendto,module,s_Wsock32Export_sendto);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_setsockopt,module,s_Wsock32Export_setsockopt);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_shutdown,module,s_Wsock32Export_shutdown);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_socket,module,s_Wsock32Export_socket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_gethostbyaddr,module,s_Wsock32Export_gethostbyaddr);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_gethostbyname,module,s_Wsock32Export_gethostbyname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_gethostname,module,s_Wsock32Export_gethostname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getprotobyname,module,s_Wsock32Export_getprotobyname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getprotobynumber,module,s_Wsock32Export_getprotobynumber);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getservbyname,module,s_Wsock32Export_getservbyname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_getservbyport,module,s_Wsock32Export_getservbyport);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetHostByAddr,module,s_Wsock32Export_WSAAsyncGetHostByAddr);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetHostByName,module,s_Wsock32Export_WSAAsyncGetHostByName);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetProtoByName,module,s_Wsock32Export_WSAAsyncGetProtoByName);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetProtoByNumber,module,s_Wsock32Export_WSAAsyncGetProtoByNumber);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetServByName,module,s_Wsock32Export_WSAAsyncGetServByName);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncGetServByPort,module,s_Wsock32Export_WSAAsyncGetServByPort);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAAsyncSelect,module,s_Wsock32Export_WSAAsyncSelect);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSACancelAsyncRequest,module,s_Wsock32Export_WSACancelAsyncRequest);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSACancelBlockingCall,module,s_Wsock32Export_WSACancelBlockingCall);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSACleanup,module,s_Wsock32Export_WSACleanup);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAGetLastError,module,s_Wsock32Export_WSAGetLastError);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAIsBlocking,module,s_Wsock32Export_WSAIsBlocking);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSASetBlockingHook,module,s_Wsock32Export_WSASetBlockingHook);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAStartup,module,s_Wsock32Export_WSAStartup);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAUnhookBlockingHook,module,s_Wsock32Export_WSAUnhookBlockingHook);
  if (resolveError != 0) return resolveError;
  startupError = (uint32_t)g_WinSock_WSAStartup(MAKEWORD(1,1),&g_WinSockStartupData);
  if (startupError != 0) return startupError;
  g_NetworkBackendMode = NETWORK_BACKEND_MODE_WSOCK32;
  g_NetworkBackendSlot0 = NetworkBackend_SetSessionContext;
  g_NetworkBackendSlot1 = NetworkFallback_NoOpBackendCleanup;
  g_NetworkBackendSlot2 = NetworkFallback_OpenAndBindUdpSocket;
  g_NetworkBackendSlot3 = NetworkFallback_CloseActiveSocket;
  g_NetworkBackendSlot4 = NetworkFallback_ReceiveDatagram;
  g_NetworkBackendSlot5 = NetworkFallback_SendDatagram;
  g_NetworkBackendSlot6 = NetworkFallback_ParsePeerEndpoint;
  g_NetworkBackendSlot7 = NetworkFallback_FormatPeerAddress;
  g_NetworkBackendInstanceTable = &NetworkBackendInstanceDescriptorPrefix_00584040;
  g_NetworkBackendInstanceCount = 1;
  return 0;
}


/* Address: 0x00584DF0.
   Stops WinSock at program end: WSACleanup of the DLL that Network_Init started. The ws2_32 mode also
   frees its heap-allocated backend instance table (the wsock32 table is static image data).
*/
void Network_Shutdown(void)

{
  if (g_NetworkBackendMode == NETWORK_BACKEND_MODE_WSOCK32) {
    g_WinSock_WSACleanup();
    g_NetworkBackendMode = NETWORK_BACKEND_MODE_NONE;
    return;
  }
  if (g_NetworkBackendMode == NETWORK_BACKEND_MODE_WS2_32) {
    g_Ws2_32_WSACleanup();
    g_NetworkBackendMode = NETWORK_BACKEND_MODE_NONE;
    g_MemoryApi.free(g_NetworkBackendInstanceTable);
    g_NetworkBackendInstanceTable = NULL;
    g_NetworkBackendInstanceCount = 0;
  }
  return;
}


/* Address: 0x00584E50.
   Backend slot 0 ("select backend instance") of the wsock32 backend, which has a single instance: it
   accepts any backendIndex and returns it with CF clear. It stores ECX, not the index, in
   g_NetworkBackendSessionContext (the ws2_32 variant NetworkBackend_SelectInstanceByIndex stores the
   index); the callers pass the index on the stack only.
   Original register convention: result in EAX, CF set on failure; ECX and EDX preserved.
*/
NetworkSetSessionResult NetworkBackend_SetSessionContext(void *sessionContext,NetworkBackendSessionReturnValue32 backendIndex)

{
  NetworkSetSessionResult sessionResult;

  g_NetworkBackendSessionContext = sessionContext;
  sessionResult.failed = false;
  sessionResult.valueOrError = backendIndex;
  return sessionResult;
}


/* Address: 0x00585210.
   Backend slot 0 ("select backend instance") of the ws2_32 backend, which can offer several instances
   (protocols): remembers the index and copies the instance's address family, socket-address length,
   socket type and protocol into the active-backend globals used by the other ws2_32 slots. CF is set for
   an index beyond g_NetworkBackendInstanceCount (the original loads 0x2B into EAX there but restores EAX).
   No recovered table points at it; like the other ws2_32 slots it belongs to the skipped part of
   Network_Init.
*/
bool NetworkBackend_SelectInstanceByIndex(uint32_t instanceIndex)

{
  NetworkBackendInstanceDescriptorPrefix *selectedBackendDescriptor;

  if (instanceIndex < g_NetworkBackendInstanceCount) {
    g_NetworkBackendSessionContext = (NetworkSessionContext *)instanceIndex;
    /* the instance descriptors are 0x100 bytes apart */
    selectedBackendDescriptor =
         (NetworkBackendInstanceDescriptorPrefix *)
         ((uint8_t *)g_NetworkBackendInstanceTable + instanceIndex * 256);
    g_NetworkBackendActiveAddressFamily = selectedBackendDescriptor->addressFamily;
    g_NetworkBackendActiveSocketAddressLength = selectedBackendDescriptor->socketAddressLength;
    g_NetworkBackendActiveSocketType = selectedBackendDescriptor->socketType;
    g_NetworkBackendActiveProtocol = selectedBackendDescriptor->protocol;
    return false;
  }
  return true;
}

