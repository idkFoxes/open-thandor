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

bool ArenaHeap_IsReady();

uint32_t ArenaHeap_Alloc(ArenaPayloadByteCount bytes,void **outPayload);

uint32_t ArenaHeap_QueryFreeBytes();

uint32_t ArenaHeap_Free(void *memory);

uint32_t ArenaHeap_AllocLargestFreeBlock(void **outAllocation,uint32_t *outBlockSize);

uint32_t ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

extern MemoryApiTable g_MemoryApi;

/* ArenaScoped: one arena block owned by a function scope and freed through the memory API when the scope ends
   (step 13 R5, owner decision D6). Only for blocks whose free is the last arena operation of the scope on every
   path: block addresses are sort keys and the simulation depends on the alloc/free order, so the guard must free
   exactly where the explicit free stood (nothing but plain stores and returns may follow it). Global or
   escaping blocks stay explicit. */
class ArenaScoped {
public:
  ArenaScoped() = default;
  ArenaScoped(const ArenaScoped &) = delete;
  ArenaScoped &operator=(const ArenaScoped &) = delete;
  ~ArenaScoped()
  {
    if (m_block != nullptr) {
      g_MemoryApi.free(m_block);
    }
  }

  /* Allocates bytes from the arena into this guard (which holds no block yet); returns the status, 0 on success.
     On failure the guard stays empty. */
  uint32_t allocate(uint32_t bytes)
  {
    void *payload = nullptr;
    uint32_t status = g_MemoryApi.alloc(bytes, &payload);
    if (status == 0) {
      m_block = payload;
    }
    return status;
  }

  void *get() const { return m_block; }
  template <class T> T *as() const { return static_cast<T *>(m_block); }

private:
  void *m_block = nullptr;
};

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
