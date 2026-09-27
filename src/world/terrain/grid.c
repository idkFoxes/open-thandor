/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/grid.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/grid.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
  FieldGridDimension rowLength;
  int columnCountOrColumnsLeft;
  int minRowOrColumnsLeft;
  uint horizontalRadiusQ12;
  int maxRowOrRowsLeft;
  int boundOrRowsLeft;
  int minColumnOrRowsLeft;
  FieldGridCell *cell;
  FieldGridCell *normalCell;
  FieldGridCell *lightCell;
  FieldGridCoordinatesEaxEdx8 minCornerGrid;
  FieldGridCoordinatesEaxEdx8 maxCornerGrid;
  FieldGridCell *heightOrLightRowStart;
  FieldGridCell *normalRowStart;
  
  if (0 < radiusWorldUnits) {
    horizontalRadiusQ12 = (int)((ulonglong)((longlong)radiusWorldUnits * 0x1bb6) >> 0x20) << 0x14 |
            (uint)((longlong)radiusWorldUnits * 0x1bb6) >> 0xc;
    boundOrRowsLeft = centerWorldXQ12 - horizontalRadiusQ12;
    minCornerGrid = FieldGrid_WorldToGridQ12(centerWorldYQ12 + radiusWorldUnits,boundOrRowsLeft);
    maxCornerGrid = FieldGrid_WorldToGridQ12
                      (centerWorldYQ12 + radiusWorldUnits + radiusWorldUnits * -2,boundOrRowsLeft + horizontalRadiusQ12 * 2)
    ;
    rowLength = fieldGrid->gridWidth;
    minColumnOrRowsLeft = minCornerGrid.columnQ12 >> 0xc;
    minRowOrColumnsLeft = minCornerGrid.rowQ12 >> 0xc;
    boundOrRowsLeft = (maxCornerGrid.columnQ12 >> 0xc) + 1;
    maxRowOrRowsLeft = (maxCornerGrid.rowQ12 >> 0xc) + 1;
    if ((int)rowLength <= boundOrRowsLeft) {
      boundOrRowsLeft = rowLength - 1;
    }
    if (minColumnOrRowsLeft < 1) {
      minColumnOrRowsLeft = 1;
    }
    if (minRowOrColumnsLeft < 1) {
      minRowOrColumnsLeft = 1;
    }
    if ((int)fieldGrid->gridHeight <= maxRowOrRowsLeft) {
      maxRowOrRowsLeft = fieldGrid->gridHeight - 1;
    }
    columnCountOrColumnsLeft = boundOrRowsLeft - minColumnOrRowsLeft;
    if ((columnCountOrColumnsLeft != 0 && minColumnOrRowsLeft <= boundOrRowsLeft) && (boundOrRowsLeft = maxRowOrRowsLeft - minRowOrColumnsLeft, boundOrRowsLeft != 0 && minRowOrColumnsLeft <= maxRowOrRowsLeft)) {
      fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
      cell = (FieldGridCell *)(fieldGrid->cells[minColumnOrRowsLeft].runtime0C_3F + minRowOrColumnsLeft * rowLength * 0x80 + -0xc);
      minRowOrColumnsLeft = columnCountOrColumnsLeft;
      heightOrLightRowStart = cell;
      normalCell = cell;
      maxRowOrRowsLeft = boundOrRowsLeft;
      do {
        do {
          FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
                    (terrainMaterialIndexOrNegativeSentinel,radiusWorldUnits,
                     terrainHeightDeltaAmplitudeQ12,centerWorldYQ12,centerWorldXQ12,cell);
          cell = cell + 1;
          minRowOrColumnsLeft = minRowOrColumnsLeft + -1;
        } while (minRowOrColumnsLeft != 0);
        cell = heightOrLightRowStart + rowLength;
        boundOrRowsLeft = boundOrRowsLeft + -1;
        minRowOrColumnsLeft = columnCountOrColumnsLeft;
        normalRowStart = normalCell;
        heightOrLightRowStart = cell;
        lightCell = normalCell;
        minColumnOrRowsLeft = maxRowOrRowsLeft;
      } while (boundOrRowsLeft != 0);
      do {
        do {
          FieldGridCell_RecomputeTriangleNormalAngles(rowLength * 0x80,normalCell);
          normalCell = normalCell + 1;
          columnCountOrColumnsLeft = columnCountOrColumnsLeft + -1;
        } while (columnCountOrColumnsLeft != 0);
        normalCell = normalRowStart + rowLength;
        maxRowOrRowsLeft = maxRowOrRowsLeft + -1;
        columnCountOrColumnsLeft = minRowOrColumnsLeft;
        heightOrLightRowStart = lightCell;
        normalRowStart = normalCell;
        boundOrRowsLeft = minRowOrColumnsLeft;
      } while (maxRowOrRowsLeft != 0);
      do {
        do {
          FieldGridCell_ComputeDirectionalLightColor(lightCell);
          lightCell = lightCell + 1;
          minRowOrColumnsLeft = minRowOrColumnsLeft + -1;
        } while (minRowOrColumnsLeft != 0);
        lightCell = heightOrLightRowStart + rowLength;
        minColumnOrRowsLeft = minColumnOrRowsLeft + -1;
        minRowOrColumnsLeft = boundOrRowsLeft;
        heightOrLightRowStart = lightCell;
      } while (minColumnOrRowsLeft != 0);
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
  FieldGridDimension rowLength;
  int cellDelta;
  dword remainingPairCount;
  int remainingCellCount;
  int countdownOrDelta;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  FieldGridCell *applyCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  bool containsAnchorPair;
  int *accumulatorPlane;
  int applyRowStrideBytes;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  scratchHeightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * 0x80;
  applyCell = fieldGrid->cells;
  countdownOrDelta = remainingCellCount;
  scanCell = applyCell;
  accumulatorPlane = scratchHeightCursor;
  applyRowStrideBytes = rowStrideBytes;
  do {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      scanCell->terrainHeight = scanCell->terrainHeight - cellDelta;
      scanCell->waterSurfaceDelta = scanCell->waterSurfaceDelta + cellDelta;
      if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
        if (((scanCell[-1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + -1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + -1);
        }
        if (((scanCell[1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + -rowLength;
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + rowLength * 2 + -1;
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        cell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        scanCell = cell + -rowLength;
      }
    }
    *scratchHeightCursor = scanCell->terrainHeight;
    scanCell = scanCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    countdownOrDelta = countdownOrDelta + -1;
  } while (countdownOrDelta != 0);
  containsAnchorPair = SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessHorizontalSpan
              (anchorWorldYQ12,anchorWorldXQ12,(int)packedDragDeltaXY16 >> 0x10,
               (int)(short)packedDragDeltaXY16,anchorWorldYQ12,anchorWorldXQ12,accumulatorPlane,
               fieldGrid);
    scratchHeightCursor = accumulatorPlane;
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    scratchHeightCursor = accumulatorPlane;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084;
        remainingPairCount != 0; remainingPairCount = remainingPairCount - 1) {
      FieldGrid_ProcessHorizontalSpan
                (anchorWorldYQ12,anchorWorldXQ12,(int)packedDragDeltaXY16 >> 0x10,
                 (int)(short)packedDragDeltaXY16,pairRecord->pairValue,pairRecord->pairKey,accumulatorPlane,
                 fieldGrid);
      pairRecord = pairRecord + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    LOCK();
    countdownOrDelta = *scratchHeightCursor;
    *scratchHeightCursor = 0;
    UNLOCK();
    countdownOrDelta = countdownOrDelta - applyCell->terrainHeight;
    if (countdownOrDelta != 0) {
      applyCell->terrainHeight = applyCell->terrainHeight + countdownOrDelta;
      applyCell->waterSurfaceDelta = applyCell->waterSurfaceDelta - countdownOrDelta;
      *scratchHeightCursor = countdownOrDelta;
      if ((applyCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
        FieldGridCell_ComputeDirectionalLightColor(applyCell);
        if (((applyCell[-1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + -1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + -1);
        }
        if (((applyCell[1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        applyCell = (FieldGridCell *)((int)applyCell - applyRowStrideBytes);
        if ((applyCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        if ((applyCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        scanCell = (FieldGridCell *)(applyCell[-1].runtime0C_3F + applyRowStrideBytes * 2 + -0xc);
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        applyCell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        applyCell = (FieldGridCell *)((int)applyCell - applyRowStrideBytes);
      }
    }
    applyCell = applyCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    remainingCellCount = remainingCellCount + -1;
  } while (remainingCellCount != 0);
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
  FieldGridDimension rowLength;
  int cellDelta;
  dword remainingPairCount;
  int remainingCellCount;
  int countdownOrDelta;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  FieldGridCell *applyCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  bool containsAnchorPair;
  int *accumulatorPlane;
  int applyRowStrideBytes;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  scratchHeightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane8088;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * 0x80;
  applyCell = fieldGrid->cells;
  countdownOrDelta = remainingCellCount;
  scanCell = applyCell;
  accumulatorPlane = scratchHeightCursor;
  applyRowStrideBytes = rowStrideBytes;
  do {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      scanCell->terrainHeight = scanCell->terrainHeight - cellDelta;
      scanCell->waterSurfaceDelta = scanCell->waterSurfaceDelta + cellDelta;
      *scratchHeightCursor = 0;
      if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
        if (((scanCell[-1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + -1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + -1);
        }
        if (((scanCell[1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + -rowLength;
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + rowLength * 2 + -1;
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        cell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        scanCell = cell + -rowLength;
      }
    }
    scanCell = scanCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    countdownOrDelta = countdownOrDelta + -1;
  } while (countdownOrDelta != 0);
  containsAnchorPair = SelectionPlayerPairList_ContainsPairCf(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessVerticalSpan
              ((int)packedDragDeltaXY16 >> 0x10,(int)(short)packedDragDeltaXY16,anchorWorldYQ12,
               anchorWorldXQ12,accumulatorPlane,fieldGrid);
    scratchHeightCursor = accumulatorPlane;
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->pairRecords80_807F;
    scratchHeightCursor = accumulatorPlane;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->activePairCount8084;
        remainingPairCount != 0; remainingPairCount = remainingPairCount - 1) {
      FieldGrid_ProcessVerticalSpan
                ((int)packedDragDeltaXY16 >> 0x10,(int)(short)packedDragDeltaXY16,pairRecord->pairValue,
                 pairRecord->pairKey,accumulatorPlane,fieldGrid);
      pairRecord = pairRecord + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  do {
    countdownOrDelta = *scratchHeightCursor;
    if (countdownOrDelta != 0) {
      applyCell->terrainHeight = applyCell->terrainHeight + countdownOrDelta;
      applyCell->waterSurfaceDelta = applyCell->waterSurfaceDelta - countdownOrDelta;
      if ((applyCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
        FieldGridCell_ComputeDirectionalLightColor(applyCell);
        if (((applyCell[-1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + -1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + -1);
        }
        if (((applyCell[1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        applyCell = (FieldGridCell *)((int)applyCell - applyRowStrideBytes);
        if ((applyCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        if ((applyCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        scanCell = (FieldGridCell *)(applyCell[-1].runtime0C_3F + applyRowStrideBytes * 2 + -0xc);
        if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        applyCell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        applyCell = (FieldGridCell *)((int)applyCell - applyRowStrideBytes);
      }
    }
    applyCell = applyCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    remainingCellCount = remainingCellCount + -1;
  } while (remainingCellCount != 0);
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
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  dword remainingPairCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  bool containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplyRectangularTransition(worldYQ12,worldXQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->pairRecords80_807F;
    for (remainingPairCount = playerBlock->activePairCount8084; remainingPairCount != 0; remainingPairCount = remainingPairCount - 1) {
      FieldGrid_ApplyRectangularTransition(pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord = pairRecord + 1;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  scratchHeightCursor = playerBlock->terrainHeightScratchPlane8088;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * 0x80;
  scanCell = fieldGrid->cells;
  do {
    if ((*scratchHeightCursor != scanCell->terrainHeight) && ((scanCell->flagsAndMaterial & 0x88006000) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
      FieldGridCell_ComputeDirectionalLightColor(scanCell);
      if (((scanCell[-1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + -1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + -1);
      }
      if (((scanCell[1].flagsAndMaterial & 0x88006000) == 0) && (scratchHeightCursor[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell + -rowLength;
      if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell + rowLength * 2 + -1;
      if ((scanCell->flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      cell = scanCell + 1;
      if ((scanCell[1].flagsAndMaterial & 0x88006000) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
      }
      scanCell = cell + -rowLength;
    }
    scanCell = scanCell + 1;
    scratchHeightCursor = scratchHeightCursor + 1;
    remainingCellCount = remainingCellCount + -1;
  } while (remainingCellCount != 0);
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
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 rowLength * 0x80 + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowLength * 0x80,cell);
        columnsLeft = columnsLeft + -1;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft = rowsLeft + -1;
      cellCursor = cell + 3;
    } while (rowsLeft != 0);
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
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;
  
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_TerrainLightDirectionX,lightElevationAngle,lightAzimuthAngle);
  if (fieldGrid != (FieldGridAsset *)0x0) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 rowLength * 0x80 + -0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        columnsLeft = columnsLeft + -1;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft = rowsLeft + -1;
      cellCursor = cell + 3;
    } while (rowsLeft != 0);
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
  Q12 *heightField;
  uint baseColumn;
  uint columnFractionQ12;
  int heightDeltaOrRowStride;
  uint fractionSumOrWidth;
  uint rowFractionQ12;
  int cellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *cell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint targetRow;
  uint targetColumn;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = heightDeltaSourceValue / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = worldZQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    baseColumn = gridCoordinates.columnQ12 >> 0xc;
    targetRow = gridCoordinates.rowQ12 >> 0xc;
    columnFractionQ12 = (uint)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, ulonglong, gridCoordinates) & 0xfff00000fff) >> 0x20);
    fractionSumOrWidth = rowFractionQ12 + columnFractionQ12 * 2;
    targetColumn = baseColumn;
    if (fractionSumOrWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow = targetRow + 1;
      }
    }
    else if (fractionSumOrWidth < 0x2001) {
      targetColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        targetRow = targetRow + 1;
        targetColumn = baseColumn;
      }
    }
    else {
      targetColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow = targetRow + 1;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    if ((((-1 < (int)targetColumn) && (fractionSumOrWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)targetRow)) &&
        (targetRow < fieldGrid->gridHeight)) &&
       (((targetColumn < fractionSumOrWidth &&
         (cellIndex = targetRow * fractionSumOrWidth + targetColumn,
         (fieldGrid->cells[cellIndex].flagsAndMaterial & 0x88006000) == 0)) &&
        (fieldGrid->cells[cellIndex].waterSurfaceDelta < 1)))) {
      heightDeltaOrRowStride = g_TerrainScanReferenceHeight - fieldGrid->cells[cellIndex].terrainHeight;
      heightField = &fieldGrid->cells[cellIndex].terrainHeight;
      *heightField = *heightField + heightDeltaOrRowStride;
      heightField = &fieldGrid->cells[cellIndex].waterSurfaceDelta;
      *heightField = *heightField - heightDeltaOrRowStride;
      heightDeltaOrRowStride = g_TerrainScanRowStrideBytes;
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               cellIndex * 0x80 + -0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      TerrainHeightDelta_ApplyWedge0(0,wedgeCellA);
      cell = wedgeCellB + -1;
      TerrainHeightDelta_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(cell[-1].runtime0C_3F + heightDeltaOrRowStride + -0xc);
      TerrainHeightDelta_ApplyWedge2(0,cell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + heightDeltaOrRowStride + -0xc);
      TerrainHeightDelta_ApplyWedge3(0,wedgeCellA);
      TerrainHeightDelta_ApplyWedge4(0,wedgeCellB);
      TerrainHeightDelta_ApplyWedge5(0,wedgeCellB + 1);
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
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  dword remainingPairCount;
  SelectionPlayerPairRecord *pairRecord;
  bool containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPairCf(worldYQ12,worldXQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplySingleCellTransition(transitionValue,worldYQ12,worldXQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->pairRecords80_807F;
    for (remainingPairCount = playerBlock->activePairCount8084; remainingPairCount != 0; remainingPairCount = remainingPairCount - 1) {
      FieldGrid_ApplySingleCellTransition
                (transitionValue,pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord = pairRecord + 1;
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
  int columnOrCellIndex;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Q12 terrainHeightQ12;
  bool outOfBounds;
  FieldGridNearestPointRegsCf13 nearestPoint;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  columnOrCellIndex = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((columnOrCellIndex < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, gridRowIndex < 0)) ||
      ((int)field->gridWidth <= columnOrCellIndex)) ||
     (columnOrCellIndex = field->gridWidth * gridRowIndex + columnOrCellIndex, (int)field->gridHeight <= gridRowIndex)) {
    terrainHeightQ12 = 0;
    outOfBounds = 1;
  }
  else {
    worldX = field->cells[columnOrCellIndex].worldX;
    worldY = field->cells[columnOrCellIndex].worldY;
    terrainHeightQ12 = field->cells[columnOrCellIndex].terrainHeight;
    outOfBounds = 0;
  }
  nearestPoint.ecx = worldY;
  nearestPoint.eax = worldX;
  nearestPoint.carry = outOfBounds;
  nearestPoint.edx = terrainHeightQ12;
  return nearestPoint;
}


/* Address: 0x004FEB10.
   Ownership: world/terrain/grid.
   Purpose: Nearest-cell companion that returns worldX in EAX, worldY in ECX, and terrainHeight + waterSurfaceDelta
   in EDX. CF reports bounds success.
*/
FieldGridSurfacePointEaxEcxEdxCf13
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int columnCellOrSurfaceZ;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  bool outOfBounds;
  FieldGridSurfacePointEaxEcxEdxCf13 nearestPoint;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  columnCellOrSurfaceZ = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((columnCellOrSurfaceZ < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, gridRowIndex < 0)) ||
      ((int)field->gridWidth <= columnCellOrSurfaceZ)) ||
     (columnCellOrSurfaceZ = field->gridWidth * gridRowIndex + columnCellOrSurfaceZ, (int)field->gridHeight <= gridRowIndex)) {
    columnCellOrSurfaceZ = 0;
    outOfBounds = 1;
  }
  else {
    worldX = field->cells[columnCellOrSurfaceZ].worldX;
    worldY = field->cells[columnCellOrSurfaceZ].worldY;
    columnCellOrSurfaceZ = field->cells[columnCellOrSurfaceZ].waterSurfaceDelta + field->cells[columnCellOrSurfaceZ].terrainHeight;
    outOfBounds = 0;
  }
  nearestPoint.worldYQ12 = worldY;
  nearestPoint.worldXQ12 = worldX;
  nearestPoint.carry = outOfBounds;
  nearestPoint.worldZQ12 = columnCellOrSurfaceZ;
  return nearestPoint;
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
  sdword columnOrWaterDelta;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  columnOrWaterDelta = (int)((((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
          >> 0xc;
  if ((((-1 < columnOrWaterDelta) &&
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, -1 < gridRowIndex)) &&
      (columnOrWaterDelta < (int)field->gridWidth)) && (gridRowIndex < (int)field->gridHeight)) {
    columnOrWaterDelta = field->cells[field->gridWidth * gridRowIndex + columnOrWaterDelta].waterSurfaceDelta;
  }
  return columnOrWaterDelta;
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
  longlong upperTriangleAccumulator;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint columnFractionOrHeightQ12;
  uint gridRowCoordinateQ12;
  uint rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  bool sampleFailed;
  FieldGridHeightEaxCf5 sampleResult;
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
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionOrHeightQ12 = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000)
           == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionOrHeightQ12 + rowFractionQ12) - 0x1000;
        if (columnFractionOrHeightQ12 + rowFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + rowIndexOrRowOffsetBytes + 0x18) * (longlong)(int)columnFractionOrHeightQ12 +
               ((longlong)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18)
                * (longlong)(int)rowFractionQ12 -
               (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) *
               (longlong)triangleDiagonalWeightQ12);
          columnFractionOrHeightQ12 = (uint)weightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        else {
          upperTriangleAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x18) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x18) * (longlong)(int)(columnFractionOrHeightQ12 - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x18) * (longlong)(int)(rowFractionQ12 - 0x1000));
          columnFractionOrHeightQ12 = (uint)upperTriangleAccumulator >> 0xc | (int)((ulonglong)upperTriangleAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        sampleResult.carry = sampleFailed;
        sampleResult.heightQ12 = columnFractionOrHeightQ12;
        return sampleResult;
      }
    }
  }
  columnFractionOrHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.carry = sampleFailed;
  sampleResult.heightQ12 = columnFractionOrHeightQ12;
  return sampleResult;
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
  longlong upperTriangleAccumulator;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint columnFractionQ12;
  uint gridRowCoordinateQ12;
  uint rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
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
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)field->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionQ12 = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(field->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000) == 0
          ) && ((*(uint *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                                producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - 0x1000;
        if (columnFractionQ12 + rowFractionQ12 < 0x1000) {
          weightedWaterDeltaAccumulator =
               (longlong)
               *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c) * (longlong)(int)columnFractionQ12 +
               ((longlong)
                *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
                (longlong)(int)rowFractionQ12 -
               (longlong)*(int *)(field->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
               (longlong)triangleDiagonalWeightQ12);
          return (uint)weightedWaterDeltaAccumulator >> 0xc |
                 (int)((ulonglong)weightedWaterDeltaAccumulator >> 0x20) << 0x14;
        }
        upperTriangleAccumulator = (longlong)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                              producerName + rowIndexOrRowOffsetBytes + 0x1c) * (longlong)triangleDiagonalWeightQ12 -
                ((longlong)
                 *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
                 (longlong)(int)(columnFractionQ12 - 0x1000) +
                (longlong)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                        rowIndexOrRowOffsetBytes + 0x1c) * (longlong)(int)(rowFractionQ12 - 0x1000));
        return (uint)upperTriangleAccumulator >> 0xc | (int)((ulonglong)upperTriangleAccumulator >> 0x20) << 0x14;
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
  uint surfaceHeightQ12;
  Q12 gridRowFixedQ12;
  uint rowFractionQ12;
  Q12 gridColumnFixedQ12;
  uint columnFractionQ12;
  int rowIndexOrRowOffsetBytes;
  Q12 upperTriangleWeightQ12;
  bool sampleFailed;
  FieldGridHeightEaxCf5 sampleResult;
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
  if (((-1 < cellColumn) && (rowIndexOrRowOffsetBytes = gridColumnFixedQ12 * 2 >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (cellColumn < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      rowFractionQ12 = gridRowFixedQ12 & 0xfff;
      columnFractionQ12 = gridColumnFixedQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000) == 0)
         && ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        upperTriangleWeightQ12 = (rowFractionQ12 + columnFractionQ12) - 0x1000;
        if (rowFractionQ12 + columnFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c)) * (longlong)(int)rowFractionQ12 +
               ((longlong)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
                (longlong)(int)columnFractionQ12 -
               (longlong)
               (*(int *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
               *(int *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
               (longlong)upperTriangleWeightQ12);
          surfaceHeightQ12 = (uint)weightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        else {
          upperTriangleWeightedHeightAccumulator =
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                              producerName + rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + rowIndexOrRowOffsetBytes + 0x1c)) * (longlong)upperTriangleWeightQ12 -
               ((longlong)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
                (longlong)(int)(rowFractionQ12 - 0x1000) +
               (longlong)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c)) * (longlong)(int)(columnFractionQ12 - 0x1000));
          surfaceHeightQ12 = (uint)upperTriangleWeightedHeightAccumulator >> 0xc |
                  (int)((ulonglong)upperTriangleWeightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        sampleResult.carry = sampleFailed;
        sampleResult.heightQ12 = surfaceHeightQ12;
        return sampleResult;
      }
    }
  }
  surfaceHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.carry = sampleFailed;
  sampleResult.heightQ12 = surfaceHeightQ12;
  return sampleResult;
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
  longlong partialAccumulator;
  int gridColumnIndex;
  uint gridColumnCoordinateQ12;
  uint columnFractionOrWaterDelta;
  uint gridRowCoordinateQ12;
  uint rowFractionQ12;
  uint terrainHeightQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  bool sampleFailed;
  FieldGridHeightEaxCf5 sampleResult;
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
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionOrWaterDelta = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000)
           == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionOrWaterDelta + rowFractionQ12) - 0x1000;
        if (columnFractionOrWaterDelta + rowFractionQ12 < 0x1000) {
          weightedSurfaceAccumulator =
               (longlong)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + rowIndexOrRowOffsetBytes + 0x18) * (longlong)(int)columnFractionOrWaterDelta +
               ((longlong)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18)
                * (longlong)(int)rowFractionQ12 -
               (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) *
               (longlong)triangleDiagonalWeightQ12);
          terrainHeightQ12 =
               (int)((ulonglong)weightedSurfaceAccumulator >> 0x20) << 0x14 |
               (uint)weightedSurfaceAccumulator >> 0xc;
          partialAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x1c) * (longlong)(int)columnFractionOrWaterDelta +
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x14) * (longlong)(int)rowFractionQ12 -
                  (longlong)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)
                  * (longlong)triangleDiagonalWeightQ12);
          columnFractionOrWaterDelta = (int)((ulonglong)partialAccumulator >> 0x20) << 0x14 | (uint)partialAccumulator >> 0xc;
          if (-1 < (int)columnFractionOrWaterDelta) {
            terrainHeightQ12 = terrainHeightQ12 + columnFractionOrWaterDelta;
          }
          sampleFailed = false;
        }
        else {
          partialAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x18) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x18) * (longlong)(int)(columnFractionOrWaterDelta - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x18) * (longlong)(int)(rowFractionQ12 - 0x1000));
          terrainHeightQ12 = (int)((ulonglong)partialAccumulator >> 0x20) << 0x14 | (uint)partialAccumulator >> 0xc;
          partialAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x1c) *
                  (longlong)triangleDiagonalWeightQ12 -
                  ((longlong)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x14) * (longlong)(int)(columnFractionOrWaterDelta - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x1c) * (longlong)(int)(rowFractionQ12 - 0x1000));
          columnFractionOrWaterDelta = (int)((ulonglong)partialAccumulator >> 0x20) << 0x14 | (uint)partialAccumulator >> 0xc;
          if (-1 < (int)columnFractionOrWaterDelta) {
            terrainHeightQ12 = terrainHeightQ12 + columnFractionOrWaterDelta;
          }
          sampleFailed = false;
        }
        sampleResult.carry = sampleFailed;
        sampleResult.heightQ12 = terrainHeightQ12;
        return sampleResult;
      }
    }
  }
  terrainHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.carry = sampleFailed;
  sampleResult.heightQ12 = terrainHeightQ12;
  return sampleResult;
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
  FieldGridDimension rowLength;
  uint cornerNormalAngles;
  longlong weightedHeightAccumulator;
  int columnOrNormalSum;
  uint interpolatedHeightQ12;
  dword firstNormalEax;
  uint columnFractionOrNormalAngles;
  dword firstNormalEcx;
  uint rowFractionOrNormalAngles;
  dword firstNormalEdx;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  FieldGridHeightNormalEaxEdxCf9 sampleResult;
  FixedMathVectorAnglesRegs8 blendedNormalAngles;
  FixedDirectionXyzRegs12 scaledNormal;
  
  rowFractionOrNormalAngles = (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = field->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)field->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint *)(field->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x18) * (longlong)(int)columnFractionOrNormalAngles +
                  ((longlong)*(int *)(field->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                   (longlong)(int)rowFractionOrNormalAngles -
                  (longlong)*(int *)(field->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                  (longlong)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          columnFractionOrNormalAngles = *(uint *)(field->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.eax;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)(field->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.edx,diagonalWeightOrNormalSum + scaledNormal.ecx,cellOffsetOrNormalSum + scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                          rowOffsetOrNormalSum + 0x18) * (longlong)diagonalWeightOrNormalSum -
                  ((longlong)*(int *)(field->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                   (longlong)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (longlong)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x18) * (longlong)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          cornerNormalAngles = *(uint *)(field->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          columnOrNormalSum = firstNormalEax - scaledNormal.eax;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.edx,rowOffsetOrNormalSum - scaledNormal.ecx,columnOrNormalSum - scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.carry = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.carry = sampleFailed;
  return sampleResult;
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
  FieldGridDimension rowLength;
  uint cornerNormalAngles;
  longlong weightedHeightAccumulator;
  int columnOrNormalSum;
  uint interpolatedHeightQ12;
  dword firstNormalEax;
  uint columnFractionOrNormalAngles;
  dword firstNormalEcx;
  uint rowFractionOrNormalAngles;
  dword firstNormalEdx;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  FieldGridHeightNormalEaxEdxCf9 sampleResult;
  FixedMathVectorAnglesRegs8 blendedNormalAngles;
  FixedDirectionXyzRegs12 scaledNormal;
  
  rowFractionOrNormalAngles = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (longlong)(int)columnFractionOrNormalAngles +
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (longlong)(int)rowFractionOrNormalAngles -
                  (longlong)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x14) *
                  (longlong)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.eax;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.edx,diagonalWeightOrNormalSum + scaledNormal.ecx,cellOffsetOrNormalSum + scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetOrNormalSum + 0x1c) * (longlong)diagonalWeightOrNormalSum -
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (longlong)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (longlong)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          cornerNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          columnOrNormalSum = firstNormalEax - scaledNormal.eax;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.edx,rowOffsetOrNormalSum - scaledNormal.ecx,columnOrNormalSum - scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.carry = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.carry = sampleFailed;
  return sampleResult;
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
  FieldGridDimension rowLength;
  uint cornerNormalAngles;
  longlong weightedHeightAccumulator;
  int columnOrNormalSum;
  uint interpolatedHeightQ12;
  dword firstNormalEax;
  uint columnFractionOrNormalAngles;
  dword firstNormalEcx;
  uint rowFractionOrNormalAngles;
  dword firstNormalEdx;
  int rowOffsetBytes;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  FieldGridHeightNormalEaxEdxCf9 sampleResult;
  FixedMathVectorAnglesRegs8 blendedNormalAngles;
  FixedDirectionXyzRegs12 scaledNormal;
  int normalSumEcx;
  
  rowFractionOrNormalAngles = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetBytes = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetBytes)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowOffsetBytes = rowOffsetBytes * rowLength * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetBytes + 0x1c) * (longlong)(int)columnFractionOrNormalAngles +
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + -0x14)
                   * (longlong)(int)rowFractionOrNormalAngles -
                  (longlong)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + -0x14) *
                  (longlong)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetBytes + 8);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + 0x18);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.eax;
          normalSumEcx = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + 0x18);
          columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.edx,normalSumEcx + scaledNormal.ecx,diagonalWeightOrNormalSum + scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetBytes + 0x1c) * (longlong)diagonalWeightOrNormalSum -
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + -0x14)
                   * (longlong)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetBytes + 0x1c) * (longlong)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                 sourceName + rowOffsetBytes + 8);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.edx;
          firstNormalEcx = scaledNormal.ecx;
          firstNormalEax = scaledNormal.eax;
          cornerNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + 0x18);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.eax;
          normalSumEcx = firstNormalEcx - scaledNormal.ecx;
          columnFractionOrNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetBytes + 8);
          columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum - scaledNormal.edx,normalSumEcx - scaledNormal.ecx,diagonalWeightOrNormalSum - scaledNormal.eax);
          rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.carry = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.carry = sampleFailed;
  return sampleResult;
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
  FieldGridDimension rowLength;
  uint cornerNormalAngles;
  longlong weightedHeightAccumulator;
  int columnOrNormalSum;
  uint interpolatedHeightQ12;
  dword firstNormalEax;
  uint columnFractionOrNormalAngles;
  dword firstNormalEcx;
  int columnWeightQ12;
  uint rowFractionOrNormalAngles;
  dword firstNormalEdx;
  int rowWeightQ12;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  FieldGridHeightNormalEaxEdxCf9 sampleResult;
  FixedMathVectorAnglesRegs8 blendedNormalAngles;
  FixedDirectionXyzRegs12 scaledNormal;
  
  rowFractionOrNormalAngles = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (longlong)(int)columnFractionOrNormalAngles +
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (longlong)(int)rowFractionOrNormalAngles -
                  (longlong)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x14) *
                  (longlong)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetOrNormalSum + 8);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          if ((int)interpolatedHeightQ12 < 0) {
            cornerNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             cellOffsetOrNormalSum + -0x20);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.edx;
            firstNormalEcx = scaledNormal.ecx;
            firstNormalEax = scaledNormal.eax;
            columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.eax;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.ecx;
            columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
            columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.edx,diagonalWeightOrNormalSum + scaledNormal.ecx,cellOffsetOrNormalSum + scaledNormal.eax);
            rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.edx;
            firstNormalEcx = scaledNormal.ecx;
            firstNormalEax = scaledNormal.eax;
            columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.eax;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.ecx;
            columnFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.edx,diagonalWeightOrNormalSum + scaledNormal.ecx,cellOffsetOrNormalSum + scaledNormal.eax);
            rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
            sampleFailed = false;
          }
        }
        else {
          columnWeightQ12 = columnFractionOrNormalAngles - 0x1000;
          rowWeightQ12 = rowFractionOrNormalAngles - 0x1000;
          weightedHeightAccumulator = (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetOrNormalSum + 0x1c) * (longlong)diagonalWeightOrNormalSum -
                  ((longlong)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (longlong)columnWeightQ12 +
                  (longlong)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (longlong)rowWeightQ12);
          rowFractionOrNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                 sourceName + rowOffsetOrNormalSum + 8);
          interpolatedHeightQ12 = (uint)weightedHeightAccumulator >> 0xc | (int)((ulonglong)weightedHeightAccumulator >> 0x20) << 0x14;
          if ((int)interpolatedHeightQ12 < 0) {
            rowFractionOrNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.edx;
            firstNormalEcx = scaledNormal.ecx;
            firstNormalEax = scaledNormal.eax;
            rowFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,columnWeightQ12);
            columnOrNormalSum = firstNormalEax - scaledNormal.eax;
            rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.ecx;
            rowFractionOrNormalAngles = *(uint *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             cellOffsetOrNormalSum + -0x20);
            cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.edx;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (cellOffsetOrNormalSum - scaledNormal.edx,rowOffsetOrNormalSum - scaledNormal.ecx,columnOrNormalSum - scaledNormal.eax);
            rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.edx;
            firstNormalEcx = scaledNormal.ecx;
            firstNormalEax = scaledNormal.eax;
            rowFractionOrNormalAngles = *(uint *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,columnWeightQ12);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.eax;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.ecx;
            rowFractionOrNormalAngles = *(uint *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                             rowOffsetOrNormalSum + 8);
            columnOrNormalSum = firstNormalEdx - scaledNormal.edx;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum - scaledNormal.edx,diagonalWeightOrNormalSum - scaledNormal.ecx,cellOffsetOrNormalSum - scaledNormal.eax);
            rowFractionOrNormalAngles = blendedNormalAngles.edx << 0x10 | blendedNormalAngles.ecx & 0xffff;
            sampleFailed = false;
          }
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.carry = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.carry = sampleFailed;
  return sampleResult;
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
  FieldGridDimension initGridWidth;
  dword randomValue;
  dword materialVariantRandomBits;
  FieldGridDimension initColumnsRemaining;
  FieldGridDimension gridWidth;
  FieldGridDimension initRowsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridDimension topRowCellsRemaining;
  FieldGridCell *initializationCellCursor;
  int rowBytesOrBottomCellAddress;
  FieldGridCell *cellCursor;
  FieldGridCell *currentRowFirstCell;
  Q12 currentCellWorldXQ12;
  Q12 currentCellWorldYQ12;
  dword phaseSeedBitWidth;

  phaseSeedBitWidth = *(dword *)((int)g_TerrainSurfacePacketTablePayload + -0x20);
  initRowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
  initGridWidth = fieldGrid->gridWidth;
  initializationCellCursor = fieldGrid->cells;
  initColumnsRemaining = initGridWidth;
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
      initColumnsRemaining = initColumnsRemaining - 1;
    } while (initColumnsRemaining != 0);
    initRowsRemaining = initRowsRemaining - 1;
    initColumnsRemaining = initGridWidth;
  } while (initRowsRemaining != 0);
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
    rowBytesOrBottomCellAddress = (int)cellCursor - (int)currentRowFirstCell;
    currentRowFirstCell = (FieldGridCell *)((int)currentRowFirstCell + rowBytesOrBottomCellAddress);
    cellCursor = (FieldGridCell *)(rowBytesOrBottomCellAddress + (int)currentRowFirstCell);
    rowsRemaining = rowsRemaining - 1;
  } while (rowsRemaining != 0);
  rowBytesOrBottomCellAddress = (int)currentRowFirstCell * 2 - (int)cellCursor;
  do {
    *(uint *)(rowBytesOrBottomCellAddress + 0x50) = *(uint *)(rowBytesOrBottomCellAddress + 0x50) | 0x80000000;
    rowBytesOrBottomCellAddress = rowBytesOrBottomCellAddress + 0x80;
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
  byte *clampLookup;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  byte mappedRuntimeByte;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  clampLookup = g_TerrainByteClampLookup;
  columnsRemaining = gridWidth;
  do {
    do {
      /* The lookup is 64-KiB aligned: the original loads AH = channel byte, AL = runtime byte into the
         pointer's low word, i.e. indexes the table with (channel << 8) | runtime byte. */
      mappedRuntimeByte = clampLookup[(uint)currentCell->runtime60_6B[sourceChannelOffset + 0x10] << 8 |
                                      (uint)currentCell->runtime60_6B[8]];
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
  uint previousPackedAngles;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCosEdxEax8 scaledSinCosPair;
  FixedSinCosEdxEax8 angleBScaledSinCosPair;
  uint packedAnglesBeforeAdvance;
  
  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 0x100;
  do {
    previousPackedAngles = currentDirectionRecord->packedAngleA_low16_AngleB_high16;
    currentDirectionRecord->packedAngleA_low16_AngleB_high16 =
         currentDirectionRecord->packedAngleA_low16_AngleB_high16 +
         *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(previousPackedAngles & 0xffff,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = (int)scaledSinCosPair;
    currentDirectionRecord->angleAComponent1ScaledQ28 = (int)(scaledSinCosPair >> 0x20);
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled((int)previousPackedAngles >> 0x10,currentDirectionRecord->scaleB);
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
  FieldGridDimension boundsGridWidth;
  FieldGridDimension boundsGridHeight;
  FieldGridDimension rowLength;
  longlong rayEndYProduct;
  longlong rayEndXProduct;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  uint rayStartHalfCoord0Q12;
  uint endHalfCoordOrCurrentCoord1Q12;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  int stepsRemaining;
  bool traversalDone;
  TerrainDistanceEaxCf5 triangleHit;
  FieldGridRaycastEaxEdxCf9 missResult;
  FieldGridRaycastEaxEdxCf9 hitResult;
  FixedDirectionXyzRegs12 rayDirection;
  
  boundsGridWidth = fieldGrid->gridWidth;
  boundsGridHeight = fieldGrid->gridHeight;
  rayStartHalfCoord0Q12 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - rayStartHalfCoord0Q12;
  rayStartCoord0Q12 = rayStartHalfCoord0Q12 * 2;
  rowLength = fieldGrid->gridWidth;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * rowLength * 0x80 + -0xc);
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndYProduct = (longlong)(int)(rayDirection.eax + rayOriginYQ12) * 0x1c6e9c;
  rayEndXProduct = (longlong)(int)(rayDirection.ecx + rayOriginXQ12) * -0x20c8cc;
  endHalfCoordOrCurrentCoord1Q12 = (int)((ulonglong)rayEndXProduct >> 0x20) << 0xb | (uint)rayEndXProduct >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)rayEndYProduct >> 0x20) << 0xc | (uint)rayEndYProduct >> 0x14) - endHalfCoordOrCurrentCoord1Q12;
  rayEndCoord0Q12 = endHalfCoordOrCurrentCoord1Q12 * 2;
  stepsRemaining = 0x400;
  endHalfCoordOrCurrentCoord1Q12 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  do {
    stepsRemaining = stepsRemaining + -1;
    if (stepsRemaining == 0) break;
    if ((((-1 < (int)endHalfCoordOrCurrentCoord1Q12) && (-1 < (int)currentGridCoord0Q12)) &&
        ((int)endHalfCoordOrCurrentCoord1Q12 < (int)((boundsGridWidth - 1) * 0x1000))) &&
       ((int)currentGridCoord0Q12 < (int)((boundsGridHeight - 1) * 0x1000))) {
      cellLocalCoord1Q12 = currentGridCoord0Q12 + rayStartHalfCoord0Q12 * -2;
      triangleHit = TerrainTriangle_IntersectRayDistanceCf
                         (rayDirection.edx,rayEndCoord0Q12 + rayStartHalfCoord0Q12 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight,currentCell[rowLength].terrainHeight,
                          currentCell[1].terrainHeight,currentCell->terrainHeight,cellLocalCoord1Q12
                          ,endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12);
      if (!triangleHit.carry) {
        hitResult.carry = true;
        hitResult.distanceQ12 = triangleHit.distanceQ12;
        hitResult.materialOrCellIndex = currentCell->flagsAndMaterial & 0xff; /* low byte: material */
        return hitResult;
      }
      endHalfCoordOrCurrentCoord1Q12 = (endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12) + rayStartCoord1Q12;
      currentGridCoord0Q12 = cellLocalCoord1Q12 + rayStartCoord0Q12;
    }
    traversalDone = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        rowLength * 0x80,currentCell,currentGridCoord0Q12,endHalfCoordOrCurrentCoord1Q12);    
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    endHalfCoordOrCurrentCoord1Q12 = g_TerrainRayNextCoord1Q12;
    currentGridCoord0Q12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentGridCoord0Q12;
  missResult.distanceQ12 = 0x7fffffff;
  missResult.carry = false;
  return missResult;
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
  FieldGridDimension boundsGridWidth;
  FieldGridDimension boundsGridHeight;
  FieldGridDimension rowLength;
  longlong rayEndYProduct;
  longlong rayEndXProduct;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  uint rayStartHalfCoord0Q12;
  uint endHalfCoordOrCurrentCoord1Q12;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  int stepsRemaining;
  bool traversalDone;
  TerrainDistanceEaxCf5 triangleHit;
  FieldGridRaycastEaxEdxCf9 missResult;
  FieldGridRaycastEaxEdxCf9 hitResult;
  FixedDirectionXyzRegs12 rayDirection;
  
  boundsGridWidth = fieldGrid->gridWidth;
  boundsGridHeight = fieldGrid->gridHeight;
  rayStartHalfCoord0Q12 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - rayStartHalfCoord0Q12;
  rayStartCoord0Q12 = rayStartHalfCoord0Q12 * 2;
  rowLength = fieldGrid->gridWidth;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * rowLength * 0x80 + -0xc);
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndYProduct = (longlong)(int)(rayDirection.eax + rayOriginYQ12) * 0x1c6e9c;
  rayEndXProduct = (longlong)(int)(rayDirection.ecx + rayOriginXQ12) * -0x20c8cc;
  endHalfCoordOrCurrentCoord1Q12 = (int)((ulonglong)rayEndXProduct >> 0x20) << 0xb | (uint)rayEndXProduct >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)rayEndYProduct >> 0x20) << 0xc | (uint)rayEndYProduct >> 0x14) - endHalfCoordOrCurrentCoord1Q12;
  rayEndCoord0Q12 = endHalfCoordOrCurrentCoord1Q12 * 2;
  stepsRemaining = 0x400;
  endHalfCoordOrCurrentCoord1Q12 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  do {
    stepsRemaining = stepsRemaining + -1;
    if (stepsRemaining == 0) break;
    if ((((-1 < (int)endHalfCoordOrCurrentCoord1Q12) && (-1 < (int)currentGridCoord0Q12)) &&
        ((int)endHalfCoordOrCurrentCoord1Q12 < (int)((boundsGridWidth - 1) * 0x1000))) &&
       ((int)currentGridCoord0Q12 < (int)((boundsGridHeight - 1) * 0x1000))) {
      cellLocalCoord1Q12 = currentGridCoord0Q12 + rayStartHalfCoord0Q12 * -2;
      triangleHit = TerrainTriangle_IntersectRayDistanceCf
                         (rayDirection.edx,rayEndCoord0Q12 + rayStartHalfCoord0Q12 * -2,
                          rayEndCoord1Q12 - rayStartCoord1Q12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight +
                          currentCell[rowLength + 1].waterSurfaceDelta,
                          currentCell[rowLength].terrainHeight + currentCell[rowLength].waterSurfaceDelta,
                          currentCell[1].terrainHeight + currentCell[1].waterSurfaceDelta,
                          currentCell->waterSurfaceDelta + currentCell->terrainHeight,
                          cellLocalCoord1Q12,endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12);
      if (!triangleHit.carry) {
        hitResult.carry = true;
        hitResult.distanceQ12 = triangleHit.distanceQ12;
        hitResult.materialOrCellIndex = currentCell->flagsAndMaterial & 0xff; /* low byte: material */
        return hitResult;
      }
      endHalfCoordOrCurrentCoord1Q12 = (endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12) + rayStartCoord1Q12;
      currentGridCoord0Q12 = cellLocalCoord1Q12 + rayStartCoord0Q12;
    }
    traversalDone = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        rowLength * 0x80,currentCell,currentGridCoord0Q12,endHalfCoordOrCurrentCoord1Q12);    
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    endHalfCoordOrCurrentCoord1Q12 = g_TerrainRayNextCoord1Q12;
    currentGridCoord0Q12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentGridCoord0Q12;
  missResult.distanceQ12 = 0x7fffffff;
  missResult.carry = false;
  return missResult;
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
  FieldGridDimension rowLength;
  longlong rayEndYProduct;
  longlong rayEndXProduct;
  uint rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint rayStartCoord1Q12;
  int rayEndCoord1Q12;
  int columnOrClampOffset;
  int maxRowIndex;
  uint rayStartHalfCoord0Q12;
  uint endHalfCoordOrCurrentCoord1Q12;
  uint currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  int cellRowIndex;
  int maxColumnIndex;
  int stepsRemaining;
  FieldGridCell *currentCell;
  FieldGridCell *sampleCell;
  int rowStrideBytes;
  bool traversalDone;
  TerrainDistanceEaxCf5 triangleHit;
  FieldGridRaycastEaxEdxCf9 hitResult;
  FieldGridRaycastEaxEdxCf9 missResult;
  FixedDirectionXyzRegs12 rayDirection;
  FieldCellPersistedAux cornerHeight0Q12;
  FieldCellPersistedAux cornerHeight1Q12;
  FieldCellPersistedAux cornerHeight2Q12;
  FieldCellPersistedAux cornerHeight3Q12;
  
  maxColumnIndex = fieldGrid->gridWidth - 1;
  maxRowIndex = fieldGrid->gridHeight - 1;
  rayStartHalfCoord0Q12 = (int)((ulonglong)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((ulonglong)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - rayStartHalfCoord0Q12;
  rayStartCoord0Q12 = rayStartHalfCoord0Q12 * 2;
  rowLength = fieldGrid->gridWidth;
  rowStrideBytes = rowLength * 0x80;
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndYProduct = (longlong)(int)(rayDirection.eax + rayOriginYQ12) * 0x1c6e9c;
  rayEndXProduct = (longlong)(int)(rayDirection.ecx + rayOriginXQ12) * -0x20c8cc;
  endHalfCoordOrCurrentCoord1Q12 = (int)((ulonglong)rayEndXProduct >> 0x20) << 0xb | (uint)rayEndXProduct >> 0x15;
  rayEndCoord1Q12 = ((int)((ulonglong)rayEndYProduct >> 0x20) << 0xc | (uint)rayEndYProduct >> 0x14) - endHalfCoordOrCurrentCoord1Q12;
  rayEndCoord0Q12 = endHalfCoordOrCurrentCoord1Q12 * 2;
  stepsRemaining = 0x400;
  endHalfCoordOrCurrentCoord1Q12 = rayStartCoord1Q12 & 0xfffff000;
  currentGridCoord0Q12 = rayStartCoord0Q12 & 0xfffff000;
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartCoord1Q12 >> 0xc].runtime0C_3F +
                ((int)rayStartCoord0Q12 >> 0xc) * rowStrideBytes + -0xc);
  do {
    stepsRemaining = stepsRemaining + -1;
    if (stepsRemaining == 0) break;
    cellLocalCoord1Q12 = currentGridCoord0Q12 + rayStartHalfCoord0Q12 * -2;
    columnOrClampOffset = (int)((endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12) + rayStartCoord1Q12) >> 0xc;
    cellRowIndex = (int)(cellLocalCoord1Q12 + rayStartCoord0Q12) >> 0xc;
    if (columnOrClampOffset < 0) {
      sampleCell = currentCell + -columnOrClampOffset;
      columnOrClampOffset = cellRowIndex;
      if ((cellRowIndex < 0) || (columnOrClampOffset = cellRowIndex - maxRowIndex, maxRowIndex <= cellRowIndex)) {
        /* both clamped: the single corner cell */
        sampleCell = (FieldGridCell *)((int)sampleCell - columnOrClampOffset * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
      else {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[rowLength].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[rowLength].terrainHeight;
      }
    }
    else if (cellRowIndex < 0) {
      sampleCell = (FieldGridCell *)((int)currentCell - cellRowIndex * rowStrideBytes);
      if (columnOrClampOffset < maxColumnIndex) {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[1].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[1].terrainHeight;
      }
      else {
        sampleCell = sampleCell + -(columnOrClampOffset - maxColumnIndex);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
    }
    else if (columnOrClampOffset < maxColumnIndex) {
      if (cellRowIndex < maxRowIndex) {
        cornerHeight3Q12 = currentCell->terrainHeight;
        cornerHeight2Q12 = currentCell[1].terrainHeight;
        cornerHeight1Q12 = currentCell[rowLength].terrainHeight;
        cornerHeight0Q12 = currentCell[rowLength + 1].terrainHeight;
        sampleCell = currentCell;
      }
      else {
        sampleCell = (FieldGridCell *)((int)currentCell - (cellRowIndex - maxRowIndex) * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[1].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[1].terrainHeight;
      }
    }
    else {
      sampleCell = currentCell + -(columnOrClampOffset - maxColumnIndex);
      if (maxRowIndex <= cellRowIndex) {
        /* both clamped: the single corner cell */
        sampleCell = (FieldGridCell *)((int)sampleCell - (cellRowIndex - maxRowIndex) * rowStrideBytes);
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell->terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell->terrainHeight;
      }
      else {
        cornerHeight3Q12 = sampleCell->terrainHeight;
        cornerHeight2Q12 = sampleCell[rowLength].terrainHeight;
        cornerHeight1Q12 = sampleCell->terrainHeight;
        cornerHeight0Q12 = sampleCell[rowLength].terrainHeight;
      }
    }
    triangleHit = TerrainTriangle_IntersectRayDistanceCf
                       (rayDirection.edx,rayEndCoord0Q12 + rayStartHalfCoord0Q12 * -2,rayEndCoord1Q12 - rayStartCoord1Q12,
                        rayOriginZQ12,cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                        cornerHeight3Q12,cellLocalCoord1Q12,endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12);
    if (!triangleHit.carry) {
      hitResult.carry = true;
      hitResult.distanceQ12 = triangleHit.distanceQ12;
      hitResult.materialOrCellIndex = sampleCell->flagsAndMaterial & 0xff; /* low byte: material */
      return hitResult;
    }
    traversalDone = TerrainRay_AdvanceGridTraversalCf
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        rowStrideBytes,currentCell,currentGridCoord0Q12,endHalfCoordOrCurrentCoord1Q12);    
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    endHalfCoordOrCurrentCoord1Q12 = g_TerrainRayNextCoord1Q12;
    currentGridCoord0Q12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentGridCoord0Q12;
  missResult.distanceQ12 = 0x7fffffff;
  missResult.carry = false;
  return missResult;
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
  FieldGridAsset *activeFieldGrid;
  int gridColumnIndex;
  uint gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  activeFieldGrid = worldRuntime->fieldGrid;
  gridHalfRowCoordinateQ12 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnIndex = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800) >> 0xc;
  if ((((-1 < gridColumnIndex) && (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 0xc, -1 < gridRowIndex)) &&
      (gridColumnIndex < (int)activeFieldGrid->gridWidth)) &&
     ((gridRowIndex < (int)activeFieldGrid->gridHeight &&
      ((activeFieldGrid->cells[activeFieldGrid->gridWidth * gridRowIndex + gridColumnIndex].runtime60_6B
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
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->flagsAndMaterial = currentCell->flagsAndMaterial & ~FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15;
    currentCell = currentCell + 1;
    cellsRemaining = cellsRemaining + -1;
  } while (cellsRemaining != 0);
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
  FieldGridAsset *writeErrorValue;
  FieldGridAsset *fieldGridImageCopy;
  uint imageSizeOrDwordsLeft;
  int cellsRemaining;
  int occupancyBytesLeft;
  dword *copyDestinationDwords;
  FieldGridCellSaveImageView80 *fieldGridCellSaveView;
  byte *occupancyBytes;
  ArenaAllocEaxCf5 allocResult;
  StatusValueEaxCf5 writeStatus;
  ArenaFreeEaxCf5 freeResult;
  
  imageSizeOrDwordsLeft = sourceImageDwords[1];
  allocResult = (*g_MemoryApi.alloc)(imageSizeOrDwordsLeft);
  fieldGridImageCopy = (FieldGridAsset *)allocResult.eax;
  if (!allocResult.carry) {
    copyDestinationDwords = (dword *)fieldGridImageCopy;
    for (imageSizeOrDwordsLeft = imageSizeOrDwordsLeft >> 2; imageSizeOrDwordsLeft != 0; imageSizeOrDwordsLeft = imageSizeOrDwordsLeft - 1) {
      *copyDestinationDwords = *sourceImageDwords;
      sourceImageDwords = sourceImageDwords + 1;
      copyDestinationDwords = copyDestinationDwords + 1;
    }
    fieldGridCellSaveView = (FieldGridCellSaveImageView80 *)fieldGridImageCopy->cells;
    fieldGridImageCopy->fieldFlags = 0;
    cellsRemaining = fieldGridImageCopy->gridWidth * fieldGridImageCopy->gridHeight;
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
      for (occupancyBytesLeft = 8; occupancyBytesLeft != 0; occupancyBytesLeft = occupancyBytesLeft + -1) {
        *occupancyBytes = 0;
        occupancyBytes = occupancyBytes + 1;
      }
      fieldGridCellSaveView = fieldGridCellSaveView + 1;
      cellsRemaining = cellsRemaining + -1;
    } while (cellsRemaining != 0);
    writeStatus = FileSystem_WriteBufferToPathCf
                      ((fieldGridImageCopy->common).allocationSizeBytes,fieldGridImageCopy,
                       (word *)&g_LevelResourcePathScratchUtf16);
    if (!writeStatus.carry) {
      freeResult = (*g_MemoryApi.free)(fieldGridImageCopy);
      return THANDOR_BITCAST(qword, StatusValueEaxCf5, ((THANDOR_BITCAST(ArenaFreeEaxCf5, qword, freeResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
    writeErrorValue = (FieldGridAsset *)writeStatus.valueOrError;
    (*g_MemoryApi.free)(fieldGridImageCopy);
    fieldGridImageCopy = writeErrorValue;
  }
  writeStatus.carry = true;
  writeStatus.valueOrError = (dword)fieldGridImageCopy;
  return writeStatus;
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
  FieldGridDimension rowLength;
  int columnIndex;
  int rowIndex;
  int rowStrideBytes;
  FieldGridCell *neighborCell;
  
  rowLength = fieldGrid->gridWidth;
  columnIndex = worldXQ12 >> 0xc;
  if ((((-1 < columnIndex) && (rowIndex = worldYQ12 >> 0xc, -1 < rowIndex)) && (columnIndex < (int)rowLength)) &&
     (rowIndex < (int)fieldGrid->gridHeight)) {
    rowStrideBytes = rowLength * 0x80;
    neighborCell = fieldGrid->cells + columnIndex + rowIndex * rowLength;
    neighborCell->waterSurfaceDelta = neighborCell->waterSurfaceDelta + heightDeltaUnits * -0x40;
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell);
    FieldGridCell_ComputeDirectionalLightColor(neighborCell);
    if ((neighborCell[-1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell + -1);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell + -1);
    }
    if ((neighborCell[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell + 1);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell + 1);
    }
    neighborCell = neighborCell + -rowLength;
    if ((neighborCell->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell);
    }
    if ((neighborCell[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell + 1);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell + 1);
    }
    neighborCell = neighborCell + rowLength * 2 + -1;
    if ((neighborCell->flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell);
    }
    if ((neighborCell[1].flagsAndMaterial & 0x88006000) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,neighborCell + 1);
      FieldGridCell_ComputeDirectionalLightColor(neighborCell + 1);
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
  longlong scaledDeltaProduct;
  ulonglong distanceSquared;
  int deltaXOrRadiusSquaredHigh;
  int deltaY;
  uint radiusSquaredOrHeightDelta;
  uint distanceSquaredHigh;
  
  deltaXOrRadiusSquaredHigh = centerWorldXQ12 - cell->worldX;
  deltaY = cell->worldY - centerWorldYQ12;
  distanceSquared = (longlong)deltaY * (longlong)deltaY + (longlong)deltaXOrRadiusSquaredHigh * (longlong)deltaXOrRadiusSquaredHigh;
  distanceSquaredHigh = (uint)(distanceSquared >> 0x20);
  deltaXOrRadiusSquaredHigh = (int)((ulonglong)((longlong)radiusWorldUnits * (longlong)radiusWorldUnits) >> 0x20);
  radiusSquaredOrHeightDelta = (uint)((longlong)radiusWorldUnits * (longlong)radiusWorldUnits);
  if (((int)distanceSquaredHigh <= deltaXOrRadiusSquaredHigh) &&
     ((((int)distanceSquaredHigh < deltaXOrRadiusSquaredHigh || ((int)distanceSquared < (int)radiusSquaredOrHeightDelta)) &&
      (radiusSquaredOrHeightDelta = deltaXOrRadiusSquaredHigh << 0x14 | radiusSquaredOrHeightDelta >> 0xc, radiusSquaredOrHeightDelta != 0)))) {
    scaledDeltaProduct = (longlong)
            ((int)((longlong)((ulonglong)distanceSquaredHigh << 0x20 | distanceSquared & 0xffffffff) / (longlong)(int)radiusSquaredOrHeightDelta)
            + -0x1000) * (longlong)terrainHeightDeltaAmplitudeQ12;
    radiusSquaredOrHeightDelta = (int)((ulonglong)scaledDeltaProduct >> 0x20) << 0x14 | (uint)scaledDeltaProduct >> 0xc;
    cell->terrainHeight = cell->terrainHeight + radiusSquaredOrHeightDelta;
    if (-1 < cell->waterSurfaceDelta) {
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - radiusSquaredOrHeightDelta;
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
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int upperRowCellAddress;
  int neighborWaterDeltaAddress;
  FieldGridDimension gridWidth;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (int)fieldGrid + rowLength * -0x80 + (rowLength * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = rowLength - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -0x80;
      if ((-1 < *(int *)(cellAfterSourceAddress + -0x34)) &&
         ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x40000000) == 0)) {
        sourceSurfaceHeightQ12 =
             *(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38);
        upperRowCellAddress = cellAfterSourceAddress + rowLength * -0x80;
        if ((*(uint *)(upperRowCellAddress + 0x50) & 0x20000000) == 0) {
          neighborWaterDeltaAddress = upperRowCellAddress + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(upperRowCellAddress + 0x4c) + *(int *)(upperRowCellAddress + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(upperRowCellAddress + 0xd0) & 0x20000000) == 0) {
          *(int *)(upperRowCellAddress + 0xcc) =
               *(int *)(upperRowCellAddress + 0xcc) -
               ((*(int *)(upperRowCellAddress + 0xcc) + *(int *)(upperRowCellAddress + 200)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(upperRowCellAddress + 0x50 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint *)(upperRowCellAddress + -0x30 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + -0x34 + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + -0x34 + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + -0x38 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * 0x80;
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
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int upperRowCellAddress;
  int neighborWaterDeltaAddress;
  FieldGridDimension gridWidth;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (int)fieldGrid + rowLength * -0x80 + (rowLength * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = rowLength - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -0x80;
      if ((*(uint *)(cellAfterSourceAddress + -0x30) & 0x40000000) == 0) {
        sourceSurfaceHeightQ12 =
             *(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38);
        upperRowCellAddress = cellAfterSourceAddress + rowLength * -0x80;
        if ((*(uint *)(upperRowCellAddress + 0x50) & 0x20000000) == 0) {
          neighborWaterDeltaAddress = upperRowCellAddress + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(upperRowCellAddress + 0x4c) + *(int *)(upperRowCellAddress + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(upperRowCellAddress + 0xd0) & 0x20000000) == 0) {
          *(int *)(upperRowCellAddress + 0xcc) =
               *(int *)(upperRowCellAddress + 0xcc) -
               ((*(int *)(upperRowCellAddress + 0xcc) + *(int *)(upperRowCellAddress + 200)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint *)(upperRowCellAddress + 0x50 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint *)(upperRowCellAddress + -0x30 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + -0x34 + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + -0x34 + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + -0x38 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * 0x80;
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
  FieldGridDimension rowLength;
  longlong falloffProduct;
  uint spanRadiusQ12;
  int maxColumnOrCenterX;
  int spanColumnCount;
  dword cellDistance;
  int minColumnOrCenterY;
  int minRowOrSourceHeight;
  int blendedHeight;
  int maxRowOrColumnsLeft;
  int heightDifference;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  
  spanRadiusQ12 = radiusUnits * 0x40;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -0x40;
  }
  if (0x5000 < spanRadiusQ12) {
    spanRadiusQ12 = 0x5000;
  }
  minColumnOrCenterY = centerWorldXQ12 + spanRadiusQ12 * -4;
  minRowOrSourceHeight = centerWorldYQ12 + spanRadiusQ12 * -4;
  maxColumnOrCenterX = (int)(minColumnOrCenterY + 0xfff + spanRadiusQ12 * 8) >> 0xc;
  maxRowOrColumnsLeft = (int)(minRowOrSourceHeight + 0xfff + spanRadiusQ12 * 8) >> 0xc;
  minColumnOrCenterY = minColumnOrCenterY >> 0xc;
  if (minColumnOrCenterY < 0) {
    minColumnOrCenterY = 0;
  }
  minRowOrSourceHeight = minRowOrSourceHeight >> 0xc;
  if (minRowOrSourceHeight < 0) {
    minRowOrSourceHeight = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumnOrCenterX) {
    maxColumnOrCenterX = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRowOrColumnsLeft) {
    maxRowOrColumnsLeft = fieldGrid->gridHeight - 1;
  }
  if ((minColumnOrCenterY <= maxColumnOrCenterX) && (minRowOrSourceHeight <= maxRowOrColumnsLeft)) {
    spanColumnCount = (maxColumnOrCenterX - minColumnOrCenterY) + 1;
    minColumnOrCenterY = minRowOrSourceHeight * fieldGrid->gridWidth + minColumnOrCenterY;
    rowsRemaining = (maxRowOrColumnsLeft - minRowOrSourceHeight) + 1;
    accumulatorCursor = accumulatorPlane + minColumnOrCenterY;
    rowLength = fieldGrid->gridWidth;
    minRowOrSourceHeight = (centerWorldYQ12 >> 0xc) * rowLength + (centerWorldXQ12 >> 0xc);
    spanCell = fieldGrid->cells + minColumnOrCenterY;
    maxColumnOrCenterX = fieldGrid->cells[minRowOrSourceHeight].worldX;
    minColumnOrCenterY = fieldGrid->cells[minRowOrSourceHeight].worldY;
    minRowOrSourceHeight = fieldGrid->cells
            [(sourceWorldYQ12 >> 0xc) * fieldGrid->gridWidth + (sourceWorldXQ12 >> 0xc)].
            terrainHeight;
    maxRowOrColumnsLeft = spanColumnCount;
    rowStartCell = spanCell;
    rowStartAccumulator = accumulatorCursor;
    do {
      do {
        cellDistance = FixedMath_Length2(spanCell->worldY - minColumnOrCenterY,spanCell->worldX - maxColumnOrCenterX);
        if (cellDistance <= spanRadiusQ12 + 1) {
          heightDifference = (minRowOrSourceHeight + heightDeltaUnits * -0x40) - *accumulatorCursor;
          falloffProduct = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)cellDistance & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)cellDistance * 0x8000 & 0xffffffffU) / (longlong)(int)(spanRadiusQ12 + 1))
                   ] + 0x10000000) * (longlong)heightDifference;
          blendedHeight = ((int)((ulonglong)falloffProduct >> 0x20) << 3 | (uint)falloffProduct >> 0x1d) + *accumulatorCursor;
          if (heightDifference != 0) {
            if (heightDifference < 0) {
              if (blendedHeight < *accumulatorCursor) {
                *accumulatorCursor = blendedHeight;
              }
            }
            else if (*accumulatorCursor < blendedHeight) {
              *accumulatorCursor = blendedHeight;
            }
          }
        }
        spanCell = spanCell + 1;
        accumulatorCursor = accumulatorCursor + 1;
        maxRowOrColumnsLeft = maxRowOrColumnsLeft + -1;
      } while (maxRowOrColumnsLeft != 0);
      accumulatorCursor = rowStartAccumulator + rowLength;
      spanCell = rowStartCell + rowLength;
      rowsRemaining = rowsRemaining + -1;
      maxRowOrColumnsLeft = spanColumnCount;
      rowStartCell = spanCell;
      rowStartAccumulator = accumulatorCursor;
    } while (rowsRemaining != 0);
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
  int targetOffset;
  FieldGridDimension rowLength;
  longlong falloffProduct;
  uint spanRadiusQ12;
  int maxColumnOrCenterX;
  int spanColumnCount;
  dword cellDistance;
  int minColumnOrCenterY;
  int minRowOrColumnsLeft;
  int maxRowOrBlendedValue;
  FieldGridCell *spanCell;
  int *accumulatorCursor;
  FieldGridCell *rowStartCell;
  int *rowStartAccumulator;
  int rowsRemaining;
  
  spanRadiusQ12 = radiusUnits * 0x40;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -0x40;
  }
  if (0x5000 < spanRadiusQ12) {
    spanRadiusQ12 = 0x5000;
  }
  targetOffset = heightDeltaUnits * -0x40;
  minColumnOrCenterY = centerWorldXQ12 + spanRadiusQ12 * -2;
  minRowOrColumnsLeft = centerWorldYQ12 + spanRadiusQ12 * -2;
  maxColumnOrCenterX = (int)(minColumnOrCenterY + 0xfff + spanRadiusQ12 * 4) >> 0xc;
  maxRowOrBlendedValue = (int)(minRowOrColumnsLeft + 0xfff + spanRadiusQ12 * 4) >> 0xc;
  minColumnOrCenterY = minColumnOrCenterY >> 0xc;
  if (minColumnOrCenterY < 0) {
    minColumnOrCenterY = 0;
  }
  minRowOrColumnsLeft = minRowOrColumnsLeft >> 0xc;
  if (minRowOrColumnsLeft < 0) {
    minRowOrColumnsLeft = 0;
  }
  if ((int)fieldGrid->gridWidth <= maxColumnOrCenterX) {
    maxColumnOrCenterX = fieldGrid->gridWidth - 1;
  }
  if ((int)fieldGrid->gridHeight <= maxRowOrBlendedValue) {
    maxRowOrBlendedValue = fieldGrid->gridHeight - 1;
  }
  if ((minColumnOrCenterY <= maxColumnOrCenterX) && (minRowOrColumnsLeft <= maxRowOrBlendedValue)) {
    spanColumnCount = (maxColumnOrCenterX - minColumnOrCenterY) + 1;
    minColumnOrCenterY = minRowOrColumnsLeft * fieldGrid->gridWidth + minColumnOrCenterY;
    rowsRemaining = (maxRowOrBlendedValue - minRowOrColumnsLeft) + 1;
    accumulatorCursor = accumulatorPlane + minColumnOrCenterY;
    rowLength = fieldGrid->gridWidth;
    minRowOrColumnsLeft = (centerWorldYQ12 >> 0xc) * rowLength + (centerWorldXQ12 >> 0xc);
    spanCell = fieldGrid->cells + minColumnOrCenterY;
    maxColumnOrCenterX = fieldGrid->cells[minRowOrColumnsLeft].worldX;
    minColumnOrCenterY = fieldGrid->cells[minRowOrColumnsLeft].worldY;
    minRowOrColumnsLeft = spanColumnCount;
    rowStartCell = spanCell;
    rowStartAccumulator = accumulatorCursor;
    do {
      do {
        cellDistance = FixedMath_Length2(spanCell->worldY - minColumnOrCenterY,spanCell->worldX - maxColumnOrCenterX);
        if (cellDistance <= spanRadiusQ12 + 1) {
          falloffProduct = (longlong)
                  (g_FixedCosQ28
                   [(int)((longlong)
                          ((((longlong)(int)cellDistance & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (longlong)(int)cellDistance * 0x8000 & 0xffffffffU) / (longlong)(int)(spanRadiusQ12 + 1))
                   ] + 0x10000000) * (longlong)targetOffset;
          maxRowOrBlendedValue = ((int)((ulonglong)falloffProduct >> 0x20) << 3 | (uint)falloffProduct >> 0x1d) + *accumulatorCursor;
          if (targetOffset < 0) {
            if (maxRowOrBlendedValue < targetOffset) {
              maxRowOrBlendedValue = targetOffset;
            }
          }
          else if (targetOffset < maxRowOrBlendedValue) {
            maxRowOrBlendedValue = targetOffset;
          }
          *accumulatorCursor = maxRowOrBlendedValue;
        }
        spanCell = spanCell + 1;
        accumulatorCursor = accumulatorCursor + 1;
        minRowOrColumnsLeft = minRowOrColumnsLeft + -1;
      } while (minRowOrColumnsLeft != 0);
      accumulatorCursor = rowStartAccumulator + rowLength;
      spanCell = rowStartCell + rowLength;
      rowsRemaining = rowsRemaining + -1;
      minRowOrColumnsLeft = spanColumnCount;
      rowStartCell = spanCell;
      rowStartAccumulator = accumulatorCursor;
    } while (rowsRemaining != 0);
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
  int columnOrCellIndex;
  
  gridRowIndex = worldYQ12 >> 0xc;
  if ((((-1 < gridRowIndex) && (columnOrCellIndex = worldXQ12 >> 0xc, -1 < columnOrCellIndex)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (columnOrCellIndex < (int)fieldGrid->gridWidth)) {
    columnOrCellIndex = gridRowIndex * fieldGrid->gridWidth + columnOrCellIndex;
    fieldGrid->cells[columnOrCellIndex].flagsAndMaterial =
         fieldGrid->cells[columnOrCellIndex].flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK | transitionValue;
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
  Q12 *heightField;
  int rowOrHeightDelta;
  int columnOrCellIndex;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowOrHeightDelta = worldYQ12 >> 0xc;
  columnOrCellIndex = worldXQ12 >> 0xc;
  if ((((1 < rowOrHeightDelta) && (1 < columnOrCellIndex)) && (rowOrHeightDelta + 1 < (int)fieldGrid->gridHeight)) &&
     (columnOrCellIndex + 1 < (int)gridWidth)) {
    columnOrCellIndex = (rowOrHeightDelta + -1) * gridWidth + columnOrCellIndex;
    rowOrHeightDelta = (fieldGrid->cells[columnOrCellIndex].terrainHeight +
             *(int *)((&fieldGrid[1].common.buildMetadata.names)[columnOrCellIndex].producerName + 0xc) +
             fieldGrid->cells[columnOrCellIndex + (gridWidth - 1)].terrainHeight +
             *(int *)((&fieldGrid[1].common.buildMetadata.names)[columnOrCellIndex + gridWidth].producerName +
                     0xc) + *(int *)(fieldGrid->sourcePath + columnOrCellIndex * 0x40 + gridWidth * 0x80 + 100)
            + fieldGrid->cells[columnOrCellIndex + gridWidth * 2].terrainHeight) / 6 -
            fieldGrid->cells[columnOrCellIndex + gridWidth].terrainHeight;
    heightField = &fieldGrid->cells[columnOrCellIndex + gridWidth].terrainHeight;
    *heightField = *heightField + rowOrHeightDelta;
    heightField = &fieldGrid->cells[columnOrCellIndex + gridWidth].waterSurfaceDelta;
    *heightField = *heightField - rowOrHeightDelta;
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
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  
  gridHalfRowCoordinateQ12 =
       (int)((ulonglong)((longlong)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint)((longlong)worldY * -0x20c8cc) >> 0x15;
  gridCoordinates.rowQ12 = gridHalfRowCoordinateQ12 * 2;
  gridCoordinates.columnQ12 =
       ((int)((ulonglong)((longlong)worldX * 0x1c6e9c) >> 0x20) << 0xc |
       (uint)((longlong)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12;
  return gridCoordinates;
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
  int columnOrCellIndex;
  
  gridRowIndex = worldYQ12 >> 0xc;
  if ((((-1 < gridRowIndex) && (columnOrCellIndex = worldXQ12 >> 0xc, -1 < columnOrCellIndex)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (columnOrCellIndex < (int)fieldGrid->gridWidth)) {
    columnOrCellIndex = gridRowIndex * fieldGrid->gridWidth + columnOrCellIndex;
    fieldGrid->cells[columnOrCellIndex].flagsAndMaterial =
         preserveMask & fieldGrid->cells[columnOrCellIndex].flagsAndMaterial | setMask;
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
  int rightDeltaOrNegativeStride;
  int neighborDeltaA;
  int neighborDeltaB;
  int neighborDeltaC;
  int neighborDeltaD;
  int neighborDeltaE;
  int neighborDeltaF;
  FixedMathVectorAnglesRegs8 normalAngles;
  
  rightDeltaOrNegativeStride = cell[1].terrainHeight - cell->terrainHeight;
  neighborDeltaA = *(int *)(cell->runtime60_6B + rowStrideBytes + -0x18) - cell->terrainHeight;
  neighborDeltaB = *(int *)((int)cell + (0x48 - rowStrideBytes)) - cell->terrainHeight;
  neighborDeltaC = cell[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaD = *(int *)(cell->runtime0C_3F + rowStrideBytes + -0x44) - cell->terrainHeight;
  neighborDeltaE = *(int *)((int)cell + (200 - rowStrideBytes)) - cell->terrainHeight;
  normalAngles = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x1c) -
                                    cell->worldY) * neighborDeltaA) - (cell[1].worldY - cell->worldY) * rightDeltaOrNegativeStride
                                 ) - (*(int *)((int)cell + (0x44 - rowStrideBytes)) - cell->worldY)
                                     * neighborDeltaB) - (cell[-1].worldY - cell->worldY) * neighborDeltaC) -
                              (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x48) - cell->worldY)
                              * neighborDeltaD) -
                              (*(int *)((int)cell + (0xc4 - rowStrideBytes)) - cell->worldY) * neighborDeltaE
                     ,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x20) - cell->worldX) *
                           neighborDeltaA) - (cell[1].worldX - cell->worldX) * rightDeltaOrNegativeStride) -
                        (*(int *)((int)cell + (0x40 - rowStrideBytes)) - cell->worldX) * neighborDeltaB) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaC) -
                      (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x4c) - cell->worldX) * neighborDeltaD
                      ) - (*(int *)((int)cell + (0xc0 - rowStrideBytes)) - cell->worldX) * neighborDeltaE);
  cell->triangle0NormalAngles = normalAngles.ecx | normalAngles.edx << 0x10;
  rightDeltaOrNegativeStride = -rowStrideBytes;
  neighborDeltaA = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaB = ((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x18) +
           *(int *)(cell->runtime60_6B + rowStrideBytes + -0x14)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaC = ((*(int *)((int)cell + rightDeltaOrNegativeStride + 0x48) + *(int *)((int)cell + rightDeltaOrNegativeStride + 0x4c)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  neighborDeltaD = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaE = ((*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x44) +
           *(int *)(cell->runtime0C_3F + rowStrideBytes + -0x40)) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaF = ((*(int *)((int)cell + rightDeltaOrNegativeStride + 200) + *(int *)((int)cell + rightDeltaOrNegativeStride + 0xcc)) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  normalAngles = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x1c) -
                                    cell->worldY) * neighborDeltaB) - (cell[1].worldY - cell->worldY) * neighborDeltaA
                                 ) - (*(int *)((int)cell + rightDeltaOrNegativeStride + 0x44) - cell->worldY) * neighborDeltaC) -
                               (cell[-1].worldY - cell->worldY) * neighborDeltaD) -
                              (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x48) - cell->worldY)
                              * neighborDeltaE) - (*(int *)((int)cell + rightDeltaOrNegativeStride + 0xc4) - cell->worldY) * neighborDeltaF
                     ,((((-((*(int *)(cell->runtime60_6B + rowStrideBytes + -0x20) - cell->worldX) *
                           neighborDeltaB) - (cell[1].worldX - cell->worldX) * neighborDeltaA) -
                        (*(int *)((int)cell + rightDeltaOrNegativeStride + 0x40) - cell->worldX) * neighborDeltaC) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaD) -
                      (*(int *)(cell->runtime0C_3F + rowStrideBytes + -0x4c) - cell->worldX) * neighborDeltaE
                      ) - (*(int *)((int)cell + rightDeltaOrNegativeStride + 0xc0) - cell->worldX) * neighborDeltaF);
  cell->triangle1NormalAngles = normalAngles.ecx | normalAngles.edx << 0x10;
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
  FixedDirectionXyzRegs12 normalDirection;
  PackedArgb32 directionalLightColor;
  
  normalDirection = FixedMath_DirectionFromAnglesQ28Regs
                    ((int)cell->triangle0NormalAngles >> 0x10,cell->triangle0NormalAngles & 0xffff);
  /* the signed Q8 dot product indexes -256..256: g_TerrainLightingColorRampArgb256 lies directly
     before this table and holds the shaded half */
  directionalLightColor =
       ((PackedArgb32 *)g_TerrainDirectionalLightColorLut)
       [(int)((ulonglong)((longlong)(int)normalDirection.eax * (longlong)(int)g_TerrainLightDirectionX) >> 0x20) +
        (int)((ulonglong)((longlong)(int)normalDirection.ecx * (longlong)(int)g_TerrainLightDirectionY) >> 0x20) +
        (int)((ulonglong)((longlong)(int)normalDirection.edx * (longlong)(int)g_TerrainLightDirectionZ) >> 0x20) >>
        0x10];
  cell->secondarySurfaceDirectionalLightColor5C = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor58 = directionalLightColor;
  return;
}

