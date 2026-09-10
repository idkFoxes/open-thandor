#include <thandor/world/pathing/influence.h>

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
void GridInfluence_AddLowDistanceBands(GameEntityRuntime *entityRuntime)

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
void GridInfluence_RemoveLowDistanceBands(GameEntityRuntime *entityRuntime)

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
void GridInfluence_AddHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *pvVar1;
  Q12 worldXQ12;
  Q12 worldYQ12;
  void *runtimeInfluenceState;
  ModelRuntimeNode *modelNode;
  void *entityDefinition;
  
  pvVar1 = (entityRuntime->common).ownership.runtimeLink;
  modelNode = (entityRuntime->common).ownership.modelNode;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    worldXQ12 = (modelNode->worldTransform).translation.x;
    worldYQ12 = (modelNode->worldTransform).translation.y;
    *(Q12 *)((int)pvVar1 + 0x68) = worldXQ12;
    *(Q12 *)((int)pvVar1 + 0x6c) = worldYQ12;
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
void GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *pvVar1;
  void *runtimeInfluenceState;
  void *entityDefinition;
  
  pvVar1 = (entityRuntime->common).ownership.runtimeLink;
  entityDefinition = (entityRuntime->common).ownership.definitionOrClassRecord;
  if (*(int *)((int)entityDefinition + 0xdc) != 0) {
    GridInfluence_ClearHighDistanceBandsAroundWorldPoint
              (*(FieldGridRadiusUnits *)((int)entityDefinition + 0xdc),*(Q12 *)((int)pvVar1 + 0x6c),
               *(Q12 *)((int)pvVar1 + 0x68));
  }
  return;
}

/* Address: 0x00527B50.
   Ownership: world/pathing/influence.
   Purpose: Exact one-argument no-op reused in unified runtime object method tables. It returns with ret 0x04 and
   preserves EAX and flags. Grid-influence add callback table slot selected by entity class id.
*/
void GridInfluence_AddNoOp(GameEntityRuntime *entityRuntime)

{
  return;
}

/* Address: 0x00527B60.
   Ownership: world/pathing/influence.
   Purpose: Second exact one-argument no-op reused in unified runtime object method tables. It returns with ret
   0x04 and preserves EAX and flags. Grid-influence remove callback table slot selected by entity class id.
*/
void GridInfluence_RemoveNoOp(GameEntityRuntime *entityRuntime)

{
  return;
}

/* Address: 0x00535A30.
   Ownership: world/pathing/influence.
   Purpose: Handles grid influence clear distance bands and refresh entities.
*/
void GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldRuntimeNode *entityListHead)

{
  int cellsRemaining;
  int nextCellsRemaining;
  dword *scratchRecordCursor;
  bool fullBlockRemaining;
  
  cellsRemaining = g_GridScratchWidth * g_GridScratchHeight;
  scratchRecordCursor = &g_GridScratchPrimary->stateMask;
  do {
    *scratchRecordCursor = *scratchRecordCursor & 0xff0000ff;
    scratchRecordCursor[2] = scratchRecordCursor[2] & 0xff0000ff;
    scratchRecordCursor[4] = scratchRecordCursor[4] & 0xff0000ff;
    scratchRecordCursor[6] = scratchRecordCursor[6] & 0xff0000ff;
    scratchRecordCursor[8] = scratchRecordCursor[8] & 0xff0000ff;
    scratchRecordCursor[10] = scratchRecordCursor[10] & 0xff0000ff;
    scratchRecordCursor[0xc] = scratchRecordCursor[0xc] & 0xff0000ff;
    scratchRecordCursor[0xe] = scratchRecordCursor[0xe] & 0xff0000ff;
    scratchRecordCursor[0x10] = scratchRecordCursor[0x10] & 0xff0000ff;
    scratchRecordCursor[0x12] = scratchRecordCursor[0x12] & 0xff0000ff;
    scratchRecordCursor[0x14] = scratchRecordCursor[0x14] & 0xff0000ff;
    scratchRecordCursor[0x16] = scratchRecordCursor[0x16] & 0xff0000ff;
    scratchRecordCursor[0x18] = scratchRecordCursor[0x18] & 0xff0000ff;
    scratchRecordCursor[0x1a] = scratchRecordCursor[0x1a] & 0xff0000ff;
    scratchRecordCursor[0x1c] = scratchRecordCursor[0x1c] & 0xff0000ff;
    scratchRecordCursor[0x1e] = scratchRecordCursor[0x1e] & 0xff0000ff;
    scratchRecordCursor = scratchRecordCursor + 0x20;
    nextCellsRemaining = cellsRemaining + -0x10;
    fullBlockRemaining = 0xf < cellsRemaining;
    cellsRemaining = nextCellsRemaining;
  } while (nextCellsRemaining != 0 && fullBlockRemaining);
  for (; entityListHead != (WorldRuntimeNode *)0x0;
      entityListHead = (entityListHead->common).nextNode) {
    if (entityListHead[2].common.nextNode == (WorldRuntimeNode *)0x0) {
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [*(int *)((int)(entityListHead->runtimePayload->common).ownership.definitionOrClassRecord +
                 0x4c)])(entityListHead->runtimePayload);
    }
  }
  return;
}

/* Address: 0x00535330.
   Ownership: world/pathing/influence.
   Purpose: Builds eight squared distance thresholds from the shared radius table, maps the world point to the
   staggered grid, and drives both diagonal writers to apply low-channel distance bands across the circular
   footprint. Typed parameters: p3 worldXQ12→Q12, p4 worldYQ12→Q12. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged. Typed parameters: p2 radiusMetric→FieldGridRadiusUnits.
   Local calls: GridInfluence_SetLowDistanceBandsDiagonalPositive,
   GridInfluence_SetLowDistanceBandsDiagonalNegative.
*/
void GridInfluence_SetLowDistanceBandsAroundWorldPoint
               (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  FieldGridCellCoordinate FVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint scanGridMetric0;
  FieldGridCellCoordinate FVar6;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar7;
  GridInfluenceDiagonalScanRegisterResult GVar8;
  
  iVar7 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar7) * (g_GridInfluenceRadiusOffset0 + iVar7);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar7) * (g_GridInfluenceRadiusOffset1 + iVar7);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar7) * (g_GridInfluenceRadiusOffset2 + iVar7);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar7) * (g_GridInfluenceRadiusOffset3 + iVar7);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar7) * (g_GridInfluenceRadiusOffset4 + iVar7);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar7) * (g_GridInfluenceRadiusOffset5 + iVar7);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar7) * (g_GridInfluenceRadiusOffset6 + iVar7);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar7) * (g_GridInfluenceRadiusOffset7 + iVar7);
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar7 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar7) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar7 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar4 + iVar7;
    iVar4 = iVar4 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar4 + (iVar7 * 0x400 + -0x600) * 2) * 0x901;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar4 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar3 = uVar5;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while( true ) {
        GVar8 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,uVar3,&scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        scratchCell3 = scratchCell1;
        if (GVar8.processedCellCount == 0) break;
        GVar8 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEcxGridMetric,
                           &scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        if (GVar8.processedCellCount == 0) break;
        scratchCell2 = scratchCell2 + -1;
        uVar3 = GVar8.preservedEcxGridMetric - 0x240;
      }
      do {
        GVar8 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,FVar2,&scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) {
GridInfluence_SetLowDistanceBands_ScanOppositeHalf:
          FVar6 = scanGridMetric0 + 499;
          FVar2 = uVar5 - 0x120;
          scratchCell2 = scratchCell1 + -g_GridScratchWidth;
          scratchCell3 = scratchCell2;
          while( true ) {
            GVar8 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell3->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell3 = scratchCell3 + -1;
            FVar2 = GVar8.preservedEcxGridMetric + -0x240;
          }
          while( true ) {
            scratchCell2 = scratchCell2 + 1;
            GVar8 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + 0x240;
          }
          FVar6 = FVar6 + -999;
          FVar2 = uVar5 + 0x120;
          scratchCell1 = scratchCell1 + g_GridScratchWidth;
          scratchCell2 = scratchCell1;
          while( true ) {
            GVar8 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell2 = scratchCell2 + 1;
            FVar2 = GVar8.preservedEcxGridMetric + 0x240;
          }
          while( true ) {
            scratchCell1 = scratchCell1 + -1;
            GVar8 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell1->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + -0x240;
          }
          return;
        }
        GVar8 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEaxGridMetric,
                           &scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) goto GridInfluence_SetLowDistanceBands_ScanOppositeHalf;
        scratchCell3 = scratchCell3 + 1;
        FVar2 = GVar8.preservedEaxGridMetric + 0x240;
      } while( true );
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
void GridInfluence_SetHighDistanceBandsAroundWorldPoint
               (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  FieldGridCellCoordinate FVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint scanGridMetric0;
  FieldGridCellCoordinate FVar6;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar7;
  GridInfluenceDiagonalScanRegisterResult GVar8;
  
  iVar7 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar7) * (g_GridInfluenceRadiusOffset0 + iVar7);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar7) * (g_GridInfluenceRadiusOffset1 + iVar7);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar7) * (g_GridInfluenceRadiusOffset2 + iVar7);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar7) * (g_GridInfluenceRadiusOffset3 + iVar7);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar7) * (g_GridInfluenceRadiusOffset4 + iVar7);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar7) * (g_GridInfluenceRadiusOffset5 + iVar7);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar7) * (g_GridInfluenceRadiusOffset6 + iVar7);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar7) * (g_GridInfluenceRadiusOffset7 + iVar7);
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar7 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar7) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar7 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar4 + iVar7;
    iVar4 = iVar4 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar4 + (iVar7 * 0x400 + -0x600) * 2) * 0x901;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar4 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar3 = uVar5;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while( true ) {
        GVar8 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,uVar3,&scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        scratchCell3 = scratchCell1;
        if (GVar8.processedCellCount == 0) break;
        GVar8 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEcxGridMetric,
                           &scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        if (GVar8.processedCellCount == 0) break;
        scratchCell2 = scratchCell2 + -1;
        uVar3 = GVar8.preservedEcxGridMetric - 0x240;
      }
      do {
        GVar8 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,FVar2,&scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) {
GridInfluence_SetHighDistanceBands_ScanOppositeHalf:
          FVar6 = scanGridMetric0 + 499;
          FVar2 = uVar5 - 0x120;
          scratchCell2 = scratchCell1 + -g_GridScratchWidth;
          scratchCell3 = scratchCell2;
          while( true ) {
            GVar8 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell3->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell3 = scratchCell3 + -1;
            FVar2 = GVar8.preservedEcxGridMetric + -0x240;
          }
          while( true ) {
            scratchCell2 = scratchCell2 + 1;
            GVar8 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + 0x240;
          }
          FVar6 = FVar6 + -999;
          FVar2 = uVar5 + 0x120;
          scratchCell1 = scratchCell1 + g_GridScratchWidth;
          scratchCell2 = scratchCell1;
          while( true ) {
            GVar8 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell2 = scratchCell2 + 1;
            FVar2 = GVar8.preservedEcxGridMetric + 0x240;
          }
          while( true ) {
            scratchCell1 = scratchCell1 + -1;
            GVar8 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell1->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + -0x240;
          }
          return;
        }
        GVar8 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEaxGridMetric,
                           &scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) goto GridInfluence_SetHighDistanceBands_ScanOppositeHalf;
        scratchCell3 = scratchCell3 + 1;
        FVar2 = GVar8.preservedEaxGridMetric + 0x240;
      } while( true );
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
void GridInfluence_ClearLowDistanceBandsAroundWorldPoint
               (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  FieldGridCellCoordinate FVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint scanGridMetric0;
  FieldGridCellCoordinate FVar6;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar7;
  GridInfluenceDiagonalScanRegisterResult GVar8;
  
  iVar7 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar7) * (g_GridInfluenceRadiusOffset0 + iVar7);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar7) * (g_GridInfluenceRadiusOffset1 + iVar7);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar7) * (g_GridInfluenceRadiusOffset2 + iVar7);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar7) * (g_GridInfluenceRadiusOffset3 + iVar7);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar7) * (g_GridInfluenceRadiusOffset4 + iVar7);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar7) * (g_GridInfluenceRadiusOffset5 + iVar7);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar7) * (g_GridInfluenceRadiusOffset6 + iVar7);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar7) * (g_GridInfluenceRadiusOffset7 + iVar7);
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar7 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar7) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar7 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar4 + iVar7;
    iVar4 = iVar4 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar4 + (iVar7 * 0x400 + -0x600) * 2) * 0x901;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar4 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar3 = uVar5;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while( true ) {
        GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,uVar3,&scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        scratchCell3 = scratchCell1;
        if (GVar8.processedCellCount == 0) break;
        GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEcxGridMetric,
                           &scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        if (GVar8.processedCellCount == 0) break;
        scratchCell2 = scratchCell2 + -1;
        uVar3 = GVar8.preservedEcxGridMetric - 0x240;
      }
      do {
        GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,FVar2,&scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) {
GridInfluence_ClearLowDistanceBands_ScanOppositeHalf:
          FVar6 = scanGridMetric0 + 499;
          FVar2 = uVar5 - 0x120;
          scratchCell2 = scratchCell1 + -g_GridScratchWidth;
          scratchCell3 = scratchCell2;
          while( true ) {
            GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell3->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell3 = scratchCell3 + -1;
            FVar2 = GVar8.preservedEcxGridMetric + -0x240;
          }
          while( true ) {
            scratchCell2 = scratchCell2 + 1;
            GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + 0x240;
          }
          FVar6 = FVar6 + -999;
          FVar2 = uVar5 + 0x120;
          scratchCell1 = scratchCell1 + g_GridScratchWidth;
          scratchCell2 = scratchCell1;
          while( true ) {
            GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell2 = scratchCell2 + 1;
            FVar2 = GVar8.preservedEcxGridMetric + 0x240;
          }
          while( true ) {
            scratchCell1 = scratchCell1 + -1;
            GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell1->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + -0x240;
          }
          return;
        }
        GVar8 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEaxGridMetric,
                           &scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0)
        goto GridInfluence_ClearLowDistanceBands_ScanOppositeHalf;
        scratchCell3 = scratchCell3 + 1;
        FVar2 = GVar8.preservedEaxGridMetric + 0x240;
      } while( true );
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
void GridInfluence_ClearHighDistanceBandsAroundWorldPoint
               (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  FieldGridCellCoordinate FVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint scanGridMetric0;
  FieldGridCellCoordinate FVar6;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar7;
  GridInfluenceDiagonalScanRegisterResult GVar8;
  
  iVar7 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar7) * (g_GridInfluenceRadiusOffset0 + iVar7);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar7) * (g_GridInfluenceRadiusOffset1 + iVar7);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar7) * (g_GridInfluenceRadiusOffset2 + iVar7);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar7) * (g_GridInfluenceRadiusOffset3 + iVar7);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar7) * (g_GridInfluenceRadiusOffset4 + iVar7);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar7) * (g_GridInfluenceRadiusOffset5 + iVar7);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar7) * (g_GridInfluenceRadiusOffset6 + iVar7);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar7) * (g_GridInfluenceRadiusOffset7 + iVar7);
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar7 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar7) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar7 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar4 + iVar7;
    iVar4 = iVar4 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar4 + (iVar7 * 0x400 + -0x600) * 2) * 0x901;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar4 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar3 = uVar5;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while( true ) {
        GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,uVar3,&scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        scratchCell3 = scratchCell1;
        if (GVar8.processedCellCount == 0) break;
        GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEcxGridMetric,
                           &scratchCell2->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        FVar2 = GVar8.preservedEaxGridMetric;
        if (GVar8.processedCellCount == 0) break;
        scratchCell2 = scratchCell2 + -1;
        uVar3 = GVar8.preservedEcxGridMetric - 0x240;
      }
      do {
        GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0,FVar2,&scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0) {
GridInfluence_ClearHighDistanceBands_ScanOppositeHalf:
          FVar6 = scanGridMetric0 + 499;
          FVar2 = uVar5 - 0x120;
          scratchCell2 = scratchCell1 + -g_GridScratchWidth;
          scratchCell3 = scratchCell2;
          while( true ) {
            GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell3->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell3 = scratchCell3 + -1;
            FVar2 = GVar8.preservedEcxGridMetric + -0x240;
          }
          while( true ) {
            scratchCell2 = scratchCell2 + 1;
            GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + 0x240;
          }
          FVar6 = FVar6 + -999;
          FVar2 = uVar5 + 0x120;
          scratchCell1 = scratchCell1 + g_GridScratchWidth;
          scratchCell2 = scratchCell1;
          while( true ) {
            GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell2->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            FVar2 = GVar8.preservedEaxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            scratchCell2 = scratchCell2 + 1;
            FVar2 = GVar8.preservedEcxGridMetric + 0x240;
          }
          while( true ) {
            scratchCell1 = scratchCell1 + -1;
            GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                              (worldYQ12,worldXQ12,FVar6,FVar2,&scratchCell1->stateMask);
            FVar6 = GVar8.preservedEdxGridMetric;
            if (GVar8.processedCellCount == 0) break;
            FVar2 = GVar8.preservedEaxGridMetric + -0x240;
          }
          return;
        }
        GVar8 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0,GVar8.preservedEaxGridMetric,
                           &scratchCell3->stateMask);
        scanGridMetric0 = GVar8.preservedEdxGridMetric;
        if (GVar8.processedCellCount == 0)
        goto GridInfluence_ClearHighDistanceBands_ScanOppositeHalf;
        scratchCell3 = scratchCell3 + 1;
        FVar2 = GVar8.preservedEaxGridMetric + 0x240;
      } while( true );
    }
  }
  return;
}

/* Address: 0x00535190.
   Ownership: world/pathing/influence.
   Purpose: Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell | iVar1 * 0x100;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x00535260.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the low-channel distance-band writer in the opposite grid direction, ORing the eight radial
   bands into scratch bits 8 through 15. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell | iVar1 * 0x100;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x005355E0.
   Ownership: world/pathing/influence.
   Purpose: Walks one grid diagonal and ORs the same eight-band radial classification into scratch bits 16 through
   23. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_SetHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell | iVar1 * 0x10000;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x005356B0.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the high-channel distance-band writer in the opposite grid direction, ORing the radial
   classification into scratch bits 16 through 23. Typed parameters: p2
   centerGridMetric0→FieldGridCellCoordinate_V331, p3 centerGridMetric1→FieldGridCellCoordinate_V331, p4
   scanGridMetric0→FieldGridCellCoordinate_V331, p5 scanGridMetric1→FieldGridCellCoordinate_V331. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_SetHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell | iVar1 * 0x10000;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x00535B20.
   Ownership: world/pathing/influence.
   Purpose: Walks one diagonal inside the active footprint and clears the selected low-channel radial bits from
   scratch bits 8 through 15. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_ClearLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (iVar1 * 0x100 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x00535BF0.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the low-channel radial-bit clearer in the opposite executable-defined grid direction. Typed
   parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_ClearLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (iVar1 * 0x100 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x00535F70.
   Ownership: world/pathing/influence.
   Purpose: Walks one diagonal inside the active footprint and clears the selected high-channel radial bits from
   scratch bits 16 through 23. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_ClearHighDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (iVar1 * 0x10000 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}

/* Address: 0x00536040.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the high-channel radial-bit clearer in the opposite executable-defined grid direction. Typed
   parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
GridInfluenceDiagonalScanRegisterResult
GridInfluence_ClearHighDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_ECX;
  undefined4 in_EDX;
  int iVar3;
  GridInfluenceDiagonalScanRegisterResult GVar4;
  uint squaredDistanceMetric;
  
  iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
  iVar1 = 0;
  do {
    iVar3 = iVar1;
    squaredDistanceMetric =
         iVar2 + (scanGridMetric1 - centerGridMetric1) * (scanGridMetric1 - centerGridMetric1);
    iVar1 = (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold0) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold1) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold2) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold3) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold4) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold5) +
            ((uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold6) +
            (uint)(squaredDistanceMetric <= g_GridInfluenceSquaredThreshold7) * 2) * 2) * 2) * 2) *
            2) * 2) * 2;
    if (iVar1 == 0) break;
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (iVar1 * 0x10000 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  GVar4.preservedEcxGridMetric = in_ECX;
  GVar4.preservedEaxGridMetric = in_EAX;
  GVar4.processedCellCount = iVar3;
  GVar4.preservedEdxGridMetric = in_EDX;
  return GVar4;
}
