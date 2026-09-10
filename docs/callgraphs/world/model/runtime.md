# Call graph: `world/model/runtime`

15 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00529360` **ModelRuntimePool_RepairDeferredChild** — local: `ModelRuntimePool_CreateInstanceByDefinitionIdCf`
- `0x004BDDB0` **ModelRuntime_CullAndRenderHierarchyRecursive** — cross: `FixedVec3_DotQ28` → `core/math/fixed`, `FixedMath_Length3` → `core/math/fixed`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `GraphicsShadingRuntime_CollectNearbyRecords` → `graphics/render/shading`, `ModelRender_DrawMeshGroupsWithTemporaryTransform` → `graphics/render/model`
- `0x004BE270` **ModelRuntime_RenderHierarchyRecursiveAlternatePath** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `FixedMath_Length3` → `core/math/fixed`, `GraphicsShadingRuntime_CollectNearbyRecords` → `graphics/render/shading`, `ModelRender_DrawMeshGroupsAlternatePath` → `graphics/render/model`
- `0x0050B440` **ModelRuntime_RaycastCandidateListNearestCf** — cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`, `FixedMath_WriteDirectionQ28` → `core/math/fixed`, `DepthBinMasks_OverlapCf` → `graphics/render/primitives`, `ModelNodeRuntime_RaycastHierarchyNearestCf` → `world/model/hierarchy`
- `0x0051C240` **ModelRuntime_QueryHierarchyScaleRatioQ12** — cross: `ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs` → `world/model/hierarchy`
- `0x0051C260` **ModelRuntime_QueryHierarchyScaleRatioQ12Regs** — cross: `ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs` → `world/model/hierarchy`
- `0x0051C280` **ModelRuntime_QueryActiveHierarchyMetric** — cross: `ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs` → `world/model/hierarchy`
- `0x0051C2A0` **ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs** — cross: `ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs` → `world/model/hierarchy`
- `0x00528A40` **ModelRuntimePool_Init**
- `0x00528A70` **ModelRuntimePool_ShutdownAndReleaseDefinitions** — cross: `Resource_Release` → `assets/resource/runtime`
- `0x00528B30` **ModelRuntimePool_UnrebaseBeforeSave**
- `0x00528CF0` **ModelRuntimePool_RebaseAfterLoad** — cross: `ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive` → `world/model/hierarchy`
- `0x00529560` **ModelRuntimePool_DestroyHierarchyAndDetach** — cross: `FrontendPlayerRuntime_ClearAssignmentTokenFromAll` → `ui/frontend/player`, `WorldRuntime_ForEachNodeInOwnerListD8` → `world/runtime/core`, `ModelRuntimeNode_ReleaseRecursiveAndDetachParent` → `world/model/hierarchy`, `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`, `ArmyRuntime_RebuildDerivedSelectionMetrics` → `gameplay/army/runtime`
- `0x00529690` **ModelRuntime_EmitProjectilesFromAttachmentPoints** — cross: `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `ShotRuntimePool_CreateProjectileFromDefinition` → `world/shots/runtime`
- `0x00529140` **ModelRuntimePool_CreateInstanceByDefinitionIdCf** — cross: `ModelNodeRuntime_CreateHierarchyRecursiveCf` → `world/model/hierarchy`, `ModelNodeRuntime_RecomputeSubtreeBoundingRadius` → `world/model/hierarchy`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`
