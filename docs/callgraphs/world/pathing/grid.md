# Call graph: `world/pathing/grid`

27 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005349D0` **EntityPathing_ResolveDestinationAndRebuildRoutes** — local: `GridPathCost_FindNearestUnblockedCell`, `EntityPathing_RebuildOverlappingGroupRoutes`, `GridPathLine_TestHexSegmentClearCf`, `GridScratch_ResetTraversalFlagsAndCosts`, `GridPathCost_PropagateWeightedHexNeighbors`, `GridPathRegion_MarkUnreachableFromCell`, `GridPathCost_BacktrackBestHexRoute`
- `0x00536500` **GridReachability_RebuildConnectedRegionAroundWorldPoint** — local: `GridFootprint_ClearTraversalFlagsAroundWorldPoint`, `GridReachability_MarkOpenRegionRecursive`, `GridReachability_ClearCostedRegionRecursive`
- `0x00533620` **GridScratch_RebuildTerrainAndRuntimeClassificationMasks** — local: `GridScratch_FloodFillConnectedCellsRegs`
- `0x00533E70` **GridScratch_TestRuntimePairReachabilityFromWorldPointCf** — local: `GridScratch_TestWorldPointReachabilityCf`
- `0x005332C0` **GridScratch_AllocateForFieldGridCf**
- `0x00533360` **GridScratch_ReleaseBuffers**
- `0x00533400` **GridScratch_PropagateFieldOccupancyMaskNeighborhood**
- `0x00533BA0` **GridScratch_TestProjectedCellMaskBandsCf**
- `0x00536C90` **EntityPathing_RebuildOverlappingGroupRoutes** — local: `GridScratch_CopyPrimaryToSecondary`, `GridScratch_SwapPrimarySecondary`, `EntityPathing_UpdateRouteSegment`; cross: `DepthInterval_BuildBinMask` → `graphics/render/primitives`, `DepthBinMasks_OverlapCf` → `graphics/render/primitives`, `PriorityPairHeap_SiftUp` → `core/memory/allocator`, `PriorityPairHeap_SiftDown` → `core/memory/allocator`, `GridInfluence_SetLowDistanceBandsAroundWorldPoint` → `world/pathing/influence`
- `0x00534F50` **GridFootprint_ClearTraversalFlagsAroundWorldPoint** — local: `GridFootprint_ClearTraversalFlagsDiagonalPositive`, `GridFootprint_ClearTraversalFlagsDiagonalNegative`
- `0x005369A0` **EntityPathing_UpdateRouteSegment** — local: `GridPathCost_FindNearestUnblockedCell`, `GridPathLine_TestHexSegmentClearCf`; cross: `GridInfluence_SetLowDistanceBandsAroundWorldPoint` → `world/pathing/influence`, `ArmyRuntime_SetPendingMoveTarget` → `gameplay/army/movement`
- `0x00533D60` **GridScratch_TestWorldPointReachabilityCf** — local: `GridScratch_TestConnectedReachabilityRecursiveCfRegs`
- `0x00534660` **GridPathCost_BacktrackBestHexRoute** — local: `GridPathLine_TestHexSegmentClearCf`
- `0x00534960` **GridPathRegion_MarkUnreachableFromCell** — local: `GridPathRegion_MarkUnreachableRecursive`
- `0x005333B0` **GridScratch_CopyPrimaryToSecondary**
- `0x005333E0` **GridScratch_SwapPrimarySecondary**
- `0x00533580` **GridScratch_FloodFillConnectedCellsRegs**
- `0x00533C50` **GridScratch_TestConnectedReachabilityRecursiveCfRegs**
- `0x00533EF0` **GridPathCost_PropagateWeightedHexNeighbors**
- `0x00534200` **GridScratch_ResetTraversalFlagsAndCosts**
- `0x00534780` **GridPathRegion_MarkUnreachableRecursive**
- `0x00534E70` **GridFootprint_ClearTraversalFlagsDiagonalNegative**
- `0x00534EE0` **GridFootprint_ClearTraversalFlagsDiagonalPositive**
- `0x005363C0` **GridReachability_MarkOpenRegionRecursive**
- `0x00536440` **GridReachability_ClearCostedRegionRecursive**
- `0x005342F0` **GridPathCost_FindNearestUnblockedCell**
- `0x005344B0` **GridPathLine_TestHexSegmentClearCf**
