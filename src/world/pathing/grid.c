/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/grid.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/pathing/grid.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/pathing/grid. */

/* Address: 0x005349D0.
   Ownership: world/pathing/grid.
   Purpose: Converts entity and requested world positions to clipped grid cells, establishes the active traversal
   masks, resolves blocked destinations, propagates weighted costs when needed, backtracks a reachable route, and
   delegates final overlapping-group route reconstruction.
   Local calls: GridPathCost_FindNearestUnblockedCell, EntityPathing_RebuildOverlappingGroupRoutes,
   GridPathLine_TestHexSegmentClearCf, GridScratch_ResetTraversalFlagsAndCosts,
   GridPathCost_PropagateWeightedHexNeighbors, GridPathRegion_MarkUnreachableFromCell,
   GridPathCost_BacktrackBestHexRoute.
*/

EntityPathingDestinationEaxEdxEbxEcxCf17
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *entityTranslation;
  ModelRuntimeClassId runtimeClassId;
  longlong wideProductXOrY;
  longlong wideProductY;
  WorldPositionXYEaxEdx8 targetWorldPosition;
  WorldPositionXYEaxEdx8 fallbackWorldPosition;
  byte gridClassShift;
  int startColumnOrScratch;
  dword columnLimitOrWidth;
  GridPathUnreachableReferenceColumn32 referenceColumn;
  FieldGridRegionMask callerBlockingMask;
  uint scaledRowTerm;
  int cellCoordOrStrideBytes;
  int gridY;
  int rawTargetRow;
  dword rowLimit;
  GridPathUnreachableReferenceRow32 referenceRow;
  GridScratchCell *routeScratchCell;
  bool segmentClear;
  WorldPositionXYEaxEdx8 primaryWorldPosition;
  GridPathNearestCellEaxEbxCf9 nearestCell;
  GridPathMarkedRegionCellRegisterResult reachableRegionCell;
  EntityPathingDestinationEaxEdxEbxEcxCf17 resolvedDestination;
  GridPathBacktrackEaxEbxEcxCf13 backtrackResult;
  ModelDefinitionRuntimeSemanticView280 *modelDefinition;
  ArmyRuntimeSlot *armyRuntime;
  GameEntityRuntime *overlappedEntity;
  ModelRuntimeNode *entityModelNode;
  
  fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
  fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
  entityModelNode = (routeEntityRuntime->common).ownership.modelNode;
  wideProductXOrY = (longlong)(entityModelNode->worldTransform).translation.x * 0x1c6e9c;
  wideProductY = (longlong)(entityModelNode->worldTransform).translation.y * -0x20c8cc;
  scaledRowTerm = (int)((ulonglong)wideProductY >> 0x20) << 0xb | (uint)wideProductY >> 0x15;
  startColumnOrScratch = (int)((((int)((ulonglong)wideProductXOrY >> 0x20) << 0xc | (uint)wideProductXOrY >> 0x14) - scaledRowTerm) + 0x800)
          >> 10;
  cellCoordOrStrideBytes = (int)(scaledRowTerm * 2 + 0x800) >> 10;
  if (startColumnOrScratch < 1) {
    startColumnOrScratch = 1;
  }
  if (cellCoordOrStrideBytes < 1) {
    cellCoordOrStrideBytes = 1;
  }
  columnLimitOrWidth = startColumnOrScratch + 2U;
  if ((int)g_GridScratchWidth < (int)(startColumnOrScratch + 2U)) {
    columnLimitOrWidth = g_GridScratchWidth;
  }
  rowLimit = cellCoordOrStrideBytes + 2U;
  if ((int)g_GridScratchHeight < (int)(cellCoordOrStrideBytes + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  startColumnOrScratch = columnLimitOrWidth - 2;
  gridY = rowLimit - 2;
  scaledRowTerm = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
           (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  cellCoordOrStrideBytes = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                  (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - scaledRowTerm) + 0x800) >>
           10;
  rawTargetRow = (int)(scaledRowTerm * 2 + 0x800) >> 10;
  if (cellCoordOrStrideBytes < 1) {
    cellCoordOrStrideBytes = 1;
  }
  if (rawTargetRow < 1) {
    rawTargetRow = 1;
  }
  columnLimitOrWidth = cellCoordOrStrideBytes + 2U;
  if ((int)g_GridScratchWidth < (int)(cellCoordOrStrideBytes + 2U)) {
    columnLimitOrWidth = g_GridScratchWidth;
  }
  rowLimit = rawTargetRow + 2U;
  if ((int)g_GridScratchHeight < (int)(rawTargetRow + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  referenceColumn = columnLimitOrWidth - 2;
  referenceRow = rowLimit - 2;
  modelDefinition = (routeEntityRuntime->common).ownership.definitionOrClassRecord;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  runtimeClassId = modelDefinition->runtimeClassId4C;
  if (overlappedEntity != (GameEntityRuntime *)0x0) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
      [*(int *)((int)(overlappedEntity->common).ownership.definitionOrClassRecord + 0x4c)])
              (overlappedEntity);
  }
  armyRuntime = (routeEntityRuntime->common).ownership.runtimeLink;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove[runtimeClassId])
            (routeEntityRuntime);
  g_GridPathEntityClassMask = 1 << ((byte)armyRuntime->factionIndex & 0x1f);
  gridClassShift = (byte)modelDefinition->gridClassification260;
  g_GridPathHighCostMask = 0x10000 << (gridClassShift & 0x1f);
  g_GridPathBlockingMask =
       0x100 << (gridClassShift & 0x1f) | 0x1000000 << ((byte)modelDefinition->gridClassification264 & 0x1f);
  nearestCell = GridPathCost_FindNearestUnblockedCell(gridY,startColumnOrScratch);
  columnLimitOrWidth = g_GridScratchWidth;
  if (nearestCell.carry) {
    if ((nearestCell.selectedColumn == startColumnOrScratch) && (nearestCell.selectedRow == gridY)) {
      entityTranslation = &(((routeEntityRuntime->common).ownership.modelNode)->worldTransform).translation;
      fallbackWorldPosition.worldXQ12 = entityTranslation->x;
      fallbackWorldPosition.worldYQ12 = entityTranslation->y;
      primaryWorldPosition.worldXQ12 = entityTranslation->x;
      primaryWorldPosition.worldYQ12 = entityTranslation->y;
    }
    else {
      startColumnOrScratch = nearestCell.selectedRow * 0x400 + -0x600;
      wideProductXOrY = (longlong)(startColumnOrScratch + (nearestCell.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
      wideProductY = (longlong)startColumnOrScratch * -1999;
      primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                         ((int)((ulonglong)wideProductY >> 0x20) << 0x14 | (uint)wideProductY >> 0xc,
                          (int)((ulonglong)wideProductXOrY >> 0x20) << 0x13 | (uint)wideProductXOrY >> 0xd,
                          routeEntityRuntime,worldRuntime);
    }
  }
  else {
    cellCoordOrStrideBytes = g_GridScratchWidth * 8;
    routeScratchCell = g_GridScratchPrimary + gridY * g_GridScratchWidth + startColumnOrScratch;
    segmentClear = GridPathLine_TestHexSegmentClearCf
                       (g_GridPathHighCostMask,gridY,startColumnOrScratch,routeScratchCell,
                        g_GridScratchPrimary + referenceRow * g_GridScratchWidth + referenceColumn);
    if (segmentClear) {
      GridScratch_ResetTraversalFlagsAndCosts();
      GridPathCost_PropagateWeightedHexNeighbors(6,routeScratchCell,referenceRow,referenceColumn);
      routeScratchCell = routeScratchCell + -columnLimitOrWidth;
      if ((((0x7ffffffe < routeScratchCell[columnLimitOrWidth].pathCost) && (0x7ffffffe < routeScratchCell->pathCost)) &&
          (0x7ffffffe < routeScratchCell[1].pathCost)) &&
         (((0x7ffffffe < routeScratchCell[columnLimitOrWidth - 1].pathCost &&
           (0x7ffffffe < routeScratchCell[columnLimitOrWidth + 1].pathCost)) &&
          ((0x7ffffffe < routeScratchCell[columnLimitOrWidth * 2 + -1].pathCost &&
           (0x7ffffffe < routeScratchCell[columnLimitOrWidth * 2].pathCost)))))) {
        reachableRegionCell = GridPathRegion_MarkUnreachableFromCell(referenceRow,referenceColumn,gridY,startColumnOrScratch);
        cellCoordOrStrideBytes = reachableRegionCell.selectedRow * 0x400 + -0x600;
        wideProductXOrY = (longlong)(cellCoordOrStrideBytes + (reachableRegionCell.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
        targetWorldXQ12 = (int)((ulonglong)wideProductXOrY >> 0x20) << 0x13 | (uint)wideProductXOrY >> 0xd;
        wideProductXOrY = (longlong)cellCoordOrStrideBytes * -1999;
        targetWorldYQ12 = (int)((ulonglong)wideProductXOrY >> 0x20) << 0x14 | (uint)wideProductXOrY >> 0xc;
        GridScratch_ResetTraversalFlagsAndCosts();
        GridPathCost_PropagateWeightedHexNeighbors
                  (6,routeScratchCell,reachableRegionCell.selectedRow,reachableRegionCell.selectedColumn);
        cellCoordOrStrideBytes = g_GridScratchWidth * 8;
        routeScratchCell = g_GridScratchPrimary +
                       ((gridY * g_GridScratchWidth + startColumnOrScratch) - g_GridScratchWidth);
      }
      fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
      fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
      targetWorldPosition.worldYQ12 = targetWorldYQ12;
      targetWorldPosition.worldXQ12 = targetWorldXQ12;
      callerBlockingMask = g_GridPathHighCostMask;
      if ((*(uint *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0x18) & 2) != 0) {
        callerBlockingMask = 0;
      }
      backtrackResult = GridPathCost_BacktrackBestHexRoute
                         (callerBlockingMask,gridY,startColumnOrScratch,
                          (GridScratchCell *)((int)&routeScratchCell->stateMask + cellCoordOrStrideBytes));
      if (!backtrackResult.carry) {
        if (backtrackResult.routeStateMask == 0) {
          startColumnOrScratch = backtrackResult.selectedRow * 0x400 + -0x600;
          wideProductXOrY = (longlong)(startColumnOrScratch + (backtrackResult.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          wideProductY = (longlong)startColumnOrScratch * -1999;
          primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)wideProductY >> 0x20) << 0x14 | (uint)wideProductY >> 0xc,
                              (int)((ulonglong)wideProductXOrY >> 0x20) << 0x13 | (uint)wideProductXOrY >> 0xd,
                              routeEntityRuntime,worldRuntime);
          fallbackWorldPosition = targetWorldPosition;
        }
        else {
          startColumnOrScratch = backtrackResult.selectedRow * 0x400 + -0x600;
          wideProductXOrY = (longlong)(startColumnOrScratch + (backtrackResult.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          wideProductY = (longlong)startColumnOrScratch * -1999;
          primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)wideProductY >> 0x20) << 0x14 | (uint)wideProductY >> 0xc,
                              (int)((ulonglong)wideProductXOrY >> 0x20) << 0x13 | (uint)wideProductXOrY >> 0xd,
                              routeEntityRuntime,worldRuntime);
        }
        goto EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn;
      }
    }
    primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                       (targetWorldYQ12,targetWorldXQ12,routeEntityRuntime,worldRuntime);
    fallbackWorldPosition = primaryWorldPosition;
  }
EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn:
  targetWorldYQ12 = fallbackWorldPosition.worldYQ12;
  targetWorldXQ12 = fallbackWorldPosition.worldXQ12;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  startColumnOrScratch = *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0x4c);
  if (overlappedEntity != (GameEntityRuntime *)0x0) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
      [*(int *)((int)(overlappedEntity->common).ownership.definitionOrClassRecord + 0x4c)])
              (overlappedEntity);
  }
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[startColumnOrScratch])(routeEntityRuntime);
  resolvedDestination.fallbackWorldXQ12 = targetWorldXQ12;
  resolvedDestination.primaryWorldXQ12 = primaryWorldPosition.worldXQ12;
  resolvedDestination.primaryWorldYQ12 = primaryWorldPosition.worldYQ12;
  resolvedDestination.fallbackWorldYQ12 = targetWorldYQ12;
  resolvedDestination.carry = false;
  return resolvedDestination;
}


/* Address: 0x00536500.
   Ownership: world/pathing/grid.
   Purpose: Initializes every scratch record as marked with zero companion cost, clears a circular traversal
   footprint, flood-marks the open component containing the requested world point, reapplies the smaller footprint,
   and removes disconnected marked components. Typed parameters: p3 worldXQ12→Q12, p4 worldYQ12→Q12. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged. Typed parameters: p2
   radiusMetric→FieldGridRadiusUnits.
   Local calls: GridFootprint_ClearTraversalFlagsAroundWorldPoint, GridReachability_MarkOpenRegionRecursive,
   GridReachability_ClearCostedRegionRecursive.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  dword scratchWidth;
  uint cellsRemainingOrRowTerm;
  int columnOrCellsRemaining;
  int cellRow;
  GridScratchCell *scratchCursor;
  dword rowStrideBytes;
  bool moreBlocksRemain;
  
  cellsRemainingOrRowTerm = g_GridScratchWidth * g_GridScratchHeight;
  scratchCursor = g_GridScratchPrimary;
  do {
    scratchCursor->stateMask = scratchCursor->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor->pathCost = 0;
    scratchCursor[1].stateMask = scratchCursor[1].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[1].pathCost = 0;
    scratchCursor[2].stateMask = scratchCursor[2].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[2].pathCost = 0;
    scratchCursor[3].stateMask = scratchCursor[3].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[3].pathCost = 0;
    scratchCursor[4].stateMask = scratchCursor[4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[4].pathCost = 0;
    scratchCursor[5].stateMask = scratchCursor[5].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[5].pathCost = 0;
    scratchCursor[6].stateMask = scratchCursor[6].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[6].pathCost = 0;
    scratchCursor[7].stateMask = scratchCursor[7].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[7].pathCost = 0;
    scratchCursor[8].stateMask = scratchCursor[8].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[8].pathCost = 0;
    scratchCursor[9].stateMask = scratchCursor[9].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[9].pathCost = 0;
    scratchCursor[10].stateMask = scratchCursor[10].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[10].pathCost = 0;
    scratchCursor[0xb].stateMask = scratchCursor[0xb].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[0xb].pathCost = 0;
    scratchCursor[0xc].stateMask = scratchCursor[0xc].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[0xc].pathCost = 0;
    scratchCursor[0xd].stateMask = scratchCursor[0xd].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[0xd].pathCost = 0;
    scratchCursor[0xe].stateMask = scratchCursor[0xe].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[0xe].pathCost = 0;
    scratchCursor[0xf].stateMask = scratchCursor[0xf].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[0xf].pathCost = 0;
    scratchCursor = scratchCursor + 0x10;
    moreBlocksRemain = 0xf < cellsRemainingOrRowTerm;
    cellsRemainingOrRowTerm = cellsRemainingOrRowTerm - 0x10;
  } while (moreBlocksRemain && cellsRemainingOrRowTerm != 0);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric * 3,worldYQ12,worldXQ12);
  scratchWidth = g_GridScratchWidth;
  cellsRemainingOrRowTerm = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  columnOrCellsRemaining = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - cellsRemainingOrRowTerm) + 0x800) >> 10;
  if ((((columnOrCellsRemaining < 0) || (cellRow = (int)(cellsRemainingOrRowTerm * 2 + 0x800) >> 10, cellRow < 0)) ||
      ((int)g_GridScratchWidth <= columnOrCellsRemaining)) || ((int)g_GridScratchHeight <= cellRow)) {
    return true;
  }
  rowStrideBytes = g_GridScratchWidth * 8;
  GridReachability_MarkOpenRegionRecursive
            (rowStrideBytes,g_GridScratchPrimary + cellRow * g_GridScratchWidth + columnOrCellsRemaining);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  columnOrCellsRemaining = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  while (((((scratchCursor[scratchWidth].stateMask & 0x80000000) != 0 || (scratchCursor[scratchWidth].pathCost == 0)
           ) || ((scratchCursor[scratchWidth].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) == 0)) ||
         (((scratchCursor->pathCost != 0 && (scratchCursor[1].pathCost != 0)) &&
          ((scratchCursor[scratchWidth - 1].pathCost != 0 &&
           (((scratchCursor[scratchWidth + 1].pathCost != 0 && (scratchCursor[scratchWidth * 2 + -1].pathCost != 0))
            && (scratchCursor[scratchWidth * 2].pathCost != 0))))))))) {
    scratchCursor = scratchCursor + 1;
    columnOrCellsRemaining = columnOrCellsRemaining + -1;
    if (columnOrCellsRemaining == 0) {
      return false;
    }
  }
  GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scratchCursor + scratchWidth);
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  columnOrCellsRemaining = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  do {
    if ((((scratchCursor[scratchWidth].stateMask & 0x80000000) == 0) && (scratchCursor[scratchWidth].pathCost != 0))
       && ((scratchCursor[scratchWidth].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0)) {
      if (scratchCursor->pathCost == 0) {
        return true;
      }
      if (scratchCursor[1].pathCost == 0) {
        return true;
      }
      if (scratchCursor[scratchWidth - 1].pathCost == 0) {
        return true;
      }
      if (scratchCursor[scratchWidth + 1].pathCost == 0) {
        return true;
      }
      if (scratchCursor[scratchWidth * 2 + -1].pathCost == 0) {
        return true;
      }
      if (scratchCursor[scratchWidth * 2].pathCost == 0) {
        return true;
      }
    }
    scratchCursor = scratchCursor + 1;
    columnOrCellsRemaining = columnOrCellsRemaining + -1;
    if (columnOrCellsRemaining == 0) {
      return false;
    }
  } while( true );
}


/* Address: 0x00533620.
   Ownership: world/pathing/grid.
   Purpose: Rebuilds grid-scratch classification masks from field-cell material, height, and neighbor state;
   incorporates active runtime footprints; performs connected-region fills; and normalizes the resulting
   classification bands. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Builds state/classification bits in separately
   allocated 8-byte GridScratchCell_V419 records. Terrain class bits 24..30 overlap FLD numeric bit positions but
   are not FLD flagsAndMaterial state and must never be written back there by semantic inference.
   [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] GridScratchCell.stateMask now uses the versionless GridScratchStateMask
   enum; GridScratch bits 24..30 remain a storage namespace distinct from FLD flagsAndMaterial.
   Local calls: GridScratch_FloodFillConnectedCellsRegs.
*/
void __thandor_void_preserve_eax_ecx_edx
GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime)

{
  FieldGridDimension fieldGridWidth;
  FieldCellPackedFlagsAndMaterial cellFlags;
  longlong wideProductX;
  longlong wideProductY;
  dword scratchWidth;
  dword scratchStride;
  int countOrWaterDeltaOrColumn;
  GridScratchStateMask cellClassMask;
  FieldGridDimension columnsRemaining;
  int angleOrCountOrRow;
  uint scaledRowTerm;
  int triangle0Angle;
  FieldGridCell *fieldCell;
  GridScratchCell *promoteCursor;
  GridScratchCell *scratchCursor;
  WorldOwnerListNode100 *ownerNode;
  FieldGridDimension rowsRemaining;
  FieldGridAsset *fieldGridAsset;
  
  fieldGridAsset = worldRuntime->fieldGrid;
  fieldGridWidth = fieldGridAsset->gridWidth;
  rowsRemaining = fieldGridAsset->gridHeight;
  countOrWaterDeltaOrColumn = fieldGridWidth * rowsRemaining;
  scratchCursor = g_GridScratchPrimary;
  do {
    scratchCursor->stateMask = scratchCursor->stateMask & 0xffff01;
    scratchCursor[1].stateMask = scratchCursor[1].stateMask & 0xffff01;
    scratchCursor[2].stateMask = scratchCursor[2].stateMask & 0xffff01;
    scratchCursor[3].stateMask = scratchCursor[3].stateMask & 0xffff01;
    scratchCursor[4].stateMask = scratchCursor[4].stateMask & 0xffff01;
    scratchCursor[5].stateMask = scratchCursor[5].stateMask & 0xffff01;
    scratchCursor[6].stateMask = scratchCursor[6].stateMask & 0xffff01;
    scratchCursor[7].stateMask = scratchCursor[7].stateMask & 0xffff01;
    scratchCursor[8].stateMask = scratchCursor[8].stateMask & 0xffff01;
    scratchCursor[9].stateMask = scratchCursor[9].stateMask & 0xffff01;
    scratchCursor[10].stateMask = scratchCursor[10].stateMask & 0xffff01;
    scratchCursor[0xb].stateMask = scratchCursor[0xb].stateMask & 0xffff01;
    scratchCursor[0xc].stateMask = scratchCursor[0xc].stateMask & 0xffff01;
    scratchCursor[0xd].stateMask = scratchCursor[0xd].stateMask & 0xffff01;
    scratchCursor[0xe].stateMask = scratchCursor[0xe].stateMask & 0xffff01;
    scratchCursor[0xf].stateMask = scratchCursor[0xf].stateMask & 0xffff01;
    scratchWidth = g_GridScratchWidth;
    scratchCursor = scratchCursor + 0x10;
    countOrWaterDeltaOrColumn = countOrWaterDeltaOrColumn + -1;
  } while (countOrWaterDeltaOrColumn != 0);
  fieldCell = fieldGridAsset->cells;
  columnsRemaining = fieldGridWidth;
  scratchCursor = g_GridScratchPrimary;
  do {
    do {
      cellClassMask = ((uint)((fieldCell->occupancyMask & 0xf900) != 0) +
              ((uint)((fieldCell->occupancyMask & 0xf90000) != 0) +
              ((uint)((fieldCell->occupancyMask & 0xf9000000) != 0) +
              ((uint)((fieldCell->occupancyMask & 0xf900000000) != 0) +
              ((uint)((fieldCell->occupancyMask & 0xf90000000000) != 0) +
              ((uint)((fieldCell->occupancyMask & 0xf9000000000000) != 0) +
              (uint)((fieldCell->occupancyMask & 0xf900000000000000) != 0) * 2) * 2) * 2) * 2) * 2) *
              2) * 2;
      countOrWaterDeltaOrColumn = fieldCell->waterSurfaceDelta;
      triangle0Angle = (int)fieldCell->triangle0NormalAngles >> 0x10;
      angleOrCountOrRow = (int)fieldCell->triangle1NormalAngles >> 0x10;
      if (countOrWaterDeltaOrColumn <= g_GridTerrainClassBit24MaxWaterSurfaceDelta) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      }
      if (angleOrCountOrRow <= g_GridTerrainClassBit24MaxTriangle1NormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      }
      if (countOrWaterDeltaOrColumn < 0) {
        angleOrCountOrRow = triangle0Angle;
      }
      if (angleOrCountOrRow <= g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT25;
      }
      if (angleOrCountOrRow <= g_GridTerrainClassBit26MaxSelectedNormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT26;
      }
      if (angleOrCountOrRow <= g_GridTerrainClassBit27MaxSelectedNormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT27;
      }
      if (g_GridTerrainClassBit28MinWaterSurfaceDelta <= countOrWaterDeltaOrColumn) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
      }
      if (g_GridTerrainClassBit29MinWaterSurfaceDelta <= countOrWaterDeltaOrColumn) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
      }
      if (g_GridTerrainClassBit30MinWaterSurfaceDelta <= countOrWaterDeltaOrColumn) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
      }
      if (triangle0Angle <= g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
      }
      if (triangle0Angle <= g_GridTerrainClassBit29MaxTriangle0NormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
      }
      if (triangle0Angle <= g_GridTerrainClassBit30MaxTriangle0NormalAngleHigh16) {
        cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
      }
      cellFlags = fieldCell->flagsAndMaterial;
      if ((cellFlags & 0x88006000) != 0) {
        cellClassMask = cellClassMask | 0x80000000;
      }
      if ((cellFlags & FIELD_CELL_FIRST_ROW_BOUNDARY) == 0) {
        scratchCursor[scratchWidth * -2 + 2].stateMask = scratchCursor[scratchWidth * -2 + 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * -2 + 3].stateMask = scratchCursor[scratchWidth * -2 + 3].stateMask | cellClassMask;
        scratchCursor[1 - scratchWidth].stateMask = scratchCursor[1 - scratchWidth].stateMask | cellClassMask;
        scratchCursor[2 - scratchWidth].stateMask = scratchCursor[2 - scratchWidth].stateMask | cellClassMask;
        scratchCursor[3 - scratchWidth].stateMask = scratchCursor[3 - scratchWidth].stateMask | cellClassMask;
        if ((cellFlags & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
          scratchCursor[scratchWidth * -2 + 4].stateMask = scratchCursor[scratchWidth * -2 + 4].stateMask | cellClassMask;
          scratchCursor[4 - scratchWidth].stateMask = scratchCursor[4 - scratchWidth].stateMask | cellClassMask;
          scratchCursor[5 - scratchWidth].stateMask = scratchCursor[5 - scratchWidth].stateMask | cellClassMask;
        }
      }
      if ((cellFlags & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
        scratchCursor[4].stateMask = scratchCursor[4].stateMask | cellClassMask;
        scratchCursor[5].stateMask = scratchCursor[5].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 4].stateMask = scratchCursor[scratchWidth + 4].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 5].stateMask = scratchCursor[scratchWidth + 5].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + 4].stateMask = scratchCursor[scratchWidth * 2 + 4].stateMask | cellClassMask;
      }
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      scratchCursor = scratchCursor + scratchWidth;
      if ((cellFlags & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
        scratchCursor[-1].stateMask = scratchCursor[-1].stateMask | cellClassMask;
        scratchCursor[scratchWidth - 1].stateMask = scratchCursor[scratchWidth - 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth - 2].stateMask = scratchCursor[scratchWidth - 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + -1].stateMask = scratchCursor[scratchWidth * 2 + -1].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + -2].stateMask = scratchCursor[scratchWidth * 2 + -2].stateMask | cellClassMask;
      }
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      scratchCursor[scratchWidth].stateMask = scratchCursor[scratchWidth].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 1].stateMask = scratchCursor[scratchWidth + 1].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 2].stateMask = scratchCursor[scratchWidth + 2].stateMask | cellClassMask;
      scratchCursor[scratchWidth + 3].stateMask = scratchCursor[scratchWidth + 3].stateMask | cellClassMask;
      scratchCursor = scratchCursor + scratchWidth * 2;
      scratchCursor->stateMask = scratchCursor->stateMask | cellClassMask;
      scratchCursor[1].stateMask = scratchCursor[1].stateMask | cellClassMask;
      scratchCursor[2].stateMask = scratchCursor[2].stateMask | cellClassMask;
      scratchCursor[3].stateMask = scratchCursor[3].stateMask | cellClassMask;
      if ((cellFlags & 0x80000000) == 0) {
        scratchCursor[scratchWidth].stateMask = scratchCursor[scratchWidth].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 1].stateMask = scratchCursor[scratchWidth + 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 2].stateMask = scratchCursor[scratchWidth + 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2].stateMask = scratchCursor[scratchWidth * 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + 1].stateMask = scratchCursor[scratchWidth * 2 + 1].stateMask | cellClassMask;
        if ((cellFlags & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
          scratchCursor[scratchWidth - 1].stateMask = scratchCursor[scratchWidth - 1].stateMask | cellClassMask;
          scratchCursor[scratchWidth - 2].stateMask = scratchCursor[scratchWidth - 2].stateMask | cellClassMask;
          scratchCursor[scratchWidth * 2 + -1].stateMask = scratchCursor[scratchWidth * 2 + -1].stateMask | cellClassMask;
        }
      }
      scratchStride = g_GridScratchWidth;
      scratchCursor = scratchCursor + scratchWidth * -3 + 4;
      fieldCell = fieldCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    scratchCursor = scratchCursor + scratchWidth * 3;
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = fieldGridWidth;
  } while (rowsRemaining != 0);
  angleOrCountOrRow = g_GridScratchHeight * g_GridScratchWidth;
  scratchCursor = g_GridScratchPrimary + g_GridScratchWidth * -4;
  countOrWaterDeltaOrColumn = angleOrCountOrRow;
  do {
    scratchCursor[scratchStride * 4].stateMask =
         scratchCursor[scratchStride * 4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    if (((scratchCursor[scratchStride * 4].stateMask & 0x80000000) == 0) &&
       (((scratchCursor[scratchStride * 4].stateMask | scratchCursor->stateMask | scratchCursor[4].stateMask |
          scratchCursor[scratchStride * 4 + -4].stateMask | scratchCursor[scratchStride * 4 + 4].stateMask |
          scratchCursor[scratchStride * 8 + -4].stateMask | scratchCursor[scratchStride * 8].stateMask) &
        GRID_SCRATCH_TERRAIN_CLASS_BIT24) != 0)) {
      scratchCursor[scratchStride * 4].stateMask =
           scratchCursor[scratchStride * 4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    scratchCursor = scratchCursor + 1;
    countOrWaterDeltaOrColumn = countOrWaterDeltaOrColumn + -1;
    promoteCursor = g_GridScratchPrimary;
  } while (countOrWaterDeltaOrColumn != 0);
  do {
    if ((promoteCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) {
      promoteCursor->stateMask = promoteCursor->stateMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      promoteCursor->stateMask = promoteCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    angleOrCountOrRow = angleOrCountOrRow + -1;
    promoteCursor = promoteCursor + 1;
  } while (angleOrCountOrRow != 0);
  ownerNode = worldRuntime->ownerListHead;
  if (ownerNode != (WorldOwnerListNode100 *)0x0) {
    do {
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (*(int *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)ownerNode->runtimePayload + 0x278) != 1)) {
        wideProductX = (longlong)ownerNode->worldXQ12 * 0x1c6e9c;
        wideProductY = (longlong)ownerNode->worldYQ12 * -0x20c8cc;
        scaledRowTerm = (int)((ulonglong)wideProductY >> 0x20) << 0xb | (uint)wideProductY >> 0x15;
        countOrWaterDeltaOrColumn = (int)((((int)((ulonglong)wideProductX >> 0x20) << 0xc | (uint)wideProductX >> 0x14) - scaledRowTerm) +
                     0x800) >> 10;
        if (((-1 < countOrWaterDeltaOrColumn) && (angleOrCountOrRow = (int)(scaledRowTerm * 2 + 0x800) >> 10, -1 < angleOrCountOrRow)) &&
           ((countOrWaterDeltaOrColumn < (int)g_GridScratchWidth && (angleOrCountOrRow < (int)g_GridScratchHeight)))) {
          GridScratch_FloodFillConnectedCellsRegs
                    (0xf0000001,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + angleOrCountOrRow * g_GridScratchWidth + countOrWaterDeltaOrColumn);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != (WorldOwnerListNode100 *)0x0);
    countOrWaterDeltaOrColumn = g_GridScratchWidth * g_GridScratchHeight;
    scratchCursor = g_GridScratchPrimary;
    do {
      if ((scratchCursor->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
           GRID_SCRATCH_TERRAIN_CLASS_BIT28|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCursor->stateMask =
             scratchCursor->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
             GRID_SCRATCH_TERRAIN_CLASS_BIT28);
      }
      scratchCursor->stateMask = scratchCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      scratchCursor = scratchCursor + 1;
      countOrWaterDeltaOrColumn = countOrWaterDeltaOrColumn + -1;
    } while (countOrWaterDeltaOrColumn != 0);
    ownerNode = worldRuntime->ownerListHead;
    do {
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (*(int *)(*(int *)((int)ownerNode->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)ownerNode->runtimePayload + 0x278) != 1)) {
        wideProductX = (longlong)ownerNode->worldXQ12 * 0x1c6e9c;
        wideProductY = (longlong)ownerNode->worldYQ12 * -0x20c8cc;
        scaledRowTerm = (int)((ulonglong)wideProductY >> 0x20) << 0xb | (uint)wideProductY >> 0x15;
        countOrWaterDeltaOrColumn = (int)((((int)((ulonglong)wideProductX >> 0x20) << 0xc | (uint)wideProductX >> 0x14) - scaledRowTerm) +
                     0x800) >> 10;
        if ((((-1 < countOrWaterDeltaOrColumn) && (angleOrCountOrRow = (int)(scaledRowTerm * 2 + 0x800) >> 10, -1 < angleOrCountOrRow)) &&
            (countOrWaterDeltaOrColumn < (int)g_GridScratchWidth)) && (angleOrCountOrRow < (int)g_GridScratchHeight)) {
          GridScratch_FloodFillConnectedCellsRegs
                    (0x8e000001,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + angleOrCountOrRow * g_GridScratchWidth + countOrWaterDeltaOrColumn);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != (WorldOwnerListNode100 *)0x0);
    countOrWaterDeltaOrColumn = g_GridScratchWidth * g_GridScratchHeight;
    scratchCursor = g_GridScratchPrimary;
    do {
      if ((scratchCursor->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
           GRID_SCRATCH_TERRAIN_CLASS_BIT25|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCursor->stateMask =
             scratchCursor->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
             GRID_SCRATCH_TERRAIN_CLASS_BIT25);
      }
      scratchCursor = scratchCursor + 1;
      countOrWaterDeltaOrColumn = countOrWaterDeltaOrColumn + -1;
    } while (countOrWaterDeltaOrColumn != 0);
  }
  return;
}


/* Address: 0x00533E70.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test runtime pair reachability from world point carry-flag result.
   Local calls: GridScratch_TestWorldPointReachabilityCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestRuntimePairReachabilityFromWorldPointCf
          (WorldPointXYQ12 *sourceWorldPoint,GridReachabilityRuntimePair8 *targetRuntimePair)

{
  bool unreachable;
  ModelDefinitionRuntimeSemanticView280 *targetModelDefinition;
  
  targetModelDefinition = (ModelDefinitionRuntimeSemanticView280 *)
          (targetRuntimePair->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  unreachable = GridScratch_TestWorldPointReachabilityCf
                    (0x100 << ((byte)targetModelDefinition->gridClassification260 & 0x1f) |
                     0x1000000 << ((byte)targetModelDefinition->gridClassification264 & 0x1f),
                     sourceWorldPoint->worldYQ12,sourceWorldPoint->worldXQ12,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.y,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.x);
  return unreachable;
}


/* Address: 0x005332C0.
   Ownership: world/pathing/grid.
   Purpose: Scales field-grid width and height by four, allocates and exchanges two equal cell-sized scratch
   buffers plus one fixed 0x180000-byte auxiliary buffer, publishes its end pointer, and reports allocation failure
   through CF. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Allocation arithmetic proves 8 bytes per GridScratch
   record: scratchWidth*scratchHeight*8. Primary and secondary buffers therefore hold GridScratchCell_V419 records.
   The fixed 0x180000-byte auxiliary allocation is the pointer queue now named g_GridPathCostQueueBegin..End.
*/
GridScratchAllocEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GridScratch_AllocateForFieldGridCf(FieldGridAsset *fieldGrid)

{
  GridScratchCell *previousSecondaryScratchBuffer;
  GridScratchCell **previousCostQueueBuffer;
  dword *newScratchBuffer;
  dword *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  dword bytes;
  bool allocationSizeOverflow;
  ArenaAllocEaxCf5 allocResult;
  ArenaFreeEaxCf5 freeResult;
  GridScratchAllocEaxCf5 failureResult;
  longlong scratchAllocationByteCountProduct;
  GridScratchCell *previousScratchBuffer;
  
  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  bytes = fieldGrid->gridWidth * 0x20 * g_GridScratchHeight;
  allocResult = (*g_MemoryApi.alloc)(bytes);
  previousScratchBuffer = g_GridScratchPrimary;
  newScratchBuffer = (dword *)allocResult.eax;
  if (!allocResult.carry) {
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    (*g_MemoryApi.free)(previousScratchBuffer);
    allocResult = (*g_MemoryApi.alloc)(bytes);
    previousSecondaryScratchBuffer = g_GridScratchSecondary;
    newSecondaryScratchBuffer = (dword *)allocResult.eax;
    newScratchBuffer = newSecondaryScratchBuffer;
    if (!allocResult.carry) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      (*g_MemoryApi.free)(previousSecondaryScratchBuffer);
      allocResult = (*g_MemoryApi.alloc)(0x180000);
      previousCostQueueBuffer = g_GridPathCostQueueBegin;
      newAuxiliaryBuffer = (void *)allocResult.eax;
      newScratchBuffer = newAuxiliaryBuffer;
      if (!allocResult.carry) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((int)newAuxiliaryBuffer + 0x180000);
        g_GridPathCostQueueBegin = newAuxiliaryBuffer;
        freeResult = (*g_MemoryApi.free)(previousCostQueueBuffer);
        return THANDOR_BITCAST(qword, GridScratchAllocEaxCf5, ((THANDOR_BITCAST(ArenaFreeEaxCf5, qword, freeResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
  }
  failureResult.carry = true;
  failureResult.eax = (dword)newScratchBuffer;
  return failureResult;
}


/* Address: 0x00533360.
   Ownership: world/pathing/grid.
   Purpose: Releases the auxiliary, primary, and secondary grid scratch allocations through the engine memory API,
   then clears all three global pointers.
*/
void __thandor_preserve_eax GridScratch_ReleaseBuffers(void)

{
  (*g_MemoryApi.free)(g_GridPathCostQueueBegin);
  (*g_MemoryApi.free)(g_GridScratchPrimary);
  (*g_MemoryApi.free)(g_GridScratchSecondary);
  g_GridPathCostQueueBegin = (GridScratchCell **)0x0;
  g_GridScratchPrimary = (GridScratchCell *)0x0;
  g_GridScratchSecondary = (GridScratchCell *)0x0;
  return;
}


/* Address: 0x00533400.
   Ownership: world/pathing/grid.
   Purpose: Builds a compact mask from FieldGridCell occupancy bytes and OR-propagates it into neighboring entries
   of g_GridScratchPrimary, skipping rejected terrain cells. Tick-wheel case 7 tail: dilates the rebuilt occupancy
   mask into the neighborhood scratch grid.
*/
void __thandor_void_preserve_eax_ecx_edx
GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid)

{
  uint *lowerScratchCursor;
  FieldGridDimension columnsRemaining;
  FieldGridCell *currentFieldCell;
  dword *scratchCellCursor;
  uint *propagatedScratchCursor;
  FieldGridDimension rowsRemaining;
  FieldGridDimension gridWidth;
  uint propagatedOccupancyGroupMask;
  dword scratchWidth;
  uint *nextScratchCellCursor;
  FieldGridCell *nextFieldCell;
  
  scratchWidth = g_GridScratchWidth;
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  columnsRemaining = gridWidth;
  currentFieldCell = fieldGrid->cells;
  scratchCellCursor = &g_GridScratchPrimary->stateMask;
  do {
    do {
      /* the original advances first and tests [ESI-0x30]: the flags of the current cell */
      nextScratchCellCursor = scratchCellCursor + 8;
      if ((currentFieldCell->flagsAndMaterial & 0x88006000) == 0) {
        propagatedOccupancyGroupMask =
             ((uint)((currentFieldCell->occupancyMask & 0xf900) != 0) +
             ((uint)((currentFieldCell->occupancyMask & 0xf90000) != 0) +
             ((uint)((currentFieldCell->occupancyMask & 0xf9000000) != 0) +
             ((uint)((currentFieldCell->occupancyMask & 0xf900000000) != 0) +
             ((uint)((currentFieldCell->occupancyMask & 0xf90000000000) != 0) +
             ((uint)((currentFieldCell->occupancyMask & 0xf9000000000000) != 0) +
             (uint)((currentFieldCell->occupancyMask & 0xf900000000000000) != 0) * 2) * 2) * 2) * 2)
             * 2) * 2) * 2;
        scratchCellCursor[scratchWidth * -4 + 4] =
             scratchCellCursor[scratchWidth * -4 + 4] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -4 + 6] =
             scratchCellCursor[scratchWidth * -4 + 6] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -2 + 2] =
             scratchCellCursor[scratchWidth * -2 + 2] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -2 + 4] =
             scratchCellCursor[scratchWidth * -2 + 4] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -2 + 6] =
             scratchCellCursor[scratchWidth * -2 + 6] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -4 + 8] =
             scratchCellCursor[scratchWidth * -4 + 8] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -2 + 8] =
             scratchCellCursor[scratchWidth * -2 + 8] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * -2 + 10] =
             scratchCellCursor[scratchWidth * -2 + 10] | propagatedOccupancyGroupMask;
        scratchCellCursor[8] = scratchCellCursor[8] | propagatedOccupancyGroupMask;
        scratchCellCursor[10] = scratchCellCursor[10] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * 2 + 8] =
             scratchCellCursor[scratchWidth * 2 + 8] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * 2 + 10] =
             scratchCellCursor[scratchWidth * 2 + 10] | propagatedOccupancyGroupMask;
        scratchCellCursor[scratchWidth * 4 + 8] =
             scratchCellCursor[scratchWidth * 4 + 8] | propagatedOccupancyGroupMask;
        *scratchCellCursor = *scratchCellCursor | propagatedOccupancyGroupMask;
        scratchCellCursor[2] = scratchCellCursor[2] | propagatedOccupancyGroupMask;
        scratchCellCursor[4] = scratchCellCursor[4] | propagatedOccupancyGroupMask;
        scratchCellCursor[6] = scratchCellCursor[6] | propagatedOccupancyGroupMask;
        propagatedScratchCursor = scratchCellCursor + scratchWidth * 2;
        propagatedScratchCursor[-2] = propagatedScratchCursor[-2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2 + -2] =
             propagatedScratchCursor[scratchWidth * 2 + -2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2 + -4] =
             propagatedScratchCursor[scratchWidth * 2 + -4] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 4 + -2] =
             propagatedScratchCursor[scratchWidth * 4 + -2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 4 + -4] =
             propagatedScratchCursor[scratchWidth * 4 + -4] | propagatedOccupancyGroupMask;
        *propagatedScratchCursor = *propagatedScratchCursor | propagatedOccupancyGroupMask;
        propagatedScratchCursor[2] = propagatedScratchCursor[2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[4] = propagatedScratchCursor[4] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[6] = propagatedScratchCursor[6] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2] =
             propagatedScratchCursor[scratchWidth * 2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2 + 2] =
             propagatedScratchCursor[scratchWidth * 2 + 2] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2 + 4] =
             propagatedScratchCursor[scratchWidth * 2 + 4] | propagatedOccupancyGroupMask;
        propagatedScratchCursor[scratchWidth * 2 + 6] =
             propagatedScratchCursor[scratchWidth * 2 + 6] | propagatedOccupancyGroupMask;
        lowerScratchCursor = propagatedScratchCursor + scratchWidth * 4;
        *lowerScratchCursor = *lowerScratchCursor | propagatedOccupancyGroupMask;
        lowerScratchCursor[2] = lowerScratchCursor[2] | propagatedOccupancyGroupMask;
        lowerScratchCursor[4] = lowerScratchCursor[4] | propagatedOccupancyGroupMask;
        lowerScratchCursor[6] = lowerScratchCursor[6] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 2] = lowerScratchCursor[scratchWidth * 2] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 2 + 2] = lowerScratchCursor[scratchWidth * 2 + 2] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 2 + 4] = lowerScratchCursor[scratchWidth * 2 + 4] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 4] = lowerScratchCursor[scratchWidth * 4] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 4 + 2] = lowerScratchCursor[scratchWidth * 4 + 2] | propagatedOccupancyGroupMask;
        lowerScratchCursor[scratchWidth * 2 + -2] = lowerScratchCursor[scratchWidth * 2 + -2] | propagatedOccupancyGroupMask
        ;
        lowerScratchCursor[scratchWidth * 2 + -4] = lowerScratchCursor[scratchWidth * 2 + -4] | propagatedOccupancyGroupMask
        ;
        lowerScratchCursor[scratchWidth * 4 + -2] = lowerScratchCursor[scratchWidth * 4 + -2] | propagatedOccupancyGroupMask
        ;
        nextScratchCellCursor = lowerScratchCursor + scratchWidth * -6 + 8;
      }
      scratchCellCursor = nextScratchCellCursor;
      columnsRemaining = columnsRemaining - 1;
      currentFieldCell = currentFieldCell + 1;
    } while (columnsRemaining != 0);
    scratchCellCursor = scratchCellCursor + scratchWidth * 6;
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00533BA0.
   Ownership: world/pathing/grid.
   Purpose: Projects a world point into the grid-scratch coordinate system and rejects cells whose signed state or
   selected low and high classification bands are set. Carry preserves the rejection result. Typed parameters: p2
   worldXQ12→Q12, p3 worldYQ12→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestProjectedCellMaskBandsCf
          (Q12 worldYQ12,Q12 worldXQ12,byte lowBandIndex,byte highBandIndex)

{
  GridScratchStateMask cellStateMask;
  int cellColumn;
  uint scaledRowTerm;
  int cellRow;
  
  scaledRowTerm = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  cellColumn = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - scaledRowTerm) + 0x800) >> 10;
  if ((((-1 < cellColumn) && (cellRow = (int)(scaledRowTerm * 2 + 0x800) >> 10, -1 < cellRow)) &&
      (cellColumn < (int)g_GridScratchWidth)) && (cellRow < (int)g_GridScratchHeight)) {
    cellStateMask = g_GridScratchPrimary[cellRow * g_GridScratchWidth + cellColumn].stateMask;
    if (((-1 < (int)cellStateMask) && ((0x100 << (lowBandIndex & 0x1f) & cellStateMask) == 0)) &&
       ((0x1000000 << (highBandIndex & 0x1f) & cellStateMask) == 0)) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00536C90.
   Ownership: world/pathing/grid.
   Purpose: Copies and swaps the scratch buffers, collects overlapping candidate entities using depth-bin masks,
   orders them through the pointer-priority heap, refreshes their callbacks, rebuilds each route in priority order,
   and restores the original scratch buffer.
   Local calls: GridScratch_CopyPrimaryToSecondary, GridScratch_SwapPrimarySecondary,
   EntityPathing_UpdateRouteSegment.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives], DepthBinMasks_OverlapCf
   [graphics/render/primitives], PriorityPairHeap_SiftUp [core/memory/allocator], PriorityPairHeap_SiftDown
   [core/memory/allocator], GridInfluence_SetLowDistanceBandsAroundWorldPoint [world/pathing/influence].
*/

WorldPositionXYEaxEdx8
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  sdword swappedPriority;
  int entityWorldY;
  GameEntityRuntime *candidateEntity;
  void *candidateRecord;
  void *candidateDefinition;
  sdword rootPriority;
  EntityPathingPriorityPair *heapBase;
  DepthBinMask32 secondMaskHigh;
  DepthBinMask32 secondMaskLow;
  int searchRadius;
  DepthIntervalRadius32 intervalRadius;
  int entityWorldXOrScratch;
  dword pairsRemaining;
  dword routesRemaining;
  uint heapSize;
  WorldOwnerListNode100 *ownerNode;
  int deltaY;
  EntityPathingPriorityPair *influencePair;
  EntityPathingPriorityPair *pairCursor;
  bool masksOverlap;
  WorldPositionXYEaxEdx8 routeTarget;
  WorldPositionXYEaxEdx8 resolvedTarget;
  ModelRuntimeNode *entityModelNode;
  
  routeTarget.worldYQ12 = targetWorldY;
  routeTarget.worldXQ12 = targetWorldX;
  GridScratch_CopyPrimaryToSecondary();
  GridScratch_SwapPrimarySecondary();
  entityModelNode = (routeEntityRuntime->common).ownership.modelNode;
  ownerNode = worldRuntime->ownerListHead;
  entityWorldXOrScratch = (entityModelNode->worldTransform).translation.x;
  entityWorldY = (entityModelNode->worldTransform).translation.y;
  searchRadius = entityWorldXOrScratch - targetWorldX;
  if (searchRadius < 0) {
    searchRadius = -searchRadius;
  }
  deltaY = entityWorldY - targetWorldY;
  if (deltaY < 0) {
    deltaY = -deltaY;
  }
  if (searchRadius < deltaY) {
    searchRadius = deltaY;
  }
  searchRadius = searchRadius + *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord +
                          0xdc);
  secondMaskHigh = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldXOrScratch + targetWorldX) >> 1);
  secondMaskLow = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldY + targetWorldY) >> 1);
  entityWorldXOrScratch = 0x20;
  g_EntityPathingPriorityPairCount = 0;
  pairCursor = g_EntityPathingPriorityPairs;
  do {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      candidateEntity = ownerNode->runtimePayload;
      candidateRecord = (candidateEntity->common).ownership.definitionOrClassRecord;
      masksOverlap = DepthBinMasks_OverlapCf
                         (ownerNode->modelDepthBinMaskFar,ownerNode->modelDepthBinMaskNear,
                          secondMaskLow,secondMaskHigh);
      if ((masksOverlap) && (*(int *)((int)candidateRecord + 0x18) != 0)) {
        pairCursor->entity = candidateEntity;
        pairCursor->priority = 0;
        g_EntityPathingPriorityPairCount = g_EntityPathingPriorityPairCount + 1;
        pairCursor = pairCursor + 1;
        entityWorldXOrScratch = entityWorldXOrScratch + -1;
        if (entityWorldXOrScratch == 0) break;
      }
    }
    ownerNode = ownerNode->nextNode;
  } while (ownerNode != (WorldOwnerListNode100 *)0x0);
  if (1 < g_EntityPathingPriorityPairCount) {
    entityWorldXOrScratch = *(int *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0xc);
    pairsRemaining = g_EntityPathingPriorityPairCount;
    pairCursor = g_EntityPathingPriorityPairs;
    do {
      candidateRecord = (pairCursor->entity->common).ownership.runtimeLink;
      candidateDefinition = (pairCursor->entity->common).ownership.definitionOrClassRecord;
      if ((((*(uint *)((int)candidateRecord + 0x18) & 2) == 0) &&
          (pairCursor->priority = pairCursor->priority + 1,
          entityWorldXOrScratch == *(int *)((int)candidateRecord + 0xc))) &&
         (pairCursor->priority = pairCursor->priority + 1,
         (*(uint *)((int)candidateRecord + 0x18) & 1) == 0)) {
        pairCursor->priority = pairCursor->priority + *(int *)((int)candidateDefinition + 0xc);
      }
      heapBase = g_EntityPathingPriorityPairs;
      pairCursor = pairCursor + 1;
      pairsRemaining = pairsRemaining - 1;
    } while (pairsRemaining != 0);
    heapSize = 0;
    pairsRemaining = g_EntityPathingPriorityPairCount;
    pairCursor = g_EntityPathingPriorityPairs;
    do {
      heapSize = heapSize + 1;
      PriorityPairHeap_SiftUp(heapSize,heapBase);
      pairCursor = pairCursor + 1;
      pairsRemaining = pairsRemaining - 1;
    } while (pairsRemaining != 0);
    do {
      rootPriority = heapBase->priority;
      LOCK();
      candidateEntity = pairCursor[-1].entity;
      pairCursor[-1].entity = heapBase->entity;
      UNLOCK();
      LOCK();
      swappedPriority = pairCursor[-1].priority;
      pairCursor[-1].priority = rootPriority;
      UNLOCK();
      heapBase->entity = candidateEntity;
      heapBase->priority = swappedPriority;
      heapSize = heapSize - 1;
      PriorityPairHeap_SiftDown(heapSize,heapBase);
      pairCursor = pairCursor + -1;
      pairsRemaining = g_EntityPathingPriorityPairCount;
      influencePair = g_EntityPathingPriorityPairs;
    } while (1 < heapSize);
    do {
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [*(int *)((int)(influencePair->entity->common).ownership.definitionOrClassRecord + 0x4c)])
                (influencePair->entity);
      pairsRemaining = pairsRemaining - 1;
      routesRemaining = g_EntityPathingPriorityPairCount;
      pairCursor = g_EntityPathingPriorityPairs;
      influencePair = influencePair + 1;
    } while (pairsRemaining != 0);
    do {
      targetWorldY = routeTarget.worldYQ12;
      targetWorldX = routeTarget.worldXQ12;
      candidateEntity = pairCursor->entity;
      if (pairCursor->priority == 1) {
        entityModelNode = (candidateEntity->common).ownership.modelNode;
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (((ModelDefinitionRuntimeSemanticView280 *)(candidateEntity->common).ownership.definitionOrClassRecord)->
                   placementRadiusOrClearanceDC,(entityModelNode->worldTransform).translation.y,
                   (entityModelNode->worldTransform).translation.x);
      }
      else if (candidateEntity == routeEntityRuntime) {
        routeTarget = EntityPathing_UpdateRouteSegment
                           (targetWorldY,targetWorldX,routeEntityRuntime,
                            (EntityPathingRouteEntityRuntimeView10 *)candidateEntity);
      }
      else {
        EntityPathing_UpdateRouteSegment
                  (0,0,routeEntityRuntime,(EntityPathingRouteEntityRuntimeView10 *)candidateEntity);
      }
      routesRemaining = routesRemaining - 1;
      pairCursor = pairCursor + 1;
    } while (routesRemaining != 0);
  }
  targetWorldY = routeTarget.worldYQ12;
  targetWorldX = routeTarget.worldXQ12;
  GridScratch_SwapPrimarySecondary();
  resolvedTarget.worldYQ12 = targetWorldY;
  resolvedTarget.worldXQ12 = targetWorldX;
  return resolvedTarget;
}


/* Address: 0x00534F50.
   Ownership: world/pathing/grid.
   Purpose: Converts a world point to the staggered scratch grid, computes the active squared radius, and drives
   the paired diagonal walkers to clear traversal flags across the complete circular footprint. Typed parameters:
   p3 worldXQ12→Q12, p4 worldYQ12→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p2 radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: GridFootprint_ClearTraversalFlagsDiagonalPositive,
   GridFootprint_ClearTraversalFlagsDiagonalNegative.
*/
void __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong wideProduct;
  int radiusColumnOrWalkerY;
  uint currentY;
  uint rowTermOrWalkerY;
  int rowOrWalkerY;
  uint cellCenterCoord;
  uint currentX;
  int walkerResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;
  
  radiusColumnOrWalkerY = radiusWorldUnits + g_GridInfluenceRadiusOffset6 + 499;
  g_GridInfluenceSquaredThreshold6 = radiusColumnOrWalkerY * radiusColumnOrWalkerY;
  rowTermOrWalkerY = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  radiusColumnOrWalkerY = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - rowTermOrWalkerY) + 0x800) >> 10;
  if ((((-1 < radiusColumnOrWalkerY) && (rowOrWalkerY = (int)(rowTermOrWalkerY * 2 + 0x800) >> 10, -1 < rowOrWalkerY)) &&
      (radiusColumnOrWalkerY < (int)g_GridScratchWidth)) && (rowOrWalkerY < (int)g_GridScratchHeight)) {
    centerCellCursor = g_GridScratchPrimary + g_GridScratchWidth * rowOrWalkerY + radiusColumnOrWalkerY;
    rowOrWalkerY = rowOrWalkerY * 0x400 + -0x600;
    wideProduct = (longlong)(rowOrWalkerY + (radiusColumnOrWalkerY * 0x400 + -0x600) * 2) * 0x901;
    cellCenterCoord = (int)((ulonglong)wideProduct >> 0x20) << 0x13 | (uint)wideProduct >> 0xd;
    wideProduct = (longlong)rowOrWalkerY * -1999;
    currentX = (int)((ulonglong)wideProduct >> 0x20) << 0x14 | (uint)wideProduct >> 0xc;
    rowTermOrWalkerY = cellCenterCoord;
    walkCursor = centerCellCursor;
    if ((centerCellCursor->stateMask & 0x80000000) == 0) {
      while ((radiusColumnOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,currentX,rowTermOrWalkerY,&walkCursor->stateMask),
             currentY = cellCenterCoord, oppositeWalkCursor = centerCellCursor, radiusColumnOrWalkerY != 0 &&
             (radiusColumnOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,currentX,rowTermOrWalkerY,&walkCursor->stateMask), radiusColumnOrWalkerY != 0)
             )) {
        walkCursor = walkCursor + -1;
        rowTermOrWalkerY = rowTermOrWalkerY - 0x240;
      }
      while ((radiusColumnOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,currentX,currentY,&oppositeWalkCursor->stateMask),
             radiusColumnOrWalkerY != 0 &&
             (radiusColumnOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,currentX,currentY,&oppositeWalkCursor->stateMask),
             radiusColumnOrWalkerY != 0))) {
        oppositeWalkCursor = oppositeWalkCursor + 1;
        currentY = currentY + 0x240;
      }
      rowOrWalkerY = cellCenterCoord - 0x120;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      radiusColumnOrWalkerY = cellCenterCoord + 0x120;
      oppositeWalkCursor = walkCursor;
      while (walkerResult = GridFootprint_ClearTraversalFlagsDiagonalPositive
                               (worldYQ12,worldXQ12,currentX + 499,rowOrWalkerY,&oppositeWalkCursor->stateMask),
            walkerResult != 0) {
        oppositeWalkCursor = oppositeWalkCursor + -1;
        rowOrWalkerY = rowOrWalkerY + -0x240;
      }
      while( true ) {
        walkCursor = walkCursor + 1;
        rowOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalPositive
                          (worldYQ12,worldXQ12,currentX + 499,radiusColumnOrWalkerY,&walkCursor->stateMask);
        if (rowOrWalkerY == 0) break;
        radiusColumnOrWalkerY = radiusColumnOrWalkerY + 0x240;
      }
      rowOrWalkerY = cellCenterCoord + 0x120;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      radiusColumnOrWalkerY = cellCenterCoord - 0x120;
      walkCursor = centerCellCursor;
      while (walkerResult = GridFootprint_ClearTraversalFlagsDiagonalNegative
                               (worldYQ12,worldXQ12,currentX - 500,rowOrWalkerY,&walkCursor->stateMask),
            walkerResult != 0) {
        walkCursor = walkCursor + 1;
        rowOrWalkerY = rowOrWalkerY + 0x240;
      }
      while( true ) {
        centerCellCursor = centerCellCursor + -1;
        rowOrWalkerY = GridFootprint_ClearTraversalFlagsDiagonalNegative
                          (worldYQ12,worldXQ12,currentX - 500,radiusColumnOrWalkerY,&centerCellCursor->stateMask);
        if (rowOrWalkerY == 0) break;
        radiusColumnOrWalkerY = radiusColumnOrWalkerY + -0x240;
      }
      return;
    }
  }
  return;
}


/* Address: 0x005369A0.
   Ownership: world/pathing/grid.
   Purpose: Resolves one entity or group member route segment against the scratch grid, applies the active
   traversal masks, clips and bisects long world-space spans, publishes low-channel influence bands along the
   route, and updates the destination when it changes.
   Local calls: GridPathCost_FindNearestUnblockedCell, GridPathLine_TestHexSegmentClearCf.
   Cross-module calls: GridInfluence_SetLowDistanceBandsAroundWorldPoint [world/pathing/influence],
   ArmyRuntime_SetPendingMoveTarget [gameplay/army/movement].
*/
WorldPositionXYEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView10 *routeEntityRuntime)

{
  ArmyMovementRuntime *entityMovement;
  longlong wideProductXOrY;
  longlong wideProductY;
  UQ12 segmentWorldXQ12;
  byte gridClassShift;
  int startColumnOrDeltaX;
  int targetColumn;
  dword columnLimitOrRadius;
  uint rowTermOrSubdivisions;
  int startRowOrDeltaY;
  int targetRow;
  dword rowLimit;
  UQ12 entityWorldYOrMidpoint;
  bool segmentClear;
  WorldPositionXYEaxEdx8 resolvedTarget;
  GridPathNearestCellEaxEbxCf9 nearestCell;
  GraphicsWorldCoordinateQ12 entityWorldX;
  UQ12 segmentWorldYQ12;
  ModelRuntimeNode *entityModelNode;
  
  entityModelNode = routeEntityRuntime->modelNode;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
    [routeEntityRuntime->modelDefinition->runtimeClassId4C])
            ((GameEntityRuntime *)routeEntityRuntime);
  wideProductXOrY = (longlong)(entityModelNode->worldTransform).translation.x * 0x1c6e9c;
  wideProductY = (longlong)(entityModelNode->worldTransform).translation.y * -0x20c8cc;
  rowTermOrSubdivisions = (int)((ulonglong)wideProductY >> 0x20) << 0xb | (uint)wideProductY >> 0x15;
  startColumnOrDeltaX = (int)((((int)((ulonglong)wideProductXOrY >> 0x20) << 0xc | (uint)wideProductXOrY >> 0x14) - rowTermOrSubdivisions) + 0x800) >>
          10;
  startRowOrDeltaY = (int)(rowTermOrSubdivisions * 2 + 0x800) >> 10;
  if (startColumnOrDeltaX < 1) {
    startColumnOrDeltaX = 1;
  }
  if (startRowOrDeltaY < 1) {
    startRowOrDeltaY = 1;
  }
  columnLimitOrRadius = startColumnOrDeltaX + 2U;
  if ((int)g_GridScratchWidth < (int)(startColumnOrDeltaX + 2U)) {
    columnLimitOrRadius = g_GridScratchWidth;
  }
  rowLimit = startRowOrDeltaY + 2U;
  if ((int)g_GridScratchHeight < (int)(startRowOrDeltaY + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  startColumnOrDeltaX = columnLimitOrRadius - 2;
  startRowOrDeltaY = rowLimit - 2;
  entityMovement = routeEntityRuntime->movementRuntime;
  if ((GameEntityRuntime *)routeEntityRuntime != sourceRouteEntityRuntime) {
    targetWorldXQ12 = entityMovement->movementWorldXQ12;
    targetWorldYQ12 = entityMovement->movementWorldYQ12;
  }
  rowTermOrSubdivisions = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  targetColumn = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - rowTermOrSubdivisions) + 0x800) >> 10;
  targetRow = (int)(rowTermOrSubdivisions * 2 + 0x800) >> 10;
  if (targetColumn < 1) {
    targetColumn = 1;
  }
  if (targetRow < 1) {
    targetRow = 1;
  }
  columnLimitOrRadius = targetColumn + 2U;
  if ((int)g_GridScratchWidth < (int)(targetColumn + 2U)) {
    columnLimitOrRadius = g_GridScratchWidth;
  }
  rowLimit = targetRow + 2U;
  if ((int)g_GridScratchHeight < (int)(targetRow + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  g_GridPathEntityClassMask = 1 << ((byte)entityMovement->factionIndex & 0x1f);
  gridClassShift = (byte)routeEntityRuntime->modelDefinition->gridClassification260;
  g_GridPathHighCostMask = 0x10000 << (gridClassShift & 0x1f);
  g_GridPathBlockingMask =
       0x100 << (gridClassShift & 0x1f) |
       0x1000000 << ((byte)routeEntityRuntime->modelDefinition->gridClassification264 & 0x1f);
  nearestCell = GridPathCost_FindNearestUnblockedCell(startRowOrDeltaY,startColumnOrDeltaX);
  targetColumn = nearestCell.selectedColumn;
  targetRow = nearestCell.selectedRow;
  if ((nearestCell.carry) ||
     (segmentClear = GridPathLine_TestHexSegmentClearCf
                         (0,startRowOrDeltaY,startColumnOrDeltaX,g_GridScratchPrimary + startRowOrDeltaY * g_GridScratchWidth + startColumnOrDeltaX,
                          g_GridScratchPrimary + (rowLimit - 2) * g_GridScratchWidth + (columnLimitOrRadius - 2)),
     targetColumn = columnLimitOrRadius - 2, targetRow = rowLimit - 2, segmentClear)) {
    startColumnOrDeltaX = targetRow * 0x400 + -0x600;
    wideProductXOrY = (longlong)(startColumnOrDeltaX + (targetColumn * 0x400 + -0x600) * 2) * 0x901;
    targetWorldXQ12 = (int)((ulonglong)wideProductXOrY >> 0x20) << 0x13 | (uint)wideProductXOrY >> 0xd;
    wideProductXOrY = (longlong)startColumnOrDeltaX * -1999;
    targetWorldYQ12 = (int)((ulonglong)wideProductXOrY >> 0x20) << 0x14 | (uint)wideProductXOrY >> 0xc;
  }
  columnLimitOrRadius = routeEntityRuntime->modelDefinition->placementRadiusOrClearanceDC;
  {
    /* Rewritten from the assembly (0x00536BB0-0x00536C89). The segment from the entity to the
       target is split in halves until each piece spans at most 0x240 on both axes (or 64 pieces
       are pending); the low-distance influence bands are stamped at the end of each piece. The
       original keeps the pending pieces as pushed frames on the machine stack, processing the
       half towards the target first; the decompiled version lost that stack and used the
       return address 0x00536C13 as a coordinate. */
    struct { int startX; int startY; int endX; int endY; } pieces[64];
    int pending = 1;
    pieces[0].startX = (routeEntityRuntime->modelNode->worldTransform).translation.x;
    pieces[0].startY = (routeEntityRuntime->modelNode->worldTransform).translation.y;
    pieces[0].endX = targetWorldXQ12;
    pieces[0].endY = targetWorldYQ12;
    while (pending != 0) {
      int top = pending - 1;
      int deltaX = pieces[top].endX - pieces[top].startX;
      int deltaY2 = pieces[top].endY - pieces[top].startY;
      if (deltaX < 0) {
        deltaX = -deltaX;
      }
      if (deltaY2 < 0) {
        deltaY2 = -deltaY2;
      }
      if (pending >= 0x40 || (deltaX <= 0x240 && deltaY2 <= 0x240)) {
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (columnLimitOrRadius,pieces[top].endY,pieces[top].endX);
        pending = pending - 1;
      }
      else {
        int midX = (pieces[top].endX + pieces[top].startX) >> 1;
        int midY = (pieces[top].endY + pieces[top].startY) >> 1;
        pieces[pending].startX = midX;
        pieces[pending].startY = midY;
        pieces[pending].endX = pieces[top].endX;
        pieces[pending].endY = pieces[top].endY;
        pieces[top].endX = midX;
        pieces[top].endY = midY;
        pending = pending + 1;
      }
    }
  }
  entityMovement = routeEntityRuntime->movementRuntime;
  if (((GameEntityRuntime *)routeEntityRuntime != sourceRouteEntityRuntime) &&
     ((targetWorldXQ12 != entityMovement->movementWorldXQ12 ||
      (targetWorldYQ12 != entityMovement->movementWorldYQ12)))) {
    ArmyRuntime_SetPendingMoveTarget(targetWorldYQ12,targetWorldXQ12,entityMovement);
  }
  resolvedTarget.worldYQ12 = targetWorldYQ12;
  resolvedTarget.worldXQ12 = targetWorldXQ12;
  return resolvedTarget;
}


/* Address: 0x00533D60.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test world point reachability carry-flag result.
   Local calls: GridScratch_TestConnectedReachabilityRecursiveCfRegs.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestWorldPointReachabilityCf
          (uint traversalMask,GraphicsWorldCoordinateQ12 sourceWorldYQ12,
          GraphicsWorldCoordinateQ12 sourceWorldXQ12,GraphicsWorldCoordinateQ12 targetWorldYQ12,
          GraphicsWorldCoordinateQ12 targetWorldXQ12)

{
  GridScratchCell *sourceCell;
  dword scratchWidth;
  int cellsRemaining;
  uint scaledRowTerm;
  GridScratchCell *targetCell;
  GridScratchCell *clearCursor;
  bool unreachable;
  
  scratchWidth = g_GridScratchWidth;
  scaledRowTerm = (int)((ulonglong)((longlong)sourceWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)sourceWorldYQ12 * -0x20c8cc) >> 0x15;
  sourceCell = g_GridScratchPrimary +
               ((int)(scaledRowTerm * 2 + 0x800) >> 10) * g_GridScratchWidth +
               ((int)((((int)((ulonglong)((longlong)sourceWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                       (uint)((longlong)sourceWorldXQ12 * 0x1c6e9c) >> 0x14) - scaledRowTerm) + 0x800) >> 10
               );
  scaledRowTerm = (int)((ulonglong)((longlong)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  targetCell = g_GridScratchPrimary +
                ((int)(scaledRowTerm * 2 + 0x800) >> 10) * g_GridScratchWidth +
                ((int)((((int)((ulonglong)((longlong)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                        (uint)((longlong)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - scaledRowTerm) + 0x800) >>
                10);
  cellsRemaining = g_GridScratchHeight * g_GridScratchWidth;
  clearCursor = g_GridScratchPrimary;
  do {
    clearCursor->stateMask = clearCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[1].stateMask = clearCursor[1].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[2].stateMask = clearCursor[2].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[3].stateMask = clearCursor[3].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[4].stateMask = clearCursor[4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[5].stateMask = clearCursor[5].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[6].stateMask = clearCursor[6].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[7].stateMask = clearCursor[7].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[8].stateMask = clearCursor[8].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[9].stateMask = clearCursor[9].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[10].stateMask = clearCursor[10].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[0xb].stateMask = clearCursor[0xb].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[0xc].stateMask = clearCursor[0xc].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[0xd].stateMask = clearCursor[0xd].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[0xe].stateMask = clearCursor[0xe].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[0xf].stateMask = clearCursor[0xf].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor = clearCursor + 0x10;
    cellsRemaining = cellsRemaining + -0x10;
  } while (cellsRemaining != 0);
  unreachable = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                    (traversalMask | 0x80000001,scratchWidth << 3,&targetCell->stateMask,
                     &sourceCell->stateMask);
  return unreachable;
}


/* Address: 0x00534660.
   Ownership: world/pathing/grid.
   Purpose: Backtracks from a starting scratch record by repeatedly selecting the lowest-cost one of six neighbors.
   It uses the segment-clear test to preserve a direct route when possible and returns the selected grid coordinate
   through the engine register convention. Typed parameters: p3 targetRow→FieldGridCellCoordinate_V331, p4
   targetColumn→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p2 param_3→FieldGridRegionMask.
   Local calls: GridPathLine_TestHexSegmentClearCf.
*/
GridPathBacktrackEaxEbxEcxCf13 __thandor_eax_cf_preserve_edx
GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate targetRow,
          FieldGridCellCoordinate targetColumn,GridScratchCell *startCell)

{
  dword scratchWidth;
  GridScratchCell *currentCell;
  uint selectedCellIndex;
  uint bestNeighborCost;
  GridScratchCell *rowAboveCell;
  GridScratchCell *bestNeighborCell;
  bool segmentClear;
  GridPathBacktrackEaxEbxEcxCf13 selectedCell;
  GridPathBacktrackEaxEbxEcxCf13 terminalResult;
  
  scratchWidth = g_GridScratchWidth;
  terminalResult.selectedRow = g_GridScratchWidth * 8;
  bestNeighborCell = startCell;
  do {
    currentCell = bestNeighborCell;
    rowAboveCell = currentCell + -scratchWidth;
    if ((currentCell->stateMask & g_GridPathHighCostMask) != 0) {
      callerBlockingMask = 0;
    }
    bestNeighborCost = currentCell->pathCost;
    bestNeighborCell = (GridScratchCell *)0x0;
    if (rowAboveCell->pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell->pathCost;
      bestNeighborCell = rowAboveCell;
    }
    if (rowAboveCell[1].pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell[1].pathCost;
      bestNeighborCell = rowAboveCell + 1;
    }
    if (rowAboveCell[scratchWidth - 1].pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell[scratchWidth - 1].pathCost;
      bestNeighborCell = rowAboveCell + (scratchWidth - 1);
    }
    if (rowAboveCell[scratchWidth + 1].pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell[scratchWidth + 1].pathCost;
      bestNeighborCell = rowAboveCell + scratchWidth + 1;
    }
    if (rowAboveCell[scratchWidth * 2 + -1].pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell[scratchWidth * 2 + -1].pathCost;
      bestNeighborCell = rowAboveCell + scratchWidth * 2 + -1;
    }
    if (rowAboveCell[scratchWidth * 2].pathCost < bestNeighborCost) {
      bestNeighborCell = rowAboveCell + scratchWidth * 2;
    }
    if (bestNeighborCell == (GridScratchCell *)0x0)
    goto GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn;
    segmentClear = GridPathLine_TestHexSegmentClearCf
                      (callerBlockingMask,targetRow,targetColumn,startCell,bestNeighborCell);
  } while (!segmentClear);
  if (currentCell == startCell) {
    currentCell = bestNeighborCell;
  }
GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn:
  if (currentCell->pathCost != 0) {
    selectedCellIndex = (uint)((int)currentCell - (int)g_GridScratchPrimary) >> 3;
    selectedCell.selectedRow = selectedCellIndex / g_GridScratchWidth;
    selectedCell.selectedColumn = selectedCellIndex % g_GridScratchWidth;
    selectedCell.routeStateMask = callerBlockingMask;
    selectedCell.carry = false;
    return selectedCell;
  }
  terminalResult.selectedColumn = (FieldGridCellCoordinate)currentCell;
  terminalResult.carry = true;
  terminalResult.routeStateMask = callerBlockingMask;
  return terminalResult;
}


/* Address: 0x00534960.
   Ownership: world/pathing/grid.
   Purpose: Typed parameters: p4 row→FieldGridCellCoordinate_V331, p5 column→FieldGridCellCoordinate_V331. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p2 param_3→GridPathUnreachableReferenceRow32_V345, p3
   param_4→GridPathUnreachableReferenceColumn32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: GridPathRegion_MarkUnreachableRecursive.
*/
GridPathMarkedRegionCellRegisterResult
GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column)

{
  ulonglong markedCellIndex;
  int rowBaseIndex;
  GridPathMarkedRegionCellRegisterResult markedCell;
  GridPathUnreachableRecursiveEdiEdx8 recursionResult;
  
  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  recursionResult = GridPathRegion_MarkUnreachableRecursive
                    (g_GridScratchWidth << 3,g_GridScratchPrimary + rowBaseIndex + column,0x7fffffff
                     ,(rowBaseIndex + column) * 8);
  markedCellIndex = THANDOR_BITCAST(GridPathUnreachableRecursiveEdiEdx8, ulonglong, recursionResult) >> 3 & 0x1fffffff;
  markedCell.selectedRow = (FieldGridCellCoordinate)(markedCellIndex / g_GridScratchWidth);
  markedCell.selectedColumn = (FieldGridCellCoordinate)(markedCellIndex % (ulonglong)g_GridScratchWidth);
  return markedCell;
}


/* Address: 0x005333B0.
   Ownership: world/pathing/grid.
   Purpose: Copies exactly gridWidth * gridHeight * 2 dwords from the primary scratch buffer to the secondary
   scratch buffer. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Copies scratchWidth*scratchHeight*2 dwords = exactly
   two dwords per GridScratchCell_V419 record.
*/
void __cdecl GridScratch_CopyPrimaryToSecondary(void)

{
  int scratchDwordsRemaining;
  dword *primaryReadCursor;
  dword *secondaryWriteCursor;
  
  scratchDwordsRemaining = g_GridScratchWidth * g_GridScratchHeight * 2;
  primaryReadCursor = &g_GridScratchPrimary->stateMask;
  secondaryWriteCursor = &g_GridScratchSecondary->stateMask;
  for (; scratchDwordsRemaining != 0; scratchDwordsRemaining = scratchDwordsRemaining + -1) {
    *secondaryWriteCursor = *primaryReadCursor;
    primaryReadCursor = primaryReadCursor + 1;
    secondaryWriteCursor = secondaryWriteCursor + 1;
  }
  return;
}

/* Address: 0x005333E0.
   Ownership: world/pathing/grid.
   Purpose: Atomically swaps the primary and secondary grid scratch buffer pointers.
*/
void __thandor_preserve_eax GridScratch_SwapPrimarySecondary(void)

{
  GridScratchCell *previousSecondaryBuffer;
  
  previousSecondaryBuffer = g_GridScratchSecondary;
  LOCK();
  g_GridScratchSecondary = g_GridScratchPrimary;
  UNLOCK();
  g_GridScratchPrimary = previousSecondaryBuffer;
  return;
}


/* Address: 0x00533580.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch flood fill connected cells register result.
*/
void __thandor_void_preserve_eax_ecx_edx
GridScratch_FloodFillConnectedCellsRegs
          (GridScratchStateMask traversalMask,dword rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *spanLeftOrPrevRowCursor;
  GridScratchCell *nextRowCursor;
  
  if ((currentCell->stateMask & 0x80000001) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    spanLeftOrPrevRowCursor = currentCell;
    while (spanLeftOrPrevRowCursor = spanLeftOrPrevRowCursor + -1, (spanLeftOrPrevRowCursor->stateMask & traversalMask) == 0) {
      spanLeftOrPrevRowCursor->stateMask = spanLeftOrPrevRowCursor->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    while (currentCell = currentCell + 1, (currentCell->stateMask & traversalMask) == 0) {
      currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    nextRowCursor = (GridScratchCell *)((int)&spanLeftOrPrevRowCursor->stateMask + rowStrideBytes);
    spanLeftOrPrevRowCursor = (GridScratchCell *)((int)spanLeftOrPrevRowCursor + (8 - rowStrideBytes));
    do {
      if ((spanLeftOrPrevRowCursor->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,spanLeftOrPrevRowCursor);
      }
      spanLeftOrPrevRowCursor = spanLeftOrPrevRowCursor + 1;
    } while (spanLeftOrPrevRowCursor <= (GridScratchCell *)((int)currentCell - rowStrideBytes));
    do {
      if ((nextRowCursor->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,nextRowCursor);
      }
      nextRowCursor = nextRowCursor + 1;
    } while (nextRowCursor <
             (GridScratchCell *)
             ((int)&((GridScratchCell *)((int)currentCell - rowStrideBytes))->stateMask +
             rowStrideBytes * 2));
  }
  return;
}


/* Address: 0x00533C50.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test connected reachability recursive carry-flag result register result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestConnectedReachabilityRecursiveCfRegs
          (dword traversalMask,dword rowStrideBytes,dword *currentCell,dword *targetCell)

{
  dword *secondRowCursor;
  uint *spanLeftBoundary;
  uint *spanLeftCell;
  dword *firstRowCursor;
  bool subRegionUnreachable;
  
  *currentCell = *currentCell | 1;
  spanLeftCell = currentCell;
  if (targetCell == currentCell) {
    return false;
  }
  while( true ) {
    spanLeftBoundary = spanLeftCell + -2;
    if (targetCell == spanLeftBoundary) {
      return false;
    }
    if ((*spanLeftBoundary & traversalMask) != 0) break;
    *spanLeftBoundary = *spanLeftBoundary | 1;
    spanLeftCell = spanLeftBoundary;
  }
  while( true ) {
    currentCell = currentCell + 2;
    if (targetCell == currentCell) {
      return false;
    }
    if ((*currentCell & traversalMask) != 0) break;
    *currentCell = *currentCell | 1;
  }
  if (targetCell <= spanLeftBoundary) {
    secondRowCursor = (dword *)(rowStrideBytes + (int)spanLeftBoundary);
    firstRowCursor = (dword *)((int)spanLeftBoundary + (8 - rowStrideBytes));
    while (((*firstRowCursor & traversalMask) != 0 ||
           (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                              (traversalMask,rowStrideBytes,firstRowCursor,targetCell), subRegionUnreachable))) {
      firstRowCursor = firstRowCursor + 2;
      if ((dword *)((int)currentCell - rowStrideBytes) < firstRowCursor) {
        while (((*secondRowCursor & traversalMask) != 0 ||
               (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                                  (traversalMask,rowStrideBytes,secondRowCursor,targetCell), subRegionUnreachable))) {
          secondRowCursor = secondRowCursor + 2;
          if ((dword *)((int)((int)currentCell - rowStrideBytes) + rowStrideBytes * 2) <= secondRowCursor) {
            return true;
          }
        }
        return false;
      }
    }
    return false;
  }
  secondRowCursor = (dword *)((int)spanLeftCell - rowStrideBytes);
  firstRowCursor = (dword *)((int)spanLeftBoundary + rowStrideBytes);
  while (((*firstRowCursor & traversalMask) != 0 ||
         (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                            (traversalMask,rowStrideBytes,firstRowCursor,targetCell), subRegionUnreachable))) {
    firstRowCursor = firstRowCursor + 2;
    if ((dword *)((int)currentCell + rowStrideBytes) <= firstRowCursor) {
      while (((*secondRowCursor & traversalMask) != 0 ||
             (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                                (traversalMask,rowStrideBytes,secondRowCursor,targetCell), subRegionUnreachable))) {
        secondRowCursor = secondRowCursor + 2;
        if ((dword *)((int)((int)currentCell + rowStrideBytes) + rowStrideBytes * -2) < secondRowCursor) {
          return true;
        }
      }
      return false;
    }
  }
  return false;
}


/* Address: 0x00533EF0.
   Ownership: world/pathing/grid.
   Purpose: Runs a bounded queue-based propagation over the six neighboring grid records. Typed parameters: p3
   queueBaseOffset→FieldGridByteOffset. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p4 startRow→FieldGridCellCoordinate_V331, p5 startColumn→FieldGridCellCoordinate_V331.
*/
void __thandor_void_preserve_eax_ecx_edx
GridPathCost_PropagateWeightedHexNeighbors
          (GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  GridPathCost currentCost;
  GridScratchStateMask neighborState;
  dword scratchWidth;
  uint neighborCost;
  GridScratchCell **queueReadCursor;
  GridScratchCell *neighborCell;
  GridScratchCell *secondaryNeighborCell;
  GridScratchCell **queueWriteCursor;
  
  scratchWidth = g_GridScratchWidth;
  queueReadCursor = g_GridPathCostQueueBegin;
  queueWriteCursor = g_GridPathCostQueueBegin + 1;
  neighborCell = g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumn;
  *g_GridPathCostQueueBegin = neighborCell;
  g_GridPathCostQueuePassBoundary = queueReadCursor;
  neighborCell->pathCost = 0;
  g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + 0x10000;
GridPathCost_ProcessNextQueuedCell:
  do {
    do {
      while( true ) {
        if (queueReadCursor == queueWriteCursor) {
          return;
        }
        if (queueReadCursor < g_GridPathCostQueuePassBoundary) break;
        g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + 0x10000;
        if (originCell->pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[-scratchWidth].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[1 - scratchWidth].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[-1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[scratchWidth - 1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[scratchWidth].pathCost < 0x7fffffff) {
          return;
        }
        remainingPasses = remainingPasses + -1;
        if (remainingPasses == 0) {
          return;
        }
      }
      neighborCell = *queueReadCursor;
      queueReadCursor = queueReadCursor + 1;
    } while ((neighborCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
    currentCost = neighborCell->pathCost;
    neighborCell = neighborCell + -scratchWidth;
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + 4;
    if (-1 < (int)neighborState) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + 3;
        if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_SkipBlockedNorthNeighbor;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + 0xc;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
        *queueWriteCursor = neighborCell;
        neighborCell->pathCost = neighborCost;
        queueWriteCursor = queueWriteCursor + 1;
      }
    }
GridPathCost_SkipBlockedNorthNeighbor:
    secondaryNeighborCell = neighborCell + 1;
    neighborState = secondaryNeighborCell->stateMask;
    neighborCost = currentCost + 4;
    if (-1 < (int)neighborState) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + 3;
        if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_SkipBlockedEastNeighbor;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + 0xc;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell[1].pathCost)) {
        *queueWriteCursor = secondaryNeighborCell;
        neighborCell[1].pathCost = neighborCost;
        queueWriteCursor = queueWriteCursor + 1;
      }
    }
GridPathCost_SkipBlockedEastNeighbor:
    secondaryNeighborCell = secondaryNeighborCell + scratchWidth;
    neighborState = secondaryNeighborCell->stateMask;
    neighborCost = currentCost + 4;
    if (-1 < (int)neighborState) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + 3;
        if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_SkipBlockedSouthEastNeighbor;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + 0xc;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < secondaryNeighborCell->pathCost)) {
        *queueWriteCursor = secondaryNeighborCell;
        secondaryNeighborCell->pathCost = neighborCost;
        queueWriteCursor = queueWriteCursor + 1;
      }
    }
GridPathCost_SkipBlockedSouthEastNeighbor:
    neighborCell = secondaryNeighborCell + -2;
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + 4;
    if (-1 < (int)neighborState) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + 3;
        if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_SkipBlockedSouthWestNeighbor;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + 0xc;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < secondaryNeighborCell[-2].pathCost)) {
        *queueWriteCursor = neighborCell;
        secondaryNeighborCell[-2].pathCost = neighborCost;
        queueWriteCursor = queueWriteCursor + 1;
      }
    }
GridPathCost_SkipBlockedSouthWestNeighbor:
    neighborCell = neighborCell + scratchWidth;
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + 4;
    if (-1 < (int)neighborState) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + 3;
        if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_SkipBlockedWestNeighbor;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + 0xc;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
        *queueWriteCursor = neighborCell;
        neighborCell->pathCost = neighborCost;
        queueWriteCursor = queueWriteCursor + 1;
      }
    }
GridPathCost_SkipBlockedWestNeighbor:
    neighborState = neighborCell[1].stateMask;
    neighborCost = currentCost + 4;
  } while ((int)neighborState < 0);
  if ((g_GridPathEntityClassMask & neighborState) != 0) {
    neighborCost = currentCost + 3;
    if ((g_GridPathBlockingMask & neighborState) != 0) goto GridPathCost_ProcessNextQueuedCell;
    if ((g_GridPathHighCostMask & neighborState) != 0) {
      neighborCost = currentCost + 0xc;
    }
  }
  if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell[1].pathCost)) {
    *queueWriteCursor = neighborCell + 1;
    neighborCell[1].pathCost = neighborCost;
    queueWriteCursor = queueWriteCursor + 1;
  }
  goto GridPathCost_ProcessNextQueuedCell;
}


/* Address: 0x00534200.
   Ownership: world/pathing/grid.
   Purpose: Clears traversal bit zero and writes INT_MAX cost values for every grid-scratch record, sixteen records
   per unrolled iteration. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Per 8-byte scratch record, clears stateMask
   bit 0 and resets pathCost to INT_MAX.
*/
void GridScratch_ResetTraversalFlagsAndCosts(void)

{
  uint cellsRemaining;
  dword *scratchRecordCursor;
  bool fullRecordBlockRemaining;
  
  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  scratchRecordCursor = &g_GridScratchPrimary->stateMask;
  do {
    *scratchRecordCursor = *scratchRecordCursor & 0xfffffffe;
    scratchRecordCursor[1] = 0x7fffffff;
    scratchRecordCursor[2] = scratchRecordCursor[2] & 0xfffffffe;
    scratchRecordCursor[3] = 0x7fffffff;
    scratchRecordCursor[4] = scratchRecordCursor[4] & 0xfffffffe;
    scratchRecordCursor[5] = 0x7fffffff;
    scratchRecordCursor[6] = scratchRecordCursor[6] & 0xfffffffe;
    scratchRecordCursor[7] = 0x7fffffff;
    scratchRecordCursor[8] = scratchRecordCursor[8] & 0xfffffffe;
    scratchRecordCursor[9] = 0x7fffffff;
    scratchRecordCursor[10] = scratchRecordCursor[10] & 0xfffffffe;
    scratchRecordCursor[0xb] = 0x7fffffff;
    scratchRecordCursor[0xc] = scratchRecordCursor[0xc] & 0xfffffffe;
    scratchRecordCursor[0xd] = 0x7fffffff;
    scratchRecordCursor[0xe] = scratchRecordCursor[0xe] & 0xfffffffe;
    scratchRecordCursor[0xf] = 0x7fffffff;
    scratchRecordCursor[0x10] = scratchRecordCursor[0x10] & 0xfffffffe;
    scratchRecordCursor[0x11] = 0x7fffffff;
    scratchRecordCursor[0x12] = scratchRecordCursor[0x12] & 0xfffffffe;
    scratchRecordCursor[0x13] = 0x7fffffff;
    scratchRecordCursor[0x14] = scratchRecordCursor[0x14] & 0xfffffffe;
    scratchRecordCursor[0x15] = 0x7fffffff;
    scratchRecordCursor[0x16] = scratchRecordCursor[0x16] & 0xfffffffe;
    scratchRecordCursor[0x17] = 0x7fffffff;
    scratchRecordCursor[0x18] = scratchRecordCursor[0x18] & 0xfffffffe;
    scratchRecordCursor[0x19] = 0x7fffffff;
    scratchRecordCursor[0x1a] = scratchRecordCursor[0x1a] & 0xfffffffe;
    scratchRecordCursor[0x1b] = 0x7fffffff;
    scratchRecordCursor[0x1c] = scratchRecordCursor[0x1c] & 0xfffffffe;
    scratchRecordCursor[0x1d] = 0x7fffffff;
    scratchRecordCursor[0x1e] = scratchRecordCursor[0x1e] & 0xfffffffe;
    scratchRecordCursor[0x1f] = 0x7fffffff;
    scratchRecordCursor = scratchRecordCursor + 0x20;
    fullRecordBlockRemaining = 0xf < cellsRemaining;
    cellsRemaining = cellsRemaining - 0x10;
  } while (fullRecordBlockRemaining && cellsRemaining != 0);
  return;
}

/* Address: 0x00534780.
   Ownership: world/pathing/grid.
   Purpose: Recursively marks a connected region with scratch flag bit 0 while stopping at finite-cost, hard-
   boundary, ownership, or blocking-mask cells. The traversal expands across the verified six-neighbor hex
   topology.
*/
GridPathUnreachableRecursiveEdiEdx8 __thandor_eax_edx_cf_preserve_ecx
GridPathRegion_MarkUnreachableRecursive
          (dword rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          dword bestCellByteOffset)

{
  GridScratchStateMask cellState;
  uint spanByteOffset;
  GridPathCost referenceDistance;
  GridScratchCell *probeOrRightEndCell;
  int spanLength;
  uint spanColumn;
  int columnDelta;
  GridPathCost columnDistance;
  GridScratchCell *leftEndCell;
  GridScratchCell *probeOrRowCursor;
  GridPathUnreachableRecursiveEdiEdx8 bestResult;
  GridPathCost updatedBestCost;
  
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  probeOrRowCursor = currentCell + 1;
  probeOrRightEndCell = currentCell + -1;
  do {
    leftEndCell = probeOrRightEndCell;
    cellState = leftEndCell->stateMask;
    if ((leftEndCell->pathCost < 0x7fffffff) || ((int)cellState < 0))
    goto GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary;
    leftEndCell->stateMask = leftEndCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    probeOrRightEndCell = leftEndCell + -1;
  } while (((g_GridPathEntityClassMask & cellState) == 0) || ((g_GridPathBlockingMask & cellState) == 0));
  leftEndCell->stateMask = leftEndCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary:
  do {
    probeOrRightEndCell = probeOrRowCursor;
    cellState = probeOrRightEndCell->stateMask;
    if ((probeOrRightEndCell->pathCost < 0x7fffffff) || ((int)cellState < 0))
    goto GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows;
    probeOrRightEndCell->stateMask = probeOrRightEndCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    probeOrRowCursor = probeOrRightEndCell + 1;
  } while (((g_GridPathEntityClassMask & cellState) == 0) || ((g_GridPathBlockingMask & cellState) == 0));
  probeOrRightEndCell->stateMask = probeOrRightEndCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows:
  spanByteOffset = (int)leftEndCell + (8 - (int)g_GridScratchPrimary);
  spanLength = ((uint)((int)probeOrRightEndCell - (int)leftEndCell) >> 3) - 2;
  spanColumn = (spanByteOffset >> 3) % g_GridScratchWidth;
  referenceDistance = (spanByteOffset >> 3) / g_GridScratchWidth - g_GridPathUnreachableRegionReferenceRow;
  if ((int)referenceDistance < 0) {
    referenceDistance = -referenceDistance;
    columnDistance = spanColumn - g_GridPathUnreachableRegionReferenceColumn;
    if ((int)columnDistance < 0) {
      columnDelta = columnDistance + spanLength;
      if (columnDelta < 0) {
        referenceDistance = referenceDistance - columnDelta;
        spanByteOffset = spanByteOffset + spanLength * 8;
      }
      else {
        spanByteOffset = spanByteOffset + (columnDelta - spanLength) * -8;
      }
    }
    else if ((int)referenceDistance < (int)columnDistance) {
      referenceDistance = columnDistance;
    }
  }
  else {
    columnDelta = spanColumn - g_GridPathUnreachableRegionReferenceColumn;
    if (columnDelta < 0) {
      columnDelta = columnDelta + spanLength;
      if (columnDelta < 0) {
        spanByteOffset = spanByteOffset + spanLength * 8;
        if ((int)referenceDistance < -columnDelta) {
          referenceDistance = -columnDelta;
        }
      }
      else {
        spanByteOffset = spanByteOffset + (columnDelta - spanLength) * -8;
      }
    }
    else {
      referenceDistance = referenceDistance + columnDelta;
    }
  }
  updatedBestCost = bestCost;
  if ((int)referenceDistance < (int)bestCost) {
    updatedBestCost = referenceDistance;
    bestCellByteOffset = spanByteOffset;
  }
  bestResult.bestCost = updatedBestCost;
  bestResult.bestCellByteOffset = bestCellByteOffset;
  probeOrRowCursor = (GridScratchCell *)((int)leftEndCell + (8 - rowStrideBytes));
  do {
    cellState = probeOrRowCursor->stateMask;
    if (((0x7ffffffe < probeOrRowCursor->pathCost) && ((cellState & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & cellState) == 0 || ((g_GridPathBlockingMask & cellState) == 0)))) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,probeOrRowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    probeOrRowCursor = probeOrRowCursor + 1;
  } while (probeOrRowCursor <= (GridScratchCell *)((int)probeOrRightEndCell - rowStrideBytes));
  probeOrRowCursor = (GridScratchCell *)((int)&leftEndCell->stateMask + rowStrideBytes);
  do {
    cellState = probeOrRowCursor->stateMask;
    if (((0x7ffffffe < probeOrRowCursor->pathCost) && ((cellState & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & cellState) == 0 || ((g_GridPathBlockingMask & cellState) == 0)))) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,probeOrRowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    probeOrRowCursor = probeOrRowCursor + 1;
  } while (probeOrRowCursor < (GridScratchCell *)
                     ((int)&((GridScratchCell *)((int)probeOrRightEndCell - rowStrideBytes))->stateMask +
                     rowStrideBytes * 2));
  return bestResult;
}


/* Address: 0x00534E70.
   Ownership: world/pathing/grid.
   Purpose: Walks one executable-defined grid diagonal while the cell center remains inside the active squared
   radius. Each visited record clears scratch bits 31 and 0 and increments its companion count. Typed parameters:
   p2 centerX→FieldGridCellCoordinate_V331, p3 centerY→FieldGridCellCoordinate_V331, p4
   currentX→FieldGridCellCoordinate_V331, p5 currentY→FieldGridCellCoordinate_V331. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
          FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord)

{
  int nextCount;
  int squaredXDistanceMetric;
  int visitedCount;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  nextCount = 0;
  do {
    visitedCount = nextCount;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return visitedCount;
    }
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    currentX = currentX + -999;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * 4 + -2;
    nextCount = visitedCount + 1;
  } while ((*scratchRecord & 0x80000000) == 0);
  return visitedCount;
}


/* Address: 0x00534EE0.
   Ownership: world/pathing/grid.
   Purpose: Mirrors the negative diagonal walker in the opposite executable-defined grid direction, clearing
   scratch bits 31 and 0 and incrementing each visited companion count inside the active squared radius. Typed
   parameters: p2 centerX→FieldGridCellCoordinate_V331, p3 centerY→FieldGridCellCoordinate_V331, p4
   currentX→FieldGridCellCoordinate_V331, p5 currentY→FieldGridCellCoordinate_V331. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
          FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord)

{
  int nextCount;
  int squaredXDistanceMetric;
  int visitedCount;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  nextCount = 0;
  do {
    visitedCount = nextCount;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return visitedCount;
    }
    currentX = currentX + 999;
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * -4 + 2;
    nextCount = visitedCount + 1;
  } while ((*scratchRecord & 0x80000000) == 0);
  return visitedCount;
}


/* Address: 0x005363C0.
   Ownership: world/pathing/grid.
   Purpose: Recursively sets scratch bit 0 across a connected region whose cells pass the verified hard-boundary
   and influence-mask test. Expansion follows the six-neighbor staggered-grid topology.
*/
void __thandor_void_preserve_eax_ecx_edx
GridReachability_MarkOpenRegionRecursive(dword rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *unusedScanCell;
  GridScratchCell *prevRowEnd;
  GridScratchCell *spanLeftCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *nextRowCursor;
  
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  spanLeftCell = currentCell;
  while (spanLeftCell = spanLeftCell + -1, (spanLeftCell->stateMask & 0xf0007f01) == 0) {
    spanLeftCell->stateMask = spanLeftCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  while (currentCell = currentCell + 1, (currentCell->stateMask & 0xf0007f01) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  nextRowCursor = (GridScratchCell *)((int)&spanLeftCell->stateMask + rowStrideBytes);
  prevRowEnd = (GridScratchCell *)((int)currentCell - rowStrideBytes);
  prevRowCursor = (GridScratchCell *)((int)spanLeftCell + (8 - rowStrideBytes));
  do {
    if ((prevRowCursor->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,prevRowCursor);
    }
    prevRowCursor = prevRowCursor + 1;
  } while (prevRowCursor <= prevRowEnd);
  do {
    if ((nextRowCursor->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,nextRowCursor);
    }
    nextRowCursor = nextRowCursor + 1;
  } while (nextRowCursor < (GridScratchCell *)((int)&prevRowEnd->stateMask + rowStrideBytes * 2));
  return;
}


/* Address: 0x00536440.
   Ownership: world/pathing/grid.
   Purpose: Recursively clears scratch bit 0 across connected cells that have a nonzero companion cost, following
   the same six-neighbor topology.
*/
void __thandor_void_preserve_eax_ecx_edx
GridReachability_ClearCostedRegionRecursive(dword rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *unusedScanCell;
  GridScratchCell *spanRightCell;
  GridScratchCell *prevRowEnd;
  GridScratchCell *spanLeftCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *nextRowCursor;
  
  currentCell->stateMask = currentCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  spanRightCell = currentCell + 1;
  while ((spanLeftCell = currentCell + -1, (spanLeftCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 &&
         (currentCell[-1].pathCost != 0))) {
    spanLeftCell->stateMask = spanLeftCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    currentCell = spanLeftCell;
  }
  for (; ((spanRightCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && (spanRightCell->pathCost != 0));
      spanRightCell = spanRightCell + 1) {
    spanRightCell->stateMask = spanRightCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  nextRowCursor = (GridScratchCell *)((int)&spanLeftCell->stateMask + rowStrideBytes);
  prevRowEnd = (GridScratchCell *)((int)spanRightCell - rowStrideBytes);
  prevRowCursor = (GridScratchCell *)((int)spanLeftCell + (8 - rowStrideBytes));
  do {
    if (((prevRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) && (prevRowCursor->pathCost != 0)
       ) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,prevRowCursor);
    }
    prevRowCursor = prevRowCursor + 1;
  } while (prevRowCursor <= prevRowEnd);
  do {
    if (((nextRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) &&
       (nextRowCursor->pathCost != 0)) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,nextRowCursor);
    }
    nextRowCursor = nextRowCursor + 1;
  } while (nextRowCursor < (GridScratchCell *)((int)&prevRowEnd->stateMask + rowStrideBytes * 2));
  return;
}


/* Address: 0x005342F0.
   Ownership: world/pathing/grid.
   Purpose: Returns the requested grid cell when it or one of its six immediate neighbors is not blocked by the
   active mask. Otherwise it scans a clipped 32-by-32 neighborhood and selects the nearest unblocked coordinate
   using the executable's mixed axial-distance comparison. Typed parameters: p2 gridY→FieldGridCellCoordinate_V331,
   p3 gridX→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
GridPathNearestCellEaxEbxCf9 __thandor_eax_cf_preserve_ecx_edx
GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  int cellIndexOrMinColumn;
  int scanColumn;
  int hexDistance;
  dword maxColumn;
  int scanWidth;
  int columnsRemaining;
  GridScratchStateMask blockedMask;
  dword maxRow;
  int bestHexDistance;
  undefined4 unaff_EBX = 0; /* open cell: the original leaves EBX unchanged; callers read EAX/EBX only when CF is set */
  int searchRow;
  int rowDelta;
  GridScratchCell *scanCell;
  GridPathNearestCellEaxEbxCf9 openCellResult;
  GridPathNearestCellEaxEbxCf9 nearestResult;
  GridPathNearestCellEaxEbxCf9 fallbackResult;
  GridScratchCell *rowStartCell;
  int rowsRemaining;
  int bestRow;
  int bestColumn;
  
  cellIndexOrMinColumn = gridY * g_GridScratchWidth + gridX;
  openCellResult.selectedColumn = cellIndexOrMinColumn * 8;
  blockedMask = g_GridPathBlockingMask | 0x80000000;
  if (((((g_GridScratchPrimary[cellIndexOrMinColumn].stateMask & blockedMask) == 0) ||
       (scanCell = g_GridScratchPrimary + cellIndexOrMinColumn + -g_GridScratchWidth,
       (scanCell->stateMask & blockedMask) == 0)) || ((scanCell[1].stateMask & blockedMask) == 0)) ||
     ((((scanCell[g_GridScratchWidth - 1].stateMask & blockedMask) == 0 ||
       ((scanCell[g_GridScratchWidth + 1].stateMask & blockedMask) == 0)) ||
      (((scanCell[g_GridScratchWidth * 2 + -1].stateMask & blockedMask) == 0 ||
       ((scanCell[g_GridScratchWidth * 2].stateMask & blockedMask) == 0)))))) {
    openCellResult.selectedRow = unaff_EBX;
    openCellResult.carry = false;
    return openCellResult;
  }
  cellIndexOrMinColumn = gridX + -0x10;
  if (cellIndexOrMinColumn < 0) {
    cellIndexOrMinColumn = 0;
  }
  searchRow = gridY + -0x10;
  if (searchRow < 0) {
    searchRow = 0;
  }
  maxColumn = gridX + 0x10U;
  if ((int)g_GridScratchWidth < (int)(gridX + 0x10U)) {
    maxColumn = g_GridScratchWidth;
  }
  maxRow = gridY + 0x10U;
  if ((int)g_GridScratchHeight < (int)(gridY + 0x10U)) {
    maxRow = g_GridScratchHeight;
  }
  scanWidth = maxColumn - cellIndexOrMinColumn;
  if (scanWidth != 0 && cellIndexOrMinColumn <= (int)maxColumn) {
    rowsRemaining = maxRow - searchRow;
    if (rowsRemaining != 0 && searchRow <= (int)maxRow) {
      scanCell = g_GridScratchPrimary + searchRow * g_GridScratchWidth + cellIndexOrMinColumn;
      bestHexDistance = 0x7fffffff;
      scanColumn = cellIndexOrMinColumn;
      columnsRemaining = scanWidth;
      rowStartCell = scanCell;
      do {
        do {
          if ((scanCell->stateMask & (g_GridPathBlockingMask | 0x80000000)) == 0) {
            hexDistance = scanColumn - gridX;
            if (hexDistance < 0) {
              hexDistance = -hexDistance;
              rowDelta = searchRow - gridY;
              if (rowDelta < 0) {
                hexDistance = hexDistance - rowDelta;
              }
              else if (hexDistance < rowDelta) {
                hexDistance = rowDelta;
              }
            }
            else {
              rowDelta = searchRow - gridY;
              if (rowDelta < 0) {
                if (hexDistance < -rowDelta) {
                  hexDistance = -rowDelta;
                }
              }
              else {
                hexDistance = hexDistance + rowDelta;
              }
            }
            if (hexDistance < bestHexDistance) {
              bestHexDistance = hexDistance;
              bestRow = searchRow;
              bestColumn = scanColumn;
            }
          }
          scanCell = scanCell + 1;
          columnsRemaining = columnsRemaining + -1;
          scanColumn = scanColumn + 1;
        } while (columnsRemaining != 0);
        scanCell = rowStartCell + g_GridScratchWidth;
        searchRow = searchRow + 1;
        rowsRemaining = rowsRemaining + -1;
        scanColumn = cellIndexOrMinColumn;
        columnsRemaining = scanWidth;
        rowStartCell = scanCell;
      } while (rowsRemaining != 0);
      if (bestHexDistance < 0x7fffffff) {
        nearestResult.selectedRow = bestRow;
        nearestResult.selectedColumn = bestColumn;
        nearestResult.carry = true;
        return nearestResult;
      }
    }
  }
  fallbackResult.selectedRow = gridY;
  fallbackResult.selectedColumn = gridX;
  fallbackResult.carry = true;
  return fallbackResult;
}


/* Address: 0x005344B0.
   Ownership: world/pathing/grid.
   Purpose: The carry contract conveys whether the segment is acceptable. Typed parameters: p2
   callerBlockingMask→FieldGridRegionMask. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p3 startRow→FieldGridCellCoordinate_V331, p4
   startColumn→FieldGridCellCoordinate_V331.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridPathLine_TestHexSegmentClearCf
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell)

{
  GridScratchStateMask cellState;
  uint endCellIndex;
  int rowDelta;
  int thresholdOrColumnsLeft;
  int columnDelta;
  int slopeError;
  int rowError;
  GridScratchCell *lineCursor;
  GridScratchCell *columnScanCell;
  GridScratchCell *columnEndCell;
  
  endCellIndex = (uint)((int)endCell - (int)g_GridScratchPrimary) >> 3;
  rowDelta = endCellIndex / g_GridScratchWidth - startRow;
  columnDelta = endCellIndex % g_GridScratchWidth - startColumn;
  lineCursor = startCell;
  if (columnDelta < 0) {
    rowDelta = -rowDelta;
    columnDelta = -columnDelta;
    lineCursor = endCell;
    endCell = startCell;
  }
  if (rowDelta < 0) {
    slopeError = 0;
    thresholdOrColumnsLeft = -rowDelta - columnDelta;
GridPathLine_TestHexSegmentClear_ScanNegativeSlopeCell:
    do {
      cellState = lineCursor->stateMask;
      if ((lineCursor != startCell) &&
         (((((int)cellState < 0 || ((g_GridPathEntityClassMask & cellState) == 0)) ||
           ((g_GridPathBlockingMask & cellState) != 0)) || ((callerBlockingMask & cellState) != 0)))) {
        return true;
      }
      if (lineCursor == endCell) {
        return false;
      }
      if (slopeError == thresholdOrColumnsLeft || slopeError < thresholdOrColumnsLeft) {
        if (slopeError != thresholdOrColumnsLeft) {
          slopeError = slopeError + columnDelta * 2;
          lineCursor = lineCursor + -g_GridScratchWidth;
          goto GridPathLine_TestHexSegmentClear_ScanNegativeSlopeCell;
        }
        slopeError = slopeError + columnDelta * 2;
        lineCursor = lineCursor + -g_GridScratchWidth;
      }
      slopeError = slopeError + rowDelta * 2;
      lineCursor = lineCursor + 1;
    } while( true );
  }
  rowError = 0;
  slopeError = 0;
  thresholdOrColumnsLeft = columnDelta;
  columnEndCell = lineCursor;
GridPathLine_TestHexSegmentClear_AdvancePositiveSlopeColumns:
  if (rowError < rowDelta) goto code_r0x00534514;
  goto GridPathLine_TestHexSegmentClear_ScanPositiveSlopeColumn;
code_r0x00534514:
  lineCursor = lineCursor + g_GridScratchWidth;
  rowError = rowError + columnDelta;
  if (lineCursor == endCell) {
GridPathLine_TestHexSegmentClear_ScanPositiveSlopeColumn:
    do {
      rowError = rowError - rowDelta;
      columnScanCell = lineCursor;
      while( true ) {
        cellState = columnScanCell->stateMask;
        if (columnScanCell != startCell) {
          if ((int)cellState < 0) {
            return true;
          }
          if ((g_GridPathEntityClassMask & cellState) == 0) {
            return true;
          }
          if ((g_GridPathBlockingMask & cellState) != 0) {
            return true;
          }
          if ((callerBlockingMask & cellState) != 0) {
            return true;
          }
        }
        if (columnScanCell == columnEndCell) break;
        columnScanCell = columnScanCell + -g_GridScratchWidth;
      }
      thresholdOrColumnsLeft = thresholdOrColumnsLeft + -1;
      if (thresholdOrColumnsLeft < 0) {
        return false;
      }
      lineCursor = lineCursor + 1;
      columnEndCell = columnEndCell + 1;
      for (; slopeError <= -columnDelta; slopeError = slopeError + columnDelta) {
        columnEndCell = columnEndCell + g_GridScratchWidth;
      }
      slopeError = slopeError - rowDelta;
    } while (thresholdOrColumnsLeft == 0);
  }
  goto GridPathLine_TestHexSegmentClear_AdvancePositiveSlopeColumns;
}

