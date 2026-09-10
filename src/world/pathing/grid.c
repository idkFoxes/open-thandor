#include <thandor/world/pathing/grid.h>

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
EntityPathingDestinationRegisterResult
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *pGVar1;
  void *pvVar2;
  longlong lVar3;
  longlong lVar4;
  WorldPositionXYRegisterPairQ12 WVar5;
  byte bVar6;
  int iVar7;
  dword dVar8;
  GridPathUnreachableReferenceColumn32 referenceColumn;
  FieldGridRegionMask callerBlockingMask;
  uint uVar9;
  int iVar10;
  int gridY;
  int iVar11;
  dword dVar12;
  GridPathUnreachableReferenceRow32 referenceRow;
  int extraout_EDX;
  GridScratchCell *scratchCell1;
  bool bVar13;
  WorldPositionXYRegisterPairQ12 WVar14;
  EntityPathingDestinationRegisterResult EVar15;
  GridPathBacktrackCfRegisterResult GVar16;
  GridPathMarkedRegionCellRegisterResult GVar17;
  GridPathNearestCellCfRegisterResult GVar18;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  WVar5 = CONCAT44(targetWorldYQ12,targetWorldXQ12);
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  lVar3 = (longlong)(modelNode1->worldTransform).translation.x * 0x1c6e9c;
  lVar4 = (longlong)(modelNode1->worldTransform).translation.y * -0x20c8cc;
  uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
  iVar7 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar9) + 0x800) >>
          10;
  iVar10 = (int)(uVar9 * 2 + 0x800) >> 10;
  if (iVar7 < 1) {
    iVar7 = 1;
  }
  if (iVar10 < 1) {
    iVar10 = 1;
  }
  dVar8 = iVar7 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar7 + 2U)) {
    dVar8 = g_GridScratchWidth;
  }
  dVar12 = iVar10 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar10 + 2U)) {
    dVar12 = g_GridScratchHeight;
  }
  iVar7 = dVar8 - 2;
  gridY = dVar12 - 2;
  uVar9 = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  iVar10 = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                  (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - uVar9) + 0x800) >> 10
  ;
  iVar11 = (int)(uVar9 * 2 + 0x800) >> 10;
  if (iVar10 < 1) {
    iVar10 = 1;
  }
  if (iVar11 < 1) {
    iVar11 = 1;
  }
  dVar8 = iVar10 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar10 + 2U)) {
    dVar8 = g_GridScratchWidth;
  }
  dVar12 = iVar11 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar11 + 2U)) {
    dVar12 = g_GridScratchHeight;
  }
  referenceColumn = dVar8 - 2;
  referenceRow = dVar12 - 2;
  entityRuntime1 =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  iVar10 = *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0x4c);
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    iVar10 = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
               [*(int *)((int)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c)])
                       (entityRuntime1);
  }
  pvVar2 = (routeEntityRuntime->common).ownership.runtimeLink;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove[iVar10])
            (routeEntityRuntime);
  g_GridPathEntityClassMask = 1 << ((byte)*(undefined4 *)((int)pvVar2 + 0xc) & 0x1f);
  bVar6 = (byte)*(undefined4 *)(extraout_EDX + 0x260);
  g_GridPathHighCostMask = 0x10000 << (bVar6 & 0x1f);
  bVar13 = false;
  g_GridPathBlockingMask =
       0x100 << (bVar6 & 0x1f) | 0x1000000 << ((byte)*(undefined4 *)(extraout_EDX + 0x264) & 0x1f);
  GVar18 = GridPathCost_FindNearestUnblockedCell(gridY,iVar7);
  dVar8 = g_GridScratchWidth;
  if (bVar13) {
    if ((GVar18.selectedColumn == iVar7) && (GVar18.selectedRow == gridY)) {
      pGVar1 = &(((routeEntityRuntime->common).ownership.modelNode)->worldTransform).translation;
      WVar5._0_4_ = pGVar1->x;
      WVar5._4_4_ = pGVar1->y;
      WVar14._0_4_ = pGVar1->x;
      WVar14._4_4_ = pGVar1->y;
    }
    else {
      iVar7 = GVar18.selectedRow * 0x400 + -0x600;
      lVar3 = (longlong)(iVar7 + (GVar18.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
      lVar4 = (longlong)iVar7 * -1999;
      WVar14 = EntityPathing_RebuildOverlappingGroupRoutes
                         ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                          (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                          routeEntityRuntime,worldRuntime);
    }
  }
  else {
    iVar10 = gridY * g_GridScratchWidth + iVar7;
    iVar11 = g_GridScratchWidth * 8;
    bVar13 = CARRY4((uint)g_GridScratchPrimary,iVar10 * 8);
    scratchCell1 = g_GridScratchPrimary + iVar10;
    GridPathLine_TestHexSegmentClearCf
              (g_GridPathHighCostMask,gridY,iVar7,scratchCell1,
               g_GridScratchPrimary + referenceRow * g_GridScratchWidth + referenceColumn);
    if (bVar13) {
      GridScratch_ResetTraversalFlagsAndCosts();
      GridPathCost_PropagateWeightedHexNeighbors(6,scratchCell1,referenceRow,referenceColumn);
      scratchCell1 = scratchCell1 + -dVar8;
      if ((((0x7ffffffe < scratchCell1[dVar8].pathCost) && (0x7ffffffe < scratchCell1->pathCost)) &&
          (0x7ffffffe < scratchCell1[1].pathCost)) &&
         (((0x7ffffffe < scratchCell1[dVar8 - 1].pathCost &&
           (0x7ffffffe < scratchCell1[dVar8 + 1].pathCost)) &&
          ((0x7ffffffe < scratchCell1[dVar8 * 2 + -1].pathCost &&
           (0x7ffffffe < scratchCell1[dVar8 * 2].pathCost)))))) {
        GVar17 = GridPathRegion_MarkUnreachableFromCell(referenceRow,referenceColumn,gridY,iVar7);
        iVar10 = GVar17.selectedRow * 0x400 + -0x600;
        lVar3 = (longlong)(iVar10 + (GVar17.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
        targetWorldXQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd;
        lVar3 = (longlong)iVar10 * -1999;
        targetWorldYQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc;
        GridScratch_ResetTraversalFlagsAndCosts();
        GridPathCost_PropagateWeightedHexNeighbors
                  (6,scratchCell1,GVar17.selectedRow,GVar17.selectedColumn);
        iVar11 = g_GridScratchWidth * 8;
        scratchCell1 = g_GridScratchPrimary +
                       ((gridY * g_GridScratchWidth + iVar7) - g_GridScratchWidth);
      }
      callerBlockingMask = g_GridPathHighCostMask;
      if ((*(uint *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0x18) & 2) != 0) {
        callerBlockingMask = 0;
      }
      bVar13 = false;
      GVar16 = GridPathCost_BacktrackBestHexRoute
                         (callerBlockingMask,gridY,iVar7,
                          (GridScratchCell *)((int)&scratchCell1->stateMask + iVar11));
      if (!bVar13) {
        if (GVar16.routeStateMask == 0) {
          iVar7 = GVar16.selectedRow * 0x400 + -0x600;
          lVar3 = (longlong)(iVar7 + (GVar16.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          lVar4 = (longlong)iVar7 * -1999;
          WVar14 = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                              (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                              routeEntityRuntime,worldRuntime);
          WVar5 = CONCAT44(targetWorldYQ12,targetWorldXQ12);
        }
        else {
          iVar7 = GVar16.selectedRow * 0x400 + -0x600;
          lVar3 = (longlong)(iVar7 + (GVar16.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          lVar4 = (longlong)iVar7 * -1999;
          WVar14 = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                              (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                              routeEntityRuntime,worldRuntime);
          WVar5 = CONCAT44(targetWorldYQ12,targetWorldXQ12);
        }
        goto EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn;
      }
    }
    WVar14 = EntityPathing_RebuildOverlappingGroupRoutes
                       (targetWorldYQ12,targetWorldXQ12,routeEntityRuntime,worldRuntime);
    WVar5 = WVar14;
  }
EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn:
  entityRuntime1 =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  iVar7 = *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0x4c);
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    iVar7 = (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
              [*(int *)((int)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c)])
                      (entityRuntime1);
  }
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[iVar7])(routeEntityRuntime);
  EVar15.fallbackWorldXQ12 = (int)WVar5;
  EVar15.fallbackWorldYQ12 = (int)(WVar5 >> 0x20);
  EVar15.primaryWorldXQ12 = (int)WVar14;
  EVar15.primaryWorldYQ12 = (int)(WVar14 >> 0x20);
  return EVar15;
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
undefined8
GridReachability_RebuildConnectedRegionAroundWorldPoint
          (FieldGridRadiusUnits radiusMetric,Q12 worldYQ12,Q12 worldXQ12)

{
  dword dVar1;
  undefined4 in_EAX;
  uint uVar2;
  int iVar3;
  undefined4 in_EDX;
  int iVar4;
  GridScratchCell *scratchCell1;
  bool bVar5;
  
  uVar2 = g_GridScratchWidth * g_GridScratchHeight;
  scratchCell1 = g_GridScratchPrimary;
  do {
    scratchCell1->stateMask = scratchCell1->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1->pathCost = 0;
    scratchCell1[1].stateMask = scratchCell1[1].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[1].pathCost = 0;
    scratchCell1[2].stateMask = scratchCell1[2].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[2].pathCost = 0;
    scratchCell1[3].stateMask = scratchCell1[3].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[3].pathCost = 0;
    scratchCell1[4].stateMask = scratchCell1[4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[4].pathCost = 0;
    scratchCell1[5].stateMask = scratchCell1[5].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[5].pathCost = 0;
    scratchCell1[6].stateMask = scratchCell1[6].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[6].pathCost = 0;
    scratchCell1[7].stateMask = scratchCell1[7].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[7].pathCost = 0;
    scratchCell1[8].stateMask = scratchCell1[8].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[8].pathCost = 0;
    scratchCell1[9].stateMask = scratchCell1[9].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[9].pathCost = 0;
    scratchCell1[10].stateMask = scratchCell1[10].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[10].pathCost = 0;
    scratchCell1[0xb].stateMask = scratchCell1[0xb].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xb].pathCost = 0;
    scratchCell1[0xc].stateMask = scratchCell1[0xc].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xc].pathCost = 0;
    scratchCell1[0xd].stateMask = scratchCell1[0xd].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xd].pathCost = 0;
    scratchCell1[0xe].stateMask = scratchCell1[0xe].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xe].pathCost = 0;
    scratchCell1[0xf].stateMask = scratchCell1[0xf].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xf].pathCost = 0;
    scratchCell1 = scratchCell1 + 0x10;
    bVar5 = 0xf < uVar2;
    uVar2 = uVar2 - 0x10;
  } while (bVar5 && uVar2 != 0);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric * 3,worldYQ12,worldXQ12);
  dVar1 = g_GridScratchWidth;
  uVar2 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar3 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar2) + 0x800) >> 10;
  if ((((iVar3 < 0) || (iVar4 = (int)(uVar2 * 2 + 0x800) >> 10, iVar4 < 0)) ||
      ((int)g_GridScratchWidth <= iVar3)) || ((int)g_GridScratchHeight <= iVar4)) {
GridReachability_RebuildConnectedRegion_ReturnAfterOutOfBoundsOrOpenBoundary:
    return CONCAT44(in_EDX,in_EAX);
  }
  GridReachability_MarkOpenRegionRecursive();
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  scratchCell1 = g_GridScratchPrimary + dVar1 * 3;
  iVar3 = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  do {
    if (((((scratchCell1[dVar1].stateMask & 0x80000000) == 0) && (scratchCell1[dVar1].pathCost != 0)
         ) && ((scratchCell1[dVar1].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0)) &&
       (((scratchCell1->pathCost == 0 || (scratchCell1[1].pathCost == 0)) ||
        ((scratchCell1[dVar1 - 1].pathCost == 0 ||
         (((scratchCell1[dVar1 + 1].pathCost == 0 || (scratchCell1[dVar1 * 2 + -1].pathCost == 0))
          || (scratchCell1[dVar1 * 2].pathCost == 0)))))))) {
      GridReachability_ClearCostedRegionRecursive();
      scratchCell1 = g_GridScratchPrimary + dVar1 * 3;
      iVar3 = (g_GridScratchHeight - 8) * g_GridScratchWidth;
      goto GridReachability_RebuildConnectedRegion_ContinueBoundaryScan;
    }
    scratchCell1 = scratchCell1 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
GridReachability_RebuildConnectedRegion_ReturnAfterBoundaryScan:
  return CONCAT44(in_EDX,in_EAX);
  while( true ) {
    scratchCell1 = scratchCell1 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) break;
GridReachability_RebuildConnectedRegion_ContinueBoundaryScan:
    if ((((scratchCell1[dVar1].stateMask & 0x80000000) == 0) && (scratchCell1[dVar1].pathCost != 0))
       && (((scratchCell1[dVar1].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 &&
           (((scratchCell1->pathCost == 0 || (scratchCell1[1].pathCost == 0)) ||
            ((scratchCell1[dVar1 - 1].pathCost == 0 ||
             (((scratchCell1[dVar1 + 1].pathCost == 0 ||
               (scratchCell1[dVar1 * 2 + -1].pathCost == 0)) ||
              (scratchCell1[dVar1 * 2].pathCost == 0))))))))))
    goto GridReachability_RebuildConnectedRegion_ReturnAfterOutOfBoundsOrOpenBoundary;
  }
  goto GridReachability_RebuildConnectedRegion_ReturnAfterBoundaryScan;
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
void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime)

{
  FieldGridDimension FVar1;
  FieldCellPackedFlagsAndMaterial FVar2;
  longlong lVar3;
  longlong lVar4;
  dword dVar5;
  dword dVar6;
  int iVar7;
  GridScratchStateMask GVar8;
  FieldGridDimension FVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  FieldGridCell *pFVar13;
  GridScratchCell *scratchCell2;
  GridScratchCell *scratchCell1;
  WorldRuntimeNode *worldNode1;
  FieldGridDimension local_20;
  FieldGridAsset *fieldGrid1;
  
  fieldGrid1 = worldRuntime->fieldGrid;
  FVar1 = fieldGrid1->gridWidth;
  local_20 = fieldGrid1->gridHeight;
  iVar7 = FVar1 * local_20;
  scratchCell1 = g_GridScratchPrimary;
  do {
    scratchCell1->stateMask = scratchCell1->stateMask & 0xffff01;
    scratchCell1[1].stateMask = scratchCell1[1].stateMask & 0xffff01;
    scratchCell1[2].stateMask = scratchCell1[2].stateMask & 0xffff01;
    scratchCell1[3].stateMask = scratchCell1[3].stateMask & 0xffff01;
    scratchCell1[4].stateMask = scratchCell1[4].stateMask & 0xffff01;
    scratchCell1[5].stateMask = scratchCell1[5].stateMask & 0xffff01;
    scratchCell1[6].stateMask = scratchCell1[6].stateMask & 0xffff01;
    scratchCell1[7].stateMask = scratchCell1[7].stateMask & 0xffff01;
    scratchCell1[8].stateMask = scratchCell1[8].stateMask & 0xffff01;
    scratchCell1[9].stateMask = scratchCell1[9].stateMask & 0xffff01;
    scratchCell1[10].stateMask = scratchCell1[10].stateMask & 0xffff01;
    scratchCell1[0xb].stateMask = scratchCell1[0xb].stateMask & 0xffff01;
    scratchCell1[0xc].stateMask = scratchCell1[0xc].stateMask & 0xffff01;
    scratchCell1[0xd].stateMask = scratchCell1[0xd].stateMask & 0xffff01;
    scratchCell1[0xe].stateMask = scratchCell1[0xe].stateMask & 0xffff01;
    scratchCell1[0xf].stateMask = scratchCell1[0xf].stateMask & 0xffff01;
    dVar5 = g_GridScratchWidth;
    scratchCell1 = scratchCell1 + 0x10;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  pFVar13 = fieldGrid1->cells;
  FVar9 = FVar1;
  scratchCell1 = g_GridScratchPrimary;
  do {
    do {
      GVar8 = ((uint)((pFVar13->occupancyMask & 0xf900) != 0) +
              ((uint)((pFVar13->occupancyMask & 0xf90000) != 0) +
              ((uint)((pFVar13->occupancyMask & 0xf9000000) != 0) +
              ((uint)((pFVar13->occupancyMask & 0xf900000000) != 0) +
              ((uint)((pFVar13->occupancyMask & 0xf90000000000) != 0) +
              ((uint)((pFVar13->occupancyMask & 0xf9000000000000) != 0) +
              (uint)((pFVar13->occupancyMask & 0xf900000000000000) != 0) * 2) * 2) * 2) * 2) * 2) *
              2) * 2;
      iVar7 = pFVar13->waterSurfaceDelta;
      iVar12 = (int)pFVar13->triangle0NormalAngles >> 0x10;
      iVar10 = (int)pFVar13->triangle1NormalAngles >> 0x10;
      if (iVar7 <= g_GridTerrainClassBit24MaxWaterSurfaceDelta) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      }
      if (iVar10 <= g_GridTerrainClassBit24MaxTriangle1NormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      }
      if (iVar7 < 0) {
        iVar10 = iVar12;
      }
      if (iVar10 <= g_GridTerrainClassBit25MaxSelectedNormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT25;
      }
      if (iVar10 <= g_GridTerrainClassBit26MaxSelectedNormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT26;
      }
      if (iVar10 <= g_GridTerrainClassBit27MaxSelectedNormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT27;
      }
      if (g_GridTerrainClassBit28MinWaterSurfaceDelta <= iVar7) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
      }
      if (g_GridTerrainClassBit29MinWaterSurfaceDelta <= iVar7) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
      }
      if (g_GridTerrainClassBit30MinWaterSurfaceDelta <= iVar7) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
      }
      if (iVar12 <= g_GridTerrainClassBit28MaxTriangle0NormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT28;
      }
      if (iVar12 <= g_GridTerrainClassBit29MaxTriangle0NormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT29;
      }
      if (iVar12 <= g_GridTerrainClassBit30MaxTriangle0NormalAngleHigh16) {
        GVar8 = GVar8 | GRID_SCRATCH_TERRAIN_CLASS_BIT30;
      }
      FVar2 = pFVar13->flagsAndMaterial;
      if ((FVar2 & 0x88006000) != 0) {
        GVar8 = GVar8 | 0x80000000;
      }
      if ((FVar2 & FIELD_CELL_FIRST_ROW_BOUNDARY) == 0) {
        scratchCell1[dVar5 * -2 + 2].stateMask = scratchCell1[dVar5 * -2 + 2].stateMask | GVar8;
        scratchCell1[dVar5 * -2 + 3].stateMask = scratchCell1[dVar5 * -2 + 3].stateMask | GVar8;
        scratchCell1[1 - dVar5].stateMask = scratchCell1[1 - dVar5].stateMask | GVar8;
        scratchCell1[2 - dVar5].stateMask = scratchCell1[2 - dVar5].stateMask | GVar8;
        scratchCell1[3 - dVar5].stateMask = scratchCell1[3 - dVar5].stateMask | GVar8;
        if ((FVar2 & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
          scratchCell1[dVar5 * -2 + 4].stateMask = scratchCell1[dVar5 * -2 + 4].stateMask | GVar8;
          scratchCell1[4 - dVar5].stateMask = scratchCell1[4 - dVar5].stateMask | GVar8;
          scratchCell1[5 - dVar5].stateMask = scratchCell1[5 - dVar5].stateMask | GVar8;
        }
      }
      if ((FVar2 & FIELD_CELL_LAST_COLUMN_BOUNDARY) == 0) {
        scratchCell1[4].stateMask = scratchCell1[4].stateMask | GVar8;
        scratchCell1[5].stateMask = scratchCell1[5].stateMask | GVar8;
        scratchCell1[dVar5 + 4].stateMask = scratchCell1[dVar5 + 4].stateMask | GVar8;
        scratchCell1[dVar5 + 5].stateMask = scratchCell1[dVar5 + 5].stateMask | GVar8;
        scratchCell1[dVar5 * 2 + 4].stateMask = scratchCell1[dVar5 * 2 + 4].stateMask | GVar8;
      }
      scratchCell1->stateMask = scratchCell1->stateMask | GVar8;
      scratchCell1[1].stateMask = scratchCell1[1].stateMask | GVar8;
      scratchCell1[2].stateMask = scratchCell1[2].stateMask | GVar8;
      scratchCell1[3].stateMask = scratchCell1[3].stateMask | GVar8;
      scratchCell1 = scratchCell1 + dVar5;
      if ((FVar2 & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
        scratchCell1[-1].stateMask = scratchCell1[-1].stateMask | GVar8;
        scratchCell1[dVar5 - 1].stateMask = scratchCell1[dVar5 - 1].stateMask | GVar8;
        scratchCell1[dVar5 - 2].stateMask = scratchCell1[dVar5 - 2].stateMask | GVar8;
        scratchCell1[dVar5 * 2 + -1].stateMask = scratchCell1[dVar5 * 2 + -1].stateMask | GVar8;
        scratchCell1[dVar5 * 2 + -2].stateMask = scratchCell1[dVar5 * 2 + -2].stateMask | GVar8;
      }
      scratchCell1->stateMask = scratchCell1->stateMask | GVar8;
      scratchCell1[1].stateMask = scratchCell1[1].stateMask | GVar8;
      scratchCell1[2].stateMask = scratchCell1[2].stateMask | GVar8;
      scratchCell1[3].stateMask = scratchCell1[3].stateMask | GVar8;
      scratchCell1[dVar5].stateMask = scratchCell1[dVar5].stateMask | GVar8;
      scratchCell1[dVar5 + 1].stateMask = scratchCell1[dVar5 + 1].stateMask | GVar8;
      scratchCell1[dVar5 + 2].stateMask = scratchCell1[dVar5 + 2].stateMask | GVar8;
      scratchCell1[dVar5 + 3].stateMask = scratchCell1[dVar5 + 3].stateMask | GVar8;
      scratchCell1 = scratchCell1 + dVar5 * 2;
      scratchCell1->stateMask = scratchCell1->stateMask | GVar8;
      scratchCell1[1].stateMask = scratchCell1[1].stateMask | GVar8;
      scratchCell1[2].stateMask = scratchCell1[2].stateMask | GVar8;
      scratchCell1[3].stateMask = scratchCell1[3].stateMask | GVar8;
      if ((FVar2 & 0x80000000) == 0) {
        scratchCell1[dVar5].stateMask = scratchCell1[dVar5].stateMask | GVar8;
        scratchCell1[dVar5 + 1].stateMask = scratchCell1[dVar5 + 1].stateMask | GVar8;
        scratchCell1[dVar5 + 2].stateMask = scratchCell1[dVar5 + 2].stateMask | GVar8;
        scratchCell1[dVar5 * 2].stateMask = scratchCell1[dVar5 * 2].stateMask | GVar8;
        scratchCell1[dVar5 * 2 + 1].stateMask = scratchCell1[dVar5 * 2 + 1].stateMask | GVar8;
        if ((FVar2 & FIELD_CELL_FIRST_COLUMN_BOUNDARY) == 0) {
          scratchCell1[dVar5 - 1].stateMask = scratchCell1[dVar5 - 1].stateMask | GVar8;
          scratchCell1[dVar5 - 2].stateMask = scratchCell1[dVar5 - 2].stateMask | GVar8;
          scratchCell1[dVar5 * 2 + -1].stateMask = scratchCell1[dVar5 * 2 + -1].stateMask | GVar8;
        }
      }
      dVar6 = g_GridScratchWidth;
      scratchCell1 = scratchCell1 + dVar5 * -3 + 4;
      pFVar13 = pFVar13 + 1;
      FVar9 = FVar9 - 1;
    } while (FVar9 != 0);
    scratchCell1 = scratchCell1 + dVar5 * 3;
    local_20 = local_20 - 1;
    FVar9 = FVar1;
  } while (local_20 != 0);
  iVar10 = g_GridScratchHeight * g_GridScratchWidth;
  scratchCell1 = g_GridScratchPrimary + g_GridScratchWidth * -4;
  iVar7 = iVar10;
  do {
    scratchCell1[dVar6 * 4].stateMask =
         scratchCell1[dVar6 * 4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    if (((scratchCell1[dVar6 * 4].stateMask & 0x80000000) == 0) &&
       (((scratchCell1[dVar6 * 4].stateMask | scratchCell1->stateMask | scratchCell1[4].stateMask |
          scratchCell1[dVar6 * 4 + -4].stateMask | scratchCell1[dVar6 * 4 + 4].stateMask |
          scratchCell1[dVar6 * 8 + -4].stateMask | scratchCell1[dVar6 * 8].stateMask) &
        GRID_SCRATCH_TERRAIN_CLASS_BIT24) != 0)) {
      scratchCell1[dVar6 * 4].stateMask =
           scratchCell1[dVar6 * 4].stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    scratchCell1 = scratchCell1 + 1;
    iVar7 = iVar7 + -1;
    scratchCell2 = g_GridScratchPrimary;
  } while (iVar7 != 0);
  do {
    if ((scratchCell2->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) {
      scratchCell2->stateMask = scratchCell2->stateMask | GRID_SCRATCH_TERRAIN_CLASS_BIT24;
      scratchCell2->stateMask = scratchCell2->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    iVar10 = iVar10 + -1;
    scratchCell2 = scratchCell2 + 1;
  } while (iVar10 != 0);
  worldNode1 = worldRuntime->ownerListHead;
  if (worldNode1 != (WorldRuntimeNode *)0x0) {
    do {
      if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
          (*(int *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)worldNode1->runtimePayload + 0x278) != 1)) {
        lVar3 = (longlong)(int)worldNode1[1].classPayload.model.meshGroupMask * 0x1c6e9c;
        lVar4 = (longlong)(int)worldNode1[1].runtimePayload * -0x20c8cc;
        uVar11 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
        iVar7 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar11) +
                     0x800) >> 10;
        if (((-1 < iVar7) && (iVar10 = (int)(uVar11 * 2 + 0x800) >> 10, -1 < iVar10)) &&
           ((iVar7 < (int)g_GridScratchWidth && (iVar10 < (int)g_GridScratchHeight)))) {
          GridScratch_FloodFillConnectedCellsRegs();
        }
      }
      worldNode1 = (worldNode1->common).nextNode;
    } while (worldNode1 != (WorldRuntimeNode *)0x0);
    iVar7 = g_GridScratchWidth * g_GridScratchHeight;
    scratchCell1 = g_GridScratchPrimary;
    do {
      if ((scratchCell1->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
           GRID_SCRATCH_TERRAIN_CLASS_BIT28|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCell1->stateMask =
             scratchCell1->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT30|GRID_SCRATCH_TERRAIN_CLASS_BIT29|
             GRID_SCRATCH_TERRAIN_CLASS_BIT28);
      }
      scratchCell1->stateMask = scratchCell1->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
      scratchCell1 = scratchCell1 + 1;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
    worldNode1 = worldRuntime->ownerListHead;
    do {
      if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
          (*(int *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)worldNode1->runtimePayload + 0x278) != 1)) {
        lVar3 = (longlong)(int)worldNode1[1].classPayload.model.meshGroupMask * 0x1c6e9c;
        lVar4 = (longlong)(int)worldNode1[1].runtimePayload * -0x20c8cc;
        uVar11 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
        iVar7 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar11) +
                     0x800) >> 10;
        if ((((-1 < iVar7) && (iVar10 = (int)(uVar11 * 2 + 0x800) >> 10, -1 < iVar10)) &&
            (iVar7 < (int)g_GridScratchWidth)) && (iVar10 < (int)g_GridScratchHeight)) {
          GridScratch_FloodFillConnectedCellsRegs();
        }
      }
      worldNode1 = (worldNode1->common).nextNode;
    } while (worldNode1 != (WorldRuntimeNode *)0x0);
    iVar7 = g_GridScratchWidth * g_GridScratchHeight;
    scratchCell1 = g_GridScratchPrimary;
    do {
      if ((scratchCell1->stateMask &
          (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
           GRID_SCRATCH_TERRAIN_CLASS_BIT25|GRID_SCRATCH_TRAVERSAL_VISITED)) == 0) {
        scratchCell1->stateMask =
             scratchCell1->stateMask |
             (GRID_SCRATCH_TERRAIN_CLASS_BIT27|GRID_SCRATCH_TERRAIN_CLASS_BIT26|
             GRID_SCRATCH_TERRAIN_CLASS_BIT25);
      }
      scratchCell1 = scratchCell1 + 1;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  return;
}

/* Address: 0x00533E70.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test runtime pair reachability from world point carry-flag result.
   Local calls: GridScratch_TestWorldPointReachabilityCf.
*/
void GridScratch_TestRuntimePairReachabilityFromWorldPointCf
               (WorldPointXYQ12 *sourceWorldPoint,undefined4 *targetRuntimePair)

{
  GridScratch_TestWorldPointReachabilityCf
            (0x100 << ((byte)*(undefined4 *)(*(int *)*targetRuntimePair + 0x260) & 0x1f) |
             0x1000000 << ((byte)*(undefined4 *)(*(int *)*targetRuntimePair + 0x264) & 0x1f),
             sourceWorldPoint->worldYQ12,sourceWorldPoint->worldXQ12,
             *(GraphicsWorldCoordinateQ12 *)(targetRuntimePair[1] + 0x98),
             *(GraphicsWorldCoordinateQ12 *)(targetRuntimePair[1] + 0x94));
  return;
}

/* Address: 0x005332C0.
   Ownership: world/pathing/grid.
   Purpose: Scales field-grid width and height by four, allocates and exchanges two equal cell-sized scratch
   buffers plus one fixed 0x180000-byte auxiliary buffer, publishes its end pointer, and reports allocation failure
   through CF. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Allocation arithmetic proves 8 bytes per GridScratch
   record: scratchWidth*scratchHeight*8. Primary and secondary buffers therefore hold GridScratchCell_V419 records.
   The fixed 0x180000-byte auxiliary allocation is the pointer queue now named g_GridPathCostQueueBegin..End.
*/
void GridScratch_AllocateForFieldGridCf(FieldGridAsset *fieldGrid)

{
  GridScratchCell *memory;
  GridScratchCell **memory_00;
  dword *newScratchBuffer;
  dword *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  dword bytes;
  bool allocationSizeOverflow;
  longlong scratchAllocationByteCountProduct;
  GridScratchCell *previousScratchBuffer;
  
  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  scratchAllocationByteCountProduct =
       (longlong)(int)(fieldGrid->gridWidth << 5) * (longlong)(int)g_GridScratchHeight;
  allocationSizeOverflow =
       (int)(dword)scratchAllocationByteCountProduct != scratchAllocationByteCountProduct;
  newScratchBuffer = (*g_MemoryApi.alloc)((dword)scratchAllocationByteCountProduct);
  previousScratchBuffer = g_GridScratchPrimary;
  if (!allocationSizeOverflow) {
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    (*g_MemoryApi.free)(previousScratchBuffer);
    newSecondaryScratchBuffer = (*g_MemoryApi.alloc)(bytes);
    memory = g_GridScratchSecondary;
    if (!allocationSizeOverflow) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      (*g_MemoryApi.free)(memory);
      newAuxiliaryBuffer = (*g_MemoryApi.alloc)(0x180000);
      memory_00 = g_GridPathCostQueueBegin;
      if (!allocationSizeOverflow) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((int)newAuxiliaryBuffer + 0x180000);
        g_GridPathCostQueueBegin = newAuxiliaryBuffer;
        (*g_MemoryApi.free)(memory_00);
        return;
      }
    }
  }
  return;
}

/* Address: 0x00533360.
   Ownership: world/pathing/grid.
   Purpose: Releases the auxiliary, primary, and secondary grid scratch allocations through the engine memory API,
   then clears all three global pointers.
*/
undefined4 __cdecl GridScratch_ReleaseBuffers(void)

{
  undefined4 in_EAX;
  
  (*g_MemoryApi.free)(g_GridPathCostQueueBegin);
  (*g_MemoryApi.free)(g_GridScratchPrimary);
  (*g_MemoryApi.free)(g_GridScratchSecondary);
  g_GridPathCostQueueBegin = (GridScratchCell **)0x0;
  g_GridScratchPrimary = (GridScratchCell *)0x0;
  g_GridScratchSecondary = (GridScratchCell *)0x0;
  return in_EAX;
}

/* Address: 0x00533400.
   Ownership: world/pathing/grid.
   Purpose: Builds a compact mask from FieldGridCell occupancy bytes and OR-propagates it into neighboring entries
   of g_GridScratchPrimary, skipping rejected terrain cells. Tick-wheel case 7 tail: dilates the rebuilt occupancy
   mask into the neighborhood scratch grid.
*/
void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid)

{
  uint *puVar1;
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
      currentFieldCell = currentFieldCell + 1;
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
        puVar1 = propagatedScratchCursor + scratchWidth * 4;
        *puVar1 = *puVar1 | propagatedOccupancyGroupMask;
        puVar1[2] = puVar1[2] | propagatedOccupancyGroupMask;
        puVar1[4] = puVar1[4] | propagatedOccupancyGroupMask;
        puVar1[6] = puVar1[6] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 2] = puVar1[scratchWidth * 2] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 2 + 2] = puVar1[scratchWidth * 2 + 2] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 2 + 4] = puVar1[scratchWidth * 2 + 4] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 4] = puVar1[scratchWidth * 4] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 4 + 2] = puVar1[scratchWidth * 4 + 2] | propagatedOccupancyGroupMask;
        puVar1[scratchWidth * 2 + -2] = puVar1[scratchWidth * 2 + -2] | propagatedOccupancyGroupMask
        ;
        puVar1[scratchWidth * 2 + -4] = puVar1[scratchWidth * 2 + -4] | propagatedOccupancyGroupMask
        ;
        puVar1[scratchWidth * 4 + -2] = puVar1[scratchWidth * 4 + -2] | propagatedOccupancyGroupMask
        ;
        nextScratchCellCursor = puVar1 + scratchWidth * -6 + 8;
      }
      scratchCellCursor = nextScratchCellCursor;
      columnsRemaining = columnsRemaining - 1;
      currentFieldCell = currentFieldCell;
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
void GridScratch_TestProjectedCellMaskBandsCf
               (Q12 worldYQ12,Q12 worldXQ12,byte lowBandIndex,byte highBandIndex)

{
  GridScratchStateMask GVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar2 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar2) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar2 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    GVar1 = g_GridScratchPrimary[iVar4 * g_GridScratchWidth + iVar2].stateMask;
    if (((-1 < (int)GVar1) && ((0x100 << (lowBandIndex & 0x1f) & GVar1) == 0)) &&
       ((0x1000000 << (highBandIndex & 0x1f) & GVar1) == 0)) {
      return;
    }
  }
  return;
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
WorldPositionXYRegisterPairQ12
EntityPathing_RebuildOverlappingGroupRoutes
          (UQ12 targetWorldY,UQ12 targetWorldX,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  undefined *puVar1;
  sdword sVar2;
  sdword sVar3;
  GameEntityRuntime *routeEntityRuntime_00;
  EntityPathingPriorityPair *heapBase;
  undefined *extraout_EAX;
  undefined4 extraout_EAX_00;
  int iVar4;
  DepthIntervalRadius32 intervalRadius;
  int extraout_ECX;
  dword dVar5;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int iVar6;
  int extraout_EDX;
  uint uVar7;
  uint extraout_EDX_00;
  uint extraout_EDX_01;
  undefined4 extraout_EDX_02;
  WorldRuntimeNode *worldNode1;
  int iVar8;
  EntityPathingPriorityPair *priorityPair1;
  EntityPathingPriorityPair *priorityPair2;
  bool bVar9;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar10;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar11;
  WorldPositionXYRegisterPairQ12 WVar12;
  ModelRuntimeNode *modelNode1;
  
  WVar12 = CONCAT44(targetWorldY,targetWorldX);
  GridScratch_CopyPrimaryToSecondary();
  GridScratch_SwapPrimarySecondary();
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  worldNode1 = worldRuntime->ownerListHead;
  iVar6 = (modelNode1->worldTransform).translation.x;
  iVar4 = iVar6 - targetWorldX;
  if (iVar4 < 0) {
    iVar4 = -iVar4;
  }
  iVar8 = (modelNode1->worldTransform).translation.y - targetWorldY;
  if (iVar8 < 0) {
    iVar8 = -iVar8;
  }
  if (iVar4 < iVar8) {
    iVar4 = iVar8;
  }
  DVar10 = DepthInterval_BuildBinMask
                     (iVar4 + *(int *)((int)(routeEntityRuntime->common).ownership.
                                            definitionOrClassRecord + 0xdc),
                      (int)(iVar6 + targetWorldX) >> 1);
  DVar11 = DepthInterval_BuildBinMask(intervalRadius,(DepthIntervalCenter32)(DVar10 >> 0x20));
  g_EntityPathingPriorityPairCount = 0;
  priorityPair2 = g_EntityPathingPriorityPairs;
  do {
    bVar9 = false;
    if (((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
        (DepthBinMasks_OverlapCf
                   (worldNode1[2].classPayload.model.localTranslationXQ12,
                    worldNode1[2].classPayload.model.worldRotationAngle2,(DepthBinMask32)DVar11,
                    (DepthBinMask32)DVar10), bVar9)) && (*(int *)(extraout_EDX + 0x18) != 0)) {
      priorityPair2->entity = extraout_EAX;
      priorityPair2->priority = 0;
      g_EntityPathingPriorityPairCount = g_EntityPathingPriorityPairCount + 1;
      priorityPair2 = priorityPair2 + 1;
      if (extraout_ECX == 1) break;
    }
    worldNode1 = (worldNode1->common).nextNode;
  } while (worldNode1 != (WorldRuntimeNode *)0x0);
  if (1 < g_EntityPathingPriorityPairCount) {
    iVar6 = *(int *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0xc);
    dVar5 = g_EntityPathingPriorityPairCount;
    priorityPair2 = g_EntityPathingPriorityPairs;
    do {
      iVar4 = *(int *)((int)priorityPair2->entity + 8);
      iVar8 = *(int *)priorityPair2->entity;
      if ((((*(uint *)(iVar4 + 0x18) & 2) == 0) &&
          (priorityPair2->priority = priorityPair2->priority + 1, iVar6 == *(int *)(iVar4 + 0xc)))
         && (priorityPair2->priority = priorityPair2->priority + 1,
            (*(uint *)(iVar4 + 0x18) & 1) == 0)) {
        priorityPair2->priority = priorityPair2->priority + *(int *)(iVar8 + 0xc);
      }
      heapBase = g_EntityPathingPriorityPairs;
      priorityPair2 = priorityPair2 + 1;
      dVar5 = dVar5 - 1;
    } while (dVar5 != 0);
    uVar7 = 0;
    priorityPair2 = g_EntityPathingPriorityPairs;
    do {
      PriorityPairHeap_SiftUp(uVar7 + 1,heapBase);
      priorityPair2 = priorityPair2 + 1;
      uVar7 = extraout_EDX_00;
    } while (extraout_ECX_00 != 1);
    do {
      sVar3 = heapBase->priority;
      LOCK();
      puVar1 = priorityPair2[-1].entity;
      priorityPair2[-1].entity = heapBase->entity;
      UNLOCK();
      LOCK();
      sVar2 = priorityPair2[-1].priority;
      priorityPair2[-1].priority = sVar3;
      UNLOCK();
      heapBase->entity = puVar1;
      heapBase->priority = sVar2;
      PriorityPairHeap_SiftDown(uVar7 - 1,heapBase);
      priorityPair2 = priorityPair2 + -1;
      uVar7 = extraout_EDX_01;
      priorityPair1 = g_EntityPathingPriorityPairs;
    } while (1 < extraout_EDX_01);
    do {
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [*(int *)((int)(((GameEntityRuntime *)priorityPair1->entity)->common).ownership.
                       definitionOrClassRecord + 0x4c)])((GameEntityRuntime *)priorityPair1->entity)
      ;
      priorityPair2 = g_EntityPathingPriorityPairs;
      priorityPair1 = priorityPair1 + 1;
    } while (extraout_ECX_01 != 1);
    do {
      targetWorldY = (UQ12)(WVar12 >> 0x20);
      targetWorldX = (UQ12)WVar12;
      routeEntityRuntime_00 = (GameEntityRuntime *)priorityPair2->entity;
      if (priorityPair2->priority == 1) {
        modelNode1 = (routeEntityRuntime_00->common).ownership.modelNode;
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (*(FieldGridRadiusUnits *)
                    ((int)(routeEntityRuntime_00->common).ownership.definitionOrClassRecord + 0xdc),
                   (modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x);
        iVar6 = extraout_ECX_04;
      }
      else if (routeEntityRuntime_00 == routeEntityRuntime) {
        WVar12 = EntityPathing_UpdateRouteSegment
                           (targetWorldY,targetWorldX,routeEntityRuntime,routeEntityRuntime_00);
        iVar6 = extraout_ECX_03;
      }
      else {
        EntityPathing_UpdateRouteSegment(0,0,routeEntityRuntime,routeEntityRuntime_00);
        iVar6 = extraout_ECX_02;
      }
      priorityPair2 = priorityPair2 + 1;
    } while (iVar6 != 1);
  }
  GridScratch_SwapPrimarySecondary();
  return CONCAT44(extraout_EDX_02,extraout_EAX_00);
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
void GridFootprint_ClearTraversalFlagsAroundWorldPoint
               (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12)

{
  longlong lVar1;
  int iVar2;
  FieldGridCellCoordinate extraout_EAX;
  FieldGridCellCoordinate extraout_EAX_00;
  FieldGridCellCoordinate currentY;
  int extraout_EAX_01;
  FieldGridCellCoordinate FVar3;
  FieldGridCellCoordinate currentY_00;
  FieldGridCellCoordinate currentY_01;
  FieldGridCellCoordinate currentY_02;
  int extraout_ECX;
  FieldGridCellCoordinate currentY_03;
  int extraout_ECX_00;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint currentX;
  uint currentX_00;
  uint extraout_EDX;
  uint currentX_01;
  uint extraout_EDX_00;
  FieldGridCellCoordinate currentX_02;
  int extraout_EDX_01;
  FieldGridCellCoordinate currentX_03;
  int unaff_EBX;
  GridScratchCell *scratchCell2;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchRecord;
  
  iVar2 = radiusWorldUnits + g_GridInfluenceRadiusOffset6 + 499;
  g_GridInfluenceSquaredThreshold6 = iVar2 * iVar2;
  uVar4 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar2 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar4) + 0x800) >> 10;
  if ((((-1 < iVar2) && (iVar5 = (int)(uVar4 * 2 + 0x800) >> 10, -1 < iVar5)) &&
      (iVar2 < (int)g_GridScratchWidth)) && (iVar5 < (int)g_GridScratchHeight)) {
    scratchCell2 = g_GridScratchPrimary + g_GridScratchWidth * iVar5 + iVar2;
    iVar5 = iVar5 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar5 + (iVar2 * 0x400 + -0x600) * 2) * 0x901;
    uVar6 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar5 * -1999;
    currentX = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar4 = uVar6;
    scratchCell1 = scratchCell2;
    if ((scratchCell2->stateMask & 0x80000000) == 0) {
      while ((GridFootprint_ClearTraversalFlagsDiagonalPositive
                        (worldYQ12,worldXQ12,currentX,uVar4,&scratchCell1->stateMask),
             FVar3 = extraout_EAX, uVar4 = currentX_00, scratchRecord = scratchCell2, unaff_EBX != 0
             && (GridFootprint_ClearTraversalFlagsDiagonalNegative
                           (worldYQ12,worldXQ12,currentX_00,currentY_02,&scratchCell1->stateMask),
                FVar3 = extraout_EAX_00, uVar4 = extraout_EDX, unaff_EBX != 0))) {
        scratchCell1 = scratchCell1 + -1;
        uVar4 = extraout_ECX - 0x240;
        currentX = extraout_EDX;
      }
      while ((GridFootprint_ClearTraversalFlagsDiagonalPositive
                        (worldYQ12,worldXQ12,uVar4,FVar3,&scratchRecord->stateMask),
             uVar4 = currentX_01, unaff_EBX != 0 &&
             (GridFootprint_ClearTraversalFlagsDiagonalNegative
                        (worldYQ12,worldXQ12,currentX_01,currentY,&scratchRecord->stateMask),
             uVar4 = extraout_EDX_00, unaff_EBX != 0))) {
        scratchRecord = scratchRecord + 1;
        FVar3 = extraout_EAX_01 + 0x240;
      }
      FVar3 = uVar4 + 499;
      currentY_03 = uVar6 - 0x120;
      iVar2 = -g_GridScratchWidth;
      scratchCell1 = scratchCell2 + iVar2;
      while (GridFootprint_ClearTraversalFlagsDiagonalPositive
                       (worldYQ12,worldXQ12,FVar3,currentY_03,&scratchCell1->stateMask),
            unaff_EBX != 0) {
        scratchCell1 = scratchCell1 + -1;
        currentY_03 = extraout_ECX_00 + -0x240;
        FVar3 = currentX_02;
      }
      GridFootprint_ClearTraversalFlagsDiagonalPositive
                (worldYQ12,worldXQ12,currentX_02,currentY_00,&(scratchCell2 + iVar2)[1].stateMask);
      scratchCell2 = scratchCell2 + g_GridScratchWidth;
      GridFootprint_ClearTraversalFlagsDiagonalNegative
                (worldYQ12,worldXQ12,extraout_EDX_01 + -999,uVar6 + 0x120,&scratchCell2->stateMask);
      GridFootprint_ClearTraversalFlagsDiagonalNegative
                (worldYQ12,worldXQ12,currentX_03,currentY_01,&scratchCell2[-1].stateMask);
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
WorldPositionXYRegisterPairQ12
EntityPathing_UpdateRouteSegment
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *sourceRouteEntityRuntime,
          GameEntityRuntime *routeEntityRuntime)

{
  void *pvVar1;
  void *pvVar2;
  ArmyMovementRuntime *movementRuntime;
  longlong lVar3;
  longlong lVar4;
  UQ12 extraout_EAX;
  UQ12 segmentWorldXQ12;
  byte bVar5;
  int iVar6;
  int iVar7;
  dword dVar8;
  int extraout_ECX;
  uint uVar9;
  int iVar10;
  int iVar11;
  dword dVar12;
  UQ12 extraout_EDX;
  UQ12 UVar13;
  bool bVar14;
  GridPathNearestCellCfRegisterResult GVar15;
  GraphicsWorldCoordinateQ12 GVar16;
  FieldGridRadiusUnits radiusMetric;
  UQ12 segmentWorldYQ12;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
    [*(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0x4c)])
            (routeEntityRuntime);
  lVar3 = (longlong)(modelNode1->worldTransform).translation.x * 0x1c6e9c;
  lVar4 = (longlong)(modelNode1->worldTransform).translation.y * -0x20c8cc;
  uVar9 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
  iVar6 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar9) + 0x800) >>
          10;
  iVar10 = (int)(uVar9 * 2 + 0x800) >> 10;
  if (iVar6 < 1) {
    iVar6 = 1;
  }
  if (iVar10 < 1) {
    iVar10 = 1;
  }
  dVar8 = iVar6 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar6 + 2U)) {
    dVar8 = g_GridScratchWidth;
  }
  dVar12 = iVar10 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar10 + 2U)) {
    dVar12 = g_GridScratchHeight;
  }
  iVar6 = dVar8 - 2;
  iVar10 = dVar12 - 2;
  pvVar1 = (routeEntityRuntime->common).ownership.runtimeLink;
  if (routeEntityRuntime != sourceRouteEntityRuntime) {
    targetWorldXQ12 = *(UQ12 *)((int)pvVar1 + 0x58);
    targetWorldYQ12 = *(UQ12 *)((int)pvVar1 + 0x5c);
  }
  uVar9 = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  iVar7 = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - uVar9) + 0x800) >> 10;
  iVar11 = (int)(uVar9 * 2 + 0x800) >> 10;
  if (iVar7 < 1) {
    iVar7 = 1;
  }
  if (iVar11 < 1) {
    iVar11 = 1;
  }
  dVar8 = iVar7 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar7 + 2U)) {
    dVar8 = g_GridScratchWidth;
  }
  dVar12 = iVar11 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar11 + 2U)) {
    dVar12 = g_GridScratchHeight;
  }
  pvVar2 = (routeEntityRuntime->common).ownership.definitionOrClassRecord;
  g_GridPathEntityClassMask = 1 << ((byte)*(undefined4 *)((int)pvVar1 + 0xc) & 0x1f);
  bVar5 = (byte)*(undefined4 *)((int)pvVar2 + 0x260);
  g_GridPathHighCostMask = 0x10000 << (bVar5 & 0x1f);
  bVar14 = false;
  g_GridPathBlockingMask =
       0x100 << (bVar5 & 0x1f) | 0x1000000 << ((byte)*(undefined4 *)((int)pvVar2 + 0x264) & 0x1f);
  GVar15 = GridPathCost_FindNearestUnblockedCell(iVar10,iVar6);
  iVar7 = GVar15.selectedColumn;
  iVar11 = GVar15.selectedRow;
  if (!bVar14) {
    iVar7 = iVar10 * g_GridScratchWidth + iVar6;
    bVar14 = CARRY4((uint)g_GridScratchPrimary,iVar7 * 8);
    GridPathLine_TestHexSegmentClearCf
              (0,iVar10,iVar6,g_GridScratchPrimary + iVar7,
               g_GridScratchPrimary + (dVar12 - 2) * g_GridScratchWidth + (dVar8 - 2));
    iVar7 = dVar8 - 2;
    iVar11 = dVar12 - 2;
    if (!bVar14) goto EntityPathing_UpdateRouteSegment_RasterizeResolvedTargetSegment;
  }
  iVar6 = iVar11 * 0x400 + -0x600;
  lVar3 = (longlong)(iVar6 + (iVar7 * 0x400 + -0x600) * 2) * 0x901;
  targetWorldXQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd;
  lVar3 = (longlong)iVar6 * -1999;
  targetWorldYQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc;
EntityPathing_UpdateRouteSegment_RasterizeResolvedTargetSegment:
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  radiusMetric = *(FieldGridRadiusUnits *)
                  ((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0xdc);
  GVar16 = (modelNode1->worldTransform).translation.x;
  UVar13 = (modelNode1->worldTransform).translation.y;
  uVar9 = 1;
  segmentWorldYQ12 = targetWorldYQ12;
  segmentWorldXQ12 = targetWorldXQ12;
  do {
    while( true ) {
      iVar6 = segmentWorldXQ12 - GVar16;
      if (iVar6 < 0) {
        iVar6 = -iVar6;
      }
      iVar10 = segmentWorldYQ12 - UVar13;
      if (iVar10 < 0) {
        iVar10 = -iVar10;
      }
      if ((0x3f < uVar9) || ((iVar6 < 0x241 && (iVar10 < 0x241)))) break;
      segmentWorldXQ12 = (int)(segmentWorldXQ12 + GVar16) >> 1;
      UVar13 = (int)(segmentWorldYQ12 + UVar13) >> 1;
      uVar9 = uVar9 + 1;
      segmentWorldYQ12 = UVar13;
    }
    GVar16 = 0x536c13;
    GridInfluence_SetLowDistanceBandsAroundWorldPoint
              (radiusMetric,segmentWorldYQ12,segmentWorldXQ12);
    uVar9 = extraout_ECX - 1;
  } while (uVar9 != 0);
  movementRuntime = (routeEntityRuntime->common).ownership.runtimeLink;
  if ((routeEntityRuntime != sourceRouteEntityRuntime) &&
     ((targetWorldXQ12 != movementRuntime->movementWorldXQ12 ||
      (targetWorldYQ12 != movementRuntime->movementWorldYQ12)))) {
    ArmyRuntime_SetPendingMoveTarget(targetWorldYQ12,targetWorldXQ12,movementRuntime);
    targetWorldXQ12 = extraout_EAX;
    targetWorldYQ12 = extraout_EDX;
  }
  return CONCAT44(targetWorldYQ12,targetWorldXQ12);
}

/* Address: 0x00533D60.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test world point reachability carry-flag result.
   Local calls: GridScratch_TestConnectedReachabilityRecursiveCfRegs.
*/
void GridScratch_TestWorldPointReachabilityCf
               (undefined4 traversalMask,GraphicsWorldCoordinateQ12 sourceWorldXQ12,
               GraphicsWorldCoordinateQ12 sourceWorldYQ12,GraphicsWorldCoordinateQ12 targetWorldXQ12
               ,GraphicsWorldCoordinateQ12 targetWorldYQ12)

{
  int iVar1;
  GridScratchCell *scratchCell1;
  
  iVar1 = g_GridScratchHeight * g_GridScratchWidth;
  scratchCell1 = g_GridScratchPrimary;
  do {
    scratchCell1->stateMask = scratchCell1->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[1].stateMask = scratchCell1[1].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[2].stateMask = scratchCell1[2].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[3].stateMask = scratchCell1[3].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[4].stateMask = scratchCell1[4].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[5].stateMask = scratchCell1[5].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[6].stateMask = scratchCell1[6].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[7].stateMask = scratchCell1[7].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[8].stateMask = scratchCell1[8].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[9].stateMask = scratchCell1[9].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[10].stateMask = scratchCell1[10].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xb].stateMask = scratchCell1[0xb].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xc].stateMask = scratchCell1[0xc].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xd].stateMask = scratchCell1[0xd].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xe].stateMask = scratchCell1[0xe].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1[0xf].stateMask = scratchCell1[0xf].stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    scratchCell1 = scratchCell1 + 0x10;
    iVar1 = iVar1 + -0x10;
  } while (iVar1 != 0);
  GridScratch_TestConnectedReachabilityRecursiveCfRegs();
  return;
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
GridPathBacktrackCfRegisterResult
GridPathCost_BacktrackBestHexRoute
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate targetRow,
          FieldGridCellCoordinate targetColumn,GridScratchCell *startCell)

{
  dword dVar1;
  GridScratchCell *extraout_EAX;
  GridScratchCell *scanCell2;
  uint bestNeighborCost1;
  FieldGridRegionMask extraout_ECX;
  uint uVar2;
  GridScratchCell *scanCell1;
  GridScratchCell *endCell;
  bool bVar3;
  GridPathBacktrackCfRegisterResult GVar4;
  GridPathBacktrackCfRegisterResult GVar5;
  
  dVar1 = g_GridScratchWidth;
  GVar5.selectedRow = g_GridScratchWidth * 8;
  scanCell2 = startCell;
  do {
    scanCell1 = scanCell2 + -dVar1;
    if ((scanCell2->stateMask & g_GridPathHighCostMask) != 0) {
      callerBlockingMask = 0;
    }
    uVar2 = scanCell2->pathCost;
    endCell = (GridScratchCell *)0x0;
    if (scanCell1->pathCost < uVar2) {
      uVar2 = scanCell1->pathCost;
      endCell = scanCell1;
    }
    if (scanCell1[1].pathCost < uVar2) {
      uVar2 = scanCell1[1].pathCost;
      endCell = scanCell1 + 1;
    }
    if (scanCell1[dVar1 - 1].pathCost < uVar2) {
      uVar2 = scanCell1[dVar1 - 1].pathCost;
      endCell = scanCell1 + (dVar1 - 1);
    }
    if (scanCell1[dVar1 + 1].pathCost < uVar2) {
      uVar2 = scanCell1[dVar1 + 1].pathCost;
      endCell = scanCell1 + dVar1 + 1;
    }
    if (scanCell1[dVar1 * 2 + -1].pathCost < uVar2) {
      uVar2 = scanCell1[dVar1 * 2 + -1].pathCost;
      endCell = scanCell1 + dVar1 * 2 + -1;
    }
    if (scanCell1[dVar1 * 2].pathCost < uVar2) {
      endCell = scanCell1 + dVar1 * 2;
    }
    bVar3 = false;
    if (endCell == (GridScratchCell *)0x0)
    goto GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn;
    GridPathLine_TestHexSegmentClearCf(callerBlockingMask,targetRow,targetColumn,startCell,endCell);
    callerBlockingMask = extraout_ECX;
    scanCell2 = endCell;
  } while (!bVar3);
  scanCell2 = extraout_EAX;
  if (extraout_EAX == startCell) {
    scanCell2 = endCell;
  }
GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn:
  if (scanCell2->pathCost != 0) {
    bestNeighborCost1 = (uint)((int)scanCell2 - (int)g_GridScratchPrimary) >> 3;
    GVar4.selectedRow = bestNeighborCost1 / g_GridScratchWidth;
    GVar4.selectedColumn = bestNeighborCost1 % g_GridScratchWidth;
    GVar4.routeStateMask = callerBlockingMask;
    return GVar4;
  }
  GVar5.selectedColumn = (FieldGridCellCoordinate)scanCell2;
  GVar5.routeStateMask = callerBlockingMask;
  return GVar5;
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
  int rowBaseIndex;
  uint uVar1;
  GridPathMarkedRegionCellRegisterResult GVar2;
  
  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  GridPathRegion_MarkUnreachableRecursive();
  uVar1 = rowBaseIndex + column & 0x1fffffff;
  GVar2.selectedRow = uVar1 / g_GridScratchWidth;
  GVar2.selectedColumn = uVar1 % g_GridScratchWidth;
  return GVar2;
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
void __cdecl GridScratch_SwapPrimarySecondary(void)

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
void GridScratch_FloodFillConnectedCellsRegs(void)

{
  uint in_EAX;
  uint uVar1;
  int iVar2;
  int unaff_EBX;
  uint *unaff_ESI;
  uint *puVar3;
  undefined8 uVar4;
  
  if ((*unaff_ESI & 0x80000001) == 0) {
    *unaff_ESI = *unaff_ESI | 1;
    puVar3 = unaff_ESI;
    while (puVar3 = puVar3 + -2, (*puVar3 & in_EAX) == 0) {
      *puVar3 = *puVar3 | 1;
    }
    while (unaff_ESI = unaff_ESI + 2, (*unaff_ESI & in_EAX) == 0) {
      *unaff_ESI = *unaff_ESI | 1;
    }
    iVar2 = unaff_EBX + (int)puVar3;
    puVar3 = (uint *)((int)puVar3 + (8 - unaff_EBX));
    do {
      uVar4 = CONCAT44(iVar2,in_EAX);
      if ((*puVar3 & in_EAX) == 0) {
        uVar4 = GridScratch_FloodFillConnectedCellsRegs();
      }
      iVar2 = (int)((ulonglong)uVar4 >> 0x20);
      in_EAX = (uint)uVar4;
      puVar3 = puVar3 + 2;
    } while (puVar3 <= (uint *)((int)unaff_ESI - unaff_EBX));
    do {
      puVar3 = (uint *)((ulonglong)uVar4 >> 0x20);
      uVar1 = (uint)uVar4;
      if ((*puVar3 & uVar1) == 0) {
        uVar1 = GridScratch_FloodFillConnectedCellsRegs();
      }
      puVar3 = puVar3 + 2;
      uVar4 = CONCAT44(puVar3,uVar1);
    } while (puVar3 < (uint *)(((int)unaff_ESI - unaff_EBX) + unaff_EBX * 2));
  }
  return;
}

/* Address: 0x00533C50.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test connected reachability recursive carry-flag result register result.
*/
void GridScratch_TestConnectedReachabilityRecursiveCfRegs(void)

{
  uint in_EAX;
  uint *extraout_ECX;
  uint *puVar1;
  uint *extraout_ECX_00;
  uint *puVar2;
  int unaff_EBX;
  uint *unaff_ESI;
  uint *puVar3;
  uint *unaff_EDI;
  bool bVar4;
  undefined8 uVar5;
  
  *unaff_ESI = *unaff_ESI | 1;
  puVar1 = unaff_ESI;
  if (unaff_EDI == unaff_ESI) {
    return;
  }
  while( true ) {
    puVar3 = puVar1 + -2;
    if (unaff_EDI == puVar3) {
      return;
    }
    if ((*puVar3 & in_EAX) != 0) break;
    *puVar3 = *puVar3 | 1;
    puVar1 = puVar3;
  }
  while( true ) {
    unaff_ESI = unaff_ESI + 2;
    if (unaff_EDI == unaff_ESI) {
      return;
    }
    if ((*unaff_ESI & in_EAX) != 0) break;
    *unaff_ESI = *unaff_ESI | 1;
  }
  if (unaff_EDI <= puVar3) {
    puVar1 = (uint *)(unaff_EBX + (int)puVar3);
    puVar2 = (uint *)((int)unaff_ESI - unaff_EBX);
    puVar3 = (uint *)((int)puVar3 + (8 - unaff_EBX));
    while( true ) {
      uVar5 = CONCAT44(puVar2,in_EAX);
      bVar4 = false;
      if (((*puVar3 & in_EAX) == 0) &&
         (uVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs(), puVar1 = extraout_ECX,
         !bVar4)) break;
      puVar2 = (uint *)((ulonglong)uVar5 >> 0x20);
      in_EAX = (uint)uVar5;
      puVar3 = puVar3 + 2;
      if (puVar2 < puVar3) {
        uVar5 = CONCAT44((int)puVar2 + unaff_EBX * 2,in_EAX);
        while ((bVar4 = false, (*puVar1 & (uint)uVar5) != 0 ||
               (uVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs(), bVar4))) {
          puVar1 = puVar1 + 2;
          if ((uint *)((ulonglong)uVar5 >> 0x20) <= puVar1) {
            return;
          }
        }
        return;
      }
    }
    return;
  }
  puVar2 = (uint *)((int)unaff_ESI + unaff_EBX);
  puVar1 = (uint *)((int)puVar1 - unaff_EBX);
  puVar3 = (uint *)((int)puVar3 + unaff_EBX);
  while( true ) {
    uVar5 = CONCAT44(puVar2,in_EAX);
    bVar4 = false;
    if (((*puVar3 & in_EAX) == 0) &&
       (uVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs(), puVar1 = extraout_ECX_00,
       !bVar4)) break;
    puVar2 = (uint *)((ulonglong)uVar5 >> 0x20);
    in_EAX = (uint)uVar5;
    puVar3 = puVar3 + 2;
    if (puVar2 <= puVar3) {
      uVar5 = CONCAT44((int)puVar2 + unaff_EBX * -2,in_EAX);
      while ((bVar4 = false, (*puVar1 & (uint)uVar5) != 0 ||
             (uVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs(), bVar4))) {
        puVar1 = puVar1 + 2;
        if ((uint *)((ulonglong)uVar5 >> 0x20) < puVar1) {
          return;
        }
      }
      return;
    }
  }
  return;
}

/* Address: 0x00533EF0.
   Ownership: world/pathing/grid.
   Purpose: Runs a bounded queue-based propagation over the six neighboring grid records. Typed parameters: p3
   queueBaseOffset→FieldGridByteOffset. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Typed parameters: p4 startRow→FieldGridCellCoordinate_V331, p5 startColumn→FieldGridCellCoordinate_V331.
*/
void GridPathCost_PropagateWeightedHexNeighbors
               (GridPathPassCount remainingPasses,GridScratchCell *originCell,
               FieldGridCellCoordinate startRow,FieldGridCellCoordinate startColumn)

{
  GridPathCost GVar1;
  GridScratchStateMask GVar2;
  dword dVar3;
  uint uVar4;
  GridScratchCell **queueCursor1;
  GridScratchCell *scratchCell1;
  GridScratchCell *scratchCell2;
  GridScratchCell **queueCursor2;
  
  dVar3 = g_GridScratchWidth;
  queueCursor1 = g_GridPathCostQueueBegin;
  queueCursor2 = g_GridPathCostQueueBegin + 1;
  scratchCell1 = g_GridScratchPrimary + startRow * g_GridScratchWidth + startColumn;
  *g_GridPathCostQueueBegin = scratchCell1;
  g_GridPathCostQueuePassBoundary = queueCursor1;
  scratchCell1->pathCost = 0;
  g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + 0x10000;
GridPathCost_ProcessNextQueuedCell:
  do {
    do {
      while( true ) {
        if (queueCursor1 == queueCursor2) {
          return;
        }
        if (queueCursor1 < g_GridPathCostQueuePassBoundary) break;
        g_GridPathCostQueuePassBoundary = g_GridPathCostQueuePassBoundary + 0x10000;
        if (originCell->pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[-dVar3].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[1 - dVar3].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[-1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[dVar3 - 1].pathCost < 0x7fffffff) {
          return;
        }
        if (originCell[dVar3].pathCost < 0x7fffffff) {
          return;
        }
        remainingPasses = remainingPasses + -1;
        if (remainingPasses == 0) {
          return;
        }
      }
      scratchCell1 = *queueCursor1;
      queueCursor1 = queueCursor1 + 1;
    } while ((scratchCell1->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0);
    GVar1 = scratchCell1->pathCost;
    scratchCell1 = scratchCell1 + -dVar3;
    GVar2 = scratchCell1->stateMask;
    uVar4 = GVar1 + 4;
    if (-1 < (int)GVar2) {
      if ((g_GridPathEntityClassMask & GVar2) != 0) {
        uVar4 = GVar1 + 3;
        if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_SkipBlockedNorthNeighbor;
        if ((g_GridPathHighCostMask & GVar2) != 0) {
          uVar4 = GVar1 + 0xc;
        }
      }
      if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell1->pathCost)) {
        *queueCursor2 = scratchCell1;
        scratchCell1->pathCost = uVar4;
        queueCursor2 = queueCursor2 + 1;
      }
    }
GridPathCost_SkipBlockedNorthNeighbor:
    scratchCell2 = scratchCell1 + 1;
    GVar2 = scratchCell2->stateMask;
    uVar4 = GVar1 + 4;
    if (-1 < (int)GVar2) {
      if ((g_GridPathEntityClassMask & GVar2) != 0) {
        uVar4 = GVar1 + 3;
        if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_SkipBlockedEastNeighbor;
        if ((g_GridPathHighCostMask & GVar2) != 0) {
          uVar4 = GVar1 + 0xc;
        }
      }
      if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell1[1].pathCost)) {
        *queueCursor2 = scratchCell2;
        scratchCell1[1].pathCost = uVar4;
        queueCursor2 = queueCursor2 + 1;
      }
    }
GridPathCost_SkipBlockedEastNeighbor:
    scratchCell2 = scratchCell2 + dVar3;
    GVar2 = scratchCell2->stateMask;
    uVar4 = GVar1 + 4;
    if (-1 < (int)GVar2) {
      if ((g_GridPathEntityClassMask & GVar2) != 0) {
        uVar4 = GVar1 + 3;
        if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_SkipBlockedSouthEastNeighbor;
        if ((g_GridPathHighCostMask & GVar2) != 0) {
          uVar4 = GVar1 + 0xc;
        }
      }
      if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell2->pathCost)) {
        *queueCursor2 = scratchCell2;
        scratchCell2->pathCost = uVar4;
        queueCursor2 = queueCursor2 + 1;
      }
    }
GridPathCost_SkipBlockedSouthEastNeighbor:
    scratchCell1 = scratchCell2 + -2;
    GVar2 = scratchCell1->stateMask;
    uVar4 = GVar1 + 4;
    if (-1 < (int)GVar2) {
      if ((g_GridPathEntityClassMask & GVar2) != 0) {
        uVar4 = GVar1 + 3;
        if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_SkipBlockedSouthWestNeighbor;
        if ((g_GridPathHighCostMask & GVar2) != 0) {
          uVar4 = GVar1 + 0xc;
        }
      }
      if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell2[-2].pathCost)) {
        *queueCursor2 = scratchCell1;
        scratchCell2[-2].pathCost = uVar4;
        queueCursor2 = queueCursor2 + 1;
      }
    }
GridPathCost_SkipBlockedSouthWestNeighbor:
    scratchCell1 = scratchCell1 + dVar3;
    GVar2 = scratchCell1->stateMask;
    uVar4 = GVar1 + 4;
    if (-1 < (int)GVar2) {
      if ((g_GridPathEntityClassMask & GVar2) != 0) {
        uVar4 = GVar1 + 3;
        if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_SkipBlockedWestNeighbor;
        if ((g_GridPathHighCostMask & GVar2) != 0) {
          uVar4 = GVar1 + 0xc;
        }
      }
      if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell1->pathCost)) {
        *queueCursor2 = scratchCell1;
        scratchCell1->pathCost = uVar4;
        queueCursor2 = queueCursor2 + 1;
      }
    }
GridPathCost_SkipBlockedWestNeighbor:
    GVar2 = scratchCell1[1].stateMask;
    uVar4 = GVar1 + 4;
  } while ((int)GVar2 < 0);
  if ((g_GridPathEntityClassMask & GVar2) != 0) {
    uVar4 = GVar1 + 3;
    if ((g_GridPathBlockingMask & GVar2) != 0) goto GridPathCost_ProcessNextQueuedCell;
    if ((g_GridPathHighCostMask & GVar2) != 0) {
      uVar4 = GVar1 + 0xc;
    }
  }
  if ((queueCursor2 < g_GridPathCostQueueEnd) && (uVar4 < scratchCell1[1].pathCost)) {
    *queueCursor2 = scratchCell1 + 1;
    scratchCell1[1].pathCost = uVar4;
    queueCursor2 = queueCursor2 + 1;
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
void GridPathRegion_MarkUnreachableRecursive(void)

{
  uint uVar1;
  uint *puVar2;
  uint *extraout_ECX;
  uint *extraout_ECX_00;
  int unaff_EBX;
  uint *unaff_ESI;
  uint *puVar3;
  uint *puVar4;
  
  *unaff_ESI = *unaff_ESI | 1;
  puVar3 = unaff_ESI + 2;
  puVar2 = unaff_ESI + -2;
  do {
    puVar4 = puVar2;
    uVar1 = *puVar4;
    if ((puVar4[1] < 0x7fffffff) || ((int)uVar1 < 0))
    goto GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary;
    *puVar4 = *puVar4 | 1;
    puVar2 = puVar4 + -2;
  } while (((g_GridPathEntityClassMask & uVar1) == 0) || ((g_GridPathBlockingMask & uVar1) == 0));
  *puVar4 = *puVar4 & 0xfffffffe;
GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary:
  do {
    puVar2 = puVar3;
    uVar1 = *puVar2;
    if ((puVar2[1] < 0x7fffffff) || ((int)uVar1 < 0))
    goto GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows;
    *puVar2 = *puVar2 | 1;
    puVar3 = puVar2 + 2;
  } while (((g_GridPathEntityClassMask & uVar1) == 0) || ((g_GridPathBlockingMask & uVar1) == 0));
  *puVar2 = *puVar2 & 0xfffffffe;
GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows:
  puVar2 = (uint *)((int)puVar2 - unaff_EBX);
  puVar3 = (uint *)((int)puVar4 + (8 - unaff_EBX));
  do {
    uVar1 = *puVar3;
    if (((0x7ffffffe < puVar3[1]) && ((uVar1 & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & uVar1) == 0 || ((g_GridPathBlockingMask & uVar1) == 0)))) {
      GridPathRegion_MarkUnreachableRecursive();
      puVar2 = extraout_ECX;
    }
    puVar3 = puVar3 + 2;
  } while (puVar3 <= puVar2);
  puVar2 = (uint *)((int)puVar2 + unaff_EBX * 2);
  puVar4 = (uint *)((int)puVar4 + unaff_EBX);
  do {
    uVar1 = *puVar4;
    if (((0x7ffffffe < puVar4[1]) && ((uVar1 & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & uVar1) == 0 || ((g_GridPathBlockingMask & uVar1) == 0)))) {
      GridPathRegion_MarkUnreachableRecursive();
      puVar2 = extraout_ECX_00;
    }
    puVar4 = puVar4 + 2;
  } while (puVar4 < puVar2);
  return;
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
void GridFootprint_ClearTraversalFlagsDiagonalNegative
               (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
               FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord
               )

{
  int squaredXDistanceMetric;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  do {
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return;
    }
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    currentX = currentX + -999;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * 4 + -2;
  } while ((*scratchRecord & 0x80000000) == 0);
  return;
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
void GridFootprint_ClearTraversalFlagsDiagonalPositive
               (FieldGridCellCoordinate centerX,FieldGridCellCoordinate centerY,
               FieldGridCellCoordinate currentX,FieldGridCellCoordinate currentY,uint *scratchRecord
               )

{
  int squaredXDistanceMetric;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  do {
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return;
    }
    currentX = currentX + 999;
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * -4 + 2;
  } while ((*scratchRecord & 0x80000000) == 0);
  return;
}

/* Address: 0x005363C0.
   Ownership: world/pathing/grid.
   Purpose: Recursively sets scratch bit 0 across a connected region whose cells pass the verified hard-boundary
   and influence-mask test. Expansion follows the six-neighbor staggered-grid topology.
*/
void GridReachability_MarkOpenRegionRecursive(void)

{
  GridScratchCell *extraout_ECX;
  GridScratchCell *scanCell2;
  GridScratchCell *scanCell3;
  GridScratchCell *extraout_EDX;
  GridScratchCell *extraout_EDX_00;
  GridScratchCell *pGVar1;
  uint *unaff_ESI;
  uint *puVar2;
  GridScratchCell *scanCell1;
  int unaff_EDI;
  
  *unaff_ESI = *unaff_ESI | 1;
  puVar2 = unaff_ESI;
  while (puVar2 = puVar2 + -2, (*puVar2 & 0xf0007f01) == 0) {
    *puVar2 = *puVar2 | 1;
  }
  while (unaff_ESI = unaff_ESI + 2, (*unaff_ESI & 0xf0007f01) == 0) {
    *unaff_ESI = *unaff_ESI | 1;
  }
  scanCell2 = (GridScratchCell *)((int)puVar2 + unaff_EDI);
  scanCell3 = (GridScratchCell *)((int)unaff_ESI - unaff_EDI);
  scanCell1 = (GridScratchCell *)((int)puVar2 + (8 - unaff_EDI));
  do {
    if ((scanCell1->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive();
      scanCell2 = extraout_ECX;
      scanCell3 = extraout_EDX;
    }
    scanCell1 = scanCell1 + 1;
  } while (scanCell1 <= scanCell3);
  pGVar1 = (GridScratchCell *)((int)&scanCell3->stateMask + unaff_EDI * 2);
  do {
    if ((scanCell2->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive();
      pGVar1 = extraout_EDX_00;
    }
    scanCell2 = scanCell2 + 1;
  } while (scanCell2 < pGVar1);
  return;
}

/* Address: 0x00536440.
   Ownership: world/pathing/grid.
   Purpose: Recursively clears scratch bit 0 across connected cells that have a nonzero companion cost, following
   the same six-neighbor topology.
*/
void GridReachability_ClearCostedRegionRecursive(void)

{
  GridScratchCell *extraout_ECX;
  GridScratchCell *scanCell2;
  uint *puVar1;
  GridScratchCell *scanCell3;
  GridScratchCell *extraout_EDX;
  GridScratchCell *extraout_EDX_00;
  GridScratchCell *pGVar2;
  uint *unaff_ESI;
  uint *puVar3;
  GridScratchCell *scanCell1;
  int unaff_EDI;
  
  *unaff_ESI = *unaff_ESI & 0xfffffffe;
  puVar1 = unaff_ESI + 2;
  while ((puVar3 = unaff_ESI + -2, (*puVar3 & 1) != 0 && (unaff_ESI[-1] != 0))) {
    *puVar3 = *puVar3 & 0xfffffffe;
    unaff_ESI = puVar3;
  }
  for (; ((*puVar1 & 1) != 0 && (puVar1[1] != 0)); puVar1 = puVar1 + 2) {
    *puVar1 = *puVar1 & 0xfffffffe;
  }
  scanCell2 = (GridScratchCell *)((int)puVar3 + unaff_EDI);
  scanCell3 = (GridScratchCell *)((int)puVar1 - unaff_EDI);
  scanCell1 = (GridScratchCell *)((int)puVar3 + (8 - unaff_EDI));
  do {
    if (((scanCell1->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) && (scanCell1->pathCost != 0)
       ) {
      GridReachability_ClearCostedRegionRecursive();
      scanCell2 = extraout_ECX;
      scanCell3 = extraout_EDX;
    }
    scanCell1 = scanCell1 + 1;
  } while (scanCell1 <= scanCell3);
  pGVar2 = (GridScratchCell *)((int)&scanCell3->stateMask + unaff_EDI * 2);
  do {
    if (((scanCell2->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) && (scanCell2->pathCost != 0)
       ) {
      GridReachability_ClearCostedRegionRecursive();
      pGVar2 = extraout_EDX_00;
    }
    scanCell2 = scanCell2 + 1;
  } while (scanCell2 < pGVar2);
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
GridPathNearestCellCfRegisterResult
GridPathCost_FindNearestUnblockedCell(FieldGridCellCoordinate gridY,FieldGridCellCoordinate gridX)

{
  int iVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  dword in_EDX;
  GridScratchStateMask GVar7;
  dword dVar8;
  int bestHexDistance1;
  undefined4 unaff_EBX;
  int searchRow1;
  int iVar9;
  GridScratchCell *scratchCell1;
  GridPathNearestCellCfRegisterResult GVar10;
  GridPathNearestCellCfRegisterResult GVar11;
  GridPathNearestCellCfRegisterResult GVar12;
  GridScratchCell *local_30;
  int local_28;
  int local_1c;
  int local_18;
  
  iVar1 = gridY * g_GridScratchWidth + gridX;
  GVar10.selectedColumn = iVar1 * 8;
  GVar7 = g_GridPathBlockingMask | 0x80000000;
  if (((((g_GridScratchPrimary[iVar1].stateMask & GVar7) == 0) ||
       (scratchCell1 = g_GridScratchPrimary + iVar1 + -g_GridScratchWidth,
       (scratchCell1->stateMask & GVar7) == 0)) || ((scratchCell1[1].stateMask & GVar7) == 0)) ||
     ((((scratchCell1[g_GridScratchWidth - 1].stateMask & GVar7) == 0 ||
       ((scratchCell1[g_GridScratchWidth + 1].stateMask & GVar7) == 0)) ||
      (((scratchCell1[g_GridScratchWidth * 2 + -1].stateMask & GVar7) == 0 ||
       ((scratchCell1[g_GridScratchWidth * 2].stateMask & GVar7) == 0)))))) {
    GVar10.selectedRow = unaff_EBX;
    GVar10.preservedEdxCompatibility = in_EDX;
    return GVar10;
  }
  iVar1 = gridX + -0x10;
  if (iVar1 < 0) {
    iVar1 = 0;
  }
  searchRow1 = gridY + -0x10;
  if (searchRow1 < 0) {
    searchRow1 = 0;
  }
  dVar4 = gridX + 0x10U;
  if ((int)g_GridScratchWidth < (int)(gridX + 0x10U)) {
    dVar4 = g_GridScratchWidth;
  }
  dVar8 = gridY + 0x10U;
  if ((int)g_GridScratchHeight < (int)(gridY + 0x10U)) {
    dVar8 = g_GridScratchHeight;
  }
  iVar5 = dVar4 - iVar1;
  if (iVar5 != 0 && iVar1 <= (int)dVar4) {
    local_28 = dVar8 - searchRow1;
    if (local_28 != 0 && searchRow1 <= (int)dVar8) {
      scratchCell1 = g_GridScratchPrimary + searchRow1 * g_GridScratchWidth + iVar1;
      bestHexDistance1 = 0x7fffffff;
      iVar2 = iVar1;
      iVar6 = iVar5;
      local_30 = scratchCell1;
      do {
        do {
          if ((scratchCell1->stateMask & (g_GridPathBlockingMask | 0x80000000)) == 0) {
            iVar3 = iVar2 - gridX;
            if (iVar3 < 0) {
              iVar3 = -iVar3;
              iVar9 = searchRow1 - gridY;
              if (iVar9 < 0) {
                iVar3 = iVar3 - iVar9;
              }
              else if (iVar3 < iVar9) {
                iVar3 = iVar9;
              }
            }
            else {
              iVar9 = searchRow1 - gridY;
              if (iVar9 < 0) {
                if (iVar3 < -iVar9) {
                  iVar3 = -iVar9;
                }
              }
              else {
                iVar3 = iVar3 + iVar9;
              }
            }
            if (iVar3 < bestHexDistance1) {
              bestHexDistance1 = iVar3;
              local_1c = searchRow1;
              local_18 = iVar2;
            }
          }
          scratchCell1 = scratchCell1 + 1;
          iVar6 = iVar6 + -1;
          iVar2 = iVar2 + 1;
        } while (iVar6 != 0);
        scratchCell1 = local_30 + g_GridScratchWidth;
        searchRow1 = searchRow1 + 1;
        local_28 = local_28 + -1;
        iVar2 = iVar1;
        iVar6 = iVar5;
        local_30 = scratchCell1;
      } while (local_28 != 0);
      if (bestHexDistance1 < 0x7fffffff) {
        GVar11.selectedRow = local_1c;
        GVar11.selectedColumn = local_18;
        GVar11.preservedEdxCompatibility = in_EDX;
        return GVar11;
      }
    }
  }
  GVar12.selectedRow = gridY;
  GVar12.selectedColumn = gridX;
  GVar12.preservedEdxCompatibility = in_EDX;
  return GVar12;
}

/* Address: 0x005344B0.
   Ownership: world/pathing/grid.
   Purpose: The carry contract conveys whether the segment is acceptable. Typed parameters: p2
   callerBlockingMask→FieldGridRegionMask. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p3 startRow→FieldGridCellCoordinate_V331, p4
   startColumn→FieldGridCellCoordinate_V331.
*/
void GridPathLine_TestHexSegmentClearCf
               (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
               FieldGridCellCoordinate startColumn,GridScratchCell *startCell,
               GridScratchCell *endCell)

{
  GridScratchStateMask GVar1;
  uint uVar2;
  int rowDelta1;
  int iVar3;
  int columnDelta1;
  int iVar4;
  int iVar5;
  GridScratchCell *scanCell2;
  GridScratchCell *scanCell1;
  GridScratchCell *scanCell3;
  
  uVar2 = (uint)((int)endCell - (int)g_GridScratchPrimary) >> 3;
  rowDelta1 = uVar2 / g_GridScratchWidth - startRow;
  columnDelta1 = uVar2 % g_GridScratchWidth - startColumn;
  scanCell2 = startCell;
  if (columnDelta1 < 0) {
    rowDelta1 = -rowDelta1;
    columnDelta1 = -columnDelta1;
    scanCell2 = endCell;
    endCell = startCell;
  }
  if (rowDelta1 < 0) {
    iVar4 = 0;
    iVar3 = -rowDelta1 - columnDelta1;
GridPathLine_TestHexSegmentClear_ScanNegativeSlopeCell:
    do {
      GVar1 = scanCell2->stateMask;
      if ((scanCell2 != startCell) &&
         (((((int)GVar1 < 0 || ((g_GridPathEntityClassMask & GVar1) == 0)) ||
           ((g_GridPathBlockingMask & GVar1) != 0)) || ((callerBlockingMask & GVar1) != 0)))) {
        return;
      }
      if (scanCell2 == endCell) {
        return;
      }
      if (iVar4 == iVar3 || iVar4 < iVar3) {
        if (iVar4 != iVar3) {
          iVar4 = iVar4 + columnDelta1 * 2;
          scanCell2 = scanCell2 + -g_GridScratchWidth;
          goto GridPathLine_TestHexSegmentClear_ScanNegativeSlopeCell;
        }
        iVar4 = iVar4 + columnDelta1 * 2;
        scanCell2 = scanCell2 + -g_GridScratchWidth;
      }
      iVar4 = iVar4 + rowDelta1 * 2;
      scanCell2 = scanCell2 + 1;
    } while( true );
  }
  iVar5 = 0;
  iVar4 = 0;
  iVar3 = columnDelta1;
  scanCell3 = scanCell2;
GridPathLine_TestHexSegmentClear_AdvancePositiveSlopeColumns:
  if (iVar5 < rowDelta1) goto code_r0x00534514;
  goto GridPathLine_TestHexSegmentClear_ScanPositiveSlopeColumn;
code_r0x00534514:
  scanCell2 = scanCell2 + g_GridScratchWidth;
  iVar5 = iVar5 + columnDelta1;
  if (scanCell2 == endCell) {
GridPathLine_TestHexSegmentClear_ScanPositiveSlopeColumn:
    do {
      iVar5 = iVar5 - rowDelta1;
      scanCell1 = scanCell2;
      while( true ) {
        GVar1 = scanCell1->stateMask;
        if (scanCell1 != startCell) {
          if ((int)GVar1 < 0) {
            return;
          }
          if ((g_GridPathEntityClassMask & GVar1) == 0) {
            return;
          }
          if ((g_GridPathBlockingMask & GVar1) != 0) {
            return;
          }
          if ((callerBlockingMask & GVar1) != 0) {
            return;
          }
        }
        if (scanCell1 == scanCell3) break;
        scanCell1 = scanCell1 + -g_GridScratchWidth;
      }
      iVar3 = iVar3 + -1;
      if (iVar3 < 0) {
        return;
      }
      scanCell2 = scanCell2 + 1;
      scanCell3 = scanCell3 + 1;
      for (; iVar4 <= -columnDelta1; iVar4 = iVar4 + columnDelta1) {
        scanCell3 = scanCell3 + g_GridScratchWidth;
      }
      iVar4 = iVar4 - rowDelta1;
    } while (iVar3 == 0);
  }
  goto GridPathLine_TestHexSegmentClear_AdvancePositiveSlopeColumns;
}
