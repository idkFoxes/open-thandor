/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/influence.c
 * Reverse engineering by idkFoxes 2026
 */

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
  void *pvVar1;
  Q12 worldXQ12;
  Q12 worldYQ12;
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
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_RemoveHighDistanceBands(GameEntityRuntime *entityRuntime)

{
  void *pvVar1;
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
   Ownership: world/pathing/influence.
   Purpose: Handles grid influence clear distance bands and refresh entities.
*/
void __thandor_preserve_eax
GridInfluence_ClearDistanceBandsAndRefreshEntities(WorldOwnerListNode100 *entityListHead)

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
  for (; entityListHead != (WorldOwnerListNode100 *)0x0; entityListHead = entityListHead->nextNode)
  {
    if (entityListHead->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
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
void __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  uint scanGridMetric1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint scanGridMetric0;
  int iVar5;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar6;
  
  iVar6 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar6) * (g_GridInfluenceRadiusOffset0 + iVar6);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar6) * (g_GridInfluenceRadiusOffset1 + iVar6);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar6) * (g_GridInfluenceRadiusOffset2 + iVar6);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar6) * (g_GridInfluenceRadiusOffset3 + iVar6);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar6) * (g_GridInfluenceRadiusOffset4 + iVar6);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar6) * (g_GridInfluenceRadiusOffset5 + iVar6);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar6) * (g_GridInfluenceRadiusOffset6 + iVar6);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar6) * (g_GridInfluenceRadiusOffset7 + iVar6);
  uVar2 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar6 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar2) + 0x800) >> 10;
  if ((((-1 < iVar6) && (iVar3 = (int)(uVar2 * 2 + 0x800) >> 10, -1 < iVar3)) &&
      (iVar6 < (int)g_GridScratchWidth)) && (iVar3 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar3 + iVar6;
    iVar3 = iVar3 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar3 + (iVar6 * 0x400 + -0x600) * 2) * 0x901;
    uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar3 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar2 = uVar4;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((iVar6 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , scanGridMetric1 = uVar4, scratchCell3 = scratchCell1, iVar6 != 0 &&
             (iVar6 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , iVar6 != 0))) {
        scratchCell2 = scratchCell2 + -1;
        uVar2 = uVar2 - 0x240;
      }
      while ((iVar6 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0 &&
             (iVar6 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      iVar3 = uVar4 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      iVar6 = uVar4 + 0x120;
      scratchCell3 = scratchCell2;
      while (iVar5 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar3,
                                &scratchCell3->stateMask), iVar5 != 0) {
        scratchCell3 = scratchCell3 + -1;
        iVar3 = iVar3 + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = GridInfluence_SetLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar6,&scratchCell2->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + 0x240;
      }
      iVar3 = uVar4 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      iVar6 = uVar4 - 0x120;
      scratchCell2 = scratchCell1;
      while (iVar5 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar3,
                                &scratchCell2->stateMask), iVar5 != 0) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = iVar3 + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        iVar3 = GridInfluence_SetLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar6,&scratchCell1->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + -0x240;
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
  longlong lVar1;
  uint scanGridMetric1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint scanGridMetric0;
  int iVar5;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar6;
  
  iVar6 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar6) * (g_GridInfluenceRadiusOffset0 + iVar6);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar6) * (g_GridInfluenceRadiusOffset1 + iVar6);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar6) * (g_GridInfluenceRadiusOffset2 + iVar6);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar6) * (g_GridInfluenceRadiusOffset3 + iVar6);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar6) * (g_GridInfluenceRadiusOffset4 + iVar6);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar6) * (g_GridInfluenceRadiusOffset5 + iVar6);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar6) * (g_GridInfluenceRadiusOffset6 + iVar6);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar6) * (g_GridInfluenceRadiusOffset7 + iVar6);
  uVar2 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar6 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar2) + 0x800) >> 10;
  if ((((-1 < iVar6) && (iVar3 = (int)(uVar2 * 2 + 0x800) >> 10, -1 < iVar3)) &&
      (iVar6 < (int)g_GridScratchWidth)) && (iVar3 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar3 + iVar6;
    iVar3 = iVar3 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar3 + (iVar6 * 0x400 + -0x600) * 2) * 0x901;
    uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar3 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar2 = uVar4;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((iVar6 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , scanGridMetric1 = uVar4, scratchCell3 = scratchCell1, iVar6 != 0 &&
             (iVar6 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , iVar6 != 0))) {
        scratchCell2 = scratchCell2 + -1;
        uVar2 = uVar2 - 0x240;
      }
      while ((iVar6 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0 &&
             (iVar6 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      iVar3 = uVar4 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      iVar6 = uVar4 + 0x120;
      scratchCell3 = scratchCell2;
      while (iVar5 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar3,
                                &scratchCell3->stateMask), iVar5 != 0) {
        scratchCell3 = scratchCell3 + -1;
        iVar3 = iVar3 + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = GridInfluence_SetHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar6,&scratchCell2->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + 0x240;
      }
      iVar3 = uVar4 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      iVar6 = uVar4 - 0x120;
      scratchCell2 = scratchCell1;
      while (iVar5 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar3,
                                &scratchCell2->stateMask), iVar5 != 0) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = iVar3 + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        iVar3 = GridInfluence_SetHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar6,&scratchCell1->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + -0x240;
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
  longlong lVar1;
  uint scanGridMetric1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint scanGridMetric0;
  int iVar5;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar6;
  
  iVar6 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar6) * (g_GridInfluenceRadiusOffset0 + iVar6);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar6) * (g_GridInfluenceRadiusOffset1 + iVar6);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar6) * (g_GridInfluenceRadiusOffset2 + iVar6);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar6) * (g_GridInfluenceRadiusOffset3 + iVar6);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar6) * (g_GridInfluenceRadiusOffset4 + iVar6);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar6) * (g_GridInfluenceRadiusOffset5 + iVar6);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar6) * (g_GridInfluenceRadiusOffset6 + iVar6);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar6) * (g_GridInfluenceRadiusOffset7 + iVar6);
  uVar2 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar6 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar2) + 0x800) >> 10;
  if ((((-1 < iVar6) && (iVar3 = (int)(uVar2 * 2 + 0x800) >> 10, -1 < iVar3)) &&
      (iVar6 < (int)g_GridScratchWidth)) && (iVar3 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar3 + iVar6;
    iVar3 = iVar3 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar3 + (iVar6 * 0x400 + -0x600) * 2) * 0x901;
    uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar3 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar2 = uVar4;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((iVar6 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , scanGridMetric1 = uVar4, scratchCell3 = scratchCell1, iVar6 != 0 &&
             (iVar6 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , iVar6 != 0))) {
        scratchCell2 = scratchCell2 + -1;
        uVar2 = uVar2 - 0x240;
      }
      while ((iVar6 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0 &&
             (iVar6 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      iVar3 = uVar4 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      iVar6 = uVar4 + 0x120;
      scratchCell3 = scratchCell2;
      while (iVar5 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar3,
                                &scratchCell3->stateMask), iVar5 != 0) {
        scratchCell3 = scratchCell3 + -1;
        iVar3 = iVar3 + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = GridInfluence_ClearLowDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar6,&scratchCell2->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + 0x240;
      }
      iVar3 = uVar4 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      iVar6 = uVar4 - 0x120;
      scratchCell2 = scratchCell1;
      while (iVar5 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar3,
                                &scratchCell2->stateMask), iVar5 != 0) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = iVar3 + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        iVar3 = GridInfluence_ClearLowDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar6,&scratchCell1->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + -0x240;
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
  longlong lVar1;
  uint scanGridMetric1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint scanGridMetric0;
  int iVar5;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell3;
  GridScratchCell *scratchCell2;
  int iVar6;
  
  iVar6 = radiusMetric + 499;
  g_GridInfluenceSquaredThreshold0 =
       (g_GridInfluenceRadiusOffset0 + iVar6) * (g_GridInfluenceRadiusOffset0 + iVar6);
  g_GridInfluenceSquaredThreshold1 =
       (g_GridInfluenceRadiusOffset1 + iVar6) * (g_GridInfluenceRadiusOffset1 + iVar6);
  g_GridInfluenceSquaredThreshold2 =
       (g_GridInfluenceRadiusOffset2 + iVar6) * (g_GridInfluenceRadiusOffset2 + iVar6);
  g_GridInfluenceSquaredThreshold3 =
       (g_GridInfluenceRadiusOffset3 + iVar6) * (g_GridInfluenceRadiusOffset3 + iVar6);
  g_GridInfluenceSquaredThreshold4 =
       (g_GridInfluenceRadiusOffset4 + iVar6) * (g_GridInfluenceRadiusOffset4 + iVar6);
  g_GridInfluenceSquaredThreshold5 =
       (g_GridInfluenceRadiusOffset5 + iVar6) * (g_GridInfluenceRadiusOffset5 + iVar6);
  g_GridInfluenceSquaredThreshold6 =
       (g_GridInfluenceRadiusOffset6 + iVar6) * (g_GridInfluenceRadiusOffset6 + iVar6);
  g_GridInfluenceSquaredThreshold7 =
       (g_GridInfluenceRadiusOffset7 + iVar6) * (g_GridInfluenceRadiusOffset7 + iVar6);
  uVar2 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar6 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar2) + 0x800) >> 10;
  if ((((-1 < iVar6) && (iVar3 = (int)(uVar2 * 2 + 0x800) >> 10, -1 < iVar3)) &&
      (iVar6 < (int)g_GridScratchWidth)) && (iVar3 < (int)g_GridScratchHeight)) {
    scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * iVar3 + iVar6;
    iVar3 = iVar3 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar3 + (iVar6 * 0x400 + -0x600) * 2) * 0x901;
    uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar3 * -1999;
    scanGridMetric0 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar2 = uVar4;
    scratchCell2 = scratchCell1;
    if ((scratchCell1->stateMask & 0x80000000) == 0) {
      while ((iVar6 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , scanGridMetric1 = uVar4, scratchCell3 = scratchCell1, iVar6 != 0 &&
             (iVar6 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,uVar2,&scratchCell2->stateMask)
             , iVar6 != 0))) {
        scratchCell2 = scratchCell2 + -1;
        uVar2 = uVar2 - 0x240;
      }
      while ((iVar6 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0 &&
             (iVar6 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                                (worldYQ12,worldXQ12,scanGridMetric0,scanGridMetric1,
                                 &scratchCell3->stateMask), iVar6 != 0))) {
        scratchCell3 = scratchCell3 + 1;
        scanGridMetric1 = scanGridMetric1 + 0x240;
      }
      iVar3 = uVar4 - 0x120;
      scratchCell2 = scratchCell1 + -g_GridScratchWidth;
      iVar6 = uVar4 + 0x120;
      scratchCell3 = scratchCell2;
      while (iVar5 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                               (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar3,
                                &scratchCell3->stateMask), iVar5 != 0) {
        scratchCell3 = scratchCell3 + -1;
        iVar3 = iVar3 + -0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = GridInfluence_ClearHighDistanceBandsDiagonalPositive
                          (worldYQ12,worldXQ12,scanGridMetric0 + 499,iVar6,&scratchCell2->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + 0x240;
      }
      iVar3 = uVar4 + 0x120;
      scratchCell1 = scratchCell1 + g_GridScratchWidth;
      iVar6 = uVar4 - 0x120;
      scratchCell2 = scratchCell1;
      while (iVar5 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                               (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar3,
                                &scratchCell2->stateMask), iVar5 != 0) {
        scratchCell2 = scratchCell2 + 1;
        iVar3 = iVar3 + 0x240;
      }
      while( true ) {
        scratchCell1 = scratchCell1 + -1;
        iVar3 = GridInfluence_ClearHighDistanceBandsDiagonalNegative
                          (worldYQ12,worldXQ12,scanGridMetric0 - 500,iVar6,&scratchCell1->stateMask)
        ;
        if (iVar3 == 0) break;
        iVar6 = iVar6 + -0x240;
      }
      return;
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
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalNegative
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell | iVar1 * 0x100;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
}


/* Address: 0x00535260.
   Ownership: world/pathing/influence.
   Purpose: Mirrors the low-channel distance-band writer in the opposite grid direction, ORing the eight radial
   bands into scratch bits 8 through 15. Typed parameters: p2 centerGridMetric0→FieldGridCellCoordinate_V331, p3
   centerGridMetric1→FieldGridCellCoordinate_V331, p4 scanGridMetric0→FieldGridCellCoordinate_V331, p5
   scanGridMetric1→FieldGridCellCoordinate_V331. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
*/
int __thandor_void_preserve_eax_ecx_edx
GridInfluence_SetLowDistanceBandsDiagonalPositive
          (FieldGridCellCoordinate centerGridMetric0,FieldGridCellCoordinate centerGridMetric1,
          FieldGridCellCoordinate scanGridMetric0,FieldGridCellCoordinate scanGridMetric1,
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell | iVar1 * 0x100;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell | iVar1 * 0x10000;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell | iVar1 * 0x10000;
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (iVar1 * 0x100 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (iVar1 * 0x100 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + -999;
    *scratchCell = *scratchCell & (iVar1 * 0x10000 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
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
          uint *scratchCell)

{
  int iVar1;
  int iVar2;
  int iVar3;
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
    if (iVar1 == 0) {
      return iVar3;
    }
    scanGridMetric0 = scanGridMetric0 + 999;
    *scratchCell = *scratchCell & (iVar1 * 0x10000 ^ 0xffffffffU);
    iVar2 = (scanGridMetric0 - centerGridMetric0) * (scanGridMetric0 - centerGridMetric0);
    scratchCell = scratchCell + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar3 + 1;
  } while ((*scratchCell & 0x80000000) == 0);
  return iVar3;
}

