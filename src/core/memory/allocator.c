/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/allocator.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/allocator.h>

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
  EntityPathingPriorityPair *pEVar1;
  uint parentSearchIndex;
  EntityPathingPriorityPair *currentHeapPair;
  sdword parentPriority;
  int childPriority;
  GameEntityRuntime *childEntity;
  
  parentSearchIndex = heapSize - 2;
  currentHeapPair = heapBase + heapSize + -1;
  if (1 < heapSize) {
    do {
      pEVar1 = heapBase + (parentSearchIndex >> 1);
      childPriority = currentHeapPair->priority;
      if (childPriority <= pEVar1->priority) {
        return;
      }
      currentHeapPair->priority = pEVar1->priority;
      pEVar1->priority = childPriority;
      childEntity = currentHeapPair->entity;
      currentHeapPair->entity = pEVar1->entity;
      pEVar1->entity = childEntity;
      parentSearchIndex = (parentSearchIndex >> 1) - 1;
      currentHeapPair = pEVar1;
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
  uint selectedChildIndex;
  GameEntityRuntime *selectedChildEntity;
  EntityPathingPriorityPair *currentHeapPair;
  int leftChildBaseIndex;
  sdword displacedParentPriority;
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
          (DwordBlockRecordCount recordCount,dword *recordArray,dword *candidateRecord)

{
  int dwordsRemainingInRecord;
  dword *sourceRecordCursor;
  dword *candidateRecordCursor;
  undefined1 in_ZF;
  
  do {
    dwordsRemainingInRecord = 0x40;
    candidateRecordCursor = candidateRecord;
    do {
      sourceRecordCursor = recordArray;
      if (dwordsRemainingInRecord == 0) break;
      dwordsRemainingInRecord = dwordsRemainingInRecord + -1;
      sourceRecordCursor = recordArray + 1;
      in_ZF = *recordArray == *candidateRecordCursor;
      recordArray = sourceRecordCursor;
      candidateRecordCursor = candidateRecordCursor + 1;
    } while ((bool)in_ZF);
    if ((bool)in_ZF) {
      return false;
    }
    recordArray = sourceRecordCursor + dwordsRemainingInRecord;
    recordCount = recordCount + -1;
    in_ZF = 0;
    if (recordCount == 0) {
      return true;
    }
  } while( true );
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
  FatalError_Exit(0x407d84,true);
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
ArenaAllocEaxCf5 __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Alloc(ArenaPayloadByteCount bytes)

{
  ArenaBlockHeader *pAVar1;
  uint uVar2;
  ArenaBlockHeader *pAVar3;
  dword dVar4;
  ArenaBlockHeader *pAVar5;
  ArenaAllocEaxCf5 AVar6;
  ArenaAllocEaxCf5 AVar7;
  ArenaAllocEaxCf5 AVar8;
  ArenaAllocEaxCf5 AVar9;
  
  dVar4 = 1;
  uVar2 = bytes + 0x1f & 0xffffffe0;
  pAVar5 = g_Arena.firstBlock;
  do {
    if (pAVar5->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (pAVar5->stateMagic != ARENA_BLOCK_FREE) {
        AVar7.carry = true;
        AVar7.eax = ARENA_HEAP_FAILURE_SENTINEL_0x13;
        return AVar7;
      }
      if (dVar4 < pAVar5->payloadSize) {
        dVar4 = pAVar5->payloadSize;
      }
      if (uVar2 <= pAVar5->payloadSize) {
        pAVar5->stateMagic = ARENA_BLOCK_ALLOCATED;
        if (pAVar5->payloadSize <= uVar2 + 0x40) {
          AVar8.carry = false;
          AVar8.eax = (dword)(pAVar5 + 1);
          return AVar8;
        }
        dVar4 = pAVar5->payloadSize;
        pAVar5->payloadSize = uVar2;
        pAVar1 = pAVar5->next;
        pAVar3 = (ArenaBlockHeader *)
                 (pAVar5[1].alignmentPadding10_1F + (pAVar5->payloadSize - 0x10));
        pAVar3->payloadSize = dVar4 - (pAVar5->payloadSize + 0x20);
        pAVar3->stateMagic = ARENA_BLOCK_FREE;
        pAVar3->previous = pAVar5;
        pAVar5->next = pAVar3;
        pAVar3->next = pAVar1;
        if (pAVar1 != (ArenaBlockHeader *)0xffffffff) {
          pAVar1->previous = pAVar3;
        }
        AVar9.carry = false;
        AVar9.eax = (dword)(pAVar5 + 1);
        return AVar9;
      }
    }
    pAVar5 = pAVar5->next;
    if (pAVar5 == (ArenaBlockHeader *)0xffffffff) {
      (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,dVar4,g_PackageLastErrorPath);
      AVar6.carry = true;
      AVar6.eax = 0x12;
      return AVar6;
    }
  } while( true );
}


/* Address: 0x00586570.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns the sum of
   all free block payload sizes in EAX.
*/
dword __cdecl ArenaHeap_QueryFreeBytes(void)

{
  dword freePayloadBytes;
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
ArenaFreeEaxCf5 __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Free(void *memory)

{
  int iVar1;
  uint in_EAX;
  int *freedBlockHeader;
  ArenaFreeEaxCf5 AVar2;
  ArenaFreeEaxCf5 AVar3;
  int *adjacentFreeBlock;
  int nextBlockAddress;
  int *previousAdjacentBlockHeader;
  
  if (memory != (void *)0x0) {
    freedBlockHeader = (int *)((int)memory + -0x20);
    if (*(int *)((int)memory + -0x1c) != 0x5a5a5a5a) {
      AVar3.carry = true;
      AVar3.eax = ARENA_HEAP_FAILURE_SENTINEL_0x13;
      return AVar3;
    }
    *(undefined4 *)((int)memory + -0x1c) = 0xa5a5a5a5;
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
      iVar1 = *(int *)((int)memory + -0x18);
      previousAdjacentBlockHeader[2] = iVar1;
      if (iVar1 != -1) {
        *(int **)(iVar1 + 0xc) = previousAdjacentBlockHeader;
      }
    }
  }
  AVar2.carry = false;
  AVar2.eax = in_EAX;
  return AVar2;
}


/* Address: 0x00586640.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Marks the largest
   free block allocated and returns its payload pointer in EAX.
*/
ArenaLargestAllocationEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
ArenaHeap_AllocLargestFreeBlock(void)

{
  dword largestFreePayloadBytes;
  ArenaBlockHeader *blockCursor;
  ArenaBlockHeader *largestFreeBlock;
  ArenaLargestAllocationEaxEcxCf9 AVar1;
  ArenaLargestAllocationEaxEcxCf9 AVar2;
  ArenaLargestAllocationEaxEcxCf9 AVar3;
  
  largestFreePayloadBytes = 0;
  blockCursor = g_Arena.firstBlock;
  do {
    if (blockCursor->stateMagic != ARENA_BLOCK_ALLOCATED) {
      if (blockCursor->stateMagic != ARENA_BLOCK_FREE) {
        AVar2.blockSizeOrSentinel = 0xffffffff;
        AVar2.allocationOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
        AVar2.carry = true;
        return AVar2;
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
    AVar1.carry = true;
    AVar1.allocationOrError = 0x12;
    AVar1.blockSizeOrSentinel = 0;
    return AVar1;
  }
  largestFreeBlock->stateMagic = ARENA_BLOCK_ALLOCATED;
  AVar3.blockSizeOrSentinel = largestFreePayloadBytes;
  AVar3.allocationOrError = (dword)(largestFreeBlock + 1);
  AVar3.carry = false;
  return AVar3;
}


/* Address: 0x005866B0.
   Ownership: core/memory/allocator.
   Purpose: Arguments are (newSize, memory). EAX has no stable success value. ABI: CF clear means success. CF set
   means failure and EAX contains an engine error code. Typed parameters: p0 newSize→ArenaPayloadByteCount_V331.
*/
ArenaShrinkEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  uint *puVar8;
  ArenaShrinkEaxCf5 AVar9;
  ArenaShrinkEaxCf5 AVar10;
  
  puVar8 = (uint *)((int)memory + -0x20);
  uVar5 = newSize + 0x1f & 0xffffffe0;
  if ((*(int *)((int)memory + -0x1c) == 0x5a5a5a5a) && (uVar5 <= *puVar8)) {
    piVar6 = (int *)(uVar5 + 0x40);
    if (piVar6 < (int *)*puVar8) {
      uVar1 = *puVar8;
      *puVar8 = uVar5;
      iVar7 = uVar1 - (uVar5 + 0x20);
      piVar2 = *(int **)((int)memory + -0x18);
      piVar6 = (int *)(uVar5 + 0x20 + (int)puVar8);
      *(int **)((int)memory + -0x18) = piVar6;
      piVar6[1] = -0x5a5a5a5b;
      *piVar6 = iVar7;
      piVar6[3] = (int)puVar8;
      piVar6[2] = (int)piVar2;
      if ((piVar2 != (int *)0xffffffff) && (piVar2[3] = (int)piVar6, piVar2[1] == -0x5a5a5a5b)) {
        iVar3 = *piVar2;
        iVar4 = piVar2[2];
        piVar6[2] = iVar4;
        *piVar6 = iVar7 + iVar3 + 0x20;
        if (iVar4 != -1) {
          *(int **)(iVar4 + 0xc) = piVar6;
        }
      }
    }
    AVar9.carry = false;
    AVar9.scratchOrError = (dword)piVar6;
    return AVar9;
  }
  AVar10.carry = true;
  AVar10.scratchOrError = ARENA_HEAP_FAILURE_SENTINEL_0x13;
  return AVar10;
}


/* Address: 0x00586750.
   Ownership: core/memory/allocator.
   Purpose: Assembly ABI: CF=0 success, CF=1 failure; EAX carries a result or engine error code. Returns the
   previous linear cursor in EAX. Typed parameters: p0 bytes→ArenaPayloadByteCount_V331. Calling convention,
   complete VariableStorage serialization, function bytes, control flow, globals, locals, and executable data
   remain unchanged.
*/
ArenaLinearReserveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes)

{
  byte *pbVar1;
  byte *reservedLinearBase;
  ArenaLinearReserveEaxCf5 AVar2;
  ArenaLinearReserveEaxCf5 AVar3;
  
  pbVar1 = g_Arena.linearCursor;
  if (g_Arena.linearCursor + bytes < g_Arena.linearLimit) {
    g_Arena.linearCursor = g_Arena.linearCursor + bytes;
    AVar2.carry = false;
    AVar2.baseOrError = (dword)pbVar1;
    return AVar2;
  }
  AVar3.carry = true;
  AVar3.baseOrError = 0x14;
  return AVar3;
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
  uint dwordsRemaining;
  
  for (dwordsRemaining = bytes >> 2; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining - 1) {
    *(undefined4 *)destination = 0;
    destination = (undefined4 *)((int)destination + 4);
  }
  return;
}

