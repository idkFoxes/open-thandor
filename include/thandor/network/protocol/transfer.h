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
#define FRONTEND_PACKET_COMMAND_SUBMIT FRONTEND_PACKET_10021   /* client -> host: its next command record */
#define FRONTEND_PACKET_COMMAND_WAIT FRONTEND_PACKET_10022     /* host -> client: command received, batch pending */
#define FRONTEND_PACKET_COMMAND_WAIT_ACK FRONTEND_PACKET_10023 /* client -> host: answer to COMMAND_WAIT */
/* Frontend (lobby / session start) command batch from the host: unit count = command records, handled by
   FrontendTransfer_HandleHostSessionAndCommandBatchPackets and FrontendTransfer_HandleGameplayCommandAndRosterPackets. */
#define FRONTEND_PACKET_LOBBY_COMMAND_BATCH_TYPE 0x10
/* High word of every Thandor sequence token (the low word is random, see
   UiTransferMailbox_RandomizeSequenceToken); a host answers only discovery probes that carry it. */
#define FRONTEND_SEQUENCE_TOKEN_HIGH_MASK 0xffff0000
#define FRONTEND_SEQUENCE_TOKEN_HIGH_WORD 0x12340000
/* Player capability bit of the 0x10006 heartbeat and the 0x20002 descriptor (FrontendTransfer_SendPacket10006
   always sets it); the host then shows L"CD" in that player's list row. */
#define FRONTEND_CAPABILITY_CD 0x100
/* Reload value of a peer's heartbeatExpiryTicks and of g_SessionTransferTimeoutTicks on every packet. */
#define FRONTEND_PEER_TIMEOUT_TICKS 0x100

/* Mailbox transfer (UiTransferMailbox_ServiceAndRetransmitTimer): data bytes per 0x80030 chunk packet (0x100
   bytes minus the 0x10-byte header and the offset/total-size dwords), and the timer ticks (8 ms each) after
   which an unanswered 0x10031 chunk request is repeated. */
#define UI_TRANSFER_CHUNK_PAYLOAD_BYTES 0xE8
#define UI_TRANSFER_CHUNK_RETRY_TICKS 4
/* protocolMagic of the 0x10000 discovery probe (UiTransfer_SendPacketType10000Value2931); a host answers only
   probes carrying it. */
#define FRONTEND_PROTOCOL_MAGIC 0x2931

/* 0x004AEB10 */
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ServiceAndRetransmitTimer(void);

/* 0x0054ECB0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleHostSessionAndCommandBatchPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054F680 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg);

/* 0x00545640 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_MarkUnavailableIfModeBit0Callback(uint32_t arg0,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x00545660 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady
          (int playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x0054E230 */
bool __thandor_cf_preserve_eax_ecx_edx UiTransfer_SendPacketType10000Value2931(void);

/* 0x0054E470 */
bool __thandor_cf_preserve_ecx_edx UiTransfer_SendPlayerDescriptorPacket20002(void);

/* 0x0054E4E0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054E9B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands
          (FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EEF0 */
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendPacket10006(void);

/* 0x005723F0 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers);

/* 0x00572920 */
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendCommandBatchRequest10021(void);

/* 0x004AF110 */
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_ClearReceivedState(void);

/* 0x004AF170 */
MailboxReceiveResult __thandor_eax_ecx_cf_preserve_edx
UiTransferMailbox_GetReceivedBuffer(void);

/* 0x004AF1C0 */
void __thandor_preserve_eax_edx UiTransferMailbox_RandomizeSequenceToken(void);

/* 0x0054E260 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HandleSessionListAndJoinAckPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EF30 */
void __thandor_preserve_eax FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRuntime);

/* 0x0054FBA0 */
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlagFrontend(void);

/* 0x005722C0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet);

/* 0x00572560 */
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_DispatchStagedCommandRecords(void);

/* 0x00572AA0 */
bool __thandor_cf_preserve_eax_ecx_edx FrontendTransfer_ConsumeProcessedFlag(void);

/* 0x00407160 */
void __thandor_void_preserve_eax_ecx_edx
UiTransfer_TransformPacketBlocks
          (uint32_t *roundKeys16,uint32_t *outputBlocks,UiTransferPayloadByteCount byteCount,
          uint32_t *inputBlocks);

/* 0x004072F0 */
void __thandor_void_preserve_eax_ecx_edx
UiTransferBlock_Transform64BitBlocksWithRoundKeys16
          (uint32_t *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount,void *source);

/* 0x004AF140 */
void __thandor_void_preserve_eax_ecx_edx UiTransferMailbox_MarkUnavailable(void);

/* 0x004AF1A0 */
void __thandor_void_preserve_eax_ecx_edx
UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation);

/* 0x0054F9A0 */
void __thandor_void_preserve_eax_ecx_edx FrontendTransfer_SendQueued10011AndOptional10004(void);

/* 0x004AEF70 */
bool __thandor_cf_preserve_eax_ecx_edx
UiTransfer_StagePacketAndSend
          (UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet);

#endif /* THANDOR_NETWORK_PROTOCOL_TRANSFER_H */
