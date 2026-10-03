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

/* 005742A4 g_WinSock_bind */
__declspec(align(4)) WinSock_bindProc *g_WinSock_bind = 0;

/* 005742A8 g_WinSock_closesocket */
__declspec(align(8)) WinSock_closesocketProc *g_WinSock_closesocket = 0;

/* 005742C0 g_WinSock_htons */
__declspec(align(16)) WinSock_htonsProc *g_WinSock_htons = 0;

/* 005742C4 g_WinSock_inet_addr */
__declspec(align(4)) WinSock_inet_addrProc *g_WinSock_inet_addr = 0;

/* 005742C8 g_WinSock_inet_ntoa */
__declspec(align(8)) WinSock_inet_ntoaProc *g_WinSock_inet_ntoa = 0;

/* 005742CC g_WinSock_ioctlsocket */
__declspec(align(4)) WinSock_ioctlsocketProc *g_WinSock_ioctlsocket = 0;

/* 005742E0 g_WinSock_recvfrom */
__declspec(align(16)) WinSock_recvfromProc *g_WinSock_recvfrom = 0;

/* 005742EC g_WinSock_sendto */
__declspec(align(4)) WinSock_sendtoProc *g_WinSock_sendto = 0;

/* 005742F0 g_WinSock_setsockopt */
__declspec(align(16)) WinSock_setsockoptProc *g_WinSock_setsockopt = 0;

/* 005742F8 g_WinSock_socket */
__declspec(align(8)) WinSock_socketProc *g_WinSock_socket = 0;

/* 00574300 g_WinSock_gethostbyname */
__declspec(align(16)) WinSock_gethostbynameProc *g_WinSock_gethostbyname = 0;

/* 0057433C g_WinSock_WSACleanup */
__declspec(align(4)) WinSock_WSACleanupProc *g_WinSock_WSACleanup = 0;

/* 00574340 g_WinSock_WSAGetLastError */
__declspec(align(16)) WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError = 0;

/* 00574350 g_WinSock_WSAStartup */
__declspec(align(16)) WinSock_WSAStartupProc *g_WinSock_WSAStartup = 0;

/* 005744DC s_Wsock32ModuleName */
__declspec(align(4)) char s_Wsock32ModuleName[8] = "WSOCK32";

/* 00574D48 s_Wsock32Export_bind */
__declspec(align(8)) char s_Wsock32Export_bind[5] = "bind";

/* 00574D4E s_Wsock32Export_closesocket */
__declspec(align(4)) char s_Wsock32Export_closesocket[12] = "closesocket";

/* 00574D8C s_Wsock32Export_htons */
__declspec(align(4)) char s_Wsock32Export_htons[6] = "htons";

/* 00574D92 s_Wsock32Export_inet_addr */
__declspec(align(4)) char s_Wsock32Export_inet_addr[10] = "inet_addr";

/* 00574D9C s_Wsock32Export_inet_ntoa */
__declspec(align(4)) char s_Wsock32Export_inet_ntoa[10] = "inet_ntoa";

/* 00574DA6 s_Wsock32Export_ioctlsocket */
__declspec(align(4)) char s_Wsock32Export_ioctlsocket[12] = "ioctlsocket";

/* 00574DCC s_Wsock32Export_recvfrom */
__declspec(align(4)) char s_Wsock32Export_recvfrom[9] = "recvfrom";

/* 00574DE4 s_Wsock32Export_sendto */
__declspec(align(4)) char s_Wsock32Export_sendto[7] = "sendto";

/* 00574DEC s_Wsock32Export_setsockopt */
__declspec(align(4)) char s_Wsock32Export_setsockopt[11] = "setsockopt";

/* 00574E02 s_Wsock32Export_socket */
__declspec(align(4)) char s_Wsock32Export_socket[7] = "socket";

/* 00574E18 s_Wsock32Export_gethostbyname */
__declspec(align(8)) char s_Wsock32Export_gethostbyname[14] = "gethostbyname";

/* 00574F36 s_Wsock32Export_WSACleanup */
__declspec(align(4)) char s_Wsock32Export_WSACleanup[11] = "WSACleanup";

/* 00574F42 s_Wsock32Export_WSAGetLastError */
__declspec(align(4)) char s_Wsock32Export_WSAGetLastError[16] = "WSAGetLastError";

/* 00574F84 s_Wsock32Export_WSAStartup */
__declspec(align(4)) char s_Wsock32Export_WSAStartup[11] = "WSAStartup";

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

/* 00583F40 g_NetworkEndpointTextScratchA */
__declspec(align(16)) uint8_t g_NetworkEndpointTextScratchA[256] = {0};

/* 00584040 NetworkBackendInstanceDescriptorPrefix_00584040 */
__declspec(align(16)) NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040 = {.displayNameUtf16 = L"WinSock32 1.1 - UDP"};

/* 00584078 s_CommandLineOptionIp: Original quirk: "IP=" has no terminator (the next byte is 0x90 filler);
   g_CommandLineFindOption gets the length 3 and never reads past it */
__declspec(align(8)) char s_CommandLineOptionIp[3] = {'I', 'P', '='};
