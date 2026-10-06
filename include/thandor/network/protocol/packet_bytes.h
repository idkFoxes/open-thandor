/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/network/protocol/packet_bytes.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_NETWORK_PROTOCOL_PACKET_BYTES_H
#define THANDOR_NETWORK_PROTOCOL_PACKET_BYTES_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

/*
Byte and dword views of network packets and the records they are copied from or into. The original moves packets,
player records and queue records dword by dword and reaches packet fields by byte offsets; the packet layouts are
the wire format of the original game and stay as they are. These helpers are the one place that reinterprets such
a record as its dwords or bytes (hence reinterpret_cast); the arithmetic is exactly that of the former C-style
casts through a byte pointer plus n.
*/

/* The record (or byte/unit array) as its dwords, for the original's dword-wise copies. */
template <class T> static inline uint32_t *Packet_Dwords(T *record)
{
  if constexpr (!std::is_void_v<T>) static_assert(sizeof(T) < 4 || sizeof(T) % 4 == 0, "Packet_Dwords: whole dwords");
  return reinterpret_cast<uint32_t *>(record);
}
template <class T> static inline const uint32_t *Packet_Dwords(const T *record)
{
  if constexpr (!std::is_void_v<T>) static_assert(sizeof(T) < 4 || sizeof(T) % 4 == 0, "Packet_Dwords: whole dwords");
  return reinterpret_cast<const uint32_t *>(record);
}

/* The T at byteOffset bytes from base. */
template <class T = uint8_t, class P, class I> static inline T *Packet_At(P *base, I byteOffset)
{
  return reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(base) + byteOffset);
}
template <class T = uint8_t, class P, class I> static inline const T *Packet_At(const P *base, I byteOffset)
{
  return reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(base) + byteOffset);
}

/* to - from in bytes, for two positions inside one block. */
template <class A, class B> static inline ptrdiff_t Packet_ByteDistance(const A *to, const B *from)
{
  return reinterpret_cast<const uint8_t *>(to) - reinterpret_cast<const uint8_t *>(from);
}

#endif
