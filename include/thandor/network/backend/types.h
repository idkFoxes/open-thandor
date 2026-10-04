/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/backend/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_BACKEND_TYPES_H
#define THANDOR_NETWORK_BACKEND_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef union NetworkEndpointAddressHeader4 NetworkEndpointAddressHeader4, *PNetworkEndpointAddressHeader4;
typedef struct NetworkEndpointFamilyPortFields4 NetworkEndpointFamilyPortFields4, *PNetworkEndpointFamilyPortFields4;
typedef struct WinSockHostEnt32 WinSockHostEnt32, *PWinSockHostEnt32;
typedef struct WinSockAddress WinSockAddress, *PWinSockAddress;
typedef struct WinSockData11 WinSockData11, *PWinSockData11;
typedef struct NetworkSessionContext NetworkSessionContext, *PNetworkSessionContext;
typedef struct NetworkBackendInstanceDescriptorPrefix NetworkBackendInstanceDescriptorPrefix, *PNetworkBackendInstanceDescriptorPrefix;
typedef struct UiTransferEndpointDescriptor UiTransferEndpointDescriptor;

enum /* NetworkAddressFamily, stored in 2 byte(s) */ {
    NETWORK_ADDRESS_FAMILY_UNSPECIFIED=0,
    NETWORK_ADDRESS_FAMILY_IPV4=2,
    NETWORK_ADDRESS_FAMILY_IPX=6
};
typedef uint16_t NetworkAddressFamily;

typedef uint16_t NetworkPortNetworkOrder;

struct NetworkEndpointFamilyPortFields4 {
    NetworkAddressFamily addressFamily; 
    NetworkPortNetworkOrder portNetworkOrder; 
};

union NetworkEndpointAddressHeader4 {
    struct NetworkEndpointFamilyPortFields4 fields; 
    uint32_t packedFamilyAndPort; 
};

enum /* WinSockIpv4AddressLength, stored in 2 byte(s) */ {
    WINSOCK_IPV4_ADDRESS_BYTES=4
};
typedef uint16_t WinSockIpv4AddressLength;

typedef uint32_t NetworkIpv4AddressNetworkOrder;

typedef uint16_t WinSockSocketCount16;

typedef uint32_t NetworkPortHostOrder;

typedef uint16_t WinSockVersionWord;

typedef uint32_t NetworkByteCount;

typedef uint16_t NetworkDatagramByteCount16;

struct WinSockHostEnt32 {
    uint8_t *canonicalName;
    uint8_t **aliases;
    NetworkAddressFamily addressType; 
    WinSockIpv4AddressLength addressLength; 
    uint8_t **addressList;
};

struct WinSockAddress {
    union NetworkEndpointAddressHeader4 addressHeader; 
    NetworkIpv4AddressNetworkOrder ipv4AddressNetworkOrder; 
    uint8_t zeroPadding[8]; 
};
#pragma pack(push, 1) /* packed layout: no alignment padding */
struct WinSockData11 {
    WinSockVersionWord version; // WinSock startup metadata.
    WinSockVersionWord highestVersion; // WinSock startup metadata.
    uint8_t description[257];
    uint8_t systemStatus[129];
    WinSockSocketCount16 maximumSockets; // WinSock startup metadata.
    NetworkDatagramByteCount16 maximumUdpDatagram; // WinSock startup metadata.
    uint8_t *vendorInfo; // Historical WinSock 1.x vendor info pointer; structure is packed with no alignment padding before this field.
};
#pragma pack(pop)

struct NetworkSessionContext {
    uint32_t transportContext00; 
    uint32_t ipv4AddressNetworkOrder; 
    uint8_t reserved08_FF[248];
};

struct NetworkBackendInstanceDescriptorPrefix {
    uint32_t addressFamily; // Winsock address family used as socket(af,...)
    uint32_t socketAddressLength; // sockaddr byte length used by bind/send/recv/address conversion
    uint32_t socketType; // Winsock socket type used as socket(...,type,...)
    uint32_t protocol; // Winsock protocol used as socket(...,...,protocol)
    uint16_t displayNameUtf16[20]; // Twenty UTF-16 code units: WinSock32 1.1 - UDP.
};

typedef uint32_t NetworkSocketHandle32;
typedef void NetworkBackendCleanupCallback();
typedef void NetworkBackendCloseCallback();
typedef void NetworkBackendFormatAddressCallback(char * outputText, WinSockAddress * socketAddress);
typedef uint32_t NetworkBackendOpenBindCallback(uint32_t localPort); /* 0 or a FATAL_ERROR_NETWORK_* code */
typedef Bool8 NetworkBackendParseEndpointCallback(UiTransferEndpointDescriptor * endpoint, char * endpointText);
typedef Bool8 NetworkBackendReceiveCallback(WinSockAddress * sourceAddress, uint32_t byteCount, uint8_t * buffer); /* true when a datagram was received */
typedef Bool8 NetworkBackendSendCallback(WinSockAddress * destinationAddress, uint32_t byteCount, uint8_t * buffer); /* true on success */
typedef uint32_t NetworkBackendSetSessionCallback(uint32_t backendIndex); /* 0 or a FATAL_ERROR_NETWORK_* code */
typedef int __stdcall WinSock_WSACleanupProc();
typedef int __stdcall WinSock_WSAGetLastErrorProc();
typedef int __stdcall WinSock_WSAStartupProc(uint16_t requestedVersion, WinSockData11 * startupData);
typedef int __stdcall WinSock_bindProc(uint32_t socket, WinSockAddress * address, int addressLength);
typedef int __stdcall WinSock_closesocketProc(uint32_t socket);
typedef WinSockHostEnt32 * __stdcall WinSock_gethostbynameProc(uint8_t * hostName);
typedef uint16_t __stdcall WinSock_htonsProc(uint16_t hostShort);
typedef uint32_t __stdcall WinSock_inet_addrProc(uint8_t * addressText);
typedef uint8_t * __stdcall WinSock_inet_ntoaProc(uint32_t ipv4AddressNetworkOrder);
typedef int __stdcall WinSock_ioctlsocketProc(uint32_t socket, uint32_t command, uint32_t * argument);
typedef int __stdcall WinSock_recvfromProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * sourceAddress, int * sourceAddressLength);
typedef int __stdcall WinSock_sendtoProc(uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * destinationAddress, int destinationAddressLength);
typedef int __stdcall WinSock_setsockoptProc(uint32_t socket, int level, int optionName, uint8_t * optionValue, int optionLength);
typedef uint32_t __stdcall WinSock_socketProc(int addressFamily, int socketType, int protocol);

#endif /* THANDOR_NETWORK_BACKEND_TYPES_H */
