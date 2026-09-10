#ifndef THANDOR_NETWORK_BACKEND_RUNTIME_H
#define THANDOR_NETWORK_BACKEND_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/backend/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0054EF60 */
void __fastcall FrontendNetwork_HandleHandshakeAndPlayerStatePackets (undefined4 param_1,undefined4 param_2,UiTransferEndpointDescriptor *senderEndpoint, FrontendTransferPacketUnion *packet);

/* 0x0054F240 */
undefined8 __fastcall FrontendNetwork_HostTickCommandAndSnapshotTransfer(undefined4 param_1,undefined4 param_2);

/* 0x0054FA10 */
void __cdecl FrontendNetwork_TickDisconnectTimeoutAndResetSession(void);

/* 0x00572710 */
undefined8 FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf (NetworkSessionContext *sessionContext,FrontendTransferPacketUnion *packet);

/* 0x00584080 */
dword __cdecl Network_Init(void);

/* 0x00584DF0 */
void __cdecl Network_Shutdown(void);

/* 0x00584E50 */
NetworkBackendSessionReturnValue32 __thiscall NetworkBackend_SetSessionContextCf(void *this,NetworkBackendSessionReturnValue32 returnValue);

/* 0x00585210 */
undefined8 NetworkBackend_SelectInstanceByIndex(NetworkSessionContext *param_1);

#endif /* THANDOR_NETWORK_BACKEND_RUNTIME_H */
