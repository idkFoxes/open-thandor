/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/command_exchange.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/command_exchange.h>
#include <thandor/network/protocol/lockstep.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(16) FrontendPacket10022StatePending g_FrontendPacket10022Buffer = {};

static FrontendPacket8000ASnapshotChunk g_FrontendPacket8000ABuffer = {};

static FrontendPacket10013HeartbeatAck g_FrontendPacket10013Buffer = {};

uint32_t g_FrontendTransferResponsePending = 0;

uintptr_t g_FrontendLocalPlayerPcxPreview = 0;

FrontendCommandPacketRecord g_FrontendClientPlayerCommandRecords[8] = {};

FrontendCommandPacketRecord g_FrontendClientCommandBatchPacketBuffer[8] = {};

FrontendCommandPacketRecord g_FrontendPacket10021Buffer = {};

static bool s_loggedSnapshotChunkOffset = false;

/* Implementation ownership: network/protocol/command_exchange. */

/* Copies one 0x20-byte command packet record dword by dword. */
void FrontendTransfer_CopyCommandRecord
          (FrontendCommandPacketRecord *destination,const FrontendCommandPacketRecord *source)
{
  uint32_t *destinationDwords;
  const uint32_t *sourceDwords;
  int dwordCount;

  destinationDwords = (uint32_t *)destination;
  sourceDwords = (const uint32_t *)source;
  for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
    *destinationDwords = *sourceDwords;
    sourceDwords++;
    destinationDwords++;
  }
}

/* Client side of the session start: executes a new command batch from the host (a repeated batch only
   re-sends the last 0x10011 answer), answers 0x10012 with 0x10013, removes a player on 0x10007, stores the
   next player snapshot (0x30005, which also seeds the random streams) and serves chunks of the local
   player's PCX preview on 0x10009. Only packets of the selected host and session count; returns true only when
   a new command batch was executed.
*/
Bool8 FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  UiTransferSenderContext batchSenderContext;
  uint32_t expectedBlockCount;
  uint32_t playerIndex;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  uint32_t *nextPlayerCursor;
  uint32_t *packetCursor;
  uint32_t *previewSourceCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *recordDwordCursor;
  uint32_t *chunkDestinationCursor;
  uint16_t *resolvedText;

  expectedBlockCount = g_FrontendExpectedPlayerRuntimeBlockCount;
  /* only packets of the selected host and session count */
  if ((g_FrontendSessionToken != packet->packet10000Handshake.header.sequenceToken) ||
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder != senderEndpoint->ipv4AddressNetworkOrder)) {
    return false;
  }
  if ((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
      FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) {
    batchSenderContext = packet->packet10000Handshake.header.senderContext;
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* the same batch again: our answer got lost, repeat it */
    if (batchSenderContext == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    g_FrontendSelectedPlayerToken = batchSenderContext;
    FrontendTransfer_ExecuteLobbyCommandRecords
              (&packet->command10011Or10021,
               packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT);
    FrontendTransfer_SendLobbyCommandAndSnapshotRequest();
    g_FrontendTransferResponsePending = 1;
    return true;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10012_WAIT) {
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013_WAIT_ACK;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10013Buffer.header);
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10007_PLAYER_REMOVAL) {
    /* The original compares the first record before the count is checked; bounded here because with no
       player block a match on the stale record 0 would close the gap with (0 - 1) records. */
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    if (playersRemaining == 0) {
      return false;
    }
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (packet->playerRemoval10007.removedPlayerToken == playerRecord->playerRuntimeId) {
        /* "player left" message with the name, then close the gap in the record array */
        resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
        FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
        nextPlayerCursor = (uint32_t *)(playerRecord + 1);
        recordDwordCursor = (uint32_t *)playerRecord;
        for (dwordCount = (playersRemaining - 1) * (sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t));
             dwordCount != 0; dwordCount--) {
          *recordDwordCursor = *nextPlayerCursor;
          nextPlayerCursor++;
          recordDwordCursor++;
        }
        g_FrontendPlayerRuntimeBlockCount--;
        return false;
      }
      playerRecord++;
      playersRemaining--;
    } while (playersRemaining != 0);
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) {
    /* only the next missing snapshot is taken */
    playerIndex = packet->packet30005PlayerSnapshot.playerIndex;
    if ((playerIndex < g_FrontendExpectedPlayerRuntimeBlockCount) &&
        (playerIndex == g_FrontendPlayerRuntimeBlockCount)) {
      Random_SetBothSeeds(packet->packet30005PlayerSnapshot.secondaryRandomSeed);
      Random_SelectSecondaryStream();
      g_FrontendPlayerRuntimeBlockCount++;
      packetCursor = (uint32_t *)packet;
      recordDwordCursor = (uint32_t *)(g_FrontendPlayerRuntimeBlocks + playerIndex);
      /* the first 0x60 bytes of the packet become the head of the player's record */
      for (dwordCount = 24; dwordCount != 0; dwordCount--) {
        *recordDwordCursor = *packetCursor;
        packetCursor++;
        recordDwordCursor++;
      }
      resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_ARRIVED);
      RichTextCommandStream_PatchPayloadBySelector
                (0,packet->packet30005PlayerSnapshot.playerDescriptorPayload,resolvedText);
      FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
    }
    return false;
  }
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST) {
    /* The original serves any requested offset; bounded here because the offset comes from the peer and
       would read (and send back) memory outside the 0x1300-byte preview. Valid hosts request 0..0x1220 in
       0xE8 steps. */
    if ((packet->packet10009SnapshotChunkRequest.snapshotChunkOffset > FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) ||
        (packet->packet10009SnapshotChunkRequest.snapshotChunkOffset % 0xE8 != 0)) {
      if (!s_loggedSnapshotChunkOffset) {
        s_loggedSnapshotChunkOffset = true;
        Thandor_Log("network: snapshot chunk request at offset 0x%X (valid: 0..0x%X in 0xE8 steps), ignored",
                    (unsigned)packet->packet10009SnapshotChunkRequest.snapshotChunkOffset,
                    (unsigned)FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET);
      }
      return false;
    }
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         packet->packet10009SnapshotChunkRequest.snapshotChunkOffset;
    chunkDestinationCursor = (uint32_t *)g_FrontendPacket8000ABuffer.packet10009Buffer;
    previewSourceCursor = (uint32_t *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK;
    /* 0xE8-byte chunks; the last one at 0x1220 has 0xE0 bytes (the preview is 0x1300 bytes) */
    dwordCount = 58;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
      dwordCount = 56;
    }
    for (; dwordCount != 0; dwordCount--) {
      *chunkDestinationCursor = *previewSourceCursor;
      previewSourceCursor++;
      chunkDestinationCursor++;
    }
    /* answered only once every expected player snapshot has arrived */
    if (expectedBlockCount == g_FrontendPlayerRuntimeBlockCount) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket8000ABuffer.header);
    }
    return false;
  }
  return false;
}

/* Lockstep channel of the in-game command exchange. */
static const LockstepHostChannel g_InGameLockstepChannel = {
  g_FrontendClientPlayerCommandRecords,
  g_FrontendClientCommandBatchPacketBuffer,
  FRONTEND_PACKET_COMMAND_BATCH_TYPE,
  &g_FrontendPacket10022Buffer.header,
  FRONTEND_PACKET_COMMAND_WAIT,
  INGAME_COMMAND_CODE_BASE,
  INGAME_COMMAND_HANDLER_REGION_END
};

/* Host side of the in-game command exchange. When every client (player records 1..n-1) has submitted its
   command, clears their ready flags, takes the host's own next command into slot 0, packs all non-empty
   command slots into g_FrontendClientCommandBatchPacketBuffer (at least one record) and sends that
   COMMAND_BATCH to every client; returns false. Otherwise returns true and, with
   notifyWaitingPeers, resends the previous batch to clients that have not submitted yet and COMMAND_WAIT
   to those that have.
*/
Bool8 FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers)

{
  if (!Lockstep_AllClientsSubmitted()) {
    if (notifyWaitingPeers != 0) {
      Lockstep_ResendBatchOrWait(g_InGameLockstepChannel);
    }
    return true;
  }
  Lockstep_ClearClientSubmissions();
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  Lockstep_PackBatch(g_InGameLockstepChannel,g_FrontendPlayerRuntimeBlockCount,LockstepEmpty::SendHostRecord);
  Lockstep_SendBatchToClients(g_InGameLockstepChannel,g_FrontendPlayerRuntimeBlockCount);
  return false;
}

/* Client side of the lockstep exchange: sends the host its next in-game command (FRONTEND_PACKET_COMMAND_SUBMIT)
   with the oldest queued command, or an empty record when none is queued. The sender context counts the
   submissions.
*/
void FrontendTransfer_SendCommandSubmit()

{
  g_FrontendPacket10021Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_SUBMIT;
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10021Buffer);
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
}

/* Host side of the in-game command exchange: finds the player the packet came from (sequence token and
   IPv4 address) and refreshes its timeout. A COMMAND_SUBMIT with a new sender context is stored in that
   player's command slot and marks the player ready for the next batch; a repeated one (retransmit) and a
   COMMAND_WAIT_ACK only refresh the timeout.
*/
void FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSequenceToken senderSequenceToken;
  UiTransferSenderContext packetSenderContext;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;
  uint32_t *copySource;
  uint32_t *copyDestination;

  senderSequenceToken =packet->packet10000Handshake.header.sequenceToken;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_SUBMIT) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_WAIT_ACK) {
      return;
    }
    while ((senderSequenceToken != playerRecord->peerSequenceToken ||
           (sourceContext->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)))
    {
      playersRemaining--;
      playerRecord++;
      if (playersRemaining == 0) {
        return;
      }
    }
    playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    return;
  }
  /* the command slots run parallel to the player records */
  commandRecord = g_FrontendClientPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((senderSequenceToken != playerRecord->peerSequenceToken ||
         (sourceContext->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder))) {
    commandRecord++;
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  packetSenderContext = packet->packet10000Handshake.header.senderContext;
  playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
  /* the client bumps its sender context per new command; an equal one is a retransmit */
  if (packetSenderContext != commandRecord->header.senderContext) {
    playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    /* copy the whole 0x20-byte packet, header included, into the slot */
    copySource = (uint32_t *)packet;
    copyDestination = (uint32_t *)commandRecord;
    for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
      *copyDestination = *copySource;
      copySource++;
      copyDestination++;
    }
    /* The original keeps the player id the client put into the record, so a client could act as another
       player; replaced here by the id of the player the packet came from. A valid client always sends its own
       id, so its records stay unchanged; an empty record (code 0) is left as it is. */
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      commandRecord->command.packedCommandAndPlayerId =
           (commandRecord->command.packedCommandAndPlayerId & 0xffffff00) | (playerRecord->playerRuntimeId & 0xff);
    }
  }
}

/* Host side: executes the command batch it has just broadcast (g_FrontendClientCommandBatchPacketBuffer) on
   the local simulation, so host and clients run the same commands in the same tick. The high 24 bits of
   each packed command are the handler's offset from InGameCommandQueue_AppendLocalPlayerCommand, the low
   8 bits the player id; offsets beyond the handler code region are ignored. The original handler address
   is resolved to its C function and the record validated by CommandDispatch_ExecuteRecord.
*/
void FrontendTransfer_DispatchStagedCommandRecords()

{
  Lockstep_ExecuteRecords(g_InGameLockstepChannel,
                          g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >>
                          FRONTEND_PACKET_UNIT_COUNT_SHIFT);
}

/* Client side: atomically takes and clears g_FrontendTransferResponsePending, which
   FrontendNetwork_HandleCommandBatchAndPlayerTimeout sets after executing a new command batch. Returns true
   when no batch arrived, so the in-game tick waits for the host instead of advancing the simulation.
*/
Bool8 FrontendTransfer_ConsumeProcessedFlag()

{
  int previousFlag;

  /* atomic exchange: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}
