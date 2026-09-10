# Call graph: `world/effects/maintenance`

4 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051E790` **EffectRuntimeMaintenance_RefreshOccupancyFlagsAndTint** — cross: `TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint` → `world/terrain/occupancy`, `TerrainOccupancyMask_ResolveRuntimeClassFlags` → `world/terrain/occupancy`, `UiModelControl_RefreshStateTint` → `ui/controls/misc`
- `0x0051E830` **EffectRuntimeMaintenance_OccupancyRebuildNoOp**
- `0x0051E840` **EffectRuntimeMaintenance_AudioRefreshNoOp**
- `0x0051E850` **EffectModelRuntimeMaintenance_UpdateLifecycleTintScaleAndTransitions** — cross: `InterpolationState_SetNegatedTargetAndRescaleProgress` → `core/math/interpolation`, `WorldRuntime_UnlinkNodeFromOwnerListD8` → `world/runtime/core`, `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `GraphicsShadingRuntime_AllocateRecordRegs` → `graphics/render/shading`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`, `ShotRuntimePool_CreateProjectileFromDefinition` → `world/shots/runtime`, `ModelRuntimePool_DestroyHierarchyAndDetach` → `world/model/runtime`, `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `FieldGrid_InterpolateTerrainHeightAndNormal` → `world/terrain/grid`, `FixedVec3_DotQ28` → `core/math/fixed`, `FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf` → `world/terrain/grid`
