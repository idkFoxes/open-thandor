# Call graph: `world/model/hierarchy`

28 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004BD1F0` **ModelNodeRuntime_UpdateStateTintRecursive** — local: `ModelNodeRuntime_ApplyTintRecursive`
- `0x004BE360` **ModelNodeRuntime_RebuildTransformsFromRoot** — local: `ModelNodeRuntime_ComposeChildTransformsRecursive`
- `0x0051DB80` **ModelRuntimeHierarchy_ApplyFactionTechnologyVariants** — local: `ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive`
- `0x004BD310` **ModelNodeRuntime_AccumulateTransformedBoundsRecursive** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`
- `0x004BD8D0` **ModelNodeRuntime_BuildViewFacingRotation** — cross: `FixedTransform_ApplyEulerRotationToVectorRegs` → `core/math/fixed`, `FixedMath_Atan2Angle16` → `core/math/fixed`, `FixedTransform_BuildRotationBasis` → `core/math/fixed`
- `0x004BD950` **ModelNodeRuntime_BuildBillboardRotation** — cross: `FixedMath_VectorToAngles3Regs` → `core/math/fixed`, `FixedTransform_BuildRotationBasis` → `core/math/fixed`
- `0x004BE9D0` **ModelNodeRuntime_RecomputeSubtreeBoundingRadius** — cross: `FixedMath_LengthVec3` → `core/math/fixed`
- `0x004BEA30` **ModelNodeRuntime_UpdateDepthBinMasks** — cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`
- `0x004BEB80` **ModelNodeRuntime_TransformLocalPointRegs** — cross: `FixedTransform_ApplyPoint` → `core/math/fixed`
- `0x004BEBC0` **ModelNodeRuntime_ComputeRelativeDirectionAngle** — cross: `FixedTransform_RotateDirectionScaledRegs` → `core/math/fixed`, `FixedMath_VectorToAngles3Regs` → `core/math/fixed`
- `0x0050A7A0` **ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf** — cross: `FixedTransform_Compose` → `core/math/fixed`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`, `GraphicsProjectedPoint_IsInsideTriangleCf` → `graphics/render/projection`, `FixedMath_Length3` → `core/math/fixed`
- `0x0050B1D0` **ModelNodeRuntime_RaycastHierarchyNearestCf** — cross: `FixedTransform_BuildRotationBasis` → `core/math/fixed`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `ModelMesh_IntersectTriangleRayDistanceCf` → `assets/model/definitions`
- `0x0051B650` **ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf** — cross: `ModelDefinition_SelectFactionUnlockedLinkedIdCf` → `assets/model/definitions`, `ModelRuntimePool_RepairDeferredChild` → `world/model/runtime`
- `0x0051BEC0` **ModelRuntimeHierarchy_SetCommandTargetRecursive**
- `0x0051BF30` **ModelRuntimeHierarchy_ClearMatchingTargetRecursive**
- `0x0051C100` **ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive**
- `0x0051C1F0` **ModelRuntimeHierarchy_SumMetric3C**
- `0x00528C20` **ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive**
- `0x00528E90` **ModelNodeRuntime_CreateHierarchyRecursiveCf** — cross: `WorldObjectArray_AllocateFreeRecordCf` → `world/runtime/core`
- `0x005294E0` **ModelRuntimeNode_ReleaseRecursiveAndDetachParent** — cross: `WorldRuntime_UnlinkNodeFromOwnerListD8` → `world/runtime/core`
- `0x0052A100` **ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics** — cross: `ShotDefinition_ComputeSelectionRange` → `assets/shot/catalog`
- `0x0052A690` **ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs**
- `0x0052A6F0` **ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs**
- `0x0052AAC0` **ModelNodeRuntime_SmoothYawTowardTarget**
- `0x0052AC00` **ModelNodeRuntime_SmoothPitchTowardTarget**
- `0x004BD1A0` **ModelNodeRuntime_ApplyTintRecursive**
- `0x004BE390` **ModelNodeRuntime_ComposeChildTransformsRecursive** — cross: `FixedTransform_BuildRotationBasis` → `core/math/fixed`, `FixedTransform_Compose` → `core/math/fixed`, `FixedTransform_ExtractEulerAnglesRegs` → `core/math/fixed`
- `0x0052AEA0` **ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive** — cross: `ModelDefinition_IsFactionTechnologyUnlockedCf` → `assets/model/definitions`, `ModelDefinitionRegistry_FindByIdWithErrorCf` → `assets/model/definitions`, `ArmyRuntime_RebuildDerivedSelectionMetrics` → `gameplay/army/runtime`
