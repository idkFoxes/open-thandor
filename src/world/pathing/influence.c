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
   Ownership: world/pathing/influence.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FFF8[4]@0051FFF8;
   g_CodePointerTable_0051FFF8[10]@0051FFF8; g_CodePointerTable_0051FFF8[11]@0051FFF8;
   g_CodePointerTable_0051FFF8[12]@0051FFF8; g_CodePointerTable_0051FFF8[13]@0051FFF8;
   g_CodePointerTable_0051FFF8[14]@0051FFF8; g_CodePointerTable_0051FFF8[15]@0051FFF8;
   g_CodePointerTable_0051FFF8[16]@0051FFF8; g_CodePointerTable_0051FFF8[20]@0051FFF8;
   g_CodePointerTable_0051FFF8[22]@0051FFF8. Grid-influence add callback table slot selected by entity class id.
   Local calls: GridInfluence_SetLowDistanceBandsAroundWorldPoint.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    GridInfluence_SetLowDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + 0xdc),
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
  return;
}


/* Address: 0x00527380.
   Ownership: world/pathing/influence.
   Purpose: Binary entry is anchored by g_CodePointerTable_00520058[4]@00520058;
   g_CodePointerTable_00520058[10]@00520058; g_CodePointerTable_00520058[11]@00520058;
   g_CodePointerTable_00520058[12]@00520058; g_CodePointerTable_00520058[13]@00520058;
   g_CodePointerTable_00520058[14]@00520058; g_CodePointerTable_00520058[15]@00520058;
   g_CodePointerTable_00520058[16]@00520058; g_CodePointerTable_00520058[20]@00520058;
   g_CodePointerTable_00520058[22]@00520058. Grid-influence remove callback table slot selected by entity class id.
   Local calls: GridInfluence_ClearLowDistanceBandsAroundWorldPoint.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime)

{
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    GridInfluence_ClearLowDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + 0xdc),
               (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x);
  }
  return;
}


/* Address: 0x00528070.
   Ownership: world/pathing/influence.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FFF8[0]@0051FFF8;
   g_CodePointerTable_0051FFF8[1]@0051FFF8; g_CodePointerTable_0051FFF8[2]@0051FFF8;
   g_CodePointerTable_0051FFF8[3]@0051FFF8; g_CodePointerTable_0051FFF8[17]@0051FFF8;
   g_CodePointerTable_0051FFF8[18]@0051FFF8; g_CodePointerTable_0051FFF8[19]@0051FFF8;
   g_CodePointerTable_0051FFF8[23]@0051FFF8. Grid-influence add callback table slot selected by entity class id.
   Local calls: GridInfluence_SetHighDistanceBandsAroundWorldPoint.
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
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    worldXQ12 = (modelNode->worldTransform).translation.x;
    worldYQ12 = (modelNode->worldTransform).translation.y;
    *(Q12 *)((int)linkedRuntime + 0x68) = worldXQ12;
    *(Q12 *)((int)linkedRuntime + 0x6c) = worldYQ12;
    GridInfluence_SetHighDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + 0xdc),worldYQ12,worldXQ12);
  }
  return;
}


/* Address: 0x005280D0.
   Ownership: world/pathing/influence.
   Purpose: Binary entry is anchored by g_CodePointerTable_00520058[0]@00520058;
   g_CodePointerTable_00520058[1]@00520058; g_CodePointerTable_00520058[2]@00520058;
   g_CodePointerTable_00520058[3]@00520058; g_CodePointerTable_00520058[17]@00520058;
   g_CodePointerTable_00520058[18]@00520058; g_CodePointerTable_00520058[19]@00520058;
   g_CodePointerTable_00520058[23]@00520058. Grid-influence remove callback table slot selected by entity class id.
   Local calls: GridInfluence_ClearHighDistanceBandsAroundWorldPoint.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *linkedRuntime;
  void *entityDefinition;
  
  linkedRuntime = (entityRuntime->common).ownership.runtimeLink;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    GridInfluence_ClearHighDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + 0xdc),*(Q12 *)((int)linkedRuntime + 0x6c),
               *(Q12 *)((int)linkedRuntime + 0x68));
  }
  return;
}


/* Address: 0x00527B50.
   Ownership: world/pathing/influence.
   Purpose: Exact one-argument no-op reused in unified runtime object method tables. It returns with ret 0x04 and
   preserves EAX and flags. Grid-influence add callback table slot selected by entity class id.
*/
void __thandor_void_preserve_eax_ecx_edx GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime)

{
  return;
}


/* Address: 0x00527B60.
   Ownership: world/pathing/influence.
   Purpose: Second exact one-argument no-op reused in unified runtime object method tables. It returns with ret
   0x04 and preserves EAX and flags. Grid-influence remove callback table slot selected by entity class id.
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
   Ownership: world/pathing/influence.
   Purpose: Builds the shared squared thresholds, maps the world point to the staggered grid, and applies the high-
   channel distance bands through the paired diagonal walkers. Typed parameters: p3 worldXQ12→Q12, p4
   worldYQ12→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter
   storage, body bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p2
   radiusMetric→FieldGridRadiusUnits.
   Local calls: GridInfluence_SetHighDistanceBandsDiagonalPositive,
   GridInfluence_SetHighDistanceBandsDiagonalNegative.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t fixedPointProduct;
  uint32_t scanGridMetric1;
  uint32_t scaledYOrLeftScanMetric;
  int cellRowOrScanValue;
  uint32_t centerScanMetric1;
  uint32_t scanGridMetric0;
  int walkResult;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int radiusColumnOrScanValue;
  
  radiusColumnOrScanValue = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue);
  scaledYOrLeftScanMetric = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  radiusColumnOrScanValue = (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - scaledYOrLeftScanMetric) + 0x800) >> 10;
  if ((((-1 < radiusColumnOrScanValue) && (cellRowOrScanValue = (int)(scaledYOrLeftScanMetric * 2 + 0x800) >> 10, -1 < cellRowOrScanValue)) &&
      (radiusColumnOrScanValue < (int)g_GridScratchWidth)) && (cellRowOrScanValue < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * cellRowOrScanValue + radiusColumnOrScanValue;
    cellRowOrScanValue = cellRowOrScanValue * 0x400 + -0x600;
    fixedPointProduct = (int64_t)(cellRowOrScanValue + (radiusColumnOrScanValue * 0x400 + -0x600) * 2) * 0x901;
    centerScanMetric1 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x13 | (uint32_t)fixedPointProduct >> 0xd;
    fixedPointProduct = (int64_t)cellRowOrScanValue * -1999;
    scanGridMetric0 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x14 | (uint32_t)fixedPointProduct >> 0xc;
    scaledYOrLeftScanMetric = centerScanMetric1;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((radiusColumnOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , scanGridMetric1 = centerScanMetric1, scratchCell3 = scratchCell1, radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , radiusColumnOrScanValue != 0))) {
        scratchCell2 = scratchCell2 + -1;
        scaledYOrLeftScanMetric = scaledYOrLeftScanMetric - 0x240;
      }
      while ((radiusColumnOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 + 0x120;
      scratchCell3 = scratchCell2;
      while (walkResult = GridInfluence_SetHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,cellRowOrScanValue,
                                &scratchCell3->stateMask), walkResult != 0) {
        scratchCell3 = scratchCell3 + -1;
        cellRowOrScanValue = cellRowOrScanValue + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,radiusColumnOrScanValue,&scratchCell2->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1;
      while (walkResult = GridInfluence_SetHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,cellRowOrScanValue,
                                &scratchCell2->stateMask), walkResult != 0) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = cellRowOrScanValue + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        cellRowOrScanValue = GridInfluence_SetHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,radiusColumnOrScanValue,&scratchCell1->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + -0x240;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00535CC0.
   Ownership: world/pathing/influence.
   Purpose: Builds the shared squared thresholds and clears low-channel distance bands across the circular
   footprint through the paired diagonal walkers. Typed parameters: p3 worldXQ12→Q12, p4 worldYQ12→Q12. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged. Typed parameters: p2
   radiusMetric→FieldGridRadiusUnits.
   Local calls: GridInfluence_ClearLowDistanceBandsDiagonalPositive,
   GridInfluence_ClearLowDistanceBandsDiagonalNegative.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t fixedPointProduct;
  uint32_t scanGridMetric1;
  uint32_t scaledYOrLeftScanMetric;
  int cellRowOrScanValue;
  uint32_t centerScanMetric1;
  uint32_t scanGridMetric0;
  int walkResult;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int radiusColumnOrScanValue;
  
  radiusColumnOrScanValue = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue);
  scaledYOrLeftScanMetric = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  radiusColumnOrScanValue = (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - scaledYOrLeftScanMetric) + 0x800) >> 10;
  if ((((-1 < radiusColumnOrScanValue) && (cellRowOrScanValue = (int)(scaledYOrLeftScanMetric * 2 + 0x800) >> 10, -1 < cellRowOrScanValue)) &&
      (radiusColumnOrScanValue < (int)g_GridScratchWidth)) && (cellRowOrScanValue < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * cellRowOrScanValue + radiusColumnOrScanValue;
    cellRowOrScanValue = cellRowOrScanValue * 0x400 + -0x600;
    fixedPointProduct = (int64_t)(cellRowOrScanValue + (radiusColumnOrScanValue * 0x400 + -0x600) * 2) * 0x901;
    centerScanMetric1 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x13 | (uint32_t)fixedPointProduct >> 0xd;
    fixedPointProduct = (int64_t)cellRowOrScanValue * -1999;
    scanGridMetric0 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x14 | (uint32_t)fixedPointProduct >> 0xc;
    scaledYOrLeftScanMetric = centerScanMetric1;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((radiusColumnOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , scanGridMetric1 = centerScanMetric1, scratchCell3 = scratchCell1, radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , radiusColumnOrScanValue != 0))) {
        scratchCell2 = scratchCell2 + -1;
        scaledYOrLeftScanMetric = scaledYOrLeftScanMetric - 0x240;
      }
      while ((radiusColumnOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 + 0x120;
      scratchCell3 = scratchCell2;
      while (walkResult = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,cellRowOrScanValue,
                                &scratchCell3->stateMask), walkResult != 0) {
        scratchCell3 = scratchCell3 + -1;
        cellRowOrScanValue = cellRowOrScanValue + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,radiusColumnOrScanValue,&scratchCell2->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1;
      while (walkResult = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,cellRowOrScanValue,
                                &scratchCell2->stateMask), walkResult != 0) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = cellRowOrScanValue + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        cellRowOrScanValue = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,radiusColumnOrScanValue,&scratchCell1->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + -0x240;
      }
      return;
    }
  }
  return;
}


/* Address: 0x00536110.
   Ownership: world/pathing/influence.
   Purpose: Builds the shared squared thresholds and clears high-channel distance bands across the circular
   footprint through the paired diagonal walkers. Typed parameters: p3 worldXQ12→Q12, p4 worldYQ12→Q12. Nearby but
   non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes,
   control flow, globals, locals, and executable data remain unchanged. Typed parameters: p2
   radiusMetric→FieldGridRadiusUnits.
   Local calls: GridInfluence_ClearHighDistanceBandsDiagonalPositive,
   GridInfluence_ClearHighDistanceBandsDiagonalNegative.
*/
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  int64_t fixedPointProduct;
  uint32_t scanGridMetric1;
  uint32_t scaledYOrLeftScanMetric;
  int cellRowOrScanValue;
  uint32_t centerScanMetric1;
  uint32_t scanGridMetric0;
  int walkResult;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int radiusColumnOrScanValue;
  
  radiusColumnOrScanValue = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset0 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset1 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset2 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset3 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset4 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset5 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset6 + radiusColumnOrScanValue);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue) * (g_GridInfluenceRadiusOffset7 + radiusColumnOrScanValue);
  scaledYOrLeftScanMetric = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  radiusColumnOrScanValue = (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - scaledYOrLeftScanMetric) + 0x800) >> 10;
  if ((((-1 < radiusColumnOrScanValue) && (cellRowOrScanValue = (int)(scaledYOrLeftScanMetric * 2 + 0x800) >> 10, -1 < cellRowOrScanValue)) &&
      (radiusColumnOrScanValue < (int)g_GridScratchWidth)) && (cellRowOrScanValue < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * cellRowOrScanValue + radiusColumnOrScanValue;
    cellRowOrScanValue = cellRowOrScanValue * 0x400 + -0x600;
    fixedPointProduct = (int64_t)(cellRowOrScanValue + (radiusColumnOrScanValue * 0x400 + -0x600) * 2) * 0x901;
    centerScanMetric1 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x13 | (uint32_t)fixedPointProduct >> 0xd;
    fixedPointProduct = (int64_t)cellRowOrScanValue * -1999;
    scanGridMetric0 = (int)((uint64_t)fixedPointProduct >> 0x20) << 0x14 | (uint32_t)fixedPointProduct >> 0xc;
    scaledYOrLeftScanMetric = centerScanMetric1;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((radiusColumnOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , scanGridMetric1 = centerScanMetric1, scratchCell3 = scratchCell1, radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scaledYOrLeftScanMetric,&scratchCell2->stateMask)
             , radiusColumnOrScanValue != 0))) {
        scratchCell2 = scratchCell2 + -1;
        scaledYOrLeftScanMetric = scaledYOrLeftScanMetric - 0x240;
      }
      while ((radiusColumnOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0 &&
             (radiusColumnOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), radiusColumnOrScanValue != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 + 0x120;
      scratchCell3 = scratchCell2;
      while (walkResult = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,cellRowOrScanValue,
                                &scratchCell3->stateMask), walkResult != 0) {
        scratchCell3 = scratchCell3 + -1;
        cellRowOrScanValue = cellRowOrScanValue + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,radiusColumnOrScanValue,&scratchCell2->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + 0x240;
      }
      cellRowOrScanValue = centerScanMetric1 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      radiusColumnOrScanValue = centerScanMetric1 - 0x120;
      scratchCell2 = scratchCell1;
      while (walkResult = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,cellRowOrScanValue,
                                &scratchCell2->stateMask), walkResult != 0) {
        scratchCell2 = scratchCell2 + 1;
        cellRowOrScanValue = cellRowOrScanValue + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        cellRowOrScanValue = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,radiusColumnOrScanValue,&scratchCell1->stateMask)
        ;
        if (cellRowOrScanValue == 0) break;
        radiusColumnOrScanValue = radiusColumnOrScanValue + -0x240;
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
   Ownership: world/pathing/influence.
   Purpose: Walks one grid diagonal and ORs the same eight-band radial classification into scratch bits 16 through
   23. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell | bandMaskOrNextStep * 0x10000;
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}


/* Address: 0x005356B0.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the high-channel distance-band writer in the opposite grid direction, ORing the radial
   classification into scratch bits 16 through 23. Typed parameters: p2
   centerGridMetric0→FieldGridCellCoordinate_V331, p3 centerGridMetric1→FieldGridCellCoordinate_V331, p4
   scanGridMetric0→FieldGridCellCoordinate_V331, p5 scanGridMetric1→FieldGridCellCoordinate_V331. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell | bandMaskOrNextStep * 0x10000;
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}


/* Address: 0x00535B20.
   Ownership: world/pathing/influence.
   Purpose: Walks one diagonal inside the active footprint and clears the selected low-channel radial bits from
   scratch bits 8 through 15. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (bandMaskOrNextStep * 0x100 ^ 0xffffffffU);
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}


/* Address: 0x00535BF0.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the low-channel radial-bit clearer in the opposite executable-defined grid direction. Typed
   parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (bandMaskOrNextStep * 0x100 ^ 0xffffffffU);
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}


/* Address: 0x00535F70.
   Ownership: world/pathing/influence.
   Purpose: Walks one diagonal inside the active footprint and clears the selected high-channel radial bits from
   scratch bits 16 through 23. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (bandMaskOrNextStep * 0x10000 ^ 0xffffffffU);
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}


/* Address: 0x00536040.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the high-channel radial-bit clearer in the opposite executable-defined grid direction. Typed
   parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint32_t *scratchCell)

{
  int bandMaskOrNextStep;
  int metric0DeltaSquared;
  int cellsWritten;
  uint32_t squaredDistanceMetric;
  
  metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  bandMaskOrNextStep = 0;
  do {
    cellsWritten = bandMaskOrNextStep;
    squaredDistanceMetric =
         metric0DeltaSquared + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    bandMaskOrNextStep = (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint32_t)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (bandMaskOrNextStep == 0) {
      return cellsWritten;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (bandMaskOrNextStep * 0x10000 ^ 0xffffffffU);
    metric0DeltaSquared = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    bandMaskOrNextStep = cellsWritten + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return cellsWritten;
}

