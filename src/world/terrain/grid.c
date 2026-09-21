/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/grid.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/grid.h>

/* Implementation ownership: world/terrain/grid. */

/* Address: 0x00505930.
   Ownership: world/terrain/grid.
   Purpose: Six 4-byte stack arguments, __stdcall RET 0x18. Clips a radial field-grid region, applies the per-cell
   height/material operation, then recomputes triangle normals and directional light. CF clear is the success path;
   CF set is the invalid/empty-region path.
   Local calls: FieldGrid_WorldToGridQ12, FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial,
   FieldGridCell_RecomputeTriangleNormalAngles, FieldGridCell_ComputeDirectionalLightColor.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  FieldGridCell *cell;
  FieldGridCell *cell_00;
  FieldGridCell *cell_01;
  FieldGridCoordinatesEaxEdx8 FVar8;
  FieldGridCoordinatesEaxEdx8 FVar9;
  FieldGridCell *pFVar10;
  FieldGridCell *pFVar11;
  
  if (0 < radiusWorldUnits) {
    uVar4 = (int)((ulonglong)((longlong)radiusWorldUnits * 0x1bb6) >> 0x20) << 0x14 |
            (uint)((longlong)radiusWorldUnits * 0x1bb6) >> 0xc;
    iVar6 = centerWorldXQ12 - uVar4;
    FVar8 = FieldGrid_WorldToGridQ12(centerWorldYQ12 + radiusWorldUnits,iVar6);
    FVar9 = FieldGrid_WorldToGridQ12
                      (centerWorldYQ12 + radiusWorldUnits + radiusWorldUnits * -2,iVar6 + uVar4 * 2)
    ;
    FVar1 = fieldGrid->gridWidth;
    iVar7 = FVar8.columnQ12 >> 0xc;
    iVar3 = FVar8.rowQ12 >> 0xc;
    iVar6 = (FVar9.columnQ12 >> 0xc) + 1;
    iVar5 = (FVar9.rowQ12 >> 0xc) + 1;
    if ((int)FVar1 <= iVar6) {
      iVar6 = FVar1 - 1;
    }
    if (iVar7 < 1) {
      iVar7 = 1;
    }
    if (iVar3 < 1) {
      iVar3 = 1;
    }
    if ((int)fieldGrid->gridHeight <= iVar5) {
      iVar5 = fieldGrid->gridHeight - 1;
    }
    iVar2 = iVar6 - iVar7;
    if ((iVar2 != 0 && iVar7 <= iVar6) && (iVar6 = iVar5 - iVar3, iVar6 != 0 && iVar3 <= iVar5)) {
      fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
      cell = (FieldGridCell *)(fieldGrid->cells[iVar7].runtime0C_3F + iVar3 * FVar1 * 0x80 + -0xc);
      iVar3 = iVar2;
      pFVar10 = cell;
      cell_00 = cell;
      iVar5 = iVar6;
      do {
        do {
          FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
                    (terrainMaterialIndexOrNegativeSentinel,radiusWorldUnits,
                     terrainHeightDeltaAmplitudeQ12,centerWorldYQ12,centerWorldXQ12,cell);
          cell = cell + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        cell = pFVar10 + FVar1;
        iVar6 = iVar6 + -1;
        iVar3 = iVar2;
        pFVar11 = cell_00;
        pFVar10 = cell;
        cell_01 = cell_00;
        iVar7 = iVar5;
      } while (iVar6 != 0);
      do {
        do {
          FieldGridCell_RecomputeTriangleNormalAngles(FVar1 * 0x80,cell_00);
          cell_00 = cell_00 + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
        cell_00 = pFVar11 + FVar1;
        iVar5 = iVar5 + -1;
        iVar2 = iVar3;
        pFVar10 = cell_01;
        pFVar11 = cell_00;
        iVar6 = iVar3;
      } while (iVar5 != 0);
      do {
        do {
          FieldGridCell_ComputeDirectionalLightColor(cell_01);
          cell_01 = cell_01 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        cell_01 = pFVar10 + FVar1;
        iVar7 = iVar7 + -1;
        iVar3 = iVar6;
        pFVar10 = cell_01;
      } while (iVar7 != 0);
      return;
    }
  }
  return;
}


/* Address: 0x00562330.
   Ownership: world/terrain/grid.
   Purpose: Runs the paired forward and reverse terrain relaxation sweeps for the requested pass count, selecting
   the sign-gated or ungated pair from flag bit zero.
   Local calls: TerrainGrid_RelaxNeighborHeightsForwardWithSignGate,
   TerrainGrid_RelaxNeighborHeightsReverseWithSignGate, TerrainGrid_RelaxNeighborHeightsForward,
   TerrainGrid_RelaxNeighborHeightsReverse.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RunDirectionalRelaxationPasses
          (FrontendPlayerRuntimeId playerRuntimeId,dword reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode)

{
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  if ((mode & TERRAIN_RELAXATION_UNGATED_LAND_TOOL) == TERRAIN_RELAXATION_SIGN_GATED) {
    do {
      TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(fieldGrid);
      passCount = passCount - 1;
    } while (passCount != 0);
  }
  else {
    do {
      TerrainGrid_RelaxNeighborHeightsForward(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverse(fieldGrid);
      passCount = passCount - 1;
    } while (passCount != 0);
  }
  return;
}


/* Address: 0x005610A0.
   Ownership: world/terrain/grid.
   Purpose: Applies positive per-cell deltas to the field grid and refreshes eligible neighboring cells through the
   established grid update helpers. EAX, ECX, and EDX are preserved or incidental caller state and are not
   synthetic parameters or normal returns.
   Local calls: FieldGridCell_RecomputeTriangleNormalAngles, FieldGridCell_ComputeDirectionalLightColor,
   FieldGrid_ProcessHorizontalSpan.
   Cross-module calls: SelectionPlayerPairList_ContainsPairCf [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyPositiveCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension FVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *pFVar6;
  FieldGridCell *pFVar7;
  SelectionPlayerPairRecord *pSVar8;
  int *piVar9;
  bool bVar10;
  int *accumulatorPlane;
  int rowStrideBytes_00;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar9 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = FVar1 * fieldGrid->gridHeight;
  rowStrideBytes = FVar1 * 0x80;
  pFVar7 = fieldGrid->cells;
  iVar5 = iVar4;
  pFVar6 = pFVar7;
  accumulatorPlane = piVar9;
  rowStrideBytes_00 = rowStrideBytes;
  do {
    iVar2 = *piVar9;
    if (iVar2 != 0) {
      pFVar6->terrainHeight = pFVar6->terrainHeight - iVar2;
      pFVar6->waterSurfaceDelta = pFVar6->waterSurfaceDelta + iVar2;
      if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
        FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        if (((pFVar6[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + -1);
        }
        if (((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + 1);
        }
        pFVar6 = pFVar6 + -FVar1;
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + 1);
        }
        pFVar6 = pFVar6 + FVar1 * 2 + -1;
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        cell = pFVar6 + 1;
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        pFVar6 = cell + -FVar1;
      }
    }
    *piVar9 = pFVar6->terrainHeight;
    pFVar6 = pFVar6 + 1;
    piVar9 = piVar9 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  bVar10 = SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  if (bVar10) {
    FieldGrid_ProcessHorizontalSpan
              (anchorWorldYQ12,anchorWorldXQ12,(int)packedDragDeltaXY16 >> 0x10,
               (int)(short)packedDragDeltaXY16,anchorWorldYQ12,anchorWorldXQ12,accumulatorPlane,
               fieldGrid);
    piVar9 = accumulatorPlane;
  }
  else {
    pSVar8 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    piVar9 = accumulatorPlane;
    for (dVar3 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084;
        dVar3 != 0; dVar3 = dVar3 - 1) {
      FieldGrid_ProcessHorizontalSpan
                (anchorWorldYQ12,anchorWorldXQ12,(int)packedDragDeltaXY16 >> 0x10,
                 (int)(short)packedDragDeltaXY16,pSVar8->pairValue,pSVar8->pairKey,accumulatorPlane,
                 fieldGrid);
      pSVar8 = pSVar8 + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    LOCK();
    iVar5 = *piVar9;
    *piVar9 = 0;
    UNLOCK();
    iVar5 = iVar5 - pFVar7->terrainHeight;
    if (iVar5 != 0) {
      pFVar7->terrainHeight = pFVar7->terrainHeight + iVar5;
      pFVar7->waterSurfaceDelta = pFVar7->waterSurfaceDelta - iVar5;
      *piVar9 = iVar5;
      if ((pFVar7->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
        FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        if (((pFVar7[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + -1);
        }
        if (((pFVar7[1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + 1);
        }
        pFVar7 = (FieldGridCell *)((int)pFVar7 - rowStrideBytes_00);
        if ((pFVar7->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        }
        if ((pFVar7[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + 1);
        }
        pFVar6 = (FieldGridCell *)(pFVar7[-1].runtime0C_3F + rowStrideBytes_00 * 2 + -0xc);
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        pFVar7 = pFVar6 + 1;
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        }
        pFVar7 = (FieldGridCell *)((int)pFVar7 - rowStrideBytes_00);
      }
    }
    pFVar7 = pFVar7 + 1;
    piVar9 = piVar9 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


/* Address: 0x005613C0.
   Ownership: world/terrain/grid.
   Purpose: Applies negative per-cell deltas to the field grid and refreshes eligible neighboring cells through the
   established grid update helpers. EAX, ECX, and EDX are preserved or incidental caller state and are not
   synthetic parameters or normal returns.
   Local calls: FieldGridCell_RecomputeTriangleNormalAngles, FieldGridCell_ComputeDirectionalLightColor,
   FieldGrid_ProcessVerticalSpan.
   Cross-module calls: SelectionPlayerPairList_ContainsPairCf [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyNegativeCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension FVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *pFVar6;
  FieldGridCell *pFVar7;
  SelectionPlayerPairRecord *pSVar8;
  int *piVar9;
  bool bVar10;
  int *accumulatorPlane;
  int rowStrideBytes_00;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar9 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = FVar1 * fieldGrid->gridHeight;
  rowStrideBytes = FVar1 * 0x80;
  pFVar7 = fieldGrid->cells;
  iVar5 = iVar4;
  pFVar6 = pFVar7;
  accumulatorPlane = piVar9;
  rowStrideBytes_00 = rowStrideBytes;
  do {
    iVar2 = *piVar9;
    if (iVar2 != 0) {
      pFVar6->terrainHeight = pFVar6->terrainHeight - iVar2;
      pFVar6->waterSurfaceDelta = pFVar6->waterSurfaceDelta + iVar2;
      *piVar9 = 0;
      if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
        FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        if (((pFVar6[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + -1);
        }
        if (((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + 1);
        }
        pFVar6 = pFVar6 + -FVar1;
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6 + 1);
        }
        pFVar6 = pFVar6 + FVar1 * 2 + -1;
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        cell = pFVar6 + 1;
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        pFVar6 = cell + -FVar1;
      }
    }
    pFVar6 = pFVar6 + 1;
    piVar9 = piVar9 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  bVar10 = SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  if (bVar10) {
    FieldGrid_ProcessVerticalSpan
              ((int)packedDragDeltaXY16 >> 0x10,(int)(short)packedDragDeltaXY16,anchorWorldYQ12,
               anchorWorldXQ12,accumulatorPlane,fieldGrid);
    piVar9 = accumulatorPlane;
  }
  else {
    pSVar8 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    piVar9 = accumulatorPlane;
    for (dVar3 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084;
        dVar3 != 0; dVar3 = dVar3 - 1) {
      FieldGrid_ProcessVerticalSpan
                ((int)packedDragDeltaXY16 >> 0x10,(int)(short)packedDragDeltaXY16,pSVar8->pairValue,
                 pSVar8->pairKey,accumulatorPlane,fieldGrid);
      pSVar8 = pSVar8 + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    iVar5 = *piVar9;
    if (iVar5 != 0) {
      pFVar7->terrainHeight = pFVar7->terrainHeight + iVar5;
      pFVar7->waterSurfaceDelta = pFVar7->waterSurfaceDelta - iVar5;
      if ((pFVar7->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
        FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        if (((pFVar7[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + -1);
        }
        if (((pFVar7[1].flagsAndMaterial & 0x88006000) == 0) && (piVar9[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + 1);
        }
        pFVar7 = (FieldGridCell *)((int)pFVar7 - rowStrideBytes_00);
        if ((pFVar7->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        }
        if ((pFVar7[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7 + 1);
        }
        pFVar6 = (FieldGridCell *)(pFVar7[-1].runtime0C_3F + rowStrideBytes_00 * 2 + -0xc);
        if ((pFVar6->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar6);
          FieldGridCell_ComputeDirectionalLightColor(pFVar6);
        }
        pFVar7 = pFVar6 + 1;
        if ((pFVar6[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar7);
          FieldGridCell_ComputeDirectionalLightColor(pFVar7);
        }
        pFVar7 = (FieldGridCell *)((int)pFVar7 - rowStrideBytes_00);
      }
    }
    pFVar7 = pFVar7 + 1;
    piVar9 = piVar9 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


/* Address: 0x00561C10.
   Ownership: world/terrain/grid.
   Purpose: Rebuilds one local field-grid influence state by walking the relevant cells and invoking the
   established grid propagation helpers. EAX, ECX, and EDX are preserved or incidental caller state and are not
   synthetic parameters or normal returns.
   Local calls: FieldGrid_ApplyRectangularTransition, FieldGridCell_RecomputeTriangleNormalAngles,
   FieldGridCell_ComputeDirectionalLightColor.
   Cross-module calls: SelectionPlayerPairList_ContainsPairCf [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  FieldGridAsset *fieldGrid;
  FieldGridDimension FVar2;
  dword dVar3;
  int iVar4;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *pFVar5;
  SelectionPlayerPairRecord *pSVar6;
  int *piVar7;
  bool bVar8;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  bVar8 = SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if (bVar8) {
    FieldGrid_ApplyRectangularTransition(worldYQ12,worldXQ12,fieldGrid);
  }
  else {
    pSVar6 = pSVar1->pairRecords80_807F;
    for (dVar3 = pSVar1->activePairCount8084; dVar3 != 0; dVar3 = dVar3 - 1) {
      FieldGrid_ApplyRectangularTransition(pSVar6->pairValue,pSVar6->pairKey,fieldGrid);
      pSVar6 = pSVar6 + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  piVar7 = pSVar1->terrainHeightScratchPlane8088;
  FVar2 = fieldGrid->gridWidth;
  iVar4 = FVar2 * fieldGrid->gridHeight;
  rowStrideBytes = FVar2 * 0x80;
  pFVar5 = fieldGrid->cells;
  do {
    if ((*piVar7 != pFVar5->terrainHeight) && ((pFVar5->flagsAndMaterial & 0x88006000) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
      FieldGridCell_ComputeDirectionalLightColor(pFVar5);
      if (((pFVar5[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar7[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + -1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5 + -1);
      }
      if (((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) && (piVar7[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + 1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
      }
      pFVar5 = pFVar5 + -FVar2;
      if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5);
      }
      if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5 + 1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
      }
      pFVar5 = pFVar5 + FVar2 * 2 + -1;
      if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar5);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5);
      }
      cell = pFVar5 + 1;
      if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
      }
      pFVar5 = cell + -FVar2;
    }
    pFVar5 = pFVar5 + 1;
    piVar7 = piVar7 + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}


/* Address: 0x00505620.
   Ownership: world/terrain/grid.
   Purpose: Marks the field dirty and recomputes both packed terrain-triangle normal angles for every non-boundary
   cell. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
   Local calls: FieldGridCell_RecomputeTriangleNormalAngles.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    FVar1 = fieldGrid->gridWidth;
    iVar3 = fieldGrid->gridHeight - 2;
    iVar2 = FVar1 - 2;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 FVar1 * 0x80 + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(FVar1 * 0x80,cell);
        iVar2 = iVar2 + -1;
        cellCursor = cell + 1;
      } while (iVar2 != 0);
      iVar2 = FVar1 - 2;
      iVar3 = iVar3 + -1;
      cellCursor = cell + 3;
    } while (iVar3 != 0);
  }
  return;
}


/* Address: 0x00505700.
   Ownership: world/terrain/grid.
   Purpose: Updates the shared Q28 light direction from two angles, marks the field dirty, and recomputes
   directional-light colors for every non-boundary cell. EAX, ECX, and EDX are preserved or incidental caller state
   and are not synthetic parameters or normal returns.
   Local calls: FieldGridCell_ComputeDirectionalLightColor.
   Cross-module calls: FixedMath_WriteDirectionQ28 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;
  
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_TerrainLightDirectionX,lightElevationAngle,lightAzimuthAngle);
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    FVar1 = fieldGrid->gridWidth;
    iVar3 = fieldGrid->gridHeight - 2;
    iVar2 = FVar1 - 2;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 FVar1 * 0x80 + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        iVar2 = iVar2 + -1;
        cellCursor = cell + 1;
      } while (iVar2 != 0);
      iVar2 = FVar1 - 2;
      iVar3 = iVar3 + -1;
      cellCursor = cell + 3;
    } while (iVar3 != 0);
  }
  return;
}


/* Address: 0x005090E0.
   Ownership: world/terrain/grid.
   Purpose: Typed parameters: p1 worldZQ12→Q12, p2 worldYQ12→Q12, p3 worldXQ12→Q12. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p0
   heightDeltaSourceValue→TerrainHeightBrushDeltaSource_V344. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: FieldGrid_WorldToGridQ12.
   Cross-module calls: TerrainHeightDelta_ApplyWedge0 [world/terrain/height], TerrainHeightDelta_ApplyWedge1
   [world/terrain/height], TerrainHeightDelta_ApplyWedge2 [world/terrain/height], TerrainHeightDelta_ApplyWedge3
   [world/terrain/height], TerrainHeightDelta_ApplyWedge4 [world/terrain/height], TerrainHeightDelta_ApplyWedge5
   [world/terrain/height].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  Q12 *pQVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  FieldGridCell *pFVar8;
  FieldGridCell *pFVar9;
  FieldGridCell *cell;
  FieldGridCoordinatesEaxEdx8 FVar10;
  uint uVar11;
  uint uVar12;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = heightDeltaSourceValue / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = worldZQ12;
    FVar10 = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar2 = FVar10.columnQ12 >> 0xc;
    uVar11 = FVar10.rowQ12 >> 0xc;
    uVar3 = (uint)((ulonglong)FVar10 & 0xfff00000fff);
    uVar6 = (uint)(((ulonglong)FVar10 & 0xfff00000fff) >> 0x20);
    uVar5 = uVar6 + uVar3 * 2;
    uVar12 = uVar2;
    if (uVar5 < 0x1000) {
      if (0xfff < uVar3 + uVar6 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    else if (uVar5 < 0x2001) {
      uVar12 = uVar2 + 1;
      if (uVar3 < uVar6) {
        uVar11 = uVar11 + 1;
        uVar12 = uVar2;
      }
    }
    else {
      uVar12 = uVar2 + 1;
      if (0x1fff < uVar3 + uVar6 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)uVar12) && (uVar5 = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)uVar11)) &&
        (uVar11 < fieldGrid->gridHeight)) &&
       (((uVar12 < uVar5 &&
         (iVar7 = uVar11 * uVar5 + uVar12,
         (fieldGrid->cells[iVar7].flagsAndMaterial & 0x88006000) == 0)) &&
        (fieldGrid->cells[iVar7].waterSurfaceDelta < 1)))) {
      iVar4 = g_TerrainScanReferenceHeight - fieldGrid->cells[iVar7].terrainHeight;
      pQVar1 = &fieldGrid->cells[iVar7].terrainHeight;
      *pQVar1 = *pQVar1 + iVar4;
      pQVar1 = &fieldGrid->cells[iVar7].waterSurfaceDelta;
      *pQVar1 = *pQVar1 - iVar4;
      iVar4 = g_TerrainScanRowStrideBytes;
      pFVar8 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar7 * 0x80 + -0x28);
      pFVar9 = (FieldGridCell *)((int)pFVar8 - g_TerrainScanRowStrideBytes);
      TerrainHeightDelta_ApplyWedge0(0,pFVar8);
      cell = pFVar9 + -1;
      TerrainHeightDelta_ApplyWedge1(0,pFVar9);
      pFVar8 = (FieldGridCell *)(cell[-1].runtime0C_3F + iVar4 + -0xc);
      TerrainHeightDelta_ApplyWedge2(0,cell);
      pFVar9 = (FieldGridCell *)(pFVar8->runtime0C_3F + iVar4 + -0xc);
      TerrainHeightDelta_ApplyWedge3(0,pFVar8);
      TerrainHeightDelta_ApplyWedge4(0,pFVar9);
      TerrainHeightDelta_ApplyWedge5(0,pFVar9 + 1);
      return;
    }
  }
  return;
}


/* Address: 0x005618A0.
   Ownership: world/terrain/grid.
   Purpose: Applies one compact local cell update to the active field grid and marks the affected runtime state
   dirty. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
   Local calls: FieldGrid_ApplySingleCellTransition.
   Cross-module calls: SelectionPlayerPairList_ContainsPairCf [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 worldYQ12,
          Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *pSVar1;
  FieldGridAsset *fieldGrid;
  dword dVar2;
  SelectionPlayerPairRecord *pSVar3;
  bool bVar4;
  
  pSVar1 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  bVar4 = SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if (bVar4) {
    FieldGrid_ApplySingleCellTransition(transitionValue,worldYQ12,worldXQ12,fieldGrid);
  }
  else {
    pSVar3 = pSVar1->pairRecords80_807F;
    for (dVar2 = pSVar1->activePairCount8084; dVar2 != 0; dVar2 = dVar2 - 1) {
      FieldGrid_ApplySingleCellTransition
                (transitionValue,pSVar3->pairValue,pSVar3->pairKey,fieldGrid);
      pSVar3 = pSVar3 + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  return;
}


/* Address: 0x00562390.
   Ownership: world/terrain/grid.
   Purpose: Marks the active field-grid state dirty and forwards one sign-extended encoded update to the exact
   lower-level grid helper. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic
   parameters or normal returns. Fixed command-payload slots remain explicit even when this wrapper does not
   consume every slot.
   Local calls: FieldGrid_ApplyEncodedUpdateCore.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyEncodedCellUpdate
          (PlayerRuntimeId playerRuntimeId,Q12 worldYQ12,Q12 worldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | 1;
  FieldGrid_ApplyEncodedUpdateCore((int)packedDragDeltaXY16 >> 0x10,worldYQ12,worldXQ12,fieldGrid);
  return;
}


/* Address: 0x005623D0.
   Ownership: world/terrain/grid.
   Purpose: Forwards the caller values with mask 0xDFFFFFFF to the shared masked-region helper and marks the field-
   grid state dirty. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or
   normal returns. Fixed command-payload slots remain explicit even when this wrapper does not consume every slot.
   Local calls: FieldGrid_ApplyMaskedRegionCore.
*/
void __thandor_preserve_eax
FieldGrid_ApplyMaskDFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(0xdfffffff,setMask,worldYQ12,worldXQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | 1;
  return;
}


/* Address: 0x00562410.
   Ownership: world/terrain/grid.
   Purpose: Forwards the caller values with mask 0xBFFFFFFF to the shared masked-region helper and marks the field-
   grid state dirty. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or
   normal returns. Fixed command-payload slots remain explicit even when this wrapper does not consume every slot.
   Local calls: FieldGrid_ApplyMaskedRegionCore.
*/
void __thandor_preserve_eax
FieldGrid_ApplyMaskBFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(0xbfffffff,setMask,worldYQ12,worldXQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | 1;
  return;
}


/* Address: 0x00562450.
   Ownership: world/terrain/grid.
   Purpose: EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns. Fixed command-payload slots remain explicit even when this wrapper does not consume every slot.
   Local calls: FieldGrid_ApplyMaskedRegionCore.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyCallerMask
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 worldYQ12,
          Q12 worldXQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRegionMask setMask;
  uint preserveMask;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  setMask = 0x800 << ((byte)materialBitIndex & 0x1f);
  preserveMask = setMask ^ 0xffffffff;
  if (materialBitIndex < 0) {
    setMask = 0;
  }
  FieldGrid_ApplyMaskedRegionCore(preserveMask,setMask,worldYQ12,worldXQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | 1;
  return;
}


/* Address: 0x004FEA80.
   Ownership: world/terrain/grid.
   Purpose: Rounds transformed coordinates to the nearest cell, bounds-checks gridWidth/gridHeight, and returns
   that cell's worldX in EAX, worldY in ECX, and terrainHeight in EDX. CF clear means success; CF set returns the
   original input coordinates and zero height.
*/
FieldGridNearestPointRegsCf13
FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int iVar1;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Q12 terrainHeightQ12;
  undefined1 uVar2;
  FieldGridNearestPointRegsCf13 FVar3;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  iVar1 = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((iVar1 < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, gridRowIndex < 0)) ||
      ((int)field->gridWidth <= iVar1)) ||
     (iVar1 = field->gridWidth * gridRowIndex + iVar1, (int)field->gridHeight <= gridRowIndex)) {
    terrainHeightQ12 = 0;
    uVar2 = 1;
  }
  else {
    worldX = field->cells[iVar1].worldX;
    worldY = field->cells[iVar1].worldY;
    terrainHeightQ12 = field->cells[iVar1].terrainHeight;
    uVar2 = 0;
  }
  FVar3.ecx = worldY;
  FVar3.eax = worldX;
  FVar3.carry = (bool)uVar2;
  FVar3.edx = terrainHeightQ12;
  return FVar3;
}


/* Address: 0x004FEB10.
   Ownership: world/terrain/grid.
   Purpose: Nearest-cell companion that returns worldX in EAX, worldY in ECX, and terrainHeight + waterSurfaceDelta
   in EDX. CF reports bounds success.
*/
FieldGridSurfacePointEaxEcxEdxCf13
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int iVar1;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  undefined1 uVar2;
  FieldGridSurfacePointEaxEcxEdxCf13 FVar3;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  iVar1 = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((iVar1 < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, gridRowIndex < 0)) ||
      ((int)field->gridWidth <= iVar1)) ||
     (iVar1 = field->gridWidth * gridRowIndex + iVar1, (int)field->gridHeight <= gridRowIndex)) {
    iVar1 = 0;
    uVar2 = 1;
  }
  else {
    worldX = field->cells[iVar1].worldX;
    worldY = field->cells[iVar1].worldY;
    iVar1 = field->cells[iVar1].waterSurfaceDelta + field->cells[iVar1].terrainHeight;
    uVar2 = 0;
  }
  FVar3.worldYQ12 = worldY;
  FVar3.worldXQ12 = worldX;
  FVar3.carry = (bool)uVar2;
  FVar3.worldZQ12 = iVar1;
  return FVar3;
}


/* Address: 0x004FEBA0.
   Ownership: world/terrain/grid.
   Purpose: Rounds to a cell and returns its signed waterSurfaceDelta. Callers use this only with coordinates
   expected to be inside the field; the out-of-range EAX value is not a defined result. Typed parameters: p0
   worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
sdword __thandor_eax_preserve_ecx_edx
FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  sdword sVar1;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  sVar1 = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((-1 < sVar1) &&
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, -1 < gridRowIndex)) &&
      (sVar1 < (int)field->gridWidth)) && (gridRowIndex < (int)field->gridHeight)) {
    sVar1 = field->cells[field->gridWidth * gridRowIndex + sVar1].waterSurfaceDelta;
  }
  return sVar1;
}


/* Address: 0x004FEC10.
   Ownership: world/terrain/grid.
   Purpose: EAX carries the Q12 result; CF reports failure. EDX is pushed and restored by the body because the sole
   indirect caller carries ModelRuntimeClassId through the call. Five-entry field-grid interpolation table
   callback; EAX is Q12 and CF reports failure.
*/
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  longlong lVar1;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint uVar2;
  uint gridRowCoordinateQ12;
  uint uVar3;
  int iVar4;
  int triangleDiagonalWeightQ12;
  bool bVar5;
  FieldGridHeightEaxCf5 FVar6;
  FieldGridDimension gridWidth;
  longlong weightedHeightAccumulator;
  
  gridRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnCoordinateQ12 =
       ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - gridRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 0xc;
  if (((-1 < gridColumnIndex) && (iVar4 = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < iVar4)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (iVar4 < (int)fieldGrid->gridHeight) {
      iVar4 = iVar4 * gridWidth * 0x80;
      uVar2 = gridColumnCoordinateQ12 & 0xfff;
      uVar3 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x10) & 0x88006000)
           == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + iVar4 + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
        if (uVar2 + uVar3 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + iVar4 + 0x18) * (longlong)(int)uVar2 +
               ((longlong)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + iVar4 + -0x18)
                * (longlong)(int)uVar3 -
               (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x18) *
               (longlong)triangleDiagonalWeightQ12);
          uVar2 = (uint)weightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          bVar5 = false;
        }
        else {
          lVar1 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + iVar4 + 0x18) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           iVar4 + -0x18) * (longlong)(int)(uVar2 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + iVar4 + 0x18) * (longlong)(int)(uVar3 - 0x1000));
          uVar2 = (uint)lVar1 >> 0xc | (int)((ulonglong)lVar1 >> 0x20) << 0x14;
          bVar5 = false;
        }
        goto LAB_004fed32;
      }
    }
  }
  uVar2 = 0;
  bVar5 = true;
LAB_004fed32:
  FVar6.carry = bVar5;
  FVar6.heightQ12 = uVar2;
  return FVar6;
}


/* Address: 0x004FED50.
   Ownership: world/terrain/grid.
   Purpose: Barycentrically interpolates FieldGridCell.waterSurfaceDelta over the same two-triangle grid square. CF
   clear means success. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
sdword __thandor_eax_preserve_ecx_edx
FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  longlong lVar1;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint uVar2;
  uint gridRowCoordinateQ12;
  uint uVar3;
  int iVar4;
  int triangleDiagonalWeightQ12;
  FieldGridDimension gridWidth;
  longlong weightedWaterDeltaAccumulator;
  
  gridRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  gridColumnCoordinateQ12 =
       ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridRowCoordinateQ12;
  gridWidth = field->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 0xc;
  if (((-1 < gridColumnIndex) && (iVar4 = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < iVar4)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (iVar4 < (int)field->gridHeight) {
      iVar4 = iVar4 * gridWidth * 0x80;
      uVar2 = gridColumnCoordinateQ12 & 0xfff;
      uVar3 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(field->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x10) & 0x88006000) == 0
          ) && ((*(uint *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                                producerName + iVar4 + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
        if (uVar2 + uVar3 < 0x1000) {
          weightedWaterDeltaAccumulator =
               (longlong)
               *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                       iVar4 + 0x1c) * (longlong)(int)uVar2 +
               ((longlong)
                *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + iVar4 + -0x14) *
                (longlong)(int)uVar3 -
               (longlong)*(int *)(field->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x14) *
               (longlong)triangleDiagonalWeightQ12);
          return (uint)weightedWaterDeltaAccumulator >> 0xc |
                 (int)((ulonglong)weightedWaterDeltaAccumulator >> 0x20) << 0x14;
        }
        lVar1 = (longlong)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                              producerName + iVar4 + 0x1c) * (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + iVar4 + -0x14) *
                 (longlong)(int)(uVar2 - 0x1000) +
                (longlong)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                        iVar4 + 0x1c) * (longlong)(int)(uVar3 - 0x1000));
        return (uint)lVar1 >> 0xc | (int)((ulonglong)lVar1 >> 0x20) << 0x14;
      }
    }
  }
  return 0;
}


/* Address: 0x004FEE90.
   Ownership: world/terrain/grid.
   Purpose: EAX carries the Q12 result; CF reports failure. EDX is pushed and restored by the body because the sole
   indirect caller carries ModelRuntimeClassId through the call. Five-entry field-grid interpolation table
   callback; EAX is Q12 and CF reports failure. World->cell: col = (worldX * -0x20C8CC) >> 21; row = ((worldY *
   0x1C6E9C) >> 20) - col; col *= 2 (inverse of the P3 2305/1152/1999 triangle lattice). Rejects when a sampled
   cell has flagsAndMaterial & 0x88006000; interpolates (terrainHeight + waterSurfaceDelta) over the triangle half
   selected by fx + fy < 0x1000.
*/
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int cellColumn;
  uint uVar1;
  Q12 gridRowFixedQ12;
  uint rowFractionQ12;
  Q12 gridColumnFixedQ12;
  uint columnFractionQ12;
  int iVar2;
  Q12 upperTriangleWeightQ12;
  bool bVar3;
  FieldGridHeightEaxCf5 FVar4;
  longlong upperTriangleWeightedHeightAccumulator;
  FieldGridDimension gridWidth;
  longlong weightedHeightAccumulator;
  
  gridColumnFixedQ12 =
       (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  gridRowFixedQ12 =
       ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - gridColumnFixedQ12;
  gridWidth = fieldGrid->gridWidth;
  cellColumn = gridRowFixedQ12 >> 0xc;
  if (((-1 < cellColumn) && (iVar2 = gridColumnFixedQ12 * 2 >> 0xc, -1 < iVar2)) &&
     (cellColumn < (int)gridWidth)) {
    if (iVar2 < (int)fieldGrid->gridHeight) {
      iVar2 = iVar2 * gridWidth * 0x80;
      rowFractionQ12 = gridRowFixedQ12 & 0xfff;
      columnFractionQ12 = gridColumnFixedQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[cellColumn].runtime60_6B + iVar2 + -0x10) & 0x88006000) == 0)
         && ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + iVar2 + 0x20) & 0x88006000) == 0)) {
        upperTriangleWeightQ12 = (rowFractionQ12 + columnFractionQ12) - 0x1000;
        if (rowFractionQ12 + columnFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        iVar2 + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       iVar2 + 0x1c)) * (longlong)(int)rowFractionQ12 +
               ((longlong)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + iVar2 + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + iVar2 + -0x14)) *
                (longlong)(int)columnFractionQ12 -
               (longlong)
               (*(int *)(fieldGrid->cells[cellColumn].runtime60_6B + iVar2 + -0x18) +
               *(int *)(fieldGrid->cells[cellColumn].runtime60_6B + iVar2 + -0x14)) *
               (longlong)upperTriangleWeightQ12);
          uVar1 = (uint)weightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          bVar3 = false;
        }
        else {
          upperTriangleWeightedHeightAccumulator =
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                              producerName + iVar2 + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + iVar2 + 0x1c)) * (longlong)upperTriangleWeightQ12 -
               ((longlong)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + iVar2 + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + iVar2 + -0x14)) *
                (longlong)(int)(rowFractionQ12 - 0x1000) +
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        iVar2 + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       iVar2 + 0x1c)) * (longlong)(int)(columnFractionQ12 - 0x1000));
          uVar1 = (uint)upperTriangleWeightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)upperTriangleWeightedHeightAccumulator >> 0x20) << 0x14;
          bVar3 = false;
        }
        goto LAB_004fefd6;
      }
    }
  }
  uVar1 = 0;
  bVar3 = true;
LAB_004fefd6:
  FVar4.carry = bVar3;
  FVar4.heightQ12 = uVar1;
  return FVar4;
}


/* Address: 0x004FEFF0.
   Ownership: world/terrain/grid.
   Purpose: EAX carries the Q12 result; CF reports failure. EDX is pushed and restored by the body because the sole
   indirect caller carries ModelRuntimeClassId through the call. Five-entry field-grid interpolation table
   callback; EAX is Q12 and CF reports failure.
*/
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  longlong lVar1;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint uVar2;
  uint gridRowCoordinateQ12;
  uint uVar3;
  uint terrainHeightQ12;
  int iVar4;
  int triangleDiagonalWeightQ12;
  bool bVar5;
  FieldGridHeightEaxCf5 FVar6;
  FieldGridDimension gridWidth;
  longlong weightedSurfaceAccumulator;
  
  gridRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnCoordinateQ12 =
       ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - gridRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 0xc;
  if (((-1 < gridColumnIndex) && (iVar4 = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < iVar4)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (iVar4 < (int)fieldGrid->gridHeight) {
      iVar4 = iVar4 * gridWidth * 0x80;
      uVar2 = gridColumnCoordinateQ12 & 0xfff;
      uVar3 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x10) & 0x88006000)
           == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + iVar4 + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
        if (uVar2 + uVar3 < 0x1000) {
          weightedSurfaceAccumulator =
               (longlong)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + iVar4 + 0x18) * (longlong)(int)uVar2 +
               ((longlong)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + iVar4 + -0x18)
                * (longlong)(int)uVar3 -
               (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x18) *
               (longlong)triangleDiagonalWeightQ12);
          terrainHeightQ12 =
               (int)((ulonglong)weightedSurfaceAccumulator >> 0x20) << 0x14 |
               (uint)weightedSurfaceAccumulator >> 0xc;
          lVar1 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + iVar4 + 0x1c) * (longlong)(int)uVar2 +
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           iVar4 + -0x14) * (longlong)(int)uVar3 -
                  (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + iVar4 + -0x14)
                  * (longlong)triangleDiagonalWeightQ12);
          uVar2 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
          if (-1 < (int)uVar2) {
            terrainHeightQ12 = terrainHeightQ12 + uVar2;
          }
          bVar5 = false;
        }
        else {
          lVar1 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + iVar4 + 0x18) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           iVar4 + -0x18) * (longlong)(int)(uVar2 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + iVar4 + 0x18) * (longlong)(int)(uVar3 - 0x1000));
          terrainHeightQ12 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
          lVar1 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + iVar4 + 0x1c) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           iVar4 + -0x14) * (longlong)(int)(uVar2 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + iVar4 + 0x1c) * (longlong)(int)(uVar3 - 0x1000));
          uVar2 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
          if (-1 < (int)uVar2) {
            terrainHeightQ12 = terrainHeightQ12 + uVar2;
          }
          bVar5 = false;
        }
        goto LAB_004ff185;
      }
    }
  }
  terrainHeightQ12 = 0;
  bVar5 = true;
LAB_004ff185:
  FVar6.carry = bVar5;
  FVar6.heightQ12 = terrainHeightQ12;
  return FVar6;
}


/* Address: 0x004FF1A0.
   Ownership: world/terrain/grid.
   Purpose: Interpolates terrainHeight in EAX and derives a packed normal-angle pair in EDX from the selected
   triangle's precomputed normal fields. CF clear means success. Typed parameters: p0 worldX→Q12, p1 worldY→Q12.
   Nearby but non-identical semantic domains were explicitly deferred. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  dword dVar6;
  uint uVar7;
  dword dVar8;
  uint uVar9;
  dword dVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  bool bVar14;
  FieldGridHeightNormalEaxEdxCf9 FVar15;
  FixedMathVectorAnglesRegs8 FVar16;
  FixedDirectionXyzRegs12 FVar17;
  
  uVar9 = (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  uVar7 = ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - uVar9;
  uVar9 = uVar9 * 2;
  FVar1 = field->gridWidth;
  iVar4 = (int)uVar7 >> 0xc;
  if (((-1 < iVar4) && (iVar11 = (int)uVar9 >> 0xc, -1 < iVar11)) && (iVar4 < (int)FVar1)) {
    if (iVar11 < (int)field->gridHeight) {
      iVar11 = iVar11 * FVar1 * 0x80;
      iVar12 = iVar11 + iVar4 * 0x80;
      uVar7 = uVar7 & 0xfff;
      uVar9 = uVar9 & 0xfff;
      if (((*(uint *)(field->cells[iVar4].runtime60_6B + iVar11 + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&field[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar11 + 0x20) & 0x88006000) == 0)) {
        iVar13 = (uVar7 + uVar9) - 0x1000;
        if (uVar7 + uVar9 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x18) * (longlong)(int)uVar7 +
                  ((longlong)*(int *)(field->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x18) *
                   (longlong)(int)uVar9 -
                  (longlong)*(int *)(field->cells[iVar4].runtime60_6B + iVar11 + -0x18) *
                  (longlong)iVar13);
          uVar2 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar12 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7);
          dVar10 = FVar17.edx;
          dVar8 = FVar17.ecx;
          dVar6 = FVar17.eax;
          uVar7 = *(uint *)(field->cells[iVar4].runtime0C_3F + iVar11 + -4);
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,iVar13)
          ;
          iVar12 = dVar6 - FVar17.eax;
          iVar13 = dVar8 - FVar17.ecx;
          uVar7 = *(uint *)(field->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
          iVar4 = dVar10 - FVar17.edx;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9);
          FVar16 = FixedMath_VectorToAngles3Regs
                             (iVar4 + FVar17.edx,iVar13 + FVar17.ecx,iVar12 + FVar17.eax);
          uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
          bVar14 = false;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                          iVar11 + 0x18) * (longlong)iVar13 -
                  ((longlong)*(int *)(field->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x18) *
                   (longlong)(int)(uVar7 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x18) * (longlong)(int)(uVar9 - 0x1000));
          uVar2 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           FVar1 * 0x80 + iVar12 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,iVar13)
          ;
          dVar10 = FVar17.edx;
          dVar8 = FVar17.ecx;
          dVar6 = FVar17.eax;
          uVar2 = *(uint *)(field->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7 - 0x1000);
          iVar4 = dVar6 - FVar17.eax;
          iVar11 = dVar8 - FVar17.ecx;
          uVar7 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar12 + -0x20);
          iVar12 = dVar10 - FVar17.edx;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9 - 0x1000);
          FVar16 = FixedMath_VectorToAngles3Regs
                             (iVar12 - FVar17.edx,iVar11 - FVar17.ecx,iVar4 - FVar17.eax);
          uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
          bVar14 = false;
        }
        goto FieldGrid_InterpolateTerrainHeightAndNormal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
  bVar14 = true;
FieldGrid_InterpolateTerrainHeightAndNormal_ReturnInterpolatedOrDefaultSample:
  FVar15.packedNormalAngles = uVar9;
  FVar15.heightQ12 = uVar5;
  FVar15.carry = bVar14;
  return FVar15;
}


/* Address: 0x004FF3D0.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  dword dVar6;
  uint uVar7;
  dword dVar8;
  uint uVar9;
  dword dVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  bool bVar14;
  FieldGridHeightNormalEaxEdxCf9 FVar15;
  FixedMathVectorAnglesRegs8 FVar16;
  FixedDirectionXyzRegs12 FVar17;
  
  uVar9 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar7 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar9;
  uVar9 = uVar9 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar7 >> 0xc;
  if (((-1 < iVar4) && (iVar11 = (int)uVar9 >> 0xc, -1 < iVar11)) && (iVar4 < (int)FVar1)) {
    if (iVar11 < (int)fieldGrid->gridHeight) {
      iVar11 = iVar11 * FVar1 * 0x80;
      iVar12 = iVar11 + iVar4 * 0x80;
      uVar7 = uVar7 & 0xfff;
      uVar9 = uVar9 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar11 + 0x20) & 0x88006000) == 0)) {
        iVar13 = (uVar7 + uVar9) - 0x1000;
        if (uVar7 + uVar9 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)(int)uVar7 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)(int)uVar9 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x14) *
                  (longlong)iVar13);
          uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar12 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7);
          dVar10 = FVar17.edx;
          dVar8 = FVar17.ecx;
          dVar6 = FVar17.eax;
          uVar7 = *(uint *)(fieldGrid->cells[iVar4].runtime0C_3F + iVar11 + -4);
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,iVar13)
          ;
          iVar12 = dVar6 - FVar17.eax;
          iVar13 = dVar8 - FVar17.ecx;
          uVar7 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
          iVar4 = dVar10 - FVar17.edx;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9);
          FVar16 = FixedMath_VectorToAngles3Regs
                             (iVar4 + FVar17.edx,iVar13 + FVar17.ecx,iVar12 + FVar17.eax);
          uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
          bVar14 = false;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar11 + 0x1c) * (longlong)iVar13 -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)(int)(uVar7 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)(int)(uVar9 - 0x1000));
          uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           FVar1 * 0x80 + iVar12 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,iVar13)
          ;
          dVar10 = FVar17.edx;
          dVar8 = FVar17.ecx;
          dVar6 = FVar17.eax;
          uVar2 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7 - 0x1000);
          iVar4 = dVar6 - FVar17.eax;
          iVar11 = dVar8 - FVar17.ecx;
          uVar7 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar12 + -0x20);
          iVar12 = dVar10 - FVar17.edx;
          FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9 - 0x1000);
          FVar16 = FixedMath_VectorToAngles3Regs
                             (iVar12 - FVar17.edx,iVar11 - FVar17.ecx,iVar4 - FVar17.eax);
          uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
          bVar14 = false;
        }
        goto FieldGrid_InterpolateTerrainHeightAndTriangle0Normal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
  bVar14 = true;
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal_ReturnInterpolatedOrDefaultSample:
  FVar15.packedNormalAngles = uVar9;
  FVar15.heightQ12 = uVar5;
  FVar15.carry = bVar14;
  return FVar15;
}


/* Address: 0x004FF600.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  dword dVar6;
  uint uVar7;
  dword dVar8;
  uint uVar9;
  dword dVar10;
  int iVar11;
  int iVar12;
  bool bVar13;
  FieldGridHeightNormalEaxEdxCf9 FVar14;
  FixedMathVectorAnglesRegs8 FVar15;
  FixedDirectionXyzRegs12 FVar16;
  int iVar17;
  
  uVar9 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar7 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar9;
  uVar9 = uVar9 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar7 >> 0xc;
  if (((-1 < iVar4) && (iVar11 = (int)uVar9 >> 0xc, -1 < iVar11)) && (iVar4 < (int)FVar1)) {
    if (iVar11 < (int)fieldGrid->gridHeight) {
      iVar11 = iVar11 * FVar1 * 0x80;
      uVar7 = uVar7 & 0xfff;
      uVar9 = uVar9 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar11 + 0x20) & 0x88006000) == 0)) {
        iVar12 = (uVar7 + uVar9) - 0x1000;
        if (uVar7 + uVar9 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)(int)uVar7 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)(int)uVar9 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x14) *
                  (longlong)iVar12);
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar11 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7);
          dVar10 = FVar16.edx;
          dVar8 = FVar16.ecx;
          dVar6 = FVar16.eax;
          uVar7 = *(uint *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + 0x18);
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,iVar12)
          ;
          iVar12 = dVar6 - FVar16.eax;
          iVar17 = dVar8 - FVar16.ecx;
          uVar7 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + 0x18);
          iVar4 = dVar10 - FVar16.edx;
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9);
          FVar15 = FixedMath_VectorToAngles3Regs
                             (iVar4 + FVar16.edx,iVar17 + FVar16.ecx,iVar12 + FVar16.eax);
          uVar9 = FVar15.edx << 0x10 | FVar15.ecx & 0xffff;
          bVar13 = false;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar11 + 0x1c) * (longlong)iVar12 -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)(int)(uVar7 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)(int)(uVar9 - 0x1000));
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                 sourceName + iVar11 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,iVar12)
          ;
          dVar10 = FVar16.edx;
          dVar8 = FVar16.ecx;
          dVar6 = FVar16.eax;
          uVar2 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + 0x18);
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7 - 0x1000);
          iVar12 = dVar6 - FVar16.eax;
          iVar17 = dVar8 - FVar16.ecx;
          uVar7 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar11 + 8);
          iVar4 = dVar10 - FVar16.edx;
          FVar16 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9 - 0x1000);
          FVar15 = FixedMath_VectorToAngles3Regs
                             (iVar4 - FVar16.edx,iVar17 - FVar16.ecx,iVar12 - FVar16.eax);
          uVar9 = FVar15.edx << 0x10 | FVar15.ecx & 0xffff;
          bVar13 = false;
        }
        goto FieldGrid_InterpolateTerrainHeightAndTriangle1Normal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
  bVar13 = true;
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal_ReturnInterpolatedOrDefaultSample:
  FVar14.packedNormalAngles = uVar9;
  FVar14.heightQ12 = uVar5;
  FVar14.carry = bVar13;
  return FVar14;
}


/* Address: 0x004FF830.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid sample interpolated terrain height and normal angles carry-flag result register
   result.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesCfRegs
          (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  dword dVar6;
  uint uVar7;
  dword dVar8;
  int scale;
  uint uVar9;
  dword dVar10;
  int scale_00;
  int iVar11;
  int iVar12;
  int iVar13;
  bool bVar14;
  FieldGridHeightNormalEaxEdxCf9 FVar15;
  FixedMathVectorAnglesRegs8 FVar16;
  FixedDirectionXyzRegs12 FVar17;
  
  uVar9 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar7 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar9;
  uVar9 = uVar9 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar7 >> 0xc;
  if (((-1 < iVar4) && (iVar11 = (int)uVar9 >> 0xc, -1 < iVar11)) && (iVar4 < (int)FVar1)) {
    if (iVar11 < (int)fieldGrid->gridHeight) {
      iVar11 = iVar11 * FVar1 * 0x80;
      iVar12 = iVar11 + iVar4 * 0x80;
      uVar7 = uVar7 & 0xfff;
      uVar9 = uVar9 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar11 + 0x20) & 0x88006000) == 0)) {
        iVar13 = (uVar7 + uVar9) - 0x1000;
        if (uVar7 + uVar9 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)(int)uVar7 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)(int)uVar9 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + -0x14) *
                  (longlong)iVar13);
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar11 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          if ((int)uVar5 < 0) {
            uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             iVar12 + -0x20);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7);
            dVar10 = FVar17.edx;
            dVar8 = FVar17.ecx;
            dVar6 = FVar17.eax;
            uVar7 = *(uint *)(fieldGrid->cells[iVar4].runtime0C_3F + iVar11 + -4);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar7 >> 0x10,uVar7 & 0xffff,iVar13);
            iVar12 = dVar6 - FVar17.eax;
            iVar13 = dVar8 - FVar17.ecx;
            uVar7 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
            iVar4 = dVar10 - FVar17.edx;
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9);
            FVar16 = FixedMath_VectorToAngles3Regs
                               (iVar4 + FVar17.edx,iVar13 + FVar17.ecx,iVar12 + FVar17.eax);
            uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
            bVar14 = false;
          }
          else {
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar7);
            dVar10 = FVar17.edx;
            dVar8 = FVar17.ecx;
            dVar6 = FVar17.eax;
            uVar7 = *(uint *)(fieldGrid->cells[iVar4].runtime60_6B + iVar11 + 0x18);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar7 >> 0x10,uVar7 & 0xffff,iVar13);
            iVar12 = dVar6 - FVar17.eax;
            iVar13 = dVar8 - FVar17.ecx;
            uVar7 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + 0x18);
            iVar4 = dVar10 - FVar17.edx;
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar7 >> 0x10,uVar7 & 0xffff,uVar9);
            FVar16 = FixedMath_VectorToAngles3Regs
                               (iVar4 + FVar17.edx,iVar13 + FVar17.ecx,iVar12 + FVar17.eax);
            uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
            bVar14 = false;
          }
        }
        else {
          scale = uVar7 - 0x1000;
          scale_00 = uVar9 - 0x1000;
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar11 + 0x1c) * (longlong)iVar13 -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + -0x14)
                   * (longlong)scale +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar11 + 0x1c) * (longlong)scale_00);
          uVar9 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                 sourceName + iVar11 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          if ((int)uVar5 < 0) {
            uVar9 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             FVar1 * 0x80 + iVar12 + -0x20);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,iVar13);
            dVar10 = FVar17.edx;
            dVar8 = FVar17.ecx;
            dVar6 = FVar17.eax;
            uVar9 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar11 + -4);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,scale);
            iVar4 = dVar6 - FVar17.eax;
            iVar11 = dVar8 - FVar17.ecx;
            uVar9 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             iVar12 + -0x20);
            iVar12 = dVar10 - FVar17.edx;
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,scale_00);
            FVar16 = FixedMath_VectorToAngles3Regs
                               (iVar12 - FVar17.edx,iVar11 - FVar17.ecx,iVar4 - FVar17.eax);
            uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
            bVar14 = false;
          }
          else {
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,iVar13);
            dVar10 = FVar17.edx;
            dVar8 = FVar17.ecx;
            dVar6 = FVar17.eax;
            uVar9 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime60_6B + iVar11 + 0x18);
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,scale);
            iVar12 = dVar6 - FVar17.eax;
            iVar13 = dVar8 - FVar17.ecx;
            uVar9 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                             iVar11 + 8);
            iVar4 = dVar10 - FVar17.edx;
            FVar17 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar9 >> 0x10,uVar9 & 0xffff,scale_00);
            FVar16 = FixedMath_VectorToAngles3Regs
                               (iVar4 - FVar17.edx,iVar13 - FVar17.ecx,iVar12 - FVar17.eax);
            uVar9 = FVar16.edx << 0x10 | FVar16.ecx & 0xffff;
            bVar14 = false;
          }
        }
        goto LAB_004ffb6c;
      }
    }
  }
  uVar5 = 0;
  bVar14 = true;
LAB_004ffb6c:
  FVar15.packedNormalAngles = uVar9;
  FVar15.heightQ12 = uVar5;
  FVar15.carry = bVar14;
  return FVar15;
}


/* Address: 0x004FFB80.
   Ownership: world/terrain/grid.
   Purpose: Converts a world-space point to field-grid row and column coordinates, bounds-checks the cell, and
   returns carry clear when the cell material and state byte intersects mask 0xF9. EAX, ECX, and EDX are preserved
   or incidental caller state and are not synthetic parameters or normal returns. CF=0 reports blocked/matching
   state; CF=1 reports outside or clear.
*/
bool __thandor_cf_preserve_eax_ecx_edx
FieldGrid_TestWorldPointBlockedCf
          (FieldGridByteOffset stateByteOffset,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          )

{
  int gridColumnIndex;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnIndex =
       (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
              (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
       >> 0xc;
  if ((((-1 < gridColumnIndex) &&
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, -1 < gridRowIndex)) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) &&
     ((gridRowIndex < (int)fieldGrid->gridHeight &&
      ((fieldGrid->cells[fieldGrid->gridWidth * gridRowIndex + gridColumnIndex].runtime60_6B
        [stateByteOffset + 0x10] & 0xf9) != 0)))) {
    return false;
  }
  return true;
}


/* Address: 0x00503C90.
   Ownership: world/terrain/grid.
   Purpose: Marks the field dirty, initializes every 0x80-byte cell runtime seed, state bits, lookup pointer, and
   sentinel fields, then marks the verified outer boundaries with their directional flags. EAX, ECX, and EDX are
   preserved or incidental caller state and are not synthetic parameters or normal returns. Load-time cell init:
   phase seed = rand & ((1 << waterDatBitWidth) - 1); +0x54 bound to g_TerrainDirectionVectorTable256[(worldY & 15)
   + (worldX & 15) * 16]; boundary flag writers: 0x2000 first column, 0x4000 first row, 0x8000000 last column,
   0x80000000 last row; material variant bits 8-10 |= random. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] FLD +0x50
   owner: clears/rebuilds 0x700 variant bits, rebuilds hard-edge bits 0x2000/0x4000/0x08000000/0x80000000, clears
   unresolved 0x8000. This does not build GridScratch terrain-class bands.
   Cross-module calls: Random_NextPrimary [core/math/random].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  dword randomValue;
  dword materialVariantRandomBits;
  FieldGridDimension FVar2;
  FieldGridDimension gridWidth;
  FieldGridDimension FVar3;
  FieldGridDimension rowsRemaining;
  FieldGridDimension topRowCellsRemaining;
  FieldGridCell *initializationCellCursor;
  int iVar4;
  FieldGridCell *cellCursor;
  FieldGridCell *currentRowFirstCell;
  Q12 currentCellWorldXQ12;
  Q12 currentCellWorldYQ12;
  undefined4 phaseSeedBitWidth;
  
  phaseSeedBitWidth = *(undefined4 *)((int)g_TerrainSurfacePacketTablePayload + -0x20);
  FVar3 = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  FVar1 = fieldGrid->gridWidth;
  initializationCellCursor = fieldGrid->cells;
  FVar2 = FVar1;
  do {
    do {
      currentCellWorldXQ12 = initializationCellCursor->worldX;
      currentCellWorldYQ12 = initializationCellCursor->worldY;
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial & 0x77ff1fff;
      randomValue = Random_NextPrimary();
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      initializationCellCursor->runtimeState00 =
           randomValue & (1 << ((byte)phaseSeedBitWidth & 0x1f)) - 1U;
      initializationCellCursor->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 +
           (currentCellWorldYQ12 & 0xfU) + (currentCellWorldXQ12 & 0xfU) * 0x10);
      initializationCellCursor->armyRuntimeSavedOffset6C = 0;
      materialVariantRandomBits = Random_NextPrimary();
      initializationCellCursor->runtimeOverlayOrHeightValue04 = 0xffffffff;
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial |
           materialVariantRandomBits & FIELD_CELL_RANDOM_VARIANT_MASK;
      initializationCellCursor = initializationCellCursor + 1;
      FVar2 = FVar2 - 1;
    } while (FVar2 != 0);
    FVar3 = FVar3 - 1;
    FVar2 = FVar1;
  } while (FVar3 != 0);
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentRowFirstCell = fieldGrid->cells;
  topRowCellsRemaining = gridWidth;
  cellCursor = currentRowFirstCell;
  do {
    cellCursor->flagsAndMaterial = cellCursor->flagsAndMaterial | FIELD_CELL_FIRST_ROW_BOUNDARY;
    cellCursor = cellCursor + 1;
    topRowCellsRemaining = topRowCellsRemaining - 1;
  } while (topRowCellsRemaining != 0);
  do {
    currentRowFirstCell->flagsAndMaterial =
         currentRowFirstCell->flagsAndMaterial | FIELD_CELL_FIRST_COLUMN_BOUNDARY;
    cellCursor[-1].flagsAndMaterial =
         cellCursor[-1].flagsAndMaterial | FIELD_CELL_LAST_COLUMN_BOUNDARY;
    iVar4 = (int)cellCursor - (int)currentRowFirstCell;
    currentRowFirstCell = (FieldGridCell *)((int)currentRowFirstCell + iVar4);
    cellCursor = (FieldGridCell *)(iVar4 + (int)currentRowFirstCell);
    rowsRemaining = rowsRemaining - 1;
  } while (rowsRemaining != 0);
  iVar4 = (int)currentRowFirstCell * 2 - (int)cellCursor;
  do {
    *(uint *)(iVar4 + 0x50) = *(uint *)(iVar4 + 0x50) | 0x80000000;
    iVar4 = iVar4 + 0x80;
    gridWidth = gridWidth - 1;
  } while (gridWidth != 0);
  return;
}


/* Address: 0x00503DB0.
   Ownership: world/terrain/grid.
   Purpose: Marks the field dirty and rebuilds each cell +0x54 lookup pointer from the low nibbles of the persisted
   cell fields at +0x40 and +0x44. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic
   parameters or normal returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  
  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      currentCell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 +
           (currentCell->worldY & 0xfU) + (currentCell->worldX & 0xfU) * 0x10);
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00503E20.
   Ownership: world/terrain/grid.
   Purpose: For every field cell, combines the byte at +0x70 plus the selected channel offset with the current
   runtime byte at +0x68 through the shared terrain clamp lookup and writes the mapped byte back to +0x68. EAX,
   ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal returns.
   Consumer of the generated clamp LUT; runs on tick-wheel cases 3 and 7 (T5), mapping each cell's +0x68 runtime
   byte through the lookup.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyByteClampLookupToCells
          (FieldGridByteOffset sourceChannelOffset,FieldGridAsset *fieldGrid)

{
  undefined3 uVar1;
  byte *pbVar2;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  byte mappedRuntimeByte;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  pbVar2 = g_TerrainByteClampLookup;
  columnsRemaining = gridWidth;
  do {
    do {
      uVar1 = CONCAT21((short)((uint)pbVar2 >> 0x10),
                       currentCell->runtime60_6B[sourceChannelOffset + 0x10]);
      mappedRuntimeByte = *(byte *)CONCAT31(uVar1,currentCell->runtime60_6B[8]);
      pbVar2 = (byte *)CONCAT31(uVar1,mappedRuntimeByte);
      currentCell->runtime60_6B[8] = mappedRuntimeByte;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00503E80.
   Ownership: world/terrain/grid.
   Purpose: Classifies the selected cell flag byte into runtime byte +0x68: zero for no tested bits, 0x87 when only
   the high classification bit is present, and 0xFF for the remaining tested-bit cases. Typed parameters: p0
   cellByteOffset→FieldGridByteOffset. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClassifyCellFlagsToRuntimeByte
          (FieldGridByteOffset cellByteOffset,FieldGridAsset *fieldGrid)

{
  byte classifiedRuntimeByte;
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    classifiedRuntimeByte = 0xff;
    if (((currentCell->runtime60_6B[cellByteOffset + 0x10] & 0x79) == 0) &&
       (classifiedRuntimeByte = 0x87, (currentCell->runtime60_6B[cellByteOffset + 0x10] & 0x80) == 0
       )) {
      classifiedRuntimeByte = 0;
    }
    currentCell->runtime60_6B[8] = classifiedRuntimeByte;
    currentCell = currentCell + 1;
    cellsRemaining = cellsRemaining + -1;
  } while (cellsRemaining != 0);
  return;
}


/* Address: 0x00503EE0.
   Ownership: world/terrain/grid.
   Purpose: Advances the angle accumulator in each of 256 fixed 0x20-byte direction records and rebuilds both
   scaled sine/cosine vector pairs from the updated angles. Direct call at 00560E09.
   Cross-module calls: FixedMath_SinCosScaled [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx TerrainDirectionTable_AdvanceAndRebuildVectors(void)

{
  uint uVar1;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCosEdxEax8 scaledSinCosPair;
  FixedSinCosEdxEax8 angleBScaledSinCosPair;
  uint packedAnglesBeforeAdvance;
  
  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 0x100;
  do {
    uVar1 = currentDirectionRecord->packedAngleA_low16_AngleB_high16;
    currentDirectionRecord->packedAngleA_low16_AngleB_high16 =
         currentDirectionRecord->packedAngleA_low16_AngleB_high16 +
         *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(uVar1 & 0xffff,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = (int)scaledSinCosPair;
    currentDirectionRecord->angleAComponent1ScaledQ28 = (int)(scaledSinCosPair >> 0x20);
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled((int)uVar1 >> 0x10,currentDirectionRecord->scaleB);
    currentDirectionRecord->angleBComponent0ScaledQ28 = (dword)angleBScaledSinCosPair;
    currentDirectionRecord = currentDirectionRecord + 1;
    recordsRemaining = recordsRemaining - 1;
  } while (recordsRemaining != 0);
  return;
}


/* Address: 0x00504B10.
   Ownership: world/terrain/grid.
   Purpose: Walks the field grid for at most 0x400 boundary steps and tests the ray against the primary terrain
   triangles built from cell terrainHeight values. On hit it returns the nearest distance in EAX, the cell material
   byte in EDX, and CF set; no hit returns 0x7FFFFFFF with CF clear. Kept distinct from Q12 coordinates, Q4/Q5
   resource scales, attachment ordinals, and raw renderer flags. Explicit Q12 fixed-point value proved by the
   accepted parameter name and fixed-math/geometry consumer. Storage remains one signed 32-bit word.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   TerrainTriangle_IntersectRayDistanceCf [world/terrain/height], TerrainRay_AdvanceGridTraversalCf
   [world/terrain/height].
*/
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainSurfaceDistanceCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  FieldGridCell *currentCell;
  FieldGridDimension FVar1;
  FieldGridDimension FVar2;
  FieldGridDimension FVar3;
  longlong lVar4;
  longlong lVar5;
  ulonglong uVar6;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  uint extraout_ECX;
  uint uVar7;
  uint uVar8;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  uint extraout_EDX;
  int iVar9;
  bool bVar10;
  TerrainDistanceEaxCf5 TVar11;
  FieldGridRaycastEaxEdxCf9 FVar12;
  FieldGridRaycastEaxEdxCf9 FVar13;
  FixedDirectionXyzRegs12 FVar14;
  
  FVar1 = fieldGrid->gridWidth;
  FVar2 = fieldGrid->gridHeight;
  uVar7 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - uVar7;
  rayStartCoord0Q12 = uVar7 * 2;
  FVar3 = fieldGrid->gridWidth;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * FVar3 * 0x80 + -0xc);
  FVar14 = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  lVar4 = (longlong)(int)(FVar14.eax + rayOriginYQ12) * 0x1c6e9c;
  lVar5 = (longlong)(int)(FVar14.ecx + rayOriginXQ12) * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar5 >> 0x20) << 0xb | (uint)lVar5 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar4 >> 0x20) << 0xc | (uint)lVar4 >> 0x14) - uVar8;
  rayEndCoord0Q12 = uVar8 * 2;
  iVar9 = 0x400;
  uVar8 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  do {
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) break;
    if ((((-1 < (int)uVar8) && (-1 < (int)currentGridCoord0Q12)) &&
        ((int)uVar8 < (int)((FVar1 - 1) * 0x1000))) &&
       ((int)currentGridCoord0Q12 < (int)((FVar2 - 1) * 0x1000))) {
      cellLocalCoord1Q12 = currentGridCoord0Q12 + uVar7 * -2;
      TVar11 = TerrainTriangle_IntersectRayDistanceCf
                         (FVar14.edx,rayEndCoord0Q12 + uVar7 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          currentCell[FVar3 + 1].terrainHeight,currentCell[FVar3].terrainHeight,
                          currentCell[1].terrainHeight,currentCell->terrainHeight,cellLocalCoord1Q12
                          ,uVar8 - rayStartCoord1Q12);
      if (!TVar11.carry) {
        uVar6 = CONCAT44(currentCell->flagsAndMaterial,TVar11.distanceQ12) & 0xffffffffff;
        FVar13.carry = true;
        FVar13.distanceQ12 = (int)uVar6;
        FVar13.materialOrCellIndex = (int)(uVar6 >> 0x20);
        return FVar13;
      }
      uVar8 = (uVar8 - rayStartCoord1Q12) + rayStartCoord1Q12;
      currentGridCoord0Q12 = cellLocalCoord1Q12 + rayStartCoord0Q12;
    }
    bVar10 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        FVar3 * 0x80,currentCell,currentGridCoord0Q12,uVar8);
    uVar8 = extraout_ECX;
    currentGridCoord0Q12 = extraout_EDX;
  } while (!bVar10);
  FVar12.materialOrCellIndex = currentGridCoord0Q12;
  FVar12.distanceQ12 = 0x7fffffff;
  FVar12.carry = false;
  return FVar12;
}


/* Address: 0x00504CA0.
   Ownership: world/terrain/grid.
   Purpose: Mirrors the primary field raycast but tests the secondary surface formed by terrainHeight plus
   waterSurfaceDelta at each triangle corner. The result contract matches the primary raycast. Kept distinct from
   Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Explicit Q12 fixed-point
   value proved by the accepted parameter name and fixed-math/geometry consumer. Storage remains one signed 32-bit
   word.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   TerrainTriangle_IntersectRayDistanceCf [world/terrain/height], TerrainRay_AdvanceGridTraversalCf
   [world/terrain/height].
*/
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastSecondarySurfaceDistanceCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  FieldGridCell *currentCell;
  FieldGridDimension FVar1;
  FieldGridDimension FVar2;
  FieldGridDimension FVar3;
  longlong lVar4;
  longlong lVar5;
  ulonglong uVar6;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  uint extraout_ECX;
  uint uVar7;
  uint uVar8;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  uint extraout_EDX;
  int iVar9;
  bool bVar10;
  TerrainDistanceEaxCf5 TVar11;
  FieldGridRaycastEaxEdxCf9 FVar12;
  FieldGridRaycastEaxEdxCf9 FVar13;
  FixedDirectionXyzRegs12 FVar14;
  
  FVar1 = fieldGrid->gridWidth;
  FVar2 = fieldGrid->gridHeight;
  uVar7 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - uVar7;
  rayStartCoord0Q12 = uVar7 * 2;
  FVar3 = fieldGrid->gridWidth;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * FVar3 * 0x80 + -0xc);
  FVar14 = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  lVar4 = (longlong)(int)(FVar14.eax + rayOriginYQ12) * 0x1c6e9c;
  lVar5 = (longlong)(int)(FVar14.ecx + rayOriginXQ12) * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar5 >> 0x20) << 0xb | (uint)lVar5 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar4 >> 0x20) << 0xc | (uint)lVar4 >> 0x14) - uVar8;
  rayEndCoord0Q12 = uVar8 * 2;
  iVar9 = 0x400;
  uVar8 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  do {
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) break;
    if ((((-1 < (int)uVar8) && (-1 < (int)currentGridCoord0Q12)) &&
        ((int)uVar8 < (int)((FVar1 - 1) * 0x1000))) &&
       ((int)currentGridCoord0Q12 < (int)((FVar2 - 1) * 0x1000))) {
      cellLocalCoord1Q12 = currentGridCoord0Q12 + uVar7 * -2;
      TVar11 = TerrainTriangle_IntersectRayDistanceCf
                         (FVar14.edx,rayEndCoord0Q12 + uVar7 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          currentCell[FVar3 + 1].terrainHeight +
                          currentCell[FVar3 + 1].waterSurfaceDelta,
                          currentCell[FVar3].terrainHeight + currentCell[FVar3].waterSurfaceDelta,
                          currentCell[1].terrainHeight + currentCell[1].waterSurfaceDelta,
                          currentCell->waterSurfaceDelta + currentCell->terrainHeight,
                          cellLocalCoord1Q12,uVar8 - rayStartCoord1Q12);
      if (!TVar11.carry) {
        uVar6 = CONCAT44(currentCell->flagsAndMaterial,TVar11.distanceQ12) & 0xffffffffff;
        FVar13.carry = true;
        FVar13.distanceQ12 = (int)uVar6;
        FVar13.materialOrCellIndex = (int)(uVar6 >> 0x20);
        return FVar13;
      }
      uVar8 = (uVar8 - rayStartCoord1Q12) + rayStartCoord1Q12;
      currentGridCoord0Q12 = cellLocalCoord1Q12 + rayStartCoord0Q12;
    }
    bVar10 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        FVar3 * 0x80,currentCell,currentGridCoord0Q12,uVar8);
    uVar8 = extraout_ECX;
    currentGridCoord0Q12 = extraout_EDX;
  } while (!bVar10);
  FVar12.materialOrCellIndex = currentGridCoord0Q12;
  FVar12.distanceQ12 = 0x7fffffff;
  FVar12.carry = false;
  return FVar12;
}


/* Address: 0x00504E60.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid raycast terrain triangles along direction.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   TerrainTriangle_IntersectRayDistanceCf [world/terrain/height], TerrainRay_AdvanceGridTraversalCf
   [world/terrain/height].
*/
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  longlong lVar2;
  longlong lVar3;
  ulonglong uVar4;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  int iVar5;
  uint extraout_ECX;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  int iVar9;
  uint extraout_EDX;
  int iVar10;
  int iVar11;
  FieldGridCell *currentCell;
  FieldGridCell *pFVar12;
  int rowStrideBytes;
  bool bVar13;
  TerrainDistanceEaxCf5 TVar14;
  FieldGridRaycastEaxEdxCf9 FVar15;
  FieldGridRaycastEaxEdxCf9 FVar16;
  FixedDirectionXyzRegs12 FVar17;
  FieldCellPersistedAux cornerHeight0Q12;
  FieldCellPersistedAux cornerHeight1Q12;
  FieldCellPersistedAux cornerHeight2Q12;
  FieldCellPersistedAux cornerHeight3Q12;
  
  iVar10 = fieldGrid->gridWidth - 1;
  iVar6 = fieldGrid->gridHeight - 1;
  uVar7 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - uVar7;
  rayStartCoord0Q12 = uVar7 * 2;
  FVar1 = fieldGrid->gridWidth;
  rowStrideBytes = FVar1 * 0x80;
  FVar17 = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  lVar2 = (longlong)(int)(FVar17.eax + rayOriginYQ12) * 0x1c6e9c;
  lVar3 = (longlong)(int)(FVar17.ecx + rayOriginXQ12) * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar3 >> 0x20) << 0xb | (uint)lVar3 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar2 >> 0x20) << 0xc | (uint)lVar2 >> 0x14) - uVar8;
  rayEndCoord0Q12 = uVar8 * 2;
  iVar11 = 0x400;
  uVar8 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * rowStrideBytes + -0xc);
  do {
    iVar11 = iVar11 + -1;
    if (iVar11 == 0) break;
    cellLocalCoord1Q12 = currentGridCoord0Q12 + uVar7 * -2;
    iVar5 = (int)((uVar8 - rayStartCoord1Q12) + rayStartCoord1Q12) >> 0xc;
    iVar9 = (int)(cellLocalCoord1Q12 + rayStartCoord0Q12) >> 0xc;
    if (iVar5 < 0) {
      pFVar12 = currentCell + -iVar5;
      iVar5 = iVar9;
      if ((iVar9 < 0) || (iVar5 = iVar9 - iVar6, iVar6 <= iVar9)) {
LAB_00505090:
        pFVar12 = (FieldGridCell *)((int)pFVar12 - iVar5 * rowStrideBytes);
        cornerHeight3Q12 = pFVar12->terrainHeight;
        cornerHeight2Q12 = pFVar12->terrainHeight;
        cornerHeight1Q12 = pFVar12->terrainHeight;
        cornerHeight0Q12 = pFVar12->terrainHeight;
      }
      else {
        cornerHeight3Q12 = pFVar12->terrainHeight;
        cornerHeight2Q12 = pFVar12[FVar1].terrainHeight;
        cornerHeight1Q12 = pFVar12->terrainHeight;
        cornerHeight0Q12 = pFVar12[FVar1].terrainHeight;
      }
    }
    else if (iVar9 < 0) {
      pFVar12 = (FieldGridCell *)((int)currentCell - iVar9 * rowStrideBytes);
      if (iVar5 < iVar10) {
        cornerHeight3Q12 = pFVar12->terrainHeight;
        cornerHeight2Q12 = pFVar12[1].terrainHeight;
        cornerHeight1Q12 = pFVar12->terrainHeight;
        cornerHeight0Q12 = pFVar12[1].terrainHeight;
      }
      else {
        pFVar12 = pFVar12 + -(iVar5 - iVar10);
        cornerHeight3Q12 = pFVar12->terrainHeight;
        cornerHeight2Q12 = pFVar12->terrainHeight;
        cornerHeight1Q12 = pFVar12->terrainHeight;
        cornerHeight0Q12 = pFVar12->terrainHeight;
      }
    }
    else if (iVar5 < iVar10) {
      if (iVar9 < iVar6) {
        cornerHeight3Q12 = currentCell->terrainHeight;
        cornerHeight2Q12 = currentCell[1].terrainHeight;
        cornerHeight1Q12 = currentCell[FVar1].terrainHeight;
        cornerHeight0Q12 = currentCell[FVar1 + 1].terrainHeight;
        pFVar12 = currentCell;
      }
      else {
        pFVar12 = (FieldGridCell *)((int)currentCell - (iVar9 - iVar6) * rowStrideBytes);
        cornerHeight3Q12 = pFVar12->terrainHeight;
        cornerHeight2Q12 = pFVar12[1].terrainHeight;
        cornerHeight1Q12 = pFVar12->terrainHeight;
        cornerHeight0Q12 = pFVar12[1].terrainHeight;
      }
    }
    else {
      pFVar12 = currentCell + -(iVar5 - iVar10);
      iVar5 = iVar9 - iVar6;
      if (iVar6 <= iVar9) goto LAB_00505090;
      cornerHeight3Q12 = pFVar12->terrainHeight;
      cornerHeight2Q12 = pFVar12[FVar1].terrainHeight;
      cornerHeight1Q12 = pFVar12->terrainHeight;
      cornerHeight0Q12 = pFVar12[FVar1].terrainHeight;
    }
    TVar14 = TerrainTriangle_IntersectRayDistanceCf
                       (FVar17.edx,rayEndCoord0Q12 + uVar7 * -2,rayEndCoord1Q12 - rayStartCoord1Q12,
                        rayOriginZQ12,cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                        cornerHeight3Q12,cellLocalCoord1Q12,uVar8 - rayStartCoord1Q12);
    if (!TVar14.carry) {
      uVar4 = CONCAT44(pFVar12->flagsAndMaterial,TVar14.distanceQ12) & 0xffffffffff;
      FVar15.carry = true;
      FVar15.distanceQ12 = (int)uVar4;
      FVar15.materialOrCellIndex = (int)(uVar4 >> 0x20);
      return FVar15;
    }
    bVar13 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        rowStrideBytes,currentCell,currentGridCoord0Q12,uVar8);
    uVar8 = extraout_ECX;
    currentGridCoord0Q12 = extraout_EDX;
    currentCell = pFVar12;
  } while (!bVar13);
  FVar16.materialOrCellIndex = currentGridCoord0Q12;
  FVar16.distanceQ12 = 0x7fffffff;
  FVar16.carry = false;
  return FVar16;
}


/* Address: 0x00505120.
   Ownership: world/terrain/grid.
   Purpose: Clears bits 0 through 6 in each byte of every FieldGridCell occupancyMask while preserving each byte
   high bit. Tick-wheel case 7 head: clears bits 0-6 of every byte of FieldGridCell.occupancyMask (+0x70) before
   the per-class occupancy-rebuild callbacks repopulate it.
*/
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid)

{
  FieldGridOccupancyBlockCount eightCellBlocksPerRow;
  FieldGridOccupancyBlockCount cellBlocksRemaining;
  FieldGridOccupancyBlockCount blocksRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *blockBaseCell;
  qword occupancyHighBitMask;
  FieldGridCell *currentEightCellBlock;
  
  occupancyHighBitMask = g_FieldGridOccupancyMmxHighBitMask;
  rowsRemaining = fieldGrid->gridHeight;
  eightCellBlocksPerRow = fieldGrid->gridWidth >> 3;
  cellBlocksRemaining = eightCellBlocksPerRow;
  currentEightCellBlock = fieldGrid->cells;
  do {
    do {
      blockBaseCell = currentEightCellBlock;
      blockBaseCell->occupancyMask = blockBaseCell->occupancyMask & occupancyHighBitMask;
      blockBaseCell[1].occupancyMask = blockBaseCell[1].occupancyMask & occupancyHighBitMask;
      blockBaseCell[2].occupancyMask = blockBaseCell[2].occupancyMask & occupancyHighBitMask;
      blockBaseCell[3].occupancyMask = blockBaseCell[3].occupancyMask & occupancyHighBitMask;
      blockBaseCell[4].occupancyMask = blockBaseCell[4].occupancyMask & occupancyHighBitMask;
      blockBaseCell[5].occupancyMask = blockBaseCell[5].occupancyMask & occupancyHighBitMask;
      blockBaseCell[6].occupancyMask = blockBaseCell[6].occupancyMask & occupancyHighBitMask;
      blockBaseCell[7].occupancyMask = blockBaseCell[7].occupancyMask & occupancyHighBitMask;
      blocksRemaining = cellBlocksRemaining - 1;
      cellBlocksRemaining = blocksRemaining;
      currentEightCellBlock = blockBaseCell + 8;
    } while (blocksRemaining != 0);
    blockBaseCell[8].occupancyMask = blockBaseCell[8].occupancyMask & occupancyHighBitMask;
    blockBaseCell[9].occupancyMask = blockBaseCell[9].occupancyMask & occupancyHighBitMask;
    blockBaseCell[10].occupancyMask = blockBaseCell[10].occupancyMask & occupancyHighBitMask;
    rowsRemaining = rowsRemaining - 1;
    cellBlocksRemaining = eightCellBlocksPerRow;
    currentEightCellBlock = blockBaseCell + 0xb;
  } while (rowsRemaining != 0);
  return;
}

/* Address: 0x00505240.
   Ownership: world/terrain/grid.
   Purpose: Sets bit 0 in one selected occupancyMask byte for every cell in a FieldGridAsset. EAX, ECX, and EDX are
   preserved or incidental caller state and are not synthetic parameters or normal returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_SetOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      currentCell->runtime60_6B[occupancyMaskByteIndex + 0x10] =
           currentCell->runtime60_6B[occupancyMaskByteIndex + 0x10] | 1;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505290.
   Ownership: world/terrain/grid.
   Purpose: Clears bit 0 in one selected occupancyMask byte for every cell in a FieldGridAsset. EAX, ECX, and EDX
   are preserved or incidental caller state and are not synthetic parameters or normal returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      currentCell->runtime60_6B[occupancyMaskByteIndex + 0x10] =
           currentCell->runtime60_6B[occupancyMaskByteIndex + 0x10] & 0xfe;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00507580.
   Ownership: world/terrain/grid.
   Purpose: Typed parameters: p2 worldXQ12→Q12, p3 worldYQ12→Q12. Nearby but non-identical semantic domains were
   explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainGrid_TestProjectedCellMaskBits01Cf
          (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *pFVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  pFVar1 = worldRuntime->fieldGrid;
  uVar3 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  iVar2 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar3) + 0x800) >> 0xc;
  if ((((-1 < iVar2) && (iVar4 = (int)(uVar3 * 2 + 0x800) >> 0xc, -1 < iVar4)) &&
      (iVar2 < (int)pFVar1->gridWidth)) &&
     ((iVar4 < (int)pFVar1->gridHeight &&
      ((pFVar1->cells[pFVar1->gridWidth * iVar4 + iVar2].runtime60_6B
        [worldRuntime->activeFactionRuntimeIndex + 0x10] & 3) != 0)))) {
    return false;
  }
  return true;
}


/* Address: 0x005092A0.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid clear cell flag8000 across grid.
*/
void FieldGrid_ClearCellFlag8000AcrossGrid(FieldGridAsset *fieldGrid)

{
  int iVar1;
  FieldGridCell *pFVar2;
  
  iVar1 = fieldGrid->gridWidth * fieldGrid->gridHeight;
  pFVar2 = fieldGrid->cells;
  do {
    pFVar2->flagsAndMaterial = pFVar2->flagsAndMaterial & ~FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15;
    pFVar2 = pFVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x005092E0.
   Ownership: world/terrain/grid.
   Purpose: Fills the overlay-color dword of every 0x80-byte field-grid cell with one ARGB value. Typed parameters:
   p0 argbColor→PackedArgb32. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx
FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->runtimeOverlayOrHeightValue04 = argbColor;
    currentCell = currentCell + 1;
    cellsRemaining = cellsRemaining + -1;
  } while (cellsRemaining != 0);
  return;
}


/* Address: 0x00532B60.
   Ownership: world/terrain/grid.
   Purpose: Builds a temporary serialized FieldGrid/runtime image, clears or initializes derived per-cell state,
   writes it through FileSystem_WriteBufferToPathCf, frees the temporary allocation and returns its 32-bit status
   with carry semantics outside the C prototype.
   Cross-module calls: FileSystem_WriteBufferToPathCf [platform/filesystem/win32].
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_SaveAssetImageFromRuntimeStateCf(dword *sourceImageDwords)

{
  FieldGridAsset *pFVar1;
  FieldGridAsset *fieldGridImageCopy;
  uint uVar2;
  int iVar3;
  int iVar4;
  dword *copyDestinationDwords;
  FieldGridCellSaveImageView80 *fieldGridCellSaveView;
  byte *occupancyBytes;
  ArenaAllocEaxCf5 AVar5;
  StatusValueEaxCf5 SVar6;
  ArenaFreeEaxCf5 AVar7;
  
  uVar2 = sourceImageDwords[1];
  AVar5 = (*g_MemoryApi.alloc)(uVar2);
  fieldGridImageCopy = (FieldGridAsset *)AVar5.eax;
  if (!AVar5.carry) {
    copyDestinationDwords = (dword *)fieldGridImageCopy;
    for (uVar2 = uVar2 >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *copyDestinationDwords = *sourceImageDwords;
      sourceImageDwords = sourceImageDwords + 1;
      copyDestinationDwords = copyDestinationDwords + 1;
    }
    fieldGridCellSaveView = (FieldGridCellSaveImageView80 *)fieldGridImageCopy->cells;
    fieldGridImageCopy->fieldFlags = 0;
    iVar3 = fieldGridImageCopy->gridWidth * fieldGridImageCopy->gridHeight;
    do {
      fieldGridCellSaveView->runtime00 = 0;
      fieldGridCellSaveView->triangle0NormalAngles = 0x40000000;
      fieldGridCellSaveView->runtime0C = 0;
      fieldGridCellSaveView->runtime10 = 0;
      fieldGridCellSaveView->runtime14 = 0;
      fieldGridCellSaveView->runtime18 = 0;
      fieldGridCellSaveView->runtime1C = 0;
      fieldGridCellSaveView->runtime2C = 0;
      fieldGridCellSaveView->runtime30 = 0;
      fieldGridCellSaveView->runtime34 = 0;
      fieldGridCellSaveView->runtime38 = 0;
      fieldGridCellSaveView->runtime3C = 0;
      fieldGridCellSaveView->flagsAndMaterial = fieldGridCellSaveView->flagsAndMaterial & 0xe80078ff
      ;
      fieldGridCellSaveView->persistedAux54 = 0;
      fieldGridCellSaveView->runtime58 = 0;
      fieldGridCellSaveView->runtime5C = 0;
      fieldGridCellSaveView->runtime60 = 0;
      fieldGridCellSaveView->runtime64 = 0;
      fieldGridCellSaveView->runtime68 = 0;
      fieldGridImageCopy->fieldFlags =
           fieldGridImageCopy->fieldFlags |
           1 << ((byte)fieldGridCellSaveView->flagsAndMaterial & 0x1f);
      occupancyBytes = (byte *)&fieldGridCellSaveView->occupancyMask;
      for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
        *occupancyBytes = 0;
        occupancyBytes = occupancyBytes + 1;
      }
      fieldGridCellSaveView = fieldGridCellSaveView + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    SVar6 = FileSystem_WriteBufferToPathCf
                      ((fieldGridImageCopy->common).allocationSizeBytes,fieldGridImageCopy,
                       (word *)&g_LevelResourcePathScratchUtf16);
    if (!SVar6.carry) {
      AVar7 = (*g_MemoryApi.free)(fieldGridImageCopy);
      return (StatusValueEaxCf5)((uint5)AVar7 & 0xffffffff);
    }
    pFVar1 = (FieldGridAsset *)SVar6.valueOrError;
    (*g_MemoryApi.free)(fieldGridImageCopy);
    fieldGridImageCopy = pFVar1;
  }
  SVar6.carry = true;
  SVar6.valueOrError = (dword)fieldGridImageCopy;
  return SVar6;
}


/* Address: 0x00561050.
   Ownership: world/terrain/grid.
   Purpose: Clears the exact width-times-height dword scratch plane associated with one player runtime. EAX, ECX,
   and EDX are preserved or incidental caller state and are not synthetic parameters or normal returns. Fixed
   command-payload slots remain explicit even when this wrapper does not consume every slot.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0;
      cellsRemaining = cellsRemaining + -1) {
    *scratchHeightCursor = 0;
    scratchHeightCursor = scratchHeightCursor + 1;
  }
  return;
}


/* Address: 0x00561BB0.
   Ownership: world/terrain/grid.
   Purpose: Resets one local field-grid influence state block before rebuilding it. EAX, ECX, and EDX are preserved
   or incidental caller state and are not synthetic parameters or normal returns. Fixed command-payload slots
   remain explicit even when this wrapper does not consume every slot.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    *scratchHeightCursor = currentCell->terrainHeight;
    currentCell = currentCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    cellsRemaining = cellsRemaining + -1;
  } while (cellsRemaining != 0);
  return;
}


/* Address: 0x00571EC0.
   Ownership: world/terrain/grid.
   Purpose: EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
   Local calls: FieldGridCell_RecomputeTriangleNormalAngles, FieldGridCell_ComputeDirectionalLightColor.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyEncodedUpdateCore
          (FieldGridHeightDeltaUnits heightDeltaUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  int rowStrideBytes;
  FieldGridCell *pFVar4;
  
  FVar1 = fieldGrid->gridWidth;
  iVar2 = worldXQ12 >> 0xc;
  if ((((-1 < iVar2) && (iVar3 = worldYQ12 >> 0xc, -1 < iVar3)) && (iVar2 < (int)FVar1)) &&
     (iVar3 < (int)fieldGrid->gridHeight)) {
    rowStrideBytes = FVar1 * 0x80;
    pFVar4 = fieldGrid->cells + iVar2 + iVar3 * FVar1;
    pFVar4->waterSurfaceDelta = pFVar4->waterSurfaceDelta + heightDeltaUnits * -0x40;
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
    FieldGridCell_ComputeDirectionalLightColor(pFVar4);
    if ((pFVar4[-1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + -1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + -1);
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
    }
    pFVar4 = pFVar4 + -FVar1;
    if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4);
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
    }
    pFVar4 = pFVar4 + FVar1 * 2 + -1;
    if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4);
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
    }
  }
  return;
}


/* Address: 0x005058A0.
   Ownership: world/terrain/grid.
   Purpose: Six 4-byte stack arguments, __stdcall RET 0x18. Applies a distance-weighted Q12 terrain-height delta to
   one FieldGridCell, keeps nonnegative water-surface delta relative to the terrain change, and replaces the
   material byte only when the signed material index is nonnegative.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell)

{
  longlong lVar1;
  ulonglong uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  iVar3 = centerWorldXQ12 - cell->worldX;
  iVar4 = cell->worldY - centerWorldYQ12;
  uVar2 = (longlong)iVar4 * (longlong)iVar4 + (longlong)iVar3 * (longlong)iVar3;
  uVar6 = (uint)(uVar2 >> 0x20);
  iVar3 = (int)((ulonglong)((longlong)radiusWorldUnits * (longlong)radiusWorldUnits) >> 0x20);
  uVar5 = (uint)((longlong)radiusWorldUnits * (longlong)radiusWorldUnits);
  if (((int)uVar6 <= iVar3) &&
     ((((int)uVar6 < iVar3 || ((int)uVar2 < (int)uVar5)) &&
      (uVar5 = iVar3 << 0x14 | uVar5 >> 0xc, uVar5 != 0)))) {
    lVar1 = (longlong)
            ((int)((longlong)((ulonglong)uVar6 << 0x20 | uVar2 & 0xffffffff) / (longlong)(int)uVar5)
            + -0x1000) * (longlong)terrainHeightDeltaAmplitudeQ12;
    uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
    cell->terrainHeight = cell->terrainHeight + uVar5;
    if (-1 < cell->waterSurfaceDelta) {
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - uVar5;
    }
    if (-1 < terrainMaterialIndexOrNegativeSentinel) {
      cell->flagsAndMaterial =
           cell->flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK |
           terrainMaterialIndexOrNegativeSentinel;
    }
  }
  return;
}


/* Address: 0x00505AA0.
   Ownership: world/terrain/grid.
   Purpose: Scans interior field cells forward and relaxes six neighboring height pairs toward the selected source
   sum when the source is nonnegative and its exclusion flag is clear. Live fluid Pass A = simulation tick-wheel
   case 1 (T5). Gates: source cell skipped when waterSurfaceDelta < 0 or flagsAndMaterial & 0x40000000
   (SkipSource); neighbor skipped when & 0x20000000 (SkipNeighbor). [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] FLD
   namespace: 0x40000000 excludes a source cell; 0x20000000 excludes a receiver/neighbor. These masks are persisted
   FLD flags, not GridScratch class bits.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

{
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  FieldGridCell *sourceCell;
  FieldGridCell *cellBeforeSource;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCell = fieldGrid->cells + gridWidth;
  do {
    columnsRemaining = gridWidth - 2;
    cellBeforeSource = sourceCell;
    do {
      cellBeforeSource = cellBeforeSource + 1;
      if ((-1 < cellBeforeSource[1].waterSurfaceDelta) &&
         ((cellBeforeSource[1].flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 =
             cellBeforeSource[1].waterSurfaceDelta + cellBeforeSource[1].terrainHeight;
        if ((cellBeforeSource[-gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          cellBeforeSource[-gridWidth].waterSurfaceDelta =
               cellBeforeSource[-gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[-gridWidth].waterSurfaceDelta +
                cellBeforeSource[-gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1 - gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[1 - gridWidth].waterSurfaceDelta =
               cellBeforeSource[1 - gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[1 - gridWidth].waterSurfaceDelta +
                cellBeforeSource[1 - gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0
           ) {
          cellBeforeSource[gridWidth].waterSurfaceDelta =
               cellBeforeSource[gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth].waterSurfaceDelta +
                cellBeforeSource[gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth - 1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[gridWidth - 1].waterSurfaceDelta =
               cellBeforeSource[gridWidth - 1].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth - 1].waterSurfaceDelta +
                cellBeforeSource[gridWidth - 1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[-1].waterSurfaceDelta =
               cellBeforeSource[-1].waterSurfaceDelta -
               ((cellBeforeSource[-1].waterSurfaceDelta + cellBeforeSource[-1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[1].waterSurfaceDelta =
               cellBeforeSource[1].waterSurfaceDelta -
               ((cellBeforeSource[1].waterSurfaceDelta + cellBeforeSource[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellBeforeSource = cellBeforeSource;
    } while (columnsRemaining != 0);
    sourceCell = cellBeforeSource + 2;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505BE0.
   Ownership: world/terrain/grid.
   Purpose: Scans interior field cells in reverse and relaxes six neighboring height pairs toward the selected
   source sum when the source is nonnegative and its exclusion flag is clear. Live fluid Pass B = tick-wheel case 5
   (T5); reverse scan of the Pass A relaxation with identical sign/flag gates.
   [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Reverse scan of the sign-gated fluid relaxation with the same FLD
   source-exclusion 0x40000000 and receiver-exclusion 0x20000000 semantics.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *piVar1;
  FieldGridDimension FVar2;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int iVar3;
  int neighborWaterDeltaAddress;
  FieldGridDimension gridWidth;
  
  FVar2 = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (int)fieldGrid + FVar2 * -0x80 + (FVar2 * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = FVar2 - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -0x80;
      if ((-1 < *(int *)(cellAfterSourceAddress + -0x34)) &&
         ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x40000000) == 0)) {
        sourceSurfaceHeightQ12 =
             *(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38);
        iVar3 = cellAfterSourceAddress + FVar2 * -0x80;
        if ((*(uint *)(iVar3 + 0x50) & 0x20000000) == 0) {
          neighborWaterDeltaAddress = iVar3 + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(iVar3 + 0x4c) + *(int *)(iVar3 + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(iVar3 + 0xd0) & 0x20000000) == 0) {
          *(int *)(iVar3 + 0xcc) =
               *(int *)(iVar3 + 0xcc) -
               ((*(int *)(iVar3 + 0xcc) + *(int *)(iVar3 + 200)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(iVar3 + 0x50 + FVar2 * 0x100) & 0x20000000) == 0) {
          piVar1 = (int *)(iVar3 + 0x4c + FVar2 * 0x100);
          *piVar1 = *piVar1 - ((*(int *)(iVar3 + 0x4c + FVar2 * 0x100) +
                               *(int *)(iVar3 + 0x48 + FVar2 * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint *)(iVar3 + -0x30 + FVar2 * 0x100) & 0x20000000) == 0) {
          piVar1 = (int *)(iVar3 + -0x34 + FVar2 * 0x100);
          *piVar1 = *piVar1 - ((*(int *)(iVar3 + -0x34 + FVar2 * 0x100) +
                               *(int *)(iVar3 + -0x38 + FVar2 * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = iVar3 + FVar2 * 0x80;
        if ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x20000000) == 0) {
          *(int *)(cellAfterSourceAddress + -0x34) =
               *(int *)(cellAfterSourceAddress + -0x34) -
               ((*(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38)
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(cellAfterSourceAddress + 0xd0) & 0x20000000) == 0) {
          *(int *)(cellAfterSourceAddress + 0xcc) =
               *(int *)(cellAfterSourceAddress + 0xcc) -
               ((*(int *)(cellAfterSourceAddress + 0xcc) + *(int *)(cellAfterSourceAddress + 200)) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellAfterSourceAddress = cellAfterSourceAddress;
    } while (columnsRemaining != 0);
    sourceCellAddress = cellAfterSourceAddress + -0x100;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505D30.
   Ownership: world/terrain/grid.
   Purpose: Performs the forward interior-cell height relaxation pass without the source sign test, while
   preserving the verified exclusion flags. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Ungated-sign terrain
   relaxation still honors FLD 0x40000000 source exclusion and 0x20000000 receiver exclusion; do not reinterpret
   them as GridScratch terrain classes.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid)

{
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  FieldGridCell *sourceCell;
  FieldGridCell *cellBeforeSource;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCell = fieldGrid->cells + gridWidth;
  do {
    columnsRemaining = gridWidth - 2;
    cellBeforeSource = sourceCell;
    do {
      cellBeforeSource = cellBeforeSource + 1;
      if ((cellBeforeSource[1].flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0) {
        sourceSurfaceHeightQ12 =
             cellBeforeSource[1].waterSurfaceDelta + cellBeforeSource[1].terrainHeight;
        if ((cellBeforeSource[-gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          cellBeforeSource[-gridWidth].waterSurfaceDelta =
               cellBeforeSource[-gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[-gridWidth].waterSurfaceDelta +
                cellBeforeSource[-gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1 - gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[1 - gridWidth].waterSurfaceDelta =
               cellBeforeSource[1 - gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[1 - gridWidth].waterSurfaceDelta +
                cellBeforeSource[1 - gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0
           ) {
          cellBeforeSource[gridWidth].waterSurfaceDelta =
               cellBeforeSource[gridWidth].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth].waterSurfaceDelta +
                cellBeforeSource[gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[gridWidth - 1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[gridWidth - 1].waterSurfaceDelta =
               cellBeforeSource[gridWidth - 1].waterSurfaceDelta -
               ((cellBeforeSource[gridWidth - 1].waterSurfaceDelta +
                cellBeforeSource[gridWidth - 1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[-1].waterSurfaceDelta =
               cellBeforeSource[-1].waterSurfaceDelta -
               ((cellBeforeSource[-1].waterSurfaceDelta + cellBeforeSource[-1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          cellBeforeSource[1].waterSurfaceDelta =
               cellBeforeSource[1].waterSurfaceDelta -
               ((cellBeforeSource[1].waterSurfaceDelta + cellBeforeSource[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellBeforeSource = cellBeforeSource;
    } while (columnsRemaining != 0);
    sourceCell = cellBeforeSource + 2;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505E60.
   Ownership: world/terrain/grid.
   Purpose: Performs the reverse interior-cell height relaxation pass without the source sign test, while
   preserving the verified exclusion flags. Land-tool Pass D: same relaxation starting at (H-2, W-2), stepping rows
   by -0x180 bytes. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Reverse ungated-sign terrain relaxation with the same
   FLD exclusion masks.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

{
  int *piVar1;
  FieldGridDimension FVar2;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int iVar3;
  int neighborWaterDeltaAddress;
  FieldGridDimension gridWidth;
  
  FVar2 = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (int)fieldGrid + FVar2 * -0x80 + (FVar2 * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = FVar2 - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -0x80;
      if ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x40000000) == 0) {
        sourceSurfaceHeightQ12 =
             *(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38);
        iVar3 = cellAfterSourceAddress + FVar2 * -0x80;
        if ((*(uint *)(iVar3 + 0x50) & 0x20000000) == 0) {
          neighborWaterDeltaAddress = iVar3 + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(iVar3 + 0x4c) + *(int *)(iVar3 + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(iVar3 + 0xd0) & 0x20000000) == 0) {
          *(int *)(iVar3 + 0xcc) =
               *(int *)(iVar3 + 0xcc) -
               ((*(int *)(iVar3 + 0xcc) + *(int *)(iVar3 + 200)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(iVar3 + 0x50 + FVar2 * 0x100) & 0x20000000) == 0) {
          piVar1 = (int *)(iVar3 + 0x4c + FVar2 * 0x100);
          *piVar1 = *piVar1 - ((*(int *)(iVar3 + 0x4c + FVar2 * 0x100) +
                               *(int *)(iVar3 + 0x48 + FVar2 * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint *)(iVar3 + -0x30 + FVar2 * 0x100) & 0x20000000) == 0) {
          piVar1 = (int *)(iVar3 + -0x34 + FVar2 * 0x100);
          *piVar1 = *piVar1 - ((*(int *)(iVar3 + -0x34 + FVar2 * 0x100) +
                               *(int *)(iVar3 + -0x38 + FVar2 * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = iVar3 + FVar2 * 0x80;
        if ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x20000000) == 0) {
          *(int *)(cellAfterSourceAddress + -0x34) =
               *(int *)(cellAfterSourceAddress + -0x34) -
               ((*(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38)
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(cellAfterSourceAddress + 0xd0) & 0x20000000) == 0) {
          *(int *)(cellAfterSourceAddress + 0xcc) =
               *(int *)(cellAfterSourceAddress + 0xcc) -
               ((*(int *)(cellAfterSourceAddress + 0xcc) + *(int *)(cellAfterSourceAddress + 200)) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellAfterSourceAddress = cellAfterSourceAddress;
    } while (columnsRemaining != 0);
    sourceCellAddress = cellAfterSourceAddress + -0x100;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00571090.
   Ownership: world/terrain/grid.
   Purpose: Processes one horizontal field-grid span, updating cell accumulators and refreshing affected
   boundaries. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or
   normal returns.
   Cross-module calls: FixedMath_Length2 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ProcessHorizontalSpan
          (Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerWorldYQ12,Q12 centerWorldXQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  longlong lVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  FieldGridCell *pFVar12;
  int *piVar13;
  FieldGridCell *pFStack_34;
  int *piStack_30;
  int iStack_24;
  
  uVar3 = radiusUnits * 0x40;
  if ((int)uVar3 < 0) {
    uVar3 = radiusUnits * -0x40;
  }
  if (0x5000 < uVar3) {
    uVar3 = 0x5000;
  }
  iVar7 = centerWorldXQ12 + uVar3 * -4;
  iVar8 = centerWorldYQ12 + uVar3 * -4;
  iVar4 = (int)(iVar7 + 0xfff + uVar3 * 8) >> 0xc;
  iVar10 = (int)(iVar8 + 0xfff + uVar3 * 8) >> 0xc;
  iVar7 = iVar7 >> 0xc;
  if (iVar7 < 0) {
    iVar7 = 0;
  }
  iVar8 = iVar8 >> 0xc;
  if (iVar8 < 0) {
    iVar8 = 0;
  }
  if ((int)fieldGrid->gridWidth <= iVar4) {
    iVar4 = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= iVar10) {
    iVar10 = fieldGrid->gridHeight - 1;
  }
  if ((iVar7 <= iVar4) && (iVar8 <= iVar10)) {
    iVar5 = (iVar4 - iVar7) + 1;
    iVar7 = iVar8 * fieldGrid->gridWidth + iVar7;
    iStack_24 = (iVar10 - iVar8) + 1;
    piVar13 = accumulatorPlane + iVar7;
    FVar1 = fieldGrid->gridWidth;
    iVar8 = (centerWorldYQ12 >> 0xc) * FVar1 + (centerWorldXQ12 >> 0xc);
    pFVar12 = fieldGrid->cells + iVar7;
    iVar4 = fieldGrid->cells[iVar8].worldX;
    iVar7 = fieldGrid->cells[iVar8].worldY;
    iVar8 = fieldGrid->cells
            [(sourceWorldYQ12 >> 0xc) * fieldGrid->gridWidth + (sourceWorldXQ12 >> 0xc)].
            terrainHeight;
    iVar10 = iVar5;
    pFStack_34 = pFVar12;
    piStack_30 = piVar13;
    do {
      do {
        dVar6 = FixedMath_Length2(pFVar12->worldY - iVar7,pFVar12->worldX - iVar4);
        if (dVar6 <= uVar3 + 1) {
          iVar11 = (iVar8 + heightDeltaUnits * -0x40) - *piVar13;
          lVar2 = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)dVar6 & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)dVar6 * 0x8000 & 0xffffffffU) / (longlong)(int)(uVar3 + 1))
                   ] + 0x10000000) * (longlong)iVar11;
          iVar9 = ((int)((ulonglong)lVar2 >> 0x20) << 3 | (uint)lVar2 >> 0x1d) + *piVar13;
          if (iVar11 != 0) {
            if (iVar11 < 0) {
              if (iVar9 < *piVar13) {
                *piVar13 = iVar9;
              }
            }
            else if (*piVar13 < iVar9) {
              *piVar13 = iVar9;
            }
          }
        }
        pFVar12 = pFVar12 + 1;
        piVar13 = piVar13 + 1;
        iVar10 = iVar10 + -1;
      } while (iVar10 != 0);
      piVar13 = piStack_30 + FVar1;
      pFVar12 = pFStack_34 + FVar1;
      iStack_24 = iStack_24 + -1;
      iVar10 = iVar5;
      pFStack_34 = pFVar12;
      piStack_30 = piVar13;
    } while (iStack_24 != 0);
  }
  return;
}


/* Address: 0x00571250.
   Ownership: world/terrain/grid.
   Purpose: Processes one vertical field-grid span, updating cell accumulators and refreshing affected boundaries.
   EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal returns.
   Cross-module calls: FixedMath_Length2 [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ProcessVerticalSpan
          (FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid)

{
  int iVar1;
  FieldGridDimension FVar2;
  longlong lVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  dword dVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  FieldGridCell *pFVar11;
  int *piVar12;
  FieldGridCell *pFStack_30;
  int *piStack_2c;
  int iStack_24;
  
  uVar4 = radiusUnits * 0x40;
  if ((int)uVar4 < 0) {
    uVar4 = radiusUnits * -0x40;
  }
  if (0x5000 < uVar4) {
    uVar4 = 0x5000;
  }
  iVar1 = heightDeltaUnits * -0x40;
  iVar8 = centerWorldXQ12 + uVar4 * -2;
  iVar9 = centerWorldYQ12 + uVar4 * -2;
  iVar5 = (int)(iVar8 + 0xfff + uVar4 * 4) >> 0xc;
  iVar10 = (int)(iVar9 + 0xfff + uVar4 * 4) >> 0xc;
  iVar8 = iVar8 >> 0xc;
  if (iVar8 < 0) {
    iVar8 = 0;
  }
  iVar9 = iVar9 >> 0xc;
  if (iVar9 < 0) {
    iVar9 = 0;
  }
  if ((int)fieldGrid->gridWidth <= iVar5) {
    iVar5 = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= iVar10) {
    iVar10 = fieldGrid->gridHeight - 1;
  }
  if ((iVar8 <= iVar5) && (iVar9 <= iVar10)) {
    iVar6 = (iVar5 - iVar8) + 1;
    iVar8 = iVar9 * fieldGrid->gridWidth + iVar8;
    iStack_24 = (iVar10 - iVar9) + 1;
    piVar12 = accumulatorPlane + iVar8;
    FVar2 = fieldGrid->gridWidth;
    iVar9 = (centerWorldYQ12 >> 0xc) * FVar2 + (centerWorldXQ12 >> 0xc);
    pFVar11 = fieldGrid->cells + iVar8;
    iVar5 = fieldGrid->cells[iVar9].worldX;
    iVar8 = fieldGrid->cells[iVar9].worldY;
    iVar9 = iVar6;
    pFStack_30 = pFVar11;
    piStack_2c = piVar12;
    do {
      do {
        dVar7 = FixedMath_Length2(pFVar11->worldY - iVar8,pFVar11->worldX - iVar5);
        if (dVar7 <= uVar4 + 1) {
          lVar3 = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)dVar7 & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)dVar7 * 0x8000 & 0xffffffffU) / (longlong)(int)(uVar4 + 1))
                   ] + 0x10000000) * (longlong)iVar1;
          iVar10 = ((int)((ulonglong)lVar3 >> 0x20) << 3 | (uint)lVar3 >> 0x1d) + *piVar12;
          if (iVar1 < 0) {
            if (iVar10 < iVar1) {
              iVar10 = iVar1;
            }
          }
          else if (iVar1 < iVar10) {
            iVar10 = iVar1;
          }
          *piVar12 = iVar10;
        }
        pFVar11 = pFVar11 + 1;
        piVar12 = piVar12 + 1;
        iVar9 = iVar9 + -1;
      } while (iVar9 != 0);
      piVar12 = piStack_2c + FVar2;
      pFVar11 = pFStack_30 + FVar2;
      iStack_24 = iStack_24 + -1;
      iVar9 = iVar6;
      pFStack_30 = pFVar11;
      piStack_2c = piVar12;
    } while (iStack_24 != 0);
  }
  return;
}


/* Address: 0x005713E0.
   Ownership: world/terrain/grid.
   Purpose: Applies one bounded field-grid cell transition and updates the associated runtime flags. EAX, ECX, and
   EDX are preserved or incidental caller state and are not synthetic parameters or normal returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplySingleCellTransition
          (FieldGridTransitionValue transitionValue,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int iVar1;
  
  gridRowIndex = worldYQ12 >> 0xc;
  if ((((-1 < gridRowIndex) && (iVar1 = worldXQ12 >> 0xc, -1 < iVar1)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (iVar1 < (int)fieldGrid->gridWidth)) {
    iVar1 = gridRowIndex * fieldGrid->gridWidth + iVar1;
    fieldGrid->cells[iVar1].flagsAndMaterial =
         fieldGrid->cells[iVar1].flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK | transitionValue;
  }
  return;
}


/* Address: 0x00571860.
   Ownership: world/terrain/grid.
   Purpose: Applies one rectangular field-grid transition through the established horizontal and vertical span
   helpers. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyRectangularTransition(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  Q12 *pQVar1;
  int iVar2;
  int iVar3;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  iVar2 = worldYQ12 >> 0xc;
  iVar3 = worldXQ12 >> 0xc;
  if ((((1 < iVar2) && (1 < iVar3)) && (iVar2 + 1 < (int)fieldGrid->gridHeight)) &&
     (iVar3 + 1 < (int)gridWidth)) {
    iVar3 = (iVar2 + -1) * gridWidth + iVar3;
    iVar2 = (fieldGrid->cells[iVar3].terrainHeight +
             *(int *)((&fieldGrid[1].common.buildMetadata.names)[iVar3].producerName + 0xc) +
             fieldGrid->cells[iVar3 + (gridWidth - 1)].terrainHeight +
             *(int *)((&fieldGrid[1].common.buildMetadata.names)[iVar3 + gridWidth].producerName +
                     0xc) + *(int *)(fieldGrid->sourcePath + iVar3 * 0x40 + gridWidth * 0x80 + 100)
            + fieldGrid->cells[iVar3 + gridWidth * 2].terrainHeight) / 6 -
            fieldGrid->cells[iVar3 + gridWidth].terrainHeight;
    pQVar1 = &fieldGrid->cells[iVar3 + gridWidth].terrainHeight;
    *pQVar1 = *pQVar1 + iVar2;
    pQVar1 = &fieldGrid->cells[iVar3 + gridWidth].waterSurfaceDelta;
    *pQVar1 = *pQVar1 - iVar2;
  }
  return;
}


/* Address: 0x004FEA50.
   Ownership: world/terrain/grid.
   Purpose: Transforms world-plane coordinates into signed Q12 grid coordinates. EAX returns columnQ12 and EDX
   returns rowQ12. The executable uses the constants 0x001C6E9C and -0x0020C8CC for the isometric inverse
   transform.
*/
FieldGridCoordinatesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx_mm0
FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX)

{
  uint gridHalfRowCoordinateQ12;
  FieldGridCoordinatesEaxEdx8 FVar1;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  FVar1.rowQ12 = gridHalfRowCoordinateQ12 * 2;
  FVar1.columnQ12 =
       ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12;
  return FVar1;
}


/* Address: 0x00571FE0.
   Ownership: world/terrain/grid.
   Purpose: EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int iVar1;
  
  gridRowIndex = worldYQ12 >> 0xc;
  if ((((-1 < gridRowIndex) && (iVar1 = worldXQ12 >> 0xc, -1 < iVar1)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (iVar1 < (int)fieldGrid->gridWidth)) {
    iVar1 = gridRowIndex * fieldGrid->gridWidth + iVar1;
    fieldGrid->cells[iVar1].flagsAndMaterial =
         preserveMask & fieldGrid->cells[iVar1].flagsAndMaterial | setMask;
  }
  return;
}


/* Address: 0x005052E0.
   Ownership: world/terrain/grid.
   Purpose: Uses the six neighboring cell positions and heights to derive fixed-point normal directions for both
   terrain triangles and stores their packed angle pairs at cell offsets +0x08 and +0x78. EAX, ECX, and EDX are
   preserved or incidental caller state and are not synthetic parameters or normal returns.
   Cross-module calls: FixedMath_VectorToAngles3Regs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_RecomputeTriangleNormalAngles
          (FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  FixedMathVectorAnglesRegs8 FVar8;
  
  iVar1 = cell[1].terrainHeight - cell->terrainHeight;
  iVar2 = *(int *)(cell->runtime60_6B + rowStrideBytes + -0x18) - cell->terrainHeight;
  iVar3 = *(int *)((int)cell + (0x48 - rowStrideBytes)) - cell->terrainHeight;
  iVar4 = cell[-1].terrainHeight - cell->terrainHeight;
  iVar5 = *(int *)(cell->runtime0C_3F + rowStrideBytes + -0x44) - cell->terrainHeight;
  iVar6 = *(int *)((int)cell + (200 - rowStrideBytes)) - cell->terrainHeight;
  FVar8 = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x1c) -
                                    cell->worldY) * iVar2) - (cell[1].worldY - cell->worldY) * iVar1
                                 ) - (*(int *)((int)cell + (0x44 - rowStrideBytes)) - cell->worldY)
                                     * iVar3) - (cell[-1].worldY - cell->worldY) * iVar4) -
                              (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x48) - cell->worldY)
                              * iVar5) -
                              (*(int *)((int)cell + (0xc4 - rowStrideBytes)) - cell->worldY) * iVar6
                     ,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x20) - cell->worldX) *
                           iVar2) - (cell[1].worldX - cell->worldX) * iVar1) -
                        (*(int *)((int)cell + (0x40 - rowStrideBytes)) - cell->worldX) * iVar3) -
                       (cell[-1].worldX - cell->worldX) * iVar4) -
                      (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x4c) - cell->worldX) * iVar5
                      ) - (*(int *)((int)cell + (0xc0 - rowStrideBytes)) - cell->worldX) * iVar6);
  cell->triangle0NormalAngles = FVar8.ecx | FVar8.edx << 0x10;
  iVar1 = -rowStrideBytes;
  iVar2 = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar3 = ((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x18) +
           *(int *)(cell->runtime60_6B + rowStrideBytes + -0x14)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar4 = ((*(int *)((int)cell + iVar1 + 0x48) + *(int *)((int)cell + iVar1 + 0x4c)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  iVar5 = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar6 = ((*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x44) +
           *(int *)(cell->runtime0C_3F + rowStrideBytes + -0x40)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar7 = ((*(int *)((int)cell + iVar1 + 200) + *(int *)((int)cell + iVar1 + 0xcc)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  FVar8 = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x1c) -
                                    cell->worldY) * iVar3) - (cell[1].worldY - cell->worldY) * iVar2
                                 ) - (*(int *)((int)cell + iVar1 + 0x44) - cell->worldY) * iVar4) -
                               (cell[-1].worldY - cell->worldY) * iVar5) -
                              (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x48) - cell->worldY)
                              * iVar6) - (*(int *)((int)cell + iVar1 + 0xc4) - cell->worldY) * iVar7
                     ,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x20) - cell->worldX) *
                           iVar3) - (cell[1].worldX - cell->worldX) * iVar2) -
                        (*(int *)((int)cell + iVar1 + 0x40) - cell->worldX) * iVar4) -
                       (cell[-1].worldX - cell->worldX) * iVar5) -
                      (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x4c) - cell->worldX) * iVar6
                      ) - (*(int *)((int)cell + iVar1 + 0xc0) - cell->worldX) * iVar7);
  cell->triangle1NormalAngles = FVar8.ecx | FVar8.edx << 0x10;
  return;
}


/* Address: 0x00505690.
   Ownership: world/terrain/grid.
   Purpose: Converts the first triangle normal angles at +0x08 to a Q28 direction, evaluates it against the shared
   light direction, selects the corresponding color-table entry, and stores the color and companion value at
   +0x58/+0x5C. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or
   normal returns.
   Cross-module calls: FixedMath_DirectionFromAnglesQ28Regs [core/math/fixed].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  FixedDirectionXZEdxEax8 triangleNormalDirectionXZQ28;
  FixedDirectionXyzRegs12 FVar1;
  PackedArgb32 directionalLightColor;
  
  FVar1 = FixedMath_DirectionFromAnglesQ28Regs
                    ((int)cell->triangle0NormalAngles >> 0x10,cell->triangle0NormalAngles & 0xffff);
  directionalLightColor =
       (&g_TerrainDirectionalLightColorLut)
       [(int)((ulonglong)((longlong)(int)FVar1.eax * (longlong)g_TerrainLightDirectionX) >> 0x20) +
        (int)((ulonglong)((longlong)(int)FVar1.ecx * (longlong)g_TerrainLightDirectionY) >> 0x20) +
        (int)((ulonglong)((longlong)(int)FVar1.edx * (longlong)g_TerrainLightDirectionZ) >> 0x20) >>
        0x10];
  cell->secondarySurfaceDirectionalLightColor5C = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor58 = directionalLightColor;
  return;
}

