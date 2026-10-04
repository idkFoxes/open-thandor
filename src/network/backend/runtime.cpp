/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

THANDOR_ALIGN(4) uint32_t g_NetworkBackendInstanceCount = 0;

THANDOR_ALIGN(4) NetworkBackendSetSessionCallback *g_NetworkBackendSlot0 = &NetworkBackendFallback_SetSessionContext;

THANDOR_ALIGN(16) NetworkBackendCleanupCallback *g_NetworkBackendSlot1 = &NetworkBackendFallback_Cleanup;

THANDOR_ALIGN(4) NetworkBackendOpenBindCallback *g_NetworkBackendSlot2 = &NetworkBackendFallback_OpenAndBindUdpSocket;

THANDOR_ALIGN(8) NetworkBackendCloseCallback *g_NetworkBackendSlot3 = &NetworkBackendFallback_CloseActiveSocket;

THANDOR_ALIGN(4) NetworkBackendParseEndpointCallback *g_NetworkBackendSlot6 = &NetworkBackendFallback_ParsePeerEndpoint;

THANDOR_ALIGN(8) NetworkBackendFormatAddressCallback *g_NetworkBackendSlot7 = &NetworkBackendFallback_FormatPeerAddress;

NetworkBackendInstanceDescriptorPrefix *g_NetworkBackendInstanceTable = nullptr;

NetworkBackendReceiveCallback *g_NetworkBackendSlot4 = &NetworkBackendFallback_ReceiveDatagram;

NetworkBackendSendCallback *g_NetworkBackendSlot5 = &NetworkBackendFallback_SendDatagram;

WinSock_bindProc *g_WinSock_bind = nullptr;

WinSock_closesocketProc *g_WinSock_closesocket = nullptr;

WinSock_htonsProc *g_WinSock_htons = nullptr;

WinSock_inet_addrProc *g_WinSock_inet_addr = nullptr;

WinSock_inet_ntoaProc *g_WinSock_inet_ntoa = nullptr;

WinSock_ioctlsocketProc *g_WinSock_ioctlsocket = nullptr;

WinSock_recvfromProc *g_WinSock_recvfrom = nullptr;

WinSock_sendtoProc *g_WinSock_sendto = nullptr;

WinSock_setsockoptProc *g_WinSock_setsockopt = nullptr;

WinSock_socketProc *g_WinSock_socket = nullptr;

WinSock_gethostbynameProc *g_WinSock_gethostbyname = nullptr;

WinSock_WSAGetLastErrorProc *g_WinSock_WSAGetLastError = nullptr;

static WinSock_WSACleanupProc *g_WinSock_WSACleanup = nullptr;

static WinSock_WSAStartupProc *g_WinSock_WSAStartup = nullptr;

static char s_Wsock32ModuleName[8] = "WSOCK32";

static char s_Wsock32Export_bind[5] = "bind";

static char s_Wsock32Export_closesocket[12] = "closesocket";

static char s_Wsock32Export_htons[6] = "htons";

static char s_Wsock32Export_inet_addr[10] = "inet_addr";

static char s_Wsock32Export_inet_ntoa[10] = "inet_ntoa";

static char s_Wsock32Export_ioctlsocket[12] = "ioctlsocket";

static char s_Wsock32Export_recvfrom[9] = "recvfrom";

static char s_Wsock32Export_sendto[7] = "sendto";

static char s_Wsock32Export_setsockopt[11] = "setsockopt";

static char s_Wsock32Export_socket[7] = "socket";

static char s_Wsock32Export_gethostbyname[14] = "gethostbyname";

static char s_Wsock32Export_WSACleanup[11] = "WSACleanup";

static char s_Wsock32Export_WSAGetLastError[16] = "WSAGetLastError";

static char s_Wsock32Export_WSAStartup[11] = "WSAStartup";

static uint32_t g_NetworkBackendMode = 0;

static WinSockData11 g_WinSockStartupData = {0};

static NetworkBackendInstanceDescriptorPrefix g_NetworkBackendInstanceDescriptorPrefix = {.displayNameUtf16 = {'W', 'i', 'n', 'S', 'o', 'c', 'k', '3', '2', ' ', '1', '.', '1', ' ', '-', ' ', 'U', 'D', 'P', 0}}; /* L"WinSock32 1.1 - UDP" */

/* Implementation ownership: network/backend/runtime. */

/* Binds the 14 exports of wsock32.dll that the code calls (the original binds 45), starts WinSock 1.1 and
   installs the UDP fallback backend
   (NetworkFallback_*) as the only network backend instance. Returns 0 on success, otherwise the
   DynDLL/DynAPI error code or the WSAStartup error; the original reports success in every case,
   so a missing WinSock is not fatal there. The original starts by jumping over a large block of code,
   presumably a ws2_32 path (not ported: nothing installed it).
*/
uint32_t __cdecl Network_Init()

{
  HINSTANCE module;
  uint32_t startupError;
  uint32_t resolveError;

  module = DynDLL_Load(s_Wsock32ModuleName);
  if (module == nullptr) return FATAL_ERROR_DLL_LOAD_FAILED;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_bind,module,s_Wsock32Export_bind);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_closesocket,module,s_Wsock32Export_closesocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_htons,module,s_Wsock32Export_htons);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_inet_addr,module,s_Wsock32Export_inet_addr);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_inet_ntoa,module,s_Wsock32Export_inet_ntoa);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_ioctlsocket,module,s_Wsock32Export_ioctlsocket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_recvfrom,module,s_Wsock32Export_recvfrom);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_sendto,module,s_Wsock32Export_sendto);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_setsockopt,module,s_Wsock32Export_setsockopt);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_socket,module,s_Wsock32Export_socket);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_gethostbyname,module,s_Wsock32Export_gethostbyname);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_WSACleanup,module,s_Wsock32Export_WSACleanup);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_WSAGetLastError,module,s_Wsock32Export_WSAGetLastError);
  if (resolveError != 0) return resolveError;
  resolveError = DynAPI_Resolve((void **)&g_WinSock_WSAStartup,module,s_Wsock32Export_WSAStartup);
  if (resolveError != 0) return resolveError;
  startupError = (uint32_t)g_WinSock_WSAStartup(MAKEWORD(1,1),&g_WinSockStartupData);
  if (startupError != 0) return startupError;
  g_NetworkBackendMode = NETWORK_BACKEND_MODE_WSOCK32;
  g_NetworkBackendSlot0 = NetworkBackend_SetSessionContext;
  g_NetworkBackendSlot1 = NetworkFallback_NoOpBackendCleanup;
  g_NetworkBackendSlot2 = NetworkFallback_OpenAndBindUdpSocket;
  g_NetworkBackendSlot3 = NetworkFallback_CloseActiveSocket;
  g_NetworkBackendSlot4 = NetworkFallback_ReceiveDatagram;
  g_NetworkBackendSlot5 = NetworkFallback_SendDatagram;
  g_NetworkBackendSlot6 = NetworkFallback_ParsePeerEndpoint;
  g_NetworkBackendSlot7 = NetworkFallback_FormatPeerAddress;
  g_NetworkBackendInstanceTable = &g_NetworkBackendInstanceDescriptorPrefix;
  g_NetworkBackendInstanceCount = 1;
  return 0;
}

/* Stops WinSock at program end: WSACleanup of the DLL that Network_Init started.
*/
void Network_Shutdown()

{
  if (g_NetworkBackendMode == NETWORK_BACKEND_MODE_WSOCK32) {
    g_WinSock_WSACleanup();
    g_NetworkBackendMode = NETWORK_BACKEND_MODE_NONE;
    return;
  }
  return;
}

/* Backend slot 0 ("select backend instance") of the wsock32 backend, which has a single instance: it
   accepts any backendIndex and always returns 0 (success). The original also stored a leftover value in a
   global (g_NetworkBackendSessionContext) that nothing read.
*/
uint32_t NetworkBackend_SetSessionContext(uint32_t backendIndex)

{
  (void)backendIndex;
  return 0;
}
