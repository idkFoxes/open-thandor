# Call graph: `world/runtime/core`

50 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00532FA0` **WorldLightingRuntime_UpdateInterpolatedTerrainLighting** — local: `WorldRuntime_SetTerrainLightingConfiguration`, `WorldRuntime_RecomputeFieldRegionNormalsAndLighting`
- `0x0050D100` **WorldRuntime_SetPosition60AndDistanceFromPosition80** — local: `WorldRuntime_ClearFieldGridDirtyFlag`; cross: `FixedMath_Length3` → `core/math/fixed`
- `0x0050D150` **WorldRuntime_SetMotionParameters6CThrough78Clamped** — local: `WorldRuntime_ClearFieldGridDirtyFlag`
- `0x0050D1E0` **WorldRuntime_SetPosition80AndRebuildPosition60FromAngles** — local: `WorldRuntime_ClearFieldGridDirtyFlag`; cross: `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`
- `0x0050D2C0` **WorldRuntime_RestoreMotionStateFromSnapshot** — local: `WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface`
- `0x0050D670` **WorldRuntime_AttachFieldGridAsset** — local: `WorldRuntime_ClearFieldGridDirtyFlag`; cross: `FieldGrid_RecomputeInteriorTriangleNormalAngles` → `world/terrain/grid`
- `0x00561E30` **WorldRuntime_AdjustFieldOriginWrappedClamped** — local: `WorldRuntime_RecomputeFieldRegionNormalsAndLighting`
- `0x004BE760` **WorldRuntime_InterpolateTerrainHeightOrSentinel** — cross: `FieldGrid_InterpolateTerrainHeight` → `world/terrain/grid`
- `0x004BE790` **WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel** — cross: `FieldGrid_InterpolateWaterSurfaceHeight` → `world/terrain/grid`
- `0x004BE7C0` **WorldRuntime_InterpolateTopSurfaceHeightOrSentinel** — cross: `FieldGrid_InterpolateTopSurfaceHeight` → `world/terrain/grid`
- `0x0050A610` **WorldRuntimeNode_IsPositionInsideBoundsCf** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x0050D260` **WorldRuntime_CaptureMotionStateToSnapshot**
- `0x0050D330` **WorldRuntime_MotionStateMatchesSnapshotCf**
- `0x0050D4F0` **WorldRuntime_CommitScalar7CFrom8C**
- `0x0050D510` **WorldRuntime_AttachObjectArray**
- `0x0050D540` **WorldRuntime_SetFlags**
- `0x0050D560` **WorldRuntime_AddFlags**
- `0x0050D580` **WorldRuntime_ClearFlags**
- `0x0050D5A0` **WorldRuntime_ToggleFlags**
- `0x0050D610` **WorldRuntime_GetVector0Regs**
- `0x0050D630` **WorldRuntime_GetVector1Regs**
- `0x0050D650` **WorldRuntime_GetFlagsCf**
- `0x0050D6A0` **WorldRuntime_GetFieldGridAsset**
- `0x0050D6D0` **WorldRuntime_GetPendingToken**
- `0x0050D6F0` **WorldRuntime_TakePendingToken**
- `0x0050D710` **WorldRuntime_AttachAndClearDwordArray**
- `0x0050D740` **WorldRuntime_GetDwordArray**
- `0x0050D7D0` **WorldObjectArray_AllocateFreeRecordCf**
- `0x0050D830` **WorldRuntime_LinkNodeIntoOwnerListD8**
- `0x0050D880` **WorldRuntime_UnlinkNodeFromOwnerListD8**
- `0x0050D8F0` **WorldRuntime_ForEachNodeInOwnerListD8**
- `0x0050EC80` **RuntimeHexSegment_GetLightImageAndToggleFlagRegs**
- `0x0050ECA0` **RuntimeHexSegment_ToggleLightImageFlag**
- `0x0050ECB0` **RuntimeHexSegment_GetFieldImageRegs**
- `0x0050ECD0` **RuntimeHexSegment_AfterFieldImageNoOp**
- `0x0051BFA0` **WorldRuntimeNode_ClearOwnedModelReferencesCallback** — cross: `ModelRuntimeHierarchy_ClearMatchingTargetRecursive` → `world/model/hierarchy`
- `0x0051D500` **WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries** — cross: `ModelDefinitionRegistry_FindByIdWithErrorCf` → `assets/model/definitions`
- `0x005233F0` **UnifiedRuntimeTable_Method5_TwoArgNoOp**
- `0x00523400` **UnifiedRuntimeTable_Method6_TwoArgNoOp**
- `0x00527B70` **UnifiedRuntimeDefault_OneArgNoOpC**
- `0x00527BA0` **UnifiedRuntimeDefault_TwoArgNoOpB**
- `0x00527BB0` **UnifiedRuntimeDefault_OneArgReturnZero**
- `0x00527BE0` **UnifiedRuntimeDefault_TwoArgSuccessCf**
- `0x00527BF0` **UnifiedRuntimeDefault_TwoArgNoOpD**
- `0x00529430` **WorldRuntimeNode_ClearDetachedEntityReferencesCallback**
- `0x00565110` **WorldRuntimeNode_ReleaseShutdownBindingsCallback** — cross: `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`
- `0x0050D3B0` **WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface** — local: `WorldRuntime_ClearFieldGridDirtyFlag`; cross: `FieldGrid_RaycastTerrainSurfaceDistanceCf` → `world/terrain/grid`, `FixedMath_SinCosScaled` → `core/math/fixed`, `FixedMath_Length3` → `core/math/fixed`, `FieldGrid_RaycastSecondarySurfaceDistanceCf` → `world/terrain/grid`, `FixedMath_DirectionFromAnglesScaledRegs` → `core/math/fixed`
- `0x0050D760` **WorldRuntime_SetTerrainLightingConfiguration** — cross: `TerrainLighting_BuildColorRampAndSetBaseColor` → `world/terrain/visuals`
- `0x0050D5C0` **WorldRuntime_RecomputeFieldRegionNormalsAndLighting** — cross: `FieldGrid_RecomputeInteriorTriangleNormalAngles` → `world/terrain/grid`, `FieldGrid_RecomputeInteriorDirectionalLighting` → `world/terrain/grid`
- `0x0050D6B0` **WorldRuntime_ClearFieldGridDirtyFlag**
