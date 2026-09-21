# Call graph: `world/model/slots`

[Source](../../../../src/world/model/slots.c) · [Header](../../../../include/thandor/world/model/slots.h) · [Graphviz](slots.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-world-model-slots) · [Developer notes](../../../../CHANGELOG.md#module-world-model-slots) · [Full changelog](../../../../CHANGELOG_FULL.md#module-world-model-slots)

20 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005200C0` **[`ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices`](../../../../src/world/model/slots.c#L5)**
- `0x00522A90` **[`ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming`](../../../../src/world/model/slots.c#L39)**
- `0x00522B90` **[`ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform`](../../../../src/world/model/slots.c#L80)**
- `0x00523CA0` **[`ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters`](../../../../src/world/model/slots.c#L109)** — cross: [`ModelLookupTable_FindPackedKeyEntryRegsCf`](../../../../src/assets/model/definitions.c#L188) → [`assets/model/definitions`](../../assets/model/definitions.md)
- `0x005243D0` **[`ModelRuntimeSlotClassInit_ClearFields64_68_74_B8`](../../../../src/world/model/slots.c#L182)**
- `0x00524CB0` **[`ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C`](../../../../src/world/model/slots.c#L198)**
- `0x00524CE0` **[`ModelRuntimeSlot_RebaseClassArmyLinkOffset6C`](../../../../src/world/model/slots.c#L216)**
- `0x00524D10` **[`ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0`](../../../../src/world/model/slots.c#L235)**
- `0x005251C0` **[`ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3`](../../../../src/world/model/slots.c#L267)** — cross: [`WorldRuntime_UnlinkNodeFromOwnerListD8`](../../../../src/world/runtime/core.c#L1077) → [`world/runtime/core`](../runtime/core.md)
- `0x00525250` **[`ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1`](../../../../src/world/model/slots.c#L301)** — cross: [`WorldRuntime_UnlinkNodeFromOwnerListD8`](../../../../src/world/runtime/core.c#L1077) → [`world/runtime/core`](../runtime/core.md)
- `0x005252E0` **[`ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity`](../../../../src/world/model/slots.c#L335)**
- `0x005254B0` **[`ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity`](../../../../src/world/model/slots.c#L356)**
- `0x00526340` **[`ModelRuntimeSlot_UnrebaseClassModelLinkOffset60`](../../../../src/world/model/slots.c#L377)**
- `0x00526370` **[`ModelRuntimeSlot_RebaseClassModelLinkOffset60`](../../../../src/world/model/slots.c#L395)**
- `0x005263A0` **[`ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset`](../../../../src/world/model/slots.c#L414)**
- `0x00526E00` **[`ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation`](../../../../src/world/model/slots.c#L434)**
- `0x00527B80` **[`ModelRuntimeSlotPointerRebase_NoOp`](../../../../src/world/model/slots.c#L474)**
- `0x00527B90` **[`ModelRuntimeSlotClassInit_NoOp`](../../../../src/world/model/slots.c#L485)**
- `0x005283B0` **[`ModelRuntimeSlotClassInit_ClearField60`](../../../../src/world/model/slots.c#L497)**
- `0x005285D0` **[`ModelRuntimeSlotClassInit_ClearFields60AndB8`](../../../../src/world/model/slots.c#L510)**

## Called by

No cross-module callers are recorded.
