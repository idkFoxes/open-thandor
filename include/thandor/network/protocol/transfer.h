#ifndef THANDOR_NETWORK_PROTOCOL_TRANSFER_H
#define THANDOR_NETWORK_PROTOCOL_TRANSFER_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/transfer. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004AEB10 */
void __cdecl UiTransferMailbox_ServiceAndRetransmitTimer(void);

/* 0x0054ECB0 */
void FrontendTransfer_HandleHostSessionAndCommandBatchPackets (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet, FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054F680 */
undefined8 __fastcall FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf (undefined4 param_1,undefined4 param_2,UiTransferEndpointDescriptor *senderEndpoint, FrontendTransferPacketUnion *packet);

/* 0x00545640 */
void FrontendTransfer_MarkUnavailableIfModeBit0Callback(dword arg0,dword arg1,dword arg2,dword arg3);

/* 0x00545660 */
undefined4 FrontendSnapshotTransfer_MarkPlayerHostPublicationReadyAndReleaseWhenAllReady(int param_1);

/* 0x0054E230 */
undefined8 __cdecl UiTransfer_SendPacketType10000Value2931Cf(void);

/* 0x0054E470 */
undefined8 __cdecl UiTransfer_SendPlayerDescriptorPacket20002Cf(void);

/* 0x0054E4E0 */
void FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet, FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054E9B0 */
void FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands (FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EEF0 */
void __fastcall FrontendTransfer_SendPacket10006(undefined4 param_1,undefined4 param_2);

/* 0x005723F0 */
undefined8 FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 sendStateReplies);

/* 0x00572920 */
void __cdecl FrontendTransfer_SendCommandBatchRequest10021(void);

/* 0x004AF110 */
void __cdecl UiTransferMailbox_ClearReceivedState(void);

/* 0x004AF170 */
void * __cdecl UiTransferMailbox_GetReceivedBufferCf(void);

/* 0x004AF1C0 */
void __cdecl UiTransferMailbox_RandomizeSequenceToken(void);

/* 0x0054E260 */
void FrontendTransfer_HandleSessionListAndJoinAckPackets (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet, FrontendRootRuntimeAddress32 frontendRuntime);

/* 0x0054EF30 */
void FrontendTransfer_TickRequestTimeoutAndResetPage(void *frontendRuntime);

/* 0x0054FBA0 */
undefined4 FrontendTransfer_ConsumeProcessedFlagFrontendCf(void);

/* 0x005722C0 */
void FrontendTransfer_HandleSyncRequest10021AndReply10023 (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet);

/* 0x00572560 */
undefined8 __cdecl FrontendTransfer_DispatchStagedCommandRecords(void);

/* 0x00572AA0 */
undefined4 __cdecl FrontendTransfer_ConsumeProcessedFlagCf(void);

/* 0x00407160 */
void UiTransfer_TransformPacketBlocks (dword *roundKeys16,dword *outputBlocks,UiTransferPayloadByteCount byteCount, dword *inputBlocks);

/* 0x004072F0 */
void UiTransferBlock_Transform64BitBlocksWithRoundKeys16 (dword *roundKeys16,void *destination,UiTransferPayloadByteCount byteCount, void *source);

/* 0x004AF140 */
void __cdecl UiTransferMailbox_MarkUnavailable(void);

/* 0x004AF1A0 */
void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation);

/* 0x0054F9A0 */
void __fastcall FrontendTransfer_SendQueued10011AndOptional10004(undefined4 param_1,undefined4 param_2);

/* 0x004AEF70 */
undefined8 UiTransfer_StagePacketAndSendCf (UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet);

#endif /* THANDOR_NETWORK_PROTOCOL_TRANSFER_H */
