/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/influence.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/pathing/influence.h>
#include <thandor/thandor.h>

/* Module data. */

/* uint32_t[8] grid influence ring radius offsets */
const uint32_t g_GridInfluenceRadiusOffset[8] = {
    1000, /* [0] */
    1250, /* [1] */
    1500, /* [2] */
    1600, /* [3] */
    1920, /* [4] */
    2240, /* [5] */
    2600, /* [6] */
    4100, /* [7] */
};

/* uint32_t[8] squared ring radii (g_GridInfluenceRadiusOffset[n] + radius + margin)^2 */
uint32_t g_GridInfluenceSquaredThreshold[8] = {
    0, /* [0] */
    0, /* [1] */
    0, /* [2] */
    0, /* [3] */
    0, /* [4] */
    0, /* [5] */
    0, /* [6] */
    0, /* [7] */
};

/* gridInfluenceAdd handler of the runtime classes 4, 10..16, 20 and 22
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd): stamps the low distance bands around the
   model's current position, unless its definition has no influence radius.
*/
void GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  ModelDefinition *entityDefinition;

  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = entityRuntime->common.ownership.modelDefinition();
  if (entityDefinition->footprintRadius != 0) {
    GridInfluence_SetLowDistanceBandsAroundWorldPoint
              (entityDefinition->footprintRadius,
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
}


/* gridInfluenceRemove handler of the runtime classes 4, 10..16, 20 and 22
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove): clears the low distance bands around the
   model's current position, the counterpart of GridInfluence_AddLowDistanceBands.
*/
void GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  ModelDefinition *entityDefinition;

  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = entityRuntime->common.ownership.modelDefinition();
  if (entityDefinition->footprintRadius != 0) {
    GridInfluence_ClearLowDistanceBandsAroundWorldPoint
              (entityDefinition->footprintRadius,
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
}


/* gridInfluenceAdd handler of the runtime classes 0..3, 17..19 and 23
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd): stamps the high distance bands around the
   model's position and remembers that position in the linked runtime (classLinkState.classState68 and armyLinkOrState.classState), so the removal clears the
   same disc even after the model has moved.
*/
void GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *linkedRuntime;
  Q12 worldXQ12;
  Q12 worldYQ12;
  ModelRuntimeNode *modelNode;
  ModelDefinition *entityDefinition;

  linkedRuntime = entityRuntime->common.ownership.linkedModelRuntime();
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = entityRuntime->common.ownership.modelDefinition();
  if (entityDefinition->footprintRadius != 0) {
    worldXQ12 = (modelNode->worldTransform).translation.x;
    worldYQ12 = (modelNode->worldTransform).translation.y;
    linkedRuntime->classLinkState.classState68 = worldXQ12;
    linkedRuntime->classLinkState.armyLinkOrState.classState = worldYQ12;
    GridInfluence_SetHighDistanceBandsAroundWorldPoint(entityDefinition->footprintRadius,worldYQ12,worldXQ12);
  }
}


/* gridInfluenceRemove handler of the runtime classes 0..3, 17..19 and 23
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove): clears the high distance bands around
   the position GridInfluence_AddHighDistanceBands stored in the linked runtime.
*/
void GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeSlot *linkedRuntime;
  ModelDefinition *entityDefinition;

  linkedRuntime = entityRuntime->common.ownership.linkedModelRuntime();
  entityDefinition = entityRuntime->common.ownership.modelDefinition();
  if (entityDefinition->footprintRadius != 0) {
    GridInfluence_ClearHighDistanceBandsAroundWorldPoint
              (entityDefinition->footprintRadius,(Q12)linkedRuntime->classLinkState.armyLinkOrState.classState,
               (Q12)linkedRuntime->classLinkState.classState68);
  }
}


/* gridInfluenceAdd handler of the runtime classes 5..9 and 21, which leave no influence in the scratch grid
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd). Does nothing.
*/
void GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime)

{
}


/* gridInfluenceRemove handler of the runtime classes 5..9 and 21 (the counterpart of GridInfluence_AddNoOp).
   Does nothing.
*/
void GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime)

{
}


/* Clears the low and high distance bands (scratch bits 8..23) of every scratch cell, then lets every runtime
   model re-add its influence through the gridInfluenceAdd handler of its runtime class. Runs on tick-wheel
   case 4 and, on the other simulation-tick path, every 16th tick.
*/
void GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode *entityListHead)

{
  int cellsRemaining;
  int cellInBlock;
  uint32_t *stateMaskCursor; /* dword view of the 8-byte scratch cells: [2n] = stateMask of cell n */
  WorldOwnerListNode *entityNode;
  ModelDefinition *entityDefinition;

  /* Original quirk: the cells are cleared in whole blocks of 16 (at least one block), so a cell count that is not
     a multiple of 16 clears up to 15 cells past the end of the grid. */
  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  stateMaskCursor = (uint32_t *)&g_GridScratchPrimary->stateMask;
  do {
    for (cellInBlock = 0; cellInBlock < 16; cellInBlock++) {
      stateMaskCursor[cellInBlock * 2] = stateMaskCursor[cellInBlock * 2] & ~GRID_SCRATCH_DISTANCE_BANDS;
    }
    stateMaskCursor = stateMaskCursor + 32;
    cellsRemaining = cellsRemaining - 16;
  } while (cellsRemaining > 0);
  for (entityNode = entityListHead; entityNode != nullptr; entityNode = entityNode->nextNode) {
    if (entityNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      entityDefinition = WorldOwnerNode_EntityRuntime(entityNode)->common.ownership.modelDefinition();
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[entityDefinition->runtimeClassId])
        (WorldOwnerNode_EntityRuntime(entityNode));
    }
  }
}


/* One of the eight band walkers (GridInfluence_{Set,Clear}{Low,High}DistanceBandsDiagonal{Positive,Negative}):
   updates the cells of one vertical walk and returns how many it touched (0: the start cell is inside no ring). */
using GridInfluenceBandWalker = int
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,uint32_t *scratchCell);

/* Shared body of the four GridInfluence_{Set,Clear}{Low,High}DistanceBandsAroundWorldPoint functions, which differ
   only in their pair of band walkers: walkUp (the ...DiagonalPositive walker) and walkDown (the ...DiagonalNegative
   walker). Builds the eight squared ring thresholds, finds the scratch cell under the world point and covers the
   disc with vertical walks. Nothing happens off the grid or on a blocked centre cell. */
static void GridInfluence_WalkDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12,GridInfluenceBandWalker *walkUp,
          GridInfluenceBandWalker *walkDown)
{
  int64_t wideProduct;
  int radiusWithMargin;
  uint32_t rowTerm;
  int column;
  int row;
  int rowCenterQ12;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  GridScratchCell *centerCell;
  GridScratchCell *leftCell;
  GridScratchCell *rightCell;
  uint32_t leftCellWorldX;
  uint32_t rightCellWorldX;
  GridScratchCell *aboveRowCell;
  int aboveLeftWorldX;
  int aboveRightWorldX;
  GridScratchCell *belowRowCell;
  int belowLeftWorldX;
  int belowRightWorldX;

  radiusWithMargin = radiusMetric + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold[0] =
       (g_GridInfluenceRadiusOffset[0] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[0] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[1] =
       (g_GridInfluenceRadiusOffset[1] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[1] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[2] =
       (g_GridInfluenceRadiusOffset[2] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[2] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[3] =
       (g_GridInfluenceRadiusOffset[3] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[3] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[4] =
       (g_GridInfluenceRadiusOffset[4] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[4] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[5] =
       (g_GridInfluenceRadiusOffset[5] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[5] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[6] =
       (g_GridInfluenceRadiusOffset[6] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[6] + radiusWithMargin);
  g_GridInfluenceSquaredThreshold[7] =
       (g_GridInfluenceRadiusOffset[7] + radiusWithMargin) * (g_GridInfluenceRadiusOffset[7] + radiusWithMargin);
  /* scratch cell under the world point */
  rowTerm = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  column = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rowTerm) +
                 GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  row = (int)(rowTerm * 2 + GRID_SCRATCH_INDEX_BIAS_Q12) >> GRID_SCRATCH_CELL_SHIFT;
  if (column < 0 || row < 0 || column >= (int)g_GridScratchWidth || row >= (int)g_GridScratchHeight) {
    return;
  }
  /* world position of the centre cell's centre */
  centerCell = g_GridScratchPrimary + (int32_t)(g_GridScratchWidth * row) + column;
  rowCenterQ12 = row * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12;
  wideProduct = (int64_t)(rowCenterQ12 + (column * GRID_SCRATCH_CELL_Q12 - GRID_SCRATCH_CELL_CENTER_Q12) * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
  centerCellWorldX = (int)((uint64_t)wideProduct >> 32) << 19 | (uint32_t)wideProduct >> 13;
  wideProduct = (int64_t)rowCenterQ12 * FIELD_GRID_WORLD_ROW_STEP_Y;
  centerCellWorldY = (int)((uint64_t)wideProduct >> 32) << 20 | (uint32_t)wideProduct >> 12;
  if ((centerCell->stateMask & GRID_SCRATCH_BLOCKED) != 0) {
    return;
  }
  /* centre row leftwards, walking up and down, until a column has nothing inside */
  leftCell = centerCell;
  leftCellWorldX = centerCellWorldX;
  while (walkUp(worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,(uint32_t *)&leftCell->stateMask) != 0 &&
         walkDown(worldYQ12,worldXQ12,centerCellWorldY,leftCellWorldX,(uint32_t *)&leftCell->stateMask) != 0) {
    leftCell--;
    leftCellWorldX = leftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* centre row rightwards (the centre column again) */
  rightCell = centerCell;
  rightCellWorldX = centerCellWorldX;
  while (walkUp(worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,(uint32_t *)&rightCell->stateMask) != 0 &&
         walkDown(worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,(uint32_t *)&rightCell->stateMask) != 0) {
    rightCell++;
    rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* row above: upward walks, leftwards then rightwards */
  aboveRowCell = centerCell - g_GridScratchWidth;
  aboveLeftWorldX = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  aboveRightWorldX = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  leftCell = aboveRowCell;
  while (walkUp(worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,aboveLeftWorldX,
                (uint32_t *)&leftCell->stateMask) != 0) {
    leftCell--;
    aboveLeftWorldX = aboveLeftWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
  rightCell = aboveRowCell + 1;
  while (walkUp(worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,aboveRightWorldX,
                (uint32_t *)&rightCell->stateMask) != 0) {
    rightCell++;
    aboveRightWorldX = aboveRightWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  /* row below: downward walks, rightwards then leftwards */
  belowRowCell = centerCell + g_GridScratchWidth;
  belowRightWorldX = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  belowLeftWorldX = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
  rightCell = belowRowCell;
  while (walkDown(worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,belowRightWorldX,
                  (uint32_t *)&rightCell->stateMask) != 0) {
    rightCell++;
    belowRightWorldX = belowRightWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
  }
  leftCell = belowRowCell - 1;
  while (walkDown(worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,belowLeftWorldX,
                  (uint32_t *)&leftCell->stateMask) != 0) {
    leftCell--;
    belowLeftWorldX = belowLeftWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
  }
}


/* Stamps a model's influence into the scratch grid: eight concentric rings (radiusMetric plus
   g_GridInfluenceRadiusOffset[0..7] plus the margin, squared into g_GridInfluenceSquaredThreshold[0..7]) around the
   world point set low distance band bit n (scratch bit 8 + n) in every cell whose centre lies within ring n.
   The disc is covered like GridFootprint_ClearTraversalFlagsAroundWorldPoint: vertical walks from the centre row
   leftwards and rightwards, then upwards from the row above and downwards from the row below. Nothing happens
   off the grid or on a blocked centre cell.
*/
void GridInfluence_SetLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  GridInfluence_WalkDistanceBandsAroundWorldPoint
            (radiusMetric,worldYQ12,worldXQ12,GridInfluence_SetLowDistanceBandsDiagonalPositive,
             GridInfluence_SetLowDistanceBandsDiagonalNegative);
}


/* High-band twin of GridInfluence_SetLowDistanceBandsAroundWorldPoint: builds the same eight ring thresholds and
   covers the disc the same way, but sets high distance band bit n (scratch bit 16 + n). Called by
   GridInfluence_AddHighDistanceBands.
*/
void GridInfluence_SetHighDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  GridInfluence_WalkDistanceBandsAroundWorldPoint
            (radiusMetric,worldYQ12,worldXQ12,GridInfluence_SetHighDistanceBandsDiagonalPositive,
             GridInfluence_SetHighDistanceBandsDiagonalNegative);
}


/* Undoes GridInfluence_SetLowDistanceBandsAroundWorldPoint: walks the same eight rings around the world point and
   clears low distance band bit n (scratch bit 8 + n) in every cell inside ring n. Called by
   GridInfluence_RemoveLowDistanceBands.
*/
void GridInfluence_ClearLowDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  GridInfluence_WalkDistanceBandsAroundWorldPoint
            (radiusMetric,worldYQ12,worldXQ12,GridInfluence_ClearLowDistanceBandsDiagonalPositive,
             GridInfluence_ClearLowDistanceBandsDiagonalNegative);
}


/* Undoes GridInfluence_SetHighDistanceBandsAroundWorldPoint: walks the same eight rings around the world point and
   clears high distance band bit n (scratch bit 16 + n) in every cell inside ring n. Called by
   GridInfluence_RemoveHighDistanceBands.
*/
void GridInfluence_ClearHighDistanceBandsAroundWorldPoint(FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  GridInfluence_WalkDistanceBandsAroundWorldPoint
            (radiusMetric,worldYQ12,worldXQ12,GridInfluence_ClearHighDistanceBandsDiagonalPositive,
             GridInfluence_ClearHighDistanceBandsDiagonalNegative);
}


/* Ring mask of the band walkers: bit n (0..7) is set when squaredDistance lies within ring n, i.e. is at most
   g_GridInfluenceSquaredThreshold[n] (unsigned compare). */
static uint32_t GridInfluence_RingBandMask(uint32_t squaredDistance)
{
  return (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[0]) |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[1]) << 1 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[2]) << 2 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[3]) << 3 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[4]) << 4 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[5]) << 5 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[6]) << 6 |
         (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold[7]) << 7;
}


/* Band walker for GridInfluence_SetLowDistanceBandsAroundWorldPoint: from scratchCell (a cell whose centre is at
   cellWorldY/X) it walks downwards, two scratch rows and one column left per step (straight down in world space),
   and ORs low distance band bit n (scratch bit 8 + n) into each cell whose centre lies within
   g_GridInfluenceSquaredThreshold[n] of the centre point. Stops at the first cell inside no ring or before a
   blocked cell. Returns the number of cells written, one less when the walk ended at a blocked cell (as in the
   original).
*/
int GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  /* the walk is straight down in world space, so the X distance stays the same */
  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMask * GRID_SCRATCH_LOW_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Mirror of GridInfluence_SetLowDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step), with the same band bits and the same return value.
*/
int GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMask * GRID_SCRATCH_LOW_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + (int32_t)(g_GridScratchWidth * -4) + 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* High-band twin of GridInfluence_SetLowDistanceBandsDiagonalNegative: walks downwards from scratchCell and ORs
   high distance band bit n (scratch bit 16 + n) into each cell inside ring n. Same stop rule and return value.
*/
int GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMask * GRID_SCRATCH_HIGH_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Mirror of GridInfluence_SetHighDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step).
*/
int GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMask * GRID_SCRATCH_HIGH_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + (int32_t)(g_GridScratchWidth * -4) + 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Clearing twin of GridInfluence_SetLowDistanceBandsDiagonalNegative: walks downwards from scratchCell and clears
   low distance band bit n (scratch bit 8 + n) in each cell inside ring n. Same stop rule and return value.
*/
int GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMask * GRID_SCRATCH_LOW_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Mirror of GridInfluence_ClearLowDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step).
*/
int GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMask * GRID_SCRATCH_LOW_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + (int32_t)(g_GridScratchWidth * -4) + 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Clearing twin of GridInfluence_SetHighDistanceBandsDiagonalNegative: walks downwards from scratchCell and clears
   high distance band bit n (scratch bit 16 + n) in each cell inside ring n. Same stop rule and return value.
*/
int GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMask * GRID_SCRATCH_HIGH_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


/* Mirror of GridInfluence_ClearHighDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one
   column right per step).
*/
int GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int squaredXDistance;
  int squaredYDistance;
  int cellsWritten;
  uint32_t bandMask;

  squaredXDistance = (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  cellsWritten = 0;
  bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  while (bandMask != 0) {
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMask * GRID_SCRATCH_HIGH_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + (int32_t)(g_GridScratchWidth * -4) + 2;
    if ((*scratchCell & GRID_SCRATCH_BLOCKED) != 0) {
      return cellsWritten; /* the cell just written is not counted */
    }
    cellsWritten = cellsWritten + 1;
    bandMask = GridInfluence_RingBandMask(squaredYDistance + squaredXDistance);
  }
  return cellsWritten;
}


