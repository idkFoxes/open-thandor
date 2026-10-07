/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/frontend_session.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_FRONTEND_SESSION_H
#define THANDOR_NETWORK_PROTOCOL_FRONTEND_SESSION_H

#include <thandor/core/types.h>
#include <thandor/network/backend/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* In-game notice shown when the host removes a player (FRONTEND_PACKET_10007_PLAYER_REMOVAL); the player's
   name is patched into it. Earlier notes tie it to the network timeout ("Zeitueberschreitung") path. */
#define TEXT_ID_NETWORK_PLAYER_REMOVED 0xFF00
/* Notice shown on a client when the host has not been heard from for FRONTEND_PEER_TIMEOUT_TICKS and the client
   falls back to a local session; the host's (player block 0) name is patched into it. */
#define TEXT_ID_NETWORK_HOST_LOST 0xFF01

/* Player snapshot exchange at session start (FrontendNetwork_HandleHandshakeAndPlayerStatePackets,
   FrontendNetwork_HostTickCommandAndSnapshotTransfer): each client sends its 0x1300-byte snapshotPayload
   in chunks of UI_TRANSFER_CHUNK_PAYLOAD_BYTES (0x8000A packets); the last chunk starts at 0x1220 and holds the
   remaining 0xE0 bytes. The host asks for the next chunk with 0x10009 and repeats the request after 4 host ticks
   without an answer; it then packs every player's flags dword (+ payload when complete) and PCK-encodes it. */
#define FRONTEND_SNAPSHOT_PAYLOAD_BYTES 0x1300
#define FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET 0x1220
#define FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES 0xE0
#define FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS 4
#define FRONTEND_SNAPSHOT_FLAGS_BYTES 4 /* the snapshotTransferFlags dword in front of each packed payload */

void FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          struct FrontendUiImage *unusedFrontendRoot);

Bool8 FrontendNetwork_HostTickCommandAndSnapshotTransfer(struct FrontendUiImage *unusedFrontendRoot);

void FrontendNetwork_TickDisconnectTimeoutAndResetSession();

Bool8 FrontendNetwork_HandleCommandBatchAndPlayerTimeout
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet);

extern uint32_t g_FrontendSelectedPlayerToken; /* uint32_t sender context of the last executed network batch (0xFFFFFFFF = none); network/backend and protocol/transfer */
extern uint32_t g_FrontendHostSnapshotTransferCountdown;
extern FrontendCommandPacketRecord g_FrontendPlayerCommandRecords[8];
extern FrontendCommandPacketRecord g_FrontendCommandBatchPacketBuffer[8];

extern FrontendPacket10023StateAck g_FrontendPacket10023Buffer;

#endif /* THANDOR_NETWORK_PROTOCOL_FRONTEND_SESSION_H */
