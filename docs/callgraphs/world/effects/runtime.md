# Call graph: `world/effects/runtime`

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0051E120` **EffectRuntime_FindDefinitionByIdCf**
- `0x0051E190` **EffectRuntime_InitGraphicsResources** — cross: `WidePath_SetExtensionCode` → `core/text/path`, `MoviePlayback_AdvanceScheduledFrameAndTick` → `movie/runtime/playback`
- `0x0051E210` **EffectRuntime_ShutdownGraphicsResources** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x0051E340` **EffectRuntime_RebaseSlotsAfterLoad**
- `0x0051E4A0` **EffectRuntimePool_CreateInstanceFromDefinitionCf** — cross: `WorldObjectArray_AllocateFreeRecordCf` → `world/runtime/core`, `WorldRuntime_LinkNodeIntoOwnerListD8` → `world/runtime/core`, `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `GraphicsShadingRuntime_AllocateRecordRegs` → `graphics/render/shading`, `TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint` → `world/terrain/occupancy`, `TerrainOccupancyMask_ResolveRuntimeClassFlags` → `world/terrain/occupancy`, `TerrainGrid_TestProjectedCellMaskBits01Cf` → `world/terrain/grid`, `SpatialSound_PlayPositionedOneShot` → `audio/spatial/runtime`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`
