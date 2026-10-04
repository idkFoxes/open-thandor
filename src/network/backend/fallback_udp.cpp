/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/fallback_udp.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/fallback_udp.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

UiTransferEndpointDescriptor g_NetworkLocalEndpoint = {0};

static NetworkSocketHandle32 g_NetworkFallbackSocket = 0xFFFFFFFF;

/* uint32_t: nonzero value (0xFFFFFFFF) passed to setsockopt(SO_BROADCAST) and ioctlsocket(FIONBIO) */
static uint32_t g_NetworkFallbackSocketOptionOn = 4294967295u;

static uint32_t g_NetworkFallbackAddressLength = 0;

static WinSockAddress g_NetworkFallbackBindEndpoint = {0};

/* narrow endpoint/address text (written with capacity 255) */
static uint8_t g_NetworkEndpointTextScratchA[256] = {0};

/* Original quirk: "IP=" has no terminator (the next byte is 0x90 filler);
   g_CommandLineFindOption gets the length 3 and never reads past it */
static char s_CommandLineOptionIp[3] = {'I', 'P', '='};

/* Implementation ownership: network/backend/fallback_udp. */

/* Default g_NetworkBackendSlot0 ("select backend instance") in the image data, active until Network_Init
   installs the WinSock backend: every backend index fails, returning FATAL_ERROR_NETWORK_UNAVAILABLE.
*/
uint32_t NetworkBackendFallback_SetSessionContext(uint32_t backendIndex)

{
  return FATAL_ERROR_NETWORK_UNAVAILABLE;
}


/* Default g_NetworkBackendSlot1 (backend cleanup) in the image data: nothing to clean up without a backend.
*/
void __cdecl NetworkBackendFallback_Cleanup()

{
}

/* Default g_NetworkBackendSlot2 (open and bind the socket) in the image data: without WinSock no socket can
   be opened, so it returns FATAL_ERROR_NETWORK_UNAVAILABLE.
*/
uint32_t NetworkBackendFallback_OpenAndBindUdpSocket(uint32_t localPort)

{
  return FATAL_ERROR_NETWORK_UNAVAILABLE;
}


/* Default g_NetworkBackendSlot3 (close the socket) in the image data: there is no socket to close.
*/
void __cdecl NetworkBackendFallback_CloseActiveSocket()

{
}

/* Default g_NetworkBackendSlot4 (receive a datagram) in the image data: returns false (nothing received), so
   UiTransfer receive loops stop at once.
*/
Bool8 NetworkBackendFallback_ReceiveDatagram
               (WinSockAddress *sourceAddress,uint32_t byteCount,uint8_t *buffer)

{
  return false;
}

/* Default g_NetworkBackendSlot5 (send a datagram) in the image data, called by UiTransfer_StagePacketAndSend:
   drops the packet and returns true (success) so a session without network keeps running.
*/
Bool8 NetworkBackendFallback_SendDatagram
               (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  return true;
}

/* Default g_NetworkBackendSlot6 (parse a typed peer address) in the image data: always fails (returns true).
*/
Bool8 NetworkBackendFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpoint,char *endpointText)

{
  return true;
}


/* Default g_NetworkBackendSlot7 (format a peer address as text) in the image data: writes an empty UTF-16
   string (one zero dword) to outputText and ignores the address.
*/
void NetworkBackendFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  outputText[0] = '\0';
  outputText[1] = '\0';
  outputText[2] = '\0';
  outputText[3] = '\0';
}

/* Cleanup slot of the WinSock UDP backend. Nothing to release here: the socket is closed by
   NetworkFallback_CloseActiveSocket and WinSock itself by Network_Shutdown.
*/
void NetworkFallback_NoOpBackendCleanup()

{
}


/* The bind address from the -IP="host" command-line option: the quoted dotted address or host name,
   resolved through WinSock. 0 (INADDR_ANY) when the option is missing, not quoted, the quote is not the
   option's last character (or never closed), or the host name is unknown. The quoted text is copied into
   g_PackageScratchBuffer.
*/
static NetworkIpv4AddressNetworkOrder NetworkFallback_ResolveIpOptionAddress()

{
  uint8_t copiedByte;
  WinSockHostEnt32 *hostEntry;
  NetworkIpv4AddressNetworkOrder bindAddress;
  uint8_t *optionCursor;
  uint8_t *outputCursor;
  uint8_t *ipOption;

  ipOption = g_CommandLineFindOption(3,s_CommandLineOptionIp); /* "IP=" */
  if ((ipOption == nullptr) || (ipOption[3] != '"')) {
    return 0;
  }
  /* copy the quoted value, up to and including the closing quote (or the terminator), into the scratch
     buffer; outputCursor ends on the last byte copied */
  optionCursor = ipOption + 4;
  outputCursor = g_PackageScratchBuffer;
  copiedByte = *optionCursor;
  *outputCursor = copiedByte;
  optionCursor++;
  while ((copiedByte != 0) && (copiedByte != '"')) {
    outputCursor++;
    copiedByte = *optionCursor;
    *outputCursor = copiedByte;
    optionCursor++;
  }
  /* only a closing quote that ends the option counts */
  if ((copiedByte != '"') || (*optionCursor != 0)) {
    return 0;
  }
  *outputCursor = 0;
  bindAddress = g_WinSock_inet_addr(g_PackageScratchBuffer);
  if (bindAddress == INADDR_NONE) {
    hostEntry = g_WinSock_gethostbyname(g_PackageScratchBuffer);
    bindAddress = 0;
    if (hostEntry != nullptr) {
      bindAddress = *(NetworkIpv4AddressNetworkOrder *)*hostEntry->addressList;
    }
  }
  return bindAddress;
}

/* Failure exit of NetworkFallback_OpenAndBindUdpSocket: leaves the WinSock error code as decimal text in
   g_PackageLastErrorPath (for the fatal-error message), closes socketToClose unless it is INVALID_SOCKET
   and returns FATAL_ERROR_NETWORK_SOCKET.
*/
static uint32_t NetworkFallback_FailSocketSetup(uint32_t socketToClose)

{
  int winsockErrorCode;

  winsockErrorCode = g_WinSock_WSAGetLastError();
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
  if (socketToClose != INVALID_SOCKET) {
    g_WinSock_closesocket(socketToClose);
  }
  return FATAL_ERROR_NETWORK_SOCKET;
}

/* Opens the game's UDP socket: bound to localPort on all interfaces, or on the address given as
   -IP="host" on the command line (dotted address or host name), with broadcast allowed and non-blocking
   I/O. Also presets the local endpoint descriptor to the IPv4 broadcast address on that port, used for
   session discovery. On failure the WinSock error code is left in g_PackageLastErrorPath and
   FATAL_ERROR_NETWORK_SOCKET is returned; 0 on success (the socket is kept in g_NetworkFallbackSocket).
*/
uint32_t NetworkFallback_OpenAndBindUdpSocket(NetworkPortHostOrder localPort)

{
  NetworkIpv4AddressNetworkOrder bindAddress;
  uint32_t socketHandle;
  int bindResult;

  socketHandle = g_WinSock_socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if (socketHandle == INVALID_SOCKET) {
    return NetworkFallback_FailSocketSetup(INVALID_SOCKET);
  }
  bindAddress = NetworkFallback_ResolveIpOptionAddress();
  g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
       g_WinSock_htons((uint16_t)localPort);
  g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = bindAddress;
  g_NetworkFallbackBindEndpoint.addressHeader.fields.addressFamily = NETWORK_ADDRESS_FAMILY_IPV4;
  g_NetworkFallbackBindEndpoint.zeroPadding[0] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[1] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[2] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[3] = 0;
  /* the local descriptor gets the same family and port (family in the low word, port in the high word) */
  g_NetworkLocalEndpoint.addressHeader.packedFamilyAndPort =
       (uint32_t)g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder << 16 |
       NETWORK_ADDRESS_FAMILY_IPV4;
  g_NetworkFallbackBindEndpoint.zeroPadding[4] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[5] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[6] = 0;
  g_NetworkFallbackBindEndpoint.zeroPadding[7] = 0;
  /* developer tools (OPEN_THANDOR_NET_PORT, not in the original): a second instance on this machine binds to
     another port; the endpoint keeps the game port for the code that reads it later */
  DebugHook_BeforeUdpBind(&g_NetworkFallbackBindEndpoint,localPort);
  bindResult = g_WinSock_bind(socketHandle,&g_NetworkFallbackBindEndpoint,sizeof(WinSockAddress));
  DebugHook_AfterUdpBind(&g_NetworkFallbackBindEndpoint,localPort);
  if (bindResult != 0) {
    return NetworkFallback_FailSocketSetup(socketHandle);
  }
  /* g_NetworkFallbackSocketOptionOn holds a nonzero value (0xFFFFFFFF): enable SO_BROADCAST and non-blocking mode */
  if (g_WinSock_setsockopt(socketHandle,SOL_SOCKET,SO_BROADCAST,
                           (uint8_t *)&g_NetworkFallbackSocketOptionOn,4) != 0) {
    return NetworkFallback_FailSocketSetup(socketHandle);
  }
  if (g_WinSock_ioctlsocket(socketHandle,FIONBIO,&g_NetworkFallbackSocketOptionOn) != 0) {
    return NetworkFallback_FailSocketSetup(socketHandle);
  }
  g_NetworkLocalEndpoint.ipv4AddressNetworkOrder = INADDR_BROADCAST;
  g_NetworkLocalEndpoint.zeroPadding[0] = 0;
  g_NetworkLocalEndpoint.zeroPadding[1] = 0;
  g_NetworkLocalEndpoint.zeroPadding[2] = 0;
  g_NetworkLocalEndpoint.zeroPadding[3] = 0;
  g_NetworkLocalEndpoint.zeroPadding[4] = 0;
  g_NetworkLocalEndpoint.zeroPadding[5] = 0;
  g_NetworkLocalEndpoint.zeroPadding[6] = 0;
  g_NetworkLocalEndpoint.zeroPadding[7] = 0;
  g_NetworkFallbackSocket = socketHandle;
  return 0;
}


/* Closes the UDP socket, if one is open. The handle is swapped out atomically before
   closesocket so that nobody uses the socket while it is being closed.
*/
void NetworkFallback_CloseActiveSocket()

{
  NetworkSocketHandle32 socket;

  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    /* atomic exchange: the timer thread sends and receives on this socket */
    socket = (NetworkSocketHandle32)THANDOR_ATOMIC_EXCHANGE(&g_NetworkFallbackSocket,INVALID_SOCKET);
    g_WinSock_closesocket(socket);
  }
}


/* Receives one datagram (non-blocking) into buffer and the sender's address into sourceAddress
   (16-byte sockaddr_in). Returns true when a datagram was received (callers do not need the byte count:
   the packet header carries its size); false when no socket is open or recvfrom fails, including
   WSAEWOULDBLOCK when nothing is pending.
*/
Bool8 NetworkFallback_ReceiveDatagram
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  int receivedByteCount;

  g_NetworkFallbackAddressLength = sizeof(WinSockAddress);
  if (g_NetworkFallbackSocket == INVALID_SOCKET) {
    return false;
  }
  receivedByteCount =
       g_WinSock_recvfrom
                 (g_NetworkFallbackSocket,buffer,byteCount,0,sourceAddress,
                  (int *)&g_NetworkFallbackAddressLength);
  if (receivedByteCount < 0) {
    return false;
  }
  DebugHook_UdpDatagram("recv",sourceAddress,receivedByteCount,buffer);
  return true;
}


/* Sends one datagram to destinationAddress (16-byte sockaddr_in) and returns true. Without an open
   socket nothing is sent and the call still succeeds. A sendto error leaves the WinSock error code in
   g_PackageLastErrorPath and returns false (the original also reported failure and returned
   FATAL_ERROR_NETWORK_SOCKET, but no caller reads the code).
*/
Bool8 NetworkFallback_SendDatagram
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t sentByteCount;
  int winsockErrorCode;

  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    DebugHook_UdpDatagram("send",destinationAddress,byteCount,buffer);
    sentByteCount =
         g_WinSock_sendto(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,sizeof(WinSockAddress));
    if ((int)sentByteCount < 0) {
      winsockErrorCode = g_WinSock_WSAGetLastError();
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      return false;
    }
  }
  return true;
}


/* Turns the UTF-16 peer address typed by the player (dotted address or host name) into a 16-byte
   sockaddr_in with the game's port. An empty text yields the broadcast address from the local
   endpoint descriptor. Returns true when the text does not convert or the host is
   unknown, false on success.
*/
Bool8 NetworkFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText)

{
  NetworkEndpointAddressHeader4 bindAddressHeader;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;

  if (!RichTextCommandStream_CopyToNarrow
                    (255,g_NetworkEndpointTextScratchA,(uint16_t *)endpointText)) {
    return true;
  }
  /* empty text: the broadcast address of the local descriptor */
  ipv4AddressNetworkOrder = g_NetworkLocalEndpoint.ipv4AddressNetworkOrder;
  if (g_NetworkEndpointTextScratchA[0] != '\0') {
    ipv4AddressNetworkOrder = g_WinSock_inet_addr(g_NetworkEndpointTextScratchA);
    if (ipv4AddressNetworkOrder == INADDR_NONE) {
      /* not a dotted address: look the host name up */
      resolvedHostEntry = g_WinSock_gethostbyname(g_NetworkEndpointTextScratchA);
      if (resolvedHostEntry == nullptr) {
        return true;
      }
      ipv4AddressNetworkOrder = *(NetworkIpv4AddressNetworkOrder *)*resolvedHostEntry->addressList;
    }
  }
  bindAddressHeader = g_NetworkFallbackBindEndpoint.addressHeader;
  endpointDescriptor16->zeroPadding[0] = 0;
  endpointDescriptor16->zeroPadding[1] = 0;
  endpointDescriptor16->zeroPadding[2] = 0;
  endpointDescriptor16->zeroPadding[3] = 0;
  endpointDescriptor16->zeroPadding[4] = 0;
  endpointDescriptor16->zeroPadding[5] = 0;
  endpointDescriptor16->zeroPadding[6] = 0;
  endpointDescriptor16->zeroPadding[7] = 0;
  endpointDescriptor16->addressHeader = bindAddressHeader;
  endpointDescriptor16->ipv4AddressNetworkOrder = ipv4AddressNetworkOrder;
  return false;
}


/* Writes the IPv4 address of socketAddress as dotted UTF-16 text (at most 0x200 bytes) into
   outputText, for showing a peer's address; an empty string when inet_ntoa fails.
*/
void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  uint8_t *dottedAddress;

  dottedAddress = g_WinSock_inet_ntoa(socketAddress->ipv4AddressNetworkOrder);
  if (dottedAddress != nullptr) {
    Text_CopyNarrowToUtf16(512,(uint16_t *)outputText,dottedAddress);
    return;
  }
  /* outputText is really UTF-16: the four zero bytes are an empty string (a single dword store) */
  outputText[0] = '\0';
  outputText[1] = '\0';
  outputText[2] = '\0';
  outputText[3] = '\0';
}

