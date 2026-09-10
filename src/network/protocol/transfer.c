#include <thandor/network/protocol/transfer.h>

/* Implementation ownership: network/protocol/transfer. */

/* Address: 0x004AEB10.
   Ownership: network/protocol/transfer.
   Purpose: 125 ms transfer-mailbox service timer. Locks the mailbox/ring, consumes validated records, handles
   segmented payload and acknowledgement/retry records, and updates retransmission state.
   Local calls: UiTransferBlock_Transform64BitBlocksWithRoundKeys16, UiTransfer_StagePacketAndSendCf.
*/
void __cdecl UiTransferMailbox_ServiceAndRetransmitTimer(void)

{
  UiTransferXorChecksum *pUVar1;
  UiTransferXorChecksum UVar2;
  UiTransferPayloadByteCount UVar3;
  dword dVar4;
  uint extraout_ECX;
  uint uVar5;
  FrontendPlayerRuntimeBlockCount FVar6;
  int iVar7;
  UiTransferPayloadByteCount UVar8;
  uint uVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  byte *arg5;
  UiTransferAuxiliaryEndpointRecord80 *auxiliaryEndpointRecord;
  byte *pbVar11;
  undefined4 *puVar12;
  UiRuntimeRecord *pUVar13;
  undefined4 *puVar14;
  undefined1 in_CF;
  bool bVar15;
  void *pvVar16;
  
  g_UiTransferMailboxTickCounter = g_UiTransferMailboxTickCounter + 1;
  (*g_SpinLockTryAcquire)(&g_UiRuntimeRecordRingLock);
  if (!(bool)in_CF) {
UiTransferMailbox_ReleaseRingLockAndReturn:
    pUVar13 = g_UiRuntimeRecordRing + g_UiRuntimeRecordWriteIndex;
    bVar15 = CARRY4(g_UiRuntimeRecordWriteIndex * 0x80,g_UiRuntimeAuxiliaryBuffer8000);
    (*g_NetworkBackendSlot4)
              ((WinSockAddress *)
               (g_UiRuntimeRecordWriteIndex * 0x80 + g_UiRuntimeAuxiliaryBuffer8000),0x100,
               (byte *)pUVar13);
    if (!bVar15) {
      UiTransferBlock_Transform64BitBlocksWithRoundKeys16
                ((dword *)&g_UiTransferRoundKeys16,pUVar13,0x100,pUVar13);
      LOCK();
      pUVar1 = &(pUVar13->packetHeader).xorChecksum;
      UVar2 = *pUVar1;
      *pUVar1 = 0;
      UNLOCK();
      iVar7 = ((pUVar13->packetHeader).packedTypeAndUnitCount >> 0x10) << 3;
      do {
        UVar2 = UVar2 ^ (pUVar13->packetHeader).packedTypeAndUnitCount;
        pUVar13 = (UiRuntimeRecord *)&(pUVar13->packetHeader).sequenceToken;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      if (UVar2 == 0) {
        pUVar13 = g_UiRuntimeRecordRing + (extraout_ECX - 1);
        auxiliaryEndpointRecord =
             (UiTransferAuxiliaryEndpointRecord80 *)
             ((extraout_ECX - 1) * 0x80 + g_UiRuntimeAuxiliaryBuffer8000);
        if ((pUVar13->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_80030) {
          if ((g_FrontendSessionToken == (pUVar13->packetHeader).sequenceToken) &&
             (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
              (auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder)) {
            g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks + 0x40;
            iVar7 = *(int *)pUVar13->payload10_FF;
            _pvVar16 = (void *)CONCAT44(*(dword *)(pUVar13->payload10_FF + 4),
                                        g_UiTransferMailbox.receivedAllocation);
            if (g_UiTransferMailbox.receivedAllocation != (void *)0x0) {
              bVar15 = g_UiTransferMailbox.receivedAllocation != (void *)0xffffffff;
              if (g_UiTransferMailbox.receivedAllocation == (void *)0xffffffff) {
                _pvVar16 = (*g_MemoryApi.alloc)(*(dword *)(pUVar13->payload10_FF + 4));
                UVar3 = (UiTransferPayloadByteCount)((ulonglong)_pvVar16 >> 0x20);
                if (bVar15) goto UiTransferMailbox_ReleaseRingLockAndReturn;
                iVar7 = 0;
                g_UiTransferMailbox.receivedByteCount = UVar3;
                g_UiTransferMailbox.receivedRemainingBytes = UVar3;
              }
              UVar8 = (UiTransferPayloadByteCount)((ulonglong)_pvVar16 >> 0x20);
              g_UiTransferMailbox.receivedAllocation = SUB84(_pvVar16,0);
              UVar3 = iVar7 + g_UiTransferMailbox.receivedRemainingBytes;
              if ((UVar3 == g_UiTransferMailbox.receivedByteCount) && (UVar3 == UVar8)) {
                iVar7 = UVar3 - g_UiTransferMailbox.receivedRemainingBytes;
                uVar9 = UVar8 - iVar7;
                uVar5 = 0xe8;
                if (uVar9 < 0xe8) {
                  uVar5 = uVar9;
                }
                g_UiTransferMailbox.receivedRemainingBytes =
                     g_UiTransferMailbox.receivedRemainingBytes - uVar5;
                pbVar11 = pUVar13->payload10_FF + 8;
                puVar12 = (undefined4 *)((int)g_UiTransferMailbox.receivedAllocation + iVar7);
                for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
                  *puVar12 = *(undefined4 *)pbVar11;
                  pbVar11 = pbVar11 + 4;
                  puVar12 = puVar12 + 1;
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
        else if ((pUVar13->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10031) {
          if (g_UiTransferMailbox.outgoingAllocation != (void *)0x0) {
            FVar6 = g_FrontendPlayerRuntimeBlockCount;
            pFVar10 = g_FrontendPlayerRuntimeBlocks;
            while (((pUVar13->packetHeader).sequenceToken != pFVar10->peerSequenceToken ||
                   ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder !=
                    (pFVar10->endpoint).ipv4AddressNetworkOrder))) {
              pFVar10 = pFVar10 + 1;
              FVar6 = FVar6 - 1;
              if (FVar6 == 0) goto UiTransferMailbox_ReleaseRingLockAndReturn;
            }
            auxiliaryEndpointRecord->transferTimeoutTicks =
                 auxiliaryEndpointRecord->transferTimeoutTicks + 0x40;
            g_UiTransferMailboxChunkOffset = *(UiTransferMailboxByteOffset *)pUVar13->payload10_FF;
            pFVar10->runtimeState70 = 0xe8;
            g_UiTransferMailboxTransferByteCount = g_UiTransferMailbox.outgoingByteCount;
            pFVar10->runtimeState70 = pFVar10->runtimeState70 + g_UiTransferMailboxChunkOffset;
            s_mohTG_sakere___e_004ae9d8[0x10] = '0';
            s_mohTG_sakere___e_004ae9d8[0x11] = '\0';
            s_mohTG_sakere___e_004ae9d8[0x12] = '\b';
            s_mohTG_sakere___e_004ae9d8[0x13] = '\0';
            uVar9 = g_UiTransferMailboxTransferByteCount - g_UiTransferMailboxChunkOffset;
            uVar5 = 0xe8;
            if (uVar9 < 0xe8) {
              uVar5 = uVar9;
            }
            puVar12 = (undefined4 *)
                      ((int)g_UiTransferMailbox.outgoingAllocation + g_UiTransferMailboxChunkOffset)
            ;
            puVar14 = (undefined4 *)0x4aea00;
            for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
              *puVar14 = *puVar12;
              puVar12 = puVar12 + 1;
              puVar14 = puVar14 + 1;
            }
            g_UiTransferChunkPacketSequenceToken = g_UiTransferSequenceToken;
            UiTransfer_StagePacketAndSendCf
                      ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                       (UiTransferPacketHeader *)(s_mohTG_sakere___e_004ae9d8 + 0x10));
          }
        }
        else if ((pUVar13->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10032) {
          g_UiTransferMailboxReplyPacket10033EchoedTick = *(undefined4 *)pUVar13->payload10_FF;
          g_UiTransferMailboxReplyPacket10033 = 0x10033;
          g_UiTransferMailboxReplyPacket10033SequenceToken = g_UiTransferSequenceToken;
          UiTransfer_StagePacketAndSendCf
                    ((UiTransferEndpointDescriptor *)auxiliaryEndpointRecord,
                     (UiTransferPacketHeader *)&g_UiTransferMailboxReplyPacket10033);
        }
        else if ((pUVar13->packetHeader).packedTypeAndUnitCount == FRONTEND_PACKET_10033) {
          iVar7 = g_FrontendPlayerRuntimeCount;
          pFVar10 = g_FrontendPlayerRuntimeBlocks;
          if (0 < g_FrontendPlayerRuntimeCount) {
            do {
              if (((pUVar13->packetHeader).sequenceToken == pFVar10->peerSequenceToken) &&
                 ((auxiliaryEndpointRecord->endpoint).ipv4AddressNetworkOrder ==
                  (pFVar10->endpoint).ipv4AddressNetworkOrder)) {
                iVar7 = g_UiTransferMailboxTickCounter - *(int *)pUVar13->payload10_FF;
                *(int *)pFVar10->reserved90_AF = iVar7;
                arg5 = pFVar10->reserved90_AF + 4;
                dVar4 = (*g_WideNumberFormatUtf16)
                                  (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar7 * 4,(word *)arg5);
                pbVar11 = arg5 + dVar4;
                pbVar11[0] = 0x6d;
                pbVar11[1] = 0;
                pbVar11[2] = 0x73;
                pbVar11[3] = 0;
                (arg5 + dVar4 + 4)[0] = 0;
                (arg5 + dVar4 + 4)[1] = 0;
                break;
              }
              pFVar10 = pFVar10 + 1;
              iVar7 = iVar7 + -1;
            } while (iVar7 != 0);
          }
        }
        else {
          g_UiRuntimeRecordWriteIndex = extraout_ECX;
          if (0xff < extraout_ECX) {
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
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
               (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
               FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint uVar1;
  int iVar2;
  undefined4 extraout_ECX;
  uint uVar3;
  uint extraout_ECX_00;
  FrontendTransferPacketUnion *extraout_EDX;
  undefined4 extraout_EDX_00;
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
      packet = extraout_EDX;
    }
    g_SessionTransferTimeoutTicks = 0x40;
    if ((packet->packet10009SnapshotChunkRequest).reserved10 != 0) {
      g_FrontendPlayerRuntimeBlockCount = 0;
      g_FrontendExpectedPlayerRuntimeBlockCount =
           (packet->packet10009SnapshotChunkRequest).reserved10;
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(frontendRuntime + 0x508));
      FrontendState_DispatchCode(1);
      g_FrontendNetworkState = 5;
      FrontendTransfer_SendQueued10011AndOptional10004(extraout_ECX,extraout_EDX_00);
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
                     (packet->packet50001SessionAdvertisement).joinAvailableFlag);
          uVar3 = extraout_ECX_00;
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
undefined8 __fastcall
FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf
          (undefined4 param_1,undefined4 param_2,UiTransferEndpointDescriptor *senderEndpoint,
          FrontendTransferPacketUnion *packet)

{
  UiTransferSenderContext UVar1;
  dword dVar2;
  undefined4 in_EAX;
  uint uVar3;
  word *pwVar4;
  uint uVar5;
  uint extraout_ECX;
  FrontendPlayerRuntimeBlockCount FVar6;
  int extraout_ECX_00;
  int iVar7;
  int extraout_ECX_01;
  FrontendTransferPacketUnion *extraout_EDX;
  void *replacementPayload;
  dword *extraout_EDX_00;
  void *replacementPayload_00;
  FrontendPlayerRuntimeRecord *pFVar8;
  dword *pdVar9;
  undefined4 *puVar10;
  FrontendTransferPacketUnion *pFVar11;
  FrontendPlayerRuntimeRecord *pFVar12;
  byte *pbVar13;
  
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
      return CONCAT44(param_2,in_EAX);
    }
    uVar5 = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
    pFVar11 = packet;
    g_FrontendSelectedPlayerToken = UVar1;
    do {
      uVar3 = (pFVar11->packet10000Handshake).protocolMagic2931 >> 8;
      if (uVar3 != 0) {
        packet = (FrontendTransferPacketUnion *)
                 (pFVar11->packet20002PlayerDescriptor).playerDescriptorPayload[0];
        if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3 < &g_FrontendRootNode) {
          (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3))
                    ((pFVar11->packet10000Handshake).protocolMagic2931 & 0xff,
                     (pFVar11->packet20002PlayerDescriptor).playerDescriptorPayload[1],packet,
                     (pFVar11->packet50001SessionAdvertisement).joinAvailableFlag);
          uVar5 = extraout_ECX;
          packet = extraout_EDX;
        }
      }
      pFVar11 = (FrontendTransferPacketUnion *)
                ((pFVar11->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
      uVar5 = uVar5 - 1;
    } while (uVar5 != 0);
    FrontendTransfer_SendQueued10011AndOptional10004(0,packet);
    g_FrontendTransferResponsePending = 1;
    return CONCAT44(param_2,in_EAX);
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10012) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = 0x100;
    g_FrontendPacket10013Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10013;
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10013Buffer.header);
    return CONCAT44(param_2,in_EAX);
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_10007_PLAYER_REMOVAL) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    FVar6 = g_FrontendPlayerRuntimeBlockCount;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
    do {
      if ((packet->packet10000Handshake).protocolMagic2931 == pFVar12->playerRuntimeId) {
        pwVar4 = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,pwVar4);
        FrontendRecentTextHistory_InsertAndRebuild5();
        if (extraout_ECX_00 + -1 != 0) {
          pFVar8 = pFVar12 + 1;
          for (iVar7 = (extraout_ECX_00 + -1) * 0x4ec; iVar7 != 0; iVar7 = iVar7 + -1) {
            pFVar12->runtimeState00 = pFVar8->runtimeState00;
            pFVar8 = (FrontendPlayerRuntimeRecord *)&pFVar8->peerSequenceToken;
            pFVar12 = (FrontendPlayerRuntimeRecord *)&pFVar12->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return CONCAT44(param_2,in_EAX);
      }
      pFVar12 = pFVar12 + 1;
      FVar6 = FVar6 - 1;
    } while (FVar6 != 0);
    return CONCAT44(param_2,in_EAX);
  }
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
         FRONTEND_PACKET_30005_PLAYER_SNAPSHOT) &&
       (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
       senderEndpoint->ipv4AddressNetworkOrder)) &&
     (((packet->packet10000Handshake).protocolMagic2931 < g_FrontendExpectedPlayerRuntimeBlockCount
      && ((packet->packet10000Handshake).protocolMagic2931 == g_FrontendPlayerRuntimeBlockCount))))
  {
    Random_SetBothSeeds((packet->packet30005PlayerSnapshot).secondaryRandomSeed);
    Random_SelectSecondaryStream();
    g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount + 1;
    pdVar9 = extraout_EDX_00;
    pFVar12 = g_FrontendPlayerRuntimeBlocks + extraout_ECX_01;
    for (iVar7 = 0x18; iVar7 != 0; iVar7 = iVar7 + -1) {
      pFVar12->runtimeState00 = *pdVar9;
      pdVar9 = pdVar9 + 1;
      pFVar12 = (FrontendPlayerRuntimeRecord *)&pFVar12->peerSequenceToken;
    }
    pwVar4 = TextResource_Resolve(0xff03);
    RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload_00,pwVar4);
    FrontendRecentTextHistory_InsertAndRebuild5();
    return CONCAT44(param_2,in_EAX);
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10009) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      senderEndpoint->ipv4AddressNetworkOrder)) {
    g_FrontendPacket8000ABuffer.snapshotChunkOffset =
         (packet->packet10009SnapshotChunkRequest).snapshotChunkOffset;
    pbVar13 = g_FrontendPacket8000ABuffer.g_FrontendPacket10009Buffer;
    puVar10 = (undefined4 *)
              (g_FrontendLocalPlayerPcxPreview + g_FrontendPacket8000ABuffer.snapshotChunkOffset);
    g_FrontendPacket8000ABuffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_8000A;
    iVar7 = 0x3a;
    if (g_FrontendPacket8000ABuffer.snapshotChunkOffset == 0x1220) {
      iVar7 = 0x38;
    }
    for (; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined4 *)pbVar13 = *puVar10;
      puVar10 = puVar10 + 1;
      pbVar13 = pbVar13 + 4;
    }
    if (dVar2 == g_FrontendPlayerRuntimeBlockCount) {
      UiTransfer_StagePacketAndSendCf
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket8000ABuffer.header);
    }
    return CONCAT44(param_2,in_EAX);
  }
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x00545640.
   Ownership: network/protocol/transfer.
   Purpose: Exact four-argument callback wrapper that marks the transfer mailbox unavailable only when frontend
   mode bit 0 is set.
   Local calls: UiTransferMailbox_MarkUnavailable.
*/
void FrontendTransfer_MarkUnavailableIfModeBit0Callback(dword arg0,dword arg1,dword arg2,dword arg3)

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
undefined4
FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady(int param_1)

{
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (param_1 == pFVar2->playerRuntimeId) {
      pFVar2->snapshotTransferFlags =
           pFVar2->snapshotTransferFlags | FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
      FVar1 = g_FrontendPlayerRuntimeBlockCount;
      pFVar2 = g_FrontendPlayerRuntimeBlocks;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) == SESSION_NETWORK_ROLE_LOCAL) {
        return in_EAX;
      }
      do {
        if ((pFVar2->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
          return in_EAX;
        }
        FVar1 = FVar1 - 1;
        pFVar2 = pFVar2 + 1;
      } while (FVar1 != 0);
      (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
      UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
      return in_EAX;
    }
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
  } while (FVar1 != 0);
  return in_EAX;
}

/* Address: 0x0054E230.
   Ownership: network/protocol/transfer.
   Purpose: Builds the fixed packet header with packed type 0x00010000 and payload value 0x2931, then submits it
   through the exact endpoint descriptor while preserving the backend CF result. Key sender: 0x10000 Handshake with
   magic 0x2931 (typed opcode census, exe_net_packets.md section 4b).
   Local calls: UiTransfer_StagePacketAndSendCf.
*/
undefined8 __cdecl UiTransfer_SendPacketType10000Value2931Cf(void)

{
  undefined8 packetSendResultPair;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic2931 = 0x2931;
  packetSendResultPair =
       UiTransfer_StagePacketAndSendCf
                 (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return packetSendResultPair;
}

/* Address: 0x0054E470.
   Ownership: network/protocol/transfer.
   Purpose: Builds packet 0x00020002 with a 0x40-byte payload, copies exactly ten dwords from the current player
   descriptor, derives status bits 0x0001 and 0x0100, and submits the packet while preserving the existing EDX and
   CF contracts. Key sender: 0x20002 PlayerDescriptor (10 dwords).
   Local calls: UiTransfer_StagePacketAndSendCf.
   Cross-module calls: PcxPreview_Load64x64PaletteAndPixelsCf [ui/support/runtime].
*/
undefined8 __cdecl UiTransfer_SendPlayerDescriptorPacket20002Cf(void)

{
  int iVar1;
  dword *pdVar2;
  dword *pdVar3;
  undefined1 in_CF;
  undefined8 uVar4;
  
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
  PcxPreview_Load64x64PaletteAndPixelsCf
            (g_FrontendLocalPlayerPcxPreview,(word *)&g_FrontendLocalPlayerNameUtf16);
  if (!(bool)in_CF) {
    *(ushort *)((int)pdVar3 + -2) = *(ushort *)((int)pdVar3 + -2) | 1;
  }
  *(ushort *)((int)pdVar3 + -2) = *(ushort *)((int)pdVar3 + -2) | 0x100;
  uVar4 = UiTransfer_StagePacketAndSendCf
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return uVar4;
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
void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
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
  FrontendPlayerRuntimeId extraout_ECX;
  int iVar8;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  void *replacementPayload;
  int iVar9;
  UiTransferPacketPackedType UVar10;
  UiTransferEndpointDescriptor *pUVar11;
  FrontendPlayerRuntimeRecord *pFVar12;
  dword *joiningPlayerRecordDwordCursor;
  FrontendCommandPacketRecord *pFVar13;
  FrontendCommandPacketRecord *pFVar14;
  undefined8 uVar15;
  
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10000_HANDSHAKE) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
    if ((((packet->packet10000Handshake).protocolMagic2931 == 0x2931) &&
        (((packet->packet10000Handshake).header.sequenceToken & 0xffff0000) == 0x12340000)) &&
       (*(uint *)(frontendRuntime + 0x5640) < *(uint *)(frontendRuntime + 0x5140))) {
      g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
    }
    pwVar4 = TextResource_Resolve(0x211a);
    uVar15 = RichTextCommandStream_PatchPayloadBySelector(0,(void *)0x50f07c,pwVar4);
    RichTextCommandStream_CopyExpandedCf
              (0x28,g_FrontendPacket50001Buffer.sessionTitleUtf16,(word *)uVar15);
    pwVar4 = TextResource_Resolve(0x211b);
    uVar15 = RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,pwVar4);
    uVar15 = RichTextCommandStream_PatchPayloadBySelector
                       (1,&g_FrontendLocalPlayerNameUtf16,(word *)uVar15);
    RichTextCommandStream_CopyExpandedCf
              (0x58,g_FrontendPacket50001Buffer.hostDescriptionUtf16,(word *)uVar15);
    pwVar4 = TextResource_Resolve(0x211c);
    uVar15 = RichTextCommandStream_PatchPayloadBySelector
                       (0,&g_FrontendNetworkRuntimeCountTextUtf16,pwVar4);
    uVar15 = RichTextCommandStream_PatchPayloadBySelector
                       (1,&g_FrontendNetworkPlayerCountTextUtf16,(word *)uVar15);
    RichTextCommandStream_CopyExpandedCf
              (8,g_FrontendPacket50001Buffer.playerCountTextUtf16,(word *)uVar15);
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
      pFVar13 = g_FrontendPlayerCommandRecords;
      iVar6 = g_FrontendPlayerRuntimeCount;
      pFVar12 = g_FrontendPlayerRuntimeBlocks;
      while (((packet->packet10000Handshake).header.sequenceToken != pFVar12->peerSequenceToken ||
             (senderEndpoint->ipv4AddressNetworkOrder != (pFVar12->endpoint).ipv4AddressNetworkOrder
             ))) {
        pFVar12 = pFVar12 + 1;
        pFVar13 = pFVar13 + 1;
        iVar6 = iVar6 + -1;
        if (iVar6 == 0) {
          return;
        }
      }
      pFVar12->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
        (pFVar13->header).packedTypeAndUnitCount =
             (packet->packet10000Handshake).header.packedTypeAndUnitCount;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        pFVar13 = (FrontendCommandPacketRecord *)&(pFVar13->header).sequenceToken;
      }
      FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
      iVar9 = 0;
      pFVar13 = g_FrontendPlayerCommandRecords;
      pFVar14 = g_FrontendCommandBatchPacketBuffer;
      iVar6 = g_FrontendPlayerRuntimeCount;
      do {
        if (((pFVar13->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
          pFVar13 = pFVar13 + 1;
        }
        else {
          for (iVar8 = 8; iVar8 != 0; iVar8 = iVar8 + -1) {
            (pFVar14->header).packedTypeAndUnitCount = (pFVar13->header).packedTypeAndUnitCount;
            pFVar13 = (FrontendCommandPacketRecord *)&(pFVar13->header).sequenceToken;
            pFVar14 = (FrontendCommandPacketRecord *)&(pFVar14->header).sequenceToken;
          }
          iVar9 = iVar9 + 1;
          pFVar13[-1].command.packedCommandAndPlayerId =
               pFVar13[-1].command.packedCommandAndPlayerId & 0xff;
        }
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
      if (iVar9 << 0x10 != 0) {
        UVar10 = iVar9 << 0x10 | 0x10;
        pUVar11 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = UVar10;
        iVar6 = g_FrontendPlayerRuntimeCount;
        while (iVar6 != 1) {
          uVar15 = UiTransfer_StagePacketAndSendCf
                             (pUVar11,&g_FrontendCommandBatchPacketBuffer[0].header);
          UVar10 = (UiTransferPacketPackedType)((ulonglong)uVar15 >> 0x20);
          pUVar11 = pUVar11 + 0x13b;
          iVar6 = extraout_ECX_00;
        }
        pFVar13 = g_FrontendCommandBatchPacketBuffer;
        uVar7 = UVar10 >> 0x10;
        do {
          uVar3 = (pFVar13->command).packedCommandAndPlayerId;
          uVar5 = uVar3 >> 8;
          if (uVar5 != 0) {
            if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar5 < &g_FrontendRootNode) {
              (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar5))
                        (uVar3 & 0xff,(pFVar13->command).payloadDword0C,
                         (pFVar13->command).payloadDword08,(pFVar13->command).payloadDword04);
              uVar7 = extraout_ECX_01;
            }
          }
          pFVar13 = pFVar13 + 1;
          uVar7 = uVar7 - 1;
        } while (uVar7 != 0);
      }
      return;
    }
    iVar6 = *(int *)(frontendRuntime + 0x5640);
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
    while (((packet->packet10000Handshake).header.sequenceToken != pFVar12->peerSequenceToken ||
           (senderEndpoint->ipv4AddressNetworkOrder != (pFVar12->endpoint).ipv4AddressNetworkOrder))
          ) {
      pFVar12 = pFVar12 + 1;
      iVar6 = iVar6 + -1;
      if (iVar6 == 0) {
        return;
      }
    }
    uVar7 = (packet->packet10000Handshake).protocolMagic2931;
    UVar2 = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
    pFVar12->capabilityFlags = uVar7;
    pFVar12->heartbeatExpiryTicks = UVar2;
    pFVar12->reserved78_7F[0] = 0;
    pFVar12->reserved78_7F[1] = 0;
    pFVar12->reserved78_7F[2] = 0;
    pFVar12->reserved78_7F[3] = 0;
    if ((uVar7 & 0x100) != 0) {
      pFVar12->reserved78_7F[0] = 0x43;
      pFVar12->reserved78_7F[1] = 0;
      pFVar12->reserved78_7F[2] = 0x44;
      pFVar12->reserved78_7F[3] = 0;
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
  pUVar11 = senderEndpoint;
  for (iVar6 = 4; iVar6 != 0; iVar6 = iVar6 + -1) {
    *joiningPlayerRecordDwordCursor = (dword)pUVar11->addressHeader;
    pUVar11 = (UiTransferEndpointDescriptor *)&pUVar11->ipv4AddressNetworkOrder;
    joiningPlayerRecordDwordCursor = joiningPlayerRecordDwordCursor + 1;
  }
  uVar7 = 0;
  iVar6 = g_FrontendPlayerRuntimeCount;
  pFVar12 = g_FrontendPlayerRuntimeBlocks;
  do {
    while (uVar7 != pFVar12->playerRuntimeId) {
      iVar6 = iVar6 + -1;
      pFVar12 = pFVar12 + 1;
      if (iVar6 == 0)
      goto 
      FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets_InitializeJoiningPlayerRecordWithNextAvailableId
      ;
    }
    uVar7 = uVar7 + 1;
    iVar6 = g_FrontendPlayerRuntimeCount;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
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
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = extraout_ECX;
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
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands
               (FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int extraout_ECX;
  int iVar4;
  int extraout_ECX_00;
  uint uVar5;
  uint extraout_ECX_01;
  int iVar6;
  UiTransferPacketPackedType UVar7;
  UiTransferEndpointDescriptor *pUVar8;
  dword *pdVar9;
  UiTransferEndpointDescriptor *endpoint;
  FrontendCommandPacketRecord *pFVar10;
  dword *pdVar11;
  FrontendCommandPacketRecord *pFVar12;
  dword dVar14;
  undefined8 uVar13;
  
  dVar14 = g_FrontendHostPublishRoundRobinCounter;
  uVar5 = *(uint *)(frontendRuntime + 0x5640);
  pUVar8 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  if (1 < (int)uVar5) {
    g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008;
    g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
    g_FrontendHostPublishRoundRobinCounter = g_FrontendHostPublishRoundRobinCounter + 1;
    g_FrontendPacket40008Buffer.selectedPlayerIndex = dVar14 % uVar5;
    g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
         pUVar8[g_FrontendPacket40008Buffer.selectedPlayerIndex * 0x13b + -0x13e].
         ipv4AddressNetworkOrder;
    g_FrontendPacket40008Buffer.selectedStatusCode0 =
         *(FrontendStatusCode *)
          pUVar8[g_FrontendPacket40008Buffer.selectedPlayerIndex * 0x13b + -0x138].zeroPadding;
    g_FrontendPacket40008Buffer.selectedStatusCode1 =
         *(FrontendStatusCode *)
          (pUVar8[g_FrontendPacket40008Buffer.selectedPlayerIndex * 0x13b + -0x138].zeroPadding + 4)
    ;
    g_FrontendPacket40008Buffer.playerCount = uVar5;
    endpoint = pUVar8;
    _dVar14 = (*g_WideNumberFormatUtf16)
                        (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                         pUVar8[g_FrontendPacket40008Buffer.selectedPlayerIndex * 0x13b + -0x136].
                         addressHeader.packedFamilyAndPort << 2,(word *)0x54d9c0);
    *(undefined4 *)((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + (dword)_dVar14)
         = 0x73006d;
    *(undefined2 *)
     ((int)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + (dword)_dVar14 + 4) = 0;
    pdVar9 = (dword *)((int)((ulonglong)_dVar14 >> 0x20) + -0x13d8 + (int)pUVar8);
    pdVar11 = g_FrontendPacket40008Buffer.playerDescriptorPayload;
    for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *pdVar11 = *pdVar9;
      pdVar9 = pdVar9 + 1;
      pdVar11 = pdVar11 + 1;
    }
    g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032;
    g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
    do {
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket40008Buffer.header);
      UiTransfer_StagePacketAndSendCf(endpoint,&g_FrontendPacket10032Buffer.header);
      endpoint = endpoint + 0x13b;
    } while (extraout_ECX != 1);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = 4;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  iVar6 = 0;
  pFVar10 = g_FrontendPlayerCommandRecords;
  pFVar12 = g_FrontendCommandBatchPacketBuffer;
  iVar3 = g_FrontendPlayerRuntimeCount;
  do {
    if (((pFVar10->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      pFVar10 = pFVar10 + 1;
    }
    else {
      for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
        (pFVar12->header).packedTypeAndUnitCount = (pFVar10->header).packedTypeAndUnitCount;
        pFVar10 = (FrontendCommandPacketRecord *)&(pFVar10->header).sequenceToken;
        pFVar12 = (FrontendCommandPacketRecord *)&(pFVar12->header).sequenceToken;
      }
      iVar6 = iVar6 + 1;
      pFVar10[-1].command.packedCommandAndPlayerId =
           pFVar10[-1].command.packedCommandAndPlayerId & 0xff;
    }
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  if (iVar6 << 0x10 != 0) {
    UVar7 = iVar6 << 0x10 | 0x10;
    pUVar8 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
    g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = UVar7;
    iVar3 = g_FrontendPlayerRuntimeCount;
    while (iVar3 != 1) {
      uVar13 = UiTransfer_StagePacketAndSendCf(pUVar8,&g_FrontendCommandBatchPacketBuffer[0].header)
      ;
      UVar7 = (UiTransferPacketPackedType)((ulonglong)uVar13 >> 0x20);
      pUVar8 = pUVar8 + 0x13b;
      iVar3 = extraout_ECX_00;
    }
    pFVar10 = g_FrontendCommandBatchPacketBuffer;
    uVar5 = UVar7 >> 0x10;
    do {
      uVar1 = (pFVar10->command).packedCommandAndPlayerId;
      uVar2 = uVar1 >> 8;
      if (uVar2 != 0) {
        if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2 < &g_FrontendRootNode) {
          (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2))
                    (uVar1 & 0xff,(pFVar10->command).payloadDword0C,
                     (pFVar10->command).payloadDword08,(pFVar10->command).payloadDword04);
          uVar5 = extraout_ECX_01;
        }
      }
      pFVar10 = pFVar10 + 1;
      uVar5 = uVar5 - 1;
    } while (uVar5 != 0);
  }
  return;
}

/* Address: 0x0054EEF0.
   Ownership: network/protocol/transfer.
   Purpose: Stages and sends packet 0x00010006 with the fixed 0x100 and 0x40 payload fields to the current frontend
   endpoint.
   Local calls: UiTransfer_StagePacketAndSendCf.
*/
void __fastcall FrontendTransfer_SendPacket10006(undefined4 param_1,undefined4 param_2)

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
undefined8
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 sendStateReplies)

{
  undefined4 in_EAX;
  int iVar1;
  int iVar2;
  FrontendPlayerRuntimeBlockCount extraout_ECX;
  FrontendPlayerRuntimeBlockCount extraout_ECX_00;
  FrontendPlayerRuntimeBlockCount extraout_ECX_01;
  FrontendPlayerRuntimeBlockCount FVar3;
  undefined4 in_EDX;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendCommandPacketRecord *pFVar5;
  UiTransferEndpointDescriptor *pUVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  FrontendCommandPacketRecord *pFVar8;
  
  iVar1 = g_FrontendPlayerRuntimeBlockCount - 1;
  pFVar7 = g_FrontendPlayerRuntimeBlocks;
  if (iVar1 != 0) {
    do {
      if (pFVar7[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        if (sendStateReplies != 0) {
          pUVar6 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
          FVar3 = g_FrontendPlayerRuntimeBlockCount;
          while (FVar3 != 1) {
            if (pUVar6[1].addressHeader.packedFamilyAndPort == 0) {
              UiTransfer_StagePacketAndSendCf
                        (pUVar6,&g_FrontendClientCommandBatchPacketBuffer[0].header);
              FVar3 = extraout_ECX_00;
            }
            else {
              g_FrontendPacket10022Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10022;
              UiTransfer_StagePacketAndSendCf(pUVar6,&g_FrontendPacket10022Buffer.header);
              FVar3 = extraout_ECX_01;
            }
            pUVar6 = pUVar6 + 0x13b;
          }
        }
        return CONCAT44(in_EDX,in_EAX);
      }
      iVar1 = iVar1 + -1;
      pFVar7 = pFVar7 + 1;
    } while (iVar1 != 0);
    iVar1 = g_FrontendPlayerRuntimeBlockCount - 1;
    pFVar7 = g_FrontendPlayerRuntimeBlocks;
    do {
      pFVar7[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      iVar1 = iVar1 + -1;
      pFVar7 = pFVar7 + 1;
    } while (iVar1 != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  InGameCommandQueue_DequeueFirstIntoRecord(g_FrontendClientPlayerCommandRecords);
  iVar1 = 0;
  pFVar5 = g_FrontendClientPlayerCommandRecords;
  pFVar8 = g_FrontendClientCommandBatchPacketBuffer;
  FVar3 = g_FrontendPlayerRuntimeBlockCount;
  do {
    FVar4 = FVar3;
    if (((pFVar5->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      pFVar5 = pFVar5 + 1;
      goto 
      FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip;
    }
    while( true ) {
      for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
        (pFVar8->header).packedTypeAndUnitCount = (pFVar5->header).packedTypeAndUnitCount;
        pFVar5 = (FrontendCommandPacketRecord *)&(pFVar5->header).sequenceToken;
        pFVar8 = (FrontendCommandPacketRecord *)&(pFVar8->header).sequenceToken;
      }
      iVar1 = iVar1 + 1;
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState_AdvanceAfterCommandRecordCopyOrSkip:
      FVar3 = FVar4 - 1;
      if (FVar4 - 1 != 0) break;
      if (iVar1 << 0x10 != 0) {
        g_FrontendClientCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
             iVar1 << 0x10 | 0x20;
        pUVar6 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        FVar3 = g_FrontendPlayerRuntimeBlockCount;
        while (FVar3 != 1) {
          UiTransfer_StagePacketAndSendCf
                    (pUVar6,&g_FrontendClientCommandBatchPacketBuffer[0].header);
          pUVar6 = pUVar6 + 0x13b;
          FVar3 = extraout_ECX;
        }
        return CONCAT44(in_EDX,in_EAX);
      }
      pFVar5 = g_FrontendClientPlayerCommandRecords;
      iVar1 = 0;
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
void __cdecl FrontendTransfer_SendCommandBatchRequest10021(void)

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
void __cdecl UiTransferMailbox_ClearReceivedState(void)

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
void * __cdecl UiTransferMailbox_GetReceivedBufferCf(void)

{
  void *in_EAX;
  
  if (((g_UiTransferMailbox.receivedAllocation != (void *)0xffffffff) &&
      (g_UiTransferMailbox.receivedAllocation != (void *)0x0)) &&
     (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
    return g_UiTransferMailbox.receivedAllocation;
  }
  return in_EAX;
}

/* Address: 0x004AF1C0.
   Ownership: network/protocol/transfer.
   Purpose: XORs the low sixteen bits of Random_NextPrimary into the transfer sequence token while preserving the
   token's high word.
   Cross-module calls: Random_NextPrimary [core/math/random].
*/
void __cdecl UiTransferMailbox_RandomizeSequenceToken(void)

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
void FrontendTransfer_HandleSessionListAndJoinAckPackets
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
void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRuntime)

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
undefined4 FrontendTransfer_ConsumeProcessedFlagFrontendCf(void)

{
  undefined4 in_EAX;
  
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return in_EAX;
}

/* Address: 0x005722C0.
   Ownership: network/protocol/transfer.
   Purpose: Matches packet types 0x10021 and 0x10023 to a frontend player by sender and endpoint identity, marks
   the player ready, and copies changed eight-dword request state into the per-player synchronization slot.
*/
void FrontendTransfer_HandleSyncRequest10021AndReply10023
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
undefined8 __cdecl FrontendTransfer_DispatchStagedCommandRecords(void)

{
  uint uVar1;
  undefined4 in_EAX;
  uint uVar2;
  uint uVar3;
  uint extraout_ECX;
  undefined4 in_EDX;
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
        uVar3 = extraout_ECX;
      }
    }
    pFVar4 = pFVar4 + 1;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00572AA0.
   Ownership: network/protocol/transfer.
   Purpose: Atomically exchanges the processed flag at 0x0050F0A8 with zero. CF is set when the consumed value was
   zero and clear when work had been marked processed; EAX is restored.
*/
undefined4 __cdecl FrontendTransfer_ConsumeProcessedFlagCf(void)

{
  undefined4 in_EAX;
  
  LOCK();
  g_FrontendTransferResponsePending = 0;
  UNLOCK();
  return in_EAX;
}

/* Address: 0x00407160.
   Ownership: network/protocol/transfer.
   Purpose: Transforms 8-byte packet blocks through sixteen table-driven rounds. The archived RET 0x10 proves four
   stack arguments; no register arguments are part of the ABI. Symmetric block transform (involution): the same
   routine encodes and decodes — no separate inverse exists.
*/
void UiTransfer_TransformPacketBlocks
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
void UiTransferBlock_Transform64BitBlocksWithRoundKeys16
               (dword *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,
               void *source)

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
void __cdecl UiTransferMailbox_MarkUnavailable(void)

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
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation)

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
void __fastcall
FrontendTransfer_SendQueued10011AndOptional10004(undefined4 param_1,undefined4 param_2)

{
  uint extraout_ECX;
  
  g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011;
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
  UiTransfer_StagePacketAndSendCf
            (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
  if (extraout_ECX < g_FrontendExpectedPlayerRuntimeBlockCount) {
    g_FrontendPacket10004Buffer.header.packedTypeAndUnitCount =
         FRONTEND_PACKET_10004_SNAPSHOT_REQUEST;
    g_FrontendPacket10004Buffer.requestedPlayerIndex = extraout_ECX;
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
undefined8
UiTransfer_StagePacketAndSendCf
          (UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet)

{
  uint uVar1;
  bool bVar2;
  byte *pbVar3;
  dword dVar4;
  dword dVar5;
  undefined4 in_EAX;
  UiTransferXorChecksum UVar6;
  uint uVar6;
  UiTransferPayloadByteCount byteCount;
  UiTransferPayloadByteCount UVar7;
  UiTransferPayloadByteCount UVar8;
  undefined4 in_EDX;
  int iVar9;
  int iVar10;
  dword *outputBlocks;
  UiTransferPacketHeader *pUVar11;
  dword *endpointDestinationDwordCursor;
  
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
  (*g_NetworkBackendSlot5)((WinSockAddress *)(pbVar3 + iVar9 + -8),byteCount,(byte *)outputBlocks);
  return CONCAT44(in_EDX,in_EAX);
}
