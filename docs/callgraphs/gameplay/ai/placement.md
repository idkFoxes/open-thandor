# Call graph: `gameplay/ai/placement`

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053A110` **AiPlacement_ReserveAdditionalSpecialSite** — local: `AiPlacement_ReserveSeparatedSpecialSiteChain`; cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x0053AE60` **AiCandidatePlanning_AddSpecialSiteCandidate** — local: `AiCandidatePlanning_ComputeSpecialSiteWeight`; cross: `AiSecondaryWorkspace_HasEntryByIdCf` → `gameplay/ai/workspaces`, `AiPrimaryWorkspace_HasEntryByIdCf` → `gameplay/ai/workspaces`, `ArmyAssetRegistry_FindEnabledByIdCf` → `assets/army/catalog`, `AiCandidateWorkspace_AddOrAccumulateWeightedEntry` → `gameplay/ai/workspaces`
- `0x00537B20` **AiSiteCandidate_AddGeneralCellIfSeparated** — cross: `AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiWorkspace02_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`
- `0x00537C10` **AiSiteCandidate_AddFlaggedCellIfSeparated** — cross: `AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiWorkspace02_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`
- `0x00537CF0` **AiSiteCandidate_AddTerrainFeatureCellIfSeparated** — cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `FixedMath_Length2` → `core/math/fixed`
- `0x00539200` **AiPlacement_TestWorkspaceRecordAtPoint** — cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x0053A1B0` **AiPlacement_TestMode4AtWorkspaceRecord** — cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x0053B570` **AiPlacement_QueryReachableSiteBucketCount** — cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x0053ACD0` **AiPlacement_ReserveMode3SiteCluster** — local: `AiPlacement_ReserveSeparatedSpecialSiteChain`; cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x0053AD50` **AiCandidatePlanning_ComputeSpecialSiteWeight** — local: `AiPlacement_ReserveMode3SiteCluster`; cross: `AiWorkspace03_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiWorkspace02_GetMinimumManhattanDistanceToPoint` → `gameplay/ai/workspaces`, `AiPrimaryWorkspace_CountAssignedEntriesById` → `gameplay/ai/workspaces`
- `0x00539330` **AiPlacement_FindNearestValidWorkspace09Anchor** — cross: `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`
- `0x00539EF0` **AiPlacement_ReserveSeparatedSpecialSiteChain** — local: `AiPlacement_FindNearestValidWorkspace09Anchor`; cross: `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`
