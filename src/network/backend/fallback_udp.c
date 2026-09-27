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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot0_ReturnError43Cf(uint32_t argument)

{
  StatusValueEaxCf5 status;
  
  status.carry = true;
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot2_ReturnError43Cf(uint32_t argument)

{
  StatusValueEaxCf5 status;
  
  status.carry = true;
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
NetworkBackendReceiveEaxCf5 __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot4_ThreeArgFailureCf
               (WinSockAddress *sourceAddress,uint32_t argument1,uint8_t *buffer)

{
  NetworkBackendReceiveEaxCf5 result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.carry = true;
  return result;
}

/* Address: 0x0041A5D0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default three-argument implementation for network backend slot 5. It clears CF and performs no backend
   operation. The live slot is the submission callback used by UiTransfer_StagePacketAndSendCf. Typed parameters:
   p1 byteCount→NetworkByteCount_V302. Nearby but non-identical semantic domains were explicitly deferred.
*/
NetworkBackendSendEaxCf5 __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot5_ThreeArgSuccessCf
               (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  NetworkBackendSendEaxCf5 result; /* result type of the backend slot */
  memset(&result, 0, sizeof result);
  result.carry = false;
  return result;
}

/* Address: 0x0041A5E0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default two-argument implementation for network backend slot 6. It sets CF and performs no backend
   operation.
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackendFallback_Slot6_TwoArgFailureCf
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
   Ownership: network/backend/fallback_udp.
   Purpose: Deliberate no-op network backend cleanup slot.
*/
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_NoOpBackendCleanup(void)

{
  return;
}


/* Address: 0x00584E80.
   Ownership: network/backend/fallback_udp.
   Purpose: Creates an IPv4 UDP socket, resolves the configured local address when present, binds the requested
   local port, applies broadcast/nonblocking/event options, and publishes the active fallback socket. CF reports
   failure and EAX carries engine error 0x2A on setup errors.
*/
NetworkBackendOpenBindEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_OpenAndBindUdpSocketCf(NetworkPortHostOrder localPort)

{
  uint8_t copiedByte;
  uint8_t *nextOutput;
  WinSockHostEnt32 *hostEntry;
  int winsockResultOrError;
  NetworkIpv4AddressNetworkOrder bindAddress;
  uint8_t *optionCursor;
  uint8_t *outputCursor;
  NetworkBackendOpenBindEaxCf5 socketResult;
  NetworkBackendOpenBindEaxCf5 failureResult;
  CommandLineFindOptionEbxCf5 ipOption;
  uint32_t socketToClose;
  
  socketToClose = 0xffffffff;
  socketResult.eax = (*g_WinSock_socket)(2,2,0x11);
  if (socketResult.eax != 0xffffffff) {
    bindAddress = 0;
    ipOption = (*g_CommandLineFindOption)(3,(char *)THANDOR_ADDR(s_CommandLineOptionIp,0));
    if (!ipOption.carry) {
      optionCursor = ipOption.ebx + 4;
      nextOutput = g_PackageScratchBuffer;
      if (ipOption.ebx[3] == 0x22) {
        do {
          outputCursor = nextOutput;
          copiedByte = *optionCursor;
          *outputCursor = copiedByte;
          optionCursor = optionCursor + 1;
          if (copiedByte == 0) break;
          nextOutput = outputCursor + 1;
        } while (copiedByte != 0x22);
        /* A closing quote that ends the option: resolve the quoted address (unterminated quote: no bind
           address). */
        if ((copiedByte == 0x22) && (*optionCursor == 0)) {
          *outputCursor = 0;
          bindAddress = (*g_WinSock_inet_addr)(g_PackageScratchBuffer);
          if (bindAddress == 0xffffffff) {
            hostEntry = (*g_WinSock_gethostbyname)(g_PackageScratchBuffer);
            bindAddress = 0;
            if (hostEntry != (WinSockHostEnt32 *)0x0) {
              bindAddress = *(NetworkIpv4AddressNetworkOrder *)*hostEntry->addressList;
            }
          }
        }
      }
    }
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         (*g_WinSock_htons)((uint16_t)localPort);
    g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = bindAddress;
    g_NetworkFallbackBindEndpoint.addressHeader.fields.addressFamily = NETWORK_ADDRESS_FAMILY_IPV4;
    g_NetworkFallbackBindEndpoint.zeroPadding[0] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[1] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[2] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[3] = 0;
    g_NetworkLocalEndpointDescriptor16.addressHeader.packedFamilyAndPort =
         (uint32_t)g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder << 0x10 | 2;
    g_NetworkFallbackBindEndpoint.zeroPadding[4] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[5] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[6] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[7] = 0;
    winsockResultOrError = (*g_WinSock_bind)(socketResult.eax,&g_NetworkFallbackBindEndpoint,0x10);
    socketToClose = socketResult.eax;
    if (winsockResultOrError == 0) {
      winsockResultOrError = (*g_WinSock_setsockopt)(socketResult.eax,0xffff,0x20,(uint8_t *)THANDOR_ADDR(g_NetworkFallbackSocketOptionOn,0),4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = (*g_WinSock_ioctlsocket)(socketResult.eax,0x8004667e,(uint32_t *)THANDOR_ADDR(g_NetworkFallbackSocketOptionOn,0));
        if (winsockResultOrError == 0) {
          g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder = 0xffffffff;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[4] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[5] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[6] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[7] = 0;
          g_NetworkFallbackSocket = socketResult.eax;
          socketResult.carry = false;
          return socketResult;
        }
      }
    }
  }
  winsockResultOrError = (*g_WinSock_WSAGetLastError)();
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockResultOrError,g_PackageLastErrorPath);
  if (socketToClose != 0xffffffff) {
    (*g_WinSock_closesocket)(socketToClose);
  }
  failureResult.carry = true;
  failureResult.eax = 0x2a;
  return failureResult;
}


/* Address: 0x00585030.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback network-backend close callback. It atomically replaces the active socket with -1 and closes
   the previous socket when present.
*/
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_CloseActiveSocket(void)

{
  NetworkSocketHandle32 socket;
  
  socket = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    LOCK();
    g_NetworkFallbackSocket = 0xffffffff;
    UNLOCK();
    (*g_WinSock_closesocket)(socket);
  }
  return;
}


/* Address: 0x00585060.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback UDP receive callback wrapping recvfrom with a fixed 16-byte source-address length. CF is clear
   on nonnegative Winsock result and set on failure.
*/
NetworkBackendReceiveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_ReceiveDatagramCf
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  NetworkBackendReceiveEaxCf5 successResult;
  NetworkBackendReceiveEaxCf5 failureResult;
  
  g_NetworkFallbackAddressLength = 0x10;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    receivedByteCount =
         (*g_WinSock_recvfrom)
                   (g_NetworkFallbackSocket,buffer,byteCount,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    if (-1 < (int)receivedByteCount) {
      successResult.carry = false;
      successResult.eax = receivedByteCount;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.eax = receivedByteCount;
  return failureResult;
}


/* Address: 0x005850B0.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback UDP send callback wrapping sendto with a fixed 16-byte destination-address length. It
   preserves the legacy error-reporting path and returns status through EAX and CF.
*/
NetworkBackendSendEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_SendDatagramCf
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer)

{
  uint32_t sentByteCount;
  int winsockErrorCode;
  NetworkBackendSendEaxCf5 successResult;
  NetworkBackendSendEaxCf5 failureResult;
  
  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    sentByteCount =
         (*g_WinSock_sendto)(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,0x10);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = (*g_WinSock_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      failureResult.carry = true;
      failureResult.eax = 0x2a;
      return failureResult;
    }
  }
  successResult.carry = false;
  successResult.eax = sentByteCount;
  return successResult;
}


/* Address: 0x00585120.
   Ownership: network/backend/fallback_udp.
   Purpose: Parses a narrow peer endpoint string into the backend 16-byte address descriptor. Uses numeric IPv4
   conversion first and host lookup as fallback. CF reports parse failure.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_ParsePeerEndpointCf
          (UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText)

{
  NetworkEndpointAddressHeader4 bindAddressHeader;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;
  StatusValueEaxCf5 copyStatus;
  NetworkPortNetworkOrder portNetworkOrder;
  
  copyStatus = RichTextCommandStream_CopyToNarrowCf
                    (0xff,(uint8_t *)&g_NetworkEndpointTextScratchA,(uint16_t *)endpointText);
  if (copyStatus.carry) {
    return true;
  }
  ipv4AddressNetworkOrder = g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder;
  if ((g_NetworkEndpointTextScratchA != '\0') &&
     (ipv4AddressNetworkOrder = (*g_WinSock_inet_addr)((uint8_t *)&g_NetworkEndpointTextScratchA),
     ipv4AddressNetworkOrder == 0xffffffff)) {
    resolvedHostEntry = (*g_WinSock_gethostbyname)((uint8_t *)&g_NetworkEndpointTextScratchA);
    if (resolvedHostEntry == (WinSockHostEnt32 *)0x0) {
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
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback network callback that formats the address field at socket-address offset +4 into a bounded
   0x200-byte narrow output string, or writes an empty string when conversion fails.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
void __thandor_void_preserve_eax_ecx_edx
NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

{
  uint8_t *source;
  
  source = (*g_WinSock_inet_ntoa)(socketAddress->ipv4AddressNetworkOrder);
  if (source != (uint8_t *)0x0) {
    Text_CopyNarrowToUtf16Cf(0x200,(uint16_t *)outputText,source);
    return;
  }
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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackend_OpenAndBindActiveSocketCf(uint16_t portHostOrder)

{
  uint16_t networkPort;
  uint32_t socketOrAddressLength;
  int winsockResultOrError;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  uint32_t bytesReturned;
  uint32_t socketHandle;
  
  socketHandle = 0xffffffff;
  socketOrAddressLength = (*g_Ws2_32_socket)(g_NetworkBackendActiveAddressFamily,g_NetworkBackendActiveSocketType,
                             g_NetworkBackendActiveProtocol);
  if (socketOrAddressLength != 0xffffffff) {
    socketHandle = socketOrAddressLength;
    networkPort = (*g_Ws2_32_htons)(portHostOrder);
    /* The asm stores all of EAX after htons; the high word is whatever htons left there. Every reader
       (this function and NetworkBackend_ParseEndpointTextCf) uses only the low word (CX). */
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
    winsockResultOrError = (*g_Ws2_32_bind)(socketHandle,&g_NetworkBackendBindAddress.ipv4,socketOrAddressLength);
    if (winsockResultOrError == 0) {
      winsockResultOrError = (*g_Ws2_32_setsockopt)(socketHandle,0xffff,0x20,(uint8_t *)THANDOR_ADDR(g_NetworkFallbackSocketOptionOn,0),4);
      if (winsockResultOrError == 0) {
        winsockResultOrError = (*g_Ws2_32_WSAIoctl)
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
          successResult.carry = false;
          successResult.valueOrError = socketHandle;
          return successResult;
        }
      }
    }
  }
  winsockResultOrError = (*g_Ws2_32_WSAGetLastError)();
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockResultOrError,g_PackageLastErrorPath);
  if (socketHandle != 0xffffffff) {
    (*g_Ws2_32_closesocket)(socketHandle);
  }
  failureResult.carry = true;
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
  
  socket = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    LOCK();
    g_NetworkFallbackSocket = 0xffffffff;
    UNLOCK();
    (*g_Ws2_32_closesocket)(socket);
  }
  return;
}


/* Address: 0x00585480.
   Ownership: network/backend/fallback_udp.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,uint8_t *buffer)

{
  uint32_t receivedByteCount;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  g_NetworkFallbackAddressLength = g_NetworkBackendActiveSocketAddressLength;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    successResult.valueOrError =
         (*g_Ws2_32_recvfrom)
                   (g_NetworkFallbackSocket,buffer,bufferLength,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    receivedByteCount = successResult.valueOrError;
    if (-1 < (int)successResult.valueOrError) {
      successResult.carry = false;
      return successResult;
    }
  }
  failureResult.carry = true;
  failureResult.valueOrError = receivedByteCount;
  return failureResult;
}


/* Address: 0x005854E0.
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback udp send datagram.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_SendDatagram(WinSockAddress *destinationAddress,int byteCount,uint8_t *buffer)

{
  NetworkSocketHandle32 sentByteCount;
  int winsockErrorCode;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    sentByteCount = (*g_Ws2_32_sendto)(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,
                               g_NetworkBackendActiveSocketAddressLength);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = (*g_Ws2_32_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      failureResult.carry = true;
      failureResult.valueOrError = 0x2a;
      return failureResult;
    }
  }
  successResult.carry = false;
  successResult.valueOrError = sentByteCount;
  return successResult;
}


/* Address: 0x00585550.
   Ownership: network/backend/fallback_udp.
   Purpose: Parses endpoint text into the recovered backend address representation.
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackend_ParseEndpointTextCf(NetworkEndpointAddressHeader4 *endpointOut,uint16_t *addressText)

{
  NetworkEndpointAddressHeader4 resolvedAddress;
  NetworkEndpointAddressHeader4 bindAddressHeader;
  int conversionResult;
  WinSockHostEnt32 *hostEntry;
  uint32_t remainingDwords;
  NetworkEndpointAddressHeader4 *sourceCursor;
  StatusValueEaxCf5 copyStatus;
  int addressLength;
  
  copyStatus = RichTextCommandStream_CopyToNarrowCf
                    (0xff,(uint8_t *)&g_NetworkEndpointTextScratchA,addressText);
  if (copyStatus.carry) {
    return true;
  }
  if (g_NetworkEndpointTextScratchA != '\0') {
    conversionResult = (*g_Ws2_32_WSAStringToAddressA)
                      (&g_NetworkEndpointTextScratchA,g_NetworkBackendActiveAddressFamily,
                       (void *)0x0,(NetworkBackendSocketAddress16 *)endpointOut,&addressLength);
    if (conversionResult != 0) {
      hostEntry = (*g_Ws2_32_gethostbyname)((uint8_t *)&g_NetworkEndpointTextScratchA);
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
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_FormatAddressUtf16(uint16_t *outputUtf16,WinSockAddress *address)

{
  int conversionResult;
  StatusValueEaxCf5 copyStatus;
  uint32_t textLength;
  
  textLength = 0xff;
  conversionResult = (*g_Ws2_32_WSAAddressToStringA)
                    (address,g_NetworkBackendActiveSocketAddressLength,(void *)0x0,
                     &g_NetworkEndpointTextScratchA,&textLength);
  if (conversionResult == 0) {
    copyStatus = Text_CopyNarrowToUtf16Cf(0x200,outputUtf16,&g_NetworkEndpointTextScratchA);
    return copyStatus.carry;
  }
  outputUtf16[0] = 0;
  outputUtf16[1] = 0;
  return false;
}

