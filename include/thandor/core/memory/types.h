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

struct ArenaBlockHeader;
struct ArenaState;
struct MemoryApiTable;

inline constexpr auto ARENA_HEAP_CORRUPT = 0x13; /* error code: the arena block chain is corrupt (bad stateMagic) */

/* ArenaBlockHeader.stateMagic: an allocated or free block; any other value means a corrupt block chain. */
enum class ArenaBlockStateMagic : uint32_t {
    ARENA_BLOCK_ALLOCATED=0x5A5A5A5A,
    ARENA_BLOCK_FREE=0xA5A5A5A5
};

using ArenaPayloadByteCount = uint32_t;

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
};

enum {
    SPIN_LOCK_UNLOCKED=0,
    SPIN_LOCK_LOCKED=4294967295
};
using RuntimeSpinLockValue = int;

struct MemoryApiTable {
    /* Every status-returning entry returns 0 on success or an engine error code (FATAL_ERROR_*,
       ARENA_HEAP_CORRUPT); out-parameters are written only on success. */
    uint32_t (*alloc)(uint32_t bytes, void **outPayload); // ArenaHeap_Alloc
    uint32_t (*free)(void *memory); // ArenaHeap_Free
    uint32_t (*allocLargestFreeBlock)(void **outAllocation, uint32_t *outBlockSize); // ArenaHeap_AllocLargestFreeBlock
    uint32_t (*shrinkInPlace)(uint32_t newSize, void *memory); // ArenaHeap_ShrinkInPlace
    uint32_t (*queryFreeBytes)(); // ArenaHeap_QueryFreeBytes
};
using SpinLockAcquireProc = void (RuntimeSpinLockValue * lockValue);
using SpinLockReleaseCallbackProc = void ();
using SpinLockReleaseAndInvokeProc = void (SpinLockReleaseCallbackProc * callback, RuntimeSpinLockValue * lockValue);
using SpinLockReleaseProc = void (RuntimeSpinLockValue * lockValue);
using SpinLockTryAcquireFlagsProc = bool (RuntimeSpinLockValue * lockValue);

#endif /* THANDOR_CORE_MEMORY_TYPES_H */
