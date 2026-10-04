/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/protocol/lobby.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/protocol/lobby.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(16) int32_t g_FrontendPlayerRuntimeCount = 0;

/* version string shown to joining players ("1.5.45") */
static uint16_t g_GameVersionUtf16[7] = {'1', '.', '5', '.', '4', '5', 0}; /* L"1.5.45" */

static FrontendPacket10000Handshake g_FrontendPacket10000Buffer = {0};

static FrontendPacket50001SessionAdvertisement g_FrontendPacket50001Buffer = {0};

static FrontendPacket20002PlayerDescriptor g_FrontendPacket20002Buffer = {0};

static FrontendPacket10003JoinAck g_FrontendPacket10003Buffer = {0};

static FrontendPacket10004PlayerSnapshotRequest g_FrontendPacket10004Buffer = {0};

static FrontendPacket10006CapabilityHeartbeat g_FrontendPacket10006Buffer = {0};

static FrontendPacket40008LobbyRosterSnapshot g_FrontendPacket40008Buffer = {0};

FrontendCommandPacketRecord g_FrontendPacket10011Buffer = {0};

static FrontendPacket10032HostValue g_FrontendPacket10032Buffer = {0};

static uint32_t g_FrontendHostPublishRoundRobinCounter = 0;

UiTransferEndpointDescriptor g_FrontendSelectedNetworkEndpoint = {0};

uint32_t g_FrontendSessionToken = 0;

SessionTransferTimeoutTicks g_SessionTransferTimeoutTicks = 0;

uint32_t g_FrontendPendingSessionPlayerCount = 0;

uint32_t g_FrontendExpectedPlayerRuntimeBlockCount = 0;

/* Implementation ownership: network/protocol/lobby. */

/* Executes commandCount consecutive 0x20-byte lobby command records (the original executes at least one: a
   count of 0 wraps). Command dword = handler offset << 8 | player id; offsets past the command handlers are
   ignored. */
void FrontendTransfer_ExecuteLobbyCommandRecords
          (const FrontendCommandPacketRecord *commandRecord,uint32_t commandCount)
{
  uint32_t packedCommand;
  uint32_t commandHandlerIndex;
  CommandQueueHandlerProc *commandHandler;

  /* Not in the original: a count of 0 (which wraps) or one past the 0x100-byte receive slot executes nothing.
     The receive check (UiTransferMailbox_DecryptAndVerifyRecord) already drops such packets; valid batches
     have 1..8 records. */
  if (commandCount == 0 || commandCount > FRONTEND_PACKET_MAX_UNIT_COUNT) {
    return;
  }
  do {
    packedCommand = commandRecord->command.packedCommandAndPlayerId;
    commandHandlerIndex = packedCommand >> 8;
    if (commandHandlerIndex != 0) {
      commandHandler = CommandDispatch_ResolveHandler
                                 (FRONTEND_COMMAND_CODE_BASE,FRONTEND_COMMAND_HANDLER_REGION_END,commandHandlerIndex);
      if (commandHandler != NULL) {
        (*commandHandler)
                  (packedCommand & 0xff,commandRecord->command.payload1,commandRecord->command.payload2,
                   commandRecord->command.payload3);
      }
    }
    commandRecord++;
    commandCount--;
  } while (commandCount != 0);
}

/* Client side of the host lobby: accepts the host's 0x40008 session packet (one player-list row and the
   player's name; a non-zero expected block count starts the session and switches to
   FRONTEND_NETWORK_STATE_CLIENT_STARTING) and the host's lobby command batches, whose commands it executes
   before answering with its own next queued command (packet 0x10011). Packets from other hosts or sessions
   are ignored.
*/
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int dwordCount;
  int playerIndex;
  uint32_t playerCount;
  uint32_t *packetCursor;
  uint32_t *nameSourceCursor;
  uint32_t *nameDestinationCursor;
  uint32_t *playerRowCursor;
  FrontendPlayerRuntimeRecord *playerRecord;

  if ((packet->packet40008LobbyRosterSnapshot.header.packedTypeAndUnitCount ==
       FRONTEND_PACKET_40008_SESSION_PLAYER_ROW) &&
      (g_FrontendSessionToken == packet->packet40008LobbyRosterSnapshot.header.sequenceToken) &&
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    playerCount = packet->packet40008LobbyRosterSnapshot.playerCount;
    if ((packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex < 8) && (playerCount < 9)) {
      packetCursor = (uint32_t *)packet;
      playerRowCursor = (uint32_t *)g_FrontendPlayerListRows
                        [packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex];
      g_FrontendPlayerRuntimeCount = playerCount;
      /* the whole 0x80-byte packet becomes the player's list row */
      for (dwordCount = 32; dwordCount != 0; dwordCount--) {
        *playerRowCursor = *packetCursor;
        packetCursor++;
        playerRowCursor++;
      }
      playerRecord = g_FrontendPlayerRuntimeBlocks +
               packet->packet40008LobbyRosterSnapshot.selectedPlayerIndex;
      playerRecord->playerRuntimeId = packet->packet40008LobbyRosterSnapshot.selectedPlayerRuntimeId;
      /* the player's name (playerName.textUtf16) */
      nameSourceCursor = packet->packet40008LobbyRosterSnapshot.playerDescriptorPayload;
      nameDestinationCursor = (uint32_t *)playerRecord->playerName.textUtf16;
      for (dwordCount = 10; dwordCount != 0; dwordCount--) {
        *nameDestinationCursor = *nameSourceCursor;
        nameSourceCursor++;
        nameDestinationCursor++;
      }
      UiPointerList_InitializeColumnLayout
                (playerCount,(Ptr32<void> *)g_FrontendPlayerListRows,
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
      playerRecord = g_FrontendPlayerRuntimeBlocks;
      for (playerIndex = 0; playerIndex < 8; playerIndex++) {
        playerRecord->factionAssignment.roleStateFlags = 0;
        playerRecord->colourCycleFlags = 0;
        playerRecord->snapshotTransferFlags = 0;
        playerRecord++;
      }
    }
    return;
  }
  if (((packet->packet10000Handshake.header.packedTypeAndUnitCount & FRONTEND_PACKET_TYPE_MASK) ==
       FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE) &&
      (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken) &&
      (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    FrontendTransfer_ExecuteLobbyCommandRecords
              (&packet->command10011Or10021,
               packet->packet10000Handshake.header.packedTypeAndUnitCount >> FRONTEND_PACKET_UNIT_COUNT_SHIFT);
    g_FrontendPacket10011Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10011_LOBBY_COMMAND;
    FrontendCommandQueue_DequeueFirstIntoRecord(&g_FrontendPacket10011Buffer);
    if (g_FrontendPacket10011Buffer.command.packedCommandAndPlayerId != 0) {
      UiTransfer_StagePacketAndSend
                (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket10011Buffer.header);
    }
  }
}

/* Frontend command handler FRONTEND_COMMAND_MARK_TRANSFER_UNAVAILABLE, queued by
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

/* Frontend command handler 0x1710 (relative to FRONTEND_COMMAND_CODE_BASE), queued by a client in
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

/* Sends the session discovery probe (0x10000 handshake with FRONTEND_PROTOCOL_MAGIC) to
   g_FrontendNetworkEndpointScratch, the address from the join dialog or the broadcast address. Hosts answer
   with a 0x50001 session advertisement. Returns the result of UiTransfer_StagePacketAndSend.
*/
Bool8 UiTransfer_SendDiscoveryProbe(void)

{
  Bool8 sendCarry;
  
  g_FrontendPacket10000Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10000_HANDSHAKE;
  g_FrontendPacket10000Buffer.protocolMagic = FRONTEND_PROTOCOL_MAGIC;
  sendCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendNetworkEndpointScratch,&g_FrontendPacket10000Buffer.header);
  return sendCarry;
}

/* Introduces the local player to the host (0x20002 player descriptor): the player name (20 UTF-16 units)
   whose last unit is replaced by flags: bit 0 = a 64x64 picture <name>.pcx was found (loaded into
   g_FrontendLocalPlayerPcxPreview), bit 8 = shown as "CD" in the lobby list (always set). Returns the
   result of UiTransfer_StagePacketAndSend.
*/
Bool8 UiTransfer_SendPlayerDescriptor(void)

{
  int dwordCount;
  uint32_t *nameSourceCursor;
  uint32_t *payloadCursor;
  Bool8 callCarry;
  
  g_FrontendPacket20002Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR;
  g_FrontendPacket20002Buffer.payloadByteCount = 64;
  nameSourceCursor = THANDOR_PTR(g_FrontendLocalPlayerNameUtf16);
  payloadCursor = g_FrontendPacket20002Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *payloadCursor = *nameSourceCursor;
    nameSourceCursor++;
    payloadCursor++;
  }
  /* the last name unit becomes the flags word */
  ((uint16_t *)payloadCursor)[-1] = 0;
  callCarry = PcxPreview_Load64x64PaletteAndPixels
                    ((PcxPreview64 *)g_FrontendLocalPlayerPcxPreview,g_FrontendLocalPlayerNameUtf16);
  if (!callCarry) {
    ((uint16_t *)payloadCursor)[-1] |= FRONTEND_DESCRIPTOR_HAS_PICTURE;
  }
  ((uint16_t *)payloadCursor)[-1] |= FRONTEND_CAPABILITY_CD;
  callCarry = UiTransfer_StagePacketAndSend
                    (&g_FrontendSelectedNetworkEndpoint,&g_FrontendPacket20002Buffer.header);
  return callCarry;
}

/* Host: answers a discovery probe (0x10000) with the session advertisement 0x50001 (title, host description,
   player count); joinable while the lobby is not full. */
static void FrontendTransfer_SendSessionAdvertisement
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList,UiRangeSliderControl *maxPlayersSlider)
{
  FrontendRootRuntimeAddress32 frontendRootNode;
  uint16_t *resolvedText;

  /* the game name comes from the frontend root node, not from the handler's frontendRuntime */
  frontendRootNode = g_FrontendRootNode;
  g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_UNAVAILABLE;
  if ((packet->packet10000Handshake.protocolMagic == FRONTEND_PROTOCOL_MAGIC) &&
      ((packet->packet10000Handshake.header.sequenceToken & FRONTEND_SEQUENCE_TOKEN_HIGH_MASK) ==
       FRONTEND_SEQUENCE_TOKEN_HIGH_WORD) &&
      (hostLobbyPlayerList->rowCount < (uint32_t)maxPlayersSlider->value)) {
    g_FrontendPacket50001Buffer.joinAvailableFlag = UI_TRANSFER_JOIN_AVAILABLE;
  }
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_TITLE_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,g_GameVersionUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded
            (40,g_FrontendPacket50001Buffer.sessionTitleUtf16,resolvedText,NULL);
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_HOST_TEMPLATE);
  /* the game name typed into gameNameEdit */
  RichTextCommandStream_PatchPayloadBySelector
            (0,((UiTextEditControl *)FRONTEND_UI(frontendRootNode,gameNameEdit))->textBuffer,resolvedText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendLocalPlayerNameUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded
            (88,g_FrontendPacket50001Buffer.hostDescriptionUtf16,resolvedText,NULL);
  resolvedText = TextResource_Resolve(TEXT_ID_SESSION_PLAYER_COUNT_TEMPLATE);
  RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendNetworkRuntimeCountTextUtf16,resolvedText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FrontendNetworkPlayerCountTextUtf16,resolvedText);
  RichTextCommandStream_CopyExpanded(8,g_FrontendPacket50001Buffer.playerCountTextUtf16,resolvedText,NULL);
  g_FrontendPacket50001Buffer.header.packedTypeAndUnitCount =
       FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT;
  g_FrontendPacket50001Buffer.payloadByteCount = 32;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket50001Buffer.header);
}

/* Smallest player runtime id below 0xFF that no current player uses (0xFF when all are taken). */
static uint32_t FrontendTransfer_FindLowestFreePlayerRuntimeId(void)
{
  uint32_t candidateId;
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  candidateId = 0;
  do {
    /* scan every player; restart with the next candidate as soon as one uses it */
    playersRemaining = g_FrontendPlayerRuntimeCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    while (candidateId != (uint32_t)playerRecord->playerRuntimeId) {
      playersRemaining--;
      playerRecord++;
      if (playersRemaining == 0) {
        return candidateId;
      }
    }
    candidateId++;
  } while (candidateId < 255);
  return candidateId;
}

/* Host: admits a joining player (0x20002 player descriptor): a new player-list row, the lowest free player id
   and the join ack 0x10003. */
static void FrontendTransfer_AdmitJoiningPlayer
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList)
{
  uint16_t descriptorStatusBits;
  uint32_t assignedPlayerRuntimeId;
  int dwordCount;
  const uint32_t *packetDwords;
  const uint32_t *endpointDwords;
  uint32_t *joiningPlayerRecordDwordCursor;

  joiningPlayerRecordDwordCursor = (uint32_t *)hostLobbyPlayerList->rowSlots[hostLobbyPlayerList->rowCount];
  hostLobbyPlayerList->rowCount = hostLobbyPlayerList->rowCount + 1;
  /* the new row: the 0x40-byte descriptor packet followed by the sender's 0x10-byte endpoint */
  packetDwords = (const uint32_t *)packet;
  for (dwordCount = 16; dwordCount != 0; dwordCount--) {
    *joiningPlayerRecordDwordCursor = *packetDwords;
    packetDwords++;
    joiningPlayerRecordDwordCursor++;
  }
  endpointDwords = (const uint32_t *)senderEndpoint;
  for (dwordCount = 4; dwordCount != 0; dwordCount--) {
    *joiningPlayerRecordDwordCursor = *endpointDwords;
    endpointDwords++;
    joiningPlayerRecordDwordCursor++;
  }
  assignedPlayerRuntimeId = FrontendTransfer_FindLowestFreePlayerRuntimeId();
  /* the cursor now points just past the copied endpoint; the fields below are addressed relative to it */
  descriptorStatusBits = *(uint16_t *)((uint8_t *)joiningPlayerRecordDwordCursor - 18);
  *joiningPlayerRecordDwordCursor = 1;
  joiningPlayerRecordDwordCursor[-15] = assignedPlayerRuntimeId;
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
  *(uint16_t *)((uint8_t *)joiningPlayerRecordDwordCursor - 18) = 0;
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)hostLobbyPlayerList->rowCount,
             g_FrontendNetworkRuntimeCountTextUtf16);
  g_FrontendPacket10003Buffer.networkTickInterval = g_SessionNetworkTickInterval;
  g_FrontendPacket10003Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10003_JOIN_ACK;
  g_FrontendPacket10003Buffer.assignedPlayerRuntimeId = assignedPlayerRuntimeId;
  UiTransfer_StagePacketAndSend(senderEndpoint,&g_FrontendPacket10003Buffer.header);
  g_FrontendPlayerRuntimeCount++;
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
}

/* Host: stores a player's capability heartbeat (0x10006) and its "CD" label. */
static void FrontendTransfer_StoreCapabilityHeartbeat
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *hostLobbyPlayerList)
{
  FrontendCapabilityFlags capabilityFlags;
  FrontendHeartbeatTickCount heartbeatExpiryTicks;
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;

  /* the first record is compared before the count is checked */
  playersRemaining = (int)hostLobbyPlayerList->rowCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken) ||
         (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)) {
    playerRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  capabilityFlags = packet->packet10006CapabilityHeartbeat.capabilityFlags;
  heartbeatExpiryTicks = packet->packet10006CapabilityHeartbeat.heartbeatExpiryTicks;
  playerRecord->capabilityFlags = capabilityFlags;
  playerRecord->heartbeatExpiryTicks = heartbeatExpiryTicks;
  playerRecord->capabilityLabelUtf16[0] = 0;
  playerRecord->capabilityLabelUtf16[1] = 0;
  playerRecord->capabilityLabelUtf16[2] = 0;
  playerRecord->capabilityLabelUtf16[3] = 0;
  if ((capabilityFlags & FRONTEND_CAPABILITY_CD) != 0) {
    /* L"CD" */
    playerRecord->capabilityLabelUtf16[0] = 'C';
    playerRecord->capabilityLabelUtf16[1] = 0;
    playerRecord->capabilityLabelUtf16[2] = 'D';
    playerRecord->capabilityLabelUtf16[3] = 0;
  }
  FrontendPlayerRuntime_UpdateStartButtonByCdShare();
}

/* Host: adds its own next queued command (slot 0) to the commands collected from the players, compacts the
   non-empty slots into one lobby command batch (clearing them; the player id stays), sends it to every player
   but the host (record 0) and executes it. Nothing is sent when no slot holds a command. */
static void FrontendTransfer_BroadcastAndExecuteLobbyCommands(void)
{
  uint32_t commandCount;
  int slotsRemaining;
  int peersRemaining;
  FrontendCommandPacketRecord *commandRecord;
  FrontendCommandPacketRecord *batchCursor;
  UiTransferEndpointDescriptor *peerEndpointCursor;

  FrontendCommandQueue_DequeueFirstIntoRecord(g_FrontendPlayerCommandRecords);
  commandCount = 0;
  commandRecord = g_FrontendPlayerCommandRecords;
  batchCursor = g_FrontendCommandBatchPacketBuffer;
  slotsRemaining = g_FrontendPlayerRuntimeCount;
  do {
    if ((commandRecord->command.packedCommandAndPlayerId & 0xffffff00) != 0) {
      FrontendTransfer_CopyCommandRecord(batchCursor,commandRecord);
      batchCursor++;
      commandCount++;
      commandRecord->command.packedCommandAndPlayerId = commandRecord->command.packedCommandAndPlayerId & 0xff;
    }
    commandRecord++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
  if (commandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT == 0) {
    return;
  }
  /* the first packed record's header doubles as the batch header */
  g_FrontendCommandBatchPacketBuffer[0].header.packedTypeAndUnitCount =
       commandCount << FRONTEND_PACKET_UNIT_COUNT_SHIFT | FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE;
  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  for (peersRemaining = g_FrontendPlayerRuntimeCount - 1; peersRemaining != 0; peersRemaining--) {
    UiTransfer_StagePacketAndSend(peerEndpointCursor,&g_FrontendCommandBatchPacketBuffer[0].header);
    peerEndpointCursor = peerEndpointCursor + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
  FrontendTransfer_ExecuteLobbyCommandRecords(g_FrontendCommandBatchPacketBuffer,commandCount & 0xffff);
}

/* Host: keeps a player's next command (0x10011) in its command slot and marks it pending, then broadcasts
   and executes the collected commands. Packets from unknown senders are ignored. */
static void FrontendTransfer_CollectLobbyCommand
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet)
{
  int playersRemaining;
  FrontendPlayerRuntimeRecord *playerRecord;
  FrontendCommandPacketRecord *commandRecord;

  /* the command slots run parallel to the player records; the first record is compared before the count is
     checked */
  commandRecord = g_FrontendPlayerCommandRecords;
  playersRemaining = g_FrontendPlayerRuntimeCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  while ((packet->packet10000Handshake.header.sequenceToken != playerRecord->peerSequenceToken) ||
         (senderEndpoint->ipv4AddressNetworkOrder != playerRecord->endpoint.ipv4AddressNetworkOrder)) {
    playerRecord++;
    commandRecord++;
    playersRemaining--;
    if (playersRemaining == 0) {
      return;
    }
  }
  playerRecord->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
  /* keep the player's 0x20-byte command packet; the host's own command goes into slot 0 */
  FrontendTransfer_CopyCommandRecord(commandRecord,&packet->command10011Or10021);
  FrontendTransfer_BroadcastAndExecuteLobbyCommands();
}

/* Host side of the lobby. Answers a discovery probe (0x10000) with the session advertisement (0x50001:
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
  UiPointerListControl *hostLobbyPlayerList;
  UiRangeSliderControl *maxPlayersSlider;

  hostLobbyPlayerList = (UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList);
  maxPlayersSlider = (UiRangeSliderControl *)FRONTEND_UI(frontendRuntime,maxPlayersSlider);
  if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10000_HANDSHAKE) {
    FrontendTransfer_SendSessionAdvertisement(senderEndpoint,packet,hostLobbyPlayerList,maxPlayersSlider);
  }
  else if ((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_20002_PLAYER_DESCRIPTOR) &&
           ((uint32_t)maxPlayersSlider->value > hostLobbyPlayerList->rowCount)) {
    FrontendTransfer_AdmitJoiningPlayer(senderEndpoint,packet,hostLobbyPlayerList);
  }
  else if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10006_CAPABILITY_HEARTBEAT) {
    FrontendTransfer_StoreCapabilityHeartbeat(senderEndpoint,packet,hostLobbyPlayerList);
  }
  else if (packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10011_LOBBY_COMMAND) {
    FrontendTransfer_CollectLobbyCommand(senderEndpoint,packet);
  }
  /* a player descriptor while the lobby is full and every other packet are ignored */
}

/* Host lobby tick, first part: sends every joined player (peerCount = rowCount - 1, all but the host) the
   session packet 0x40008 for the player-list row chosen round robin, plus a 0x10032 tick stamp. */
static void FrontendTransfer_SendSessionPlayerRowToPeers(uint32_t roundRobinCounter,uint32_t rowCount,int peerCount)
{
  uint32_t selectedIndex;
  uint32_t textByteCount;
  int dwordCount;
  UiTransferEndpointDescriptor *peerEndpointCursor;
  UiTransferEndpointDescriptor *endpoint;
  uint8_t *descriptorSourceCursor;
  uint32_t *descriptorDestinationCursor;

  peerEndpointCursor = &g_FrontendPlayerRuntimeBlocks[1].endpoint;
  g_FrontendPacket40008Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_40008_SESSION_PLAYER_ROW;
  g_FrontendPacket40008Buffer.pendingSessionPlayerCount = g_FrontendPendingSessionPlayerCount;
  g_FrontendHostPublishRoundRobinCounter++;
  selectedIndex = roundRobinCounter % rowCount;
  /* the selected player's record fields, addressed from the record-1 endpoint in 0x10-byte steps: the unit at
     heartbeatExpiryTicks gives playerRuntimeId (+4) and playerName (+8), the one at transferProgressBytes
     capabilityLabelUtf16 (+8), and pingRoundTripTicks is read directly */
  g_FrontendPacket40008Buffer.selectedPlayerRuntimeId =
       peerEndpointCursor[(int32_t)((selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks))].ipv4AddressNetworkOrder;
  g_FrontendPacket40008Buffer.selectedStatusCode0 =
       *(FrontendStatusCode *)peerEndpointCursor[(int32_t)((selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes))].zeroPadding;
  g_FrontendPacket40008Buffer.selectedStatusCode1 =
       *(FrontendStatusCode *)(peerEndpointCursor[(int32_t)((selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(transferProgressBytes))].zeroPadding + 4);
  g_FrontendPacket40008Buffer.selectedPlayerIndex = selectedIndex;
  g_FrontendPacket40008Buffer.playerCount = rowCount;
  endpoint = peerEndpointCursor;
  textByteCount = g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,
                     peerEndpointCursor[(int32_t)((selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(pingRoundTripTicks))].addressHeader.packedFamilyAndPort << 2,
                     g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16);
  *(uint32_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + textByteCount) =
       ('s' << 16 | 'm'); /* L"ms" */
  *(uint16_t *)((uint8_t *)g_FrontendPacket40008Buffer.selectedPlayerStatusTextUtf16 + textByteCount + 4) = 0;
  descriptorSourceCursor = peerEndpointCursor[(int32_t)((selectedIndex - 1) * FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE +
           FRONTEND_PLAYER_RECORD_ENDPOINT_UNITS_TO(heartbeatExpiryTicks))].zeroPadding;
  descriptorDestinationCursor = g_FrontendPacket40008Buffer.playerDescriptorPayload;
  for (dwordCount = 10; dwordCount != 0; dwordCount--) {
    *descriptorDestinationCursor = *(uint32_t *)descriptorSourceCursor;
    descriptorSourceCursor = descriptorSourceCursor + 4;
    descriptorDestinationCursor++;
  }
  g_FrontendPacket10032Buffer.header.packedTypeAndUnitCount = FRONTEND_PACKET_10032_PING;
  g_FrontendPacket10032Buffer.backendSessionValue = g_UiTransferMailboxTickCounter;
  /* to every player but the host (record 0) */
  for (; peerCount != 0; peerCount--) {
    UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket40008Buffer.header);
    UiTransfer_StagePacketAndSend(endpoint,&g_FrontendPacket10032Buffer.header);
    endpoint = endpoint + FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE;
  }
}

/* Host lobby tick. Sends every joined player the session packet 0x40008 for one player-list row, chosen
   round robin (with its row, name and ping text "<n>ms"), plus a 0x10032 tick stamp; a pending session start
   switches to FRONTEND_NETWORK_STATE_HOST_STARTING. Then it adds the host's own next queued command to the
   players' collected ones, broadcasts the non-empty ones as one lobby command batch and executes them.
*/
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(FrontendRootRuntimeAddress32 frontendRuntime)

{
  uint32_t roundRobinCounter;
  uint32_t rowCount;
  int peerCount;

  roundRobinCounter = g_FrontendHostPublishRoundRobinCounter;
  rowCount = ((UiPointerListControl *)FRONTEND_UI(frontendRuntime,hostLobbyPlayerList))->rowCount;
  peerCount = rowCount - 1;
  if (peerCount != 0 && 0 < (int)rowCount) {
    FrontendTransfer_SendSessionPlayerRowToPeers(roundRobinCounter,rowCount,peerCount);
  }
  if (g_FrontendPendingSessionPlayerCount != 0) {
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_HOST_STARTING;
    g_FrontendPendingSessionPlayerCount = 0;
  }
  FrontendTransfer_BroadcastAndExecuteLobbyCommands();
}

/* Sends the client's capability heartbeat (0x10006) to the selected host: the CD capability and a heartbeat
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

/* Session advertisement (0x50001): updates the known session (same sequence token and IPv4 address) in place,
   or appends it while the list has fewer than FRONTEND_SESSION_LIST_CAPACITY rows. */
static void FrontendTransfer_StoreSessionAdvertisement
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          UiPointerListControl *sessionList)
{
  int sessionsRemaining;
  int dwordCount;
  FrontendSessionDiscoveryRecord *discoveryRecord;
  Ptr32<FrontendSessionDiscoveryRecord> *sessionRowCursor;
  uint32_t *recordDwordCursor;
  const uint32_t *sourceDwords;

  discoveryRecord = g_FrontendSessionDiscoveryRecords;
  sessionRowCursor = g_FrontendSessionListRows;
  for (sessionsRemaining = (int)sessionList->rowCount; sessionsRemaining != 0; sessionsRemaining--) {
    if ((packet->packet10000Handshake.header.sequenceToken == discoveryRecord->advertisement.header.sequenceToken) &&
        (senderEndpoint->ipv4AddressNetworkOrder == discoveryRecord->senderEndpoint.ipv4AddressNetworkOrder)) {
      break;
    }
    sessionRowCursor++;
    discoveryRecord++;
  }
  if ((sessionsRemaining == 0) && (sessionList->rowCount >= FRONTEND_SESSION_LIST_CAPACITY)) {
    return; /* unknown session and the list is full */
  }
  if (sessionsRemaining == 0) {
    /* append: the row after the last one */
    *sessionRowCursor = discoveryRecord;
    sessionList->rowCount = sessionList->rowCount + 1;
  }
  /* the payload byte count is overwritten with 0x20 before the packet is stored */
  packet->packet50001SessionAdvertisement.payloadByteCount = 32;
  /* the 0xA0-byte advertisement followed by the sender's 0x10-byte endpoint */
  recordDwordCursor = (uint32_t *)discoveryRecord;
  sourceDwords = (const uint32_t *)packet;
  for (dwordCount = 40; dwordCount != 0; dwordCount--) {
    *recordDwordCursor = *sourceDwords;
    sourceDwords++;
    recordDwordCursor++;
  }
  sourceDwords = (const uint32_t *)senderEndpoint;
  for (dwordCount = 4; dwordCount != 0; dwordCount--) {
    *recordDwordCursor = *sourceDwords;
    sourceDwords++;
    recordDwordCursor++;
  }
  UiPointerList_RefreshSelectionAndQueueAction(sessionList);
}

/* Network game page (browsing): a session advertisement (0x50001) updates its row in the session list or
   appends one (at most 0x20 sessions); the join ack (0x10003) from the selected host takes over the assigned
   player id and network tick interval, switches to the host-lobby page and FRONTEND_NETWORK_STATE_JOINED and
   marks this machine as a network client.
*/
void FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime)

{
  int dwordCount;
  uint32_t *playerRowCursor;

  if (packet->packet10000Handshake.header.packedTypeAndUnitCount ==
      FRONTEND_PACKET_50001_SESSION_ADVERTISEMENT) {
    FrontendTransfer_StoreSessionAdvertisement
              (senderEndpoint,packet,(UiPointerListControl *)FRONTEND_UI(frontendRuntime,sessionList));
  }
  else if ((packet->packet10000Handshake.header.packedTypeAndUnitCount == FRONTEND_PACKET_10003_JOIN_ACK) &&
           (g_FrontendSessionToken == packet->packet10000Handshake.header.sequenceToken) &&
           (g_FrontendSelectedNetworkEndpoint.ipv4AddressNetworkOrder == senderEndpoint->ipv4AddressNetworkOrder)) {
    g_LocalPlayerRuntimeId = packet->packet10003JoinAck.assignedPlayerRuntimeId;
    g_SessionNetworkTickInterval = packet->packet10003JoinAck.networkTickInterval;
    UiPageStack_SetActiveIndex(FRONTEND_PAGE_CLIENT_LOBBY,(UiPageStackControl *)FRONTEND_UI(frontendRuntime,frontendPageStack));
    g_FrontendNetworkState = FRONTEND_NETWORK_STATE_JOINED;
    g_SessionTransferTimeoutTicks = FRONTEND_LOBBY_TIMEOUT_TICKS;
    g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags | SESSION_NETWORK_ROLE_CLIENT;
    playerRowCursor = (uint32_t *)g_FrontendPlayerListRows[0];
    for (dwordCount = 256; dwordCount != 0; dwordCount--) {
      *playerRowCursor = 0;
      playerRowCursor++;
    }
    UiPointerList_InitializeColumnLayout
              (0,(Ptr32<void> *)g_FrontendPlayerListRows,
               (UiPointerListControl *)FRONTEND_UI(frontendRuntime,clientLobbyPlayerList));
  }
}

/* Host timeout of a client in the host's lobby, called by FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState
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

/* Frontend copy of FrontendTransfer_ConsumeProcessedFlag: atomically takes and clears
   g_FrontendTransferResponsePending (set by FrontendTransfer_HandleGameplayCommandAndRosterPackets after a new
   command batch). Returns true when no batch arrived, so Frontend_StateTick ends its tick early.
*/
Bool8 FrontendTransfer_ConsumeProcessedFlagForMenuTick(void)

{
  int previousFlag;

  /* atomic exchange: the flag is set and consumed on both the main and the timer thread */
  previousFlag = (int)THANDOR_ATOMIC_EXCHANGE(&g_FrontendTransferResponsePending,0);
  return previousFlag == 0;
}

/* Client answer to the host while the session starts and after each lobby command batch: sends the oldest
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
