/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/data.h
 */

#ifndef THANDOR_NETWORK_PROTOCOL_DATA_H
#define THANDOR_NETWORK_PROTOCOL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint32_t g_UiTransferEncryptSboxes[8][16][16]; /* 00403160 g_UiTransferEncryptSboxes: Encryption S-boxes of the UI transfer 64-bit block cipher (UiTransfer_EncryptPacketBlocks): uint32_t[8][16][16], table n (0x400 bytes each, 0x00403160-0x00405160) indexed [round-key nibble n][data nibble n], each entry a 4-bit output (each row a permutation of 0..15). k_SpatialSoundStereoCosineSecondHalfBaseBias (0x004046A0) is only an address inside table 5 (+0x140). */

extern uint32_t g_UiTransferDecryptSboxes[8][16][16]; /* 00405160 g_UiTransferDecryptSboxes: Decryption S-boxes of the UI transfer block cipher: uint32_t[8][16][16], table n indexed [round-key nibble n][data nibble], 4-bit outputs (a separate table set from g_UiTransferEncryptSboxes); used by UiTransfer_DecryptPacketBlocks. */

extern uint32_t g_UiRuntimeRecordWriteIndex; /* 004AE974 g_UiRuntimeRecordWriteIndex */

extern uint32_t g_UiTransferUnitCursor; /* 004AE978 g_UiTransferUnitCursor */

extern uint32_t g_UiTransferSequenceToken; /* 004AE97C g_UiTransferSequenceToken: uint32_t sequence token stamped into outgoing network packets (initial 0x12340000, low 16 bits XORed with a random value in transfer.c; network/protocol/transfer.c, ui/frontend/network.c). */

extern uint32_t g_UiTransferSenderContext; /* 004AE980 g_UiTransferSenderContext */

extern UiTransferMailboxState g_UiTransferMailbox; /* 004AE990 g_UiTransferMailbox */

extern uint32_t g_UiTransferRoundKeys[16]; /* 004AE9A8 g_UiTransferRoundKeys: uint32_t[16] packet cipher round keys (UiTransfer_EncryptPacketBlocks/DecryptPacketBlocks take this as the 16-key table; keys 12..15 at 0x004AE9D8 were read by Ghidra as the text "mohTG sakere!!!e"). */

extern UiRuntimeRecord g_UiTransferChunkPacket; /* 004AE9E8 g_UiTransferChunkPacket: mailbox chunk packet (0x10031 request / 0x80030 chunk, 0x100 bytes): header (sequence token at 0x004AE9EC), payload = chunk offset (0x004AE9F8), transfer byte count (0x004AE9FC), chunk data (0x004AEA00, 58 dwords). */

extern FrontendPacket10032HostValue g_UiTransferPingEchoPacket; /* 004AEAE8 g_UiTransferPingEchoPacket: ping answer packet 0x10033 (same layout as the 0x10032 ping): header (sequence token at 0x004AEAEC), echoed tick (0x004AEAF8). */

extern UiTransferMailboxTickCounter g_UiTransferMailboxTickCounter; /* 004AEB08 g_UiTransferMailboxTickCounter */

extern uint16_t g_GameVersionUtf16[7]; /* 0050F07C g_GameVersionUtf16: version string shown to joining players ("1.5.45") */

extern UiTransferEndpointDescriptor g_FrontendSelectedNetworkEndpoint; /* 0050F090 g_FrontendSelectedNetworkEndpoint */

extern uint32_t g_FrontendSessionToken; /* 0050F0A0 g_FrontendSessionToken */

extern SessionTransferTimeoutTicks g_SessionTransferTimeoutTicks; /* 0050F0A4 g_SessionTransferTimeoutTicks */

extern uint32_t g_FrontendTransferResponsePending; /* 0050F0A8 g_FrontendTransferResponsePending */

extern UiCommandQueueRecord g_FrontendCommandQueueRecords[16]; /* 00543E40 g_FrontendCommandQueueRecords */

extern UiCommandQueueRecord *g_FrontendCommandQueueEnd; /* 00543F40 g_FrontendCommandQueueEnd: followed by 12 bytes 0x90 fill (dropped) */

extern uint32_t g_FrontendLocalPlayerPcxPreview; /* 00545920 g_FrontendLocalPlayerPcxPreview */

extern FrontendPacket10000Handshake g_FrontendPacket10000Buffer; /* 0054D7A0 g_FrontendPacket10000Buffer */

extern FrontendPacket50001SessionAdvertisement g_FrontendPacket50001Buffer; /* 0054D7C0 g_FrontendPacket50001Buffer */

extern FrontendPacket20002PlayerDescriptor g_FrontendPacket20002Buffer; /* 0054D860 g_FrontendPacket20002Buffer */

extern FrontendPacket10003JoinAck g_FrontendPacket10003Buffer; /* 0054D8A0 g_FrontendPacket10003Buffer */

extern FrontendPacket10004PlayerSnapshotRequest g_FrontendPacket10004Buffer; /* 0054D8C0 g_FrontendPacket10004Buffer */

extern FrontendPacket10006CapabilityHeartbeat g_FrontendPacket10006Buffer; /* 0054D940 g_FrontendPacket10006Buffer */

extern FrontendPacket40008LobbyRosterSnapshot g_FrontendPacket40008Buffer; /* 0054D960 g_FrontendPacket40008Buffer */

extern FrontendPacket8000ASnapshotChunk g_FrontendPacket8000ABuffer; /* 0054DA00 g_FrontendPacket8000ABuffer */

extern FrontendCommandPacketRecord g_FrontendPacket10011Buffer; /* 0054DD20 g_FrontendPacket10011Buffer */

extern FrontendPacket10013HeartbeatAck g_FrontendPacket10013Buffer; /* 0054DD60 g_FrontendPacket10013Buffer */

extern FrontendPacket10032HostValue g_FrontendPacket10032Buffer; /* 0054DD80 g_FrontendPacket10032Buffer */

extern uint32_t g_FrontendPendingSessionPlayerCount; /* 0054DDA0 g_FrontendPendingSessionPlayerCount: followed by an all-zero dword (0x0054DDA4) no code reaches (dropped) */

extern uint32_t g_FrontendExpectedPlayerRuntimeBlockCount; /* 0054DDA8 g_FrontendExpectedPlayerRuntimeBlockCount */

extern uint32_t g_FrontendHostPublishRoundRobinCounter; /* 0054DDAC g_FrontendHostPublishRoundRobinCounter */

extern UiCommandQueueRecord g_InGameCommandQueueRecords[16]; /* 0055EFC0 g_InGameCommandQueueRecords */

extern UiCommandQueueRecord *g_InGameCommandQueueEnd; /* 0055F0C0 g_InGameCommandQueueEnd */

extern FrontendCommandPacketRecord g_FrontendClientPlayerCommandRecords[8]; /* 00572060 g_FrontendClientPlayerCommandRecords */

extern FrontendCommandPacketRecord g_FrontendClientCommandBatchPacketBuffer[8]; /* 00572160 g_FrontendClientCommandBatchPacketBuffer */

extern FrontendCommandPacketRecord g_FrontendPacket10021Buffer; /* 00572260 g_FrontendPacket10021Buffer */

#endif
