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

union NetworkEndpointAddressHeader4;
struct NetworkEndpointFamilyPortFields4;
struct WinSockHostEnt32;
struct WinSockAddress;
struct WinSockData11;
struct NetworkSessionContext;
struct NetworkBackendInstanceDescriptorPrefix;
struct UiTransferEndpointDescriptor;

/* WinSock address family, stored in 2 bytes (sin_family, h_addrtype). */
enum class NetworkAddressFamily : uint16_t {
    NETWORK_ADDRESS_FAMILY_UNSPECIFIED=0,
    NETWORK_ADDRESS_FAMILY_IPV4=2,
    NETWORK_ADDRESS_FAMILY_IPX=6
};

using NetworkPortNetworkOrder = uint16_t;

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
using WinSockIpv4AddressLength = uint16_t;

using NetworkIpv4AddressNetworkOrder = uint32_t;

using WinSockSocketCount16 = uint16_t;

using NetworkPortHostOrder = uint32_t;

using WinSockVersionWord = uint16_t;

using NetworkByteCount = uint32_t;

using NetworkDatagramByteCount16 = uint16_t;

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

using NetworkSocketHandle32 = uint32_t;
using NetworkBackendCleanupCallback = void ();
using NetworkBackendCloseCallback = void ();
using NetworkBackendFormatAddressCallback = void (char * outputText, WinSockAddress * socketAddress);
using NetworkBackendOpenBindCallback = uint32_t (uint32_t localPort); /* 0 or a FATAL_ERROR_NETWORK_* code */
using NetworkBackendParseEndpointCallback = bool (UiTransferEndpointDescriptor * endpoint, char * endpointText);
using NetworkBackendReceiveCallback = bool (WinSockAddress * sourceAddress, uint32_t byteCount, uint8_t * buffer); /* true when a datagram was received */
using NetworkBackendSendCallback = bool (WinSockAddress * destinationAddress, uint32_t byteCount, uint8_t * buffer); /* true on success */
using NetworkBackendSetSessionCallback = uint32_t (uint32_t backendIndex); /* 0 or a FATAL_ERROR_NETWORK_* code */
using WinSock_WSACleanupProc = int __stdcall ();
using WinSock_WSAGetLastErrorProc = int __stdcall ();
using WinSock_WSAStartupProc = int __stdcall (uint16_t requestedVersion, WinSockData11 * startupData);
using WinSock_bindProc = int __stdcall (uint32_t socket, WinSockAddress * address, int addressLength);
using WinSock_closesocketProc = int __stdcall (uint32_t socket);
using WinSock_gethostbynameProc = WinSockHostEnt32 * __stdcall (uint8_t * hostName);
using WinSock_htonsProc = uint16_t __stdcall (uint16_t hostShort);
using WinSock_inet_addrProc = uint32_t __stdcall (uint8_t * addressText);
using WinSock_inet_ntoaProc = uint8_t * __stdcall (uint32_t ipv4AddressNetworkOrder);
using WinSock_ioctlsocketProc = int __stdcall (uint32_t socket, uint32_t command, uint32_t * argument);
using WinSock_recvfromProc = int __stdcall (uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * sourceAddress, int * sourceAddressLength);
using WinSock_sendtoProc = int __stdcall (uint32_t socket, uint8_t * buffer, int length, int flags, WinSockAddress * destinationAddress, int destinationAddressLength);
using WinSock_setsockoptProc = int __stdcall (uint32_t socket, int level, int optionName, uint8_t * optionValue, int optionLength);
using WinSock_socketProc = uint32_t __stdcall (int addressFamily, int socketType, int protocol);

#endif /* THANDOR_NETWORK_BACKEND_TYPES_H */
