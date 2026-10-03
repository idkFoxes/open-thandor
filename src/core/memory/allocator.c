/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/allocator.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/allocator.h>
#include <thandor/thandor.h>

/* Module data. */

__declspec(align(16)) MemoryApiTable g_MemoryApi = {0};

/* its address doubles as the error code */
static uint16_t g_ErrorTextHeapAllocationFailed[71] = L"error: HEAP: cannot allocate heap memory! Please check your swap-file.";

/* linear (bump) region of g_Arena (g_Arena.linearCursor starts at [0], g_Arena.linearLimit is [0x4000]);
   the 0xC00 bytes past the limit run to the end of the original image and are never handed out. One array
   so cursor and limit stay in the same object. The alignment is kept on purpose: ArenaHeap_ReserveLinear
   hands out base + offset without rounding, so the blocks inherit the 16-byte alignment of the original region. */
static __declspec(align(16)) uint8_t g_ArenaLinearStorage[0x4C00] = {0};

static ArenaState g_Arena = {.linearCursor = &g_ArenaLinearStorage[0], .linearLimit = &g_ArenaLinearStorage[0x4000]};

/* Implementation ownership: core/memory/allocator. */

/* Heap-building step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): the newly
   appended last entity/priority pair moves up the max-heap, swapping with its parent while its priority is
   larger.
*/
void PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  EntityPathingPriorityPair *parentHeapPair;
  uint32_t parentSearchIndex;
  EntityPathingPriorityPair *currentHeapPair;
  int childPriority;
  GameEntityRuntime *childEntity;

  /* parentSearchIndex is the current index - 1: its half is the parent index */
  parentSearchIndex = heapSize - 2;
  currentHeapPair = heapBase + heapSize - 1;
  if (1 < heapSize) {
    do {
      parentHeapPair = heapBase + (parentSearchIndex >> 1);
      childPriority = currentHeapPair->priority;
      if (childPriority <= parentHeapPair->priority) {
        return;
      }
      currentHeapPair->priority = parentHeapPair->priority;
      parentHeapPair->priority = childPriority;
      childEntity = currentHeapPair->entity;
      currentHeapPair->entity = parentHeapPair->entity;
      parentHeapPair->entity = childEntity;
      parentSearchIndex = (parentSearchIndex >> 1) - 1;
      currentHeapPair = parentHeapPair;
    } while (-1 < (int)parentSearchIndex);
  }
}


/* Extraction step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): after the root was
   swapped with the last entry, the new root entity/priority pair moves down the max-heap, swapping with its
   larger-priority child while that child is larger.
*/
void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  int selectedChildPriority;
  uint32_t leftChildIndex;
  uint32_t selectedChildIndex;
  GameEntityRuntime *selectedChildEntity;
  EntityPathingPriorityPair *currentHeapPair;
  int32_t displacedParentPriority;
  GameEntityRuntime *displacedParentEntity;

  currentHeapPair = heapBase;
  /* children of index i are 2i + 1 and 2i + 2; the index comparisons are unsigned (heapSize - 1U) */
  leftChildIndex = 1;
  while (leftChildIndex <= heapSize - 1U) {
    selectedChildIndex = leftChildIndex;
    selectedChildPriority = heapBase[selectedChildIndex].priority;
    selectedChildEntity = heapBase[selectedChildIndex].entity;
    if ((leftChildIndex < heapSize - 1U) &&
       (selectedChildPriority < heapBase[leftChildIndex + 1].priority)) {
      selectedChildIndex = leftChildIndex + 1;
      selectedChildPriority = heapBase[selectedChildIndex].priority;
      selectedChildEntity = heapBase[selectedChildIndex].entity;
    }
    if (selectedChildPriority <= currentHeapPair->priority) {
      return;
    }
    /* swap parent and child */
    displacedParentPriority = currentHeapPair->priority;
    currentHeapPair->priority = selectedChildPriority;
    displacedParentEntity = currentHeapPair->entity;
    currentHeapPair->entity = selectedChildEntity;
    heapBase[selectedChildIndex].priority = displacedParentPriority;
    heapBase[selectedChildIndex].entity = displacedParentEntity;
    currentHeapPair = heapBase + selectedChildIndex;
    leftChildIndex = selectedChildIndex * 2 + 1;
  }
}


/* Tells whether recordArray (recordCount records of 0x40 dwords each) contains a record equal to
   candidateRecord. Inverted like all failure flags: false = found, true = not found.
   recordCount must be at least 1.
*/
bool DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord)

{
  int dwordsRemainingInRecord;
  uint32_t *candidateRecordCursor;
  bool dwordsEqual;

  do {
    /* compare the 0x40 dwords until the first difference (the count is nonzero, so dwordsEqual holds the
       last comparison) */
    dwordsRemainingInRecord = DWORD_BLOCK64_RECORD_DWORDS;
    candidateRecordCursor = candidateRecord;
    do {
      dwordsRemainingInRecord--;
      dwordsEqual = *recordArray == *candidateRecordCursor;
      recordArray++;
      candidateRecordCursor++;
    } while (dwordsEqual && (dwordsRemainingInRecord != 0));
    if (dwordsEqual) {
      return false;
    }
    recordArray = recordArray + dwordsRemainingInRecord; /* skip the rest of the mismatching record */
    recordCount--;
  } while (recordCount != 0);
  return true;
}


/* Creates the game's 96 MiB memory arena: allocates it in one piece from a private Win32 heap, installs the
   ArenaHeap_* functions in g_MemoryApi and makes the whole arena one free block. Returns the raw HeapAlloc
   pointer; if the heap cannot be created or allocated the game exits with the heap error message.
*/
void * __cdecl ArenaHeap_Init(void)

{
  HANDLE heap;
  LPVOID rawArenaAllocation;
  ArenaBlockHeader *alignedFirstBlock;

  heap = HeapCreate(0,ARENA_HEAP_RESERVE_BYTES,0);
  if (heap != NULL) {
    g_MemoryApi.alloc = ArenaHeap_Alloc;
    g_MemoryApi.free = ArenaHeap_Free;
    g_MemoryApi.allocLargestFreeBlock = ArenaHeap_AllocLargestFreeBlock;
    g_MemoryApi.shrinkInPlace = ArenaHeap_ShrinkInPlace;
    g_MemoryApi.queryFreeBytes = ArenaHeap_QueryFreeBytes;
    g_MemoryApi.reserveLinear = ArenaHeap_ReserveLinear;
    g_Arena.processHeap = heap;
    rawArenaAllocation = HeapAlloc(heap,0,ARENA_HEAP_RESERVE_BYTES);
    if (rawArenaAllocation != NULL) {
      alignedFirstBlock = (ArenaBlockHeader *)
          (((int)rawArenaAllocation + ARENA_BLOCK_ALIGNMENT_MASK) & ~ARENA_BLOCK_ALIGNMENT_MASK);
      g_Arena.rawAllocation = rawArenaAllocation;
      g_Arena.firstBlock = alignedFirstBlock;
      alignedFirstBlock->payloadSize = ARENA_HEAP_PAYLOAD_BYTES;
      alignedFirstBlock->stateMagic = ARENA_BLOCK_FREE;
      alignedFirstBlock->next = ARENA_BLOCK_LIST_END;
      alignedFirstBlock->previous = ARENA_BLOCK_LIST_END;
      return rawArenaAllocation;
    }
  }
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextHeapAllocationFailed,0),true);
}


/* Frees the arena allocation and destroys the private Win32 heap created by ArenaHeap_Init.
*/
void ArenaHeap_Shutdown(void)

{
  HeapFree(g_Arena.processHeap,0,g_Arena.rawAllocation);
  HeapDestroy(g_Arena.processHeap);
  return;
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
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
        return ARENA_HEAP_CORRUPT;
      }
      if (largestFreePayloadBytes < blockCursor->payloadSize) {
        largestFreePayloadBytes = blockCursor->payloadSize;
      }
      if (alignedBytes <= blockCursor->payloadSize) {
        blockCursor->stateMagic = ARENA_BLOCK_ALLOCATED;
        if (blockCursor->payloadSize <= alignedBytes + ARENA_BLOCK_SPLIT_SLACK_BYTES) {
          *outPayload = blockCursor + 1;
          return 0;
        }
        originalPayloadSize = blockCursor->payloadSize;
        blockCursor->payloadSize = alignedBytes;
        followingBlock = blockCursor->next;
        /* the new free block starts right behind the shortened payload */
        splitBlock = (ArenaBlockHeader *)((uint8_t *)(blockCursor + 1) + blockCursor->payloadSize);
        splitBlock->payloadSize = originalPayloadSize - (blockCursor->payloadSize + ARENA_BLOCK_HEADER_BYTES);
        splitBlock->stateMagic = ARENA_BLOCK_FREE;
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
uint32_t __cdecl ArenaHeap_QueryFreeBytes(void)

{
  uint32_t freePayloadBytes;
  ArenaBlockHeader *blockCursor;

  freePayloadBytes = 0;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
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

  if (memory != NULL) {
    /* the header is addressed both as freedBlock (sizes) and as memory[-1] (links), as in the original */
    freedBlock = (ArenaBlockHeader *)((int)memory - ARENA_BLOCK_HEADER_BYTES);
    if (((ArenaBlockHeader *)memory)[-1].stateMagic != ARENA_BLOCK_ALLOCATED) {
      return ARENA_HEAP_CORRUPT;
    }
    ((ArenaBlockHeader *)memory)[-1].stateMagic = ARENA_BLOCK_FREE;
    /* merge the following block */
    followingBlock = ((ArenaBlockHeader *)memory)[-1].next;
    if (followingBlock != ARENA_BLOCK_LIST_END && followingBlock->stateMagic == ARENA_BLOCK_FREE) {
      freedBlock->payloadSize = freedBlock->payloadSize + followingBlock->payloadSize + ARENA_BLOCK_HEADER_BYTES;
      mergedNextBlock = followingBlock->next;
      ((ArenaBlockHeader *)memory)[-1].next = mergedNextBlock;
      if (mergedNextBlock != ARENA_BLOCK_LIST_END) {
        mergedNextBlock->previous = freedBlock;
      }
    }
    /* merge into the preceding block */
    previousBlock = ((ArenaBlockHeader *)memory)[-1].previous;
    if (previousBlock != ARENA_BLOCK_LIST_END && previousBlock->stateMagic == ARENA_BLOCK_FREE) {
      previousBlock->payloadSize = previousBlock->payloadSize + freedBlock->payloadSize + ARENA_BLOCK_HEADER_BYTES;
      mergedNextBlock = ((ArenaBlockHeader *)memory)[-1].next;
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
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
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
  largestFreeBlock->stateMagic = ARENA_BLOCK_ALLOCATED;
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
  block = (ArenaBlockHeader *)memory - 1;
  alignedBytes = (newSize + ARENA_BLOCK_ALIGNMENT_MASK) & ~ARENA_BLOCK_ALIGNMENT_MASK;
  if (block->stateMagic != ARENA_BLOCK_ALLOCATED || alignedBytes > block->payloadSize) {
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
  splitBlock = (ArenaBlockHeader *)((uint8_t *)block + ARENA_BLOCK_HEADER_BYTES + alignedBytes);
  block->next = splitBlock;
  splitBlock->stateMagic = ARENA_BLOCK_FREE;
  splitBlock->payloadSize = splitPayloadSize;
  splitBlock->previous = block;
  splitBlock->next = followingBlock;
  if (followingBlock != ARENA_BLOCK_LIST_END) {
    followingBlock->previous = splitBlock;
    if (followingBlock->stateMagic == ARENA_BLOCK_FREE) {
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


/* Bump allocation from the linear region g_Arena.linearCursor..linearLimit (g_MemoryApi.reserveLinear):
   returns 0 with the old cursor in *outBase and advances it by bytes, or FATAL_ERROR_GENERAL_FAILURE
   (*outBase unchanged) when the region is full. Nothing is ever given back.
*/
uint32_t ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes,void **outBase)

{
  uint8_t *previousLinearCursor;

  previousLinearCursor = g_Arena.linearCursor;
  if (g_Arena.linearCursor + bytes < g_Arena.linearLimit) {
    g_Arena.linearCursor = g_Arena.linearCursor + bytes;
    *outBase = previousLinearCursor;
    return 0;
  }
  return FATAL_ERROR_GENERAL_FAILURE;
}


/* Zeroes bytes / 4 dwords at destination; a trailing one to three bytes are left unchanged,
   so callers pass multiples of 4.
*/
void Memory_ZeroDwords(MemoryByteCount bytes,void *destination)

{
  uint32_t dwordsRemaining;

  for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *(uint32_t *)destination = 0;
    destination = (uint32_t *)destination + 1;
  }
  return;
}

