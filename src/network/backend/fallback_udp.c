/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/fallback_udp.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/fallback_udp.h>
#include <thandor/thandor.h>

/* Implementation ownership: network/backend/fallback_udp. */

/* Address: 0x0041A580.
   Ownership: network/backend/fallback_udp.
   Purpose: Default implementation for network backend slot 0. It returns EAX 0x2B, sets CF, consumes one dword
   argument, and performs no other work.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot0_ReturnError43(uint32_t argument)

{
  StatusResult status;
  
  status.failed = true;
  status.valueOrError = 0x2b;
  return status;
}


/* Address: 0x0041A590.
   Ownership: network/backend/fallback_udp.
   Purpose: Default no-argument no-op for network backend slot 1.
*/
void __cdecl NetworkBackendFallback_Slot1_NoOp(void)

{
  return;
}

/* Address: 0x0041A5A0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default implementation for network backend slot 2. It returns EAX 0x2B, sets CF, consumes one dword
   argument, and performs no other work.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot2_ReturnError43(uint32_t argument)

{
  StatusResult status;
  
  status.failed = true;
  status.valueOrError = 0x2b;
  return status;
}


/* Address: 0x0041A5B0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default no-argument no-op for network backend slot 3.
*/
void __cdecl NetworkBackendFallback_Slot3_NoOp(void)

{
  return;
}

/* Address: 0x0041A5C0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default three-argument implementation for network backend slot 4. It sets CF and otherwise preserves
   the incoming register state.
*/
NetworkReceiveResult __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot4_ThreeArgFailure
               (WinSockAddress *sourceAddress,uint32_t argument1,uint8_t *buffer)

{
  NetworkReceiveResult result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.failed = true;
  return result;
}

/* Address: 0x0041A5D0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default three-argument implementation for network backend slot 5. It clears CF and performs no backend
   operation. The live slot is the submission callback used by UiTransfer_StagePacketAndSend. Typed parameters:
   p1 byteCount→NetworkByteCount_V302. Nearby but non-identical semantic domains were explicitly deferred.
*/
NetworkSendResult __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot5_ThreeArgSuccess
               (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  NetworkSendResult result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.failed = false;
  return result;
}

/* Address: 0x0041A5E0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default two-argument implementation for network backend slot 6. It sets CF and performs no backend
   operation.
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackendFallback_Slot6_TwoArgFailure
          (UiTransferEndpointDescriptor *endpoint,char *endpointText)

{
  return true;
}


/* Address: 0x0041A5F0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default two-argument implementation for network backend slot 7. It clears the dword addressed by the
   first argument and ignores the second.
*/
void NetworkBackendFallback_Slot7_ClearOutput(char *outputText,WinSockAddress *socketAddress)

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
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_NoOpBackendCleanup(void)

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
NetworkOpenBindResult __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_OpenAndBindUdpSocket(NetworkPortHostOrder localPort)

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
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         g_WinSock_htons((uint16_t)localPort);
    g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = bindAddress;
    g_NetworkFallbackBindEndpoint.addressHeader.fields.addressFamily = NETWORK_ADDRESS_FAMILY_IPV4;
    g_NetworkFallbackBindEndpoint.zeroPadding[0] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[1] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[2] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[3] = 0;
    /* the local descriptor gets the same family and port (family in the low word, port in the high word) */
    g_NetworkLocalEndpointDescriptor16.addressHeader.packedFamilyAndPort =
         (uint32_t)g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder << 16 |
         NETWORK_ADDRESS_FAMILY_IPV4;
    g_NetworkFallbackBindEndpoint.zeroPadding[4] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[5] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[6] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[7] = 0;
    winsockResultOrError = g_WinSock_bind(socketResult.valueOrError,&g_NetworkFallbackBindEndpoint,0x10);
    socketToClose = socketResult.valueOrError;
    if (winsockResultOrError == 0) {
      /* g_NetworkFallbackSocketOptionOn holds 1: enable SO_BROADCAST and non-blocking mode */
      winsockResultOrError = g_WinSock_setsockopt(socketResult.valueOrError,SOL_SOCKET,SO_BROADCAST,
                                                  (uint8_t *)&g_NetworkFallbackSocketOptionOn,4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = g_WinSock_ioctlsocket(socketResult.valueOrError,FIONBIO,
                                                     &g_NetworkFallbackSocketOptionOn);
        if (winsockResultOrError == 0) {
          g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder = INADDR_BROADCAST;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[4] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[5] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[6] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[7] = 0;
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
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_CloseActiveSocket(void)

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
NetworkReceiveResult __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_ReceiveDatagram
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  NetworkReceiveResult successResult;
  NetworkReceiveResult failureResult;

  g_NetworkFallbackAddressLength = 0x10;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    receivedByteCount =
         g_WinSock_recvfrom
                   (g_NetworkFallbackSocket,buffer,byteCount,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    if (-1 < (int)receivedByteCount) {
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
NetworkSendResult __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_SendDatagram
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t sentByteCount;
  int winsockErrorCode;
  NetworkSendResult successResult;
  NetworkSendResult failureResult;

  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != INVALID_SOCKET) {
    sentByteCount =
         g_WinSock_sendto(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,0x10);
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
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_ParsePeerEndpoint
          (UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText)

{
  NetworkEndpointAddressHeader4 bindAddressHeader;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;
  StatusResult copyStatus;

  copyStatus = RichTextCommandStream_CopyToNarrow
                    (0xff,(uint8_t *)&g_NetworkEndpointTextScratchA,(uint16_t *)endpointText);
  if (copyStatus.failed) {
    return true;
  }
  ipv4AddressNetworkOrder = g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder;
  if ((g_NetworkEndpointTextScratchA != '\0') &&
     (ipv4AddressNetworkOrder = g_WinSock_inet_addr((uint8_t *)&g_NetworkEndpointTextScratchA),
     ipv4AddressNetworkOrder == INADDR_NONE)) {
    resolvedHostEntry = g_WinSock_gethostbyname((uint8_t *)&g_NetworkEndpointTextScratchA);
    if (resolvedHostEntry == NULL) {
      return true;
    }
    ipv4AddressNetworkOrder = *(NetworkIpv4AddressNetworkOrder *)*resolvedHostEntry->addressList;
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


/* Address: 0x005851C0.
   Writes the IPv4 address of socketAddress as dotted UTF-16 text (at most 0x200 bytes) into
   outputText, for showing a peer's address; an empty string when inet_ntoa fails.
*/
void __thandor_void_preserve_eax_ecx_edx
NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  uint8_t *dottedAddress;

  dottedAddress = g_WinSock_inet_ntoa(socketAddress->ipv4AddressNetworkOrder);
  if (dottedAddress != NULL) {
    Text_CopyNarrowToUtf16(0x200,(uint16_t *)outputText,dottedAddress);
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
   Ownership: network/backend/fallback_udp.
   Purpose: Recovered no-op network-backend cleanup owner.
*/
void __thandor_void_preserve_eax_ecx_edx NetworkBackend_NoOpCleanup(void)

{
  return;
}

/* Address: 0x005852A0.
   Ownership: network/backend/fallback_udp.
   Purpose: Opens and binds the active backend socket with the recovered carry/error contract.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
NetworkBackend_OpenAndBindActiveSocket(uint16_t portHostOrder)

{
  uint16_t networkPort;
  uint32_t socketOrAddressLength;
  int winsockResultOrError;
  StatusResult successResult;
  StatusResult failureResult;
  uint32_t bytesReturned;
  uint32_t socketHandle;
  
  socketHandle = 0xffffffff;
  socketOrAddressLength = g_Ws2_32_socket(g_NetworkBackendActiveAddressFamily,g_NetworkBackendActiveSocketType,
                             g_NetworkBackendActiveProtocol);
  if (socketOrAddressLength != 0xffffffff) {
    socketHandle = socketOrAddressLength;
    networkPort = g_Ws2_32_htons(portHostOrder);
    /* The asm stores all of EAX after htons; the high word is whatever htons left there. Every reader
       (this function and NetworkBackend_ParseEndpointText) uses only the low word (CX). */
    g_NetworkBackendPortNetworkOrderCarrier = (uint32_t)networkPort;
    g_NetworkBackendBindAddress.ipv4.ipv4AddressNetworkOrder = 0;
    THANDOR_PART(uint32_t, g_NetworkBackendBindAddress, 8) = 0;
    THANDOR_PART(uint32_t, g_NetworkBackendBindAddress, 12) = 0;
    g_NetworkBackendBindAddress.ipv4.addressHeader =
         THANDOR_BITCAST(uint32_t, NetworkEndpointAddressHeader4, g_NetworkBackendActiveAddressFamily);
    socketOrAddressLength = g_NetworkBackendActiveSocketAddressLength;
    if (g_NetworkBackendActiveSocketAddressLength < 0x10) {
      socketOrAddressLength = 0x10;
    }
    if (g_NetworkBackendActiveAddressFamily == 2) {
      THANDOR_PART(uint16_t, g_NetworkBackendBindAddress, 2) = networkPort;
      g_NetworkBackendBindAddress.ipx.addressFamily = 2;
    }
    else if (g_NetworkBackendActiveAddressFamily == 6) {
      THANDOR_PART(uint8_t, g_NetworkBackendBindAddress, 14) = 0;
      THANDOR_PART(uint8_t, g_NetworkBackendBindAddress, 15) = 0;
      g_NetworkBackendBindAddress.ipx.socketNetworkOrder = networkPort;
    }
    winsockResultOrError = g_Ws2_32_bind(socketHandle,&g_NetworkBackendBindAddress.ipv4,socketOrAddressLength);
    if (winsockResultOrError == 0) {
      winsockResultOrError = g_Ws2_32_setsockopt(socketHandle,0xffff,0x20,(uint8_t *)THANDOR_ADDR(g_NetworkFallbackSocketOptionOn,0),4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = g_Ws2_32_WSAIoctl
                          (socketHandle,0x8004667e,(void *)THANDOR_ADDR(g_NetworkFallbackSocketOptionOn,0),4,(void *)0x0,0,&bytesReturned,
                           (void *)0x0,(void *)0x0);
        if (winsockResultOrError == 0) {
          if (g_NetworkBackendActiveAddressFamily == 2) {
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPV4;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder, 0) = 0xffff;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder, 2) = 0xffff;
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.portNetworkOrder =
                 (NetworkPortNetworkOrder)g_NetworkBackendPortNetworkOrderCarrier;
          }
          else if (g_NetworkBackendActiveAddressFamily == 6) {
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPX;
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.portNetworkOrder = 0;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder, 0) = 0;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder, 2) = 0xffff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0xff;
            THANDOR_PART(uint16_t, g_NetworkLocalEndpointDescriptor16.zeroPadding, 4) =
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
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockResultOrError,g_PackageLastErrorPath);
  if (socketHandle != 0xffffffff) {
    g_Ws2_32_closesocket(socketHandle);
  }
  failureResult.failed = true;
  failureResult.valueOrError = 0x2a;
  return failureResult;
}

/* Address: 0x00585450.
   Ownership: network/backend/fallback_udp.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void __thandor_void_preserve_eax_ecx_edx NetworkFallbackUdp_CloseSocket(void)

{
  NetworkSocketHandle32 socket;
  
  if (g_NetworkFallbackSocket != 0xffffffff) {
    /* XCHG: the timer thread sends and receives on this socket */
    socket = (NetworkSocketHandle32)THANDOR_ATOMIC_EXCHANGE(&g_NetworkFallbackSocket,0xffffffff);
    g_Ws2_32_closesocket(socket);
  }
  return;
}


/* Address: 0x00585480.
   Ownership: network/backend/fallback_udp.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  StatusResult successResult;
  StatusResult failureResult;
  
  g_NetworkFallbackAddressLength = g_NetworkBackendActiveSocketAddressLength;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
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
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback udp send datagram.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_SendDatagram(WinSockAddress *destinationAddress,int byteCount,uint8_t *buffer)

{
  NetworkSocketHandle32 sentByteCount;
  int winsockErrorCode;
  StatusResult successResult;
  StatusResult failureResult;
  
  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    sentByteCount = g_Ws2_32_sendto(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,
                               g_NetworkBackendActiveSocketAddressLength);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = g_Ws2_32_WSAGetLastError();
      g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      failureResult.failed = true;
      failureResult.valueOrError = 0x2a;
      return failureResult;
    }
  }
  successResult.failed = false;
  successResult.valueOrError = sentByteCount;
  return successResult;
}


/* Address: 0x00585550.
   Ownership: network/backend/fallback_udp.
   Purpose: Parses endpoint text into the recovered backend address representation.
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackend_ParseEndpointText(NetworkEndpointAddressHeader4 *endpointOut,uint16_t *addressText)

{
  NetworkEndpointAddressHeader4 resolvedAddress;
  NetworkEndpointAddressHeader4 bindAddressHeader;
  int conversionResult;
  WinSockHostEnt32 *hostEntry;
  uint32_t remainingDwords;
  NetworkEndpointAddressHeader4 *sourceCursor;
  StatusResult copyStatus;
  int addressLength;
  
  copyStatus = RichTextCommandStream_CopyToNarrow
                    (0xff,(uint8_t *)&g_NetworkEndpointTextScratchA,addressText);
  if (copyStatus.failed) {
    return true;
  }
  if (g_NetworkEndpointTextScratchA != '\0') {
    conversionResult = g_Ws2_32_WSAStringToAddressA
                      (&g_NetworkEndpointTextScratchA,g_NetworkBackendActiveAddressFamily,
                       (void *)0x0,(NetworkBackendSocketAddress16 *)endpointOut,&addressLength);
    if (conversionResult != 0) {
      hostEntry = g_Ws2_32_gethostbyname((uint8_t *)&g_NetworkEndpointTextScratchA);
      bindAddressHeader = g_NetworkFallbackBindEndpoint.addressHeader;
      if (hostEntry == (WinSockHostEnt32 *)0x0) {
        return true;
      }
      resolvedAddress = *(NetworkEndpointAddressHeader4 *)*hostEntry->addressList;
      endpointOut[2].packedFamilyAndPort = 0;
      endpointOut[3].packedFamilyAndPort = 0;
      *endpointOut = bindAddressHeader;
      endpointOut[1] = resolvedAddress;
    }
    if (g_NetworkBackendActiveAddressFamily == 2) {
      (endpointOut->fields).portNetworkOrder =
           (NetworkAddressFamily)g_NetworkBackendPortNetworkOrderCarrier;
    }
    else if (g_NetworkBackendActiveAddressFamily == 6) {
      endpointOut[3].fields.addressFamily =
           (NetworkAddressFamily)g_NetworkBackendPortNetworkOrderCarrier;
    }
    return false;
  }
  remainingDwords = g_NetworkBackendActiveSocketAddressLength >> 2;
  sourceCursor = &g_NetworkLocalEndpointDescriptor16.addressHeader;
  for (; remainingDwords != 0; remainingDwords = remainingDwords - 1) {
    *endpointOut = *sourceCursor;
    sourceCursor = sourceCursor + 1;
    endpointOut = endpointOut + 1;
  }
  return false;
}

/* Address: 0x00585640.
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback format address utf16.
   Cross-module calls: Text_CopyNarrowToUtf16 [core/text/string].
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_FormatAddressUtf16(uint16_t *outputUtf16,WinSockAddress *address)

{
  int conversionResult;
  StatusResult copyStatus;
  uint32_t textLength;
  
  textLength = 0xff;
  conversionResult = g_Ws2_32_WSAAddressToStringA
                    (address,g_NetworkBackendActiveSocketAddressLength,(void *)0x0,
                     &g_NetworkEndpointTextScratchA,&textLength);
  if (conversionResult == 0) {
    copyStatus = Text_CopyNarrowToUtf16(0x200,outputUtf16,&g_NetworkEndpointTextScratchA);
    return copyStatus.failed;
  }
  outputUtf16[0] = 0;
  outputUtf16[1] = 0;
  return false;
}

