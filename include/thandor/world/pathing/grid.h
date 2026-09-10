#ifndef THANDOR_WORLD_PATHING_GRID_H
#define THANDOR_WORLD_PATHING_GRID_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/pathing/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005349D0 */
EntityPathingDestinationRegisterResult EntityPathing_ResolveDestinationAndRebuildRoutes (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime, WorldRuntimeContext *worldRuntime);

/* 0x00536500 */
undefined8 GridReachability_RebuildConnectedRegionAroundWorldPoint (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00533620 */
void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime);

/* 0x00533E70 */
void GridScratch_TestRuntimePairReachabilityFromWorldPointCf (WorldPointXYQ12 *sourceWorldPoint,undefined4 *targetRuntimePair);

/* 0x005332C0 */
void GridScratch_AllocateForFieldGridCf(FieldGridAsset *fieldGrid);

/* 0x00533360 */
undefined4 __cdecl GridScratch_ReleaseBuffers(void);

/* 0x00533400 */
void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid);

/* 0x00533BA0 */
void GridScratch_TestProjectedCellMaskBandsCf (Q12 worldYQ12,Q12 worldXQ12,byte lowBandIndex,byte highBandIndex);

/* 0x00536C90 */
WorldPositionXYRegisterPairQ12 EntityPathing_RebuildOverlappingGroupRoutes (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime, WorldRuntimeContext *worldRuntime);

/* 0x00534F50 */
void GridFootprint_ClearTraversalFlagsAroundWorldPoint (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12);

/* 0x005369A0 */
WorldPositionXYRegisterPairQ12 EntityPathing_UpdateRouteSegment (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime, GameEntityRuntime *routeEntityRuntime);

/* 0x00533D60 */
void GridScratch_TestWorldPointReachabilityCf (undefined4 traversalMask,GraphicsWorldCoordinateQ12 sourceWorldXQ12, GraphicsWorldCoordinateQ12 sourceWorldYQ12,GraphicsWorldCoordinateQ12 targetWorldXQ12 ,GraphicsWorldCoordinateQ12 targetWorldYQ12);

/* 0x00534660 */
GridPathBacktrackCfRegisterResult GridPathCost_BacktrackBestHexRoute (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate targetRow, FieldGridCellCoordinate targetColumn,GridScratchCell *startCell);

/* 0x00534960 */
GridPathMarkedRegionCellRegisterResult GridPathRegion_MarkUnreachableFromCell (GridPathUnreachableReferenceRow32 referenceRow, GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row, FieldGridCellCoordinate column);

/* 0x005333B0 */
void __cdecl GridScratch_CopyPrimaryToSecondary(void);

/* 0x005333E0 */
void __cdecl GridScratch_SwapPrimarySecondary(void);

/* 0x00533580 */
void GridScratch_FloodFillConnectedCellsRegs(void);

/* 0x00533C50 */
void GridScratch_TestConnectedReachabilityRecursiveCfRegs(void);

/* 0x00533EF0 */
void GridPathCost_PropagateWeightedHexNeighbors (GridPathPassCount remainingPasses,GridScratchCell *originCell, FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn);

/* 0x00534200 */
void GridScratch_ResetTraversalFlagsAndCosts(void);

/* 0x00534780 */
void GridPathRegion_MarkUnreachableRecursive(void);

/* 0x00534E70 */
void GridFootprint_ClearTraversalFlagsDiagonalNegative (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY, FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord );

/* 0x00534EE0 */
void GridFootprint_ClearTraversalFlagsDiagonalPositive (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY, FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord );

/* 0x005363C0 */
void GridReachability_MarkOpenRegionRecursive(void);

/* 0x00536440 */
void GridReachability_ClearCostedRegionRecursive(void);

/* 0x005342F0 */
GridPathNearestCellCfRegisterResult GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

/* 0x005344B0 */
void GridPathLine_TestHexSegmentClearCf (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow, FieldGridCellCoordinate startColumn,GridScratchCell *startCell, GridScratchCell *endCell);

#endif /* THANDOR_WORLD_PATHING_GRID_H */
