# Call graph: `gameplay/technology/runtime`

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005139C0` **Technology_UnlockForFaction** — cross: `InGameNotificationQueue_InsertPriorityRecord` → `ui/ingame/runtime`, `ModelRuntimeHierarchy_ApplyFactionTechnologyVariants` → `world/model/hierarchy`, `UiCatalogGroup48_RebuildGrid` → `ui/ingame/technology`, `UiCatalogGroup42_RebuildGrid` → `ui/ingame/technology`
- `0x00513AE0` **Technology_IsUnlockedForFactionCf**
- `0x00513B20` **Technology_IsAvailableForFactionCf**
- `0x0052AE10` **Technology_ApplyRecordToEntity**
- `0x00539BB0` **TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks** — cross: `ModelDefinitionRegistry_FindByIdWithErrorCf` → `assets/model/definitions`
