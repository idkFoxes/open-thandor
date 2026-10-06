/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/flags.h
 */

#ifndef THANDOR_CORE_FLAGS_H
#define THANDOR_CORE_FLAGS_H

/* Not part of the original: bit operations for flag sets declared as `enum class E : uint32_t` (or another
   unsigned underlying type). THANDOR_FLAG_ENUM(E), written after the enum in the same namespace, gives E the
   operators | & ^ ~ |= &= ^= (results stay E, the bit values are unchanged) and marks it as a flag enum for
   Any, ToBits and FromBits. A flag enum does not convert to int or bool: `flags != 0` becomes Any(flags),
   `flags & MASK` in a condition becomes Any(flags & E::Mask), raw fields and file bytes go through
   ToBits/FromBits. The compile-time tests are in src/core/layout_checks.cpp. */

#include <type_traits>

/* The underlying integer of enum E (helper of the macro; code uses ToBits). */
template <class E> constexpr std::underlying_type_t<E> ThandorFlagBits(E e)

{
  return static_cast<std::underlying_type_t<E>>(e);
}

/* Marks E as a flag enum (ThandorIsFlagEnum is found by argument-dependent lookup, so the macro works in any
   namespace). ~ keeps every bit of the underlying type: ~E::A also sets the bits that have no enumerator. */
#define THANDOR_FLAG_ENUM(E)                                                                                    \
  constexpr bool ThandorIsFlagEnum(E) { return true; }                                                         \
  constexpr E operator|(E a, E b) { return static_cast<E>(::ThandorFlagBits(a) | ::ThandorFlagBits(b)); }     \
  constexpr E operator&(E a, E b) { return static_cast<E>(::ThandorFlagBits(a) & ::ThandorFlagBits(b)); }     \
  constexpr E operator^(E a, E b) { return static_cast<E>(::ThandorFlagBits(a) ^ ::ThandorFlagBits(b)); }     \
  constexpr E operator~(E a) { return static_cast<E>(~::ThandorFlagBits(a)); }                                 \
  constexpr E &operator|=(E &a, E b) { return a = a | b; }                                                     \
  constexpr E &operator&=(E &a, E b) { return a = a & b; }                                                     \
  constexpr E &operator^=(E &a, E b) { return a = a ^ b; }                                                     \
  static_assert(std::is_unsigned_v<std::underlying_type_t<E>>, #E " is a flag enum with an unsigned underlying type")

template <class E>
concept ThandorFlagEnum = std::is_enum_v<E> && requires(E e) { ThandorIsFlagEnum(e); };

/* True when any bit of e is set (the `flags != 0` of an integer flag field). */
template <ThandorFlagEnum E> constexpr bool Any(E e)

{
  return ThandorFlagBits(e) != 0;
}

/* The raw bits of e, e.g. for hashing, packets and file bytes. */
template <ThandorFlagEnum E> constexpr std::underlying_type_t<E> ToBits(E e)

{
  return ThandorFlagBits(e);
}

/* Raw bits (a file byte, a packet field, a template initialiser) as the flag set E; no bits are dropped. */
template <ThandorFlagEnum E> constexpr E FromBits(std::underlying_type_t<E> bits)

{
  return static_cast<E>(bits);
}

#endif /* THANDOR_CORE_FLAGS_H */
