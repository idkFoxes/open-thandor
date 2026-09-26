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
   Ownership: network/protocol/transfer.
   Purpose: 125 ms transfer-mailbox service timer. Locks the mailbox/ring, consumes validated records, handles
   segmented payload and acknowledgement/retry records, and updates retransmission state.
   Local calls: UiTransferBlock_Transform64BitBlocksWithRoundKeys16, UiTransfer_StagePacketAndSendCf.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ServiceAndRetransmitTimer(void)

{
  UiTransferXorChecksum *checksumField;
  byte *latencySuffixCursor;
  UiTransferXorChecksum checksum;
  dword chunkEndOffset;
  dword slotIndexOrByteCount;
  uint nextIndexOrChunkSize;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int counterOrOffset;
  uint bytesRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  byte *latencyTextCursor;
  UiTransferAuxiliaryEndpointRecord80 *auxiliaryEndpointRecord;
  dword *receivedChunkSourceDwords;
  dword *mailboxCopySourceOrDestinationDwords;
  UiRuntimeRecord *ringRecord;
  dword *receivedChunkDestinationDwords;
  dword *chunkPayloadCursor;
  bool lockBusy;
  NetworkBackendReceiveEaxCf5 receiveResult;
  ArenaAllocEaxCf5 allocResult;
  
  g_UiTransferMailboxTickCounter = g_UiTransferMailboxTickCounter + 1;
  lockBusy = (*g_SpinLockTryAcquire)(&g_UiRuntimeRecordRingLock);
  if (!lockBusy) {
UiTransferMailbox_ReleaseRingLockAndReturn:
    slotIndexOrByteCount = g_UiRuntimeRecordWriteIndex;
    ringRecord = g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex;
    nextIndexOrChunkSize = g_UiRuntimeRecordWriteIndex + 1;
    receiveResult = (*g_NetworkBackendSlot4)
                       ((WinSockAddress *)
                        (g_UiRuntimeRecordWriteIndex * 0x80 + g_UiRuntimeAuxiliaryBuffer8000),0x100,
                        (byte *)ringRecord);
    if (!receiveResult.carry) {
      UiTransferBlock_Transform64BitBlocksWithRoundKeys16
                ((dword *)&g_UiTransferRoundKeys16,ringRecord,0x100,ringRecord);
      LOCK();
      checksumField = &(ringRecord->packetHeader).xorChecksum;
      checksum = *checksumField;
      *checksumField = 0;
      UNLOCK();
      counterOrOffset = ((ringRecord->packetHeader).packedTypeAndUnitCount >> 0x10) << 3;
      do {
        checksum = checksum ^ (ringRecord->packetHeader).packedTypeAndUnitCount;
        ringRecord = (UiRuntimeRecord *)&(ringRecord->packetHeader).sequenceToken;
        counterOrOffset = counterOrOffset + -1;
      } while (counterOrOffset != 0);
      if (checksum == 0) {
        ringRecord = g_UiRuntimeRecordRing + slotIndexOrByteCount;
        auxiliaryEndpointRecord =
             (UiTransferAuxiliaryEndpointRecord80 *)(slotIndexOrByteCount * 0x80 + g_UiRuntimeAuxiliaryBuffer8000);
        if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_80030) {
          if ((g_FrontendSessionToken == (ringRecord->packetHeader).sequenceToken) &&
             (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
              (auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder)) {
            g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + 0x40;
            counterOrOffset = *(int *)ringRecord->payload10_FF;
            slotIndexOrByteCount = *(dword *)(ringRecord->payload10_FF + 4);
            if (g_UiTransferMailbox.receivedAllocation != (void *)0x0) {
              if (g_UiTransferMailbox.receivedAllocation == (void *)0xffffffff) {
                allocResult = (*g_MemoryApi.alloc)(slotIndexOrByteCount);
                if (allocResult.carry) goto UiTransferMailbox_ReleaseRingLockAndReturn;
                counterOrOffset = 0;
                g_UiTransferMailbox.receivedAllocation = (void *)allocResult.eax;
                g_UiTransferMailbox.receivedByteCount = slotIndexOrByteCount;
                g_UiTransferMailbox.receivedRemainingBytes = slotIndexOrByteCount;
              }
              chunkEndOffset = counterOrOffset + g_UiTransferMailbox.receivedRemainingBytes;
              if ((chunkEndOffset == g_UiTransferMailbox.receivedByteCount) && (chunkEndOffset == slotIndexOrByteCount)) {
                counterOrOffset = chunkEndOffset - g_UiTransferMailbox.receivedRemainingBytes;
                bytesRemaining = slotIndexOrByteCount - counterOrOffset;
                nextIndexOrChunkSize = 0xe8;
                if (bytesRemaining < 0xe8) {
                  nextIndexOrChunkSize = bytesRemaining;
                }
                g_UiTransferMailbox.receivedRemainingBytes =
                     g_UiTransferMailbox.receivedRemainingBytes - nextIndexOrChunkSize;
                receivedChunkSourceDwords = (dword *)(ringRecord->payload10_FF + 8);
                receivedChunkDestinationDwords =
                     (dword *)((int)g_UiTransferMailbox.receivedAllocation + counterOrOffset);
                for (nextIndexOrChunkSize = nextIndexOrChunkSize >> 2; nextIndexOrChunkSize != 0; nextIndexOrChunkSize = nextIndexOrChunkSize - 1) {
                  *receivedChunkDestinationDwords = *receivedChunkSourceDwords;
                  receivedChunkSourceDwords = receivedChunkSourceDwords + 1;
                  receivedChunkDestinationDwords = receivedChunkDestinationDwords + 1;
                }
                if (g_UiTransferMailbox.receivedRemainingBytes != 0) {
                  g_UiTransferMailbox.receiveRetryTicks = 4;
                  g_UiTransferMailboxChunkOffset =
                       g_UiTransferMailbox.receivedByteCount -
                       g_UiTransferMailbox.receivedRemainingBytes;
                  s_mohTG_sakere___e_004ae9d8[0x10] = '1';
                  s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
                  s_mohTG_sakere___e_004ae9d8[0x12] = '\x01';
                  s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
                  g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
                  UiTransfer_StagePacketAndSendCf
                            ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                             (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
                }
              }
            }
          }
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10031) {
          if (g_UiTransferMailbox.outgoingAllocation != (void *)0x0) {
            playersRemaining = g_FrontendPlayerRuntimeBlockCount;
            playerRecord = g_FrontendPlayerRuntimeBlocks;
            while (((ringRecord->packetHeader).sequenceToken != playerRecord->peerSequenceToken ||
                   ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder !=
                    (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
              playerRecord = playerRecord + 1;
              playersRemaining = playersRemaining - 1;
              if (playersRemaining == 0) goto UiTransferMailbox_ReleaseRingLockAndReturn;
            }
            auxiliaryEndpointRecord->transferTimeoutTicks =
                 auxiliaryEndpointRecord->transferTimeoutTicks + 0x40;
            g_UiTransferMailboxChunkOffset = *(UiTransferMailboxByteOffset *)ringRecord->payload10_FF;
            playerRecord->runtimeState70 = 0xe8;
            g_UiTransferMailboxTransferByteCount = g_UiTransferMailbox.outgoingByteCount;
            playerRecord->runtimeState70 = playerRecord->runtimeState70 + g_UiTransferMailboxChunkOffset;
            s_mohTG_sakere___e_004ae9d8[0x10] = '0';
            s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
            s_mohTG_sakere___e_004ae9d8[0x12] = '\b';
            s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
            bytesRemaining = g_UiTransferMailboxTransferByteCount - g_UiTransferMailboxChunkOffset;
            nextIndexOrChunkSize = 0xe8;
            if (bytesRemaining < 0xe8) {
              nextIndexOrChunkSize = bytesRemaining;
            }
            mailboxCopySourceOrDestinationDwords =
                 (dword *)((int)g_UiTransferMailbox.outgoingAllocation +
                          g_UiTransferMailboxChunkOffset);
            chunkPayloadCursor = (dword *)THANDOR_ADDR(g_UiTransferChunkPayload,0);
            for (nextIndexOrChunkSize = nextIndexOrChunkSize >> 2; nextIndexOrChunkSize != 0; nextIndexOrChunkSize = nextIndexOrChunkSize - 1) {
              *chunkPayloadCursor = *mailboxCopySourceOrDestinationDwords;
              mailboxCopySourceOrDestinationDwords = mailboxCopySourceOrDestinationDwords + 1;
              chunkPayloadCursor = chunkPayloadCursor + 1;
            }
            g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
            UiTransfer_StagePacketAndSendCf
                      ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                       (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
          }
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10032) {
          g_UiTransferMailboxReplyPacket10033EchoedTick = *(undefined4 *)ringRecord->payload10_FF;
          g_UiTransferMailboxReplyPacket10033 = 0x10033;
          g_UiTransferMailboxReplyPacket10033SequenceToken = g_UiTransferSequenceToken;
          UiTransfer_StagePacketAndSendCf
                    ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                     (UiTransferPacketHeader *)&g_UiTransferMailboxReplyPacket10033);
        }
        else if ((ringRecord->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10033) {
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
                slotIndexOrByteCount = (*g_WideNumberFormatUtf16)
                                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,counterOrOffset * 4,(word *)latencyTextCursor);
                latencySuffixCursor = latencyTextCursor + slotIndexOrByteCount;
                latencySuffixCursor[0] = 0x6d;
                latencySuffixCursor[1] = 0;
                latencySuffixCursor[2] = 0x73;
                latencySuffixCursor[3] = 0;
                (latencyTextCursor + slotIndexOrByteCount + 4)[0] = 0;
                (latencyTextCursor + slotIndexOrByteCount + 4)[1] = 0;
                break;
              }
              playerRecord = playerRecord + 1;
              counterOrOffset = counterOrOffset + -1;
            } while (counterOrOffset != 0);
          }
        }
        else {
          g_UiRuntimeRecordWriteIndex = nextIndexOrChunkSize;
          if (0xff < nextIndexOrChunkSize) {
            g_UiRuntimeRecordWriteIndex = 0;
          }
        }
      }
      goto UiTransferMailbox_ReleaseRingLockAndReturn;
    }
    if (((g_UiTransferMailbox.receiveRetryTicks != 0) &&
        (g_UiTransferMailbox.receiveRetryTicks = g_UiTransferMailbox.receiveRetryTicks - 1,
        g_UiTransferMailbox.receiveRetryTicks == 0)) &&
       (g_UiTransferMailbox.receivedRemainingBytes != 0)) {
      g_UiTransferMailbox.receiveRetryTicks = 4;
      g_UiTransferMailboxChunkOffset =
           g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
      s_mohTG_sakere___e_004ae9d8[0x10] = '1';
      s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
      s_mohTG_sakere___e_004ae9d8[0x12] = '\x01';
      s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
      g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
      UiTransfer_StagePacketAndSendCf
                (&g_FrontendSelectedNetworkEndpoint,
                 (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
    }
    (*g_SpinLockRelease)(&g_UiRuntimeRecordRingLock);
  }
  return;
}


/* Address: 0x0054ECB0.
   Ownership: network/protocol/transfer.
   Purpose: Handles host-session packet 0x00040008 and low-type 0x0010 command batches, initializes player/session
   state, dispatches commands, and emits the required acknowledgement. Guest lobby recv: host session state +
   (n<<16)|0x10 LOBBY command batches (the 0x10 low-word side of the N4 phase split). Typed parameters: p4
   frontendRuntime→FrontendRootRuntimeAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FrontendTransfer_SendQueued10011AndOptional10004, UiTransfer_StagePacketAndSendCf.
   Cross-module calls: UiPointerList_InitializeColumnLayout [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], FrontendState_DispatchCode [ui/frontend/runtime],
   FrontendCommandQueue_DequeueFirstIntoRecord [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint commandHandlerIndex;
  int loopCount;
  uint playerOrCommandCount;
  FrontendTransferPacketUnion *packetCursor;
  dword *nameSourceCursor;
  dword *playerRowCursor;
  FrontendPlayerNameUtf16_28 *nameDestinationCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_40008) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount = (packet->packet20002PlayerDescriptor).playerDescriptorPayload[3];
    if (((packet->packet20002PlayerDescriptor).playerDescriptorPayload[2] < 8) && (playerOrCommandCount < 9)) {
      packetCursor = packet;
      playerRowCursor = (dword *)(&g_FrontendPlayerListRows)
                        [(packet->packet20002PlayerDescriptor).playerDescriptorPayload[2]];
      g_FrontendPlayerRuntimeCount = playerOrCommandCount;
      for (loopCount = 0x20; loopCount != 0; loopCount = loopCount + -1) {
        *playerRowCursor = (packetCursor->packet10000Handshake).header.packedTypeAndUnitCount;
        packetCursor = (FrontendTransferPacketUnion *)&(packetCursor->packet10000Handshake).header.sequenceToken
        ;
        playerRowCursor = playerRowCursor + 1;
      }
      playerRecord = g_FrontendPlayerRuntimeBlocks +
               (packet->packet20002PlayerDescriptor).playerDescriptorPayload[2];
      *(dword *)((int)(&playerRecord->playerName + -1) + 0x24) =
           (packet->packet20002PlayerDescriptor).playerDescriptorPayload[4];
      nameSourceCursor = &(packet->genericTransferPacket).commands[2].payloadDword08;
      nameDestinationCursor = &playerRecord->playerName;
      for (loopCount = 10; loopCount != 0; loopCount = loopCount + -1) {
        *(dword *)nameDestinationCursor->textUtf16 = *nameSourceCursor;
        nameSourceCursor = nameSourceCursor + 1;
        nameDestinationCursor = (FrontendPlayerNameUtf16_28 *)(nameDestinationCursor->textUtf16 + 2);
      }
      UiPointerList_InitializeColumnLayout
                (playerOrCommandCount,(void **)&g_FrontendPlayerListRows,
                 (UiPointerListControl *)(frontendRuntime + 0x5874));
    }
    g_SessionTransferTimeoutTicks = 0x40;
    if ((packet->packet10009SnapshotChunkRequest).reserved10 != 0) {
      g_FrontendPlayerRuntimeBlockCount = 0;
      g_FrontendExpectedPlayerRuntimeBlockCount =
           (packet->packet10009SnapshotChunkRequest).reserved10;
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRuntime + 0x508));
      FrontendState_DispatchCode(1);
      g_FrontendNetworkState = 5;
      FrontendTransfer_SendQueued10011AndOptional10004();
      loopCount = 8;
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      do {
        (playerRecord->factionAssignment).roleStateFlags = 0;
        playerRecord->runtimeState64 = 0;
        playerRecord->snapshotTransferFlags = 0;
        playerRecord = playerRecord + 1;
        loopCount = loopCount + -1;
      } while (loopCount != 0);
    }
    return;
  }
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x10) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    playerOrCommandCount = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
    do {
      commandHandlerIndex = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(code *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
      packet = (FrontendTransferPacketUnion *)
               ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      playerOrCommandCount = playerOrCommandCount - 1;
    } while (playerOrCommandCount != 0);
    g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
    FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
    if (g_FrontendPacket10011Buffer.command.packedCommandAndPlayerId != 0) {
      UiTransfer_StagePacketAndSendCf
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
    }
    return;
  }
  return;
}


/* Address: 0x0054F680.
   Ownership: network/protocol/transfer.
   Purpose: Validates and handles gameplay command batches plus packet types 0x00010012, 0x00010007, 0x00030005,
   and 0x00010009. Carry is set only when a new command batch is accepted.
   Local calls: UiTransfer_StagePacketAndSendCf, FrontendTransfer_SendQueued10011AndOptional10004.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], FrontendRecentTextHistory_InsertAndRebuild5 [ui/frontend/runtime], Random_SetBothSeeds
   [core/math/random], Random_SelectSecondaryStream [core/math/random].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          dword unusedDispatchArg)

{
  UiTransferSenderContext batchSenderContext;
  dword expectedBlockCount;
  uint commandHandlerIndex;
  uint commandCountOrPlayerIndex;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int copyCount;
  FrontendPlayerRuntimeRecord *nextPlayerCursor;
  FrontendTransferPacketUnion *packetCursor;
  undefined4 *previewSourceCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  byte *chunkDestinationCursor;
  TextResourceResolveEaxCf5 resolvedText;
  
  expectedBlockCount = g_FrontendExpectedPlayerRuntimeBlockCount;
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x10) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    batchSenderContext = (packet->packet10000Handshake).header.senderContext;
    g_SessionTransferTimeoutTicks = 0x100;
    if (batchSenderContext == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSendCf
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    commandCountOrPlayerIndex = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
    g_FrontendSelectedPlayerToken = batchSenderContext;
    do {
      commandHandlerIndex = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(code *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
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
    g_SessionTransferTimeoutTicks = 0x100;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013;
    UiTransfer_StagePacketAndSendCf
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
        resolvedText = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText.eax);
        FrontendRecentTextHistory_InsertAndRebuild5(resolvedText.eax);
        if (playersRemaining - 1 != 0) {
          nextPlayerCursor = playerRecord + 1;
          for (copyCount = (playersRemaining - 1) * 0x4ec; copyCount != 0; copyCount = copyCount + -1) {
            playerRecord->runtimeState00 = nextPlayerCursor->runtimeState00;
            nextPlayerCursor = (FrontendPlayerRuntimeRecord *)&nextPlayerCursor->peerSequenceToken;
            playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return false;
      }
      playerRecord = playerRecord + 1;
      playersRemaining = playersRemaining - 1;
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
    g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount + 1;
    packetCursor = packet;
    playerRecord = g_FrontendPlayerRuntimeBlocks + commandCountOrPlayerIndex;
    for (copyCount = 0x18; copyCount != 0; copyCount = copyCount + -1) {
      playerRecord->runtimeState00 = (packetCursor->packet10000Handshake).header.packedTypeAndUnitCount;
      packetCursor = (FrontendTransferPacketUnion *)&(packetCursor->packet10000Handshake).header.sequenceToken;
      playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
    }
    resolvedText = TextResource_Resolve(0xff03);
    RichTextCommandStream_PatchPayloadBySelector
              (0,(packet->packet10000Handshake).reserved14_1F + 4,resolvedText.eax);
    FrontendRecentTextHistory_InsertAndRebuild5(resolvedText.eax);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10009) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         (packet->packet10009SnapshotChunkRequest).snapshotChunkOffset;
    chunkDestinationCursor = g_FrontendPacket8000ABuffer.packet10009Buffer;
    previewSourceCursor = (undefined4 *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A;
    copyCount = 0x3a;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == 0x1220) {
      copyCount = 0x38;
    }
    for (; copyCount != 0; copyCount = copyCount + -1) {
      *(undefined4 *)chunkDestinationCursor = *previewSourceCursor;
      previewSourceCursor = previewSourceCursor + 1;
      chunkDestinationCursor = chunkDestinationCursor + 4;
    }
    if (expectedBlockCount == g_FrontendPlayerRuntimeBlockCount) {
      UiTransfer_StagePacketAndSendCf
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
FrontendTransfer_MarkUnavailableIfModeBit0Callback(dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3)

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
          (int playerRuntimeId,dword callbackArg1,dword callbackArg2,dword callbackArg3)

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
      (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
      return;
    }
    playerRecord = playerRecord + 1;
    playersRemaining = playersRemaining - 1;
  } while (playersRemaining != 0);
  return;
}


/* Address: 0x0054E230.
   Ownership: network/protocol/transfer.
   Purpose: Builds the fixed packet header with packed type 0x00010000 and payload value 0x2931, then submits it
   through the exact endpoint descriptor while preserving the backend CF result. Key sender: 0x10000 Handshake with
   magic 0x2931 (typed opcode census, exe_net_packets.md section 4b).
   Local calls: UiTransfer_StagePacketAndSendCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx UiTransfer_SendPacketType10000Value2931Cf(void)

{
  bool sendCarry;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic2931 = 0x2931;
  sendCarry = UiTransfer_StagePacketAndSendCf
                    (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return sendCarry;
}


/* Address: 0x0054E470.
   Ownership: network/protocol/transfer.
   Purpose: Builds packet 0x00020002 with a 0x40-byte payload, copies exactly ten dwords from the current player
   descriptor, derives status bits 0x0001 and 0x0100, and submits the packet while preserving the existing EDX and
   CF contracts. Key sender: 0x20002 PlayerDescriptor (10 dwords).
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: PcxPreview_Load64x64PaletteAndPixelsCf [ui/support/runtime].
*/
bool __thandor_cf_preserve_ecx_edx UiTransfer_SendPlayerDescriptorPacket20002Cf(void)

{
  int dwordCount;
  dword *nameSourceCursor;
  dword *payloadCursor;
  bool callCarry;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002;
  g_FrontendPacket20002Buffer.payloadByteCount = 0x40;
  nameSourceCursor = &g_FrontendLocalPlayerNameUtf16;
  payloadCursor = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount = dwordCount + -1) {
    *payloadCursor = *nameSourceCursor;
    nameSourceCursor = nameSourceCursor + 1;
    payloadCursor = payloadCursor + 1;
  }
  *(undefined2 *)((int)payloadCursor + -2) = 0;
  callCarry = PcxPreview_Load64x64PaletteAndPixelsCf
                    (g_FrontendLocalPlayerPcxPreview,(word *)&g_FrontendLocalPlayerNameUtf16);
  if (!callCarry) {
    *(ushort *)((int)payloadCursor + -2) = *(ushort *)((int)payloadCursor + -2) | 1;
  }
  *(ushort *)((int)payloadCursor + -2) = *(ushort *)((int)payloadCursor + -2) | 0x100;
  callCarry = UiTransfer_StagePacketAndSendCf
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return callCarry;
}


/* Address: 0x0054E4E0.
   Ownership: network/protocol/transfer.
   Purpose: Dispatches the frontend lobby packet family including 0x00010000, 0x00020002, 0x00010006, and
   0x00010011, formatting discovery replies and updating player records. Typed parameters: p4
   frontendRuntime→FrontendRootRuntimeAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], RichTextCommandStream_CopyExpandedCf [assets/text/richtext],
   FrontendCommandQueue_DequeueFirstIntoRecord [network/protocol/commands],
   FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction [ui/frontend/player].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  ushort descriptorStatusBits;
  UiTransferJoinAvailability heartbeatValue;
  uint packedCommand;
  word *resolvedText;
  uint commandHandlerIndex;
  int rootNodeOrCount;
  uint countFlagsOrId;
  int dwordCount;
  UiTransferEndpointDescriptor *endpointCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  dword *joiningPlayerRecordDwordCursor;
  FrontendCommandPacketRecord *commandRecordCursor;
  FrontendCommandPacketRecord *batchCursor;
  TextResourceResolveEaxCf5 textResolveResult;
  
  rootNodeOrCount = g_FrontendRootNode;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10000_HANDSHAKE) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
    if ((((packet->packet10000Handshake).protocolMagic2931 == 0x2931) &&
        (((packet->packet10000Handshake).header.sequenceToken & 0xffff0000) == 0x12340000)) &&
       (*(uint *)(frontendRuntime + 0x5640) < *(uint *)(frontendRuntime + 0x5140))) {
      g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
    }
    textResolveResult = TextResource_Resolve(0x211a);
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)THANDOR_ADDR(g_GameVersionUtf16,0),textResolveResult.eax);
    RichTextCommandStream_CopyExpandedCf
              (0x28,g_FrontendPacket50001Buffer.sessionTitleUtf16,textResolveResult.eax);
    textResolveResult = TextResource_Resolve(0x211b);
    resolvedText = textResolveResult.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)(rootNodeOrCount + 0x50c0),resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendLocalPlayerNameUtf16,resolvedText);
    RichTextCommandStream_CopyExpandedCf
              (0x58,g_FrontendPacket50001Buffer.hostDescriptionUtf16,resolvedText);
    textResolveResult = TextResource_Resolve(0x211c);
    resolvedText = textResolveResult.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkRuntimeCountTextUtf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendNetworkPlayerCountTextUtf16,resolvedText);
    RichTextCommandStream_CopyExpandedCf(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,resolvedText);
    g_FrontendPacket50001Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT;
    g_FrontendPacket50001Buffer.payloadByteCount = 0x20;
    UiTransfer_StagePacketAndSendCf(senderEndpoint,&g_FrontendPacket50001Buffer.header);
    return;
  }
  if (((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_20002) ||
     (*(uint *)(frontendRuntime + 0x5140) <= *(uint *)(frontendRuntime + 0x5640))) {
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
      for (rootNodeOrCount = 8; rootNodeOrCount != 0; rootNodeOrCount = rootNodeOrCount + -1) {
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
      do {
        if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
          commandRecordCursor = commandRecordCursor + 1;
        }
        else {
          for (dwordCount = 8; dwordCount != 0; dwordCount = dwordCount + -1) {
            (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
            commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
            batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
          }
          countFlagsOrId = countFlagsOrId + 1;
          commandRecordCursor[-1].command.packedCommandAndPlayerId =
               commandRecordCursor[-1].command.packedCommandAndPlayerId & 0xff;
        }
        rootNodeOrCount = rootNodeOrCount + -1;
      } while (rootNodeOrCount != 0);
      if (countFlagsOrId << 0x10 != 0) {
        g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = countFlagsOrId << 0x10 | 0x10;
        endpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        rootNodeOrCount = g_FrontendPlayerRuntimeCount;
        while (rootNodeOrCount = rootNodeOrCount + -1, rootNodeOrCount != 0) {
          UiTransfer_StagePacketAndSendCf(endpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
          endpointCursor = endpointCursor + 0x13b;
        }
        commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
        countFlagsOrId = countFlagsOrId & 0xffff;
        do {
          packedCommand = (commandRecordCursor->command).packedCommandAndPlayerId;
          commandHandlerIndex = packedCommand >> 8;
          if (commandHandlerIndex != 0) {
            if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
              (*(code *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                        (packedCommand & 0xff,(commandRecordCursor->command).payloadDword0C,
                         (commandRecordCursor->command).payloadDword08,(commandRecordCursor->command).payloadDword04);
            }
          }
          commandRecordCursor = commandRecordCursor + 1;
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
      playerRecord = playerRecord + 1;
      rootNodeOrCount = rootNodeOrCount + -1;
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
    if ((countFlagsOrId & 0x100) != 0) {
      playerRecord->reserved78_7F[0] = 0x43;
      playerRecord->reserved78_7F[1] = 0;
      playerRecord->reserved78_7F[2] = 0x44;
      playerRecord->reserved78_7F[3] = 0;
    }
    FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
    return;
  }
  joiningPlayerRecordDwordCursor =
       *(dword **)(*(int *)(frontendRuntime + 0x563c) + *(int *)(frontendRuntime + 0x5640) * 4);
  *(int *)(frontendRuntime + 0x5640) = *(int *)(frontendRuntime + 0x5640) + 1;
  for (rootNodeOrCount = 0x10; rootNodeOrCount != 0; rootNodeOrCount = rootNodeOrCount + -1) {
    *joiningPlayerRecordDwordCursor = (packet->packet10000Handshake).header.packedTypeAndUnitCount;
    packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
    joiningPlayerRecordDwordCursor = joiningPlayerRecordDwordCursor + 1;
  }
  endpointCursor = senderEndpoint;
  for (rootNodeOrCount = 4; rootNodeOrCount != 0; rootNodeOrCount = rootNodeOrCount + -1) {
    *joiningPlayerRecordDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, dword, endpointCursor->addressHeader);
    endpointCursor = (UiTransferEndpointDescriptor *)&endpointCursor->ipv4AddressNetworkOrder;
    joiningPlayerRecordDwordCursor = joiningPlayerRecordDwordCursor + 1;
  }
  countFlagsOrId = 0;
  rootNodeOrCount = g_FrontendPlayerRuntimeCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    while (countFlagsOrId != playerRecord->playerRuntimeId) {
      rootNodeOrCount = rootNodeOrCount + -1;
      playerRecord = playerRecord + 1;
      if (rootNodeOrCount == 0)
      goto 
      FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets_InitializeJoiningPlayerRecordWithNextAvailableId
      ;
    }
    countFlagsOrId = countFlagsOrId + 1;
    rootNodeOrCount = g_FrontendPlayerRuntimeCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
  } while (countFlagsOrId < 0xff);

  FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets_InitializeJoiningPlayerRecordWithNextAvailableId
  :
  descriptorStatusBits = *(ushort *)((int)joiningPlayerRecordDwordCursor + -0x12);
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
  if ((descriptorStatusBits & 0x100) != 0) {
    joiningPlayerRecordDwordCursor[10] = 0x440043;
  }
  *(undefined2 *)((int)joiningPlayerRecordDwordCursor + -0x12) = 0;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,*(sdword *)(frontendRuntime + 0x5640),
             (word *)&g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = countFlagsOrId;
  UiTransfer_StagePacketAndSendCf(senderEndpoint,&g_FrontendPacket10003Buffer.header);
  g_FrontendPlayerRuntimeCount = g_FrontendPlayerRuntimeCount + 1;
  FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
  return;
}


/* Address: 0x0054E9B0.
   Ownership: network/protocol/transfer.
   Purpose: Dequeues one frontend command into g_FrontendPlayerCommandRecords, compacts active 0x20-byte
   FrontendCommandPacketRecord slots into g_FrontendCommandBatchPacketBuffer, broadcasts them, and dispatches the
   local copies. Host side: periodic 0x50001 SessionAdvertisement publish (title[20]/host[44]/ count[4] UTF-16) +
   queued lobby command dispatch. Typed parameters: p2 frontendRuntime→FrontendRootRuntimeAddress32_V345. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: FrontendCommandQueue_DequeueFirstIntoRecord [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands
          (FrontendRootRuntimeAddress32 frontendRuntime)

{
  dword roundRobinOrTextLength;
  uint commandHandlerIndex;
  int remainingCount;
  int dwordCount;
  uint selectedIndexOrPackedCommand;
  uint playerOrCommandCount;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  byte *descriptorSourceCursor;
  UiTransferEndpointDescriptor *endpoint;
  FrontendCommandPacketRecord *commandRecordCursor;
  dword *descriptorDestinationCursor;
  FrontendCommandPacketRecord *batchCursor;
  
  roundRobinOrTextLength = g_FrontendHostPublishRoundRobinCounter;
  playerOrCommandCount = *(uint *)(frontendRuntime + 0x5640);
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  remainingCount = playerOrCommandCount - 1;
  if (remainingCount != 0 && 0 < (int)playerOrCommandCount) {
    g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008;
    g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
    g_FrontendHostPublishRoundRobinCounter = g_FrontendHostPublishRoundRobinCounter + 1;
    selectedIndexOrPackedCommand = roundRobinOrTextLength % playerOrCommandCount;
    g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
         peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x13e].ipv4AddressNetworkOrder;
    g_FrontendPacket40008Buffer.selectedStatusCode0 =
         *(FrontendStatusCode *)peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x138].zeroPadding;
    g_FrontendPacket40008Buffer.selectedStatusCode1 =
         *(FrontendStatusCode *)(peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x138].zeroPadding + 4);
    g_FrontendPacket40008Buffer.selectedPlayerIndex = selectedIndexOrPackedCommand;
    g_FrontendPacket40008Buffer.playerCount = playerOrCommandCount;
    endpoint = peerEndpointCursor;
    roundRobinOrTextLength = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                       peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x136].addressHeader.packedFamilyAndPort << 2,
                       g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
    *(undefined4 *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength) =
         0x73006d;
    *(undefined2 *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + roundRobinOrTextLength + 4) = 0;
    descriptorSourceCursor = peerEndpointCursor[selectedIndexOrPackedCommand * 0x13b + -0x13e].zeroPadding;
    descriptorDestinationCursor = g_FrontendPacket40008Buffer.playerDescriptorPayload;
    for (dwordCount = 10; dwordCount != 0; dwordCount = dwordCount + -1) {
      *descriptorDestinationCursor = *(dword *)descriptorSourceCursor;
      descriptorSourceCursor = descriptorSourceCursor + 4;
      descriptorDestinationCursor = descriptorDestinationCursor + 1;
    }
    g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032;
    g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
    do {
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket40008Buffer.header);
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket10032Buffer.header);
      endpoint = endpoint + 0x13b;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = 4;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  playerOrCommandCount = 0;
  commandRecordCursor = g_FrontendPlayerCommandRecords;
  batchCursor = g_FrontendCommandBatchPacketBuffer;
  remainingCount = g_FrontendPlayerRuntimeCount;
  do {
    if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor = commandRecordCursor + 1;
    }
    else {
      for (dwordCount = 8; dwordCount != 0; dwordCount = dwordCount + -1) {
        (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
      }
      playerOrCommandCount = playerOrCommandCount + 1;
      commandRecordCursor[-1].command.packedCommandAndPlayerId =
           commandRecordCursor[-1].command.packedCommandAndPlayerId & 0xff;
    }
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  if (playerOrCommandCount << 0x10 != 0) {
    g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = playerOrCommandCount << 0x10 | 0x10;
    peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
    remainingCount = g_FrontendPlayerRuntimeCount;
    while (remainingCount = remainingCount + -1, remainingCount != 0) {
      UiTransfer_StagePacketAndSendCf(peerEndpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
      peerEndpointCursor = peerEndpointCursor + 0x13b;
    }
    commandRecordCursor = g_FrontendCommandBatchPacketBuffer;
    playerOrCommandCount = playerOrCommandCount & 0xffff;
    do {
      selectedIndexOrPackedCommand = (commandRecordCursor->command).packedCommandAndPlayerId;
      commandHandlerIndex = selectedIndexOrPackedCommand >> 8;
      if (commandHandlerIndex != 0) {
        if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
          (*(code *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                    (selectedIndexOrPackedCommand & 0xff,(commandRecordCursor->command).payloadDword0C,(commandRecordCursor->command).payloadDword08,
                     (commandRecordCursor->command).payloadDword04);
        }
      }
      commandRecordCursor = commandRecordCursor + 1;
      playerOrCommandCount = playerOrCommandCount - 1;
    } while (playerOrCommandCount != 0);
  }
  return;
}


/* Address: 0x0054EEF0.
   Ownership: network/protocol/transfer.
   Purpose: Stages and sends packet 0x00010006 with the fixed 0x100 and 0x40 payload fields to the current frontend
   endpoint.
   Local calls: UiTransfer_StagePacketAndSendCf.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendPacket10006(void)

{
  g_FrontendPacket10006Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT;
  g_FrontendPacket10006Buffer.capabilityFlags = 0x100;
  g_FrontendPacket10006Buffer.heartbeatExpiryTicks = 0x40;
  UiTransfer_StagePacketAndSendCf
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10006Buffer.header);
  return;
}


/* Address: 0x005723F0.
   Ownership: network/protocol/transfer.
   Purpose: Dequeues one in-game command into g_FrontendClientPlayerCommandRecords, compacts active 0x20-byte
   FrontendCommandPacketRecord slots into g_FrontendClientCommandBatchPacketBuffer, and broadcasts either the
   command batch or state reply. IN-GAME lockstep broadcast: requires every peer's commandSyncPending (+0x50) set,
   clears them, compacts g_FrontendClientPlayerCommandRecords -> batch buffer, type = (count<<16)|0x20; peers not
   ready + sendStateReplies -> 0x10022 StatePending. Batch stride 0x20 = docs "32 B / player" (exe_net_cmd_sync.md
   senior for the ring layout). Typed parameters: p2 sendStateReplies→FrontendBooleanState32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: InGameCommandQueue_DequeueFirstIntoRecord [network/protocol/commands].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 sendStateReplies)

{
  FrontendPlayerRuntimeBlockCount peersRemaining;
  int remainingOrBatchCount;
  int dwordCount;
  FrontendPlayerRuntimeBlockCount recordsRemaining;
  FrontendCommandPacketRecord *commandRecordCursor;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *batchCursor;
  
  remainingOrBatchCount = g_FrontendPlayerRuntimeBlockCount - 1;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (remainingOrBatchCount != 0) {
    do {
      if (playerRecord[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        if (sendStateReplies != 0) {
          peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
          peersRemaining = g_FrontendPlayerRuntimeBlockCount;
          while (peersRemaining = peersRemaining - 1, peersRemaining != 0) {
            if (peerEndpointCursor[1].addressHeader.packedFamilyAndPort == 0) {
              UiTransfer_StagePacketAndSendCf
                        (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
            }
            else {
              g_FrontendPacket10022Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10022;
              UiTransfer_StagePacketAndSendCf(peerEndpointCursor,&g_FrontendPacket10022Buffer.header);
            }
            peerEndpointCursor = peerEndpointCursor + 0x13b;
          }
        }
        return true;
      }
      remainingOrBatchCount = remainingOrBatchCount + -1;
      playerRecord = playerRecord + 1;
    } while (remainingOrBatchCount != 0);
    remainingOrBatchCount = g_FrontendPlayerRuntimeBlockCount - 1;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      playerRecord[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      remainingOrBatchCount = remainingOrBatchCount + -1;
      playerRecord = playerRecord + 1;
    } while (remainingOrBatchCount != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  remainingOrBatchCount = 0;
  commandRecordCursor = g_FrontendClientPlayerCommandRecords;
  batchCursor = g_FrontendClientCommandBatchPacketBuffer;
  peersRemaining = g_FrontendPlayerRuntimeBlockCount;
  do {
    recordsRemaining = peersRemaining;
    if (((commandRecordCursor->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecordCursor = commandRecordCursor + 1;
      goto 
      FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip;
    }
    while( true ) {
      for (dwordCount = 8; dwordCount != 0; dwordCount = dwordCount + -1) {
        (batchCursor->header).packedTypeAndUnitCount = (commandRecordCursor->header).packedTypeAndUnitCount;
        commandRecordCursor = (FrontendCommandPacketRecord *)&(commandRecordCursor->header).sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
      }
      remainingOrBatchCount = remainingOrBatchCount + 1;
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip:
      peersRemaining = recordsRemaining - 1;
      if (recordsRemaining - 1 != 0) break;
      if (remainingOrBatchCount << 0x10 != 0) {
        g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
             remainingOrBatchCount << 0x10 | 0x20;
        peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        peersRemaining = g_FrontendPlayerRuntimeBlockCount;
        while (peersRemaining = peersRemaining - 1, peersRemaining != 0) {
          UiTransfer_StagePacketAndSendCf
                    (peerEndpointCursor,&g_FrontendClientCommandBatchPacketBuffer[0].header);
          peerEndpointCursor = peerEndpointCursor + 0x13b;
        }
        return false;
      }
      commandRecordCursor = g_FrontendClientPlayerCommandRecords;
      remainingOrBatchCount = 0;
    }
  } while( true );
}


/* Address: 0x00572920.
   Ownership: network/protocol/transfer.
   Purpose: Builds request type 0x00010021 in the fixed frontend packet, increments g_UiTransferSenderContext, runs
   the packet preparation helper, and submits it through UiTransfer_StagePacketAndSendCf with the fixed endpoint
   descriptor. EAX and CF remain authoritative. Key sender: 0x10021 in-game command single/batch request mirror.
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: InGameCommandQueue_DequeueFirstIntoRecord [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendCommandBatchRequest10021(void)

{
  g_FrontendPacket10021Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10021;
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  InGameCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10021Buffer);
  UiTransfer_StagePacketAndSendCf
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
  return;
}


/* Address: 0x004AF110.
   Ownership: network/protocol/transfer.
   Purpose: Clears receivedAllocation, receivedByteCount, receiveBusy, and receiveComplete without modifying the
   outgoing allocation pair.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ClearReceivedState(void)

{
  g_UiTransferMailbox.receivedAllocation = (void *)0x0;
  g_UiTransferMailbox.receivedByteCount = 0;
  g_UiTransferMailbox.receivedRemainingBytes = 0;
  g_UiTransferMailbox.receiveRetryTicks = 0;
  return;
}


/* Address: 0x004AF170.
   Ownership: network/protocol/transfer.
   Purpose: If receivedAllocation is neither null nor 0xFFFFFFFF and receiveBusy is zero, returns allocation in EAX
   and byte count in ECX with CF clear. Otherwise CF is set.
*/
UiTransferMailboxReceivedEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
UiTransferMailbox_GetReceivedBufferCf(void)

{
  undefined4 in_EAX = 0; /* unavailable (CF set): EAX/ECX untouched; callers use them only with CF clear */
  undefined4 in_ECX = 0; /* see in_EAX */
  UiTransferMailboxReceivedEaxEcxCf9 receivedResult;
  UiTransferMailboxReceivedEaxEcxCf9 unavailableResult;
  
  if (((g_UiTransferMailbox.receivedAllocation != (void *)0xffffffff) &&
      (g_UiTransferMailbox.receivedAllocation != (void *)0x0)) &&
     (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
    receivedResult.ecx = g_UiTransferMailbox.receivedByteCount;
    receivedResult.eax = (dword)g_UiTransferMailbox.receivedAllocation;
    receivedResult.carry = false;
    return receivedResult;
  }
  unavailableResult.ecx = in_ECX;
  unavailableResult.eax = in_EAX;
  unavailableResult.carry = true;
  return unavailableResult;
}


/* Address: 0x004AF1C0.
   Ownership: network/protocol/transfer.
   Purpose: XORs the low sixteen bits of Random_NextPrimary into the transfer sequence token while preserving the
   token's high word.
   Cross-module calls: Random_NextPrimary [core/math/random].
*/
void __thandor_preserve_eax_edx UiTransferMailbox_RandomizeSequenceToken(void)

{
  dword sequenceTokenRandomSample;
  
  sequenceTokenRandomSample = Random_NextPrimary();
  g_UiTransferSequenceToken = g_UiTransferSequenceToken ^ sequenceTokenRandomSample & 0xffff;
  return;
}


/* Address: 0x0054E260.
   Ownership: network/protocol/transfer.
   Purpose: Handles packet 0x00050001 session-list records and packet 0x00010003 join acknowledgements, updating
   frontend state and player selection data. Browse-screen recv handler: 0x50001 SessionAdvertisement rows +
   0x10003 JoinAck (assignedPlayerRuntimeId, networkTickInterval) — exe_net_client_join.md flow. Typed parameters:
   p4 frontendRuntime→FrontendRootRuntimeAddress32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPointerList_RefreshSelectionAndQueueAction [ui/controls/lists], UiPageStack_SetActiveIndex
   [ui/controls/layout], UiPointerList_InitializeColumnLayout [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int remainingCount;
  FrontendSessionDiscoveryRecordB0 **sessionRowCursor;
  dword *sessionDiscoveryRecordDwordCursor;
  undefined4 *playerRowCursor;
  
  remainingCount = *(int *)(frontendRuntime + 0x4bbc);
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    sessionDiscoveryRecordDwordCursor = (dword *)g_FrontendSessionDiscoveryRecords;
    sessionRowCursor = g_FrontendSessionListRows;
    for (; remainingCount != 0; remainingCount = remainingCount + -1) {
      if (((packet->packet10000Handshake).header.sequenceToken ==
           (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
           header.sequenceToken) &&
         (senderEndpoint->ipv4AddressNetworkOrder ==
          (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->senderEndpoint).
          ipv4AddressNetworkOrder))
      goto 
      FrontendTransfer_HandleSessionListAndJoinAckPackets_UpdateOrAppendSessionAdvertisementAndRefreshList
      ;
      sessionRowCursor = sessionRowCursor + 1;
      sessionDiscoveryRecordDwordCursor = (dword *)((int)sessionDiscoveryRecordDwordCursor + 0xb0);
    }
    if (*(uint *)(frontendRuntime + 0x4bbc) < 0x20) {
      *sessionRowCursor = (FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor;
      *(int *)(frontendRuntime + 0x4bbc) = *(int *)(frontendRuntime + 0x4bbc) + 1;
FrontendTransfer_HandleSessionListAndJoinAckPackets_UpdateOrAppendSessionAdvertisementAndRefreshList
      :
      (packet->packet10000Handshake).protocolMagic2931 = 0x20;
      for (remainingCount = 0x28; remainingCount != 0; remainingCount = remainingCount + -1) {
        (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
        header.packedTypeAndUnitCount = (packet->packet10000Handshake).header.packedTypeAndUnitCount
        ;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        sessionDiscoveryRecordDwordCursor =
             &(((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->
              advertisement).header.sequenceToken;
      }
      for (remainingCount = 4; remainingCount != 0; remainingCount = remainingCount + -1) {
        *sessionDiscoveryRecordDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, dword, senderEndpoint->addressHeader);
        senderEndpoint = (UiTransferEndpointDescriptor *)&senderEndpoint->ipv4AddressNetworkOrder;
        sessionDiscoveryRecordDwordCursor = sessionDiscoveryRecordDwordCursor + 1;
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
    g_FrontendNetworkState = 3;
    g_SessionTransferTimeoutTicks = 0x40;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_CLIENT;
    playerRowCursor = (undefined4 *)g_FrontendPlayerListRows;
    for (remainingCount = 0x100; remainingCount != 0; remainingCount = remainingCount + -1) {
      *playerRowCursor = 0;
      playerRowCursor = playerRowCursor + 1;
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
   Ownership: network/protocol/transfer.
   Purpose: Atomically clears the frontend processed flag and returns carry set when the prior value was nonzero.
   The instruction body is byte-identical to FrontendTransfer_ConsumeProcessedFlagCf at 0x00572AA0.
*/
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlagFrontendCf(void)

{
  int previousFlag;
  
  previousFlag = g_FrontendTransferResponsePending;
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return previousFlag == 0;
}


/* Address: 0x005722C0.
   Ownership: network/protocol/transfer.
   Purpose: Matches packet types 0x10021 and 0x10023 to a frontend player by sender and endpoint identity, marks
   the player ready, and copies changed eight-dword request state into the per-player synchronization slot.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleSyncRequest10021AndReply10023
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSequenceToken senderSequenceToken;
  UiTransferSenderContext packetSenderContext;
  FrontendPlayerRuntimeBlockCount playersRemaining;
  int dwordCount;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;
  
  senderSequenceToken = (packet->packet10000Handshake).header.sequenceToken;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_10021) {
    playersRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_10023) {
      return;
    }
    while ((senderSequenceToken != playerRecord->peerSequenceToken ||
           (sourceContext->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder)))
    {
      playersRemaining = playersRemaining - 1;
      playerRecord = playerRecord + 1;
      if (playersRemaining == 0) {
        return;
      }
    }
    playerRecord->heartbeatExpiryTicks = 0x100;
    return;
  }
  commandRecord = g_FrontendClientPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((senderSequenceToken != playerRecord->peerSequenceToken ||
         (sourceContext->ipv4AddressNetworkOrder != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
    commandRecord = commandRecord + 1;
    playerRecord = playerRecord + 1;
    playersRemaining = playersRemaining - 1;
    if (playersRemaining == 0) {
      return;
    }
  }
  packetSenderContext = (packet->packet10000Handshake).header.senderContext;
  playerRecord->heartbeatExpiryTicks = 0x100;
  if (packetSenderContext != (commandRecord->header).senderContext) {
    playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    for (dwordCount = 8; dwordCount != 0; dwordCount = dwordCount + -1) {
      (commandRecord->header).packedTypeAndUnitCount =
           (packet->packet10000Handshake).header.packedTypeAndUnitCount;
      packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
      commandRecord = (FrontendCommandPacketRecord *)&(commandRecord->header).sequenceToken;
    }
  }
  return;
}


/* Address: 0x00572560.
   Ownership: network/protocol/transfer.
   Purpose: Walks the high-word count of staged 0x20-byte command records and dispatches each bounded command code
   with its four verified arguments. Executes staged command records on the local simulation after batch consensus.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_DispatchStagedCommandRecords(void)

{
  uint packedCommand;
  uint commandHandlerIndex;
  uint remainingCount;
  FrontendCommandPacketRecord *commandRecord;
  
  commandRecord = g_FrontendClientCommandBatchPacketBuffer;
  for (remainingCount = g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >> 0x10;
      remainingCount != 0; remainingCount = remainingCount - 1) {
    packedCommand = (commandRecord->command).packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      if (THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&InGameCommandHandlerCodeRegionEnd)
      {
        (*(code *)THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex))
                  (packedCommand & 0xff,(commandRecord->command).payloadDword0C,(commandRecord->command).payloadDword08,
                   (commandRecord->command).payloadDword04);
      }
    }
    commandRecord = commandRecord + 1;
  }
  return;
}


/* Address: 0x00572AA0.
   Ownership: network/protocol/transfer.
   Purpose: Atomically exchanges the processed flag at 0x0050F0A8 with zero. CF is set when the consumed value was
   zero and clear when work had been marked processed; EAX is restored.
*/
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlagCf(void)

{
  int previousFlag;
  
  previousFlag = g_FrontendTransferResponsePending;
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return previousFlag == 0;
}


/* Address: 0x00407160.
   Ownership: network/protocol/transfer.
   Purpose: Transforms 8-byte packet blocks through sixteen table-driven rounds. The archived RET 0x10 proves four
   stack arguments; no register arguments are part of the ABI. Symmetric block transform (involution): the same
   routine encodes and decodes — no separate inverse exists.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTransfer_TransformPacketBlocks
          (dword *roundKeys16,dword *outputBlocks,UiTransferPayloadByteCount byteCount,
          dword *inputBlocks)

{
  uint *roundKeyNibble0;
  uint *roundKeyNibble1;
  uint *roundKeyNibble2;
  uint *roundKeyNibble3;
  uint *roundKeyNibble4;
  uint *roundKeyNibble5;
  uint *roundKeyNibble6;
  uint *roundKeyNibble7;
  uint roundInputHalf;
  uint roundIndex;
  uint blocksRemaining;
  uint leftState;
  uint rightState;
  
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
        roundIndex = roundIndex + 1;
        rightState = (((((((*(int *)(&g_RandomPrimaryNibbleMixTable0 +
                                    (roundInputHalf & 0xf) * 4 + (*roundKeyNibble0 & 0xf) * 0x40) << 4 |
                           *(uint *)(&g_RandomPrimaryNibbleMixTable1 +
                                    ((roundInputHalf & 0xf0) >> 4) * 4 + (*roundKeyNibble1 & 0xf0) * 4)) << 4
                          | *(uint *)(&g_RandomPrimaryNibbleMixTable2 +
                                     ((roundInputHalf & 0xf00) >> 8) * 4 + ((*roundKeyNibble2 & 0xf00) >> 2))
                          ) << 4 | *(uint *)(&g_RandomPrimaryNibbleMixTable3 +
                                            ((roundInputHalf & 0xf000) >> 0xc) * 4 +
                                            ((*roundKeyNibble3 & 0xf000) >> 6))) << 4 |
                        *(uint *)(&g_RandomPrimaryNibbleMixTable4 +
                                 ((roundInputHalf & 0xf0000) >> 0x10) * 4 +
                                 ((*roundKeyNibble4 & 0xf0000) >> 10))) << 4 |
                       *(uint *)(&g_RandomPrimaryNibbleMixTable5 +
                                ((roundInputHalf & 0xf00000) >> 0x14) * 4 +
                                ((*roundKeyNibble5 & 0xf00000) >> 0xe))) << 4 |
                      *(uint *)(&g_RandomPrimaryNibbleMixTable6 +
                               ((roundInputHalf & 0xf000000) >> 0x18) * 4 +
                               ((*roundKeyNibble6 & 0xf000000) >> 0x12))) << 4 |
                     *(uint *)(&g_RandomPrimaryNibbleMixTable7 +
                              (roundInputHalf >> 0x1c) * 4 + ((*roundKeyNibble7 & 0xf0000000) >> 0x16))) ^
                     leftState;
        roundInputHalf = leftState;
      } while (roundIndex < 0x10);
      *outputBlocks = leftState;
      outputBlocks[1] = rightState;
      inputBlocks = inputBlocks + 2;
      outputBlocks = outputBlocks + 2;
      blocksRemaining = blocksRemaining - 1;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x004072F0.
   Ownership: network/protocol/transfer.
   Purpose: Transforms byteCount/8 fixed 64-bit blocks from source to destination using the 16-round key schedule
   and eight archived substitution tables; source and destination may alias.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTransferBlock_Transform64BitBlocksWithRoundKeys16
          (dword *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source)

{
  uint leftHalf;
  int roundIndex;
  uint rightHalf;
  uint savedHalf;
  uint blocksRemaining;
  uint previousCipherLow;
  uint previousCipherHigh;
  
  blocksRemaining = byteCount >> 3;
  if (blocksRemaining != 0) {
    previousCipherHigh = 0;
    previousCipherLow = 0;
    do {
      roundIndex = 0xf;
      leftHalf = *(uint *)source;
      rightHalf = *(uint *)((int)source + 4);
      do {
        savedHalf = leftHalf;
        rightHalf = rightHalf ^ savedHalf;
        leftHalf = ((((((*(int *)(((roundKeys16[roundIndex] & 0xf0000000) >> 0x16) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1c00) +
                              (rightHalf & 0xf) * 4) << 4 |
                     *(uint *)(((roundKeys16[roundIndex] & 0xf000000) >> 0x12) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1800) +
                              ((rightHalf & 0xf0) >> 4) * 4)) << 4 |
                    *(uint *)(((roundKeys16[roundIndex] & 0xf00000) >> 0xe) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1400) +
                             ((rightHalf & 0xf00) >> 8) * 4)) << 4 |
                   *(uint *)(((roundKeys16[roundIndex] & 0xf0000) >> 10) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x1000) +
                            ((rightHalf & 0xf000) >> 0xc) * 4)) << 4 |
                  *(uint *)(((roundKeys16[roundIndex] & 0xf000) >> 6) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0xc00) +
                           ((rightHalf & 0xf0000) >> 0x10) * 4)) << 4 |
                 *(uint *)(((roundKeys16[roundIndex] & 0xf00) >> 2) + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x800) +
                          ((rightHalf & 0xf00000) >> 0x14) * 4)) << 4 |
                *(uint *)((roundKeys16[roundIndex] & 0xf0) * 4 + THANDOR_ADDR(g_UiTransferCipherSubstitution,0x400) +
                         ((rightHalf & 0xf000000) >> 0x18) * 4)) << 4 |
                *(uint *)((roundKeys16[roundIndex] & 0xf) * 0x40 + THANDOR_ADDR(g_UiTransferCipherSubstitution,0) + (rightHalf >> 0x1c) * 4);
        roundIndex = roundIndex + -1;
        rightHalf = savedHalf;
      } while (-1 < roundIndex);
      leftHalf = leftHalf ^ previousCipherLow;
      savedHalf = savedHalf ^ previousCipherHigh;
      previousCipherLow = *(uint *)source;
      previousCipherHigh = *(uint *)((int)source + 4);
      *(uint *)destination = leftHalf;
      *(uint *)((int)destination + 4) = savedHalf;
      source = (void *)((int)source + 8);
      destination = (void *)((int)destination + 8);
      blocksRemaining = blocksRemaining - 1;
    } while (blocksRemaining != 0);
  }
  return;
}


/* Address: 0x004AF140.
   Ownership: network/protocol/transfer.
   Purpose: Publishes the 0xFFFFFFFF unavailable sentinel and sets receivedByteCount, receiveBusy, and
   receiveComplete to one.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_MarkUnavailable(void)

{
  g_UiTransferMailbox.receivedAllocation = (void *)0xffffffff;
  g_UiTransferMailbox.receivedByteCount = 1;
  g_UiTransferMailbox.receivedRemainingBytes = 1;
  g_UiTransferMailbox.receiveRetryTicks = 1;
  return;
}


/* Address: 0x004AF1A0.
   Ownership: network/protocol/transfer.
   Purpose: Publishes an outgoing allocation and byte count. The allocation is later released through
   g_MemoryApi.free by frontend transfer consumers.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

{
  g_UiTransferMailbox.outgoingAllocation = allocation;
  g_UiTransferMailbox.outgoingByteCount = byteCount;
  return;
}


/* Address: 0x0054F9A0.
   Ownership: network/protocol/transfer.
   Purpose: Dequeues and sends a packet 0x00010011 record, then conditionally sends packet 0x00010004 for the next
   active player index.
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: FrontendCommandQueue_DequeueFirstIntoRecord [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendQueued10011AndOptional10004(void)

{
  FrontendPlayerRuntimeBlockCount nextPlayerIndex;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
  nextPlayerIndex = g_FrontendPlayerRuntimeBlockCount;
  UiTransfer_StagePacketAndSendCf
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
  if (nextPlayerIndex < g_FrontendExpectedPlayerRuntimeBlockCount) {
    g_FrontendPacket10004Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_10004_SNAPSHOT_REQUEST;
    g_FrontendPacket10004Buffer.requestedPlayerIndex = nextPlayerIndex;
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10004Buffer.header);
  }
  return;
}


/* Address: 0x004AEF70.
   Ownership: network/protocol/transfer.
   Purpose: Stages a variable packet in the exact 0x2000-byte rolling data buffer, copies its four-dword endpoint
   descriptor into the parallel 0x1000-byte buffer, writes the sequence token and sender context, computes the
   dword XOR checksum, and invokes network backend slot 5. The high word of packedTypeAndUnitCount is an exact
   count of 0x20-byte units. All general registers are restored and CF is preserved from the backend submission
   callback. Stages the packet (high word of packedTypeAndUnitCount = exact 0x20-byte unit count), computes the XOR
   checksum over every dword (checksum field cleared first), scrambles via TransformPacketBlocks, sends. Wire
   captures (exe_net_*.md) stay senior for live traffic.
   Local calls: UiTransfer_TransformPacketBlocks.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTransfer_StagePacketAndSendCf
          (UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet)

{
  uint nextUnitCursor;
  bool moreBytes;
  byte *endpointBufferBase;
  dword currentSequenceToken;
  dword currentSenderContext;
  UiTransferXorChecksum checksum;
  uint unitCount;
  UiTransferPayloadByteCount byteCount;
  UiTransferPayloadByteCount bytesRemaining;
  UiTransferPayloadByteCount nextBytesRemaining;
  int endpointOffset;
  int dataOffsetOrDwordCount;
  dword *outputBlocks;
  UiTransferPacketHeader *checksumCursor;
  dword *endpointDestinationDwordCursor;
  NetworkBackendSendEaxCf5 sendResult;
  
  currentSenderContext = g_UiTransferSenderContext;
  currentSequenceToken = g_UiTransferSequenceToken;
  unitCount = packet->packedTypeAndUnitCount >> 0x10;
  dataOffsetOrDwordCount = g_UiTransferUnitCursor << 5;
  nextUnitCursor = unitCount + g_UiTransferUnitCursor;
  endpointOffset = g_UiTransferUnitCursor << 4;
  g_UiTransferUnitCursor = nextUnitCursor;
  if (0xff < nextUnitCursor) {
    dataOffsetOrDwordCount = 0;
    endpointOffset = 0;
    g_UiTransferUnitCursor = unitCount;
  }
  outputBlocks = (dword *)(g_UiTransferDataBuffer + dataOffsetOrDwordCount);
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
            ((dword *)&g_UiTransferRoundKeys16,outputBlocks,byteCount,
             &packet->packedTypeAndUnitCount);
  endpointDestinationDwordCursor = (dword *)(endpointBufferBase + endpointOffset + -8);
  for (dataOffsetOrDwordCount = 4; dataOffsetOrDwordCount != 0; dataOffsetOrDwordCount = dataOffsetOrDwordCount + -1) {
    *endpointDestinationDwordCursor = THANDOR_BITCAST(NetworkEndpointAddressHeader4, dword, endpoint->addressHeader);
    endpoint = (UiTransferEndpointDescriptor *)&endpoint->ipv4AddressNetworkOrder;
    endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
  }
  sendResult = (*g_NetworkBackendSlot5)
                     ((WinSockAddress *)(endpointBufferBase + endpointOffset + -8),byteCount,(byte *)outputBlocks);
  return sendResult.carry;
}

