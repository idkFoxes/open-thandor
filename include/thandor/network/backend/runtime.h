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
#define NETWORK_BACKEND_MODE_WS2_32 2 /* ws2_32.dll; no reachable code sets it */

/* UDP port (host byte order) the frontend passes to g_NetworkBackendSlot2 (open and bind) for every session. */
#define NETWORK_GAME_UDP_PORT 929

/* In-game notice shown when the host removes a player (FRONTEND_PACKET_10007_PLAYER_REMOVAL); the player's
   name is patched into it. Earlier notes tie it to the network timeout ("Zeitueberschreitung") path. */
#define TEXT_ID_NETWORK_PLAYER_REMOVED 0xFF00
/* Notice shown on a client when the host has not been heard from for FRONTEND_PEER_TIMEOUT_TICKS and the client
   falls back to a local session; the host's (player block 0) name is patched into it. */
#define TEXT_ID_NETWORK_HOST_LOST 0xFF01

/* Player snapshot exchange at session start (FrontendNetwork_HandleHandshakeAndPlayerStatePackets,
   FrontendNetwork_HostTickCommandAndSnapshotTransfer): each client sends its 0x1300-byte snapshotPayloadB0_13AF
   in chunks of UI_TRANSFER_CHUNK_PAYLOAD_BYTES (0x8000A packets); the last chunk starts at 0x1220 and holds the
   remaining 0xE0 bytes. The host asks for the next chunk with 0x10009 and repeats the request after 4 host ticks
   without an answer; it then packs every player's flags dword (+ payload when complete) and PCK-encodes it. */
#define FRONTEND_SNAPSHOT_PAYLOAD_BYTES 0x1300
#define FRONTEND_SNAPSHOT_LAST_CHUNK_OFFSET 0x1220
#define FRONTEND_SNAPSHOT_LAST_CHUNK_BYTES 0xE0
#define FRONTEND_SNAPSHOT_REQUEST_RETRY_TICKS 4
#define FRONTEND_SNAPSHOT_FLAGS_BYTES 4 /* the snapshotTransferFlags dword in front of each packed payload */

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0054EF60 */
void __thandor_void_preserve_eax_ecx_edx
FrontendNetwork_HandleHandshakeAndPlayerStatePackets
          (UiTransferEndpointDescriptor *senderEndpoint,FrontendTransferPacketUnion *packet,
          uint32_t unusedDispatchArg);

/* 0x0054F240 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetwork_HostTickCommandAndSnapshotTransfer(uint32_t callbackArg);

/* 0x0054FA10 */
void __thandor_void_preserve_eax_ecx FrontendNetwork_TickDisconnectTimeoutAndResetSession(void);

/* 0x00572710 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendNetwork_HandleCommandBatchAndPlayerTimeout
          (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet);

/* 0x00584080 */
uint32_t __cdecl Network_Init(void);

/* 0x00584DF0 */
void __thandor_preserve_eax Network_Shutdown(void);

/* 0x00584E50 */
NetworkSetSessionResult __thandor_this_eax_cf_preserve_ecx_edx
NetworkBackend_SetSessionContext(void *sessionContext,NetworkBackendSessionReturnValue32 backendIndex);

/* 0x00585210 */
bool __thandor_cf_preserve_eax_ecx_edx NetworkBackend_SelectInstanceByIndex(uint32_t instanceIndex);


/* 0x00583D10 */
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D10(void);

/* 0x00583D30 */
void __thandor_void_preserve_eax_ecx_edx Unreferenced_NoOpPreserveRegs_00583D30(void);

/* 0x00583D40 */
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D40(void);

#endif /* THANDOR_NETWORK_BACKEND_RUNTIME_H */
