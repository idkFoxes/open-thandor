/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/influence.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/pathing/influence.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/pathing/influence. */

/* Address: 0x00527330.
   gridInfluenceAdd handler of the runtime classes 4, 10..16, 20 and 22
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd): stamps the low distance bands around the
   model's current position, unless its definition has no influence radius.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET) != 0) {
    GridInfluence_SetLowDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET),
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
}


/* Address: 0x00527380.
   gridInfluenceRemove handler of the runtime classes 4, 10..16, 20 and 22
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove): clears the low distance bands around the
   model's current position, the counterpart of GridInfluence_AddLowDistanceBands.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET) != 0) {
    GridInfluence_ClearLowDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET),
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
}


/* Address: 0x00528070.
   gridInfluenceAdd handler of the runtime classes 0..3, 17..19 and 23
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd): stamps the high distance bands around the
   model's position and remembers that position in the linked runtime (+0x68/+0x6C), so the removal clears the
   same disc even after the model has moved.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *linkedRuntime;
  Q12 worldXQ12;
  Q12 worldYQ12;
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  linkedRuntime = (entityRuntime->common).ownership.runtimeLink;
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET) != 0) {
    worldXQ12 = (modelNode->worldTransform).translation.x;
    worldYQ12 = (modelNode->worldTransform).translation.y;
    *(Q12 *)((int)linkedRuntime + 0x68) = worldXQ12;
    *(Q12 *)((int)linkedRuntime + 0x6c) = worldYQ12;
    GridInfluence_SetHighDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET),worldYQ12,worldXQ12);
  }
}


/* Address: 0x005280D0.
   gridInfluenceRemove handler of the runtime classes 0..3, 17..19 and 23
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove): clears the high distance bands around
   the position GridInfluence_AddHighDistanceBands stored in the linked runtime.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *linkedRuntime;
  void *entityDefinition;
  
  linkedRuntime = (entityRuntime->common).ownership.runtimeLink;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET) != 0) {
    GridInfluence_ClearHighDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + MODEL_DEFINITION_RADIUS_OFFSET),
               *(Q12 *)((int)linkedRuntime + 0x6c),*(Q12 *)((int)linkedRuntime + 0x68));
  }
}


/* Address: 0x00527B50.
   gridInfluenceAdd handler of the runtime classes 5..9 and 21, which leave no influence in the scratch grid
   (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd). Returns with RET 4 and keeps EAX and the
   flags.
*/
void __thandor_void_preserve_eax_ecx_edx GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime)

{
  return;
}


/* Address: 0x00527B60.
   gridInfluenceRemove handler of the runtime classes 5..9 and 21 (the counterpart of GridInfluence_AddNoOp).
   Returns with RET 4 and keeps EAX and the flags.
*/
void __thandor_void_preserve_eax_ecx_edx GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime)

{
  return;
}


/* Address: 0x00535A30.
   Clears the low and high distance bands (scratch bits 8..23) of every scratch cell, then lets every runtime
   model re-add its influence through the gridInfluenceAdd handler of its runtime class. Runs on tick-wheel
   case 4 and, on the other simulation-tick path, every 16th tick.
*/
void __thandor_preserve_eax
GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode100 *entityListHead)

{
  int cellsRemaining;
  int nextCellsRemaining;
  uint32_t *scratchRecordCursor; /* dword view of the 8-byte scratch cells: [2n] = stateMask of cell n */
  bool fullBlockRemaining;
  
  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  scratchRecordCursor = &g_GridScratchPrimary->stateMask;
  do {
    *scratchRecordCursor = *scratchRecordCursor & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[2] = scratchRecordCursor[2] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[4] = scratchRecordCursor[4] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[6] = scratchRecordCursor[6] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[8] = scratchRecordCursor[8] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[10] = scratchRecordCursor[10] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[12] = scratchRecordCursor[12] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[14] = scratchRecordCursor[14] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[16] = scratchRecordCursor[16] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[18] = scratchRecordCursor[18] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[20] = scratchRecordCursor[20] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[22] = scratchRecordCursor[22] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[24] = scratchRecordCursor[24] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[26] = scratchRecordCursor[26] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[28] = scratchRecordCursor[28] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor[30] = scratchRecordCursor[30] & ~GRID_SCRATCH_DISTANCE_BANDS;
    scratchRecordCursor = scratchRecordCursor + 32;
    nextCellsRemaining = cellsRemaining - 16;
    fullBlockRemaining = 15 < cellsRemaining;
    cellsRemaining = nextCellsRemaining;
  } while (nextCellsRemaining != 0 && fullBlockRemaining);
  for (; entityListHead != NULL; entityListHead = entityListHead->nextNode)
  {
    if (entityListHead->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      /* +0x4C of the definition record is its ModelRuntimeClassId */
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [*(int *)((int)(((GameEntityRuntime *)entityListHead->runtimePayload)->common).ownership.definitionOrClassRecord +
                 0x4c)])(entityListHead->runtimePayload);
    }
  }
}


/* Address: 0x00535330.
   Stamps a model's influence into the scratch grid: eight concentric rings (radiusMetric plus
   g_GridInfluenceRadiusOffset0..7 plus the margin, squared into g_GridInfluenceSquaredThreshold0..7) around the
   world point set low distance band bit n (scratch bit 8 + n) in every cell whose centre lies within ring n.
   The disc is covered like GridFootprint_ClearTraversalFlagsAroundWorldPoint: vertical walks from the centre row
   leftwards and rightwards, then upwards from the row above and downwards from the row below. Nothing happens
   off the grid or on a blocked centre cell.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t wideProduct;
  uint32_t rightCellWorldX;
  uint32_t rowTermOrLeftCellWorldX;
  int rowOrWalkerValue;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  int walkResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;
  int columnOrWalkerValue;
  
  columnOrWalkerValue = radiusMetric + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue);
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
      while ((columnOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , rightCellWorldX = centerCellWorldX, oppositeWalkCursor = centerCellCursor, columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , columnOrWalkerValue != 0))) {
        walkCursor--;
        rowTermOrLeftCellWorldX = rowTermOrLeftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* centre row rightwards (the centre column again) */
      while ((columnOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0))) {
        oppositeWalkCursor++;
        rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row above: upward walks, leftwards then rightwards */
      rowOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      oppositeWalkCursor = walkCursor;
      while (walkResult = GridInfluence_SetLowDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                rowOrWalkerValue,&oppositeWalkCursor->stateMask), walkResult != 0) {
        oppositeWalkCursor--;
        rowOrWalkerValue = rowOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        walkCursor++;
        rowOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                           columnOrWalkerValue,&walkCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row below: downward walks, rightwards then leftwards */
      rowOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor;
      while (walkResult = GridInfluence_SetLowDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                rowOrWalkerValue,&walkCursor->stateMask), walkResult != 0) {
        walkCursor++;
        rowOrWalkerValue = rowOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        centerCellCursor--;
        rowOrWalkerValue = GridInfluence_SetLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                           columnOrWalkerValue,&centerCellCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00535780.
   High-band twin of GridInfluence_SetLowDistanceBandsAroundWorldPoint: builds the same eight ring thresholds and
   covers the disc the same way, but sets high distance band bit n (scratch bit 16 + n). Called by
   GridInfluence_AddHighDistanceBands.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t wideProduct;
  uint32_t rightCellWorldX;
  uint32_t rowTermOrLeftCellWorldX;
  int rowOrWalkerValue;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  int walkResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;
  int columnOrWalkerValue;
  
  columnOrWalkerValue = radiusMetric + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue);
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
      while ((columnOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , rightCellWorldX = centerCellWorldX, oppositeWalkCursor = centerCellCursor, columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , columnOrWalkerValue != 0))) {
        walkCursor--;
        rowTermOrLeftCellWorldX = rowTermOrLeftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* centre row rightwards (the centre column again) */
      while ((columnOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0))) {
        oppositeWalkCursor++;
        rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row above: upward walks, leftwards then rightwards */
      rowOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      oppositeWalkCursor = walkCursor;
      while (walkResult = GridInfluence_SetHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                rowOrWalkerValue,&oppositeWalkCursor->stateMask), walkResult != 0) {
        oppositeWalkCursor--;
        rowOrWalkerValue = rowOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        walkCursor++;
        rowOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                           columnOrWalkerValue,&walkCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row below: downward walks, rightwards then leftwards */
      rowOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor;
      while (walkResult = GridInfluence_SetHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                rowOrWalkerValue,&walkCursor->stateMask), walkResult != 0) {
        walkCursor++;
        rowOrWalkerValue = rowOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        centerCellCursor--;
        rowOrWalkerValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                           columnOrWalkerValue,&centerCellCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00535CC0.
   Undoes GridInfluence_SetLowDistanceBandsAroundWorldPoint: walks the same eight rings around the world point and
   clears low distance band bit n (scratch bit 8 + n) in every cell inside ring n. Called by
   GridInfluence_RemoveLowDistanceBands.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t wideProduct;
  uint32_t rightCellWorldX;
  uint32_t rowTermOrLeftCellWorldX;
  int rowOrWalkerValue;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  int walkResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;
  int columnOrWalkerValue;
  
  columnOrWalkerValue = radiusMetric + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue);
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
      while ((columnOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , rightCellWorldX = centerCellWorldX, oppositeWalkCursor = centerCellCursor, columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , columnOrWalkerValue != 0))) {
        walkCursor--;
        rowTermOrLeftCellWorldX = rowTermOrLeftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* centre row rightwards (the centre column again) */
      while ((columnOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0))) {
        oppositeWalkCursor++;
        rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row above: upward walks, leftwards then rightwards */
      rowOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      oppositeWalkCursor = walkCursor;
      while (walkResult = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                rowOrWalkerValue,&oppositeWalkCursor->stateMask), walkResult != 0) {
        oppositeWalkCursor--;
        rowOrWalkerValue = rowOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        walkCursor++;
        rowOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                           columnOrWalkerValue,&walkCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row below: downward walks, rightwards then leftwards */
      rowOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor;
      while (walkResult = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                rowOrWalkerValue,&walkCursor->stateMask), walkResult != 0) {
        walkCursor++;
        rowOrWalkerValue = rowOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        centerCellCursor--;
        rowOrWalkerValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                           columnOrWalkerValue,&centerCellCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00536110.
   Undoes GridInfluence_SetHighDistanceBandsAroundWorldPoint: walks the same eight rings around the world point and
   clears high distance band bit n (scratch bit 16 + n) in every cell inside ring n. Called by
   GridInfluence_RemoveHighDistanceBands.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t wideProduct;
  uint32_t rightCellWorldX;
  uint32_t rowTermOrLeftCellWorldX;
  int rowOrWalkerValue;
  uint32_t centerCellWorldX;
  uint32_t centerCellWorldY;
  int walkResult;
  GridScratchCell *centerCellCursor;
  GridScratchCell *oppositeWalkCursor;
  GridScratchCell *walkCursor;
  int columnOrWalkerValue;
  
  columnOrWalkerValue = radiusMetric + GRID_FOOTPRINT_RADIUS_MARGIN;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset0 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset1 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset2 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset3 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset4 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset5 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset6 + columnOrWalkerValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue) * (g_GridInfluenceRadiusOffset7 + columnOrWalkerValue);
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
      while ((columnOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , rightCellWorldX = centerCellWorldX, oppositeWalkCursor = centerCellCursor, columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rowTermOrLeftCellWorldX,&walkCursor->stateMask)
             , columnOrWalkerValue != 0))) {
        walkCursor--;
        rowTermOrLeftCellWorldX = rowTermOrLeftCellWorldX - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* centre row rightwards (the centre column again) */
      while ((columnOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0 &&
             (columnOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,centerCellWorldY,rightCellWorldX,
                                 &oppositeWalkCursor->stateMask), columnOrWalkerValue != 0))) {
        oppositeWalkCursor++;
        rightCellWorldX = rightCellWorldX + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row above: upward walks, leftwards then rightwards */
      rowOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor + -g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      oppositeWalkCursor = walkCursor;
      while (walkResult = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                                rowOrWalkerValue,&oppositeWalkCursor->stateMask), walkResult != 0) {
        oppositeWalkCursor--;
        rowOrWalkerValue = rowOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        walkCursor++;
        rowOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,centerCellWorldY + GRID_SCRATCH_ROW_ABOVE_WORLD_Y,
                           columnOrWalkerValue,&walkCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      /* row below: downward walks, rightwards then leftwards */
      rowOrWalkerValue = centerCellWorldX + GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      centerCellCursor = centerCellCursor + g_GridScratchWidth;
      columnOrWalkerValue = centerCellWorldX - GRID_SCRATCH_HALF_COLUMN_WORLD_X;
      walkCursor = centerCellCursor;
      while (walkResult = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                                rowOrWalkerValue,&walkCursor->stateMask), walkResult != 0) {
        walkCursor++;
        rowOrWalkerValue = rowOrWalkerValue + GRID_SCRATCH_COLUMN_WORLD_X;
      }
      while( true ) {
        centerCellCursor--;
        rowOrWalkerValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,centerCellWorldY - GRID_SCRATCH_ROW_BELOW_WORLD_Y,
                           columnOrWalkerValue,&centerCellCursor->stateMask);
        if (rowOrWalkerValue == 0) break;
        columnOrWalkerValue = columnOrWalkerValue - GRID_SCRATCH_COLUMN_WORLD_X;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00535190.
   Band walker for GridInfluence_SetLowDistanceBandsAroundWorldPoint: from scratchCell (a cell whose centre is at
   cellWorldY/X) it walks downwards, two scratch rows and one column left per step (straight down in world space),
   and ORs low distance band bit n (scratch bit 8 + n) into each cell whose centre lies within
   g_GridInfluenceSquaredThreshold<n> of the centre point. Stops at the first cell inside no ring or before a
   blocked cell. Returns the number of cells written, one less when the walk ended at a blocked cell (DEC in the
   original).
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    /* bit n set when the cell centre lies inside ring n */
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMaskOrNextStep * GRID_SCRATCH_LOW_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x00535260.
   Mirror of GridInfluence_SetLowDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step), with the same band bits and the same return value.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMaskOrNextStep * GRID_SCRATCH_LOW_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x005355E0.
   High-band twin of GridInfluence_SetLowDistanceBandsDiagonalNegative: walks downwards from scratchCell and ORs
   high distance band bit n (scratch bit 16 + n) into each cell inside ring n. Same stop rule and return value.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMaskOrNextStep * GRID_SCRATCH_HIGH_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x005356B0.
   Mirror of GridInfluence_SetHighDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step).
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell | bandMaskOrNextStep * GRID_SCRATCH_HIGH_BAND0;
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x00535B20.
   Clearing twin of GridInfluence_SetLowDistanceBandsDiagonalNegative: walks downwards from scratchCell and clears
   low distance band bit n (scratch bit 8 + n) in each cell inside ring n. Same stop rule and return value.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMaskOrNextStep * GRID_SCRATCH_LOW_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x00535BF0.
   Mirror of GridInfluence_ClearLowDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one column
   right per step).
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMaskOrNextStep * GRID_SCRATCH_LOW_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x00535F70.
   Clearing twin of GridInfluence_SetHighDistanceBandsDiagonalNegative: walks downwards from scratchCell and clears
   high distance band bit n (scratch bit 16 + n) in each cell inside ring n. Same stop rule and return value.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 - GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMaskOrNextStep * GRID_SCRATCH_HIGH_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * 4 - 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


/* Address: 0x00536040.
   Mirror of GridInfluence_ClearHighDistanceBandsDiagonalNegative walking upwards (two scratch rows up and one
   column right per step).
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerWorldYQ12,FieldGridCellCoordinate centerWorldXQ12,
          FieldGridCellCoordinate cellWorldYQ12,FieldGridCellCoordinate cellWorldXQ12,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int squaredYDistance;
  int cellsWritten;
  uint32_t squaredDistance;
  
  squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistance =
         squaredYDistance + (cellWorldXQ12 - centerWorldXQ12) * (cellWorldXQ12 - centerWorldXQ12);
    bandMaskOrNextStep = (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistance <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    cellWorldYQ12 = cellWorldYQ12 + GRID_SCRATCH_ROW_PAIR_WORLD_Y;
    *scratchCell = *scratchCell & ~(bandMaskOrNextStep * GRID_SCRATCH_HIGH_BAND0);
    squaredYDistance = (cellWorldYQ12 - centerWorldYQ12) * (cellWorldYQ12 - centerWorldYQ12);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & GRID_SCRATCH_BLOCKED) == 0);
  return cellsWritten;
}


