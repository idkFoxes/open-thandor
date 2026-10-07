/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/allocator.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MEMORY_ALLOCATOR_H
#define THANDOR_CORE_MEMORY_ALLOCATOR_H

#include <thandor/core/memory/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Arena heap layout (ArenaHeap_Init): one private Win32 heap block holding a 32-byte aligned chain of
   ArenaBlockHeader blocks, ended by ARENA_BLOCK_LIST_END in next/previous. */
inline constexpr auto ARENA_HEAP_PAYLOAD_BYTES = 0x6000000; /* 96 MiB, payload of the initial single free block */
#define ARENA_HEAP_RESERVE_BYTES (ARENA_HEAP_PAYLOAD_BYTES + 0x40) /* + header and alignment slack */
inline constexpr auto ARENA_BLOCK_ALIGNMENT_MASK = 0x1f; /* blocks and payload sizes are 32-byte aligned */
#define ARENA_BLOCK_LIST_END (Thandor_U32ToPointer<ArenaBlockHeader>(-1)) /* all bits set, as 0xffffffff in the original */
inline constexpr auto ARENA_BLOCK_HEADER_BYTES = 0x20; /* sizeof(ArenaBlockHeader); the payload follows the header */
/* A free block is split only when it exceeds the aligned request by more than this (room for a header
   and a 32-byte payload); smaller remainders stay with the allocation. */
inline constexpr auto ARENA_BLOCK_SPLIT_SLACK_BYTES = 0x40;

void * ArenaHeap_Init();

void ArenaHeap_Shutdown();

uint32_t ArenaHeap_Alloc(ArenaPayloadByteCount bytes,void **outPayload);

uint32_t ArenaHeap_QueryFreeBytes();

uint32_t ArenaHeap_Free(void *memory);

uint32_t ArenaHeap_AllocLargestFreeBlock(void **outAllocation,uint32_t *outBlockSize);

uint32_t ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

extern MemoryApiTable g_MemoryApi;

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
