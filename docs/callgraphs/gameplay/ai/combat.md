# Call graph: `gameplay/ai/combat`

[Source](../../../../src/gameplay/ai/combat.c) · [Header](../../../../include/thandor/gameplay/ai/combat.h) · [Graphviz](combat.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-gameplay-ai-combat) · [Full changelog](../../../../CHANGELOG_FULL.md#module-gameplay-ai-combat)

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00536FC0` **[`AiCombatDecision_UpdateTargetAssignment`](../../../../src/gameplay/ai/combat.c#L5)** — local: [`AiCombatTarget_SelectBestCandidate`](../../../../src/gameplay/ai/combat.c#L133); cross: [`ArmyRuntime_ResolveCommandTarget`](../../../../src/gameplay/army/runtime.c#L1685) → [`gameplay/army/runtime`](../army/runtime.md)
- `0x0053B8B0` **[`AiUnitGroup_AssignCollectedEntitiesToBestTarget`](../../../../src/gameplay/ai/combat.c#L47)** — cross: [`ModelRuntime_QueryHierarchyScaleRatioQ12Regs`](../../../../src/world/model/runtime.c#L321) → [`world/model/runtime`](../../world/model/runtime.md), [`ArmyRuntime_ResolveCommandTargetAndRoute`](../../../../src/gameplay/army/movement.c#L314) → [`gameplay/army/movement`](../army/movement.md)
- `0x005372C0` **[`AiCombatTarget_SelectBestCandidate`](../../../../src/gameplay/ai/combat.c#L133)** — local: [`AiCombatTarget_EvaluateCandidateScore`](../../../../src/gameplay/ai/combat.c#L218); cross: [`DepthInterval_BuildBinMask`](../../../../src/graphics/render/primitives.c#L689) → [`graphics/render/primitives`](../../graphics/render/primitives.md)
- `0x00537060` **[`AiCombatTarget_EvaluateCandidateScore`](../../../../src/gameplay/ai/combat.c#L218)** — cross: [`DepthBinMasks_OverlapCf`](../../../../src/graphics/render/primitives.c#L716) → [`graphics/render/primitives`](../../graphics/render/primitives.md), [`GameFactionRuntime_TestCapabilityBitClearCf`](../../../../src/gameplay/faction/runtime.c#L332) → [`gameplay/faction/runtime`](../faction/runtime.md), [`FixedMath_UInt64Sqrt`](../../../../src/core/math/fixed.c#L1243) → [`core/math/fixed`](../../core/math/fixed.md), [`ModelRuntime_QueryHierarchyScaleRatioQ12Regs`](../../../../src/world/model/runtime.c#L321) → [`world/model/runtime`](../../world/model/runtime.md), [`ArmyWeaponRuntime_TestTargetLineOfFireCf`](../../../../src/gameplay/army/combat.c#L443) → [`gameplay/army/combat`](../army/combat.md)

## Called by

- [`gameplay/ai/planning`](planning.md): [`AiRuntime_DispatchFactionPlanningPhase`](../../../../src/gameplay/ai/planning.c#L300) → [`AiUnitGroup_AssignCollectedEntitiesToBestTarget`](../../../../src/gameplay/ai/combat.c#L47)
- [`gameplay/army/runtime`](../army/runtime.md): [`ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers`](../../../../src/gameplay/army/runtime.c#L1410) → [`AiCombatDecision_UpdateTargetAssignment`](../../../../src/gameplay/ai/combat.c#L5); [`ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate`](../../../../src/gameplay/army/runtime.c#L3591) → [`AiCombatTarget_EvaluateCandidateScore`](../../../../src/gameplay/ai/combat.c#L218)
