/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/route.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_ROUTE_H
#define THANDOR_WORLD_PATHING_ROUTE_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/pathing/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

extern EntityPathingPriorityPair *g_EntityPathingPriorityPairs;

PathingDestination
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

WorldPositionXY
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime);

void GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12);

WorldPositionXY EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView *routeEntityRuntime);

int GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

int GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord);

void PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase);

#endif /* THANDOR_WORLD_PATHING_ROUTE_H */
