/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MEMORY_TYPES_H
#define THANDOR_CORE_MEMORY_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct ArenaBlockHeader ArenaBlockHeader, *PArenaBlockHeader;
typedef struct ArenaState ArenaState, *PArenaState;
typedef struct MemoryApiTable MemoryApiTable, *PMemoryApiTable;

#define ARENA_HEAP_CORRUPT 0x13 /* error code: the arena block chain is corrupt (bad stateMagic) */

enum {
    ARENA_BLOCK_ALLOCATED=1515870810,
    ARENA_BLOCK_FREE=2779096485
};
typedef int ArenaBlockStateMagic;

typedef uint32_t ArenaPayloadByteCount;

struct ArenaBlockHeader {
    uint32_t payloadSize; 
    ArenaBlockStateMagic stateMagic; 
    struct ArenaBlockHeader *next; 
    struct ArenaBlockHeader *previous; 
    uint8_t alignmentPadding[24 - 2 * sizeof(void *)]; /* header stays 0x20 bytes on x64 (5f) */
};

struct ArenaState {
    void *processHeap; 
    void *rawAllocation; 
    struct ArenaBlockHeader *firstBlock; 
    uint8_t *linearCursor; 
    uint8_t *linearLimit; 
};

enum {
    SPIN_LOCK_UNLOCKED=0,
    SPIN_LOCK_LOCKED=4294967295
};
typedef int RuntimeSpinLockValue;

struct MemoryApiTable {
    /* Every status-returning entry returns 0 on success or an engine error code (FATAL_ERROR_*,
       ARENA_HEAP_CORRUPT); out-parameters are written only on success. */
    uint32_t (*alloc)(uint32_t bytes, void **outPayload); // ArenaHeap_Alloc
    uint32_t (*free)(void *memory); // ArenaHeap_Free
    uint32_t (*allocLargestFreeBlock)(void **outAllocation, uint32_t *outBlockSize); // ArenaHeap_AllocLargestFreeBlock
    uint32_t (*shrinkInPlace)(uint32_t newSize, void *memory); // ArenaHeap_ShrinkInPlace
    uint32_t (*queryFreeBytes)(void); // ArenaHeap_QueryFreeBytes
    uint32_t (*reserveLinear)(uint32_t bytes, void **outBase); // ArenaHeap_ReserveLinear; *outBase = previous linear cursor
};
typedef void SpinLockAcquireProc(RuntimeSpinLockValue * lockValue);
typedef void SpinLockReleaseCallbackProc(void);
typedef void SpinLockReleaseAndInvokeProc(SpinLockReleaseCallbackProc * callback, RuntimeSpinLockValue * lockValue);
typedef void SpinLockReleaseProc(RuntimeSpinLockValue * lockValue);
typedef Bool8 SpinLockTryAcquireFlagsProc(RuntimeSpinLockValue * lockValue);

#endif /* THANDOR_CORE_MEMORY_TYPES_H */
