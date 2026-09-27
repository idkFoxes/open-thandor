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
   Ownership: network/backend/runtime.
   Purpose: Recv: 0x10000 Handshake (magic check 0x2931), 0x20002 PlayerDescriptor, roster/state sync
   (exe_net_lobby_session.md).
   Cross-module calls: Random_GetSecondarySeed [core/math/random], UiTransfer_StagePacketAndSendCf
   [network/protocol/transfer].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendNetwork_HandleHandshakeAndPlayerStatePackets
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
  FrontendPacket30005PlayerSnapshot *snapshotCursor;
  uint8_t *payloadCursor;
  
  sequenceTokenOrPlayerIndex = (packet->packet10000Handshake).header.sequenceToken;
  senderAddress = senderEndpoint->ipv4AddressNetworkOrder;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10011) {
    commandRecord = g_FrontendPlayerCommandRecords;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
      playerRecord = playerRecord + 1;
      commandRecord = commandRecord + 1;
      remainingPlayers = remainingPlayers - 1;
      if (remainingPlayers == 0) {
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
        packet = (FrontendTransferPacketUnion *)&(packet->packet10000Handshake).header.sequenceToken
        ;
        commandRecord = (FrontendCommandPacketRecord *)&(commandRecord->header).sequenceToken;
      }
    }
    return;
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount == FRONTEND_PACKET_10013) {
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
      remainingPlayers = remainingPlayers - 1;
      playerRecord = playerRecord + 1;
      if (remainingPlayers == 0) {
        return;
      }
    }
    playerRecord->heartbeatExpiryTicks = 0x100;
    return;
  }
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_10004_SNAPSHOT_REQUEST) {
    while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
           (senderAddress != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
      remainingPlayers = remainingPlayers - 1;
      playerRecord = playerRecord + 1;
      if (remainingPlayers == 0) {
        return;
      }
    }
    sequenceTokenOrPlayerIndex = (packet->packet10004PlayerSnapshotRequest).requestedPlayerIndex;
    if (sequenceTokenOrPlayerIndex < g_FrontendPlayerRuntimeBlockCount) {
      playerRecord = g_FrontendPlayerRuntimeBlocks + sequenceTokenOrPlayerIndex;
      snapshotCursor = &g_FrontendPacket30005Buffer;
      for (dwordCount = 0x18; dwordCount != 0; dwordCount = dwordCount + -1) {
        (snapshotCursor->header).packedTypeAndUnitCount = playerRecord->runtimeState00;
        playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
        snapshotCursor = (FrontendPacket30005PlayerSnapshot *)&(snapshotCursor->header).sequenceToken;
      }
      g_FrontendPacket30005Buffer.header.packedTypeAndUnitCount =
           FRONTEND_PACKET_30005_PLAYER_SNAPSHOT;
      g_FrontendPacket30005Buffer.playerIndex = sequenceTokenOrPlayerIndex;
      g_FrontendPacket30005Buffer.secondaryRandomSeed = Random_GetSecondarySeed();
      UiTransfer_StagePacketAndSendCf(senderEndpoint,&g_FrontendPacket30005Buffer.header);
      return;
    }
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if ((packet->packet10000Handshake).header.packedTypeAndUnitCount != FRONTEND_PACKET_8000A) {
    return;
  }
  while ((sequenceTokenOrPlayerIndex != playerRecord->peerSequenceToken ||
         (senderAddress != (playerRecord->endpoint).ipv4AddressNetworkOrder))) {
    remainingPlayers = remainingPlayers - 1;
    playerRecord = playerRecord + 1;
    if (remainingPlayers == 0) {
      return;
    }
  }
  packetChunkOffset = (packet->packet50001SessionAdvertisement).joinAvailableFlag;
  if ((((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
      ((playerRecord->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) &&
     (packetChunkOffset == playerRecord->snapshotChunkOffset)) {
    sourceDword = &(packet->command10011Or10021).command.payloadDword08;
    payloadCursor = playerRecord->snapshotPayloadB0_13AF + packetChunkOffset;
    dwordCount = 0x3a;
    if (packetChunkOffset == 0x1220) {
      dwordCount = 0x38;
      playerRecord->snapshotTransferFlags =
           playerRecord->snapshotTransferFlags | FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE;
      playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + 0xe0;
    }
    for (; dwordCount != 0; dwordCount = dwordCount + -1) {
      *(uint32_t *)payloadCursor = *sourceDword;
      sourceDword = sourceDword + 1;
      payloadCursor = payloadCursor + 4;
    }
    if (packetChunkOffset != 0x1220) {
      g_FrontendPacket10009Buffer.snapshotChunkOffset = packetChunkOffset + 0xe8;
      playerRecord->snapshotChunkOffset = playerRecord->snapshotChunkOffset + 0xe8;
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
FrontendNetwork_HostTickCommandAndSnapshotTransfer(uint32_t callbackArg)

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
  FrontendSnapshotTransferFlags *scratchCursor;
  FrontendCommandPacketRecord *batchCursor;
  FrontendSnapshotTransferFlags *outgoingCursor;
  PckCodecEaxCf5 encodeResult;
  ArenaAllocEaxCf5 allocResult;
  
  loopCount = g_FrontendPlayerRuntimeBlockCount - 1;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  if (loopCount != 0) {
    do {
      if (playerRecord[1].commandSyncPending == FRONTEND_COMMAND_SYNC_CLEAR) {
        peerEndpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
        remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
        while (remainingPlayerCount = remainingPlayerCount - 1, remainingPlayerCount != 0) {
          if (peerEndpoint[1].addressHeader.packedFamilyAndPort == 0) {
            UiTransfer_StagePacketAndSendCf(peerEndpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
          }
          else {
            g_FrontendPacket10012Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10012;
            UiTransfer_StagePacketAndSendCf(peerEndpoint,&g_FrontendPacket10012Buffer.header);
          }
          peerEndpoint = peerEndpoint + 0x13b;
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
  /* Pack every non-empty 0x20-byte command record into the batch. */
  do {
    if (((commandRecord->command).packedCommandAndPlayerId & 0xffffff00) == 0) {
      commandRecord = commandRecord + 1;
    }
    else {
      for (loopCount = 8; loopCount != 0; loopCount = loopCount + -1) {
        (batchCursor->header).packedTypeAndUnitCount = (commandRecord->header).packedTypeAndUnitCount;
        commandRecord = (FrontendCommandPacketRecord *)&(commandRecord->header).sequenceToken;
        batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
      }
      commandCountOrBufferSize = commandCountOrBufferSize + 1;
    }
    remainingPlayerCount = remainingPlayerCount - 1;
  } while (remainingPlayerCount != 0);
  if (commandCountOrBufferSize << 0x10 == 0) {
    /* Nothing pending: send the first record (the host's own) as a batch of one. */
    commandRecord = g_FrontendPlayerCommandRecords;
    for (loopCount = 8; loopCount != 0; loopCount = loopCount + -1) {
      (batchCursor->header).packedTypeAndUnitCount = (commandRecord->header).packedTypeAndUnitCount;
      commandRecord = (FrontendCommandPacketRecord *)&(commandRecord->header).sequenceToken;
      batchCursor = (FrontendCommandPacketRecord *)&(batchCursor->header).sequenceToken;
    }
    commandCountOrBufferSize = 1;
  }
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount = commandCountOrBufferSize << 0x10 | 0x10;
  peerEndpoint = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
  while (remainingPlayerCount = remainingPlayerCount - 1, remainingPlayerCount != 0) {
    UiTransfer_StagePacketAndSendCf(peerEndpoint,&g_FrontendCommandBatchPacketBuffer[0].header);
    peerEndpoint = peerEndpoint + 0x13b;
  }
  /* Execute the batch locally as well. */
  commandRecord = g_FrontendCommandBatchPacketBuffer;
  commandCountOrBufferSize = commandCountOrBufferSize & 0xffff;
  do {
    packedCommandOrDwordCount = (commandRecord->command).packedCommandAndPlayerId;
    commandHandlerIndex = packedCommandOrDwordCount >> 8;
    if (commandHandlerIndex != 0) {
      if (THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&g_FrontendRootNode) {
        (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(FrontendCommandQueue_EnqueueLocalPlayerCommand, commandHandlerIndex))
                  (packedCommandOrDwordCount & 0xff,(commandRecord->command).payloadDword0C,
                   (commandRecord->command).payloadDword08,(commandRecord->command).payloadDword04);
      }
    }
    remainingPlayerCount = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    commandRecord = commandRecord + 1;
    commandCountOrBufferSize = commandCountOrBufferSize - 1;
  } while (commandCountOrBufferSize != 0);
  if ((g_FrontendHostSnapshotTransferCountdown != 0) &&
     (g_FrontendHostSnapshotTransferCountdown = g_FrontendHostSnapshotTransferCountdown + -1,
     remainingPlayers = g_FrontendPlayerRuntimeBlockCount, transferPlayer = g_FrontendPlayerRuntimeBlocks,
     g_FrontendHostSnapshotTransferCountdown == 0)) {
    do {
      if (((transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_SOURCE_AVAILABLE) != 0) &&
         ((transferPlayer->snapshotTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) == 0)) {
        g_FrontendPacket10009Buffer.snapshotChunkOffset = transferPlayer->snapshotChunkOffset;
        g_FrontendPacket10009Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10009;
        UiTransfer_StagePacketAndSendCf
                  (&transferPlayer->endpoint,&g_FrontendPacket10009Buffer.header);
        g_FrontendHostSnapshotTransferCountdown = 4;
        return false;
      }
      remainingPlayers = remainingPlayers - 1;
      transferPlayer = transferPlayer + 1;
    } while (remainingPlayers != 0);
    g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags =
         g_FrontendPlayerRuntimeBlocks->snapshotTransferFlags |
         FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
    scratchSizeBytes = 0;
    scratchCursor = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
    if (1 < remainingPlayerCount) {
      do {
        playerFlags = playerRecord->snapshotTransferFlags;
        *scratchCursor = playerFlags;
        sourceSizeBytes = scratchSizeBytes + FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY;
        scratchCursor = scratchCursor + 1;
        if ((playerFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
          payloadCursor = playerRecord->snapshotPayloadB0_13AF;
          for (loopCount = 0x4c0; loopCount != 0; loopCount = loopCount + -1) {
            *scratchCursor = *(FrontendSnapshotTransferFlags *)payloadCursor;
            payloadCursor = payloadCursor + 4;
            scratchCursor = scratchCursor + 1;
          }
          sourceSizeBytes = scratchSizeBytes + 0x1304;
        }
        playerRecord = playerRecord + 1;
        remainingPlayerCount = remainingPlayerCount - 1;
        scratchSizeBytes = sourceSizeBytes;
      } while (remainingPlayerCount != 0);
      encodeResult = PckCodec_EncodeHuffmanRle
                         (0x7ffffc - sourceSizeBytes,(uint8_t *)(scratchCursor + 1),sourceSizeBytes,
                          g_PackageScratchBuffer);
      if (!encodeResult.carry) {
        *scratchCursor = sourceSizeBytes;
        commandCountOrBufferSize = encodeResult.eax + 4;
        allocResult = (*g_MemoryApi.alloc)(commandCountOrBufferSize);
        if (!allocResult.carry) {
          outgoingCursor = (FrontendSnapshotTransferFlags *)allocResult.eax;
          for (packedCommandOrDwordCount = commandCountOrBufferSize >> 2; packedCommandOrDwordCount != 0; packedCommandOrDwordCount = packedCommandOrDwordCount - 1) {
            *outgoingCursor = *scratchCursor;
            scratchCursor = scratchCursor + 1;
            outgoingCursor = outgoingCursor + 1;
          }
          UiTransferMailbox_SetOutgoingBuffer
                    (commandCountOrBufferSize,(FrontendSnapshotTransferFlags *)allocResult.eax);
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x16f0,0,0,0);
        }
      }
    }
  }
  return false;
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
  uint32_t *frontendRootFlags;
  FrontendPlayerRuntimeRecord *localPlayerRecord;
  int frontendRootBase;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceResolveEaxCf5 resolvedText;
  
  g_SessionTransferTimeoutTicks = g_SessionTransferTimeoutTicks - 1;
  if (g_SessionTransferTimeoutTicks == 0) {
    UiTransferMailbox_ClearReceivedState();
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
    g_FrontendNetworkState = 0;
    (*g_NetworkBackendSlot3)();
    (*g_NetworkBackendSlot1)();
    frontendRootBase = g_FrontendRootNode;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((g_FrontendRuntimeFlags & 0x10) != 0) {
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(g_FrontendRootNode,frontendPageStack));
      frontendRootFlags = &FRONTEND_UI_FIELD(frontendRootBase,menuRoomModelView,0x4C,uint32_t);
      *frontendRootFlags = *frontendRootFlags & 0xffffdfff;
      g_FrontendPendingPageAction = 0;
      g_FrontendRomTransitionContextValue = 0;
      FrontendRomTransition_ActivateRecordByIdCf
                (1,(WorldRuntimeContext *)FRONTEND_UI(frontendRootBase,menuRoomModelView));
    }
    resolvedText = TextResource_Resolve(0xff01);
    RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText.eax);
    FrontendRecentTextHistory_InsertAndRebuild5(resolvedText.eax);
    localPlayerRecord = g_FrontendPlayerRuntimeBlocks;
    for (remainingPlayers = g_FrontendPlayerRuntimeBlockCount; remainingPlayers != 0; remainingPlayers = remainingPlayers - 1) {
      if ((playerRecord->factionAssignment).readyOrWaitState == 0) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0xd0,0,0,0);
        }
        playerRecord = g_FrontendPlayerRuntimeBlocks;
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        (playerRecord->playerName).textUtf16[0] = 0;
        (playerRecord->playerName).textUtf16[1] = 0;
        playerRecord->playerRuntimeId = 0;
        (playerRecord->factionAssignment).roleStateFlags = 0;
        playerRecord->runtimeState64 = 0;
        playerRecord->snapshotTransferFlags = 0;
        return;
      }
      playerRecord = playerRecord + 1;
    }
    g_FrontendPlayerRuntimeBlockCount = 1;
    g_LocalPlayerRuntimeId = 0;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    (playerRecord->playerName).textUtf16[0] = 0;
    (playerRecord->playerName).textUtf16[1] = 0;
    localPlayerRecord->playerRuntimeId = 0;
    (localPlayerRecord->factionAssignment).roleStateFlags = 0;
    localPlayerRecord->runtimeState64 = 0;
    localPlayerRecord->snapshotTransferFlags = 0;
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
  UiTransferSenderContext packetSenderContext;
  uint32_t commandHandlerIndex;
  uint32_t remainingCommands;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  int dwordCount;
  FrontendPlayerRuntimeRecord *nextPlayerRecord;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceResolveEaxCf5 resolvedText;
  
  if (((((packet->packet10000Handshake).header.packedTypeAndUnitCount & 0xffff) == 0x20) &&
      (g_FrontendSessionToken == (packet->packet10000Handshake).header.sequenceToken)) &&
     (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder ==
      sessionContext->ipv4AddressNetworkOrder)) {
    packetSenderContext = (packet->packet10000Handshake).header.senderContext;
    g_SessionTransferTimeoutTicks = 0x100;
    if (packetSenderContext != g_FrontendSelectedPlayerToken) {
      remainingCommands = (packet->packet10000Handshake).header.packedTypeAndUnitCount >> 0x10;
      g_FrontendSelectedPlayerToken = packetSenderContext;
      do {
        commandHandlerIndex = (packet->packet10000Handshake).protocolMagic2931 >> 8;
        if (commandHandlerIndex != 0) {
          if (THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex) < (unsigned char *)&InGameCommandHandlerCodeRegionEnd) {
            (*(CommandQueueHandlerProc *)THANDOR_CODE_AT(InGameCommandQueue_AppendLocalPlayerCommand, commandHandlerIndex))
                      ((packet->packet10000Handshake).protocolMagic2931 & 0xff,
                       (packet->packet20002PlayerDescriptor).playerDescriptorPayload[1],
                       (packet->packet20002PlayerDescriptor).playerDescriptorPayload[0],
                       (packet->packet20002PlayerDescriptor).reserved14);
          }
        }
        packet = (FrontendTransferPacketUnion *)
                 ((packet->packet50001SessionAdvertisement).sessionTitleUtf16 + 4);
        remainingCommands = remainingCommands - 1;
      } while (remainingCommands != 0);
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
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    do {
      if ((packet->packet10000Handshake).protocolMagic2931 == playerRecord->playerRuntimeId) {
        resolvedText = TextResource_Resolve(0xff00);
        RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,resolvedText.eax);
        InGameRecentTextHistory_InsertAndRebuild8(resolvedText.eax);
        if (remainingPlayers - 1 != 0) {
          nextPlayerRecord = playerRecord + 1;
          for (dwordCount = (remainingPlayers - 1) * 0x4ec; dwordCount != 0; dwordCount = dwordCount + -1) {
            playerRecord->runtimeState00 = nextPlayerRecord->runtimeState00;
            nextPlayerRecord = (FrontendPlayerRuntimeRecord *)&nextPlayerRecord->peerSequenceToken;
            playerRecord = (FrontendPlayerRuntimeRecord *)&playerRecord->peerSequenceToken;
          }
        }
        g_FrontendPlayerRuntimeBlockCount = g_FrontendPlayerRuntimeBlockCount - 1;
        return false;
      }
      playerRecord = playerRecord + 1;
      remainingPlayers = remainingPlayers - 1;
    } while (remainingPlayers != 0);
    return false;
  }
  return false;
}


/* Address: 0x00583D10.
   Ownership: network/backend/runtime.
   Purpose: Proven unreferenced trivial stub that preserves its recovered register set and returns zero.
*/
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D10(void)

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
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D40(void)

{
  return 0;
}

/* Address: 0x00584080.
   Ownership: network/backend/runtime.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
   Cross-module calls: DynDLL_Load [platform/bootstrap/runtime], DynAPI_Resolve [platform/bootstrap/runtime].
*/
uint32_t __cdecl Network_Init(void)

{
  HINSTANCE module;
  HINSTANCE resultOrError;
  DynDllLoadEaxCf5 loadResult;
  DynApiResolveEaxCf5 resolveResult;
  
  loadResult = DynDLL_Load(s_Wsock32ModuleName);
  module = loadResult.moduleOrError;
  resultOrError = module;
  if (!loadResult.carry) {
    resolveResult = DynAPI_Resolve(&g_WinSock_accept,module,s_Wsock32Export_accept);
    resultOrError = resolveResult.procedureOrError;
    if (!resolveResult.carry) {
      resolveResult = DynAPI_Resolve(&g_WinSock_bind,module,s_Wsock32Export_bind);
      resultOrError = resolveResult.procedureOrError;
      if (!resolveResult.carry) {
        resolveResult = DynAPI_Resolve(&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
        resultOrError = resolveResult.procedureOrError;
        if (!resolveResult.carry) {
          resolveResult = DynAPI_Resolve(&g_WinSock_connect,module,s_Wsock32Export_connect);
          resultOrError = resolveResult.procedureOrError;
          if (!resolveResult.carry) {
            resolveResult = DynAPI_Resolve(&g_WinSock_getpeername,module,s_Wsock32Export_getpeername);
            resultOrError = resolveResult.procedureOrError;
            if (!resolveResult.carry) {
              resolveResult = DynAPI_Resolve(&g_WinSock_getsockname,module,s_Wsock32Export_getsockname);
              resultOrError = resolveResult.procedureOrError;
              if (!resolveResult.carry) {
                resolveResult = DynAPI_Resolve(&g_WinSock_getsockopt,module,s_Wsock32Export_getsockopt);
                resultOrError = resolveResult.procedureOrError;
                if (!resolveResult.carry) {
                  resolveResult = DynAPI_Resolve(&g_WinSock_htonl,module,s_Wsock32Export_htonl);
                  resultOrError = resolveResult.procedureOrError;
                  if (!resolveResult.carry) {
                    resolveResult = DynAPI_Resolve(&g_WinSock_htons,module,s_Wsock32Export_htons);
                    resultOrError = resolveResult.procedureOrError;
                    if (!resolveResult.carry) {
                      resolveResult = DynAPI_Resolve(&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr);
                      resultOrError = resolveResult.procedureOrError;
                      if (!resolveResult.carry) {
                        resolveResult = DynAPI_Resolve(&g_WinSock_inet_ntoa,module,s_Wsock32Export_inet_ntoa
                                              );
                        resultOrError = resolveResult.procedureOrError;
                        if (!resolveResult.carry) {
                          resolveResult = DynAPI_Resolve(&g_WinSock_ioctlsocket,module,
                                                 s_Wsock32Export_ioctlsocket);
                          resultOrError = resolveResult.procedureOrError;
                          if (!resolveResult.carry) {
                            resolveResult = DynAPI_Resolve(&g_WinSock_listen,module,s_Wsock32Export_listen);
                            resultOrError = resolveResult.procedureOrError;
                            if (!resolveResult.carry) {
                              resolveResult = DynAPI_Resolve(&g_WinSock_ntohl,module,s_Wsock32Export_ntohl);
                              resultOrError = resolveResult.procedureOrError;
                              if (!resolveResult.carry) {
                                resolveResult = DynAPI_Resolve(&g_WinSock_ntohs,module,s_Wsock32Export_ntohs
                                                      );
                                resultOrError = resolveResult.procedureOrError;
                                if (!resolveResult.carry) {
                                  resolveResult = DynAPI_Resolve(&g_WinSock_recv,module,s_Wsock32Export_recv
                                                        );
                                  resultOrError = resolveResult.procedureOrError;
                                  if (!resolveResult.carry) {
                                    resolveResult = DynAPI_Resolve(&g_WinSock_recvfrom,module,
                                                           s_Wsock32Export_recvfrom);
                                    resultOrError = resolveResult.procedureOrError;
                                    if (!resolveResult.carry) {
                                      resolveResult = DynAPI_Resolve(&g_WinSock_select,module,
                                                             s_Wsock32Export_select);
                                      resultOrError = resolveResult.procedureOrError;
                                      if (!resolveResult.carry) {
                                        resolveResult = DynAPI_Resolve(&g_WinSock_send,module,
                                                               s_Wsock32Export_send);
                                        resultOrError = resolveResult.procedureOrError;
                                        if (!resolveResult.carry) {
                                          resolveResult = DynAPI_Resolve(&g_WinSock_sendto,module,
                                                                 s_Wsock32Export_sendto);
                                          resultOrError = resolveResult.procedureOrError;
                                          if (!resolveResult.carry) {
                                            resolveResult = DynAPI_Resolve(&g_WinSock_setsockopt,module,
                                                                   s_Wsock32Export_setsockopt);
                                            resultOrError = resolveResult.procedureOrError;
                                            if (!resolveResult.carry) {
                                              resolveResult = DynAPI_Resolve(&g_WinSock_shutdown,module,
                                                                     s_Wsock32Export_shutdown);
                                              resultOrError = resolveResult.procedureOrError;
                                              if (!resolveResult.carry) {
                                                resolveResult = DynAPI_Resolve(&g_WinSock_socket,module,
                                                                       s_Wsock32Export_socket);
                                                resultOrError = resolveResult.procedureOrError;
                                                if (!resolveResult.carry) {
                                                  resolveResult = DynAPI_Resolve(&g_WinSock_gethostbyaddr,
                                                                         module,
                                                  s_Wsock32Export_gethostbyaddr);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_gethostbyname,
                                                                           module,
                                                  s_Wsock32Export_gethostbyname);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_gethostname,
                                                                           module,
                                                  s_Wsock32Export_gethostname);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_getprotobyname
                                                                           ,module,
                                                  s_Wsock32Export_getprotobyname);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_getprotobynumber,module,
                                                  s_Wsock32Export_getprotobynumber);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_getservbyname,
                                                                           module,
                                                  s_Wsock32Export_getservbyname);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_getservbyport,
                                                                           module,
                                                  s_Wsock32Export_getservbyport);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByAddr,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByAddr);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetHostByName,module,
                                                  s_Wsock32Export_WSAAsyncGetHostByName);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByName,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByName);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetProtoByNumber,module,
                                                  s_Wsock32Export_WSAAsyncGetProtoByNumber);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByName,module,
                                                  s_Wsock32Export_WSAAsyncGetServByName);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAAsyncGetServByPort,module,
                                                  s_Wsock32Export_WSAAsyncGetServByPort);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_WSAAsyncSelect
                                                                           ,module,
                                                  s_Wsock32Export_WSAAsyncSelect);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelAsyncRequest,module,
                                                  s_Wsock32Export_WSACancelAsyncRequest);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSACancelBlockingCall,module,
                                                  s_Wsock32Export_WSACancelBlockingCall);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_WSACleanup,
                                                                           module,
                                                  s_Wsock32Export_WSACleanup);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAGetLastError,module,
                                                  s_Wsock32Export_WSAGetLastError);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_WSAIsBlocking,
                                                                           module,
                                                  s_Wsock32Export_WSAIsBlocking);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSASetBlockingHook,module,
                                                  s_Wsock32Export_WSASetBlockingHook);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&g_WinSock_WSAStartup,
                                                                           module,
                                                  s_Wsock32Export_WSAStartup);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resolveResult = DynAPI_Resolve(&
                                                  g_WinSock_WSAUnhookBlockingHook,module,
                                                  s_Wsock32Export_WSAUnhookBlockingHook);
                                                  resultOrError = resolveResult.procedureOrError;
                                                  if (!resolveResult.carry) {
                                                    resultOrError = (HINSTANCE)
                                                             (*g_WinSock_WSAStartup)
                                                                       (0x101,&g_WinSockStartupData)
                                                    ;
                                                    if (resultOrError == (HINSTANCE)0x0) {
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
  return (uint32_t)resultOrError;
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
  NetworkBackendSetSessionEaxCf5 sessionResult;
  
  g_NetworkBackendSessionContext = this;
  sessionResult.carry = false;
  sessionResult.eax = returnValue;
  return sessionResult;
}


/* Address: 0x00585210.
   Ownership: network/backend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
bool __thandor_cf_preserve_eax_ecx_edx NetworkBackend_SelectInstanceByIndex(uint32_t instanceIndex)

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

