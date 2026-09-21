/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/grid.c
 * Reverse engineering by idkFoxes 2026
 */

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

EntityPathingDestinationEaxEdxEbxEcxCf17
EntityPathing_ResolveDestinationAndRebuildRoutes
          (UQ12 targetWorldYQ12,UQ12 targetWorldXQ12,GameEntityRuntime *routeEntityRuntime,
          WorldRuntimeContext *worldRuntime)

{
  GraphicsFixedVec3 *pGVar1;
  ModelRuntimeClassId MVar2;
  longlong lVar3;
  longlong lVar4;
  WorldPositionXYEaxEdx8 WVar5;
  WorldPositionXYEaxEdx8 WVar6;
  byte bVar7;
  int iVar8;
  dword dVar9;
  GridPathUnreachableReferenceColumn32 referenceColumn;
  FieldGridRegionMask callerBlockingMask;
  uint uVar10;
  int iVar11;
  int gridY;
  int iVar12;
  dword dVar13;
  GridPathUnreachableReferenceRow32 referenceRow;
  GridScratchCell *scratchCell1;
  bool bVar14;
  WorldPositionXYEaxEdx8 WVar15;
  GridPathNearestCellEaxEbxCf9 GVar16;
  GridPathMarkedRegionCellRegisterResult GVar17;
  EntityPathingDestinationEaxEdxEbxEcxCf17 EVar18;
  GridPathBacktrackEaxEbxEcxCf13 GVar19;
  ModelDefinitionRuntimeSemanticView280 *modelDefinition;
  ArmyRuntimeSlot *armyRuntime;
  GameEntityRuntime *entityRuntime1;
  ModelRuntimeNode *modelNode1;
  
  WVar6.worldYQ12 = targetWorldYQ12;
  WVar6.worldXQ12 = targetWorldXQ12;
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  lVar3 = (longlong)(modelNode1->worldTransform).translation.x * 0x1c6e9c;
  lVar4 = (longlong)(modelNode1->worldTransform).translation.y * -0x20c8cc;
  uVar10 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
  iVar8 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar10) + 0x800)
          >> 10;
  iVar11 = (int)(uVar10 * 2 + 0x800) >> 10;
  if (iVar8 < 1) {
    iVar8 = 1;
  }
  if (iVar11 < 1) {
    iVar11 = 1;
  }
  dVar9 = iVar8 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar8 + 2U)) {
    dVar9 = g_GridScratchWidth;
  }
  dVar13 = iVar11 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar11 + 2U)) {
    dVar13 = g_GridScratchHeight;
  }
  iVar8 = dVar9 - 2;
  gridY = dVar13 - 2;
  uVar10 = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
           (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  iVar11 = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                  (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - uVar10) + 0x800) >>
           10;
  iVar12 = (int)(uVar10 * 2 + 0x800) >> 10;
  if (iVar11 < 1) {
    iVar11 = 1;
  }
  if (iVar12 < 1) {
    iVar12 = 1;
  }
  dVar9 = iVar11 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar11 + 2U)) {
    dVar9 = g_GridScratchWidth;
  }
  dVar13 = iVar12 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar12 + 2U)) {
    dVar13 = g_GridScratchHeight;
  }
  referenceColumn = dVar9 - 2;
  referenceRow = dVar13 - 2;
  modelDefinition = (routeEntityRuntime->common).ownership.definitionOrClassRecord;
  entityRuntime1 =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  MVar2 = modelDefinition->runtimeClassId4C;
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
      [*(int *)((int)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c)])
              (entityRuntime1);
  }
  armyRuntime = (routeEntityRuntime->common).ownership.runtimeLink;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove[MVar2])
            (routeEntityRuntime);
  g_GridPathEntityClassMask = 1 << ((byte)armyRuntime->factionIndex & 0x1f);
  bVar7 = (byte)modelDefinition->gridClassification260;
  g_GridPathHighCostMask = 0x10000 << (bVar7 & 0x1f);
  g_GridPathBlockingMask =
       0x100 << (bVar7 & 0x1f) | 0x1000000 << ((byte)modelDefinition->gridClassification264 & 0x1f);
  GVar16 = GridPathCost_FindNearestUnblockedCell(gridY,iVar8);
  dVar9 = g_GridScratchWidth;
  if (GVar16.carry) {
    if ((GVar16.selectedColumn == iVar8) && (GVar16.selectedRow == gridY)) {
      pGVar1 = &(((routeEntityRuntime->common).ownership.modelNode)->worldTransform).translation;
      WVar6.worldXQ12 = pGVar1->x;
      WVar6.worldYQ12 = pGVar1->y;
      WVar15.worldXQ12 = pGVar1->x;
      WVar15.worldYQ12 = pGVar1->y;
    }
    else {
      iVar8 = GVar16.selectedRow * 0x400 + -0x600;
      lVar3 = (longlong)(iVar8 + (GVar16.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
      lVar4 = (longlong)iVar8 * -1999;
      WVar15 = EntityPathing_RebuildOverlappingGroupRoutes
                         ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                          (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                          routeEntityRuntime,worldRuntime);
    }
  }
  else {
    iVar11 = g_GridScratchWidth * 8;
    scratchCell1 = g_GridScratchPrimary + gridY * g_GridScratchWidth + iVar8;
    bVar14 = GridPathLine_TestHexSegmentClearCf
                       (g_GridPathHighCostMask,gridY,iVar8,scratchCell1,
                        g_GridScratchPrimary + referenceRow * g_GridScratchWidth + referenceColumn);
    if (bVar14) {
      GridScratch_ResetTraversalFlagsAndCosts();
      GridPathCost_PropagateWeightedHexNeighbors(6,scratchCell1,referenceRow,referenceColumn);
      scratchCell1 = scratchCell1 + -dVar9;
      if ((((0x7ffffffe < scratchCell1[dVar9].pathCost) && (0x7ffffffe < scratchCell1->pathCost)) &&
          (0x7ffffffe < scratchCell1[1].pathCost)) &&
         (((0x7ffffffe < scratchCell1[dVar9 - 1].pathCost &&
           (0x7ffffffe < scratchCell1[dVar9 + 1].pathCost)) &&
          ((0x7ffffffe < scratchCell1[dVar9 * 2 + -1].pathCost &&
           (0x7ffffffe < scratchCell1[dVar9 * 2].pathCost)))))) {
        GVar17 = GridPathRegion_MarkUnreachableFromCell(referenceRow,referenceColumn,gridY,iVar8);
        iVar11 = GVar17.selectedRow * 0x400 + -0x600;
        lVar3 = (longlong)(iVar11 + (GVar17.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
        targetWorldXQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd;
        lVar3 = (longlong)iVar11 * -1999;
        targetWorldYQ12 = (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc;
        GridScratch_ResetTraversalFlagsAndCosts();
        GridPathCost_PropagateWeightedHexNeighbors
                  (6,scratchCell1,GVar17.selectedRow,GVar17.selectedColumn);
        iVar11 = g_GridScratchWidth * 8;
        scratchCell1 = g_GridScratchPrimary +
                       ((gridY * g_GridScratchWidth + iVar8) - g_GridScratchWidth);
      }
      WVar6.worldYQ12 = targetWorldYQ12;
      WVar6.worldXQ12 = targetWorldXQ12;
      WVar5.worldYQ12 = targetWorldYQ12;
      WVar5.worldXQ12 = targetWorldXQ12;
      callerBlockingMask = g_GridPathHighCostMask;
      if ((*(uint *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0x18) & 2) != 0) {
        callerBlockingMask = 0;
      }
      GVar19 = GridPathCost_BacktrackBestHexRoute
                         (callerBlockingMask,gridY,iVar8,
                          (GridScratchCell *)((int)&scratchCell1->stateMask + iVar11));
      if (!GVar19.carry) {
        if (GVar19.routeStateMask == 0) {
          iVar8 = GVar19.selectedRow * 0x400 + -0x600;
          lVar3 = (longlong)(iVar8 + (GVar19.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          lVar4 = (longlong)iVar8 * -1999;
          WVar15 = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                              (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                              routeEntityRuntime,worldRuntime);
          WVar6 = WVar5;
        }
        else {
          iVar8 = GVar19.selectedRow * 0x400 + -0x600;
          lVar3 = (longlong)(iVar8 + (GVar19.selectedColumn * 0x400 + -0x600) * 2) * 0x901;
          lVar4 = (longlong)iVar8 * -1999;
          WVar15 = EntityPathing_RebuildOverlappingGroupRoutes
                             ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc,
                              (int)((ulonglong)lVar3 >> 0x20) << 0x13 | (uint)lVar3 >> 0xd,
                              routeEntityRuntime,worldRuntime);
        }
        goto EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn;
      }
    }
    WVar15 = EntityPathing_RebuildOverlappingGroupRoutes
                       (targetWorldYQ12,targetWorldXQ12,routeEntityRuntime,worldRuntime);
    WVar6 = WVar15;
  }
EntityPathing_ResolveDestinationAndRebuildRoutes_RestoreGridInfluenceAndReturn:
  targetWorldYQ12 = WVar6.worldYQ12;
  targetWorldXQ12 = WVar6.worldXQ12;
  entityRuntime1 =
       (routeEntityRuntime->common).pathingAndImpactState.pathingReferences.overlappingEntity;
  iVar8 = *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord + 0x4c);
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
      [*(int *)((int)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x4c)])
              (entityRuntime1);
  }
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd[iVar8])(routeEntityRuntime);
  EVar18.fallbackWorldXQ12 = targetWorldXQ12;
  EVar18.primaryWorldXQ12 = WVar15.worldXQ12;
  EVar18.primaryWorldYQ12 = WVar15.worldYQ12;
  EVar18.fallbackWorldYQ12 = targetWorldYQ12;
  EVar18.carry = false;
  return EVar18;
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
  dword dVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  GridScratchCell *scratchCell1;
  dword rowStrideBytes;
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
    return true;
  }
  rowStrideBytes = g_GridScratchWidth * 8;
  GridReachability_MarkOpenRegionRecursive
            (rowStrideBytes,g_GridScratchPrimary + iVar4 * g_GridScratchWidth + iVar3);
  GridFootprint_ClearTraversalFlagsAroundWorldPoint(radiusMetric,worldYQ12,worldXQ12);
  scratchCell1 = g_GridScratchPrimary + dVar1 * 3;
  iVar3 = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  while (((((scratchCell1[dVar1].stateMask & 0x80000000) != 0 || (scratchCell1[dVar1].pathCost == 0)
           ) || ((scratchCell1[dVar1].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) == 0)) ||
         (((scratchCell1->pathCost != 0 && (scratchCell1[1].pathCost != 0)) &&
          ((scratchCell1[dVar1 - 1].pathCost != 0 &&
           (((scratchCell1[dVar1 + 1].pathCost != 0 && (scratchCell1[dVar1 * 2 + -1].pathCost != 0))
            && (scratchCell1[dVar1 * 2].pathCost != 0))))))))) {
    scratchCell1 = scratchCell1 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      return false;
    }
  }
  GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scratchCell1 + dVar1);
  scratchCell1 = g_GridScratchPrimary + dVar1 * 3;
  iVar3 = (g_GridScratchHeight - 8) * g_GridScratchWidth;
  do {
    if ((((scratchCell1[dVar1].stateMask & 0x80000000) == 0) && (scratchCell1[dVar1].pathCost != 0))
       && ((scratchCell1[dVar1].stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0)) {
      if (scratchCell1->pathCost == 0) {
        return true;
      }
      if (scratchCell1[1].pathCost == 0) {
        return true;
      }
      if (scratchCell1[dVar1 - 1].pathCost == 0) {
        return true;
      }
      if (scratchCell1[dVar1 + 1].pathCost == 0) {
        return true;
      }
      if (scratchCell1[dVar1 * 2 + -1].pathCost == 0) {
        return true;
      }
      if (scratchCell1[dVar1 * 2].pathCost == 0) {
        return true;
      }
    }
    scratchCell1 = scratchCell1 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
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
  WorldOwnerListNode100 *worldNode1;
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
  if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
    do {
      if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (*(int *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)worldNode1->runtimePayload + 0x278) != 1)) {
        lVar3 = (longlong)worldNode1->worldXQ12 * 0x1c6e9c;
        lVar4 = (longlong)worldNode1->worldYQ12 * -0x20c8cc;
        uVar11 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
        iVar7 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar11) +
                     0x800) >> 10;
        if (((-1 < iVar7) && (iVar10 = (int)(uVar11 * 2 + 0x800) >> 10, -1 < iVar10)) &&
           ((iVar7 < (int)g_GridScratchWidth && (iVar10 < (int)g_GridScratchHeight)))) {
          GridScratch_FloodFillConnectedCellsRegs
                    (0xf0000001,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + iVar10 * g_GridScratchWidth + iVar7);
        }
      }
      worldNode1 = worldNode1->nextNode;
    } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
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
      if (((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
          (*(int *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) != 0)) &&
         (*(int *)(*(int *)worldNode1->runtimePayload + 0x278) != 1)) {
        lVar3 = (longlong)worldNode1->worldXQ12 * 0x1c6e9c;
        lVar4 = (longlong)worldNode1->worldYQ12 * -0x20c8cc;
        uVar11 = (int)((ulonglong)lVar4 >> 0x20) << 0xb | (uint)lVar4 >> 0x15;
        iVar7 = (int)((((int)((ulonglong)lVar3 >> 0x20) << 0xc | (uint)lVar3 >> 0x14) - uVar11) +
                     0x800) >> 10;
        if ((((-1 < iVar7) && (iVar10 = (int)(uVar11 * 2 + 0x800) >> 10, -1 < iVar10)) &&
            (iVar7 < (int)g_GridScratchWidth)) && (iVar10 < (int)g_GridScratchHeight)) {
          GridScratch_FloodFillConnectedCellsRegs
                    (0x8e000001,g_GridScratchWidth << 3,
                     g_GridScratchPrimary + iVar10 * g_GridScratchWidth + iVar7);
        }
      }
      worldNode1 = worldNode1->nextNode;
    } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
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
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestRuntimePairReachabilityFromWorldPointCf
          (WorldPointXYQ12 *sourceWorldPoint,GridReachabilityRuntimePair8 *targetRuntimePair)

{
  bool bVar1;
  ModelDefinitionRuntimeSemanticView280 *dVar1;
  
  dVar1 = (ModelDefinitionRuntimeSemanticView280 *)
          (targetRuntimePair->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
  bVar1 = GridScratch_TestWorldPointReachabilityCf
                    (0x100 << ((byte)dVar1->gridClassification260 & 0x1f) |
                     0x1000000 << ((byte)dVar1->gridClassification264 & 0x1f),
                     sourceWorldPoint->worldYQ12,sourceWorldPoint->worldXQ12,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.y,
                     (targetRuntimePair->modelNodeRuntime->worldTransform).translation.x);
  return bVar1;
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
  GridScratchCell *memory;
  GridScratchCell **memory_00;
  dword *newScratchBuffer;
  dword *newSecondaryScratchBuffer;
  void *newAuxiliaryBuffer;
  dword bytes;
  bool allocationSizeOverflow;
  ArenaAllocEaxCf5 AVar1;
  ArenaFreeEaxCf5 AVar2;
  GridScratchAllocEaxCf5 GVar3;
  longlong scratchAllocationByteCountProduct;
  GridScratchCell *previousScratchBuffer;
  
  g_GridScratchWidth = fieldGrid->gridWidth * 4;
  g_GridScratchHeight = fieldGrid->gridHeight * 4;
  bytes = fieldGrid->gridWidth * 0x20 * g_GridScratchHeight;
  AVar1 = (*g_MemoryApi.alloc)(bytes);
  previousScratchBuffer = g_GridScratchPrimary;
  newScratchBuffer = (dword *)AVar1.eax;
  if (!AVar1.carry) {
    LOCK();
    UNLOCK();
    g_GridScratchPrimary = (GridScratchCell *)newScratchBuffer;
    (*g_MemoryApi.free)(previousScratchBuffer);
    AVar1 = (*g_MemoryApi.alloc)(bytes);
    memory = g_GridScratchSecondary;
    newSecondaryScratchBuffer = (dword *)AVar1.eax;
    newScratchBuffer = newSecondaryScratchBuffer;
    if (!AVar1.carry) {
      LOCK();
      UNLOCK();
      g_GridScratchSecondary = (GridScratchCell *)newSecondaryScratchBuffer;
      (*g_MemoryApi.free)(memory);
      AVar1 = (*g_MemoryApi.alloc)(0x180000);
      memory_00 = g_GridPathCostQueueBegin;
      newAuxiliaryBuffer = (void *)AVar1.eax;
      newScratchBuffer = newAuxiliaryBuffer;
      if (!AVar1.carry) {
        g_GridPathCostQueueEnd = (GridScratchCell **)((int)newAuxiliaryBuffer + 0x180000);
        g_GridPathCostQueueBegin = newAuxiliaryBuffer;
        AVar2 = (*g_MemoryApi.free)(memory_00);
        return (GridScratchAllocEaxCf5)((uint5)AVar2 & 0xffffffff);
      }
    }
  }
  GVar3.carry = true;
  GVar3.eax = (dword)newScratchBuffer;
  return GVar3;
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
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestProjectedCellMaskBandsCf
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
  sdword sVar1;
  int iVar2;
  GameEntityRuntime *pGVar3;
  void *pvVar4;
  void *pvVar5;
  sdword sVar6;
  EntityPathingPriorityPair *heapBase;
  DepthBinMask32 secondMaskHigh;
  DepthBinMask32 secondMaskLow;
  int iVar7;
  DepthIntervalRadius32 intervalRadius;
  int iVar8;
  dword dVar9;
  dword dVar10;
  uint heapSize;
  WorldOwnerListNode100 *worldNode1;
  int iVar11;
  EntityPathingPriorityPair *priorityPair1;
  EntityPathingPriorityPair *priorityPair2;
  bool bVar12;
  WorldPositionXYEaxEdx8 WVar13;
  WorldPositionXYEaxEdx8 WVar14;
  ModelRuntimeNode *modelNode1;
  
  WVar13.worldYQ12 = targetWorldY;
  WVar13.worldXQ12 = targetWorldX;
  GridScratch_CopyPrimaryToSecondary();
  GridScratch_SwapPrimarySecondary();
  modelNode1 = (routeEntityRuntime->common).ownership.modelNode;
  worldNode1 = worldRuntime->ownerListHead;
  iVar8 = (modelNode1->worldTransform).translation.x;
  iVar2 = (modelNode1->worldTransform).translation.y;
  iVar7 = iVar8 - targetWorldX;
  if (iVar7 < 0) {
    iVar7 = -iVar7;
  }
  iVar11 = iVar2 - targetWorldY;
  if (iVar11 < 0) {
    iVar11 = -iVar11;
  }
  if (iVar7 < iVar11) {
    iVar7 = iVar11;
  }
  iVar7 = iVar7 + *(int *)((int)(routeEntityRuntime->common).ownership.definitionOrClassRecord +
                          0xdc);
  secondMaskHigh = DepthInterval_BuildBinMask(iVar7,(int)(iVar8 + targetWorldX) >> 1);
  secondMaskLow = DepthInterval_BuildBinMask(iVar7,(int)(iVar2 + targetWorldY) >> 1);
  iVar8 = 0x20;
  g_EntityPathingPriorityPairCount = 0;
  priorityPair2 = g_EntityPathingPriorityPairs;
  do {
    if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      pGVar3 = worldNode1->runtimePayload;
      pvVar4 = (pGVar3->common).ownership.definitionOrClassRecord;
      bVar12 = DepthBinMasks_OverlapCf
                         (worldNode1->modelDepthBinMaskFar,worldNode1->modelDepthBinMaskNear,
                          secondMaskLow,secondMaskHigh);
      if ((bVar12) && (*(int *)((int)pvVar4 + 0x18) != 0)) {
        priorityPair2->entity = pGVar3;
        priorityPair2->priority = 0;
        g_EntityPathingPriorityPairCount = g_EntityPathingPriorityPairCount + 1;
        priorityPair2 = priorityPair2 + 1;
        iVar8 = iVar8 + -1;
        if (iVar8 == 0) break;
      }
    }
    worldNode1 = worldNode1->nextNode;
  } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
  if (1 < g_EntityPathingPriorityPairCount) {
    iVar8 = *(int *)((int)(routeEntityRuntime->common).ownership.runtimeLink + 0xc);
    dVar9 = g_EntityPathingPriorityPairCount;
    priorityPair2 = g_EntityPathingPriorityPairs;
    do {
      pvVar4 = (priorityPair2->entity->common).ownership.runtimeLink;
      pvVar5 = (priorityPair2->entity->common).ownership.definitionOrClassRecord;
      if ((((*(uint *)((int)pvVar4 + 0x18) & 2) == 0) &&
          (priorityPair2->priority = priorityPair2->priority + 1,
          iVar8 == *(int *)((int)pvVar4 + 0xc))) &&
         (priorityPair2->priority = priorityPair2->priority + 1,
         (*(uint *)((int)pvVar4 + 0x18) & 1) == 0)) {
        priorityPair2->priority = priorityPair2->priority + *(int *)((int)pvVar5 + 0xc);
      }
      heapBase = g_EntityPathingPriorityPairs;
      priorityPair2 = priorityPair2 + 1;
      dVar9 = dVar9 - 1;
    } while (dVar9 != 0);
    heapSize = 0;
    dVar9 = g_EntityPathingPriorityPairCount;
    priorityPair2 = g_EntityPathingPriorityPairs;
    do {
      heapSize = heapSize + 1;
      PriorityPairHeap_SiftUp(heapSize,heapBase);
      priorityPair2 = priorityPair2 + 1;
      dVar9 = dVar9 - 1;
    } while (dVar9 != 0);
    do {
      sVar6 = heapBase->priority;
      LOCK();
      pGVar3 = priorityPair2[-1].entity;
      priorityPair2[-1].entity = heapBase->entity;
      UNLOCK();
      LOCK();
      sVar1 = priorityPair2[-1].priority;
      priorityPair2[-1].priority = sVar6;
      UNLOCK();
      heapBase->entity = pGVar3;
      heapBase->priority = sVar1;
      heapSize = heapSize - 1;
      PriorityPairHeap_SiftDown(heapSize,heapBase);
      priorityPair2 = priorityPair2 + -1;
      dVar9 = g_EntityPathingPriorityPairCount;
      priorityPair1 = g_EntityPathingPriorityPairs;
    } while (1 < heapSize);
    do {
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceAdd
        [*(int *)((int)(priorityPair1->entity->common).ownership.definitionOrClassRecord + 0x4c)])
                (priorityPair1->entity);
      dVar9 = dVar9 - 1;
      dVar10 = g_EntityPathingPriorityPairCount;
      priorityPair2 = g_EntityPathingPriorityPairs;
      priorityPair1 = priorityPair1 + 1;
    } while (dVar9 != 0);
    do {
      targetWorldY = WVar13.worldYQ12;
      targetWorldX = WVar13.worldXQ12;
      pGVar3 = priorityPair2->entity;
      if (priorityPair2->priority == 1) {
        modelNode1 = (pGVar3->common).ownership.modelNode;
        GridInfluence_SetLowDistanceBandsAroundWorldPoint
                  (((pGVar3->common).ownership.definitionOrClassRecord)->
                   placementRadiusOrClearanceDC,(modelNode1->worldTransform).translation.y,
                   (modelNode1->worldTransform).translation.x);
      }
      else if (pGVar3 == routeEntityRuntime) {
        WVar13 = EntityPathing_UpdateRouteSegment
                           (targetWorldY,targetWorldX,routeEntityRuntime,
                            (EntityPathingRouteEntityRuntimeView10 *)pGVar3);
      }
      else {
        EntityPathing_UpdateRouteSegment
                  (0,0,routeEntityRuntime,(EntityPathingRouteEntityRuntimeView10 *)pGVar3);
      }
      dVar10 = dVar10 - 1;
      priorityPair2 = priorityPair2 + 1;
    } while (dVar10 != 0);
  }
  targetWorldY = WVar13.worldYQ12;
  targetWorldX = WVar13.worldXQ12;
  GridScratch_SwapPrimarySecondary();
  WVar14.worldYQ12 = targetWorldY;
  WVar14.worldXQ12 = targetWorldX;
  return WVar14;
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
  longlong lVar1;
  int iVar2;
  uint currentY;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint currentX;
  int iVar6;
  GridScratchCell *scratchCell2;
  GridScratchCell *scratchCell1;
  GridScratchCell *pGVar7;
  
  iVar2 = radiusWorldUnits + g_GridInfluenceRadiusOffset6 + 499;
  g_GridInfluenceSquaredThreshold6 = iVar2 * iVar2;
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar2 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10;
  if ((((-1 < iVar2) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 10, -1 < iVar4)) &&
      (iVar2 < (int)g_GridScratchWidth)) && (iVar4 < (int)g_GridScratchHeight)) {
    scratchCell2 = g_GridScratchPrimary + g_GridScratchWidth * iVar4 + iVar2;
    iVar4 = iVar4 * 0x400 + -0x600;
    lVar1 = (longlong)(iVar4 + (iVar2 * 0x400 + -0x600) * 2) * 0x901;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd;
    lVar1 = (longlong)iVar4 * -1999;
    currentX = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    uVar3 = uVar5;
    pGVar7 = scratchCell2;
    if ((scratchCell2->stateMask & 0x80000000) == 0) {
      while ((iVar2 = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,currentX,uVar3,&pGVar7->stateMask),
             currentY = uVar5, scratchCell1 = scratchCell2, iVar2 != 0 &&
             (iVar2 = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,currentX,uVar3,&pGVar7->stateMask), iVar2 != 0)
             )) {
        pGVar7 = pGVar7 + -1;
        uVar3 = uVar3 - 0x240;
      }
      while ((iVar2 = GridFootprint_ClearTraversalFlagsDiagonalPositive
                                (worldYQ12,worldXQ12,currentX,currentY,&scratchCell1->stateMask),
             iVar2 != 0 &&
             (iVar2 = GridFootprint_ClearTraversalFlagsDiagonalNegative
                                (worldYQ12,worldXQ12,currentX,currentY,&scratchCell1->stateMask),
             iVar2 != 0))) {
        scratchCell1 = scratchCell1 + 1;
        currentY = currentY + 0x240;
      }
      iVar4 = uVar5 - 0x120;
      pGVar7 = scratchCell2 + -g_GridScratchWidth;
      iVar2 = uVar5 + 0x120;
      scratchCell1 = pGVar7;
      while (iVar6 = GridFootprint_ClearTraversalFlagsDiagonalPositive
                               (worldYQ12,worldXQ12,currentX + 499,iVar4,&scratchCell1->stateMask),
            iVar6 != 0) {
        scratchCell1 = scratchCell1 + -1;
        iVar4 = iVar4 + -0x240;
      }
      while( true ) {
        pGVar7 = pGVar7 + 1;
        iVar4 = GridFootprint_ClearTraversalFlagsDiagonalPositive
                          (worldYQ12,worldXQ12,currentX + 499,iVar2,&pGVar7->stateMask);
        if (iVar4 == 0) break;
        iVar2 = iVar2 + 0x240;
      }
      iVar4 = uVar5 + 0x120;
      scratchCell2 = scratchCell2 + g_GridScratchWidth;
      iVar2 = uVar5 - 0x120;
      pGVar7 = scratchCell2;
      while (iVar6 = GridFootprint_ClearTraversalFlagsDiagonalNegative
                               (worldYQ12,worldXQ12,currentX - 500,iVar4,&pGVar7->stateMask),
            iVar6 != 0) {
        pGVar7 = pGVar7 + 1;
        iVar4 = iVar4 + 0x240;
      }
      while( true ) {
        scratchCell2 = scratchCell2 + -1;
        iVar4 = GridFootprint_ClearTraversalFlagsDiagonalNegative
                          (worldYQ12,worldXQ12,currentX - 500,iVar2,&scratchCell2->stateMask);
        if (iVar4 == 0) break;
        iVar2 = iVar2 + -0x240;
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
  ArmyMovementRuntime *pAVar1;
  longlong lVar2;
  longlong lVar3;
  UQ12 segmentWorldXQ12;
  byte bVar4;
  int iVar5;
  int iVar6;
  dword dVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  dword dVar11;
  UQ12 UVar12;
  bool bVar13;
  WorldPositionXYEaxEdx8 WVar14;
  GridPathNearestCellEaxEbxCf9 GVar15;
  GraphicsWorldCoordinateQ12 GVar16;
  UQ12 segmentWorldYQ12;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = routeEntityRuntime->modelNode;
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.gridInfluenceRemove
    [routeEntityRuntime->modelDefinition->runtimeClassId4C])
            ((GameEntityRuntime *)routeEntityRuntime);
  lVar2 = (longlong)(modelNode1->worldTransform).translation.x * 0x1c6e9c;
  lVar3 = (longlong)(modelNode1->worldTransform).translation.y * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
  iVar5 = (int)((((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8) + 0x800) >>
          10;
  iVar9 = (int)(uVar8 * 2 + 0x800) >> 10;
  if (iVar5 < 1) {
    iVar5 = 1;
  }
  if (iVar9 < 1) {
    iVar9 = 1;
  }
  dVar7 = iVar5 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar5 + 2U)) {
    dVar7 = g_GridScratchWidth;
  }
  dVar11 = iVar9 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar9 + 2U)) {
    dVar11 = g_GridScratchHeight;
  }
  iVar5 = dVar7 - 2;
  iVar9 = dVar11 - 2;
  pAVar1 = routeEntityRuntime->movementRuntime;
  if ((GameEntityRuntime *)routeEntityRuntime != sourceRouteEntityRuntime) {
    targetWorldXQ12 = pAVar1->movementWorldXQ12;
    targetWorldYQ12 = pAVar1->movementWorldYQ12;
  }
  uVar8 = (int)((ulonglong)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)(int)targetWorldYQ12 * -0x20c8cc) >> 0x15;
  iVar6 = (int)((((int)((ulonglong)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)(int)targetWorldXQ12 * 0x1c6e9c) >> 0x14) - uVar8) + 0x800) >> 10;
  iVar10 = (int)(uVar8 * 2 + 0x800) >> 10;
  if (iVar6 < 1) {
    iVar6 = 1;
  }
  if (iVar10 < 1) {
    iVar10 = 1;
  }
  dVar7 = iVar6 + 2U;
  if ((int)g_GridScratchWidth < (int)(iVar6 + 2U)) {
    dVar7 = g_GridScratchWidth;
  }
  dVar11 = iVar10 + 2U;
  if ((int)g_GridScratchHeight < (int)(iVar10 + 2U)) {
    dVar11 = g_GridScratchHeight;
  }
  g_GridPathEntityClassMask = 1 << ((byte)pAVar1->factionIndex & 0x1f);
  bVar4 = (byte)routeEntityRuntime->modelDefinition->gridClassification260;
  g_GridPathHighCostMask = 0x10000 << (bVar4 & 0x1f);
  g_GridPathBlockingMask =
       0x100 << (bVar4 & 0x1f) |
       0x1000000 << ((byte)routeEntityRuntime->modelDefinition->gridClassification264 & 0x1f);
  GVar15 = GridPathCost_FindNearestUnblockedCell(iVar9,iVar5);
  iVar6 = GVar15.selectedColumn;
  iVar10 = GVar15.selectedRow;
  if ((GVar15.carry) ||
     (bVar13 = GridPathLine_TestHexSegmentClearCf
                         (0,iVar9,iVar5,g_GridScratchPrimary + iVar9 * g_GridScratchWidth + iVar5,
                          g_GridScratchPrimary + (dVar11 - 2) * g_GridScratchWidth + (dVar7 - 2)),
     iVar6 = dVar7 - 2, iVar10 = dVar11 - 2, bVar13)) {
    iVar5 = iVar10 * 0x400 + -0x600;
    lVar2 = (longlong)(iVar5 + (iVar6 * 0x400 + -0x600) * 2) * 0x901;
    targetWorldXQ12 = (int)((ulonglong)lVar2 >> 0x20) << 0x13 | (uint)lVar2 >> 0xd;
    lVar2 = (longlong)iVar5 * -1999;
    targetWorldYQ12 = (int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc;
  }
  dVar7 = routeEntityRuntime->modelDefinition->placementRadiusOrClearanceDC;
  GVar16 = (routeEntityRuntime->modelNode->worldTransform).translation.x;
  UVar12 = (routeEntityRuntime->modelNode->worldTransform).translation.y;
  uVar8 = 1;
  segmentWorldYQ12 = targetWorldYQ12;
  segmentWorldXQ12 = targetWorldXQ12;
  do {
    while( true ) {
      iVar5 = segmentWorldXQ12 - GVar16;
      if (iVar5 < 0) {
        iVar5 = -iVar5;
      }
      iVar9 = segmentWorldYQ12 - UVar12;
      if (iVar9 < 0) {
        iVar9 = -iVar9;
      }
      if ((0x3f < uVar8) || ((iVar5 < 0x241 && (iVar9 < 0x241)))) break;
      segmentWorldXQ12 = (int)(segmentWorldXQ12 + GVar16) >> 1;
      UVar12 = (int)(segmentWorldYQ12 + UVar12) >> 1;
      uVar8 = uVar8 + 1;
      segmentWorldYQ12 = UVar12;
    }
    GVar16 = 0x536c13;
    GridInfluence_SetLowDistanceBandsAroundWorldPoint(dVar7,segmentWorldYQ12,segmentWorldXQ12);
    uVar8 = uVar8 - 1;
  } while (uVar8 != 0);
  pAVar1 = routeEntityRuntime->movementRuntime;
  if (((GameEntityRuntime *)routeEntityRuntime != sourceRouteEntityRuntime) &&
     ((targetWorldXQ12 != pAVar1->movementWorldXQ12 ||
      (targetWorldYQ12 != pAVar1->movementWorldYQ12)))) {
    ArmyRuntime_SetPendingMoveTarget(targetWorldYQ12,targetWorldXQ12,pAVar1);
  }
  WVar14.worldYQ12 = targetWorldYQ12;
  WVar14.worldXQ12 = targetWorldXQ12;
  return WVar14;
}


/* Address: 0x00533D60.
   Ownership: world/pathing/grid.
   Purpose: Handles grid scratch test world point reachability carry-flag result.
   Local calls: GridScratch_TestConnectedReachabilityRecursiveCfRegs.
*/
bool __thandor_cf_preserve_eax_ecx_edx
GridScratch_TestWorldPointReachabilityCf
          (uint traversalMask,GraphicsWorldCoordinateQ12 sourceWorldXQ12,
          GraphicsWorldCoordinateQ12 sourceWorldYQ12,GraphicsWorldCoordinateQ12 targetWorldXQ12,
          GraphicsWorldCoordinateQ12 targetWorldYQ12)

{
  GridScratchCell *targetCell;
  dword dVar1;
  int iVar2;
  uint uVar3;
  GridScratchCell *currentCell;
  GridScratchCell *scratchCell1;
  bool bVar4;
  
  dVar1 = g_GridScratchWidth;
  uVar3 = (int)((ulonglong)((longlong)sourceWorldXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)sourceWorldXQ12 * -0x20c8cc) >> 0x15;
  targetCell = g_GridScratchPrimary +
               ((int)(uVar3 * 2 + 0x800) >> 10) * g_GridScratchWidth +
               ((int)((((int)((ulonglong)((longlong)sourceWorldYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                       (uint)((longlong)sourceWorldYQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 10
               );
  uVar3 = (int)((ulonglong)((longlong)targetWorldXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)targetWorldXQ12 * -0x20c8cc) >> 0x15;
  currentCell = g_GridScratchPrimary +
                ((int)(uVar3 * 2 + 0x800) >> 10) * g_GridScratchWidth +
                ((int)((((int)((ulonglong)((longlong)targetWorldYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                        (uint)((longlong)targetWorldYQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >>
                10);
  iVar2 = g_GridScratchHeight * g_GridScratchWidth;
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
    iVar2 = iVar2 + -0x10;
  } while (iVar2 != 0);
  bVar4 = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                    (traversalMask | 0x80000001,dVar1 << 3,&currentCell->stateMask,
                     &targetCell->stateMask);
  return bVar4;
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
  dword dVar1;
  GridScratchCell *scanCell2;
  uint bestNeighborCost1;
  uint uVar2;
  GridScratchCell *scanCell1;
  GridScratchCell *endCell;
  bool bVar3;
  GridPathBacktrackEaxEbxEcxCf13 GVar4;
  GridPathBacktrackEaxEbxEcxCf13 GVar5;
  
  dVar1 = g_GridScratchWidth;
  GVar5.selectedRow = g_GridScratchWidth * 8;
  endCell = startCell;
  do {
    scanCell2 = endCell;
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
    if (endCell == (GridScratchCell *)0x0)
    goto GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn;
    bVar3 = GridPathLine_TestHexSegmentClearCf
                      (callerBlockingMask,targetRow,targetColumn,startCell,endCell);
  } while (!bVar3);
  if (scanCell2 == startCell) {
    scanCell2 = endCell;
  }
GridPathCost_BacktrackBestHexRoute_ReturnTerminalCellOrColumn:
  if (scanCell2->pathCost != 0) {
    bestNeighborCost1 = (uint)((int)scanCell2 - (int)g_GridScratchPrimary) >> 3;
    GVar4.selectedRow = bestNeighborCost1 / g_GridScratchWidth;
    GVar4.selectedColumn = bestNeighborCost1 % g_GridScratchWidth;
    GVar4.routeStateMask = callerBlockingMask;
    GVar4.carry = false;
    return GVar4;
  }
  GVar5.selectedColumn = (FieldGridCellCoordinate)scanCell2;
  GVar5.carry = true;
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
  ulonglong uVar1;
  int rowBaseIndex;
  GridPathMarkedRegionCellRegisterResult GVar2;
  GridPathUnreachableRecursiveEdiEdx8 GVar3;
  
  g_GridPathUnreachableRegionReferenceColumn = referenceColumn;
  g_GridPathUnreachableRegionReferenceRow = referenceRow;
  rowBaseIndex = row * g_GridScratchWidth;
  GVar3 = GridPathRegion_MarkUnreachableRecursive
                    (g_GridScratchWidth << 3,g_GridScratchPrimary + rowBaseIndex + column,0x7fffffff
                     ,(rowBaseIndex + column) * 8);
  uVar1 = (ulonglong)GVar3 >> 3 & 0x1fffffff;
  GVar2.selectedRow = (FieldGridCellCoordinate)(uVar1 / g_GridScratchWidth);
  GVar2.selectedColumn = (FieldGridCellCoordinate)(uVar1 % (ulonglong)g_GridScratchWidth);
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
  GridScratchCell *pGVar1;
  GridScratchCell *currentCell_00;
  
  if ((currentCell->stateMask & 0x80000001) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    pGVar1 = currentCell;
    while (pGVar1 = pGVar1 + -1, (pGVar1->stateMask & traversalMask) == 0) {
      pGVar1->stateMask = pGVar1->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    while (currentCell = currentCell + 1, (currentCell->stateMask & traversalMask) == 0) {
      currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    }
    currentCell_00 = (GridScratchCell *)((int)&pGVar1->stateMask + rowStrideBytes);
    pGVar1 = (GridScratchCell *)((int)pGVar1 + (8 - rowStrideBytes));
    do {
      if ((pGVar1->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,pGVar1);
      }
      pGVar1 = pGVar1 + 1;
    } while (pGVar1 <= (GridScratchCell *)((int)currentCell - rowStrideBytes));
    do {
      if ((currentCell_00->stateMask & traversalMask) == 0) {
        GridScratch_FloodFillConnectedCellsRegs(traversalMask,rowStrideBytes,currentCell_00);
      }
      currentCell_00 = currentCell_00 + 1;
    } while (currentCell_00 <
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
  dword *pdVar1;
  uint *puVar2;
  uint *puVar3;
  dword *pdVar4;
  bool bVar5;
  
  *currentCell = *currentCell | 1;
  puVar3 = currentCell;
  if (targetCell == currentCell) {
    return false;
  }
  while( true ) {
    puVar2 = puVar3 + -2;
    if (targetCell == puVar2) {
      return false;
    }
    if ((*puVar2 & traversalMask) != 0) break;
    *puVar2 = *puVar2 | 1;
    puVar3 = puVar2;
  }
  while( true ) {
    currentCell = currentCell + 2;
    if (targetCell == currentCell) {
      return false;
    }
    if ((*currentCell & traversalMask) != 0) break;
    *currentCell = *currentCell | 1;
  }
  if (targetCell <= puVar2) {
    pdVar1 = (dword *)(rowStrideBytes + (int)puVar2);
    pdVar4 = (dword *)((int)puVar2 + (8 - rowStrideBytes));
    while (((*pdVar4 & traversalMask) != 0 ||
           (bVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                              (traversalMask,rowStrideBytes,pdVar4,targetCell), bVar5))) {
      pdVar4 = pdVar4 + 2;
      if ((dword *)((int)currentCell - rowStrideBytes) < pdVar4) {
        while (((*pdVar1 & traversalMask) != 0 ||
               (bVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                                  (traversalMask,rowStrideBytes,pdVar1,targetCell), bVar5))) {
          pdVar1 = pdVar1 + 2;
          if ((dword *)((int)((int)currentCell - rowStrideBytes) + rowStrideBytes * 2) <= pdVar1) {
            return true;
          }
        }
        return false;
      }
    }
    return false;
  }
  pdVar1 = (dword *)((int)puVar3 - rowStrideBytes);
  pdVar4 = (dword *)((int)puVar2 + rowStrideBytes);
  while (((*pdVar4 & traversalMask) != 0 ||
         (bVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                            (traversalMask,rowStrideBytes,pdVar4,targetCell), bVar5))) {
    pdVar4 = pdVar4 + 2;
    if ((dword *)((int)currentCell + rowStrideBytes) <= pdVar4) {
      while (((*pdVar1 & traversalMask) != 0 ||
             (bVar5 = GridScratch_TestConnectedReachabilityRecursiveCfRegs
                                (traversalMask,rowStrideBytes,pdVar1,targetCell), bVar5))) {
        pdVar1 = pdVar1 + 2;
        if ((dword *)((int)((int)currentCell + rowStrideBytes) + rowStrideBytes * -2) < pdVar1) {
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
GridPathUnreachableRecursiveEdiEdx8 __thandor_eax_edx_cf_preserve_ecx
GridPathRegion_MarkUnreachableRecursive
          (dword rowStrideBytes,GridScratchCell *currentCell,GridPathCost bestCost,
          dword bestCellByteOffset)

{
  GridScratchStateMask GVar1;
  uint uVar2;
  GridPathCost GVar3;
  GridScratchCell *pGVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  GridPathCost GVar8;
  GridScratchCell *pGVar9;
  GridScratchCell *pGVar10;
  GridPathUnreachableRecursiveEdiEdx8 GVar11;
  GridPathCost local_14;
  
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  pGVar10 = currentCell + 1;
  pGVar4 = currentCell + -1;
  do {
    pGVar9 = pGVar4;
    GVar1 = pGVar9->stateMask;
    if ((pGVar9->pathCost < 0x7fffffff) || ((int)GVar1 < 0))
    goto GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary;
    pGVar9->stateMask = pGVar9->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    pGVar4 = pGVar9 + -1;
  } while (((g_GridPathEntityClassMask & GVar1) == 0) || ((g_GridPathBlockingMask & GVar1) == 0));
  pGVar9->stateMask = pGVar9->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
GridPathRegion_MarkUnreachableRecursive_ScanRightBoundary:
  do {
    pGVar4 = pGVar10;
    GVar1 = pGVar4->stateMask;
    if ((pGVar4->pathCost < 0x7fffffff) || ((int)GVar1 < 0))
    goto GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows;
    pGVar4->stateMask = pGVar4->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
    pGVar10 = pGVar4 + 1;
  } while (((g_GridPathEntityClassMask & GVar1) == 0) || ((g_GridPathBlockingMask & GVar1) == 0));
  pGVar4->stateMask = pGVar4->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
GridPathRegion_MarkUnreachableRecursive_RecurseAcrossAdjacentRows:
  uVar2 = (int)pGVar9 + (8 - (int)g_GridScratchPrimary);
  iVar5 = ((uint)((int)pGVar4 - (int)pGVar9) >> 3) - 2;
  uVar6 = (uVar2 >> 3) % g_GridScratchWidth;
  GVar3 = (uVar2 >> 3) / g_GridScratchWidth - g_GridPathUnreachableRegionReferenceRow;
  if ((int)GVar3 < 0) {
    GVar3 = -GVar3;
    GVar8 = uVar6 - g_GridPathUnreachableRegionReferenceColumn;
    if ((int)GVar8 < 0) {
      iVar7 = GVar8 + iVar5;
      if (iVar7 < 0) {
        GVar3 = GVar3 - iVar7;
        uVar2 = uVar2 + iVar5 * 8;
      }
      else {
        uVar2 = uVar2 + (iVar7 - iVar5) * -8;
      }
    }
    else if ((int)GVar3 < (int)GVar8) {
      GVar3 = GVar8;
    }
  }
  else {
    iVar7 = uVar6 - g_GridPathUnreachableRegionReferenceColumn;
    if (iVar7 < 0) {
      iVar7 = iVar7 + iVar5;
      if (iVar7 < 0) {
        uVar2 = uVar2 + iVar5 * 8;
        if ((int)GVar3 < -iVar7) {
          GVar3 = -iVar7;
        }
      }
      else {
        uVar2 = uVar2 + (iVar7 - iVar5) * -8;
      }
    }
    else {
      GVar3 = GVar3 + iVar7;
    }
  }
  local_14 = bestCost;
  if ((int)GVar3 < (int)bestCost) {
    local_14 = GVar3;
    bestCellByteOffset = uVar2;
  }
  GVar11.bestCost = local_14;
  GVar11.bestCellByteOffset = bestCellByteOffset;
  pGVar10 = (GridScratchCell *)((int)pGVar9 + (8 - rowStrideBytes));
  do {
    GVar1 = pGVar10->stateMask;
    if (((0x7ffffffe < pGVar10->pathCost) && ((GVar1 & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & GVar1) == 0 || ((g_GridPathBlockingMask & GVar1) == 0)))) {
      GVar11 = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,pGVar10,GVar11.bestCost,GVar11.bestCellByteOffset);
    }
    pGVar10 = pGVar10 + 1;
  } while (pGVar10 <= (GridScratchCell *)((int)pGVar4 - rowStrideBytes));
  pGVar10 = (GridScratchCell *)((int)&pGVar9->stateMask + rowStrideBytes);
  do {
    GVar1 = pGVar10->stateMask;
    if (((0x7ffffffe < pGVar10->pathCost) && ((GVar1 & 0x80000001) == 0)) &&
       (((g_GridPathEntityClassMask & GVar1) == 0 || ((g_GridPathBlockingMask & GVar1) == 0)))) {
      GVar11 = GridPathRegion_MarkUnreachableRecursive
                         (rowStrideBytes,pGVar10,GVar11.bestCost,GVar11.bestCellByteOffset);
    }
    pGVar10 = pGVar10 + 1;
  } while (pGVar10 < (GridScratchCell *)
                     ((int)&((GridScratchCell *)((int)pGVar4 - rowStrideBytes))->stateMask +
                     rowStrideBytes * 2));
  return GVar11;
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
  int iVar1;
  int squaredXDistanceMetric;
  int iVar2;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  iVar1 = 0;
  do {
    iVar2 = iVar1;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return iVar2;
    }
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    currentX = currentX + -999;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * 4 + -2;
    iVar1 = iVar2 + 1;
  } while ((*scratchRecord & 0x80000000) == 0);
  return iVar2;
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
  int iVar1;
  int squaredXDistanceMetric;
  int iVar2;
  
  squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
  iVar1 = 0;
  do {
    iVar2 = iVar1;
    if (g_GridInfluenceSquaredThreshold6 <
        (uint)(squaredXDistanceMetric + (currentY - centerY) * (currentY - centerY))) {
      return iVar2;
    }
    currentX = currentX + 999;
    *scratchRecord = *scratchRecord & 0x7ffffffe;
    scratchRecord[1] = scratchRecord[1] + 1;
    squaredXDistanceMetric = (currentX - centerX) * (currentX - centerX);
    scratchRecord = scratchRecord + g_GridScratchWidth * -4 + 2;
    iVar1 = iVar2 + 1;
  } while ((*scratchRecord & 0x80000000) == 0);
  return iVar2;
}


/* Address: 0x005363C0.
   Ownership: world/pathing/grid.
   Purpose: Recursively sets scratch bit 0 across a connected region whose cells pass the verified hard-boundary
   and influence-mask test. Expansion follows the six-neighbor staggered-grid topology.
*/
void __thandor_void_preserve_eax_ecx_edx
GridReachability_MarkOpenRegionRecursive(dword rowStrideBytes,GridScratchCell *currentCell)

{
  GridScratchCell *scanCell2;
  GridScratchCell *scanCell3;
  GridScratchCell *pGVar1;
  GridScratchCell *scanCell1;
  GridScratchCell *currentCell_00;
  
  currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  pGVar1 = currentCell;
  while (pGVar1 = pGVar1 + -1, (pGVar1->stateMask & 0xf0007f01) == 0) {
    pGVar1->stateMask = pGVar1->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  while (currentCell = currentCell + 1, (currentCell->stateMask & 0xf0007f01) == 0) {
    currentCell->stateMask = currentCell->stateMask | GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  currentCell_00 = (GridScratchCell *)((int)&pGVar1->stateMask + rowStrideBytes);
  scanCell3 = (GridScratchCell *)((int)currentCell - rowStrideBytes);
  scanCell1 = (GridScratchCell *)((int)pGVar1 + (8 - rowStrideBytes));
  do {
    if ((scanCell1->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,scanCell1);
    }
    scanCell1 = scanCell1 + 1;
  } while (scanCell1 <= scanCell3);
  do {
    if ((currentCell_00->stateMask & 0xf0007f01) == 0) {
      GridReachability_MarkOpenRegionRecursive(rowStrideBytes,currentCell_00);
    }
    currentCell_00 = currentCell_00 + 1;
  } while (currentCell_00 < (GridScratchCell *)((int)&scanCell3->stateMask + rowStrideBytes * 2));
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
  GridScratchCell *scanCell2;
  GridScratchCell *pGVar1;
  GridScratchCell *scanCell3;
  GridScratchCell *pGVar2;
  GridScratchCell *scanCell1;
  GridScratchCell *currentCell_00;
  
  currentCell->stateMask = currentCell->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  pGVar1 = currentCell + 1;
  while ((pGVar2 = currentCell + -1, (pGVar2->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 &&
         (currentCell[-1].pathCost != 0))) {
    pGVar2->stateMask = pGVar2->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
    currentCell = pGVar2;
  }
  for (; ((pGVar1->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0 && (pGVar1->pathCost != 0));
      pGVar1 = pGVar1 + 1) {
    pGVar1->stateMask = pGVar1->stateMask & ~GRID_SCRATCH_TRAVERSAL_VISITED;
  }
  currentCell_00 = (GridScratchCell *)((int)&pGVar2->stateMask + rowStrideBytes);
  scanCell3 = (GridScratchCell *)((int)pGVar1 - rowStrideBytes);
  scanCell1 = (GridScratchCell *)((int)pGVar2 + (8 - rowStrideBytes));
  do {
    if (((scanCell1->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) && (scanCell1->pathCost != 0)
       ) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,scanCell1);
    }
    scanCell1 = scanCell1 + 1;
  } while (scanCell1 <= scanCell3);
  do {
    if (((currentCell_00->stateMask & GRID_SCRATCH_TRAVERSAL_VISITED) != 0) &&
       (currentCell_00->pathCost != 0)) {
      GridReachability_ClearCostedRegionRecursive(rowStrideBytes,currentCell_00);
    }
    currentCell_00 = currentCell_00 + 1;
  } while (currentCell_00 < (GridScratchCell *)((int)&scanCell3->stateMask + rowStrideBytes * 2));
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
  int iVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  int iVar5;
  int iVar6;
  GridScratchStateMask GVar7;
  dword dVar8;
  int bestHexDistance1;
  undefined4 unaff_EBX;
  int searchRow1;
  int iVar9;
  GridScratchCell *scratchCell1;
  GridPathNearestCellEaxEbxCf9 GVar10;
  GridPathNearestCellEaxEbxCf9 GVar11;
  GridPathNearestCellEaxEbxCf9 GVar12;
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
    GVar10.carry = false;
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
        GVar11.carry = true;
        return GVar11;
      }
    }
  }
  GVar12.selectedRow = gridY;
  GVar12.selectedColumn = gridX;
  GVar12.carry = true;
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
bool __thandor_cf_preserve_eax_ecx_edx
GridPathLine_TestHexSegmentClearCf
          (FieldGridRegionMask callerBlockingMask,FieldGridCellCoordinate startRow,
          FieldGridCellCoordinate startColumn,GridScratchCell *startCell,GridScratchCell *endCell)

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
        return true;
      }
      if (scanCell2 == endCell) {
        return false;
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
            return true;
          }
          if ((g_GridPathEntityClassMask & GVar1) == 0) {
            return true;
          }
          if ((g_GridPathBlockingMask & GVar1) != 0) {
            return true;
          }
          if ((callerBlockingMask & GVar1) != 0) {
            return true;
          }
        }
        if (scanCell1 == scanCell3) break;
        scanCell1 = scanCell1 + -g_GridScratchWidth;
      }
      iVar3 = iVar3 + -1;
      if (iVar3 < 0) {
        return false;
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

