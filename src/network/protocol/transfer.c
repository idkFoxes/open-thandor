/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/transfer.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/transfer.h>

/* Implementation ownership: network/protocol/transfer. */

/* Address: 0x004AEB10.
   Ownership: network/protocol/transfer.
   Purpose: 125 ms transfer-mailbox service timer. Locks the mailbox/ring, consumes validated records, handles
   segmented payload and acknowledgement/retry records, and updates retransmission state.
   Local calls: UiTransferBlock_Transform64BitBlocksWithRoundKeys16, UiTransfer_StagePacketAndSendCf.
*/
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ServiceAndRetransmitTimer(void)

{
  UiTransferXorChecksum *pUVar1;
  byte *pbVar2;
  UiTransferXorChecksum UVar2;
  dword dVar3;
  dword dVar4;
  uint uVar5;
  FrontendPlayerRuntimeBlockCount FVar6;
  int iVar7;
  uint uVar8;
  FrontendPlayerRuntimeRecord *pFVar9;
  byte *arg5;
  UiTransferAuxiliaryEndpointRecord80 *auxiliaryEndpointRecord;
  dword *receivedChunkSourceDwords;
  dword *mailboxCopySourceOrDestinationDwords;
  UiRuntimeRecord *pUVar10;
  dword *receivedChunkDestinationDwords;
  dword *pdVar11;
  bool bVar12;
  NetworkBackendReceiveEaxCf5 NVar13;
  ArenaAllocEaxCf5 AVar14;
  
  g_UiTransferMailboxTickCounter = g_UiTransferMailboxTickCounter + 1;
  bVar12 = (*g_SpinLockTryAcquire)(&g_UiRuntimeRecordRingLock);
  if (!bVar12) {
UiTransferMailbox_ReleaseRingLockAndReturn:
    dVar4 = g_UiRuntimeRecordWriteIndex;
    pUVar10 = g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex;
    uVar5 = g_UiRuntimeRecordWriteIndex + 1;
    NVar13 = (*g_NetworkBackendSlot4)
                       ((WinSockAddress *)
                        (g_UiRuntimeRecordWriteIndex * 0x80 + g_UiRuntimeAuxiliaryBuffer8000),0x100,
                        (byte *)pUVar10);
    if (!NVar13.carry) {
      UiTransferBlock_Transform64BitBlocksWithRoundKeys16
                ((dword *)&g_UiTransferRoundKeys16,pUVar10,0x100,pUVar10);
      LOCK();
      pUVar1 = &(pUVar10->packetHeader).xorChecksum;
      UVar2 = *pUVar1;
      *pUVar1 = 0;
      UNLOCK();
      iVar7 = ((pUVar10->packetHeader).packedTypeAndUnitCount >> 0x10) << 3;
      do {
        UVar2 = UVar2 ^ (pUVar10->packetHeader).packedTypeAndUnitCount;
        pUVar10 = (UiRuntimeRecord *)&(pUVar10->packetHeader).sequenceToken;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      if (UVar2 == 0) {
        pUVar10 = g_UiRuntimeRecordRing + dVar4;
        auxiliaryEndpointRecord =
             (UiTransferAuxiliaryEndpointRecord80 *)(dVar4 * 0x80 + g_UiRuntimeAuxiliaryBuffer8000);
        if ((pUVar10->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_80030) {
          if ((g_FrontendSessionToken == (pUVar10->packetHeader).sequenceToken) &&
             (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
              (auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder)) {
            g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + 0x40;
            iVar7 = *(int *)pUVar10->payload10_FF;
            dVar4 = *(dword *)(pUVar10->payload10_FF + 4);
            if (g_UiTransferMailbox.receivedAllocation != (void *)0x0) {
              if (g_UiTransferMailbox.receivedAllocation == (void *)0xffffffff) {
                AVar14 = (*g_MemoryApi.alloc)(dVar4);
                if (AVar14.carry) goto UiTransferMailbox_ReleaseRingLockAndReturn;
                iVar7 = 0;
                g_UiTransferMailbox.receivedAllocation = (void *)AVar14.eax;
                g_UiTransferMailbox.receivedByteCount = dVar4;
                g_UiTransferMailbox.receivedRemainingBytes = dVar4;
              }
              dVar3 = iVar7 + g_UiTransferMailbox.receivedRemainingBytes;
              if ((dVar3 == g_UiTransferMailbox.receivedByteCount) && (dVar3 == dVar4)) {
                iVar7 = dVar3 - g_UiTransferMailbox.receivedRemainingBytes;
                uVar8 = dVar4 - iVar7;
                uVar5 = 0xe8;
                if (uVar8 < 0xe8) {
                  uVar5 = uVar8;
                }
                g_UiTransferMailbox.receivedRemainingBytes =
                     g_UiTransferMailbox.receivedRemainingBytes - uVar5;
                receivedChunkSourceDwords = (dword *)(pUVar10->payload10_FF + 8);
                receivedChunkDestinationDwords =
                     (dword *)((int)g_UiTransferMailbox.receivedAllocation + iVar7);
                for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
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
        else if ((pUVar10->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10031) {
          if (g_UiTransferMailbox.outgoingAllocation != (void *)0x0) {
            FVar6 = g_FrontendPlayerRuntimeBlockCount;
            pFVar9 = g_FrontendPlayerRuntimeBlocks;
            while (((pUVar10->packetHeader).sequenceToken != pFVar9->peerSequenceToken ||
                   ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder !=
                    (pFVar9->endpoint).ipv4AddressNetworkOrder))) {
              pFVar9 = pFVar9 + 1;
              FVar6 = FVar6 - 1;
              if (FVar6 == 0) goto UiTransferMailbox_ReleaseRingLockAndReturn;
            }
            auxiliaryEndpointRecord->transferTimeoutTicks =
                 auxiliaryEndpointRecord->transferTimeoutTicks + 0x40;
            g_UiTransferMailboxChunkOffset = *(UiTransferMailboxByteOffset *)pUVar10->payload10_FF;
            pFVar9->runtimeState70 = 0xe8;
            g_UiTransferMailboxTransferByteCount = g_UiTransferMailbox.outgoingByteCount;
            pFVar9->runtimeState70 = pFVar9->runtimeState70 + g_UiTransferMailboxChunkOffset;
            s_mohTG_sakere___e_004ae9d8[0x10] = '0';
            s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
            s_mohTG_sakere___e_004ae9d8[0x12] = '\b';
            s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
            uVar8 = g_UiTransferMailboxTransferByteCount - g_UiTransferMailboxChunkOffset;
            uVar5 = 0xe8;
            if (uVar8 < 0xe8) {
              uVar5 = uVar8;
            }
            mailboxCopySourceOrDestinationDwords =
                 (dword *)((int)g_UiTransferMailbox.outgoingAllocation +
                          g_UiTransferMailboxChunkOffset);
            pdVar11 = (dword *)0x4aea00;
            for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
              *pdVar11 = *mailboxCopySourceOrDestinationDwords;
              mailboxCopySourceOrDestinationDwords = mailboxCopySourceOrDestinationDwords + 1;
              pdVar11 = pdVar11 + 1;
            }
            g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
            UiTransfer_StagePacketAndSendCf
                      ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                       (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
          }
        }
        else if ((pUVar10->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10032) {
          g_UiTransferMailboxReplyPacket10033EchoedTick = *(undefined4 *)pUVar10->payload10_FF;
          g_UiTransferMailboxReplyPacket10033 = 0x10033;
          g_UiTransferMailboxReplyPacket10033SequenceToken = g_UiTransferSequenceToken;
          UiTransfer_StagePacketAndSendCf
                    ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                     (UiTransferPacketHeader *)&g_UiTransferMailboxReplyPacket10033);
        }
        else if ((pUVar10->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10033) {
          iVar7 = g_FrontendPlayerRuntimeCount;
          pFVar9 = g_FrontendPlayerRuntimeBlocks;
          if (0 < g_FrontendPlayerRuntimeCount) {
            do {
              if (((pUVar10->packetHeader).sequenceToken == pFVar9->peerSequenceToken) &&
                 ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder ==
                  (pFVar9->endpoint).ipv4AddressNetworkOrder)) {
                iVar7 = g_UiTransferMailboxTickCounter - *(int *)pUVar10->payload10_FF;
                *(int *)pFVar9->reserved90_AF = iVar7;
                arg5 = pFVar9->reserved90_AF + 4;
                dVar4 = (*g_WideNumberFormatUtf16)
                                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar7 * 4,(word *)arg5);
                pbVar2 = arg5 + dVar4;
                pbVar2[0] = 0x6d;
                pbVar2[1] = 0;
                pbVar2[2] = 0x73;
                pbVar2[3] = 0;
                (arg5 + dVar4 + 4)[0] = 0;
                (arg5 + dVar4 + 4)[1] = 0;
                break;
              }
              pFVar9 = pFVar9 + 1;
              iVar7 = iVar7 + -1;
            } while (iVar7 != 0);
          }
        }
        else {
          g_UiRuntimeRecordWriteIndex = uVar5;
          if (0xff < uVar5) {
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
  uint uVar1;
  int iVar2;
  uint uVar3;
  FrontendTransferPacketUnion *pFVar4;
  dword *pdVar5;
  dword *pUVar6;
  FrontendPlayerNameUtf16_28 *pFVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_40008) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    uVar3 = (packet->packet20002PlayerDescriptor).playerDescriptorPayload[3];
    if (((packet->packet20002PlayerDescriptor).playerDescriptorPayload[2] < 8) && (uVar3 < 9)) {
      pFVar4 = packet;
      pUVar6 = (dword *)(&g_FrontendPlayerListRows)
                        [(packet->packet20002PlayerDescriptor).playerDescriptorPayload[2]];
      g_FrontendPlayerRuntimeCount = uVar3;
      for (iVar2 = 0x20; iVar2 != 0; iVar2 = iVar2 + -1) {
        *pUVar6 = (pFVar4->packet10000Handshake).header.packedTypeAndUnitCount;
        pFVar4 = (FrontendTransferPacketUnion *)&(pFVar4->packet10000Handshake).header.sequenceToken
        ;
        pUVar6 = pUVar6 + 1;
      }
      pFVar7 = g_FrontendPlayerRuntimeBlocks +
               (packet->packet20002PlayerDescriptor).playerDescriptorPayload[2];
      *(dword *)((int)(&pFVar7->playerName + -1) + 0x24) =
           (packet->packet20002PlayerDescriptor).playerDescriptorPayload[4];
      pdVar5 = &(packet->genericTransferPacket).commands[2].payloadDword08;
      pFVar6 = &pFVar7->playerName;
      for (iVar2 = 10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *(dword *)pFVar6->textUtf16 = *pdVar5;
        pdVar5 = pdVar5 + 1;
        pFVar6 = (FrontendPlayerNameUtf16_28 *)(pFVar6->textUtf16 + 2);
      }
      UiPointerList_InitializeColumnLayout
                (uVar3,(void **)&g_FrontendPlayerListRows,
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
      iVar2 = 8;
      pFVar7 = g_FrontendPlayerRuntimeBlocks;
      do {
        (pFVar7->factionAssignment).roleStateFlags = 0;
        pFVar7->runtimeState64 = 0;
        pFVar7->snapshotTransferFlags = 0;
        pFVar7 = pFVar7 + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    return;
  }
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x10) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    uVar3 = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
    do {
      uVar1 = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (uVar1 != 0) {
        if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar1 < &g_FrontendRootNode) {
          (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar1))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
      packet = (FrontendTransferPacketUnion *)
               ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      uVar3 = uVar3 - 1;
    } while (uVar3 != 0);
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
  UiTransferSenderContext UVar1;
  dword dVar2;
  uint uVar3;
  uint uVar4;
  FrontendPlayerRuntimeBlockCount FVar5;
  int iVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  FrontendTransferPacketUnion *pFVar8;
  undefined4 *puVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  byte *pbVar11;
  TextResourceResolveEaxCf5 TVar12;
  
  dVar2 = g_FrontendExpectedPlayerRuntimeBlockCount;
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x10) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    UVar1 = (packet->packet10000Handshake).header.senderContext;
    g_SessionTransferTimeoutTicks = 0x100;
    if (UVar1 == g_FrontendSelectedPlayerToken) {
      UiTransfer_StagePacketAndSendCf
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
      return false;
    }
    uVar4 = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
    g_FrontendSelectedPlayerToken = UVar1;
    do {
      uVar3 = (packet->packet10000Handshake).protocolMagic2931 >> 8;
      if (uVar3 != 0) {
        if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3 < &g_FrontendRootNode) {
          (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3))
                    ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                     (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                     (packet->packet20002PlayerDescriptor).reserved14);
        }
      }
      packet = (FrontendTransferPacketUnion *)
               ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
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
    FVar5 = g_FrontendPlayerRuntimeBlockCount;
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
    do {
      if ((packet->packet10000Handshake).protocolMagic2931 == pFVar10->playerRuntimeId) {
        TVar12 = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&pFVar10->playerName,TVar12.eax);
        FrontendRecentTextHistory_InsertAndRebuild5(TVar12.eax);
        if (FVar5 - 1 != 0) {
          pFVar7 = pFVar10 + 1;
          for (iVar6 = (FVar5 - 1) * 0x4ec; iVar6 != 0; iVar6 = iVar6 + -1) {
            pFVar10->runtimeState00 = pFVar7->runtimeState00;
            pFVar7 = (FrontendPlayerRuntimeRecord *)&pFVar7->peerSequenceToken;
            pFVar10 = (FrontendPlayerRuntimeRecord *)&pFVar10->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return false;
      }
      pFVar10 = pFVar10 + 1;
      FVar5 = FVar5 - 1;
    } while (FVar5 != 0);
    return false;
  }
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
         FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) &&
       (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
      (uVar4 = (packet->packet10000Handshake).protocolMagic2931,
      g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) &&
     ((uVar4 < g_FrontendExpectedPlayerRuntimeBlockCount &&
      (uVar4 == g_FrontendPlayerRuntimeBlockCount)))) {
    Random_SetBothSeeds((packet->packet30005PlayerSnapshot).secondaryRandomSeed);
    Random_SelectSecondaryStream();
    g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount + 1;
    pFVar8 = packet;
    pFVar10 = g_FrontendPlayerRuntimeBlocks + uVar4;
    for (iVar6 = 0x18; iVar6 != 0; iVar6 = iVar6 + -1) {
      pFVar10->runtimeState00 = (pFVar8->packet10000Handshake).header.packedTypeAndUnitCount;
      pFVar8 = (FrontendTransferPacketUnion *)&(pFVar8->packet10000Handshake).header.sequenceToken;
      pFVar10 = (FrontendPlayerRuntimeRecord *)&pFVar10->peerSequenceToken;
    }
    TVar12 = TextResource_Resolve(0xff03);
    RichTextCommandStream_PatchPayloadBySelector
              (0,(packet->packet10000Handshake).reserved14_1F + 4,TVar12.eax);
    FrontendRecentTextHistory_InsertAndRebuild5(TVar12.eax);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10009) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         (packet->packet10009SnapshotChunkRequest).snapshotChunkOffset;
    pbVar11 = g_FrontendPacket8000ABuffer.g_FrontendPacket10009Buffer;
    puVar9 = (undefined4 *)
             (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A;
    iVar6 = 0x3a;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == 0x1220) {
      iVar6 = 0x38;
    }
    for (; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(undefined4 *)pbVar11 = *puVar9;
      puVar9 = puVar9 + 1;
      pbVar11 = pbVar11 + 4;
    }
    if (dVar2 == g_FrontendPlayerRuntimeBlockCount) {
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
FrontendTransfer_MarkUnavailableIfModeBit0Callback(dword arg0,dword arg1,dword arg2,dword arg3)

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
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerRuntimeId == pFVar2->playerRuntimeId) {
      pFVar2->snapshotTransferFlags =
           pFVar2->snapshotTransferFlags | FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
      FVar1 = g_FrontendPlayerRuntimeBlockCount;
      pFVar2 = g_FrontendPlayerRuntimeBlocks;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
        return;
      }
      do {
        if ((pFVar2->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
          return;
        }
        FVar1 = FVar1 - 1;
        pFVar2 = pFVar2 + 1;
      } while (FVar1 != 0);
      (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
      return;
    }
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
  } while (FVar1 != 0);
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
  bool bVar1;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic2931 = 0x2931;
  bVar1 = UiTransfer_StagePacketAndSendCf
                    (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return bVar1;
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
  int iVar1;
  dword *pdVar2;
  dword *pdVar3;
  bool bVar4;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002;
  g_FrontendPacket20002Buffer.payloadByteCount = 0x40;
  pdVar2 = &g_FrontendLocalPlayerNameUtf16;
  pdVar3 = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (iVar1 = 10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *pdVar3 = *pdVar2;
    pdVar2 = pdVar2 + 1;
    pdVar3 = pdVar3 + 1;
  }
  *(undefined2 *)((int)pdVar3 + -2) = 0;
  bVar4 = PcxPreview_Load64x64PaletteAndPixelsCf
                    (g_FrontendLocalPlayerPcxPreview,(word *)&g_FrontendLocalPlayerNameUtf16);
  if (!bVar4) {
    *(ushort *)((int)pdVar3 + -2) = *(ushort *)((int)pdVar3 + -2) | 1;
  }
  *(ushort *)((int)pdVar3 + -2) = *(ushort *)((int)pdVar3 + -2) | 0x100;
  bVar4 = UiTransfer_StagePacketAndSendCf
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return bVar4;
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
  ushort uVar1;
  UiTransferJoinAvailability UVar2;
  uint uVar3;
  word *pwVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  UiTransferEndpointDescriptor *pUVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  dword *joiningPlayerRecordDwordCursor;
  FrontendCommandPacketRecord *pFVar11;
  FrontendCommandPacketRecord *pFVar12;
  TextResourceResolveEaxCf5 TVar13;
  
  iVar6 = g_FrontendRootNode;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10000_HANDSHAKE) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
    if ((((packet->packet10000Handshake).protocolMagic2931 == 0x2931) &&
        (((packet->packet10000Handshake).header.sequenceToken & 0xffff0000) == 0x12340000)) &&
       (*(uint *)(frontendRuntime + 0x5640) < *(uint *)(frontendRuntime + 0x5140))) {
      g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
    }
    TVar13 = TextResource_Resolve(0x211a);
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)0x50f07c,TVar13.eax);
    RichTextCommandStream_CopyExpandedCf
              (0x28,g_FrontendPacket50001Buffer.sessionTitleUtf16,TVar13.eax);
    TVar13 = TextResource_Resolve(0x211b);
    pwVar4 = TVar13.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,(void *)(iVar6 + 0x50c0),pwVar4);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendLocalPlayerNameUtf16,pwVar4);
    RichTextCommandStream_CopyExpandedCf
              (0x58,g_FrontendPacket50001Buffer.hostDescriptionUtf16,pwVar4);
    TVar13 = TextResource_Resolve(0x211c);
    pwVar4 = TVar13.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkRuntimeCountTextUtf16,pwVar4);
    RichTextCommandStream_PatchPayloadBySelector(1,&g_FrontendNetworkPlayerCountTextUtf16,pwVar4);
    RichTextCommandStream_CopyExpandedCf(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,pwVar4);
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
      pFVar11 = g_FrontendPlayerCommandRecords;
      iVar6 = g_FrontendPlayerRuntimeCount;
      pFVar10 = g_FrontendPlayerRuntimeBlocks;
      while (((packet->packet10000Handshake).header.sequenceToken != pFVar10->peerSequenceToken ||
             (senderEndpoint->ipv4AddressNetworkOrder != (pFVar10->endpoint).ipv4AddressNetworkOrder
             ))) {
        pFVar10 = pFVar10 + 1;
        pFVar11 = pFVar11 + 1;
        iVar6 = iVar6 + -1;
        if (iVar6 == 0) {
          return;
        }
      }
      pFVar10->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
        (pFVar11->header).packedTypeAndUnitCount =
             (packet->packet10000Handshake).header.packedTypeAndUnitCount;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        pFVar11 = (FrontendCommandPacketRecord *)&(pFVar11->header).sequenceToken;
      }
      FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
      uVar7 = 0;
      pFVar11 = g_FrontendPlayerCommandRecords;
      pFVar12 = g_FrontendCommandBatchPacketBuffer;
      iVar6 = g_FrontendPlayerRuntimeCount;
      do {
        if (((pFVar11->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
          pFVar11 = pFVar11 + 1;
        }
        else {
          for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
            (pFVar12->header).packedTypeAndUnitCount = (pFVar11->header).packedTypeAndUnitCount;
            pFVar11 = (FrontendCommandPacketRecord *)&(pFVar11->header).sequenceToken;
            pFVar12 = (FrontendCommandPacketRecord *)&(pFVar12->header).sequenceToken;
          }
          uVar7 = uVar7 + 1;
          pFVar11[-1].command.packedCommandAndPlayerId =
               pFVar11[-1].command.packedCommandAndPlayerId & 0xff;
        }
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
      if (uVar7 << 0x10 != 0) {
        g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = uVar7 << 0x10 | 0x10;
        pUVar9 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        iVar6 = g_FrontendPlayerRuntimeCount;
        while (iVar6 = iVar6 + -1, iVar6 != 0) {
          UiTransfer_StagePacketAndSendCf(pUVar9,&g_FrontendCommandBatchPacketBuffer[0].header);
          pUVar9 = pUVar9 + 0x13b;
        }
        pFVar11 = g_FrontendCommandBatchPacketBuffer;
        uVar7 = uVar7 & 0xffff;
        do {
          uVar3 = (pFVar11->command).packedCommandAndPlayerId;
          uVar5 = uVar3 >> 8;
          if (uVar5 != 0) {
            if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar5 < &g_FrontendRootNode) {
              (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar5))
                        (uVar3 & 0xff,(pFVar11->command).payloadDword0C,
                         (pFVar11->command).payloadDword08,(pFVar11->command).payloadDword04);
            }
          }
          pFVar11 = pFVar11 + 1;
          uVar7 = uVar7 - 1;
        } while (uVar7 != 0);
      }
      return;
    }
    iVar6 = *(int *)(frontendRuntime + 0x5640);
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
    while (((packet->packet10000Handshake).header.sequenceToken != pFVar10->peerSequenceToken ||
           (senderEndpoint->ipv4AddressNetworkOrder != (pFVar10->endpoint).ipv4AddressNetworkOrder))
          ) {
      pFVar10 = pFVar10 + 1;
      iVar6 = iVar6 + -1;
      if (iVar6 == 0) {
        return;
      }
    }
    uVar7 = (packet->packet10000Handshake).protocolMagic2931;
    UVar2 = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
    pFVar10->capabilityFlags = uVar7;
    pFVar10->heartbeatExpiryTicks = UVar2;
    pFVar10->reserved78_7F[0] = 0;
    pFVar10->reserved78_7F[1] = 0;
    pFVar10->reserved78_7F[2] = 0;
    pFVar10->reserved78_7F[3] = 0;
    if ((uVar7 & 0x100) != 0) {
      pFVar10->reserved78_7F[0] = 0x43;
      pFVar10->reserved78_7F[1] = 0;
      pFVar10->reserved78_7F[2] = 0x44;
      pFVar10->reserved78_7F[3] = 0;
    }
    FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction();
    return;
  }
  joiningPlayerRecordDwordCursor =
       *(dword **)(*(int *)(frontendRuntime + 0x563c) + *(int *)(frontendRuntime + 0x5640) * 4);
  *(int *)(frontendRuntime + 0x5640) = *(int *)(frontendRuntime + 0x5640) + 1;
  for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
    *joiningPlayerRecordDwordCursor = (packet->packet10000Handshake).header.packedTypeAndUnitCount;
    packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
    joiningPlayerRecordDwordCursor = joiningPlayerRecordDwordCursor + 1;
  }
  pUVar9 = senderEndpoint;
  for (iVar6 = 4; iVar6 != 0; iVar6 = iVar6 + -1) {
    *joiningPlayerRecordDwordCursor = (dword)pUVar9->addressHeader;
    pUVar9 = (UiTransferEndpointDescriptor *)&pUVar9->ipv4AddressNetworkOrder;
    joiningPlayerRecordDwordCursor = joiningPlayerRecordDwordCursor + 1;
  }
  uVar7 = 0;
  iVar6 = g_FrontendPlayerRuntimeCount;
  pFVar10 = g_FrontendPlayerRuntimeBlocks;
  do {
    while (uVar7 != pFVar10->playerRuntimeId) {
      iVar6 = iVar6 + -1;
      pFVar10 = pFVar10 + 1;
      if (iVar6 == 0)
      goto 
      FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets_InitializeJoiningPlayerRecordWithNextAvailableId
      ;
    }
    uVar7 = uVar7 + 1;
    iVar6 = g_FrontendPlayerRuntimeCount;
    pFVar10 = g_FrontendPlayerRuntimeBlocks;
  } while (uVar7 < 0xff);

  FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets_InitializeJoiningPlayerRecordWithNextAvailableId
  :
  uVar1 = *(ushort *)((int)joiningPlayerRecordDwordCursor + -0x12);
  *joiningPlayerRecordDwordCursor = 1;
  joiningPlayerRecordDwordCursor[-0xf] = uVar7;
  joiningPlayerRecordDwordCursor[6] = uVar1 & 0xff;
  joiningPlayerRecordDwordCursor[9] = uVar1 & 0xff00;
  joiningPlayerRecordDwordCursor[0x10] = 0;
  joiningPlayerRecordDwordCursor[0x11] = 0;
  joiningPlayerRecordDwordCursor[7] = 0;
  joiningPlayerRecordDwordCursor[10] = 0;
  joiningPlayerRecordDwordCursor[0xb] = 0;
  joiningPlayerRecordDwordCursor[0xd] = 0;
  joiningPlayerRecordDwordCursor[0xe] = 0;
  joiningPlayerRecordDwordCursor[0xf] = 0;
  if ((uVar1 & 0x100) != 0) {
    joiningPlayerRecordDwordCursor[10] = 0x440043;
  }
  *(undefined2 *)((int)joiningPlayerRecordDwordCursor + -0x12) = 0;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,*(sdword *)(frontendRuntime + 0x5640),
             (word *)&g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = uVar7;
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
  dword dVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  UiTransferEndpointDescriptor *pUVar7;
  byte *pbVar8;
  UiTransferEndpointDescriptor *endpoint;
  FrontendCommandPacketRecord *pFVar9;
  dword *pdVar10;
  FrontendCommandPacketRecord *pFVar11;
  
  dVar1 = g_FrontendHostPublishRoundRobinCounter;
  uVar6 = *(uint *)(frontendRuntime + 0x5640);
  pUVar7 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  iVar3 = uVar6 - 1;
  if (iVar3 != 0 && 0 < (int)uVar6) {
    g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008;
    g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
    g_FrontendHostPublishRoundRobinCounter = g_FrontendHostPublishRoundRobinCounter + 1;
    uVar5 = dVar1 % uVar6;
    g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
         pUVar7[uVar5 * 0x13b + -0x13e].ipv4AddressNetworkOrder;
    g_FrontendPacket40008Buffer.selectedStatusCode0 =
         *(FrontendStatusCode *)pUVar7[uVar5 * 0x13b + -0x138].zeroPadding;
    g_FrontendPacket40008Buffer.selectedStatusCode1 =
         *(FrontendStatusCode *)(pUVar7[uVar5 * 0x13b + -0x138].zeroPadding + 4);
    g_FrontendPacket40008Buffer.selectedPlayerIndex = uVar5;
    g_FrontendPacket40008Buffer.playerCount = uVar6;
    endpoint = pUVar7;
    dVar1 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                       pUVar7[uVar5 * 0x13b + -0x136].addressHeader.packedFamilyAndPort << 2,
                       g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
    *(undefined4 *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + dVar1) =
         0x73006d;
    *(undefined2 *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + dVar1 + 4) = 0;
    pbVar8 = pUVar7[uVar5 * 0x13b + -0x13e].zeroPadding;
    pdVar10 = g_FrontendPacket40008Buffer.playerDescriptorPayload;
    for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
      *pdVar10 = *(dword *)pbVar8;
      pbVar8 = pbVar8 + 4;
      pdVar10 = pdVar10 + 1;
    }
    g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032;
    g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
    do {
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket40008Buffer.header);
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket10032Buffer.header);
      endpoint = endpoint + 0x13b;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = 4;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  uVar6 = 0;
  pFVar9 = g_FrontendPlayerCommandRecords;
  pFVar11 = g_FrontendCommandBatchPacketBuffer;
  iVar3 = g_FrontendPlayerRuntimeCount;
  do {
    if (((pFVar9->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      pFVar9 = pFVar9 + 1;
    }
    else {
      for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
        (pFVar11->header).packedTypeAndUnitCount = (pFVar9->header).packedTypeAndUnitCount;
        pFVar9 = (FrontendCommandPacketRecord *)&(pFVar9->header).sequenceToken;
        pFVar11 = (FrontendCommandPacketRecord *)&(pFVar11->header).sequenceToken;
      }
      uVar6 = uVar6 + 1;
      pFVar9[-1].command.packedCommandAndPlayerId =
           pFVar9[-1].command.packedCommandAndPlayerId & 0xff;
    }
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  if (uVar6 << 0x10 != 0) {
    g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = uVar6 << 0x10 | 0x10;
    pUVar7 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
    iVar3 = g_FrontendPlayerRuntimeCount;
    while (iVar3 = iVar3 + -1, iVar3 != 0) {
      UiTransfer_StagePacketAndSendCf(pUVar7,&g_FrontendCommandBatchPacketBuffer[0].header);
      pUVar7 = pUVar7 + 0x13b;
    }
    pFVar9 = g_FrontendCommandBatchPacketBuffer;
    uVar6 = uVar6 & 0xffff;
    do {
      uVar5 = (pFVar9->command).packedCommandAndPlayerId;
      uVar2 = uVar5 >> 8;
      if (uVar2 != 0) {
        if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2 < &g_FrontendRootNode) {
          (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2))
                    (uVar5 & 0xff,(pFVar9->command).payloadDword0C,(pFVar9->command).payloadDword08,
                     (pFVar9->command).payloadDword04);
        }
      }
      pFVar9 = pFVar9 + 1;
      uVar6 = uVar6 - 1;
    } while (uVar6 != 0);
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
  FrontendPlayerRuntimeBlockCount FVar1;
  int iVar2;
  int iVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendCommandPacketRecord *pFVar5;
  UiTransferEndpointDescriptor *pUVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  FrontendCommandPacketRecord *pFVar8;
  
  iVar2 = g_FrontendPlayerRuntimeBlockCount - 1;
  pFVar7 = g_FrontendPlayerRuntimeBlocks;
  if (iVar2 != 0) {
    do {
      if (pFVar7[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        if (sendStateReplies != 0) {
          pUVar6 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
          FVar1 = g_FrontendPlayerRuntimeBlockCount;
          while (FVar1 = FVar1 - 1, FVar1 != 0) {
            if (pUVar6[1].addressHeader.packedFamilyAndPort == 0) {
              UiTransfer_StagePacketAndSendCf
                        (pUVar6,&g_FrontendClientCommandBatchPacketBuffer[0].header);
            }
            else {
              g_FrontendPacket10022Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10022;
              UiTransfer_StagePacketAndSendCf(pUVar6,&g_FrontendPacket10022Buffer.header);
            }
            pUVar6 = pUVar6 + 0x13b;
          }
        }
        return true;
      }
      iVar2 = iVar2 + -1;
      pFVar7 = pFVar7 + 1;
    } while (iVar2 != 0);
    iVar2 = g_FrontendPlayerRuntimeBlockCount - 1;
    pFVar7 = g_FrontendPlayerRuntimeBlocks;
    do {
      pFVar7[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      iVar2 = iVar2 + -1;
      pFVar7 = pFVar7 + 1;
    } while (iVar2 != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  iVar2 = 0;
  pFVar5 = g_FrontendClientPlayerCommandRecords;
  pFVar8 = g_FrontendClientCommandBatchPacketBuffer;
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  do {
    FVar4 = FVar1;
    if (((pFVar5->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      pFVar5 = pFVar5 + 1;
      goto 
      FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip;
    }
    while( true ) {
      for (iVar3 = 8; iVar3 != 0; iVar3 = iVar3 + -1) {
        (pFVar8->header).packedTypeAndUnitCount = (pFVar5->header).packedTypeAndUnitCount;
        pFVar5 = (FrontendCommandPacketRecord *)&(pFVar5->header).sequenceToken;
        pFVar8 = (FrontendCommandPacketRecord *)&(pFVar8->header).sequenceToken;
      }
      iVar2 = iVar2 + 1;
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip:
      FVar1 = FVar4 - 1;
      if (FVar4 - 1 != 0) break;
      if (iVar2 << 0x10 != 0) {
        g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
             iVar2 << 0x10 | 0x20;
        pUVar6 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        FVar1 = g_FrontendPlayerRuntimeBlockCount;
        while (FVar1 = FVar1 - 1, FVar1 != 0) {
          UiTransfer_StagePacketAndSendCf
                    (pUVar6,&g_FrontendClientCommandBatchPacketBuffer[0].header);
          pUVar6 = pUVar6 + 0x13b;
        }
        return false;
      }
      pFVar5 = g_FrontendClientPlayerCommandRecords;
      iVar2 = 0;
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
  undefined4 in_EAX;
  undefined4 in_ECX;
  UiTransferMailboxReceivedEaxEcxCf9 UVar1;
  UiTransferMailboxReceivedEaxEcxCf9 UVar2;
  
  if (((g_UiTransferMailbox.receivedAllocation != (void *)0xffffffff) &&
      (g_UiTransferMailbox.receivedAllocation != (void *)0x0)) &&
     (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
    UVar1.ecx = g_UiTransferMailbox.receivedByteCount;
    UVar1.eax = (dword)g_UiTransferMailbox.receivedAllocation;
    UVar1.carry = false;
    return UVar1;
  }
  UVar2.ecx = in_ECX;
  UVar2.eax = in_EAX;
  UVar2.carry = true;
  return UVar2;
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
  int iVar1;
  FrontendSessionDiscoveryRecordB0 **ppFVar2;
  dword *sessionDiscoveryRecordDwordCursor;
  undefined4 *puVar3;
  
  iVar1 = *(int *)(frontendRuntime + 0x4bbc);
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    sessionDiscoveryRecordDwordCursor = (dword *)g_FrontendSessionDiscoveryRecords;
    ppFVar2 = g_FrontendSessionListRows;
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      if (((packet->packet10000Handshake).header.sequenceToken ==
           (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
           header.sequenceToken) &&
         (senderEndpoint->ipv4AddressNetworkOrder ==
          (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->senderEndpoint).
          ipv4AddressNetworkOrder))
      goto 
      FrontendTransfer_HandleSessionListAndJoinAckPackets_UpdateOrAppendSessionAdvertisementAndRefreshList
      ;
      ppFVar2 = ppFVar2 + 1;
      sessionDiscoveryRecordDwordCursor = (dword *)((int)sessionDiscoveryRecordDwordCursor + 0xb0);
    }
    if (*(uint *)(frontendRuntime + 0x4bbc) < 0x20) {
      *ppFVar2 = (FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor;
      *(int *)(frontendRuntime + 0x4bbc) = *(int *)(frontendRuntime + 0x4bbc) + 1;
FrontendTransfer_HandleSessionListAndJoinAckPackets_UpdateOrAppendSessionAdvertisementAndRefreshList
      :
      (packet->packet10000Handshake).protocolMagic2931 = 0x20;
      for (iVar1 = 0x28; iVar1 != 0; iVar1 = iVar1 + -1) {
        (((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->advertisement).
        header.packedTypeAndUnitCount = (packet->packet10000Handshake).header.packedTypeAndUnitCount
        ;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        sessionDiscoveryRecordDwordCursor =
             &(((FrontendSessionDiscoveryRecordB0 *)sessionDiscoveryRecordDwordCursor)->
              advertisement).header.sequenceToken;
      }
      for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
        *sessionDiscoveryRecordDwordCursor = (dword)senderEndpoint->addressHeader;
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
    puVar3 = (undefined4 *)g_FrontendPlayerListRows;
    for (iVar1 = 0x100; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
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
  int iVar1;
  
  iVar1 = g_FrontendTransferResponsePending;
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return iVar1 == 0;
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
  UiTransferSequenceToken UVar1;
  UiTransferSenderContext UVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
  int iVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  FrontendCommandPacketRecord *pFVar6;
  
  UVar1 = (packet->packet10000Handshake).header.sequenceToken;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_10021) {
    FVar3 = g_FrontendPlayerRuntimeBlockCount;
    pFVar5 = g_FrontendPlayerRuntimeBlocks;
    if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_10023) {
      return;
    }
    while ((UVar1 != pFVar5->peerSequenceToken ||
           (sourceContext->ipv4AddressNetworkOrder != (pFVar5->endpoint).ipv4AddressNetworkOrder)))
    {
      FVar3 = FVar3 - 1;
      pFVar5 = pFVar5 + 1;
      if (FVar3 == 0) {
        return;
      }
    }
    pFVar5->heartbeatExpiryTicks = 0x100;
    return;
  }
  pFVar6 = g_FrontendClientPlayerCommandRecords;
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
  pFVar5 = g_FrontendPlayerRuntimeBlocks;
  while ((UVar1 != pFVar5->peerSequenceToken ||
         (sourceContext->ipv4AddressNetworkOrder != (pFVar5->endpoint).ipv4AddressNetworkOrder))) {
    pFVar6 = pFVar6 + 1;
    pFVar5 = pFVar5 + 1;
    FVar3 = FVar3 - 1;
    if (FVar3 == 0) {
      return;
    }
  }
  UVar2 = (packet->packet10000Handshake).header.senderContext;
  pFVar5->heartbeatExpiryTicks = 0x100;
  if (UVar2 != (pFVar6->header).senderContext) {
    pFVar5->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
      (pFVar6->header).packedTypeAndUnitCount =
           (packet->packet10000Handshake).header.packedTypeAndUnitCount;
      packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken;
      pFVar6 = (FrontendCommandPacketRecord *)&(pFVar6->header).sequenceToken;
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
  uint uVar1;
  uint uVar2;
  uint uVar3;
  FrontendCommandPacketRecord *pFVar4;
  
  pFVar4 = g_FrontendClientCommandBatchPacketBuffer;
  for (uVar3 = g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount >> 0x10;
      uVar3 != 0; uVar3 = uVar3 - 1) {
    uVar1 = (pFVar4->command).packedCommandAndPlayerId;
    uVar2 = uVar1 >> 8;
    if (uVar2 != 0) {
      if (InGameCommandQueue_AppendLocalPlayerCommand + uVar2 < &InGameCommandHandlerCodeRegionEnd)
      {
        (*(InGameCommandQueue_AppendLocalPlayerCommand + uVar2))
                  (uVar1 & 0xff,(pFVar4->command).payloadDword0C,(pFVar4->command).payloadDword08,
                   (pFVar4->command).payloadDword04);
      }
    }
    pFVar4 = pFVar4 + 1;
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
  int iVar1;
  
  iVar1 = g_FrontendTransferResponsePending;
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return iVar1 == 0;
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
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
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
        puVar1 = roundKeys16 + roundIndex;
        puVar2 = roundKeys16 + roundIndex;
        puVar3 = roundKeys16 + roundIndex;
        puVar4 = roundKeys16 + roundIndex;
        puVar5 = roundKeys16 + roundIndex;
        puVar6 = roundKeys16 + roundIndex;
        puVar7 = roundKeys16 + roundIndex;
        puVar8 = roundKeys16 + roundIndex;
        roundIndex = roundIndex + 1;
        rightState = (((((((*(int *)(&g_RandomPrimaryNibbleMixTable0 +
                                    (roundInputHalf & 0xf) * 4 + (*puVar1 & 0xf) * 0x40) << 4 |
                           *(uint *)(&g_RandomPrimaryNibbleMixTable1 +
                                    ((roundInputHalf & 0xf0) >> 4) * 4 + (*puVar2 & 0xf0) * 4)) << 4
                          | *(uint *)(&g_RandomPrimaryNibbleMixTable2 +
                                     ((roundInputHalf & 0xf00) >> 8) * 4 + ((*puVar3 & 0xf00) >> 2))
                          ) << 4 | *(uint *)(&g_RandomPrimaryNibbleMixTable3 +
                                            ((roundInputHalf & 0xf000) >> 0xc) * 4 +
                                            ((*puVar4 & 0xf000) >> 6))) << 4 |
                        *(uint *)(&g_RandomPrimaryNibbleMixTable4 +
                                 ((roundInputHalf & 0xf0000) >> 0x10) * 4 +
                                 ((*puVar5 & 0xf0000) >> 10))) << 4 |
                       *(uint *)(&g_RandomPrimaryNibbleMixTable5 +
                                ((roundInputHalf & 0xf00000) >> 0x14) * 4 +
                                ((*puVar6 & 0xf00000) >> 0xe))) << 4 |
                      *(uint *)(&g_RandomPrimaryNibbleMixTable6 +
                               ((roundInputHalf & 0xf000000) >> 0x18) * 4 +
                               ((*puVar7 & 0xf000000) >> 0x12))) << 4 |
                     *(uint *)(&g_RandomPrimaryNibbleMixTable7 +
                              (roundInputHalf >> 0x1c) * 4 + ((*puVar8 & 0xf0000000) >> 0x16))) ^
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
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uStack_28;
  uint uStack_24;
  uint uStack_20;
  
  uStack_28 = byteCount >> 3;
  if (uStack_28 != 0) {
    uStack_20 = 0;
    uStack_24 = 0;
    do {
      iVar2 = 0xf;
      uVar1 = *(uint *)source;
      uVar3 = *(uint *)((int)source + 4);
      do {
        uVar4 = uVar1;
        uVar3 = uVar3 ^ uVar4;
        uVar1 = ((((((*(int *)(((roundKeys16[iVar2] & 0xf0000000) >> 0x16) + 0x406d60 +
                              (uVar3 & 0xf) * 4) << 4 |
                     *(uint *)(((roundKeys16[iVar2] & 0xf000000) >> 0x12) + 0x406960 +
                              ((uVar3 & 0xf0) >> 4) * 4)) << 4 |
                    *(uint *)(((roundKeys16[iVar2] & 0xf00000) >> 0xe) + 0x406560 +
                             ((uVar3 & 0xf00) >> 8) * 4)) << 4 |
                   *(uint *)(((roundKeys16[iVar2] & 0xf0000) >> 10) + 0x406160 +
                            ((uVar3 & 0xf000) >> 0xc) * 4)) << 4 |
                  *(uint *)(((roundKeys16[iVar2] & 0xf000) >> 6) + 0x405d60 +
                           ((uVar3 & 0xf0000) >> 0x10) * 4)) << 4 |
                 *(uint *)(((roundKeys16[iVar2] & 0xf00) >> 2) + 0x405960 +
                          ((uVar3 & 0xf00000) >> 0x14) * 4)) << 4 |
                *(uint *)((roundKeys16[iVar2] & 0xf0) * 4 + 0x405560 +
                         ((uVar3 & 0xf000000) >> 0x18) * 4)) << 4 |
                *(uint *)((roundKeys16[iVar2] & 0xf) * 0x40 + 0x405160 + (uVar3 >> 0x1c) * 4);
        iVar2 = iVar2 + -1;
        uVar3 = uVar4;
      } while (-1 < iVar2);
      uVar1 = uVar1 ^ uStack_24;
      uVar4 = uVar4 ^ uStack_20;
      uStack_24 = *(uint *)source;
      uStack_20 = *(uint *)((int)source + 4);
      *(uint *)destination = uVar1;
      *(uint *)((int)destination + 4) = uVar4;
      source = (void *)((int)source + 8);
      destination = (void *)((int)destination + 8);
      uStack_28 = uStack_28 - 1;
    } while (uStack_28 != 0);
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
  FrontendPlayerRuntimeBlockCount FVar1;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  UiTransfer_StagePacketAndSendCf
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
  if (FVar1 < g_FrontendExpectedPlayerRuntimeBlockCount) {
    g_FrontendPacket10004Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_10004_SNAPSHOT_REQUEST;
    g_FrontendPacket10004Buffer.requestedPlayerIndex = FVar1;
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
  uint uVar1;
  bool bVar2;
  byte *pbVar3;
  dword dVar4;
  dword dVar5;
  UiTransferXorChecksum UVar6;
  uint uVar6;
  UiTransferPayloadByteCount byteCount;
  UiTransferPayloadByteCount UVar7;
  UiTransferPayloadByteCount UVar8;
  int iVar9;
  int iVar10;
  dword *outputBlocks;
  UiTransferPacketHeader *pUVar11;
  dword *endpointDestinationDwordCursor;
  NetworkBackendSendEaxCf5 NVar12;
  
  dVar5 = g_UiTransferSenderContext;
  dVar4 = g_UiTransferSequenceToken;
  uVar6 = packet->packedTypeAndUnitCount >> 0x10;
  iVar10 = g_UiTransferUnitCursor << 5;
  uVar1 = uVar6 + g_UiTransferUnitCursor;
  iVar9 = g_UiTransferUnitCursor << 4;
  g_UiTransferUnitCursor = uVar1;
  if (0xff < uVar1) {
    iVar10 = 0;
    iVar9 = 0;
    g_UiTransferUnitCursor = uVar6;
  }
  outputBlocks = (dword *)(g_UiTransferDataBuffer + iVar10);
  pbVar3 = g_UiTransferEndpointBuffer->zeroPadding;
  byteCount = uVar6 << 5;
  packet->xorChecksum = 0;
  packet->sequenceToken = dVar4;
  packet->senderContext = dVar5;
  UVar6 = 0;
  UVar7 = byteCount;
  pUVar11 = packet;
  do {
    UVar6 = UVar6 ^ pUVar11->packedTypeAndUnitCount;
    pUVar11 = (UiTransferPacketHeader *)&pUVar11->sequenceToken;
    UVar8 = UVar7 - 4;
    bVar2 = 3 < (int)UVar7;
    UVar7 = UVar8;
  } while (UVar8 != 0 && bVar2);
  packet->xorChecksum = UVar6;
  UiTransfer_TransformPacketBlocks
            ((dword *)&g_UiTransferRoundKeys16,outputBlocks,byteCount,
             &packet->packedTypeAndUnitCount);
  endpointDestinationDwordCursor = (dword *)(pbVar3 + iVar9 + -8);
  for (iVar10 = 4; iVar10 != 0; iVar10 = iVar10 + -1) {
    *endpointDestinationDwordCursor = (dword)endpoint->addressHeader;
    endpoint = (UiTransferEndpointDescriptor *)&endpoint->ipv4AddressNetworkOrder;
    endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
  }
  NVar12 = (*g_NetworkBackendSlot5)
                     ((WinSockAddress *)(pbVar3 + iVar9 + -8),byteCount,(byte *)outputBlocks);
  return NVar12.carry;
}

