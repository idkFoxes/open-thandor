/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/transfer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/transfer.h>
#include <thandor/thandor.h>

/* Implementation ownership: network/protocol/transfer. */

/* Address: 0x004AEB10.
   Network receive timer (125 Hz, so one tick is 8 ms). Drains the UDP socket into the record ring: each
   datagram is descrambled and its XOR checksum verified. The transfer and ping packets are answered right
   here; every other valid packet is kept in the ring for the frontend/in-game handlers. Transfer: the host
   sends a data blob (g_UiTransferMailbox) in chunks of UI_TRANSFER_CHUNK_PAYLOAD_BYTES as 0x80030 packets,
   each requested by the receiver with a 0x10031 packet naming the next offset; a request that stays
   unanswered for UI_TRANSFER_CHUNK_RETRY_TICKS ticks is sent again. Ping: 0x10032 is echoed as 0x10033,
   whose round trip becomes the player's latency text. Skipped while the ring lock is held elsewhere.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ServiceAndRetransmitTimer(void)

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
  UiTransferAuxiliaryEndpointRecord80 *auxiliaryEndpointRecord;
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
    /* Receive loop: every handled (or rejected) record jumps back here until the backend has no more data.
       The datagram goes to ring slot g_UiRuntimeRecordWriteIndex (0x100 bytes), the sender's address to
       the matching 0x80-byte slot of the auxiliary buffer; the slot is only kept (write index advanced)
       for packets that are not handled here. */
UiTransferMailbox_ReceiveNextRecord:
    slotIndexOrByteCount = g_UiRuntimeRecordWriteIndex;
    ringRecord = g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex;
    nextIndexOrChunkSize = g_UiRuntimeRecordWriteIndex + 1;
    receiveResult = g_NetworkBackendSlot4
                       ((WinSockAddress *)
                        (g_UiRuntimeRecordWriteIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeAuxiliaryBuffer8000),0x100,
                        (uint8_t *)ringRecord);
    if (!receiveResult.failed) {
      UiTransferBlock_Transform64BitBlocksWithRoundKeys16
                ((uint32_t *)&g_UiTransferRoundKeys16,ringRecord,0x100,ringRecord);
      LOCK();
      checksumField = &(ringRecord->packetHeader).xorChecksum;
      checksum = *checksumField;
      *checksumField = 0;
      UNLOCK();
      /* the XOR of all dwords of the packet (unit count * 8 dwords, checksum field zeroed) must equal the
         transmitted checksum */
      counterOrOffset = ((ringRecord->packetHeader).packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT) << 3;
      do {
        checksum = checksum ^ (ringRecord->packetHeader).packedTypeAndUnitCount;
        ringRecord = (UiRuntimeRecord *)&(ringRecord->packetHeader).sequenceToken;
        counterOrOffset--;
      } while (counterOrOffset != 0);
      if (checksum == 0) {
        ringRecord = g_UiRuntimeRecordRing + slotIndexOrByteCount;
        auxiliaryEndpointRecord =
             (UiTransferAuxiliaryEndpointRecord80 *)(slotIndexOrByteCount * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeAuxiliaryBuffer8000);
        if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_80030) {
          /* a chunk of the transfer from the host of this session: payload = offset, total size, data */
          if ((g_FrontendSessionToken == (ringRecord->packetHeader).sequenceToken) &&
             (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
              (auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder)) {
            g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + 0x40; /* 512 ms */
            counterOrOffset = *(int *)ringRecord->payload10_FF;
            slotIndexOrByteCount = *(uint32_t *)(ringRecord->payload10_FF + 4);
            /* receivedAllocation: NULL = no transfer requested, UI_TRANSFER_MAILBOX_UNAVAILABLE = requested,
               first chunk still missing (allocated here), otherwise the buffer being filled */
            if (g_UiTransferMailbox.receivedAllocation != NULL) {
              if (g_UiTransferMailbox.receivedAllocation == UI_TRANSFER_MAILBOX_UNAVAILABLE) {
                allocResult = g_MemoryApi.alloc(slotIndexOrByteCount);
                if (allocResult.failed) goto UiTransferMailbox_ReceiveNextRecord;
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
                receivedChunkSourceDwords = (uint32_t *)(ringRecord->payload10_FF + 8);
                receivedChunkDestinationDwords =
                     (uint32_t *)((int)g_UiTransferMailbox.receivedAllocation + counterOrOffset);
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
                  /* chunk packet header (at 0x004AE9E8) = FRONTEND_PACKET_10031: type 0x31, 1 unit */
                  s_mohTG_sakere___e_004ae9d8[0x10] = '1';
                  s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
                  s_mohTG_sakere___e_004ae9d8[0x12] = '\x01';
                  s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
                  g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
                  UiTransfer_StagePacketAndSend
                            ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                             (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
                }
              }
            }
          }
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10031) {
          /* host: a player requests the chunk at the offset in the payload of the outgoing transfer */
          if (g_UiTransferMailbox.outgoingAllocation != (void *)0x0) {
            playersRemaining = g_FrontendPlayerRuntimeBlockCount;
            playerRecord = g_FrontendPlayerRuntimeBlocks;
            while (((ringRecord->packetHeader).sequenceToken != playerRecord->peerSequenceToken ||
                   ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder !=
                    (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
              playerRecord++;
              playersRemaining--;
              if (playersRemaining == 0) goto UiTransferMailbox_ReceiveNextRecord;
            }
            auxiliaryEndpointRecord->transferTimeoutTicks =
                 auxiliaryEndpointRecord->transferTimeoutTicks + 0x40; /* 512 ms */
            g_UiTransferMailboxChunkOffset = *(UiTransferMailboxByteOffset *)ringRecord->payload10_FF;
            playerRecord->runtimeState70 = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
            g_UiTransferMailboxTransferByteCount = g_UiTransferMailbox.outgoingByteCount;
            playerRecord->runtimeState70 = playerRecord->runtimeState70 + g_UiTransferMailboxChunkOffset;
            /* chunk packet header = FRONTEND_PACKET_80030: type 0x30, 8 units (0x100 bytes) */
            s_mohTG_sakere___e_004ae9d8[0x10] = '0';
            s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
            s_mohTG_sakere___e_004ae9d8[0x12] = '\b';
            s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
            bytesRemaining = g_UiTransferMailboxTransferByteCount - g_UiTransferMailboxChunkOffset;
            nextIndexOrChunkSize = UI_TRANSFER_CHUNK_PAYLOAD_BYTES;
            if (bytesRemaining < UI_TRANSFER_CHUNK_PAYLOAD_BYTES) {
              nextIndexOrChunkSize = bytesRemaining;
            }
            mailboxCopySourceOrDestinationDwords =
                 (uint32_t *)((int)g_UiTransferMailbox.outgoingAllocation +
                          g_UiTransferMailboxChunkOffset);
            chunkPayloadCursor = g_UiTransferChunkPayload;
            for (nextIndexOrChunkSize = nextIndexOrChunkSize >> 2; nextIndexOrChunkSize != 0; nextIndexOrChunkSize--) {
              *chunkPayloadCursor = *mailboxCopySourceOrDestinationDwords;
              mailboxCopySourceOrDestinationDwords++;
              chunkPayloadCursor++;
            }
            g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
            UiTransfer_StagePacketAndSend
                      ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                       (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
          }
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10032) {
          /* ping: echo the sender's tick count back as 0x10033 */
          g_UiTransferMailboxReplyPacket10033EchoedTick = *(uint32_t *)ringRecord->payload10_FF;
          g_UiTransferMailboxReplyPacket10033 = 0x10033;
          g_UiTransferMailboxReplyPacket10033SequenceToken = g_UiTransferSequenceToken;
          UiTransfer_StagePacketAndSend
                    ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                     (UiTransferPacketHeader *)&g_UiTransferMailboxReplyPacket10033);
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10033) {
          /* ping answer: store the round trip in ticks and "<ticks * 4>ms" (half the round trip) as text */
          counterOrOffset = g_FrontendPlayerRuntimeCount;
          playerRecord = g_FrontendPlayerRuntimeBlocks;
          if (0 < g_FrontendPlayerRuntimeCount) {
            do {
              if (((ringRecord->packetHeader).sequenceToken == playerRecord->peerSequenceToken) &&
                 ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder ==
                  (playerRecord->endpoint).ipv4AddressNetworkOrder)) {
                counterOrOffset = g_UiTransferMailboxTickCounter - *(int *)ringRecord->payload10_FF;
                *(int *)playerRecord->reserved90_AF = counterOrOffset;
                latencyTextCursor = playerRecord->reserved90_AF + 4;
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
      goto UiTransferMailbox_ReceiveNextRecord;
    }
    /* no more data: re-request the missing chunk when the retry countdown expires */
    if (((g_UiTransferMailbox.receiveRetryTicks != 0) &&
        (g_UiTransferMailbox.receiveRetryTicks = g_UiTransferMailbox.receiveRetryTicks - 1,
        g_UiTransferMailbox.receiveRetryTicks == 0)) &&
       (g_UiTransferMailbox.receivedRemainingBytes != 0)) {
      g_UiTransferMailbox.receiveRetryTicks = UI_TRANSFER_CHUNK_RETRY_TICKS;
      g_UiTransferMailboxChunkOffset =
           g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
      s_mohTG_sakere___e_004ae9d8[0x10] = '1';
      s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
      s_mohTG_sakere___e_004ae9d8[0x12] = '\x01';
      s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
      g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,
                 (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
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
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint32_t commandHandlerIndex;
  int loopCount;
  uint32_t playerOrCommandCount;
  FrontendTransferPacketUnion *packetCursor;
  uint32_t *nameSourceCursor;
  uint32_t *playerRowCursor;
  FrontendPlayerNameUtf16_28 *nameDestinationCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_40008) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount = (packet->packet20002PlayerDescriptor).playerDescriptorPayload[3];
    if (((packet->packet20002PlayerDescriptor).playerDescriptorPayload[2] < 8) && (playerOrCommandCount < 9)) {
      packetCursor = packet;
      playerRowCursor = (uint32_t *)(&g_FrontendPlayerListRows)
                        [(packet->packet20002PlayerDescriptor).playerDescriptorPayload[2]];
      g_FrontendPlayerRuntimeCount = playerOrCommandCount;
      /* the whole 0x80-byte packet becomes the player's list row */
      for (loopCount = 0x20; loopCount != 0; loopCount--) {
        *playerRowCursor = (packetCursor->packet10000Handshake).header.packedTypeAndUnitCount;
        packetCursor = (FrontendTransferPacketUnion *)&(packetCursor->packet10000Handshake).header.sequenceToken
        ;
        playerRowCursor++;
      }
      playerRecord = g_FrontendPlayerRuntimeBlocks +
               (packet->packet20002PlayerDescriptor).playerDescriptorPayload[2];
      /* playerRecord->playerRuntimeId (+0x14), addressed from the name field like the original */
      *(uint32_t *)((int)(&playerRecord->playerName - 1) + 0x24) =
           (packet->packet20002PlayerDescriptor).playerDescriptorPayload[4];
      nameSourceCursor = &(packet->genericTransferPacket).commands[2].payloadDword08;
      nameDestinationCursor = &playerRecord->playerName;
      for (loopCount = 10; loopCount != 0; loopCount--) {
        *(uint32_t *)nameDestinationCursor->textUtf16 = *nameSourceCursor;
        nameSourceCursor++;
        nameDestinationCursor = (FrontendPlayerNameUtf16_28 *)(nameDestinationCursor->textUtf16 + 2);
      }
      UiPointerList_InitializeColumnLayout
                (playerOrCommandCount,(void **)&g_FrontendPlayerListRows,
                 (UiPointerListControl *)(frontendRuntime + 0x5874));
    }
    g_SessionTransferTimeoutTicks = 0x40;
    /* the host starts the session: this many player snapshots follow */
    if ((packet->packet10009SnapshotChunkRequest).reserved10 != 0) {
      g_FrontendPlayerRuntimeBlockCount = 0;
      g_FrontendExpectedPlayerRuntimeBlockCount =
           (packet->packet10009SnapshotChunkRequest).reserved10;
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRuntime + 0x508));
      FrontendState_DispatchCode(1);
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_CLIENT_STARTING;
      FrontendTransfer_SendQueued10011AndOptional10004();
      loopCount = 8;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      do {
        (playerRecord->factionAssignment).roleStateFlags = 0;
        playerRecord->runtimeState64 = 0;
        playerRecord->snapshotTransferFlags = 0;
        playerRecord++;
        loopCount--;
      } while (loopCount != 0);
    }
    return;
  }
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
        FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount =
         (packet->packet10000Handshake).header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
    do {
      /* command dword = handler offset << 8 | player id; offsets past the command handlers are ignored */
      commandHandlerIndex = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
      /* next 0x20-byte unit */
      packet = (FrontendTransferPacketUnion *)
               ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      playerOrCommandCount = playerOrCommandCount - 1;
    } while (playerOrCommandCount != 0);
    g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
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
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg)

{
  UiTransferSenderContext batchSenderContext;
  uint32_t expectedBlockCount;
  uint32_t commandHandlerIndex;
  uint32_t commandCountOrPlayerIndex;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int copyCount;
  FrontendPlayerRuntimeRecord *nextPlayerCursor;
  FrontendTransferPacketUnion *packetCursor;
  uint32_t *previewSourceCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint8_t *chunkDestinationCursor;
  TextResolveResult resolvedText;
  
  expectedBlockCount = g_FrontendExpectedPlayerRuntimeBlockCount;
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
        FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    batchSenderContext = (packet->packet10000Handshake).header.senderContext;
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    /* the same batch again: our answer got lost, repeat it */
    if (batchSenderContext == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    commandCountOrPlayerIndex =
         (packet->packet10000Handshake).header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT;
    g_FrontendSelectedPlayerToken = batchSenderContext;
    do {
      /* command dword = handler offset << 8 | player id; offsets past the command handlers are ignored */
      commandHandlerIndex = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
      /* next 0x20-byte unit */
      packet = (FrontendTransferPacketUnion *)
               ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      commandCountOrPlayerIndex = commandCountOrPlayerIndex - 1;
    } while (commandCountOrPlayerIndex != 0);
    FrontendTransfer_SendQueued10011AndOptional10004();
    g_FrontendTransferResponsePending = 1;
    return true;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10012) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = FRONTEND_PEER_TIMEOUT_TICKS;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013;
    UiTransfer_StagePacketAndSend
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10013Buffer.header);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_10007_PLAYER_REMOVAL) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if ((packet->packet10000Handshake).protocolMagic2931 == playerRecord->playerRuntimeId) {
        /* "player left" message with the name, then close the gap in the record array */
        resolvedText = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText.text);
        FrontendRecentTextHistory_InsertAndRebuild5(resolvedText.text);
        if (playersRemaining - 1 != 0) {
          nextPlayerCursor = playerRecord + 1;
          /* 0x4EC dwords = sizeof(FrontendPlayerRuntimeRecord) */
          for (copyCount = (playersRemaining - 1) * 0x4ec; copyCount != 0; copyCount--) {
            playerRecord->runtimeState00 = nextPlayerCursor->runtimeState00;
            nextPlayerCursor = (FrontendPlayerRuntimeRecord *)&nextPlayerCursor->peerSequenceToken;
            playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
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
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
         FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) &&
       (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
      (commandCountOrPlayerIndex = (packet->packet10000Handshake).protocolMagic2931,
      g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) &&
     ((commandCountOrPlayerIndex < g_FrontendExpectedPlayerRuntimeBlockCount &&
      (commandCountOrPlayerIndex == g_FrontendPlayerRuntimeBlockCount)))) {
    Random_SetBothSeeds((packet->packet30005PlayerSnapshot).secondaryRandomSeed);
    Random_SelectSecondaryStream();
    g_FrontendPlayerRuntimeBlockCount++;
    packetCursor = packet;
    playerRecord = g_FrontendPlayerRuntimeBlocks + commandCountOrPlayerIndex;
    /* the first 0x60 bytes of the packet become the head of the player's record */
    for (copyCount = 0x18; copyCount != 0; copyCount--) {
      playerRecord->runtimeState00 = (packetCursor->packet10000Handshake).header.packedTypeAndUnitCount;
      packetCursor = (FrontendTransferPacketUnion *)&(packetCursor->packet10000Handshake).header.sequenceToken;
      playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
    }
    resolvedText = TextResource_Resolve(0xff03);
    RichTextCommandStream_PatchPayloadBySelector
              (0,(packet->packet10000Handshake).reserved14_1F + 4,resolvedText.text);
    FrontendRecentTextHistory_InsertAndRebuild5(resolvedText.text);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10009) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         (packet->packet10009SnapshotChunkRequest).snapshotChunkOffset;
    chunkDestinationCursor = g_FrontendPacket8000ABuffer.packet10009Buffer;
    previewSourceCursor = (uint32_t *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A;
    /* 0xE8-byte chunks; the last one at 0x1220 has 0xE0 bytes (the preview is 0x1300 bytes) */
    copyCount = 0x3a;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == 0x1220) {
      copyCount = 0x38;
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
   Ownership: network/protocol/transfer.
   Purpose: Exact four-argument callback wrapper that marks the transfer mailbox unavailable only when frontend
   mode bit 0 is set.
   Local calls: UiTransferMailbox_MarkUnavailable.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) {
    UiTransferMailbox_MarkUnavailable();
  }
  return;
}


/* Address: 0x00545660.
   Ownership: network/protocol/transfer.
   Purpose: Handles frontend snapshot transfer mark player host publication ready and release when all ready.
   Local calls: UiTransferMailbox_SetOutgoingBuffer.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

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
        playersRemaining = playersRemaining - 1;
        playerRecord = playerRecord + 1;
      } while (playersRemaining != 0);
      g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
      return;
    }
    playerRecord = playerRecord + 1;
    playersRemaining = playersRemaining - 1;
  } while (playersRemaining != 0);
  return;
}


/* Address: 0x0054E230.
   Sends the session discovery probe (0x10000 handshake with FRONTEND_PROTOCOL_MAGIC) to
   g_FrontendNetworkEndpointScratch, the address from the join dialog or the broadcast address. Hosts answer
   with a 0x50001 session advertisement. CF is the send result.
*/
bool __thandor_cf_preserve_eax_ecx_edx UiTransfer_SendPacketType10000Value2931(void)

{
  bool sendCarry;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic2931 = FRONTEND_PROTOCOL_MAGIC;
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
bool __thandor_cf_preserve_ecx_edx UiTransfer_SendPlayerDescriptorPacket20002(void)

{
  int dwordCount;
  uint32_t *nameSourceCursor;
  uint32_t *payloadCursor;
  bool callCarry;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002;
  g_FrontendPacket20002Buffer.payloadByteCount = 0x40;
  nameSourceCursor = (void *)g_FrontendLocalPlayerNameUtf16;
  payloadCursor = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *payloadCursor = *nameSourceCursor;
    nameSourceCursor++;
    payloadCursor++;
  }
  /* the last name unit becomes the flags word */
  *(uint16_t *)((int)payloadCursor + -2) = 0;
  callCarry = PcxPreview_Load64x64PaletteAndPixels
                    (g_FrontendLocalPlayerPcxPreview,g_FrontendLocalPlayerNameUtf16);
  if (!callCarry) {
    *(uint16_t *)((int)payloadCursor + -2) = *(uint16_t *)((int)payloadCursor + -2) | 1;
  }
  *(uint16_t *)((int)payloadCursor + -2) = *(uint16_t *)((int)payloadCursor + -2) | 0x100;
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
   frontendRuntime +0x563C/+0x5640 are the row slots and row count of the lobby's player list
   (a UiPointerListControl), +0x5140 the player limit.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
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
  TextResolveResult textResolveResult;
  
  rootNodeOrCount = g_FrontendRootNode;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10000_HANDSHAKE) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
    if ((((packet->packet10000Handshake).protocolMagic2931 == FRONTEND_PROTOCOL_MAGIC) &&
        (((packet->packet10000Handshake).header.sequenceToken & FRONTEND_SEQUENCE_TOKEN_HIGH_MASK) ==
         FRONTEND_SEQUENCE_TOKEN_HIGH_WORD)) &&
       (*(uint32_t *)(frontendRuntime + 0x5640) < *(uint32_t *)(frontendRuntime + 0x5140))) {
      g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
    }
    textResolveResult = TextResource_Resolve(0x211a);
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)THANDOR_ADDR(g_GameVersionUtf16,0),textResolveResult.text);
    RichTextCommandStream_CopyExpanded
              (0x28,g_FrontendPacket50001Buffer.sessionTitleUtf16,textResolveResult.text);
    textResolveResult = TextResource_Resolve(0x211b);
    resolvedText = textResolveResult.text;
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)(rootNodeOrCount + 0x50c0),resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendLocalPlayerNameUtf16,resolvedText);
    RichTextCommandStream_CopyExpanded
              (0x58,g_FrontendPacket50001Buffer.hostDescriptionUtf16,resolvedText);
    textResolveResult = TextResource_Resolve(0x211c);
    resolvedText = textResolveResult.text;
    RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkRuntimeCountTextUtf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendNetworkPlayerCountTextUtf16,resolvedText);
    RichTextCommandStream_CopyExpanded(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,resolvedText);
    g_FrontendPacket50001Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT;
    g_FrontendPacket50001Buffer.payloadByteCount = 0x20;
    UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket50001Buffer.header);
    return;
  }
  if (((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_20002) ||
     (*(uint32_t *)(frontendRuntime + 0x5140) <= *(uint32_t *)(frontendRuntime + 0x5640))) {
    if ((packet->packet10000Handshake).header.packedTypeAndUnitCount !=
        FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT) {
      if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_10011) {
        return;
      }
      commandRecordCursor = g_FrontendPlayerCommandRecords;
      rootNodeOrCount = g_FrontendPlayerRuntimeCount;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      while (((packet->packet10000Handshake).header.sequenceToken != playerRecord->peerSequenceToken ||
             (senderEndpoint->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder
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
      for (rootNodeOrCount = 8; rootNodeOrCount != 0; rootNodeOrCount--) {
        (commandRecordCursor->header).packedTypeAndUnitCount =
             (packet->packet10000Handshake).header.packedTypeAndUnitCount;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
      }
      FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
      countFlagsOrId = 0;
      commandRecordCursor = g_FrontendPlayerCommandRecords;
      batchCursor = g_FrontendCommandBatchPacketBuffer;
      rootNodeOrCount = g_FrontendPlayerRuntimeCount;
      /* compact the slots holding a command into the batch and clear them (the player id stays) */
      do {
        if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
          commandRecordCursor++;
        }
        else {
          for (dwordCount = 8; dwordCount != 0; dwordCount--) {
            (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
            commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
            batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
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
        /* to every player but the host (record 0); 0x13B endpoint sizes = one player record */
        endpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        rootNodeOrCount = g_FrontendPlayerRuntimeCount;
        while (rootNodeOrCount = rootNodeOrCount - 1, rootNodeOrCount != 0) {
          UiTransfer_StagePacketAndSend(endpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
          endpointCursor = endpointCursor + 0x13b;
        }
        commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
        countFlagsOrId = countFlagsOrId & 0xffff;
        do {
          packedCommand = (commandRecordCursor->command).packedCommandAndPlayerId;
          commandHandlerIndex = packedCommand >> 8;
          if (commandHandlerIndex != 0) {
            if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
              (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                        (packedCommand & 0xff,(commandRecordCursor->command).payloadDword0C,
                         (commandRecordCursor->command).payloadDword08,(commandRecordCursor->command).payloadDword04);
            }
          }
          commandRecordCursor++;
          countFlagsOrId = countFlagsOrId - 1;
        } while (countFlagsOrId != 0);
      }
      return;
    }
    rootNodeOrCount = *(int *)(frontendRuntime + 0x5640);
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while (((packet->packet10000Handshake).header.sequenceToken != playerRecord->peerSequenceToken ||
           (senderEndpoint->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder))
          ) {
      playerRecord++;
      rootNodeOrCount--;
      if (rootNodeOrCount == 0) {
        return;
      }
    }
    countFlagsOrId = (packet->packet10000Handshake).protocolMagic2931;
    heartbeatValue = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
    playerRecord->capabilityFlags = countFlagsOrId;
    playerRecord->heartbeatExpiryTicks = heartbeatValue;
    playerRecord->reserved78_7F[0] = 0;
    playerRecord->reserved78_7F[1] = 0;
    playerRecord->reserved78_7F[2] = 0;
    playerRecord->reserved78_7F[3] = 0;
    if ((countFlagsOrId & FRONTEND_CAPABILITY_CD) != 0) {
      /* L"CD" */
      playerRecord->reserved78_7F[0] = 'C';
      playerRecord->reserved78_7F[1] = 0;
      playerRecord->reserved78_7F[2] = 'D';
      playerRecord->reserved78_7F[3] = 0;
    }
    FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
    return;
  }
  joiningPlayerRecordDwordCursor =
       *(uint32_t **)(*(int *)(frontendRuntime + 0x563c) + *(int *)(frontendRuntime + 0x5640) * 4);
  *(int *)(frontendRuntime + 0x5640) = *(int *)(frontendRuntime + 0x5640) + 1;
  /* the new row: the 0x40-byte descriptor packet followed by the sender's 0x10-byte endpoint */
  for (rootNodeOrCount = 0x10; rootNodeOrCount != 0; rootNodeOrCount--) {
    *joiningPlayerRecordDwordCursor = (packet->packet10000Handshake).header.packedTypeAndUnitCount;
    packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
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
      if (rootNodeOrCount == 0) goto FrontendTransfer_InitializeJoiningPlayerWithFreeId;
    }
    countFlagsOrId++;
    rootNodeOrCount = g_FrontendPlayerRuntimeCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (countFlagsOrId < 0xff);
FrontendTransfer_InitializeJoiningPlayerWithFreeId:
  descriptorStatusBits = *(uint16_t *)((int)joiningPlayerRecordDwordCursor + -0x12);
  *joiningPlayerRecordDwordCursor = 1;
  joiningPlayerRecordDwordCursor[-0xf] = countFlagsOrId;
  joiningPlayerRecordDwordCursor[6] = descriptorStatusBits & 0xff;
  joiningPlayerRecordDwordCursor[9] = descriptorStatusBits & 0xff00;
  joiningPlayerRecordDwordCursor[0x10] = 0;
  joiningPlayerRecordDwordCursor[0x11] = 0;
  joiningPlayerRecordDwordCursor[7] = 0;
  joiningPlayerRecordDwordCursor[10] = 0;
  joiningPlayerRecordDwordCursor[0xb] = 0;
  joiningPlayerRecordDwordCursor[0xd] = 0;
  joiningPlayerRecordDwordCursor[0xe] = 0;
  joiningPlayerRecordDwordCursor[0xf] = 0;
  if ((descriptorStatusBits & FRONTEND_CAPABILITY_CD) != 0) {
    joiningPlayerRecordDwordCursor[10] = 0x440043; /* L"CD" */
  }
  *(uint16_t *)((int)joiningPlayerRecordDwordCursor + -0x12) = 0;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,*(int32_t *)(frontendRuntime + 0x5640),
             (uint16_t *)&g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = countFlagsOrId;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10003Buffer.header);
  g_FrontendPlayerRuntimeCount++;
  FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
  return;
}


/* Address: 0x0054E9B0.
   Host lobby tick. Sends every joined player the session packet 0x40008 for one player-list row, chosen
   round robin (with its row, name and ping text "<n>ms"), plus a 0x10032 tick stamp; a pending session start
   switches to FRONTEND_NETWORK_STATE_HOST_STARTING. Then it adds the host's own next queued command to the
   players' collected ones, broadcasts the non-empty ones as one lobby command batch and executes them.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands
          (FrontendRootRuntimeAddress32 frontendRuntime)

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
  playerOrCommandCount = *(uint32_t *)(frontendRuntime + 0x5640);
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  remainingCount = playerOrCommandCount - 1;
  if (remainingCount != 0 && 0 < (int)playerOrCommandCount) {
    g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008;
    g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
    g_FrontendHostPublishRoundRobinCounter++;
    selectedIndexOrPackedCommand = roundRobinOrTextLength % playerOrCommandCount;
    /* the selected player's record fields, addressed from the record-1 endpoint in 0x10-byte steps */
    g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
         peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x13e].ipv4AddressNetworkOrder;
    g_FrontendPacket40008Buffer.selectedStatusCode0 =
         *(FrontendStatusCode *)peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x138].zeroPadding;
    g_FrontendPacket40008Buffer.selectedStatusCode1 =
         *(FrontendStatusCode *)(peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x138].zeroPadding + 4);
    g_FrontendPacket40008Buffer.selectedPlayerIndex = selectedIndexOrPackedCommand;
    g_FrontendPacket40008Buffer.playerCount = playerOrCommandCount;
    endpoint = peerEndpointCursor;
    roundRobinOrTextLength = g_WideNumberFormatUtf16
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                       peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x136].addressHeader.packedFamilyAndPort << 2,
                       g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
    *(uint32_t *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength) =
         0x73006d; /* L"ms" */
    *(uint16_t *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength + 4) = 0;
    descriptorSourceCursor = peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x13e].zeroPadding;
    descriptorDestinationCursor = g_FrontendPacket40008Buffer.playerDescriptorPayload;
    for (dwordCount = 10; dwordCount != 0; dwordCount--) {
      *descriptorDestinationCursor = *(uint32_t *)descriptorSourceCursor;
      descriptorSourceCursor = descriptorSourceCursor + 4;
      descriptorDestinationCursor++;
    }
    g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032;
    g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
    /* to every player but the host (record 0); 0x13B endpoint sizes = one player record */
    do {
      UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket40008Buffer.header);
      UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket10032Buffer.header);
      endpoint = endpoint + 0x13b;
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
    if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor++;
    }
    else {
      for (dwordCount = 8; dwordCount != 0; dwordCount--) {
        (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
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
      peerEndpointCursor = peerEndpointCursor + 0x13b;
    }
    commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
    playerOrCommandCount = playerOrCommandCount & 0xffff;
    do {
      selectedIndexOrPackedCommand = (commandRecordCursor->command).packedCommandAndPlayerId;
      commandHandlerIndex = selectedIndexOrPackedCommand >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    (selectedIndexOrPackedCommand & 0xff,(commandRecordCursor->command).payloadDword0C,(commandRecordCursor->command).payloadDword08,
                     (commandRecordCursor->command).payloadDword04);
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
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendPacket10006(void)

{
  g_FrontendPacket10006Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT;
  g_FrontendPacket10006Buffer.capabilityFlags = FRONTEND_CAPABILITY_CD;
  g_FrontendPacket10006Buffer.heartbeatExpiryTicks = 0x40;
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
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers)

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
            peerEndpointCursor = peerEndpointCursor + 0x13b;
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
    if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor++;
    }
    else {
      for (dwordCount = 8; dwordCount != 0; dwordCount--) {
        (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
      }
      remainingOrBatchCount++;
    }
    peersRemaining--;
  } while (peersRemaining != 0);
  if (remainingOrBatchCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    /* Nothing pending: send the first record (the host's, empty) as a batch of one. */
    commandRecordCursor = g_FrontendClientPlayerCommandRecords;
    for (dwordCount = 8; dwordCount != 0; dwordCount--) {
      (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
      commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
      batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
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
    peerEndpointCursor = peerEndpointCursor + 0x13b;
  }
  return false;
}


/* Address: 0x00572920.
   Client side of the lockstep exchange: sends the host its next in-game command (FRONTEND_PACKET_COMMAND_SUBMIT)
   with the oldest queued command, or an empty record when none is queued. The sender context counts the
   submissions.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendCommandBatchRequest10021(void)

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
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ClearReceivedState(void)

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
MailboxReceiveResult __thandor_eax_ecx_cf_preserve_edx
UiTransferMailbox_GetReceivedBuffer(void)

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
void __thandor_preserve_eax_edx UiTransferMailbox_RandomizeSequenceToken(void)

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
   frontendRuntime +0x4BBC is the session list's row count.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int remainingCount;
  FrontendSessionDiscoveryRecordB0 **sessionRowCursor;
  uint32_t *sessionDiscoveryRecordDwordCursor;
  uint32_t *playerRowCursor;
  
  remainingCount = *(int *)(frontendRuntime + 0x4bbc);
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    sessionDiscoveryRecordDwordCursor = (uint32_t *)g_FrontendSessionDiscoveryRecords;
    sessionRowCursor = g_FrontendSessionListRows;
    for (; remainingCount != 0; remainingCount--) {
      if (((packet->packet10000Handshake).header.sequenceToken ==
           (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
           header.sequenceToken) &&
         (senderEndpoint->ipv4AddressNetworkOrder ==
          (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->senderEndpoint).
          ipv4AddressNetworkOrder)) break;
      sessionRowCursor++;
      sessionDiscoveryRecordDwordCursor = (uint32_t *)((int)sessionDiscoveryRecordDwordCursor + 0xb0);
    }
    /* Update the known session in place, or append it while the list has fewer than 0x20 rows. */
    if ((remainingCount != 0) || (*(uint32_t *)(frontendRuntime + 0x4bbc) < 0x20)) {
      if (remainingCount == 0) {
        *sessionRowCursor = (FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor;
        *(int *)(frontendRuntime + 0x4bbc) = *(int *)(frontendRuntime + 0x4bbc) + 1;
      }
      /* the packet's first dword after the header is overwritten with 0x20 before it is stored */
      (packet->packet10000Handshake).protocolMagic2931 = 0x20;
      /* the 0xA0-byte advertisement followed by the sender's 0x10-byte endpoint */
      for (remainingCount = 0x28; remainingCount != 0; remainingCount--) {
        (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
        header.packedTypeAndUnitCount = (packet->packet10000Handshake).header.packedTypeAndUnitCount
        ;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        sessionDiscoveryRecordDwordCursor =
             &(((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->
              advertisement).header.sequenceToken;
      }
      for (remainingCount = 4; remainingCount != 0; remainingCount--) {
        *sessionDiscoveryRecordDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, uint32_t, senderEndpoint->addressHeader);
        senderEndpoint = (UiTransferEndpointDescriptor *)&senderEndpoint->ipv4AddressNetworkOrder;
        sessionDiscoveryRecordDwordCursor++;
      }
      UiPointerList_RefreshSelectionAndQueueAction
                ((UiPointerListControl *)(frontendRuntime + 0x4b68));
    }
  }
  else if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
             FRONTEND_PACKET_10003_JOIN_ACK) &&
           (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
          (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
           senderEndpoint->ipv4AddressNetworkOrder)) {
    g_LocalPlayerRuntimeId = (packet->packet10000Handshake).protocolMagic2931;
    g_SessionNetworkTickInterval = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
    UiPageStack_SetActiveIndex(4,(UiPageStackControl *)(frontendRuntime + 0x508));
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_JOINED;
    g_SessionTransferTimeoutTicks = 0x40;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_CLIENT;
    playerRowCursor = (uint32_t *)g_FrontendPlayerListRows;
    for (remainingCount = 0x100; remainingCount != 0; remainingCount--) {
      *playerRowCursor = 0;
      playerRowCursor++;
    }
    UiPointerList_InitializeColumnLayout
              (0,(void **)&g_FrontendPlayerListRows,
               (UiPointerListControl *)(frontendRuntime + 0x5874));
    return;
  }
  return;
}


/* Address: 0x0054EF30.
   Ownership: network/protocol/transfer.
   Purpose: Decrements the shared frontend transfer timeout and resets the request/mailbox page when the timer
   reaches zero.
   Cross-module calls: FrontendTransferPage_ResetSessionOpenAndRequestMailbox [ui/frontend/session].
*/
void __thandor_preserve_eax FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRuntime)

{
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    FrontendTransferPage_ResetSessionOpenAndRequestMailbox
              ((UiNodeBase *)((int)frontendRuntime + 0x5784));
  }
  return;
}


/* Address: 0x0054FBA0.
   Frontend copy of FrontendTransfer_ConsumeProcessedFlag: atomically takes and clears
   g_FrontendTransferResponsePending (set by FrontendTransfer_HandleGameplayCommandAndRosterPackets after a new
   command batch). Returns true (CF set) when no batch arrived, so Frontend_StateTick ends its tick early.
*/
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlagFrontend(void)

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
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSequenceToken senderSequenceToken;
  UiTransferSenderContext packetSenderContext;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;

  senderSequenceToken = (packet->packet10000Handshake).header.sequenceToken;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_SUBMIT) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_COMMAND_WAIT_ACK) {
      return;
    }
    while ((senderSequenceToken != playerRecord->peerSequenceToken ||
           (sourceContext->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder)))
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
         (sourceContext->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
    commandRecord++;
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  packetSenderContext = (packet->packet10000Handshake).header.senderContext;
  playerRecord->heartbeatExpiryTicks = FRONTEND_PEER_TIMEOUT_TICKS;
  /* the client bumps its sender context per new command; an equal one is a retransmit */
  if (packetSenderContext != (commandRecord->header).senderContext) {
    playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    /* copy the whole 0x20-byte packet, header included, into the slot (REP MOVSD) */
    for (dwordCount = 8; dwordCount != 0; dwordCount--) {
      (commandRecord->header).packedTypeAndUnitCount =
           (packet->packet10000Handshake).header.packedTypeAndUnitCount;
      packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
      commandRecord = (FrontendCommandPacketRecord *)&(commandRecord->header).sequenceToken;
    }
  }
  return;
}


/* Address: 0x00572560.
   Host side: executes the command batch it has just broadcast (g_FrontendClientCommandBatchPacketBuffer) on
   the local simulation, so host and clients run the same commands in the same tick. The high 24 bits of
   each packed command are the handler's offset from InGameCommandQueue_AppendLocalPlayerCommand, the low
   8 bits the player id; offsets beyond the handler code region are ignored.
   Known broken (deferred): THANDOR_CODE_AT adds the original code offset to the address of the compiled
   InGameCommandQueue_AppendLocalPlayerCommand and bounds it by the image-data stand-in for the original
   code-region end; neither matches the original code layout, so multiplayer commands do not dispatch.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_DispatchStagedCommandRecords(void)

{
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  uint32_t remainingCount;
  FrontendCommandPacketRecord *commandRecord;

  commandRecord = g_FrontendClientCommandBatchPacketBuffer;
  for (remainingCount = g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >>
                        FRONTEND_PACKET_UNIT_COUNT_SHIFT;
      remainingCount != 0; remainingCount--) {
    packedCommand = (commandRecord->command).packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      if (THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&InGameCommandHandlerCodeRegionEnd)
      {
        (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex))
                  (packedCommand & 0xff,(commandRecord->command).payloadDword0C,(commandRecord->command).payloadDword08,
                   (commandRecord->command).payloadDword04);
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
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlag(void)

{
  int previousFlag;

  /* XCHG in the original: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}


/* Address: 0x00407160.
   Encrypts an outgoing packet: byteCount/8 64-bit blocks in CBC mode (each input block is XORed with the
   previous output block, starting from zero), each through 16 rounds keyed by roundKeys16 and the eight
   nibble substitution tables at 0x00403160. UiTransferBlock_Transform64BitBlocksWithRoundKeys16 is the
   matching decryption used on receive.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTransfer_TransformPacketBlocks
          (uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
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
        rightState =(((((((*(int *)(&g_RandomPrimaryNibbleMixTable0 +
                                    (roundInputHalf & 0xf) * 4 + (*roundKeyNibble0 & 0xf) * 0x40) << 4 |
                           *(uint32_t *)(&g_RandomPrimaryNibbleMixTable1 +
                                    ((roundInputHalf & 0xf0) >> 4) * 4 + (*roundKeyNibble1 & 0xf0) * 4)) << 4
                          | *(uint32_t *)(&g_RandomPrimaryNibbleMixTable2 +
                                     ((roundInputHalf & 0xf00) >> 8) * 4 + ((*roundKeyNibble2 & 0xf00) >> 2))
                          ) << 4 | *(uint32_t *)(&g_RandomPrimaryNibbleMixTable3 +
                                            ((roundInputHalf & 0xf000) >> 0xc) * 4 +
                                            ((*roundKeyNibble3 & 0xf000) >> 6))) << 4 |
                        *(uint32_t *)(&g_RandomPrimaryNibbleMixTable4 +
                                 ((roundInputHalf & 0xf0000) >> 0x10) * 4 +
                                 ((*roundKeyNibble4 & 0xf0000) >> 10))) << 4 |
                       *(uint32_t *)(&g_RandomPrimaryNibbleMixTable5 +
                                ((roundInputHalf & 0xf00000) >> 0x14) * 4 +
                                ((*roundKeyNibble5 & 0xf00000) >> 0xe))) << 4 |
                      *(uint32_t *)(&g_RandomPrimaryNibbleMixTable6 +
                               ((roundInputHalf & 0xf000000) >> 0x18) * 4 +
                               ((*roundKeyNibble6 & 0xf000000) >> 0x12))) << 4 |
                     *(uint32_t *)(&g_RandomPrimaryNibbleMixTable7 +
                              (roundInputHalf >> 0x1c) * 4 + ((*roundKeyNibble7 & 0xf0000000) >> 0x16))) ^
                     leftState;
        roundInputHalf = leftState;
      } while (roundIndex < 0x10);
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
   UiTransfer_TransformPacketBlocks, running the 16 rounds backwards with the second table set
   (g_UiTransferCipherSubstitution, 0x00405160) and XORing each result with the previous ciphertext block (CBC).
*/
void __thandor_void_preserve_eax_ecx_edx
UiTransferBlock_Transform64BitBlocksWithRoundKeys16
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
      roundIndex = 0xf;
      leftHalf = *(uint32_t *)source;
      rightHalf = *(uint32_t *)((int)source + 4);
      do {
        savedHalf = leftHalf;
        rightHalf = rightHalf ^ savedHalf;
        leftHalf = ((((((*(int *)(((roundKeys16[roundIndex] & 0xf0000000) >> 0x16) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1c00) +
                              (rightHalf & 0xf) * 4) << 4 |
                     *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000000) >> 0x12) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1800) +
                              ((rightHalf & 0xf0) >> 4) * 4)) << 4 |
                    *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00000) >> 0xe) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1400) +
                             ((rightHalf & 0xf00) >> 8) * 4)) << 4 |
                   *(uint32_t *)(((roundKeys16[roundIndex] & 0xf0000) >> 10) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1000) +
                            ((rightHalf & 0xf000) >> 0xc) * 4)) << 4 |
                  *(uint32_t *)(((roundKeys16[roundIndex] & 0xf000) >> 6) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0xc00) +
                           ((rightHalf & 0xf0000) >> 0x10) * 4)) << 4 |
                 *(uint32_t *)(((roundKeys16[roundIndex] & 0xf00) >> 2) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x800) +
                          ((rightHalf & 0xf00000) >> 0x14) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf0) * 4 + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x400) +
                         ((rightHalf & 0xf000000) >> 0x18) * 4)) << 4 |
                *(uint32_t *)((roundKeys16[roundIndex] & 0xf) * 0x40 + THANDOR_ADDR(g_UiTransferCipherSubstitution,0) + (rightHalf >> 0x1c) * 4);
        roundIndex--;
        rightHalf = savedHalf;
      } while (-1 < roundIndex);
      leftHalf = leftHalf ^ previousCipherLow;
      savedHalf = savedHalf ^ previousCipherHigh;
      previousCipherLow = *(uint32_t *)source;
      previousCipherHigh = *(uint32_t *)((int)source + 4);
      *(uint32_t *)destination = leftHalf;
      *(uint32_t *)((int)destination + 4) = savedHalf;
      source = (void *)((int)source + 8);
      destination = (void *)((int)destination + 8);
      blocksRemaining--;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x004AF140.
   Marks the receive side as unavailable: publishes the UI_TRANSFER_MAILBOX_UNAVAILABLE sentinel and sets the
   byte count, remaining bytes and retry ticks to one, so the mailbox is neither empty nor receivable.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_MarkUnavailable(void)

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
void __thandor_void_preserve_eax_ecx_edx
UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

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
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendQueued10011AndOptional10004(void)

{
  FrontendPlayerRuntimeBlockCount nextPlayerIndex;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
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
   copy (UiTransfer_TransformPacketBlocks) into the next free units of a 256-unit ring (0x20 bytes per unit,
   with a parallel ring of 16-byte endpoint copies) and hands that copy to the backend send slot. The unit
   count is the high word of packedTypeAndUnitCount. CF is the backend's send result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTransfer_StagePacketAndSend
          (UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet)

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
  if (0xff < nextUnitCursor) {
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
  UiTransfer_TransformPacketBlocks
            ((uint32_t *)&g_UiTransferRoundKeys16,outputBlocks,byteCount,
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

