/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/record_bytes.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_RECORD_BYTES_H
#define THANDOR_ASSETS_RECORD_BYTES_H

#include <cstddef>
#include <cstdint>

/*
Byte positions inside a loaded asset. An asset file (a PCK entry: ARM, MDL, SPR, ROM, text pages, scenario
lists) is loaded as one byte block and walked as chains of variable-size records, each found by a byte size or a
byte offset read from the file. These helpers are the one place that turns such a byte position back into a
record pointer: a genuine reinterpretation of file bytes, hence reinterpret_cast. The arithmetic is exactly that
of the former C-style casts through a byte pointer plus n (the offset keeps its own integer type, so it is zero-
or sign-extended as before).
*/

/* The T at byteOffset bytes from base. */
template <class T = uint8_t, class P, class I> static inline T *Asset_RecordAt(P *base, I byteOffset)
{
  return reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(base) + byteOffset);
}
template <class T = uint8_t, class P, class I> static inline const T *Asset_RecordAt(const P *base, I byteOffset)
{
  return reinterpret_cast<const T *>(reinterpret_cast<const uint8_t *>(base) + byteOffset);
}

/* The T that directly follows header (the bytes at header + 1). */
template <class T, class H> static inline T *Asset_RecordAfter(H *header)
{
  return reinterpret_cast<T *>(header + 1);
}
template <class T, class H> static inline const T *Asset_RecordAfter(const H *header)
{
  return reinterpret_cast<const T *>(header + 1);
}

/* to - from in bytes, for two positions inside one block. */
template <class A, class B> static inline ptrdiff_t Asset_ByteDistance(const A *to, const B *from)
{
  return reinterpret_cast<const uint8_t *>(to) - reinterpret_cast<const uint8_t *>(from);
}

#endif
