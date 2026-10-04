/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/command_exchange.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_COMMAND_EXCHANGE_H
#define THANDOR_NETWORK_PROTOCOL_COMMAND_EXCHANGE_H

#include <thandor/core/types.h>
#include <thandor/network/backend/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/command_exchange. */

#define FRONTEND_PACKET_COMMAND_BATCH_TYPE 0x20             /* host -> clients; unit count = command records */
#define FRONTEND_PACKET_COMMAND_SUBMIT FRONTEND_PACKET_10021_COMMAND_SUBMIT   /* client -> host: its next command record */
#define FRONTEND_PACKET_COMMAND_WAIT FRONTEND_PACKET_10022_COMMAND_WAIT     /* host -> client: command received, batch pending */
#define FRONTEND_PACKET_COMMAND_WAIT_ACK FRONTEND_PACKET_10023_COMMAND_WAIT_ACK /* client -> host: answer to COMMAND_WAIT */

/* Reload value of a peer's heartbeatExpiryTicks and of g_SessionTransferTimeoutTicks on every packet. */
#define FRONTEND_PEER_TIMEOUT_TICKS 0x100

/* Functions are grouped by semantic ownership. */

Bool8 FrontendTransfer_HandleGameplayCommandAndRosterPackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg);

Bool8 FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(FrontendBooleanState32 notifyWaitingPeers);

void FrontendTransfer_SendCommandSubmit(void);

void FrontendTransfer_HostHandleCommandSubmitOrWaitAck
          (NetworkSessionContext *sourceContext,FrontendTransferPacketUnion *packet);

void FrontendTransfer_DispatchStagedCommandRecords(void);

Bool8 FrontendTransfer_ConsumeProcessedFlag(void);

void FrontendTransfer_CopyCommandRecord
          (FrontendCommandPacketRecord *destination,const FrontendCommandPacketRecord *source);

extern uint32_t g_FrontendTransferResponsePending;
extern uintptr_t g_FrontendLocalPlayerPcxPreview;

extern FrontendCommandPacketRecord g_FrontendClientPlayerCommandRecords[8];
extern FrontendCommandPacketRecord g_FrontendClientCommandBatchPacketBuffer[8];
extern FrontendCommandPacketRecord g_FrontendPacket10021Buffer;

extern FrontendPacket10022StatePending g_FrontendPacket10022Buffer;

#endif /* THANDOR_NETWORK_PROTOCOL_COMMAND_EXCHANGE_H */
