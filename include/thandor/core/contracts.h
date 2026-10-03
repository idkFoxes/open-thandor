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

typedef int Q12;
typedef unsigned int UQ12;

/* `bool` is the one-byte boolean from generated/types.h, so <stdbool.h> cannot be used. */
#ifndef true
#define true 1
#define false 0
#endif

/*
Reinterpret the bytes of a value as another type: multi-value results are structs (a value plus a
failure flag, two values plus a flag, ...) that some code casts to integers such as uint5 or to other
layout-compatible structs, which C only allows through a union.
*/
#define THANDOR_BITCAST(From, To, value) (((union { From from_; To to_; }){ .from_ = (value) }).to_)
#ifdef _MSC_VER
#pragma warning(disable: 4116) /* unnamed type definition in parentheses (THANDOR_BITCAST) */
#endif

/* THANDOR_ALLOWS_OVERREAD: on a function that reads past the end of its source on purpose, as the original
   does, where the extra bytes never reach a result (a copy of twice the length into a scratch buffer). The
   AddressSanitizer build (ot-scratch/build_asan.bat), which looks for accesses into neighbouring objects
   since the image data is no longer one block, skips such a function. */
#if defined(__SANITIZE_ADDRESS__) && defined(_MSC_VER)
#define THANDOR_ALLOWS_OVERREAD __declspec(no_sanitize_address)
#else
#define THANDOR_ALLOWS_OVERREAD
#endif

#include <stddef.h> /* offsetof (THANDOR_UI_SIBLING) */
#include <thandor/core/ghidra.h>
/* The function pointer types and the UI template layouts. The data of the original image are ordinary C
   variables of the modules, declared in the module headers. */
#include <thandor/generated/proc_types.h>
#include <thandor/generated/ui_templates.h>

/* Address of `offset` bytes into an object, as an integer: for code that steps through a table or record by
   byte offsets like the original. */
#define THANDOR_ADDR(object, offset) ((uintptr_t)&(object) + (int)(offset))
#include <thandor/generated/imports.h>

/* A UI node at a byte offset from base: the variadic UiSelectableGroup_* helpers take the group's controls
   as extra arguments, which the original addresses as base + byte offset. */
#define THANDOR_UI_AT(base, offset) ((UiNodeBase *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* A field of a UI node inside a template image copy (root + byte offset), for bytes past the node's
   UiNodeBase; the node offsets are those of the template layouts in generated/ui_templates.h. */
#define THANDOR_UI_FIELD(base, offset, type) (*(type *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* Node `node` of a UI template copy, reached from `self`, which is template node `selfNode` of the same
   copy (ImageType is the template struct, e.g. InGameUiImage): the nodes' fixed distance in the template. */
#define THANDOR_UI_SIBLING(self, ImageType, selfNode, node) \
    THANDOR_UI_AT(self, (int)offsetof(ImageType, node) - (int)offsetof(ImageType, selfNode))

/* The top-level UI node: follow parent links until the -1 sentinel (the original's inline loop). */
static __inline UiNodeBase *Thandor_UiRoot(const void *node)
{
    UiNodeBase *current = (UiNodeBase *)node;
    while (current->parent != UI_TEMPLATE_NO_LINK) {
        current = current->parent;
    }
    return current;
}


#endif /* THANDOR_CORE_CONTRACTS_H */
