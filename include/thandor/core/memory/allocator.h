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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005368E0 */
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00536930 */
void __thandor_void_preserve_eax_ecx_edx
PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

/* 0x00547D20 */
bool __thandor_cf_preserve_eax_ecx_edx
DwordBlock64Array_ContainsExactRecordCf
          (DwordBlockRecordCount recordCount,dword *recordArray,dword *candidateRecord);

/* 0x005863C0 */
void * __cdecl ArenaHeap_Init(void);

/* 0x00586470 */
void __thandor_preserve_eax ArenaHeap_Shutdown(void);

/* 0x005864A0 */
ArenaAllocEaxCf5 __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Alloc(ArenaPayloadByteCount bytes);

/* 0x00586570 */
dword __cdecl ArenaHeap_QueryFreeBytes(void);

/* 0x005865B0 */
ArenaFreeEaxCf5 __thandor_eax_cf_preserve_ecx_edx ArenaHeap_Free(void *memory);

/* 0x00586640 */
ArenaLargestAllocationEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
ArenaHeap_AllocLargestFreeBlock(void);

/* 0x005866B0 */
ArenaShrinkEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ShrinkInPlace(ArenaPayloadByteCount newSize,void *memory);

/* 0x00586750 */
ArenaLinearReserveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArenaHeap_ReserveLinear(ArenaPayloadByteCount bytes);

/* 0x005873A0 */
void __thandor_void_preserve_eax_ecx_edx Memory_ZeroDwords(MemoryByteCount bytes,void *destination);

#endif /* THANDOR_CORE_MEMORY_ALLOCATOR_H */
