/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/network/backend/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable = 0;

__declspec(align(8)) NetworkSessionContext *g_NetworkBackendSessionContext = 0;

__declspec(align(4)) NetworkBackendReceiveCallback *g_NetworkBackendSlot4 = (void *)NetworkBackendFallback_ReceiveDatagram;

__declspec(align(16)) NetworkBackendSendCallback *g_NetworkBackendSlot5 = (void *)NetworkBackendFallback_SendDatagram;

__declspec(align(4)) UiTransferEndpointDescriptor g_NetworkLocalEndpoint = {0};

/* uint32_t sender context of the last executed network batch (0xFFFFFFFF = none); network/backend and protocol/transfer */
__declspec(align(4)) uint32_t g_FrontendSelectedPlayerToken = 1;

__declspec(align(4)) uint32_t g_FrontendHostSnapshotTransferCountdown = 0;

__declspec(align(16)) FrontendPacket30005PlayerSnapshot g_FrontendPacket30005Buffer = {0};

__declspec(align(16)) FrontendPacket10009SnapshotChunkRequest g_FrontendPacket10009Buffer = {0};

__declspec(align(16)) FrontendCommandPacketRecord g_FrontendPlayerCommandRecords[8] = {0};

__declspec(align(16)) FrontendCommandPacketRecord g_FrontendCommandBatchPacketBuffer[8] = {0};

__declspec(align(16)) FrontendPacket10012SyncPending g_FrontendPacket10012Buffer = {0};

__declspec(align(4)) WinSock_bindProc *g_WinSock_bind = 0;

__declspec(align(8)) WinSock_closesocketProc *g_WinSock_closesocket = 0;

__declspec(align(16)) WinSock_htonsProc *g_WinSock_htons = 0;

__declspec(align(4)) WinSock_inet_addrProc *g_WinSock_inet_addr = 0;

__declspec(align(8)) WinSock_inet_ntoaProc *g_WinSock_inet_ntoa = 0;

__declspec(align(4)) WinSock_ioctlsocketProc *g_WinSock_ioctlsocket = 0;

__declspec(align(16)) WinSock_recvfromProc *g_WinSock_recvfrom = 0;

__declspec(align(4)) WinSock_sendtoProc *g_WinSock_sendto = 0;

__declspec(align(16)) WinSock_setsockoptProc *g_WinSock_setsockopt = 0;

__declspec(align(8)) WinSock_socketProc *g_WinSock_socket = 0;

__declspec(align(16)) WinSock_gethostbynameProc *g_WinSock_gethostbyname = 0;

__declspec(align(4)) WinSock_WSACleanupProc *g_WinSock_WSACleanup = 0;

__declspec(align(16)) WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError = 0;

__declspec(align(16)) WinSock_WSAStartupProc *g_WinSock_WSAStartup = 0;

__declspec(align(4)) char s_Wsock32ModuleName[8] = "WSOCK32";

__declspec(align(8)) char s_Wsock32Export_bind[5] = "bind";

__declspec(align(4)) char s_Wsock32Export_closesocket[12] = "closesocket";

__declspec(align(4)) char s_Wsock32Export_htons[6] = "htons";

__declspec(align(4)) char s_Wsock32Export_inet_addr[10] = "inet_addr";

__declspec(align(4)) char s_Wsock32Export_inet_ntoa[10] = "inet_ntoa";

__declspec(align(4)) char s_Wsock32Export_ioctlsocket[12] = "ioctlsocket";

__declspec(align(4)) char s_Wsock32Export_recvfrom[9] = "recvfrom";

__declspec(align(4)) char s_Wsock32Export_sendto[7] = "sendto";

__declspec(align(4)) char s_Wsock32Export_setsockopt[11] = "setsockopt";

__declspec(align(4)) char s_Wsock32Export_socket[7] = "socket";

__declspec(align(8)) char s_Wsock32Export_gethostbyname[14] = "gethostbyname";

__declspec(align(4)) char s_Wsock32Export_WSACleanup[11] = "WSACleanup";

__declspec(align(4)) char s_Wsock32Export_WSAGetLastError[16] = "WSAGetLastError";

__declspec(align(4)) char s_Wsock32Export_WSAStartup[11] = "WSAStartup";

__declspec(align(16)) uint32_t g_NetworkBackendMode = 0;

__declspec(align(4)) WinSockData11 g_WinSockStartupData = {0};

__declspec(align(4)) NetworkSocketHandle32 g_NetworkFallbackSocket = 0xFFFFFFFF;

/* uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO); network/backend/fallback_udp.c */
__declspec(align(4)) uint32_t g_NetworkFallbackSocketOptionOn = 4294967295u;

__declspec(align(4)) uint32_t g_NetworkFallbackAddressLength = 0;

__declspec(align(4)) WinSockAddress g_NetworkFallbackBindEndpoint = {0};

__declspec(align(16)) uint8_t g_NetworkEndpointTextScratchA[256] = {0};

__declspec(align(16)) NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix_00584040 = {.displayNameUtf16 = L"WinSock32 1.1 - UDP"};

/* Original quirk: "IP=" has no terminator (the next byte is 0x90 filler);
   g_CommandLineFindOption gets the length 3 and never reads past it */
__declspec(align(8)) char s_CommandLineOptionIp[3] = {'I', 'P', '='};
