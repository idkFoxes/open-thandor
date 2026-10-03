/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: network/backend/runtime. */

/* Returns the player record whose peer sequence token and IPv4 address match the sender, or NULL. Record 0 is
   always compared (the player count is at least 1). */
static FrontendPlayerRuntimeRecord *FrontendNetwork_FindPlayerBySender
          (uint32_t sequenceToken,NetworkIpv4AddressNetworkOrder senderAddress)

{
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeBlockCount remainingPlayers;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  while (sequenceToken != playerRecord->peerSequenceToken ||
         senderAddress != playerRecord->endpoint.ipv4AddressNetworkOrder) {
    playerRecord++;
    remainingPlayers--;
    if (remainingPlayers == 0) {
      return NULL;
    }
  }
  return playerRecord;
}


/* Host-side packet handler of the frontend session, for packets from known players (matched by sequence token
   and IPv4 address): 0x10011 stores the player's next command record (and marks it pending when new), 0x10013
   only refreshes the player's timeout, 0x10004 answers with the requested player's 0x30005 snapshot, and 0x8000A
   stores one chunk of the player's snapshot payload and requests the next one with 0x10009.
*/
void FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  NetworkIpv4AddressNetworkOrder senderAddress;
  uint32_t sequenceToken;
  uint32_t requestedPlayerIndex;
  uint32_t packetType;
  FrontendSnapshotChunkByteOffset packetChunkOffset;
  uint32_t dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;
  const uint32_t *chunkSource;
  uint32_t *payloadDestination;
  const uint32_t *snapshotSource;
  uint32_t *snapshotDestination;

  sequenceToken = packet->packet10000Handshake.header.sequenceToken;
  senderAddress = senderEndpoint->ipv4AddressNetworkOrder;
  packetType = packet->packet10000Handshake.header.packedTypeAndUnitCount;

  if (packetType == FRONTEND_PACKET_10011_LOBBY_COMMAND) {
    playerRecord = FrontendNetwork_FindPlayerBySender(sequenceToken,senderAddress);
    if (playerRecord == NULL) {
      return;
    }
    /* the command records run parallel to the player records */
    commandRecord = g_FrontendPlayerCommandRecords + (playerRecord - g_FrontendPlayerRuntimeBlocks);
    playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* a new sender context means a new command record: copy it and mark the player's command as submitted */
    if (packet->packet10000Handshake.header.senderContext != commandRecord->header.senderContext) {
      playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      *commandRecord = packet->command10011Or10021;
    }
    return;
  }

  if (packetType == FRONTEND_PACKET_10013_WAIT_ACK) {
    playerRecord = FrontendNetwork_FindPlayerBySender(sequenceToken,senderAddress);
    if (playerRecord != NULL) {
      playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    }
    return;
  }

  if (packetType == FRONTEND_PACKET_10004_SNAPSHOT_REQUEST) {
    playerRecord = FrontendNetwork_FindPlayerBySender(sequenceToken,senderAddress);
    if (playerRecord == NULL) {
      return;
    }
    requestedPlayerIndex = packet->packet10004PlayerSnapshotRequest.requestedPlayerIndex;
    if (requestedPlayerIndex < g_FrontendPlayerRuntimeBlockCount) {
      /* the packet is filled from the first 0x60 bytes of the player record, then its header and fields are set */
      snapshotSource = (const uint32_t *)(g_FrontendPlayerRuntimeBlocks + requestedPlayerIndex);
      snapshotDestination = (uint32_t *)&g_FrontendPacket30005Buffer;
      for (dwordCount = 0; dwordCount < sizeof(FrontendPacket30005PlayerSnapshot) / sizeof(uint32_t);
           dwordCount++) {
        snapshotDestination[dwordCount] = snapshotSource[dwordCount];
      }
      g_FrontendPacket30005Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_30005_PLAYER_SNAPSHOT;
      g_FrontendPacket30005Buffer.playerIndex = requestedPlayerIndex;
      g_FrontendPacket30005Buffer.secondaryRandomSeed = Random_GetSecondarySeed();
      UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket30005Buffer.header);
    }
    return;
  }

  if (packetType != FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK) {
    return;
  }
  playerRecord = FrontendNetwork_FindPlayerBySender(sequenceToken,senderAddress);
  if (playerRecord == NULL) {
    return;
  }
  /* only the chunk at the expected offset of an incomplete snapshot is stored */
  packetChunkOffset = packet->packet8000ASnapshotChunk.snapshotChunkOffset;
  if ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) == 0 ||
      (playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0 ||
      packetChunkOffset != playerRecord->snapshotChunkOffset) {
    return;
  }
  chunkSource = (const uint32_t *)packet->packet8000ASnapshotChunk.packet10009Buffer;
  payloadDestination = (uint32_t *)(playerRecord->snapshotPayload + packetChunkOffset);
  dwordCount = UI_TRANSFER_CHUNK_PAYLOAD_BYTES / sizeof(uint32_t);
  if (packetChunkOffset == FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
    dwordCount = FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES / sizeof(uint32_t);
    playerRecord->snapshotTransferFlags = playerRecord->snapshotTransferFlags | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
    playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES;
  }
  for (; dwordCount != 0; dwordCount--) {
    *payloadDestination = *chunkSource;
    chunkSource++;
    payloadDestination++;
  }
  if (packetChunkOffset != FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
    /* request the next chunk */
    g_FrontendPacket10009Buffer.snapshotChunkOffset = packetChunkOffset + UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
    playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
    g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST;
    UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10009Buffer.header);
    g_FrontendHostSnapshotTransferCountdown = FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS;
  }
}


/* While some client's command record is missing: re-sends the last command batch to every client whose record
   is also still missing and 0x10012 (wait) to the clients that already sent theirs. */
static void FrontendNetwork_ResendBatchOrWaitToClients(void)

{
  FrontendPlayerRuntimeRecord *clientRecord;
  FrontendPlayerRuntimeBlockCount remainingClients;

  clientRecord = &g_FrontendPlayerRuntimeBlocks[1];
  for (remainingClients = g_FrontendPlayerRuntimeBlockCount - 1; remainingClients != 0; remainingClients--) {
    if (clientRecord->commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
      UiTransfer_StagePacketAndSend(&clientRecord->endpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
    }
    else {
      g_FrontendPacket10012Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10012_WAIT;
      UiTransfer_StagePacketAndSend(&clientRecord->endpoint,&g_FrontendPacket10012Buffer.header);
    }
    clientRecord++;
  }
}


/* Packs every non-empty 0x20-byte command record (command code in bits 8..31, player id in the low byte) into
   the batch buffer and returns the number of packed records. With nothing pending the first record (the host's
   own) is sent as a batch of one. */
static uint32_t FrontendNetwork_BuildCommandBatch(void)

{
  const FrontendCommandPacketRecord *commandRecord;
  FrontendCommandPacketRecord *batchRecord;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  uint32_t commandCount;

  commandCount = 0;
  commandRecord = g_FrontendPlayerCommandRecords;
  batchRecord = g_FrontendCommandBatchPacketBuffer;
  for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers--) {
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      *batchRecord = *commandRecord;
      batchRecord++;
      commandCount++;
    }
    commandRecord++;
  }
  if (commandCount == 0) {
    *batchRecord = g_FrontendPlayerCommandRecords[0];
    commandCount = 1;
  }
  return commandCount;
}


/* Executes the first commandCount records of the command batch locally. */
static void FrontendNetwork_ExecuteCommandBatch(uint32_t commandCount)

{
  const FrontendCommandPacketRecord *commandRecord;
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  CommandQueueHandlerProc *commandHandler;

  commandRecord = g_FrontendCommandBatchPacketBuffer;
  for (; commandCount != 0; commandCount--) {
    packedCommand = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      /* the handler (FRONTEND_COMMAND_CODE_BASE + code) must lie in the code section */
      commandHandler = CommandDispatch_ResolveHandler
                                 (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL) {
        (*commandHandler)
                  (packedCommand & 0xff,commandRecord->command.payload1,commandRecord->command.payload2,
                   commandRecord->command.payload3);
      }
    }
    commandRecord++;
  }
}


/* Packs the flags dword (plus the payload when complete) of every player's snapshot into the package scratch
   buffer, PCK-encodes the block, hands a copy (size dword + encoded bytes) to the outgoing transfer mailbox and
   queues FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE. Only called with more than one player. */
static void FrontendNetwork_PublishSnapshots(void)

{
  const FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendSnapshotTransferFlags playerFlags;
  uint32_t packedSizeBytes;
  uint32_t *scratchCursor;
  const uint32_t *payloadSource;
  uint32_t payloadDwordIndex;
  uint32_t encodedByteCount;
  uint32_t bufferSizeBytes;
  uint32_t dwordCount;
  void *allocPayload;
  uint32_t *outgoingCursor;

  packedSizeBytes = 0;
  scratchCursor = (uint32_t *)g_PackageScratchBuffer;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers--) {
    playerFlags = playerRecord->snapshotTransferFlags;
    *scratchCursor = playerFlags;
    scratchCursor++;
    packedSizeBytes = packedSizeBytes + FRONTEND_SNAPSHOT_FLAGS_BYTES;
    if ((playerFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
      payloadSource = (const uint32_t *)playerRecord->snapshotPayload;
      for (payloadDwordIndex = 0; payloadDwordIndex < FRONTEND_SNAPSHOT_PAYLOAD_BYTES / sizeof(uint32_t);
           payloadDwordIndex++) {
        *scratchCursor = payloadSource[payloadDwordIndex];
        scratchCursor++;
      }
      packedSizeBytes = packedSizeBytes + FRONTEND_SNAPSHOT_PAYLOAD_BYTES;
    }
    playerRecord++;
  }
  /* encoded behind the packed data, after a size dword; the size and the encoded bytes are then copied out */
  if (!PckCodec_EncodeHuffmanRle
           (PACKAGE_SCRATCH_BUFFER_BYTES - 4 - packedSizeBytes,(uint8_t *)(scratchCursor + 1),packedSizeBytes,
            g_PackageScratchBuffer,&encodedByteCount,NULL)) {
    return;
  }
  *scratchCursor = packedSizeBytes;
  bufferSizeBytes = encodedByteCount + 4;
  if (g_MemoryApi.alloc(bufferSizeBytes,&allocPayload) != 0) {
    return;
  }
  /* Original quirk: only whole dwords are copied; up to 3 trailing encoded bytes stay uninitialised */
  outgoingCursor = (uint32_t *)allocPayload;
  for (dwordCount = bufferSizeBytes >> 2; dwordCount != 0; dwordCount--) {
    *outgoingCursor = *scratchCursor;
    scratchCursor++;
    outgoingCursor++;
  }
  UiTransferMailbox_SetOutgoingBuffer(bufferSizeBytes,allocPayload);
  FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE,0,0,0);
}


/* Snapshot exchange: each time the countdown runs out, re-request the missing chunk of the first incomplete
   snapshot, or (all complete) mark the host's publication ready and publish all snapshots. */
static void FrontendNetwork_TickSnapshotExchange(void)

{
  FrontendPlayerRuntimeRecord *transferPlayer;
  FrontendPlayerRuntimeBlockCount remainingPlayers;

  if (g_FrontendHostSnapshotTransferCountdown == 0) {
    return;
  }
  g_FrontendHostSnapshotTransferCountdown = g_FrontendHostSnapshotTransferCountdown - 1;
  if (g_FrontendHostSnapshotTransferCountdown != 0) {
    return;
  }
  transferPlayer = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers--) {
    if ((transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0 &&
        (transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0) {
      g_FrontendPacket10009Buffer.snapshotChunkOffset = transferPlayer->snapshotChunkOffset;
      g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST;
      UiTransfer_StagePacketAndSend(&transferPlayer->endpoint,&g_FrontendPacket10009Buffer.header);
      g_FrontendHostSnapshotTransferCountdown = FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS;
      return;
    }
    transferPlayer++;
  }
  g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags =
       g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags | FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
  if (1 < g_FrontendPlayerRuntimeBlockCount) {
    FrontendNetwork_PublishSnapshots();
  }
}


/* Host tick of the frontend session. While a client's command record is missing it re-sends the last batch to
   clients without a new record and 0x10012 (wait) to the others and returns true. Otherwise it
   broadcasts all non-empty command records as one lobby command batch, executes the batch locally, and drives
   the snapshot exchange: re-requests a missing chunk, or once every snapshot is complete packs all of them,
   PCK-encodes the block into the outgoing transfer mailbox and queues FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE.
*/
bool FrontendNetwork_HostTickCommandAndSnapshotTransfer(uint32_t callbackArg)

{
  FrontendPlayerRuntimeRecord *clientRecord;
  FrontendPlayerRuntimeRecord *playerRecordEnd;
  FrontendPlayerRuntimeBlockCount remainingClients;
  uint32_t commandCount;

  /* wait until every client has sent its command record for this tick */
  playerRecordEnd = g_FrontendPlayerRuntimeBlocks + g_FrontendPlayerRuntimeBlockCount;
  for (clientRecord = &g_FrontendPlayerRuntimeBlocks[1]; clientRecord < playerRecordEnd; clientRecord++) {
    if (clientRecord->commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
      FrontendNetwork_ResendBatchOrWaitToClients();
      return true;
    }
  }
  for (clientRecord = &g_FrontendPlayerRuntimeBlocks[1]; clientRecord < playerRecordEnd; clientRecord++) {
    clientRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
  }

  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  commandCount = FrontendNetwork_BuildCommandBatch();
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       commandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
  clientRecord = &g_FrontendPlayerRuntimeBlocks[1];
  for (remainingClients = g_FrontendPlayerRuntimeBlockCount - 1; remainingClients != 0; remainingClients--) {
    UiTransfer_StagePacketAndSend(&clientRecord->endpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
    clientRecord++;
  }
  /* execute the batch locally as well */
  FrontendNetwork_ExecuteCommandBatch(commandCount);
  FrontendNetwork_TickSnapshotExchange();
  return false;
}


/* Host timeout of a client while the session starts, called by
   FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState while g_FrontendNetworkState is
   FRONTEND_NETWORK_STATE_CLIENT_STARTING: when g_SessionTransferTimeoutTicks runs out the host is lost. The session is dropped to a
   local one (mailbox cleared, network role cleared, backend socket closed), the frontend returns to its
   first page if it was still waiting for players, the TEXT_ID_NETWORK_HOST_LOST notice with the host's name
   is shown and the player list collapses to the local player alone.
*/
void FrontendNetwork_TickDisconnectTimeoutAndResetSession(void)

{
  int frontendRootBase;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  uint16_t *resolvedText;

  g_SessionTransferTimeoutTicks--;
  if (g_SessionTransferTimeoutTicks != 0) {
    return;
  }
  UiTransferMailbox_ClearReceivedState();
  g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
  g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
  g_NetworkBackendSlot3(); /* close the socket */
  g_NetworkBackendSlot1(); /* backend cleanup */
  frontendRootBase = g_FrontendRootNode;
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
  RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendPlayerRuntimeBlocks->playerName,resolvedText);
  FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers--) {
    if (playerRecord->factionAssignment.readyOrWaitState == 0) {
      /* some player had not reported ready yet: report it for the local player, so a waiting
         Frontend_Init can finish (the role was cleared above, so the local branch is always taken) */
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_PLAYER_READY,0,0,0);
      }
      break;
    }
    playerRecord++;
  }
  /* collapse the player list to the local player alone, in block 0 */
  g_FrontendPlayerRuntimeBlockCount = 1;
  g_LocalPlayerRuntimeId = 0;
  localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
  localPlayerRecord->playerName.textUtf16[0] = 0;
  localPlayerRecord->playerName.textUtf16[1] = 0;
  localPlayerRecord->playerRuntimeId = 0;
  localPlayerRecord->factionAssignment.roleStateFlags = 0;
  localPlayerRecord->colourCycleFlags = 0;
  localPlayerRecord->snapshotTransferFlags = 0;
}


/* Client side of the in-game command exchange, for one received packet from the host of this session. A
   new COMMAND_BATCH is executed and answered with the client's next COMMAND_SUBMIT (returns true);
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
      /* Not in the original: a count of 0 (which wraps) or one past the 0x100-byte receive slot executes
         nothing. The receive check (UiTransferMailbox_DecryptAndVerifyRecord) already drops such packets;
         valid batches have 1..8 records. */
      if (remainingCommands == 0 || remainingCommands > FRONTEND_PACKET_MAX_UNIT_COUNT) {
        remainingCommands = 0;
      }
      /* the batch is an array of 0x20-byte command records; the first header is the batch header */
      while (remainingCommands != 0) {
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
      }
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
          /* close the gap: move the following records down by one */
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


/* Binds the 14 exports of wsock32.dll that the code calls (the original binds 45), starts WinSock 1.1 and
   installs the UDP fallback backend
   (NetworkFallback_*) as the only network backend instance. Returns 0 on success, otherwise the
   DynDLL/DynAPI error code or the WSAStartup error; the original reports success in every case,
   so a missing WinSock is not fatal there. The original starts by jumping over a large block of code,
   presumably a ws2_32 path (not ported: nothing installed it).
*/
uint32_t __cdecl Network_Init(void)

{
  HINSTANCE module;
  uint32_t startupError;
  uint32_t resolveError;

  module = DynDLL_Load(s_Wsock32ModuleName);
  if (module == NULL) return FATAL_ERROR_DLL_LOAD_FAILED;
  resolveError = DynAPI_Resolve(&g_WinSock_bind,module,s_Wsock32Export_bind);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_htons,module,s_Wsock32Export_htons);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_inet_ntoa,module,s_Wsock32Export_inet_ntoa);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_ioctlsocket,module,s_Wsock32Export_ioctlsocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_recvfrom,module,s_Wsock32Export_recvfrom);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_sendto,module,s_Wsock32Export_sendto);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_setsockopt,module,s_Wsock32Export_setsockopt);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_socket,module,s_Wsock32Export_socket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_gethostbyname,module,s_Wsock32Export_gethostbyname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSACleanup,module,s_Wsock32Export_WSACleanup);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAGetLastError,module,s_Wsock32Export_WSAGetLastError);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve(&g_WinSock_WSAStartup,module,s_Wsock32Export_WSAStartup);
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


/* Stops WinSock at program end: WSACleanup of the DLL that Network_Init started.
*/
void Network_Shutdown(void)

{
  if (g_NetworkBackendMode == NETWORK_BACKEND_MODE_WSOCK32) {
    g_WinSock_WSACleanup();
    g_NetworkBackendMode = NETWORK_BACKEND_MODE_NONE;
    return;
  }
  return;
}


/* Backend slot 0 ("select backend instance") of the wsock32 backend, which has a single instance: it
   accepts any backendIndex and always returns 0 (success). The original stores a leftover value unrelated to
   the index in g_NetworkBackendSessionContext. Like the previous C version (which received the
   index in its sessionContext parameter), this stores the index; nothing reads the global.
*/
uint32_t NetworkBackend_SetSessionContext(uint32_t backendIndex)

{
  g_NetworkBackendSessionContext = (NetworkSessionContext *)(uintptr_t)backendIndex;
  return 0;
}

