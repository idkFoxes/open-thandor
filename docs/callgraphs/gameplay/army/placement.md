# Call graph: `gameplay/army/placement`

25 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005244B0` **ArmyPlacementCandidate_TestOffsetClearanceCf** — local: `ArmyPlacementCollision_TestCandidateAndClearanceCf`, `ArmyPlacementCollision_TestPointAgainstRuntimeListCf`; cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `FixedMath_Vector2AngleAndLengthRegs` → `core/math/fixed`, `FixedMath_SinCosScaled` → `core/math/fixed`
- `0x00524570` **ArmyPlacement_TestModelTerrainAndRuntimeClearance** — local: `ArmyPlacementCollision_TestCurrentRuntimeCf`, `ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf`; cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `TerrainAuxHeightThreshold_TestAroundWorldPoint` → `world/terrain/height`, `TerrainHeightBand_TestAroundWorldPoint` → `world/terrain/height`
- `0x0051D380` **ArmyPlacement_ValidateAssetAtPointAndCellCornersCf** — local: `ArmyPlacement_DispatchAssetAtFieldPoint`
- `0x00524EB0` **ArmyPlacementCandidate_TestFieldOccupancyCf** — local: `ArmyPlacementCollision_TestCandidateAndClearanceCf`
- `0x00524F70` **ArmyPlacement_TestGridOccupancyMask** — local: `ArmyPlacementCollision_TestCurrentRuntimeCf`; cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00528110` **ArmyPlacement_TestGridRuntimeAndFieldBlocking** — local: `ArmyCollision_FindBlockingRuntimeForCurrentUnitCf`; cross: `GridScratch_TestProjectedCellMaskBandsCf` → `world/pathing/grid`, `FieldGrid_TestWorldPointBlockedCf` → `world/terrain/grid`
- `0x005281A0` **ArmyRuntimeCollision_TestShotSpawnPointCf** — local: `ArmyCollision_TestPointAgainstRuntimeListCf`; cross: `GridScratch_TestProjectedCellMaskBandsCf` → `world/pathing/grid`, `FieldGrid_TestWorldPointBlockedCf` → `world/terrain/grid`
- `0x004BE7F0` **ArmyPlacementContact_ApplyTerrainHeight** — cross: `FieldGrid_InterpolateTerrainHeight` → `world/terrain/grid`
- `0x004BE860` **ArmyPlacementContact_ApplyWaterSurfaceHeight** — cross: `FieldGrid_InterpolateWaterSurfaceHeight` → `world/terrain/grid`
- `0x004BE8C0` **ArmyPlacementContact_ApplyTerrainHeightAndNormal** — cross: `FieldGrid_InterpolateTerrainHeightAndNormal` → `world/terrain/grid`
- `0x004BE930` **ArmyPlacementContact_ApplyTopSurfaceHeight** — cross: `FieldGrid_InterpolateTopSurfaceHeight` → `world/terrain/grid`
- `0x004BE990` **ArmyPlacementContact_InitializeArticulatedSuspension** — cross: `ArmyArticulatedRuntime_InitializeTerrainContactGeometry` → `gameplay/army/movement`, `ArmyArticulatedRuntime_UpdateSuspensionHierarchy` → `gameplay/army/movement`
- `0x00525320` **ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation** — cross: `FieldGrid_WorldToGridQ12` → `world/terrain/grid`
- `0x00525420` **ArmyPlacement_ReleaseFactionCapacity**
- `0x005263E0` **ArmyPlacement_ReleaseClassStateReservation**
- `0x00527BD0` **ArmyPlacementAssetClassDispatch_AlwaysSuccessCf**
- `0x00529CB0` **ArmyCollision_TestPointAgainstRuntimeListCf** — local: `ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf`; cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`, `DepthBinMasks_OverlapCf` → `graphics/render/primitives`
- `0x00529E60` **ArmyCollision_FindBlockingRuntimeForCurrentUnitCf** — local: `ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf`; cross: `DepthBinMasks_OverlapCf` → `graphics/render/primitives`
- `0x0051D450` **ArmyPlacement_DispatchAssetAtFieldPoint** — cross: `ArmyAssetRegistry_FindByIdCf` → `assets/army/catalog`, `ModelDefinitionRegistry_FindByIdWithErrorCf` → `assets/model/definitions`
- `0x00529D70` **ArmyPlacementCollision_TestPointAgainstRuntimeListCf** — local: `ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf`, `ArmyPlacementCandidate_TestModelAnchorDistanceCf`; cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`, `DepthBinMasks_OverlapCf` → `graphics/render/primitives`
- `0x00529F30` **ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf** — local: `ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf`, `ArmyPlacementCandidate_TestModelAnchorDistanceCf`; cross: `DepthBinMasks_OverlapCf` → `graphics/render/primitives`
- `0x00527740` **ArmyPlacementCollision_TestCurrentRuntimeCf** — local: `ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf`; cross: `FieldGrid_TestWorldPointBlockedCf` → `world/terrain/grid`
- `0x005278D0` **ArmyPlacementCollision_TestCandidateAndClearanceCf** — local: `ArmyPlacementCollision_TestPointAgainstRuntimeListCf`; cross: `FieldGrid_TestWorldPointBlockedCf` → `world/terrain/grid`, `FixedMath_UInt64Sqrt` → `core/math/fixed`
- `0x00524650` **ArmyPlacementCandidate_TestModelAnchorDistanceCf** — cross: `ModelLookupTable_ContainsPackedKeyCf` → `assets/model/definitions`, `ModelNodeRuntime_TransformLocalPointRegs` → `world/model/hierarchy`, `FixedMath_Length2` → `core/math/fixed`
- `0x00529C40` **ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf**
