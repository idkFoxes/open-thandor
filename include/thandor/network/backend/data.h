/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/data.h
 */

#ifndef THANDOR_NETWORK_BACKEND_DATA_H
#define THANDOR_NETWORK_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable;

extern NetworkSessionContext *g_NetworkBackendSessionContext;

extern NetworkBackendReceiveCallback *g_NetworkBackendSlot4;

extern NetworkBackendSendCallback *g_NetworkBackendSlot5;

extern UiTransferEndpointDescriptor g_NetworkLocalEndpoint;

extern uint32_t g_FrontendSelectedPlayerToken; /* uint32_t sender context of the last executed network batch (0xFFFFFFFF = none); network/backend and protocol/transfer */

extern uint32_t g_FrontendHostSnapshotTransferCountdown;

extern FrontendPacket30005PlayerSnapshot g_FrontendPacket30005Buffer;

extern FrontendPacket10009SnapshotChunkRequest g_FrontendPacket10009Buffer;

extern FrontendCommandPacketRecord g_FrontendPlayerCommandRecords[8];

extern FrontendCommandPacketRecord g_FrontendCommandBatchPacketBuffer[8];

extern FrontendPacket10012SyncPending g_FrontendPacket10012Buffer;

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

extern WinSock_WSACleanupProc *g_WinSock_WSACleanup;

extern WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError;

extern WinSock_WSAStartupProc *g_WinSock_WSAStartup;

extern char s_Wsock32ModuleName[8];

extern char s_Wsock32Export_bind[5];

extern char s_Wsock32Export_closesocket[12];

extern char s_Wsock32Export_htons[6];

extern char s_Wsock32Export_inet_addr[10];

extern char s_Wsock32Export_inet_ntoa[10];

extern char s_Wsock32Export_ioctlsocket[12];

extern char s_Wsock32Export_recvfrom[9];

extern char s_Wsock32Export_sendto[7];

extern char s_Wsock32Export_setsockopt[11];

extern char s_Wsock32Export_socket[7];

extern char s_Wsock32Export_gethostbyname[14];

extern char s_Wsock32Export_WSACleanup[11];

extern char s_Wsock32Export_WSAGetLastError[16];

extern char s_Wsock32Export_WSAStartup[11];

extern uint32_t g_NetworkBackendMode;

extern WinSockData11 g_WinSockStartupData;

extern NetworkSocketHandle32 g_NetworkFallbackSocket;

extern uint32_t g_NetworkFallbackSocketOptionOn; /* uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO); network/backend/fallback_udp.c */

extern uint32_t g_NetworkFallbackAddressLength;

extern WinSockAddress g_NetworkFallbackBindEndpoint;

extern uint8_t g_NetworkEndpointTextScratchA[256]; /* narrow endpoint/address text (written with capacity 255) */

extern NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040;

extern char s_CommandLineOptionIp[3]; /* "IP=" without terminator (the option search gets the length 3) */

#endif
