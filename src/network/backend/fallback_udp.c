/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/network/backend/fallback_udp.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/network/backend/fallback_udp.h>

/* Implementation ownership: network/backend/fallback_udp. */

/* Address: 0x0041A580.
   Ownership: network/backend/fallback_udp.
   Purpose: Default implementation for network backend slot 0. It returns EAX 0x2B, sets CF, consumes one dword
   argument, and performs no other work.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot0_ReturnError43Cf(dword argument)

{
  StatusValueEaxCf5 SVar1;
  
  SVar1.carry = true;
  SVar1.valueOrError = 0x2b;
  return SVar1;
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
NetworkBackendFallback_Slot2_ReturnError43Cf(dword argument)

{
  StatusValueEaxCf5 SVar1;
  
  SVar1.carry = true;
  SVar1.valueOrError = 0x2b;
  return SVar1;
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
void NetworkBackendFallback_Slot4_ThreeArgFailureCf
               (WinSockAddress *sourceAddress,dword argument1,byte *buffer)

{
  return;
}

/* Address: 0x0041A5D0.
   Ownership: network/backend/fallback_udp.
   Purpose: Default three-argument implementation for network backend slot 5. It clears CF and performs no backend
   operation. The live slot is the submission callback used by UiTransfer_StagePacketAndSendCf. Typed parameters:
   p1 byteCount→NetworkByteCount_V302. Nearby but non-identical semantic domains were explicitly deferred.
*/
void NetworkBackendFallback_Slot5_ThreeArgSuccessCf
               (WinSockAddress *destinationAddress,NetworkByteCount byteCount,byte *buffer)

{
  return;
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
  byte bVar1;
  byte *pbVar2;
  WinSockHostEnt32 *pWVar3;
  int iVar4;
  NetworkIpv4AddressNetworkOrder NVar5;
  byte *pbVar6;
  byte *pbVar7;
  NetworkBackendOpenBindEaxCf5 NVar8;
  NetworkBackendOpenBindEaxCf5 NVar9;
  CommandLineFindOptionEbxCf5 CVar10;
  dword dStack_1c;
  
  dStack_1c = 0xffffffff;
  NVar8.eax = (*g_WinSock_socket)(2,2,0x11);
  if (NVar8.eax != 0xffffffff) {
    NVar5 = 0;
    CVar10 = (*g_CommandLineFindOption)(3,(char *)0x584078);
    if (!CVar10.carry) {
      pbVar6 = CVar10.ebx + 4;
      pbVar2 = g_PackageScratchBuffer;
      if (CVar10.ebx[3] == 0x22) {
        do {
          pbVar7 = pbVar2;
          bVar1 = *pbVar6;
          *pbVar7 = bVar1;
          pbVar6 = pbVar6 + 1;
          if (bVar1 == 0)
          goto 
          NetworkFallback_OpenAndBindUdpSocketCf_ConfigureEndpointAfterOptionalBindAddressResolution
          ;
          pbVar2 = pbVar7 + 1;
        } while (bVar1 != 0x22);
        if (*pbVar6 == 0) {
          *pbVar7 = 0;
          NVar5 = (*g_WinSock_inet_addr)(g_PackageScratchBuffer);
          if (NVar5 == 0xffffffff) {
            pWVar3 = (*g_WinSock_gethostbyname)(g_PackageScratchBuffer);
            NVar5 = 0;
            if (pWVar3 != (WinSockHostEnt32 *)0x0) {
              NVar5 = *(NetworkIpv4AddressNetworkOrder *)*pWVar3->addressList;
            }
          }
        }
      }
    }
NetworkFallback_OpenAndBindUdpSocketCf_ConfigureEndpointAfterOptionalBindAddressResolution:
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         (*g_WinSock_htons)((word)localPort);
    g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = NVar5;
    g_NetworkFallbackBindEndpoint.addressHeader.fields.addressFamily = NETWORK_ADDRESS_FAMILY_IPV4;
    g_NetworkFallbackBindEndpoint.zeroPadding[0] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[1] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[2] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[3] = 0;
    g_NetworkLocalEndpointDescriptor16.addressHeader.packedFamilyAndPort =
         (uint)g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder << 0x10 | 2;
    g_NetworkFallbackBindEndpoint.zeroPadding[4] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[5] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[6] = 0;
    g_NetworkFallbackBindEndpoint.zeroPadding[7] = 0;
    iVar4 = (*g_WinSock_bind)(NVar8.eax,&g_NetworkFallbackBindEndpoint,0x10);
    dStack_1c = NVar8.eax;
    if (iVar4 == 0) {
      iVar4 = (*g_WinSock_setsockopt)(NVar8.eax,0xffff,0x20,(byte *)0x583ef6,4);
      if (iVar4 == 0) {
        iVar4 = (*g_WinSock_ioctlsocket)(NVar8.eax,0x8004667e,(dword *)0x583ef6);
        if (iVar4 == 0) {
          g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder = 0xffffffff;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[4] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[5] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[6] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[7] = 0;
          g_NetworkFallbackSocket = NVar8.eax;
          NVar8.carry = false;
          return NVar8;
        }
      }
    }
  }
  iVar4 = (*g_WinSock_WSAGetLastError)();
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar4,g_PackageLastErrorPath);
  if (dStack_1c != 0xffffffff) {
    (*g_WinSock_closesocket)(dStack_1c);
  }
  NVar9.carry = true;
  NVar9.eax = 0x2a;
  return NVar9;
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
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,byte *buffer)

{
  dword receivedByteCount;
  NetworkBackendReceiveEaxCf5 NVar1;
  NetworkBackendReceiveEaxCf5 NVar2;
  
  g_NetworkFallbackAddressLength = 0x10;
  receivedByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    receivedByteCount =
         (*g_WinSock_recvfrom)
                   (g_NetworkFallbackSocket,buffer,byteCount,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    if (-1 < (int)receivedByteCount) {
      NVar1.carry = false;
      NVar1.eax = receivedByteCount;
      return NVar1;
    }
  }
  NVar2.carry = true;
  NVar2.eax = receivedByteCount;
  return NVar2;
}


/* Address: 0x005850B0.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback UDP send callback wrapping sendto with a fixed 16-byte destination-address length. It
   preserves the legacy error-reporting path and returns status through EAX and CF.
*/
NetworkBackendSendEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_SendDatagramCf
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,byte *buffer)

{
  dword sentByteCount;
  int winsockErrorCode;
  NetworkBackendSendEaxCf5 NVar1;
  NetworkBackendSendEaxCf5 NVar2;
  
  sentByteCount = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    sentByteCount =
         (*g_WinSock_sendto)(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,0x10);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = (*g_WinSock_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      NVar2.carry = true;
      NVar2.eax = 0x2a;
      return NVar2;
    }
  }
  NVar1.carry = false;
  NVar1.eax = sentByteCount;
  return NVar1;
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
  NetworkEndpointAddressHeader4 NVar1;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;
  StatusValueEaxCf5 SVar2;
  NetworkPortNetworkOrder portNetworkOrder;
  
  SVar2 = RichTextCommandStream_CopyToNarrowCf
                    (0xff,(byte *)&g_NetworkEndpointTextScratchA,(word *)endpointText);
  if (SVar2.carry) {
    return true;
  }
  ipv4AddressNetworkOrder = g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder;
  if ((g_NetworkEndpointTextScratchA != '\0') &&
     (ipv4AddressNetworkOrder = (*g_WinSock_inet_addr)((byte *)&g_NetworkEndpointTextScratchA),
     ipv4AddressNetworkOrder == 0xffffffff)) {
    resolvedHostEntry = (*g_WinSock_gethostbyname)((byte *)&g_NetworkEndpointTextScratchA);
    if (resolvedHostEntry == (WinSockHostEnt32 *)0x0) {
      return true;
    }
    ipv4AddressNetworkOrder = *(NetworkIpv4AddressNetworkOrder *)*resolvedHostEntry->addressList;
  }
  NVar1 = g_NetworkFallbackBindEndpoint.addressHeader;
  endpointDescriptor16->zeroPadding[0] = 0;
  endpointDescriptor16->zeroPadding[1] = 0;
  endpointDescriptor16->zeroPadding[2] = 0;
  endpointDescriptor16->zeroPadding[3] = 0;
  endpointDescriptor16->zeroPadding[4] = 0;
  endpointDescriptor16->zeroPadding[5] = 0;
  endpointDescriptor16->zeroPadding[6] = 0;
  endpointDescriptor16->zeroPadding[7] = 0;
  endpointDescriptor16->addressHeader = NVar1;
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
  byte *source;
  
  source = (*g_WinSock_inet_ntoa)(socketAddress->ipv4AddressNetworkOrder);
  if (source != (byte *)0x0) {
    Text_CopyNarrowToUtf16Cf(0x200,(word *)outputText,source);
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
NetworkBackend_OpenAndBindActiveSocketCf(word portHostOrder)

{
  word wVar1;
  dword dVar2;
  undefined2 extraout_var;
  int iVar3;
  StatusValueEaxCf5 SVar4;
  StatusValueEaxCf5 SVar5;
  dword dStack_20;
  dword dStack_1c;
  
  dStack_1c = 0xffffffff;
  dVar2 = (*g_Ws2_32_socket)(g_NetworkBackendActiveAddressFamily,g_NetworkBackendActiveSocketType,
                             g_NetworkBackendActiveProtocol);
  if (dVar2 != 0xffffffff) {
    dStack_1c = dVar2;
    wVar1 = (*g_Ws2_32_htons)(portHostOrder);
    g_NetworkBackendPortNetworkOrderCarrier = CONCAT22(extraout_var,wVar1);
    g_NetworkBackendBindAddress.ipv4.ipv4AddressNetworkOrder = 0;
    g_NetworkBackendBindAddress._8_4_ = 0;
    g_NetworkBackendBindAddress._12_4_ = 0;
    g_NetworkBackendBindAddress.ipv4.addressHeader =
         (NetworkEndpointAddressHeader4)g_NetworkBackendActiveAddressFamily;
    dVar2 = g_NetworkBackendActiveSocketAddressLength;
    if (g_NetworkBackendActiveSocketAddressLength < 0x10) {
      dVar2 = 0x10;
    }
    if (g_NetworkBackendActiveAddressFamily == 2) {
      g_NetworkBackendBindAddress._2_2_ = wVar1;
      g_NetworkBackendBindAddress.ipx.addressFamily = 2;
    }
    else if (g_NetworkBackendActiveAddressFamily == 6) {
      g_NetworkBackendBindAddress._14_1_ = 0;
      g_NetworkBackendBindAddress._15_1_ = 0;
      g_NetworkBackendBindAddress.ipx.socketNetworkOrder = wVar1;
    }
    iVar3 = (*g_Ws2_32_bind)(dStack_1c,&g_NetworkBackendBindAddress.ipv4,dVar2);
    if (iVar3 == 0) {
      iVar3 = (*g_Ws2_32_setsockopt)(dStack_1c,0xffff,0x20,(byte *)0x583ef6,4);
      if (iVar3 == 0) {
        iVar3 = (*g_Ws2_32_WSAIoctl)
                          (dStack_1c,0x8004667e,(void *)0x583ef6,4,(void *)0x0,0,&dStack_20,
                           (void *)0x0,(void *)0x0);
        if (iVar3 == 0) {
          if (g_NetworkBackendActiveAddressFamily == 2) {
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPV4;
            g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder._0_2_ = 0xffff;
            g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder._2_2_ = 0xffff;
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.portNetworkOrder =
                 (NetworkPortNetworkOrder)g_NetworkBackendPortNetworkOrderCarrier;
          }
          else if (g_NetworkBackendActiveAddressFamily == 6) {
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.addressFamily =
                 NETWORK_ADDRESS_FAMILY_IPX;
            g_NetworkLocalEndpointDescriptor16.addressHeader.fields.portNetworkOrder = 0;
            g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder._0_2_ = 0;
            g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder._2_2_ = 0xffff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0xff;
            g_NetworkLocalEndpointDescriptor16.zeroPadding._4_2_ =
                 (NetworkPortNetworkOrder)g_NetworkBackendPortNetworkOrderCarrier;
          }
          g_NetworkFallbackSocket = dStack_1c;
          SVar4.carry = false;
          SVar4.valueOrError = dStack_1c;
          return SVar4;
        }
      }
    }
  }
  iVar3 = (*g_Ws2_32_WSAGetLastError)();
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar3,g_PackageLastErrorPath);
  if (dStack_1c != 0xffffffff) {
    (*g_Ws2_32_closesocket)(dStack_1c);
  }
  SVar5.carry = true;
  SVar5.valueOrError = 0x2a;
  return SVar5;
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
NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,byte *buffer)

{
  uint uVar1;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  g_NetworkFallbackAddressLength = g_NetworkBackendActiveSocketAddressLength;
  uVar1 = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    SVar2.valueOrError =
         (*g_Ws2_32_recvfrom)
                   (g_NetworkFallbackSocket,buffer,bufferLength,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    uVar1 = SVar2.valueOrError;
    if (-1 < (int)SVar2.valueOrError) {
      SVar2.carry = false;
      return SVar2;
    }
  }
  SVar3.carry = true;
  SVar3.valueOrError = uVar1;
  return SVar3;
}


/* Address: 0x005854E0.
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback udp send datagram.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_SendDatagram(WinSockAddress *destinationAddress,int byteCount,byte *buffer)

{
  NetworkSocketHandle32 NVar1;
  int arg4;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  NVar1 = g_NetworkFallbackSocket;
  if (g_NetworkFallbackSocket != 0xffffffff) {
    NVar1 = (*g_Ws2_32_sendto)(g_NetworkFallbackSocket,buffer,byteCount,0,destinationAddress,
                               g_NetworkBackendActiveSocketAddressLength);
    if ((int)NVar1 < 0) {
      arg4 = (*g_Ws2_32_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,arg4,g_PackageLastErrorPath);
      SVar3.carry = true;
      SVar3.valueOrError = 0x2a;
      return SVar3;
    }
  }
  SVar2.carry = false;
  SVar2.valueOrError = NVar1;
  return SVar2;
}


/* Address: 0x00585550.
   Ownership: network/backend/fallback_udp.
   Purpose: Parses endpoint text into the recovered backend address representation.
*/
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackend_ParseEndpointTextCf(NetworkEndpointAddressHeader4 *endpointOut,word *addressText)

{
  NetworkEndpointAddressHeader4 NVar1;
  NetworkEndpointAddressHeader4 NVar2;
  int iVar3;
  WinSockHostEnt32 *pWVar4;
  uint uVar5;
  NetworkEndpointAddressHeader4 *pNVar6;
  StatusValueEaxCf5 SVar7;
  int iStack_20;
  
  SVar7 = RichTextCommandStream_CopyToNarrowCf
                    (0xff,(byte *)&g_NetworkEndpointTextScratchA,addressText);
  if (SVar7.carry) {
    return true;
  }
  if (g_NetworkEndpointTextScratchA != '\0') {
    iVar3 = (*g_Ws2_32_WSAStringToAddressA)
                      (&g_NetworkEndpointTextScratchA,g_NetworkBackendActiveAddressFamily,
                       (void *)0x0,(NetworkBackendSocketAddress16 *)endpointOut,&iStack_20);
    if (iVar3 != 0) {
      pWVar4 = (*g_Ws2_32_gethostbyname)((byte *)&g_NetworkEndpointTextScratchA);
      NVar2 = g_NetworkFallbackBindEndpoint.addressHeader;
      if (pWVar4 == (WinSockHostEnt32 *)0x0) {
        return true;
      }
      NVar1 = *(NetworkEndpointAddressHeader4 *)*pWVar4->addressList;
      endpointOut[2].packedFamilyAndPort = 0;
      endpointOut[3].packedFamilyAndPort = 0;
      *endpointOut = NVar2;
      endpointOut[1] = NVar1;
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
  uVar5 = g_NetworkBackendActiveSocketAddressLength >> 2;
  pNVar6 = &g_NetworkLocalEndpointDescriptor16.addressHeader;
  for (; uVar5 != 0; uVar5 = uVar5 - 1) {
    *endpointOut = *pNVar6;
    pNVar6 = pNVar6 + 1;
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
NetworkFallback_FormatAddressUtf16(word *outputUtf16,WinSockAddress *address)

{
  int iVar1;
  StatusValueEaxCf5 SVar2;
  dword dStack_20;
  
  dStack_20 = 0xff;
  iVar1 = (*g_Ws2_32_WSAAddressToStringA)
                    (address,g_NetworkBackendActiveSocketAddressLength,(void *)0x0,
                     &g_NetworkEndpointTextScratchA,&dStack_20);
  if (iVar1 == 0) {
    SVar2 = Text_CopyNarrowToUtf16Cf(0x200,outputUtf16,&g_NetworkEndpointTextScratchA);
    return SVar2.carry;
  }
  outputUtf16[0] = 0;
  outputUtf16[1] = 0;
  return false;
}

