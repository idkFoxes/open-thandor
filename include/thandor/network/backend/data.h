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

extern WinSock_acceptProc *g_WinSock_accept; /* 005742A0 g_WinSock_accept */

extern WinSock_bindProc *g_WinSock_bind; /* 005742A4 g_WinSock_bind */

extern WinSock_closesocketProc *g_WinSock_closesocket; /* 005742A8 g_WinSock_closesocket */

extern WinSock_connectProc *g_WinSock_connect; /* 005742AC g_WinSock_connect */

extern WinSock_getpeernameProc *g_WinSock_getpeername; /* 005742B0 g_WinSock_getpeername */

extern WinSock_getsocknameProc *g_WinSock_getsockname; /* 005742B4 g_WinSock_getsockname */

extern WinSock_getsockoptProc *g_WinSock_getsockopt; /* 005742B8 g_WinSock_getsockopt */

extern WinSock_htonlProc *g_WinSock_htonl; /* 005742BC g_WinSock_htonl */

extern WinSock_htonsProc *g_WinSock_htons; /* 005742C0 g_WinSock_htons */

extern WinSock_inet_addrProc *g_WinSock_inet_addr; /* 005742C4 g_WinSock_inet_addr */

extern WinSock_inet_ntoaProc *g_WinSock_inet_ntoa; /* 005742C8 g_WinSock_inet_ntoa */

extern WinSock_ioctlsocketProc *g_WinSock_ioctlsocket; /* 005742CC g_WinSock_ioctlsocket */

extern WinSock_listenProc *g_WinSock_listen; /* 005742D0 g_WinSock_listen */

extern WinSock_ntohlProc *g_WinSock_ntohl; /* 005742D4 g_WinSock_ntohl */

extern WinSock_ntohsProc *g_WinSock_ntohs; /* 005742D8 g_WinSock_ntohs */

extern WinSock_recvProc *g_WinSock_recv; /* 005742DC g_WinSock_recv */

extern WinSock_recvfromProc *g_WinSock_recvfrom; /* 005742E0 g_WinSock_recvfrom */

extern WinSock_selectProc *g_WinSock_select; /* 005742E4 g_WinSock_select */

extern WinSock_sendProc *g_WinSock_send; /* 005742E8 g_WinSock_send */

extern WinSock_sendtoProc *g_WinSock_sendto; /* 005742EC g_WinSock_sendto */

extern WinSock_setsockoptProc *g_WinSock_setsockopt; /* 005742F0 g_WinSock_setsockopt */

extern WinSock_shutdownProc *g_WinSock_shutdown; /* 005742F4 g_WinSock_shutdown */

extern WinSock_socketProc *g_WinSock_socket; /* 005742F8 g_WinSock_socket */

extern WinSock_gethostbyaddrProc *g_WinSock_gethostbyaddr; /* 005742FC g_WinSock_gethostbyaddr */

extern WinSock_gethostbynameProc *g_WinSock_gethostbyname; /* 00574300 g_WinSock_gethostbyname */

extern WinSock_gethostnameProc *g_WinSock_gethostname; /* 00574304 g_WinSock_gethostname */

extern WinSock_getprotobynameProc *g_WinSock_getprotobyname; /* 00574308 g_WinSock_getprotobyname */

extern WinSock_getprotobynumberProc *g_WinSock_getprotobynumber; /* 0057430C g_WinSock_getprotobynumber */

extern WinSock_getservbynameProc *g_WinSock_getservbyname; /* 00574310 g_WinSock_getservbyname */

extern WinSock_getservbyportProc *g_WinSock_getservbyport; /* 00574314 g_WinSock_getservbyport */

extern WinSock_WSAAsyncGetHostByAddrProc *g_WinSock_WSAAsyncGetHostByAddr; /* 00574318 g_WinSock_WSAAsyncGetHostByAddr */

extern WinSock_WSAAsyncGetHostByNameProc *g_WinSock_WSAAsyncGetHostByName; /* 0057431C g_WinSock_WSAAsyncGetHostByName */

extern WinSock_WSAAsyncGetProtoByNameProc *g_WinSock_WSAAsyncGetProtoByName; /* 00574320 g_WinSock_WSAAsyncGetProtoByName */

extern WinSock_WSAAsyncGetProtoByNumberProc *g_WinSock_WSAAsyncGetProtoByNumber; /* 00574324 g_WinSock_WSAAsyncGetProtoByNumber */

extern WinSock_WSAAsyncGetServByNameProc *g_WinSock_WSAAsyncGetServByName; /* 00574328 g_WinSock_WSAAsyncGetServByName */

extern WinSock_WSAAsyncGetServByPortProc *g_WinSock_WSAAsyncGetServByPort; /* 0057432C g_WinSock_WSAAsyncGetServByPort */

extern WinSock_WSAAsyncSelectProc *g_WinSock_WSAAsyncSelect; /* 00574330 g_WinSock_WSAAsyncSelect */

extern WinSock_WSACancelAsyncRequestProc *g_WinSock_WSACancelAsyncRequest; /* 00574334 g_WinSock_WSACancelAsyncRequest */

extern WinSock_WSACancelBlockingCallProc *g_WinSock_WSACancelBlockingCall; /* 00574338 g_WinSock_WSACancelBlockingCall */

extern WinSock_WSACleanupProc *g_WinSock_WSACleanup; /* 0057433C g_WinSock_WSACleanup */

extern WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError; /* 00574340 g_WinSock_WSAGetLastError */

extern WinSock_WSAIsBlockingProc *g_WinSock_WSAIsBlocking; /* 00574344 g_WinSock_WSAIsBlocking */

extern WinSock_WSASetBlockingHookProc *g_WinSock_WSASetBlockingHook; /* 00574348 g_WinSock_WSASetBlockingHook */

extern WinSock_WSAStartupProc *g_WinSock_WSAStartup; /* 00574350 g_WinSock_WSAStartup */

extern WinSock_WSAUnhookBlockingHookProc *g_WinSock_WSAUnhookBlockingHook; /* 00574354 g_WinSock_WSAUnhookBlockingHook */

extern WinSock_bindProc *g_Ws2_32_bind; /* 0057435C g_Ws2_32_bind */

extern WinSock_closesocketProc *g_Ws2_32_closesocket; /* 00574360 g_Ws2_32_closesocket */

extern WinSock_htonsProc *g_Ws2_32_htons; /* 00574378 g_Ws2_32_htons */

extern WinSock_recvfromProc *g_Ws2_32_recvfrom; /* 00574390 g_Ws2_32_recvfrom */

extern WinSock_sendtoProc *g_Ws2_32_sendto; /* 0057439C g_Ws2_32_sendto */

extern WinSock_setsockoptProc *g_Ws2_32_setsockopt; /* 005743A0 g_Ws2_32_setsockopt */

extern WinSock_socketProc *g_Ws2_32_socket; /* 005743A8 g_Ws2_32_socket */

extern WinSock_WSACleanupProc *g_Ws2_32_WSACleanup; /* 005743B8 g_Ws2_32_WSACleanup */

extern WinSock_WSAGetLastErrorProc *g_Ws2_32_WSAGetLastError; /* 005743D8 g_Ws2_32_WSAGetLastError */

extern WSAIoctl_Proc *g_Ws2_32_WSAIoctl; /* 005743EC g_Ws2_32_WSAIoctl */

extern WinSock_gethostbynameProc *g_Ws2_32_gethostbyname; /* 00574444 g_Ws2_32_gethostbyname */

extern WSAAddressToStringA_Proc *g_Ws2_32_WSAAddressToStringA; /* 00574478 g_Ws2_32_WSAAddressToStringA */

extern WSAStringToAddressA_Proc *g_Ws2_32_WSAStringToAddressA; /* 005744A0 g_Ws2_32_WSAStringToAddressA */

extern char s_Wsock32ModuleName[8]; /* 005744DC s_Wsock32ModuleName */

extern char s_Wsock32Export_accept[7]; /* 00574D40 s_Wsock32Export_accept */

extern char s_Wsock32Export_bind[5]; /* 00574D48 s_Wsock32Export_bind */

extern char s_Wsock32Export_closesocket[12]; /* 00574D4E s_Wsock32Export_closesocket */

extern char s_Wsock32Export_connect[8]; /* 00574D5A s_Wsock32Export_connect */

extern char s_Wsock32Export_getpeername[12]; /* 00574D62 s_Wsock32Export_getpeername */

extern char s_Wsock32Export_getsockname[12]; /* 00574D6E s_Wsock32Export_getsockname */

extern char s_Wsock32Export_getsockopt[11]; /* 00574D7A s_Wsock32Export_getsockopt */

extern char s_Wsock32Export_htonl[6]; /* 00574D86 s_Wsock32Export_htonl */

extern char s_Wsock32Export_htons[6]; /* 00574D8C s_Wsock32Export_htons */

extern char s_Wsock32Export_inet_addr[10]; /* 00574D92 s_Wsock32Export_inet_addr */

extern char s_Wsock32Export_inet_ntoa[10]; /* 00574D9C s_Wsock32Export_inet_ntoa */

extern char s_Wsock32Export_ioctlsocket[12]; /* 00574DA6 s_Wsock32Export_ioctlsocket */

extern char s_Wsock32Export_listen[7]; /* 00574DB2 s_Wsock32Export_listen */

extern char s_Wsock32Export_ntohl[6]; /* 00574DBA s_Wsock32Export_ntohl */

extern char s_Wsock32Export_ntohs[6]; /* 00574DC0 s_Wsock32Export_ntohs */

extern char s_Wsock32Export_recv[5]; /* 00574DC6 s_Wsock32Export_recv */

extern char s_Wsock32Export_recvfrom[9]; /* 00574DCC s_Wsock32Export_recvfrom */

extern char s_Wsock32Export_select[7]; /* 00574DD6 s_Wsock32Export_select */

extern char s_Wsock32Export_send[5]; /* 00574DDE s_Wsock32Export_send */

extern char s_Wsock32Export_sendto[7]; /* 00574DE4 s_Wsock32Export_sendto */

extern char s_Wsock32Export_setsockopt[11]; /* 00574DEC s_Wsock32Export_setsockopt */

extern char s_Wsock32Export_shutdown[9]; /* 00574DF8 s_Wsock32Export_shutdown */

extern char s_Wsock32Export_socket[7]; /* 00574E02 s_Wsock32Export_socket */

extern char s_Wsock32Export_gethostbyaddr[14]; /* 00574E0A s_Wsock32Export_gethostbyaddr */

extern char s_Wsock32Export_gethostbyname[14]; /* 00574E18 s_Wsock32Export_gethostbyname */

extern char s_Wsock32Export_gethostname[12]; /* 00574E26 s_Wsock32Export_gethostname */

extern char s_Wsock32Export_getprotobyname[15]; /* 00574E32 s_Wsock32Export_getprotobyname */

extern char s_Wsock32Export_getprotobynumber[17]; /* 00574E42 s_Wsock32Export_getprotobynumber */

extern char s_Wsock32Export_getservbyname[14]; /* 00574E54 s_Wsock32Export_getservbyname */

extern char s_Wsock32Export_getservbyport[14]; /* 00574E62 s_Wsock32Export_getservbyport */

extern char s_Wsock32Export_WSAAsyncGetHostByAddr[22]; /* 00574E70 s_Wsock32Export_WSAAsyncGetHostByAddr */

extern char s_Wsock32Export_WSAAsyncGetHostByName[22]; /* 00574E86 s_Wsock32Export_WSAAsyncGetHostByName */

extern char s_Wsock32Export_WSAAsyncGetProtoByName[23]; /* 00574E9C s_Wsock32Export_WSAAsyncGetProtoByName */

extern char s_Wsock32Export_WSAAsyncGetProtoByNumber[25]; /* 00574EB4 s_Wsock32Export_WSAAsyncGetProtoByNumber */

extern char s_Wsock32Export_WSAAsyncGetServByName[22]; /* 00574ECE s_Wsock32Export_WSAAsyncGetServByName */

extern char s_Wsock32Export_WSAAsyncGetServByPort[22]; /* 00574EE4 s_Wsock32Export_WSAAsyncGetServByPort */

extern char s_Wsock32Export_WSAAsyncSelect[15]; /* 00574EFA s_Wsock32Export_WSAAsyncSelect */

extern char s_Wsock32Export_WSACancelAsyncRequest[22]; /* 00574F0A s_Wsock32Export_WSACancelAsyncRequest */

extern char s_Wsock32Export_WSACancelBlockingCall[22]; /* 00574F20 s_Wsock32Export_WSACancelBlockingCall */

extern char s_Wsock32Export_WSACleanup[11]; /* 00574F36 s_Wsock32Export_WSACleanup */

extern char s_Wsock32Export_WSAGetLastError[16]; /* 00574F42 s_Wsock32Export_WSAGetLastError */

extern char s_Wsock32Export_WSAIsBlocking[14]; /* 00574F52 s_Wsock32Export_WSAIsBlocking */

extern char s_Wsock32Export_WSASetBlockingHook[19]; /* 00574F60 s_Wsock32Export_WSASetBlockingHook */

extern char s_Wsock32Export_WSAStartup[11]; /* 00574F84 s_Wsock32Export_WSAStartup */

extern char s_Wsock32Export_WSAUnhookBlockingHook[22]; /* 00574F90 s_Wsock32Export_WSAUnhookBlockingHook */

extern uint32_t g_NetworkBackendMode; /* 00583D60 g_NetworkBackendMode */

extern WinSockData11 g_WinSockStartupData; /* 00583D64 g_WinSockStartupData */

extern NetworkSocketHandle32 g_NetworkFallbackSocket; /* 00583EF2 g_NetworkFallbackSocket */

extern uint32_t g_NetworkFallbackSocketOptionOn; /* 00583EF6 g_NetworkFallbackSocketOptionOn: uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO); network/backend/fallback_udp.c */

extern uint32_t g_NetworkFallbackAddressLength; /* 00583EFA g_NetworkFallbackAddressLength */

extern WinSockAddress g_NetworkFallbackBindEndpoint; /* 00583EFE g_NetworkFallbackBindEndpoint */

extern uint32_t g_NetworkBackendActiveAddressFamily; /* 00583F0E g_NetworkBackendActiveAddressFamily */

extern uint32_t g_NetworkBackendActiveSocketAddressLength; /* 00583F12 g_NetworkBackendActiveSocketAddressLength */

extern uint32_t g_NetworkBackendActiveSocketType; /* 00583F16 g_NetworkBackendActiveSocketType */

extern uint32_t g_NetworkBackendActiveProtocol; /* 00583F1A g_NetworkBackendActiveProtocol */

extern uint32_t g_NetworkBackendPortNetworkOrderCarrier; /* 00583F1E g_NetworkBackendPortNetworkOrderCarrier */

extern NetworkBackendSocketAddress16 g_NetworkBackendBindAddress; /* 00583F22 g_NetworkBackendBindAddress (followed by 14 bytes of NOP fill) */

extern uint8_t g_NetworkEndpointTextScratchA[256]; /* 00583F40 g_NetworkEndpointTextScratchA: narrow endpoint/address text (written with capacity 255) */

extern NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040; /* 00584040 NetworkBackendInstanceDescriptorPrefix_00584040 */

extern char s_CommandLineOptionIp[3]; /* 00584078 s_CommandLineOptionIp: "IP=" without terminator (the option search gets the length 3) */

#endif
