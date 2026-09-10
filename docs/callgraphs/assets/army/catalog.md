# Call graph: `assets/army/catalog`

23 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005719F0` **ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf`, `ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf`
- `0x00571A10` **ArmyAssetRegistry_StepForwardFlag0100Without0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf`, `ArmyAssetRegistry_HasIdWithoutFlag0200Cf`
- `0x00571A60` **ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf`, `ArmyAssetRegistry_HasIdWithoutFlag0200Cf`
- `0x00571B00` **ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf** — local: `ArmyAssetRegistry_HasIdWithoutFlag0200Cf`, `ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf`
- `0x00571C30` **ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlags0100And0200Cf`, `ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf`
- `0x00571C50` **ArmyAssetRegistry_StepForwardFlags0100And0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlags0100And0200Cf`, `ArmyAssetRegistry_HasIdWithFlag0200Cf`
- `0x00571CA0` **ArmyAssetRegistry_StepBackwardFlags0100And0200Cf** — local: `ArmyAssetRegistry_HasIdWithFlags0100And0200Cf`, `ArmyAssetRegistry_HasIdWithFlag0200Cf`
- `0x00571D40` **ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf** — local: `ArmyAssetRegistry_HasIdWithFlag0200Cf`, `ArmyAssetRegistry_HasIdWithFlags0100And0200Cf`
- `0x0051B5E0` **ArmyAsset_PrepareRecords** — local: `ArmyAssetRecord_RegisterAndRelocate`; cross: `Package_SetLastErrorPath` → `assets/package/runtime`
- `0x0051B740` **ArmyAssetRegistry_FindEnabledByIdCf** — local: `ArmyAssetRegistry_FindByIdCf`
- `0x0051B770` **ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf** — local: `ArmyAssetRegistry_FindByIdCf`; cross: `ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf` → `assets/model/definitions`
- `0x00571E40` **ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected** — local: `ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf`
- `0x0051C170` **ArmyAssetHierarchy_SumFactionUnlockedArmour** — cross: `ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf` → `assets/model/definitions`
- `0x0051C2C0` **ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4** — cross: `ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf` → `assets/model/definitions`
- `0x00571AB0` **ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf** — local: `ArmyAssetRegistry_HasIdWithoutFlag0200Cf`, `ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf`
- `0x00571CF0` **ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf** — local: `ArmyAssetRegistry_HasIdWithFlag0200Cf`, `ArmyAssetRegistry_HasIdWithFlags0100And0200Cf`
- `0x0051B4A0` **ArmyAssetRecord_RegisterAndRelocate** — local: `ArmyAssetRegistry_FindByIdCf`; cross: `ModelDefinitionRegistry_FindBuildMetricTupleByIdCf` → `assets/model/definitions`
- `0x00571D90` **ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf** — cross: `ArmyRuntime_RenderPreviewTextureCf` → `gameplay/army/runtime`
- `0x0051B6D0` **ArmyAssetRegistry_FindByIdCf**
- `0x00571910` **ArmyAssetRegistry_HasIdWithoutFlag0200Cf**
- `0x00571B50` **ArmyAssetRegistry_HasIdWithFlag0200Cf**
- `0x00571980` **ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf**
- `0x00571BC0` **ArmyAssetRegistry_HasIdWithFlags0100And0200Cf**
