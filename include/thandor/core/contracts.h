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
FieldGridCell is 0x80 bytes. FLD flags/material at +0x50 and runtime occupancy at +0x70 are separate namespaces.
LEV file offsets and the loaded LevelAsset overlay are separate representations.
Faction runtime index, frontend player index, player-runtime ID, ARM ID, MDL ID and TEC ID are separate identity domains.
*/

#include <stdint.h>

typedef int Q12;
typedef unsigned int UQ12;

/* `bool` is Ghidra's one-byte boolean from generated/types.h, so <stdbool.h> cannot be used. */
#ifndef true
#define true 1
#define false 0
#endif

/*
Reinterpret the bytes of a register-image value as another type. Ghidra models multi-register
results as structs ({eax, carry}, {eax, ecx, carry}, ...) and casts them to integers such as uint5
or to other layout-compatible structs, which C only allows through a union.
*/
#define THANDOR_BITCAST(From, To, value) (((union { From from_; To to_; }){ .from_ = (value) }).to_)
#ifdef _MSC_VER
#pragma warning(disable: 4116) /* unnamed type definition in parentheses (THANDOR_BITCAST) */
#endif

#include <stddef.h> /* offsetof (THANDOR_UI_SIBLING) */
#include <thandor/core/ghidra.h>
/* Where the original image data lives. By default the generated C data (src/generated/image_data.c,
   tools/data/gen_image_data.py); with THANDOR_MAPPED_IMAGE the copy of the original executable mapped
   at its original address (platform/bootstrap/image.c), as before the data was generated. */
#ifdef THANDOR_MAPPED_IMAGE
#define THANDOR_IMAGE(address) ((uintptr_t)(address))
#else
#define THANDOR_IMAGE(address) THANDOR_IMAGE_##address
#endif
#include <thandor/generated/globals.h>
#include <thandor/generated/ui_templates.h>
#ifndef THANDOR_MAPPED_IMAGE
#include <thandor/generated/image_data.h>
#endif
#include <thandor/data/recovered.h>
#include <thandor/generated/imports.h>

/* EAX + CF results: CF clear with a value, or CF set with an engine error code in EAX. */
static __inline StatusValueEaxCf5 StatusValue_Ok(uint32_t value)
{
    StatusValueEaxCf5 result;
    result.valueOrError = value;
    result.carry = false;
    return result;
}

static __inline StatusValueEaxCf5 StatusValue_Fail(uint32_t errorCode)
{
    StatusValueEaxCf5 result;
    result.valueOrError = errorCode;
    result.carry = true;
    return result;
}

/* Variadic UiSelectableGroup_* helpers take the group's controls as extra stack arguments, which
   the decompiler dropped at every call site. The original addresses them as base + byte offset. */
#define THANDOR_UI_AT(base, offset) ((UiNodeBase *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* A field of a UI node inside a template image copy (root + byte offset), for bytes past the node's
   UiNodeBase; the node offsets are those of the template in src/generated/image_data.c. */
#define THANDOR_UI_FIELD(base, offset, type) (*(type *)((uint8_t *)(uintptr_t)(base) + (int)(offset)))

/* Node `node` of a UI template copy, reached from `self`, which is template node `selfNode` of the same
   copy (ImageType is the template struct, e.g. InGameUiImage): the nodes' fixed distance in the template. */
#define THANDOR_UI_SIBLING(self, ImageType, selfNode, node) \
    THANDOR_UI_AT(self, (int)offsetof(ImageType, node) - (int)offsetof(ImageType, selfNode))

/* The top-level UI node: follow parent links until the -1 sentinel (the original's inline loop). */
static __inline UiNodeBase *Thandor_UiRoot(const void *node)
{
    UiNodeBase *current = (UiNodeBase *)node;
    while (current->parent != (UiNodeBase *)0xffffffff) {
        current = current->parent;
    }
    return current;
}


#endif /* THANDOR_CORE_CONTRACTS_H */
