/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/network/backend/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0041A540 g_NetworkBackendInstanceTable */
__declspec(align(16)) NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable = 0;

/* 0041A548 g_NetworkBackendSessionContext */
__declspec(align(8)) NetworkSessionContext *g_NetworkBackendSessionContext = 0;

/* 0041A55C g_NetworkBackendSlot4 */
__declspec(align(4)) NetworkBackendReceiveCallback *g_NetworkBackendSlot4 = (void *)NetworkBackendFallback_ReceiveDatagram;

/* 0041A560 g_NetworkBackendSlot5 */
__declspec(align(16)) NetworkBackendSendCallback *g_NetworkBackendSlot5 = (void *)NetworkBackendFallback_SendDatagram;

/* 0041A56C g_NetworkLocalEndpoint */
__declspec(align(4)) UiTransferEndpointDescriptor g_NetworkLocalEndpoint = {0};

/* 0050F0CC g_FrontendSelectedPlayerToken: uint32_t sender context of the last executed network batch (0xFFFFFFFF = none); network/backend and protocol/transfer */
__declspec(align(4)) uint32_t g_FrontendSelectedPlayerToken = 1;

/* 00545924 g_FrontendHostSnapshotTransferCountdown */
__declspec(align(4)) uint32_t g_FrontendHostSnapshotTransferCountdown = 0;

/* 0054D8E0 g_FrontendPacket30005Buffer */
__declspec(align(16)) FrontendPacket30005PlayerSnapshot g_FrontendPacket30005Buffer = {0};

/* 0054D9E0 g_FrontendPacket10009Buffer */
__declspec(align(16)) FrontendPacket10009SnapshotChunkRequest g_FrontendPacket10009Buffer = {0};

/* 0054DB20 g_FrontendPlayerCommandRecords */
__declspec(align(16)) FrontendCommandPacketRecord g_FrontendPlayerCommandRecords[8] = {0};

/* 0054DC20 g_FrontendCommandBatchPacketBuffer */
__declspec(align(16)) FrontendCommandPacketRecord g_FrontendCommandBatchPacketBuffer[8] = {0};

/* 0054DD40 g_FrontendPacket10012Buffer */
__declspec(align(16)) FrontendPacket10012SyncPending g_FrontendPacket10012Buffer = {0};

/* 005742A0 g_WinSock_accept */
__declspec(align(16)) WinSock_acceptProc *g_WinSock_accept = 0;

/* 005742A4 g_WinSock_bind */
__declspec(align(4)) WinSock_bindProc *g_WinSock_bind = 0;

/* 005742A8 g_WinSock_closesocket */
__declspec(align(8)) WinSock_closesocketProc *g_WinSock_closesocket = 0;

/* 005742AC g_WinSock_connect */
__declspec(align(4)) WinSock_connectProc *g_WinSock_connect = 0;

/* 005742B0 g_WinSock_getpeername */
__declspec(align(16)) WinSock_getpeernameProc *g_WinSock_getpeername = 0;

/* 005742B4 g_WinSock_getsockname */
__declspec(align(4)) WinSock_getsocknameProc *g_WinSock_getsockname = 0;

/* 005742B8 g_WinSock_getsockopt */
__declspec(align(8)) WinSock_getsockoptProc *g_WinSock_getsockopt = 0;

/* 005742BC g_WinSock_htonl */
__declspec(align(4)) WinSock_htonlProc *g_WinSock_htonl = 0;

/* 005742C0 g_WinSock_htons */
__declspec(align(16)) WinSock_htonsProc *g_WinSock_htons = 0;

/* 005742C4 g_WinSock_inet_addr */
__declspec(align(4)) WinSock_inet_addrProc *g_WinSock_inet_addr = 0;

/* 005742C8 g_WinSock_inet_ntoa */
__declspec(align(8)) WinSock_inet_ntoaProc *g_WinSock_inet_ntoa = 0;

/* 005742CC g_WinSock_ioctlsocket */
__declspec(align(4)) WinSock_ioctlsocketProc *g_WinSock_ioctlsocket = 0;

/* 005742D0 g_WinSock_listen */
__declspec(align(16)) WinSock_listenProc *g_WinSock_listen = 0;

/* 005742D4 g_WinSock_ntohl */
__declspec(align(4)) WinSock_ntohlProc *g_WinSock_ntohl = 0;

/* 005742D8 g_WinSock_ntohs */
__declspec(align(8)) WinSock_ntohsProc *g_WinSock_ntohs = 0;

/* 005742DC g_WinSock_recv */
__declspec(align(4)) WinSock_recvProc *g_WinSock_recv = 0;

/* 005742E0 g_WinSock_recvfrom */
__declspec(align(16)) WinSock_recvfromProc *g_WinSock_recvfrom = 0;

/* 005742E4 g_WinSock_select */
__declspec(align(4)) WinSock_selectProc *g_WinSock_select = 0;

/* 005742E8 g_WinSock_send */
__declspec(align(8)) WinSock_sendProc *g_WinSock_send = 0;

/* 005742EC g_WinSock_sendto */
__declspec(align(4)) WinSock_sendtoProc *g_WinSock_sendto = 0;

/* 005742F0 g_WinSock_setsockopt */
__declspec(align(16)) WinSock_setsockoptProc *g_WinSock_setsockopt = 0;

/* 005742F4 g_WinSock_shutdown */
__declspec(align(4)) WinSock_shutdownProc *g_WinSock_shutdown = 0;

/* 005742F8 g_WinSock_socket */
__declspec(align(8)) WinSock_socketProc *g_WinSock_socket = 0;

/* 005742FC g_WinSock_gethostbyaddr */
__declspec(align(4)) WinSock_gethostbyaddrProc *g_WinSock_gethostbyaddr = 0;

/* 00574300 g_WinSock_gethostbyname */
__declspec(align(16)) WinSock_gethostbynameProc *g_WinSock_gethostbyname = 0;

/* 00574304 g_WinSock_gethostname */
__declspec(align(4)) WinSock_gethostnameProc *g_WinSock_gethostname = 0;

/* 00574308 g_WinSock_getprotobyname */
__declspec(align(8)) WinSock_getprotobynameProc *g_WinSock_getprotobyname = 0;

/* 0057430C g_WinSock_getprotobynumber */
__declspec(align(4)) WinSock_getprotobynumberProc *g_WinSock_getprotobynumber = 0;

/* 00574310 g_WinSock_getservbyname */
__declspec(align(16)) WinSock_getservbynameProc *g_WinSock_getservbyname = 0;

/* 00574314 g_WinSock_getservbyport */
__declspec(align(4)) WinSock_getservbyportProc *g_WinSock_getservbyport = 0;

/* 00574318 g_WinSock_WSAAsyncGetHostByAddr */
__declspec(align(8)) WinSock_WSAAsyncGetHostByAddrProc *g_WinSock_WSAAsyncGetHostByAddr = 0;

/* 0057431C g_WinSock_WSAAsyncGetHostByName */
__declspec(align(4)) WinSock_WSAAsyncGetHostByNameProc *g_WinSock_WSAAsyncGetHostByName = 0;

/* 00574320 g_WinSock_WSAAsyncGetProtoByName */
__declspec(align(16)) WinSock_WSAAsyncGetProtoByNameProc *g_WinSock_WSAAsyncGetProtoByName = 0;

/* 00574324 g_WinSock_WSAAsyncGetProtoByNumber */
__declspec(align(4)) WinSock_WSAAsyncGetProtoByNumberProc *g_WinSock_WSAAsyncGetProtoByNumber = 0;

/* 00574328 g_WinSock_WSAAsyncGetServByName */
__declspec(align(8)) WinSock_WSAAsyncGetServByNameProc *g_WinSock_WSAAsyncGetServByName = 0;

/* 0057432C g_WinSock_WSAAsyncGetServByPort */
__declspec(align(4)) WinSock_WSAAsyncGetServByPortProc *g_WinSock_WSAAsyncGetServByPort = 0;

/* 00574330 g_WinSock_WSAAsyncSelect */
__declspec(align(16)) WinSock_WSAAsyncSelectProc *g_WinSock_WSAAsyncSelect = 0;

/* 00574334 g_WinSock_WSACancelAsyncRequest */
__declspec(align(4)) WinSock_WSACancelAsyncRequestProc *g_WinSock_WSACancelAsyncRequest = 0;

/* 00574338 g_WinSock_WSACancelBlockingCall */
__declspec(align(8)) WinSock_WSACancelBlockingCallProc *g_WinSock_WSACancelBlockingCall = 0;

/* 0057433C g_WinSock_WSACleanup */
__declspec(align(4)) WinSock_WSACleanupProc *g_WinSock_WSACleanup = 0;

/* 00574340 g_WinSock_WSAGetLastError */
__declspec(align(16)) WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError = 0;

/* 00574344 g_WinSock_WSAIsBlocking */
__declspec(align(4)) WinSock_WSAIsBlockingProc *g_WinSock_WSAIsBlocking = 0;

/* 00574348 g_WinSock_WSASetBlockingHook */
__declspec(align(8)) WinSock_WSASetBlockingHookProc *g_WinSock_WSASetBlockingHook = 0;

/* 00574350 g_WinSock_WSAStartup */
__declspec(align(16)) WinSock_WSAStartupProc *g_WinSock_WSAStartup = 0;

/* 00574354 g_WinSock_WSAUnhookBlockingHook */
__declspec(align(4)) WinSock_WSAUnhookBlockingHookProc *g_WinSock_WSAUnhookBlockingHook = 0;

/* 0057435C g_Ws2_32_bind */
__declspec(align(4)) WinSock_bindProc *g_Ws2_32_bind = 0;

/* 00574360 g_Ws2_32_closesocket */
__declspec(align(16)) WinSock_closesocketProc *g_Ws2_32_closesocket = 0;

/* 00574378 g_Ws2_32_htons */
__declspec(align(8)) WinSock_htonsProc *g_Ws2_32_htons = 0;

/* 00574390 g_Ws2_32_recvfrom */
__declspec(align(16)) WinSock_recvfromProc *g_Ws2_32_recvfrom = 0;

/* 0057439C g_Ws2_32_sendto */
__declspec(align(4)) WinSock_sendtoProc *g_Ws2_32_sendto = 0;

/* 005743A0 g_Ws2_32_setsockopt */
__declspec(align(16)) WinSock_setsockoptProc *g_Ws2_32_setsockopt = 0;

/* 005743A8 g_Ws2_32_socket */
__declspec(align(8)) WinSock_socketProc *g_Ws2_32_socket = 0;

/* 005743B8 g_Ws2_32_WSACleanup */
__declspec(align(8)) WinSock_WSACleanupProc *g_Ws2_32_WSACleanup = 0;

/* 005743D8 g_Ws2_32_WSAGetLastError */
__declspec(align(8)) WinSock_WSAGetLastErrorProc *g_Ws2_32_WSAGetLastError = 0;

/* 005743EC g_Ws2_32_WSAIoctl */
__declspec(align(4)) WSAIoctl_Proc *g_Ws2_32_WSAIoctl = 0;

/* 00574444 g_Ws2_32_gethostbyname */
__declspec(align(4)) WinSock_gethostbynameProc *g_Ws2_32_gethostbyname = 0;

/* 00574478 g_Ws2_32_WSAAddressToStringA */
__declspec(align(8)) WSAAddressToStringA_Proc *g_Ws2_32_WSAAddressToStringA = 0;

/* 005744A0 g_Ws2_32_WSAStringToAddressA */
__declspec(align(16)) WSAStringToAddressA_Proc *g_Ws2_32_WSAStringToAddressA = 0;

/* 005744DC s_Wsock32ModuleName */
__declspec(align(4)) char s_Wsock32ModuleName[8] = "WSOCK32";

/* 00574D40 s_Wsock32Export_accept */
__declspec(align(16)) char s_Wsock32Export_accept[7] = "accept";

/* 00574D48 s_Wsock32Export_bind */
__declspec(align(8)) char s_Wsock32Export_bind[5] = "bind";

/* 00574D4E s_Wsock32Export_closesocket */
__declspec(align(4)) char s_Wsock32Export_closesocket[12] = "closesocket";

/* 00574D5A s_Wsock32Export_connect */
__declspec(align(4)) char s_Wsock32Export_connect[8] = "connect";

/* 00574D62 s_Wsock32Export_getpeername */
__declspec(align(4)) char s_Wsock32Export_getpeername[12] = "getpeername";

/* 00574D6E s_Wsock32Export_getsockname */
__declspec(align(4)) char s_Wsock32Export_getsockname[12] = "getsockname";

/* 00574D7A s_Wsock32Export_getsockopt */
__declspec(align(4)) char s_Wsock32Export_getsockopt[11] = "getsockopt";

/* 00574D86 s_Wsock32Export_htonl */
__declspec(align(4)) char s_Wsock32Export_htonl[6] = "htonl";

/* 00574D8C s_Wsock32Export_htons */
__declspec(align(4)) char s_Wsock32Export_htons[6] = "htons";

/* 00574D92 s_Wsock32Export_inet_addr */
__declspec(align(4)) char s_Wsock32Export_inet_addr[10] = "inet_addr";

/* 00574D9C s_Wsock32Export_inet_ntoa */
__declspec(align(4)) char s_Wsock32Export_inet_ntoa[10] = "inet_ntoa";

/* 00574DA6 s_Wsock32Export_ioctlsocket */
__declspec(align(4)) char s_Wsock32Export_ioctlsocket[12] = "ioctlsocket";

/* 00574DB2 s_Wsock32Export_listen */
__declspec(align(4)) char s_Wsock32Export_listen[7] = "listen";

/* 00574DBA s_Wsock32Export_ntohl */
__declspec(align(4)) char s_Wsock32Export_ntohl[6] = "ntohl";

/* 00574DC0 s_Wsock32Export_ntohs */
__declspec(align(16)) char s_Wsock32Export_ntohs[6] = "ntohs";

/* 00574DC6 s_Wsock32Export_recv */
__declspec(align(4)) char s_Wsock32Export_recv[5] = "recv";

/* 00574DCC s_Wsock32Export_recvfrom */
__declspec(align(4)) char s_Wsock32Export_recvfrom[9] = "recvfrom";

/* 00574DD6 s_Wsock32Export_select */
__declspec(align(4)) char s_Wsock32Export_select[7] = "select";

/* 00574DDE s_Wsock32Export_send */
__declspec(align(4)) char s_Wsock32Export_send[5] = "send";

/* 00574DE4 s_Wsock32Export_sendto */
__declspec(align(4)) char s_Wsock32Export_sendto[7] = "sendto";

/* 00574DEC s_Wsock32Export_setsockopt */
__declspec(align(4)) char s_Wsock32Export_setsockopt[11] = "setsockopt";

/* 00574DF8 s_Wsock32Export_shutdown */
__declspec(align(8)) char s_Wsock32Export_shutdown[9] = "shutdown";

/* 00574E02 s_Wsock32Export_socket */
__declspec(align(4)) char s_Wsock32Export_socket[7] = "socket";

/* 00574E0A s_Wsock32Export_gethostbyaddr */
__declspec(align(4)) char s_Wsock32Export_gethostbyaddr[14] = "gethostbyaddr";

/* 00574E18 s_Wsock32Export_gethostbyname */
__declspec(align(8)) char s_Wsock32Export_gethostbyname[14] = "gethostbyname";

/* 00574E26 s_Wsock32Export_gethostname */
__declspec(align(4)) char s_Wsock32Export_gethostname[12] = "gethostname";

/* 00574E32 s_Wsock32Export_getprotobyname */
__declspec(align(4)) char s_Wsock32Export_getprotobyname[15] = "getprotobyname";

/* 00574E42 s_Wsock32Export_getprotobynumber */
__declspec(align(4)) char s_Wsock32Export_getprotobynumber[17] = "getprotobynumber";

/* 00574E54 s_Wsock32Export_getservbyname */
__declspec(align(4)) char s_Wsock32Export_getservbyname[14] = "getservbyname";

/* 00574E62 s_Wsock32Export_getservbyport */
__declspec(align(4)) char s_Wsock32Export_getservbyport[14] = "getservbyport";

/* 00574E70 s_Wsock32Export_WSAAsyncGetHostByAddr */
__declspec(align(16)) char s_Wsock32Export_WSAAsyncGetHostByAddr[22] = "WSAAsyncGetHostByAddr";

/* 00574E86 s_Wsock32Export_WSAAsyncGetHostByName */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncGetHostByName[22] = "WSAAsyncGetHostByName";

/* 00574E9C s_Wsock32Export_WSAAsyncGetProtoByName */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncGetProtoByName[23] = "WSAAsyncGetProtoByName";

/* 00574EB4 s_Wsock32Export_WSAAsyncGetProtoByNumber */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncGetProtoByNumber[25] = "WSAAsyncGetProtoByNumber";

/* 00574ECE s_Wsock32Export_WSAAsyncGetServByName */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncGetServByName[22] = "WSAAsyncGetServByName";

/* 00574EE4 s_Wsock32Export_WSAAsyncGetServByPort */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncGetServByPort[22] = "WSAAsyncGetServByPort";

/* 00574EFA s_Wsock32Export_WSAAsyncSelect */
__declspec(align(4)) char s_Wsock32Export_WSAAsyncSelect[15] = "WSAAsyncSelect";

/* 00574F0A s_Wsock32Export_WSACancelAsyncRequest */
__declspec(align(4)) char s_Wsock32Export_WSACancelAsyncRequest[22] = "WSACancelAsyncRequest";

/* 00574F20 s_Wsock32Export_WSACancelBlockingCall */
__declspec(align(16)) char s_Wsock32Export_WSACancelBlockingCall[22] = "WSACancelBlockingCall";

/* 00574F36 s_Wsock32Export_WSACleanup */
__declspec(align(4)) char s_Wsock32Export_WSACleanup[11] = "WSACleanup";

/* 00574F42 s_Wsock32Export_WSAGetLastError */
__declspec(align(4)) char s_Wsock32Export_WSAGetLastError[16] = "WSAGetLastError";

/* 00574F52 s_Wsock32Export_WSAIsBlocking */
__declspec(align(4)) char s_Wsock32Export_WSAIsBlocking[14] = "WSAIsBlocking";

/* 00574F60 s_Wsock32Export_WSASetBlockingHook */
__declspec(align(16)) char s_Wsock32Export_WSASetBlockingHook[19] = "WSASetBlockingHook";

/* 00574F84 s_Wsock32Export_WSAStartup */
__declspec(align(4)) char s_Wsock32Export_WSAStartup[11] = "WSAStartup";

/* 00574F90 s_Wsock32Export_WSAUnhookBlockingHook */
__declspec(align(16)) char s_Wsock32Export_WSAUnhookBlockingHook[22] = "WSAUnhookBlockingHook";

/* 00583D60 g_NetworkBackendMode */
__declspec(align(16)) uint32_t g_NetworkBackendMode = 0;

/* 00583D64 g_WinSockStartupData */
__declspec(align(4)) WinSockData11 g_WinSockStartupData = {0};

/* 00583EF2 g_NetworkFallbackSocket */
__declspec(align(4)) NetworkSocketHandle32 g_NetworkFallbackSocket = 0xFFFFFFFF;

/* 00583EF6 g_NetworkFallbackSocketOptionOn: uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO); network/backend/fallback_udp.c */
__declspec(align(4)) uint32_t g_NetworkFallbackSocketOptionOn = 4294967295u;

/* 00583EFA g_NetworkFallbackAddressLength */
__declspec(align(4)) uint32_t g_NetworkFallbackAddressLength = 0;

/* 00583EFE g_NetworkFallbackBindEndpoint */
__declspec(align(4)) WinSockAddress g_NetworkFallbackBindEndpoint = {0};

/* 00583F0E g_NetworkBackendActiveAddressFamily */
__declspec(align(4)) uint32_t g_NetworkBackendActiveAddressFamily = 0;

/* 00583F12 g_NetworkBackendActiveSocketAddressLength */
__declspec(align(4)) uint32_t g_NetworkBackendActiveSocketAddressLength = 0;

/* 00583F16 g_NetworkBackendActiveSocketType */
__declspec(align(4)) uint32_t g_NetworkBackendActiveSocketType = 0;

/* 00583F1A g_NetworkBackendActiveProtocol */
__declspec(align(4)) uint32_t g_NetworkBackendActiveProtocol = 0;

/* 00583F1E g_NetworkBackendPortNetworkOrderCarrier */
__declspec(align(4)) uint32_t g_NetworkBackendPortNetworkOrderCarrier = 0;

/* 00583F22 g_NetworkBackendBindAddress */
__declspec(align(4)) NetworkBackendSocketAddress16 g_NetworkBackendBindAddress = {0};

/* 00583F40 g_NetworkEndpointTextScratchA */
__declspec(align(16)) uint8_t g_NetworkEndpointTextScratchA[256] = {0};

/* 00584040 NetworkBackendInstanceDescriptorPrefix_00584040 */
__declspec(align(16)) NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040 = {.displayNameUtf16 = L"WinSock32 1.1 - UDP"};

/* 00584078 s_CommandLineOptionIp: Original quirk: "IP=" has no terminator (the next byte is 0x90 filler);
   g_CommandLineFindOption gets the length 3 and never reads past it */
__declspec(align(8)) char s_CommandLineOptionIp[3] = {'I', 'P', '='};
