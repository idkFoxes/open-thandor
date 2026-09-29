/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/fallback_udp.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/fallback_udp.h>
#include <thandor/thandor.h>
#ifdef THANDOR_TEST_AIDS
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/test_aids.h>
#endif

/* Implementation ownership: network/backend/fallback_udp. */

/* Address: 0x0041A580.
   Default g_NetworkBackendSlot0 ("select backend instance") in the image data, active until Network_Init
   installs the WinSock backend: every backend index fails with FATAL_ERROR_NETWORK_UNAVAILABLE (CF set).
*/
StatusResult NetworkBackendFallback_SetSessionContext(uint32_t backendIndex)

{
  StatusResult status;
  
  status.failed = true;
  status.valueOrError = FATAL_ERROR_NETWORK_UNAVAILABLE;
  return status;
}


/* Address: 0x0041A590.
   Default g_NetworkBackendSlot1 (backend cleanup) in the image data: nothing to clean up without a backend.
*/
void __cdecl NetworkBackendFallback_Cleanup(void)

{
  return;
}

/* Address: 0x0041A5A0.
   Default g_NetworkBackendSlot2 (open and bind the socket) in the image data: without WinSock no socket can
   be opened, so it fails with FATAL_ERROR_NETWORK_UNAVAILABLE (CF set).
*/
StatusResult NetworkBackendFallback_OpenAndBindUdpSocket(uint32_t localPort)

{
  StatusResult status;
  
  status.failed = true;
  status.valueOrError = FATAL_ERROR_NETWORK_UNAVAILABLE;
  return status;
}


/* Address: 0x0041A5B0.
   Default g_NetworkBackendSlot3 (close the socket) in the image data: there is no socket to close.
*/
void __cdecl NetworkBackendFallback_CloseActiveSocket(void)

{
  return;
}

/* Address: 0x0041A5C0.
   Default g_NetworkBackendSlot4 (receive a datagram) in the image data: sets CF (nothing received) and leaves
   EAX as it was, so UiTransfer receive loops stop at once.
*/
NetworkReceiveResult NetworkBackendFallback_ReceiveDatagram
               (WinSockAddress *sourceAddress,uint32_t byteCount,uint8_t *buffer)

{
  NetworkReceiveResult result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.failed = true;
  return result;
}

/* Address: 0x0041A5D0.
   Default g_NetworkBackendSlot5 (send a datagram) in the image data, called by UiTransfer_StagePacketAndSend:
   drops the packet and reports success (CF clear) so a session without network keeps running.
*/
NetworkSendResult NetworkBackendFallback_SendDatagram
               (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  NetworkSendResult result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.failed = false;
  return result;
}

/* Address: 0x0041A5E0.
   Default g_NetworkBackendSlot6 (parse a typed peer address) in the image data: always fails (CF set).
*/
bool NetworkBackendFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpoint,char *endpointText)

{
  return true;
}


/* Address: 0x0041A5F0.
   Default g_NetworkBackendSlot7 (format a peer address as text) in the image data: writes an empty UTF-16
   string (one zero dword) to outputText and ignores the address.
*/
void NetworkBackendFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  outputText[0] = '\0';
  outputText[1] = '\0';
  outputText[2] = '\0';
  outputText[3] = '\0';
  return;
}

/* Address: 0x00584E70.
   Cleanup slot of the WinSock UDP backend. Nothing to release here: the socket is closed by
   NetworkFallback_CloseActiveSocket and WinSock itself by Network_Shutdown.
*/
void NetworkFallback_NoOpBackendCleanup(void)

{
  return;
}


/* Address: 0x00584E80.
   Opens the game's UDP socket: bound to localPort on all interfaces, or on the address given as
   -IP="host" on the command line (dotted address or host name), with broadcast allowed and non-blocking
   I/O. Also presets the local endpoint descriptor to the IPv4 broadcast address on that port, used for
   session discovery. On failure the WinSock error code is left in g_PackageLastErrorPath and
   FATAL_ERROR_NETWORK_SOCKET is returned with CF set.
*/
NetworkOpenBindResult NetworkFallback_OpenAndBindUdpSocket(NetworkPortHostOrder localPort)

{
  uint8_t copiedByte;
  uint8_t *nextOutput;
  WinSockHostEnt32 *hostEntry;
  int winsockResultOrError;
  NetworkIpv4AddressNetworkOrder bindAddress;
  uint8_t *optionCursor;
  uint8_t *outputCursor;
  NetworkOpenBindResult socketResult;
  NetworkOpenBindResult failureResult;
  CommandLineOptionResult ipOption;
  uint32_t socketToClose;

  socketToClose = INVALID_SOCKET;
  socketResult.valueOrError = g_WinSock_socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if (socketResult.valueOrError != INVALID_SOCKET) {
    bindAddress = 0; /* INADDR_ANY */
    ipOption = g_CommandLineFindOption(3,s_CommandLineOptionIp); /* "IP=" */
    if (!ipOption.notFound) {
      optionCursor = ipOption.option + 4;
      nextOutput = g_PackageScratchBuffer;
      if (ipOption.option[3] == '"') {
        /* copy the quoted value, including the closing quote, into the scratch buffer */
        do {
          outputCursor = nextOutput;
          copiedByte = *optionCursor;
          *outputCursor = copiedByte;
          optionCursor++;
          if (copiedByte == 0) break;
          nextOutput = outputCursor + 1;
        } while (copiedByte != '"');
        /* A closing quote that ends the option: resolve the quoted address (unterminated quote: no bind
           address). */
        if ((copiedByte == '"') && (*optionCursor == 0)) {
          *outputCursor = 0;
          bindAddress = g_WinSock_inet_addr(g_PackageScratchBuffer);
          if (bindAddress == INADDR_NONE) {
            hostEntry = g_WinSock_gethostbyname(g_PackageScratchBuffer);
            bindAddress = 0;
            if (hostEntry != NULL) {
              bindAddress = *(NetworkIpv4AddressNetworkOrder *)*hostEntry->addressList;
            }
          }
        }
      }
    }
#ifdef THANDOR_TEST_AIDS
    /* test aid (OPEN_THANDOR_NET_PORT, not in the original): a second instance on this machine binds to
       another port; the local/broadcast descriptor below keeps the game port */
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         g_WinSock_htons((uint16_t)Thandor_TestAidNetworkBindPort(localPort));
#else
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         g_WinSock_htons((uint16_t)localPort);
#endif
    g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = bindAddress;
    g_NetworkFallbackBindEndpoint.addressHeader.fields.addressFamily = NETWORK_ADDRESS_FAMILY_IPV4;
    g_NetworkFallbackBindEndpoint.zeroPadding[0] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[1] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[2] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[3] = 0;
    /* the local descriptor gets the same family and port (family in the low word, port in the high word) */
#ifdef THANDOR_TEST_AIDS
    g_NetworkLocalEndpoint.addressHeader.packedFamilyAndPort =
         (uint32_t)g_WinSock_htons((uint16_t)localPort) << 16 | NETWORK_ADDRESS_FAMILY_IPV4;
#else
    g_NetworkLocalEndpoint.addressHeader.packedFamilyAndPort =
         (uint32_t)g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder << 16 |
         NETWORK_ADDRESS_FAMILY_IPV4;
#endif
    g_NetworkFallbackBindEndpoint.zeroPadding[4] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[5] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[6] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[7] = 0;
    winsockResultOrError = g_WinSock_bind(socketResult.valueOrError,&g_NetworkFallbackBindEndpoint,sizeof(WinSockAddress));
    socketToClose = socketResult.valueOrError;
    if (winsockResultOrError == 0) {
      /* g_NetworkFallbackSocketOptionOn holds a nonzero value (0xFFFFFFFF): enable SO_BROADCAST and non-blocking mode */
      winsockResultOrError = g_WinSock_setsockopt(socketResult.valueOrError,SOL_SOCKET,SO_BROADCAST,
                                                  (uint8_t *)&g_NetworkFallbackSocketOptionOn,4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = g_WinSock_ioctlsocket(socketResult.valueOrError,FIONBIO,
                                                     &g_NetworkFallbackSocketOptionOn);
        if (winsockResultOrError == 0) {
          g_NetworkLocalEndpoint.ipv4AddressNetworkOrder = INADDR_BROADCAST;
          g_NetworkLocalEndpoint.zeroPadding[0] = 0;
          g_NetworkLocalEndpoint.zeroPadding[1] = 0;
          g_NetworkLocalEndpoint.zeroPadding[2] = 0;
          g_NetworkLocalEndpoint.zeroPadding[3] = 0;
          g_NetworkLocalEndpoint.zeroPadding[4] = 0;
          g_NetworkLocalEndpoint.zeroPadding[5] = 0;
          g_NetworkLocalEndpoint.zeroPadding[6] = 0;
          g_NetworkLocalEndpoint.zeroPadding[7] = 0;
          g_NetworkFallbackSocket = socketResult.valueOrError;
          socketResult.failed = false;
          return socketResult;
        }
      }
    }
  }
  winsockResultOrError = g_WinSock_WSAGetLastError();
  /* the error code as decimal text, for the fatal-error message */
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockResultOrError,g_PackageLastErrorPath);
  if (socketToClose != INVALID_SOCKET) {
    g_WinSock_closesocket(socketToClose);
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_NETWORK_SOCKET;
  return failureResult;
}


/* Address: 0x00585030.
   Closes the UDP socket, if one is open. The handle is swapped out (XCHG in the original) before
   closesocket so that nobody uses the socket while it is being closed.
*/
void NetworkFallback_CloseActiveSocket(void)

{
  NetworkSocketHandle32 socket;

  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    /* XCHG: the timer thread sends and receives on this socket */
    socket = (NetworkSocketHandle32)THANDOR_ATOMIC_EXCHANGE(&g_NetworkFallbackSocket,INVALID_SOCKET);
    g_WinSock_closesocket(socket);
  }
  return;
}


/* Address: 0x00585060.
   Receives one datagram (non-blocking) into buffer and the sender's address into sourceAddress
   (16-byte sockaddr_in). Returns the byte count; CF is set when no socket is open or recvfrom fails,
   including WSAEWOULDBLOCK when nothing is pending.
*/
NetworkReceiveResult NetworkFallback_ReceiveDatagram
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  NetworkReceiveResult successResult;
  NetworkReceiveResult failureResult;

  g_NetworkFallbackAddressLength = sizeof(WinSockAddress);
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    receivedByteCount =
         g_WinSock_recvfrom
                   (g_NetworkFallbackSocket,buffer,byteCount,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    if (-1 < (int)receivedByteCount) {
#ifdef THANDOR_TEST_AIDS
      Thandor_TestAidLogDatagram("recv",sourceAddress,receivedByteCount,buffer);
#endif
      successResult.failed = false;
      successResult.byteCountOrError = receivedByteCount;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.byteCountOrError = receivedByteCount;
  return failureResult;
}


/* Address: 0x005850B0.
   Sends one datagram to destinationAddress (16-byte sockaddr_in) and returns the byte count. Without
   an open socket nothing is sent and the call still succeeds (returning INVALID_SOCKET as the count).
   A sendto error leaves the WinSock error code in g_PackageLastErrorPath and returns
   FATAL_ERROR_NETWORK_SOCKET with CF set.
*/
NetworkSendResult NetworkFallback_SendDatagram
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t sentByteCount;
  int winsockErrorCode;
  NetworkSendResult successResult;
  NetworkSendResult failureResult;

  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
#ifdef THANDOR_TEST_AIDS
    Thandor_TestAidLogDatagram("send",destinationAddress,byteCount,buffer);
#endif
    sentByteCount =
         g_WinSock_sendto(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,sizeof(WinSockAddress));
    if ((int)sentByteCount < 0) {
      winsockErrorCode = g_WinSock_WSAGetLastError();
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      failureResult.failed = true;
      failureResult.valueOrError = FATAL_ERROR_NETWORK_SOCKET;
      return failureResult;
    }
  }
  successResult.failed = false;
  successResult.valueOrError = sentByteCount;
  return successResult;
}


/* Address: 0x00585120.
   Turns the UTF-16 peer address typed by the player (dotted address or host name) into a 16-byte
   sockaddr_in with the game's port. An empty text yields the broadcast address from the local
   endpoint descriptor. CF is set when the text does not convert or the host is unknown.
*/
bool NetworkFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText)

{
  NetworkEndpointAddressHeader4 bindAddressHeader;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;
  StatusResult copyStatus;

  copyStatus = RichTextCommandStream_CopyToNarrow
                    (255,(uint8_t *)&g_NetworkEndpointTextScratchA,(uint16_t *)endpointText);
  if (copyStatus.failed) {
    return true;
  }
  ipv4AddressNetworkOrder = g_NetworkLocalEndpoint.ipv4AddressNetworkOrder;
  if ((g_NetworkEndpointTextScratchA != '\0') &&
     (ipv4AddressNetworkOrder = g_WinSock_inet_addr((uint8_t *)&g_NetworkEndpointTextScratchA),
     ipv4AddressNetworkOrder == INADDR_NONE)) {
    resolvedHostEntry = g_WinSock_gethostbyname((uint8_t *)&g_NetworkEndpointTextScratchA);
    if (resolvedHostEntry == NULL) {
      return true;
    }
    ipv4AddressNetworkOrder = *(NetworkIpv4AddressNetworkOrder *)*resolvedHostEntry->addressList;
  }
#ifdef THANDOR_TEST_AIDS
  /* The original copies family and port from g_NetworkFallbackBindEndpoint. The local descriptor holds the
     same header (family, game port); it is read here so the OPEN_THANDOR_NET_PORT test aid, which binds to
     another port, still addresses the peer's game port. */
  bindAddressHeader = g_NetworkLocalEndpoint.addressHeader;
#else
  bindAddressHeader = g_NetworkFallbackBindEndpoint.addressHeader;
#endif
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


/* Address: 0x005851C0.
   Writes the IPv4 address of socketAddress as dotted UTF-16 text (at most 0x200 bytes) into
   outputText, for showing a peer's address; an empty string when inet_ntoa fails.
*/
void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  uint8_t *dottedAddress;

  dottedAddress = g_WinSock_inet_ntoa(socketAddress->ipv4AddressNetworkOrder);
  if (dottedAddress != NULL) {
    Text_CopyNarrowToUtf16(512,(uint16_t *)outputText,dottedAddress);
    return;
  }
  /* outputText is really UTF-16: the four zero bytes are an empty string (a single dword store) */
  outputText[0] = '\0';
  outputText[1] = '\0';
  outputText[2] = '\0';
  outputText[3] = '\0';
  return;
}


/* Address: 0x00585290.
   Cleanup slot of the ws2_32 backend (0x00585210..0x0058569F), the counterpart of
   NetworkFallback_NoOpBackendCleanup. No recovered table points at it: the ws2_32 backend is presumably
   installed by the part of Network_Init that the original jumps over.
*/
void NetworkBackend_NoOpCleanup(void)

{
  return;
}

/* Address: 0x005852A0.
   ws2_32 counterpart of NetworkFallback_OpenAndBindUdpSocket for the selected backend instance (IPv4 or
   IPX, see NetworkBackend_SelectInstanceByIndex): opens a socket of the instance's family, type and
   protocol, binds it to the port on any local address, enables SO_BROADCAST and non-blocking I/O and
   presets the local endpoint descriptor to the family's broadcast address. Failures leave the WinSock
   error code in g_PackageLastErrorPath and return FATAL_ERROR_NETWORK_SOCKET with CF set.
*/
StatusResult NetworkBackend_OpenAndBindActiveSocket(uint16_t portHostOrder)

{
  uint16_t networkPort;
  uint32_t socketOrAddressLength;
  int winsockResultOrError;
  StatusResult successResult;
  StatusResult failureResult;
  uint32_t bytesReturned;
  uint32_t socketHandle;

  socketHandle = INVALID_SOCKET;
  socketOrAddressLength = g_Ws2_32_socket(g_NetworkBackendActiveAddressFamily,g_NetworkBackendActiveSocketType,
                             g_NetworkBackendActiveProtocol);
  if (socketOrAddressLength != INVALID_SOCKET) {
    socketHandle = socketOrAddressLength;
    networkPort = g_Ws2_32_htons(portHostOrder);
    /* The asm stores all of EAX after htons; the high word is whatever htons left there. Every reader
       (this function and NetworkBackend_ParseEndpointText) uses only the low word (CX). */
    g_NetworkBackendPortNetworkOrderCarrier = (uint32_t)networkPort;
    /* bind address: the family dword, then all zero (INADDR_ANY / any IPX network and node) */
    g_NetworkBackendBindAddress.ipv4.ipv4AddressNetworkOrder = 0;
    THANDOR_PART(uint32_t, g_NetworkBackendBindAddress, 8) = 0;
    THANDOR_PART(uint32_t, g_NetworkBackendBindAddress, 12) = 0;
    g_NetworkBackendBindAddress.ipv4.addressHeader =
         THANDOR_BITCAST(uint32_t, NetworkEndpointAddressHeader4, g_NetworkBackendActiveAddressFamily);
    /* bind with the instance's address length, at least 16 bytes */
    socketOrAddressLength = g_NetworkBackendActiveSocketAddressLength;
    if (g_NetworkBackendActiveSocketAddressLength < 16) {
      socketOrAddressLength = 16;
    }
    if (g_NetworkBackendActiveAddressFamily == AF_INET) {
      THANDOR_PART(uint16_t, g_NetworkBackendBindAddress, 2) = networkPort;
      g_NetworkBackendBindAddress.ipx.addressFamily = AF_INET;
    }
    else if (g_NetworkBackendActiveAddressFamily == AF_IPX) {
      THANDOR_PART(uint8_t, g_NetworkBackendBindAddress, 14) = 0;
      THANDOR_PART(uint8_t, g_NetworkBackendBindAddress, 15) = 0;
      g_NetworkBackendBindAddress.ipx.socketNetworkOrder = networkPort;
    }
    winsockResultOrError = g_Ws2_32_bind(socketHandle,&g_NetworkBackendBindAddress.ipv4,socketOrAddressLength);
    if (winsockResultOrError == 0) {
      /* g_NetworkFallbackSocketOptionOn holds a nonzero value (0xFFFFFFFF): enable SO_BROADCAST and non-blocking mode */
      winsockResultOrError = g_Ws2_32_setsockopt(socketHandle,SOL_SOCKET,SO_BROADCAST,(uint8_t *)&g_NetworkFallbackSocketOptionOn,4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = g_Ws2_32_WSAIoctl
                          (socketHandle,FIONBIO,&g_NetworkFallbackSocketOptionOn,4,NULL,0,&bytesReturned,
                           NULL,NULL);
        if (winsockResultOrError == 0) {
          if (g_NetworkBackendActiveAddressFamily == AF_INET) {
            /* 255.255.255.255 on the game port */
            g_NetworkLocalEndpoint.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPV4;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpoint.ipv4AddressNetworkOrder, 0) = 0xffff;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpoint.ipv4AddressNetworkOrder, 2) = 0xffff;
            g_NetworkLocalEndpoint.addressHeader.fields.portNetworkOrder =
                 (NetworkPortNetworkOrder)g_NetworkBackendPortNetworkOrderCarrier;
          }
          else if (g_NetworkBackendActiveAddressFamily == AF_IPX) {
            /* SOCKADDR_IPX: network 0 (this network), node FF:FF:FF:FF:FF:FF (broadcast), the game socket */
            g_NetworkLocalEndpoint.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPX;
            g_NetworkLocalEndpoint.addressHeader.fields.portNetworkOrder = 0;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpoint.ipv4AddressNetworkOrder, 0) = 0;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpoint.ipv4AddressNetworkOrder, 2) = 0xffff;
            g_NetworkLocalEndpoint.zeroPadding[0] = 0xff;
            g_NetworkLocalEndpoint.zeroPadding[1] = 0xff;
            g_NetworkLocalEndpoint.zeroPadding[2] = 0xff;
            g_NetworkLocalEndpoint.zeroPadding[3] = 0xff;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpoint.zeroPadding, 4) =
                 (NetworkPortNetworkOrder)g_NetworkBackendPortNetworkOrderCarrier;
          }
          g_NetworkFallbackSocket = socketHandle;
          successResult.failed = false;
          successResult.valueOrError = socketHandle;
          return successResult;
        }
      }
    }
  }
  winsockResultOrError = g_Ws2_32_WSAGetLastError();
  /* the error code as decimal text, for the fatal-error message */
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockResultOrError,g_PackageLastErrorPath);
  if (socketHandle != INVALID_SOCKET) {
    g_Ws2_32_closesocket(socketHandle);
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_NETWORK_SOCKET;
  return failureResult;
}

/* Address: 0x00585450.
   ws2_32 counterpart of NetworkFallback_CloseActiveSocket: closes the backend socket, if one is open. The
   handle is swapped out (XCHG in the original) before closesocket so that nobody uses the socket while it
   is being closed.
*/
void NetworkFallbackUdp_CloseSocket(void)

{
  NetworkSocketHandle32 socket;

  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    /* XCHG: the timer thread sends and receives on this socket */
    socket = (NetworkSocketHandle32)THANDOR_ATOMIC_EXCHANGE(&g_NetworkFallbackSocket,INVALID_SOCKET);
    g_Ws2_32_closesocket(socket);
  }
  return;
}


/* Address: 0x00585480.
   ws2_32 counterpart of NetworkFallback_ReceiveDatagram: receives one datagram (non-blocking) into buffer
   and the sender's address (the instance's address length) into sourceAddress. Returns the byte count; CF
   is set when no socket is open or recvfrom fails, including WSAEWOULDBLOCK when nothing is pending.
*/
StatusResult NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  StatusResult successResult;
  StatusResult failureResult;

  g_NetworkFallbackAddressLength = g_NetworkBackendActiveSocketAddressLength;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    successResult.valueOrError =
         g_Ws2_32_recvfrom
                   (g_NetworkFallbackSocket,buffer,bufferLength,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    receivedByteCount = successResult.valueOrError;
    if (-1 < (int)successResult.valueOrError) {
      successResult.failed = false;
      return successResult;
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = receivedByteCount;
  return failureResult;
}


/* Address: 0x005854E0.
   ws2_32 counterpart of NetworkFallback_SendDatagram: sends one datagram to destinationAddress and returns
   the byte count. Without an open socket nothing is sent and the call still succeeds (returning
   INVALID_SOCKET as the count). A sendto error leaves the WinSock error code in g_PackageLastErrorPath
   and returns FATAL_ERROR_NETWORK_SOCKET with CF set.
*/
StatusResult NetworkFallbackUdp_SendDatagram(WinSockAddress *destinationAddress,int byteCount,uint8_t *buffer)

{
  NetworkSocketHandle32 sentByteCount;
  int winsockErrorCode;
  StatusResult successResult;
  StatusResult failureResult;

  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    sentByteCount = g_Ws2_32_sendto(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,
                               g_NetworkBackendActiveSocketAddressLength);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = g_Ws2_32_WSAGetLastError();
      /* the error code as decimal text, for the fatal-error message */
      g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      failureResult.failed = true;
      failureResult.valueOrError = FATAL_ERROR_NETWORK_SOCKET;
      return failureResult;
    }
  }
  successResult.failed = false;
  successResult.valueOrError = sentByteCount;
  return successResult;
}


/* Address: 0x00585550.
   ws2_32 counterpart of NetworkFallback_ParsePeerEndpoint: turns the UTF-16 peer address typed by the
   player into a socket address of the active family (WSAStringToAddressA, else a host-name lookup) and
   puts the game's port into it. An empty text yields the broadcast address from the local endpoint
   descriptor. CF is set when the text does not convert or the host is unknown.
*/
bool NetworkBackend_ParseEndpointText(NetworkEndpointAddressHeader4 *endpointOut,uint16_t *addressText)

{
  NetworkEndpointAddressHeader4 resolvedAddress;
  NetworkEndpointAddressHeader4 bindAddressHeader;
  int conversionResult;
  WinSockHostEnt32 *hostEntry;
  uint32_t remainingDwords;
  NetworkEndpointAddressHeader4 *sourceCursor;
  StatusResult copyStatus;
  int addressLength; /* never initialised, in the original as well */

  copyStatus = RichTextCommandStream_CopyToNarrow
                    (255,(uint8_t *)&g_NetworkEndpointTextScratchA,addressText);
  if (copyStatus.failed) {
    return true;
  }
  if (g_NetworkEndpointTextScratchA != '\0') {
    conversionResult = g_Ws2_32_WSAStringToAddressA
                      (&g_NetworkEndpointTextScratchA,g_NetworkBackendActiveAddressFamily,
                       NULL,(NetworkBackendSocketAddress16 *)endpointOut,&addressLength);
    if (conversionResult != 0) {
      /* not an address literal: resolve it as an IPv4 host name, with the bind endpoint's family and port */
      hostEntry = g_Ws2_32_gethostbyname((uint8_t *)&g_NetworkEndpointTextScratchA);
      bindAddressHeader = g_NetworkFallbackBindEndpoint.addressHeader;
      if (hostEntry == NULL) {
        return true;
      }
      resolvedAddress = *(NetworkEndpointAddressHeader4 *)*hostEntry->addressList;
      endpointOut[2].packedFamilyAndPort = 0;
      endpointOut[3].packedFamilyAndPort = 0;
      *endpointOut = bindAddressHeader;
      endpointOut[1] = resolvedAddress;
    }
    /* the game's port: sin_port for IPv4, sa_socket (+0xC) for IPX */
    if (g_NetworkBackendActiveAddressFamily == AF_INET) {
      endpointOut->fields.portNetworkOrder =
           (NetworkAddressFamily)g_NetworkBackendPortNetworkOrderCarrier;
    }
    else if (g_NetworkBackendActiveAddressFamily == AF_IPX) {
      endpointOut[3].fields.addressFamily =
           (NetworkAddressFamily)g_NetworkBackendPortNetworkOrderCarrier;
    }
    return false;
  }
  remainingDwords = g_NetworkBackendActiveSocketAddressLength >> 2;
  sourceCursor = &g_NetworkLocalEndpoint.addressHeader;
  for (; remainingDwords != 0; remainingDwords--) {
    *endpointOut = *sourceCursor;
    sourceCursor = sourceCursor + 1;
    endpointOut = endpointOut + 1;
  }
  return false;
}

/* Address: 0x00585640.
   ws2_32 counterpart of NetworkFallback_FormatPeerAddress: writes address as UTF-16 text
   (WSAAddressToStringA, at most 0x200 bytes) into outputUtf16, for showing a peer's address. When the
   conversion fails the output is an empty string and CF is clear; otherwise CF is the copy result.
*/
bool NetworkFallback_FormatAddressUtf16(uint16_t *outputUtf16,WinSockAddress *address)

{
  int conversionResult;
  StatusResult copyStatus;
  uint32_t textBufferLength; /* in: the scratch buffer size, out: the text length */

  textBufferLength = 255;
  conversionResult = g_Ws2_32_WSAAddressToStringA
                    (address,g_NetworkBackendActiveSocketAddressLength,NULL,
                     &g_NetworkEndpointTextScratchA,&textBufferLength);
  if (conversionResult == 0) {
    copyStatus = Text_CopyNarrowToUtf16(512,outputUtf16,&g_NetworkEndpointTextScratchA);
    return copyStatus.failed;
  }
  outputUtf16[0] = 0;
  outputUtf16[1] = 0;
  return false;
}

