/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/grid.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/pathing/grid.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/pathing/grid. */

/* World position of the centre of scratch cell (row, column). */
static WorldPositionXY GridScratch_CellCenterWorldPosition(FieldGridCellCoordinate row,FieldGridCellCoordinate column)
{
  WorldPositionXY worldPosition;
  int64_t wideProductX;
  int64_t wideProductY;
  int rowCenterQ12;

  rowCenterQ12 = row * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
  wideProductX = (int64_t)(rowCenterQ12 + (column * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
  wideProductY = (int64_t)rowCenterQ12 * FIELD_GRID_WORLD_ROW_STEP_Y;
  worldPosition.worldXQ12 = (int)((uint64_t)wideProductX >> 32) << 19 | (uint32_t)wideProductX >> 13;
  worldPosition.worldYQ12 = (int)((uint64_t)wideProductY >> 32) << 20 | (uint32_t)wideProductY >> 12;
  return worldPosition;
}

/* Plans a move of routeEntityRuntime towards the target world point on the scratch grid. The influence of the
   entity (and of the entity it overlaps) is lifted so it does not block itself, the traversal masks are set for
   its faction and grid class, and a blocked start cell is relocated to the nearest open cell. When the straight
   line to the target is blocked, path costs are propagated from the target and the route is backtracked to the
   farthest directly reachable cell; if the start is cut off, the target moves to the nearest cell of the start's
   region first. The chosen point goes to EntityPathing_RebuildOverlappingGroupRoutes; the influence is restored
   before returning. Returns the primary (next route) point and the fallback (possibly retargeted) point; it
   cannot fail.
*/

PathingDestination
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *entityTranslation;
  ModelRuntimeClassId runtimeClassId;
  int64_t wideProductX;
  int64_t wideProductY;
  WorldPositionXY cellCenterWorldPosition;
  WorldPositionXY fallbackWorldPosition;
  uint8_t gridClassShift;
  int startColumn;
  int rawStartRow;
  int rawTargetColumn;
  int rowStrideBytes;
  uint32_t columnLimit;
  uint32_t scratchWidth;
  GridPathUnreachableReferenceColumn32 targetColumn;
  FieldGridRegionMask callerBlockingMask;
  uint32_t scaledRowTerm;
  int startRow;
  int rawTargetRow;
  uint32_t rowLimit;
  GridPathUnreachableReferenceRow32 targetRow;
  GridScratchCell *routeScratchCell;
  bool segmentBlocked;
  WorldPositionXY primaryWorldPosition;
  bool startRelocated;
  FieldGridCellCoordinate nearestRow;
  FieldGridCellCoordinate nearestColumn;
  FieldGridCellCoordinate reachableRow;
  FieldGridCellCoordinate reachableColumn;
  PathingDestination resolvedDestination;
  bool backtrackReachedTarget;
  FieldGridCellCoordinate backtrackRow;
  FieldGridCellCoordinate backtrackColumn;
  FieldGridRegionMask backtrackRouteStateMask;
  ModelDefinition *modelDefinition;
  ArmyRuntimeSlot *armyRuntime;
  GameEntityRuntime *overlappedEntity;
  ModelRuntimeNode *entityModelNode;

  fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
  fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
  /* start cell from the entity position, clamped to 1..size-2 */
  entityModelNode = (routeEntityRuntime->common).ownership.modelNode;
  wideProductX = (int64_t)(entityModelNode->worldTransform).translation.x * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  wideProductY = (int64_t)(entityModelNode->worldTransform).translation.y * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  scaledRowTerm = (int)((uint64_t)wideProductY >> 32) << 11 | (uint32_t)wideProductY >> 21;
  startColumn = (int)((((int)((uint64_t)wideProductX >> 32) << 12 | (uint32_t)wideProductX >> 20) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12)
          >> GRID_SCRATCH_CELL_SHIFT;
  rawStartRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (startColumn < 1) {
    startColumn = 1;
  }
  if (rawStartRow < 1) {
    rawStartRow = 1;
  }
  columnLimit = startColumn + 2U;
  if ((int)g_GridScratchWidth < (int)(startColumn + 2U)) {
    columnLimit = g_GridScratchWidth;
  }
  rowLimit = rawStartRow + 2U;
  if ((int)g_GridScratchHeight < (int)(rawStartRow + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  startColumn = columnLimit - 2;
  startRow = rowLimit - 2;
  /* target cell, clamped the same way */
  scaledRowTerm = (int)((uint64_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
           (uint32_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  rawTargetColumn = (int)((((int)((uint64_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                  (uint32_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12) >>
           GRID_SCRATCH_CELL_SHIFT;
  rawTargetRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (rawTargetColumn < 1) {
    rawTargetColumn = 1;
  }
  if (rawTargetRow < 1) {
    rawTargetRow = 1;
  }
  columnLimit = rawTargetColumn + 2U;
  if ((int)g_GridScratchWidth < (int)(rawTargetColumn + 2U)) {
    columnLimit = g_GridScratchWidth;
  }
  rowLimit = rawTargetRow + 2U;
  if ((int)g_GridScratchHeight < (int)(rawTargetRow + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  targetColumn = columnLimit - 2;
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
  startRelocated = GridPathCost_RelocateFromBlockedCell(startRow,startColumn,&nearestRow,&nearestColumn);
  scratchWidth = g_GridScratchWidth;
  if (startRelocated) {
    if ((nearestColumn == startColumn) && (nearestRow == startRow)) {
      /* no open cell nearby: stay where the entity is */
      entityTranslation = &(((routeEntityRuntime->common).ownership.modelNode)->worldTransform).translation;
      fallbackWorldPosition.worldXQ12 = entityTranslation->x;
      fallbackWorldPosition.worldYQ12 = entityTranslation->y;
      primaryWorldPosition.worldXQ12 = entityTranslation->x;
      primaryWorldPosition.worldYQ12 = entityTranslation->y;
    }
    else {
      /* move out of the blocked start: to the centre of the nearest open cell */
      cellCenterWorldPosition = GridScratch_CellCenterWorldPosition(nearestRow,nearestColumn);
      primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                         (cellCenterWorldPosition.worldYQ12,cellCenterWorldPosition.worldXQ12,
                          routeEntityRuntime,worldRuntime);
    }
  }
  else {
    rowStrideBytes = g_GridScratchWidth * 8;
    routeScratchCell = g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumn;
    segmentBlocked = GridPathLine_TestHexSegmentBlocked
                       (g_GridPathHighCostMask,startRow,startColumn,routeScratchCell,
                        g_GridScratchPrimary + targetRow * g_GridScratchWidth + targetColumn);
    if (segmentBlocked) {
      GridScratch_ResetTraversalFlagsAndCosts();
      GridPathCost_PropagateWeightedHexNeighbors
                (GRID_PATH_PROPAGATION_PASSES,routeScratchCell,targetRow,targetColumn);
      /* from here on routeScratchCell points at the row above the start cell */
      routeScratchCell = routeScratchCell - scratchWidth;
      /* start cell and all six neighbours unreached: the start is cut off from the target */
      if ((GRID_PATH_COST_MAX_REACHED < routeScratchCell[scratchWidth].pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell->pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[1].pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[scratchWidth - 1].pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[scratchWidth + 1].pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[scratchWidth * 2 - 1].pathCost) &&
          (GRID_PATH_COST_MAX_REACHED < routeScratchCell[scratchWidth * 2].pathCost)) {
        /* retarget to the cell of the start's region nearest to the target and propagate again */
        GridPathRegion_MarkUnreachableFromCell
                  (targetRow,targetColumn,startRow,startColumn,&reachableRow,&reachableColumn);
        cellCenterWorldPosition = GridScratch_CellCenterWorldPosition(reachableRow,reachableColumn);
        targetWorldXQ12 = cellCenterWorldPosition.worldXQ12;
        targetWorldYQ12 = cellCenterWorldPosition.worldYQ12;
        GridScratch_ResetTraversalFlagsAndCosts();
        /* unlike the first propagation, the origin passed here is the row above the start cell */
        GridPathCost_PropagateWeightedHexNeighbors
                  (GRID_PATH_PROPAGATION_PASSES,routeScratchCell,reachableRow,reachableColumn);
        rowStrideBytes = g_GridScratchWidth * 8;
        routeScratchCell = g_GridScratchPrimary +
                       ((startRow * g_GridScratchWidth + startColumn) - g_GridScratchWidth);
      }
      fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
      fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
      /* high-cost cells block the backtrack's straight-line test, unless bit 1 of the runtime record's +0x18
         flags is set */
      callerBlockingMask = g_GridPathHighCostMask;
      if ((((ArmyRuntimeSlot *)(routeEntityRuntime->common).ownership.runtimeLink)->movementStateFlags & 2) != 0) {
        callerBlockingMask = 0;
      }
      /* the start cell again: one row below routeScratchCell */
      backtrackReachedTarget = GridPathCost_BacktrackBestHexRoute
                         (callerBlockingMask,startRow,startColumn,
                          (GridScratchCell *)((uint8_t *)routeScratchCell + rowStrideBytes),
                          &backtrackRow,&backtrackColumn,&backtrackRouteStateMask);
    }
    if (segmentBlocked && !backtrackReachedTarget) {
      /* head for the centre of the selected cell; the fallback stays the (possibly retargeted) target. The
         original has two identical copies of this, selected by backtrackRouteStateMask. */
      cellCenterWorldPosition = GridScratch_CellCenterWorldPosition(backtrackRow,backtrackColumn);
      primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                         (cellCenterWorldPosition.worldYQ12,cellCenterWorldPosition.worldXQ12,
                          routeEntityRuntime,worldRuntime);
    }
    else {
      /* straight line clear (or the backtrack reached the target): head for the target itself */
      primaryWorldPosition = EntityPathing_RebuildOverlappingGroupRoutes
                         (targetWorldYQ12,targetWorldXQ12,routeEntityRuntime,worldRuntime);
      fallbackWorldPosition = primaryWorldPosition;
    }
  }
  targetWorldYQ12 = fallbackWorldPosition.worldYQ12;
  targetWorldXQ12 = fallbackWorldPosition.worldXQ12;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  runtimeClassId = ((ModelDefinition *)(routeEntityRuntime->common).ownership.definitionOrClassRecord)->runtimeClassId;
  if (overlappedEntity != NULL) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
      [((ModelDefinition *)(overlappedEntity->common).ownership.definitionOrClassRecord)->runtimeClassId])
              (overlappedEntity);
  }
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[runtimeClassId](routeEntityRuntime);
  resolvedDestination.fallbackWorldXQ12 = targetWorldXQ12;
  resolvedDestination.primaryWorldXQ12 = primaryWorldPosition.worldXQ12;
  resolvedDestination.primaryWorldYQ12 = primaryWorldPosition.worldYQ12;
  resolvedDestination.fallbackWorldYQ12 = targetWorldYQ12;
  return resolvedDestination;
}


/* True when the cell below rowAboveCell (rowAboveCell[scratchWidth]) is open, counted (inside the outer
   footprint) and marked visited, and at least one of its six hex neighbours has count 0 (outside the footprint). */
static bool GridReachability_IsMarkedRingEdgeCell(GridScratchCell *rowAboveCell,uint32_t scratchWidth)
{
  GridScratchCell *cell;

  cell = rowAboveCell + scratchWidth;
  if (((cell->stateMask & GRID_SCRATCH_BLOCKED) != 0) || (cell->pathCost == 0) ||
      ((cell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) == 0)) {
    return false;
  }
  return (rowAboveCell->pathCost == 0) || (rowAboveCell[1].pathCost == 0) ||
         (rowAboveCell[scratchWidth - 1].pathCost == 0) || (rowAboveCell[scratchWidth + 1].pathCost == 0) ||
         (rowAboveCell[scratchWidth * 2 - 1].pathCost == 0) || (rowAboveCell[scratchWidth * 2].pathCost == 0);
}

/* Tests whether an obstacle of radiusMetric at the world point would split the open area around it. Every
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
  uint32_t cellsToClear;
  uint32_t scaledRowTerm;
  int cellColumn;
  int cellRow;
  int cellsToScan;
  GridScratchCell *scratchCursor;
  uint32_t rowStrideBytes;
  bool moreBlocksRemain;

  cellsToClear = g_GridScratchWidth * g_GridScratchHeight;
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
    moreBlocksRemain = 15 < cellsToClear;
    cellsToClear = cellsToClear - 16;
  } while (moreBlocksRemain && cellsToClear != 0);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric * 3,worldYQ12,worldXQ12);
  scratchWidth = g_GridScratchWidth;
  scaledRowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  cellColumn = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  cellRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((cellColumn < 0) || (cellRow < 0) || ((int)g_GridScratchWidth <= cellColumn) ||
      ((int)g_GridScratchHeight <= cellRow)) {
    return true;
  }
  rowStrideBytes = g_GridScratchWidth * 8;
  GridReachability_MarkOpenRegionRecursive
            (rowStrideBytes,g_GridScratchPrimary + cellRow * g_GridScratchWidth + cellColumn);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  /* find the first marked cell inside the outer footprint that has a neighbour outside it (count 0); the
     tested cell is scratchCursor[scratchWidth], the cursor sits on the row above it */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  cellsToScan = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  while (!GridReachability_IsMarkedRingEdgeCell(scratchCursor,scratchWidth)) {
    scratchCursor = scratchCursor + 1;
    cellsToScan--;
    if (cellsToScan == 0) {
      return false;
    }
  }
  GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scratchCursor + scratchWidth);
  /* any other marked edge cell belongs to a separate piece of the ring */
  scratchCursor = g_GridScratchPrimary + scratchWidth * 3;
  cellsToScan = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  do {
    if (GridReachability_IsMarkedRingEdgeCell(scratchCursor,scratchWidth)) {
      return true;
    }
    scratchCursor = scratchCursor + 1;
    cellsToScan--;
  } while (cellsToScan != 0);
  return false;
}


/* Class bits of one field cell for its 4x4 scratch block: faction presence bits 1..7 from the occupancy
   bytes, terrain class bits 24..30 from water depth and slope, GRID_SCRATCH_BLOCKED on the map edge. */
static GridScratchStateMask GridScratch_ClassifyFieldCell(FieldGridCell *fieldCell)
{
  GridScratchStateMask cellClassMask;
  int waterSurfaceDelta;
  int triangle0Angle;
  int triangle1Angle;
  int selectedNormalAngle;

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
  waterSurfaceDelta = fieldCell->waterSurfaceDelta;
  triangle0Angle = (int)fieldCell->triangle0NormalAngles >> 16;
  triangle1Angle = (int)fieldCell->triangle1NormalAngles >> 16;
  if (waterSurfaceDelta <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT24_MAX_WATER_SURFACE_DELTA]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
  }
  if (triangle1Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT24_MAX_TRIANGLE1_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
  }
  /* below the water surface the first triangle's slope counts */
  selectedNormalAngle = triangle1Angle;
  if (waterSurfaceDelta < 0) {
    selectedNormalAngle = triangle0Angle;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT25_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT25;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT26_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT26;
  }
  if (selectedNormalAngle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT27_MAX_SELECTED_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT27;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT29_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
  }
  if (g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT30_MIN_WATER_SURFACE_DELTA] <= waterSurfaceDelta) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT29_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
  }
  if (triangle0Angle <= g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_BIT30_MAX_TRIANGLE0_NORMAL_ANGLE]) {
    cellClassMask = cellClassMask | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
  }
  if ((fieldCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    cellClassMask = cellClassMask | GRID_SCRATCH_BLOCKED;
  }
  return cellClassMask;
}

/* Flood-fills (GridScratch_FloodFillConnectedCells with wallMask) from every placed runtime model in the owner
   list starting at ownerNode (not NULL): the +0x08 record of its payload must have a non-zero +0x0C, and its
   model definition's placement contact kind (+0x278, the ArmyPlacementContact dispatch index) must not be 1.
   The world point is projected into the skewed scratch grid: row from Y, column from X minus half the row
   term, both rounded (+ GRID_SCRATCH_INDEX_BIAS_Q12 >> GRID_SCRATCH_CELL_SHIFT); points off the grid are
   skipped. */
static void GridScratch_FloodFillFromPlacedRuntimeModels(WorldOwnerListNode *ownerNode,GridScratchStateMask wallMask)
{
  int64_t wideProductX;
  int64_t wideProductY;
  uint32_t scaledRowTerm;
  int modelColumn;
  int modelRow;

  do {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
        (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != 0) &&
        ((int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->placementContactKindIndex != 1)) {
      wideProductX = (int64_t)ownerNode->worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
      wideProductY = (int64_t)ownerNode->worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
      scaledRowTerm = FIXED_PRODUCT_SHR(wideProductY, Q20_SHIFT + 1);
      modelColumn = (int)(((FIXED_PRODUCT_SHR(wideProductX, Q20_SHIFT)) - scaledRowTerm) +
                   GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
      modelRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
      if ((-1 < modelColumn) && (-1 < modelRow) && (modelColumn < (int)g_GridScratchWidth) &&
          (modelRow < (int)g_GridScratchHeight)) {
        GridScratch_FloodFillConnectedCells
                  (wallMask,g_GridScratchWidth << 3,g_GridScratchPrimary + modelRow * g_GridScratchWidth + modelColumn);
      }
    }
    ownerNode = ownerNode->nextNode;
  } while (ownerNode != NULL);
}

/* Rebuilds the terrain classification of the scratch grid (4x4 scratch cells per field cell): clears the
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
  uint32_t scratchWidth;
  uint32_t scratchStride;
  int blocksToClear;
  int cellsRemaining;
  int cellsToPromote;
  GridScratchStateMask cellClassMask;
  FieldGridDimension columnsRemaining;
  FieldGridCell *fieldCell;
  GridScratchCell *promoteCursor;
  GridScratchCell *scratchCursor;
  WorldOwnerListNode *ownerNode;
  FieldGridDimension rowsRemaining;
  FieldGridAsset *fieldGridAsset;

  fieldGridAsset = worldRuntime->fieldGrid;
  fieldGridWidth = fieldGridAsset->gridWidth;
  rowsRemaining = fieldGridAsset->gridHeight;
  /* one 16-cell block per field cell */
  blocksToClear = fieldGridWidth * rowsRemaining;
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
    blocksToClear--;
  } while (blocksToClear != 0);
  fieldCell = fieldGridAsset->cells;
  columnsRemaining = fieldGridWidth;
  scratchCursor = g_GridScratchPrimary;
  do {
    do {
      cellClassMask = GridScratch_ClassifyFieldCell(fieldCell);
      cellFlags = fieldCell->flagsAndMaterial;
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
  /* grow class 24 by one field cell: mark (visited) every open cell with a class-24 cell four scratch cells
     away in one of the six hex directions, then promote the marks */
  cellsToPromote = g_GridScratchHeight * g_GridScratchWidth;
  scratchCursor = g_GridScratchPrimary + g_GridScratchWidth * -4;
  cellsRemaining = cellsToPromote;
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
    cellsRemaining--;
  } while (cellsRemaining != 0);
  promoteCursor = g_GridScratchPrimary;
  do {
    if ((promoteCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) {
      promoteCursor->stateMask = promoteCursor->stateMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      promoteCursor->stateMask = promoteCursor->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    cellsToPromote--;
    promoteCursor++;
  } while (cellsToPromote != 0);
  ownerNode = worldRuntime->ownerListHead;
  if (ownerNode != NULL) {
    /* flood-fill from every placed runtime model through cells open for classes 28..30; every cell left
       unreached gets those classes */
    GridScratch_FloodFillFromPlacedRuntimeModels
              (ownerNode,GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 |
               GRID_SCRATCH_TERRAIN_CLASS_BIT28 | GRID_SCRATCH_TRAVERSAL_VISITED);
    cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
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
      cellsRemaining--;
    } while (cellsRemaining != 0);
    /* second flood fill from the same models, this time for classes 25..27 */
    GridScratch_FloodFillFromPlacedRuntimeModels
              (worldRuntime->ownerListHead,GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT27 |
               GRID_SCRATCH_TERRAIN_CLASS_BIT26 | GRID_SCRATCH_TERRAIN_CLASS_BIT25 | GRID_SCRATCH_TRAVERSAL_VISITED);
    cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
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
      cellsRemaining--;
    } while (cellsRemaining != 0);
  }
}


/* Sizes the pathing scratch grids for a field grid (4x4 scratch cells per field cell, 8-byte GridScratchCell
   records): allocates the primary and secondary scratch grids and the 0x180000-byte path-cost pointer queue
   (g_GridPathCostQueueBegin..End), each replacing and freeing the previous buffer. Returns true on success;
   on failure returns false and writes the allocator error to *outError (untouched on success).
*/
bool GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid,uint32_t *outError)

{
  GridScratchCell *previousSecondaryScratchBuffer;
  GridScratchCell **previousCostQueueBuffer;
  uint32_t *newScratchBuffer;
  uint32_t *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  uint32_t bytes;
  uint32_t allocError;
  GridScratchCell *previousScratchBuffer;

  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  bytes = fieldGrid->gridWidth * (4 * sizeof(GridScratchCell)) * g_GridScratchHeight; /* scratch width * height * 8 */
  allocError = g_MemoryApi.alloc(bytes,(void **)&newScratchBuffer);
  previousScratchBuffer = g_GridScratchPrimary;
  if (allocError == 0) {
    /* the original swaps the pointers with XCHG */
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    g_MemoryApi.free(previousScratchBuffer);
    allocError = g_MemoryApi.alloc(bytes,(void **)&newSecondaryScratchBuffer);
    previousSecondaryScratchBuffer = g_GridScratchSecondary;
    if (allocError == 0) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      g_MemoryApi.free(previousSecondaryScratchBuffer);
      allocError = g_MemoryApi.alloc(GRID_PATH_COST_QUEUE_BYTES,&newAuxiliaryBuffer);
      previousCostQueueBuffer = g_GridPathCostQueueBegin;
      if (allocError == 0) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((int)newAuxiliaryBuffer + GRID_PATH_COST_QUEUE_BYTES);
        g_GridPathCostQueueBegin = newAuxiliaryBuffer;
        g_MemoryApi.free(previousCostQueueBuffer);
        return true;
      }
    }
  }
  *outError = allocError;
  return false;
}


/* Frees the path-cost queue and both scratch grids allocated by GridScratch_AllocateForFieldGrid and
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


/* Tail of tick-wheel case 7: after the occupancy rebuild, turns each non-edge field cell's occupancy bytes
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


/* Tests whether a world point may be used for pathing: projects it onto the grid-scratch cells (the same
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
  cellRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (cellColumn < 0 || cellRow < 0 ||
      cellColumn >= (int)g_GridScratchWidth || cellRow >= (int)g_GridScratchHeight) {
    return true;
  }
  cellStateMask = g_GridScratchPrimary[cellRow * g_GridScratchWidth + cellColumn].stateMask;
  /* sign bit: GRID_SCRATCH_BLOCKED */
  if ((int)cellStateMask < 0) {
    return true;
  }
  if ((GRID_SCRATCH_LOW_BAND0 << (lowBandIndex & 31) & cellStateMask) != 0) {
    return true;
  }
  return (GRID_SCRATCH_TERRAIN_CLASS_BIT24 << (highBandIndex & 31) & cellStateMask) != 0;
}


/* Plans the routes of every runtime model near the move of routeEntityRuntime together, on a copy of the scratch
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
  int entityWorldX;
  int pairSlotsLeft;
  int routeFactionIndex;
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
  entityWorldX = (entityModelNode->worldTransform).translation.x;
  entityWorldY = (entityModelNode->worldTransform).translation.y;
  searchRadius = entityWorldX - targetWorldX;
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
  secondMaskHigh = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldX + targetWorldX) >> 1);
  secondMaskLow = DepthInterval_BuildBinMask(searchRadius,(int)(entityWorldY + targetWorldY) >> 1);
  pairSlotsLeft = ENTITY_PATHING_PRIORITY_PAIR_CAPACITY; /* g_EntityPathingPriorityPairs points at g_EntityPathingPriorityPairStorage */
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
        pairSlotsLeft--;
        if (pairSlotsLeft == 0) {
          break;
        }
      }
    }
    ownerNode = ownerNode->nextNode;
  } while (ownerNode != NULL);
  if (1 < g_EntityPathingPriorityPairCount) {
    /* priority: 0 when bit 1 of the runtime record's +0x18 flags is set, 1 for another faction (+0x0C),
       2 for the own faction, plus the definition's +0x0C weight when flag bit 0 is clear */
    routeFactionIndex = ((ArmyRuntimeSlot *)(routeEntityRuntime->common).ownership.runtimeLink)->factionIndex;
    pairsRemaining = g_EntityPathingPriorityPairCount;
    pairCursor = g_EntityPathingPriorityPairs;
    do {
      candidateRecord = (pairCursor->entity->common).ownership.runtimeLink;
      candidateDefinition = (pairCursor->entity->common).ownership.definitionOrClassRecord;
      if ((((ArmyRuntimeSlot *)candidateRecord)->movementStateFlags & 2) == 0) {
        pairCursor->priority = pairCursor->priority + 1;
        if (routeFactionIndex == ((ArmyRuntimeSlot *)candidateRecord)->factionIndex) {
          pairCursor->priority = pairCursor->priority + 1;
          if ((((ArmyRuntimeSlot *)candidateRecord)->movementStateFlags & 1) == 0) {
            pairCursor->priority = pairCursor->priority + ((ModelDefinition *)candidateDefinition)->movementSpeed;
          }
        }
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


/* Opens a disc of radiusWorldUnits (plus g_GridInfluenceRadiusOffset[6] and the margin) around the world point in
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
  int clearanceRadius;
  uint32_t scaledRowTerm;
  int centerColumn;
  int centerRow;
  int centerRowQ12;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  uint32_t leftCellWorldX;
  uint32_t rightCellWorldX;
  int sideRowLeftWorldX;
  int sideRowRightWorldX;
  GridScratchCell *centerCellCursor;
  GridScratchCell *leftWalkCursor;
  GridScratchCell *rightWalkCursor;

  clearanceRadius = radiusWorldUnits + g_GridInfluenceRadiusOffset[6] + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold[6] = clearanceRadius * clearanceRadius;
  scaledRowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  centerColumn = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  centerRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if ((centerColumn < 0) || (centerRow < 0) || ((int)g_GridScratchWidth <= centerColumn) ||
      ((int)g_GridScratchHeight <= centerRow)) {
    return;
  }
  /* world position of the centre cell's centre */
  centerCellCursor = g_GridScratchPrimary + g_GridScratchWidth * centerRow + centerColumn;
  centerRowQ12 = centerRow * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
  wideProduct = (int64_t)(centerRowQ12 + (centerColumn * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
  centerCellWorldX = (int)((uint64_t)wideProduct >> 32) << 19 | (uint32_t)wideProduct >> 13;
  wideProduct = (int64_t)centerRowQ12 * FIELD_GRID_WORLD_ROW_STEP_Y;
  centerCellWorldY = (int)((uint64_t)wideProduct >> 32) << 20 | (uint32_t)wideProduct >> 12;
  if ((centerCellCursor->stateMask & GRID_SCRATCH_BLOCKED) != 0) {
    return;
  }
  /* centre row leftwards, walking up and down, until a column has nothing inside */
  leftCellWorldX = centerCellWorldX;
  leftWalkCursor = centerCellCursor;
  while ((GridFootprint_ClearTraversalFlagsDiagonalPositive
            (worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,&leftWalkCursor->stateMask) != 0) &&
         (GridFootprint_ClearTraversalFlagsDiagonalNegative
            (worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,&leftWalkCursor->stateMask) != 0)) {
    leftWalkCursor--;
    leftCellWorldX = leftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* centre row rightwards (the centre column again) */
  rightCellWorldX = centerCellWorldX;
  rightWalkCursor = centerCellCursor;
  while ((GridFootprint_ClearTraversalFlagsDiagonalPositive
            (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,&rightWalkCursor->stateMask) != 0) &&
         (GridFootprint_ClearTraversalFlagsDiagonalNegative
            (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,&rightWalkCursor->stateMask) != 0)) {
    rightWalkCursor++;
    rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* row above: upward walks, leftwards then rightwards (starting one cell right of the left start) */
  sideRowLeftWorldX = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  sideRowRightWorldX = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  leftWalkCursor = centerCellCursor - g_GridScratchWidth;
  rightWalkCursor = leftWalkCursor + 1;
  while (GridFootprint_ClearTraversalFlagsDiagonalPositive
           (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,sideRowLeftWorldX,
            &leftWalkCursor->stateMask) != 0) {
    leftWalkCursor--;
    sideRowLeftWorldX = sideRowLeftWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  while (GridFootprint_ClearTraversalFlagsDiagonalPositive
           (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,sideRowRightWorldX,
            &rightWalkCursor->stateMask) != 0) {
    sideRowRightWorldX = sideRowRightWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
    rightWalkCursor++;
  }
  /* row below: downward walks, rightwards then leftwards (starting one cell left of the right start) */
  sideRowRightWorldX = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  sideRowLeftWorldX = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  rightWalkCursor = centerCellCursor + g_GridScratchWidth;
  leftWalkCursor = rightWalkCursor - 1;
  while (GridFootprint_ClearTraversalFlagsDiagonalNegative
           (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,sideRowRightWorldX,
            &rightWalkCursor->stateMask) != 0) {
    rightWalkCursor++;
    sideRowRightWorldX = sideRowRightWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  while (GridFootprint_ClearTraversalFlagsDiagonalNegative
           (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,sideRowLeftWorldX,
            &leftWalkCursor->stateMask) != 0) {
    sideRowLeftWorldX = sideRowLeftWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
    leftWalkCursor--;
  }
}


/* Stamps the planned movement of one model into the (copied) scratch grid so that models planned later avoid
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
  int64_t wideProductX;
  int64_t wideProductY;
  uint8_t gridClassShift;
  int startColumn;
  int startRow;
  int targetColumn;
  int targetRow;
  WorldPositionXY snappedTarget;
  uint32_t columnLimit;
  uint32_t rowLimit;
  uint32_t scaledRowTerm;
  uint32_t footprintRadius;
  bool segmentBlocked;
  WorldPositionXY resolvedTarget;
  bool startRelocated;
  ModelRuntimeNode *entityModelNode;

  entityModelNode = routeEntityRuntime->modelNode;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
    [routeEntityRuntime->modelDefinition->runtimeClassId])
            ((GameEntityRuntime *)routeEntityRuntime);
  /* start cell from the model position, clamped to 1..size-2 */
  wideProductX = (int64_t)(entityModelNode->worldTransform).translation.x * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  wideProductY = (int64_t)(entityModelNode->worldTransform).translation.y * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  scaledRowTerm = (int)((uint64_t)wideProductY >> 32) << 11 | (uint32_t)wideProductY >> 21;
  startColumn = (int)((((int)((uint64_t)wideProductX >> 32) << 12 | (uint32_t)wideProductX >> 20) - scaledRowTerm) + GRID_SCRATCH_INDEX_BIAS_Q12) >>
          GRID_SCRATCH_CELL_SHIFT;
  startRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (startColumn < 1) {
    startColumn = 1;
  }
  if (startRow < 1) {
    startRow = 1;
  }
  columnLimit = startColumn + 2U;
  if ((int)g_GridScratchWidth < (int)(startColumn + 2U)) {
    columnLimit = g_GridScratchWidth;
  }
  rowLimit = startRow + 2U;
  if ((int)g_GridScratchHeight < (int)(startRow + 2U)) {
    rowLimit = g_GridScratchHeight;
  }
  startColumn = columnLimit - 2;
  startRow = rowLimit - 2;
  entityMovement = routeEntityRuntime->movementRuntime;
  if ((GameEntityRuntime *)routeEntityRuntime != sourceRouteEntityRuntime) {
    targetWorldXQ12 = entityMovement->movementWorldXQ12;
    targetWorldYQ12 = entityMovement->movementWorldYQ12;
  }
  /* target cell, clamped the same way */
  scaledRowTerm = (int)((uint64_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)(int)targetWorldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  targetColumn = (int)((((int)((uint64_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)(int)targetWorldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - scaledRowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  targetRow = (int)(scaledRowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (targetColumn < 1) {
    targetColumn = 1;
  }
  if (targetRow < 1) {
    targetRow = 1;
  }
  columnLimit = targetColumn + 2U;
  if ((int)g_GridScratchWidth < (int)(targetColumn + 2U)) {
    columnLimit = g_GridScratchWidth;
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
  /* a relocated start writes the nearest open cell into targetRow/targetColumn */
  startRelocated = GridPathCost_RelocateFromBlockedCell(startRow,startColumn,&targetRow,&targetColumn);
  segmentBlocked = false;
  if (!startRelocated) {
    segmentBlocked = GridPathLine_TestHexSegmentBlocked
                       (0,startRow,startColumn,g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumn,
                        g_GridScratchPrimary + (rowLimit - 2) * g_GridScratchWidth + (columnLimit - 2));
    targetColumn = columnLimit - 2;
    targetRow = rowLimit - 2;
  }
  if (startRelocated || segmentBlocked) {
    /* snap the target to the centre of the selected cell */
    snappedTarget = GridScratch_CellCenterWorldPosition(targetRow,targetColumn);
    targetWorldXQ12 = snappedTarget.worldXQ12;
    targetWorldYQ12 = snappedTarget.worldYQ12;
  }
  footprintRadius = routeEntityRuntime->modelDefinition->footprintRadius;
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
                  (footprintRadius,pieces[top].endY,pieces[top].endX);
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


/* Returns the cheapest of the six hex neighbours of cell (two in the row above, left/right, two in the row
   below) whose path cost is below the cell's own, the first one on ties; NULL at a local minimum. */
static GridScratchCell *GridPathCost_FindCheaperHexNeighbor(GridScratchCell *cell,uint32_t scratchWidth)
{
  GridScratchCell *rowAboveCell;
  GridScratchCell *bestNeighborCell;
  uint32_t bestNeighborCost;

  rowAboveCell = cell - scratchWidth;
  bestNeighborCost = cell->pathCost;
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
  return bestNeighborCell;
}

/* Follows the propagated path costs downhill from startCell (the mover's cell, at startRow/startColumn) to the
   cheapest of the six neighbours, as long as that neighbour can still be seen from startCell in a straight line
   (GridPathLine_TestHexSegmentBlocked with callerBlockingMask, dropped once a high-cost cell is entered); at least
   one step is taken. Returns true when that cell is the cost origin itself (cost 0, the target was reached);
   otherwise returns false and writes the row/column of the farthest such cell to *outRow/*outColumn.
   *outRouteStateMask always receives the final blocking mask (callerBlockingMask, or 0 once a high-cost cell
   was entered).
*/
bool GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn,FieldGridRegionMask *outRouteStateMask)

{
  uint32_t scratchWidth;
  GridScratchCell *currentCell;
  GridScratchCell *nextCell;
  uint32_t selectedCellIndex;

  scratchWidth = g_GridScratchWidth;
  /* step to the cheapest neighbour until a local minimum is reached or the straight line from startCell to
     that neighbour is blocked */
  nextCell = startCell;
  do {
    currentCell = nextCell;
    if ((currentCell->stateMask & g_GridPathHighCostMask) != 0) {
      callerBlockingMask = 0;
    }
    nextCell = GridPathCost_FindCheaperHexNeighbor(currentCell,scratchWidth);
  } while ((nextCell != NULL) &&
           !GridPathLine_TestHexSegmentBlocked(callerBlockingMask,startRow,startColumn,startCell,nextCell));
  if ((nextCell != NULL) && (currentCell == startCell)) {
    /* blocked on the first step: take that step anyway */
    currentCell = nextCell;
  }
  *outRouteStateMask = callerBlockingMask;
  if (currentCell->pathCost != 0) {
    selectedCellIndex = (uint32_t)((int)currentCell - (int)g_GridScratchPrimary) >> 3;
    *outRow = selectedCellIndex / g_GridScratchWidth;
    *outColumn = selectedCellIndex % g_GridScratchWidth;
    return false;
  }
  return true;
}


/* Used when the start cell (row, column) was not reached by the cost propagation from the reference (target)
   cell: flood-marks the unreached region around the start and returns the cell of that region with the smallest
   hex distance to the reference cell (written to *outRow/*outColumn), the closest the mover can get to the
   target.
*/
void GridPathRegion_MarkUnreachableFromCell
          (GridPathUnreachableReferenceRow32 referenceRow,
          GridPathUnreachableReferenceColumn32 referenceColumn,FieldGridCellCoordinate row,
          FieldGridCellCoordinate column,FieldGridCellCoordinate *outRow,FieldGridCellCoordinate *outColumn)

{
  uint32_t markedCellIndex;
  int rowBaseIndex;
  GridPathBestUnreachableCell recursionResult;

  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  /* best distance starts at INT_MAX, best cell at the start cell (as a byte offset) */
  recursionResult = GridPathRegion_MarkUnreachableRecursive
                    (g_GridScratchWidth << 3,g_GridScratchPrimary + rowBaseIndex + column,INT32_MAX
                     ,(rowBaseIndex + column) * 8);
  markedCellIndex = recursionResult.bestCellByteOffset >> 3;
  *outRow = (FieldGridCellCoordinate)(markedCellIndex / g_GridScratchWidth);
  *outColumn = (FieldGridCellCoordinate)(markedCellIndex % g_GridScratchWidth);
}


/* Copies the whole primary scratch grid into the secondary one (two dwords per 8-byte GridScratchCell, a
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

/* Swaps the primary and secondary scratch grid pointers (XCHG in the original), making the copy made by
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


/* Scanline flood fill over the scratch grid: marks the horizontal run of cells around currentCell that have no
   traversalMask bit as visited, then recurses into every such cell of the row above and the row below that span.
   A blocked or already visited start cell does nothing.
*/
void GridScratch_FloodFillConnectedCells
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *rowAboveCell;
  GridScratchCell *rowAboveLastCell;
  GridScratchCell *rowBelowCell;
  GridScratchCell *rowBelowEndCell;

  if ((currentCell->stateMask & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) != 0) {
    return;
  }
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1; (leftStopCell->stateMask & traversalMask) == 0; leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1; (rightStopCell->stateMask & traversalMask) == 0; rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* the row above is scanned from the span's first cell up to the column of the right stopping cell, the row
     below from the column of the left stopping cell up to the span's last cell (addresses as in the original).
     Both ranges are never empty, so testing before the first cell is the same as the original's do-while. */
  rowAboveLastCell = (GridScratchCell *)((uint8_t *)rightStopCell - rowStrideBytes);
  for (rowAboveCell = (GridScratchCell *)((uint8_t *)(leftStopCell + 1) - rowStrideBytes);
       rowAboveCell <= rowAboveLastCell; rowAboveCell++) {
    if ((rowAboveCell->stateMask & traversalMask) == 0) {
      GridScratch_FloodFillConnectedCells(traversalMask,rowStrideBytes,rowAboveCell);
    }
  }
  rowBelowEndCell = (GridScratchCell *)((uint8_t *)rightStopCell + rowStrideBytes);
  for (rowBelowCell = (GridScratchCell *)((uint8_t *)leftStopCell + rowStrideBytes);
       rowBelowCell < rowBelowEndCell; rowBelowCell++) {
    if ((rowBelowCell->stateMask & traversalMask) == 0) {
      GridScratch_FloodFillConnectedCells(traversalMask,rowStrideBytes,rowBelowCell);
    }
  }
}


/* State of one GridPathCost_PropagateWeightedHexNeighbors run (the queue storage and the pass boundary are the
   g_GridPathCostQueue* globals). */
typedef struct GridPathCostQueueState {
  GridScratchCell **readCursor;
  GridScratchCell **writeCursor;
  GridPathPassCount remainingPasses;
  GridScratchCell *originCell;
  uint32_t scratchWidth;
} GridPathCostQueueState;

/* True when originCell or one of its six hex neighbours already has a cost. */
static bool GridPathCost_OriginOrNeighborReached(GridScratchCell *originCell,uint32_t scratchWidth)
{
  GridScratchCell *rowAboveCell;

  rowAboveCell = originCell - scratchWidth;
  return (originCell->pathCost < GRID_PATH_COST_UNREACHED) || (rowAboveCell->pathCost < GRID_PATH_COST_UNREACHED) ||
         (rowAboveCell[1].pathCost < GRID_PATH_COST_UNREACHED) || (originCell[-1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth - 1].pathCost < GRID_PATH_COST_UNREACHED) ||
         (originCell[scratchWidth].pathCost < GRID_PATH_COST_UNREACHED);
}

/* Next queued cell that is not already visited, or NULL when propagation ends: at an empty queue, or at the end
   of a pass once the origin (or a neighbour of it) has been reached or the passes are used up. */
static GridScratchCell *GridPathCost_DequeueUnvisitedCell(GridPathCostQueueState *queue)
{
  GridScratchCell *cell;

  do {
    while ((queue->readCursor != queue->writeCursor) && (queue->readCursor >= g_GridPathCostQueuePassBoundary)) {
      /* end of a pass */
      g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
      if (GridPathCost_OriginOrNeighborReached(queue->originCell,queue->scratchWidth)) {
        return NULL;
      }
      queue->remainingPasses--;
      if (queue->remainingPasses == 0) {
        return NULL;
      }
    }
    if (queue->readCursor == queue->writeCursor) {
      return NULL;
    }
    cell = *queue->readCursor;
    queue->readCursor++;
  } while ((cell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
  return cell;
}

/* One neighbour step from a cell of cost currentCost: skipped when the neighbour is blocked (bit 31) or has the
   mover's faction presence bit and a blocking band; step cost 4, 3 with the faction presence bit, 12 when it
   also has a high-cost band. A cheaper cost is stored and the neighbour queued while the queue has room. */
static void GridPathCost_RelaxNeighbor(GridPathCostQueueState *queue,GridScratchCell *neighborCell,
          GridPathCost currentCost)
{
  GridScratchStateMask neighborState;
  uint32_t neighborCost;

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
    if ((queue->writeCursor < g_GridPathCostQueueEnd) && (neighborCost < neighborCell->pathCost)) {
      *queue->writeCursor = neighborCell;
      neighborCell->pathCost = neighborCost;
      queue->writeCursor++;
    }
  }
}

/* Fills GridScratchCell.pathCost outwards from the cell (startRow, startColumn), which gets cost 0, over the six
   hex neighbours with a FIFO queue (g_GridPathCostQueueBegin..End), lowering a neighbour's cost whenever a
   cheaper step is found. The queue is processed in passes of GRID_PATH_COST_QUEUE_PASS_ENTRIES entries; after a
   pass it stops once originCell (the mover's cell) or one of its neighbours has a cost, or after remainingPasses
   passes, or when the queue runs empty.
*/
void GridPathCost_PropagateWeightedHexNeighbors(GridPathPassCount remainingPasses,GridScratchCell *originCell,
          FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  GridPathCostQueueState queue;
  GridScratchCell *startCell;
  GridScratchCell *currentCell;
  GridPathCost currentCost;
  uint32_t scratchWidth;

  scratchWidth = g_GridScratchWidth;
  queue.scratchWidth = scratchWidth;
  queue.remainingPasses = remainingPasses;
  queue.originCell = originCell;
  queue.readCursor = g_GridPathCostQueueBegin;
  queue.writeCursor = g_GridPathCostQueueBegin + 1;
  startCell = g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumn;
  *g_GridPathCostQueueBegin = startCell;
  g_GridPathCostQueuePassBoundary = queue.readCursor;
  startCell->pathCost = 0;
  g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + GRID_PATH_COST_QUEUE_PASS_ENTRIES;
  while ((currentCell = GridPathCost_DequeueUnvisitedCell(&queue)) != NULL) {
    /* the six neighbours: two in the row above, right, left, two in the row below */
    currentCost = currentCell->pathCost;
    GridPathCost_RelaxNeighbor(&queue,currentCell - scratchWidth,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - scratchWidth + 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell + 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - 1,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell - 1 + scratchWidth,currentCost);
    GridPathCost_RelaxNeighbor(&queue,currentCell + scratchWidth,currentCost);
  }
}


/* Prepares the scratch grid for a cost propagation: clears the visited bit and sets pathCost to
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

/* Scans from currentCell in direction (-1 left, +1 right) over unreached open cells, marking them visited, and
   returns the span boundary cell: a finite-cost or blocked (bit 31) cell, or a cell with the mover's faction
   presence bit and a blocking band. Original quirk: such a faction-blocked boundary cell is marked and unmarked
   again, so it ends with its visited bit cleared even if it was set before. */
static GridScratchCell *GridPathRegion_ScanUnreachedSpanEnd(GridScratchCell *currentCell,int direction)

{
  GridScratchCell *cell;
  GridScratchStateMask cellState;

  cell = currentCell + direction;
  cellState = cell->stateMask;
  while (!(cell->pathCost < GRID_PATH_COST_UNREACHED) && (int)cellState >= 0) {
    if (((g_GridPathEntityClassMask & cellState) != 0) && ((g_GridPathBlockingMask & cellState) != 0)) {
      cell->stateMask = cellState & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      break;
    }
    cell->stateMask = cellState | GRID_SCRATCH_TRAVERSAL_VISITED;
    cell = cell + direction;
    cellState = cell->stateMask;
  }
  return cell;
}


/* True for a cell the region walk recurses into: unreached, neither blocked (bit 31) nor visited, and not
   a cell with the mover's faction presence bit and a blocking band. */
static bool GridPathRegion_IsUnvisitedUnreachedOpenCell(GridScratchCell *cell)

{
  GridScratchStateMask cellState;

  cellState = cell->stateMask;
  return GRID_PATH_COST_MAX_REACHED < cell->pathCost &&
         (cellState & (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TRAVERSAL_VISITED)) == 0 &&
         ((g_GridPathEntityClassMask & cellState) == 0 || (g_GridPathBlockingMask & cellState) == 0);
}

/* Scanline flood fill for GridPathRegion_MarkUnreachableFromCell: marks the horizontal run of unreached cells
   around currentCell as visited (stopping at reached, blocked or faction-blocked cells), keeps the run cell
   nearest (hex distance) to g_GridPathUnreachableRegionReference{Row,Column} as the best cell if it beats
   bestCost, and recurses into the unvisited open cells of the rows above and below. Returns the best distance
   and the best cell's byte offset in the scratch grid.
*/
GridPathBestUnreachableCell GridPathRegion_MarkUnreachableRecursive
          (uint32_t rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          uint32_t bestCellByteOffset)

{
  uint32_t spanByteOffset;
  GridPathCost referenceDistance;
  GridScratchCell *rightEndCell;
  int spanLength;
  uint32_t spanColumn;
  int columnDelta;
  GridPathCost columnDistance;
  GridScratchCell *leftEndCell;
  GridScratchCell *rowCursor;
  GridPathBestUnreachableCell bestResult;
  GridPathCost updatedBestCost;

  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  leftEndCell = GridPathRegion_ScanUnreachedSpanEnd(currentCell,-1);
  rightEndCell = GridPathRegion_ScanUnreachedSpanEnd(currentCell,1);
  /* hex distance from the span to the reference cell; spanByteOffset moves to the span cell nearest to it */
  spanByteOffset = (uint32_t)((uint8_t *)(leftEndCell + 1) - (uint8_t *)g_GridScratchPrimary);
  spanLength = (int)(rightEndCell - leftEndCell) - 2;
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
  rowCursor = (GridScratchCell *)((uint8_t *)(leftEndCell + 1) - rowStrideBytes);
  do {
    if (GridPathRegion_IsUnvisitedUnreachedOpenCell(rowCursor)) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,rowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    rowCursor++;
  } while (rowCursor <= (GridScratchCell *)((uint8_t *)rightEndCell - rowStrideBytes));
  rowCursor = (GridScratchCell *)((uint8_t *)leftEndCell + rowStrideBytes);
  do {
    if (GridPathRegion_IsUnvisitedUnreachedOpenCell(rowCursor)) {
      bestResult = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,rowCursor,bestResult.bestCost,bestResult.bestCellByteOffset);
    }
    rowCursor++;
  } while (rowCursor < (GridScratchCell *)((uint8_t *)rightEndCell + rowStrideBytes));
  return bestResult;
}


/* Footprint walker for GridFootprint_ClearTraversalFlagsAroundWorldPoint: from scratchRecord (a cell whose centre
   is at cellWorldY/X) it walks downwards, two scratch rows and one column left per step, i.e. straight down in
   world space, while the cell centre lies within g_GridInfluenceSquaredThreshold[6] of the centre point. Each cell
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
    if (g_GridInfluenceSquaredThreshold[6] <
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


/* Mirror of GridFootprint_ClearTraversalFlagsDiagonalNegative walking upwards (two scratch rows up and one
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
    if (g_GridInfluenceSquaredThreshold[6] <
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


/* Scanline flood fill for GridReachability_RebuildConnectedRegionAroundWorldPoint: marks the horizontal run of
   open cells around currentCell as visited, then recurses into the open cells of the hex-adjacent spans in the
   rows above and below. A cell is open when none of GRID_REACHABILITY_OPEN_STOP_MASK is set (blocked, terrain
   classes 28..30, low bands 0..6, already visited).
*/
void GridReachability_MarkOpenRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *prevRowEnd;
  GridScratchCell *nextRowCursor;
  GridScratchCell *nextRowEnd;

  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1; (leftStopCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0;
       leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1; (rightStopCell->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0;
       rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* row above: from the span's first cell up to the column of the right stop cell (inclusive); row below: from
     the column of the left stop cell up to the span's last cell. Neither range is ever empty, so testing before
     the first cell matches the original's do-while. */
  prevRowEnd = (GridScratchCell *)((uint8_t *)rightStopCell - rowStrideBytes);
  for (prevRowCursor = (GridScratchCell *)((uint8_t *)(leftStopCell + 1) - rowStrideBytes);
       prevRowCursor <= prevRowEnd; prevRowCursor++) {
    if ((prevRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,prevRowCursor);
    }
  }
  nextRowEnd = (GridScratchCell *)((uint8_t *)rightStopCell + rowStrideBytes);
  for (nextRowCursor = (GridScratchCell *)((uint8_t *)leftStopCell + rowStrideBytes);
       nextRowCursor < nextRowEnd; nextRowCursor++) {
    if ((nextRowCursor->stateMask & GRID_REACHABILITY_OPEN_STOP_MASK) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,nextRowCursor);
    }
  }
}


/* Scanline flood fill that undoes GridReachability_MarkOpenRegionRecursive for one connected piece: clears the
   visited bit across the connected cells that are visited and inside the footprint (pathCost counter non-zero),
   with the same hex-adjacent recursion into the rows above and below.
*/
void GridReachability_ClearCostedRegionRecursive(uint32_t rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *leftStopCell;
  GridScratchCell *rightStopCell;
  GridScratchCell *prevRowCursor;
  GridScratchCell *prevRowEnd;
  GridScratchCell *nextRowCursor;
  GridScratchCell *nextRowEnd;

  currentCell->stateMask = currentCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  for (leftStopCell = currentCell - 1;
       (leftStopCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && leftStopCell->pathCost != 0;
       leftStopCell--) {
    leftStopCell->stateMask = leftStopCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  for (rightStopCell = currentCell + 1;
       (rightStopCell->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && rightStopCell->pathCost != 0;
       rightStopCell++) {
    rightStopCell->stateMask = rightStopCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  /* same row ranges as GridReachability_MarkOpenRegionRecursive; neither is ever empty, so testing before the
     first cell matches the original's do-while */
  prevRowEnd = (GridScratchCell *)((uint8_t *)rightStopCell - rowStrideBytes);
  for (prevRowCursor = (GridScratchCell *)((uint8_t *)(leftStopCell + 1) - rowStrideBytes);
       prevRowCursor <= prevRowEnd; prevRowCursor++) {
    if ((prevRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && prevRowCursor->pathCost != 0) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,prevRowCursor);
    }
  }
  nextRowEnd = (GridScratchCell *)((uint8_t *)rightStopCell + rowStrideBytes);
  for (nextRowCursor = (GridScratchCell *)((uint8_t *)leftStopCell + rowStrideBytes);
       nextRowCursor < nextRowEnd; nextRowCursor++) {
    if ((nextRowCursor->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && nextRowCursor->pathCost != 0) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,nextRowCursor);
    }
  }
}


/* Hex distance on the skewed scratch grid: |dc| + |dr| when both deltas have the same sign, else the larger
   of the two. */
static int GridPathCost_HexDistance(int columnDelta,int rowDelta)

{
  int hexDistance;

  hexDistance = columnDelta;
  if (hexDistance < 0) {
    hexDistance = -hexDistance;
    if (rowDelta < 0) {
      hexDistance = hexDistance - rowDelta;
    }
    else if (hexDistance < rowDelta) {
      hexDistance = rowDelta;
    }
  }
  else if (rowDelta < 0) {
    if (hexDistance < -rowDelta) {
      hexDistance = -rowDelta;
    }
  }
  else {
    hexDistance = hexDistance + rowDelta;
  }
  return hexDistance;
}


/* Checks whether a mover can leave the scratch cell (cellRow, cellColumn): when the cell or one of its six hex
   neighbours is free of g_GridPathBlockingMask and GRID_SCRATCH_BLOCKED, returns false and leaves *outRow and
   *outColumn untouched. Otherwise returns true and writes the nearest (hex distance) free cell within +-16
   rows/columns, or the cell itself when there is none.
*/
bool GridPathCost_RelocateFromBlockedCell
          (FieldGridCellCoordinate cellRow,FieldGridCellCoordinate cellColumn,FieldGridCellCoordinate *outRow,
          FieldGridCellCoordinate *outColumn)

{
  int cellIndex;
  int minColumn;
  int scanColumn;
  int hexDistance;
  uint32_t maxColumn;
  int scanWidth;
  GridScratchStateMask blockedMask;
  uint32_t maxRow;
  int bestHexDistance;
  int searchRow;
  int rowsRemaining;
  GridScratchCell *scanCell;
  GridScratchCell *rowStartCell;
  GridScratchCell *rowAboveCell;
  int bestRow;
  int bestColumn;

  cellIndex = cellRow * g_GridScratchWidth + cellColumn;
  blockedMask = g_GridPathBlockingMask | GRID_SCRATCH_BLOCKED;
  /* the cell itself, then its six hex neighbours: above, above-right, left, right, below-left, below */
  rowAboveCell = g_GridScratchPrimary + cellIndex - g_GridScratchWidth;
  if ((g_GridScratchPrimary[cellIndex].stateMask & blockedMask) == 0 ||
      (rowAboveCell->stateMask & blockedMask) == 0 ||
      (rowAboveCell[1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth - 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth + 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth * 2 - 1].stateMask & blockedMask) == 0 ||
      (rowAboveCell[g_GridScratchWidth * 2].stateMask & blockedMask) == 0) {
    /* open cell: both callers (EntityPathing_ResolveDestinationAndRebuildRoutes,
       EntityPathing_UpdateRouteSegment) read the cell only after a relocation */
    return false;
  }
  /* search window: +-GRID_PATH_NEAREST_SEARCH_RADIUS, clipped to the grid */
  minColumn = cellColumn - GRID_PATH_NEAREST_SEARCH_RADIUS;
  if (minColumn < 0) {
    minColumn = 0;
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
  scanWidth = maxColumn - minColumn;
  rowsRemaining = maxRow - searchRow;
  if (scanWidth == 0 || minColumn > (int)maxColumn || rowsRemaining == 0 || searchRow > (int)maxRow) {
    *outRow = cellRow;
    *outColumn = cellColumn;
    return true;
  }
  bestHexDistance = INT32_MAX;
  rowStartCell = g_GridScratchPrimary + searchRow * g_GridScratchWidth + minColumn;
  for (; rowsRemaining != 0; rowsRemaining--) {
    scanCell = rowStartCell;
    for (scanColumn = minColumn; scanColumn != minColumn + scanWidth; scanColumn++) {
      if ((scanCell->stateMask & blockedMask) == 0) {
        hexDistance = GridPathCost_HexDistance(scanColumn - cellColumn,searchRow - cellRow);
        if (hexDistance < bestHexDistance) {
          bestHexDistance = hexDistance;
          bestRow = searchRow;
          bestColumn = scanColumn;
        }
      }
      scanCell++;
    }
    rowStartCell = rowStartCell + g_GridScratchWidth;
    searchRow++;
  }
  if (bestHexDistance < INT32_MAX) {
    *outRow = bestRow;
    *outColumn = bestColumn;
    return true;
  }
  *outRow = cellRow;
  *outColumn = cellColumn;
  return true;
}


/* True when a cell blocks a line segment: blocked (bit 31), lacking the mover's faction presence bit, or having
   a g_GridPathBlockingMask or callerBlockingMask bit. */
static bool GridPathLine_CellBlocksSegment(FieldGridRegionMask callerBlockingMask,GridScratchCell *cell)

{
  GridScratchStateMask cellState;

  cellState = cell->stateMask;
  return (int)cellState < 0 || (g_GridPathEntityClassMask & cellState) == 0 ||
         (g_GridPathBlockingMask & cellState) != 0 || (callerBlockingMask & cellState) != 0;
}


/* Downward segment: scans one column from columnStartCell up to columnEndCell (inclusive); true as soon as a
   cell other than startCell blocks the segment. */
static bool GridPathLine_ColumnBlocksSegment(FieldGridRegionMask callerBlockingMask,GridScratchCell *startCell,
          GridScratchCell *columnStartCell,GridScratchCell *columnEndCell)

{
  GridScratchCell *columnScanCell;

  columnScanCell = columnStartCell;
  while (columnScanCell == startCell || !GridPathLine_CellBlocksSegment(callerBlockingMask,columnScanCell)) {
    if (columnScanCell == columnEndCell) {
      return false;
    }
    columnScanCell = columnScanCell - g_GridScratchWidth;
  }
  return true;
}


/* Downward segment: moves lineCursor down the rows while the row error allows it, stopping at endCell.
   Returns the new cursor. */
static GridScratchCell *GridPathLine_AdvanceDownRows(GridScratchCell *lineCursor,GridScratchCell *endCell,
          int rowDelta,int columnDelta,int *rowError)

{
  while (*rowError < rowDelta) {
    lineCursor = lineCursor + g_GridScratchWidth;
    *rowError = *rowError + columnDelta;
    if (lineCursor == endCell) break;
  }
  return lineCursor;
}


/* Rasterises the straight line from startCell (at startRow, startColumn) to endCell over the scratch grid and
   returns true as soon as a cell other than startCell is blocked (bit 31), lacks the mover's faction
   presence bit (g_GridPathEntityClassMask), or has a g_GridPathBlockingMask or callerBlockingMask bit; false
   when the whole line is clear. The line is always walked left to right;
   upward lines step row by row, downward lines column by column.
*/
bool GridPathLine_TestHexSegmentBlocked(FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell)

{
  uint32_t endCellIndex;
  int rowDelta;
  int upwardStepThreshold;
  int columnsLeft;
  int columnDelta;
  int slopeError;
  int rowError;
  GridScratchCell *lineCursor;
  GridScratchCell *columnEndCell;

  endCellIndex = (uint32_t)(endCell - g_GridScratchPrimary);
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
    upwardStepThreshold = -rowDelta - columnDelta;
    while (lineCursor == startCell || !GridPathLine_CellBlocksSegment(callerBlockingMask,lineCursor)) {
      if (lineCursor == endCell) {
        return false;
      }
      if (slopeError < upwardStepThreshold) {
        /* step up one row only */
        slopeError = slopeError + columnDelta * 2;
        lineCursor = lineCursor - g_GridScratchWidth;
      }
      else {
        if (slopeError == upwardStepThreshold) {
          /* diagonal: step up one row, then right */
          slopeError = slopeError + columnDelta * 2;
          lineCursor = lineCursor - g_GridScratchWidth;
        }
        slopeError = slopeError + rowDelta * 2;
        lineCursor++;
      }
    }
    return true;
  }
  rowError = 0;
  slopeError = 0;
  columnsLeft = columnDelta;
  columnEndCell = lineCursor;
  lineCursor = GridPathLine_AdvanceDownRows(lineCursor,endCell,rowDelta,columnDelta,&rowError);
  rowError = rowError - rowDelta;
  /* scan each column from lineCursor up to columnEndCell, then move one column right; the rows are advanced
     again before every column except the last one */
  while (!GridPathLine_ColumnBlocksSegment(callerBlockingMask,startCell,lineCursor,columnEndCell)) {
    if (columnsLeft == 0) {
      return false;
    }
    columnsLeft--;
    lineCursor++;
    columnEndCell++;
    for (; slopeError <= -columnDelta; slopeError = slopeError + columnDelta) {
      columnEndCell = columnEndCell + g_GridScratchWidth;
    }
    slopeError = slopeError - rowDelta;
    if (columnsLeft != 0) {
      lineCursor = GridPathLine_AdvanceDownRows(lineCursor,endCell,rowDelta,columnDelta,&rowError);
    }
    rowError = rowError - rowDelta;
  }
  return true;
}

