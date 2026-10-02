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
uint32_t NetworkBackendFallback_SetSessionContext(uint32_t backendIndex);

/* 0x0041A590 */
void __cdecl NetworkBackendFallback_Cleanup(void);

/* 0x0041A5A0 */
uint32_t NetworkBackendFallback_OpenAndBindUdpSocket(uint32_t localPort);

/* 0x0041A5B0 */
void __cdecl NetworkBackendFallback_CloseActiveSocket(void);

/* 0x0041A5C0 */
bool NetworkBackendFallback_ReceiveDatagram(WinSockAddress *sourceAddress,uint32_t byteCount,uint8_t *buffer);

/* 0x0041A5D0 */
bool NetworkBackendFallback_SendDatagram(WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x0041A5E0 */
bool NetworkBackendFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpoint,char *endpointText);

/* 0x0041A5F0 */
void NetworkBackendFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

/* 0x00584E70 */
void NetworkFallback_NoOpBackendCleanup(void);

/* 0x00584E80 */
uint32_t NetworkFallback_OpenAndBindUdpSocket(NetworkPortHostOrder localPort);

/* 0x00585030 */
void NetworkFallback_CloseActiveSocket(void);

/* 0x00585060 */
bool NetworkFallback_ReceiveDatagram
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x005850B0 */
bool NetworkFallback_SendDatagram
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

/* 0x00585120 */
bool NetworkFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText);

/* 0x005851C0 */
void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

/* 0x00585450 */
void NetworkFallbackUdp_CloseSocket(void);

/* 0x00585480: the received byte count, or -1 */
int NetworkFallbackUdp_ReceiveDatagram(WinSockAddress *sourceAddress,int bufferLength,uint8_t *buffer);

/* 0x005854E0: 0, or FATAL_ERROR_NETWORK_SOCKET */
uint32_t NetworkFallbackUdp_SendDatagram
          (WinSockAddress *destinationAddress,int byteCount,uint8_t *buffer,uint32_t *outSentByteCount);

/* 0x00585640 */
bool NetworkFallback_FormatAddressUtf16(uint16_t *outputUtf16,WinSockAddress *address);


/* 0x00585290 */
void NetworkBackend_NoOpCleanup(void);

/* 0x005852A0: 0, or FATAL_ERROR_NETWORK_SOCKET */
uint32_t NetworkBackend_OpenAndBindActiveSocket(uint16_t portHostOrder,uint32_t *outSocket);

/* 0x00585550 */
bool NetworkBackend_ParseEndpointText(NetworkEndpointAddressHeader4 *endpointOut,uint16_t *addressText);

#endif /* THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H */
