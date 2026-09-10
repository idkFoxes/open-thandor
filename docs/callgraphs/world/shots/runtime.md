# Call graph: `world/shots/runtime`

7 functions. Addresses are current V523 executable VAs.

## Functions

- `0x0052CC60` **ShotRuntime_ApplyArmyHitRelationAndNotifications** — local: `ShotRuntime_PostImpactRelationNotificationNoOp`; cross: `ModelRuntime_QueryHierarchyScaleRatioQ12Regs` → `world/model/runtime`, `ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration` → `gameplay/army/movement`, `GameFactionRuntime_TestCapabilityBitClearCf` → `gameplay/faction/runtime`, `GameFactionRuntime_UpdateImpactAlertAnchorAndNotify` → `gameplay/faction/runtime`, `GameFactionRuntime_GetPackedStateNibble` → `gameplay/faction/runtime`, `GameFactionRuntime_ApplyPairwiseRelationTransition` → `gameplay/faction/runtime`
- `0x0052B540` **ShotRuntime_InitGraphicsResources** — cross: `WidePath_SetExtensionCode` → `core/text/path`, `MoviePlayback_AdvanceScheduledFrameAndTick` → `movie/runtime/playback`
- `0x0052B5C0` **ShotRuntime_ShutdownGraphicsResources** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x0052B660` **ShotRuntime_FindDefinitionByIdCf**
- `0x0052B750` **ShotRuntime_RebaseSlotsAfterLoad**
- `0x0052BDB0` **ShotRuntimePool_CreateProjectileFromDefinition** — cross: `WorldObjectArray_AllocateFreeRecordCf` → `world/runtime/core`, `WorldRuntime_LinkNodeIntoOwnerListD8` → `world/runtime/core`, `ShotDefinition_ComputeLaunchAnglesRegs` → `assets/shot/catalog`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`, `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `GraphicsShadingRuntime_AllocateRecordRegs` → `graphics/render/shading`, `TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint` → `world/terrain/occupancy`, `TerrainOccupancyMask_ResolveRuntimeClassFlags` → `world/terrain/occupancy`, `UiNode_GetStateTintArgb` → `ui/core/runtime`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `ModelNodeRuntime_UpdateDepthBinMasks` → `world/model/hierarchy`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`
- `0x00514710` **ShotRuntime_PostImpactRelationNotificationNoOp**
