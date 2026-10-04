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
#define THANDOR_PTR(p) (ThandorAnyPtr{(void *)(p)})

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

#include <stddef.h> /* offsetof (THANDOR_UI_SIBLING) */
#include <thandor/core/x86_emulation.h>

/* Address of `offset` bytes into an object, as an integer: for code that steps through a table or record by
   byte offsets like the original. */
#define THANDOR_ADDR(object, offset) ((uintptr_t)&(object) + (int)(offset))
#include <thandor/generated/imports.h>

/* A UI node at a byte offset from base: the variadic UiSelectableGroup_* helpers take the group's controls
   as extra arguments, which the original addresses as base + byte offset. */
#define THANDOR_UI_AT(base, offset) ((UiNodeBase *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* A field of a UI node inside a template image copy (root + byte offset), for bytes past the node's
   UiNodeBase; the node offsets are those of the template layouts (the *UiImage structs of the ui type headers). */
#define THANDOR_UI_FIELD(base, offset, type) (*(type *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* Node `node` of a UI template copy, reached from `self`, which is template node `selfNode` of the same
   copy (ImageType is the template struct, e.g. InGameUiImage): the nodes' fixed distance in the template. */
#define THANDOR_UI_SIBLING(self, ImageType, selfNode, node) \
    THANDOR_UI_AT(self, (int)offsetof(ImageType, node) - (int)offsetof(ImageType, selfNode))


#endif /* THANDOR_CORE_CONTRACTS_H */
