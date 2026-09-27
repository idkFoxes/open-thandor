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
   Ownership: core/memory/allocator.
   Purpose: Sifts the final pointer-and-priority pair upward in a binary max-heap until the parent priority is not
   smaller. Typed parameters: p2 heapSize→PriorityPairHeapCount_V343, p3 heapBase→EntityPathingPriorityPair *.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  EntityPathingPriorityPair *parentHeapPair;
  uint32_t parentSearchIndex;
  EntityPathingPriorityPair *currentHeapPair;
  int32_t parentPriority;
  int childPriority;
  GameEntityRuntime *childEntity;
  
  parentSearchIndex = heapSize - 2;
  currentHeapPair = heapBase + heapSize + -1;
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
  return;
}


/* Address: 0x00536930.
   Ownership: core/memory/allocator.
   Purpose: Sifts the root pointer-and-priority pair downward in a binary max-heap, selecting the larger-priority
   child at each step. Typed parameters: p2 heapSize→PriorityPairHeapCount_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  int selectedChildPriority;
  uint32_t selectedChildIndex;
  GameEntityRuntime *selectedChildEntity;
  EntityPathingPriorityPair *currentHeapPair;
  int leftChildBaseIndex;
  int32_t displacedParentPriority;
  GameEntityRuntime *displacedParentEntity;
  
  selectedChildIndex = 0;
  currentHeapPair = heapBase;
  while( true ) {
    leftChildBaseIndex = selectedChildIndex * 2;
    selectedChildIndex = leftChildBaseIndex + 1;
    if (heapSize - 1U < selectedChildIndex) {
      return;
    }
    selectedChildPriority = heapBase[selectedChildIndex].priority;
    selectedChildEntity = heapBase[selectedChildIndex].entity;
    if ((selectedChildIndex < heapSize - 1U) &&
       (selectedChildPriority < heapBase[leftChildBaseIndex + 2].priority)) {
      selectedChildIndex = leftChildBaseIndex + 2;
      selectedChildPriority = heapBase[selectedChildIndex].priority;
      selectedChildEntity = heapBase[selectedChildIndex].entity;
    }
    if (selectedChildPriority <= currentHeapPair->priority) break;
    LOCK();
    displacedParentPriority = currentHeapPair->priority;
    currentHeapPair->priority = selectedChildPriority;
    UNLOCK();
    LOCK();
    displacedParentEntity = currentHeapPair->entity;
    currentHeapPair->entity = selectedChildEntity;
    UNLOCK();
    heapBase[selectedChildIndex].priority = displacedParentPriority;
    heapBase[selectedChildIndex].entity = displacedParentEntity;
    currentHeapPair = heapBase + selectedChildIndex;
  }
  return;
}


/* Address: 0x00547D20.
   Ownership: core/memory/allocator.
   Purpose: Scans recordCount consecutive records of exactly 0x40 dwords and compares each against targetRecord. CF
   clear reports a match; CF set reports exhaustion. Typed parameters: p0 recordCount→DwordBlockRecordCount_V343.
   Calling convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
DwordBlock64Array_ContainsExactRecordCf
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
      dwordsRemainingInRecord = dwordsRemainingInRecord + -1;
      dwordsEqual = *recordArray == *candidateRecordCursor;
      recordArray = recordArray + 1;
      candidateRecordCursor = candidateRecordCursor + 1;
    } while (dwordsEqual && (dwordsRemainingInRecord != 0));
    if (dwordsEqual) {
      return false;
    }
    recordArray = recordArray + dwordsRemainingInRecord;
    recordCount = recordCount + -1;
  } while (recordCount != 0);
  return true;
}


/* Address: 0x005863C0.
   Ownership: core/memory/allocator.
   Purpose: Creates a 96 MiB custom arena on a private Win32 heap and returns the raw HeapAlloc pointer in EAX.
   Cross-module calls: FatalError_Exit [core/error/runtime].
*/
void * __cdecl ArenaHeap_Init(void)

{
  HANDLE hHeap;
  LPVOID rawArenaAllocation;
  ArenaBlockHeader *alignedFirstBlock;
  
  hHeap = HeapCreate(0,0x6000040,0);
  if (hHeap != (HANDLE)0x0) {
    g_MemoryApi.alloc = ArenaHeap_Alloc;
    g_MemoryApi.free = ArenaHeap_Free;
    g_MemoryApi.allocLargestFreeBlock = ArenaHeap_AllocLargestFreeBlock;
    g_MemoryApi.shrinkInPlace = ArenaHeap_ShrinkInPlace;
    g_MemoryApi.queryFreeBytes = ArenaHeap_QueryFreeBytes;
    g_MemoryApi.reserveLinear = ArenaHeap_ReserveLinear;
    g_Arena.processHeap = hHeap;
    rawArenaAllocation = HeapAlloc(hHeap,0,0x6000040);
    if (rawArenaAllocation != (LPVOID)0x0) {
      alignedFirstBlock = (ArenaBlockHeader *)((int)rawArenaAllocation + 0x1fU & 0xffffffe0);
      g_Arena.rawAllocation = rawArenaAllocation;
      g_Arena.firstBlock = alignedFirstBlock;
      alignedFirstBlock->payloadSize = 0x6000000;
      alignedFirstBlock->stateMagic = ARENA_BLOCK_FREE;
      alignedFirstBlock->next = (ArenaBlockHeader *)0xffffffff;
      alignedFirstBlock->previous = (ArenaBlockHeader *)0xffffffff;
      return rawArenaAllocation;
    }
  }
                    // WARNING: Subroutine does not return
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextHeapAllocationFailed,0),true);
}


/* Address: 0x00586470.
   Ownership: core/memory/allocator.
   Purpose: Handles arena heap shutdown.
*/
void __thandor_preserve_eax ArenaHeap_Shutdown(void)

{
  HeapFree(g_Arena.processHeap,0,g_Arena.rawAllocation);
  HeapDestroy(g_Arena.processHeap);
  return;
}


/* Address: 0x005864A0.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns payload
   pointer in EAX. Typed parameters: p0 bytes→ArenaPayloadByteCount_V331. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
ArenaAllocResult __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Alloc(ArenaPayloadByteCount bytes)

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
  alignedBytes = bytes + 0x1f & 0xffffffe0;
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
        if (blockCursor->payloadSize <= alignedBytes + 0x40) {
          exactFitResult.failed = false;
          exactFitResult.payloadOrError = (uint32_t)(blockCursor + 1);
          return exactFitResult;
        }
        largestFreeOrOriginalSize = blockCursor->payloadSize;
        blockCursor->payloadSize = alignedBytes;
        followingBlock = blockCursor->next;
        splitBlock = (ArenaBlockHeader *)
                 (blockCursor[1].alignmentPadding10_1F + (blockCursor->payloadSize - 0x10));
        splitBlock->payloadSize = largestFreeOrOriginalSize - (blockCursor->payloadSize + 0x20);
        splitBlock->stateMagic = ARENA_BLOCK_FREE;
        splitBlock->previous = blockCursor;
        blockCursor->next = splitBlock;
        splitBlock->next = followingBlock;
        if (followingBlock != (ArenaBlockHeader *)0xffffffff) {
          followingBlock->previous = splitBlock;
        }
        splitResult.failed = false;
        splitResult.payloadOrError = (uint32_t)(blockCursor + 1);
        return splitResult;
      }
    }
    blockCursor = blockCursor->next;
    if (blockCursor == (ArenaBlockHeader *)0xffffffff) {
      (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,largestFreeOrOriginalSize,g_PackageLastErrorPath);
      outOfMemoryResult.failed = true;
      outOfMemoryResult.payloadOrError = 0x12;
      return outOfMemoryResult;
    }
  } while( true );
}


/* Address: 0x00586570.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns the sum of
   all free block payload sizes in EAX.
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
  } while (blockCursor != (ArenaBlockHeader *)0xffffffff);
  return freePayloadBytes;
}

/* Address: 0x005865B0.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code.
*/
ArenaFreeResult __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Free(void *memory)

{
  int mergedNextBlockAddress;
  int *freedBlockHeader;
  ArenaFreeResult successResult;
  ArenaFreeResult corruptBlockResult;
  int *adjacentFreeBlock;
  int nextBlockAddress;
  int *previousAdjacentBlockHeader;
  
  if (memory != (void *)0x0) {
    freedBlockHeader = (int *)((int)memory + -0x20);
    if (*(int *)((int)memory + -0x1c) != 0x5a5a5a5a) {
      corruptBlockResult.failed = true;
      corruptBlockResult.valueOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
      return corruptBlockResult;
    }
    *(uint32_t *)((int)memory + -0x1c) = 0xa5a5a5a5;
    adjacentFreeBlock = *(int **)((int)memory + -0x18);
    if ((adjacentFreeBlock != (int *)0xffffffff) && (adjacentFreeBlock[1] == -0x5a5a5a5b)) {
      *freedBlockHeader = *freedBlockHeader + *adjacentFreeBlock + 0x20;
      nextBlockAddress = adjacentFreeBlock[2];
      *(int *)((int)memory + -0x18) = nextBlockAddress;
      if (nextBlockAddress != -1) {
        *(int **)(nextBlockAddress + 0xc) = freedBlockHeader;
      }
    }
    previousAdjacentBlockHeader = *(int **)((int)memory + -0x14);
    if ((previousAdjacentBlockHeader != (int *)0xffffffff) &&
       (previousAdjacentBlockHeader[1] == -0x5a5a5a5b)) {
      *previousAdjacentBlockHeader = *previousAdjacentBlockHeader + *freedBlockHeader + 0x20;
      mergedNextBlockAddress = *(int *)((int)memory + -0x18);
      previousAdjacentBlockHeader[2] = mergedNextBlockAddress;
      if (mergedNextBlockAddress != -1) {
        *(int **)(mergedNextBlockAddress + 0xc) = previousAdjacentBlockHeader;
      }
    }
  }
  /* The original returns with EAX unchanged on success; callers only test CF. */
  successResult.failed = false;
  successResult.valueOrError = 0;
  return successResult;
}


/* Address: 0x00586640.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Marks the largest
   free block allocated and returns its payload pointer in EAX.
*/
ArenaLargestAllocResult __thandor_eax_ecx_cf_preserve_edx
ArenaHeap_AllocLargestFreeBlock(void)

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
  } while (blockCursor != (ArenaBlockHeader *)0xffffffff);
  if (largestFreePayloadBytes == 0) {
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0,g_PackageLastErrorPath);
    outOfMemoryResult.failed = true;
    outOfMemoryResult.allocationOrError = 0x12;
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
   Ownership: core/memory/allocator.
   Purpose: Arguments are (newSize, memory). EAX has no stable success value. ABI: CF clear means success. CF set
   means failure and EAX contains an engine error code. Typed parameters: p0 newSize→ArenaPayloadByteCount_V331.
*/
ArenaShrinkResult __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory)

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
  
  blockHeader = (uint32_t *)((int)memory + -0x20);
  alignedBytes = newSize + 0x1f & 0xffffffe0;
  if ((*(int *)((int)memory + -0x1c) == 0x5a5a5a5a) && (alignedBytes <= *blockHeader)) {
    thresholdOrSplitBlock = (int *)(alignedBytes + 0x40);
    if (thresholdOrSplitBlock < (int *)*blockHeader) {
      originalPayloadSize = *blockHeader;
      *blockHeader = alignedBytes;
      splitPayloadSize = originalPayloadSize - (alignedBytes + 0x20);
      followingBlock = *(int **)((int)memory + -0x18);
      thresholdOrSplitBlock = (int *)(alignedBytes + 0x20 + (int)blockHeader);
      *(int **)((int)memory + -0x18) = thresholdOrSplitBlock;
      thresholdOrSplitBlock[1] = -0x5a5a5a5b;
      *thresholdOrSplitBlock = splitPayloadSize;
      thresholdOrSplitBlock[3] = (int)blockHeader;
      thresholdOrSplitBlock[2] = (int)followingBlock;
      if ((followingBlock != (int *)0xffffffff) && (followingBlock[3] = (int)thresholdOrSplitBlock, followingBlock[1] == -0x5a5a5a5b)) {
        followingPayloadSize = *followingBlock;
        followingNextAddress = followingBlock[2];
        thresholdOrSplitBlock[2] = followingNextAddress;
        *thresholdOrSplitBlock = splitPayloadSize + followingPayloadSize + 0x20;
        if (followingNextAddress != -1) {
          *(int **)(followingNextAddress + 0xc) = thresholdOrSplitBlock;
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
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns the
   previous linear cursor in EAX. Typed parameters: p0 bytes→ArenaPayloadByteCount_V331. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
*/
ArenaReserveResult __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes)

{
  uint8_t *previousLinearCursor;
  uint8_t *reservedLinearBase;
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
  failureResult.baseOrError = 0x14;
  return failureResult;
}


/* Address: 0x005873A0.
   Ownership: core/memory/allocator.
   Purpose: Zeros floor(bytes/4) dwords at destination with rep stosd. Any trailing one to three bytes are
   intentionally left unchanged. Typed parameters: p0 bytes→MemoryByteCount_V343. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx Memory_ZeroDwords(MemoryByteCount bytes,void *destination)

{
  uint32_t dwordsRemaining;
  
  for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
    *(uint32_t *)destination = 0;
    destination = (uint32_t *)((int)destination + 4);
  }
  return;
}

