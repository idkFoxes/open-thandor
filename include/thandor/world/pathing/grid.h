/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_GRID_H
#define THANDOR_WORLD_PATHING_GRID_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/pathing/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005349D0 */
EntityPathingDestinationEaxEdxEbxEcxCf17
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00536500 */
bool __thandor_cf_preserve_eax_ecx_edx
GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00533620 */
void __thandor_void_preserve_eax_ecx_edx
GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime);

/* 0x00533E70 */
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestRuntimePairReachabilityFromWorldPointCf
          (WorldPointXYQ12 *sourceWorldPoint,GridReachabilityRuntimePair8 *targetRuntimePair);

/* 0x005332C0 */
GridScratchAllocEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GridScratch_AllocateForFieldGridCf(FieldGridAsset *fieldGrid);

/* 0x00533360 */
void __thandor_preserve_eax GridScratch_ReleaseBuffers(void);

/* 0x00533400 */
void __thandor_void_preserve_eax_ecx_edx
GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid);

/* 0x00533BA0 */
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestProjectedCellMaskBandsCf
          (Q12 worldYQ12,Q12 worldXQ12,byte lowBandIndex,byte highBandIndex);

/* 0x00536C90 */
WorldPositionXYEaxEdx8
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x00534F50 */
void __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12);

/* 0x005369A0 */
WorldPositionXYEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView10 *routeEntityRuntime);

/* 0x00533D60 */
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestWorldPointReachabilityCf
          (uint traversalMask,GraphicsWorldCoordinateQ12 sourceWorldXQ12,
          GraphicsWorldCoordinateQ12 sourceWorldYQ12,GraphicsWorldCoordinateQ12 targetWorldXQ12,
          GraphicsWorldCoordinateQ12 targetWorldYQ12);

/* 0x00534660 */
GridPathBacktrackEaxEbxEcxCf13 __thandor_eax_cf_preserve_edx
GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate targetRow,
          FieldGridCellCoordinate targetColumn,GridScratchCell *startCell);

/* 0x00534960 */
GridPathMarkedRegionCellRegisterResult GridPathRegion_MarkUnreachableFromCell (GridPathUnreachableReferenceRow32 referenceRow, GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row, FieldGridCellCoordinate column);

/* 0x005333B0 */
void __cdecl GridScratch_CopyPrimaryToSecondary(void);

/* 0x005333E0 */
void __thandor_preserve_eax GridScratch_SwapPrimarySecondary(void);

/* 0x00533580 */
void __thandor_void_preserve_eax_ecx_edx
GridScratch_FloodFillConnectedCellsRegs
          (GridScratchStateMask traversalMask,dword rowStrideBytes,GridScratchCell *currentCell);

/* 0x00533C50 */
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestConnectedReachabilityRecursiveCfRegs
          (dword traversalMask,dword rowStrideBytes,dword *currentCell,dword *targetCell);

/* 0x00533EF0 */
void __thandor_void_preserve_eax_ecx_edx
GridPathCost_PropagateWeightedHexNeighbors
          (GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn);

/* 0x00534200 */
void GridScratch_ResetTraversalFlagsAndCosts(void);

/* 0x00534780 */
GridPathUnreachableRecursiveEdiEdx8 __thandor_eax_edx_cf_preserve_ecx
GridPathRegion_MarkUnreachableRecursive
          (dword rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          dword bestCellByteOffset);

/* 0x00534E70 */
int __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
          FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord);

/* 0x00534EE0 */
int __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
          FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord);

/* 0x005363C0 */
void __thandor_void_preserve_eax_ecx_edx
GridReachability_MarkOpenRegionRecursive(dword rowStrideBytes,GridScratchCell *currentCell);

/* 0x00536440 */
void __thandor_void_preserve_eax_ecx_edx
GridReachability_ClearCostedRegionRecursive(dword rowStrideBytes,GridScratchCell *currentCell);

/* 0x005342F0 */
GridPathNearestCellEaxEbxCf9 __thandor_eax_cf_preserve_ecx_edx
GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX);

/* 0x005344B0 */
bool __thandor_cf_preserve_eax_ecx_edx
GridPathLine_TestHexSegmentClearCf
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell);

#endif /* THANDOR_WORLD_PATHING_GRID_H */
