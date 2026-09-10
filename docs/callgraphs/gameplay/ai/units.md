# Call graph: `gameplay/ai/units`

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053B0E0` **AiUnitBehavior_UpdateWorkspace01Entities** — local: `AiUnitBehavior_UpdateSpecialClass12Entity`, `AiUnitBehavior_SelectBestAnchorAction`
- `0x0053B4C0` **AiUnitBehavior_SelectBestAnchorAction** — local: `AiUnitBehavior_ComputeWorkspace05DistanceScore`, `AiUnitBehavior_ComputeFactionAnchorDistanceScore`, `AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore`, `AiUnitCommand_AssignWorkspacePoint`, `AiUnitCommand_AssignFactionAnchorPoint`, `AiUnitBehavior_CollectUnassignedEntity`
- `0x0053B1D0` **AiUnitBehavior_ComputeWorkspace05DistanceScore**
- `0x0053B260` **AiUnitBehavior_ComputeFactionAnchorDistanceScore**
- `0x0053B330` **AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore**
- `0x0053B3E0` **AiUnitCommand_AssignWorkspacePoint** — cross: `ArmyRuntime_QueueOrStartMoveCommandVariantA` → `gameplay/army/movement`
- `0x0053B420` **AiUnitCommand_AssignFactionAnchorPoint** — cross: `ArmyRuntime_QueueOrStartMoveCommandVariantA` → `gameplay/army/movement`
- `0x0053B480` **AiUnitBehavior_CollectUnassignedEntity**
- `0x0053B620` **AiUnitBehavior_UpdateSpecialClass12Entity** — cross: `ArmyRuntime_UpdateMovementAndWaypoints` → `gameplay/army/movement`, `FixedMath_SinCosScaled` → `core/math/fixed`, `ArmyRuntime_QueueOrStartMoveCommandVariantA` → `gameplay/army/movement`, `AiWorkspace03_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiWorkspace02_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiPlacement_QueryReachableSiteBucketCount` → `gameplay/ai/placement`, `AiPlacement_ReserveSeparatedSpecialSiteChain` → `gameplay/ai/placement`, `AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiPrimaryWorkspace_CountAssignedEntriesById` → `gameplay/ai/workspaces`, `ArmyRuntime_ResetMovementStateFromModel` → `gameplay/army/movement`
