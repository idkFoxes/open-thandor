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
FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnCountOrColumnsLeft;
  int minRowOrColumnsLeft;
  uint32_t horizontalRadiusQ12;
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
    horizontalRadiusQ12 = (int)((uint64_t)((int64_t)radiusWorldUnits * 0x1bb6) >> 0x20) << 0x14 |
            (uint32_t)((int64_t)radiusWorldUnits * 0x1bb6) >> 0xc;
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
          (FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
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
   Cross-module calls: SelectionPlayerPairList_ContainsPair [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyPositiveCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  uint32_t remainingPairCount;
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
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
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
   Cross-module calls: SelectionPlayerPairList_ContainsPair [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyNegativeCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  int cellDelta;
  uint32_t remainingPairCount;
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
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorWorldYQ12,anchorWorldXQ12,playerRuntimeId);
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
   Cross-module calls: SelectionPlayerPairList_ContainsPair [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 worldYQ12,Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  FieldGridDimension rowLength;
  uint32_t remainingPairCount;
  int remainingCellCount;
  int rowStrideBytes;
  FieldGridCell *cell;
  FieldGridCell *scanCell;
  SelectionPlayerPairRecord *pairRecord;
  int *scratchHeightCursor;
  bool containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(worldYQ12,worldXQ12,playerRuntimeId);
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
   Recomputes the packed normal angles of both terrain triangles of every interior cell (the one-cell
   border ring is skipped) after the heights changed, and marks the field grid dirty (runtimeStateFlags
   bit 0).
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;

  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1); Ghidra addresses fieldGrid->cells through a field behind the asset */
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 rowLength * 0x80 - 0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowLength * 0x80,cell); /* row stride in bytes */
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3; /* skip the right border cell of this row and the left one of the next */
    } while (rowsLeft != 0);
  }
}


/* Address: 0x00505700.
   Sets the terrain light direction (g_TerrainLightDirectionX/Y/Z, Q28) from an elevation and azimuth
   and relights every interior cell with it (border ring skipped); marks the field grid dirty.
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
  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1), see FieldGrid_RecomputeInteriorTriangleNormalAngles */
    cellCursor = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 rowLength * 0x80 - 0x28);
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3;
    } while (rowsLeft != 0);
  }
}


/* Address: 0x005090E0.
   Terrain shaping at a world point (used by armies whose class shapes the ground under them): sets the
   grid vertex nearest to (worldX, worldY) to the height worldZ (moving its water surface by the opposite
   amount, so the water level stays) and lets the six wedge scans around
   the vertex adapt the neighbouring terrain; heightDeltaSourceValue / 0x240 (clamped to 1..255) limits
   those scans. Border cells and cells under water are left alone. (The original also returns CF: clear
   when the height was applied, set otherwise; this C version returns nothing.)
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  Q12 *heightField;
  uint32_t baseColumn;
  uint32_t columnFractionQ12;
  int heightDeltaOrRowStride;
  uint32_t fractionSumOrWidth;
  uint32_t rowFractionQ12;
  int cellIndex;
  FieldGridCell *wedgeCellA;
  FieldGridCell *wedgeCellB;
  FieldGridCell *cell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  uint32_t targetRow;
  uint32_t targetColumn;
  
  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = heightDeltaSourceValue / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (255 < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 255;
    }
    g_TerrainScanReferenceHeight = worldZQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    baseColumn = gridCoordinates.columnQ12 >> 12;
    targetRow = gridCoordinates.rowQ12 >> 12;
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, uint64_t, gridCoordinates) & 0xfff00000fff) >> 32);
    /* pick the nearest vertex of the triangulated cell from the Q12 fractions (0x1000 = one cell) */
    fractionSumOrWidth = rowFractionQ12 + columnFractionQ12 * 2;
    targetColumn = baseColumn;
    if (fractionSumOrWidth < 0x1000) {
      if (0xfff < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow++;
      }
    }
    else if (fractionSumOrWidth < 0x2001) {
      targetColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        targetRow++;
        targetColumn = baseColumn;
      }
    }
    else {
      targetColumn = baseColumn + 1;
      if (0x1fff < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow++;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    if ((((-1 < (int)targetColumn) && (fractionSumOrWidth = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)targetRow)) &&
        (targetRow < fieldGrid->gridHeight)) &&
       (((targetColumn < fractionSumOrWidth &&
         (cellIndex = targetRow * fractionSumOrWidth + targetColumn,
         (fieldGrid->cells[cellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) &&
        (fieldGrid->cells[cellIndex].waterSurfaceDelta < 1)))) {
      heightDeltaOrRowStride = g_TerrainScanReferenceHeight - fieldGrid->cells[cellIndex].terrainHeight;
      heightField = &fieldGrid->cells[cellIndex].terrainHeight;
      *heightField = *heightField + heightDeltaOrRowStride;
      heightField = &fieldGrid->cells[cellIndex].waterSurfaceDelta;
      *heightField = *heightField - heightDeltaOrRowStride;
      heightDeltaOrRowStride = g_TerrainScanRowStrideBytes;
      /* the six neighbours of vertex cell C, one per wedge: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid
         width; the first address is cells[cellIndex + 1], and runtime0C_3F - 0xC is a cell's own address) */
      wedgeCellA = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               cellIndex * 0x80 - 0x28);
      wedgeCellB = (FieldGridCell *)((int)wedgeCellA - g_TerrainScanRowStrideBytes);
      TerrainHeightDelta_ApplyWedge0(0,wedgeCellA);
      cell = wedgeCellB - 1;
      TerrainHeightDelta_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)(cell[-1].runtime0C_3F + heightDeltaOrRowStride - 0xc);
      TerrainHeightDelta_ApplyWedge2(0,cell);
      wedgeCellB = (FieldGridCell *)(wedgeCellA->runtime0C_3F + heightDeltaOrRowStride - 0xc);
      TerrainHeightDelta_ApplyWedge3(0,wedgeCellA);
      TerrainHeightDelta_ApplyWedge4(0,wedgeCellB);
      TerrainHeightDelta_ApplyWedge5(0,wedgeCellB + 1);
      return;
    }
  }
}


/* Address: 0x005618A0.
   Ownership: world/terrain/grid.
   Purpose: Applies one compact local cell update to the active field grid and marks the affected runtime state
   dirty. EAX, ECX, and EDX are preserved or incidental caller state and are not synthetic parameters or normal
   returns.
   Local calls: FieldGrid_ApplySingleCellTransition.
   Cross-module calls: SelectionPlayerPairList_ContainsPair [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 worldYQ12,
          Q12 worldXQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  uint32_t remainingPairCount;
  SelectionPlayerPairRecord *pairRecord;
  bool containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(worldYQ12,worldXQ12,playerRuntimeId);
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
  uint32_t preserveMask;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
  setMask = 0x800 << ((uint8_t)materialBitIndex & 0x1f);
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
   Snaps a world position to the nearest grid vertex (cell): returns that cell's worldX (EAX), worldY (ECX)
   and terrain height (EDX) with CF clear. Outside the grid CF is set and the input position comes back
   with height 0.
*/
TerrainPointResult
FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int columnOrCellIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  Q12 terrainHeightQ12;
  bool outOfBounds;
  TerrainPointResult nearestPoint;

  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldY * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
       (uint32_t)((int64_t)worldY * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  columnOrCellIndex = (int)((((int)((uint64_t)((int64_t)worldX * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldX * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - gridHalfRowCoordinateQ12) + 0x800)
          >> 12;
  if ((((columnOrCellIndex < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 12, gridRowIndex < 0)) ||
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
  nearestPoint.worldYQ12 = worldY;
  nearestPoint.worldXQ12 = worldX;
  nearestPoint.outOfBounds = outOfBounds;
  nearestPoint.terrainHeightQ12 = terrainHeightQ12;
  return nearestPoint;
}


/* Address: 0x004FEB10.
   Ownership: world/terrain/grid.
   Purpose: Nearest-cell companion that returns worldX in EAX, worldY in ECX, and terrainHeight + waterSurfaceDelta
   in EDX. CF reports bounds success.
*/
SurfacePointResult
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int columnCellOrSurfaceZ;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  bool outOfBounds;
  SurfacePointResult nearestPoint;
  
  gridHalfRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldY * -0x20c8cc) >> 0x15;
  columnCellOrSurfaceZ = (int)((((int)((uint64_t)((int64_t)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint32_t)((int64_t)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
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
  nearestPoint.outOfBounds = outOfBounds;
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
int32_t __thandor_eax_preserve_ecx_edx
FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int32_t columnOrWaterDelta;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  gridHalfRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldY * -0x20c8cc) >> 0x15;
  columnOrWaterDelta = (int)((((int)((uint64_t)((int64_t)worldX * 0x1c6e9c) >> 0x20) << 0xc |
                 (uint32_t)((int64_t)worldX * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
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
HeightSampleResult __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int64_t upperTriangleAccumulator;
  int gridColumnIndex;
  uint32_t gridColumnCoordinateQ12;
  uint32_t columnFractionOrHeightQ12;
  uint32_t gridRowCoordinateQ12;
  uint32_t rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  bool sampleFailed;
  HeightSampleResult sampleResult;
  FieldGridDimension gridWidth;
  int64_t weightedHeightAccumulator;
  
  gridRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnCoordinateQ12 =
       ((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - gridRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 0xc;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionOrHeightQ12 = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint32_t *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000)
           == 0) &&
         ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionOrHeightQ12 + rowFractionQ12) - 0x1000;
        if (columnFractionOrHeightQ12 + rowFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (int64_t)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + rowIndexOrRowOffsetBytes + 0x18) * (int64_t)(int)columnFractionOrHeightQ12 +
               ((int64_t)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18)
                * (int64_t)(int)rowFractionQ12 -
               (int64_t)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) *
               (int64_t)triangleDiagonalWeightQ12);
          columnFractionOrHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc |
                  (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        else {
          upperTriangleAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x18) *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x18) * (int64_t)(int)(columnFractionOrHeightQ12 - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x18) * (int64_t)(int)(rowFractionQ12 - 0x1000));
          columnFractionOrHeightQ12 = (uint32_t)upperTriangleAccumulator >> 0xc | (int)((uint64_t)upperTriangleAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        sampleResult.failed = sampleFailed;
        sampleResult.heightQ12 = columnFractionOrHeightQ12;
        return sampleResult;
      }
    }
  }
  columnFractionOrHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.failed = sampleFailed;
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
int32_t __thandor_eax_preserve_ecx_edx
FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int64_t upperTriangleAccumulator;
  int gridColumnIndex;
  uint32_t gridColumnCoordinateQ12;
  uint32_t columnFractionQ12;
  uint32_t gridRowCoordinateQ12;
  uint32_t rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  FieldGridDimension gridWidth;
  int64_t weightedWaterDeltaAccumulator;
  
  gridRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldY * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldY * -0x20c8cc) >> 0x15;
  gridColumnCoordinateQ12 =
       ((int)((uint64_t)((int64_t)worldX * 0x1c6e9c) >> 0x20) << 0xc |
       (uint32_t)((int64_t)worldX * 0x1c6e9c) >> 0x14) - gridRowCoordinateQ12;
  gridWidth = field->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 0xc;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)field->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionQ12 = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      if (((*(uint32_t *)(field->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000) == 0
          ) && ((*(uint32_t *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                                producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - 0x1000;
        if (columnFractionQ12 + rowFractionQ12 < 0x1000) {
          weightedWaterDeltaAccumulator =
               (int64_t)
               *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c) * (int64_t)(int)columnFractionQ12 +
               ((int64_t)
                *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
                (int64_t)(int)rowFractionQ12 -
               (int64_t)*(int *)(field->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
               (int64_t)triangleDiagonalWeightQ12);
          return (uint32_t)weightedWaterDeltaAccumulator >> 0xc |
                 (int)((uint64_t)weightedWaterDeltaAccumulator >> 0x20) << 0x14;
        }
        upperTriangleAccumulator = (int64_t)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                              producerName + rowIndexOrRowOffsetBytes + 0x1c) * (int64_t)triangleDiagonalWeightQ12 -
                ((int64_t)
                 *(int *)(field->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14) *
                 (int64_t)(int)(columnFractionQ12 - 0x1000) +
                (int64_t)
                *(int *)((int)(&field[1].common.buildMetadata.names)[gridColumnIndex].producerName +
                        rowIndexOrRowOffsetBytes + 0x1c) * (int64_t)(int)(rowFractionQ12 - 0x1000));
        return (uint32_t)upperTriangleAccumulator >> 0xc | (int)((uint64_t)upperTriangleAccumulator >> 0x20) << 0x14;
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
HeightSampleResult __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int cellColumn;
  uint32_t surfaceHeightQ12;
  Q12 gridRowFixedQ12;
  uint32_t rowFractionQ12;
  Q12 gridColumnFixedQ12;
  uint32_t columnFractionQ12;
  int rowIndexOrRowOffsetBytes;
  Q12 upperTriangleWeightQ12;
  bool sampleFailed;
  HeightSampleResult sampleResult;
  int64_t upperTriangleWeightedHeightAccumulator;
  FieldGridDimension gridWidth;
  int64_t weightedHeightAccumulator;
  
  gridColumnFixedQ12 =
       (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  gridRowFixedQ12 =
       ((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - gridColumnFixedQ12;
  gridWidth = fieldGrid->gridWidth;
  cellColumn = gridRowFixedQ12 >> 0xc;
  if (((-1 < cellColumn) && (rowIndexOrRowOffsetBytes = gridColumnFixedQ12 * 2 >> 0xc, -1 < rowIndexOrRowOffsetBytes)) &&
     (cellColumn < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      rowFractionQ12 = gridRowFixedQ12 & 0xfff;
      columnFractionQ12 = gridColumnFixedQ12 * 2 & 0xfff;
      if (((*(uint32_t *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & 0x88006000) == 0)
         && ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + rowIndexOrRowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        upperTriangleWeightQ12 = (rowFractionQ12 + columnFractionQ12) - 0x1000;
        if (rowFractionQ12 + columnFractionQ12 < 0x1000) {
          weightedHeightAccumulator =
               (int64_t)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c)) * (int64_t)(int)rowFractionQ12 +
               ((int64_t)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
                (int64_t)(int)columnFractionQ12 -
               (int64_t)
               (*(int *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
               *(int *)(fieldGrid->cells[cellColumn].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
               (int64_t)upperTriangleWeightQ12);
          surfaceHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc |
                  (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        else {
          upperTriangleWeightedHeightAccumulator =
               (int64_t)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                              producerName + rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn + gridWidth].
                             producerName + rowIndexOrRowOffsetBytes + 0x1c)) * (int64_t)upperTriangleWeightQ12 -
               ((int64_t)
                (*(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) +
                *(int *)(fieldGrid->cells[cellColumn + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)) *
                (int64_t)(int)(rowFractionQ12 - 0x1000) +
               (int64_t)
               (*(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                        rowIndexOrRowOffsetBytes + 0x18) +
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[cellColumn].producerName +
                       rowIndexOrRowOffsetBytes + 0x1c)) * (int64_t)(int)(columnFractionQ12 - 0x1000));
          surfaceHeightQ12 = (uint32_t)upperTriangleWeightedHeightAccumulator >> 0xc |
                  (int)((uint64_t)upperTriangleWeightedHeightAccumulator >> 0x20) << 0x14;
          sampleFailed = false;
        }
        sampleResult.failed = sampleFailed;
        sampleResult.heightQ12 = surfaceHeightQ12;
        return sampleResult;
      }
    }
  }
  surfaceHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.failed = sampleFailed;
  sampleResult.heightQ12 = surfaceHeightQ12;
  return sampleResult;
}


/* Address: 0x004FEFF0.
   Height of the top surface (terrain plus water above it) at a world position, interpolated linearly
   over the grid triangle that contains it; one of the five height samplers of the field-grid
   interpolation table. Q12 height in EAX; CF set (height 0) outside the grid or on a border cell.
*/
HeightSampleResult __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int64_t partialAccumulator;
  int gridColumnIndex;
  uint32_t gridColumnCoordinateQ12;
  uint32_t columnFractionOrWaterDelta;
  uint32_t gridRowCoordinateQ12;
  uint32_t rowFractionQ12;
  uint32_t terrainHeightQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  bool sampleFailed;
  HeightSampleResult sampleResult;
  FieldGridDimension gridWidth;
  int64_t weightedSurfaceAccumulator;
  
  /* FieldGrid_WorldToGridQ12 inlined; gridRowCoordinateQ12 is half the row coordinate */
  gridRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
       (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  gridColumnCoordinateQ12 =
       ((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
       (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - gridRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> 12;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 12, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * 0x80;
      columnFractionOrWaterDelta = gridColumnCoordinateQ12 & 0xfff;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & 0xfff;
      /* Ghidra's addressing of the cells around (row, column), rowIndexOrRowOffsetBytes being the row's byte
         offset: cells[column].runtime60_6B - 0x18/-0x14/-0x10 are the cell's terrainHeight (+0x48),
         waterSurfaceDelta (+0x4C) and flagsAndMaterial (+0x50); the names[...].producerName + 0x18/0x1C/0x20
         forms are the same fields of the right neighbour (index column) and of the cell diagonally below
         right (index column + gridWidth), cells[column + gridWidth] is the cell below. */
      if (((*(uint32_t *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x10) & FIELD_CELL_GRID_EDGE_MASK)
           == 0) &&
         ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex + gridWidth].
                          producerName + rowIndexOrRowOffsetBytes + 0x20) & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        /* fx + fy - 1: which of the cell's two triangles, and the weight of the far vertex */
        triangleDiagonalWeightQ12 = (columnFractionOrWaterDelta + rowFractionQ12) - 0x1000;
        if (columnFractionOrWaterDelta + rowFractionQ12 < 0x1000) {
          weightedSurfaceAccumulator =
               (int64_t)
               *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                             producerName + rowIndexOrRowOffsetBytes + 0x18) * (int64_t)(int)columnFractionOrWaterDelta +
               ((int64_t)
                *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18)
                * (int64_t)(int)rowFractionQ12 -
               (int64_t)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x18) *
               (int64_t)triangleDiagonalWeightQ12);
          terrainHeightQ12 =
               (int)((uint64_t)weightedSurfaceAccumulator >> 32) << 20 |
               (uint32_t)weightedSurfaceAccumulator >> 12;
          partialAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x1c) * (int64_t)(int)columnFractionOrWaterDelta +
                  ((int64_t)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x14) * (int64_t)(int)rowFractionQ12 -
                  (int64_t)*(int *)(fieldGrid->cells[gridColumnIndex].runtime60_6B + rowIndexOrRowOffsetBytes + -0x14)
                  * (int64_t)triangleDiagonalWeightQ12);
          columnFractionOrWaterDelta = (int)((uint64_t)partialAccumulator >> 32) << 20 | (uint32_t)partialAccumulator >> 12;
          if (-1 < (int)columnFractionOrWaterDelta) {
            terrainHeightQ12 = terrainHeightQ12 + columnFractionOrWaterDelta;
          }
          sampleFailed = false;
        }
        else {
          partialAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x18) *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x18) * (int64_t)(int)(columnFractionOrWaterDelta - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x18) * (int64_t)(int)(rowFractionQ12 - 0x1000));
          terrainHeightQ12 = (int)((uint64_t)partialAccumulator >> 32) << 20 | (uint32_t)partialAccumulator >> 12;
          partialAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)
                                [gridColumnIndex + gridWidth].producerName + rowIndexOrRowOffsetBytes + 0x1c) *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   *(int *)(fieldGrid->cells[gridColumnIndex + gridWidth].runtime60_6B +
                           rowIndexOrRowOffsetBytes + -0x14) * (int64_t)(int)(columnFractionOrWaterDelta - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[gridColumnIndex].
                                producerName + rowIndexOrRowOffsetBytes + 0x1c) * (int64_t)(int)(rowFractionQ12 - 0x1000));
          columnFractionOrWaterDelta = (int)((uint64_t)partialAccumulator >> 32) << 20 | (uint32_t)partialAccumulator >> 12;
          if (-1 < (int)columnFractionOrWaterDelta) {
            terrainHeightQ12 = terrainHeightQ12 + columnFractionOrWaterDelta;
          }
          sampleFailed = false;
        }
        sampleResult.failed = sampleFailed;
        sampleResult.heightQ12 = terrainHeightQ12;
        return sampleResult;
      }
    }
  }
  terrainHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.failed = sampleFailed;
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
HeightNormalSampleResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  FieldGridDimension rowLength;
  uint32_t cornerNormalAngles;
  int64_t weightedHeightAccumulator;
  int columnOrNormalSum;
  uint32_t interpolatedHeightQ12;
  uint32_t firstNormalEax;
  uint32_t columnFractionOrNormalAngles;
  uint32_t firstNormalEcx;
  uint32_t rowFractionOrNormalAngles;
  uint32_t firstNormalEdx;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  HeightNormalSampleResult sampleResult;
  FixedVectorAngles blendedNormalAngles;
  FixedDirection scaledNormal;
  
  rowFractionOrNormalAngles = (int)((uint64_t)((int64_t)worldY * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldY * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((uint64_t)((int64_t)worldX * 0x1c6e9c) >> 0x20) << 0xc |
          (uint32_t)((int64_t)worldX * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = field->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)field->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint32_t *)(field->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint32_t *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x18) * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)*(int *)(field->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                   (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)*(int *)(field->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint32_t *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = *(uint32_t *)(field->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)(field->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                          rowOffsetOrNormalSum + 0x18) * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)*(int *)(field->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x18) *
                   (int64_t)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&field[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x18) * (int64_t)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint32_t *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = *(uint32_t *)(field->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          columnOrNormalSum = firstNormalEax - scaledNormal.x;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)(field[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.failed = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.failed = sampleFailed;
  return sampleResult;
}


/* Address: 0x004FF3D0.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
HeightNormalSampleResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  uint32_t cornerNormalAngles;
  int64_t weightedHeightAccumulator;
  int columnOrNormalSum;
  uint32_t interpolatedHeightQ12;
  uint32_t firstNormalEax;
  uint32_t columnFractionOrNormalAngles;
  uint32_t firstNormalEcx;
  uint32_t rowFractionOrNormalAngles;
  uint32_t firstNormalEdx;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  HeightNormalSampleResult sampleResult;
  FixedVectorAngles blendedNormalAngles;
  FixedDirection scaledNormal;
  
  rowFractionOrNormalAngles = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x14) *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetOrNormalSum + 0x1c) * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (int64_t)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (int64_t)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          columnOrNormalSum = firstNormalEax - scaledNormal.x;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                           cellOffsetOrNormalSum + -0x20);
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.failed = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.failed = sampleFailed;
  return sampleResult;
}


/* Address: 0x004FF600.
   Ownership: world/terrain/grid.
   Purpose: Semantic ABI remains deferred.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
HeightNormalSampleResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  uint32_t cornerNormalAngles;
  int64_t weightedHeightAccumulator;
  int columnOrNormalSum;
  uint32_t interpolatedHeightQ12;
  uint32_t firstNormalEax;
  uint32_t columnFractionOrNormalAngles;
  uint32_t firstNormalEcx;
  uint32_t rowFractionOrNormalAngles;
  uint32_t firstNormalEdx;
  int rowOffsetBytes;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  HeightNormalSampleResult sampleResult;
  FixedVectorAngles blendedNormalAngles;
  FixedDirection scaledNormal;
  int normalSumEcx;
  
  rowFractionOrNormalAngles = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetBytes = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetBytes)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowOffsetBytes = rowOffsetBytes * rowLength * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + -0x10) & 0x88006000) == 0) &&
         ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetBytes + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetBytes + 0x1c) * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + -0x14)
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + -0x14) *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetBytes + 8);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetBytes + 0x18);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.x;
          normalSumEcx = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + 0x18);
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,normalSumEcx + scaledNormal.y,diagonalWeightOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetBytes + 0x1c) * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + -0x14)
                   * (int64_t)(int)(columnFractionOrNormalAngles - 0x1000) +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetBytes + 0x1c) * (int64_t)(int)(rowFractionOrNormalAngles - 0x1000));
          cornerNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                 sourceName + rowOffsetBytes + 8);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetBytes + 0x18);
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles - 0x1000);
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.x;
          normalSumEcx = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetBytes + 8);
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles - 0x1000);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum - scaledNormal.z,normalSumEcx - scaledNormal.y,diagonalWeightOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
          sampleFailed = false;
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.failed = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.failed = sampleFailed;
  return sampleResult;
}


/* Address: 0x004FF830.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid sample interpolated terrain height and normal angles carry-flag result register
   result.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed], FixedMath_VectorToAngles3Regs
   [core/math/fixed].
*/
HeightNormalSampleResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesRegs
          (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  uint32_t cornerNormalAngles;
  int64_t weightedHeightAccumulator;
  int columnOrNormalSum;
  uint32_t interpolatedHeightQ12;
  uint32_t firstNormalEax;
  uint32_t columnFractionOrNormalAngles;
  uint32_t firstNormalEcx;
  int columnWeightQ12;
  uint32_t rowFractionOrNormalAngles;
  uint32_t firstNormalEdx;
  int rowWeightQ12;
  int rowOffsetOrNormalSum;
  int cellOffsetOrNormalSum;
  int diagonalWeightOrNormalSum;
  bool sampleFailed;
  HeightNormalSampleResult sampleResult;
  FixedVectorAngles blendedNormalAngles;
  FixedDirection scaledNormal;
  
  rowFractionOrNormalAngles = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  columnFractionOrNormalAngles = ((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
          (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 0xc;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 0xc, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * 0x80;
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * 0x80;
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & 0xfff;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & 0xfff;
      if (((*(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x10) & 0x88006000) == 0) &&
         ((*(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].producerName +
                    rowOffsetOrNormalSum + 0x20) & 0x88006000) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - 0x1000;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < 0x1000) {
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + -0x14) *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                           rowOffsetOrNormalSum + 8);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          if ((int)interpolatedHeightQ12 < 0) {
            cornerNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             cellOffsetOrNormalSum + -0x20);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime0C_3F + rowOffsetOrNormalSum + -4);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 0x10,cornerNormalAngles & 0xffff,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            columnFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 0x10,columnFractionOrNormalAngles & 0xffff,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
            sampleFailed = false;
          }
        }
        else {
          columnWeightQ12 = columnFractionOrNormalAngles - 0x1000;
          rowWeightQ12 = rowFractionOrNormalAngles - 0x1000;
          weightedHeightAccumulator = (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                producerName + rowOffsetOrNormalSum + 0x1c) * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)*(int *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + -0x14)
                   * (int64_t)columnWeightQ12 +
                  (int64_t)
                  *(int *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].producerName +
                          rowOffsetOrNormalSum + 0x1c) * (int64_t)rowWeightQ12);
          rowFractionOrNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum + rowLength].
                                 sourceName + rowOffsetOrNormalSum + 8);
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 0xc | (int)((uint64_t)weightedHeightAccumulator >> 0x20) << 0x14;
          if ((int)interpolatedHeightQ12 < 0) {
            rowFractionOrNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             rowLength * 0x80 + cellOffsetOrNormalSum + -0x20);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            rowFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime0C_3F + rowOffsetOrNormalSum + -4);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,columnWeightQ12);
            columnOrNormalSum = firstNormalEax - scaledNormal.x;
            rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
            rowFractionOrNormalAngles = *(uint32_t *)(fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                             cellOffsetOrNormalSum + -0x20);
            cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            rowFractionOrNormalAngles = *(uint32_t *)(fieldGrid->cells[columnOrNormalSum + rowLength].runtime60_6B + rowOffsetOrNormalSum + 0x18);
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,columnWeightQ12);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            rowFractionOrNormalAngles = *(uint32_t *)((int)(&fieldGrid[1].common.buildMetadata.names)[columnOrNormalSum].sourceName +
                             rowOffsetOrNormalSum + 8);
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 0x10,rowFractionOrNormalAngles & 0xffff,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum - scaledNormal.z,diagonalWeightOrNormalSum - scaledNormal.y,cellOffsetOrNormalSum - scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 0x10 | blendedNormalAngles.azimuthAngle & 0xffff;
            sampleFailed = false;
          }
        }
        sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
        sampleResult.heightQ12 = interpolatedHeightQ12;
        sampleResult.failed = sampleFailed;
        return sampleResult;
      }
    }
  }
  interpolatedHeightQ12 = 0;
  sampleFailed = true;
  sampleResult.packedNormalAngles = rowFractionOrNormalAngles;
  sampleResult.heightQ12 = interpolatedHeightQ12;
  sampleResult.failed = sampleFailed;
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
FieldGrid_TestWorldPointBlocked
          (FieldGridByteOffset stateByteOffset,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          )

{
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  gridHalfRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
       (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
  gridColumnIndex =
       (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
              (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - gridHalfRowCoordinateQ12) + 0x800)
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
   Load-time cell setup: marks the field surface dirty and gives every cell a random animation phase (masked to
   the bit width stored just before the terrain surface packet table), a terrain direction record chosen by the
   low nibbles of its world X/Y, a white overlay colour and random material variant bits 8-10. Then it rebuilds
   the four map-edge flags on the outermost ring of cells, which neighbour loops test before stepping outside.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

{
  FieldGridDimension cellsPerRow;
  uint32_t phaseRandomValue;
  uint32_t materialVariantRandomBits;
  FieldGridDimension columnsRemaining;
  FieldGridDimension bottomRowCellsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridDimension edgeRowsRemaining;
  FieldGridDimension topRowCellsRemaining;
  FieldGridCell *cell;
  int rowBytesOrBottomCellAddress;
  FieldGridCell *nextRowFirstCell;
  FieldGridCell *currentRowFirstCell;
  Q12 cellWorldXQ12;
  Q12 cellWorldYQ12;
  uint32_t phaseSeedBitWidth;

  phaseSeedBitWidth = *(uint32_t *)((int)g_TerrainSurfacePacketTablePayload - 0x20);
  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  cellsPerRow = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  columnsRemaining = cellsPerRow;
  do {
    do {
      cellWorldXQ12 = cell->worldX;
      cellWorldYQ12 = cell->worldY;
      /* 0x77ff1fff: the edge flags and the unresolved bit are rebuilt from scratch */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial & ~(FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15);
      phaseRandomValue = Random_NextPrimary();
      cell->flagsAndMaterial = cell->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      cell->runtimeState00 = phaseRandomValue & (1 << ((uint8_t)phaseSeedBitWidth & 0x1f)) - 1U;
      /* 16x16 tiling of the 256 direction records over the world */
      cell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 + (cellWorldYQ12 & 0xfU) + (cellWorldXQ12 & 0xfU) * 16);
      cell->armyRuntimeSavedOffset6C = 0;
      materialVariantRandomBits = Random_NextPrimary();
      cell->runtimeOverlayOrHeightValue04 = 0xffffffff; /* ARGB opaque white */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial | materialVariantRandomBits & FIELD_CELL_RANDOM_VARIANT_MASK;
      cell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = cellsPerRow;
  } while (rowsRemaining != 0);
  bottomRowCellsRemaining = fieldGrid->gridWidth;
  edgeRowsRemaining = fieldGrid->gridHeight;
  currentRowFirstCell = fieldGrid->cells;
  topRowCellsRemaining = bottomRowCellsRemaining;
  nextRowFirstCell = currentRowFirstCell;
  do {
    nextRowFirstCell->flagsAndMaterial = nextRowFirstCell->flagsAndMaterial | FIELD_CELL_FIRST_ROW_BOUNDARY;
    nextRowFirstCell++;
    topRowCellsRemaining--;
  } while (topRowCellsRemaining != 0);
  do {
    currentRowFirstCell->flagsAndMaterial =
         currentRowFirstCell->flagsAndMaterial | FIELD_CELL_FIRST_COLUMN_BOUNDARY;
    nextRowFirstCell[-1].flagsAndMaterial =
         nextRowFirstCell[-1].flagsAndMaterial | FIELD_CELL_LAST_COLUMN_BOUNDARY;
    rowBytesOrBottomCellAddress = (int)nextRowFirstCell - (int)currentRowFirstCell;
    currentRowFirstCell = (FieldGridCell *)((int)currentRowFirstCell + rowBytesOrBottomCellAddress);
    nextRowFirstCell = (FieldGridCell *)(rowBytesOrBottomCellAddress + (int)currentRowFirstCell);
    edgeRowsRemaining--;
  } while (edgeRowsRemaining != 0);
  /* both cursors ran one row past the grid: step back to the first cell of the last row */
  rowBytesOrBottomCellAddress = (int)currentRowFirstCell * 2 - (int)nextRowFirstCell;
  do {
    ((FieldGridCell *)rowBytesOrBottomCellAddress)->flagsAndMaterial =
         ((FieldGridCell *)rowBytesOrBottomCellAddress)->flagsAndMaterial | FIELD_CELL_LAST_ROW_BOUNDARY;
    rowBytesOrBottomCellAddress = rowBytesOrBottomCellAddress + 0x80; /* sizeof(FieldGridCell) */
    bottomRowCellsRemaining--;
  } while (bottomRowCellsRemaining != 0);
}


/* Address: 0x00503DB0.
   Marks the field surface dirty and re-binds every cell's terrain direction record (+0x54) from the low nibbles
   of its world X/Y, the same 16x16 tiling FieldGrid_InitializeRuntimeCellsAndBoundaryFlags uses. The secondary
   terrain resource load calls this instead of the full initialization, so the other cell state is kept.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  columnsRemaining = gridWidth;
  do {
    do {
      currentCell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 +
           (currentCell->worldY & 0xfU) + (currentCell->worldX & 0xfU) * 16);
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}


/* Address: 0x00503E20.
   For every field cell, maps the runtime byte at +0x68 through the 256x256 terrain clamp lookup, keyed by
   the cell's occupancy byte of the given faction, and writes the result back. Runs on tick-wheel cases 3
   and 7 after the per-class terrain-state refresh callbacks.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyByteClampLookupToCells
          (FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid)

{
  uint8_t *clampLookup;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  uint8_t mappedRuntimeByte;
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
      mappedRuntimeByte =
           clampLookup[(uint32_t)currentCell->runtime60_6B[factionIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK]
                       << 8 |
                       (uint32_t)currentCell->runtime60_6B[FIELD_CELL_RUNTIME60_INDEX_RUNTIME_BYTE68]];
      currentCell->runtime60_6B[FIELD_CELL_RUNTIME60_INDEX_RUNTIME_BYTE68] = mappedRuntimeByte;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00503E80.
   Rebuilds every cell's runtime class byte (+0x68) from the occupancy byte of one faction slot (usually the
   active faction): 0xFF when a current presence bit (FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) is set,
   0x87 when only the persistent bit 7 is, 0 otherwise.
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClassifyCellFlagsToRuntimeByte
          (FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid)

{
  uint8_t classifiedRuntimeByte;
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    classifiedRuntimeByte = 0xff;
    if (((currentCell->runtime60_6B[factionSlot + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] &
          FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) == 0) &&
       (classifiedRuntimeByte = 0x87,
       (currentCell->runtime60_6B[factionSlot + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] &
        FIELD_CELL_OCCUPANCY_PERSISTENT_BIT) == 0)) {
      classifiedRuntimeByte = 0;
    }
    currentCell->runtime60_6B[FIELD_CELL_RUNTIME60_INDEX_RUNTIME_BYTE68] = classifiedRuntimeByte;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x00503EE0.
   Animates the 256 terrain direction records (random rates and scales from TerrainVisualResources_LoadPrimary):
   both 16-bit angles advance by their rates, and the scaled sine/cosine of angle A and one component of
   angle B are stored, computed from the angles before this step.
*/
void __thandor_void_preserve_eax_ecx_edx TerrainDirectionTable_AdvanceAndRebuildVectors(void)

{
  uint32_t previousPackedAngles;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCosEdxEax8 scaledSinCosPair;
  FixedSinCosEdxEax8 angleBScaledSinCosPair;

  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 256;
  do {
    previousPackedAngles = currentDirectionRecord->packedAngleA_low16_AngleB_high16;
    /* one 32-bit add advances both packed angles by rateA (low word) and rateB (high word); a carry out
       of angle A moves angle B by one more */
    currentDirectionRecord->packedAngleA_low16_AngleB_high16 =
         currentDirectionRecord->packedAngleA_low16_AngleB_high16 +
         *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(previousPackedAngles & 0xffff,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = (int)scaledSinCosPair;
    currentDirectionRecord->angleAComponent1ScaledQ28 = (int)(scaledSinCosPair >> 32);
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled((int)previousPackedAngles >> 16,currentDirectionRecord->scaleB);
    currentDirectionRecord->angleBComponent0ScaledQ28 = (uint32_t)angleBScaledSinCosPair;
    currentDirectionRecord++;
    recordsRemaining--;
  } while (recordsRemaining != 0);
}


/* Address: 0x00504B10.
   Casts a ray from a world point along (elevation, azimuth) scaled to rayScaleQ12 and walks the field grid cell by
   cell towards its end point, testing the two terrain triangles of each in-bounds cell. On a hit it returns the
   distance and the cell's material byte with CF set; a miss (or more than FIELD_GRID_RAYCAST_MAX_STEPS cells)
   returns FIELD_GRID_RAYCAST_MISS_DISTANCE with CF clear. Used for line-of-fire tests and terrain picking.
*/
TerrainRaycastResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainSurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  FieldGridDimension gridHeight;
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndRowProduct;
  uint32_t rayStartRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  uint32_t rayStartHalfRowQ12;
  uint32_t rayEndHalfRowOrCurrentColumnQ12;
  uint32_t currentRowQ12;
  int currentRowFromStartQ12;
  int stepsRemaining;
  bool traversalDone;
  TerrainRayTriangleResult triangleHit;
  TerrainRaycastResult missResult;
  TerrainRaycastResult hitResult;
  FixedDirection rayDirection;

  gridWidth = fieldGrid->gridWidth;
  gridHeight = fieldGrid->gridHeight;
  /* FieldGrid_WorldToGridQ12 inlined for the ray start */
  rayStartHalfRowQ12 = (int)((uint64_t)((int64_t)rayOriginYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)rayOriginYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  rayStartColumnQ12 =
       ((int)((uint64_t)((int64_t)rayOriginXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
       (uint32_t)((int64_t)rayOriginXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartColumnQ12 >> 12].runtime0C_3F +
                ((int)rayStartRowQ12 >> 12) * rowLength * 0x80 - 0xc);
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowOrCurrentColumnQ12 =
       (int)((uint64_t)rayEndRowProduct >> 32) << 11 | (uint32_t)rayEndRowProduct >> 21;
  rayEndColumnQ12 = ((int)((uint64_t)rayEndColumnProduct >> 32) << 12 | (uint32_t)rayEndColumnProduct >> 20) -
                    rayEndHalfRowOrCurrentColumnQ12;
  rayEndRowQ12 = rayEndHalfRowOrCurrentColumnQ12 * 2;
  stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS;
  /* from here on the variable holds the current cell's column */
  rayEndHalfRowOrCurrentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  do {
    stepsRemaining--;
    if (stepsRemaining == 0) break;
    /* the cell and its right/lower neighbours must lie inside the grid */
    if ((((-1 < (int)rayEndHalfRowOrCurrentColumnQ12) && (-1 < (int)currentRowQ12)) &&
        ((int)rayEndHalfRowOrCurrentColumnQ12 < (int)((gridWidth - 1) * FIELD_GRID_CELL_Q12))) &&
       ((int)currentRowQ12 < (int)((gridHeight - 1) * FIELD_GRID_CELL_Q12))) {
      currentRowFromStartQ12 = currentRowQ12 + rayStartHalfRowQ12 * -2;
      triangleHit = TerrainTriangle_IntersectRayDistance
                         (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,
                          rayEndColumnQ12 - rayStartColumnQ12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight,currentCell[rowLength].terrainHeight,
                          currentCell[1].terrainHeight,currentCell->terrainHeight,currentRowFromStartQ12,
                          rayEndHalfRowOrCurrentColumnQ12 - rayStartColumnQ12);
      if (!triangleHit.missed) {
        hitResult.hit = true;
        hitResult.distanceQ12 = triangleHit.distanceQ12;
        hitResult.materialOrCellIndex = currentCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
        return hitResult;
      }
      /* the original subtracted the start in the argument registers and adds it back here */
      rayEndHalfRowOrCurrentColumnQ12 =
           (rayEndHalfRowOrCurrentColumnQ12 - rayStartColumnQ12) + rayStartColumnQ12;
      currentRowQ12 = currentRowFromStartQ12 + rayStartRowQ12;
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowLength * 0x80,currentCell,currentRowQ12,rayEndHalfRowOrCurrentColumnQ12);
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    rayEndHalfRowOrCurrentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentRowQ12; /* a miss leaves the traversal's row in EDX */
  missResult.distanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  missResult.hit = false;
  return missResult;
}


/* Address: 0x00504CA0.
   Same walk as FieldGrid_RaycastTerrainSurfaceDistance, but against the secondary (water) surface: each triangle
   corner is terrainHeight + waterSurfaceDelta. Same result contract: distance and material byte with CF set on
   a hit, FIELD_GRID_RAYCAST_MISS_DISTANCE with CF clear on a miss.
*/
TerrainRaycastResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastSecondarySurfaceDistance
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;
  FieldGridDimension gridHeight;
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndRowProduct;
  uint32_t rayStartRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  uint32_t rayStartHalfRowQ12;
  uint32_t rayEndHalfRowOrCurrentColumnQ12;
  uint32_t currentRowQ12;
  int currentRowFromStartQ12;
  int stepsRemaining;
  bool traversalDone;
  TerrainRayTriangleResult triangleHit;
  TerrainRaycastResult missResult;
  TerrainRaycastResult hitResult;
  FixedDirection rayDirection;

  gridWidth = fieldGrid->gridWidth;
  gridHeight = fieldGrid->gridHeight;
  /* FieldGrid_WorldToGridQ12 inlined for the ray start */
  rayStartHalfRowQ12 = (int)((uint64_t)((int64_t)rayOriginYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)rayOriginYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  rayStartColumnQ12 =
       ((int)((uint64_t)((int64_t)rayOriginXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
       (uint32_t)((int64_t)rayOriginXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)
                (fieldGrid->cells[(int)rayStartColumnQ12 >> 12].runtime0C_3F +
                ((int)rayStartRowQ12 >> 12) * rowLength * 0x80 - 0xc);
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowOrCurrentColumnQ12 =
       (int)((uint64_t)rayEndRowProduct >> 32) << 11 | (uint32_t)rayEndRowProduct >> 21;
  rayEndColumnQ12 = ((int)((uint64_t)rayEndColumnProduct >> 32) << 12 | (uint32_t)rayEndColumnProduct >> 20) -
                    rayEndHalfRowOrCurrentColumnQ12;
  rayEndRowQ12 = rayEndHalfRowOrCurrentColumnQ12 * 2;
  stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS;
  /* from here on the variable holds the current cell's column */
  rayEndHalfRowOrCurrentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1);
  do {
    stepsRemaining--;
    if (stepsRemaining == 0) break;
    /* the cell and its right/lower neighbours must lie inside the grid */
    if ((((-1 < (int)rayEndHalfRowOrCurrentColumnQ12) && (-1 < (int)currentRowQ12)) &&
        ((int)rayEndHalfRowOrCurrentColumnQ12 < (int)((gridWidth - 1) * FIELD_GRID_CELL_Q12))) &&
       ((int)currentRowQ12 < (int)((gridHeight - 1) * FIELD_GRID_CELL_Q12))) {
      currentRowFromStartQ12 = currentRowQ12 + rayStartHalfRowQ12 * -2;
      triangleHit = TerrainTriangle_IntersectRayDistance
                         (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,
                          rayEndColumnQ12 - rayStartColumnQ12,rayOriginZQ12,
                          currentCell[rowLength + 1].terrainHeight +
                          currentCell[rowLength + 1].waterSurfaceDelta,
                          currentCell[rowLength].terrainHeight + currentCell[rowLength].waterSurfaceDelta,
                          currentCell[1].terrainHeight + currentCell[1].waterSurfaceDelta,
                          currentCell->waterSurfaceDelta + currentCell->terrainHeight,
                          currentRowFromStartQ12,rayEndHalfRowOrCurrentColumnQ12 - rayStartColumnQ12);
      if (!triangleHit.missed) {
        hitResult.hit = true;
        hitResult.distanceQ12 = triangleHit.distanceQ12;
        hitResult.materialOrCellIndex = currentCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK;
        return hitResult;
      }
      /* the original subtracted the start in the argument registers and adds it back here */
      rayEndHalfRowOrCurrentColumnQ12 =
           (rayEndHalfRowOrCurrentColumnQ12 - rayStartColumnQ12) + rayStartColumnQ12;
      currentRowQ12 = currentRowFromStartQ12 + rayStartRowQ12;
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowLength * 0x80,currentCell,currentRowQ12,rayEndHalfRowOrCurrentColumnQ12);
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    rayEndHalfRowOrCurrentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentRowQ12; /* a miss leaves the traversal's row in EDX */
  missResult.distanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  missResult.hit = false;
  return missResult;
}


/* Address: 0x00504E60.
   Ownership: world/terrain/grid.
   Purpose: Handles field grid raycast terrain triangles along direction.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   TerrainTriangle_IntersectRayDistance [world/terrain/height], TerrainRay_AdvanceGridTraversal
   [world/terrain/height].
*/
TerrainRaycastResult __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int64_t rayEndYProduct;
  int64_t rayEndXProduct;
  uint32_t rayStartCoord0Q12;
  int rayEndCoord0Q12;
  uint32_t rayStartCoord1Q12;
  int rayEndCoord1Q12;
  int columnOrClampOffset;
  int maxRowIndex;
  uint32_t rayStartHalfCoord0Q12;
  uint32_t endHalfCoordOrCurrentCoord1Q12;
  uint32_t currentGridCoord0Q12;
  int cellLocalCoord1Q12;
  int cellRowIndex;
  int maxColumnIndex;
  int stepsRemaining;
  FieldGridCell *currentCell;
  FieldGridCell *sampleCell;
  int rowStrideBytes;
  bool traversalDone;
  TerrainRayTriangleResult triangleHit;
  TerrainRaycastResult hitResult;
  TerrainRaycastResult missResult;
  FixedDirection rayDirection;
  FieldCellPersistedAux cornerHeight0Q12;
  FieldCellPersistedAux cornerHeight1Q12;
  FieldCellPersistedAux cornerHeight2Q12;
  FieldCellPersistedAux cornerHeight3Q12;
  
  maxColumnIndex = fieldGrid->gridWidth - 1;
  maxRowIndex = fieldGrid->gridHeight - 1;
  rayStartHalfCoord0Q12 = (int)((uint64_t)((int64_t)rayOriginXQ12 * -0x20c8cc) >> 0x20) << 0xb |
          (uint32_t)((int64_t)rayOriginXQ12 * -0x20c8cc) >> 0x15;
  rayStartCoord1Q12 =
       ((int)((uint64_t)((int64_t)rayOriginYQ12 * 0x1c6e9c) >> 0x20) << 0xc |
       (uint32_t)((int64_t)rayOriginYQ12 * 0x1c6e9c) >> 0x14) - rayStartHalfCoord0Q12;
  rayStartCoord0Q12 = rayStartHalfCoord0Q12 * 2;
  rowLength = fieldGrid->gridWidth;
  rowStrideBytes = rowLength * 0x80;
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndYProduct = (int64_t)(int)(rayDirection.x + rayOriginYQ12) * 0x1c6e9c;
  rayEndXProduct = (int64_t)(int)(rayDirection.y + rayOriginXQ12) * -0x20c8cc;
  endHalfCoordOrCurrentCoord1Q12 = (int)((uint64_t)rayEndXProduct >> 0x20) << 0xb | (uint32_t)rayEndXProduct >> 0x15;
  rayEndCoord1Q12 = ((int)((uint64_t)rayEndYProduct >> 0x20) << 0xc | (uint32_t)rayEndYProduct >> 0x14) - endHalfCoordOrCurrentCoord1Q12;
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
    triangleHit = TerrainTriangle_IntersectRayDistance
                       (rayDirection.z,rayEndCoord0Q12 + rayStartHalfCoord0Q12 * -2,rayEndCoord1Q12 - rayStartCoord1Q12,
                        rayOriginZQ12,cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                        cornerHeight3Q12,cellLocalCoord1Q12,endHalfCoordOrCurrentCoord1Q12 - rayStartCoord1Q12);
    if (!triangleHit.missed) {
      hitResult.hit = true;
      hitResult.distanceQ12 = triangleHit.distanceQ12;
      hitResult.materialOrCellIndex = sampleCell->flagsAndMaterial & 0xff; /* low byte: material */
      return hitResult;
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndCoord0Q12,rayEndCoord1Q12,rayStartCoord0Q12,rayStartCoord1Q12,
                        rowStrideBytes,currentCell,currentGridCoord0Q12,endHalfCoordOrCurrentCoord1Q12);    
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    endHalfCoordOrCurrentCoord1Q12 = g_TerrainRayNextCoord1Q12;
    currentGridCoord0Q12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentGridCoord0Q12;
  missResult.distanceQ12 = 0x7fffffff;
  missResult.hit = false;
  return missResult;
}


/* Address: 0x00505120.
   Clears the rebuilt bits 0..6 of every faction byte of every cell's occupancyMask, keeping bit 7. Head of
   tick-wheel case 7, before the per-class occupancy-rebuild callbacks repopulate the mask. The MMX original
   handles eight cells per step and then exactly three more per row, i.e. it assumes a row width of 8n + 3.
*/
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid)

{
  FieldGridOccupancyBlockCount eightCellBlocksPerRow;
  FieldGridOccupancyBlockCount cellBlocksRemaining;
  FieldGridOccupancyBlockCount blocksRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *blockBaseCell;
  uint64_t occupancyHighBitMask;
  FieldGridCell *currentEightCellBlock;

  /* FIELD_CELL_OCCUPANCY_PERSISTENT_BIT in all eight bytes (0x8080808080808080) */
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
      /* kept as a separate temporary: plain -- compiles to SUB instead of the original ADD -1 */
      blocksRemaining = cellBlocksRemaining - 1;
      cellBlocksRemaining = blocksRemaining;
      currentEightCellBlock = blockBaseCell + 8;
    } while (blocksRemaining != 0);
    /* the three trailing cells of the row, right after the last block */
    blockBaseCell[8].occupancyMask = blockBaseCell[8].occupancyMask & occupancyHighBitMask;
    blockBaseCell[9].occupancyMask = blockBaseCell[9].occupancyMask & occupancyHighBitMask;
    blockBaseCell[10].occupancyMask = blockBaseCell[10].occupancyMask & occupancyHighBitMask;
    rowsRemaining--;
    cellBlocksRemaining = eightCellBlocksPerRow;
    currentEightCellBlock = blockBaseCell + 11;
  } while (rowsRemaining != 0);
  return;
}

/* Address: 0x00505240.
   Sets FIELD_CELL_OCCUPANCY_BIT0 in one faction's occupancy byte of every cell. Tick-wheel case 7 calls it
   for the active faction when bit 3 of g_UiCommandRuntimeFlags is set, right after the rebuild clear, so
   every cell carries bit 0 for that faction during the rebuild.
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
      currentCell->runtime60_6B[occupancyMaskByteIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] =
           currentCell->runtime60_6B[occupancyMaskByteIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] |
           FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505290.
   Counterpart of FieldGrid_SetOccupancyMaskByteBit0AllCells: clears FIELD_CELL_OCCUPANCY_BIT0 in one
   faction's occupancy byte of every cell.
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
      currentCell->runtime60_6B[occupancyMaskByteIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] =
           currentCell->runtime60_6B[occupancyMaskByteIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] &
           (uint8_t)~FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
}


/* Address: 0x00507580.
   Rounds a world point to the nearest field-grid cell and tests occupancy bits 0/1 of the active faction there.
   Returns false (CF clear) when one of them is set, true when the point is outside the grid or neither bit is
   set. Unit, shot and effect code play positioned sounds only when this returns false.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainGrid_TestProjectedCellMaskBits01
          (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;

  activeFieldGrid = worldRuntime->fieldGrid;
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 = (int)((uint64_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
          (uint32_t)((int64_t)worldYQ12 * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  gridColumnIndex = (int)((((int)((uint64_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
                 (uint32_t)((int64_t)worldXQ12 * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - gridHalfRowCoordinateQ12) +
                 0x800) >> 12;
  if ((((-1 < gridColumnIndex) && (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + 0x800) >> 12, -1 < gridRowIndex)) &&
      (gridColumnIndex < (int)activeFieldGrid->gridWidth)) &&
     ((gridRowIndex < (int)activeFieldGrid->gridHeight &&
      ((activeFieldGrid->cells[activeFieldGrid->gridWidth * gridRowIndex + gridColumnIndex].runtime60_6B
        [worldRuntime->activeFactionRuntimeIndex + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK] &
        FIELD_CELL_OCCUPANCY_BITS01) != 0)))) {
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
   Sets the overlay colour (ARGB, cell +0x04) of every field-grid cell to one value.
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
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x00532B60.
   Ownership: world/terrain/grid.
   Purpose: Builds a temporary serialized FieldGrid/runtime image, clears or initializes derived per-cell state,
   writes it through FileSystem_WriteBufferToPath, frees the temporary allocation and returns its 32-bit status
   with carry semantics outside the C prototype.
   Cross-module calls: FileSystem_WriteBufferToPath [platform/filesystem/win32].
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords)

{
  FieldGridAsset *writeErrorValue;
  FieldGridAsset *fieldGridImageCopy;
  uint32_t imageSizeOrDwordsLeft;
  int cellsRemaining;
  int occupancyBytesLeft;
  uint32_t *copyDestinationDwords;
  FieldGridCellSaveImageView80 *fieldGridCellSaveView;
  uint8_t *occupancyBytes;
  ArenaAllocResult allocResult;
  StatusResult writeStatus;
  ArenaFreeResult freeResult;
  
  imageSizeOrDwordsLeft = sourceImageDwords[1];
  allocResult = g_MemoryApi.alloc(imageSizeOrDwordsLeft);
  fieldGridImageCopy = (FieldGridAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    copyDestinationDwords = (uint32_t *)fieldGridImageCopy;
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
           1 << ((uint8_t)fieldGridCellSaveView->flagsAndMaterial & 0x1f);
      occupancyBytes = (uint8_t *)&fieldGridCellSaveView->occupancyMask;
      for (occupancyBytesLeft = 8; occupancyBytesLeft != 0; occupancyBytesLeft = occupancyBytesLeft + -1) {
        *occupancyBytes = 0;
        occupancyBytes = occupancyBytes + 1;
      }
      fieldGridCellSaveView = fieldGridCellSaveView + 1;
      cellsRemaining = cellsRemaining + -1;
    } while (cellsRemaining != 0);
    writeStatus = FileSystem_WriteBufferToPath
                      ((fieldGridImageCopy->common).allocationSizeBytes,fieldGridImageCopy,
                       (uint16_t *)&g_LevelResourcePathScratchUtf16);
    if (!writeStatus.failed) {
      freeResult = g_MemoryApi.free(fieldGridImageCopy);
      return THANDOR_BITCAST(uint64_t, StatusResult, ((THANDOR_BITCAST(ArenaFreeResult, uint64_t, freeResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
    writeErrorValue = (FieldGridAsset *)writeStatus.valueOrError;
    g_MemoryApi.free(fieldGridImageCopy);
    fieldGridImageCopy = writeErrorValue;
  }
  writeStatus.failed = true;
  writeStatus.valueOrError = (uint32_t)fieldGridImageCopy;
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
  int64_t scaledDeltaProduct;
  uint64_t distanceSquared;
  int deltaXOrRadiusSquaredHigh;
  int deltaY;
  uint32_t radiusSquaredOrHeightDelta;
  uint32_t distanceSquaredHigh;
  
  deltaXOrRadiusSquaredHigh = centerWorldXQ12 - cell->worldX;
  deltaY = cell->worldY - centerWorldYQ12;
  distanceSquared = (int64_t)deltaY * (int64_t)deltaY + (int64_t)deltaXOrRadiusSquaredHigh * (int64_t)deltaXOrRadiusSquaredHigh;
  distanceSquaredHigh = (uint32_t)(distanceSquared >> 0x20);
  deltaXOrRadiusSquaredHigh = (int)((uint64_t)((int64_t)radiusWorldUnits * (int64_t)radiusWorldUnits) >> 0x20);
  radiusSquaredOrHeightDelta = (uint32_t)((int64_t)radiusWorldUnits * (int64_t)radiusWorldUnits);
  if (((int)distanceSquaredHigh <= deltaXOrRadiusSquaredHigh) &&
     ((((int)distanceSquaredHigh < deltaXOrRadiusSquaredHigh || ((int)distanceSquared < (int)radiusSquaredOrHeightDelta)) &&
      (radiusSquaredOrHeightDelta = deltaXOrRadiusSquaredHigh << 0x14 | radiusSquaredOrHeightDelta >> 0xc, radiusSquaredOrHeightDelta != 0)))) {
    scaledDeltaProduct = (int64_t)
            ((int)((int64_t)((uint64_t)distanceSquaredHigh << 0x20 | distanceSquared & 0xffffffff) / (int64_t)(int)radiusSquaredOrHeightDelta)
            + -0x1000) * (int64_t)terrainHeightDeltaAmplitudeQ12;
    radiusSquaredOrHeightDelta = (int)((uint64_t)scaledDeltaProduct >> 0x20) << 0x14 | (uint32_t)scaledDeltaProduct >> 0xc;
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
   Water flow pass A (tick-wheel case 1): scans the interior cells row by row and pulls the water surface
   (terrainHeight + waterSurfaceDelta) of the six hexagonal neighbours 1/8 of the way toward the source
   cell's surface. Sources with negative water or FIELD_CELL_FLUID_SOURCE_EXCLUDED are skipped, receivers
   with FIELD_CELL_FLUID_RECEIVER_EXCLUDED are left alone.
   The source is the centre cell itself ([ESI+0x4C] with ESI = centre); verified against the original
   machine code by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

{
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  FieldGridCell *rowStartCell;
  FieldGridCell *centerCell;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCell = fieldGrid->cells + gridWidth;
  do {
    columnsRemaining = gridWidth - 2;
    centerCell = rowStartCell;
    do {
      centerCell++;
      if ((-1 < centerCell->waterSurfaceDelta) &&
         ((centerCell->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 = centerCell->waterSurfaceDelta + centerCell->terrainHeight;
        if ((centerCell[-gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          centerCell[-gridWidth].waterSurfaceDelta =
               centerCell[-gridWidth].waterSurfaceDelta -
               ((centerCell[-gridWidth].waterSurfaceDelta +
                centerCell[-gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[1 - gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          centerCell[1 - gridWidth].waterSurfaceDelta =
               centerCell[1 - gridWidth].waterSurfaceDelta -
               ((centerCell[1 - gridWidth].waterSurfaceDelta +
                centerCell[1 - gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[gridWidth].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0
           ) {
          centerCell[gridWidth].waterSurfaceDelta =
               centerCell[gridWidth].waterSurfaceDelta -
               ((centerCell[gridWidth].waterSurfaceDelta +
                centerCell[gridWidth].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[gridWidth - 1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          centerCell[gridWidth - 1].waterSurfaceDelta =
               centerCell[gridWidth - 1].waterSurfaceDelta -
               ((centerCell[gridWidth - 1].waterSurfaceDelta +
                centerCell[gridWidth - 1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          centerCell[-1].waterSurfaceDelta =
               centerCell[-1].waterSurfaceDelta -
               ((centerCell[-1].waterSurfaceDelta + centerCell[-1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          centerCell[1].waterSurfaceDelta =
               centerCell[1].waterSurfaceDelta -
               ((centerCell[1].waterSurfaceDelta + centerCell[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowStartCell = centerCell + 2;
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00505BE0.
   Water flow pass B (tick-wheel case 5): the same neighbour relaxation as pass A, scanning the interior
   cells backwards from the bottom-right, so water spreads evenly in both directions over two ticks.
   Cells are addressed by raw byte offsets (cell size 0x80; +0x48 terrainHeight, +0x4C waterSurfaceDelta,
   +0x50 flagsAndMaterial; +/-0x80 is the next/previous cell).
   The source is the centre cell itself (ESI); verified by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int rowStartCellAddress;
  int centerCellAddress;
  int upperCellAddress;
  int neighborWaterDeltaAddress;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCellAddress =
       (int)fieldGrid + rowLength * -0x80 + (rowLength * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = rowLength - 2;
    centerCellAddress = rowStartCellAddress;
    do {
      centerCellAddress = centerCellAddress - 0x80;
      if ((-1 < *(int *)(centerCellAddress + 0x4c)) &&
         ((*(uint32_t *)(centerCellAddress + 0x50) & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 =
             *(int *)(centerCellAddress + 0x4c) + *(int *)(centerCellAddress + 0x48);
        upperCellAddress = centerCellAddress + rowLength * -0x80;
        if ((*(uint32_t *)(upperCellAddress + 0x50) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          neighborWaterDeltaAddress = upperCellAddress + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(upperCellAddress + 0x4c) + *(int *)(upperCellAddress + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperCellAddress + 0xd0) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          *(int *)(upperCellAddress + 0xcc) =
               *(int *)(upperCellAddress + 0xcc) -
               ((*(int *)(upperCellAddress + 0xcc) + *(int *)(upperCellAddress + 0xc8)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperCellAddress + 0x50 + rowLength * 0x100) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint32_t *)(upperCellAddress + -0x30 + rowLength * 0x100) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperCellAddress + -0x34 + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperCellAddress + -0x34 + rowLength * 0x100) +
                               *(int *)(upperCellAddress + -0x38 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        centerCellAddress = upperCellAddress + rowLength * 0x80;
        if ((*(uint32_t *)(centerCellAddress + -0x30) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          *(int *)(centerCellAddress + -0x34) =
               *(int *)(centerCellAddress + -0x34) -
               ((*(int *)(centerCellAddress + -0x34) + *(int *)(centerCellAddress + -0x38)
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(centerCellAddress + 0xd0) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          *(int *)(centerCellAddress + 0xcc) =
               *(int *)(centerCellAddress + 0xcc) -
               ((*(int *)(centerCellAddress + 0xcc) + *(int *)(centerCellAddress + 0xc8)) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowStartCellAddress = centerCellAddress + -0x100;
    rowsRemaining--;
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
      if ((cellBeforeSource->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0) {
        sourceSurfaceHeightQ12 =
             cellBeforeSource->waterSurfaceDelta + cellBeforeSource->terrainHeight;
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
      if ((*(uint32_t *)(cellAfterSourceAddress + 0x50) & 0x40000000) == 0) {
        sourceSurfaceHeightQ12 =
             *(int *)(cellAfterSourceAddress + 0x4c) + *(int *)(cellAfterSourceAddress + 0x48);
        upperRowCellAddress = cellAfterSourceAddress + rowLength * -0x80;
        if ((*(uint32_t *)(upperRowCellAddress + 0x50) & 0x20000000) == 0) {
          neighborWaterDeltaAddress = upperRowCellAddress + 0x4c;
          *(int *)neighborWaterDeltaAddress =
               *(int *)neighborWaterDeltaAddress -
               ((*(int *)(upperRowCellAddress + 0x4c) + *(int *)(upperRowCellAddress + 0x48)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperRowCellAddress + 0xd0) & 0x20000000) == 0) {
          *(int *)(upperRowCellAddress + 0xcc) =
               *(int *)(upperRowCellAddress + 0xcc) -
               ((*(int *)(upperRowCellAddress + 0xcc) + *(int *)(upperRowCellAddress + 200)) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperRowCellAddress + 0x50 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((*(uint32_t *)(upperRowCellAddress + -0x30 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + -0x34 + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + -0x34 + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + -0x38 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * 0x80;
        if ((*(uint32_t *)(cellAfterSourceAddress + -0x30) & 0x20000000) == 0) {
          *(int *)(cellAfterSourceAddress + -0x34) =
               *(int *)(cellAfterSourceAddress + -0x34) -
               ((*(int *)(cellAfterSourceAddress + -0x34) + *(int *)(cellAfterSourceAddress + -0x38)
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(cellAfterSourceAddress + 0xd0) & 0x20000000) == 0) {
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
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int maxColumnOrCenterX;
  int spanColumnCount;
  uint32_t cellDistance;
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
          falloffProduct = (int64_t)
                  (g_FixedCosQ28
                   [(int)((int64_t)
                          ((((int64_t)(int)cellDistance & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (int64_t)(int)cellDistance * 0x8000 & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                   ] + 0x10000000) * (int64_t)heightDifference;
          blendedHeight = ((int)((uint64_t)falloffProduct >> 0x20) << 3 | (uint32_t)falloffProduct >> 0x1d) + *accumulatorCursor;
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
  int64_t falloffProduct;
  uint32_t spanRadiusQ12;
  int maxColumnOrCenterX;
  int spanColumnCount;
  uint32_t cellDistance;
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
          falloffProduct = (int64_t)
                  (g_FixedCosQ28
                   [(int)((int64_t)
                          ((((int64_t)(int)cellDistance & 0x1ffffffffffffU) >> 0x11) << 0x20 |
                          (int64_t)(int)cellDistance * 0x8000 & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                   ] + 0x10000000) * (int64_t)targetOffset;
          maxRowOrBlendedValue = ((int)((uint64_t)falloffProduct >> 0x20) << 3 | (uint32_t)falloffProduct >> 0x1d) + *accumulatorCursor;
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
   Converts a world-plane position to field-grid coordinates in Q12 (integer part = cell column/row,
   fraction = position inside the cell): column in EAX, row in EDX. The triangular lattice makes the
   column shift by half a cell per row.
*/
FieldGridCoordinatesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx_mm0
FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX)

{
  uint32_t gridHalfRowCoordinateQ12;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;

  /* Q12 * Q20 >> 21: half the row coordinate */
  gridHalfRowCoordinateQ12 =
       (int)((uint64_t)((int64_t)worldY * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 32) << 11 |
       (uint32_t)((int64_t)worldY * FIELD_GRID_WORLD_Y_TO_ROW_Q20) >> 21;
  gridCoordinates.rowQ12 = gridHalfRowCoordinateQ12 * 2;
  gridCoordinates.columnQ12 =
       ((int)((uint64_t)((int64_t)worldX * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 32) << 12 |
       (uint32_t)((int64_t)worldX * FIELD_GRID_WORLD_X_TO_COLUMN_Q20) >> 20) - gridHalfRowCoordinateQ12;
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
   Recomputes a cell's two vertex normals from its six lattice neighbours and stores them as packed
   (azimuth | elevation << 16) angle pairs: triangle0NormalAngles (+0x08) for the terrain surface and
   triangle1NormalAngles (+0x78) for the secondary surface (terrainHeight + waterSurfaceDelta). The lighting in
   FieldGridCell_ComputeDirectionalLightColor reads the first one.
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
  FixedVectorAngles normalAngles;

  /* The six neighbours of the triangular lattice, addressed as raw byte offsets from the cell (0x80 bytes per
     cell, rowStrideBytes per row; worldX +0x40, worldY +0x44, terrainHeight +0x48, waterSurfaceDelta +0x4C):
       cell[1] right, cell[-1] left, +rowStrideBytes below, -rowStrideBytes above,
       +rowStrideBytes - 0x80 below-left, -rowStrideBytes + 0x80 above-right.
     Each normal is (-sum(dX * dH), -sum(dY * dH), 0xC00000) over the neighbours. First pass (terrain):
     rightDelta.. = right, A = below, B = above, C = left, D = below-left, E = above-right. Second pass (surface):
     rightDelta.. = -rowStrideBytes, A = right, B = below, C = above, D = left, E = below-left, F = above-right. */
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
  cell->triangle0NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
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
  cell->triangle1NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
}


/* Address: 0x00505690.
   Diffuse terrain lighting for one cell: turns the terrain normal (triangle0NormalAngles) back into a Q28
   direction, dots it with the global light direction and looks the result up in the directional light colour
   table (+0x58). The secondary surface always gets the one fixed secondary colour (+0x5C).
*/
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  FixedDirection normalDirection;
  PackedArgb32 directionalLightColor;

  /* packed as azimuth (low word) | elevation (high word) */
  normalDirection = FixedMath_DirectionFromAnglesQ28Regs
                    ((int)cell->triangle0NormalAngles >> 16,cell->triangle0NormalAngles & 0xffff);
  /* the signed Q8 dot product indexes -256..256: g_TerrainLightingColorRampArgb256 lies directly
     before this table and holds the shaded half */
  directionalLightColor =
       ((PackedArgb32 *)g_TerrainDirectionalLightColorLut)
       [(int)((uint64_t)((int64_t)(int)normalDirection.x * (int64_t)(int)g_TerrainLightDirectionX) >> 0x20) +
        (int)((uint64_t)((int64_t)(int)normalDirection.y * (int64_t)(int)g_TerrainLightDirectionY) >> 0x20) +
        (int)((uint64_t)((int64_t)(int)normalDirection.z * (int64_t)(int)g_TerrainLightDirectionZ) >> 0x20) >>
        16];
  cell->secondarySurfaceDirectionalLightColor5C = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor58 = directionalLightColor;
}

