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
NetworkBackend_SetSessionContext(void *this,NetworkBackendSessionReturnValue32 returnValue);

/* 0x00585210 */
bool __thandor_cf_preserve_eax_ecx_edx NetworkBackend_SelectInstanceByIndex(uint32_t instanceIndex);


/* 0x00583D10 */
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D10(void);

/* 0x00583D30 */
void __thandor_void_preserve_eax_ecx_edx Unreferenced_NoOpPreserveRegs_00583D30(void);

/* 0x00583D40 */
uint32_t __thandor_eax_preserve_ecx_edx Unreferenced_ReturnZeroPreserveRegs_00583D40(void);

#endif /* THANDOR_NETWORK_BACKEND_RUNTIME_H */
