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

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;
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

#include <thandor/core/ghidra.h>
#include <thandor/generated/globals.h>
#include <thandor/generated/imports.h>

/* Variadic UiSelectableGroup_* helpers take the group's controls as extra stack arguments, which
   the decompiler dropped at every call site. The original addresses them as base + byte offset. */
#define THANDOR_UI_AT(base, offset) ((UiNodeBase *)((byte *)(uintptr_t)(base) + (int)(offset)))

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
