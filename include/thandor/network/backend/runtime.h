/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_BACKEND_RUNTIME_H
#define THANDOR_NETWORK_BACKEND_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/backend/runtime. */

/* g_NetworkBackendMode: which WinSock DLL Network_Init started (Network_Shutdown calls its WSACleanup) */
#define NETWORK_BACKEND_MODE_NONE 0
#define NETWORK_BACKEND_MODE_WSOCK32 1 /* wsock32.dll, WinSock 1.1 */

/* UDP port (host byte order) the frontend passes to g_NetworkBackendSlot2 (open and bind) for every session. */
#define NETWORK_GAME_UDP_PORT 929

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

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg);

bool FrontendNetwork_HostTickCommandAndSnapshotTransfer(uint32_t callbackArg);

void FrontendNetwork_TickDisconnectTimeoutAndResetSession(void);

bool FrontendNetwork_HandleCommandBatchAndPlayerTimeout
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet);

uint32_t __cdecl Network_Init(void);

void Network_Shutdown(void);

uint32_t NetworkBackend_SetSessionContext(uint32_t backendIndex);

#endif /* THANDOR_NETWORK_BACKEND_RUNTIME_H */
