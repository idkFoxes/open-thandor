/*
 * Open Thandor - native WinSock2 bridge
 *
 * This translation unit is the only network file allowed to include the Windows WinSock headers.
 * The reconstructed project deliberately supplies its own Win32 ABI declarations, so mixing the
 * SDK headers with <thandor/...> headers causes duplicate HINSTANCE/DWORD/OVERLAPPED/etc definitions.
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <string.h>

#include "winsock2_native.h"

static_assert(sizeof(uintptr_t) == sizeof(SOCKET), "native SOCKET must fit the opaque bridge handle");

static WSADATA g_ThandorWs2StartupData = {};

extern "C" int ThandorWs2_Startup22(void)
{
  return ::WSAStartup(MAKEWORD(2,2),&g_ThandorWs2StartupData);
}

extern "C" void ThandorWs2_Cleanup(void)
{
  ::WSACleanup();
}

extern "C" int ThandorWs2_Bind(uintptr_t socketHandle,const void *address,int addressLength)
{
  return ::bind((SOCKET)socketHandle,reinterpret_cast<const sockaddr *>(address),addressLength);
}

extern "C" int ThandorWs2_CloseSocket(uintptr_t socketHandle)
{
  return ::closesocket((SOCKET)socketHandle);
}

extern "C" uint16_t ThandorWs2_Htons(uint16_t hostShort)
{
  return ::htons(hostShort);
}

extern "C" uint32_t ThandorWs2_InetAddr(const char *addressText)
{
  return ::inet_addr(addressText);
}

extern "C" const char *ThandorWs2_InetNtoa(uint32_t ipv4AddressNetworkOrder)
{
  in_addr address;
  address.s_addr = ipv4AddressNetworkOrder;
  return ::inet_ntoa(address);
}

extern "C" int ThandorWs2_IoctlSocket(uintptr_t socketHandle,uint32_t command,uint32_t *argument)
{
  u_long nativeArgument = argument != nullptr ? (u_long)*argument : 0;
  int result = ::ioctlsocket((SOCKET)socketHandle,(long)command,&nativeArgument);
  if (argument != nullptr) *argument = (uint32_t)nativeArgument;
  return result;
}

extern "C" int ThandorWs2_RecvFrom(uintptr_t socketHandle,void *buffer,int length,int flags,
                                    void *sourceAddress,int *sourceAddressLength)
{
  return ::recvfrom((SOCKET)socketHandle,reinterpret_cast<char *>(buffer),length,flags,
                    reinterpret_cast<sockaddr *>(sourceAddress),sourceAddressLength);
}

extern "C" int ThandorWs2_SendTo(uintptr_t socketHandle,const void *buffer,int length,int flags,
                                  const void *destinationAddress,int destinationAddressLength)
{
  return ::sendto((SOCKET)socketHandle,reinterpret_cast<const char *>(buffer),length,flags,
                  reinterpret_cast<const sockaddr *>(destinationAddress),destinationAddressLength);
}

extern "C" int ThandorWs2_SetSockOpt(uintptr_t socketHandle,int level,int optionName,
                                      const void *optionValue,int optionLength)
{
  return ::setsockopt((SOCKET)socketHandle,level,optionName,
                      reinterpret_cast<const char *>(optionValue),optionLength);
}

extern "C" uintptr_t ThandorWs2_Socket(int addressFamily,int socketType,int protocol)
{
  return (uintptr_t)::socket(addressFamily,socketType,protocol);
}

extern "C" int ThandorWs2_ResolveHostIpv4(const char *hostName,uint32_t *ipv4AddressNetworkOrder)
{
  hostent *entry = ::gethostbyname(hostName);
  if (entry == nullptr || entry->h_addrtype != AF_INET || entry->h_length < 4 ||
      entry->h_addr_list == nullptr || entry->h_addr_list[0] == nullptr ||
      ipv4AddressNetworkOrder == nullptr) {
    return -1;
  }
  memcpy(ipv4AddressNetworkOrder,entry->h_addr_list[0],sizeof(*ipv4AddressNetworkOrder));
  return 0;
}

extern "C" int ThandorWs2_GetLastError(void)
{
  return ::WSAGetLastError();
}

extern "C" int ThandorWs2_WSAIoctl(uintptr_t socketHandle,uint32_t controlCode,void *inputBuffer,
                                    uint32_t inputBytes,void *outputBuffer,uint32_t outputBytes,
                                    uint32_t *bytesReturned,void *overlapped,void *completionRoutine)
{
  DWORD nativeBytesReturned = 0;
  int result = ::WSAIoctl((SOCKET)socketHandle,(DWORD)controlCode,inputBuffer,(DWORD)inputBytes,
                          outputBuffer,(DWORD)outputBytes,&nativeBytesReturned,
                          reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
                          reinterpret_cast<LPWSAOVERLAPPED_COMPLETION_ROUTINE>(completionRoutine));
  if (bytesReturned != nullptr) *bytesReturned = (uint32_t)nativeBytesReturned;
  return result;
}
