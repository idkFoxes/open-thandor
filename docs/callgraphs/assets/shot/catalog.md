# Call graph: `assets/shot/catalog`

8 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0052B4D0` **ShotAsset_PrepareEntries** — local: `ShotDefinition_RegisterAndResolveReferencesCf`; cross: `Package_SetLastErrorPath` → `assets/package/runtime`
- `0x0052B7E0` **ShotDefinitions_ValidateTerrainMaterialReferences**
- `0x0052B860` **ShotDefinitionRegistry_FindByIdWithErrorCf**
- `0x0052B8C0` **ShotDefinition_ComputeLaunchAnglesRegs** — cross: `FixedMath_Vector2AngleAndLengthRegs` → `core/math/fixed`, `FixedMath_UInt64Sqrt` → `core/math/fixed`, `FixedMath_Atan2Angle16` → `core/math/fixed`, `FixedMath_VectorToAngles3Regs` → `core/math/fixed`
- `0x0052BCE0` **ShotDefinition_ComputeSelectionRange**
- `0x0052BD50` **ShotDefinition_GetModeRangeLimitEbx**
- `0x0052BD80` **ShotDefinition_ComputeMode3LeadAdjustment**
- `0x0052B350` **ShotDefinition_RegisterAndResolveReferencesCf** — cross: `ShotRuntime_FindDefinitionByIdCf` → `world/shots/runtime`, `WidePath_SetExtensionCode` → `core/text/path`, `Package_LoadEntry` → `assets/package/runtime`, `SpriteAssetRegistry_FindById` → `assets/sprite/catalog`, `SpriteAsset_RegisterAndRelocatePointers` → `assets/sprite/catalog`, `Resource_Release` → `assets/resource/runtime`, `EffectDefinitionRegistry_FindByIdWithErrorCf` → `assets/effect/catalog`
