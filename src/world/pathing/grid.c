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
   Plans a move of routeEntityRuntime towards the target world point on the scratch grid. The influence of the
   entity (and of the entity it overlaps) is lifted so it does not block itself, the traversal masks are set for
   its faction and grid class, and a blocked start cell is relocated to the nearest open cell. When the straight
   line to the target is blocked, path costs are propagated from the target and the route is backtracked to the
   farthest directly reachable cell; if the start is cut off, the target moves to the nearest cell of the start's
   region first. The chosen point goes to EntityPathing_RebuildOverlappingGroupRoutes; the influence is restored
   before returning.
*/

PathingDestinationResult
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *entityTranslation;
  ModelRuntimeClassId runtimeClassId;
  int64_t wideProductXOrY;
  int64_t wideProductY;
  WorldPositionXY targetWorldPosition;
  WorldPositionXY fallbackWorldPosition;
  uint8_t gridClassShift;
  int startColumnOrScratch; /* also the Q12 row of a cell centre and the class id at the end */
  uint32_t columnLimitOrWidth;
  GridPathUnreachableReferenceColumn32 targetColumn;
  FieldGridRegionMask callerBlockingMask;
  uint32_t scaledRowTerm;
  int cellCoordOrStrideBytes;
  int startRow;
  int rawTargetRow;
  uint32_t rowLimit;
  GridPathUnreachableReferenceRow32 targetRow;
  GridScratchCell *routeScratchCell;
  bool segmentBlocked;
  WorldPositionXY primaryWorldPosition;
  NearestCellResult nearestCell;
  GridPathMarkedRegionCellRegisterResult reachableRegionCell;
  PathingDestinationResult resolvedDestination;
  PathBacktrackResult backtrackResult;
  ModelDefinition *modelDefinition;
  ArmyRuntimeSlot *armyRuntime;
  GameEntityRuntime *overlappedEntity;
  ModelRuntimeNode *entityModelNode;

  fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
  fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
  /* start cell from the entity position, clamped to 1..size-2 */
  entityModelNode = (routeEntityRuntime->common).ownership.modelNode;
  wideProductXOrY = (int64_t)(entityModelNode->worldTransform).translation.x * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  wideProductY = (int64_t)(entityModelNode->worldTransform).translation.y * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  scaledRowTerm = (int)((uint64_t)wideProductY >> 32) << 11 | (uint32_t)wideProductY >> 21;
  startColumnOrScratch = (int)((((int)((uint64_t)wideProductXOrY >> 32) << 12 | (uint32_t)wideProductXOrY >> 20) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12)
          >> GRID_SCRATCH_CELL_SHIFT;
  cellCoordOrStrideBytes = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
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
  startRow = rowLimit - 2;
  /* target cell, clamped the same way */
  scaledRowTerm = (int)((uint64_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
           (uint32_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  cellCoordOrStrideBytes = (int)((((int)((uint64_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                  (uint32_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12) >>
           GRID_SCRATCH_CELL_SHIFT;
  rawTargetRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
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
  targetColumn = columnLimitOrWidth - 2;
  targetRow = rowLimit - 2;
  modelDefinition = (routeEntityRuntime->common).ownership.definitionOrClassRecord;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  runtimeClassId = modelDefinition->runtimeClassId;
  if (overlappedEntity != NULL) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
      [((ModelDefinition *)(overlappedEntity->common).ownership.definitionOrClassRecord)->runtimeClassId])
              (overlappedEntity);
  }
  armyRuntime = (routeEntityRuntime->common).ownership.runtimeLink;
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove[runtimeClassId]
            (routeEntityRuntime);
  g_GridPathEntityClassMask = 1 << ((uint8_t)armyRuntime->factionIndex & 31);
  gridClassShift = (uint8_t)modelDefinition->footprintRadiusClass;
  g_GridPathHighCostMask = GRID_SCRATCH_HIGH_BAND0 << (gridClassShift & 31);
  g_GridPathBlockingMask =
       GRID_SCRATCH_LOW_BAND0 << (gridClassShift & 31) |
       GRID_SCRATCH_TERRAIN_CLASS_BIT24 << ((uint8_t)modelDefinition->terrainTraversalClass & 31);
  nearestCell = GridPathCost_FindNearestUnblockedCell(startRow,startColumnOrScratch);
  columnLimitOrWidth = g_GridScratchWidth;
  if (nearestCell.relocated) {
    if ((nearestCell.selectedColumn == startColumnOrScratch) && (nearestCell.selectedRow == startRow)) {
      /* no open cell nearby: stay where the entity is */
      entityTranslation = &(((routeEntityRuntime->common).ownership.modelNode)->worldTransform).translation;
      fallbackWorldPosition.worldXQ12 = entityTranslation->x;
      fallbackWorldPosition.worldYQ12 = entityTranslation->y;
      primaryWorldPosition.worldXQ12 = entityTranslation->x;
      primaryWorldPosition.worldYQ12 = entityTranslation->y;
    }
    else {
      /* move out of the blocked start: to the centre of the nearest open cell */
      startColumnOrScratch = nearestCell.selectedRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
      wideProductXOrY = (int64_t)(startColumnOrScratch + (nearestCell.selectedColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
      wideProductY = (int64_t)startColumnOrScratch * FIELD_GRID_WORLD_ROW_STEP_Y;
      primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                         ((int)((uint64_t)wideProductY >> 32) << 20 | (uint32_t)wideProductY >> 12,
                          (int)((uint64_t)wideProductXOrY >> 32) << 19 | (uint32_t)wideProductXOrY >> 13,
                          routeEntityRuntime,worldRuntime);
    }
  }
  else {
    cellCoordOrStrideBytes = g_GridScratchWidth * 8;
    routeScratchCell = g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumnOrScratch;
    segmentBlocked = GridPathLine_TestHexSegmentBlocked
                       (g_GridPathHighCostMask,startRow,startColumnOrScratch,routeScratchCell,
                        g_GridScratchPrimary + targetRow * g_GridScratchWidth + targetColumn);
    if (segmentBlocked) {
      GridScratch_ResetTraversalFlagsAndCosts();
      GridPathCost_PropagateWeightedHexNeighbors
                (GRID_PATH_PROPAGATION_PASSES,routeScratchCell,targetRow,targetColumn);
      routeScratchCell = routeScratchCell + -columnLimitOrWidth;
      /* start cell and all six neighbours unreached: the start is cut off from the target */
      if ((((GRID_PATH_COST_MAX_REACHED < routeScratchCell[columnLimitOrWidth].pathCost) &&
            (GRID_PATH_COST_MAX_REACHED < routeScratchCell->pathCost)) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[1].pathCost)) &&
         (((GRID_PATH_COST_MAX_REACHED < routeScratchCell[columnLimitOrWidth - 1].pathCost &&
           (GRID_PATH_COST_MAX_REACHED < routeScratchCell[columnLimitOrWidth + 1].pathCost)) &&
          ((GRID_PATH_COST_MAX_REACHED < routeScratchCell[columnLimitOrWidth * 2 - 1].pathCost &&
           (GRID_PATH_COST_MAX_REACHED < routeScratchCell[columnLimitOrWidth * 2].pathCost)))))) {
        /* retarget to the cell of the start's region nearest to the target and propagate again */
        reachableRegionCell = GridPathRegion_MarkUnreachableFromCell(targetRow,targetColumn,startRow,startColumnOrScratch);
        cellCoordOrStrideBytes = reachableRegionCell.selectedRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
        wideProductXOrY = (int64_t)(cellCoordOrStrideBytes + (reachableRegionCell.selectedColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
        targetWorldXQ12 = (int)((uint64_t)wideProductXOrY >> 32) << 19 | (uint32_t)wideProductXOrY >> 13;
        wideProductXOrY = (int64_t)cellCoordOrStrideBytes * FIELD_GRID_WORLD_ROW_STEP_Y;
        targetWorldYQ12 = (int)((uint64_t)wideProductXOrY >> 32) << 20 | (uint32_t)wideProductXOrY >> 12;
        GridScratch_ResetTraversalFlagsAndCosts();
        GridPathCost_PropagateWeightedHexNeighbors
                  (GRID_PATH_PROPAGATION_PASSES,routeScratchCell,reachableRegionCell.selectedRow,
                   reachableRegionCell.selectedColumn);
        cellCoordOrStrideBytes = g_GridScratchWidth * 8;
        routeScratchCell = g_GridScratchPrimary +
                       ((startRow * g_GridScratchWidth + startColumnOrScratch) - g_GridScratchWidth);
      }
      fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
      fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
      targetWorldPosition.worldYQ12 = targetWorldYQ12;
      targetWorldPosition.worldXQ12 = targetWorldXQ12;
      /* high-cost cells block the backtrack's straight-line test, unless bit 1 of the runtime record's +0x18
         flags is set */
      callerBlockingMask = g_GridPathHighCostMask;
      if ((((ArmyRuntimeSlot *)(routeEntityRuntime->common).ownership.runtimeLink)->movementStateFlags & 2) != 0) {
        callerBlockingMask = 0;
      }
      backtrackResult = GridPathCost_BacktrackBestHexRoute
                         (callerBlockingMask,startRow,startColumnOrScratch,
                          (GridScratchCell *)((uint8_t *)routeScratchCell + cellCoordOrStrideBytes));
      if (!backtrackResult.reachedTarget) {
        /* both branches head for the centre of the selected cell (the original has two identical copies) */
        if (backtrackResult.routeStateMask == 0) {
          startColumnOrScratch = backtrackResult.selectedRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
          wideProductXOrY = (int64_t)(startColumnOrScratch + (backtrackResult.selectedColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
          wideProductY = (int64_t)startColumnOrScratch * FIELD_GRID_WORLD_ROW_STEP_Y;
          primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((uint64_t)wideProductY >> 32) << 20 | (uint32_t)wideProductY >> 12,
                              (int)((uint64_t)wideProductXOrY >> 32) << 19 | (uint32_t)wideProductXOrY >> 13,
                              routeEntityRuntime,worldRuntime);
          fallbackWorldPosition = targetWorldPosition;
        }
        else {
          startColumnOrScratch = backtrackResult.selectedRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
          wideProductXOrY = (int64_t)(startColumnOrScratch + (backtrackResult.selectedColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
          wideProductY = (int64_t)startColumnOrScratch * FIELD_GRID_WORLD_ROW_STEP_Y;
          primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((uint64_t)wideProductY >> 32) << 20 | (uint32_t)wideProductY >> 12,
                              (int)((uint64_t)wideProductXOrY >> 32) << 19 | (uint32_t)wideProductXOrY >> 13,
                              routeEntityRuntime,worldRuntime);
        }
        goto EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn;
      }
    }
    /* straight line clear (or the backtrack reached the target): head for the target itself */
    primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                       (targetWorldYQ12,targetWorldXQ12,routeEntityRuntime,worldRuntime);
    fallbackWorldPosition = primaryWorldPosition;
  }
EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn:
  targetWorldYQ12 = fallbackWorldPosition.worldYQ12;
  targetWorldXQ12 = fallbackWorldPosition.worldXQ12;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  startColumnOrScratch = ((ModelDefinition *)(routeEntityRuntime->common).ownership.definitionOrClassRecord)->runtimeClassId;
  if (overlappedEntity != NULL) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
      [((ModelDefinition *)(overlappedEntity->common).ownership.definitionOrClassRecord)->runtimeClassId])
              (overlappedEntity);
  }
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[startColumnOrScratch](routeEntityRuntime);
  resolvedDestination.fallbackWorldXQ12 = targetWorldXQ12;
  resolvedDestination.primaryWorldXQ12 = primaryWorldPosition.worldXQ12;
  resolvedDestination.primaryWorldYQ12 = primaryWorldPosition.worldYQ12;
  resolvedDestination.fallbackWorldYQ12 = targetWorldYQ12;
  resolvedDestination.failed = false;
  return resolvedDestination;
}


/* Address: 0x00536500.
   Tests whether an obstacle of radiusMetric at the world point would split the open area around it. Every
   scratch cell is marked visited with count 0; the footprint of 3 * radius is unmarked (its cells get a non-zero
   count), the open region around the point is flood-marked inside it, and the inner footprint of radius is
   unmarked again. The first marked cell on the outer footprint's edge has its piece of the ring cleared; any
   other marked edge cell left over means the ring fell apart and returns true (CF). A point outside the grid
   also returns true.
*/
bool GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  uint32_t scratchWidth;
  uint32_t cellsRemainingOrRowTerm;
  int columnOrCellsRemaining;
  int cellRow;
  GridScratchCell *scratchCursor;
  uint32_t rowStrideBytes;
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
    scratchCursor[11].stateMask = scratchCursor[11].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[11].pathCost = 0;
    scratchCursor[12].stateMask = scratchCursor[12].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[12].pathCost = 0;
    scratchCursor[13].stateMask = scratchCursor[13].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[13].pathCost = 0;
    scratchCursor[14].stateMask = scratchCursor[14].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[14].pathCost = 0;
    scratchCursor[15].stateMask = scratchCursor[15].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCursor[15].pathCost = 0;
    scratchCursor = scratchCursor + 16;
    moreBlocksRemain = 15 < cellsRemainingOrRowTerm;
    cellsRemainingOrRowTerm = cellsRemainingOrRowTerm - 16;
  } while (moreBlocksRemain && cellsRemainingOrRowTerm != 0);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric * 3,worldYQ12,worldXQ12);
  scratchWidth = g_GridScratchWidth;
  cellsRemainingOrRowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  columnOrCellsRemaining = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - cellsRemainingOrRowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((((columnOrCellsRemaining < 0) ||
        (cellRow = (int)(cellsRemainingOrRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT,
         cellRow < 0)) ||
      ((int)g_GridScratchWidth <= columnOrCellsRemaining)) || ((int)g_GridScratchHeight <= cellRow)) {
    return true;
  }
  rowStrideBytes = g_GridScratchWidth * 8;
  GridReachability_MarkOpenRegionRecursive
            (rowStrideBytes,g_GridScratchPrimary + cellRow * g_GridScratchWidth + columnOrCellsRemaining);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  /* find the first marked cell inside the outer footprint that has a neighbour outside it (count 0); the
     tested cell is scratchCursor[scratchWidth], the cursor sits on the row above it */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  columnOrCellsRemaining = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  while (((((scratchCursor[scratchWidth].stateMask & GRID_SCRATCH_BLOCKED) != 0 ||
            (scratchCursor[scratchWidth].pathCost == 0)) ||
          ((scratchCursor[scratchWidth].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) == 0)) ||
         (((scratchCursor->pathCost != 0 && (scratchCursor[1].pathCost != 0)) &&
          ((scratchCursor[scratchWidth - 1].pathCost != 0 &&
           (((scratchCursor[scratchWidth + 1].pathCost != 0 && (scratchCursor[scratchWidth * 2 - 1].pathCost != 0))
            && (scratchCursor[scratchWidth * 2].pathCost != 0))))))))) {
    scratchCursor = scratchCursor + 1;
    columnOrCellsRemaining--;
    if (columnOrCellsRemaining == 0) {
      return false;
    }
  }
  GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scratchCursor + scratchWidth);
  /* any other marked edge cell belongs to a separate piece of the ring */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  columnOrCellsRemaining = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  do {
    if ((((scratchCursor[scratchWidth].stateMask & GRID_SCRATCH_BLOCKED) == 0) &&
         (scratchCursor[scratchWidth].pathCost != 0))
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
      if (scratchCursor[scratchWidth * 2 - 1].pathCost == 0) {
        return true;
      }
      if (scratchCursor[scratchWidth * 2].pathCost == 0) {
        return true;
      }
    }
    scratchCursor = scratchCursor + 1;
    columnOrCellsRemaining--;
    if (columnOrCellsRemaining == 0) {
      return false;
    }
  } while( true );
}


/* Address: 0x00533620.
   Rebuilds the terrain classification of the scratch grid (4x4 scratch cells per field cell): clears the
   faction and class bits, derives terrain class bits 24..30 from water depth and slope of each field cell
   and faction presence bits 1..7 from its occupancy bytes, and marks map-edge cells GRID_SCRATCH_BLOCKED.
   Then every class-24 area is grown by one field cell, and each runtime model flood-fills the region it stands
   in; cells no model can reach get classes 28..30 resp. 25..27 so they count as unreachable. The class
   bits are a scratch-only namespace and never written back to FieldGridCell.flagsAndMaterial.
   The dilation pass reads four scratch rows before the first and after the last row, as in the original.
*/
void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime)

{
  FieldGridDimension fieldGridWidth;
  FieldCellPackedFlagsAndMaterial cellFlags;
  int64_t wideProductX;
  int64_t wideProductY;
  uint32_t scratchWidth;
  uint32_t scratchStride;
  int countOrWaterDeltaOrColumn;
  GridScratchStateMask cellClassMask;
  FieldGridDimension columnsRemaining;
  int angleOrCountOrRow;
  uint32_t scaledRowTerm;
  int triangle0Angle;
  FieldGridCell *fieldCell;
  GridScratchCell *promoteCursor;
  GridScratchCell *scratchCursor;
  WorldOwnerListNode *ownerNode;
  FieldGridDimension rowsRemaining;
  FieldGridAsset *fieldGridAsset;
  
  fieldGridAsset = worldRuntime->fieldGrid;
  fieldGridWidth = fieldGridAsset->gridWidth;
  rowsRemaining = fieldGridAsset->gridHeight;
  countOrWaterDeltaOrColumn = fieldGridWidth * rowsRemaining;
  scratchCursor = g_GridScratchPrimary;
  do {
    scratchCursor->stateMask = scratchCursor->stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[1].stateMask = scratchCursor[1].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[2].stateMask = scratchCursor[2].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[3].stateMask = scratchCursor[3].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[4].stateMask = scratchCursor[4].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[5].stateMask = scratchCursor[5].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[6].stateMask = scratchCursor[6].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[7].stateMask = scratchCursor[7].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[8].stateMask = scratchCursor[8].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[9].stateMask = scratchCursor[9].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[10].stateMask = scratchCursor[10].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[11].stateMask = scratchCursor[11].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[12].stateMask = scratchCursor[12].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[13].stateMask = scratchCursor[13].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[14].stateMask = scratchCursor[14].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchCursor[15].stateMask = scratchCursor[15].stateMask & GRID_SCRATCH_REBUILD_KEEP_BITS;
    scratchWidth = g_GridScratchWidth;
    scratchCursor = scratchCursor + 16;
    countOrWaterDeltaOrColumn--;
  } while (countOrWaterDeltaOrColumn != 0);
  fieldCell = fieldGridAsset->cells;
  columnsRemaining = fieldGridWidth;
  scratchCursor = g_GridScratchPrimary;
  do {
    do {
      /* bit n set when faction slot n (1..7) is present on this field cell */
      cellClassMask =
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,1)) != 0) +
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,2)) != 0) +
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,3)) != 0) +
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,4)) != 0) +
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,5)) != 0) +
           ((uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,6)) != 0) +
           (uint32_t)((fieldCell->occupancyMask &
                       FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,7)) != 0) * 2) * 2) * 2) * 2) *
           2) * 2) * 2;
      countOrWaterDeltaOrColumn = fieldCell->waterSurfaceDelta;
      triangle0Angle = (int)fieldCell->triangle0NormalAngles >> 16;
      angleOrCountOrRow = (int)fieldCell->triangle1NormalAngles >> 16;
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
      if ((cellFlags & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        cellClassMask = cellClassMask | GRID_SCRATCH_BLOCKED;
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
        scratchCursor[scratchWidth * 2 - 1].stateMask = scratchCursor[scratchWidth * 2 - 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 - 2].stateMask = scratchCursor[scratchWidth * 2 - 2].stateMask | cellClassMask;
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
      if ((cellFlags & FIELD_CELL_LAST_ROW_BOUNDARY) == 0) {
        scratchCursor[scratchWidth].stateMask = scratchCursor[scratchWidth].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 1].stateMask = scratchCursor[scratchWidth + 1].stateMask | cellClassMask;
        scratchCursor[scratchWidth + 2].stateMask = scratchCursor[scratchWidth + 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2].stateMask = scratchCursor[scratchWidth * 2].stateMask | cellClassMask;
        scratchCursor[scratchWidth * 2 + 1].stateMask = scratchCursor[scratchWidth * 2 + 1].stateMask | cellClassMask;
        if ((cellFlags & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
          scratchCursor[scratchWidth - 1].stateMask = scratchCursor[scratchWidth - 1].stateMask | cellClassMask;
          scratchCursor[scratchWidth - 2].stateMask = scratchCursor[scratchWidth - 2].stateMask | cellClassMask;
          scratchCursor[scratchWidth * 2 - 1].stateMask = scratchCursor[scratchWidth * 2 - 1].stateMask | cellClassMask;
        }
      }
      scratchStride = g_GridScratchWidth;
      scratchCursor = scratchCursor + scratchWidth * -3 + 4;
      fieldCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    scratchCursor = scratchCursor + scratchWidth * 3;
    rowsRemaining--;
    columnsRemaining = fieldGridWidth;
  } while (rowsRemaining != 0);
  angleOrCountOrRow = g_GridScratchHeight * g_GridScratchWidth;
  scratchCursor = g_GridScratchPrimary + g_GridScratchWidth * -4;
  countOrWaterDeltaOrColumn = angleOrCountOrRow;
  do {
    scratchCursor[scratchStride * 4].stateMask =
         scratchCursor[scratchStride * 4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    if (((scratchCursor[scratchStride * 4].stateMask & GRID_SCRATCH_BLOCKED) == 0) &&
       (((scratchCursor[scratchStride * 4].stateMask | scratchCursor->stateMask | scratchCursor[4].stateMask |
          scratchCursor[scratchStride * 4 - 4].stateMask | scratchCursor[scratchStride * 4 + 4].stateMask |
          scratchCursor[scratchStride * 8 - 4].stateMask | scratchCursor[scratchStride * 8].stateMask) &
        GRID_SCRATCH_TERRAIN_CLASS_BIT24) != 0)) {
      scratchCursor[scratchStride * 4].stateMask =
           scratchCursor[scratchStride * 4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    scratchCursor++;
    countOrWaterDeltaOrColumn--;
    promoteCursor = g_GridScratchPrimary;
  } while (countOrWaterDeltaOrColumn != 0);
  do {
    if ((promoteCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) {
      promoteCursor->stateMask = promoteCursor->stateMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      promoteCursor->stateMask = promoteCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    angleOrCountOrRow--;
    promoteCursor++;
  } while (angleOrCountOrRow != 0);
  /* Flood-fill from every placed runtime model: the +0x08 record of its payload must have a non-zero
     +0x0C, and its model definition's placement contact kind (+0x278, the ArmyPlacementContact dispatch
     index) must not be 1. The world point is projected into the skewed scratch grid: row from Y, column
     from X minus half the row term, both rounded (+ GRID_SCRATCH_INDEX_BIAS_Q12 >> GRID_SCRATCH_CELL_SHIFT). */
  ownerNode = worldRuntime->ownerListHead;
  if (ownerNode != NULL) {
    do {
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != 0)) &&
         ((int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->placementContactKindIndex != 1)) {
        wideProductX = (int64_t)ownerNode->worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
        wideProductY = (int64_t)ownerNode->worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        scaledRowTerm = FIXED_PRODUCT_SHR(wideProductY, Q20_SHIFT + 1);
        countOrWaterDeltaOrColumn = (int)(((FIXED_PRODUCT_SHR(wideProductX, Q20_SHIFT)) - scaledRowTerm) +
                     GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
        if (((-1 < countOrWaterDeltaOrColumn) && (angleOrCountOrRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT, -1 < angleOrCountOrRow)) &&
           ((countOrWaterDeltaOrColumn < (int)g_GridScratchWidth && (angleOrCountOrRow < (int)g_GridScratchHeight)))) {
          GridScratch_FloodFillConnectedCellsRegs
                    (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 |
                     GRID_SCRATCH_TERRAIN_CLASS_BIT28 | GRID_SCRATCH_TRAVERSAL_VISITED,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + angleOrCountOrRow * g_GridScratchWidth + countOrWaterDeltaOrColumn);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
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
      scratchCursor++;
      countOrWaterDeltaOrColumn--;
    } while (countOrWaterDeltaOrColumn != 0);
    /* second flood fill from the same models, this time for classes 25..27 */
    ownerNode = worldRuntime->ownerListHead;
    do {
      if (((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != 0)) &&
         ((int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->placementContactKindIndex != 1)) {
        wideProductX = (int64_t)ownerNode->worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
        wideProductY = (int64_t)ownerNode->worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
        scaledRowTerm = FIXED_PRODUCT_SHR(wideProductY, Q20_SHIFT + 1);
        countOrWaterDeltaOrColumn = (int)(((FIXED_PRODUCT_SHR(wideProductX, Q20_SHIFT)) - scaledRowTerm) +
                     GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
        if ((((-1 < countOrWaterDeltaOrColumn) && (angleOrCountOrRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT, -1 < angleOrCountOrRow)) &&
            (countOrWaterDeltaOrColumn < (int)g_GridScratchWidth)) && (angleOrCountOrRow < (int)g_GridScratchHeight)) {
          GridScratch_FloodFillConnectedCellsRegs
                    (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT27 | GRID_SCRATCH_TERRAIN_CLASS_BIT26 |
                     GRID_SCRATCH_TERRAIN_CLASS_BIT25 | GRID_SCRATCH_TRAVERSAL_VISITED,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + angleOrCountOrRow * g_GridScratchWidth + countOrWaterDeltaOrColumn);
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
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
      scratchCursor++;
      countOrWaterDeltaOrColumn--;
    } while (countOrWaterDeltaOrColumn != 0);
  }
}


/* Address: 0x00533E70.
   Tests whether the army of targetRuntimePair could walk from sourceWorldPoint to its model node's position:
   the cells blocked for it are the low distance band of its grid class and the terrain class bit of its
   second grid classification. Returns true (CF set) when the target cannot be reached. No caller in the C
   code or in the handler tables references it.
*/
bool GridScratch_TestRuntimePairReachabilityFromWorldPoint
          (WorldPointXYQ12 *sourceWorldPoint,GridReachabilityRuntimePair *targetRuntimePair)

{
  bool unreachable;
  ModelDefinition *targetModelDefinition;

  targetModelDefinition = (ModelDefinition *)
          (targetRuntimePair->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  unreachable = GridScratch_TestWorldPointReachability
                    (GRID_SCRATCH_LOW_BAND0 << ((uint8_t)targetModelDefinition->footprintRadiusClass & 31) |
                     GRID_SCRATCH_TERRAIN_CLASS_BIT24 << ((uint8_t)targetModelDefinition->terrainTraversalClass & 31),
                     sourceWorldPoint->worldYQ12,sourceWorldPoint->worldXQ12,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.y,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.x);
  return unreachable;
}


/* Address: 0x005332C0.
   Sizes the pathing scratch grids for a field grid (4x4 scratch cells per field cell, 8-byte GridScratchCell
   records): allocates the primary and secondary scratch grids and the 0x180000-byte path-cost pointer queue
   (g_GridPathCostQueueBegin..End), each replacing and freeing the previous buffer. Returns the allocator
   error with CF set on failure.
*/
GridScratchAllocResult GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid)

{
  GridScratchCell *previousSecondaryScratchBuffer;
  GridScratchCell **previousCostQueueBuffer;
  uint32_t *newScratchBuffer;
  uint32_t *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  uint32_t bytes;
  ArenaAllocResult allocResult;
  ArenaFreeResult freeResult;
  GridScratchAllocResult failureResult;
  GridScratchCell *previousScratchBuffer;

  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  bytes = fieldGrid->gridWidth * (4 * sizeof(GridScratchCell)) * g_GridScratchHeight; /* scratch width * height * 8 */
  allocResult = g_MemoryApi.alloc(bytes);
  previousScratchBuffer = g_GridScratchPrimary;
  newScratchBuffer = (uint32_t *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    /* the original swaps the pointers with XCHG */
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    g_MemoryApi.free(previousScratchBuffer);
    allocResult = g_MemoryApi.alloc(bytes);
    previousSecondaryScratchBuffer = g_GridScratchSecondary;
    newSecondaryScratchBuffer = (uint32_t *)allocResult.payloadOrError;
    newScratchBuffer = newSecondaryScratchBuffer;
    if (!allocResult.failed) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      g_MemoryApi.free(previousSecondaryScratchBuffer);
      allocResult = g_MemoryApi.alloc(GRID_PATH_COST_QUEUE_BYTES);
      previousCostQueueBuffer = g_GridPathCostQueueBegin;
      newAuxiliaryBuffer = (void *)allocResult.payloadOrError;
      newScratchBuffer = newAuxiliaryBuffer;
      if (!allocResult.failed) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((int)newAuxiliaryBuffer + GRID_PATH_COST_QUEUE_BYTES);
        g_GridPathCostQueueBegin = newAuxiliaryBuffer;
        freeResult = g_MemoryApi.free(previousCostQueueBuffer);
        return THANDOR_BITCAST(uint64_t, GridScratchAllocResult, ((THANDOR_BITCAST(ArenaFreeResult, uint64_t, freeResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
  }
  failureResult.failed = true;
  failureResult.valueOrError = (uint32_t)newScratchBuffer;
  return failureResult;
}


/* Address: 0x00533360.
   Frees the path-cost queue and both scratch grids allocated by GridScratch_AllocateForFieldGrid and
   clears the three pointers, so a later session starts without stale buffers.
*/
void GridScratch_ReleaseBuffers(void)

{
  g_MemoryApi.free(g_GridPathCostQueueBegin);
  g_MemoryApi.free(g_GridScratchPrimary);
  g_MemoryApi.free(g_GridScratchSecondary);
  g_GridPathCostQueueBegin = NULL;
  g_GridScratchPrimary = NULL;
  g_GridScratchSecondary = NULL;
}


/* Address: 0x00533400.
   Tail of tick-wheel case 7: after the occupancy rebuild, turns each non-edge field cell's occupancy bytes
   of faction slots 1..7 into scratch bits 1..7 and ORs them into the cell's 4x4 scratch block and a ring of
   surrounding scratch cells, so faction presence is dilated into the scratch grid used by pathing.
*/
void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid)

{
  uint32_t *lowerScratchCursor;
  FieldGridDimension columnsRemaining;
  FieldGridCell *currentFieldCell;
  uint32_t *scratchCellCursor; /* dword view of the 8-byte scratch cells: [2n] = stateMask of cell n */
  uint32_t *propagatedScratchCursor;
  FieldGridDimension rowsRemaining;
  FieldGridDimension gridWidth;
  uint32_t factionPresenceMask;
  uint32_t scratchWidth;
  uint32_t *nextScratchCellCursor;
  
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
      if ((currentFieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        factionPresenceMask =
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,1)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,2)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,3)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,4)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,5)) != 0) +
             ((uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,6)) != 0) +
             (uint32_t)((currentFieldCell->occupancyMask &
                         FIELD_CELL_OCCUPANCY_SLOT_MASK(FIELD_CELL_OCCUPANCY_PRESENCE_BITS,7)) != 0) * 2) * 2) * 2) * 2)
             * 2) * 2) * 2;
        scratchCellCursor[scratchWidth * -4 + 4] =
             scratchCellCursor[scratchWidth * -4 + 4] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -4 + 6] =
             scratchCellCursor[scratchWidth * -4 + 6] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -2 + 2] =
             scratchCellCursor[scratchWidth * -2 + 2] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -2 + 4] =
             scratchCellCursor[scratchWidth * -2 + 4] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -2 + 6] =
             scratchCellCursor[scratchWidth * -2 + 6] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -4 + 8] =
             scratchCellCursor[scratchWidth * -4 + 8] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -2 + 8] =
             scratchCellCursor[scratchWidth * -2 + 8] | factionPresenceMask;
        scratchCellCursor[scratchWidth * -2 + 10] =
             scratchCellCursor[scratchWidth * -2 + 10] | factionPresenceMask;
        scratchCellCursor[8] = scratchCellCursor[8] | factionPresenceMask;
        scratchCellCursor[10] = scratchCellCursor[10] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 2 + 8] =
             scratchCellCursor[scratchWidth * 2 + 8] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 2 + 10] =
             scratchCellCursor[scratchWidth * 2 + 10] | factionPresenceMask;
        scratchCellCursor[scratchWidth * 4 + 8] =
             scratchCellCursor[scratchWidth * 4 + 8] | factionPresenceMask;
        *scratchCellCursor = *scratchCellCursor | factionPresenceMask;
        scratchCellCursor[2] = scratchCellCursor[2] | factionPresenceMask;
        scratchCellCursor[4] = scratchCellCursor[4] | factionPresenceMask;
        scratchCellCursor[6] = scratchCellCursor[6] | factionPresenceMask;
        propagatedScratchCursor = scratchCellCursor + scratchWidth * 2;
        propagatedScratchCursor[-2] = propagatedScratchCursor[-2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + -2] =
             propagatedScratchCursor[scratchWidth * 2 + -2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + -4] =
             propagatedScratchCursor[scratchWidth * 2 + -4] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 4 + -2] =
             propagatedScratchCursor[scratchWidth * 4 + -2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 4 + -4] =
             propagatedScratchCursor[scratchWidth * 4 + -4] | factionPresenceMask;
        *propagatedScratchCursor = *propagatedScratchCursor | factionPresenceMask;
        propagatedScratchCursor[2] = propagatedScratchCursor[2] | factionPresenceMask;
        propagatedScratchCursor[4] = propagatedScratchCursor[4] | factionPresenceMask;
        propagatedScratchCursor[6] = propagatedScratchCursor[6] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2] =
             propagatedScratchCursor[scratchWidth * 2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 2] =
             propagatedScratchCursor[scratchWidth * 2 + 2] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 4] =
             propagatedScratchCursor[scratchWidth * 2 + 4] | factionPresenceMask;
        propagatedScratchCursor[scratchWidth * 2 + 6] =
             propagatedScratchCursor[scratchWidth * 2 + 6] | factionPresenceMask;
        lowerScratchCursor = propagatedScratchCursor + scratchWidth * 4;
        *lowerScratchCursor = *lowerScratchCursor | factionPresenceMask;
        lowerScratchCursor[2] = lowerScratchCursor[2] | factionPresenceMask;
        lowerScratchCursor[4] = lowerScratchCursor[4] | factionPresenceMask;
        lowerScratchCursor[6] = lowerScratchCursor[6] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2] = lowerScratchCursor[scratchWidth * 2] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2 + 2] = lowerScratchCursor[scratchWidth * 2 + 2] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2 + 4] = lowerScratchCursor[scratchWidth * 2 + 4] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 4] = lowerScratchCursor[scratchWidth * 4] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 4 + 2] = lowerScratchCursor[scratchWidth * 4 + 2] | factionPresenceMask;
        lowerScratchCursor[scratchWidth * 2 + -2] = lowerScratchCursor[scratchWidth * 2 + -2] | factionPresenceMask
        ;
        lowerScratchCursor[scratchWidth * 2 + -4] = lowerScratchCursor[scratchWidth * 2 + -4] | factionPresenceMask
        ;
        lowerScratchCursor[scratchWidth * 4 + -2] = lowerScratchCursor[scratchWidth * 4 + -2] | factionPresenceMask
        ;
        nextScratchCellCursor = lowerScratchCursor + scratchWidth * -6 + 8;
      }
      scratchCellCursor = nextScratchCellCursor;
      columnsRemaining--;
      currentFieldCell++;
    } while (columnsRemaining != 0);
    scratchCellCursor = scratchCellCursor + scratchWidth * 6;
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00533BA0.
   Tests whether a world point may be used for pathing: projects it onto the grid-scratch cells (the same
   skewed field projection as the placement tests, with 10 instead of 12 fraction bits) and rejects it (CF set)
   when it lies outside the scratch grid, the cell is GRID_SCRATCH_BLOCKED, or the cell has distance band bit
   8 + lowBandIndex or bit 24 + highBandIndex set.
*/
bool GridScratch_TestProjectedCellMaskBands(Q12 worldYQ12,Q12 worldXQ12,uint8_t lowBandIndex,uint8_t highBandIndex)

{
  GridScratchStateMask cellStateMask;
  int cellColumn;
  uint32_t scaledRowTerm;
  int cellRow;

  /* 64-bit products shifted back (SHLD EDX,EAX,11 / 12); column = x term - row term, row = 2 * row term,
     both biased by GRID_SCRATCH_INDEX_BIAS_Q12 and taken in scratch cells */
  scaledRowTerm = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  cellColumn = (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((((-1 < cellColumn) && (cellRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT, -1 < cellRow)) &&
      (cellColumn < (int)g_GridScratchWidth)) && (cellRow < (int)g_GridScratchHeight)) {
    cellStateMask = g_GridScratchPrimary[cellRow * g_GridScratchWidth + cellColumn].stateMask;
    /* sign bit: GRID_SCRATCH_BLOCKED */
    if (((-1 < (int)cellStateMask) && ((GRID_SCRATCH_LOW_BAND0 << (lowBandIndex & 31) & cellStateMask) == 0)) &&
       ((GRID_SCRATCH_TERRAIN_CLASS_BIT24 << (highBandIndex & 31) & cellStateMask) == 0)) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00536C90.
   Plans the routes of every runtime model near the move of routeEntityRuntime together, on a copy of the scratch
   grid (the real grid is swapped back at the end). Up to 32 models whose depth-bin masks overlap the box around
   the move are collected and heap-sorted by priority (other factions lowest, then the own faction plus the
   definition's +0x0C weight); after their influence is added to the copy, other-faction models only stamp their
   low distance bands and all others have their route segment updated in that order. Returns the target of
   routeEntityRuntime's own segment (the input target when fewer than two models were found).
*/

WorldPositionXY
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  int32_t swappedPriority;
  int entityWorldY;
  GameEntityRuntime *candidateEntity;
  void *candidateRecord;
  void *candidateDefinition;
  int32_t rootPriority;
  EntityPathingPriorityPair *heapBase;
  DepthBinMask32 secondMaskHigh;
  DepthBinMask32 secondMaskLow;
  int searchRadius;
  int entityWorldXOrScratch;
  uint32_t pairsRemaining;
  uint32_t routesRemaining;
  uint32_t heapSize;
  WorldOwnerListNode *ownerNode;
  int deltaY;
  EntityPathingPriorityPair *influencePair;
  EntityPathingPriorityPair *pairCursor;
  bool masksOverlap;
  WorldPositionXY routeTarget;
  WorldPositionXY resolvedTarget;
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
  /* the definition's clearance radius */
  searchRadius = searchRadius + (int)((ModelDefinition *)(routeEntityRuntime->common).ownership.definitionOrClassRecord)->footprintRadius;
  secondMaskHigh = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldXOrScratch + targetWorldX) >> 1);
  secondMaskLow = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldY + targetWorldY) >> 1);
  entityWorldXOrScratch = 32; /* capacity of g_EntityPathingPriorityPairs */
  g_EntityPathingPriorityPairCount = 0;
  pairCursor = g_EntityPathingPriorityPairs;
  do {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      candidateEntity = ownerNode->runtimePayload;
      candidateRecord = (candidateEntity->common).ownership.definitionOrClassRecord;
      masksOverlap = DepthBinMasks_Overlap
                         (ownerNode->modelDepthBinMaskFar,ownerNode->modelDepthBinMaskNear,
                          secondMaskLow,secondMaskHigh);
      if ((masksOverlap) && (((ModelDefinition *)candidateRecord)->accelerationPerTick != 0)) {
        pairCursor->entity = candidateEntity;
        pairCursor->priority = 0;
        g_EntityPathingPriorityPairCount++;
        pairCursor++;
        entityWorldXOrScratch--;
        if (entityWorldXOrScratch == 0) break;
      }
    }
    ownerNode = ownerNode->nextNode;
  } while (ownerNode != NULL);
  if (1 < g_EntityPathingPriorityPairCount) {
    /* priority: 0 when bit 1 of the runtime record's +0x18 flags is set, 1 for another faction (+0x0C),
       2 for the own faction, plus the definition's +0x0C weight when flag bit 0 is clear */
    entityWorldXOrScratch = ((ArmyRuntimeSlot *)(routeEntityRuntime->common).ownership.runtimeLink)->factionIndex;
    pairsRemaining = g_EntityPathingPriorityPairCount;
    pairCursor = g_EntityPathingPriorityPairs;
    do {
      candidateRecord = (pairCursor->entity->common).ownership.runtimeLink;
      candidateDefinition = (pairCursor->entity->common).ownership.definitionOrClassRecord;
      if ((((((ArmyRuntimeSlot *)candidateRecord)->movementStateFlags & 2) == 0) &&
          (pairCursor->priority = pairCursor->priority + 1,
          entityWorldXOrScratch == ((ArmyRuntimeSlot *)candidateRecord)->factionIndex)) &&
         (pairCursor->priority = pairCursor->priority + 1,
         (((ArmyRuntimeSlot *)candidateRecord)->movementStateFlags & 1) == 0)) {
        pairCursor->priority = pairCursor->priority + ((ModelDefinition *)candidateDefinition)->movementSpeed;
      }
      heapBase = g_EntityPathingPriorityPairs;
      pairCursor++;
      pairsRemaining--;
    } while (pairsRemaining != 0);
    /* heap sort: build the heap, then move the root to the end until one entry is left */
    heapSize = 0;
    pairsRemaining = g_EntityPathingPriorityPairCount;
    pairCursor = g_EntityPathingPriorityPairs;
    do {
      heapSize++;
      PriorityPairHeap_SiftUp(heapSize,heapBase);
      pairCursor++;
      pairsRemaining--;
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
      heapSize--;
      PriorityPairHeap_SiftDown(heapSize,heapBase);
      pairCursor--;
      pairsRemaining = g_EntityPathingPriorityPairCount;
      influencePair = g_EntityPathingPriorityPairs;
    } while (1 < heapSize);
    do {
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [((ModelDefinition *)(influencePair->entity->common).ownership.definitionOrClassRecord)->runtimeClassId])
                (influencePair->entity);
      pairsRemaining--;
      routesRemaining = g_EntityPathingPriorityPairCount;
      pairCursor = g_EntityPathingPriorityPairs;
      influencePair++;
    } while (pairsRemaining != 0);
    do {
      targetWorldY = routeTarget.worldYQ12;
      targetWorldX = routeTarget.worldXQ12;
      candidateEntity = pairCursor->entity;
      if (pairCursor->priority == 1) {
        /* another faction's model is only an obstacle */
        entityModelNode = (candidateEntity->common).ownership.modelNode;
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (((ModelDefinition *)(candidateEntity->common).ownership.definitionOrClassRecord)->
                   footprintRadius,(entityModelNode->worldTransform).translation.y,
                   (entityModelNode->worldTransform).translation.x);
      }
      else if (candidateEntity == routeEntityRuntime) {
        routeTarget = EntityPathing_UpdateRouteSegment
                           (targetWorldY,targetWorldX,routeEntityRuntime,
                            (EntityPathingRouteEntityRuntimeView *)candidateEntity);
      }
      else {
        /* the others keep their own movement target (the 0,0 target is replaced inside) */
        EntityPathing_UpdateRouteSegment
                  (0,0,routeEntityRuntime,(EntityPathingRouteEntityRuntimeView *)candidateEntity);
      }
      routesRemaining--;
      pairCursor++;
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
   Opens a disc of radiusWorldUnits (plus g_GridInfluenceRadiusOffset6 and the margin) around the world point in
   the scratch grid: every cell whose centre lies inside loses its visited and blocked bits and has its pathCost
   counter incremented (GridReachability_RebuildConnectedRegionAroundWorldPoint uses the count as "inside").
   The disc is covered by vertical walks (constant world X) from the centre row leftwards and rightwards, then
   upwards from the row above and downwards from the row below. Nothing happens off the grid or on a blocked
   centre cell.
*/
void GridFootprint_ClearTraversalFlagsAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t wideProduct;
  int columnOrWalkerValue;
  uint32_t rightCellWorldX;
  uint32_t rowTermOrLeftCellWorldX;
  int rowOrWalkerValue;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  int walkerResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;

  columnOrWalkerValue = radiusWorldUnits + g_GridInfluenceRadiusOffset6 + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold6 = columnOrWalkerValue * columnOrWalkerValue;
  rowTermOrLeftCellWorldX = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  columnOrWalkerValue = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rowTermOrLeftCellWorldX) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((((-1 < columnOrWalkerValue) &&
        (rowOrWalkerValue = (int)(rowTermOrLeftCellWorldX * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT,
         -1 < rowOrWalkerValue)) &&
      (columnOrWalkerValue < (int)g_GridScratchWidth)) && (rowOrWalkerValue < (int)g_GridScratchHeight)) {
    /* world position of the centre cell's centre */
    centerCellCursor = g_GridScratchPrimary + g_GridScratchWidth * rowOrWalkerValue + columnOrWalkerValue;
    rowOrWalkerValue = rowOrWalkerValue * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
    wideProduct = (int64_t)(rowOrWalkerValue + (columnOrWalkerValue * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
    centerCellWorldX = (int)((uint64_t)wideProduct >> 32) << 19 | (uint32_t)wideProduct >> 13;
    wideProduct = (int64_t)rowOrWalkerValue * FIELD_GRID_WORLD_ROW_STEP_Y;
    centerCellWorldY = (int)((uint64_t)wideProduct >> 32) << 20 | (uint32_t)wideProduct >> 12;
    rowTermOrLeftCellWorldX = centerCellWorldX;
    walkCursor = centerCellCursor;
    if ((centerCellCursor->stateMask & GRID_SCRATCH_BLOCKED) == 0) {
      /* centre row leftwards, walking up and down, until a column has nothing inside */
      while ((columnOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask),
             rightCellWorldX = centerCellWorldX, oppositeWalkCursor = centerCellCursor, columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask),
              columnOrWalkerValue != 0)
             )) {
        walkCursor--;
        rowTermOrLeftCellWorldX = rowTermOrLeftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* centre row rightwards (the centre column again) */
      while ((columnOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,&oppositeWalkCursor->stateMask),
             columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,&oppositeWalkCursor->stateMask),
             columnOrWalkerValue != 0))) {
        oppositeWalkCursor++;
        rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row above: upward walks, leftwards then rightwards */
      rowOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      oppositeWalkCursor = walkCursor;
      while (walkerResult = GridFootprint_ClearTraversalFlagsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                rowOrWalkerValue,&oppositeWalkCursor->stateMask),
            walkerResult != 0) {
        oppositeWalkCursor--;
        rowOrWalkerValue = rowOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while (walkCursor++,
             rowOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                columnOrWalkerValue,&walkCursor->stateMask),
             rowOrWalkerValue != 0) {
        columnOrWalkerValue = columnOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row below: downward walks, rightwards then leftwards */
      rowOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor;
      while (walkerResult = GridFootprint_ClearTraversalFlagsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                rowOrWalkerValue,&walkCursor->stateMask),
            walkerResult != 0) {
        walkCursor++;
        rowOrWalkerValue = rowOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while (centerCellCursor--,
             rowOrWalkerValue = GridFootprint_ClearTraversalFlagsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                columnOrWalkerValue,&centerCellCursor->stateMask),
             rowOrWalkerValue != 0) {
        columnOrWalkerValue = columnOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      return;
    }
  }
  return;
}


/* Address: 0x005369A0.
   Stamps the planned movement of one model into the (copied) scratch grid so that models planned later avoid
   it: the model's own influence is lifted, the straight segment from its position to its target (the given
   target for sourceRouteEntityRuntime itself, its current movement target otherwise) gets low distance bands
   of its clearance radius every 0x240 world units. A blocked start snaps the target to the nearest open cell,
   a blocked line snaps it to the target cell's centre. Another model whose target changed gets it as new
   pending move target. Returns the target used.
*/
WorldPositionXY EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          EntityPathingRouteEntityRuntimeView *routeEntityRuntime)

{
  ArmyMovementRuntime *entityMovement;
  int64_t wideProductXOrY;
  int64_t wideProductY;
  uint8_t gridClassShift;
  int startColumnOrDeltaX;
  int targetColumn;
  uint32_t columnLimitOrRadius;
  uint32_t rowTermOrSubdivisions;
  int startRowOrDeltaY;
  int targetRow;
  uint32_t rowLimit;
  bool segmentBlocked;
  WorldPositionXY resolvedTarget;
  NearestCellResult nearestCell;
  ModelRuntimeNode *entityModelNode;

  entityModelNode = routeEntityRuntime->modelNode;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
    [routeEntityRuntime->modelDefinition->runtimeClassId])
            ((GameEntityRuntime *)routeEntityRuntime);
  /* start cell from the model position, clamped to 1..size-2 */
  wideProductXOrY = (int64_t)(entityModelNode->worldTransform).translation.x * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  wideProductY = (int64_t)(entityModelNode->worldTransform).translation.y * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rowTermOrSubdivisions = (int)((uint64_t)wideProductY >> 32) << 11 | (uint32_t)wideProductY >> 21;
  startColumnOrDeltaX = (int)((((int)((uint64_t)wideProductXOrY >> 32) << 12 | (uint32_t)wideProductXOrY >> 20) - rowTermOrSubdivisions) + GRID_SCRATCH_INDEX_BIAS_Q12) >>
          GRID_SCRATCH_CELL_SHIFT;
  startRowOrDeltaY = (int)(rowTermOrSubdivisions * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
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
  /* target cell, clamped the same way */
  rowTermOrSubdivisions = (int)((uint64_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  targetColumn = (int)((((int)((uint64_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rowTermOrSubdivisions) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  targetRow = (int)(rowTermOrSubdivisions * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
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
  g_GridPathEntityClassMask = 1 << ((uint8_t)entityMovement->factionIndex & 31);
  gridClassShift = (uint8_t)routeEntityRuntime->modelDefinition->footprintRadiusClass;
  g_GridPathHighCostMask = GRID_SCRATCH_HIGH_BAND0 << (gridClassShift & 31);
  g_GridPathBlockingMask =
       GRID_SCRATCH_LOW_BAND0 << (gridClassShift & 31) |
       GRID_SCRATCH_TERRAIN_CLASS_BIT24 << ((uint8_t)routeEntityRuntime->modelDefinition->terrainTraversalClass & 31);
  nearestCell = GridPathCost_FindNearestUnblockedCell(startRowOrDeltaY,startColumnOrDeltaX);
  targetColumn = nearestCell.selectedColumn;
  targetRow = nearestCell.selectedRow;
  if ((nearestCell.relocated) ||
     (segmentBlocked = GridPathLine_TestHexSegmentBlocked
                         (0,startRowOrDeltaY,startColumnOrDeltaX,g_GridScratchPrimary + startRowOrDeltaY * g_GridScratchWidth + startColumnOrDeltaX,
                          g_GridScratchPrimary + (rowLimit - 2) * g_GridScratchWidth + (columnLimitOrRadius - 2)),
     targetColumn = columnLimitOrRadius - 2, targetRow = rowLimit - 2, segmentBlocked)) {
    /* snap the target to the centre of the selected cell */
    startColumnOrDeltaX = targetRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
    wideProductXOrY = (int64_t)(startColumnOrDeltaX + (targetColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
    targetWorldXQ12 = (int)((uint64_t)wideProductXOrY >> 32) << 19 | (uint32_t)wideProductXOrY >> 13;
    wideProductXOrY = (int64_t)startColumnOrDeltaX * FIELD_GRID_WORLD_ROW_STEP_Y;
    targetWorldYQ12 = (int)((uint64_t)wideProductXOrY >> 32) << 20 | (uint32_t)wideProductXOrY >> 12;
  }
  columnLimitOrRadius = routeEntityRuntime->modelDefinition->footprintRadius;
  {
    /* Rewritten from the assembly (0x00536BB0-0x00536C89). The segment from the entity to the
       target is split in halves until each piece spans at most one scratch column (0x240) on both axes (or 64 pieces
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
      int deltaY = pieces[top].endY - pieces[top].startY;
      if (deltaX < 0) {
        deltaX = -deltaX;
      }
      if (deltaY < 0) {
        deltaY = -deltaY;
      }
      if (pending >= 64 || (deltaX <= GRID_SCRATCH_COLUMN_WORLD_X && deltaY <= GRID_SCRATCH_COLUMN_WORLD_X)) {
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (columnLimitOrRadius,pieces[top].endY,pieces[top].endX);
        pending--;
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
        pending++;
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
   Tests whether a mover blocked by traversalMask can get from the source world point to the target world
   point: converts both to primary scratch cells, clears every visited bit and flood-fills from the source
   (GridScratch_TestConnectedReachabilityRecursiveRegs), treating map-edge and visited cells as walls. Returns
   true (CF set) when the fill never reaches the target cell.
*/
bool GridScratch_TestWorldPointReachability(uint32_t traversalMask,GraphicsWorldCoordinateQ12 sourceWorldYQ12,
          GraphicsWorldCoordinateQ12 sourceWorldXQ12,GraphicsWorldCoordinateQ12 targetWorldYQ12,
          GraphicsWorldCoordinateQ12 targetWorldXQ12)

{
  GridScratchCell *sourceCell;
  uint32_t scratchWidth;
  int cellsRemaining;
  uint32_t scaledRowTerm;
  GridScratchCell *targetCell;
  GridScratchCell *clearCursor;
  bool unreachable;

  scratchWidth = g_GridScratchWidth;
  /* world -> scratch cell as described at GRID_SCRATCH_CELL_Q12 */
  scaledRowTerm = (int)((uint64_t)((int64_t)sourceWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)sourceWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  sourceCell = g_GridScratchPrimary +
               ((int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT) * g_GridScratchWidth +
               ((int)((((int)((uint64_t)((int64_t)sourceWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                       (uint32_t)((int64_t)sourceWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                      GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT
               );
  scaledRowTerm = (int)((uint64_t)((int64_t)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  targetCell = g_GridScratchPrimary +
                ((int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT) * g_GridScratchWidth +
                ((int)((((int)((uint64_t)((int64_t)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                        (uint32_t)((int64_t)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                       GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT);
  /* clear the visited bits, 16 cells per iteration as in the original */
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
    clearCursor[11].stateMask = clearCursor[11].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[12].stateMask = clearCursor[12].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[13].stateMask = clearCursor[13].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[14].stateMask = clearCursor[14].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor[15].stateMask = clearCursor[15].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    clearCursor = clearCursor + 16;
    cellsRemaining = cellsRemaining - 16;
  } while (cellsRemaining != 0);
  /* the row stride is passed in bytes (8-byte GridScratchCell records) */
  unreachable = GridScratch_TestConnectedReachabilityRecursiveRegs
                    (traversalMask | (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED),scratchWidth << 3,
                     &targetCell->stateMask,&sourceCell->stateMask);
  return unreachable;
}


/* Address: 0x00534660.
   Follows the propagated path costs downhill from startCell (the mover's cell, at startRow/startColumn) to the
   cheapest of the six neighbours, as long as that neighbour can still be seen from startCell in a straight line
   (GridPathLine_TestHexSegmentBlocked with callerBlockingMask, dropped once a high-cost cell is entered); at least
   one step is taken. Returns the row/column of the farthest such cell with CF clear, or CF set when that cell is
   the cost origin itself (cost 0, the target was reached).
*/
PathBacktrackResult GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell)

{
  uint32_t scratchWidth;
  GridScratchCell *currentCell;
  uint32_t selectedCellIndex;
  uint32_t bestNeighborCost;
  GridScratchCell *rowAboveCell;
  GridScratchCell *bestNeighborCell;
  bool segmentBlocked;
  PathBacktrackResult selectedCell;
  PathBacktrackResult terminalResult;

  scratchWidth = g_GridScratchWidth;
  terminalResult.selectedRow = g_GridScratchWidth * 8; /* EBX keeps the row stride on the CF-set return */
  bestNeighborCell = startCell;
  for (;;) {
    currentCell = bestNeighborCell;
    rowAboveCell = currentCell + -scratchWidth;
    if ((currentCell->stateMask & g_GridPathHighCostMask) != 0) {
      callerBlockingMask = 0;
    }
    /* the six hex neighbours: two in the row above, left/right, two in the row below */
    bestNeighborCost = currentCell->pathCost;
    bestNeighborCell = NULL;
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
    if (rowAboveCell[scratchWidth * 2 - 1].pathCost < bestNeighborCost) {
      bestNeighborCost = rowAboveCell[scratchWidth * 2 - 1].pathCost;
      bestNeighborCell = rowAboveCell + scratchWidth * 2 - 1;
    }
    if (rowAboveCell[scratchWidth * 2].pathCost < bestNeighborCost) {
      bestNeighborCell = rowAboveCell + scratchWidth * 2;
    }
    if (bestNeighborCell == NULL) {
      break; /* local minimum: return the current cell */
    }
    segmentBlocked = GridPathLine_TestHexSegmentBlocked
                      (callerBlockingMask,startRow,startColumn,startCell,bestNeighborCell);
    if (segmentBlocked) {
      if (currentCell == startCell) {
        currentCell = bestNeighborCell;
      }
      break;
    }
  }
  if (currentCell->pathCost != 0) {
    selectedCellIndex = (uint32_t)((int)currentCell - (int)g_GridScratchPrimary) >> 3;
    selectedCell.selectedRow = selectedCellIndex / g_GridScratchWidth;
    selectedCell.selectedColumn = selectedCellIndex % g_GridScratchWidth;
    selectedCell.routeStateMask = callerBlockingMask;
    selectedCell.reachedTarget = false;
    return selectedCell;
  }
  terminalResult.selectedColumn = (FieldGridCellCoordinate)currentCell;
  terminalResult.reachedTarget = true;
  terminalResult.routeStateMask = callerBlockingMask;
  return terminalResult;
}


/* Address: 0x00534960.
   Used when the start cell (row, column) was not reached by the cost propagation from the reference (target)
   cell: flood-marks the unreached region around the start and returns the cell of that region with the smallest
   hex distance to the reference cell, the closest the mover can get to the target.
*/
GridPathMarkedRegionCellRegisterResult
GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column)

{
  uint64_t markedCellIndex;
  int rowBaseIndex;
  GridPathMarkedRegionCellRegisterResult markedCell;
  GridPathBestUnreachableCell recursionResult;

  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  /* best distance starts at INT_MAX, best cell at the start cell (as a byte offset) */
  recursionResult = GridPathRegion_MarkUnreachableRecursive
                    (g_GridScratchWidth << 3,g_GridScratchPrimary + rowBaseIndex + column,INT32_MAX
                     ,(rowBaseIndex + column) * 8);
  markedCellIndex = THANDOR_BITCAST(GridPathBestUnreachableCell, uint64_t, recursionResult) >> 3 & (UINT32_MAX >> 3);
  markedCell.selectedRow = (FieldGridCellCoordinate)(markedCellIndex / g_GridScratchWidth);
  markedCell.selectedColumn = (FieldGridCellCoordinate)(markedCellIndex % (uint64_t)g_GridScratchWidth);
  return markedCell;
}


/* Address: 0x005333B0.
   Copies the whole primary scratch grid into the secondary one (two dwords per 8-byte GridScratchCell, a
   REP MOVSD in the original), so pathing can plan on a copy and swap back afterwards.
*/
void __cdecl GridScratch_CopyPrimaryToSecondary(void)

{
  int scratchDwordsRemaining;
  uint32_t *primaryReadCursor;
  uint32_t *secondaryWriteCursor;

  scratchDwordsRemaining = g_GridScratchWidth * g_GridScratchHeight * 2;
  primaryReadCursor = &g_GridScratchPrimary->stateMask;
  secondaryWriteCursor = &g_GridScratchSecondary->stateMask;
  for (; scratchDwordsRemaining != 0; scratchDwordsRemaining--) {
    *secondaryWriteCursor = *primaryReadCursor;
    primaryReadCursor++;
    secondaryWriteCursor++;
  }
  return;
}

/* Address: 0x005333E0.
   Swaps the primary and secondary scratch grid pointers (XCHG in the original), making the copy made by
   GridScratch_CopyPrimaryToSecondary the working grid, or restoring the original afterwards.
*/
void GridScratch_SwapPrimarySecondary(void)

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
   Scanline flood fill over the scratch grid: marks the horizontal run of cells around currentCell that have no
   traversalMask bit as visited, then recurses into every such cell of the row above and the row below that span.
   A blocked or already visited start cell does nothing.
*/
void GridScratch_FloodFillConnectedCellsRegs
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *spanLeftOrPrevRowCursor;
  GridScratchCell *nextRowCursor;

  if ((currentCell->stateMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    spanLeftOrPrevRowCursor = currentCell;
    while (spanLeftOrPrevRowCursor = spanLeftOrPrevRowCursor - 1, (spanLeftOrPrevRowCursor->stateMask & traversalMask) == 0) {
      spanLeftOrPrevRowCursor->stateMask = spanLeftOrPrevRowCursor->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    while (currentCell = currentCell + 1, (currentCell->stateMask & traversalMask) == 0) {
      currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    /* the row above is scanned from the span's first cell up to the column of the right stopping cell, the row
       below from the column of the left stopping cell up to the span's last cell (addresses as in the original) */
    nextRowCursor = (GridScratchCell *)((uint8_t *)spanLeftOrPrevRowCursor + rowStrideBytes);
    spanLeftOrPrevRowCursor = (GridScratchCell *)((uint8_t *)(spanLeftOrPrevRowCursor + 1) - rowStrideBytes);
    do {
      if ((spanLeftOrPrevRowCursor->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,spanLeftOrPrevRowCursor);
      }
      spanLeftOrPrevRowCursor = spanLeftOrPrevRowCursor + 1;
    } while (spanLeftOrPrevRowCursor <= (GridScratchCell *)((uint8_t *)currentCell - rowStrideBytes));
    do {
      if ((nextRowCursor->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,nextRowCursor);
      }
      nextRowCursor = nextRowCursor + 1;
    } while (nextRowCursor < (GridScratchCell *)((uint8_t *)currentCell - rowStrideBytes + rowStrideBytes * 2));
  }
  return;
}


/* Address: 0x00533C50.
   Scanline flood fill that stops as soon as it reaches targetCell (the same walk as
   GridScratch_FloodFillConnectedCellsRegs, on the stateMask words of 8-byte scratch cells): marks the run of
   open cells around currentCell as visited, then recurses into the open cells of the neighbouring rows,
   searching the row on the side of the target first. Cells with any traversalMask bit are walls. Returns false
   (CF clear) when the target was reached, true when this region does not contain it.
*/
bool GridScratch_TestConnectedReachabilityRecursiveRegs
          (uint32_t traversalMask,uint32_t rowStrideBytes,uint32_t *currentCell,uint32_t *targetCell)

{
  uint32_t *secondRowCursor;
  uint32_t *spanLeftBoundary;
  uint32_t *spanLeftCell;
  uint32_t *firstRowCursor;
  bool subRegionUnreachable;

  *currentCell = *currentCell | GRID_SCRATCH_TRAVERSAL_VISITED;
  spanLeftCell = currentCell;
  if (targetCell == currentCell) {
    return false;
  }
  /* each cell is two dwords, so +-2 is the next/previous cell of the row */
  while( true ) {
    spanLeftBoundary = spanLeftCell - 2;
    if (targetCell == spanLeftBoundary) {
      return false;
    }
    if ((*spanLeftBoundary & traversalMask) != 0) break;
    *spanLeftBoundary = *spanLeftBoundary | GRID_SCRATCH_TRAVERSAL_VISITED;
    spanLeftCell = spanLeftBoundary;
  }
  while( true ) {
    currentCell = currentCell + 2;
    if (targetCell == currentCell) {
      return false;
    }
    if ((*currentCell & traversalMask) != 0) break;
    *currentCell = *currentCell | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  if (targetCell <= spanLeftBoundary) {
    /* the target lies in an earlier row: search the previous row first, then the next one */
    secondRowCursor = (uint32_t *)((uint8_t *)spanLeftBoundary + rowStrideBytes);
    firstRowCursor = (uint32_t *)((uint8_t *)(spanLeftBoundary + 2) - rowStrideBytes);
    while (((*firstRowCursor & traversalMask) != 0 ||
           (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveRegs
                              (traversalMask,rowStrideBytes,firstRowCursor,targetCell), subRegionUnreachable))) {
      firstRowCursor = firstRowCursor + 2;
      if ((uint32_t *)((uint8_t *)currentCell - rowStrideBytes) < firstRowCursor) {
        while (((*secondRowCursor & traversalMask) != 0 ||
               (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveRegs
                                  (traversalMask,rowStrideBytes,secondRowCursor,targetCell), subRegionUnreachable))) {
          secondRowCursor = secondRowCursor + 2;
          if ((uint32_t *)((uint8_t *)currentCell - rowStrideBytes + rowStrideBytes * 2) <= secondRowCursor) {
            return true;
          }
        }
        return false;
      }
    }
    return false;
  }
  /* the target lies in a later row: search the next row first, then the previous one */
  secondRowCursor = (uint32_t *)((uint8_t *)spanLeftCell - rowStrideBytes);
  firstRowCursor = (uint32_t *)((uint8_t *)spanLeftBoundary + rowStrideBytes);
  while (((*firstRowCursor & traversalMask) != 0 ||
         (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveRegs
                            (traversalMask,rowStrideBytes,firstRowCursor,targetCell), subRegionUnreachable))) {
    firstRowCursor = firstRowCursor + 2;
    if ((uint32_t *)((uint8_t *)currentCell + rowStrideBytes) <= firstRowCursor) {
      while (((*secondRowCursor & traversalMask) != 0 ||
             (subRegionUnreachable = GridScratch_TestConnectedReachabilityRecursiveRegs
                                (traversalMask,rowStrideBytes,secondRowCursor,targetCell), subRegionUnreachable))) {
        secondRowCursor = secondRowCursor + 2;
        if ((uint32_t *)((uint8_t *)currentCell + rowStrideBytes + rowStrideBytes * -2) < secondRowCursor) {
          return true;
        }
      }
      return false;
    }
  }
  return false;
}


/* Address: 0x00533EF0.
   Fills GridScratchCell.pathCost outwards from the cell (startRow, startColumn), which gets cost 0, over the six
   hex neighbours with a FIFO queue (g_GridPathCostQueueBegin..End), lowering a neighbour's cost whenever a
   cheaper step is found. The queue is processed in passes of GRID_PATH_COST_QUEUE_PASS_ENTRIES entries; after a
   pass it stops once originCell (the mover's cell) or one of its neighbours has a cost, or after remainingPasses
   passes, or when the queue runs empty.
*/
void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  GridPathCost currentCost;
  GridScratchStateMask neighborState;
  uint32_t scratchWidth;
  uint32_t neighborCost;
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
  g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
  for (;;) {
    /* next queued cell that is not already visited; ends at an empty queue or after the passes */
    do {
      while( true ) {
        if (queueReadCursor == queueWriteCursor) {
          return;
        }
        if (queueReadCursor < g_GridPathCostQueuePassBoundary) break;
        /* end of a pass: stop when the mover's cell or a neighbour of it has been reached */
        g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
        if (originCell->pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[-scratchWidth].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[1 - scratchWidth].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[-1].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[1].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[scratchWidth - 1].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        if (originCell[scratchWidth].pathCost < GRID_PATH_COST_UNREACHED) {
          return;
        }
        remainingPasses--;
        if (remainingPasses == 0) {
          return;
        }
      }
      neighborCell = *queueReadCursor;
      queueReadCursor++;
    } while ((neighborCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
    currentCost = neighborCell->pathCost;
    neighborCell = neighborCell + -scratchWidth;
    /* Each of the six neighbours: skipped when blocked (bit 31) or when it has the mover's faction presence
       bit and a blocking band; step cost 4, 3 with the faction presence bit, 12 when it also has a high-cost
       band. */
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
        *queueWriteCursor = neighborCell;
        neighborCell->pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
    secondaryNeighborCell = neighborCell + 1;
    neighborState = secondaryNeighborCell->stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell[1].pathCost)) {
        *queueWriteCursor = secondaryNeighborCell;
        neighborCell[1].pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
    secondaryNeighborCell = secondaryNeighborCell + scratchWidth;
    neighborState = secondaryNeighborCell->stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < secondaryNeighborCell->pathCost)) {
        *queueWriteCursor = secondaryNeighborCell;
        secondaryNeighborCell->pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
    neighborCell = secondaryNeighborCell + -2;
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < secondaryNeighborCell[-2].pathCost)) {
        *queueWriteCursor = neighborCell;
        secondaryNeighborCell[-2].pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
    neighborCell = neighborCell + scratchWidth;
    neighborState = neighborCell->stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
        *queueWriteCursor = neighborCell;
        neighborCell->pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
    neighborState = neighborCell[1].stateMask;
    neighborCost = currentCost + GRID_PATH_STEP_COST;
    if ((-1 < (int)neighborState) &&
       (((g_GridPathEntityClassMask & neighborState) == 0) || ((g_GridPathBlockingMask & neighborState) == 0))) {
      if ((g_GridPathEntityClassMask & neighborState) != 0) {
        neighborCost = currentCost + GRID_PATH_STEP_COST_OWN_FACTION;
        if ((g_GridPathHighCostMask & neighborState) != 0) {
          neighborCost = currentCost + GRID_PATH_STEP_COST_HIGH;
        }
      }
      if ((queueWriteCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell[1].pathCost)) {
        *queueWriteCursor = neighborCell + 1;
        neighborCell[1].pathCost = neighborCost;
        queueWriteCursor++;
      }
    }
  }
}


/* Address: 0x00534200.
   Prepares the scratch grid for a cost propagation: clears the visited bit and sets pathCost to
   GRID_PATH_COST_UNREACHED in every cell, sixteen cells per unrolled iteration.
*/
void GridScratch_ResetTraversalFlagsAndCosts(void)

{
  uint32_t cellsRemaining;
  uint32_t *scratchRecordCursor; /* dword view of the 8-byte cells: [2n] = stateMask, [2n + 1] = pathCost */
  bool fullRecordBlockRemaining;

  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  scratchRecordCursor = &g_GridScratchPrimary->stateMask;
  do {
    *scratchRecordCursor = *scratchRecordCursor & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[1] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[2] = scratchRecordCursor[2] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[3] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[4] = scratchRecordCursor[4] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[5] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[6] = scratchRecordCursor[6] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[7] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[8] = scratchRecordCursor[8] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[9] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[10] = scratchRecordCursor[10] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[11] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[12] = scratchRecordCursor[12] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[13] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[14] = scratchRecordCursor[14] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[15] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[16] = scratchRecordCursor[16] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[17] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[18] = scratchRecordCursor[18] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[19] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[20] = scratchRecordCursor[20] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[21] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[22] = scratchRecordCursor[22] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[23] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[24] = scratchRecordCursor[24] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[25] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[26] = scratchRecordCursor[26] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[27] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[28] = scratchRecordCursor[28] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[29] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor[30] = scratchRecordCursor[30] & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchRecordCursor[31] = GRID_PATH_COST_UNREACHED;
    scratchRecordCursor = scratchRecordCursor + 32;
    fullRecordBlockRemaining = 15 < cellsRemaining;
    cellsRemaining = cellsRemaining - 16;
  } while (fullRecordBlockRemaining && cellsRemaining != 0);
  return;
}

/* Address: 0x00534780.
   Scanline flood fill for GridPathRegion_MarkUnreachableFromCell: marks the horizontal run of unreached cells
   around currentCell as visited (stopping at reached, blocked or faction-blocked cells), keeps the run cell
   nearest (hex distance) to g_GridPathUnreachableRegionReference{Row,Column} as the best cell if it beats
   bestCost, and recurses into the unvisited open cells of the rows above and below. Returns the best distance
   and the best cell's byte offset in the scratch grid.
*/
GridPathBestUnreachableCell GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset)

{
  GridScratchStateMask cellState;
  uint32_t spanByteOffset;
  GridPathCost referenceDistance;
  GridScratchCell *probeOrRightEndCell;
  int spanLength;
  uint32_t spanColumn;
  int columnDelta;
  GridPathCost columnDistance;
  GridScratchCell *leftEndCell;
  GridScratchCell *probeOrRowCursor;
  GridPathBestUnreachableCell bestResult;
  GridPathCost updatedBestCost;
  
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  probeOrRowCursor = currentCell + 1;
  probeOrRightEndCell = currentCell + -1;
  /* scan left to the span boundary: a finite-cost or closed cell, or a blocked class cell (marked, then unmarked) */
  for (;;) {
    leftEndCell = probeOrRightEndCell;
    cellState = leftEndCell->stateMask;
    if ((leftEndCell->pathCost < GRID_PATH_COST_UNREACHED) || ((int)cellState < 0)) break;
    leftEndCell->stateMask = leftEndCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    probeOrRightEndCell = leftEndCell + -1;
    if (((g_GridPathEntityClassMask & cellState) != 0) && ((g_GridPathBlockingMask & cellState) != 0)) {
      leftEndCell->stateMask = leftEndCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      break;
    }
  }
  /* scan right to the span boundary, the same way */
  for (;;) {
    probeOrRightEndCell = probeOrRowCursor;
    cellState = probeOrRightEndCell->stateMask;
    if ((probeOrRightEndCell->pathCost < GRID_PATH_COST_UNREACHED) || ((int)cellState < 0)) break;
    probeOrRightEndCell->stateMask = probeOrRightEndCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    probeOrRowCursor = probeOrRightEndCell + 1;
    if (((g_GridPathEntityClassMask & cellState) != 0) && ((g_GridPathBlockingMask & cellState) != 0)) {
      probeOrRightEndCell->stateMask = probeOrRightEndCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      break;
    }
  }
  /* hex distance from the span to the reference cell; spanByteOffset moves to the span cell nearest to it */
  spanByteOffset = (int)leftEndCell + (8 - (int)g_GridScratchPrimary);
  spanLength = ((uint32_t)((int)probeOrRightEndCell - (int)leftEndCell) >> 3) - 2;
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
  /* recurse into the unreached, unvisited, open cells of the rows above and below the span */
  probeOrRowCursor = (GridScratchCell *)((uint8_t *)(leftEndCell + 1) - rowStrideBytes);
  do {
    cellState = probeOrRowCursor->stateMask;
    if (((GRID_PATH_COST_MAX_REACHED < probeOrRowCursor->pathCost) &&
         ((cellState & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) == 0)) &&
       (((g_GridPathEntityClassMask & cellState) == 0 || ((g_GridPathBlockingMask & cellState) == 0)))) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,probeOrRowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    probeOrRowCursor++;
  } while (probeOrRowCursor <= (GridScratchCell *)((uint8_t *)probeOrRightEndCell - rowStrideBytes));
  probeOrRowCursor = (GridScratchCell *)((uint8_t *)leftEndCell + rowStrideBytes);
  do {
    cellState = probeOrRowCursor->stateMask;
    if (((GRID_PATH_COST_MAX_REACHED < probeOrRowCursor->pathCost) &&
         ((cellState & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) == 0)) &&
       (((g_GridPathEntityClassMask & cellState) == 0 || ((g_GridPathBlockingMask & cellState) == 0)))) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,probeOrRowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    probeOrRowCursor++;
  } while (probeOrRowCursor <
           (GridScratchCell *)((uint8_t *)probeOrRightEndCell - rowStrideBytes + rowStrideBytes * 2));
  return bestResult;
}


/* Address: 0x00534E70.
   Footprint walker for GridFootprint_ClearTraversalFlagsAroundWorldPoint: from scratchRecord (a cell whose centre
   is at cellWorldY/X) it walks downwards, two scratch rows and one column left per step, i.e. straight down in
   world space, while the cell centre lies within g_GridInfluenceSquaredThreshold6 of the centre point. Each cell
   loses its blocked and visited bits and has its pathCost counter incremented. Stops before a blocked cell.
   Returns the number of cells cleared, one less when the walk ended at a blocked cell (DEC in the original).
*/
int GridFootprint_ClearTraversalFlagsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord)

{
  int nextCount;
  int squaredYDistance;
  int visitedCount;

  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  nextCount = 0;
  do {
    visitedCount = nextCount;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint32_t)(squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12))) {
      return visitedCount;
    }
    /* scratchRecord[0] = stateMask, [1] = pathCost */
    *scratchRecord = *scratchRecord & ~(GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED);
    scratchRecord[1]++;
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchRecord = scratchRecord + g_GridScratchWidth * 4 - 2;
    nextCount = visitedCount + 1;
  } while ((*scratchRecord & GRID_SCRATCH_BLOCKED) == 0);
  return visitedCount;
}


/* Address: 0x00534EE0.
   Mirror of GridFootprint_ClearTraversalFlagsDiagonalNegative walking upwards (two scratch rows up and one
   column right per step), with the same clearing and the same return value.
*/
int GridFootprint_ClearTraversalFlagsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchRecord)

{
  int nextCount;
  int squaredYDistance;
  int visitedCount;

  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  nextCount = 0;
  do {
    visitedCount = nextCount;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint32_t)(squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12))) {
      return visitedCount;
    }
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchRecord = *scratchRecord & ~(GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED);
    scratchRecord[1]++;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchRecord = scratchRecord + g_GridScratchWidth * -4 + 2;
    nextCount = visitedCount + 1;
  } while ((*scratchRecord & GRID_SCRATCH_BLOCKED) == 0);
  return visitedCount;
}


/* Address: 0x005363C0.
   Scanline flood fill for GridReachability_RebuildConnectedRegionAroundWorldPoint: marks the horizontal run of
   open cells around currentCell as visited, then recurses into the open cells of the hex-adjacent spans in the
   rows above and below. A cell is open when none of GRID_REACHABILITY_OPEN_STOP_MASK is set (blocked, terrain
   classes 28..30, low bands 0..6, already visited).
*/
void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *prevRowEnd;
  GridScratchCell *spanLeftCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *nextRowCursor;

  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  spanLeftCell = currentCell;
  while (spanLeftCell = spanLeftCell + -1, (spanLeftCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
    spanLeftCell->stateMask = spanLeftCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  while (currentCell = currentCell + 1, (currentCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* spanLeftCell and currentCell are now the stop cells left and right of the run */
  nextRowCursor = (GridScratchCell *)((uint8_t *)spanLeftCell + rowStrideBytes);
  prevRowEnd = (GridScratchCell *)((uint8_t *)currentCell - rowStrideBytes);
  prevRowCursor = (GridScratchCell *)((uint8_t *)(spanLeftCell + 1) - rowStrideBytes);
  do {
    if ((prevRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,prevRowCursor);
    }
    prevRowCursor++;
  } while (prevRowCursor <= prevRowEnd);
  do {
    if ((nextRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,nextRowCursor);
    }
    nextRowCursor++;
  } while (nextRowCursor < (GridScratchCell *)((uint8_t *)prevRowEnd + rowStrideBytes * 2));
  return;
}


/* Address: 0x00536440.
   Scanline flood fill that undoes GridReachability_MarkOpenRegionRecursive for one connected piece: clears the
   visited bit across the connected cells that are visited and inside the footprint (pathCost counter non-zero),
   with the same hex-adjacent recursion into the rows above and below.
*/
void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
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
  nextRowCursor = (GridScratchCell *)((uint8_t *)spanLeftCell + rowStrideBytes);
  prevRowEnd = (GridScratchCell *)((uint8_t *)spanRightCell - rowStrideBytes);
  prevRowCursor = (GridScratchCell *)((uint8_t *)(spanLeftCell + 1) - rowStrideBytes);
  do {
    if (((prevRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) && (prevRowCursor->pathCost != 0)
       ) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,prevRowCursor);
    }
    prevRowCursor++;
  } while (prevRowCursor <= prevRowEnd);
  do {
    if (((nextRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) &&
       (nextRowCursor->pathCost != 0)) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,nextRowCursor);
    }
    nextRowCursor++;
  } while (nextRowCursor < (GridScratchCell *)((uint8_t *)prevRowEnd + rowStrideBytes * 2));
  return;
}


/* Address: 0x005342F0.
   Checks whether a mover can leave the scratch cell (cellRow, cellColumn): when the cell or one of its six hex
   neighbours is free of g_GridPathBlockingMask and GRID_SCRATCH_BLOCKED, CF is clear. Otherwise CF is set and the
   nearest (hex distance) free cell within +-16 rows/columns is returned, or the cell itself when there is none.
*/
NearestCellResult GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn)

{
  int cellIndexOrMinColumn;
  int scanColumn;
  int hexDistance;
  uint32_t maxColumn;
  int scanWidth;
  int columnsRemaining;
  GridScratchStateMask blockedMask;
  uint32_t maxRow;
  int bestHexDistance;
  int searchRow;
  int rowDelta;
  GridScratchCell *scanCell;
  NearestCellResult openCellResult;
  NearestCellResult nearestResult;
  NearestCellResult fallbackResult;
  GridScratchCell *rowStartCell;
  int rowsRemaining;
  int bestRow;
  int bestColumn;
  
  cellIndexOrMinColumn = cellRow * g_GridScratchWidth + cellColumn;
  openCellResult.selectedColumn = cellIndexOrMinColumn * 8;
  blockedMask = g_GridPathBlockingMask | GRID_SCRATCH_BLOCKED;
  if (((((g_GridScratchPrimary[cellIndexOrMinColumn].stateMask & blockedMask) == 0) ||
       (scanCell = g_GridScratchPrimary + cellIndexOrMinColumn + -g_GridScratchWidth,
       (scanCell->stateMask & blockedMask) == 0)) || ((scanCell[1].stateMask & blockedMask) == 0)) ||
     ((((scanCell[g_GridScratchWidth - 1].stateMask & blockedMask) == 0 ||
       ((scanCell[g_GridScratchWidth + 1].stateMask & blockedMask) == 0)) ||
      (((scanCell[g_GridScratchWidth * 2 - 1].stateMask & blockedMask) == 0 ||
       ((scanCell[g_GridScratchWidth * 2].stateMask & blockedMask) == 0)))))) {
    /* open cell: the original leaves EBX unchanged; both callers (EntityPathing_ResolveDestinationAndRebuildRoutes,
       EntityPathing_UpdateRouteSegment) read EAX/EBX only when CF is set */
    openCellResult.selectedRow = 0;
    openCellResult.relocated = false;
    return openCellResult;
  }
  /* search window: +-GRID_PATH_NEAREST_SEARCH_RADIUS, clipped to the grid */
  cellIndexOrMinColumn = cellColumn - GRID_PATH_NEAREST_SEARCH_RADIUS;
  if (cellIndexOrMinColumn < 0) {
    cellIndexOrMinColumn = 0;
  }
  searchRow = cellRow - GRID_PATH_NEAREST_SEARCH_RADIUS;
  if (searchRow < 0) {
    searchRow = 0;
  }
  maxColumn = cellColumn + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS;
  if ((int)g_GridScratchWidth < (int)(cellColumn + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS)) {
    maxColumn = g_GridScratchWidth;
  }
  maxRow = cellRow + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS;
  if ((int)g_GridScratchHeight < (int)(cellRow + (uint32_t)GRID_PATH_NEAREST_SEARCH_RADIUS)) {
    maxRow = g_GridScratchHeight;
  }
  scanWidth = maxColumn - cellIndexOrMinColumn;
  if (scanWidth != 0 && cellIndexOrMinColumn <= (int)maxColumn) {
    rowsRemaining = maxRow - searchRow;
    if (rowsRemaining != 0 && searchRow <= (int)maxRow) {
      scanCell = g_GridScratchPrimary + searchRow * g_GridScratchWidth + cellIndexOrMinColumn;
      bestHexDistance = INT32_MAX;
      scanColumn = cellIndexOrMinColumn;
      columnsRemaining = scanWidth;
      rowStartCell = scanCell;
      do {
        do {
          if ((scanCell->stateMask & (g_GridPathBlockingMask | GRID_SCRATCH_BLOCKED)) == 0) {
            /* hex distance on the skewed grid: |dc| + |dr| when both deltas have the same sign, else the larger */
            hexDistance = scanColumn - cellColumn;
            if (hexDistance < 0) {
              hexDistance = -hexDistance;
              rowDelta = searchRow - cellRow;
              if (rowDelta < 0) {
                hexDistance = hexDistance - rowDelta;
              }
              else if (hexDistance < rowDelta) {
                hexDistance = rowDelta;
              }
            }
            else {
              rowDelta = searchRow - cellRow;
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
          scanCell++;
          columnsRemaining--;
          scanColumn++;
        } while (columnsRemaining != 0);
        scanCell = rowStartCell + g_GridScratchWidth;
        searchRow++;
        rowsRemaining--;
        scanColumn = cellIndexOrMinColumn;
        columnsRemaining = scanWidth;
        rowStartCell = scanCell;
      } while (rowsRemaining != 0);
      if (bestHexDistance < INT32_MAX) {
        nearestResult.selectedRow = bestRow;
        nearestResult.selectedColumn = bestColumn;
        nearestResult.relocated = true;
        return nearestResult;
      }
    }
  }
  fallbackResult.selectedRow = cellRow;
  fallbackResult.selectedColumn = cellColumn;
  fallbackResult.relocated = true;
  return fallbackResult;
}


/* Address: 0x005344B0.
   Rasterises the straight line from startCell (at startRow, startColumn) to endCell over the scratch grid and
   returns true (CF set) as soon as a cell other than startCell is blocked (bit 31), lacks the mover's faction
   presence bit (g_GridPathEntityClassMask), or has a g_GridPathBlockingMask or callerBlockingMask bit; false
   when the whole line is clear. The line is always walked left to right;
   upward lines step row by row, downward lines column by column.
*/
bool GridPathLine_TestHexSegmentBlocked(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell)

{
  GridScratchStateMask cellState;
  uint32_t endCellIndex;
  int rowDelta;
  int thresholdOrColumnsLeft;
  int columnDelta;
  int slopeError;
  int rowError;
  GridScratchCell *lineCursor;
  GridScratchCell *columnScanCell;
  GridScratchCell *columnEndCell;
  
  endCellIndex = (uint32_t)((int)endCell - (int)g_GridScratchPrimary) >> 3;
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
    for (;;) {
      cellState = lineCursor->stateMask;
      if ((lineCursor != startCell) &&
         (((((int)cellState < 0 || ((g_GridPathEntityClassMask & cellState) == 0)) ||
           ((g_GridPathBlockingMask & cellState) != 0)) || ((callerBlockingMask & cellState) != 0)))) {
        return true;
      }
      if (lineCursor == endCell) {
        return false;
      }
      if (slopeError < thresholdOrColumnsLeft) {
        /* step up one row only */
        slopeError = slopeError + columnDelta * 2;
        lineCursor = lineCursor + -g_GridScratchWidth;
        continue;
      }
      if (slopeError == thresholdOrColumnsLeft) {
        /* diagonal: step up one row, then right */
        slopeError = slopeError + columnDelta * 2;
        lineCursor = lineCursor + -g_GridScratchWidth;
      }
      slopeError = slopeError + rowDelta * 2;
      lineCursor++;
    }
  }
  rowError = 0;
  slopeError = 0;
  thresholdOrColumnsLeft = columnDelta;
  columnEndCell = lineCursor;
  for (;;) {
    /* advance down the rows while the row error allows it, stopping at the end cell */
    while (rowError < rowDelta) {
      lineCursor = lineCursor + g_GridScratchWidth;
      rowError = rowError + columnDelta;
      if (lineCursor == endCell) break;
    }
    /* scan the column from lineCursor up to columnEndCell, then move one column right */
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
      thresholdOrColumnsLeft--;
      if (thresholdOrColumnsLeft < 0) {
        return false;
      }
      lineCursor++;
      columnEndCell++;
      for (; slopeError <= -columnDelta; slopeError = slopeError + columnDelta) {
        columnEndCell = columnEndCell + g_GridScratchWidth;
      }
      slopeError = slopeError - rowDelta;
    } while (thresholdOrColumnsLeft == 0);
  }
}

