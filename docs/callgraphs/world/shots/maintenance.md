# Call graph: `world/shots/maintenance`

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0052C080` **ShotModelRuntimeMaintenance_RefreshTerrainClassAndTint** — cross: `TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint` → `world/terrain/occupancy`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `TerrainOccupancyMask_ResolveRuntimeClassFlags` → `world/terrain/occupancy`, `UiModelControl_RefreshStateTint` → `ui/controls/misc`
- `0x0052C1A0` **ShotRuntimeMaintenance_OccupancyRebuildNoOp**
- `0x0052C1B0` **ShotRuntimeMaintenance_UpdateHierarchyProjectedSound** — cross: `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_UpdateDesiredPositionedGains` → `audio/spatial/runtime`
- `0x0052C230` **ShotModelRuntimeMaintenance_UpdateProjectileMotionCollisionAndEffects** — cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`, `ModelRuntime_RaycastCandidateListNearestCf` → `world/model/runtime`, `FieldGrid_RaycastTerrainSurfaceDistanceCf` → `world/terrain/grid`, `FieldGrid_RaycastSecondarySurfaceDistanceCf` → `world/terrain/grid`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `ShotRuntime_ApplyArmyHitRelationAndNotifications` → `world/shots/runtime`, `ArmyRuntime_ApplyImpactDamageToRuntimeAndParent` → `gameplay/army/combat`, `InterpolationState_SetNegatedTargetAndRescaleProgress` → `core/math/interpolation`, `WorldRuntime_UnlinkNodeFromOwnerListD8` → `world/runtime/core`, `FixedMath_VectorToAngles3Regs` → `core/math/fixed`, `FixedMath_VectorToAnglesAndLengthVec3Regs` → `core/math/fixed`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `ModelNodeRuntime_UpdateDepthBinMasks` → `world/model/hierarchy`
