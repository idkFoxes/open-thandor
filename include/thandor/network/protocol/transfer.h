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
FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 sendStateReplies);

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
FrontendTransfer_HandleSyncRequest10021AndReply10023
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
