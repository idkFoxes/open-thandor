/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/data.h
 */

#ifndef THANDOR_NETWORK_PROTOCOL_DATA_H
#define THANDOR_NETWORK_PROTOCOL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_UiTransferEncryptSboxes[8][16][16]; /* Encryption S-boxes of the UI transfer 64-bit block cipher (UiTransfer_EncryptPacketBlocks): uint32_t[8][16][16], table n (0x400 bytes each) indexed [round-key nibble n][data nibble n], each entry a 4-bit output (each row a permutation of 0..15). */

extern uint32_t g_UiTransferDecryptSboxes[8][16][16]; /* Decryption S-boxes of the UI transfer block cipher: uint32_t[8][16][16], table n indexed [round-key nibble n][data nibble], 4-bit outputs (a separate table set from g_UiTransferEncryptSboxes); used by UiTransfer_DecryptPacketBlocks. */

extern uint32_t g_UiRuntimeRecordWriteIndex;

extern uint32_t g_UiTransferUnitCursor;

extern uint32_t g_UiTransferSequenceToken; /* uint32_t sequence token stamped into outgoing network packets (initial 0x12340000, low 16 bits XORed with a random value in transfer.c; network/protocol/transfer.c, ui/frontend/network.c). */

extern uint32_t g_UiTransferSenderContext;

extern UiTransferMailboxState g_UiTransferMailbox;

extern uint32_t g_UiTransferRoundKeys[16]; /* uint32_t[16] packet cipher round keys (UiTransfer_EncryptPacketBlocks/DecryptPacketBlocks take this as the 16-key table; keys 12..15 are plain key values, although their bytes spell the text "mohTG sakere!!!e"). */

extern UiRuntimeRecord g_UiTransferChunkPacket; /* mailbox chunk packet (0x10031 request / 0x80030 chunk, 0x100 bytes): header (with packetHeader.sequenceToken), payload = chunk offset (payload byte 0), transfer byte count (payload byte 4), chunk data (from payload byte 8, 58 dwords). */

extern FrontendPacket10032HostValue g_UiTransferPingEchoPacket; /* ping answer packet 0x10033 (same layout as the 0x10032 ping): header (with header.sequenceToken), echoed tick (backendSessionValue). */

extern UiTransferMailboxTickCounter g_UiTransferMailboxTickCounter;

extern uint16_t g_GameVersionUtf16[7]; /* version string shown to joining players ("1.5.45") */

extern UiTransferEndpointDescriptor g_FrontendSelectedNetworkEndpoint;

extern uint32_t g_FrontendSessionToken;

extern SessionTransferTimeoutTicks g_SessionTransferTimeoutTicks;

extern uint32_t g_FrontendTransferResponsePending;

extern UiCommandQueueRecord g_FrontendCommandQueueRecords[16];

extern UiCommandQueueRecord *g_FrontendCommandQueueEnd; /* followed by 12 bytes 0x90 fill (dropped) */

extern uint32_t g_FrontendLocalPlayerPcxPreview;

extern FrontendPacket10000Handshake g_FrontendPacket10000Buffer;

extern FrontendPacket50001SessionAdvertisement g_FrontendPacket50001Buffer;

extern FrontendPacket20002PlayerDescriptor g_FrontendPacket20002Buffer;

extern FrontendPacket10003JoinAck g_FrontendPacket10003Buffer;

extern FrontendPacket10004PlayerSnapshotRequest g_FrontendPacket10004Buffer;

extern FrontendPacket10006CapabilityHeartbeat g_FrontendPacket10006Buffer;

extern FrontendPacket40008LobbyRosterSnapshot g_FrontendPacket40008Buffer;

extern FrontendPacket8000ASnapshotChunk g_FrontendPacket8000ABuffer;

extern FrontendCommandPacketRecord g_FrontendPacket10011Buffer;

extern FrontendPacket10013HeartbeatAck g_FrontendPacket10013Buffer;

extern FrontendPacket10032HostValue g_FrontendPacket10032Buffer;

extern uint32_t g_FrontendPendingSessionPlayerCount; /* followed by an all-zero dword no code reaches (dropped) */

extern uint32_t g_FrontendExpectedPlayerRuntimeBlockCount;

extern uint32_t g_FrontendHostPublishRoundRobinCounter;

extern UiCommandQueueRecord g_InGameCommandQueueRecords[16];

extern UiCommandQueueRecord *g_InGameCommandQueueEnd;

extern FrontendCommandPacketRecord g_FrontendClientPlayerCommandRecords[8];

extern FrontendCommandPacketRecord g_FrontendClientCommandBatchPacketBuffer[8];

extern FrontendCommandPacketRecord g_FrontendPacket10021Buffer;

#endif
