/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/fallback_udp.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H
#define THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H

#include <thandor/core/types.h>
#include <thandor/network/backend/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

/* Submodule: network/backend/fallback_udp. */
/* Functions are grouped by semantic ownership. */

uint32_t NetworkBackendFallback_SetSessionContext(uint32_t backendIndex);

void __cdecl NetworkBackendFallback_Cleanup();

uint32_t NetworkBackendFallback_OpenAndBindUdpSocket(uint32_t localPort);

void __cdecl NetworkBackendFallback_CloseActiveSocket();

Bool8 NetworkBackendFallback_ReceiveDatagram(WinSockAddress *sourceAddress,uint32_t byteCount,uint8_t *buffer);

Bool8 NetworkBackendFallback_SendDatagram(WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

Bool8 NetworkBackendFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpoint,char *endpointText);

void NetworkBackendFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

void NetworkFallback_NoOpBackendCleanup();

uint32_t NetworkFallback_OpenAndBindUdpSocket(NetworkPortHostOrder localPort);

void NetworkFallback_CloseActiveSocket();

Bool8 NetworkFallback_ReceiveDatagram
          (WinSockAddress *sourceAddress,NetworkByteCount byteCount,uint8_t *buffer);

Bool8 NetworkFallback_SendDatagram
          (WinSockAddress *destinationAddress,NetworkByteCount byteCount,uint8_t *buffer);

Bool8 NetworkFallback_ParsePeerEndpoint(UiTransferEndpointDescriptor *endpointDescriptor16,char *endpointText);

void NetworkFallback_FormatPeerAddress(char *outputText,WinSockAddress *socketAddress);

extern UiTransferEndpointDescriptor g_NetworkLocalEndpoint;

#endif /* THANDOR_NETWORK_BACKEND_FALLBACK_UDP_H */
