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
#define ARENA_BLOCK_LIST_END ((ArenaBlockHeader *)(intptr_t)-1) /* all bits set, as 0xffffffff in the original */
#define ARENA_BLOCK_HEADER_BYTES 0x20 /* sizeof(ArenaBlockHeader); the payload follows the header */
/* A free block is split only when it exceeds the aligned request by more than this (room for a header
   and a 32-byte payload); smaller remainders stay with the allocation. */
#define ARENA_BLOCK_SPLIT_SLACK_BYTES 0x40
/* DwordBlock64Array_ContainsExactRecord: dwords per compared record */
#define DWORD_BLOCK64_RECORD_DWORDS 0x40
/* Functions are grouped by semantic ownership. */

void PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

Bool8 DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord);

void * __cdecl ArenaHeap_Init(void);

void ArenaHeap_Shutdown(void);

uint32_t ArenaHeap_Alloc(ArenaPayloadByteCount bytes,void **outPayload);

uint32_t __cdecl ArenaHeap_QueryFreeBytes(void);

uint32_t ArenaHeap_Free(void *memory);

uint32_t ArenaHeap_AllocLargestFreeBlock(void **outAllocation,uint32_t *outBlockSize);

uint32_t ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

uint32_t ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes,void **outBase);

extern MemoryApiTable g_MemoryApi;

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
