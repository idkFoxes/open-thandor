/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/bytes.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_BYTES_H
#define THANDOR_CORE_BYTES_H

#include <stddef.h>
#include <stdint.h>

/*
Byte positions inside a block of memory (an arena block, a loaded file, a record walked by byte offsets). The
original reaches records by a byte pointer plus n and reads them through the cast pointer; these helpers are the
one place in core and movie that turns such a byte position back into a typed pointer: a genuine reinterpretation
of the bytes, hence reinterpret_cast. The arithmetic is exactly that of the former C-style casts through a byte
pointer plus n (the offset keeps its own integer type, so it is zero- or sign-extended as before).
*/

/* The bytes of p (a view of any object as uint8_t). */
template <class P> static inline uint8_t *Thandor_Bytes(P *p)
{
  return reinterpret_cast<uint8_t *>(p);
}
template <class P> static inline const uint8_t *Thandor_Bytes(const P *p)
{
  return reinterpret_cast<const uint8_t *>(p);
}

/* The T at byteOffset bytes from base. */
template <class T = uint8_t, class P, class I> static inline T *Thandor_At(P *base, I byteOffset)
{
  return reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(base) + byteOffset);
}
template <class T = uint8_t, class P, class I> static inline const T *Thandor_At(const P *base, I byteOffset)
{
  return reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(base) + byteOffset);
}

/* to - from in bytes, for two positions inside one block. */
template <class A, class B> static inline ptrdiff_t Thandor_ByteDistance(const A *to, const B *from)
{
  return reinterpret_cast<const uint8_t *>(to) - reinterpret_cast<const uint8_t *>(from);
}

#endif
