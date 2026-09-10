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
void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf
               (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
               FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
               Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int extraout_EAX;
  int extraout_EAX_00;
  int extraout_EAX_01;
  int extraout_ECX;
  int iVar2;
  uint uVar3;
  int iVar4;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int iVar5;
  int iVar6;
  FieldGridCell *cell;
  FieldGridCell *cell_00;
  FieldGridCell *cell_01;
  qword qVar7;
  qword qVar8;
  FieldGridCell *pFVar9;
  FieldGridCell *pFVar10;
  
  if (0 < radiusWorldUnits) {
    uVar3 = (int)((ulonglong)((longlong)radiusWorldUnits * 0x1bb6) >> 0x20) << 0x14 |
            (uint)((longlong)radiusWorldUnits * 0x1bb6) >> 0xc;
    iVar5 = centerWorldXQ12 - uVar3;
    qVar7 = FieldGrid_WorldToGridQ12(centerWorldYQ12 + radiusWorldUnits,iVar5);
    qVar8 = FieldGrid_WorldToGridQ12(extraout_ECX + radiusWorldUnits * -2,iVar5 + uVar3 * 2);
    FVar1 = fieldGrid->gridWidth;
    iVar6 = (int)qVar7 >> 0xc;
    iVar2 = (int)((longlong)qVar7 >> 0x2c);
    iVar5 = ((int)qVar8 >> 0xc) + 1;
    iVar4 = (int)((longlong)qVar8 >> 0x2c) + 1;
    if ((int)FVar1 <= iVar5) {
      iVar5 = FVar1 - 1;
    }
    if (iVar6 < 1) {
      iVar6 = 1;
    }
    if (iVar2 < 1) {
      iVar2 = 1;
    }
    if ((int)fieldGrid->gridHeight <= iVar4) {
      iVar4 = fieldGrid->gridHeight - 1;
    }
    if ((iVar6 < iVar5) && (iVar2 < iVar4)) {
      fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
      cell = (FieldGridCell *)(fieldGrid->cells[iVar6].runtime00_07 + iVar2 * FVar1 * 0x80);
      pFVar9 = cell;
      cell_00 = cell;
      do {
        do {
          FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
                    (terrainMaterialIndexOrNegativeSentinel,radiusWorldUnits,
                     terrainHeightDeltaAmplitudeQ12,centerWorldYQ12,centerWorldXQ12,cell);
          cell = cell + 1;
        } while (extraout_EAX != 1);
        cell = pFVar9 + FVar1;
        pFVar10 = cell_00;
        pFVar9 = cell;
        cell_01 = cell_00;
      } while (extraout_EDX != 1);
      do {
        do {
          FieldGridCell_RecomputeTriangleNormalAngles(FVar1 * 0x80,cell_00);
          cell_00 = cell_00 + 1;
        } while (extraout_EAX_00 != 1);
        cell_00 = pFVar10 + FVar1;
        pFVar9 = cell_01;
        pFVar10 = cell_00;
      } while (extraout_EDX_00 != 1);
      do {
        do {
          FieldGridCell_ComputeDirectionalLightColor(cell_01);
          cell_01 = cell_01 + 1;
        } while (extraout_EAX_01 != 1);
        cell_01 = pFVar9 + FVar1;
        pFVar9 = cell_01;
      } while (extraout_EDX_01 != 1);
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
void __fastcall
TerrainGrid_RunDirectionalRelaxationPasses
          (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode)

{
  FieldGridAsset *fieldGrid;
  int extraout_ECX;
  int extraout_ECX_00;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  if ((mode & TERRAIN_RELAXATION_UNGATED_LAND_TOOL) == TERRAIN_RELAXATION_SIGN_GATED) {
    do {
      TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(fieldGrid);
    } while (extraout_ECX != 1);
  }
  else {
    do {
      TerrainGrid_RelaxNeighborHeightsForward(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverse(fieldGrid);
    } while (extraout_ECX_00 != 1);
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
void FieldGrid_ApplyPositiveCellDeltas
               (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
               PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridCell *cell;
  FieldGridAsset *fieldGrid;
  int iVar1;
  int extraout_EAX;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int iVar3;
  FieldGridHeightDeltaUnits heightDeltaUnits;
  FieldGridHeightDeltaUnits extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int rowStrideBytes;
  FieldGridRowStrideBytes rowStrideBytes_00;
  FieldGridRowStrideBytes extraout_EDX;
  int extraout_EDX_00;
  FieldGridRowStrideBytes extraout_EDX_01;
  int extraout_EDX_02;
  FieldGridRowStrideBytes extraout_EDX_03;
  int extraout_EDX_04;
  FieldGridAccumulatorValue *accumulatorPlane;
  FieldGridAccumulatorValue *extraout_EDX_05;
  FieldGridRowStrideBytes rowStrideBytes_01;
  FieldGridRowStrideBytes extraout_EDX_06;
  int extraout_EDX_07;
  FieldGridRowStrideBytes extraout_EDX_08;
  int extraout_EDX_09;
  FieldGridRowStrideBytes extraout_EDX_10;
  int extraout_EDX_11;
  int rowStrideBytes_02;
  FieldGridCell *pFVar4;
  FieldGridCell *pFVar5;
  int *piVar6;
  SelectionPlayerPairRecord *pSVar7;
  FieldGridAccumulatorValue *pFVar8;
  bool bVar9;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar6 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  iVar2 = fieldGrid->gridWidth * fieldGrid->gridHeight;
  rowStrideBytes = fieldGrid->gridWidth << 7;
  pFVar5 = fieldGrid->cells;
  iVar3 = iVar2;
  pFVar4 = pFVar5;
  accumulatorPlane = piVar6;
  rowStrideBytes_02 = rowStrideBytes;
  do {
    iVar1 = *piVar6;
    if (iVar1 != 0) {
      pFVar4->terrainHeight = pFVar4->terrainHeight - iVar1;
      pFVar4->waterSurfaceDelta = pFVar4->waterSurfaceDelta + iVar1;
      if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
        FieldGridCell_ComputeDirectionalLightColor(pFVar4);
        iVar3 = extraout_ECX;
        rowStrideBytes = rowStrideBytes_00;
        if (((pFVar4[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + -1);
          iVar3 = extraout_ECX_00;
          rowStrideBytes = extraout_EDX;
        }
        if (((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_01;
          rowStrideBytes = extraout_EDX_00;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
        if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_02;
          rowStrideBytes = extraout_EDX_01;
        }
        if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_03;
          rowStrideBytes = extraout_EDX_02;
        }
        cell = (FieldGridCell *)(pFVar4[-1].runtime00_07 + rowStrideBytes * 2);
        if ((cell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
          iVar3 = extraout_ECX_04;
          rowStrideBytes = extraout_EDX_03;
        }
        pFVar4 = cell + 1;
        if ((cell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_05;
          rowStrideBytes = extraout_EDX_04;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
      }
    }
    *piVar6 = pFVar4->terrainHeight;
    pFVar4 = pFVar4 + 1;
    bVar9 = (int *)0xfffffffb < piVar6;
    piVar6 = piVar6 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  heightDeltaUnits = (int)packedDragDeltaXY16 >> 0x10;
  if (bVar9) {
    FieldGrid_ProcessHorizontalSpan
              (anchorWorldYQ12,anchorWorldXQ12,heightDeltaUnits,(int)(short)packedDragDeltaXY16,
               anchorWorldYQ12,anchorWorldXQ12,accumulatorPlane,fieldGrid);
    pFVar8 = accumulatorPlane;
  }
  else {
    pSVar7 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    pFVar8 = accumulatorPlane;
    if (g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084 != 0) {
      do {
        FieldGrid_ProcessHorizontalSpan
                  (anchorWorldYQ12,anchorWorldXQ12,heightDeltaUnits,(int)(short)packedDragDeltaXY16,
                   pSVar7->pairValue,pSVar7->pairKey,accumulatorPlane,fieldGrid);
        pSVar7 = pSVar7 + 1;
        heightDeltaUnits = extraout_ECX_06;
        accumulatorPlane = extraout_EDX_05;
      } while (extraout_EAX != 1);
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    LOCK();
    iVar3 = *pFVar8;
    *pFVar8 = 0;
    UNLOCK();
    iVar3 = iVar3 - pFVar5->terrainHeight;
    if (iVar3 != 0) {
      pFVar5->terrainHeight = pFVar5->terrainHeight + iVar3;
      pFVar5->waterSurfaceDelta = pFVar5->waterSurfaceDelta - iVar3;
      *pFVar8 = iVar3;
      if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5);
        iVar2 = extraout_ECX_07;
        rowStrideBytes_02 = rowStrideBytes_01;
        if (((pFVar5[-1].flagsAndMaterial & 0x88006000) == 0) && (pFVar8[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_01,pFVar5 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + -1);
          iVar2 = extraout_ECX_08;
          rowStrideBytes_02 = extraout_EDX_06;
        }
        if (((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) && (pFVar8[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
          iVar2 = extraout_ECX_09;
          rowStrideBytes_02 = extraout_EDX_07;
        }
        pFVar5 = (FieldGridCell *)((int)pFVar5 - rowStrideBytes_02);
        if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
          iVar2 = extraout_ECX_10;
          rowStrideBytes_02 = extraout_EDX_08;
        }
        if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
          iVar2 = extraout_ECX_11;
          rowStrideBytes_02 = extraout_EDX_09;
        }
        pFVar4 = (FieldGridCell *)(pFVar5[-1].runtime00_07 + rowStrideBytes_02 * 2);
        if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar2 = extraout_ECX_12;
          rowStrideBytes_02 = extraout_EDX_10;
        }
        pFVar5 = pFVar4 + 1;
        if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
          iVar2 = extraout_ECX_13;
          rowStrideBytes_02 = extraout_EDX_11;
        }
        pFVar5 = (FieldGridCell *)((int)pFVar5 - rowStrideBytes_02);
      }
    }
    pFVar5 = pFVar5 + 1;
    pFVar8 = pFVar8 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
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
void FieldGrid_ApplyNegativeCellDeltas
               (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
               PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridCell *cell;
  FieldGridAsset *fieldGrid;
  int iVar1;
  int extraout_EAX;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int iVar3;
  FieldGridHeightDeltaUnits heightDeltaUnits;
  FieldGridHeightDeltaUnits extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int rowStrideBytes;
  FieldGridRowStrideBytes rowStrideBytes_00;
  FieldGridRowStrideBytes extraout_EDX;
  int extraout_EDX_00;
  FieldGridRowStrideBytes extraout_EDX_01;
  int extraout_EDX_02;
  FieldGridRowStrideBytes extraout_EDX_03;
  int extraout_EDX_04;
  FieldGridAccumulatorValue *accumulatorPlane;
  FieldGridAccumulatorValue *extraout_EDX_05;
  FieldGridRowStrideBytes rowStrideBytes_01;
  FieldGridRowStrideBytes extraout_EDX_06;
  int extraout_EDX_07;
  FieldGridRowStrideBytes extraout_EDX_08;
  int extraout_EDX_09;
  FieldGridRowStrideBytes extraout_EDX_10;
  int extraout_EDX_11;
  int rowStrideBytes_02;
  FieldGridCell *pFVar4;
  FieldGridCell *pFVar5;
  int *piVar6;
  SelectionPlayerPairRecord *pSVar7;
  FieldGridAccumulatorValue *pFVar8;
  bool bVar9;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  piVar6 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  iVar2 = fieldGrid->gridWidth * fieldGrid->gridHeight;
  rowStrideBytes = fieldGrid->gridWidth << 7;
  pFVar5 = fieldGrid->cells;
  iVar3 = iVar2;
  pFVar4 = pFVar5;
  accumulatorPlane = piVar6;
  rowStrideBytes_02 = rowStrideBytes;
  do {
    iVar1 = *piVar6;
    if (iVar1 != 0) {
      pFVar4->terrainHeight = pFVar4->terrainHeight - iVar1;
      pFVar4->waterSurfaceDelta = pFVar4->waterSurfaceDelta + iVar1;
      *piVar6 = 0;
      if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
        FieldGridCell_ComputeDirectionalLightColor(pFVar4);
        iVar3 = extraout_ECX;
        rowStrideBytes = rowStrideBytes_00;
        if (((pFVar4[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + -1);
          iVar3 = extraout_ECX_00;
          rowStrideBytes = extraout_EDX;
        }
        if (((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) && (piVar6[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_01;
          rowStrideBytes = extraout_EDX_00;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
        if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_02;
          rowStrideBytes = extraout_EDX_01;
        }
        if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
          iVar3 = extraout_ECX_03;
          rowStrideBytes = extraout_EDX_02;
        }
        cell = (FieldGridCell *)(pFVar4[-1].runtime00_07 + rowStrideBytes * 2);
        if ((cell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
          iVar3 = extraout_ECX_04;
          rowStrideBytes = extraout_EDX_03;
        }
        pFVar4 = cell + 1;
        if ((cell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar3 = extraout_ECX_05;
          rowStrideBytes = extraout_EDX_04;
        }
        pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes);
      }
    }
    pFVar4 = pFVar4 + 1;
    bVar9 = (int *)0xfffffffb < piVar6;
    piVar6 = piVar6 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  heightDeltaUnits = (int)packedDragDeltaXY16 >> 0x10;
  if (bVar9) {
    FieldGrid_ProcessVerticalSpan
              (heightDeltaUnits,(int)(short)packedDragDeltaXY16,anchorWorldYQ12,anchorWorldXQ12,
               accumulatorPlane,fieldGrid);
    pFVar8 = accumulatorPlane;
  }
  else {
    pSVar7 = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    pFVar8 = accumulatorPlane;
    if (g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084 != 0) {
      do {
        FieldGrid_ProcessVerticalSpan
                  (heightDeltaUnits,(int)(short)packedDragDeltaXY16,pSVar7->pairValue,
                   pSVar7->pairKey,accumulatorPlane,fieldGrid);
        pSVar7 = pSVar7 + 1;
        heightDeltaUnits = extraout_ECX_06;
        accumulatorPlane = extraout_EDX_05;
      } while (extraout_EAX != 1);
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    iVar3 = *pFVar8;
    if (iVar3 != 0) {
      pFVar5->terrainHeight = pFVar5->terrainHeight + iVar3;
      pFVar5->waterSurfaceDelta = pFVar5->waterSurfaceDelta - iVar3;
      if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
        FieldGridCell_ComputeDirectionalLightColor(pFVar5);
        iVar2 = extraout_ECX_07;
        rowStrideBytes_02 = rowStrideBytes_01;
        if (((pFVar5[-1].flagsAndMaterial & 0x88006000) == 0) && (pFVar8[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_01,pFVar5 + -1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + -1);
          iVar2 = extraout_ECX_08;
          rowStrideBytes_02 = extraout_EDX_06;
        }
        if (((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) && (pFVar8[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
          iVar2 = extraout_ECX_09;
          rowStrideBytes_02 = extraout_EDX_07;
        }
        pFVar5 = (FieldGridCell *)((int)pFVar5 - rowStrideBytes_02);
        if ((pFVar5->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
          iVar2 = extraout_ECX_10;
          rowStrideBytes_02 = extraout_EDX_08;
        }
        if ((pFVar5[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5 + 1);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5 + 1);
          iVar2 = extraout_ECX_11;
          rowStrideBytes_02 = extraout_EDX_09;
        }
        pFVar4 = (FieldGridCell *)(pFVar5[-1].runtime00_07 + rowStrideBytes_02 * 2);
        if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar4);
          FieldGridCell_ComputeDirectionalLightColor(pFVar4);
          iVar2 = extraout_ECX_12;
          rowStrideBytes_02 = extraout_EDX_10;
        }
        pFVar5 = pFVar4 + 1;
        if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_02,pFVar5);
          FieldGridCell_ComputeDirectionalLightColor(pFVar5);
          iVar2 = extraout_ECX_13;
          rowStrideBytes_02 = extraout_EDX_11;
        }
        pFVar5 = (FieldGridCell *)((int)pFVar5 - rowStrideBytes_02);
      }
    }
    pFVar5 = pFVar5 + 1;
    pFVar8 = pFVar8 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
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
void FieldGrid_RebuildLocalInfluenceState
               (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
               Q12 worldYQ12,Q12 worldXQ12)

{
  FieldGridCell *cell;
  FieldGridAsset *fieldGrid;
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int rowStrideBytes;
  FieldGridRowStrideBytes rowStrideBytes_00;
  FieldGridRowStrideBytes extraout_EDX_02;
  int extraout_EDX_03;
  FieldGridRowStrideBytes extraout_EDX_04;
  int extraout_EDX_05;
  FieldGridRowStrideBytes extraout_EDX_06;
  int extraout_EDX_07;
  FieldGridCell *pFVar2;
  Q12 *pQVar3;
  int *piVar4;
  undefined1 in_CF;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if ((bool)in_CF) {
    FieldGrid_ApplyRectangularTransition(worldYQ12,worldXQ12,fieldGrid);
    iVar1 = extraout_EDX_01;
  }
  else {
    pQVar3 = (Q12 *)(extraout_EDX + 0x80);
    iVar1 = extraout_EDX;
    if (*(int *)(extraout_EDX + 0x8084) != 0) {
      do {
        FieldGrid_ApplyRectangularTransition(pQVar3[1],*pQVar3,fieldGrid);
        pQVar3 = pQVar3 + 2;
        iVar1 = extraout_EDX_00;
      } while (extraout_ECX != 1);
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  piVar4 = *(int **)(iVar1 + 0x8088);
  iVar1 = fieldGrid->gridWidth * fieldGrid->gridHeight;
  rowStrideBytes = fieldGrid->gridWidth << 7;
  pFVar2 = fieldGrid->cells;
  do {
    if ((*piVar4 != pFVar2->terrainHeight) && ((pFVar2->flagsAndMaterial & 0x88006000) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar2);
      FieldGridCell_ComputeDirectionalLightColor(pFVar2);
      iVar1 = extraout_ECX_00;
      rowStrideBytes = rowStrideBytes_00;
      if (((pFVar2[-1].flagsAndMaterial & 0x88006000) == 0) && (piVar4[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar2 + -1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar2 + -1);
        iVar1 = extraout_ECX_01;
        rowStrideBytes = extraout_EDX_02;
      }
      if (((pFVar2[1].flagsAndMaterial & 0x88006000) == 0) && (piVar4[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar2 + 1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar2 + 1);
        iVar1 = extraout_ECX_02;
        rowStrideBytes = extraout_EDX_03;
      }
      pFVar2 = (FieldGridCell *)((int)pFVar2 - rowStrideBytes);
      if ((pFVar2->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar2);
        FieldGridCell_ComputeDirectionalLightColor(pFVar2);
        iVar1 = extraout_ECX_03;
        rowStrideBytes = extraout_EDX_04;
      }
      if ((pFVar2[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar2 + 1);
        FieldGridCell_ComputeDirectionalLightColor(pFVar2 + 1);
        iVar1 = extraout_ECX_04;
        rowStrideBytes = extraout_EDX_05;
      }
      cell = (FieldGridCell *)(pFVar2[-1].runtime00_07 + rowStrideBytes * 2);
      if ((cell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
        iVar1 = extraout_ECX_05;
        rowStrideBytes = extraout_EDX_06;
      }
      pFVar2 = cell + 1;
      if ((cell[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar2);
        FieldGridCell_ComputeDirectionalLightColor(pFVar2);
        iVar1 = extraout_ECX_06;
        rowStrideBytes = extraout_EDX_07;
      }
      pFVar2 = (FieldGridCell *)((int)pFVar2 - rowStrideBytes);
    }
    pFVar2 = pFVar2 + 1;
    piVar4 = piVar4 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x00505620.
   Ownership: world/terrain/grid.
   Purpose: Marks the field dirty and recomputes both packed terrain-triangle normal angles for every non-boundary
   cell. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
   Local calls: FieldGridCell_RecomputeTriangleNormalAngles.
*/
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

{
  int extraout_ECX;
  int extraout_EDX;
  FieldGridCell *cell;
  int rowStrideBytes;
  FieldGridCell *cellCursor;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowStrideBytes = fieldGrid->gridWidth * 0x80;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 rowStrideBytes + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        cellCursor = cell + 1;
      } while (extraout_ECX != 1);
      cellCursor = cell + 3;
    } while (extraout_EDX != 1);
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
void FieldGrid_RecomputeInteriorDirectionalLighting
               (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,
               FieldGridAsset *fieldGrid)

{
  int extraout_ECX;
  int extraout_EDX;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;
  
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_TerrainLightDirectionX,lightElevationAngle,lightAzimuthAngle);
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 fieldGrid->gridWidth * 0x80 + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        cellCursor = cell + 1;
      } while (extraout_ECX != 1);
      cellCursor = cell + 3;
    } while (extraout_EDX != 1);
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
void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
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
  qword qVar10;
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
    qVar10 = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    uVar2 = (int)qVar10 >> 0xc;
    uVar11 = (uint)((longlong)qVar10 >> 0x2c);
    uVar3 = (uint)(qVar10 & 0xfff00000fff);
    uVar6 = (uint)((qVar10 & 0xfff00000fff) >> 0x20);
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
      pFVar8 = (FieldGridCell *)(cell[-1].runtime00_07 + iVar4);
      TerrainHeightDelta_ApplyWedge2(0,cell);
      pFVar9 = (FieldGridCell *)(pFVar8->runtime00_07 + iVar4);
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
void FieldGrid_ApplyLocalCellUpdate
               (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,
               Q12 worldYQ12,Q12 worldXQ12)

{
  FieldGridAsset *fieldGrid;
  int extraout_ECX;
  int extraout_EDX;
  Q12 *pQVar1;
  undefined1 in_CF;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if ((bool)in_CF) {
    FieldGrid_ApplySingleCellTransition(transitionValue,worldYQ12,worldXQ12,fieldGrid);
  }
  else {
    pQVar1 = (Q12 *)(extraout_EDX + 0x80);
    if (*(int *)(extraout_EDX + 0x8084) != 0) {
      do {
        FieldGrid_ApplySingleCellTransition(transitionValue,pQVar1[1],*pQVar1,fieldGrid);
        pQVar1 = pQVar1 + 2;
      } while (extraout_ECX != 1);
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
void FieldGrid_ApplyEncodedCellUpdate
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
void FieldGrid_ApplyMaskDFFFFFFF
               (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,
               Q12 worldXQ12)

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
void FieldGrid_ApplyMaskBFFFFFFF
               (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,
               Q12 worldXQ12)

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
void FieldGrid_ApplyCallerMask
               (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,
               Q12 worldYQ12,Q12 worldXQ12)

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
qword FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int iVar1;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Q12 terrainHeightQ12;
  
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
  }
  else {
    worldX = field->cells[iVar1].worldX;
    terrainHeightQ12 = field->cells[iVar1].terrainHeight;
  }
  return CONCAT44(terrainHeightQ12,worldX);
}

/* Address: 0x004FEB10.
   Ownership: world/terrain/grid.
   Purpose: Nearest-cell companion that returns worldX in EAX, worldY in ECX, and terrainHeight + waterSurfaceDelta
   in EDX. CF reports bounds success.
*/
qword FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int iVar1;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
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
  }
  else {
    worldX = field->cells[iVar1].worldX;
    iVar1 = field->cells[iVar1].waterSurfaceDelta + field->cells[iVar1].terrainHeight;
  }
  return CONCAT44(iVar1,worldX);
}

/* Address: 0x004FEBA0.
   Ownership: world/terrain/grid.
   Purpose: Rounds to a cell and returns its signed waterSurfaceDelta. Callers use this only with coordinates
   expected to be inside the field; the out-of-range EAX value is not a defined result. Typed parameters: p0
   worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains were explicitly deferred. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
sdword FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

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
Q12 FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

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
      if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime58_6F + iVar4 + -8) & 0x88006000) ==
           0) && ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                  [gridColumnIndex + gridWidth].producerName + iVar4 + 0x20) &
                  0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
        if (uVar2 + uVar3 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + iVar4 + 0x18) * (longlong)(int)uVar2 +
               ((longlong)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar4 + -0x10)
                * (longlong)(int)uVar3 -
               (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime58_6F + iVar4 + -0x10) *
               (longlong)triangleDiagonalWeightQ12);
          return (uint)weightedHeightAccumulator >> 0xc |
                 (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
        }
        lVar1 = (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                              [gridColumnIndex + gridWidth].producerName + iVar4 + 0x18) *
                (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar4 + -0x10
                         ) * (longlong)(int)(uVar2 - 0x1000) +
                (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                              producerName + iVar4 + 0x18) * (longlong)(int)(uVar3 - 0x1000));
        return (uint)lVar1 >> 0xc | (int)((ulonglong)lVar1 >> 0x20) << 0x14;
      }
    }
  }
  return 0;
}

/* Address: 0x004FED50.
   Ownership: world/terrain/grid.
   Purpose: Barycentrically interpolates FieldGridCell.waterSurfaceDelta over the same two-triangle grid square. CF
   clear means success. Typed parameters: p0 worldX→Q12, p1 worldY→Q12. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
sdword FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

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
      if (((*(uint *)(field->cells[gridColumnIndex].runtime58_6F + iVar4 + -8) & 0x88006000) == 0)
         && ((*(uint *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                             producerName + iVar4 + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
        if (uVar2 + uVar3 < 0x1000) {
          weightedWaterDeltaAccumulator =
               (longlong)
               *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                       iVar4 + 0x1c) * (longlong)(int)uVar2 +
               ((longlong)
                *(int *)(field->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar4 + -0xc) *
                (longlong)(int)uVar3 -
               (longlong)*(int *)(field->cells[gridColumnIndex].runtime58_6F + iVar4 + -0xc) *
               (longlong)triangleDiagonalWeightQ12);
          return (uint)weightedWaterDeltaAccumulator >> 0xc |
                 (int)((ulonglong)weightedWaterDeltaAccumulator >> 0x20) << 0x14;
        }
        lVar1 = (longlong)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                              producerName + iVar4 + 0x1c) * (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(field->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar4 + -0xc) *
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
Q12 FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int cellColumn;
  Q12 gridRowFixedQ12;
  uint rowFractionQ12;
  Q12 gridColumnFixedQ12;
  uint columnFractionQ12;
  int iVar1;
  Q12 upperTriangleWeightQ12;
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
  if (((-1 < cellColumn) && (iVar1 = gridColumnFixedQ12 * 2 >> 0xc, -1 < iVar1)) &&
     (cellColumn < (int)gridWidth)) {
    if (iVar1 < (int)fieldGrid->gridHeight) {
      iVar1 = iVar1 * gridWidth * 0x80;
      rowFractionQ12 = gridRowFixedQ12 & 0xfff;
      columnFractionQ12 = gridColumnFixedQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[cellColumn].runtime58_6F + iVar1 + -8) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                          producerName + iVar1 + 0x20) & 0x88006000) == 0)) {
        upperTriangleWeightQ12 = (rowFractionQ12 + columnFractionQ12) - 0x1000;
        if (rowFractionQ12 + columnFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        iVar1 + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       iVar1 + 0x1c)) * (longlong)(int)rowFractionQ12 +
               ((longlong)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime58_6F + iVar1 + -0x10) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime58_6F + iVar1 + -0xc)) *
                (longlong)(int)columnFractionQ12 -
               (longlong)
               (*(int *)(fieldGrid->cells[cellColumn].runtime58_6F + iVar1 + -0x10) +
               *(int *)(fieldGrid->cells[cellColumn].runtime58_6F + iVar1 + -0xc)) *
               (longlong)upperTriangleWeightQ12);
          return (uint)weightedHeightAccumulator >> 0xc |
                 (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
        }
        upperTriangleWeightedHeightAccumulator =
             (longlong)
             (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                            producerName + iVar1 + 0x18) +
             *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                           producerName + iVar1 + 0x1c)) * (longlong)upperTriangleWeightQ12 -
             ((longlong)
              (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime58_6F + iVar1 + -0x10) +
              *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime58_6F + iVar1 + -0xc)) *
              (longlong)(int)(rowFractionQ12 - 0x1000) +
             (longlong)
             (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                      iVar1 + 0x18) +
             *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                     iVar1 + 0x1c)) * (longlong)(int)(columnFractionQ12 - 0x1000));
        return (uint)upperTriangleWeightedHeightAccumulator >> 0xc |
               (int)((ulonglong)upperTriangleWeightedHeightAccumulator >> 0x20) << 0x14;
      }
    }
  }
  return 0;
}

/* Address: 0x004FEFF0.
   Ownership: world/terrain/grid.
   Purpose: EAX carries the Q12 result; CF reports failure. EDX is pushed and restored by the body because the sole
   indirect caller carries ModelRuntimeClassId through the call. Five-entry field-grid interpolation table
   callback; EAX is Q12 and CF reports failure.
*/
Q12 FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  longlong lVar1;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint uVar2;
  uint gridRowCoordinateQ12;
  uint uVar3;
  uint terrainHeightQ12;
  uint uVar4;
  int iVar5;
  int triangleDiagonalWeightQ12;
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
  if ((((-1 < gridColumnIndex) && (iVar5 = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < iVar5)) &&
      (gridColumnIndex < (int)gridWidth)) && (iVar5 < (int)fieldGrid->gridHeight)) {
    iVar5 = iVar5 * gridWidth * 0x80;
    uVar2 = gridColumnCoordinateQ12 & 0xfff;
    uVar3 = gridRowCoordinateQ12 * 2 & 0xfff;
    if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime58_6F + iVar5 + -8) & 0x88006000) == 0)
       && ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                           producerName + iVar5 + 0x20) & 0x88006000) == 0)) {
      triangleDiagonalWeightQ12 = (uVar2 + uVar3) - 0x1000;
      if (0xfff < uVar2 + uVar3) {
        lVar1 = (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                              [gridColumnIndex + gridWidth].producerName + iVar5 + 0x18) *
                (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar5 + -0x10
                         ) * (longlong)(int)(uVar2 - 0x1000) +
                (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                              producerName + iVar5 + 0x18) * (longlong)(int)(uVar3 - 0x1000));
        uVar4 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
        lVar1 = (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                              [gridColumnIndex + gridWidth].producerName + iVar5 + 0x1c) *
                (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar5 + -0xc)
                 * (longlong)(int)(uVar2 - 0x1000) +
                (longlong)
                *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                              producerName + iVar5 + 0x1c) * (longlong)(int)(uVar3 - 0x1000));
        uVar2 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
        if ((int)uVar2 < 0) {
          return uVar4;
        }
        return uVar4 + uVar2;
      }
      weightedSurfaceAccumulator =
           (longlong)
           *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                   iVar5 + 0x18) * (longlong)(int)uVar2 +
           ((longlong)
            *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar5 + -0x10) *
            (longlong)(int)uVar3 -
           (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime58_6F + iVar5 + -0x10) *
           (longlong)triangleDiagonalWeightQ12);
      terrainHeightQ12 =
           (int)((ulonglong)weightedSurfaceAccumulator >> 0x20) << 0x14 |
           (uint)weightedSurfaceAccumulator >> 0xc;
      lVar1 = (longlong)
              *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].producerName
                      + iVar5 + 0x1c) * (longlong)(int)uVar2 +
              ((longlong)
               *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime58_6F + iVar5 + -0xc) *
               (longlong)(int)uVar3 -
              (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime58_6F + iVar5 + -0xc) *
              (longlong)triangleDiagonalWeightQ12);
      uVar2 = (int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc;
      if ((int)uVar2 < 0) {
        return terrainHeightQ12;
      }
      return terrainHeightQ12 + uVar2;
    }
  }
  return 0;
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
qword FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  uint extraout_ECX_06;
  uint uVar7;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar8;
  int iVar9;
  int scale;
  FixedDirectionXZEdxEax8 FVar10;
  FixedDirectionXZEdxEax8 FVar11;
  int iVar12;
  
  uVar7 = (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  uVar6 = ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - uVar7;
  uVar7 = uVar7 * 2;
  FVar1 = field->gridWidth;
  iVar4 = (int)uVar6 >> 0xc;
  if (((-1 < iVar4) && (iVar8 = (int)uVar7 >> 0xc, -1 < iVar8)) && (iVar4 < (int)FVar1)) {
    if (iVar8 < (int)field->gridHeight) {
      iVar8 = iVar8 * FVar1 * 0x80;
      iVar9 = iVar8 + iVar4 * 0x80;
      uVar6 = uVar6 & 0xfff;
      uVar7 = uVar7 & 0xfff;
      if (((*(uint *)(field->cells[iVar4].runtime58_6F + iVar8 + -8) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&field[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar8 + 0x20) & 0x88006000) == 0)) {
        scale = (uVar6 + uVar7) - 0x1000;
        if (uVar6 + uVar7 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x18) * (longlong)(int)uVar6 +
                  ((longlong)*(int *)(field->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0x10) *
                   (longlong)(int)uVar7 -
                  (longlong)*(int *)(field->cells[iVar4].runtime58_6F + iVar8 + -0x10) *
                  (longlong)scale);
          uVar2 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar9 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6);
          uVar6 = *(uint *)(field->cells[iVar4].runtime0C_3F + iVar8 + -4);
          iVar12 = extraout_ECX;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,scale);
          iVar9 = (int)FVar10 - (int)FVar11;
          iVar12 = iVar12 - extraout_ECX_00;
          uVar6 = *(uint *)(field->cells[iVar4 + FVar1].runtime0C_3F + iVar8 + -4);
          iVar4 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7);
          FixedMath_VectorToAngles3Regs
                    (iVar4 + (int)(FVar10 >> 0x20),iVar12 + extraout_ECX_01,iVar9 + (int)FVar10);
          uVar7 = extraout_EDX << 0x10 | extraout_ECX_02 & 0xffff;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                          iVar8 + 0x18) * (longlong)scale -
                  ((longlong)*(int *)(field->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0x10) *
                   (longlong)(int)(uVar6 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x18) * (longlong)(int)(uVar7 - 0x1000));
          uVar2 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           FVar1 * 0x80 + iVar9 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,scale);
          uVar2 = *(uint *)(field->cells[iVar4 + FVar1].runtime0C_3F + iVar8 + -4);
          iVar8 = extraout_ECX_03;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6 - 0x1000);
          iVar4 = (int)FVar10 - (int)FVar11;
          iVar8 = iVar8 - extraout_ECX_04;
          uVar6 = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar9 + -0x20);
          iVar9 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7 - 0x1000);
          FixedMath_VectorToAngles3Regs
                    (iVar9 - (int)(FVar10 >> 0x20),iVar8 - extraout_ECX_05,iVar4 - (int)FVar10);
          uVar7 = extraout_EDX_00 << 0x10 | extraout_ECX_06 & 0xffff;
        }
        goto FieldGrid_InterpolateTerrainHeightAndNormal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
FieldGrid_InterpolateTerrainHeightAndNormal_ReturnInterpolatedOrDefaultSample:
  return CONCAT44(uVar7,uVar5);
}

/* Address: 0x004FF3D0.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
undefined8
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  uint extraout_ECX_06;
  uint uVar7;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar8;
  int iVar9;
  int scale;
  FixedDirectionXZEdxEax8 FVar10;
  FixedDirectionXZEdxEax8 FVar11;
  int iVar12;
  
  uVar7 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar6 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar7;
  uVar7 = uVar7 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar6 >> 0xc;
  if (((-1 < iVar4) && (iVar8 = (int)uVar7 >> 0xc, -1 < iVar8)) && (iVar4 < (int)FVar1)) {
    if (iVar8 < (int)fieldGrid->gridHeight) {
      iVar8 = iVar8 * FVar1 * 0x80;
      iVar9 = iVar8 + iVar4 * 0x80;
      uVar6 = uVar6 & 0xfff;
      uVar7 = uVar7 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime58_6F + iVar8 + -8) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar8 + 0x20) & 0x88006000) == 0)) {
        scale = (uVar6 + uVar7) - 0x1000;
        if (uVar6 + uVar7 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x1c) * (longlong)(int)uVar6 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0xc) *
                   (longlong)(int)uVar7 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime58_6F + iVar8 + -0xc) *
                  (longlong)scale);
          uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar9 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6);
          uVar6 = *(uint *)(fieldGrid->cells[iVar4].runtime0C_3F + iVar8 + -4);
          iVar12 = extraout_ECX;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,scale);
          iVar9 = (int)FVar10 - (int)FVar11;
          iVar12 = iVar12 - extraout_ECX_00;
          uVar6 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar8 + -4);
          iVar4 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7);
          FixedMath_VectorToAngles3Regs
                    (iVar4 + (int)(FVar10 >> 0x20),iVar12 + extraout_ECX_01,iVar9 + (int)FVar10);
          uVar7 = extraout_EDX << 0x10 | extraout_ECX_02 & 0xffff;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar8 + 0x1c) * (longlong)scale -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0xc) *
                   (longlong)(int)(uVar6 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x1c) * (longlong)(int)(uVar7 - 0x1000));
          uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           FVar1 * 0x80 + iVar9 + -0x20);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,scale);
          uVar2 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar8 + -4);
          iVar8 = extraout_ECX_03;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6 - 0x1000);
          iVar4 = (int)FVar10 - (int)FVar11;
          iVar8 = iVar8 - extraout_ECX_04;
          uVar6 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           iVar9 + -0x20);
          iVar9 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7 - 0x1000);
          FixedMath_VectorToAngles3Regs
                    (iVar9 - (int)(FVar10 >> 0x20),iVar8 - extraout_ECX_05,iVar4 - (int)FVar10);
          uVar7 = extraout_EDX_00 << 0x10 | extraout_ECX_06 & 0xffff;
        }
        goto FieldGrid_InterpolateTerrainHeightAndTriangle0Normal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal_ReturnInterpolatedOrDefaultSample:
  return CONCAT44(uVar7,uVar5);
}

/* Address: 0x004FF600.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
undefined8
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  uint extraout_ECX_06;
  uint uVar7;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar8;
  int iVar9;
  FixedDirectionXZEdxEax8 FVar10;
  FixedDirectionXZEdxEax8 FVar11;
  int iVar12;
  
  uVar7 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar6 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar7;
  uVar7 = uVar7 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar6 >> 0xc;
  if (((-1 < iVar4) && (iVar8 = (int)uVar7 >> 0xc, -1 < iVar8)) && (iVar4 < (int)FVar1)) {
    if (iVar8 < (int)fieldGrid->gridHeight) {
      iVar8 = iVar8 * FVar1 * 0x80;
      uVar6 = uVar6 & 0xfff;
      uVar7 = uVar7 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime58_6F + iVar8 + -8) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar8 + 0x20) & 0x88006000) == 0)) {
        iVar9 = (uVar6 + uVar7) - 0x1000;
        if (uVar6 + uVar7 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x1c) * (longlong)(int)uVar6 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0xc) *
                   (longlong)(int)uVar7 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime58_6F + iVar8 + -0xc) *
                  (longlong)iVar9);
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar8 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6);
          uVar6 = *(uint *)(fieldGrid->cells[iVar4].runtime58_6F + iVar8 + 0x20);
          iVar12 = extraout_ECX;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,iVar9);
          iVar9 = (int)FVar10 - (int)FVar11;
          iVar12 = iVar12 - extraout_ECX_00;
          uVar6 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + 0x20);
          iVar4 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7);
          FixedMath_VectorToAngles3Regs
                    (iVar4 + (int)(FVar10 >> 0x20),iVar12 + extraout_ECX_01,iVar9 + (int)FVar10);
          uVar7 = extraout_EDX << 0x10 | extraout_ECX_02 & 0xffff;
        }
        else {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar8 + 0x1c) * (longlong)iVar9 -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + -0xc) *
                   (longlong)(int)(uVar6 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar8 + 0x1c) * (longlong)(int)(uVar7 - 0x1000));
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                 sourceName + iVar8 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs((int)uVar2 >> 0x10,uVar2 & 0xffff,iVar9);
          uVar2 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar8 + 0x20);
          iVar12 = extraout_ECX_03;
          FVar11 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6 - 0x1000);
          iVar9 = (int)FVar10 - (int)FVar11;
          iVar12 = iVar12 - extraout_ECX_04;
          uVar6 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar8 + 8);
          iVar4 = (int)(FVar10 >> 0x20) - (int)(FVar11 >> 0x20);
          FVar10 = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar7 - 0x1000);
          FixedMath_VectorToAngles3Regs
                    (iVar4 - (int)(FVar10 >> 0x20),iVar12 - extraout_ECX_05,iVar9 - (int)FVar10);
          uVar7 = extraout_EDX_00 << 0x10 | extraout_ECX_06 & 0xffff;
        }
        goto FieldGrid_InterpolateTerrainHeightAndTriangle1Normal_ReturnInterpolatedOrDefaultSample;
      }
    }
  }
  uVar5 = 0;
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal_ReturnInterpolatedOrDefaultSample:
  return CONCAT44(uVar7,uVar5);
}

/* Address: 0x004FF830.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid sample interpolated terrain height and normal angles carry-flag result register
   result.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
undefined8
FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesCfRegs
          (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  uint uVar2;
  longlong lVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  uint extraout_ECX_06;
  int iVar7;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int extraout_ECX_09;
  uint extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  uint extraout_ECX_14;
  uint uVar8;
  int extraout_EDX;
  int extraout_EDX_00;
  int scale;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int iVar9;
  int iVar10;
  int iVar11;
  FixedDirectionXZEdxEax8 FVar12;
  FixedDirectionXZEdxEax8 FVar13;
  
  uVar8 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  uVar6 = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar8;
  uVar8 = uVar8 * 2;
  FVar1 = fieldGrid->gridWidth;
  iVar4 = (int)uVar6 >> 0xc;
  if (((-1 < iVar4) && (iVar9 = (int)uVar8 >> 0xc, -1 < iVar9)) && (iVar4 < (int)FVar1)) {
    if (iVar9 < (int)fieldGrid->gridHeight) {
      iVar9 = iVar9 * FVar1 * 0x80;
      iVar10 = iVar9 + iVar4 * 0x80;
      uVar6 = uVar6 & 0xfff;
      uVar8 = uVar8 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[iVar4].runtime58_6F + iVar9 + -8) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].producerName +
                    iVar9 + 0x20) & 0x88006000) == 0)) {
        iVar11 = (uVar6 + uVar8) - 0x1000;
        if (uVar6 + uVar8 < 0x1000) {
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar9 + 0x1c) * (longlong)(int)uVar6 +
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar9 + -0xc) *
                   (longlong)(int)uVar8 -
                  (longlong)*(int *)(fieldGrid->cells[iVar4].runtime58_6F + iVar9 + -0xc) *
                  (longlong)iVar11);
          uVar2 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                           iVar9 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          if ((int)uVar5 < 0) {
            uVar2 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             iVar10 + -0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6);
            uVar6 = *(uint *)(fieldGrid->cells[iVar4].runtime0C_3F + iVar9 + -4);
            iVar7 = extraout_ECX_03;
            FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar6 >> 0x10,uVar6 & 0xffff,iVar11);
            iVar10 = (int)FVar12 - (int)FVar13;
            iVar7 = iVar7 - extraout_ECX_04;
            uVar6 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar9 + -4);
            iVar4 = (int)(FVar12 >> 0x20) - (int)(FVar13 >> 0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar8);
            FixedMath_VectorToAngles3Regs
                      (iVar4 + (int)(FVar12 >> 0x20),iVar7 + extraout_ECX_05,iVar10 + (int)FVar12);
            uVar8 = extraout_EDX_00 << 0x10 | extraout_ECX_06 & 0xffff;
          }
          else {
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar2 >> 0x10,uVar2 & 0xffff,uVar6);
            uVar6 = *(uint *)(fieldGrid->cells[iVar4].runtime58_6F + iVar9 + 0x20);
            iVar7 = extraout_ECX;
            FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar6 >> 0x10,uVar6 & 0xffff,iVar11);
            iVar10 = (int)FVar12 - (int)FVar13;
            iVar7 = iVar7 - extraout_ECX_00;
            uVar6 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar9 + 0x20);
            iVar4 = (int)(FVar12 >> 0x20) - (int)(FVar13 >> 0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar6 >> 0x10,uVar6 & 0xffff,uVar8);
            FixedMath_VectorToAngles3Regs
                      (iVar4 + (int)(FVar12 >> 0x20),iVar7 + extraout_ECX_01,iVar10 + (int)FVar12);
            uVar8 = extraout_EDX << 0x10 | extraout_ECX_02 & 0xffff;
          }
        }
        else {
          iVar7 = uVar6 - 0x1000;
          scale = uVar8 - 0x1000;
          lVar3 = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                producerName + iVar9 + 0x1c) * (longlong)iVar11 -
                  ((longlong)*(int *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar9 + -0xc) *
                   (longlong)iVar7 +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].producerName +
                          iVar9 + 0x1c) * (longlong)scale);
          uVar8 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4 + FVar1].
                                 sourceName + iVar9 + 8);
          uVar5 = (uint)lVar3 >> 0xc | (int)((ulonglong)lVar3 >> 0x20) << 0x14;
          if ((int)uVar5 < 0) {
            uVar8 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             FVar1 * 0x80 + iVar10 + -0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,iVar11);
            uVar8 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime0C_3F + iVar9 + -4);
            iVar9 = extraout_ECX_11;
            FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,iVar7);
            iVar4 = (int)FVar12 - (int)FVar13;
            iVar9 = iVar9 - extraout_ECX_12;
            uVar8 = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             iVar10 + -0x20);
            iVar10 = (int)(FVar12 >> 0x20) - (int)(FVar13 >> 0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,scale);
            FixedMath_VectorToAngles3Regs
                      (iVar10 - (int)(FVar12 >> 0x20),iVar9 - extraout_ECX_13,iVar4 - (int)FVar12);
            uVar8 = extraout_EDX_02 << 0x10 | extraout_ECX_14 & 0xffff;
          }
          else {
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,iVar11);
            uVar8 = *(uint *)(fieldGrid->cells[iVar4 + FVar1].runtime58_6F + iVar9 + 0x20);
            iVar11 = extraout_ECX_07;
            FVar13 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,iVar7);
            iVar10 = (int)FVar12 - (int)FVar13;
            iVar11 = iVar11 - extraout_ECX_08;
            uVar8 = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[iVar4].sourceName +
                             iVar9 + 8);
            iVar4 = (int)(FVar12 >> 0x20) - (int)(FVar13 >> 0x20);
            FVar12 = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)uVar8 >> 0x10,uVar8 & 0xffff,scale);
            FixedMath_VectorToAngles3Regs
                      (iVar4 - (int)(FVar12 >> 0x20),iVar11 - extraout_ECX_09,iVar10 - (int)FVar12);
            uVar8 = extraout_EDX_01 << 0x10 | extraout_ECX_10 & 0xffff;
          }
        }
        goto LAB_004ffb6c;
      }
    }
  }
  uVar5 = 0;
LAB_004ffb6c:
  return CONCAT44(uVar8,uVar5);
}

/* Address: 0x004FFB80.
   Ownership: world/terrain/grid.
   Purpose: Converts a world-space point to field-grid row and column coordinates, bounds-checks the cell, and
   returns carry clear when the cell material and state byte intersects mask 0xF9. EAX, ECX, and EDX are preserved
   or incidental caller state and are not synthetic parameters or normal returns. CF=0 reports blocked/matching
   state; CF=1 reports outside or clear.
*/
void FieldGrid_TestWorldPointBlockedCf
               (FieldGridByteOffset stateByteOffset,Q12 worldYQ12,Q12 worldXQ12,
               FieldGridAsset *fieldGrid)

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
      ((fieldGrid->cells[fieldGrid->gridWidth * gridRowIndex + gridColumnIndex].runtime58_6F
        [stateByteOffset + 0x18] & 0xf9) != 0)))) {
    return;
  }
  return;
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
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

{
  dword randomValue;
  dword materialVariantRandomBits;
  int extraout_ECX;
  FieldGridDimension gridWidth;
  int extraout_EDX;
  FieldGridDimension rowsRemaining;
  FieldGridDimension topRowCellsRemaining;
  FieldGridCell *initializationCellCursor;
  int iVar1;
  FieldGridCell *cellCursor;
  FieldGridCell *currentRowFirstCell;
  Q12 currentCellWorldXQ12;
  Q12 currentCellWorldYQ12;
  undefined4 phaseSeedBitWidth;
  
  phaseSeedBitWidth = *(undefined4 *)((int)g_TerrainSurfacePacketTablePayload + -0x20);
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  initializationCellCursor = fieldGrid->cells;
  do {
    do {
      currentCellWorldXQ12 = initializationCellCursor->worldX;
      currentCellWorldYQ12 = initializationCellCursor->worldY;
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial & 0x77ff1fff;
      randomValue = Random_NextPrimary();
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      *(dword *)initializationCellCursor->runtime00_07 =
           randomValue & (1 << ((byte)phaseSeedBitWidth & 0x1f)) - 1U;
      initializationCellCursor->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 +
           (currentCellWorldYQ12 & 0xfU) + (currentCellWorldXQ12 & 0xfU) * 0x10);
      initializationCellCursor->runtime58_6F[0x14] = 0;
      initializationCellCursor->runtime58_6F[0x15] = 0;
      initializationCellCursor->runtime58_6F[0x16] = 0;
      initializationCellCursor->runtime58_6F[0x17] = 0;
      materialVariantRandomBits = Random_NextPrimary();
      initializationCellCursor->runtime00_07[4] = 0xff;
      initializationCellCursor->runtime00_07[5] = 0xff;
      initializationCellCursor->runtime00_07[6] = 0xff;
      initializationCellCursor->runtime00_07[7] = 0xff;
      initializationCellCursor->flagsAndMaterial =
           initializationCellCursor->flagsAndMaterial |
           materialVariantRandomBits & FIELD_CELL_RANDOM_VARIANT_MASK;
      initializationCellCursor = initializationCellCursor + 1;
    } while (extraout_ECX != 1);
  } while (extraout_EDX != 1);
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
    iVar1 = (int)cellCursor - (int)currentRowFirstCell;
    currentRowFirstCell = (FieldGridCell *)((int)currentRowFirstCell + iVar1);
    cellCursor = (FieldGridCell *)(iVar1 + (int)currentRowFirstCell);
    rowsRemaining = rowsRemaining - 1;
  } while (rowsRemaining != 0);
  iVar1 = (int)currentRowFirstCell * 2 - (int)cellCursor;
  do {
    *(uint *)(iVar1 + 0x50) = *(uint *)(iVar1 + 0x50) | 0x80000000;
    iVar1 = iVar1 + 0x80;
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
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

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
void FieldGrid_ApplyByteClampLookupToCells
               (FieldGridByteOffset sourceChannelOffset,FieldGridAsset *fieldGrid)

{
  undefined3 uVar1;
  undefined4 uVar2;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  byte mappedRuntimeByte;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  uVar2 = g_TerrainByteClampLookup;
  columnsRemaining = gridWidth;
  do {
    do {
      uVar1 = CONCAT21((short)((uint)uVar2 >> 0x10),
                       currentCell->runtime58_6F[sourceChannelOffset + 0x18]);
      mappedRuntimeByte = *(byte *)CONCAT31(uVar1,currentCell->runtime58_6F[0x10]);
      uVar2 = CONCAT31(uVar1,mappedRuntimeByte);
      currentCell->runtime58_6F[0x10] = mappedRuntimeByte;
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
void FieldGrid_ClassifyCellFlagsToRuntimeByte
               (FieldGridByteOffset cellByteOffset,FieldGridAsset *fieldGrid)

{
  byte classifiedRuntimeByte;
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    classifiedRuntimeByte = 0xff;
    if (((currentCell->runtime58_6F[cellByteOffset + 0x18] & 0x79) == 0) &&
       (classifiedRuntimeByte = 0x87, (currentCell->runtime58_6F[cellByteOffset + 0x18] & 0x80) == 0
       )) {
      classifiedRuntimeByte = 0;
    }
    currentCell->runtime58_6F[0x10] = classifiedRuntimeByte;
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
void __fastcall
TerrainDirectionTable_AdvanceAndRebuildVectors(undefined4 param_1,undefined4 param_2)

{
  int extraout_ECX;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCosEdxEax8 scaledSinCosPair;
  FixedSinCosEdxEax8 angleBScaledSinCosPair;
  uint packedAnglesBeforeAdvance;
  
  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 0x100;
  do {
    packedAnglesBeforeAdvance = currentDirectionRecord->packedAngleA_low16_AngleB_high16;
    currentDirectionRecord->packedAngleA_low16_AngleB_high16 =
         currentDirectionRecord->packedAngleA_low16_AngleB_high16 +
         *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair =
         FixedMath_SinCosScaled(packedAnglesBeforeAdvance & 0xffff,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = (int)scaledSinCosPair;
    currentDirectionRecord->angleAComponent1ScaledQ28 = (int)(scaledSinCosPair >> 0x20);
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled(extraout_ECX >> 0x10,currentDirectionRecord->scaleB);
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
ulonglong FieldGrid_RaycastTerrainSurfaceDistanceCf
                    (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,
                    Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  byte *pbVar1;
  longlong lVar2;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int extraout_ECX;
  int rayEndCoord1Q12;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  FixedDirectionXZEdxEax8 FVar11;
  longlong lVar12;
  undefined8 uVar13;
  
  uVar7 = (fieldGrid->gridWidth - 1) * 0x1000;
  uVar3 = (fieldGrid->gridHeight - 1) * 0x1000;
  uVar4 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - uVar4;
  rayStartCoord0Q12 = uVar4 * 2;
  iVar9 = fieldGrid->gridWidth * 0x80;
  pbVar1 = fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime00_07 +
           ((int)rayStartCoord0Q12 >> 0xc) * iVar9;
  FVar11 = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  lVar12 = (longlong)((int)FVar11 + rayOriginYQ12) * 0x1c6e9c;
  lVar2 = (longlong)(extraout_ECX + rayOriginXQ12) * -0x20c8cc;
  uVar5 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar12 >> 0x20) << 0xc | (uint)lVar12 >> 0x14) - uVar5;
  rayEndCoord0Q12 = uVar5 * 2;
  iVar8 = 0x400;
  uVar5 = rayStartCoord1Q12 & 0xfffff000;
  uVar6 = rayStartCoord0Q12 & 0xfffff000;
  do {
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) break;
    bVar10 = false;
    if ((((-1 < (int)uVar5) && (bVar10 = false, -1 < (int)uVar6)) &&
        (bVar10 = uVar5 < uVar7, (int)uVar5 < (int)uVar7)) &&
       (bVar10 = uVar6 < uVar3, (int)uVar6 < (int)uVar3)) {
      bVar10 = uVar6 < rayStartCoord0Q12;
      lVar12 = TerrainTriangle_IntersectRayDistanceCf
                         ((Q12)(FVar11 >> 0x20),rayEndCoord0Q12 + uVar4 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          *(Q12 *)(pbVar1 + iVar9 + 200),*(Q12 *)(pbVar1 + iVar9 + 0x48),
                          *(Q12 *)(pbVar1 + 200),*(Q12 *)(pbVar1 + 0x48),uVar6 + uVar4 * -2,
                          uVar5 - rayStartCoord1Q12);
      uVar6 = (uint)((ulonglong)lVar12 >> 0x20);
      if (!bVar10) {
        return CONCAT44(*(FieldCellPackedFlagsAndMaterial *)(pbVar1 + 0x50),(int)lVar12) &
               0xffffffffff;
      }
      uVar5 = extraout_ECX_00 + rayStartCoord1Q12;
      bVar10 = CARRY4(uVar6,rayStartCoord0Q12);
      uVar6 = uVar6 + rayStartCoord0Q12;
    }
    uVar13 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,iVar9,
                        pbVar1,uVar6,uVar5);
    uVar6 = (uint)((ulonglong)uVar13 >> 0x20);
    uVar5 = extraout_ECX_01;
  } while (!bVar10);
  return CONCAT44(uVar6,0x7fffffff);
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
ulonglong FieldGrid_RaycastSecondarySurfaceDistanceCf
                    (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,
                    Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  byte *pbVar1;
  longlong lVar2;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int extraout_ECX;
  int rayEndCoord1Q12;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  FixedDirectionXZEdxEax8 FVar11;
  longlong lVar12;
  undefined8 uVar13;
  
  uVar7 = (fieldGrid->gridWidth - 1) * 0x1000;
  uVar3 = (fieldGrid->gridHeight - 1) * 0x1000;
  uVar4 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - uVar4;
  rayStartCoord0Q12 = uVar4 * 2;
  iVar9 = fieldGrid->gridWidth * 0x80;
  pbVar1 = fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime00_07 +
           ((int)rayStartCoord0Q12 >> 0xc) * iVar9;
  FVar11 = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  lVar12 = (longlong)((int)FVar11 + rayOriginYQ12) * 0x1c6e9c;
  lVar2 = (longlong)(extraout_ECX + rayOriginXQ12) * -0x20c8cc;
  uVar5 = (int)((ulonglong)lVar2 >> 0x20) << 0xb | (uint)lVar2 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar12 >> 0x20) << 0xc | (uint)lVar12 >> 0x14) - uVar5;
  rayEndCoord0Q12 = uVar5 * 2;
  iVar8 = 0x400;
  uVar5 = rayStartCoord1Q12 & 0xfffff000;
  uVar6 = rayStartCoord0Q12 & 0xfffff000;
  do {
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) break;
    bVar10 = false;
    if ((((-1 < (int)uVar5) && (bVar10 = false, -1 < (int)uVar6)) &&
        (bVar10 = uVar5 < uVar7, (int)uVar5 < (int)uVar7)) &&
       (bVar10 = uVar6 < uVar3, (int)uVar6 < (int)uVar3)) {
      bVar10 = CARRY4(*(uint *)(pbVar1 + iVar9 + 200),*(uint *)(pbVar1 + iVar9 + 0xcc));
      lVar12 = TerrainTriangle_IntersectRayDistanceCf
                         ((Q12)(FVar11 >> 0x20),rayEndCoord0Q12 + uVar4 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          *(uint *)(pbVar1 + iVar9 + 200) + *(uint *)(pbVar1 + iVar9 + 0xcc),
                          *(int *)(pbVar1 + iVar9 + 0x48) + *(int *)(pbVar1 + iVar9 + 0x4c),
                          *(int *)(pbVar1 + 200) + *(int *)(pbVar1 + 0xcc),
                          *(int *)(pbVar1 + 0x4c) + *(int *)(pbVar1 + 0x48),uVar6 + uVar4 * -2,
                          uVar5 - rayStartCoord1Q12);
      uVar6 = (uint)((ulonglong)lVar12 >> 0x20);
      if (!bVar10) {
        return CONCAT44(*(FieldCellPackedFlagsAndMaterial *)(pbVar1 + 0x50),(int)lVar12) &
               0xffffffffff;
      }
      uVar5 = extraout_ECX_00 + rayStartCoord1Q12;
      bVar10 = CARRY4(uVar6,rayStartCoord0Q12);
      uVar6 = uVar6 + rayStartCoord0Q12;
    }
    uVar13 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,iVar9,
                        pbVar1,uVar6,uVar5);
    uVar6 = (uint)((ulonglong)uVar13 >> 0x20);
    uVar5 = extraout_ECX_01;
  } while (!bVar10);
  return CONCAT44(uVar6,0x7fffffff);
}

/* Address: 0x00504E60.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid raycast terrain triangles along direction.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   TerrainTriangle_IntersectRayDistanceCf [world/terrain/height], TerrainRay_AdvanceGridTraversalCf
   [world/terrain/height].
*/
ulonglong FieldGrid_RaycastTerrainTrianglesAlongDirection
                    (AngleTurn32 param_1,AngleTurn32 param_2,FixedMathScale32 param_3,Q12 param_4,
                    int param_5,int param_6,int param_7)

{
  longlong lVar1;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int extraout_ECX;
  int rayEndCoord1Q12;
  uint uVar2;
  uint extraout_ECX_00;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  bool bVar12;
  FixedDirectionXZEdxEax8 FVar13;
  longlong lVar14;
  undefined8 uVar15;
  Q12 cornerHeight0Q12;
  Q12 cornerHeight1Q12;
  Q12 cornerHeight2Q12;
  Q12 cornerHeight3Q12;
  
  uVar8 = *(int *)(param_7 + 0xb8) - 1;
  uVar3 = *(int *)(param_7 + 0xbc) - 1;
  uVar4 = (int)((ulonglong)((longlong)param_5 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)param_5 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)param_6 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)param_6 * 0x1c6e9c) >> 0x14) - uVar4;
  rayStartCoord0Q12 = uVar4 * 2;
  iVar11 = *(int *)(param_7 + 0xb8) * 0x80;
  FVar13 = FixedMath_DirectionFromAnglesScaledRegs(param_1,param_2,param_3);
  lVar14 = (longlong)((int)FVar13 + param_6) * 0x1c6e9c;
  lVar1 = (longlong)(extraout_ECX + param_5) * -0x20c8cc;
  uVar5 = (int)((ulonglong)lVar1 >> 0x20) << 0xb | (uint)lVar1 >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)lVar14 >> 0x20) << 0xc | (uint)lVar14 >> 0x14) - uVar5;
  rayEndCoord0Q12 = uVar5 * 2;
  iVar9 = 0x400;
  uVar5 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  uVar10 = ((int)rayStartCoord0Q12 >> 0xc) * iVar11 + ((int)rayStartCoord1Q12 >> 0xc) * 0x80 + 0x200
           + param_7;
  do {
    iVar9 = iVar9 + -1;
    if (iVar9 == 0) break;
    cellLocalCoord1Q12 = currentGridCoord0Q12 + uVar4 * -2;
    uVar2 = (int)((uVar5 - rayStartCoord1Q12) + rayStartCoord1Q12) >> 0xc;
    uVar6 = (int)(cellLocalCoord1Q12 + rayStartCoord0Q12) >> 0xc;
    if ((int)uVar2 < 0) {
      uVar7 = uVar10 + uVar2 * -0x80;
      uVar2 = uVar6;
      if (-1 < (int)uVar6) {
        bVar12 = uVar6 < uVar3;
        uVar2 = uVar6 - uVar3;
        if ((int)uVar6 < (int)uVar3) {
          cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
          cornerHeight2Q12 = *(Q12 *)(uVar7 + 0x48 + iVar11);
          cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
          cornerHeight0Q12 = *(Q12 *)(uVar7 + 0x48 + iVar11);
          goto LAB_00504f8a;
        }
      }
LAB_00505090:
      bVar12 = uVar7 < uVar2 * iVar11;
      uVar7 = uVar7 - uVar2 * iVar11;
      cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
      cornerHeight2Q12 = *(Q12 *)(uVar7 + 0x48);
      cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
      cornerHeight0Q12 = *(Q12 *)(uVar7 + 0x48);
    }
    else if ((int)uVar6 < 0) {
      uVar7 = uVar10 - uVar6 * iVar11;
      bVar12 = uVar2 < uVar8;
      if ((int)uVar2 < (int)uVar8) {
        cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight2Q12 = *(Q12 *)(uVar7 + 200);
        cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight0Q12 = *(Q12 *)(uVar7 + 200);
      }
      else {
        bVar12 = uVar7 < (uVar2 - uVar8) * 0x80;
        uVar7 = uVar7 + (uVar2 - uVar8) * -0x80;
        cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight2Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight0Q12 = *(Q12 *)(uVar7 + 0x48);
      }
    }
    else if ((int)uVar2 < (int)uVar8) {
      bVar12 = uVar6 < uVar3;
      if ((int)uVar6 < (int)uVar3) {
        cornerHeight3Q12 = *(Q12 *)(uVar10 + 0x48);
        cornerHeight2Q12 = *(Q12 *)(uVar10 + 200);
        cornerHeight1Q12 = *(Q12 *)(uVar10 + 0x48 + iVar11);
        cornerHeight0Q12 = *(Q12 *)(uVar10 + 200 + iVar11);
        uVar7 = uVar10;
      }
      else {
        uVar7 = (uVar6 - uVar3) * iVar11;
        bVar12 = uVar10 < uVar7;
        uVar7 = uVar10 - uVar7;
        cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight2Q12 = *(Q12 *)(uVar7 + 200);
        cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
        cornerHeight0Q12 = *(Q12 *)(uVar7 + 200);
      }
    }
    else {
      uVar7 = uVar10 + (uVar2 - uVar8) * -0x80;
      bVar12 = uVar6 < uVar3;
      uVar2 = uVar6 - uVar3;
      if ((int)uVar3 <= (int)uVar6) goto LAB_00505090;
      cornerHeight3Q12 = *(Q12 *)(uVar7 + 0x48);
      cornerHeight2Q12 = *(Q12 *)(uVar7 + 0x48 + iVar11);
      cornerHeight1Q12 = *(Q12 *)(uVar7 + 0x48);
      cornerHeight0Q12 = *(Q12 *)(uVar7 + 0x48 + iVar11);
    }
LAB_00504f8a:
    lVar14 = TerrainTriangle_IntersectRayDistanceCf
                       ((Q12)(FVar13 >> 0x20),rayEndCoord0Q12 + uVar4 * -2,
                        rayEndCoord1Q12 - rayStartCoord1Q12,param_4,cornerHeight0Q12,
                        cornerHeight1Q12,cornerHeight2Q12,cornerHeight3Q12,cellLocalCoord1Q12,
                        uVar5 - rayStartCoord1Q12);
    if (!bVar12) {
      return CONCAT44(*(undefined4 *)(uVar7 + 0x50),(int)lVar14) & 0xffffffffff;
    }
    uVar15 = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,iVar11,
                        uVar10,currentGridCoord0Q12,uVar5);
    currentGridCoord0Q12 = (uint)((ulonglong)uVar15 >> 0x20);
    uVar5 = extraout_ECX_00;
    uVar10 = uVar7;
  } while (!bVar12);
  return CONCAT44(currentGridCoord0Q12,0x7fffffff);
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
void FieldGrid_SetOccupancyMaskByteBit0AllCells
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
      currentCell->runtime58_6F[occupancyMaskByteIndex + 0x18] =
           currentCell->runtime58_6F[occupancyMaskByteIndex + 0x18] | 1;
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
void FieldGrid_ClearOccupancyMaskByteBit0AllCells
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
      currentCell->runtime58_6F[occupancyMaskByteIndex + 0x18] =
           currentCell->runtime58_6F[occupancyMaskByteIndex + 0x18] & 0xfe;
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
undefined8
TerrainGrid_TestProjectedCellMaskBits01Cf
          (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *pFVar1;
  undefined4 in_EAX;
  int iVar2;
  undefined4 in_EDX;
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
      ((pFVar1->cells[pFVar1->gridWidth * iVar4 + iVar2].runtime58_6F
        [worldRuntime->activeFactionRuntimeIndex + 0x18] & 3) != 0)))) {
    return CONCAT44(in_EDX,in_EAX);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005092A0.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid clear cell flag8000 across grid.
*/
void FieldGrid_ClearCellFlag8000AcrossGrid(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0xb8) * *(int *)(param_1 + 0xbc);
  iVar2 = param_1 + 0x200;
  do {
    *(uint *)(iVar2 + 0x50) = *(uint *)(iVar2 + 0x50) & 0xffff7fff;
    iVar2 = iVar2 + 0x80;
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
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    *(PackedArgb32 *)(currentCell->runtime00_07 + 4) = argbColor;
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
dword FieldGrid_SaveAssetImageFromRuntimeStateCf(dword fieldGridRuntimeImageCarrier)

{
  undefined4 *source;
  dword dVar1;
  uint extraout_ECX;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined1 in_CF;
  bool bVar7;
  
  source = (*g_MemoryApi.alloc)(*(dword *)(fieldGridRuntimeImageCarrier + 4));
  if (!(bool)in_CF) {
    puVar5 = source;
    for (uVar2 = extraout_ECX >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
      *puVar5 = *(undefined4 *)fieldGridRuntimeImageCarrier;
      fieldGridRuntimeImageCarrier = (dword)(fieldGridRuntimeImageCarrier + 4);
      puVar5 = puVar5 + 1;
    }
    puVar5 = source + 0x80;
    source[0x2c] = 0;
    iVar3 = source[0x2e] * source[0x2f];
    do {
      *puVar5 = 0;
      puVar5[2] = 0x40000000;
      puVar5[3] = 0;
      puVar5[4] = 0;
      puVar5[5] = 0;
      puVar5[6] = 0;
      puVar5[7] = 0;
      puVar5[0xb] = 0;
      puVar5[0xc] = 0;
      puVar5[0xd] = 0;
      puVar5[0xe] = 0;
      puVar5[0xf] = 0;
      puVar5[0x14] = puVar5[0x14] & 0xe80078ff;
      puVar5[0x15] = 0;
      puVar5[0x16] = 0;
      puVar5[0x17] = 0;
      puVar5[0x18] = 0;
      puVar5[0x19] = 0;
      puVar5[0x1a] = 0;
      source[0x2c] = source[0x2c] | 1 << ((byte)puVar5[0x14] & 0x1f);
      puVar6 = puVar5 + 0x1c;
      for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
        *(undefined1 *)puVar6 = 0;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
      }
      bVar7 = (undefined4 *)0xffffff7f < puVar5;
      puVar5 = puVar5 + 0x20;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    FileSystem_WriteBufferToPathCf(source[1],source,(word *)&g_LevelResourcePathScratchUtf16);
    if (!bVar7) {
      dVar1 = (*g_MemoryApi.free)(source);
      return dVar1;
    }
    (*g_MemoryApi.free)(source);
  }
  return (dword)source;
}

/* Address: 0x00561050.
   Ownership: world/terrain/grid.
   Purpose: Clears the exact width-times-height dword scratch plane associated with one player runtime. EAX, ECX,
   and EDX are preserved or incidental caller state and are not synthetic parameters or normal returns. Fixed
   command-payload slots remain explicit even when this wrapper does not consume every slot.
*/
void FieldGrid_ClearPlayerScratchPlane
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
void FieldGrid_ResetLocalInfluenceState
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
void FieldGrid_ApplyEncodedUpdateCore
               (FieldGridHeightDeltaUnits heightDeltaUnits,Q12 worldYQ12,Q12 worldXQ12,
               FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  int iVar3;
  FieldGridRowStrideBytes rowStrideBytes;
  FieldGridRowStrideBytes extraout_EDX;
  int extraout_EDX_00;
  FieldGridRowStrideBytes extraout_EDX_01;
  int extraout_EDX_02;
  FieldGridRowStrideBytes extraout_EDX_03;
  FieldGridRowStrideBytes rowStrideBytes_00;
  FieldGridCell *pFVar4;
  
  FVar1 = fieldGrid->gridWidth;
  iVar2 = worldXQ12 >> 0xc;
  if ((((-1 < iVar2) && (iVar3 = worldYQ12 >> 0xc, -1 < iVar3)) && (iVar2 < (int)FVar1)) &&
     (iVar3 < (int)fieldGrid->gridHeight)) {
    pFVar4 = fieldGrid->cells + iVar2 + iVar3 * FVar1;
    pFVar4->waterSurfaceDelta = pFVar4->waterSurfaceDelta + heightDeltaUnits * -0x40;
    FieldGridCell_RecomputeTriangleNormalAngles(FVar1 << 7,pFVar4);
    FieldGridCell_ComputeDirectionalLightColor(pFVar4);
    rowStrideBytes_00 = rowStrideBytes;
    if ((pFVar4[-1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,pFVar4 + -1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + -1);
      rowStrideBytes_00 = extraout_EDX;
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + 1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
      rowStrideBytes_00 = extraout_EDX_00;
    }
    pFVar4 = (FieldGridCell *)((int)pFVar4 - rowStrideBytes_00);
    if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4);
      rowStrideBytes_00 = extraout_EDX_01;
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + 1);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4 + 1);
      rowStrideBytes_00 = extraout_EDX_02;
    }
    pFVar4 = (FieldGridCell *)(pFVar4[-1].runtime00_07 + rowStrideBytes_00 * 2);
    if ((pFVar4->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4);
      FieldGridCell_ComputeDirectionalLightColor(pFVar4);
      rowStrideBytes_00 = extraout_EDX_03;
    }
    if ((pFVar4[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes_00,pFVar4 + 1);
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
void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
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
undefined8 TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

{
  undefined4 in_EAX;
  int columnsRemaining;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00505BE0.
   Ownership: world/terrain/grid.
   Purpose: Scans interior field cells in reverse and relaxes six neighboring height pairs toward the selected
   source sum when the source is nonnegative and its exclusion flag is clear. Live fluid Pass B = tick-wheel case 5
   (T5); reverse scan of the Pass A relaxation with identical sign/flag gates.
   [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Reverse scan of the sign-gated fluid relaxation with the same FLD
   source-exclusion 0x40000000 and receiver-exclusion 0x20000000 semantics.
*/
undefined8 TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *piVar1;
  FieldGridDimension FVar2;
  undefined4 in_EAX;
  int columnsRemaining;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00505D30.
   Ownership: world/terrain/grid.
   Purpose: Performs the forward interior-cell height relaxation pass without the source sign test, while
   preserving the verified exclusion flags. [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Ungated-sign terrain
   relaxation still honors FLD 0x40000000 source exclusion and 0x20000000 receiver exclusion; do not reinterpret
   them as GridScratch terrain classes.
*/
void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid)

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
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

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
void FieldGrid_ProcessHorizontalSpan
               (Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
               FieldGridRadiusUnits radiusUnits,Q12 centerWorldYQ12,Q12 centerWorldXQ12,
               FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  longlong lVar2;
  uint uVar3;
  int iVar4;
  dword dVar5;
  int iVar6;
  int extraout_ECX;
  int iVar7;
  int iVar8;
  int iVar9;
  FieldGridCell *pFVar10;
  int *piVar11;
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
  iVar6 = centerWorldXQ12 + uVar3 * -4;
  iVar7 = centerWorldYQ12 + uVar3 * -4;
  iVar4 = (int)(iVar6 + 0xfff + uVar3 * 8) >> 0xc;
  iVar8 = (int)(iVar7 + 0xfff + uVar3 * 8) >> 0xc;
  iVar6 = iVar6 >> 0xc;
  if (iVar6 < 0) {
    iVar6 = 0;
  }
  iVar7 = iVar7 >> 0xc;
  if (iVar7 < 0) {
    iVar7 = 0;
  }
  if ((int)fieldGrid->gridWidth <= iVar4) {
    iVar4 = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= iVar8) {
    iVar8 = fieldGrid->gridHeight - 1;
  }
  if ((iVar6 <= iVar4) && (iVar7 <= iVar8)) {
    iVar6 = iVar7 * fieldGrid->gridWidth + iVar6;
    iStack_24 = (iVar8 - iVar7) + 1;
    piVar11 = accumulatorPlane + iVar6;
    FVar1 = fieldGrid->gridWidth;
    iVar7 = (centerWorldYQ12 >> 0xc) * FVar1 + (centerWorldXQ12 >> 0xc);
    pFVar10 = fieldGrid->cells + iVar6;
    iVar4 = fieldGrid->cells[iVar7].worldX;
    iVar6 = fieldGrid->cells[iVar7].worldY;
    iVar7 = fieldGrid->cells
            [(sourceWorldYQ12 >> 0xc) * fieldGrid->gridWidth + (sourceWorldXQ12 >> 0xc)].
            terrainHeight;
    pFStack_34 = pFVar10;
    piStack_30 = piVar11;
    do {
      do {
        dVar5 = FixedMath_Length2(pFVar10->worldY - iVar6,pFVar10->worldX - iVar4);
        if (dVar5 <= uVar3 + 1) {
          iVar9 = (iVar7 + heightDeltaUnits * -0x40) - *piVar11;
          lVar2 = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)dVar5 & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)dVar5 * 0x8000 & 0xffffffffU) / (longlong)(int)(uVar3 + 1))
                   ] + 0x10000000) * (longlong)iVar9;
          iVar8 = ((int)((ulonglong)lVar2 >> 0x20) << 3 | (uint)lVar2 >> 0x1d) + *piVar11;
          if (iVar9 != 0) {
            if (iVar9 < 0) {
              if (iVar8 < *piVar11) {
                *piVar11 = iVar8;
              }
            }
            else if (*piVar11 < iVar8) {
              *piVar11 = iVar8;
            }
          }
        }
        pFVar10 = pFVar10 + 1;
        piVar11 = piVar11 + 1;
      } while (extraout_ECX != 1);
      piVar11 = piStack_30 + FVar1;
      pFVar10 = pFStack_34 + FVar1;
      iStack_24 = iStack_24 + -1;
      pFStack_34 = pFVar10;
      piStack_30 = piVar11;
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
void FieldGrid_ProcessVerticalSpan
               (FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
               Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAccumulatorValue *accumulatorPlane,
               FieldGridAsset *fieldGrid)

{
  int iVar1;
  FieldGridDimension FVar2;
  longlong lVar3;
  uint uVar4;
  int iVar5;
  dword dVar6;
  int iVar7;
  int extraout_ECX;
  int iVar8;
  int iVar9;
  FieldGridCell *pFVar10;
  int *piVar11;
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
  iVar7 = centerWorldXQ12 + uVar4 * -2;
  iVar8 = centerWorldYQ12 + uVar4 * -2;
  iVar5 = (int)(iVar7 + 0xfff + uVar4 * 4) >> 0xc;
  iVar9 = (int)(iVar8 + 0xfff + uVar4 * 4) >> 0xc;
  iVar7 = iVar7 >> 0xc;
  if (iVar7 < 0) {
    iVar7 = 0;
  }
  iVar8 = iVar8 >> 0xc;
  if (iVar8 < 0) {
    iVar8 = 0;
  }
  if ((int)fieldGrid->gridWidth <= iVar5) {
    iVar5 = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= iVar9) {
    iVar9 = fieldGrid->gridHeight - 1;
  }
  if ((iVar7 <= iVar5) && (iVar8 <= iVar9)) {
    iVar7 = iVar8 * fieldGrid->gridWidth + iVar7;
    iStack_24 = (iVar9 - iVar8) + 1;
    piVar11 = accumulatorPlane + iVar7;
    FVar2 = fieldGrid->gridWidth;
    iVar8 = (centerWorldYQ12 >> 0xc) * FVar2 + (centerWorldXQ12 >> 0xc);
    pFVar10 = fieldGrid->cells + iVar7;
    iVar5 = fieldGrid->cells[iVar8].worldX;
    iVar7 = fieldGrid->cells[iVar8].worldY;
    pFStack_30 = pFVar10;
    piStack_2c = piVar11;
    do {
      do {
        dVar6 = FixedMath_Length2(pFVar10->worldY - iVar7,pFVar10->worldX - iVar5);
        if (dVar6 <= uVar4 + 1) {
          lVar3 = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)dVar6 & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)dVar6 * 0x8000 & 0xffffffffU) / (longlong)(int)(uVar4 + 1))
                   ] + 0x10000000) * (longlong)iVar1;
          iVar8 = ((int)((ulonglong)lVar3 >> 0x20) << 3 | (uint)lVar3 >> 0x1d) + *piVar11;
          if (iVar1 < 0) {
            if (iVar8 < iVar1) {
              iVar8 = iVar1;
            }
          }
          else if (iVar1 < iVar8) {
            iVar8 = iVar1;
          }
          *piVar11 = iVar8;
        }
        pFVar10 = pFVar10 + 1;
        piVar11 = piVar11 + 1;
      } while (extraout_ECX != 1);
      piVar11 = piStack_2c + FVar2;
      pFVar10 = pFStack_30 + FVar2;
      iStack_24 = iStack_24 + -1;
      pFStack_30 = pFVar10;
      piStack_2c = piVar11;
    } while (iStack_24 != 0);
  }
  return;
}

/* Address: 0x005713E0.
   Ownership: world/terrain/grid.
   Purpose: Applies one bounded field-grid cell transition and updates the associated runtime flags. EAX, ECX, and
   EDX are preserved or incidental caller state and are not synthetic parameters or normal returns.
*/
void FieldGrid_ApplySingleCellTransition
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
void FieldGrid_ApplyRectangularTransition(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

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
qword FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX)

{
  uint gridHalfRowCoordinateQ12;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  return CONCAT44(gridHalfRowCoordinateQ12 * 2,
                  ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                  (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12);
}

/* Address: 0x00571FE0.
   Ownership: world/terrain/grid.
   Purpose: EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
*/
void FieldGrid_ApplyMaskedRegionCore
               (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 worldYQ12,
               Q12 worldXQ12,FieldGridAsset *fieldGrid)

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
void FieldGridCell_RecomputeTriangleNormalAngles
               (FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar1 = cell[1].terrainHeight - cell->terrainHeight;
  iVar2 = *(int *)(cell->runtime58_6F + rowStrideBytes + -0x10) - cell->terrainHeight;
  iVar3 = *(int *)((int)cell + (0x48 - rowStrideBytes)) - cell->terrainHeight;
  iVar4 = cell[-1].terrainHeight - cell->terrainHeight;
  iVar5 = *(int *)(cell->runtime00_07 + rowStrideBytes + -0x38) - cell->terrainHeight;
  iVar6 = *(int *)((int)cell + (200 - rowStrideBytes)) - cell->terrainHeight;
  FixedMath_VectorToAngles3Regs
            (0xc00000,((((-((*(int *)(cell->runtime58_6F + rowStrideBytes + -0x14) - cell->worldY) *
                           iVar2) - (cell[1].worldY - cell->worldY) * iVar1) -
                        (*(int *)((int)cell + (0x44 - rowStrideBytes)) - cell->worldY) * iVar3) -
                       (cell[-1].worldY - cell->worldY) * iVar4) -
                      (*(int *)(cell->runtime00_07 + rowStrideBytes + -0x3c) - cell->worldY) * iVar5
                      ) - (*(int *)((int)cell + (0xc4 - rowStrideBytes)) - cell->worldY) * iVar6,
             ((((-((*(int *)(cell->runtime58_6F + rowStrideBytes + -0x18) - cell->worldX) * iVar2) -
                (cell[1].worldX - cell->worldX) * iVar1) -
               (*(int *)((int)cell + (0x40 - rowStrideBytes)) - cell->worldX) * iVar3) -
              (cell[-1].worldX - cell->worldX) * iVar4) -
             (*(int *)(cell->runtime00_07 + rowStrideBytes + -0x40) - cell->worldX) * iVar5) -
             (*(int *)((int)cell + (0xc0 - rowStrideBytes)) - cell->worldX) * iVar6);
  cell->triangle0NormalAngles = extraout_ECX | extraout_EDX << 0x10;
  iVar1 = -rowStrideBytes;
  iVar2 = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar3 = ((*(int *)(cell->runtime58_6F + rowStrideBytes + -0x10) +
           *(int *)(cell->runtime58_6F + rowStrideBytes + -0xc)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar4 = ((*(int *)((int)cell + iVar1 + 0x48) + *(int *)((int)cell + iVar1 + 0x4c)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  iVar5 = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar6 = ((*(int *)(cell->runtime00_07 + rowStrideBytes + -0x38) +
           *(int *)(cell->runtime00_07 + rowStrideBytes + -0x34)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  iVar7 = ((*(int *)((int)cell + iVar1 + 200) + *(int *)((int)cell + iVar1 + 0xcc)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  FixedMath_VectorToAngles3Regs
            (0xc00000,((((-((*(int *)(cell->runtime58_6F + rowStrideBytes + -0x14) - cell->worldY) *
                           iVar3) - (cell[1].worldY - cell->worldY) * iVar2) -
                        (*(int *)((int)cell + iVar1 + 0x44) - cell->worldY) * iVar4) -
                       (cell[-1].worldY - cell->worldY) * iVar5) -
                      (*(int *)(cell->runtime00_07 + rowStrideBytes + -0x3c) - cell->worldY) * iVar6
                      ) - (*(int *)((int)cell + iVar1 + 0xc4) - cell->worldY) * iVar7,
             ((((-((*(int *)(cell->runtime58_6F + rowStrideBytes + -0x18) - cell->worldX) * iVar3) -
                (cell[1].worldX - cell->worldX) * iVar2) -
               (*(int *)((int)cell + iVar1 + 0x40) - cell->worldX) * iVar4) -
              (cell[-1].worldX - cell->worldX) * iVar5) -
             (*(int *)(cell->runtime00_07 + rowStrideBytes + -0x40) - cell->worldX) * iVar6) -
             (*(int *)((int)cell + iVar1 + 0xc0) - cell->worldX) * iVar7);
  cell->triangle1NormalAngles = extraout_ECX_00 | extraout_EDX_00 << 0x10;
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
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  int extraout_ECX;
  FixedDirectionXZEdxEax8 triangleNormalDirectionXZQ28;
  undefined4 directionalLightColor;
  
  triangleNormalDirectionXZQ28 =
       FixedMath_DirectionFromAnglesQ28Regs
                 ((int)cell->triangle0NormalAngles >> 0x10,cell->triangle0NormalAngles & 0xffff);
  directionalLightColor =
       (&g_TerrainDirectionalLightColorLut)
       [(int)((ulonglong)
              ((longlong)(int)triangleNormalDirectionXZQ28 * (longlong)g_TerrainLightDirectionX) >>
             0x20) +
        (int)((ulonglong)((longlong)extraout_ECX * (longlong)g_TerrainLightDirectionY) >> 0x20) +
        (int)((ulonglong)
              ((longlong)(int)(triangleNormalDirectionXZQ28 >> 0x20) *
              (longlong)g_TerrainLightDirectionZ) >> 0x20) >> 0x10];
  *(undefined4 *)(cell->runtime58_6F + 4) = g_TerrainDirectionalLightSecondaryColor;
  *(undefined4 *)cell->runtime58_6F = directionalLightColor;
  return;
}
