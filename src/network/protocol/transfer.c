/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/transfer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/transfer.h>
#include <thandor/thandor.h>

/* Implementation ownership: network/protocol/transfer. */

/* Header bytes of the mailbox chunk packet at 0x004AE9E8; its payload follows as
   g_UiTransferChunkPacketSequenceToken / g_UiTransferChunkPayload. The packed type is written byte by byte:
   0x31,0,1,0 = FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST, 0x30,0,8,0 = FRONTEND_PACKET_80030_MAILBOX_CHUNK.
   g_UiTransferRoundKeys16Tail is not a string: Ghidra read the bytes "mohTG sakere!!!e" as text, but they are
   round keys 12..15 of the UI_TRANSFER_CIPHER_ROUND_COUNT keys starting at g_UiTransferRoundKeys16 (keys
   0..11, 0x004AE9A8). The packet header is the first byte behind the key table, right after the tail. */
#define UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES ((uint8_t *)(g_UiTransferRoundKeys16Tail + 4))

/* Address: 0x004AEB10.
   Network receive timer (125 Hz, so one tick is 8 ms). Drains the UDP socket into the record ring: each
   datagram is descrambled and its XOR checksum verified. The transfer and ping packets are answered right
   here; every other valid packet is kept in the ring for the frontend/in-game handlers. Transfer: the host
   sends a data blob (g_UiTransferMailbox) in chunks of UI_TRANSFER_CHUNK_PAYLOAD_BYTES as 0x80030 packets,
   each requested by the receiver with a 0x10031 packet naming the next offset; a request that stays
   unanswered for UI_TRANSFER_CHUNK_RETRY_TICKS ticks is sent again. Ping: 0x10032 is echoed as 0x10033,
   whose round trip becomes the player's latency text. Skipped while the ring lock is held elsewhere.
   Original quirk: the checksum loop trusts the unit count of the (descrambled) packet header (0x004AEB74..
   0x004AEB96: SHR 0x10, SHL 3, DEC/JNZ) - neither the received byte count nor the 0x100-byte ring slot limit it,
   so a count above 8 XORs past the slot and a count of 0 wraps the counter and reads on until it faults.
   Original quirk: on a host chunk request (0x10031) the timeout extension goes to the dword at +0x10 of the
   sender's receive scratch slot (0x004AED10 ADD [ESI+0x10],0x40; ESI = auxiliary endpoint slot), not to the
   requesting player's heartbeatExpiryTicks at +0x10 of the player record (EBX), which was probably meant; the
   scratch dword is never read, so the host's heartbeat countdown is not extended by chunk requests. The client
   branch (0x80030) extends g_SessionTransferTimeoutTicks as intended (0x004AEBDD).
*/
void UiTransferMailbox_ServiceAndRetransmitTimer(void)

{
  UiTransferXorChecksum *checksumField;
  uint8_t *latencySuffixCursor;
  UiTransferXorChecksum checksum;
  uint32_t chunkEndOffset;
  uint32_t slotIndexOrByteCount;
  uint32_t nextIndexOrChunkSize;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int counterOrOffset;
  uint32_t bytesRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *latencyTextCursor;
  UiTransferSenderEndpointSlot *senderEndpointSlot;
  uint32_t *receivedChunkSourceDwords;
  uint32_t *mailboxCopySourceOrDestinationDwords;
  UiRuntimeRecord *ringRecord;
  uint32_t *receivedChunkDestinationDwords;
  uint32_t *chunkPayloadCursor;
  bool lockBusy;
  NetworkReceiveResult receiveResult;
  ArenaAllocResult allocResult;
  
  g_UiTransferMailboxTickCounter++;
  lockBusy = g_SpinLockTryAcquire(&g_UiRuntimeRecordRingLock);
  if (!lockBusy) {
    /* Receive loop: handles (or rejects) one record per pass until the backend has no more data.
       The datagram goes to ring slot g_UiRuntimeRecordWriteIndex (0x100 bytes), the sender's address to
       the matching 0x80-byte slot of the auxiliary buffer; the slot is only kept (write index advanced)
       for packets that are not handled here. */
    while (slotIndexOrByteCount = g_UiRuntimeRecordWriteIndex,
           ringRecord = g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex,
           nextIndexOrChunkSize = g_UiRuntimeRecordWriteIndex + 1,
           receiveResult = g_NetworkBackendSlot4
                              ((WinSockAddress *)
                               (g_UiRuntimeRecordWriteIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeRecordEndpointSlots),256,
                               (uint8_t *)ringRecord),
           !receiveResult.failed) {
      UiTransfer_DecryptPacketBlocks
                (g_UiTransferRoundKeys16,ringRecord,256,ringRecord);
      LOCK();
      checksumField = &ringRecord->packetHeader.xorChecksum;
      checksum = *checksumField;
      *checksumField = 0;
      UNLOCK();
      /* the XOR of all dwords of the packet (unit count * 8 dwords, checksum field zeroed) must equal the
         transmitted checksum */
      counterOrOffset = (ringRecord->packetHeader.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT) << 3;
      do {
        checksum = checksum ^ ringRecord->packetHeader.packedTypeAndUnitCount;
        ringRecord = (UiRuntimeRecord *)&ringRecord->packetHeader.sequenceToken;
        counterOrOffset--;
      } while (counterOrOffset != 0);
      if (checksum == 0) {
        ringRecord = g_UiRuntimeRecordRing + slotIndexOrByteCount;
        senderEndpointSlot =
             (UiTransferSenderEndpointSlot *)(slotIndexOrByteCount * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeRecordEndpointSlots);
        if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_80030_MAILBOX_CHUNK) {
          /* a chunk of the transfer from the host of this session: payload = offset, total size, data */
          if ((g_FrontendSessionToken == ringRecord->packetHeader.sequenceToken) &&
             (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
              senderEndpointSlot->endpoint.ipv4AddressNetworkOrder)) {
            g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS;
            counterOrOffset = *(int *)ringRecord->payload;
            slotIndexOrByteCount = *(uint32_t *)(ringRecord->payload + 4);
            /* receivedAllocation: NULL = no transfer requested, UI_TRANSFER_MAILBOX_UNAVAILABLE = requested,
               first chunk still missing (allocated here), otherwise the buffer being filled */
            if (g_UiTransferMailbox.receivedAllocation != NULL) {
              if (g_UiTransferMailbox.receivedAllocation == UI_TRANSFER_MAILBOX_UNAVAILABLE) {
                allocResult = g_MemoryApi.alloc(slotIndexOrByteCount);
                if (allocResult.failed) continue;
                counterOrOffset = 0;
                g_UiTransferMailbox.receivedAllocation = (void *)allocResult.payloadOrError;
                g_UiTransferMailbox.receivedByteCount = slotIndexOrByteCount;
                g_UiTransferMailbox.receivedRemainingBytes = slotIndexOrByteCount;
              }
              /* only the chunk at the expected offset of a transfer of the expected size is taken */
              chunkEndOffset = counterOrOffset + g_UiTransferMailbox.receivedRemainingBytes;
              if ((chunkEndOffset == g_UiTransferMailbox.receivedByteCount) && (chunkEndOffset == slotIndexOrByteCount)) {
                counterOrOffset = chunkEndOffset - g_UiTransferMailbox.receivedRemainingBytes;
                bytesRemaining = slotIndexOrByteCount - counterOrOffset;
                nextIndexOrChunkSize = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
                if (bytesRemaining < UI_TRANSFER_CHUNK_PAYLOAD_BYTES) {
                  nextIndexOrChunkSize = bytesRemaining;
                }
                g_UiTransferMailbox.receivedRemainingBytes =
                     g_UiTransferMailbox.receivedRemainingBytes - nextIndexOrChunkSize;
                receivedChunkSourceDwords = (uint32_t *)(ringRecord->payload + 8);
                receivedChunkDestinationDwords =
                     (uint32_t *)((uint8_t *)g_UiTransferMailbox.receivedAllocation + counterOrOffset);
                for (nextIndexOrChunkSize = nextIndexOrChunkSize >> 2; nextIndexOrChunkSize != 0; nextIndexOrChunkSize--) {
                  *receivedChunkDestinationDwords = *receivedChunkSourceDwords;
                  receivedChunkSourceDwords++;
                  receivedChunkDestinationDwords++;
                }
                if (g_UiTransferMailbox.receivedRemainingBytes != 0) {
                  /* request the next chunk */
                  g_UiTransferMailbox.receiveRetryTicks = UI_TRANSFER_CHUNK_RETRY_TICKS;
                  g_UiTransferMailboxChunkOffset =
                       g_UiTransferMailbox.receivedByteCount -
                       g_UiTransferMailbox.receivedRemainingBytes;
                  /* chunk packet header = FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST: type 0x31, 1 unit */
                  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[0] = UI_TRANSFER_CHUNK_REQUEST_TYPE_BYTE;
                  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[1] = 0;
                  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[2] = 1;
                  UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[3] = 0;
                  g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
                  UiTransfer_StagePacketAndSend
                            ((UiTransferEndpointDescriptor *)senderEndpointSlot,
                             (UiTransferPacketHeader *)UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES);
                }
              }
            }
          }
        }
        else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST) {
          /* host: a player requests the chunk at the offset in the payload of the outgoing transfer */
          if (g_UiTransferMailbox.outgoingAllocation != NULL) {
            playersRemaining = g_FrontendPlayerRuntimeBlockCount;
            playerRecord = g_FrontendPlayerRuntimeBlocks;
            while ((ringRecord->packetHeader.sequenceToken != playerRecord->peerSequenceToken ||
                   (senderEndpointSlot->endpoint.ipv4AddressNetworkOrder !=
                    playerRecord->endpoint.ipv4AddressNetworkOrder))) {
              playerRecord++;
              playersRemaining--;
              if (playersRemaining == 0) goto nextRecord; /* not a player: drop the request */
            }
            /* original quirk: ESI (receive scratch slot) instead of EBX (player record), see above */
            senderEndpointSlot->transferTimeoutTicks =
                 senderEndpointSlot->transferTimeoutTicks + UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS;
            g_UiTransferMailboxChunkOffset = *(UiTransferMailboxByteOffset *)ringRecord->payload;
            playerRecord->transferProgressBytes = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
            g_UiTransferMailboxTransferByteCount = g_UiTransferMailbox.outgoingByteCount;
            playerRecord->transferProgressBytes = playerRecord->transferProgressBytes + g_UiTransferMailboxChunkOffset;
            /* chunk packet header = FRONTEND_PACKET_80030_MAILBOX_CHUNK: type 0x30, 8 units (0x100 bytes) */
            UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[0] = UI_TRANSFER_CHUNK_TYPE_BYTE;
            UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[1] = 0;
            UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[2] = 8;
            UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[3] = 0;
            bytesRemaining = g_UiTransferMailboxTransferByteCount - g_UiTransferMailboxChunkOffset;
            nextIndexOrChunkSize = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
            if (bytesRemaining < UI_TRANSFER_CHUNK_PAYLOAD_BYTES) {
              nextIndexOrChunkSize = bytesRemaining;
            }
            mailboxCopySourceOrDestinationDwords =
                 (uint32_t *)((uint8_t *)g_UiTransferMailbox.outgoingAllocation + g_UiTransferMailboxChunkOffset);
            chunkPayloadCursor = g_UiTransferChunkPayload;
            for (nextIndexOrChunkSize = nextIndexOrChunkSize >> 2; nextIndexOrChunkSize != 0; nextIndexOrChunkSize--) {
              *chunkPayloadCursor = *mailboxCopySourceOrDestinationDwords;
              mailboxCopySourceOrDestinationDwords++;
              chunkPayloadCursor++;
            }
            g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
            UiTransfer_StagePacketAndSend
                      ((UiTransferEndpointDescriptor *)senderEndpointSlot,
                       (UiTransferPacketHeader *)UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES);
          }
        }
        else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10032_PING) {
          /* ping: echo the sender's tick count back as 0x10033 */
          g_UiTransferMailboxReplyPacket10033EchoedTick = *(uint32_t *)ringRecord->payload;
          g_UiTransferPingEchoPacket = FRONTEND_PACKET_10033_PING_ECHO;
          g_UiTransferMailboxReplyPacket10033SequenceToken = g_UiTransferSequenceToken;
          UiTransfer_StagePacketAndSend
                    ((UiTransferEndpointDescriptor *)senderEndpointSlot,
                     (UiTransferPacketHeader *)&g_UiTransferPingEchoPacket);
        }
        else if (ringRecord->packetHeader.packedTypeAndUnitCount == FRONTEND_PACKET_10033_PING_ECHO) {
          /* ping answer: store the round trip in ticks and "<ticks * 4>ms" (half the round trip) as text */
          counterOrOffset = g_FrontendPlayerRuntimeCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
          if (0 < g_FrontendPlayerRuntimeCount) {
            do {
              if ((ringRecord->packetHeader.sequenceToken == playerRecord->peerSequenceToken) &&
                 (senderEndpointSlot->endpoint.ipv4AddressNetworkOrder ==
                  playerRecord->endpoint.ipv4AddressNetworkOrder)) {
                counterOrOffset = g_UiTransferMailboxTickCounter - *(int *)ringRecord->payload;
                playerRecord->pingRoundTripTicks = counterOrOffset;
                latencyTextCursor = (uint8_t *)playerRecord->pingTextUtf16;
                slotIndexOrByteCount = g_WideNumberFormatUtf16
                                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,counterOrOffset * 4,(uint16_t *)latencyTextCursor);
                latencySuffixCursor = latencyTextCursor + slotIndexOrByteCount;
                latencySuffixCursor[0] = 'm'; /* UTF-16 "ms" and terminator */
                latencySuffixCursor[1] = 0;
                latencySuffixCursor[2] = 's';
                latencySuffixCursor[3] = 0;
                (latencyTextCursor + slotIndexOrByteCount + 4)[0] = 0;
                (latencyTextCursor + slotIndexOrByteCount + 4)[1] = 0;
                break;
              }
              playerRecord++;
              counterOrOffset--;
            } while (counterOrOffset != 0);
          }
        }
        else {
          /* keep the packet: advance the ring write index (256 slots) */
          g_UiRuntimeRecordWriteIndex = nextIndexOrChunkSize;
          if (UI_RUNTIME_RECORD_RING_LAST_INDEX < nextIndexOrChunkSize) {
            g_UiRuntimeRecordWriteIndex = 0;
          }
        }
      }
nextRecord: ;
    }
    /* no more data: re-request the missing chunk when the retry countdown expires */
    if (((g_UiTransferMailbox.receiveRetryTicks != 0) &&
        (g_UiTransferMailbox.receiveRetryTicks = g_UiTransferMailbox.receiveRetryTicks - 1,
        g_UiTransferMailbox.receiveRetryTicks == 0)) &&
       (g_UiTransferMailbox.receivedRemainingBytes != 0)) {
      g_UiTransferMailbox.receiveRetryTicks = UI_TRANSFER_CHUNK_RETRY_TICKS;
      g_UiTransferMailboxChunkOffset =
           g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
      UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[0] = UI_TRANSFER_CHUNK_REQUEST_TYPE_BYTE;
      UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[1] = 0;
      UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[2] = 1;
      UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES[3] = 0;
      g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,
                 (UiTransferPacketHeader *)UI_TRANSFER_CHUNK_PACKET_HEADER_BYTES);
    }
    g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
  }
  return;
}


/* Address: 0x0054ECB0.
   Client side of the host lobby: accepts the host's 0x40008 session packet (one player-list row and the
   player's name; a non-zero expected block count starts the session and switches to
   FRONTEND_NETWORK_STATE_CLIENT_STARTING) and the host's lobby command batches, whose commands it executes
   before answering with its own next queued command (packet 0x10011). Packets from other hosts or sessions
   are ignored.
*/
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint32_t commandHandlerIndex;
  int loopCount;
  uint32_t playerOrCommandCount;
  uint32_t *packetCursor;
  uint32_t *nameSourceCursor;
  uint32_t *playerRowCursor;
  FrontendPlayerNameUtf16 *nameDestinationCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  
  if (((packet->packet40008LobbyRosterSnapshot.header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_40008_SESSION_PLAYER_ROW) &&
      (g_FrontendSessionToken == packet->packet40008LobbyRosterSnapshot.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount = packet->packet40008LobbyRosterSnapshot.playerCount;
    if ((packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex < 8) && (playerOrCommandCount < 9)) {
      packetCursor = (uint32_t *)packet;
      playerRowCursor = (uint32_t *)(&g_FrontendPlayerListRows)
                        [packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex];
      g_FrontendPlayerRuntimeCount = playerOrCommandCount;
      /* the whole 0x80-byte packet becomes the player's list row */
      for (loopCount = 32; loopCount != 0; loopCount--) {
        *playerRowCursor = *packetCursor;
        packetCursor++;
        playerRowCursor++;
      }
      playerRecord = g_FrontendPlayerRuntimeBlocks +
               packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex;
      playerRecord->playerRuntimeId = packet->packet40008LobbyRosterSnapshot.selectedPlayerRuntimeId;
      /* the player's name at +0x38 */
      nameSourceCursor = packet->packet40008LobbyRosterSnapshot.playerDescriptorPayload;
      nameDestinationCursor = &playerRecord->playerName;
      for (loopCount = 10; loopCount != 0; loopCount--) {
        *(uint32_t *)nameDestinationCursor->textUtf16 = *nameSourceCursor;
        nameSourceCursor++;
        nameDestinationCursor = (FrontendPlayerNameUtf16 *)(nameDestinationCursor->textUtf16 + 2);
      }
      UiPointerList_InitializeColumnLayout
                (playerOrCommandCount,(void **)&g_FrontendPlayerListRows,
                 (UiPointerListControl *)FRONTEND_UI(frontendRuntime,clientLobbyPlayerList));
    }
    g_SessionTransferTimeoutTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
    /* the host starts the session: this many player snapshots follow */
    if (packet->packet40008LobbyRosterSnapshot.pendingSessionPlayerCount != 0) {
      g_FrontendPlayerRuntimeBlockCount = 0;
      g_FrontendExpectedPlayerRuntimeBlockCount =
           packet->packet40008LobbyRosterSnapshot.pendingSessionPlayerCount;
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(frontendRuntime,frontendPageStack));
      FrontendState_DispatchCode(1);
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_CLIENT_STARTING;
      FrontendTransfer_SendLobbyCommandAndSnapshotRequest();
      loopCount = 8;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      do {
        playerRecord->factionAssignment.roleStateFlags = 0;
        playerRecord->colourCycleFlags = 0;
        playerRecord->snapshotTransferFlags = 0;
        playerRecord++;
        loopCount--;
      } while (loopCount != 0);
    }
    return;
  }
  if ((((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
        FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount =
         packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
    do {
      /* command dword = handler offset << 8 | player id; offsets past the command handlers are ignored */
      commandHandlerIndex = packet->command10011Or10021.command.packedCommandAndPlayerId >> 8;
      if (commandHandlerIndex != 0) {
        CommandQueueHandlerProc *commandHandler =
             CommandDispatch_ResolveHandler
                       (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
        if (commandHandler != NULL) {
          (*commandHandler)
                    (packet->command10011Or10021.command.packedCommandAndPlayerId & 0xff,
                     packet->command10011Or10021.command.payload1,
                     packet->command10011Or10021.command.payload2,
                     packet->command10011Or10021.command.payload3);
        }
      }
      /* next 0x20-byte unit */
      packet = (FrontendTransferPacketUnion *)(&packet->command10011Or10021 + 1);
      playerOrCommandCount = playerOrCommandCount - 1;
    } while (playerOrCommandCount != 0);
    g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011_LOBBY_COMMAND;
    FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
    if (g_FrontendPacket10011Buffer.command.packedCommandAndPlayerId != 0) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
    }
    return;
  }
  return;
}


/* Address: 0x0054F680.
   Client side of the session start: executes a new command batch from the host (a repeated batch only
   re-sends the last 0x10011 answer), answers 0x10012 with 0x10013, removes a player on 0x10007, stores the
   next player snapshot (0x30005, which also seeds the random streams) and serves chunks of the local
   player's PCX preview on 0x10009. Only packets of the selected host and session count; CF is set only when
   a new command batch was executed.
*/
bool FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  UiTransferSenderContext batchSenderContext;
  uint32_t expectedBlockCount;
  uint32_t commandHandlerIndex;
  uint32_t commandCountOrPlayerIndex;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int copyCount;
  uint32_t *nextPlayerCursor;
  uint32_t *packetCursor;
  uint32_t *previewSourceCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *recordDwordCursor;
  uint8_t *chunkDestinationCursor;
  uint16_t *resolvedText;
  
  expectedBlockCount = g_FrontendExpectedPlayerRuntimeBlockCount;
  if ((((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
        FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    batchSenderContext = packet->packet10000Handshake.header.senderContext;
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* the same batch again: our answer got lost, repeat it */
    if (batchSenderContext == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    commandCountOrPlayerIndex =
         packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
    g_FrontendSelectedPlayerToken = batchSenderContext;
    do {
      /* command dword = handler offset << 8 | player id; offsets past the command handlers are ignored */
      commandHandlerIndex = packet->command10011Or10021.command.packedCommandAndPlayerId >> 8;
      if (commandHandlerIndex != 0) {
        CommandQueueHandlerProc *commandHandler =
             CommandDispatch_ResolveHandler
                       (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
        if (commandHandler != NULL) {
          (*commandHandler)
                    (packet->command10011Or10021.command.packedCommandAndPlayerId & 0xff,
                     packet->command10011Or10021.command.payload1,
                     packet->command10011Or10021.command.payload2,
                     packet->command10011Or10021.command.payload3);
        }
      }
      /* next 0x20-byte unit */
      packet = (FrontendTransferPacketUnion *)(&packet->command10011Or10021 + 1);
      commandCountOrPlayerIndex = commandCountOrPlayerIndex - 1;
    } while (commandCountOrPlayerIndex != 0);
    FrontendTransfer_SendLobbyCommandAndSnapshotRequest();
    g_FrontendTransferResponsePending = 1;
    return true;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10012_WAIT) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013_WAIT_ACK;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10013Buffer.header);
    return false;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_10007_PLAYER_REMOVAL) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if (packet->playerRemoval10007.removedPlayerToken == playerRecord->playerRuntimeId) {
        /* "player left" message with the name, then close the gap in the record array */
        resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_REMOVED);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText);
        FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
        if (playersRemaining - 1 != 0) {
          nextPlayerCursor = (uint32_t *)(playerRecord + 1);
          recordDwordCursor = (uint32_t *)playerRecord;
          for (copyCount = (playersRemaining - 1) * (sizeof(FrontendPlayerRuntimeRecord) / sizeof(uint32_t));
               copyCount != 0; copyCount--) {
            *recordDwordCursor = *nextPlayerCursor;
            nextPlayerCursor++;
            recordDwordCursor++;
          }
        }
        g_FrontendPlayerRuntimeBlockCount--;
        return false;
      }
      playerRecord++;
      playersRemaining--;
    } while (playersRemaining != 0);
    return false;
  }
  if ((((packet->packet10000Handshake.header.packedTypeAndUnitCount ==
         FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) &&
       (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
      (commandCountOrPlayerIndex = packet->packet30005PlayerSnapshot.playerIndex,
      g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) &&
     ((commandCountOrPlayerIndex < g_FrontendExpectedPlayerRuntimeBlockCount &&
      (commandCountOrPlayerIndex == g_FrontendPlayerRuntimeBlockCount)))) {
    Random_SetBothSeeds(packet->packet30005PlayerSnapshot.secondaryRandomSeed);
    Random_SelectSecondaryStream();
    g_FrontendPlayerRuntimeBlockCount++;
    packetCursor = (uint32_t *)packet;
    recordDwordCursor = (uint32_t *)(g_FrontendPlayerRuntimeBlocks + commandCountOrPlayerIndex);
    /* the first 0x60 bytes of the packet become the head of the player's record */
    for (copyCount = 24; copyCount != 0; copyCount--) {
      *recordDwordCursor = *packetCursor;
      packetCursor++;
      recordDwordCursor++;
    }
    resolvedText = TextResource_Resolve(TEXT_ID_NETWORK_PLAYER_ARRIVED);
    RichTextCommandStream_PatchPayloadBySelector
              (0,packet->packet30005PlayerSnapshot.playerDescriptorPayload,resolvedText);
    FrontendRecentTextHistory_InsertAndRebuild5(resolvedText);
    return false;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10009_SNAPSHOT_CHUNK_REQUEST) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         packet->packet10009SnapshotChunkRequest.snapshotChunkOffset;
    chunkDestinationCursor = g_FrontendPacket8000ABuffer.packet10009Buffer;
    previewSourceCursor = (uint32_t *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A_SNAPSHOT_CHUNK;
    /* 0xE8-byte chunks; the last one at 0x1220 has 0xE0 bytes (the preview is 0x1300 bytes) */
    copyCount = 58;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET) {
      copyCount = 56;
    }
    for (; copyCount != 0; copyCount--) {
      *(uint32_t *)chunkDestinationCursor = *previewSourceCursor;
      previewSourceCursor++;
      chunkDestinationCursor = chunkDestinationCursor + 4;
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


/* Address: 0x00545640.
   Frontend command handler FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE, queued by
   FrontendNetwork_HostTickCommandAndSnapshotTransfer once the host has published the packed player
   snapshots and executed on every peer: a client marks its receive mailbox unavailable, so it waits for
   the new transfer instead of reading an old one. The host and a local game do nothing.
*/
void FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t senderPlayerId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_MarkUnavailable();
  }
  return;
}


/* Address: 0x00545660.
   Frontend command handler 0x1710 (relative to FRONTEND_COMMAND_CODE_BASE), queued by a client in
   Frontend_MainLoop once it has unpacked the host's published player snapshots, and executed on every
   peer: marks that player FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY. On the host, once every player is
   marked, the published block is no longer needed: its allocation is freed and the outgoing mailbox
   emptied.
*/
void FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3)

{
  FrontendPlayerRuntimeBlockCount playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerRuntimeId == playerRecord->playerRuntimeId) {
      playerRecord->snapshotTransferFlags =
           playerRecord->snapshotTransferFlags | FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
      playersRemaining = g_FrontendPlayerRuntimeBlockCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
        return;
      }
      do {
        if ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
          return;
        }
        playersRemaining--;
        playerRecord = playerRecord + 1;
      } while (playersRemaining != 0);
      g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,NULL);
      return;
    }
    playerRecord = playerRecord + 1;
    playersRemaining--;
  } while (playersRemaining != 0);
  return;
}


/* Address: 0x0054E230.
   Sends the session discovery probe (0x10000 handshake with FRONTEND_PROTOCOL_MAGIC) to
   g_FrontendNetworkEndpointScratch, the address from the join dialog or the broadcast address. Hosts answer
   with a 0x50001 session advertisement. CF is the send result.
*/
bool UiTransfer_SendDiscoveryProbe(void)

{
  bool sendCarry;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic = FRONTEND_PROTOCOL_MAGIC;
  sendCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return sendCarry;
}


/* Address: 0x0054E470.
   Introduces the local player to the host (0x20002 player descriptor): the player name (20 UTF-16 units)
   whose last unit is replaced by flags: bit 0 = a 64x64 picture <name>.pcx was found (loaded into
   g_FrontendLocalPlayerPcxPreview), bit 8 = shown as "CD" in the lobby list (always set). CF is the send
   result.
*/
bool UiTransfer_SendPlayerDescriptor(void)

{
  int dwordCount;
  uint32_t *nameSourceCursor;
  uint32_t *payloadCursor;
  bool callCarry;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR;
  g_FrontendPacket20002Buffer.payloadByteCount = 64;
  nameSourceCursor = (void *)g_FrontendLocalPlayerNameUtf16;
  payloadCursor = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *payloadCursor = *nameSourceCursor;
    nameSourceCursor++;
    payloadCursor++;
  }
  /* the last name unit becomes the flags word */
  ((uint16_t *)payloadCursor)[-1] = 0;
  callCarry = PcxPreview_Load64x64PaletteAndPixels
                    (g_FrontendLocalPlayerPcxPreview,g_FrontendLocalPlayerNameUtf16);
  if (!callCarry) {
    ((uint16_t *)payloadCursor)[-1] |= FRONTEND_DESCRIPTOR_HAS_PICTURE;
  }
  ((uint16_t *)payloadCursor)[-1] |= FRONTEND_CAPABILITY_CD;
  callCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return callCarry;
}


/* Address: 0x0054E4E0.
   Host side of the lobby. Answers a discovery probe (0x10000) with the session advertisement (0x50001:
   title, host description, player count; joinable while the lobby is not full), admits a joining player
   (0x20002: new player-list row, lowest free player id, join ack 0x10003), stores a player's capability
   heartbeat (0x10006), and collects each player's next command (0x10011), broadcasting the non-empty ones
   as one lobby command batch that the host then executes itself.
   The lobby's player list is the frontend's hostLobbyPlayerList, the player limit the value of
   maxPlayersSlider.
*/
void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint16_t descriptorStatusBits;
  UiTransferJoinAvailability heartbeatValue;
  uint32_t packedCommand;
  uint16_t *resolvedText;
  uint32_t commandHandlerIndex;
  int rootNodeOrCount;
  uint32_t countFlagsOrId;
  int dwordCount;
  UiTransferEndpointDescriptor *endpointCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint32_t *joiningPlayerRecordDwordCursor;
  FrontendCommandPacketRecord *commandRecordCursor;
  FrontendCommandPacketRecord *batchCursor;
  uint32_t *copySource;
  uint32_t *copyDestination;
  
  rootNodeOrCount = g_FrontendRootNode;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10000_HANDSHAKE) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
    if (((packet->packet10000Handshake.protocolMagic == FRONTEND_PROTOCOL_MAGIC) &&
        ((packet->packet10000Handshake.header.sequenceToken & FRONTEND_SEQUENCE_TOKEN_HIGH_MASK) ==
         FRONTEND_SEQUENCE_TOKEN_HIGH_WORD)) &&
       (((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount < (uint32_t)((UiRangeSliderControl *)FRONTEND_UI(frontendRuntime,maxPlayersSlider))->value)) {
      g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
    }
    resolvedText = TextResource_Resolve(TEXT_ID_SESSION_TITLE_TEMPLATE);
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)THANDOR_ADDR(g_GameVersionUtf16,0),resolvedText);
    RichTextCommandStream_CopyExpanded
              (40,g_FrontendPacket50001Buffer.sessionTitleUtf16,resolvedText);
    resolvedText = TextResource_Resolve(TEXT_ID_SESSION_HOST_TEMPLATE);
    /* the game name typed into gameNameEdit */
    RichTextCommandStream_PatchPayloadBySelector
              (0,((UiTextEditControl *)FRONTEND_UI(rootNodeOrCount,gameNameEdit))->textBuffer,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendLocalPlayerNameUtf16,resolvedText);
    RichTextCommandStream_CopyExpanded
              (88,g_FrontendPacket50001Buffer.hostDescriptionUtf16,resolvedText);
    resolvedText = TextResource_Resolve(TEXT_ID_SESSION_PLAYER_COUNT_TEMPLATE);
    RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkRuntimeCountTextUtf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendNetworkPlayerCountTextUtf16,resolvedText);
    RichTextCommandStream_CopyExpanded(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,resolvedText);
    g_FrontendPacket50001Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT;
    g_FrontendPacket50001Buffer.payloadByteCount = 32;
    UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket50001Buffer.header);
    return;
  }
  if ((packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR) ||
     ((uint32_t)((UiRangeSliderControl *)FRONTEND_UI(frontendRuntime,maxPlayersSlider))->value <= ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount)) {
    if (packet->packet10000Handshake.header.packedTypeAndUnitCount !=
        FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT) {
      if (packet->packet10000Handshake.header.packedTypeAndUnitCount != FRONTEND_PACKET_10011_LOBBY_COMMAND) {
        return;
      }
      commandRecordCursor = g_FrontendPlayerCommandRecords;
      rootNodeOrCount = g_FrontendPlayerRuntimeCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken ||
             (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder
             ))) {
        playerRecord = playerRecord + 1;
        commandRecordCursor = commandRecordCursor + 1;
        rootNodeOrCount = rootNodeOrCount + -1;
        if (rootNodeOrCount == 0) {
          return;
        }
      }
      playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      /* keep the player's 0x20-byte command packet; the host's own command goes into slot 0 below */
      copySource = (uint32_t *)packet;
      copyDestination = (uint32_t *)commandRecordCursor;
      for (rootNodeOrCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); rootNodeOrCount != 0;
           rootNodeOrCount--) {
        *copyDestination = *copySource;
        copySource++;
        copyDestination++;
      }
      FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
      countFlagsOrId = 0;
      commandRecordCursor = g_FrontendPlayerCommandRecords;
      batchCursor = g_FrontendCommandBatchPacketBuffer;
      rootNodeOrCount = g_FrontendPlayerRuntimeCount;
      /* compact the slots holding a command into the batch and clear them (the player id stays) */
      do {
        if ((commandRecordCursor->command.packedCommandAndPlayerId & 0xffffff00) == 0) {
          commandRecordCursor++;
        }
        else {
          for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
            batchCursor->header.packedTypeAndUnitCount = commandRecordCursor->header.packedTypeAndUnitCount;
            commandRecordCursor = (FrontendCommandPacketRecord *)&commandRecordCursor->header.sequenceToken;
            batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
          }
          countFlagsOrId++;
          commandRecordCursor[-1].command.packedCommandAndPlayerId =
               commandRecordCursor[-1].command.packedCommandAndPlayerId & 0xff;
        }
        rootNodeOrCount--;
      } while (rootNodeOrCount != 0);
      if (countFlagsOrId << FRONTEND_PACKET_UNIT_COUNT_SHIFT != 0) {
        g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
             countFlagsOrId << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
        /* to every player but the host (record 0) */
        endpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        rootNodeOrCount = g_FrontendPlayerRuntimeCount;
        while (rootNodeOrCount = rootNodeOrCount - 1, rootNodeOrCount != 0) {
          UiTransfer_StagePacketAndSend(endpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
          endpointCursor = endpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
        }
        commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
        countFlagsOrId = countFlagsOrId & 0xffff;
        do {
          packedCommand = commandRecordCursor->command.packedCommandAndPlayerId;
          commandHandlerIndex = packedCommand >> 8;
          if (commandHandlerIndex != 0) {
            CommandQueueHandlerProc *commandHandler =
                 CommandDispatch_ResolveHandler
                           (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
            if (commandHandler != NULL) {
              (*commandHandler)
                        (packedCommand & 0xff,commandRecordCursor->command.payload1,
                         commandRecordCursor->command.payload2,commandRecordCursor->command.payload3);
            }
          }
          commandRecordCursor++;
          countFlagsOrId = countFlagsOrId - 1;
        } while (countFlagsOrId != 0);
      }
      return;
    }
    rootNodeOrCount = (int)((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken ||
           (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder))
          ) {
      playerRecord++;
      rootNodeOrCount--;
      if (rootNodeOrCount == 0) {
        return;
      }
    }
    countFlagsOrId = packet->packet10006CapabilityHeartbeat.capabilityFlags;
    heartbeatValue = packet->packet10006CapabilityHeartbeat.heartbeatExpiryTicks;
    playerRecord->capabilityFlags = countFlagsOrId;
    playerRecord->heartbeatExpiryTicks = heartbeatValue;
    playerRecord->capabilityLabelUtf16[0] = 0;
    playerRecord->capabilityLabelUtf16[1] = 0;
    playerRecord->capabilityLabelUtf16[2] = 0;
    playerRecord->capabilityLabelUtf16[3] = 0;
    if ((countFlagsOrId & FRONTEND_CAPABILITY_CD) != 0) {
      /* L"CD" */
      playerRecord->capabilityLabelUtf16[0] = 'C';
      playerRecord->capabilityLabelUtf16[1] = 0;
      playerRecord->capabilityLabelUtf16[2] = 'D';
      playerRecord->capabilityLabelUtf16[3] = 0;
    }
    FrontendPlayerRuntime_UpdateStartButtonByCdShare();
    return;
  }
  joiningPlayerRecordDwordCursor =
       (uint32_t *)((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowSlots[((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount];
  ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount = ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount + 1;
  /* the new row: the 0x40-byte descriptor packet followed by the sender's 0x10-byte endpoint */
  for (rootNodeOrCount = 16; rootNodeOrCount != 0; rootNodeOrCount--) {
    *joiningPlayerRecordDwordCursor = packet->packet10000Handshake.header.packedTypeAndUnitCount;
    packet = (FrontendTransferPacketUnion *)&packet->packet10000Handshake.header.sequenceToken;
    joiningPlayerRecordDwordCursor++;
  }
  endpointCursor = senderEndpoint;
  for (rootNodeOrCount = 4; rootNodeOrCount != 0; rootNodeOrCount--) {
    *joiningPlayerRecordDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, uint32_t, endpointCursor->addressHeader);
    endpointCursor = (UiTransferEndpointDescriptor *)&endpointCursor->ipv4AddressNetworkOrder;
    joiningPlayerRecordDwordCursor++;
  }
  /* Smallest player runtime id below 0xFF that no current player uses (0xFF when all are taken). */
  countFlagsOrId = 0;
  rootNodeOrCount = g_FrontendPlayerRuntimeCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    while (countFlagsOrId != playerRecord->playerRuntimeId) {
      rootNodeOrCount--;
      playerRecord++;
      if (rootNodeOrCount == 0) goto freeIdFound;
    }
    countFlagsOrId++;
    rootNodeOrCount = g_FrontendPlayerRuntimeCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (countFlagsOrId < 255);
freeIdFound:
  descriptorStatusBits = *(uint16_t *)((int)joiningPlayerRecordDwordCursor + -18);
  *joiningPlayerRecordDwordCursor = 1;
  joiningPlayerRecordDwordCursor[-15] = countFlagsOrId;
  joiningPlayerRecordDwordCursor[6] = descriptorStatusBits & 0xff;
  joiningPlayerRecordDwordCursor[9] = descriptorStatusBits & 0xff00;
  joiningPlayerRecordDwordCursor[16] = 0;
  joiningPlayerRecordDwordCursor[17] = 0;
  joiningPlayerRecordDwordCursor[7] = 0;
  joiningPlayerRecordDwordCursor[10] = 0;
  joiningPlayerRecordDwordCursor[11] = 0;
  joiningPlayerRecordDwordCursor[13] = 0;
  joiningPlayerRecordDwordCursor[14] = 0;
  joiningPlayerRecordDwordCursor[15] = 0;
  if ((descriptorStatusBits & FRONTEND_CAPABILITY_CD) != 0) {
    joiningPlayerRecordDwordCursor[10] = 0x440043; /* L"CD" */
  }
  *(uint16_t *)((int)joiningPlayerRecordDwordCursor + -18) = 0;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount,
             (uint16_t *)&g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = countFlagsOrId;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10003Buffer.header);
  g_FrontendPlayerRuntimeCount++;
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
  return;
}


/* Address: 0x0054E9B0.
   Host lobby tick. Sends every joined player the session packet 0x40008 for one player-list row, chosen
   round robin (with its row, name and ping text "<n>ms"), plus a 0x10032 tick stamp; a pending session start
   switches to FRONTEND_NETWORK_STATE_HOST_STARTING. Then it adds the host's own next queued command to the
   players' collected ones, broadcasts the non-empty ones as one lobby command batch and executes them.
*/
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint32_t roundRobinOrTextLength;
  uint32_t commandHandlerIndex;
  int remainingCount;
  int dwordCount;
  uint32_t selectedIndexOrPackedCommand;
  uint32_t playerOrCommandCount;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  uint8_t *descriptorSourceCursor;
  UiTransferEndpointDescriptor *endpoint;
  FrontendCommandPacketRecord *commandRecordCursor;
  uint32_t *descriptorDestinationCursor;
  FrontendCommandPacketRecord *batchCursor;
  
  roundRobinOrTextLength = g_FrontendHostPublishRoundRobinCounter;
  playerOrCommandCount = ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount;
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  remainingCount = playerOrCommandCount - 1;
  if (remainingCount != 0 && 0 < (int)playerOrCommandCount) {
    g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008_SESSION_PLAYER_ROW;
    g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
    g_FrontendHostPublishRoundRobinCounter++;
    selectedIndexOrPackedCommand = roundRobinOrTextLength % playerOrCommandCount;
    /* the selected player's record fields, addressed from the record-1 endpoint in 0x10-byte steps: the unit at
       heartbeatExpiryTicks gives playerRuntimeId (+4) and playerName (+8), the one at transferProgressBytes
       capabilityLabelUtf16 (+8), and pingRoundTripTicks is read directly */
    g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
         peerEndpointCursor[(selectedIndexOrPackedCommand - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
             FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks)].ipv4AddressNetworkOrder;
    g_FrontendPacket40008Buffer.selectedStatusCode0 =
         *(FrontendStatusCode *)peerEndpointCursor[(selectedIndexOrPackedCommand - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
             FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes)].zeroPadding;
    g_FrontendPacket40008Buffer.selectedStatusCode1 =
         *(FrontendStatusCode *)(peerEndpointCursor[(selectedIndexOrPackedCommand - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
             FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes)].zeroPadding + 4);
    g_FrontendPacket40008Buffer.selectedPlayerIndex = selectedIndexOrPackedCommand;
    g_FrontendPacket40008Buffer.playerCount = playerOrCommandCount;
    endpoint = peerEndpointCursor;
    roundRobinOrTextLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                       peerEndpointCursor[(selectedIndexOrPackedCommand - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
             FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(pingRoundTripTicks)].addressHeader.packedFamilyAndPort << 2,
                       g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
    *(uint32_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength) =
         ('s' << 16 | 'm'); /* L"ms" */
    *(uint16_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength + 4) = 0;
    descriptorSourceCursor = peerEndpointCursor[(selectedIndexOrPackedCommand - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
             FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks)].zeroPadding;
    descriptorDestinationCursor = g_FrontendPacket40008Buffer.playerDescriptorPayload;
    for (dwordCount = 10; dwordCount != 0; dwordCount--) {
      *descriptorDestinationCursor = *(uint32_t *)descriptorSourceCursor;
      descriptorSourceCursor = descriptorSourceCursor + 4;
      descriptorDestinationCursor++;
    }
    g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032_PING;
    g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
    /* to every player but the host (record 0) */
    do {
      UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket40008Buffer.header);
      UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket10032Buffer.header);
      endpoint = endpoint + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
      remainingCount--;
    } while (remainingCount != 0);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOST_STARTING;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  playerOrCommandCount = 0;
  commandRecordCursor = g_FrontendPlayerCommandRecords;
  batchCursor = g_FrontendCommandBatchPacketBuffer;
  remainingCount = g_FrontendPlayerRuntimeCount;
  /* compact the slots holding a command into the batch and clear them (the player id stays) */
  do {
    if ((commandRecordCursor->command.packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor++;
    }
    else {
      for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
        batchCursor->header.packedTypeAndUnitCount = commandRecordCursor->header.packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&commandRecordCursor->header.sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
      }
      playerOrCommandCount++;
      commandRecordCursor[-1].command.packedCommandAndPlayerId =
           commandRecordCursor[-1].command.packedCommandAndPlayerId & 0xff;
    }
    remainingCount--;
  } while (remainingCount != 0);
  if (playerOrCommandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT != 0) {
    g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
         playerOrCommandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
    peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
    remainingCount = g_FrontendPlayerRuntimeCount;
    while (remainingCount = remainingCount - 1, remainingCount != 0) {
      UiTransfer_StagePacketAndSend(peerEndpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
      peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
    }
    commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
    playerOrCommandCount = playerOrCommandCount & 0xffff;
    do {
      selectedIndexOrPackedCommand = commandRecordCursor->command.packedCommandAndPlayerId;
      commandHandlerIndex = selectedIndexOrPackedCommand >> 8;
      if (commandHandlerIndex != 0) {
        CommandQueueHandlerProc *commandHandler =
             CommandDispatch_ResolveHandler
                       (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
        if (commandHandler != NULL) {
          (*commandHandler)
                    (selectedIndexOrPackedCommand & 0xff,commandRecordCursor->command.payload1,commandRecordCursor->command.payload2,
                     commandRecordCursor->command.payload3);
        }
      }
      commandRecordCursor++;
      playerOrCommandCount = playerOrCommandCount - 1;
    } while (playerOrCommandCount != 0);
  }
  return;
}


/* Address: 0x0054EEF0.
   Sends the client's capability heartbeat (0x10006) to the selected host: the CD capability and a heartbeat
   value of 0x40, which the host stores in this player's record.
*/
void FrontendTransfer_SendCapabilityHeartbeat(void)

{
  g_FrontendPacket10006Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT;
  g_FrontendPacket10006Buffer.capabilityFlags = FRONTEND_CAPABILITY_CD;
  g_FrontendPacket10006Buffer.heartbeatExpiryTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10006Buffer.header);
  return;
}


/* Address: 0x005723F0.
   Host side of the in-game command exchange. When every client (player records 1..n-1) has submitted its
   command, clears their ready flags, takes the host's own next command into slot 0, packs all non-empty
   command slots into g_FrontendClientCommandBatchPacketBuffer (at least one record) and sends that
   COMMAND_BATCH to every client; returns false (CF clear). Otherwise returns true (CF set) and, with
   notifyWaitingPeers, resends the previous batch to clients that have not submitted yet and COMMAND_WAIT
   to those that have.
*/
bool FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers)

{
  FrontendPlayerRuntimeBlockCount peersRemaining;
  int remainingOrBatchCount;
  int dwordCount;
  FrontendCommandPacketRecord *commandRecordCursor;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *batchCursor;

  /* playerRecord[1] below: record 0 is the host itself, only the clients are checked */
  remainingOrBatchCount = g_FrontendPlayerRuntimeBlockCount - 1;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (remainingOrBatchCount != 0) {
    do {
      if (playerRecord[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        if (notifyWaitingPeers != 0) {
          peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
          peersRemaining = g_FrontendPlayerRuntimeBlockCount;
          while (peersRemaining--, peersRemaining != 0) {
            /* peerEndpointCursor[1] is the 0x10 bytes after the endpoint, i.e. the same record's
               commandSyncPending (+0x50) */
            if (peerEndpointCursor[1].addressHeader.packedFamilyAndPort == 0) {
              UiTransfer_StagePacketAndSend
                        (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
            }
            else {
              g_FrontendPacket10022Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_WAIT;
              UiTransfer_StagePacketAndSend(peerEndpointCursor,&g_FrontendPacket10022Buffer.header);
            }
            /* 0x13B endpoints of 0x10 bytes = one 0x13B0-byte player record */
            peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
          }
        }
        return true;
      }
      remainingOrBatchCount--;
      playerRecord++;
    } while (remainingOrBatchCount != 0);
    remainingOrBatchCount = g_FrontendPlayerRuntimeBlockCount - 1;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      playerRecord[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      remainingOrBatchCount--;
      playerRecord++;
    } while (remainingOrBatchCount != 0);
  }
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  remainingOrBatchCount = 0;
  commandRecordCursor = g_FrontendClientPlayerCommandRecords;
  batchCursor = g_FrontendClientCommandBatchPacketBuffer;
  peersRemaining = g_FrontendPlayerRuntimeBlockCount;
  /* Pack every non-empty 0x20-byte command slot (handler offset != 0) into the batch (REP MOVSD). */
  do {
    if ((commandRecordCursor->command.packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor++;
    }
    else {
      for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
        batchCursor->header.packedTypeAndUnitCount = commandRecordCursor->header.packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&commandRecordCursor->header.sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
      }
      remainingOrBatchCount++;
    }
    peersRemaining--;
  } while (peersRemaining != 0);
  if (remainingOrBatchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    /* Nothing pending: send the first record (the host's, empty) as a batch of one. */
    commandRecordCursor = g_FrontendClientPlayerCommandRecords;
    for (dwordCount = sizeof(FrontendCommandPacketRecord) / sizeof(uint32_t); dwordCount != 0; dwordCount--) {
      batchCursor->header.packedTypeAndUnitCount = commandRecordCursor->header.packedTypeAndUnitCount;
      commandRecordCursor = (FrontendCommandPacketRecord *)&commandRecordCursor->header.sequenceToken;
      batchCursor = (FrontendCommandPacketRecord *)&batchCursor->header.sequenceToken;
    }
    remainingOrBatchCount = 1;
  }
  /* the first packed record's header doubles as the batch header */
  g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       remainingOrBatchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_COMMAND_BATCH_TYPE;
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  peersRemaining = g_FrontendPlayerRuntimeBlockCount;
  while (peersRemaining--, peersRemaining != 0) {
    UiTransfer_StagePacketAndSend
              (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
    peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
  return false;
}


/* Address: 0x00572920.
   Client side of the lockstep exchange: sends the host its next in-game command (FRONTEND_PACKET_COMMAND_SUBMIT)
   with the oldest queued command, or an empty record when none is queued. The sender context counts the
   submissions.
*/
void FrontendTransfer_SendCommandSubmit(void)

{
  g_FrontendPacket10021Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_COMMAND_SUBMIT;
  g_UiTransferSenderContext++;
  InGameCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10021Buffer);
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
  return;
}


/* Address: 0x004AF110.
   Empties the receive side of the transfer mailbox (allocation, byte count, remaining bytes, retry ticks) so a
   new transfer can be received; the outgoing buffer is left alone. Consumers call it after taking a buffer.
*/
void UiTransferMailbox_ClearReceivedState(void)

{
  g_UiTransferMailbox.receivedAllocation = NULL;
  g_UiTransferMailbox.receivedByteCount = 0;
  g_UiTransferMailbox.receivedRemainingBytes = 0;
  g_UiTransferMailbox.receiveRetryTicks = 0;
  return;
}


/* Address: 0x004AF170.
   Hands out a completely received transfer: returns its buffer (EAX) and byte count (ECX) with CF clear once an
   allocation exists and no bytes are outstanding. An empty, unavailable or still incomplete mailbox sets CF.
*/
MailboxReceiveResult UiTransferMailbox_GetReceivedBuffer(void)

{
  MailboxReceiveResult receivedResult;
  MailboxReceiveResult unavailableResult;

  if (g_UiTransferMailbox.receivedAllocation != UI_TRANSFER_MAILBOX_UNAVAILABLE &&
      g_UiTransferMailbox.receivedAllocation != NULL &&
      g_UiTransferMailbox.receivedRemainingBytes == 0) {
    receivedResult.byteCount = g_UiTransferMailbox.receivedByteCount;
    receivedResult.buffer = (uint32_t)g_UiTransferMailbox.receivedAllocation;
    receivedResult.unavailable = false;
    return receivedResult;
  }
  /* Unavailable (CF set): the asm leaves the caller's EAX/ECX untouched; every caller reads them only with
     CF clear. */
  unavailableResult.byteCount = 0;
  unavailableResult.buffer = 0;
  unavailableResult.unavailable = true;
  return unavailableResult;
}


/* Address: 0x004AF1C0.
   Gives this machine a new random session identity before it opens or looks for a session: XORs a random
   16-bit value into the low word of the transfer sequence token. The high word stays (a host answers the
   discovery probe only for 0x1234).
*/
void UiTransferMailbox_RandomizeSequenceToken(void)

{
  uint32_t randomValue;
  
  randomValue = Random_NextPrimary();
  g_UiTransferSequenceToken = g_UiTransferSequenceToken ^ randomValue & 0xffff;
  return;
}


/* Address: 0x0054E260.
   Network game page (browsing): a session advertisement (0x50001) updates its row in the session list or
   appends one (at most 0x20 sessions); the join ack (0x10003) from the selected host takes over the assigned
   player id and network tick interval, switches to the host-lobby page and FRONTEND_NETWORK_STATE_JOINED and
   marks this machine as a network client.
*/
void FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int remainingCount;
  FrontendSessionDiscoveryRecord **sessionRowCursor;
  uint32_t *sessionDiscoveryRecordDwordCursor;
  uint32_t *playerRowCursor;
  
  remainingCount = (int)((UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList))->rowCount;
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    sessionDiscoveryRecordDwordCursor = (uint32_t *)g_FrontendSessionDiscoveryRecords;
    sessionRowCursor = g_FrontendSessionListRows;
    for (; remainingCount != 0; remainingCount--) {
      if ((packet->packet10000Handshake.header.sequenceToken ==
           (((FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor)->advertisement).
           header.sequenceToken) &&
         (senderEndpoint->ipv4AddressNetworkOrder ==
          (((FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor)->senderEndpoint).
          ipv4AddressNetworkOrder)) break;
      sessionRowCursor++;
      sessionDiscoveryRecordDwordCursor =
           (uint32_t *)((FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor + 1);
    }
    /* Update the known session in place, or append it while the list has fewer than 0x20 rows. */
    if ((remainingCount != 0) || (((UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList))->rowCount < FRONTEND_SESSION_LIST_CAPACITY)) {
      if (remainingCount == 0) {
        *sessionRowCursor = (FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor;
        ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList))->rowCount = ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList))->rowCount + 1;
      }
      /* the payload byte count is overwritten with 0x20 before the packet is stored */
      packet->packet50001SessionAdvertisement.payloadByteCount = 32;
      /* the 0xA0-byte advertisement followed by the sender's 0x10-byte endpoint */
      for (remainingCount = 40; remainingCount != 0; remainingCount--) {
        (((FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor)->advertisement).
        header.packedTypeAndUnitCount = packet->packet10000Handshake.header.packedTypeAndUnitCount
        ;
        packet = (FrontendTransferPacketUnion *)&packet->packet10000Handshake.header.sequenceToken
        ;
        sessionDiscoveryRecordDwordCursor =
             &(((FrontendSessionDiscoveryRecord *)sessionDiscoveryRecordDwordCursor)->
              advertisement).header.sequenceToken;
      }
      for (remainingCount = 4; remainingCount != 0; remainingCount--) {
        *sessionDiscoveryRecordDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, uint32_t, senderEndpoint->addressHeader);
        senderEndpoint = (UiTransferEndpointDescriptor *)&senderEndpoint->ipv4AddressNetworkOrder;
        sessionDiscoveryRecordDwordCursor++;
      }
      UiPointerList_RefreshSelectionAndQueueAction
                ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList));
    }
  }
  else if (((packet->packet10000Handshake.header.packedTypeAndUnitCount ==
             FRONTEND_PACKET_10003_JOIN_ACK) &&
           (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken)) &&
          (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
           senderEndpoint->ipv4AddressNetworkOrder)) {
    g_LocalPlayerRuntimeId = packet->packet10003JoinAck.assignedPlayerRuntimeId;
    g_SessionNetworkTickInterval = packet->packet10003JoinAck.networkTickInterval;
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_CLIENT_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendRuntime,frontendPageStack));
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_JOINED;
    g_SessionTransferTimeoutTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_CLIENT;
    playerRowCursor = (uint32_t *)g_FrontendPlayerListRows;
    for (remainingCount = 256; remainingCount != 0; remainingCount--) {
      *playerRowCursor = 0;
      playerRowCursor++;
    }
    UiPointerList_InitializeColumnLayout
              (0,(void **)&g_FrontendPlayerListRows,
               (UiPointerListControl *)FRONTEND_UI(frontendRuntime,clientLobbyPlayerList));
    return;
  }
  return;
}


/* Address: 0x0054EF30.
   Host timeout of a client in the host's lobby, called by FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState
   while g_FrontendNetworkState is FRONTEND_NETWORK_STATE_JOINED: when g_SessionTransferTimeoutTicks runs out
   (nothing heard from the host), the client leaves as if its lobby Leave button had been pressed and goes
   back to the session list.
*/
void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRoot)

{
  g_SessionTransferTimeoutTicks--;
  if (g_SessionTransferTimeoutTicks == 0) {
    FrontendTransferPage_ResetSessionOpenAndRequestMailbox(FRONTEND_UI(frontendRoot,clientLobbyLeaveButton));
  }
  return;
}


/* Address: 0x0054FBA0.
   Frontend copy of FrontendTransfer_ConsumeProcessedFlag: atomically takes and clears
   g_FrontendTransferResponsePending (set by FrontendTransfer_HandleGameplayCommandAndRosterPackets after a new
   command batch). Returns true (CF set) when no batch arrived, so Frontend_StateTick ends its tick early.
*/
bool FrontendTransfer_ConsumeProcessedFlagForMenuTick(void)

{
  int previousFlag;

  /* XCHG in the original: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}


/* Address: 0x005722C0.
   Host side of the in-game command exchange: finds the player the packet came from (sequence token and
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
    /* copy the whole 0x20-byte packet, header included, into the slot (REP MOVSD) */
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


/* Address: 0x00572560.
   Host side: executes the command batch it has just broadcast (g_FrontendClientCommandBatchPacketBuffer) on
   the local simulation, so host and clients run the same commands in the same tick. The high 24 bits of
   each packed command are the handler's offset from InGameCommandQueue_AppendLocalPlayerCommand, the low
   8 bits the player id; offsets beyond the handler code region are ignored. The original handler address
   is resolved to its recovered C function by CommandDispatch_ResolveHandler.
*/
void FrontendTransfer_DispatchStagedCommandRecords(void)

{
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  uint32_t remainingCount;
  FrontendCommandPacketRecord *commandRecord;

  commandRecord = g_FrontendClientCommandBatchPacketBuffer;
  for (remainingCount = g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >>
                        FRONTEND_PACKET_UNIT_COUNT_SHIFT;
      remainingCount != 0; remainingCount--) {
    packedCommand = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      CommandQueueHandlerProc *commandHandler =
           CommandDispatch_ResolveHandler
                     (INGAME_COMMAND_CODE_BASE,INGAME_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL)
      {
        (*commandHandler)
                  (packedCommand & 0xff,commandRecord->command.payload1,commandRecord->command.payload2,
                   commandRecord->command.payload3);
      }
    }
    commandRecord++;
  }
  return;
}


/* Address: 0x00572AA0.
   Client side: atomically takes and clears g_FrontendTransferResponsePending, which
   FrontendNetwork_HandleCommandBatchAndPlayerTimeout sets after executing a new command batch. Returns true
   (CF set) when no batch arrived, so the in-game tick waits for the host instead of advancing the simulation.
*/
bool FrontendTransfer_ConsumeProcessedFlag(void)

{
  int previousFlag;

  /* XCHG in the original: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}


/* Address: 0x00407160.
   Encrypts an outgoing packet: byteCount/8 64-bit blocks in CBC mode (each input block is XORed with the
   previous output block, starting from zero), each through 16 rounds keyed by roundKeys16 and the eight
   nibble substitution tables at 0x00403160. UiTransfer_DecryptPacketBlocks is the
   matching decryption used on receive.
*/
void UiTransfer_EncryptPacketBlocks(uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
          uint32_t *inputBlocks)

{
  uint32_t *roundKeyNibble0;
  uint32_t *roundKeyNibble1;
  uint32_t *roundKeyNibble2;
  uint32_t *roundKeyNibble3;
  uint32_t *roundKeyNibble4;
  uint32_t *roundKeyNibble5;
  uint32_t *roundKeyNibble6;
  uint32_t *roundKeyNibble7;
  uint32_t roundInputHalf;
  uint32_t roundIndex;
  uint32_t blocksRemaining;
  uint32_t leftState;
  uint32_t rightState;
  
  blocksRemaining = byteCount >> 3;
  if (blocksRemaining != 0) {
    rightState = 0;
    leftState = 0;
    do {
      roundIndex = 0;
      roundInputHalf = *inputBlocks ^ leftState;
      rightState = inputBlocks[1] ^ rightState;
      do {
        leftState = rightState;
        roundKeyNibble0 = roundKeys16 + roundIndex;
        roundKeyNibble1 = roundKeys16 + roundIndex;
        roundKeyNibble2 = roundKeys16 + roundIndex;
        roundKeyNibble3 = roundKeys16 + roundIndex;
        roundKeyNibble4 = roundKeys16 + roundIndex;
        roundKeyNibble5 = roundKeys16 + roundIndex;
        roundKeyNibble6 = roundKeys16 + roundIndex;
        roundKeyNibble7 = roundKeys16 + roundIndex;
        roundIndex++;
        /* one nibble per table: table n, row = key nibble n, column = input nibble n */
        /* byte offsets into the uint32_t[16][16] tables: row = key nibble * 64, column = input nibble * 4 */
        rightState =(((((((*(int *)((uint8_t *)g_UiTransferEncryptSbox0 +
                                    (roundInputHalf & 0xf) * 4 + (*roundKeyNibble0 & 0xf) * UI_TRANSFER_CIPHER_ROW_BYTES) << 4 |
                           *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox1 +
                                    ((roundInputHalf & 0xf0) >> 4) * 4 + (*roundKeyNibble1 & 0xf0) * 4)) << 4
                          | *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox2 +
                                     ((roundInputHalf & 0xf00) >> 8) * 4 + ((*roundKeyNibble2 & 0xf00) >> 2))
                          ) << 4 | *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox3 +
                                            ((roundInputHalf & 0xf000) >> 12) * 4 +
                                            ((*roundKeyNibble3 & 0xf000) >> 6))) << 4 |
                        *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox4 +
                                 ((roundInputHalf & 0xf0000) >> 16) * 4 +
                                 ((*roundKeyNibble4 & 0xf0000) >> 10))) << 4 |
                       *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox5 +
                                ((roundInputHalf & 0xf00000) >> 20) * 4 +
                                ((*roundKeyNibble5 & 0xf00000) >> 14))) << 4 |
                      *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox6 +
                               ((roundInputHalf & 0xf000000) >> 24) * 4 +
                               ((*roundKeyNibble6 & 0xf000000) >> 18))) << 4 |
                     *(uint32_t *)((uint8_t *)g_UiTransferEncryptSbox7 +
                              (roundInputHalf >> 28) * 4 + ((*roundKeyNibble7 & 0xf0000000) >> 22))) ^
                     leftState;
        roundInputHalf = leftState;
      } while (roundIndex < 16);
      *outputBlocks = leftState;
      outputBlocks[1] = rightState;
      inputBlocks = inputBlocks + 2;
      outputBlocks = outputBlocks + 2;
      blocksRemaining--;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x004072F0.
   Decrypts a received packet in place or into destination (they may alias): the inverse of
   UiTransfer_EncryptPacketBlocks, running the 16 rounds backwards with the second table set
   (g_UiTransferDecryptSboxes, 0x00405160) and XORing each result with the previous ciphertext block (CBC).
*/
void UiTransfer_DecryptPacketBlocks
          (uint32_t *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source)

{
  uint32_t leftHalf;
  int roundIndex;
  uint32_t rightHalf;
  uint32_t savedHalf;
  uint32_t blocksRemaining;
  uint32_t previousCipherLow;
  uint32_t previousCipherHigh;
  
  blocksRemaining = byteCount >> 3;
  if (blocksRemaining != 0) {
    previousCipherHigh = 0;
    previousCipherLow = 0;
    do {
      roundIndex = 15;
      leftHalf = *(uint32_t *)source;
      rightHalf = ((uint32_t *)source)[1];
      do {
        savedHalf = leftHalf;
        rightHalf = rightHalf ^ savedHalf;
        leftHalf = ((((((*(int *)(((roundKeys16[roundIndex] & 0xf0000000) >> 22) + THANDOR_ADDR(g_UiTransferDecryptSboxes,7 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                              (rightHalf & 0xf) * 4) << 4 |
                     *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000000) >> 18) + THANDOR_ADDR(g_UiTransferDecryptSboxes,6 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                              ((rightHalf & 0xf0) >> 4) * 4)) << 4 |
                    *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00000) >> 14) + THANDOR_ADDR(g_UiTransferDecryptSboxes,5 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                             ((rightHalf & 0xf00) >> 8) * 4)) << 4 |
                   *(uint32_t *)(((roundKeys16[roundIndex] & 0xf0000) >> 10) + THANDOR_ADDR(g_UiTransferDecryptSboxes,4 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                            ((rightHalf & 0xf000) >> 12) * 4)) << 4 |
                  *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000) >> 6) + THANDOR_ADDR(g_UiTransferDecryptSboxes,3 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                           ((rightHalf & 0xf0000) >> 16) * 4)) << 4 |
                 *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00) >> 2) + THANDOR_ADDR(g_UiTransferDecryptSboxes,2 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                          ((rightHalf & 0xf00000) >> 20) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf0) * 4 + THANDOR_ADDR(g_UiTransferDecryptSboxes,1 * UI_TRANSFER_CIPHER_TABLE_BYTES) +
                         ((rightHalf & 0xf000000) >> 24) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf) * UI_TRANSFER_CIPHER_ROW_BYTES + THANDOR_ADDR(g_UiTransferDecryptSboxes,0) + (rightHalf >> 28) * 4);
        roundIndex--;
        rightHalf = savedHalf;
      } while (-1 < roundIndex);
      leftHalf = leftHalf ^ previousCipherLow;
      savedHalf = savedHalf ^ previousCipherHigh;
      previousCipherLow = *(uint32_t *)source;
      previousCipherHigh = ((uint32_t *)source)[1];
      *(uint32_t *)destination = leftHalf;
      ((uint32_t *)destination)[1] = savedHalf;
      source = (uint8_t *)source + 8;
      destination = (uint8_t *)destination + 8;
      blocksRemaining--;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x004AF140.
   Marks the receive side as unavailable: publishes the UI_TRANSFER_MAILBOX_UNAVAILABLE sentinel and sets the
   byte count, remaining bytes and retry ticks to one, so the mailbox is neither empty nor receivable.
*/
void UiTransferMailbox_MarkUnavailable(void)

{
  g_UiTransferMailbox.receivedAllocation = UI_TRANSFER_MAILBOX_UNAVAILABLE;
  g_UiTransferMailbox.receivedByteCount = 1;
  g_UiTransferMailbox.receivedRemainingBytes = 1;
  g_UiTransferMailbox.receiveRetryTicks = 1;
  return;
}


/* Address: 0x004AF1A0.
   Publishes the buffer the next outgoing transfer sends (NULL/0 withdraws it). The allocation is later
   released through g_MemoryApi.free by the frontend transfer consumers.
*/
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

{
  g_UiTransferMailbox.outgoingAllocation = allocation;
  g_UiTransferMailbox.outgoingByteCount = byteCount;
  return;
}


/* Address: 0x0054F9A0.
   Client answer to the host while the session starts and after each lobby command batch: sends the oldest
   queued lobby command in packet 0x10011 (a new sender sequence number each time) and, while player snapshots
   are still missing, requests the next one with packet 0x10004.
*/
void FrontendTransfer_SendLobbyCommandAndSnapshotRequest(void)

{
  FrontendPlayerRuntimeBlockCount nextPlayerIndex;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011_LOBBY_COMMAND;
  g_UiTransferSenderContext++;
  FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
  /* the snapshots received so far, read before sending like the original */
  nextPlayerIndex = g_FrontendPlayerRuntimeBlockCount;
  UiTransfer_StagePacketAndSend
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
  if (nextPlayerIndex < g_FrontendExpectedPlayerRuntimeBlockCount) {
    g_FrontendPacket10004Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_10004_SNAPSHOT_REQUEST;
    g_FrontendPacket10004Buffer.requestedPlayerIndex = nextPlayerIndex;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10004Buffer.header);
  }
  return;
}


/* Address: 0x004AEF70.
   Sends one packet to endpoint; every packet of the game goes through here. Stamps the header with this
   machine's sequence token and sender context and the XOR checksum over all dwords, then writes a scrambled
   copy (UiTransfer_EncryptPacketBlocks) into the next free units of a 256-unit ring (0x20 bytes per unit,
   with a parallel ring of 16-byte endpoint copies) and hands that copy to the backend send slot. The unit
   count is the high word of packedTypeAndUnitCount. CF is the backend's send result.
*/
bool UiTransfer_StagePacketAndSend(UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet)

{
  uint32_t nextUnitCursor;
  bool moreBytes;
  uint8_t *endpointBufferBase;
  uint32_t currentSequenceToken;
  uint32_t currentSenderContext;
  UiTransferXorChecksum checksum;
  uint32_t unitCount;
  UiTransferPayloadByteCount byteCount;
  UiTransferPayloadByteCount bytesRemaining;
  UiTransferPayloadByteCount nextBytesRemaining;
  int endpointOffset;
  int dataOffsetOrDwordCount;
  uint32_t *outputBlocks;
  UiTransferPacketHeader *checksumCursor;
  uint32_t *endpointDestinationDwordCursor;
  NetworkSendResult sendResult;
  
  currentSenderContext = g_UiTransferSenderContext;
  currentSequenceToken = g_UiTransferSequenceToken;
  unitCount = packet->packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
  dataOffsetOrDwordCount = g_UiTransferUnitCursor << 5; /* 0x20 bytes per unit */
  nextUnitCursor = unitCount + g_UiTransferUnitCursor;
  endpointOffset = g_UiTransferUnitCursor << 4; /* 16 bytes per endpoint copy */
  g_UiTransferUnitCursor = nextUnitCursor;
  if (255 < nextUnitCursor) {
    /* the packet does not fit before the end of the ring: start over at unit 0 */
    dataOffsetOrDwordCount = 0;
    endpointOffset = 0;
    g_UiTransferUnitCursor = unitCount;
  }
  outputBlocks = (uint32_t *)(g_UiTransferDataBuffer + dataOffsetOrDwordCount);
  endpointBufferBase = g_UiTransferEndpointBuffer->zeroPadding;
  byteCount = unitCount << 5;
  packet->xorChecksum = 0;
  packet->sequenceToken = currentSequenceToken;
  packet->senderContext = currentSenderContext;
  checksum = 0;
  bytesRemaining = byteCount;
  checksumCursor = packet;
  do {
    checksum = checksum ^ checksumCursor->packedTypeAndUnitCount;
    checksumCursor = (UiTransferPacketHeader *)&checksumCursor->sequenceToken;
    nextBytesRemaining = bytesRemaining - 4;
    moreBytes = 3 < (int)bytesRemaining;
    bytesRemaining = nextBytesRemaining;
  } while (nextBytesRemaining != 0 && moreBytes);
  packet->xorChecksum = checksum;
  UiTransfer_EncryptPacketBlocks
            (g_UiTransferRoundKeys16,outputBlocks,byteCount,
             &packet->packedTypeAndUnitCount);
  /* endpointBufferBase points 8 bytes into the endpoint ring, so "- 8" is the endpoint slot itself */
  endpointDestinationDwordCursor = (uint32_t *)(endpointBufferBase + endpointOffset + -8);
  for (dataOffsetOrDwordCount = 4; dataOffsetOrDwordCount != 0; dataOffsetOrDwordCount--) {
    *endpointDestinationDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, uint32_t, endpoint->addressHeader);
    endpoint = (UiTransferEndpointDescriptor *)&endpoint->ipv4AddressNetworkOrder;
    endpointDestinationDwordCursor++;
  }
  sendResult = g_NetworkBackendSlot5
                     ((WinSockAddress *)(endpointBufferBase + endpointOffset + -8),byteCount,(uint8_t *)outputBlocks);
  return sendResult.failed;
}

