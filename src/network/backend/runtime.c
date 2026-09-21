/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/runtime.h>

/* Implementation ownership: network/backend/runtime. */

/* Address: 0x0054EF60.
   Ownership: network/backend/runtime.
   Purpose: Recv: 0x10000 Handshake (magic check 0x2931), 0x20002 PlayerDescriptor, roster/state sync
   (exe_net_lobby_session.md).
   Cross-module calls: Random_GetSecondarySeed [core/math/random], UiTransfer_StagePacketAndSendCf
   [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          dword unusedDispatchArg)

{
  NetworkIpv4AddressNetworkOrder NVar1;
  UiTransferSenderContext UVar2;
  UiTransferJoinAvailability UVar3;
  uint uVar4;
  FrontendPlayerRuntimeBlockCount FVar5;
  int iVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  dword *pdVar8;
  FrontendCommandPacketRecord *pFVar9;
  FrontendPacket30005PlayerSnapshot *pFVar10;
  byte *pbVar11;
  
  uVar4 = (packet->packet10000Handshake).header.sequenceToken;
  NVar1 = senderEndpoint->ipv4AddressNetworkOrder;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10011) {
    pFVar9 = g_FrontendPlayerCommandRecords;
    FVar5 = g_FrontendPlayerRuntimeBlockCount;
    pFVar7 = g_FrontendPlayerRuntimeBlocks;
    while ((uVar4 != pFVar7->peerSequenceToken ||
           (NVar1 != (pFVar7->endpoint).ipv4AddressNetworkOrder))) {
      pFVar7 = pFVar7 + 1;
      pFVar9 = pFVar9 + 1;
      FVar5 = FVar5 - 1;
      if (FVar5 == 0) {
        return;
      }
    }
    UVar2 = (packet->packet10000Handshake).header.senderContext;
    pFVar7->heartbeatExpiryTicks = 0x100;
    if (UVar2 != (pFVar9->header).senderContext) {
      pFVar7->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
      for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
        (pFVar9->header).packedTypeAndUnitCount =
             (packet->packet10000Handshake).header.packedTypeAndUnitCount;
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        pFVar9 = (FrontendCommandPacketRecord *)&(pFVar9->header).sequenceToken;
      }
    }
    return;
  }
  FVar5 = g_FrontendPlayerRuntimeBlockCount;
  pFVar7 = g_FrontendPlayerRuntimeBlocks;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10013) {
    while ((uVar4 != pFVar7->peerSequenceToken ||
           (NVar1 != (pFVar7->endpoint).ipv4AddressNetworkOrder))) {
      FVar5 = FVar5 - 1;
      pFVar7 = pFVar7 + 1;
      if (FVar5 == 0) {
        return;
      }
    }
    pFVar7->heartbeatExpiryTicks = 0x100;
    return;
  }
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10004_SNAPSHOT_REQUEST) {
    while ((uVar4 != pFVar7->peerSequenceToken ||
           (NVar1 != (pFVar7->endpoint).ipv4AddressNetworkOrder))) {
      FVar5 = FVar5 - 1;
      pFVar7 = pFVar7 + 1;
      if (FVar5 == 0) {
        return;
      }
    }
    uVar4 = (packet->packet10004PlayerSnapshotRequest).requestedPlayerIndex;
    if (uVar4 < g_FrontendPlayerRuntimeBlockCount) {
      pFVar7 = g_FrontendPlayerRuntimeBlocks + uVar4;
      pFVar10 = &g_FrontendPacket30005Buffer;
      for (iVar6 = 0x18; iVar6 != 0; iVar6 = iVar6 + -1) {
        (pFVar10->header).packedTypeAndUnitCount = pFVar7->runtimeState00;
        pFVar7 = (FrontendPlayerRuntimeRecord *)&pFVar7->peerSequenceToken;
        pFVar10 = (FrontendPacket30005PlayerSnapshot *)&(pFVar10->header).sequenceToken;
      }
      g_FrontendPacket30005Buffer.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_30005_PLAYER_SNAPSHOT;
      g_FrontendPacket30005Buffer.playerIndex = uVar4;
      g_FrontendPacket30005Buffer.secondaryRandomSeed = Random_GetSecondarySeed();
      UiTransfer_StagePacketAndSendCf(senderEndpoint,&g_FrontendPacket30005Buffer.header);
      return;
    }
  }
  FVar5 = g_FrontendPlayerRuntimeBlockCount;
  pFVar7 = g_FrontendPlayerRuntimeBlocks;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_8000A) {
    return;
  }
  while ((uVar4 != pFVar7->peerSequenceToken ||
         (NVar1 != (pFVar7->endpoint).ipv4AddressNetworkOrder))) {
    FVar5 = FVar5 - 1;
    pFVar7 = pFVar7 + 1;
    if (FVar5 == 0) {
      return;
    }
  }
  UVar3 = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
  if ((((pFVar7->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
      ((pFVar7->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) &&
     (UVar3 == pFVar7->snapshotChunkOffset)) {
    pdVar8 = &(packet->command10011Or10021).command.payloadDword08;
    pbVar11 = pFVar7->snapshotPayloadB0_13AF + UVar3;
    iVar6 = 0x3a;
    if (UVar3 == 0x1220) {
      iVar6 = 0x38;
      pFVar7->snapshotTransferFlags =
           pFVar7->snapshotTransferFlags | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
      pFVar7->snapshotChunkOffset = pFVar7->snapshotChunkOffset + 0xe0;
    }
    for (; iVar6 != 0; iVar6 = iVar6 + -1) {
      *(dword *)pbVar11 = *pdVar8;
      pdVar8 = pdVar8 + 1;
      pbVar11 = pbVar11 + 4;
    }
    if (UVar3 != 0x1220) {
      g_FrontendPacket10009Buffer.snapshotChunkOffset = UVar3 + 0xe8;
      pFVar7->snapshotChunkOffset = pFVar7->snapshotChunkOffset + 0xe8;
      g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009;
      UiTransfer_StagePacketAndSendCf(senderEndpoint,&g_FrontendPacket10009Buffer.header);
      g_FrontendHostSnapshotTransferCountdown = 4;
    }
  }
  return;
}


/* Address: 0x0054F240.
   Ownership: network/backend/runtime.
   Purpose: Handles frontend network host tick command and snapshot transfer.
   Cross-module calls: UiTransfer_StagePacketAndSendCf [network/protocol/transfer],
   FrontendCommandQueue_DequeueFirstIntoRecord [network/protocol/commands], PckCodec_EncodeHuffmanRle
   [assets/package/codec], UiTransferMailbox_SetOutgoingBuffer [network/protocol/transfer],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetwork_HostTickCommandAndSnapshotTransfer(dword callbackArg)

{
  FrontendSnapshotTransferFlags FVar1;
  uint uVar2;
  int iVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  uint uVar5;
  uint uVar6;
  FrontendPlayerRuntimeBlockCount FVar7;
  FrontendSnapshotTransferFlags FVar8;
  FrontendSnapshotTransferFlags sourceSizeBytes;
  FrontendCommandPacketRecord *pFVar9;
  UiTransferEndpointDescriptor *pUVar10;
  FrontendPlayerRuntimeRecord *pFVar11;
  FrontendPlayerRuntimeRecord *pFVar12;
  byte *pbVar13;
  FrontendSnapshotTransferFlags *pFVar14;
  FrontendCommandPacketRecord *pFVar15;
  FrontendSnapshotTransferFlags *pFVar16;
  PckCodecEaxCf5 PVar17;
  ArenaAllocEaxCf5 AVar18;
  
  iVar3 = g_FrontendPlayerRuntimeBlockCount - 1;
  pFVar12 = g_FrontendPlayerRuntimeBlocks;
  if (iVar3 != 0) {
    do {
      if (pFVar12[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        pUVar10 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        FVar7 = g_FrontendPlayerRuntimeBlockCount;
        while (FVar7 = FVar7 - 1, FVar7 != 0) {
          if (pUVar10[1].addressHeader.packedFamilyAndPort == 0) {
            UiTransfer_StagePacketAndSendCf(pUVar10,&g_FrontendCommandBatchPacketBuffer[0].header);
          }
          else {
            g_FrontendPacket10012Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10012;
            UiTransfer_StagePacketAndSendCf(pUVar10,&g_FrontendPacket10012Buffer.header);
          }
          pUVar10 = pUVar10 + 0x13b;
        }
        return true;
      }
      iVar3 = iVar3 + -1;
      pFVar12 = pFVar12 + 1;
    } while (iVar3 != 0);
    iVar3 = g_FrontendPlayerRuntimeBlockCount - 1;
    pFVar12 = g_FrontendPlayerRuntimeBlocks;
    do {
      pFVar12[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      iVar3 = iVar3 + -1;
      pFVar12 = pFVar12 + 1;
    } while (iVar3 != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  uVar6 = 0;
  pFVar9 = g_FrontendPlayerCommandRecords;
  pFVar15 = g_FrontendCommandBatchPacketBuffer;
  FVar7 = g_FrontendPlayerRuntimeBlockCount;
  do {
    FVar4 = FVar7;
    if (((pFVar9->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      pFVar9 = pFVar9 + 1;
      goto FrontendNetwork_HostTick_AdvanceCommandScanOrRestartUntilBatchNonempty;
    }
    while( true ) {
      for (iVar3 = 8; iVar3 != 0; iVar3 = iVar3 + -1) {
        (pFVar15->header).packedTypeAndUnitCount = (pFVar9->header).packedTypeAndUnitCount;
        pFVar9 = (FrontendCommandPacketRecord *)&(pFVar9->header).sequenceToken;
        pFVar15 = (FrontendCommandPacketRecord *)&(pFVar15->header).sequenceToken;
      }
      uVar6 = uVar6 + 1;
FrontendNetwork_HostTick_AdvanceCommandScanOrRestartUntilBatchNonempty:
      FVar7 = FVar4 - 1;
      if (FVar4 - 1 != 0) break;
      if (uVar6 << 0x10 != 0) {
        g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = uVar6 << 0x10 | 0x10;
        pUVar10 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        FVar7 = g_FrontendPlayerRuntimeBlockCount;
        while (FVar7 = FVar7 - 1, FVar7 != 0) {
          UiTransfer_StagePacketAndSendCf(pUVar10,&g_FrontendCommandBatchPacketBuffer[0].header);
          pUVar10 = pUVar10 + 0x13b;
        }
        pFVar9 = g_FrontendCommandBatchPacketBuffer;
        uVar6 = uVar6 & 0xffff;
        do {
          uVar5 = (pFVar9->command).packedCommandAndPlayerId;
          uVar2 = uVar5 >> 8;
          if (uVar2 != 0) {
            if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2 < &g_FrontendRootNode) {
              (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar2))
                        (uVar5 & 0xff,(pFVar9->command).payloadDword0C,
                         (pFVar9->command).payloadDword08,(pFVar9->command).payloadDword04);
            }
          }
          FVar7 = g_FrontendPlayerRuntimeBlockCount;
          pFVar12 = g_FrontendPlayerRuntimeBlocks;
          pFVar9 = pFVar9 + 1;
          uVar6 = uVar6 - 1;
        } while (uVar6 != 0);
        if ((g_FrontendHostSnapshotTransferCountdown != 0) &&
           (g_FrontendHostSnapshotTransferCountdown = g_FrontendHostSnapshotTransferCountdown + -1,
           FVar4 = g_FrontendPlayerRuntimeBlockCount, pFVar11 = g_FrontendPlayerRuntimeBlocks,
           g_FrontendHostSnapshotTransferCountdown == 0)) {
          do {
            if (((pFVar11->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
               ((pFVar11->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) {
              g_FrontendPacket10009Buffer.snapshotChunkOffset = pFVar11->snapshotChunkOffset;
              g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009;
              UiTransfer_StagePacketAndSendCf
                        (&pFVar11->endpoint,&g_FrontendPacket10009Buffer.header);
              g_FrontendHostSnapshotTransferCountdown = 4;
              return false;
            }
            FVar4 = FVar4 - 1;
            pFVar11 = pFVar11 + 1;
          } while (FVar4 != 0);
          g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags =
               g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags |
               FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
          FVar8 = 0;
          pFVar14 = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
          if (1 < FVar7) {
            do {
              FVar1 = pFVar12->snapshotTransferFlags;
              *pFVar14 = FVar1;
              sourceSizeBytes = FVar8 + FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
              pFVar14 = pFVar14 + 1;
              if ((FVar1 & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
                pbVar13 = pFVar12->snapshotPayloadB0_13AF;
                for (iVar3 = 0x4c0; iVar3 != 0; iVar3 = iVar3 + -1) {
                  *pFVar14 = *(FrontendSnapshotTransferFlags *)pbVar13;
                  pbVar13 = pbVar13 + 4;
                  pFVar14 = pFVar14 + 1;
                }
                sourceSizeBytes = FVar8 + 0x1304;
              }
              pFVar12 = pFVar12 + 1;
              FVar7 = FVar7 - 1;
              FVar8 = sourceSizeBytes;
            } while (FVar7 != 0);
            PVar17 = PckCodec_EncodeHuffmanRle
                               (0x7ffffc - sourceSizeBytes,(byte *)(pFVar14 + 1),sourceSizeBytes,
                                g_PackageScratchBuffer);
            if (!PVar17.carry) {
              *pFVar14 = sourceSizeBytes;
              uVar6 = PVar17.eax + 4;
              AVar18 = (*g_MemoryApi.alloc)(uVar6);
              if (!AVar18.carry) {
                pFVar16 = (FrontendSnapshotTransferFlags *)AVar18.eax;
                for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
                  *pFVar16 = *pFVar14;
                  pFVar14 = pFVar14 + 1;
                  pFVar16 = pFVar16 + 1;
                }
                UiTransferMailbox_SetOutgoingBuffer
                          (uVar6,(FrontendSnapshotTransferFlags *)AVar18.eax);
                FrontendCommandQueue_EnqueueLocalPlayerCommand(0x16f0,0,0,0);
              }
            }
          }
        }
        return false;
      }
      pFVar9 = g_FrontendPlayerCommandRecords;
      uVar6 = 0;
    }
  } while( true );
}


/* Address: 0x0054FA10.
   Ownership: network/backend/runtime.
   Purpose: Ticks the frontend disconnect timeout and, when it expires, clears mailbox/network state, resets page
   and ROM state, and collapses the player/session runtime to its local baseline.
   Cross-module calls: UiTransferMailbox_ClearReceivedState [network/protocol/transfer], UiPageStack_SetActiveIndex
   [ui/controls/layout], FrontendRomTransition_ActivateRecordByIdCf [assets/rom/runtime], TextResource_Resolve
   [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext],
   FrontendRecentTextHistory_InsertAndRebuild5 [ui/frontend/runtime].
*/
void __thandor_void_preserve_eax_ecx FrontendNetwork_TickDisconnectTimeoutAndResetSession(void)

{
  uint *puVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  int iVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  TextResourceResolveEaxCf5 TVar6;
  
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    UiTransferMailbox_ClearReceivedState();
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    g_FrontendNetworkState = 0;
    (*g_NetworkBackendSlot3)();
    (*g_NetworkBackendSlot1)();
    iVar3 = g_FrontendRootNode;
    pFVar5 = g_FrontendPlayerRuntimeBlocks;
    if ((g_FrontendRuntimeFlags & 0x10) != 0) {
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)(g_FrontendRootNode + 0x508));
      puVar1 = (uint *)(iVar3 + 0x3b4);
      *puVar1 = *puVar1 & 0xffffdfff;
      g_FrontendPendingPageAction = 0;
      g_FrontendRomTransitionContextValue = 0;
      FrontendRomTransition_ActivateRecordByIdCf(1,(WorldRuntimeContext *)(iVar3 + 0x368));
    }
    TVar6 = TextResource_Resolve(0xff01);
    RichTextCommandStream_PatchPayloadBySelector(0,&pFVar5->playerName,TVar6.eax);
    FrontendRecentTextHistory_InsertAndRebuild5(TVar6.eax);
    pFVar2 = g_FrontendPlayerRuntimeBlocks;
    for (FVar4 = g_FrontendPlayerRuntimeBlockCount; FVar4 != 0; FVar4 = FVar4 - 1) {
      if ((pFVar5->factionAssignment).readyOrWaitState == 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0xd0,0,0,0);
        }
        pFVar5 = g_FrontendPlayerRuntimeBlocks;
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        (pFVar5->playerName).textUtf16[0] = 0;
        (pFVar5->playerName).textUtf16[1] = 0;
        pFVar5->playerRuntimeId = 0;
        (pFVar5->factionAssignment).roleStateFlags = 0;
        pFVar5->runtimeState64 = 0;
        pFVar5->snapshotTransferFlags = 0;
        return;
      }
      pFVar5 = pFVar5 + 1;
    }
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    pFVar5 = g_FrontendPlayerRuntimeBlocks;
    (pFVar5->playerName).textUtf16[0] = 0;
    (pFVar5->playerName).textUtf16[1] = 0;
    pFVar2->playerRuntimeId = 0;
    (pFVar2->factionAssignment).roleStateFlags = 0;
    pFVar2->runtimeState64 = 0;
    pFVar2->snapshotTransferFlags = 0;
  }
  return;
}


/* Address: 0x00572710.
   Ownership: network/backend/runtime.
   Purpose: Handles validated frontend packet types 0x20, 0x10022, and 0x10007. It dispatches bounded 0x20-byte
   command records, emits reply 0x10023, or removes a timed-out 0x13B0-byte player block after localized message
   0xFF00. CF is set only after processing a new command batch. In-game recv: (n<<16)|0x20 batches + per-peer
   timeout tracking (host-leave Zeitueberschreitung path, exe_net_host_timeout.md).
   Cross-module calls: FrontendTransfer_SendCommandBatchRequest10021 [network/protocol/transfer],
   UiTransfer_StagePacketAndSendCf [network/protocol/transfer], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8
   [ui/ingame/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSenderContext UVar1;
  uint uVar2;
  uint uVar3;
  FrontendPlayerRuntimeBlockCount FVar4;
  int iVar5;
  FrontendPlayerRuntimeRecord *pFVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  TextResourceResolveEaxCf5 TVar8;
  
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x20) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    UVar1 = (packet->packet10000Handshake).header.senderContext;
    g_SessionTransferTimeoutTicks = 0x100;
    if (UVar1 != g_FrontendSelectedPlayerToken) {
      uVar3 = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
      g_FrontendSelectedPlayerToken = UVar1;
      do {
        uVar2 = (packet->packet10000Handshake).protocolMagic2931 >> 8;
        if (uVar2 != 0) {
          if (InGameCommandQueue_AppendLocalPlayerCommand + uVar2 <
              &InGameCommandHandlerCodeRegionEnd) {
            (*(InGameCommandQueue_AppendLocalPlayerCommand + uVar2))
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
      FrontendTransfer_SendCommandBatchRequest10021();
      g_FrontendTransferResponsePending = 1;
      return true;
    }
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10022) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = 0x100;
    g_FrontendPacket10023Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10023;
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10023Buffer.header);
    return false;
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
        FRONTEND_PACKET_10007_PLAYER_REMOVAL) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    FVar4 = g_FrontendPlayerRuntimeBlockCount;
    pFVar7 = g_FrontendPlayerRuntimeBlocks;
    do {
      if ((packet->packet10000Handshake).protocolMagic2931 == pFVar7->playerRuntimeId) {
        TVar8 = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&pFVar7->playerName,TVar8.eax);
        InGameRecentTextHistory_InsertAndRebuild8(TVar8.eax);
        if (FVar4 - 1 != 0) {
          pFVar6 = pFVar7 + 1;
          for (iVar5 = (FVar4 - 1) * 0x4ec; iVar5 != 0; iVar5 = iVar5 + -1) {
            pFVar7->runtimeState00 = pFVar6->runtimeState00;
            pFVar6 = (FrontendPlayerRuntimeRecord *)&pFVar6->peerSequenceToken;
            pFVar7 = (FrontendPlayerRuntimeRecord *)&pFVar7->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return false;
      }
      pFVar7 = pFVar7 + 1;
      FVar4 = FVar4 - 1;
    } while (FVar4 != 0);
    return false;
  }
  return false;
}


/* Address: 0x00583D10.
   Ownership: network/backend/runtime.
   Purpose: Proven unreferenced trivial stub that preserves its recovered register set and returns zero.
*/
dword __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D10(void)

{
  return 0;
}

/* Address: 0x00583D30.
   Ownership: network/backend/runtime.
   Purpose: Proven unreferenced trivial no-op stub with the recovered register-preservation contract.
*/
void __thandor_void_preserve_eax_ecx_edx Unreferenced_NoOpPreserveRegs_00583D30(void)

{
  return;
}

/* Address: 0x00583D40.
   Ownership: network/backend/runtime.
   Purpose: Proven unreferenced trivial stub that preserves its recovered register set and returns zero.
*/
dword __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D40(void)

{
  return 0;
}

/* Address: 0x00584080.
   Ownership: network/backend/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime].
*/
dword __cdecl Network_Init(void)

{
  HINSTANCE module;
  HINSTANCE pHVar1;
  DynDllLoadEaxCf5 DVar2;
  DynApiResolveEaxCf5 DVar3;
  
  DVar2 = DynDLL_Load(s_Wsock32ModuleName);
  module = DVar2.moduleOrError;
  pHVar1 = module;
  if (!DVar2.carry) {
    DVar3 = DynAPI_Resolve(&g_WinSock_accept,module,s_Wsock32Export_accept);
    pHVar1 = DVar3.procedureOrError;
    if (!DVar3.carry) {
      DVar3 = DynAPI_Resolve(&g_WinSock_bind,module,s_Wsock32Export_bind);
      pHVar1 = DVar3.procedureOrError;
      if (!DVar3.carry) {
        DVar3 = DynAPI_Resolve(&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
        pHVar1 = DVar3.procedureOrError;
        if (!DVar3.carry) {
          DVar3 = DynAPI_Resolve(&g_WinSock_connect,module,s_Wsock32Export_connect);
          pHVar1 = DVar3.procedureOrError;
          if (!DVar3.carry) {
            DVar3 = DynAPI_Resolve(&g_WinSock_getpeername,module,s_Wsock32Export_getpeername);
            pHVar1 = DVar3.procedureOrError;
            if (!DVar3.carry) {
              DVar3 = DynAPI_Resolve(&g_WinSock_getsockname,module,s_Wsock32Export_getsockname);
              pHVar1 = DVar3.procedureOrError;
              if (!DVar3.carry) {
                DVar3 = DynAPI_Resolve(&g_WinSock_getsockopt,module,s_Wsock32Export_getsockopt);
                pHVar1 = DVar3.procedureOrError;
                if (!DVar3.carry) {
                  DVar3 = DynAPI_Resolve(&g_WinSock_htonl,module,s_Wsock32Export_htonl);
                  pHVar1 = DVar3.procedureOrError;
                  if (!DVar3.carry) {
                    DVar3 = DynAPI_Resolve(&g_WinSock_htons,module,s_Wsock32Export_htons);
                    pHVar1 = DVar3.procedureOrError;
                    if (!DVar3.carry) {
                      DVar3 = DynAPI_Resolve(&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr);
                      pHVar1 = DVar3.procedureOrError;
                      if (!DVar3.carry) {
                        DVar3 = DynAPI_Resolve(&g_WinSock_inet_ntoa,module,s_Wsock32Export_inet_ntoa
                                              );
                        pHVar1 = DVar3.procedureOrError;
                        if (!DVar3.carry) {
                          DVar3 = DynAPI_Resolve(&g_WinSock_ioctlsocket,module,
                                                 s_Wsock32Export_ioctlsocket);
                          pHVar1 = DVar3.procedureOrError;
                          if (!DVar3.carry) {
                            DVar3 = DynAPI_Resolve(&g_WinSock_listen,module,s_Wsock32Export_listen);
                            pHVar1 = DVar3.procedureOrError;
                            if (!DVar3.carry) {
                              DVar3 = DynAPI_Resolve(&g_WinSock_ntohl,module,s_Wsock32Export_ntohl);
                              pHVar1 = DVar3.procedureOrError;
                              if (!DVar3.carry) {
                                DVar3 = DynAPI_Resolve(&g_WinSock_ntohs,module,s_Wsock32Export_ntohs
                                                      );
                                pHVar1 = DVar3.procedureOrError;
                                if (!DVar3.carry) {
                                  DVar3 = DynAPI_Resolve(&g_WinSock_recv,module,s_Wsock32Export_recv
                                                        );
                                  pHVar1 = DVar3.procedureOrError;
                                  if (!DVar3.carry) {
                                    DVar3 = DynAPI_Resolve(&g_WinSock_recvfrom,module,
                                                           s_Wsock32Export_recvfrom);
                                    pHVar1 = DVar3.procedureOrError;
                                    if (!DVar3.carry) {
                                      DVar3 = DynAPI_Resolve(&g_WinSock_select,module,
                                                             s_Wsock32Export_select);
                                      pHVar1 = DVar3.procedureOrError;
                                      if (!DVar3.carry) {
                                        DVar3 = DynAPI_Resolve(&g_WinSock_send,module,
                                                               s_Wsock32Export_send);
                                        pHVar1 = DVar3.procedureOrError;
                                        if (!DVar3.carry) {
                                          DVar3 = DynAPI_Resolve(&g_WinSock_sendto,module,
                                                                 s_Wsock32Export_sendto);
                                          pHVar1 = DVar3.procedureOrError;
                                          if (!DVar3.carry) {
                                            DVar3 = DynAPI_Resolve(&g_WinSock_setsockopt,module,
                                                                   s_Wsock32Export_setsockopt);
                                            pHVar1 = DVar3.procedureOrError;
                                            if (!DVar3.carry) {
                                              DVar3 = DynAPI_Resolve(&g_WinSock_shutdown,module,
                                                                     s_Wsock32Export_shutdown);
                                              pHVar1 = DVar3.procedureOrError;
                                              if (!DVar3.carry) {
                                                DVar3 = DynAPI_Resolve(&g_WinSock_socket,module,
                                                                       s_Wsock32Export_socket);
                                                pHVar1 = DVar3.procedureOrError;
                                                if (!DVar3.carry) {
                                                  DVar3 = DynAPI_Resolve(&g_WinSock_gethostbyaddr,
                                                                         module,
                                                  s_Wsock32Export_gethostbyaddr);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_gethostbyname,
                                                                           module,
                                                  s_Wsock32Export_gethostbyname);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_gethostname,
                                                                           module,
                                                  s_Wsock32Export_gethostname);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_getprotobyname
                                                                           ,module,
                                                  s_Wsock32Export_getprotobyname);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_getprotobynumber,module,
                                                  s_Wsock32Export_getprotobynumber);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_getservbyname,
                                                                           module,
                                                  s_Wsock32Export_getservbyname);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_getservbyport,
                                                                           module,
                                                  s_Wsock32Export_getservbyport);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByAddr,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByAddr);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByName,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByName);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByName,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByName);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByNumber,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByNumber);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByName,module,
                                                  s_Wsock32Export_WSAAsyncGetServByName);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByPort,module,
                                                  s_Wsock32Export_WSAAsyncGetServByPort);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_WSAAsyncSelect
                                                                           ,module,
                                                  s_Wsock32Export_WSAAsyncSelect);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelAsyncRequest,module,
                                                  s_Wsock32Export_WSACancelAsyncRequest);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelBlockingCall,module,
                                                  s_Wsock32Export_WSACancelBlockingCall);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_WSACleanup,
                                                                           module,
                                                  s_Wsock32Export_WSACleanup);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAGetLastError,module,
                                                  s_Wsock32Export_WSAGetLastError);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_WSAIsBlocking,
                                                                           module,
                                                  s_Wsock32Export_WSAIsBlocking);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSASetBlockingHook,module,
                                                  s_Wsock32Export_WSASetBlockingHook);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&g_WinSock_WSAStartup,
                                                                           module,
                                                  s_Wsock32Export_WSAStartup);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    DVar3 = DynAPI_Resolve(&
                                                  g_WinSock_WSAUnhookBlockingHook,module,
                                                  s_Wsock32Export_WSAUnhookBlockingHook);
                                                  pHVar1 = DVar3.procedureOrError;
                                                  if (!DVar3.carry) {
                                                    pHVar1 = (HINSTANCE)
                                                             (*g_WinSock_WSAStartup)
                                                                       (0x101,&g_WinSockStartupData)
                                                    ;
                                                    if (pHVar1 == (HINSTANCE)0x0) {
                                                      g_NetworkBackendMode = 1;
                                                      g_NetworkBackendSlot0 =
                                                           NetworkBackend_SetSessionContextCf;
                                                      g_NetworkBackendSlot1 =
                                                           NetworkFallback_NoOpBackendCleanup;
                                                      g_NetworkBackendSlot2 =
                                                           NetworkFallback_OpenAndBindUdpSocketCf;
                                                      g_NetworkBackendSlot3 =
                                                           NetworkFallback_CloseActiveSocket;
                                                      g_NetworkBackendSlot4 =
                                                           NetworkFallback_ReceiveDatagramCf;
                                                      g_NetworkBackendSlot5 =
                                                           NetworkFallback_SendDatagramCf;
                                                      g_NetworkBackendSlot6 =
                                                           NetworkFallback_ParsePeerEndpointCf;
                                                      g_NetworkBackendSlot7 =
                                                           NetworkFallback_FormatPeerAddress;
                                                      g_NetworkBackendInstanceTable =
                                                           &
                                                  NetworkBackendInstanceDescriptorPrefix_00584040;
                                                  g_NetworkBackendInstanceCount = 1;
                                                  return 0;
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return (dword)pHVar1;
}


/* Address: 0x00584DF0.
   Ownership: network/backend/runtime.
   Purpose: Handles network shutdown.
*/
void __thandor_preserve_eax Network_Shutdown(void)

{
  if (g_NetworkBackendMode == 1) {
    (*g_WinSock_WSACleanup)();
    g_NetworkBackendMode = 0;
    return;
  }
  if (g_NetworkBackendMode == 2) {
    (*g_Ws2_32_WSACleanup)();
    g_NetworkBackendMode = 0;
    (*g_MemoryApi.free)(g_NetworkBackendInstanceTable);
    g_NetworkBackendInstanceTable = (NetworkBackendInstanceDescriptorPrefix *)0x0;
    g_NetworkBackendInstanceCount = 0;
  }
  return;
}


/* Address: 0x00584E50.
   Ownership: network/backend/runtime.
   Purpose: Typed parameters: p1 returnValue→NetworkBackendSessionReturnValue32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
NetworkBackendSetSessionEaxCf5 __thandor_this_eax_cf_preserve_ecx_edx
NetworkBackend_SetSessionContextCf(void *this,NetworkBackendSessionReturnValue32 returnValue)

{
  NetworkBackendSetSessionEaxCf5 NVar1;
  
  g_NetworkBackendSessionContext = this;
  NVar1.carry = false;
  NVar1.eax = returnValue;
  return NVar1;
}


/* Address: 0x00585210.
   Ownership: network/backend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
bool __thandor_cf_preserve_eax_ecx_edx NetworkBackend_SelectInstanceByIndex(dword instanceIndex)

{
  NetworkBackendInstanceDescriptorPrefix *selectedBackendDescriptor;
  
  if (instanceIndex < g_NetworkBackendInstanceCount) {
    g_NetworkBackendSessionContext = (NetworkSessionContext *)instanceIndex;
    selectedBackendDescriptor =
         (NetworkBackendInstanceDescriptorPrefix *)
         ((int)g_NetworkBackendInstanceTable + instanceIndex * 0x100);
    g_NetworkBackendActiveAddressFamily = selectedBackendDescriptor->addressFamily;
    g_NetworkBackendActiveSocketAddressLength = selectedBackendDescriptor->socketAddressLength;
    g_NetworkBackendActiveSocketType = selectedBackendDescriptor->socketType;
    g_NetworkBackendActiveProtocol = selectedBackendDescriptor->protocol;
    return false;
  }
  return true;
}

