/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/transfer.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_TRANSFER_H
#define THANDOR_NETWORK_PROTOCOL_TRANSFER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/transfer. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* g_UiTransferMailbox.receivedAllocation sentinel published by UiTransferMailbox_MarkUnavailable when a
   requested transfer cannot be served; UiTransferMailbox_GetReceivedBuffer reports it like an empty mailbox. */
#define UI_TRANSFER_MAILBOX_UNAVAILABLE ((void *)0xffffffff)

/* In-game lockstep command exchange. packedTypeAndUnitCount holds the packet type in the low word and the
   number of 0x20-byte units in the high word. Each tick interval the host collects one command record per
   client (COMMAND_SUBMIT), then broadcasts all records as one COMMAND_BATCH; every peer executes the batch.
   A client that still lacks the batch gets it again, a client whose command already arrived gets
   COMMAND_WAIT and answers COMMAND_WAIT_ACK, which only refreshes its timeout on the host. */
#define FRONTEND_PACKET_TYPE_MASK 0xffff
#define FRONTEND_PACKET_UNIT_COUNT_SHIFT 16
#define FRONTEND_PACKET_COMMAND_BATCH_TYPE 0x20             /* host -> clients; unit count = command records */
#define FRONTEND_PACKET_COMMAND_SUBMIT FRONTEND_PACKET_10021_COMMAND_SUBMIT   /* client -> host: its next command record */
#define FRONTEND_PACKET_COMMAND_WAIT FRONTEND_PACKET_10022_COMMAND_WAIT     /* host -> client: command received, batch pending */
#define FRONTEND_PACKET_COMMAND_WAIT_ACK FRONTEND_PACKET_10023_COMMAND_WAIT_ACK /* client -> host: answer to COMMAND_WAIT */
/* Frontend (lobby / session start) command batch from the host: unit count = command records, handled by
   FrontendTransfer_HandleHostSessionAndCommandBatchPackets and FrontendTransfer_HandleGameplayCommandAndRosterPackets. */
#define FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE 0x10
/* High word of every Thandor sequence token (the low word is random, see
   UiTransferMailbox_RandomizeSequenceToken); a host answers only discovery probes that carry it. */
#define FRONTEND_SEQUENCE_TOKEN_HIGH_MASK 0xffff0000
#define FRONTEND_SEQUENCE_TOKEN_HIGH_WORD 0x12340000
/* Player capability bit of the 0x10006 heartbeat and the 0x20002 descriptor (FrontendTransfer_SendCapabilityHeartbeat
   always sets it); the host then shows L"CD" in that player's list row. */
#define FRONTEND_CAPABILITY_CD 0x100
/* Bit 0 of the same word: a 64x64 picture <player name>.pcx was found (UiTransfer_SendPlayerDescriptor). */
#define FRONTEND_DESCRIPTOR_HAS_PICTURE 0x1
/* Reload value of a peer's heartbeatExpiryTicks and of g_SessionTransferTimeoutTicks on every packet. */
#define FRONTEND_PEER_TIMEOUT_TICKS 0x100

/* Mailbox transfer (UiTransferMailbox_ServiceAndRetransmitTimer): data bytes per 0x80030 chunk packet (0x100
   bytes minus the 0x10-byte header and the offset/total-size dwords), and the timer ticks (8 ms each) after
   which an unanswered 0x10031 chunk request is repeated. */
#define UI_TRANSFER_CHUNK_PAYLOAD_BYTES 0xE8
#define UI_TRANSFER_CHUNK_RETRY_TICKS 4
/* Every accepted chunk packet extends the peer's timeout by 0x40 timer ticks (512 ms). */
#define UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS 0x40
/* Lobby timeout: reload value of g_SessionTransferTimeoutTicks on a client (join ack, host session packet) and
   the heartbeat value of the 0x10006 packet. */
#define FRONTEND_LOBBY_TIMEOUT_TICKS 0x40
/* Rows of the network game page's session list (sessionList); further advertised sessions are ignored. */
#define FRONTEND_SESSION_LIST_CAPACITY 0x20
/* Step from one player record's endpoint to the next one's in UiTransferEndpointDescriptor units (0x13B). */
#define FRONTEND_PLAYER_RECORD_ENDPOINT_STRIDE (sizeof(FrontendPlayerRuntimeRecord) / sizeof(UiTransferEndpointDescriptor))
/* protocolMagic of the 0x10000 discovery probe (UiTransfer_SendDiscoveryProbe); a host answers only
   probes carrying it. */
#define FRONTEND_PROTOCOL_MAGIC 0x2931
/* Text resources of the 0x50001 session advertisement (FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets) and
   of the chat-history notice for an arriving player snapshot (0x30005). */
#define TEXT_ID_SESSION_TITLE_TEMPLATE 0x211A        /* selector 0 = game version */
#define TEXT_ID_SESSION_HOST_TEMPLATE 0x211B         /* selector 0 = game name, selector 1 = host player name */
#define TEXT_ID_SESSION_PLAYER_COUNT_TEMPLATE 0x211C /* selector 0 = players, selector 1 = player limit */
#define TEXT_ID_NETWORK_PLAYER_ARRIVED 0xFF03        /* selector 0 = player name */

/* 0x004AEB10 */
void UiTransferMailbox_ServiceAndRetransmitTimer(void);

/* 0x0054ECB0 */
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054F680 */
bool FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg);

/* 0x00545640 */
void FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t senderPlayerId,uint32_t payloadDword0C,uint32_t payloadDword08,uint32_t payloadDword04);

/* 0x00545660 */
void FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t payloadDword0C,uint32_t payloadDword08,uint32_t payloadDword04);

/* 0x0054E230 */
bool UiTransfer_SendDiscoveryProbe(void);

/* 0x0054E470 */
bool UiTransfer_SendPlayerDescriptor(void);

/* 0x0054E4E0 */
void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054E9B0 */
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EEF0 */
void FrontendTransfer_SendCapabilityHeartbeat(void);

/* 0x005723F0 */
bool FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers);

/* 0x00572920 */
void FrontendTransfer_SendCommandSubmit(void);

/* 0x004AF110 */
void UiTransferMailbox_ClearReceivedState(void);

/* 0x004AF170 */
MailboxReceiveResult UiTransferMailbox_GetReceivedBuffer(void);

/* 0x004AF1C0 */
void UiTransferMailbox_RandomizeSequenceToken(void);

/* 0x0054E260 */
void FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EF30 */
void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRoot);

/* 0x0054FBA0 */
bool FrontendTransfer_ConsumeProcessedFlagFrontend(void);

/* 0x005722C0 */
void FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet);

/* 0x00572560 */
void FrontendTransfer_DispatchStagedCommandRecords(void);

/* 0x00572AA0 */
bool FrontendTransfer_ConsumeProcessedFlag(void);

/* 0x00407160 */
void UiTransfer_EncryptPacketBlocks(uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
          uint32_t *inputBlocks);

/* 0x004072F0 */
void UiTransfer_DecryptPacketBlocks
          (uint32_t *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source);

/* 0x004AF140 */
void UiTransferMailbox_MarkUnavailable(void);

/* 0x004AF1A0 */
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation);

/* 0x0054F9A0 */
void FrontendTransfer_SendLobbyCommandAndSnapshotRequest(void);

/* 0x004AEF70 */
bool UiTransfer_StagePacketAndSend(UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet);

#endif /* THANDOR_NETWORK_PROTOCOL_TRANSFER_H */
