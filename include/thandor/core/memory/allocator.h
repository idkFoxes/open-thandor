/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/allocator.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MEMORY_ALLOCATOR_H
#define THANDOR_CORE_MEMORY_ALLOCATOR_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/memory/allocator. */
/* Arena heap layout (ArenaHeap_Init): one private Win32 heap block holding a 32-byte aligned chain of
   ArenaBlockHeader blocks, ended by ARENA_BLOCK_LIST_END in next/previous. */
#define ARENA_HEAP_PAYLOAD_BYTES 0x6000000 /* 96 MiB, payload of the initial single free block */
#define ARENA_HEAP_RESERVE_BYTES (ARENA_HEAP_PAYLOAD_BYTES + 0x40) /* + header and alignment slack */
#define ARENA_BLOCK_ALIGNMENT_MASK 0x1f /* blocks and payload sizes are 32-byte aligned */
#define ARENA_BLOCK_LIST_END ((ArenaBlockHeader *)0xffffffff)
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005368E0 */
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00536930 */
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00547D20 */
bool __thandor_cf_preserve_eax_ecx_edx
DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord);

/* 0x005863C0 */
void * __cdecl ArenaHeap_Init(void);

/* 0x00586470 */
void __thandor_preserve_eax ArenaHeap_Shutdown(void);

/* 0x005864A0 */
ArenaAllocResult __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Alloc(ArenaPayloadByteCount bytes);

/* 0x00586570 */
uint32_t __cdecl ArenaHeap_QueryFreeBytes(void);

/* 0x005865B0 */
ArenaFreeResult __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Free(void *memory);

/* 0x00586640 */
ArenaLargestAllocResult __thandor_eax_ecx_cf_preserve_edx
ArenaHeap_AllocLargestFreeBlock(void);

/* 0x005866B0 */
ArenaShrinkResult __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

/* 0x00586750 */
ArenaReserveResult __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes);

/* 0x005873A0 */
void __thandor_void_preserve_eax_ecx_edx Memory_ZeroDwords(MemoryByteCount bytes,void *destination);

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
