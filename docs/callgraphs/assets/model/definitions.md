# Call graph: `assets/model/definitions`

14 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051B3C0` **ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf** — local: `ModelDefinition_IsFactionTechnologyUnlockedCf`, `ModelDefinitionRegistry_FindByIdWithErrorCf`
- `0x0051DB00` **ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology** — local: `ModelDefinition_SelectFactionUnlockedLinkedIdCf`, `ModelDefinition_UnlockLinkedTechnologyForFactionCf`
- `0x0051DA60` **ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf** — local: `ModelDefinition_IsFactionTechnologyUnlockedCf`
- `0x00528950` **ModelAsset_PrepareRecords** — local: `ModelDefinition_RegisterAndResolveReferencesCf`
- `0x004BE670` **ModelLookupTable_FindPackedKeyEntryRegsCf**
- `0x004BE6F0` **ModelLookupTable_ContainsPackedKeyCf**
- `0x0050AEA0` **ModelMesh_IntersectTriangleRayDistanceCf**
- `0x005289C0` **ModelDefinitionRegistry_FindBuildMetricTupleByIdCf**
- `0x0053BA00` **ModelDefinitionRegistry_FindByRuntimeClassId**
- `0x0051B430` **ModelDefinition_SelectFactionUnlockedLinkedIdCf** — local: `ModelDefinition_IsFactionTechnologyUnlockedCf`
- `0x00528600` **ModelDefinition_RegisterAndResolveReferencesCf** — local: `ModelDefinitionRegistry_FindByIdWithErrorCf`; cross: `WidePath_SetExtensionCode` → `core/text/path`, `Package_LoadEntry` → `assets/package/runtime`, `SpriteAssetRegistry_FindById` → `assets/sprite/catalog`, `SpriteAsset_RegisterAndRelocatePointers` → `assets/sprite/catalog`, `Resource_Release` → `assets/resource/runtime`, `ShotDefinitionRegistry_FindByIdWithErrorCf` → `assets/shot/catalog`, `EffectDefinitionRegistry_FindByIdWithErrorCf` → `assets/effect/catalog`
- `0x0052ADE0` **ModelDefinition_UnlockLinkedTechnologyForFactionCf** — local: `ModelDefinitionRegistry_FindByIdWithErrorCf`; cross: `Technology_UnlockForFaction` → `gameplay/technology/runtime`
- `0x0052AD90` **ModelDefinition_IsFactionTechnologyUnlockedCf** — local: `ModelDefinitionRegistry_FindByIdWithErrorCf`
- `0x00528E20` **ModelDefinitionRegistry_FindByIdWithErrorCf**
