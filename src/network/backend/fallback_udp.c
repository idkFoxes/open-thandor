#include <thandor/network/backend/fallback_udp.h>

/* Implementation ownership: network/backend/fallback_udp. */

/* Address: 0x0041A580.
   Ownership: network/backend/fallback_udp.
   Purpose: Default implementation for network backend slot 0. It returns EAX 0x2B, sets CF, consumes one dword
   argument, and performs no other work.
*/
dword NetworkBackendFallback_Slot0_ReturnError43Cf(dword argument)

{
  return 0x2b;
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
dword NetworkBackendFallback_Slot2_ReturnError43Cf(dword argument)

{
  return 0x2b;
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
void NetworkBackendFallback_Slot6_TwoArgFailureCf
               (UiTransferEndpointDescriptor *endpoint,char *endpointText)

{
  return;
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
void __cdecl NetworkFallback_NoOpBackendCleanup(void)

{
  return;
}

/* Address: 0x00584E80.
   Ownership: network/backend/fallback_udp.
   Purpose: Creates an IPv4 UDP socket, resolves the configured local address when present, binds the requested
   local port, applies broadcast/nonblocking/event options, and publishes the active fallback socket. CF reports
   failure and EAX carries engine error 0x2A on setup errors.
*/
dword NetworkFallback_OpenAndBindUdpSocketCf(NetworkPortHostOrder localPort)

{
  byte bVar1;
  byte *pbVar2;
  dword socket;
  NetworkIpv4AddressNetworkOrder NVar3;
  WinSockHostEnt32 *pWVar4;
  int iVar5;
  NetworkIpv4AddressNetworkOrder extraout_EDX;
  int unaff_EBX;
  byte *pbVar6;
  byte *pbVar7;
  bool bVar8;
  dword dStack_1c;
  
  dStack_1c = 0xffffffff;
  socket = (*g_WinSock_socket)(2,2,0x11);
  if (socket != 0xffffffff) {
    bVar8 = false;
    (*g_CommandLineFindOption)(3,(char *)0x584078);
    NVar3 = extraout_EDX;
    if (!bVar8) {
      pbVar6 = (byte *)(unaff_EBX + 4);
      pbVar2 = g_PackageScratchBuffer;
      if (*(char *)(unaff_EBX + 3) == '\"') {
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
          NVar3 = (*g_WinSock_inet_addr)(g_PackageScratchBuffer);
          if (NVar3 == 0xffffffff) {
            pWVar4 = (*g_WinSock_gethostbyname)(g_PackageScratchBuffer);
            NVar3 = 0;
            if (pWVar4 != (WinSockHostEnt32 *)0x0) {
              NVar3 = *(NetworkIpv4AddressNetworkOrder *)*pWVar4->addressList;
            }
          }
        }
      }
    }
NetworkFallback_OpenAndBindUdpSocketCf_ConfigureEndpointAfterOptionalBindAddressResolution:
    g_NetworkFallbackBindEndpoint.addressHeader.fields.portNetworkOrder =
         (*g_WinSock_htons)((word)localPort);
    g_NetworkFallbackBindEndpoint.ipv4AddressNetworkOrder = NVar3;
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
    iVar5 = (*g_WinSock_bind)(socket,&g_NetworkFallbackBindEndpoint,0x10);
    dStack_1c = socket;
    if (iVar5 == 0) {
      iVar5 = (*g_WinSock_setsockopt)(socket,0xffff,0x20,(byte *)0x583ef6,4);
      if (iVar5 == 0) {
        iVar5 = (*g_WinSock_ioctlsocket)(socket,0x8004667e,(dword *)0x583ef6);
        if (iVar5 == 0) {
          g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder = 0xffffffff;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[0] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[1] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[2] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[3] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[4] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[5] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[6] = 0;
          g_NetworkLocalEndpointDescriptor16.zeroPadding[7] = 0;
          ram0x00583ef2 = socket;
          return socket;
        }
      }
    }
  }
  iVar5 = (*g_WinSock_WSAGetLastError)();
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar5,g_PackageLastErrorPath);
  if (dStack_1c != 0xffffffff) {
    (*g_WinSock_closesocket)(dStack_1c);
  }
  return 0x2a;
}

/* Address: 0x00585030.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback network-backend close callback. It atomically replaces the active socket with -1 and closes
   the previous socket when present.
*/
void __cdecl NetworkFallback_CloseActiveSocket(void)

{
  dword socket;
  
  socket = ram0x00583ef2;
  if (ram0x00583ef2 != 0xffffffff) {
    LOCK();
    ram0x00583ef2 = 0xffffffff;
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
dword NetworkFallback_ReceiveDatagramCf
                (WinSockAddress *sourceAddress,NetworkByteCount byteCount,byte *buffer)

{
  dword receivedByteCount;
  
  g_NetworkFallbackAddressLength = 0x10;
  receivedByteCount = ram0x00583ef2;
  if (ram0x00583ef2 != 0xffffffff) {
    receivedByteCount =
         (*g_WinSock_recvfrom)
                   (ram0x00583ef2,buffer,byteCount,0,sourceAddress,
                    (int *)&g_NetworkFallbackAddressLength);
    if (-1 < (int)receivedByteCount) {
      return receivedByteCount;
    }
  }
  return receivedByteCount;
}

/* Address: 0x005850B0.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback UDP send callback wrapping sendto with a fixed 16-byte destination-address length. It
   preserves the legacy error-reporting path and returns status through EAX and CF.
*/
dword NetworkFallback_SendDatagramCf
                (WinSockAddress *destinationAddress,NetworkByteCount byteCount,byte *buffer)

{
  dword sentByteCount;
  int winsockErrorCode;
  
  sentByteCount = ram0x00583ef2;
  if (ram0x00583ef2 != 0xffffffff) {
    sentByteCount = (*g_WinSock_sendto)(ram0x00583ef2,buffer,byteCount,0,destinationAddress,0x10);
    if ((int)sentByteCount < 0) {
      winsockErrorCode = (*g_WinSock_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,winsockErrorCode,g_PackageLastErrorPath);
      return 0x2a;
    }
  }
  return sentByteCount;
}

/* Address: 0x00585120.
   Ownership: network/backend/fallback_udp.
   Purpose: Parses a narrow peer endpoint string into the backend 16-byte address descriptor. Uses numeric IPv4
   conversion first and host lookup as fallback. CF reports parse failure.
   Cross-module calls: RichTextCommandStream_CopyToNarrowCf [assets/text/richtext].
*/
void NetworkFallback_ParsePeerEndpointCf
               (UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText)

{
  NetworkEndpointAddressHeader4 NVar1;
  NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder;
  WinSockHostEnt32 *resolvedHostEntry;
  undefined1 in_CF;
  NetworkPortNetworkOrder portNetworkOrder;
  
  RichTextCommandStream_CopyToNarrowCf
            (0xff,(byte *)&g_NetworkEndpointTextScratchA,(word *)endpointText);
  if ((bool)in_CF) {
    return;
  }
  ipv4AddressNetworkOrder = g_NetworkLocalEndpointDescriptor16.ipv4AddressNetworkOrder;
  if ((g_NetworkEndpointTextScratchA != '\0') &&
     (ipv4AddressNetworkOrder = (*g_WinSock_inet_addr)((byte *)&g_NetworkEndpointTextScratchA),
     ipv4AddressNetworkOrder == 0xffffffff)) {
    resolvedHostEntry = (*g_WinSock_gethostbyname)((byte *)&g_NetworkEndpointTextScratchA);
    if (resolvedHostEntry == (WinSockHostEnt32 *)0x0) {
      return;
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
  return;
}

/* Address: 0x005851C0.
   Ownership: network/backend/fallback_udp.
   Purpose: Fallback network callback that formats the address field at socket-address offset +4 into a bounded
   0x200-byte narrow output string, or writes an empty string when conversion fails.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress)

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

/* Address: 0x00585450.
   Ownership: network/backend/fallback_udp.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
undefined8 __fastcall NetworkFallbackUdp_CloseSocket(undefined4 param_1,undefined4 param_2)

{
  dword socket;
  undefined4 in_EAX;
  
  socket = ram0x00583ef2;
  if (ram0x00583ef2 != 0xffffffff) {
    LOCK();
    ram0x00583ef2 = 0xffffffff;
    UNLOCK();
    (*g_Ws2_32_closesocket)(socket);
  }
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x00585480.
   Ownership: network/backend/fallback_udp.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *param_1,int param_2,byte *param_3)

{
  int iVar1;
  
  g_NetworkFallbackAddressLength = DAT_00583f12;
  if (ram0x00583ef2 != 0xffffffff) {
    iVar1 = (*g_Ws2_32_recvfrom)
                      (ram0x00583ef2,param_3,param_2,0,param_1,
                       (int *)&g_NetworkFallbackAddressLength);
    if (-1 < iVar1) {
      return;
    }
  }
  return;
}

/* Address: 0x005854E0.
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback udp send datagram.
*/
undefined8 __fastcall
NetworkFallbackUdp_SendDatagram
          (undefined4 param_1,undefined4 param_2,WinSockAddress *param_3,int param_4,byte *param_5)

{
  dword dVar1;
  int arg4;
  
  dVar1 = ram0x00583ef2;
  if (ram0x00583ef2 != 0xffffffff) {
    dVar1 = (*g_Ws2_32_sendto)(ram0x00583ef2,param_5,param_4,0,param_3,DAT_00583f12);
    if ((int)dVar1 < 0) {
      arg4 = (*g_Ws2_32_WSAGetLastError)();
      (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,arg4,g_PackageLastErrorPath);
      return CONCAT44(param_2,0x2a);
    }
  }
  return CONCAT44(param_2,dVar1);
}

/* Address: 0x00585640.
   Ownership: network/backend/fallback_udp.
   Purpose: Handles network fallback format address utf16.
   Cross-module calls: Text_CopyNarrowToUtf16Cf [core/text/string].
*/
undefined8 __fastcall
NetworkFallback_FormatAddressUtf16
          (undefined4 param_1,undefined4 param_2,word *param_3,undefined4 param_4)

{
  undefined4 in_EAX;
  int iVar1;
  undefined4 uStack_20;
  
  uStack_20 = 0xff;
  iVar1 = (*DAT_00574478)(param_4,DAT_00583f12,0,&g_NetworkEndpointTextScratchA,&uStack_20);
  if (iVar1 == 0) {
    Text_CopyNarrowToUtf16Cf(0x200,param_3,&g_NetworkEndpointTextScratchA);
    return CONCAT44(param_2,in_EAX);
  }
  param_3[0] = 0;
  param_3[1] = 0;
  return CONCAT44(param_2,in_EAX);
}
