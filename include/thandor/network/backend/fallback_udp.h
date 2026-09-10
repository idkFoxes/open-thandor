#ifndef THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H
#define THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/backend/fallback_udp. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041A580 */
dword NetworkBackendFallback_Slot0_ReturnError43Cf(dword argument);

/* 0x0041A590 */
void __cdecl NetworkBackendFallback_Slot1_NoOp(void);

/* 0x0041A5A0 */
dword NetworkBackendFallback_Slot2_ReturnError43Cf(dword argument);

/* 0x0041A5B0 */
void __cdecl NetworkBackendFallback_Slot3_NoOp(void);

/* 0x0041A5C0 */
void NetworkBackendFallback_Slot4_ThreeArgFailureCf (WinSockAddress *sourceAddress,dword argument1,byte *buffer);

/* 0x0041A5D0 */
void NetworkBackendFallback_Slot5_ThreeArgSuccessCf (WinSockAddress *destinationAddress,NetworkByteCount byteCount,byte *buffer);

/* 0x0041A5E0 */
void NetworkBackendFallback_Slot6_TwoArgFailureCf (UiTransferEndpointDescriptor *endpoint,char *endpointText);

/* 0x0041A5F0 */
void NetworkBackendFallback_Slot7_ClearOutput(char *outputText,WinSockAddress *socketAddress);

/* 0x00584E70 */
void __cdecl NetworkFallback_NoOpBackendCleanup(void);

/* 0x00584E80 */
dword NetworkFallback_OpenAndBindUdpSocketCf(NetworkPortHostOrder localPort);

/* 0x00585030 */
void __cdecl NetworkFallback_CloseActiveSocket(void);

/* 0x00585060 */
dword NetworkFallback_ReceiveDatagramCf (WinSockAddress *sourceAddress,NetworkByteCount byteCount,byte *buffer);

/* 0x005850B0 */
dword NetworkFallback_SendDatagramCf (WinSockAddress *destinationAddress,NetworkByteCount byteCount,byte *buffer);

/* 0x00585120 */
void NetworkFallback_ParsePeerEndpointCf (UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText);

/* 0x005851C0 */
void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

/* 0x00585450 */
undefined8 __fastcall NetworkFallbackUdp_CloseSocket(undefined4 param_1,undefined4 param_2);

/* 0x00585480 */
void NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *param_1,int param_2,byte *param_3);

/* 0x005854E0 */
undefined8 __fastcall NetworkFallbackUdp_SendDatagram (undefined4 param_1,undefined4 param_2,WinSockAddress *param_3,int param_4,byte *param_5);

/* 0x00585640 */
undefined8 __fastcall NetworkFallback_FormatAddressUtf16 (undefined4 param_1,undefined4 param_2,word *param_3,undefined4 param_4);

#endif /* THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H */
