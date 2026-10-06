/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/contracts.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_CONTRACTS_H
#define THANDOR_CORE_CONTRACTS_H

/*
Core contracts shared by the split submodules.
Q12: 0x1000 == 1.0. Q4 resource values use 16 units per displayed unit. Q5 research time uses 32 units per displayed unit.
FieldGridCell is 0x80 bytes. FLD flags/material (flagsAndMaterial) and runtime occupancy (occupancyMask) are separate namespaces.
LEV file offsets and the loaded LevelAsset overlay are separate representations.
Faction runtime index, frontend player index, player-runtime ID, ARM ID, MDL ID and TEC ID are separate identity domains.
*/

#include <stdint.h>

using Q12 = int;
using UQ12 = unsigned int;

/* Compile-time check. */
#define THANDOR_STATIC_ASSERT(condition, message) static_assert(condition, message)

/* THANDOR_PTR(pointer): an untyped object address for a table entry or argument whose exact pointer type is
   given by its target. The value converts to the object pointer type it
   is assigned to, never to a function pointer: function-pointer slots take THANDOR_SLOT(function) (core/slot.h),
   which checks the signature. (The former THANDOR_FN(function), which converted to any function pointer, is gone
   since step 8.) A plain void * that only identifies a function by its address, such as the handler of the
   network command tables (network/protocol/commands.cpp), may still hold THANDOR_PTR(&function). */
#include <thandor/core/ptr32.h> /* ThandorAnyPtr, Ptr32 */
#define THANDOR_PTR(p) (Thandor_AnyPtr(p))

/* A temporary of type T from a braced initializer: THANDOR_COMPOUND(T){a, b}. */
#define THANDOR_COMPOUND(T) T

/* THANDOR_ALLOWS_OVERREAD: on a function that reads past the end of its source on purpose, as the original
   does, where the extra bytes never reach a result (a copy of twice the length into a scratch buffer). The
   AddressSanitizer build (ot-scratch/build_asan.bat), which looks for accesses into neighbouring objects
   since the image data is no longer one block, skips such a function. */
#if defined(__SANITIZE_ADDRESS__) && defined(_MSC_VER)
#define THANDOR_ALLOWS_OVERREAD __declspec(no_sanitize_address)
#else
#define THANDOR_ALLOWS_OVERREAD
#endif

/* THANDOR_ALIGN(n): at least n-byte alignment for a variable (the original's data alignment); like
   __declspec(align(n)) it never lowers the type's own alignment (alignas(n) below the natural alignment, e.g. 4 for
   a 64-bit pointer, would be ill-formed). */
#ifdef _MSC_VER
#define THANDOR_ALIGN(n) __declspec(align(n))
#else
#define THANDOR_ALIGN(n) __attribute__((aligned(n)))
#endif

#include <stddef.h>
#include <thandor/core/x86_emulation.h>

/* Address of `offset` bytes into an object, as an integer: for code that steps through a table or record by
   byte offsets like the original. */
#define THANDOR_ADDR(object, offset) (reinterpret_cast<uintptr_t>(&(object)) + static_cast<int>(offset))
#include <thandor/generated/imports.h>

/* The byte address base stands for (a pointer, an integer address or a Ptr32/UPtr32 field), as the former
   C-style cast to a byte pointer did: through its integer value (uintptr_t). */
template <class B> static __forceinline uint8_t *Thandor_AddressBytes(B base)
{
  if constexpr (std::is_pointer_v<B>) {
    return reinterpret_cast<uint8_t *>(reinterpret_cast<uintptr_t>(base));
  }
  else {
    return reinterpret_cast<uint8_t *>(static_cast<uintptr_t>(base));
  }
}

#endif /* THANDOR_CORE_CONTRACTS_H */
