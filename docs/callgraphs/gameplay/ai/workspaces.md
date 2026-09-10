# Call graph: `gameplay/ai/workspaces`

26 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0053A1E0` **AiWorkspaceAssetCandidate_AddWeightedEntry** — local: `AiPrimaryWorkspace_HasUnassignedEntryByIdCf`, `AiPrimaryWorkspace_CountAssignedEntriesById`, `AiCandidateWorkspace_AddOrAccumulateWeightedEntry`; cross: `AiPlacement_TestMode4AtWorkspaceRecord` → `gameplay/ai/placement`
- `0x00538230` **AiPlanning_RebuildFactionWorkspaces** — local: `AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents`, `AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents`; cross: `GameFactionRuntime_TestCapabilityBitClearCf` → `gameplay/faction/runtime`, `AiPlanning_CollectActiveGridMaskClasses` → `gameplay/ai/planning`, `AiSiteCandidate_AddGeneralCellIfSeparated` → `gameplay/ai/placement`, `AiSiteCandidate_AddFlaggedCellIfSeparated` → `gameplay/ai/placement`, `AiSiteCandidate_AddTerrainFeatureCellIfSeparated` → `gameplay/ai/placement`, `ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf` → `assets/model/definitions`, `AiTechnologyCandidate_IsCurrentlyAvailableCf` → `gameplay/ai/technology`, `AiTechnologyPlanning_AddCandidateRecord` → `gameplay/ai/technology`
- `0x0053BF30` **AiStrategicCandidate_AddBestWorkspace12Entry** — local: `AiPrimaryWorkspace_HasEntryByIdCf`, `AiCandidateWorkspace_AddOrAccumulateWeightedEntry`
- `0x00537420` **AiCandidateWorkspace_Clear**
- `0x00537430` **AiCandidateWorkspace_SaveToFactionImage**
- `0x00537470` **AiCandidateWorkspace_LoadFromFactionImage**
- `0x00537570` **AiCandidateWorkspace_SortDescending**
- `0x005375D0` **AiCandidateWorkspace_GetEntryEntityValue** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`
- `0x00538C90` **AiSecondaryWorkspace_HasUnassignedEntryByIdCf**
- `0x00538CF0` **AiSecondaryWorkspace_HasEntryByIdCf**
- `0x00538D40` **AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate**
- `0x00538D90` **AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint**
- `0x00538E00` **AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint**
- `0x00538E80` **AiWorkspace02_GetMinimumManhattanDistanceToPoint**
- `0x00538EF0` **AiWorkspace03_GetMinimumManhattanDistanceToPoint**
- `0x00538F60` **AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint**
- `0x00539240` **AiConstructionPlanner_PlaceSpecialAssetFromWorkspace** — cross: `AiPlacement_TestWorkspaceRecordAtPoint` → `gameplay/ai/placement`, `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `ArmyRuntime_DispatchClassCommand` → `gameplay/army/runtime`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`, `AiConstructionPlanner_ConsumeFactionPendingArmyAsset` → `gameplay/ai/planning`
- `0x0053BCB0` **AiWorkspace12Score_DefaultZero**
- `0x0053C6C0` **AiRuntime_InitWorkspace** — cross: `Package_LoadEntry` → `assets/package/runtime`
- `0x00537F80` **AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents** — local: `AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf`
- `0x00537FC0` **AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents** — local: `AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf`
- `0x00538B90` **AiPrimaryWorkspace_HasUnassignedEntryByIdCf**
- `0x00538BF0` **AiPrimaryWorkspace_HasEntryByIdCf**
- `0x00538C40` **AiPrimaryWorkspace_CountAssignedEntriesById**
- `0x005374B0` **AiCandidateWorkspace_AddOrAccumulateWeightedEntry**
- `0x00538FD0` **AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf**
