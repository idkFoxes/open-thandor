#include <thandor/network/backend/runtime.h>

/* Implementation ownership: network/backend/runtime. */

/* Address: 0x0054EF60.
   Ownership: network/backend/runtime.
   Purpose: Recv: 0x10000 Handshake (magic check 0x2931), 0x20002 PlayerDescriptor, roster/state sync
   (exe_net_lobby_session.md).
   Cross-module calls: Random_GetSecondarySeed [core/math/random], UiTransfer_StagePacketAndSendCf
   [network/protocol/transfer].
*/
void __fastcall
FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (undefined4 param_1,undefined4 param_2,UiTransferEndpointDescriptor *senderEndpoint,
          FrontendTransferPacketUnion *packet)

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
undefined8 __fastcall
FrontendNetwork_HostTickCommandAndSnapshotTransfer(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  FrontendSnapshotTransferFlags FVar2;
  undefined4 in_EAX;
  uint uVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  FrontendPlayerRuntimeBlockCount extraout_ECX;
  uint uVar7;
  uint extraout_ECX_00;
  FrontendPlayerRuntimeBlockCount FVar8;
  uint extraout_ECX_01;
  FrontendPlayerRuntimeBlockCount extraout_ECX_02;
  FrontendPlayerRuntimeBlockCount extraout_ECX_03;
  UiTransferPacketPackedType UVar9;
  FrontendPlayerRuntimeBlockCount FVar10;
  FrontendSnapshotTransferFlags FVar11;
  FrontendSnapshotTransferFlags sourceSizeBytes;
  FrontendCommandPacketRecord *pFVar12;
  UiTransferEndpointDescriptor *pUVar13;
  FrontendPlayerRuntimeRecord *pFVar14;
  FrontendPlayerRuntimeRecord *pFVar15;
  byte *pbVar16;
  FrontendSnapshotTransferFlags *pFVar17;
  FrontendCommandPacketRecord *pFVar18;
  FrontendSnapshotTransferFlags *pFVar19;
  bool bVar20;
  void *pvVar22;
  undefined8 uVar21;
  
  iVar5 = g_FrontendPlayerRuntimeBlockCount - 1;
  pFVar15 = g_FrontendPlayerRuntimeBlocks;
  if (iVar5 != 0) {
    do {
      if (pFVar15[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        pUVar13 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        FVar10 = g_FrontendPlayerRuntimeBlockCount;
        while (FVar10 != 1) {
          if (pUVar13[1].addressHeader.packedFamilyAndPort == 0) {
            UiTransfer_StagePacketAndSendCf(pUVar13,&g_FrontendCommandBatchPacketBuffer[0].header);
            FVar10 = extraout_ECX_02;
          }
          else {
            g_FrontendPacket10012Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10012;
            UiTransfer_StagePacketAndSendCf(pUVar13,&g_FrontendPacket10012Buffer.header);
            FVar10 = extraout_ECX_03;
          }
          pUVar13 = pUVar13 + 0x13b;
        }
        return CONCAT44(param_2,in_EAX);
      }
      iVar5 = iVar5 + -1;
      pFVar15 = pFVar15 + 1;
    } while (iVar5 != 0);
    iVar5 = g_FrontendPlayerRuntimeBlockCount - 1;
    pFVar15 = g_FrontendPlayerRuntimeBlocks;
    do {
      pFVar15[1].commandSyncPending = FRONTEND_COMMAND_SYNC_CLEAR;
      iVar5 = iVar5 + -1;
      pFVar15 = pFVar15 + 1;
    } while (iVar5 != 0);
  }
  g_UiTransferSenderContext = g_UiTransferSenderContext + 1;
  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  iVar5 = 0;
  pFVar12 = g_FrontendPlayerCommandRecords;
  pFVar18 = g_FrontendCommandBatchPacketBuffer;
  FVar10 = g_FrontendPlayerRuntimeBlockCount;
FrontendNetwork_HostTick_ScanNextPlayerCommandRecord:
  FVar8 = FVar10;
  if (((pFVar12->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
    pFVar12 = pFVar12 + 1;
    goto FrontendNetwork_HostTick_AdvanceCommandScanOrRestartUntilBatchNonempty;
  }
  while( true ) {
    for (iVar6 = 8; iVar6 != 0; iVar6 = iVar6 + -1) {
      (pFVar18->header).packedTypeAndUnitCount = (pFVar12->header).packedTypeAndUnitCount;
      pFVar12 = (FrontendCommandPacketRecord *)&(pFVar12->header).sequenceToken;
      pFVar18 = (FrontendCommandPacketRecord *)&(pFVar18->header).sequenceToken;
    }
    iVar5 = iVar5 + 1;
FrontendNetwork_HostTick_AdvanceCommandScanOrRestartUntilBatchNonempty:
    FVar10 = FVar8 - 1;
    if (FVar8 - 1 != 0) goto FrontendNetwork_HostTick_ScanNextPlayerCommandRecord;
    if (iVar5 << 0x10 != 0) break;
    pFVar12 = g_FrontendPlayerCommandRecords;
    iVar5 = 0;
  }
  UVar9 = iVar5 << 0x10 | 0x10;
  pUVar13 = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = UVar9;
  FVar10 = g_FrontendPlayerRuntimeBlockCount;
  while (FVar10 != 1) {
    uVar21 = UiTransfer_StagePacketAndSendCf(pUVar13,&g_FrontendCommandBatchPacketBuffer[0].header);
    UVar9 = (UiTransferPacketPackedType)((ulonglong)uVar21 >> 0x20);
    pUVar13 = pUVar13 + 0x13b;
    FVar10 = extraout_ECX;
  }
  pFVar12 = g_FrontendCommandBatchPacketBuffer;
  uVar7 = UVar9 >> 0x10;
  do {
    uVar1 = (pFVar12->command).packedCommandAndPlayerId;
    uVar3 = uVar1 >> 8;
    if (uVar3 != 0) {
      if (FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3 < &g_FrontendRootNode) {
        (*(FrontendCommandQueue_EnqueueLocalPlayerCommand + uVar3))
                  (uVar1 & 0xff,(pFVar12->command).payloadDword0C,(pFVar12->command).payloadDword08,
                   (pFVar12->command).payloadDword04);
        uVar7 = extraout_ECX_00;
      }
    }
    FVar10 = g_FrontendPlayerRuntimeBlockCount;
    pFVar15 = g_FrontendPlayerRuntimeBlocks;
    pFVar12 = pFVar12 + 1;
    uVar7 = uVar7 - 1;
  } while (uVar7 != 0);
  if ((g_FrontendHostSnapshotTransferCountdown != 0) &&
     (g_FrontendHostSnapshotTransferCountdown = g_FrontendHostSnapshotTransferCountdown + -1,
     FVar8 = g_FrontendPlayerRuntimeBlockCount, pFVar14 = g_FrontendPlayerRuntimeBlocks,
     g_FrontendHostSnapshotTransferCountdown == 0)) {
    do {
      if (((pFVar14->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
         ((pFVar14->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) {
        g_FrontendPacket10009Buffer.snapshotChunkOffset = pFVar14->snapshotChunkOffset;
        g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009;
        UiTransfer_StagePacketAndSendCf(&pFVar14->endpoint,&g_FrontendPacket10009Buffer.header);
        g_FrontendHostSnapshotTransferCountdown = 4;
        goto FrontendNetwork_HostTick_ReturnAfterSnapshotTransferStep;
      }
      FVar8 = FVar8 - 1;
      pFVar14 = pFVar14 + 1;
    } while (FVar8 != 0);
    g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags =
         g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags |
         FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
    FVar11 = 0;
    pFVar17 = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
    if (1 < FVar10) {
      do {
        FVar2 = pFVar15->snapshotTransferFlags;
        *pFVar17 = FVar2;
        sourceSizeBytes = FVar11 + FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
        pFVar17 = pFVar17 + 1;
        if ((FVar2 & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
          pbVar16 = pFVar15->snapshotPayloadB0_13AF;
          for (iVar5 = 0x4c0; iVar5 != 0; iVar5 = iVar5 + -1) {
            *pFVar17 = *(FrontendSnapshotTransferFlags *)pbVar16;
            pbVar16 = pbVar16 + 4;
            pFVar17 = pFVar17 + 1;
          }
          sourceSizeBytes = FVar11 + 0x1304;
        }
        pFVar15 = pFVar15 + 1;
        FVar10 = FVar10 - 1;
        FVar11 = sourceSizeBytes;
      } while (FVar10 != 0);
      bVar20 = 0x7ffffc < sourceSizeBytes;
      dVar4 = PckCodec_EncodeHuffmanRle
                        (0x7ffffc - sourceSizeBytes,(byte *)(pFVar17 + 1),sourceSizeBytes,
                         g_PackageScratchBuffer);
      if (!bVar20) {
        *pFVar17 = sourceSizeBytes;
        _pvVar22 = (*g_MemoryApi.alloc)(dVar4 + 4);
        if (!bVar20) {
          pFVar19 = SUB84(_pvVar22,0);
          for (uVar7 = extraout_ECX_01 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
            *pFVar19 = *pFVar17;
            pFVar17 = pFVar17 + 1;
            pFVar19 = pFVar19 + 1;
          }
          UiTransferMailbox_SetOutgoingBuffer
                    ((UiTransferPayloadByteCount)((ulonglong)_pvVar22 >> 0x20),SUB84(_pvVar22,0));
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x16f0,0,0,0);
        }
      }
    }
  }
FrontendNetwork_HostTick_ReturnAfterSnapshotTransferStep:
  return CONCAT44(param_2,in_EAX);
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
void __cdecl FrontendNetwork_TickDisconnectTimeoutAndResetSession(void)

{
  uint *puVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  int iVar3;
  word *stream;
  void *replacementPayload;
  FrontendPlayerRuntimeBlockCount FVar4;
  FrontendPlayerRuntimeRecord *pFVar5;
  
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
    stream = TextResource_Resolve(0xff01);
    RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
    FrontendRecentTextHistory_InsertAndRebuild5();
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
undefined8
FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet)

{
  UiTransferSenderContext UVar1;
  undefined4 in_EAX;
  uint uVar2;
  word *stream;
  uint uVar3;
  uint extraout_ECX;
  FrontendPlayerRuntimeBlockCount FVar4;
  int extraout_ECX_00;
  int iVar5;
  undefined4 in_EDX;
  void *replacementPayload;
  FrontendPlayerRuntimeRecord *pFVar6;
  FrontendPlayerRuntimeRecord *pFVar7;
  
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
                       (packet->packet50001SessionAdvertisement).joinAvailableFlag);
            uVar3 = extraout_ECX;
          }
        }
        packet = (FrontendTransferPacketUnion *)
                 ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
        uVar3 = uVar3 - 1;
      } while (uVar3 != 0);
      FrontendTransfer_SendCommandBatchRequest10021();
      g_FrontendTransferResponsePending = 1;
      return CONCAT44(in_EDX,in_EAX);
    }
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10021Buffer.header);
    return CONCAT44(in_EDX,in_EAX);
  }
  if ((((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10022) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    g_SessionTransferTimeoutTicks = 0x100;
    g_FrontendPacket10023Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10023;
    UiTransfer_StagePacketAndSendCf
              (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10023Buffer.header);
    return CONCAT44(in_EDX,in_EAX);
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
        stream = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
        InGameRecentTextHistory_InsertAndRebuild8();
        if (extraout_ECX_00 + -1 != 0) {
          pFVar6 = pFVar7 + 1;
          for (iVar5 = (extraout_ECX_00 + -1) * 0x4ec; iVar5 != 0; iVar5 = iVar5 + -1) {
            pFVar7->runtimeState00 = pFVar6->runtimeState00;
            pFVar6 = (FrontendPlayerRuntimeRecord *)&pFVar6->peerSequenceToken;
            pFVar7 = (FrontendPlayerRuntimeRecord *)&pFVar7->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return CONCAT44(in_EDX,in_EAX);
      }
      pFVar7 = pFVar7 + 1;
      FVar4 = FVar4 - 1;
    } while (FVar4 != 0);
    return CONCAT44(in_EDX,in_EAX);
  }
  return CONCAT44(in_EDX,in_EAX);
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
  bool bVar2;
  
  bVar2 = &stack0xffffffe8 < (undefined1 *)0x4;
  module = DynDLL_Load(s_Wsock32ModuleName);
  pHVar1 = module;
  if (!bVar2) {
    pHVar1 = DynAPI_Resolve(&g_WinSock_accept,module,s_Wsock32Export_accept);
    if (!bVar2) {
      pHVar1 = DynAPI_Resolve(&g_WinSock_bind,module,s_Wsock32Export_bind);
      if (!bVar2) {
        pHVar1 = DynAPI_Resolve(&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
        if (!bVar2) {
          pHVar1 = DynAPI_Resolve(&g_WinSock_connect,module,s_Wsock32Export_connect);
          if (!bVar2) {
            pHVar1 = DynAPI_Resolve(&g_WinSock_getpeername,module,s_Wsock32Export_getpeername);
            if (!bVar2) {
              pHVar1 = DynAPI_Resolve(&g_WinSock_getsockname,module,s_Wsock32Export_getsockname);
              if (!bVar2) {
                pHVar1 = DynAPI_Resolve(&g_WinSock_getsockopt,module,s_Wsock32Export_getsockopt);
                if (!bVar2) {
                  pHVar1 = DynAPI_Resolve(&g_WinSock_htonl,module,s_Wsock32Export_htonl);
                  if (!bVar2) {
                    pHVar1 = DynAPI_Resolve(&g_WinSock_htons,module,s_Wsock32Export_htons);
                    if (!bVar2) {
                      pHVar1 = DynAPI_Resolve(&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr)
                      ;
                      if (!bVar2) {
                        pHVar1 = DynAPI_Resolve(&g_WinSock_inet_ntoa,module,
                                                s_Wsock32Export_inet_ntoa);
                        if (!bVar2) {
                          pHVar1 = DynAPI_Resolve(&g_WinSock_ioctlsocket,module,
                                                  s_Wsock32Export_ioctlsocket);
                          if (!bVar2) {
                            pHVar1 = DynAPI_Resolve(&g_WinSock_listen,module,s_Wsock32Export_listen)
                            ;
                            if (!bVar2) {
                              pHVar1 = DynAPI_Resolve(&g_WinSock_ntohl,module,s_Wsock32Export_ntohl)
                              ;
                              if (!bVar2) {
                                pHVar1 = DynAPI_Resolve(&g_WinSock_ntohs,module,
                                                        s_Wsock32Export_ntohs);
                                if (!bVar2) {
                                  pHVar1 = DynAPI_Resolve(&g_WinSock_recv,module,
                                                          s_Wsock32Export_recv);
                                  if (!bVar2) {
                                    pHVar1 = DynAPI_Resolve(&g_WinSock_recvfrom,module,
                                                            s_Wsock32Export_recvfrom);
                                    if (!bVar2) {
                                      pHVar1 = DynAPI_Resolve(&g_WinSock_select,module,
                                                              s_Wsock32Export_select);
                                      if (!bVar2) {
                                        pHVar1 = DynAPI_Resolve(&g_WinSock_send,module,
                                                                s_Wsock32Export_send);
                                        if (!bVar2) {
                                          pHVar1 = DynAPI_Resolve(&g_WinSock_sendto,module,
                                                                  s_Wsock32Export_sendto);
                                          if (!bVar2) {
                                            pHVar1 = DynAPI_Resolve(&g_WinSock_setsockopt,module,
                                                                    s_Wsock32Export_setsockopt);
                                            if (!bVar2) {
                                              pHVar1 = DynAPI_Resolve(&g_WinSock_shutdown,module,
                                                                      s_Wsock32Export_shutdown);
                                              if (!bVar2) {
                                                pHVar1 = DynAPI_Resolve(&g_WinSock_socket,module,
                                                                        s_Wsock32Export_socket);
                                                if (!bVar2) {
                                                  pHVar1 = DynAPI_Resolve(&g_WinSock_gethostbyaddr,
                                                                          module,
                                                  s_Wsock32Export_gethostbyaddr);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_gethostbyname
                                                                            ,module,
                                                  s_Wsock32Export_gethostbyname);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_gethostname,
                                                                            module,
                                                  s_Wsock32Export_gethostname);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_getprotobyname,module,
                                                  s_Wsock32Export_getprotobyname);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_getprotobynumber,module,
                                                  s_Wsock32Export_getprotobynumber);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_getservbyname
                                                                            ,module,
                                                  s_Wsock32Export_getservbyname);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_getservbyport
                                                                            ,module,
                                                  s_Wsock32Export_getservbyport);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByAddr,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByAddr);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByName,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByName);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByName,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByName);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByNumber,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByNumber);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByName,module,
                                                  s_Wsock32Export_WSAAsyncGetServByName);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByPort,module,
                                                  s_Wsock32Export_WSAAsyncGetServByPort);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncSelect,module,
                                                  s_Wsock32Export_WSAAsyncSelect);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelAsyncRequest,module,
                                                  s_Wsock32Export_WSACancelAsyncRequest);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelBlockingCall,module,
                                                  s_Wsock32Export_WSACancelBlockingCall);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_WSACleanup,
                                                                            module,
                                                  s_Wsock32Export_WSACleanup);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAGetLastError,module,
                                                  s_Wsock32Export_WSAGetLastError);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_WSAIsBlocking
                                                                            ,module,
                                                  s_Wsock32Export_WSAIsBlocking);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSASetBlockingHook,module,
                                                  s_Wsock32Export_WSASetBlockingHook);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&g_WinSock_WSAStartup,
                                                                            module,
                                                  s_Wsock32Export_WSAStartup);
                                                  if (!bVar2) {
                                                    pHVar1 = DynAPI_Resolve(&
                                                  g_WinSock_WSAUnhookBlockingHook,module,
                                                  s_Wsock32Export_WSAUnhookBlockingHook);
                                                  if (!bVar2) {
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
void __cdecl Network_Shutdown(void)

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
NetworkBackendSessionReturnValue32 __thiscall
NetworkBackend_SetSessionContextCf(void *this,NetworkBackendSessionReturnValue32 returnValue)

{
  g_NetworkBackendSessionContext = this;
  return returnValue;
}

/* Address: 0x00585210.
   Ownership: network/backend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
undefined8 NetworkBackend_SelectInstanceByIndex(NetworkSessionContext *param_1)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
  undefined4 *puVar1;
  
  if (param_1 < g_NetworkBackendInstanceCount) {
    g_NetworkBackendSessionContext = param_1;
    puVar1 = (undefined4 *)((int)g_NetworkBackendInstanceTable + (int)param_1 * 0x100);
    DAT_00583f0e = *puVar1;
    DAT_00583f12 = puVar1[1];
    DAT_00583f16 = puVar1[2];
    DAT_00583f1a = puVar1[3];
    return CONCAT44(in_EDX,in_EAX);
  }
  return CONCAT44(in_EDX,in_EAX);
}
