# Call graph: `gameplay/technology/runtime`

[Source](../../../../src/gameplay/technology/runtime.c) · [Header](../../../../include/thandor/gameplay/technology/runtime.h) · [Graphviz](runtime.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-gameplay-technology-runtime)

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005139C0` **[`Technology_UnlockForFaction`](../../../../src/gameplay/technology/runtime.c#L5)** — cross: [`InGameNotificationQueue_InsertPriorityRecord`](../../../../src/ui/ingame/runtime.c#L3108) → [`ui/ingame/runtime`](../../ui/ingame/runtime.md), [`ModelRuntimeHierarchy_ApplyFactionTechnologyVariants`](../../../../src/world/model/hierarchy.c#L85) → [`world/model/hierarchy`](../../world/model/hierarchy.md), [`UiCatalogGroup48_RebuildGrid`](../../../../src/ui/ingame/technology.c#L101) → [`ui/ingame/technology`](../../ui/ingame/technology.md), [`UiCatalogGroup42_RebuildGrid`](../../../../src/ui/ingame/technology.c#L259) → [`ui/ingame/technology`](../../ui/ingame/technology.md)
- `0x00513AE0` **[`Technology_IsUnlockedForFactionCf`](../../../../src/gameplay/technology/runtime.c#L84)**
- `0x00513B20` **[`Technology_IsAvailableForFactionCf`](../../../../src/gameplay/technology/runtime.c#L104)**
- `0x0052AE10` **[`Technology_ApplyRecordToEntity`](../../../../src/gameplay/technology/runtime.c#L165)**
- `0x00539BB0` **[`TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks`](../../../../src/gameplay/technology/runtime.c#L202)** — cross: [`ModelDefinitionRegistry_FindByIdWithErrorCf`](../../../../src/assets/model/definitions.c#L800) → [`assets/model/definitions`](../../assets/model/definitions.md)

## Called by

- [`assets/model/definitions`](../../assets/model/definitions.md): [`ModelDefinition_UnlockLinkedTechnologyForFactionCf`](../../../../src/assets/model/definitions.c#L757) → [`Technology_UnlockForFaction`](../../../../src/gameplay/technology/runtime.c#L5)
- [`gameplay/ai/planning`](../ai/planning.md): [`AiArmyCandidate_AddBestScoredVariantA`](../../../../src/gameplay/ai/planning.c#L690) → [`Technology_IsUnlockedForFactionCf`](../../../../src/gameplay/technology/runtime.c#L84); [`AiStrategicClass_AddCandidate12DOr12FTo132`](../../../../src/gameplay/ai/planning.c#L765) → [`Technology_IsUnlockedForFactionCf`](../../../../src/gameplay/technology/runtime.c#L84); [`AiPurchaseCandidate_HasEligibleProducerCf`](../../../../src/gameplay/ai/planning.c#L1091) → [`Technology_IsAvailableForFactionCf`](../../../../src/gameplay/technology/runtime.c#L104); [`AiPurchaseCandidate_ApplyToFaction`](../../../../src/gameplay/ai/planning.c#L1187) → [`Technology_ApplyRecordToEntity`](../../../../src/gameplay/technology/runtime.c#L165)
- [`gameplay/army/runtime`](../army/runtime.md): [`ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive`](../../../../src/gameplay/army/runtime.c#L2995) → [`Technology_UnlockForFaction`](../../../../src/gameplay/technology/runtime.c#L5)
- [`gameplay/faction/runtime`](../faction/runtime.md): [`GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10`](../../../../src/gameplay/faction/runtime.c#L367) → [`Technology_UnlockForFaction`](../../../../src/gameplay/technology/runtime.c#L5)
- [`gameplay/session/runtime`](../session/runtime.md): [`InGameRuntime_InitializeNewSession`](../../../../src/gameplay/session/runtime.c#L718) → [`TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks`](../../../../src/gameplay/technology/runtime.c#L202); [`InGameRuntime_InitializeLoadedSession`](../../../../src/gameplay/session/runtime.c#L1085) → [`TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks`](../../../../src/gameplay/technology/runtime.c#L202)
- [`ui/frontend/player`](../../ui/frontend/player.md): [`FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology`](../../../../src/ui/frontend/player.c#L1108) → [`Technology_ApplyRecordToEntity`](../../../../src/gameplay/technology/runtime.c#L165)
- [`ui/ingame/runtime`](../../ui/ingame/runtime.md): [`InGameSelectionDetailPanel_Rebuild`](../../../../src/ui/ingame/runtime.c#L5357) → [`Technology_IsAvailableForFactionCf`](../../../../src/gameplay/technology/runtime.c#L104)
- [`ui/ingame/technology`](../../ui/ingame/technology.md): [`InGameTechnologyPanel_Rebuild`](../../../../src/ui/ingame/technology.c#L457) → [`Technology_IsAvailableForFactionCf`](../../../../src/gameplay/technology/runtime.c#L104)
