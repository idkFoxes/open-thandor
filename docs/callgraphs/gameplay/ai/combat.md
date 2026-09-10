# Call graph: `gameplay/ai/combat`

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00536FC0` **AiCombatDecision_UpdateTargetAssignment** — local: `AiCombatTarget_SelectBestCandidate`; cross: `ArmyRuntime_ResolveCommandTarget` → `gameplay/army/runtime`
- `0x0053B8B0` **AiUnitGroup_AssignCollectedEntitiesToBestTarget** — cross: `ModelRuntime_QueryHierarchyScaleRatioQ12Regs` → `world/model/runtime`, `ArmyRuntime_ResolveCommandTargetAndRoute` → `gameplay/army/movement`
- `0x005372C0` **AiCombatTarget_SelectBestCandidate** — local: `AiCombatTarget_EvaluateCandidateScore`; cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`
- `0x00537060` **AiCombatTarget_EvaluateCandidateScore** — cross: `DepthBinMasks_OverlapCf` → `graphics/render/primitives`, `GameFactionRuntime_TestCapabilityBitClearCf` → `gameplay/faction/runtime`, `FixedMath_UInt64Sqrt` → `core/math/fixed`, `ModelRuntime_QueryHierarchyScaleRatioQ12Regs` → `world/model/runtime`, `ArmyWeaponRuntime_TestTargetLineOfFireCf` → `gameplay/army/combat`
