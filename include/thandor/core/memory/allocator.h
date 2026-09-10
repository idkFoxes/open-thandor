#ifndef THANDOR_CORE_MEMORY_ALLOCATOR_H
#define THANDOR_CORE_MEMORY_ALLOCATOR_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/memory/allocator. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005368E0 */
undefined8 PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00536930 */
void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00547D20 */
void DwordBlock64Array_ContainsExactRecordCf (DwordBlockRecordCount recordCount,dword *recordArray,dword *candidateRecord);

/* 0x005863C0 */
void * __cdecl ArenaHeap_Init(void);

/* 0x00586470 */
void __cdecl ArenaHeap_Shutdown(void);

/* 0x005864A0 */
void * ArenaHeap_Alloc(ArenaPayloadByteCount bytes);

/* 0x00586570 */
dword __cdecl ArenaHeap_QueryFreeBytes(void);

/* 0x005865B0 */
void ArenaHeap_Free(void *memory);

/* 0x00586640 */
void * __cdecl ArenaHeap_AllocLargestFreeBlock(void);

/* 0x005866B0 */
void ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

/* 0x00586750 */
void * ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes);

/* 0x005873A0 */
void Memory_ZeroDwords(MemoryByteCount bytes,void *destination);

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
