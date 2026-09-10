# Call graph: `core/memory/allocator`

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005368E0` **PriorityPairHeap_SiftUp**
- `0x00536930` **PriorityPairHeap_SiftDown**
- `0x00547D20` **DwordBlock64Array_ContainsExactRecordCf**
- `0x005863C0` **ArenaHeap_Init** — cross: `FatalError_Exit` → `core/error/runtime`
- `0x00586470` **ArenaHeap_Shutdown**
- `0x005864A0` **ArenaHeap_Alloc**
- `0x00586570` **ArenaHeap_QueryFreeBytes**
- `0x005865B0` **ArenaHeap_Free**
- `0x00586640` **ArenaHeap_AllocLargestFreeBlock**
- `0x005866B0` **ArenaHeap_ShrinkInPlace**
- `0x00586750` **ArenaHeap_ReserveLinear**
- `0x005873A0` **Memory_ZeroDwords**
