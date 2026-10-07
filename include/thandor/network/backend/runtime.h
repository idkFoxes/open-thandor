/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_BACKEND_RUNTIME_H
#define THANDOR_NETWORK_BACKEND_RUNTIME_H

#include <thandor/core/types.h>
#include <thandor/network/backend/types.h>
#include <thandor/core/contracts.h>

/* g_NetworkBackendMode: which WinSock DLL Network_Init started (Network_Shutdown calls its WSACleanup) */
enum class NetworkBackendMode : uint32_t {
    NETWORK_BACKEND_MODE_NONE=0,
    NETWORK_BACKEND_MODE_WSOCK32=1 /* wsock32.dll, WinSock 1.1 */
};

/* UDP port (host byte order) the frontend passes to g_NetworkBackendSlot2 (open and bind) for every session. */
inline constexpr auto NETWORK_GAME_UDP_PORT = 929;

uint32_t Network_Init();

void Network_Shutdown();

uint32_t NetworkBackend_SetSessionContext(uint32_t backendIndex);

extern NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable;
extern NetworkBackendReceiveCallback *g_NetworkBackendSlot4;
extern NetworkBackendSendCallback *g_NetworkBackendSlot5;

extern WinSock_bindProc *g_WinSock_bind;
extern WinSock_closesocketProc *g_WinSock_closesocket;
extern WinSock_htonsProc *g_WinSock_htons;
extern WinSock_inet_addrProc *g_WinSock_inet_addr;
extern WinSock_inet_ntoaProc *g_WinSock_inet_ntoa;
extern WinSock_ioctlsocketProc *g_WinSock_ioctlsocket;
extern WinSock_recvfromProc *g_WinSock_recvfrom;
extern WinSock_sendtoProc *g_WinSock_sendto;
extern WinSock_setsockoptProc *g_WinSock_setsockopt;
extern WinSock_socketProc *g_WinSock_socket;
extern WinSock_gethostbynameProc *g_WinSock_gethostbyname;
extern WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError;

extern uint32_t g_NetworkBackendInstanceCount;
extern NetworkBackendSetSessionCallback *g_NetworkBackendSlot0;
extern NetworkBackendCleanupCallback *g_NetworkBackendSlot1;
extern NetworkBackendOpenBindCallback *g_NetworkBackendSlot2;
extern NetworkBackendCloseCallback *g_NetworkBackendSlot3;
extern NetworkBackendParseEndpointCallback *g_NetworkBackendSlot6;
extern NetworkBackendFormatAddressCallback *g_NetworkBackendSlot7;

#endif /* THANDOR_NETWORK_BACKEND_RUNTIME_H */
