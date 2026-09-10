# Call graph: `gameplay/ai/units`

[Source](../../../../src/gameplay/ai/units.c) · [Header](../../../../include/thandor/gameplay/ai/units.h) · [Graphviz](units.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-gameplay-ai-units)

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053B0E0` **[`AiUnitBehavior_UpdateWorkspace01Entities`](../../../../src/gameplay/ai/units.c#L5)** — local: [`AiUnitBehavior_UpdateSpecialClass12Entity`](../../../../src/gameplay/ai/units.c#L378), [`AiUnitBehavior_SelectBestAnchorAction`](../../../../src/gameplay/ai/units.c#L74)
- `0x0053B4C0` **[`AiUnitBehavior_SelectBestAnchorAction`](../../../../src/gameplay/ai/units.c#L74)** — local: [`AiUnitBehavior_ComputeWorkspace05DistanceScore`](../../../../src/gameplay/ai/units.c#L134), [`AiUnitBehavior_ComputeFactionAnchorDistanceScore`](../../../../src/gameplay/ai/units.c#L181), [`AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore`](../../../../src/gameplay/ai/units.c#L250), [`AiUnitCommand_AssignWorkspacePoint`](../../../../src/gameplay/ai/units.c#L309), [`AiUnitCommand_AssignFactionAnchorPoint`](../../../../src/gameplay/ai/units.c#L328), [`AiUnitBehavior_CollectUnassignedEntity`](../../../../src/gameplay/ai/units.c#L357)
- `0x0053B1D0` **[`AiUnitBehavior_ComputeWorkspace05DistanceScore`](../../../../src/gameplay/ai/units.c#L134)**
- `0x0053B260` **[`AiUnitBehavior_ComputeFactionAnchorDistanceScore`](../../../../src/gameplay/ai/units.c#L181)**
- `0x0053B330` **[`AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore`](../../../../src/gameplay/ai/units.c#L250)**
- `0x0053B3E0` **[`AiUnitCommand_AssignWorkspacePoint`](../../../../src/gameplay/ai/units.c#L309)** — cross: [`ArmyRuntime_QueueOrStartMoveCommandVariantA`](../../../../src/gameplay/army/movement.c#L1636) → [`gameplay/army/movement`](../army/movement.md)
- `0x0053B420` **[`AiUnitCommand_AssignFactionAnchorPoint`](../../../../src/gameplay/ai/units.c#L328)** — cross: [`ArmyRuntime_QueueOrStartMoveCommandVariantA`](../../../../src/gameplay/army/movement.c#L1636) → [`gameplay/army/movement`](../army/movement.md)
- `0x0053B480` **[`AiUnitBehavior_CollectUnassignedEntity`](../../../../src/gameplay/ai/units.c#L357)**
- `0x0053B620` **[`AiUnitBehavior_UpdateSpecialClass12Entity`](../../../../src/gameplay/ai/units.c#L378)** — cross: [`ArmyRuntime_UpdateMovementAndWaypoints`](../../../../src/gameplay/army/movement.c#L3431) → [`gameplay/army/movement`](../army/movement.md), [`FixedMath_SinCosScaled`](../../../../src/core/math/fixed.c#L446) → [`core/math/fixed`](../../core/math/fixed.md), [`ArmyRuntime_QueueOrStartMoveCommandVariantA`](../../../../src/gameplay/army/movement.c#L1636) → [`gameplay/army/movement`](../army/movement.md), [`AiWorkspace03_GetMinimumManhattanDistanceToPoint`](../../../../src/gameplay/ai/workspaces.c#L950) → [`gameplay/ai/workspaces`](workspaces.md), [`AiWorkspace02_GetMinimumManhattanDistanceToPoint`](../../../../src/gameplay/ai/workspaces.c#L910) → [`gameplay/ai/workspaces`](workspaces.md), [`AiPlacement_QueryReachableSiteBucketCount`](../../../../src/gameplay/ai/placement.c#L459) → [`gameplay/ai/placement`](placement.md), [`AiPlacement_ReserveSeparatedSpecialSiteChain`](../../../../src/gameplay/ai/placement.c#L703) → [`gameplay/ai/placement`](placement.md), [`AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint`](../../../../src/gameplay/ai/workspaces.c#L829) → [`gameplay/ai/workspaces`](workspaces.md), [`AiPrimaryWorkspace_CountAssignedEntriesById`](../../../../src/gameplay/ai/workspaces.c#L1309) → [`gameplay/ai/workspaces`](workspaces.md), [`ArmyRuntime_ResetMovementStateFromModel`](../../../../src/gameplay/army/movement.c#L1388) → [`gameplay/army/movement`](../army/movement.md)

## Called by

- [`gameplay/ai/planning`](planning.md): [`AiRuntime_DispatchFactionPlanningPhase`](../../../../src/gameplay/ai/planning.c#L300) → [`AiUnitBehavior_UpdateWorkspace01Entities`](../../../../src/gameplay/ai/units.c#L5)
- [`gameplay/army/movement`](../army/movement.md): [`ArmyRuntimeClass_UpdateSpecialBehaviorAndGroundMovement`](../../../../src/gameplay/army/movement.c#L366) → [`AiUnitBehavior_UpdateSpecialClass12Entity`](../../../../src/gameplay/ai/units.c#L378)
