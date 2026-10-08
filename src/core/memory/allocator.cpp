/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/allocator.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/allocator.h>
#include <thandor/core/bytes.h>
#include <thandor/thandor.h>

/* Module data. */

static_assert(sizeof(ArenaBlockHeader) == ARENA_BLOCK_HEADER_BYTES, "arena block header is 0x20 bytes");

THANDOR_ALIGN(16) MemoryApiTable g_MemoryApi = {nullptr};

/* its address doubles as the error code */
/* "error: HEAP: cannot allocate heap memory! Please check your swap-file." */
static uint16_t g_ErrorTextHeapAllocationFailed[71] = {
    'e', 'r', 'r', 'o', 'r', ':', ' ', 'H', 'E', 'A', 'P', ':', ' ', 'c', 'a', 'n', 'n', 'o', 't', ' ', 'a', 'l', 'l', 'o',
    'c', 'a', 't', 'e', ' ', 'h', 'e', 'a', 'p', ' ', 'm', 'e', 'm', 'o', 'r', 'y', '!', ' ', 'P', 'l', 'e', 'a', 's', 'e',
    ' ', 'c', 'h', 'e', 'c', 'k', ' ', 'y', 'o', 'u', 'r', ' ', 's', 'w', 'a', 'p', '-', 'f', 'i', 'l', 'e', '.'};

static ArenaState g_Arena = {};

/* Creates the game's 96 MiB memory arena: allocates it in one piece from a private Win32 heap, installs the
   ArenaHeap_* functions in g_MemoryApi and makes the whole arena one free block. Returns the raw HeapAlloc
   pointer; if the heap cannot be created or allocated the game exits with the heap error message.
*/
void * ArenaHeap_Init()

{
  HANDLE heap;
  LPVOID rawArenaAllocation;
  ArenaBlockHeader *alignedFirstBlock;

  heap = HeapCreate(0,ARENA_HEAP_RESERVE_BYTES,0);
  if (heap != nullptr) {
    g_MemoryApi.alloc = ArenaHeap_Alloc;
    g_MemoryApi.free = ArenaHeap_Free;
    g_MemoryApi.allocLargestFreeBlock = ArenaHeap_AllocLargestFreeBlock;
    g_MemoryApi.shrinkInPlace = ArenaHeap_ShrinkInPlace;
    g_MemoryApi.queryFreeBytes = ArenaHeap_QueryFreeBytes;
    g_Arena.processHeap = heap;
    rawArenaAllocation = HeapAlloc(heap,0,ARENA_HEAP_RESERVE_BYTES);
    if (rawArenaAllocation != nullptr) {
      alignedFirstBlock = reinterpret_cast<ArenaBlockHeader *>
          ((reinterpret_cast<uintptr_t>(rawArenaAllocation) + ARENA_BLOCK_ALIGNMENT_MASK) &
           ~static_cast<uintptr_t>(ARENA_BLOCK_ALIGNMENT_MASK));
      g_Arena.rawAllocation = rawArenaAllocation;
      g_Arena.firstBlock = alignedFirstBlock;
      alignedFirstBlock->payloadSize = ARENA_HEAP_PAYLOAD_BYTES;
      alignedFirstBlock->stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_FREE;
      alignedFirstBlock->next = ARENA_BLOCK_LIST_END;
      alignedFirstBlock->previous = ARENA_BLOCK_LIST_END;
      return rawArenaAllocation;
    }
  }
  FatalError_ShowAndExit(THANDOR_ADDR(g_ErrorTextHeapAllocationFailed,0));
}

/* Frees the arena allocation and destroys the private Win32 heap created by ArenaHeap_Init.
   The original called HeapFree/HeapDestroy unconditionally; bounded here because the fatal-error path
   (ArenaHeap_Init failing, then Runtime_Shutdown) reaches this without a heap or without the arena
   allocation, and HeapFree/HeapDestroy on a null heap handle is undefined. The state is cleared, so a
   second call does nothing.
*/
void ArenaHeap_Shutdown()

{
  if (g_Arena.processHeap == nullptr) {
    return;
  }
  if (g_Arena.rawAllocation != nullptr) {
    HeapFree(g_Arena.processHeap,0,g_Arena.rawAllocation);
  }
  HeapDestroy(g_Arena.processHeap);
  g_Arena = ArenaState{};
}

/* open-thandor: true once ArenaHeap_Init has created the arena (heap and arena allocation), until
   ArenaHeap_Shutdown. Runtime_Shutdown asks it to skip the subsystem shutdowns on the fatal-error path of a
   failed ArenaHeap_Init, where nothing else is initialised yet.
*/
bool ArenaHeap_IsReady()

{
  return g_Arena.firstBlock != nullptr;
}

/* The arena's malloc (g_MemoryApi.alloc): first fit over the block chain for the size rounded up to 32
   bytes, splitting off the rest of the block as a new free block when it is large enough. Returns 0 with
   the payload pointer in *outPayload, or FATAL_ERROR_ARENA_EXHAUSTED (largest free size left in
   g_PackageLastErrorPath) or ARENA_HEAP_CORRUPT for a corrupt block chain; *outPayload is then unchanged.
*/
uint32_t ArenaHeap_Alloc(ArenaPayloadByteCount bytes,void **outPayload)

{
  ArenaBlockHeader *followingBlock;
  uint32_t alignedBytes;
  ArenaBlockHeader *splitBlock;
  uint32_t largestFreePayloadBytes;
  uint32_t originalPayloadSize;
  ArenaBlockHeader *blockCursor;

  largestFreePayloadBytes = 1;
  alignedBytes = (bytes + ARENA_BLOCK_ALIGNMENT_MASK) & ~ARENA_BLOCK_ALIGNMENT_MASK;
  /* The original rounded in 32 bits, so a request above 0xFFFFFFE0 wrapped to 0 and got a 0-byte block;
     bounded here because sizes can come from the network (mailbox transfers). Nothing larger than the
     arena can fit, so such a request walks the chain as an unsatisfiable size and fails exactly like
     any other too large request (FATAL_ERROR_ARENA_EXHAUSTED with the largest free size). */
  if (bytes > ARENA_HEAP_PAYLOAD_BYTES) {
    alignedBytes = UINT32_MAX;
  }
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
        return ARENA_HEAP_CORRUPT;
      }
      if (largestFreePayloadBytes < blockCursor->payloadSize) {
        largestFreePayloadBytes = blockCursor->payloadSize;
      }
      if (alignedBytes <= blockCursor->payloadSize) {
        blockCursor->stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED;
        if (blockCursor->payloadSize <= alignedBytes + ARENA_BLOCK_SPLIT_SLACK_BYTES) {
          *outPayload = blockCursor + 1;
          return 0;
        }
        originalPayloadSize = blockCursor->payloadSize;
        blockCursor->payloadSize = alignedBytes;
        followingBlock = blockCursor->next;
        /* the new free block starts right behind the shortened payload */
        splitBlock = Thandor_At<ArenaBlockHeader>(blockCursor + 1, blockCursor->payloadSize);
        splitBlock->payloadSize = originalPayloadSize - (blockCursor->payloadSize + ARENA_BLOCK_HEADER_BYTES);
        splitBlock->stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_FREE;
        splitBlock->previous = blockCursor;
        blockCursor->next = splitBlock;
        splitBlock->next = followingBlock;
        if (followingBlock != ARENA_BLOCK_LIST_END) {
          followingBlock->previous = splitBlock;
        }
        *outPayload = blockCursor + 1;
        return 0;
      }
    }
    blockCursor = blockCursor->next;
  } while (blockCursor != ARENA_BLOCK_LIST_END);
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,largestFreePayloadBytes,g_PackageLastErrorPath);
  return FATAL_ERROR_ARENA_EXHAUSTED;
}

/* Returns the sum of all free payload bytes in the arena (g_MemoryApi.queryFreeBytes), or
   ARENA_HEAP_CORRUPT when the block chain is corrupt.
*/
uint32_t ArenaHeap_QueryFreeBytes()

{
  uint32_t freePayloadBytes;
  ArenaBlockHeader *blockCursor;

  freePayloadBytes = 0;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
        return ARENA_HEAP_CORRUPT;
      }
      freePayloadBytes = freePayloadBytes + blockCursor->payloadSize;
    }
    blockCursor = blockCursor->next;
  } while (blockCursor != ARENA_BLOCK_LIST_END);
  return freePayloadBytes;
}

/* The arena's free (g_MemoryApi.free): marks the block free and merges it with a free following block,
   then merges a free preceding block with it. NULL is accepted. Returns 0, or ARENA_HEAP_CORRUPT for a
   payload whose header is not marked allocated.
   The ArenaBlockHeader lies directly below the payload.
*/
uint32_t ArenaHeap_Free(void *memory)

{
  ArenaBlockHeader *freedBlock;
  ArenaBlockHeader *followingBlock;
  ArenaBlockHeader *previousBlock;
  ArenaBlockHeader *mergedNextBlock;

  if (memory != nullptr) {
    /* the header is addressed both as freedBlock (sizes) and as memory[-1] (links), as in the original */
    freedBlock = reinterpret_cast<ArenaBlockHeader *>(reinterpret_cast<uintptr_t>(memory) - ARENA_BLOCK_HEADER_BYTES);
    if (static_cast<ArenaBlockHeader *>(memory)[-1].stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED) {
      return ARENA_HEAP_CORRUPT;
    }
    static_cast<ArenaBlockHeader *>(memory)[-1].stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_FREE;
    /* merge the following block */
    followingBlock = static_cast<ArenaBlockHeader *>(memory)[-1].next;
    if (followingBlock != ARENA_BLOCK_LIST_END && followingBlock->stateMagic == ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
      freedBlock->payloadSize = freedBlock->payloadSize + followingBlock->payloadSize + ARENA_BLOCK_HEADER_BYTES;
      mergedNextBlock = followingBlock->next;
      static_cast<ArenaBlockHeader *>(memory)[-1].next = mergedNextBlock;
      if (mergedNextBlock != ARENA_BLOCK_LIST_END) {
        mergedNextBlock->previous = freedBlock;
      }
    }
    /* merge into the preceding block */
    previousBlock = static_cast<ArenaBlockHeader *>(memory)[-1].previous;
    if (previousBlock != ARENA_BLOCK_LIST_END && previousBlock->stateMagic == ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
      previousBlock->payloadSize = previousBlock->payloadSize + freedBlock->payloadSize + ARENA_BLOCK_HEADER_BYTES;
      mergedNextBlock = static_cast<ArenaBlockHeader *>(memory)[-1].next;
      previousBlock->next = mergedNextBlock;
      if (mergedNextBlock != ARENA_BLOCK_LIST_END) {
        mergedNextBlock->previous = previousBlock;
      }
    }
  }
  /* The original returns an unrelated leftover value on success; the few callers that read it get 0
     here. */
  return 0;
}

/* Takes the largest free block whole (g_MemoryApi.allocLargestFreeBlock): marks it allocated and returns 0
   with its payload pointer in *outAllocation and its size in *outBlockSize, for callers that shrink it
   afterwards with ArenaHeap_ShrinkInPlace. Returns FATAL_ERROR_ARENA_EXHAUSTED when nothing is free, or
   ARENA_HEAP_CORRUPT for a corrupt block chain; the out-parameters are then unchanged.
*/
uint32_t ArenaHeap_AllocLargestFreeBlock(void **outAllocation,uint32_t *outBlockSize)

{
  uint32_t largestFreePayloadBytes;
  ArenaBlockHeader *blockCursor;
  ArenaBlockHeader *largestFreeBlock;

  largestFreePayloadBytes = 0;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
        return ARENA_HEAP_CORRUPT;
      }
      if (largestFreePayloadBytes < blockCursor->payloadSize) {
        largestFreePayloadBytes = blockCursor->payloadSize;
        largestFreeBlock = blockCursor;
      }
    }
    blockCursor = blockCursor->next;
  } while (blockCursor != ARENA_BLOCK_LIST_END);
  if (largestFreePayloadBytes == 0) {
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0,g_PackageLastErrorPath);
    return FATAL_ERROR_ARENA_EXHAUSTED;
  }
  largestFreeBlock->stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED;
  *outBlockSize = largestFreePayloadBytes;
  *outAllocation = largestFreeBlock + 1;
  return 0;
}

/* Shrinks an allocated block to newSize (rounded up to 32 bytes) and returns the tail as a free block,
   merged with a free following block (g_MemoryApi.shrinkInPlace). A tail too small to split is kept.
   Returns 0 on success, or ARENA_HEAP_CORRUPT when the block is not allocated or newSize is larger
   than the block.
*/
uint32_t ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory)

{
  ArenaBlockHeader *block;
  uint32_t alignedBytes;
  uint32_t originalPayloadSize;
  uint32_t splitPayloadSize;
  ArenaBlockHeader *splitBlock;
  ArenaBlockHeader *followingBlock;
  ArenaBlockHeader *followingNextBlock;

  /* the ArenaBlockHeader lies directly below the payload */
  block = static_cast<ArenaBlockHeader *>(memory) - 1;
  alignedBytes = (newSize + ARENA_BLOCK_ALIGNMENT_MASK) & ~ARENA_BLOCK_ALIGNMENT_MASK;
  /* The original rounded in 32 bits, so a newSize above 0xFFFFFFE0 wrapped to 0 and shrank the block to 0
     bytes; bounded here like ArenaHeap_Alloc: such a size is larger than any block and rejected below. */
  if (newSize > ARENA_HEAP_PAYLOAD_BYTES) {
    alignedBytes = UINT32_MAX;
  }
  if (block->stateMagic != ArenaBlockStateMagic::ARENA_BLOCK_ALLOCATED || alignedBytes > block->payloadSize) {
    return ARENA_HEAP_CORRUPT;
  }
  /* The original returns a leftover split-block value on success; no caller reads it, so 0 here. */
  if (block->payloadSize <= alignedBytes + ARENA_BLOCK_SPLIT_SLACK_BYTES) {
    return 0;
  }
  originalPayloadSize = block->payloadSize;
  block->payloadSize = alignedBytes;
  splitPayloadSize = originalPayloadSize - (alignedBytes + ARENA_BLOCK_HEADER_BYTES);
  followingBlock = block->next;
  /* the new free block starts right behind the shortened payload */
  splitBlock = reinterpret_cast<ArenaBlockHeader *>(Thandor_Bytes(block) + ARENA_BLOCK_HEADER_BYTES + alignedBytes);
  block->next = splitBlock;
  splitBlock->stateMagic = ArenaBlockStateMagic::ARENA_BLOCK_FREE;
  splitBlock->payloadSize = splitPayloadSize;
  splitBlock->previous = block;
  splitBlock->next = followingBlock;
  if (followingBlock != ARENA_BLOCK_LIST_END) {
    followingBlock->previous = splitBlock;
    if (followingBlock->stateMagic == ArenaBlockStateMagic::ARENA_BLOCK_FREE) {
      /* merge the free following block into the split block */
      followingNextBlock = followingBlock->next;
      splitBlock->next = followingNextBlock;
      splitBlock->payloadSize = splitPayloadSize + followingBlock->payloadSize + ARENA_BLOCK_HEADER_BYTES;
      if (followingNextBlock != ARENA_BLOCK_LIST_END) {
        followingNextBlock->previous = splitBlock;
      }
    }
  }
  return 0;
}
