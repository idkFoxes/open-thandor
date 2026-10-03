/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/data.h
 */

#ifndef THANDOR_NETWORK_BACKEND_DATA_H
#define THANDOR_NETWORK_BACKEND_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable; /* 0041A540 g_NetworkBackendInstanceTable */

extern NetworkSessionContext *g_NetworkBackendSessionContext; /* 0041A548 g_NetworkBackendSessionContext */

extern NetworkBackendReceiveCallback *g_NetworkBackendSlot4; /* 0041A55C g_NetworkBackendSlot4 */

extern NetworkBackendSendCallback *g_NetworkBackendSlot5; /* 0041A560 g_NetworkBackendSlot5 */

extern UiTransferEndpointDescriptor g_NetworkLocalEndpoint; /* 0041A56C g_NetworkLocalEndpoint */

extern uint32_t g_FrontendSelectedPlayerToken; /* 0050F0CC g_FrontendSelectedPlayerToken: uint32_t sender context of the last executed network batch (0xFFFFFFFF = none); network/backend and protocol/transfer */

extern uint32_t g_FrontendHostSnapshotTransferCountdown; /* 00545924 g_FrontendHostSnapshotTransferCountdown */

extern FrontendPacket30005PlayerSnapshot g_FrontendPacket30005Buffer; /* 0054D8E0 g_FrontendPacket30005Buffer */

extern FrontendPacket10009SnapshotChunkRequest g_FrontendPacket10009Buffer; /* 0054D9E0 g_FrontendPacket10009Buffer */

extern FrontendCommandPacketRecord g_FrontendPlayerCommandRecords[8]; /* 0054DB20 g_FrontendPlayerCommandRecords */

extern FrontendCommandPacketRecord g_FrontendCommandBatchPacketBuffer[8]; /* 0054DC20 g_FrontendCommandBatchPacketBuffer */

extern FrontendPacket10012SyncPending g_FrontendPacket10012Buffer; /* 0054DD40 g_FrontendPacket10012Buffer */

extern WinSock_bindProc *g_WinSock_bind; /* 005742A4 g_WinSock_bind */

extern WinSock_closesocketProc *g_WinSock_closesocket; /* 005742A8 g_WinSock_closesocket */

extern WinSock_htonsProc *g_WinSock_htons; /* 005742C0 g_WinSock_htons */

extern WinSock_inet_addrProc *g_WinSock_inet_addr; /* 005742C4 g_WinSock_inet_addr */

extern WinSock_inet_ntoaProc *g_WinSock_inet_ntoa; /* 005742C8 g_WinSock_inet_ntoa */

extern WinSock_ioctlsocketProc *g_WinSock_ioctlsocket; /* 005742CC g_WinSock_ioctlsocket */

extern WinSock_recvfromProc *g_WinSock_recvfrom; /* 005742E0 g_WinSock_recvfrom */

extern WinSock_sendtoProc *g_WinSock_sendto; /* 005742EC g_WinSock_sendto */

extern WinSock_setsockoptProc *g_WinSock_setsockopt; /* 005742F0 g_WinSock_setsockopt */

extern WinSock_socketProc *g_WinSock_socket; /* 005742F8 g_WinSock_socket */

extern WinSock_gethostbynameProc *g_WinSock_gethostbyname; /* 00574300 g_WinSock_gethostbyname */

extern WinSock_WSACleanupProc *g_WinSock_WSACleanup; /* 0057433C g_WinSock_WSACleanup */

extern WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError; /* 00574340 g_WinSock_WSAGetLastError */

extern WinSock_WSAStartupProc *g_WinSock_WSAStartup; /* 00574350 g_WinSock_WSAStartup */

extern char s_Wsock32ModuleName[8]; /* 005744DC s_Wsock32ModuleName */

extern char s_Wsock32Export_bind[5]; /* 00574D48 s_Wsock32Export_bind */

extern char s_Wsock32Export_closesocket[12]; /* 00574D4E s_Wsock32Export_closesocket */

extern char s_Wsock32Export_htons[6]; /* 00574D8C s_Wsock32Export_htons */

extern char s_Wsock32Export_inet_addr[10]; /* 00574D92 s_Wsock32Export_inet_addr */

extern char s_Wsock32Export_inet_ntoa[10]; /* 00574D9C s_Wsock32Export_inet_ntoa */

extern char s_Wsock32Export_ioctlsocket[12]; /* 00574DA6 s_Wsock32Export_ioctlsocket */

extern char s_Wsock32Export_recvfrom[9]; /* 00574DCC s_Wsock32Export_recvfrom */

extern char s_Wsock32Export_sendto[7]; /* 00574DE4 s_Wsock32Export_sendto */

extern char s_Wsock32Export_setsockopt[11]; /* 00574DEC s_Wsock32Export_setsockopt */

extern char s_Wsock32Export_socket[7]; /* 00574E02 s_Wsock32Export_socket */

extern char s_Wsock32Export_gethostbyname[14]; /* 00574E18 s_Wsock32Export_gethostbyname */

extern char s_Wsock32Export_WSACleanup[11]; /* 00574F36 s_Wsock32Export_WSACleanup */

extern char s_Wsock32Export_WSAGetLastError[16]; /* 00574F42 s_Wsock32Export_WSAGetLastError */

extern char s_Wsock32Export_WSAStartup[11]; /* 00574F84 s_Wsock32Export_WSAStartup */

extern uint32_t g_NetworkBackendMode; /* 00583D60 g_NetworkBackendMode */

extern WinSockData11 g_WinSockStartupData; /* 00583D64 g_WinSockStartupData */

extern NetworkSocketHandle32 g_NetworkFallbackSocket; /* 00583EF2 g_NetworkFallbackSocket */

extern uint32_t g_NetworkFallbackSocketOptionOn; /* 00583EF6 g_NetworkFallbackSocketOptionOn: uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO); network/backend/fallback_udp.c */

extern uint32_t g_NetworkFallbackAddressLength; /* 00583EFA g_NetworkFallbackAddressLength */

extern WinSockAddress g_NetworkFallbackBindEndpoint; /* 00583EFE g_NetworkFallbackBindEndpoint */

extern uint8_t g_NetworkEndpointTextScratchA[256]; /* 00583F40 g_NetworkEndpointTextScratchA: narrow endpoint/address text (written with capacity 255) */

extern NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040; /* 00584040 NetworkBackendInstanceDescriptorPrefix_00584040 */

extern char s_CommandLineOptionIp[3]; /* 00584078 s_CommandLineOptionIp: "IP=" without terminator (the option search gets the length 3) */

#endif
