# Call graph: `gameplay/army/combat`

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00523980` **ArmyRuntimeWeapon_UpdateTargetAimAndFireAttachments** — local: `ArmyRuntime_EmitDamageThresholdEffect`; cross: `ArmyRuntime_ResolveShotAimPointCf` → `gameplay/army/runtime`, `ArmyRuntime_UpdateMovementAndWaypoints` → `gameplay/army/movement`, `ModelNodeRuntime_SmoothYawTowardTarget` → `world/model/hierarchy`, `ModelNodeRuntime_SmoothPitchTowardTarget` → `world/model/hierarchy`, `ShotDefinition_ComputeLaunchAnglesRegs` → `assets/shot/catalog`, `ModelNodeRuntime_ComputeRelativeDirectionAngle` → `world/model/hierarchy`, `ArmyRuntimeCommand_UpdateTargetFollowingState` → `gameplay/army/movement`, `ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf` → `gameplay/army/runtime`, `ArmyRuntime_SetNonzeroActionVector` → `gameplay/army/runtime`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`
- `0x00525130` **ArmyRuntimeClass_UpdateTransformAndDamageEffect** — local: `ArmyRuntime_EmitDamageThresholdEffect`; cross: `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`
- `0x00527AC0` **ArmyRuntimeClass_UpdateTimedEffectsModelsAndDamage** — local: `ArmyRuntime_EmitDamageThresholdEffect`; cross: `ArmyRuntime_UpdateTimedShotAndEffectEmitters` → `gameplay/army/runtime`, `ArmyRuntime_UpdateAnimatedModelSubnodes` → `gameplay/army/runtime`
- `0x0052A2E0` **ArmyRuntime_ApplyImpactDamageAndFinalizeState** — local: `ArmyRuntime_ApplyDamageAndPropagateToParent`
- `0x0052A3E0` **ArmyRuntime_ApplyDamageAndFactionRelationState** — local: `ArmyRuntime_ApplyDamageAndPropagateToParent`
- `0x0052A640` **ArmyRuntime_ApplyImpactDamageToRuntimeAndParent** — cross: `GameEntityRuntime_ApplyImpactDamageAndFactionRelationState` → `gameplay/faction/runtime`
- `0x0052B9D0` **ArmyWeaponRuntime_TestTargetLineOfFireCf** — cross: `FixedMath_Vector2AngleAndLengthRegs` → `core/math/fixed`, `FixedMath_UInt64Sqrt` → `core/math/fixed`, `FixedMath_Atan2Angle16` → `core/math/fixed`, `ModelRuntime_RaycastCandidateListNearestCf` → `world/model/runtime`, `FixedMath_VectorToAnglesAndLength3Regs` → `core/math/fixed`, `FieldGrid_RaycastTerrainSurfaceDistanceCf` → `world/terrain/grid`
- `0x0052A200` **ArmyRuntime_ApplyDamageAndPropagateToParent**
- `0x00528200` **ArmyRuntime_EmitDamageThresholdEffect** — cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`
