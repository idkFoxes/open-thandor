/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/allocator.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/allocator.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/memory/allocator. */

/* Address: 0x005368E0.
   Heap-building step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): the newly
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


/* Address: 0x00536930.
   Extraction step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): after the root was
   swapped with the last entry, the new root entity/priority pair moves down the max-heap, swapping with its
   larger-priority child while that child is larger.
*/
void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  int selectedChildPriority;
  uint32_t selectedChildIndex;
  GameEntityRuntime *selectedChildEntity;
  EntityPathingPriorityPair *currentHeapPair;
  int doubledParentIndex;
  int32_t displacedParentPriority;
  GameEntityRuntime *displacedParentEntity;

  selectedChildIndex = 0;
  currentHeapPair = heapBase;
  while( true ) {
    /* children of index i are 2i + 1 and 2i + 2 */
    doubledParentIndex = selectedChildIndex * 2;
    selectedChildIndex = doubledParentIndex + 1;
    if (heapSize - 1U < selectedChildIndex) {
      return;
    }
    selectedChildPriority = heapBase[selectedChildIndex].priority;
    selectedChildEntity = heapBase[selectedChildIndex].entity;
    if ((selectedChildIndex < heapSize - 1U) &&
       (selectedChildPriority < heapBase[doubledParentIndex + 2].priority)) {
      selectedChildIndex = doubledParentIndex + 2;
      selectedChildPriority = heapBase[selectedChildIndex].priority;
      selectedChildEntity = heapBase[selectedChildIndex].entity;
    }
    if (selectedChildPriority <= currentHeapPair->priority) break;
    /* the original swaps parent and child with XCHG */
    displacedParentPriority = currentHeapPair->priority;
    currentHeapPair->priority = selectedChildPriority;
    displacedParentEntity = currentHeapPair->entity;
    currentHeapPair->entity = selectedChildEntity;
    heapBase[selectedChildIndex].priority = displacedParentPriority;
    heapBase[selectedChildIndex].entity = displacedParentEntity;
    currentHeapPair = heapBase + selectedChildIndex;
  }
}


/* Address: 0x00547D20.
   Tells whether recordArray (recordCount records of 0x40 dwords each) contains a record equal to
   candidateRecord. Inverted like all CF results: false (CF clear) = found, true (CF set) = not found.
   recordCount must be at least 1.
*/
bool DwordBlock64Array_ContainsExactRecord
          (DwordBlockRecordCount recordCount,uint32_t *recordArray,uint32_t *candidateRecord)

{
  int dwordsRemainingInRecord;
  uint32_t *candidateRecordCursor;
  bool dwordsEqual;

  do {
    /* REPE CMPSD over the 0x40 dwords (the count is nonzero, so ZF is the last comparison) */
    dwordsRemainingInRecord = 0x40;
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


/* Address: 0x005863C0.
   Creates the game's 96 MiB memory arena: allocates it in one piece from a private Win32 heap, installs the
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
          ((int)rawArenaAllocation + ARENA_BLOCK_ALIGNMENT_MASK & ~ARENA_BLOCK_ALIGNMENT_MASK);
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


/* Address: 0x00586470.
   Frees the arena allocation and destroys the private Win32 heap created by ArenaHeap_Init.
*/
void ArenaHeap_Shutdown(void)

{
  HeapFree(g_Arena.processHeap,0,g_Arena.rawAllocation);
  HeapDestroy(g_Arena.processHeap);
  return;
}


/* Address: 0x005864A0.
   The arena's malloc (g_MemoryApi.alloc): first fit over the block chain for the size rounded up to 32
   bytes, splitting off the rest of the block as a new free block when it is large enough. Returns the
   payload pointer with CF clear; with CF set FATAL_ERROR_ARENA_EXHAUSTED (largest free size left in
   g_PackageLastErrorPath) or ARENA_HEAP_FAILURE_SENTINEL_0x13 for a corrupt block chain.
*/
ArenaAllocResult ArenaHeap_Alloc(ArenaPayloadByteCount bytes)

{
  ArenaBlockHeader *followingBlock;
  uint32_t alignedBytes;
  ArenaBlockHeader *splitBlock;
  uint32_t largestFreeOrOriginalSize;
  ArenaBlockHeader *blockCursor;
  ArenaAllocResult outOfMemoryResult;
  ArenaAllocResult corruptHeapResult;
  ArenaAllocResult exactFitResult;
  ArenaAllocResult splitResult;

  largestFreeOrOriginalSize = 1;
  alignedBytes = bytes + ARENA_BLOCK_ALIGNMENT_MASK & ~ARENA_BLOCK_ALIGNMENT_MASK;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
        corruptHeapResult.failed = true;
        corruptHeapResult.payloadOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
        return corruptHeapResult;
      }
      if (largestFreeOrOriginalSize < blockCursor->payloadSize) {
        largestFreeOrOriginalSize = blockCursor->payloadSize;
      }
      if (alignedBytes <= blockCursor->payloadSize) {
        blockCursor->stateMagic = ARENA_BLOCK_ALLOCATED;
        if (blockCursor->payloadSize <= alignedBytes + ARENA_BLOCK_SPLIT_SLACK_BYTES) {
          exactFitResult.failed = false;
          exactFitResult.payloadOrError = (uint32_t)(blockCursor + 1);
          return exactFitResult;
        }
        largestFreeOrOriginalSize = blockCursor->payloadSize;
        blockCursor->payloadSize = alignedBytes;
        followingBlock = blockCursor->next;
        /* the new free block starts right behind the shortened payload */
        splitBlock = (ArenaBlockHeader *)
                 (blockCursor[1].alignmentPadding10_1F + (blockCursor->payloadSize - 0x10));
        splitBlock->payloadSize = largestFreeOrOriginalSize - (blockCursor->payloadSize + ARENA_BLOCK_HEADER_BYTES);
        splitBlock->stateMagic = ARENA_BLOCK_FREE;
        splitBlock->previous = blockCursor;
        blockCursor->next = splitBlock;
        splitBlock->next = followingBlock;
        if (followingBlock != ARENA_BLOCK_LIST_END) {
          followingBlock->previous = splitBlock;
        }
        splitResult.failed = false;
        splitResult.payloadOrError = (uint32_t)(blockCursor + 1);
        return splitResult;
      }
    }
    blockCursor = blockCursor->next;
    if (blockCursor == ARENA_BLOCK_LIST_END) {
      g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,largestFreeOrOriginalSize,g_PackageLastErrorPath);
      outOfMemoryResult.failed = true;
      outOfMemoryResult.payloadOrError = FATAL_ERROR_ARENA_EXHAUSTED;
      return outOfMemoryResult;
    }
  } while( true );
}


/* Address: 0x00586570.
   Returns the sum of all free payload bytes in the arena (g_MemoryApi.queryFreeBytes), or
   ARENA_HEAP_FAILURE_SENTINEL_0x13 when the block chain is corrupt.
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
        return ARENA_HEAP_FAILURE_SENTINEL_0x13;
      }
      freePayloadBytes = freePayloadBytes + blockCursor->payloadSize;
    }
    blockCursor = blockCursor->next;
  } while (blockCursor != ARENA_BLOCK_LIST_END);
  return freePayloadBytes;
}

/* Address: 0x005865B0.
   The arena's free (g_MemoryApi.free): marks the block free and merges it with a free following block,
   then merges a free preceding block with it. NULL is accepted; a payload whose header is not marked
   allocated returns ARENA_HEAP_FAILURE_SENTINEL_0x13 with CF set.
   The ArenaBlockHeader lies directly below the payload; the neighbour headers are also read as dword
   arrays ([0] payloadSize, [1] stateMagic, [2] next, [3] previous).
*/
ArenaFreeResult ArenaHeap_Free(void *memory)

{
  int mergedNextBlockAddress;
  int *freedBlockHeader;
  ArenaFreeResult successResult;
  ArenaFreeResult corruptBlockResult;
  int *adjacentFreeBlock;
  int nextBlockAddress;
  int *previousAdjacentBlockHeader;

  if (memory != NULL) {
    freedBlockHeader = (int *)((int)memory - ARENA_BLOCK_HEADER_BYTES);
    if (((ArenaBlockHeader *)memory)[-1].stateMagic != ARENA_BLOCK_ALLOCATED) {
      corruptBlockResult.failed = true;
      corruptBlockResult.valueOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
      return corruptBlockResult;
    }
    ((ArenaBlockHeader *)memory)[-1].stateMagic = ARENA_BLOCK_FREE;
    /* merge the following block ([2] = next, [3] = previous) */
    adjacentFreeBlock = (int *)((ArenaBlockHeader *)memory - 1)->next;
    if ((adjacentFreeBlock != (int *)ARENA_BLOCK_LIST_END) && (adjacentFreeBlock[1] == (int)ARENA_BLOCK_FREE)) {
      *freedBlockHeader = *freedBlockHeader + *adjacentFreeBlock + ARENA_BLOCK_HEADER_BYTES;
      nextBlockAddress = adjacentFreeBlock[2];
      ((ArenaBlockHeader *)memory)[-1].next = (ArenaBlockHeader *)nextBlockAddress;
      if (nextBlockAddress != -1) {
        ((ArenaBlockHeader *)nextBlockAddress)->previous = (ArenaBlockHeader *)freedBlockHeader;
      }
    }
    /* merge into the preceding block */
    previousAdjacentBlockHeader = (int *)((ArenaBlockHeader *)memory)[-1].previous;
    if ((previousAdjacentBlockHeader != (int *)ARENA_BLOCK_LIST_END) &&
       (previousAdjacentBlockHeader[1] == (int)ARENA_BLOCK_FREE)) {
      *previousAdjacentBlockHeader = *previousAdjacentBlockHeader + *freedBlockHeader + ARENA_BLOCK_HEADER_BYTES;
      mergedNextBlockAddress = (int)((ArenaBlockHeader *)memory)[-1].next;
      previousAdjacentBlockHeader[2] = mergedNextBlockAddress;
      if (mergedNextBlockAddress != -1) {
        ((ArenaBlockHeader *)mergedNextBlockAddress)->previous = (ArenaBlockHeader *)previousAdjacentBlockHeader;
      }
    }
  }
  /* The original returns with EAX unchanged on success; callers only test CF. */
  successResult.failed = false;
  successResult.valueOrError = 0;
  return successResult;
}


/* Address: 0x00586640.
   Takes the largest free block whole (g_MemoryApi.allocLargestFreeBlock): marks it allocated and returns
   its payload pointer in EAX and its size in ECX, for callers that shrink it afterwards with
   ArenaHeap_ShrinkInPlace. CF set with FATAL_ERROR_ARENA_EXHAUSTED when nothing is free, or
   ARENA_HEAP_FAILURE_SENTINEL_0x13 (ECX 0xFFFFFFFF) for a corrupt block chain.
*/
ArenaLargestAllocResult ArenaHeap_AllocLargestFreeBlock(void)

{
  uint32_t largestFreePayloadBytes;
  ArenaBlockHeader *blockCursor;
  ArenaBlockHeader *largestFreeBlock;
  ArenaLargestAllocResult outOfMemoryResult;
  ArenaLargestAllocResult corruptHeapResult;
  ArenaLargestAllocResult successResult;
  
  largestFreePayloadBytes = 0;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
        corruptHeapResult.blockSizeOrSentinel = 0xffffffff;
        corruptHeapResult.allocationOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
        corruptHeapResult.failed = true;
        return corruptHeapResult;
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
    outOfMemoryResult.failed = true;
    outOfMemoryResult.allocationOrError = FATAL_ERROR_ARENA_EXHAUSTED;
    outOfMemoryResult.blockSizeOrSentinel = 0;
    return outOfMemoryResult;
  }
  largestFreeBlock->stateMagic = ARENA_BLOCK_ALLOCATED;
  successResult.blockSizeOrSentinel = largestFreePayloadBytes;
  successResult.allocationOrError = (uint32_t)(largestFreeBlock + 1);
  successResult.failed = false;
  return successResult;
}


/* Address: 0x005866B0.
   Shrinks an allocated block to newSize (rounded up to 32 bytes) and returns the tail as a free block,
   merged with a free following block (g_MemoryApi.shrinkInPlace). A tail too small to split is kept.
   CF clear on success (EAX carries no meaning); CF set with ARENA_HEAP_FAILURE_SENTINEL_0x13 when the
   block is not allocated or newSize is larger than the block.
*/
ArenaShrinkResult ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory)

{
  uint32_t originalPayloadSize;
  int *followingBlock;
  int followingPayloadSize;
  int followingNextAddress;
  uint32_t alignedBytes;
  int *thresholdOrSplitBlock;
  int splitPayloadSize;
  uint32_t *blockHeader;
  ArenaShrinkResult successResult;
  ArenaShrinkResult failureResult;

  /* header dwords: [0] payloadSize, [1] stateMagic, [2] next, [3] previous (see ArenaBlockHeader) */
  blockHeader = (uint32_t *)((int)memory - ARENA_BLOCK_HEADER_BYTES);
  alignedBytes = newSize + ARENA_BLOCK_ALIGNMENT_MASK & ~ARENA_BLOCK_ALIGNMENT_MASK;
  if ((((ArenaBlockHeader *)memory)[-1].stateMagic == ARENA_BLOCK_ALLOCATED) && (alignedBytes <= *blockHeader)) {
    thresholdOrSplitBlock = (int *)(alignedBytes + ARENA_BLOCK_SPLIT_SLACK_BYTES);
    if (thresholdOrSplitBlock < (int *)*blockHeader) {
      originalPayloadSize = *blockHeader;
      *blockHeader = alignedBytes;
      splitPayloadSize = originalPayloadSize - (alignedBytes + ARENA_BLOCK_HEADER_BYTES);
      followingBlock = (int *)((ArenaBlockHeader *)memory)[-1].next;
      thresholdOrSplitBlock = (int *)(alignedBytes + ARENA_BLOCK_HEADER_BYTES + (int)blockHeader);
      ((ArenaBlockHeader *)memory)[-1].next = (ArenaBlockHeader *)thresholdOrSplitBlock;
      thresholdOrSplitBlock[1] = (int)ARENA_BLOCK_FREE;
      *thresholdOrSplitBlock = splitPayloadSize;
      thresholdOrSplitBlock[3] = (int)blockHeader;
      thresholdOrSplitBlock[2] = (int)followingBlock;
      if ((followingBlock != (int *)ARENA_BLOCK_LIST_END) && (followingBlock[3] = (int)thresholdOrSplitBlock, followingBlock[1] == (int)ARENA_BLOCK_FREE)) {
        followingPayloadSize = *followingBlock;
        followingNextAddress = followingBlock[2];
        thresholdOrSplitBlock[2] = followingNextAddress;
        *thresholdOrSplitBlock = splitPayloadSize + followingPayloadSize + ARENA_BLOCK_HEADER_BYTES;
        if (followingNextAddress != -1) {
          ((ArenaBlockHeader *)followingNextAddress)->previous = (ArenaBlockHeader *)thresholdOrSplitBlock;
        }
      }
    }
    successResult.failed = false;
    successResult.scratchOrError = (uint32_t)thresholdOrSplitBlock;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.scratchOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
  return failureResult;
}


/* Address: 0x00586750.
   Bump allocation from the linear region g_Arena.linearCursor..linearLimit (g_MemoryApi.reserveLinear):
   returns the old cursor and advances it by bytes, or CF set with FATAL_ERROR_GENERAL_FAILURE when the
   region is full. Nothing is ever given back.
*/
ArenaReserveResult ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes)

{
  uint8_t *previousLinearCursor;
  ArenaReserveResult successResult;
  ArenaReserveResult failureResult;

  previousLinearCursor = g_Arena.linearCursor;
  if (g_Arena.linearCursor + bytes < g_Arena.linearLimit) {
    g_Arena.linearCursor = g_Arena.linearCursor + bytes;
    successResult.failed = false;
    successResult.baseOrError = (uint32_t)previousLinearCursor;
    return successResult;
  }
  failureResult.failed = true;
  failureResult.baseOrError = FATAL_ERROR_GENERAL_FAILURE;
  return failureResult;
}


/* Address: 0x005873A0.
   Zeroes bytes / 4 dwords at destination (REP STOSD); a trailing one to three bytes are left unchanged,
   so callers pass multiples of 4.
*/
void Memory_ZeroDwords(MemoryByteCount bytes,void *destination)

{
  uint32_t dwordsRemaining;

  for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
    *(uint32_t *)destination = 0;
    destination = (uint32_t *)((int)destination + 4);
  }
  return;
}

