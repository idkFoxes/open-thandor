/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/fallback_udp.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H
#define THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/backend/fallback_udp. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041A580 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot0_ReturnError43Cf(uint32_t argument);

/* 0x0041A590 */
void __cdecl NetworkBackendFallback_Slot1_NoOp(void);

/* 0x0041A5A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackendFallback_Slot2_ReturnError43Cf(uint32_t argument);

/* 0x0041A5B0 */
void __cdecl NetworkBackendFallback_Slot3_NoOp(void);

/* 0x0041A5C0 */
NetworkBackendReceiveEaxCf5 __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot4_ThreeArgFailureCf (WinSockAddress *sourceAddress,uint32_t argument1,uint8_t *buffer);

/* 0x0041A5D0 */
NetworkBackendSendEaxCf5 __thandor_eax_cf_preserve_ecx_edx NetworkBackendFallback_Slot5_ThreeArgSuccessCf (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x0041A5E0 */
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackendFallback_Slot6_TwoArgFailureCf
          (UiTransferEndpointDescriptor *endpoint,char *endpointText);

/* 0x0041A5F0 */
void NetworkBackendFallback_Slot7_ClearOutput(char *outputText,WinSockAddress *socketAddress);

/* 0x00584E70 */
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_NoOpBackendCleanup(void);

/* 0x00584E80 */
NetworkBackendOpenBindEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_OpenAndBindUdpSocketCf(NetworkPortHostOrder localPort);

/* 0x00585030 */
void __thandor_void_preserve_eax_ecx_edx NetworkFallback_CloseActiveSocket(void);

/* 0x00585060 */
NetworkBackendReceiveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_ReceiveDatagramCf
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x005850B0 */
NetworkBackendSendEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallback_SendDatagramCf
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x00585120 */
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_ParsePeerEndpointCf
          (UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText);

/* 0x005851C0 */
void __thandor_void_preserve_eax_ecx_edx
NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

/* 0x00585450 */
void __thandor_void_preserve_eax_ecx_edx NetworkFallbackUdp_CloseSocket(void);

/* 0x00585480 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,uint8_t *buffer);

/* 0x005854E0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkFallbackUdp_SendDatagram(WinSockAddress *destinationAddress,int byteCount,uint8_t *buffer);

/* 0x00585640 */
bool __thandor_cf_preserve_eax_ecx_edx
NetworkFallback_FormatAddressUtf16(uint16_t *outputUtf16,WinSockAddress *address);


/* 0x00585290 */
void __thandor_void_preserve_eax_ecx_edx NetworkBackend_NoOpCleanup(void);

/* 0x005852A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
NetworkBackend_OpenAndBindActiveSocketCf(uint16_t portHostOrder);

/* 0x00585550 */
bool __thandor_cf_preserve_eax_ecx_edx
NetworkBackend_ParseEndpointTextCf(NetworkEndpointAddressHeader4 *endpointOut,uint16_t *addressText);

#endif /* THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H */
