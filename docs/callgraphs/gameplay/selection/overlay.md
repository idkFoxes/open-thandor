# Call graph: `gameplay/selection/overlay`

16 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00568300` **InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf** — local: `InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint`; cross: `SelectionInfo_ValidateOwnerType16AndAnyActiveCf` → `gameplay/selection/runtime`, `ArmyRuntime_CreateInstanceFromAssetCf` → `gameplay/army/runtime`, `ModelNodeRuntime_RebuildTransformsFromRoot` → `world/model/hierarchy`, `ArmyRuntime_DestroyInstanceAndRefreshUi` → `gameplay/army/runtime`, `ArmyPlacement_ValidateAssetAtPointAndCellCornersCf` → `gameplay/army/placement`, `ArmyPlacement_DispatchAssetAtFieldPoint` → `gameplay/army/placement`, `EffectDefinitionRegistry_FindByIdWithErrorCf` → `assets/effect/catalog`, `FieldGrid_InterpolateTopSurfaceHeight` → `world/terrain/grid`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`, `InterpolationState_SetNegatedTargetAndRescaleProgress` → `core/math/interpolation`, `WorldRuntime_UnlinkNodeFromOwnerListD8` → `world/runtime/core`
- `0x0052F0C0` **SelectionOverlay_RenderSelectedArmyMetrics** — cross: `ModelProjectedBounds_AccumulateHierarchyRecursive` → `graphics/render/model`, `SelectionPanel_RenderArmyRuntimeMetrics` → `gameplay/selection/runtime`
- `0x0052F1B0` **SelectionOverlay_RenderArmyMetricsForEntity** — cross: `ModelProjectedBounds_AccumulateHierarchyRecursive` → `graphics/render/model`, `SelectionPanel_RenderArmyRuntimeMetrics` → `gameplay/selection/runtime`
- `0x0052F2A0` **SelectionOverlay_DrawBoundsFrame**
- `0x0052F490` **SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints** — cross: `FieldGrid_GetNearestTerrainPoint` → `world/terrain/grid`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x0052F5A0` **SelectionOverlay_DrawMarkerACForWorldSurfacePoint** — cross: `FieldGrid_GetNearestTerrainPoint` → `world/terrain/grid`, `FieldGrid_GetNearestTopSurfacePoint` → `world/terrain/grid`, `FixedTransform_ApplyPoint` → `core/math/fixed`, `Graphics_ProjectViewPoint` → `graphics/core/runtime`
- `0x0052F680` **SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices**
- `0x0052F780` **SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags**
- `0x0052F8C0` **SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800**
- `0x0052FA20` **SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000**
- `0x00560020` **SelectionMarkerCoordinates_ApplyType3** — cross: `SelectionPointerArray_ApplyType16MarkerCoordinates` → `gameplay/selection/runtime`
- `0x00560050` **SelectionMarkerCoordinates_ApplyType4** — cross: `SelectionPointerArray_ApplyType16MarkerCoordinates` → `gameplay/selection/runtime`
- `0x00560080` **SelectionMarkerCoordinates_ApplyType5** — cross: `SelectionPointerArray_ApplyType16MarkerCoordinates` → `gameplay/selection/runtime`
- `0x005600B0` **SelectionMarkerCoordinates_ApplyType6** — cross: `SelectionPointerArray_ApplyType16MarkerCoordinates` → `gameplay/selection/runtime`
- `0x005600E0` **SelectionMarkerCoordinates_ApplyType7** — cross: `SelectionPointerArray_ApplyType16MarkerCoordinates` → `gameplay/selection/runtime`
- `0x00568210` **InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint** — cross: `FieldGrid_InterpolateTopSurfaceHeight` → `world/terrain/grid`, `EffectRuntimePool_CreateInstanceFromDefinitionCf` → `world/effects/runtime`
