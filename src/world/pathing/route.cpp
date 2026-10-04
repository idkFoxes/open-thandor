/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/route.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Entity routes: destination resolution and route rebuilds for one entity or an overlapping group, route
   segment updates and the footprint traversal flags around a world point. */

#include <thandor/world/pathing/route.h>
#include <thandor/thandor.h>

/* The 32 pairs g_EntityPathingPriorityPairs points at (EntityPathing_RebuildOverlappingGroupRoutes
   fills at most ENTITY_PATHING_PRIORITY_PAIR_CAPACITY of them and heap-sorts them in place) */
#define ENTITY_PATHING_PRIORITY_PAIR_CAPACITY 32

/* Module data. */

static EntityPathingPriorityPair g_EntityPathingPriorityPairStorage[ENTITY_PATHING_PRIORITY_PAIR_CAPACITY] = {0};

static uint32_t g_EntityPathingPriorityPairCount = 0;

EntityPathingPriorityPair *g_EntityPathingPriorityPairs = g_EntityPathingPriorityPairStorage;

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
  Bool8 segmentBlocked;
  WorldPositionXY primaryWorldPosition;
  Bool8 startRelocated;
  FieldGridCellCoordinate nearestRow;
  FieldGridCellCoordinate nearestColumn;
  FieldGridCellCoordinate reachableRow;
  FieldGridCellCoordinate reachableColumn;
  PathingDestination resolvedDestination;
  Bool8 backtrackReachedTarget;
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
  modelDefinition = (ModelDefinition *)(routeEntityRuntime->common).ownership.definitionOrClassRecord;
  overlappedEntity =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  runtimeClassId = modelDefinition->runtimeClassId;
  if (overlappedEntity != NULL) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
      [((ModelDefinition *)(overlappedEntity->common).ownership.definitionOrClassRecord)->runtimeClassId])
              (overlappedEntity);
  }
  armyRuntime = (ArmyRuntimeSlot *)(routeEntityRuntime->common).ownership.runtimeLink;
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
    routeScratchCell = g_GridScratchPrimary + (int32_t)(startRow * g_GridScratchWidth) + startColumn;
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
                       (int32_t)((startRow * g_GridScratchWidth + startColumn) - g_GridScratchWidth);
      }
      fallbackWorldPosition.worldYQ12 = targetWorldYQ12;
      fallbackWorldPosition.worldXQ12 = targetWorldXQ12;
      /* high-cost cells block the backtrack's straight-line test, unless bit 1 of the runtime record's
         movementStateFlags is set */
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

/* Plans the routes of every runtime model near the move of routeEntityRuntime together, on a copy of the scratch
   grid (the real grid is swapped back at the end). Up to 32 models whose depth-bin masks overlap the box around
   the move are collected and heap-sorted by priority (other factions lowest, then the own faction plus the
   definition's movementSpeed weight); after their influence is added to the copy, other-faction models only stamp their
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
  Bool8 masksOverlap;
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
      candidateEntity = (GameEntityRuntime *)ownerNode->runtimePayload;
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
    /* priority: 0 when bit 1 of the runtime record's movementStateFlags is set, 1 for another faction
       (factionIndex), 2 for the own faction, plus the definition's movementSpeed weight when flag bit 0 is clear */
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
  centerCellCursor = g_GridScratchPrimary + (int32_t)(g_GridScratchWidth * centerRow) + centerColumn;
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
            (worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,(uint32_t *)&leftWalkCursor->stateMask) != 0) &&
         (GridFootprint_ClearTraversalFlagsDiagonalNegative
            (worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,(uint32_t *)&leftWalkCursor->stateMask) != 0)) {
    leftWalkCursor--;
    leftCellWorldX = leftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* centre row rightwards (the centre column again) */
  rightCellWorldX = centerCellWorldX;
  rightWalkCursor = centerCellCursor;
  while ((GridFootprint_ClearTraversalFlagsDiagonalPositive
            (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,(uint32_t *)&rightWalkCursor->stateMask) != 0) &&
         (GridFootprint_ClearTraversalFlagsDiagonalNegative
            (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,(uint32_t *)&rightWalkCursor->stateMask) != 0)) {
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
            (uint32_t *)&leftWalkCursor->stateMask) != 0) {
    leftWalkCursor--;
    sideRowLeftWorldX = sideRowLeftWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  while (GridFootprint_ClearTraversalFlagsDiagonalPositive
           (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,sideRowRightWorldX,
            (uint32_t *)&rightWalkCursor->stateMask) != 0) {
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
            (uint32_t *)&rightWalkCursor->stateMask) != 0) {
    rightWalkCursor++;
    sideRowRightWorldX = sideRowRightWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  while (GridFootprint_ClearTraversalFlagsDiagonalNegative
           (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,sideRowLeftWorldX,
            (uint32_t *)&leftWalkCursor->stateMask) != 0) {
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
  Bool8 segmentBlocked;
  WorldPositionXY resolvedTarget;
  Bool8 startRelocated;
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
                       (0,startRow,startColumn,g_GridScratchPrimary + (int32_t)(startRow * g_GridScratchWidth) + startColumn,
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
    /* The segment from the entity to the
       target is split in halves until each piece spans at most one scratch column (0x240) on both axes (or 64 pieces
       are pending); the low-distance influence bands are stamped at the end of each piece. The
       original keeps the pending pieces as pushed frames on the machine stack, processing the
       half towards the target first; here they live in the pieces array. */
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

/* Footprint walker for GridFootprint_ClearTraversalFlagsAroundWorldPoint: from scratchRecord (a cell whose centre
   is at cellWorldY/X) it walks downwards, two scratch rows and one column left per step, i.e. straight down in
   world space, while the cell centre lies within g_GridInfluenceSquaredThreshold[6] of the centre point. Each cell
   loses its blocked and visited bits and has its pathCost counter incremented. Stops before a blocked cell.
   Returns the number of cells cleared, one less when the walk ended at a blocked cell (as in the original).
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
    scratchRecord = scratchRecord + (int32_t)(g_GridScratchWidth * -4) + 2;
    nextCount = visitedCount + 1;
  } while ((*scratchRecord & GRID_SCRATCH_BLOCKED) == 0);
  return visitedCount;
}

/* Heap-building step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): the newly
   appended last entity/priority pair moves up the max-heap, swapping with its parent while its priority is
   larger.
*/
void PriorityPairHeap_SiftUp(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  EntityPathingPriorityPair *parentHeapPair;
  uint32_t parentSearchIndex;
  EntityPathingPriorityPair *currentHeapPair;
  int childPriority;
  GameEntityRuntime *childEntity;

  /* parentSearchIndex is the current index - 1: its half is the parent index */
  parentSearchIndex = heapSize - 2;
  currentHeapPair = heapBase + heapSize - 1;
  if (1 < heapSize) {
    do {
      parentHeapPair = heapBase + (parentSearchIndex >> 1);
      childPriority = currentHeapPair->priority;
      if (childPriority <= parentHeapPair->priority) {
        return;
      }
      currentHeapPair->priority = parentHeapPair->priority;
      parentHeapPair->priority = childPriority;
      childEntity = currentHeapPair->entity;
      currentHeapPair->entity = parentHeapPair->entity;
      parentHeapPair->entity = childEntity;
      parentSearchIndex = (parentSearchIndex >> 1) - 1;
      currentHeapPair = parentHeapPair;
    } while (-1 < (int)parentSearchIndex);
  }
}

/* Extraction step of the heapsort of g_EntityPathingPriorityPairs (world/pathing/grid): after the root was
   swapped with the last entry, the new root entity/priority pair moves down the max-heap, swapping with its
   larger-priority child while that child is larger.
*/
void PriorityPairHeap_SiftDown(PriorityPairHeapCount heapSize,EntityPathingPriorityPair *heapBase)

{
  int selectedChildPriority;
  uint32_t leftChildIndex;
  uint32_t selectedChildIndex;
  GameEntityRuntime *selectedChildEntity;
  EntityPathingPriorityPair *currentHeapPair;
  int32_t displacedParentPriority;
  GameEntityRuntime *displacedParentEntity;

  currentHeapPair = heapBase;
  /* children of index i are 2i + 1 and 2i + 2; the index comparisons are unsigned (heapSize - 1U) */
  leftChildIndex = 1;
  while (leftChildIndex <= heapSize - 1U) {
    selectedChildIndex = leftChildIndex;
    selectedChildPriority = heapBase[selectedChildIndex].priority;
    selectedChildEntity = heapBase[selectedChildIndex].entity;
    if ((leftChildIndex < heapSize - 1U) &&
       (selectedChildPriority < heapBase[leftChildIndex + 1].priority)) {
      selectedChildIndex = leftChildIndex + 1;
      selectedChildPriority = heapBase[selectedChildIndex].priority;
      selectedChildEntity = heapBase[selectedChildIndex].entity;
    }
    if (selectedChildPriority <= currentHeapPair->priority) {
      return;
    }
    /* swap parent and child */
    displacedParentPriority = currentHeapPair->priority;
    currentHeapPair->priority = selectedChildPriority;
    displacedParentEntity = currentHeapPair->entity;
    currentHeapPair->entity = selectedChildEntity;
    heapBase[selectedChildIndex].priority = displacedParentPriority;
    heapBase[selectedChildIndex].entity = displacedParentEntity;
    currentHeapPair = heapBase + selectedChildIndex;
    leftChildIndex = selectedChildIndex * 2 + 1;
  }
}
