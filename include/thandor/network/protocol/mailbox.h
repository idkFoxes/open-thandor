/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/mailbox.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_MAILBOX_H
#define THANDOR_NETWORK_PROTOCOL_MAILBOX_H

#include <thandor/core/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/protocol/mailbox. */

/* g_UiTransferMailbox.receivedAllocation sentinel published by UiTransferMailbox_MarkUnavailable when a
   requested transfer cannot be served; UiTransferMailbox_GetReceivedBuffer reports it like an empty mailbox. */
#define UI_TRANSFER_MAILBOX_UNAVAILABLE ((void *)(intptr_t)-1) /* all bits set, as 0xffffffff in the original */

/* In-game lockstep command exchange. packedTypeAndUnitCount holds the packet type in the low word and the
   number of 0x20-byte units in the high word. Each tick interval the host collects one command record per
   client (COMMAND_SUBMIT), then broadcasts all records as one COMMAND_BATCH; every peer executes the batch.
   A client that still lacks the batch gets it again, a client whose command already arrived gets
   COMMAND_WAIT and answers COMMAND_WAIT_ACK, which only refreshes its timeout on the host. */
#define FRONTEND_PACKET_TYPE_MASK 0xffff
#define FRONTEND_PACKET_UNIT_COUNT_SHIFT 16
/* Most 0x20-byte units a received packet can have: the 0x100-byte receive ring slot (and receive buffer). */
#define FRONTEND_PACKET_MAX_UNIT_COUNT 8

/* Mailbox transfer (UiTransferMailbox_ServiceAndRetransmitTimer): data bytes per 0x80030 chunk packet (0x100
   bytes minus the 0x10-byte header and the offset/total-size dwords), and the timer ticks (8 ms each) after
   which an unanswered 0x10031 chunk request is repeated. */
#define UI_TRANSFER_CHUNK_PAYLOAD_BYTES 0xE8
#define UI_TRANSFER_CHUNK_RETRY_TICKS 4
/* Every accepted chunk packet extends the peer's timeout by 0x40 timer ticks (512 ms). */
#define UI_TRANSFER_CHUNK_TIMEOUT_EXTENSION_TICKS 0x40
/* Byte 0 of the chunk packet header, which is written byte by byte: the low byte of the packet type (byte 1 is
   the zero high byte, bytes 2..3 the unit count: 1 for the request, 8 for the chunk). */
#define UI_TRANSFER_CHUNK_REQUEST_TYPE_BYTE ((uint8_t)FRONTEND_PACKET_10031_MAILBOX_CHUNK_REQUEST) /* 0x31 */
#define UI_TRANSFER_CHUNK_TYPE_BYTE ((uint8_t)FRONTEND_PACKET_80030_MAILBOX_CHUNK)                /* 0x30 */

/* protocolMagic of the 0x10000 discovery probe (UiTransfer_SendDiscoveryProbe); a host answers only
   probes carrying it. */
#define FRONTEND_PROTOCOL_MAGIC 0x2931

/* Functions are grouped by semantic ownership. */

void UiTransferMailbox_ServiceAndRetransmitTimer(void);

void UiTransferMailbox_ClearReceivedState(void);

void *UiTransferMailbox_GetReceivedBuffer(uint32_t *outByteCount); /* NULL while nothing complete */

void UiTransferMailbox_RandomizeSequenceToken(void);

void UiTransferMailbox_MarkUnavailable(void);

void UiTransferMailbox_SetOutgoingBuffer(UiTransferPayloadByteCount byteCount,void *allocation);

Bool8 UiTransfer_StagePacketAndSend(UiTransferEndpointDescriptor *endpoint,UiTransferPacketHeader *packet);

extern uint32_t g_UiRuntimeRecordWriteIndex;
extern uint32_t g_UiTransferUnitCursor;
extern uint32_t g_UiTransferSequenceToken; /* uint32_t sequence token stamped into outgoing network packets (initial 0x12340000, low 16 bits XORed with a random value in transfer.c; network/protocol/transfer.c, ui/frontend/network.c). */
extern uint32_t g_UiTransferSenderContext;
extern UiTransferMailboxState g_UiTransferMailbox;
extern UiTransferMailboxTickCounter g_UiTransferMailboxTickCounter;

#endif /* THANDOR_NETWORK_PROTOCOL_MAILBOX_H */
