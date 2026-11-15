/*
 * Open Thandor - native WinSock2 bridge
 *
 * Keep the real Windows SDK types isolated from the reconstructed game's custom Win32 ABI types.
 * This header intentionally exposes only fixed-width/opaque C ABI values so it can be included by
 * src/network/backend/runtime.cpp without pulling <windows.h>/<winsock2.h> into the recovered headers.
 */
#ifndef THANDOR_NETWORK_BACKEND_WINSOCK2_NATIVE_H
#define THANDOR_NETWORK_BACKEND_WINSOCK2_NATIVE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int ThandorWs2_Startup22(void);
void ThandorWs2_Cleanup(void);

int ThandorWs2_Bind(uintptr_t socketHandle,const void *address,int addressLength);
int ThandorWs2_CloseSocket(uintptr_t socketHandle);
uint16_t ThandorWs2_Htons(uint16_t hostShort);
uint32_t ThandorWs2_InetAddr(const char *addressText);
const char *ThandorWs2_InetNtoa(uint32_t ipv4AddressNetworkOrder);
int ThandorWs2_IoctlSocket(uintptr_t socketHandle,uint32_t command,uint32_t *argument);
int ThandorWs2_RecvFrom(uintptr_t socketHandle,void *buffer,int length,int flags,
                        void *sourceAddress,int *sourceAddressLength);
int ThandorWs2_SendTo(uintptr_t socketHandle,const void *buffer,int length,int flags,
                      const void *destinationAddress,int destinationAddressLength);
int ThandorWs2_SetSockOpt(uintptr_t socketHandle,int level,int optionName,
                          const void *optionValue,int optionLength);
uintptr_t ThandorWs2_Socket(int addressFamily,int socketType,int protocol);
int ThandorWs2_ResolveHostIpv4(const char *hostName,uint32_t *ipv4AddressNetworkOrder);
int ThandorWs2_GetLastError(void);
int ThandorWs2_WSAIoctl(uintptr_t socketHandle,uint32_t controlCode,void *inputBuffer,
                        uint32_t inputBytes,void *outputBuffer,uint32_t outputBytes,
                        uint32_t *bytesReturned,void *overlapped,void *completionRoutine);

#ifdef __cplusplus
}
#endif

#endif /* THANDOR_NETWORK_BACKEND_WINSOCK2_NATIVE_H */
