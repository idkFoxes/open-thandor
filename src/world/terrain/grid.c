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
   Deforms the terrain around a world point (crater/mound of an effect): clips the cell rectangle around the
   circle to the grid interior, applies FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial to every cell in
   it, then recomputes the triangle normals and the directional light of the same rectangle. Called by effect
   maintenance slot 2 (EffectModelRuntimeMaintenance, 0x0051E850) when a finished effect invokes its linked
   handler. (The original also returns CF: clear after the edit, set for a non-positive radius or an empty
   rectangle; the only caller ignores it.)
*/
void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface
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
  FieldGridCoordinates minCornerGrid;
  FieldGridCoordinates maxCornerGrid;
  FieldGridCell *heightOrLightRowStart;
  FieldGridCell *normalRowStart;
  
  if (0 < radiusWorldUnits) {
    /* radius * sqrt(3): the X half-extent of the box that the corners are taken from */
    horizontalRadiusQ12 = FIXED_MUL_SHR(radiusWorldUnits, FIELD_GRID_SQRT3_Q12, Q12_SHIFT);
    boundOrRowsLeft = centerWorldXQ12 - horizontalRadiusQ12;
    minCornerGrid = FieldGrid_WorldToGridQ12(centerWorldYQ12 + radiusWorldUnits,boundOrRowsLeft);
    maxCornerGrid = FieldGrid_WorldToGridQ12
                      (centerWorldYQ12 + radiusWorldUnits + radiusWorldUnits * -2,boundOrRowsLeft + horizontalRadiusQ12 * 2)
    ;
    rowLength = fieldGrid->gridWidth;
    minColumnOrRowsLeft = minCornerGrid.columnQ12 >> Q12_SHIFT;
    minRowOrColumnsLeft = minCornerGrid.rowQ12 >> Q12_SHIFT;
    boundOrRowsLeft = (maxCornerGrid.columnQ12 >> Q12_SHIFT) + 1;
    maxRowOrRowsLeft = (maxCornerGrid.rowQ12 >> Q12_SHIFT) + 1;
    /* clip to the interior: the one-cell border ring is never edited */
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
      fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
      /* &cells[minRow * rowLength + minColumn] */
      cell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[minColumnOrRowsLeft] + minRowOrColumnsLeft * rowLength * sizeof(FieldGridCell));
      minRowOrColumnsLeft = columnCountOrColumnsLeft;
      heightOrLightRowStart = cell;
      normalCell = cell;
      maxRowOrRowsLeft = boundOrRowsLeft;
      do {
        do {
          FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
                    (terrainMaterialIndexOrNegativeSentinel,radiusWorldUnits,
                     terrainHeightDeltaAmplitudeQ12,centerWorldYQ12,centerWorldXQ12,cell);
          cell++;
          minRowOrColumnsLeft--;
        } while (minRowOrColumnsLeft != 0);
        cell = heightOrLightRowStart + rowLength;
        boundOrRowsLeft--;
        minRowOrColumnsLeft = columnCountOrColumnsLeft;
        normalRowStart = normalCell;
        heightOrLightRowStart = cell;
        lightCell = normalCell;
        minColumnOrRowsLeft = maxRowOrRowsLeft;
      } while (boundOrRowsLeft != 0);
      do {
        do {
          FieldGridCell_RecomputeTriangleNormalAngles(rowLength * sizeof(FieldGridCell),normalCell); /* row stride in bytes */
          normalCell++;
          columnCountOrColumnsLeft--;
        } while (columnCountOrColumnsLeft != 0);
        normalCell = normalRowStart + rowLength;
        maxRowOrRowsLeft--;
        columnCountOrColumnsLeft = minRowOrColumnsLeft;
        heightOrLightRowStart = lightCell;
        normalRowStart = normalCell;
        boundOrRowsLeft = minRowOrColumnsLeft;
      } while (maxRowOrRowsLeft != 0);
      do {
        do {
          FieldGridCell_ComputeDirectionalLightColor(lightCell);
          lightCell++;
          minRowOrColumnsLeft--;
        } while (minRowOrColumnsLeft != 0);
        lightCell = heightOrLightRowStart + rowLength;
        minColumnOrRowsLeft--;
        minRowOrColumnsLeft = boundOrRowsLeft;
        heightOrLightRowStart = lightCell;
      } while (minColumnOrRowsLeft != 0);
      return;
    }
  }
}


/* Address: 0x00562330.
   In-game command INGAME_COMMAND_TERRAIN_RELAXATION (0x3200, handler at INGAME_COMMAND_CODE_BASE + code):
   runs passCount pairs of forward and reverse water relaxation sweeps over the active field grid, the
   sign-gated pair or (mode bit 0 set) the ungated land-tool pair. Queued or called directly by
   InGameCommandRange_DispatchState0/1 (ui/ingame/commands.c) with 0x80 passes. passCount must not be 0.
*/
void TerrainGrid_RunDirectionalRelaxationPasses(FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode)

{
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  if ((mode & TERRAIN_RELAXATION_UNGATED_LAND_TOOL) == TERRAIN_RELAXATION_SIGN_GATED) {
    do {
      TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(fieldGrid);
      passCount--;
    } while (passCount != 0);
  }
  else {
    do {
      TerrainGrid_RelaxNeighborHeightsForward(fieldGrid);
      TerrainGrid_RelaxNeighborHeightsReverse(fieldGrid);
      passCount--;
    } while (passCount != 0);
  }
}


/* Address: 0x005610A0.
   In-game command INGAME_COMMAND_EDITOR_RAISE_HEIGHTS (0x1F70, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 0): pulls the terrain towards the anchor cell's height moved by the vertical
   drag distance, with a cosine falloff over the horizontal drag distance. The player's scratch plane holds the
   height change of the previous drag step: it is undone first, the plane is refilled with the current heights
   and FieldGrid_ProcessHorizontalSpan moves them towards the target around the anchor (or around every selected
   pair); the difference is applied again and kept in the plane. Water surfaces stay at their level
   (waterSurfaceDelta moves opposite to the terrain). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode (0x005703D0).
*/
void FieldGrid_ApplyPositiveCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
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
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  applyCell = fieldGrid->cells;
  countdownOrDelta = remainingCellCount;
  scanCell = applyCell;
  accumulatorPlane = scratchHeightCursor;
  applyRowStrideBytes = rowStrideBytes;
  /* undo the previous step; every changed interior cell refreshes the normals and light of itself and its six
     neighbours (the left and right one only while their scratch entry is 0) */
  do {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      scanCell->terrainHeight = scanCell->terrainHeight - cellDelta;
      scanCell->waterSurfaceDelta = scanCell->waterSurfaceDelta + cellDelta;
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
        if (((scanCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell - 1);
        }
        if (((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell - rowLength; /* row above */
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + rowLength * 2 - 1; /* row below, one to the left */
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        cell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        scanCell = cell - rowLength; /* back to the changed cell */
      }
    }
    *scratchHeightCursor = scanCell->terrainHeight;
    scanCell++;
    scratchHeightCursor++;
    countdownOrDelta--;
  } while (countdownOrDelta != 0);
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessHorizontalSpan
              (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
               (int)(short)packedDragDeltaXY16,anchorRowQ12,anchorColumnQ12,accumulatorPlane,
               fieldGrid);
    scratchHeightCursor = accumulatorPlane;
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
    scratchHeightCursor = accumulatorPlane;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessHorizontalSpan
                (anchorRowQ12,anchorColumnQ12,(int)packedDragDeltaXY16 >> 16,
                 (int)(short)packedDragDeltaXY16,pairRecord->pairValue,pairRecord->pairKey,accumulatorPlane,
                 fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new heights and keep their difference in the plane for the next step */
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
      if ((applyCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
        FieldGridCell_ComputeDirectionalLightColor(applyCell);
        if (((applyCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell - 1);
        }
        if (((applyCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        applyCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(applyCell,-applyRowStrideBytes); /* row above */
        if ((applyCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        if ((applyCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        /* row below, one to the left: applyCell - 1 + two rows */
        scanCell = (FieldGridCell *)((uint8_t *)(applyCell - 1) + applyRowStrideBytes * 2);
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        applyCell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        applyCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(applyCell,-applyRowStrideBytes); /* back to the changed cell */
      }
    }
    applyCell++;
    scratchHeightCursor++;
    remainingCellCount--;
  } while (remainingCellCount != 0);
}


/* Address: 0x005613C0.
   In-game command INGAME_COMMAND_EDITOR_LOWER_HEIGHTS (0x2290, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor drag (mode G 0, C 1): raises or lowers the terrain by the vertical drag distance, with a cosine
   falloff over the horizontal drag distance. The player's scratch plane holds the height change of the previous
   drag step: it is undone and cleared, then FieldGrid_ProcessVerticalSpan writes the new per-cell offsets around
   the anchor (or around every selected pair) and they are added to the terrain; water surfaces stay at their
   level. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode (0x005703D0).
*/
void FieldGrid_ApplyNegativeCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
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
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  applyCell = fieldGrid->cells;
  countdownOrDelta = remainingCellCount;
  scanCell = applyCell;
  accumulatorPlane = scratchHeightCursor;
  applyRowStrideBytes = rowStrideBytes;
  /* undo and clear the previous step; neighbour refresh as in FieldGrid_ApplyPositiveCellDeltas */
  do {
    cellDelta = *scratchHeightCursor;
    if (cellDelta != 0) {
      scanCell->terrainHeight = scanCell->terrainHeight - cellDelta;
      scanCell->waterSurfaceDelta = scanCell->waterSurfaceDelta + cellDelta;
      *scratchHeightCursor = 0;
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
        if (((scanCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell - 1);
        }
        if (((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell - rowLength; /* row above */
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
        }
        scanCell = scanCell + rowLength * 2 - 1; /* row below, one to the left */
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        cell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
          FieldGridCell_ComputeDirectionalLightColor(cell);
        }
        scanCell = cell - rowLength; /* back to the changed cell */
      }
    }
    scanCell++;
    scratchHeightCursor++;
    countdownOrDelta--;
  } while (countdownOrDelta != 0);
  /* the drag distances are packed as vertical << 16 | horizontal (signed 16-bit each) */
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(anchorRowQ12,anchorColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ProcessVerticalSpan
              ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,anchorRowQ12,
               anchorColumnQ12,accumulatorPlane,fieldGrid);
    scratchHeightCursor = accumulatorPlane;
  }
  else {
    pairRecord = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCells;
    scratchHeightCursor = accumulatorPlane;
    for (remainingPairCount = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->markedCellCount;
        remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ProcessVerticalSpan
                ((int)packedDragDeltaXY16 >> 16,(int)(short)packedDragDeltaXY16,pairRecord->pairValue,
                 pairRecord->pairKey,accumulatorPlane,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  /* apply the new offsets; they stay in the plane for the next step */
  do {
    countdownOrDelta = *scratchHeightCursor;
    if (countdownOrDelta != 0) {
      applyCell->terrainHeight = applyCell->terrainHeight + countdownOrDelta;
      applyCell->waterSurfaceDelta = applyCell->waterSurfaceDelta - countdownOrDelta;
      if ((applyCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
        FieldGridCell_ComputeDirectionalLightColor(applyCell);
        if (((applyCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell - 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell - 1);
        }
        if (((applyCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        applyCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(applyCell,-applyRowStrideBytes); /* row above */
        if ((applyCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        if ((applyCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell + 1);
          FieldGridCell_ComputeDirectionalLightColor(applyCell + 1);
        }
        /* row below, one to the left: applyCell - 1 + two rows */
        scanCell = (FieldGridCell *)((uint8_t *)(applyCell - 1) + applyRowStrideBytes * 2);
        if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,scanCell);
          FieldGridCell_ComputeDirectionalLightColor(scanCell);
        }
        applyCell = scanCell + 1;
        if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
          FieldGridCell_RecomputeTriangleNormalAngles(applyRowStrideBytes,applyCell);
          FieldGridCell_ComputeDirectionalLightColor(applyCell);
        }
        applyCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(applyCell,-applyRowStrideBytes); /* back to the changed cell */
      }
    }
    applyCell++;
    scratchHeightCursor++;
    remainingCellCount--;
  } while (remainingCellCount != 0);
}


/* Address: 0x00561C10.
   In-game command INGAME_COMMAND_EDITOR_REBUILD_INFLUENCE (0x2AE0, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor smoothing brush (mode G 0, C 2): smooths the cell at the given grid position (or at every
   selected pair) with FieldGrid_ApplyRectangularTransition, then refreshes normals and light wherever the height
   now differs from the player's scratch plane (filled with the heights by FieldGrid_ResetLocalInfluenceState
   when the stroke began). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode (0x005703D0).
*/
void FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 gridRowQ12,Q12 gridColumnQ12)

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
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplyRectangularTransition(gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplyRectangularTransition(pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  scratchHeightCursor = playerBlock->terrainHeightScratchPlane;
  rowLength = fieldGrid->gridWidth;
  remainingCellCount = rowLength * fieldGrid->gridHeight;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  scanCell = fieldGrid->cells;
  /* neighbour refresh as in FieldGrid_ApplyPositiveCellDeltas */
  do {
    if ((*scratchHeightCursor != scanCell->terrainHeight) && ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
      FieldGridCell_ComputeDirectionalLightColor(scanCell);
      if (((scanCell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[-1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell - 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell - 1);
      }
      if (((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) && (scratchHeightCursor[1] == 0)) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell - rowLength; /* row above */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell + 1);
        FieldGridCell_ComputeDirectionalLightColor(scanCell + 1);
      }
      scanCell = scanCell + rowLength * 2 - 1; /* row below, one to the left */
      if ((scanCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,scanCell);
        FieldGridCell_ComputeDirectionalLightColor(scanCell);
      }
      cell = scanCell + 1;
      if ((scanCell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
        FieldGridCell_ComputeDirectionalLightColor(cell);
      }
      scanCell = cell - rowLength; /* back to the changed cell */
    }
    scanCell++;
    scratchHeightCursor++;
    remainingCellCount--;
  } while (remainingCellCount != 0);
}


/* Address: 0x00505620.
   Recomputes the packed normal angles of both terrain triangles of every interior cell (the one-cell
   border ring is skipped) after the heights changed, and marks the field grid dirty (runtimeStateFlags
   bit 0).
*/
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

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
    /* cell (row 1, column 1) */
    cellCursor = &fieldGrid->cells[rowLength + 1];
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowLength * sizeof(FieldGridCell),cell); /* row stride in bytes */
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
void FieldGrid_RecomputeInteriorDirectionalLighting
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
    cellCursor = &fieldGrid->cells[rowLength + 1];
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
   when the height was applied (CLC 0x0050927E), set otherwise (STC 0x0050928B); this C version returns
   nothing. The only caller, ArmyRuntime_ClassCommandHandlerGroupA (CALL at 0x005275D7), ignores it: the
   code after the call joins the skip path and overwrites CF with TEST ESI,ESI at 0x00527606.)
*/
void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors
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
  FieldGridCoordinates gridCoordinates;
  uint32_t targetRow;
  uint32_t targetColumn;
  
  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = heightDeltaSourceValue / TERRAIN_SCAN_RADIUS_PER_STEP;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (255 < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 255;
    }
    g_TerrainScanReferenceHeight = worldZQ12;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    baseColumn = gridCoordinates.columnQ12 >> Q12_SHIFT;
    targetRow = gridCoordinates.rowQ12 >> Q12_SHIFT;
    columnFractionQ12 = (uint32_t)(THANDOR_BITCAST(FieldGridCoordinates, uint64_t, gridCoordinates) & FIELD_GRID_COORDINATES_FRACTION_MASK);
    rowFractionQ12 = (uint32_t)((THANDOR_BITCAST(FieldGridCoordinates, uint64_t, gridCoordinates) & FIELD_GRID_COORDINATES_FRACTION_MASK) >> 32);
    /* pick the nearest vertex of the triangulated cell from the Q12 fractions (0x1000 = one cell) */
    fractionSumOrWidth = rowFractionQ12 + columnFractionQ12 * 2;
    targetColumn = baseColumn;
    if (fractionSumOrWidth < FIELD_GRID_CELL_Q12) {
      if (FIELD_GRID_CELL_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow++;
      }
    }
    else if (fractionSumOrWidth < FIELD_GRID_TWO_CELLS_Q12 + 1) {
      targetColumn = baseColumn + 1;
      if (columnFractionQ12 < rowFractionQ12) {
        targetRow++;
        targetColumn = baseColumn;
      }
    }
    else {
      targetColumn = baseColumn + 1;
      if (FIELD_GRID_TWO_CELLS_Q12 - 1 < columnFractionQ12 + rowFractionQ12 * 2) {
        targetRow++;
      }
    }
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    if ((((-1 < (int)targetColumn) && (fractionSumOrWidth = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK, -1 < (int)targetRow)) &&
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
         width) */
      wedgeCellA = &fieldGrid->cells[cellIndex + 1];
      wedgeCellB = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedgeCellA,-g_TerrainScanRowStrideBytes);
      TerrainHeightDelta_ApplyWedge0(0,wedgeCellA);
      cell = wedgeCellB - 1;
      TerrainHeightDelta_ApplyWedge1(0,wedgeCellB);
      wedgeCellA = (FieldGridCell *)((uint8_t *)(cell - 1) + heightDeltaOrRowStride);
      TerrainHeightDelta_ApplyWedge2(0,cell);
      wedgeCellB = (FieldGridCell *)((uint8_t *)wedgeCellA + heightDeltaOrRowStride);
      TerrainHeightDelta_ApplyWedge3(0,wedgeCellA);
      TerrainHeightDelta_ApplyWedge4(0,wedgeCellB);
      TerrainHeightDelta_ApplyWedge5(0,wedgeCellB + 1);
      return;
    }
  }
}


/* Address: 0x005618A0.
   In-game command INGAME_COMMAND_EDITOR_PAINT_MATERIAL (0x2770, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor material brush (mode G 1): writes the material index transitionValue into the cell at the Q12
   grid row/column (or into the cell of every selected pair) with FieldGrid_ApplySingleCellTransition and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode
   (0x005703D0).
*/
void FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  FieldGridAsset *fieldGrid;
  uint32_t remainingPairCount;
  SelectionPlayerPairRecord *pairRecord;
  bool containsAnchorPair;
  
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  containsAnchorPair = SelectionPlayerPairList_ContainsPair(gridRowQ12,gridColumnQ12,playerRuntimeId);
  if (containsAnchorPair) {
    FieldGrid_ApplySingleCellTransition(transitionValue,gridRowQ12,gridColumnQ12,fieldGrid);
  }
  else {
    pairRecord = playerBlock->markedCells;
    for (remainingPairCount = playerBlock->markedCellCount; remainingPairCount != 0; remainingPairCount--) {
      FieldGrid_ApplySingleCellTransition
                (transitionValue,pairRecord->pairValue,pairRecord->pairKey,fieldGrid);
      pairRecord++;
    }
  }
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* Address: 0x00562390.
   In-game command INGAME_COMMAND_EDITOR_SMOOTH (0x3260, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor water drag (mode G 2, E 0): marks the surface dirty and lets FieldGrid_ApplyEncodedUpdateCore
   move the water level of the cell at the Q12 grid row/column by the vertical drag distance (the signed high 16
   bits of packedDragDeltaXY16). Called directly or through the command queue by
   InGameUiCommand_UpdateInteractionByMode (0x005703D0). (The command constant's name notwithstanding, nothing
   is smoothed here; FieldGrid_RebuildLocalInfluenceState is the smoothing brush.)
*/
void FieldGrid_ApplyEncodedCellUpdate(PlayerRuntimeId playerRuntimeId,Q12 gridRowQ12,Q12 gridColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  FieldGrid_ApplyEncodedUpdateCore((int)packedDragDeltaXY16 >> 16,gridRowQ12,gridColumnQ12,fieldGrid);
}


/* Address: 0x005623D0.
   In-game command INGAME_COMMAND_EDITOR_SET_RECEIVER_EXCLUDED (0x32A0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 1): replaces FIELD_CELL_FLUID_RECEIVER_EXCLUDED of the cell at
   the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the surface
   dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode (0x005703D0).
*/
void FieldGrid_SetCellFluidReceiverExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_RECEIVER_EXCLUDED,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* Address: 0x00562410.
   In-game command INGAME_COMMAND_EDITOR_SET_SOURCE_EXCLUDED (0x32E0, handler at INGAME_COMMAND_CODE_BASE +
   code), terrain-editor flag toggle (mode G 2, E 2 and up): replaces FIELD_CELL_FLUID_SOURCE_EXCLUDED of the
   cell at the Q12 grid row/column with setMask (0 or the flag, g_UiCommandTerrainMaskToggleValue) and marks the
   surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode
   (0x005703D0).
*/
void FieldGrid_SetCellFluidSourceExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  FieldGrid_ApplyMaskedRegionCore(~FIELD_CELL_FLUID_SOURCE_EXCLUDED,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
}


/* Address: 0x00562450.
   In-game command INGAME_COMMAND_EDITOR_APPLY_REGION_MASK (0x3320, handler at INGAME_COMMAND_CODE_BASE + code),
   terrain-editor resource brush (mode G 5): sets cell flag bit 11 + materialBitIndex (FIELD_CELL_XENITE_SUPPORT
   for 0, FIELD_CELL_TRITIUM_SUPPORT for 1) in the cell at the Q12 grid row/column, or clears it when bit 31 of
   materialBitIndex is set (g_UiCommandCallerMaskHighBit; the shift count only uses the low 5 bits), and marks
   the surface dirty. Called directly or through the command queue by InGameUiCommand_UpdateInteractionByMode
   (0x005703D0).
*/
void FieldGrid_SetCellResourceSupportFlag
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12)

{
  FieldGridAsset *fieldGrid;
  FieldGridRegionMask setMask;
  uint32_t preserveMask;
  FieldGridRuntimeFlags *runtimeFlagsField;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  setMask = FIELD_CELL_XENITE_SUPPORT << ((uint8_t)materialBitIndex & 31);
  preserveMask = setMask ^ 0xffffffff;
  if (materialBitIndex < 0) {
    setMask = 0;
  }
  FieldGrid_ApplyMaskedRegionCore(preserveMask,setMask,gridRowQ12,gridColumnQ12,fieldGrid);
  runtimeFlagsField = &fieldGrid->runtimeStateFlags;
  *runtimeFlagsField = *runtimeFlagsField | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
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
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnOrCellIndex = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  if ((((columnOrCellIndex < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, gridRowIndex < 0)) ||
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
   Snaps a world position to the nearest grid vertex (cell) like FieldGrid_GetNearestTerrainPoint, but returns
   the height of the top surface there (terrainHeight + waterSurfaceDelta): worldX (EAX), worldY (ECX) and
   height (EDX) with CF clear. Outside the grid CF is set and the input position comes back with height 0.
   Used by SelectionOverlay_DrawWorldPointMarker (0x0052F5A0).
*/
SurfacePointResult
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int columnCellOrSurfaceZ;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  bool outOfBounds;
  SurfacePointResult nearestPoint;
  
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnCellOrSurfaceZ = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  if ((((columnCellOrSurfaceZ < 0) ||
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, gridRowIndex < 0)) ||
      ((int)field->gridWidth <= columnCellOrSurfaceZ)) ||
     (columnCellOrSurfaceZ = field->gridWidth * gridRowIndex + columnCellOrSurfaceZ, (int)field->gridHeight <= gridRowIndex)) {
    columnCellOrSurfaceZ = 0;
    outOfBounds = true;
  }
  else {
    worldX = field->cells[columnCellOrSurfaceZ].worldX;
    worldY = field->cells[columnCellOrSurfaceZ].worldY;
    columnCellOrSurfaceZ = field->cells[columnCellOrSurfaceZ].waterSurfaceDelta + field->cells[columnCellOrSurfaceZ].terrainHeight;
    outOfBounds = false;
  }
  nearestPoint.worldYQ12 = worldY;
  nearestPoint.worldXQ12 = worldX;
  nearestPoint.outOfBounds = outOfBounds;
  nearestPoint.worldZQ12 = columnCellOrSurfaceZ;
  return nearestPoint;
}


/* Address: 0x004FEBA0.
   Water depth at the grid vertex nearest to a world position: the cell's signed waterSurfaceDelta (positive
   when the cell is under water). Outside the grid the result is meaningless (the rounded column index or
   whatever the failed test left in EAX); the callers only ask for points inside the field
   (ArmyArticulatedRuntime_UpdateContactChildAndEffects 0x00521580, ArmyRuntime_UpdateTimedShotAndEffectEmitters
   0x00527C00).
*/
int32_t FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int32_t columnOrWaterDelta;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnOrWaterDelta = (int)((FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
          >> Q12_SHIFT;
  if ((((-1 < columnOrWaterDelta) &&
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, -1 < gridRowIndex)) &&
      (columnOrWaterDelta < (int)field->gridWidth)) && (gridRowIndex < (int)field->gridHeight)) {
    columnOrWaterDelta = field->cells[field->gridWidth * gridRowIndex + columnOrWaterDelta].waterSurfaceDelta;
  }
  return columnOrWaterDelta;
}


/* Address: 0x004FEC10.
   Terrain height at a world position, interpolated linearly over the grid triangle that contains it. Q12
   height in EAX; CF set (height 0) outside the grid or when the cell or its diagonal neighbour is a border
   cell. Entries 0, 2 and 3 of g_FieldGridInterpolationCallbacks5 (0x004FEA30); also called directly by
   ArmyPlacementContact_ApplyTerrainHeight, WorldRuntime_InterpolateTerrainHeightOrSentinel and the army
   movement code.
*/
HeightSampleResult FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int64_t upperTriangleAccumulator;
  int gridColumnIndex;
  uint32_t gridColumnCoordinateQ12;
  uint32_t columnFractionOrHeightQ12;
  uint32_t gridHalfRowCoordinateQ12;
  uint32_t rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  bool sampleFailed;
  HeightSampleResult sampleResult;
  FieldGridDimension gridWidth;
  int64_t weightedHeightAccumulator;
  
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridHalfRowCoordinateQ12 * 2) >> 12, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * sizeof(FieldGridCell);
      columnFractionOrHeightQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
      rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
      /* cell addressing as explained in FieldGrid_InterpolateTopSurfaceHeight */
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)
           == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionOrHeightQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
        if (columnFractionOrHeightQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator =
               (int64_t)
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight * (int64_t)(int)columnFractionOrHeightQ12 +
               ((int64_t)
                FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight
                * (int64_t)(int)rowFractionQ12 -
               (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].terrainHeight *
               (int64_t)triangleDiagonalWeightQ12);
          columnFractionOrHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 |
                  (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          sampleFailed = false;
        }
        else {
          upperTriangleAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].terrainHeight *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight * (int64_t)(int)(columnFractionOrHeightQ12 - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
          columnFractionOrHeightQ12 = (uint32_t)upperTriangleAccumulator >> 12 | (int)((uint64_t)upperTriangleAccumulator >> 32) << 20;
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
   Water depth (waterSurfaceDelta) at a world position, interpolated linearly over the grid triangle that
   contains it like FieldGrid_InterpolateTerrainHeight. Returns 0 outside the grid or on a border cell (the
   original also leaves CF set there; the C prototype drops it). Called by the army movement code
   (ArmyRuntimeClass_UpdateArticulatedMovement, ..UpdateGroundMovementCollisionAndTrackAnimation,
   ..UpdateGroundMovementVariantA).
*/
int32_t FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field)

{
  int64_t upperTriangleAccumulator;
  int gridColumnIndex;
  uint32_t gridColumnCoordinateQ12;
  uint32_t columnFractionQ12;
  uint32_t gridHalfRowCoordinateQ12;
  uint32_t rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  int triangleDiagonalWeightQ12;
  FieldGridDimension gridWidth;
  int64_t weightedWaterDeltaAccumulator;
  
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = field->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridHalfRowCoordinateQ12 * 2) >> 12, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)field->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * sizeof(FieldGridCell);
      columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
      rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
      /* cell addressing as in FieldGrid_InterpolateTopSurfaceHeight */
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0
          ) && ((FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
        if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
          weightedWaterDeltaAccumulator =
               (int64_t)
               FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta * (int64_t)(int)columnFractionQ12 +
               ((int64_t)
                FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta *
                (int64_t)(int)rowFractionQ12 -
               (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].waterSurfaceDelta *
               (int64_t)triangleDiagonalWeightQ12);
          return (uint32_t)weightedWaterDeltaAccumulator >> 12 |
                 (int)((uint64_t)weightedWaterDeltaAccumulator >> 32) << 20;
        }
        upperTriangleAccumulator = (int64_t)
                FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].waterSurfaceDelta * (int64_t)triangleDiagonalWeightQ12 -
                ((int64_t)
                 FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta *
                 (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
                (int64_t)
                FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
        return (uint32_t)upperTriangleAccumulator >> 12 | (int)((uint64_t)upperTriangleAccumulator >> 32) << 20;
      }
    }
  }
  return 0;
}


/* Address: 0x004FEE90.
   Height of the water surface (terrainHeight + waterSurfaceDelta, also where waterSurfaceDelta is negative, i.e.
   the surface lies below the ground) at a world position, interpolated linearly over the grid triangle that
   contains it. Q12 height in EAX; CF set (height 0) outside the grid or on a border cell. Entry 1 of
   g_FieldGridInterpolationCallbacks5 (0x004FEA30); also called directly by
   ArmyPlacementContact_ApplyWaterSurfaceHeight and WorldRuntime_InterpolateWaterSurfaceHeightOrSentinel.
*/
HeightSampleResult FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  int gridColumnIndex;
  uint32_t surfaceHeightQ12;
  Q12 gridColumnCoordinateQ12;
  uint32_t columnFractionQ12;
  Q12 gridHalfRowCoordinateQ12;
  uint32_t rowFractionQ12;
  int rowIndexOrRowOffsetBytes;
  Q12 triangleDiagonalWeightQ12;
  bool sampleFailed;
  HeightSampleResult sampleResult;
  int64_t upperTriangleWeightedHeightAccumulator;
  FieldGridDimension gridWidth;
  int64_t weightedHeightAccumulator;
  
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = gridColumnCoordinateQ12 >> Q12_SHIFT;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = gridHalfRowCoordinateQ12 * 2 >> 12, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * sizeof(FieldGridCell);
      columnFractionQ12 = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
      rowFractionQ12 = gridHalfRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
      /* cell addressing as in FieldGrid_InterpolateTopSurfaceHeight */
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)
         && ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        triangleDiagonalWeightQ12 = (columnFractionQ12 + rowFractionQ12) - FIELD_GRID_CELL_Q12;
        if (columnFractionQ12 + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator =
               (int64_t)
               (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight +
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta) * (int64_t)(int)columnFractionQ12 +
               ((int64_t)
                (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight +
                FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta) *
                (int64_t)(int)rowFractionQ12 -
               (int64_t)
               (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].terrainHeight +
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].waterSurfaceDelta) *
               (int64_t)triangleDiagonalWeightQ12);
          surfaceHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 |
                  (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          sampleFailed = false;
        }
        else {
          upperTriangleWeightedHeightAccumulator =
               (int64_t)
               (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].terrainHeight +
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].waterSurfaceDelta) * (int64_t)triangleDiagonalWeightQ12 -
               ((int64_t)
                (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight +
                FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta) *
                (int64_t)(int)(columnFractionQ12 - FIELD_GRID_CELL_Q12) +
               (int64_t)
               (FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight +
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta) * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
          surfaceHeightQ12 = (uint32_t)upperTriangleWeightedHeightAccumulator >> 12 |
                  (int)((uint64_t)upperTriangleWeightedHeightAccumulator >> 32) << 20;
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
HeightSampleResult FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

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
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnCoordinateQ12 =
       FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridRowCoordinateQ12;
  gridWidth = fieldGrid->gridWidth;
  gridColumnIndex = (int)gridColumnCoordinateQ12 >> Q12_SHIFT;
  if (((-1 < gridColumnIndex) && (rowIndexOrRowOffsetBytes = (int)(gridRowCoordinateQ12 * 2) >> 12, -1 < rowIndexOrRowOffsetBytes)) &&
     (gridColumnIndex < (int)gridWidth)) {
    if (rowIndexOrRowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowIndexOrRowOffsetBytes = rowIndexOrRowOffsetBytes * gridWidth * sizeof(FieldGridCell);
      columnFractionOrWaterDelta = gridColumnCoordinateQ12 & Q12_FRACTION_MASK;
      rowFractionQ12 = gridRowCoordinateQ12 * 2 & Q12_FRACTION_MASK;
      /* the cells around (row, column) are indexed from the start of the row (rowIndexOrRowOffsetBytes is the
         row's byte offset): [column] this cell, [column + 1] the right neighbour, [column + gridWidth] the cell
         below and [column + gridWidth + 1] the one diagonally below right. */
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)
           == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        /* fx + fy - 1: which of the cell's two triangles, and the weight of the far vertex */
        triangleDiagonalWeightQ12 = (columnFractionOrWaterDelta + rowFractionQ12) - FIELD_GRID_CELL_Q12;
        if (columnFractionOrWaterDelta + rowFractionQ12 < FIELD_GRID_CELL_Q12) {
          weightedSurfaceAccumulator =
               (int64_t)
               FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight * (int64_t)(int)columnFractionOrWaterDelta +
               ((int64_t)
                FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight
                * (int64_t)(int)rowFractionQ12 -
               (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].terrainHeight *
               (int64_t)triangleDiagonalWeightQ12);
          terrainHeightQ12 =
               FIXED_PRODUCT_SHR(weightedSurfaceAccumulator, Q12_SHIFT);
          partialAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta * (int64_t)(int)columnFractionOrWaterDelta +
                  ((int64_t)
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta * (int64_t)(int)rowFractionQ12 -
                  (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex].waterSurfaceDelta
                  * (int64_t)triangleDiagonalWeightQ12);
          columnFractionOrWaterDelta = FIXED_PRODUCT_SHR(partialAccumulator, Q12_SHIFT);
          if (-1 < (int)columnFractionOrWaterDelta) {
            terrainHeightQ12 = terrainHeightQ12 + columnFractionOrWaterDelta;
          }
          sampleFailed = false;
        }
        else {
          partialAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].terrainHeight *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].terrainHeight * (int64_t)(int)(columnFractionOrWaterDelta - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].terrainHeight * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
          terrainHeightQ12 = FIXED_PRODUCT_SHR(partialAccumulator, Q12_SHIFT);
          partialAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth + 1].waterSurfaceDelta *
                  (int64_t)triangleDiagonalWeightQ12 -
                  ((int64_t)
                   FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + gridWidth].waterSurfaceDelta * (int64_t)(int)(columnFractionOrWaterDelta - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowIndexOrRowOffsetBytes)[gridColumnIndex + 1].waterSurfaceDelta * (int64_t)(int)(rowFractionQ12 - FIELD_GRID_CELL_Q12));
          columnFractionOrWaterDelta = FIXED_PRODUCT_SHR(partialAccumulator, Q12_SHIFT);
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
   Terrain height and terrain normal at a world position: interpolates terrainHeight over the grid triangle
   that contains it (Q12, EAX) and blends the three vertex normals (triangle0NormalAngles, +0x08) with the
   same barycentric weights, returned as packed angles elevation << 16 | azimuth (EDX). CF set (height 0, EDX
   left over from the fractions) outside the grid or on a border cell. Used by the articulated army contact
   code (ArmyArticulatedRuntime_UpdateLeftTerrainContact, ..RightTerrainContact and their siblings).
*/
HeightNormalSampleResult FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field)

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
  
  rowFractionOrNormalAngles = FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnFractionOrNormalAngles = (FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = field->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 12;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 12, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)field->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * sizeof(FieldGridCell);
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * sizeof(FieldGridCell);
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & Q12_FRACTION_MASK;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & Q12_FRACTION_MASK;
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - FIELD_GRID_CELL_Q12;
        /* height as in FieldGrid_InterpolateTerrainHeight; the vertex normals are scaled by the same weights
           (FixedMath_DirectionFromAnglesScaledRegs), summed and turned back into angles. The right neighbour is
           read relative to the cell's byte offset, the diagonal one with an extra row of bytes. */
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].terrainHeight * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].terrainHeight *
                   (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum].terrainHeight *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum].triangle0NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].terrainHeight * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].terrainHeight *
                   (int64_t)(int)(columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].terrainHeight * (int64_t)(int)(rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12));
          cornerNormalAngles = ((FieldGridCell *)((uint8_t *)field->cells + rowLength * sizeof(FieldGridCell) + cellOffsetOrNormalSum))[1].triangle0NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          columnOrNormalSum = firstNormalEax - scaledNormal.x;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(field->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
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
   Like FieldGrid_InterpolateTerrainHeightAndNormal, but the interpolated value is the water depth
   (waterSurfaceDelta, +0x4C), not the terrain height; the blended normal is still the
   terrain normal (triangle0NormalAngles, +0x08). No caller found in src/ or the image tables.
*/
HeightNormalSampleResult FieldGrid_InterpolateWaterDepthAndTriangle0Normal
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
  
  rowFractionOrNormalAngles = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnFractionOrNormalAngles = (FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 12;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 12, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * sizeof(FieldGridCell);
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * sizeof(FieldGridCell);
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & Q12_FRACTION_MASK;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & Q12_FRACTION_MASK;
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - FIELD_GRID_CELL_Q12;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].waterSurfaceDelta *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].triangle0NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
          diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].waterSurfaceDelta * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)(int)(columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)(int)(rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12));
          cornerNormalAngles = ((FieldGridCell *)((uint8_t *)fieldGrid->cells + rowLength * sizeof(FieldGridCell) + cellOffsetOrNormalSum))[1].triangle0NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          columnOrNormalSum = firstNormalEax - scaledNormal.x;
          rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
          cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
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
   Like FieldGrid_InterpolateTerrainHeightAndNormal, but interpolates the water depth (waterSurfaceDelta, +0x4C)
   and blends the water-surface normals (triangle1NormalAngles, +0x78). No caller found in src/ or the image
   tables.
*/
HeightNormalSampleResult FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
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
  
  rowFractionOrNormalAngles = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnFractionOrNormalAngles = (FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 12;
  if (((-1 < columnOrNormalSum) && (rowOffsetBytes = (int)rowFractionOrNormalAngles >> 12, -1 < rowOffsetBytes)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetBytes < (int)fieldGrid->gridHeight) {
      rowOffsetBytes = rowOffsetBytes * rowLength * sizeof(FieldGridCell);
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & Q12_FRACTION_MASK;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & Q12_FRACTION_MASK;
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - FIELD_GRID_CELL_Q12;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum].waterSurfaceDelta *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + 1].triangle1NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles);
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum].triangle1NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.x;
          normalSumEcx = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength].triangle1NormalAngles;
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum + scaledNormal.z,normalSumEcx + scaledNormal.y,diagonalWeightOrNormalSum + scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
          sampleFailed = false;
        }
        else {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength + 1].waterSurfaceDelta * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)(int)(columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12) +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)(int)(rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12));
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength + 1].triangle1NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum)
          ;
          firstNormalEdx = scaledNormal.z;
          firstNormalEcx = scaledNormal.y;
          firstNormalEax = scaledNormal.x;
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + rowLength].triangle1NormalAngles;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          diagonalWeightOrNormalSum = firstNormalEax - scaledNormal.x;
          normalSumEcx = firstNormalEcx - scaledNormal.y;
          columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetBytes)[columnOrNormalSum + 1].triangle1NormalAngles;
          columnOrNormalSum = firstNormalEdx - scaledNormal.z;
          scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                             ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12);
          blendedNormalAngles = FixedMath_VectorToAngles3Regs
                             (columnOrNormalSum - scaledNormal.z,normalSumEcx - scaledNormal.y,diagonalWeightOrNormalSum - scaledNormal.x);
          rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
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
   Water depth and the normal of the surface that is on top at a world position: interpolates waterSurfaceDelta
   (+0x4C) over the grid triangle (EAX) and blends the terrain normals (triangle0NormalAngles) where the result is
   negative (dry) or the water-surface normals (triangle1NormalAngles) otherwise (packed angles in EDX). CF set
   outside the grid or on a border cell. No caller found in src/ or the image tables.
*/
HeightNormalSampleResult FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesRegs
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
  
  rowFractionOrNormalAngles = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  columnFractionOrNormalAngles = (FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rowFractionOrNormalAngles;
  rowFractionOrNormalAngles = rowFractionOrNormalAngles * 2;
  rowLength = fieldGrid->gridWidth;
  columnOrNormalSum = (int)columnFractionOrNormalAngles >> 12;
  if (((-1 < columnOrNormalSum) && (rowOffsetOrNormalSum = (int)rowFractionOrNormalAngles >> 12, -1 < rowOffsetOrNormalSum)) && (columnOrNormalSum < (int)rowLength)) {
    if (rowOffsetOrNormalSum < (int)fieldGrid->gridHeight) {
      rowOffsetOrNormalSum = rowOffsetOrNormalSum * rowLength * sizeof(FieldGridCell);
      cellOffsetOrNormalSum = rowOffsetOrNormalSum + columnOrNormalSum * sizeof(FieldGridCell);
      columnFractionOrNormalAngles = columnFractionOrNormalAngles & Q12_FRACTION_MASK;
      rowFractionOrNormalAngles = rowFractionOrNormalAngles & Q12_FRACTION_MASK;
      if (((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) &&
         ((FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)) {
        diagonalWeightOrNormalSum = (columnFractionOrNormalAngles + rowFractionOrNormalAngles) - FIELD_GRID_CELL_Q12;
        if (columnFractionOrNormalAngles + rowFractionOrNormalAngles < FIELD_GRID_CELL_Q12) {
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)(int)columnFractionOrNormalAngles +
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)(int)rowFractionOrNormalAngles -
                  (int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].waterSurfaceDelta *
                  (int64_t)diagonalWeightOrNormalSum);
          cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].triangle1NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          if ((int)interpolatedHeightQ12 < 0) {
            cornerNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].triangle0NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)cornerNormalAngles >> 16,cornerNormalAngles & FIXED_ANGLE16_MASK,columnFractionOrNormalAngles);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum].triangle1NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            columnFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle1NormalAngles;
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)columnFractionOrNormalAngles >> 16,columnFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowFractionOrNormalAngles);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum + scaledNormal.z,diagonalWeightOrNormalSum + scaledNormal.y,cellOffsetOrNormalSum + scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
            sampleFailed = false;
          }
        }
        else {
          columnWeightQ12 = columnFractionOrNormalAngles - FIELD_GRID_CELL_Q12;
          rowWeightQ12 = rowFractionOrNormalAngles - FIELD_GRID_CELL_Q12;
          weightedHeightAccumulator = (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].waterSurfaceDelta * (int64_t)diagonalWeightOrNormalSum -
                  ((int64_t)FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].waterSurfaceDelta
                   * (int64_t)columnWeightQ12 +
                  (int64_t)
                  FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].waterSurfaceDelta * (int64_t)rowWeightQ12);
          rowFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength + 1].triangle1NormalAngles;
          interpolatedHeightQ12 = (uint32_t)weightedHeightAccumulator >> 12 | (int)((uint64_t)weightedHeightAccumulator >> 32) << 20;
          if ((int)interpolatedHeightQ12 < 0) {
            rowFractionOrNormalAngles = ((FieldGridCell *)((uint8_t *)fieldGrid->cells + rowLength * sizeof(FieldGridCell) + cellOffsetOrNormalSum))[1].triangle0NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            rowFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle0NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,columnWeightQ12);
            columnOrNormalSum = firstNormalEax - scaledNormal.x;
            rowOffsetOrNormalSum = firstNormalEcx - scaledNormal.y;
            rowFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,cellOffsetOrNormalSum)[1].triangle0NormalAngles;
            cellOffsetOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (cellOffsetOrNormalSum - scaledNormal.z,rowOffsetOrNormalSum - scaledNormal.y,columnOrNormalSum - scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
            sampleFailed = false;
          }
          else {
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,diagonalWeightOrNormalSum);
            firstNormalEdx = scaledNormal.z;
            firstNormalEcx = scaledNormal.y;
            firstNormalEax = scaledNormal.x;
            rowFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + rowLength].triangle1NormalAngles;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,columnWeightQ12);
            cellOffsetOrNormalSum = firstNormalEax - scaledNormal.x;
            diagonalWeightOrNormalSum = firstNormalEcx - scaledNormal.y;
            rowFractionOrNormalAngles = FIELD_GRID_CELL_AT_BYTE_OFFSET(fieldGrid->cells,rowOffsetOrNormalSum)[columnOrNormalSum + 1].triangle1NormalAngles;
            columnOrNormalSum = firstNormalEdx - scaledNormal.z;
            scaledNormal = FixedMath_DirectionFromAnglesScaledRegs
                               ((int)rowFractionOrNormalAngles >> 16,rowFractionOrNormalAngles & FIXED_ANGLE16_MASK,rowWeightQ12);
            blendedNormalAngles = FixedMath_VectorToAngles3Regs
                               (columnOrNormalSum - scaledNormal.z,diagonalWeightOrNormalSum - scaledNormal.y,cellOffsetOrNormalSum - scaledNormal.x);
            rowFractionOrNormalAngles = blendedNormalAngles.elevationAngle << 16 | blendedNormalAngles.azimuthAngle & FIXED_ANGLE16_MASK;
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
   Placement test at a world position: returns false (CF clear) when the nearest grid cell's occupancy byte of
   faction slot factionSlot has any FIELD_CELL_OCCUPANCY_PRESENCE_BITS set, true (CF set, "blocked") when the
   faction is not present there or the point is outside the grid. Called by
   ArmyPlacement_TestGridRuntimeAndFieldBlocking (0x00528110), ArmyPlacementCollision_TestCurrentRuntime
   (0x00527740) and ..TestCandidateAndClearance (0x005278D0) with the owner army's faction index.
*/
bool FieldGrid_TestWorldPointBlocked
          (FieldGridByteOffset factionSlot,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          )

{
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex =
       (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) + FIELD_GRID_CELL_Q12 / 2)
       >> Q12_SHIFT;
  if ((((-1 < gridColumnIndex) &&
       (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, -1 < gridRowIndex)) &&
      (gridColumnIndex < (int)fieldGrid->gridWidth)) &&
     ((gridRowIndex < (int)fieldGrid->gridHeight &&
      ((((uint8_t *)&fieldGrid->cells[fieldGrid->gridWidth * gridRowIndex + gridColumnIndex].occupancyMask)[factionSlot] & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0)))) {
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
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid)

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

  phaseSeedBitWidth = *(uint32_t *)((uint8_t *)g_TerrainSurfacePacketTablePayload - 0x20);
  rowsRemaining = fieldGrid->gridHeight;
  fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | FIELD_GRID_RUNTIME_SURFACE_DIRTY;
  cellsPerRow = fieldGrid->gridWidth;
  cell = fieldGrid->cells;
  columnsRemaining = cellsPerRow;
  do {
    do {
      cellWorldXQ12 = cell->worldX;
      cellWorldYQ12 = cell->worldY;
      /* 0x77ff1fff: the edge flags are rebuilt from scratch, the debug mark starts cleared */
      cell->flagsAndMaterial =
           cell->flagsAndMaterial & ~(FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_DEBUG_MARKED);
      phaseRandomValue = Random_NextPrimary();
      cell->flagsAndMaterial = cell->flagsAndMaterial & ~FIELD_CELL_RANDOM_VARIANT_MASK;
      cell->surfacePacketIndex = phaseRandomValue & (1 << ((uint8_t)phaseSeedBitWidth & 31)) - 1U;
      /* 16x16 tiling of the 256 direction records over the world */
      cell->persistedAux54 =
           (FieldCellPersistedAux)
           (g_TerrainDirectionRecordTable256 + (cellWorldYQ12 & 0xfU) + (cellWorldXQ12 & 0xfU) * 16);
      cell->armyRuntimeSavedOffset = 0;
      materialVariantRandomBits = Random_NextPrimary();
      cell->overlayColor = 0xffffffff; /* ARGB opaque white */
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
    rowBytesOrBottomCellAddress = rowBytesOrBottomCellAddress + sizeof(FieldGridCell);
    bottomRowCellsRemaining--;
  } while (bottomRowCellsRemaining != 0);
}


/* Address: 0x00503DB0.
   Marks the field surface dirty and re-binds every cell's terrain direction record (+0x54) from the low nibbles
   of its world X/Y, the same 16x16 tiling FieldGrid_InitializeRuntimeCellsAndBoundaryFlags uses. The secondary
   terrain resource load calls this instead of the full initialization, so the other cell state is kept.
*/
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid)

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
   For every field cell, maps the fog-of-war lighting index (visibilityLightingIndex, +0x68) through the
   256x256 terrain clamp lookup, keyed by the cell's occupancy byte of the given faction, and writes the result
   back. Runs on tick-wheel cases 3 and 7 after the per-class terrain-state refresh callbacks.
*/
void FieldGrid_ApplyByteClampLookupToCells(FieldGridByteOffset factionIndex,FieldGridAsset *fieldGrid)

{
  uint8_t *clampLookup;
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  uint8_t mappedLightingIndex;
  FieldGridDimension gridWidth;

  gridWidth = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  clampLookup = g_TerrainByteClampLookup;
  columnsRemaining = gridWidth;
  do {
    do {
      /* The lookup is 64-KiB aligned: the original loads AH = occupancy byte, AL = lighting index into the
         pointer's low word, i.e. indexes the table with (occupancy << 8) | lighting index. */
      mappedLightingIndex =
           clampLookup[(uint32_t)((uint8_t *)&currentCell->occupancyMask)[factionIndex] << 8 |
                       (uint32_t)currentCell->visibilityLightingIndex];
      currentCell->visibilityLightingIndex = mappedLightingIndex;
      currentCell = currentCell + 1;
      columnsRemaining = columnsRemaining - 1;
    } while (columnsRemaining != 0);
    rowsRemaining = rowsRemaining - 1;
    columnsRemaining = gridWidth;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x00503E80.
   Rebuilds every cell's fog-of-war lighting index (visibilityLightingIndex, +0x68) from the occupancy byte of
   one faction slot (usually the active faction): FIELD_CELL_LIGHTING_VISIBLE when a current presence bit
   (FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) is set, FIELD_CELL_LIGHTING_EXPLORED when only the persistent
   bit 7 is, FIELD_CELL_LIGHTING_UNEXPLORED otherwise.
*/
void FieldGrid_ClassifyCellFlagsToRuntimeByte(FieldGridByteOffset factionSlot,FieldGridAsset *fieldGrid)

{
  uint8_t lightingIndex;
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    lightingIndex = FIELD_CELL_LIGHTING_VISIBLE;
    if (((((uint8_t *)&currentCell->occupancyMask)[factionSlot] & FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) == 0) &&
       (lightingIndex = FIELD_CELL_LIGHTING_EXPLORED,
       (((uint8_t *)&currentCell->occupancyMask)[factionSlot] & FIELD_CELL_OCCUPANCY_PERSISTENT_BIT) == 0)) {
      lightingIndex = FIELD_CELL_LIGHTING_UNEXPLORED;
    }
    currentCell->visibilityLightingIndex = lightingIndex;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x00503EE0.
   Animates the 256 terrain direction records (random rates and scales from TerrainVisualResources_LoadPrimary):
   both 16-bit angles advance by their rates, and the scaled sine/cosine of angle A and one component of
   angle B are stored, computed from the angles before this step.
*/
void TerrainDirectionTable_AdvanceAndRebuildVectors(void)

{
  uint32_t previousPackedAngles;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCosEdxEax8 scaledSinCosPair;
  FixedSinCosEdxEax8 angleBScaledSinCosPair;

  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 256;
  do {
    previousPackedAngles = currentDirectionRecord->packedAngles;
    /* one 32-bit add advances both packed angles by rateA (low word) and rateB (high word); a carry out
       of angle A moves angle B by one more */
    currentDirectionRecord->packedAngles = currentDirectionRecord->packedAngles + *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(previousPackedAngles & FIXED_ANGLE16_MASK,currentDirectionRecord->scaleA);
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
TerrainRaycastResult FieldGrid_RaycastTerrainSurfaceDistance
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
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell));
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowOrCurrentColumnQ12 =
       FIXED_PRODUCT_SHR(rayEndRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) -
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
                        rowLength * sizeof(FieldGridCell),currentCell,currentRowQ12,rayEndHalfRowOrCurrentColumnQ12);
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    rayEndHalfRowOrCurrentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  /* a miss leaves EDX as is: the current row, or end row - current row when the traversal reached the ray end.
     No caller reads it on a miss (shots index their impact table only with a terrain distance within range). */
  missResult.materialOrCellIndex = currentRowQ12;
  missResult.distanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  missResult.hit = false;
  return missResult;
}


/* Address: 0x00504CA0.
   Same walk as FieldGrid_RaycastTerrainSurfaceDistance, but against the secondary (water) surface: each triangle
   corner is terrainHeight + waterSurfaceDelta. Same result contract: distance and material byte with CF set on
   a hit, FIELD_GRID_RAYCAST_MISS_DISTANCE with CF clear on a miss.
*/
TerrainRaycastResult FieldGrid_RaycastSecondarySurfaceDistance
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
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  /* &cells[row * rowLength + column], written as the original's byte arithmetic */
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowLength * sizeof(FieldGridCell));
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  /* ...and for the ray end */
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  rayEndHalfRowOrCurrentColumnQ12 =
       FIXED_PRODUCT_SHR(rayEndRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) -
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
                        rowLength * sizeof(FieldGridCell),currentCell,currentRowQ12,rayEndHalfRowOrCurrentColumnQ12);
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    rayEndHalfRowOrCurrentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  /* a miss leaves EDX as is: the current row, or end row - current row when the traversal reached the ray end.
     No caller reads it on a miss (shots index their impact table only with a terrain distance within range). */
  missResult.materialOrCellIndex = currentRowQ12;
  missResult.distanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
  missResult.hit = false;
  return missResult;
}


/* Address: 0x00504E60.
   Casts a ray of length rayScaleQ12 from a world point in the direction (elevation, azimuth) over the terrain
   triangles and returns the first hit: distance and the material byte of the hit cell (hit = true, CF clear),
   or FIELD_GRID_RAYCAST_MISS_DISTANCE when it ends first or after FIELD_GRID_RAYCAST_MAX_STEPS - 1 cells (EDX
   then holds the traversal's last row coordinate). Cells outside the grid are clamped to the border. Used by
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy (0x004CDD40) along the render context's view
   angles from a model's sample points, to find terrain between them and the viewer.
*/
TerrainRaycastResult FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginYQ12,Q12 rayOriginXQ12,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int64_t rayEndColumnProduct;
  int64_t rayEndHalfRowProduct;
  uint32_t rayStartRowQ12;
  int rayEndRowQ12;
  uint32_t rayStartColumnQ12;
  int rayEndColumnQ12;
  int columnOrClampOffset;
  int maxRowIndex;
  uint32_t rayStartHalfRowQ12;
  uint32_t endHalfRowOrCurrentColumnQ12;
  uint32_t currentRowQ12;
  int rowFromStartQ12;
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
  rayStartHalfRowQ12 = FIXED_MUL_SHR(rayOriginYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  rayStartColumnQ12 =
       (FIXED_MUL_SHR(rayOriginXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT)) - rayStartHalfRowQ12;
  rayStartRowQ12 = rayStartHalfRowQ12 * 2;
  rowLength = fieldGrid->gridWidth;
  rowStrideBytes = rowLength * sizeof(FieldGridCell);
  rayDirection = FixedMath_DirectionFromAnglesScaledRegs(elevationAngle,azimuthAngle,rayScaleQ12);
  rayEndColumnProduct = (int64_t)(int)(rayDirection.x + rayOriginXQ12) * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rayEndHalfRowProduct = (int64_t)(int)(rayDirection.y + rayOriginYQ12) * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  endHalfRowOrCurrentColumnQ12 = FIXED_PRODUCT_SHR(rayEndHalfRowProduct, Q20_SHIFT + 1);
  rayEndColumnQ12 = (FIXED_PRODUCT_SHR(rayEndColumnProduct, Q20_SHIFT)) - endHalfRowOrCurrentColumnQ12;
  rayEndRowQ12 = endHalfRowOrCurrentColumnQ12 * 2;
  stepsRemaining = FIELD_GRID_RAYCAST_MAX_STEPS;
  endHalfRowOrCurrentColumnQ12 = rayStartColumnQ12 & ~(FIELD_GRID_CELL_Q12 - 1u);
  currentRowQ12 = rayStartRowQ12 & ~(FIELD_GRID_CELL_Q12 - 1u);
  currentCell = (FieldGridCell *)((uint8_t *)&fieldGrid->cells[(int)rayStartColumnQ12 >> Q12_SHIFT] + ((int)rayStartRowQ12 >> Q12_SHIFT) * rowStrideBytes);
  do {
    stepsRemaining--;
    if (stepsRemaining == 0) break;
    rowFromStartQ12 = currentRowQ12 + rayStartHalfRowQ12 * -2;
    columnOrClampOffset = (int)((endHalfRowOrCurrentColumnQ12 - rayStartColumnQ12) + rayStartColumnQ12) >> Q12_SHIFT;
    cellRowIndex = (int)(rowFromStartQ12 + rayStartRowQ12) >> Q12_SHIFT;
    if (columnOrClampOffset < 0) {
      sampleCell = currentCell + -columnOrClampOffset;
      columnOrClampOffset = cellRowIndex;
      if ((cellRowIndex < 0) || (columnOrClampOffset = cellRowIndex - maxRowIndex, maxRowIndex <= cellRowIndex)) {
        /* both clamped: the single corner cell */
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(sampleCell,-columnOrClampOffset * rowStrideBytes);
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
      sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(currentCell,-cellRowIndex * rowStrideBytes);
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
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(currentCell,-(cellRowIndex - maxRowIndex) * rowStrideBytes);
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
        sampleCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(sampleCell,-(cellRowIndex - maxRowIndex) * rowStrideBytes);
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
                       (rayDirection.z,rayEndRowQ12 + rayStartHalfRowQ12 * -2,rayEndColumnQ12 - rayStartColumnQ12,
                        rayOriginZQ12,cornerHeight0Q12,cornerHeight1Q12,cornerHeight2Q12,
                        cornerHeight3Q12,rowFromStartQ12,endHalfRowOrCurrentColumnQ12 - rayStartColumnQ12);
    if (!triangleHit.missed) {
      hitResult.hit = true;
      hitResult.distanceQ12 = triangleHit.distanceQ12;
      hitResult.materialOrCellIndex = sampleCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK; /* low byte: material */
      return hitResult;
    }
    traversalDone = TerrainRay_AdvanceGridTraversal
                       (rayEndRowQ12,rayEndColumnQ12,rayStartRowQ12,rayStartColumnQ12,
                        rowStrideBytes,currentCell,currentRowQ12,endHalfRowOrCurrentColumnQ12);    
    currentCell = g_TerrainRayNextCell; /* ESI/ECX/EDX results of the step */
    endHalfRowOrCurrentColumnQ12 = g_TerrainRayNextCoord1Q12;
    currentRowQ12 = g_TerrainRayNextCoord0Q12;
  } while (!traversalDone);
  missResult.materialOrCellIndex = currentRowQ12; /* EDX as the traversal left it (see above) */
  missResult.distanceQ12 = FIELD_GRID_RAYCAST_MISS_DISTANCE;
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
      ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] =
           ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] |
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
      ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] =
           ((uint8_t *)&currentCell->occupancyMask)[occupancyMaskByteIndex] &
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
bool TerrainGrid_TestProjectedCellMaskBits01(Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;

  activeFieldGrid = worldRuntime->fieldGrid;
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) +
                 FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((((-1 < gridColumnIndex) && (gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, -1 < gridRowIndex)) &&
      (gridColumnIndex < (int)activeFieldGrid->gridWidth)) &&
     ((gridRowIndex < (int)activeFieldGrid->gridHeight &&
      ((((uint8_t *)&activeFieldGrid->cells[activeFieldGrid->gridWidth * gridRowIndex + gridColumnIndex].occupancyMask)[worldRuntime->activeFactionRuntimeIndex] &
        FIELD_CELL_OCCUPANCY_BITS01) != 0)))) {
    return false;
  }
  return true;
}


/* Address: 0x005092A0.
   Clears the debug mark (FIELD_CELL_DEBUG_MARKED, bit 15) in every cell of the grid. No caller
   found in src/ or the image tables, and no code in the game sets the mark.
*/
void FieldGrid_ClearUnresolvedFlagInAllCells(FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->flagsAndMaterial = currentCell->flagsAndMaterial & ~FIELD_CELL_DEBUG_MARKED;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x005092E0.
   Sets the overlay colour (ARGB, cell +0x04) of every field-grid cell to one value.
*/
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid)

{
  int cellsRemaining;
  FieldGridCell *currentCell;

  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    currentCell->overlayColor = argbColor;
    currentCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x00532B60.
   Map editor save of the field grid: copies the loaded asset image (its size is the second dword) into a
   temporary block, resets the runtime-only cell state (normals to straight up, derived dwords and occupancy
   to 0, runtime flag bits cleared), rebuilds fieldFlags as the set of used material ids and writes the block to
   g_LevelResourcePathScratchUtf16. Returns the free status (CF clear) on success, or CF set with the
   allocation or write error. Called by InGameUiCommand_SaveFieldAndLevelAssetImages (0x005622F0).
*/
StatusResult FieldGrid_SaveAssetImageFromRuntimeState(uint32_t *sourceImageDwords)

{
  FieldGridAsset *writeErrorValue;
  FieldGridAsset *fieldGridImageCopy;
  uint32_t imageSizeOrDwordsLeft;
  int cellsRemaining;
  int occupancyBytesLeft;
  uint32_t *copyDestinationDwords;
  FieldGridCellSaveImageView *fieldGridCellSaveView;
  uint8_t *occupancyBytes;
  ArenaAllocResult allocResult;
  StatusResult writeStatus;
  ArenaFreeResult freeResult;
  
  imageSizeOrDwordsLeft = sourceImageDwords[1];
  allocResult = g_MemoryApi.alloc(imageSizeOrDwordsLeft);
  fieldGridImageCopy = (FieldGridAsset *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    copyDestinationDwords = (uint32_t *)fieldGridImageCopy;
    for (imageSizeOrDwordsLeft = imageSizeOrDwordsLeft >> 2; imageSizeOrDwordsLeft != 0; imageSizeOrDwordsLeft--) {
      *copyDestinationDwords = *sourceImageDwords;
      sourceImageDwords++;
      copyDestinationDwords++;
    }
    fieldGridCellSaveView = (FieldGridCellSaveImageView *)fieldGridImageCopy->cells;
    fieldGridImageCopy->fieldFlags = 0;
    cellsRemaining = fieldGridImageCopy->gridWidth * fieldGridImageCopy->gridHeight;
    do {
      fieldGridCellSaveView->surfacePacketIndex = 0;
      fieldGridCellSaveView->triangle0NormalAngles = FIXED_ANGLE16_QUARTER_TURN << 16; /* elevation: straight up */
      fieldGridCellSaveView->groundScreenX = 0;
      fieldGridCellSaveView->groundScreenY = 0;
      fieldGridCellSaveView->groundViewX = 0;
      fieldGridCellSaveView->groundViewY = 0;
      fieldGridCellSaveView->groundViewZ = 0;
      fieldGridCellSaveView->secondarySurfaceScreenX = 0;
      fieldGridCellSaveView->secondarySurfaceScreenY = 0;
      fieldGridCellSaveView->secondarySurfaceViewX = 0;
      fieldGridCellSaveView->secondarySurfaceViewY = 0;
      fieldGridCellSaveView->secondarySurfaceViewZ = 0;
      /* keep the material byte, the resource-support, map-edge and fluid-exclusion bits (0xe80078ff) */
      fieldGridCellSaveView->flagsAndMaterial =
           fieldGridCellSaveView->flagsAndMaterial &
           (FIELD_CELL_MATERIAL_ID_MASK | FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK | FIELD_CELL_GRID_EDGE_MASK |
            FIELD_CELL_FLUID_RECEIVER_EXCLUDED | FIELD_CELL_FLUID_SOURCE_EXCLUDED);
      fieldGridCellSaveView->persistedAux54 = 0;
      fieldGridCellSaveView->groundDirectionalLightColor = 0;
      fieldGridCellSaveView->secondarySurfaceDirectionalLightColor = 0;
      fieldGridCellSaveView->shadedGroundColor = 0;
      fieldGridCellSaveView->shadedSecondarySurfaceColor = 0;
      fieldGridCellSaveView->visibilityLightingIndex = 0;
      /* one bit per material id in use */
      fieldGridImageCopy->fieldFlags =
           fieldGridImageCopy->fieldFlags |
           1 << ((uint8_t)fieldGridCellSaveView->flagsAndMaterial & 31);
      occupancyBytes = (uint8_t *)&fieldGridCellSaveView->occupancyMask;
      for (occupancyBytesLeft = 8; occupancyBytesLeft != 0; occupancyBytesLeft--) {
        *occupancyBytes = 0;
        occupancyBytes++;
      }
      fieldGridCellSaveView++;
      cellsRemaining--;
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
   In-game command INGAME_COMMAND_EDITOR_CLEAR_SCRATCH (0x1F20, handler at INGAME_COMMAND_CODE_BASE + code):
   zeroes the player's terrain scratch plane (one dword per cell) at the start of a height drag (mode G 0,
   C 0/1), so that FieldGrid_ApplyPositiveCellDeltas / ..NegativeCellDeltas have no previous step to undo. The
   other payload dwords are unused. Called directly or through the command queue by
   InGameUiCommand_BeginInteractionByMode (0x0056FA70).
*/
void FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0;
      cellsRemaining--) {
    *scratchHeightCursor = 0;
    scratchHeightCursor++;
  }
}


/* Address: 0x00561BB0.
   In-game command INGAME_COMMAND_EDITOR_RESET_INFLUENCE (0x2A80, handler at INGAME_COMMAND_CODE_BASE + code):
   copies every cell's terrain height into the player's scratch plane at the start of a smoothing stroke
   (mode G 0, C 2), the reference FieldGrid_RebuildLocalInfluenceState compares against. The other payload
   dwords are unused. Called directly or through the command queue by InGameUiCommand_BeginInteractionByMode
   (0x0056FA70).
*/
void FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12)

{
  int cellsRemaining;
  FieldGridCell *currentCell;
  int *scratchHeightCursor;
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  scratchHeightCursor =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->terrainHeightScratchPlane;
  cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight;
  currentCell = fieldGrid->cells;
  do {
    *scratchHeightCursor = currentCell->terrainHeight;
    currentCell++;
    scratchHeightCursor++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
}


/* Address: 0x00571EC0.
   Moves the water level of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) by -64 per
   heightDeltaUnits (the vertical drag distance, so dragging up raises it) and refreshes the normals and light of
   the cell and of its six neighbours that are not border cells. Called by FieldGrid_ApplyEncodedCellUpdate.
*/
void FieldGrid_ApplyEncodedUpdateCore(FieldGridHeightDeltaUnits heightDeltaUnits,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnIndex;
  int rowIndex;
  int rowStrideBytes;
  FieldGridCell *cell;
  
  rowLength = fieldGrid->gridWidth;
  columnIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((((-1 < columnIndex) && (rowIndex = gridRowQ12 >> Q12_SHIFT, -1 < rowIndex)) && (columnIndex < (int)rowLength)) &&
     (rowIndex < (int)fieldGrid->gridHeight)) {
    rowStrideBytes = rowLength * sizeof(FieldGridCell);
    cell = fieldGrid->cells + columnIndex + rowIndex * rowLength;
    cell->waterSurfaceDelta = cell->waterSurfaceDelta + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12; /* 64 per unit */
    FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
    FieldGridCell_ComputeDirectionalLightColor(cell);
    if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell - 1);
      FieldGridCell_ComputeDirectionalLightColor(cell - 1);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell - rowLength; /* row above */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
    cell = cell + rowLength * 2 - 1; /* row below, one to the left */
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell);
      FieldGridCell_ComputeDirectionalLightColor(cell);
    }
    if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FieldGridCell_RecomputeTriangleNormalAngles(rowStrideBytes,cell + 1);
      FieldGridCell_ComputeDirectionalLightColor(cell + 1);
    }
  }
}


/* Address: 0x005058A0.
   One cell of FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface: when the cell lies strictly inside the
   circle, its height changes by amplitude * (distance^2 / radius^2 - 1) (a paraboloid, -amplitude at the centre,
   0 at the rim), cells with water (waterSurfaceDelta >= 0) keep their water level, and a non-negative
   terrainMaterialIndexOrNegativeSentinel replaces the material byte.
*/
void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial(TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
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
  distanceSquaredHigh = (uint32_t)(distanceSquared >> 32);
  deltaXOrRadiusSquaredHigh = (int)((uint64_t)((int64_t)radiusWorldUnits * (int64_t)radiusWorldUnits) >> 32);
  radiusSquaredOrHeightDelta = (uint32_t)((int64_t)radiusWorldUnits * (int64_t)radiusWorldUnits);
  if (((int)distanceSquaredHigh <= deltaXOrRadiusSquaredHigh) &&
     ((((int)distanceSquaredHigh < deltaXOrRadiusSquaredHigh || ((int)distanceSquared < (int)radiusSquaredOrHeightDelta)) &&
      (radiusSquaredOrHeightDelta = deltaXOrRadiusSquaredHigh << 20 | radiusSquaredOrHeightDelta >> 12, radiusSquaredOrHeightDelta != 0)))) {
    scaledDeltaProduct = (int64_t)
            ((int)((int64_t)((uint64_t)distanceSquaredHigh << 32 | distanceSquared & UINT32_MAX) / (int64_t)(int)radiusSquaredOrHeightDelta)
            - Q12_ONE) * (int64_t)terrainHeightDeltaAmplitudeQ12;
    radiusSquaredOrHeightDelta = FIXED_PRODUCT_SHR(scaledDeltaProduct, Q12_SHIFT);
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
}


/* Address: 0x00505AA0.
   Water flow pass A (tick-wheel case 1): scans the interior cells row by row and pulls the water surface
   (terrainHeight + waterSurfaceDelta) of the six hexagonal neighbours 1/8 of the way toward the source
   cell's surface. Sources with negative water or FIELD_CELL_FLUID_SOURCE_EXCLUDED are skipped, receivers
   with FIELD_CELL_FLUID_RECEIVER_EXCLUDED are left alone.
   The source is the centre cell itself ([ESI+0x4C] with ESI = centre); verified against the original
   machine code by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

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
void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int rowStartCellAddress;
  int centerCellAddress;
  int upperCellAddress;
  int *neighborWaterDelta;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCellAddress =
       (int)fieldGrid + rowLength * -0x80 + (rowLength * fieldGrid->gridHeight + -1) * 0x80 + 0x200;
  do {
    columnsRemaining = rowLength - 2;
    centerCellAddress = rowStartCellAddress;
    do {
      centerCellAddress = centerCellAddress - 0x80;
      if ((-1 < ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta) &&
         (((uint32_t)((FieldGridCell *)centerCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)->terrainHeight;
        upperCellAddress = centerCellAddress + rowLength * -0x80;
        if (((uint32_t)((FieldGridCell *)upperCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          neighborWaterDelta = &((FieldGridCell *)upperCellAddress)->waterSurfaceDelta;
          *neighborWaterDelta =
               *neighborWaterDelta -
               ((((FieldGridCell *)upperCellAddress)->waterSurfaceDelta + ((FieldGridCell *)upperCellAddress)->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)upperCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)upperCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)upperCellAddress)[1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperCellAddress + 0x50 + rowLength * 0x100) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperCellAddress + rowLength * 0x100))[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperCellAddress + rowLength * 0x100))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperCellAddress + rowLength * 0x100))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperCellAddress + rowLength * 0x100))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
                              3);
        }
        centerCellAddress = upperCellAddress + rowLength * sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)centerCellAddress)[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta =
               ((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta -
               ((((FieldGridCell *)centerCellAddress)[-1].waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)[-1].terrainHeight
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)centerCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)centerCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)[1].terrainHeight) -
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
   Water relaxation, forward, without the sign gate: like pass A
   (TerrainGrid_RelaxNeighborHeightsForwardWithSignGate) it pulls the water surface of the six neighbours of
   every interior cell 1/8 of the way toward that cell's surface, but cells with negative water are sources too.
   FIELD_CELL_FLUID_SOURCE_EXCLUDED and FIELD_CELL_FLUID_RECEIVER_EXCLUDED are honoured. Run by
   TerrainGrid_RunDirectionalRelaxationPasses when mode bit 0 is set (the editor's land tool); verified against
   the original by OPEN_THANDOR_SELFTEST=relaxcmp. (cellBeforeSource is advanced at the top of the inner loop, so
   inside it it is the source cell.)
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
   Water relaxation, backward, without the sign gate: TerrainGrid_RelaxNeighborHeightsForward scanning the
   interior cells from the bottom-right, the partner pass of the land tool in
   TerrainGrid_RunDirectionalRelaxationPasses. Cells are addressed by raw byte offsets (cell size 0x80; +0x48
   terrainHeight, +0x4C waterSurfaceDelta, +0x50 flagsAndMaterial; 0x40000000 = FIELD_CELL_FLUID_SOURCE_EXCLUDED,
   0x20000000 = FIELD_CELL_FLUID_RECEIVER_EXCLUDED). Verified by OPEN_THANDOR_SELFTEST=relaxcmp.
*/
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  int sourceCellAddress;
  int cellAfterSourceAddress;
  int upperRowCellAddress;
  int *neighborWaterDelta;
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
      if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)->flagsAndMaterial & 0x40000000) == 0) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)cellAfterSourceAddress)->waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)->terrainHeight;
        upperRowCellAddress = cellAfterSourceAddress + rowLength * -0x80;
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)->flagsAndMaterial & 0x20000000) == 0) {
          neighborWaterDelta = &((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta;
          *neighborWaterDelta =
               *neighborWaterDelta -
               ((((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)[1].flagsAndMaterial & 0x20000000) == 0) {
          ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)[1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperRowCellAddress + 0x50 + rowLength * 0x100) & 0x20000000) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + 0x4c + rowLength * 0x100);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + 0x4c + rowLength * 0x100) +
                               *(int *)(upperRowCellAddress + 0x48 + rowLength * 0x100)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperRowCellAddress + rowLength * 0x100))[-1].flagsAndMaterial & 0x20000000) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperRowCellAddress + rowLength * 0x100))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperRowCellAddress + rowLength * 0x100))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperRowCellAddress + rowLength * 0x100))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[-1].flagsAndMaterial & 0x20000000) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[-1].terrainHeight
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[1].flagsAndMaterial & 0x20000000) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[1].terrainHeight) -
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
   Height-drag brush of FieldGrid_ApplyPositiveCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): the target height is the source cell's terrainHeight - 64 * heightDeltaUnits;
   every cell of accumulatorPlane (one height per cell) within radius = min(|64 * radiusUnits|, 0x5000) world
   units of the centre cell moves towards it by (1 + cos(pi * distance / (radius + 1))) / 2 of the difference,
   but never away from it (so overlapping brushes keep the strongest pull). The scanned box is the centre +/- 4 *
   radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessHorizontalSpan(Q12 sourceRowQ12,Q12 sourceColumnQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerRowQ12,Q12 centerColumnQ12,
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
  
  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  minColumnOrCenterY = centerColumnQ12 + spanRadiusQ12 * -4;
  minRowOrSourceHeight = centerRowQ12 + spanRadiusQ12 * -4;
  maxColumnOrCenterX = (int)(minColumnOrCenterY + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  maxRowOrColumnsLeft = (int)(minRowOrSourceHeight + Q12_FRACTION_MASK + spanRadiusQ12 * 8) >> Q12_SHIFT;
  minColumnOrCenterY = minColumnOrCenterY >> 12;
  if (minColumnOrCenterY < 0) {
    minColumnOrCenterY = 0;
  }
  minRowOrSourceHeight = minRowOrSourceHeight >> 12;
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
    minRowOrSourceHeight = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
    spanCell = fieldGrid->cells + minColumnOrCenterY;
    maxColumnOrCenterX = fieldGrid->cells[minRowOrSourceHeight].worldX;
    minColumnOrCenterY = fieldGrid->cells[minRowOrSourceHeight].worldY;
    minRowOrSourceHeight = fieldGrid->cells
            [(sourceRowQ12 >> Q12_SHIFT) * fieldGrid->gridWidth + (sourceColumnQ12 >> Q12_SHIFT)].
            terrainHeight;
    maxRowOrColumnsLeft = spanColumnCount;
    rowStartCell = spanCell;
    rowStartAccumulator = accumulatorCursor;
    do {
      do {
        cellDistance = FixedMath_Length2(spanCell->worldY - minColumnOrCenterY,spanCell->worldX - maxColumnOrCenterX);
        if (cellDistance <= spanRadiusQ12 + 1) {
          heightDifference = (minRowOrSourceHeight + heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12) - *accumulatorCursor;
          falloffProduct = (int64_t)
                  (g_FixedCosQ28
                   [(int)((int64_t)
                          ((((int64_t)(int)cellDistance & 0x1ffffffffffffU) >> 17) << 32 |
                          (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                   ] + Q28_ONE) * (int64_t)heightDifference;
          blendedHeight = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
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
        spanCell++;
        accumulatorCursor++;
        maxRowOrColumnsLeft--;
      } while (maxRowOrColumnsLeft != 0);
      accumulatorCursor = rowStartAccumulator + rowLength;
      spanCell = rowStartCell + rowLength;
      rowsRemaining--;
      maxRowOrColumnsLeft = spanColumnCount;
      rowStartCell = spanCell;
      rowStartAccumulator = accumulatorCursor;
    } while (rowsRemaining != 0);
  }
}


/* Address: 0x00571250.
   Raise/lower brush of FieldGrid_ApplyNegativeCellDeltas around one centre cell (Q12 grid row/column
   centerRowQ12/centerColumnQ12): adds (1 + cos(pi * distance / (radius + 1))) / 2 * offset, offset = -64 *
   heightDeltaUnits, to every accumulatorPlane entry within radius = min(|64 * radiusUnits|, 0x5000) world units
   of the centre cell, capping the sum at offset (so overlapping brushes do not add up beyond it). The scanned
   box is the centre +/- 2 * radius in grid Q12 coordinates, clipped to the grid.
*/
void FieldGrid_ProcessVerticalSpan(FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerRowQ12,Q12 centerColumnQ12,FieldGridAccumulatorValue *accumulatorPlane,
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
  
  spanRadiusQ12 = radiusUnits * FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  if ((int)spanRadiusQ12 < 0) {
    spanRadiusQ12 = radiusUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  }
  if (FIELD_GRID_EDIT_BRUSH_RADIUS_MAX < spanRadiusQ12) {
    spanRadiusQ12 = FIELD_GRID_EDIT_BRUSH_RADIUS_MAX;
  }
  targetOffset = heightDeltaUnits * -FIELD_GRID_EDIT_DRAG_UNIT_Q12;
  minColumnOrCenterY = centerColumnQ12 + spanRadiusQ12 * -2;
  minRowOrColumnsLeft = centerRowQ12 + spanRadiusQ12 * -2;
  maxColumnOrCenterX = (int)(minColumnOrCenterY + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  maxRowOrBlendedValue = (int)(minRowOrColumnsLeft + Q12_FRACTION_MASK + spanRadiusQ12 * 4) >> Q12_SHIFT;
  minColumnOrCenterY = minColumnOrCenterY >> 12;
  if (minColumnOrCenterY < 0) {
    minColumnOrCenterY = 0;
  }
  minRowOrColumnsLeft = minRowOrColumnsLeft >> 12;
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
    minRowOrColumnsLeft = (centerRowQ12 >> Q12_SHIFT) * rowLength + (centerColumnQ12 >> Q12_SHIFT);
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
                          ((((int64_t)(int)cellDistance & 0x1ffffffffffffU) >> 17) << 32 |
                          (int64_t)(int)cellDistance * FIXED_ANGLE16_HALF_TURN & 0xffffffffU) / (int64_t)(int)(spanRadiusQ12 + 1))
                   ] + Q28_ONE) * (int64_t)targetOffset;
          maxRowOrBlendedValue = (FIXED_PRODUCT_SHR(falloffProduct, Q28_SHIFT + 1)) + *accumulatorCursor;
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
        spanCell++;
        accumulatorCursor++;
        minRowOrColumnsLeft--;
      } while (minRowOrColumnsLeft != 0);
      accumulatorCursor = rowStartAccumulator + rowLength;
      spanCell = rowStartCell + rowLength;
      rowsRemaining--;
      minRowOrColumnsLeft = spanColumnCount;
      rowStartCell = spanCell;
      rowStartAccumulator = accumulatorCursor;
    } while (rowsRemaining != 0);
  }
}


/* Address: 0x005713E0.
   Replaces the material byte of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) with transitionValue,
   when the cell is inside the grid. Called by FieldGrid_ApplyLocalCellUpdate.
*/
void FieldGrid_ApplySingleCellTransition(FieldGridTransitionValue transitionValue,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int columnOrCellIndex;
  
  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  if ((((-1 < gridRowIndex) && (columnOrCellIndex = gridColumnQ12 >> Q12_SHIFT, -1 < columnOrCellIndex)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (columnOrCellIndex < (int)fieldGrid->gridWidth)) {
    columnOrCellIndex = gridRowIndex * fieldGrid->gridWidth + columnOrCellIndex;
    fieldGrid->cells[columnOrCellIndex].flagsAndMaterial =
         fieldGrid->cells[columnOrCellIndex].flagsAndMaterial & ~FIELD_CELL_MATERIAL_ID_MASK | transitionValue;
  }
}


/* Address: 0x00571860.
   Smooths one cell: sets the terrain height of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to the
   average of its six neighbours (the water level stays); only rows 2..height-2 and columns 2..width-2 are
   touched. Called by FieldGrid_RebuildLocalInfluenceState.
*/
void FieldGrid_ApplyRectangularTransition(Q12 gridRowQ12,Q12 gridColumnQ12,FieldGridAsset *fieldGrid)

{
  Q12 *heightField;
  int rowOrHeightDelta;
  int columnOrCellIndex;
  FieldGridDimension gridWidth;
  
  gridWidth = fieldGrid->gridWidth;
  rowOrHeightDelta = gridRowQ12 >> Q12_SHIFT;
  columnOrCellIndex = gridColumnQ12 >> Q12_SHIFT;
  if ((((1 < rowOrHeightDelta) && (1 < columnOrCellIndex)) && (rowOrHeightDelta + 1 < (int)fieldGrid->gridHeight)) &&
     (columnOrCellIndex + 1 < (int)gridWidth)) {
    /* index of the neighbour above; the six terms are the neighbours above, above right, left, right, below
       left and below, the centre is cells[index + gridWidth] */
    columnOrCellIndex = (rowOrHeightDelta - 1) * gridWidth + columnOrCellIndex;
    rowOrHeightDelta = (fieldGrid->cells[columnOrCellIndex].terrainHeight +
             fieldGrid->cells[columnOrCellIndex + 1].terrainHeight +
             fieldGrid->cells[columnOrCellIndex + (gridWidth - 1)].terrainHeight +
             fieldGrid->cells[columnOrCellIndex + gridWidth + 1].terrainHeight +
             fieldGrid->cells[columnOrCellIndex + gridWidth * 2 - 1].terrainHeight
            + fieldGrid->cells[columnOrCellIndex + gridWidth * 2].terrainHeight) / 6 -
            fieldGrid->cells[columnOrCellIndex + gridWidth].terrainHeight;
    heightField = &fieldGrid->cells[columnOrCellIndex + gridWidth].terrainHeight;
    *heightField = *heightField + rowOrHeightDelta;
    heightField = &fieldGrid->cells[columnOrCellIndex + gridWidth].waterSurfaceDelta;
    *heightField = *heightField - rowOrHeightDelta;
  }
}


/* Address: 0x004FEA50.
   Converts a world-plane position to field-grid coordinates in Q12 (integer part = cell column/row,
   fraction = position inside the cell): column in EAX, row in EDX. The triangular lattice makes the
   column shift by half a cell per row.
   Original register convention: result in EAX and EDX, CF flag; ECX preserved; uses MMX register MM0.
*/
FieldGridCoordinates FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX)

{
  uint32_t gridHalfRowCoordinateQ12;
  FieldGridCoordinates gridCoordinates;

  /* Q12 * Q20 >> 21: half the row coordinate */
  gridHalfRowCoordinateQ12 =
       FIXED_MUL_SHR(worldY, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridCoordinates.rowQ12 = gridHalfRowCoordinateQ12 * 2;
  gridCoordinates.columnQ12 =
       FIXED_MUL_SHR(worldX, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12;
  return gridCoordinates;
}


/* Address: 0x00571FE0.
   Sets the flags of the cell at Q12 grid row/column (gridRowQ12/gridColumnQ12) to (flags & preserveMask) |
   setMask, when the cell is inside the grid. Shared by FieldGrid_SetCellFluidReceiverExcluded,
   FieldGrid_SetCellFluidSourceExcluded and FieldGrid_SetCellResourceSupportFlag.
*/
void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid)

{
  int gridRowIndex;
  int columnOrCellIndex;
  
  gridRowIndex = gridRowQ12 >> Q12_SHIFT;
  if ((((-1 < gridRowIndex) && (columnOrCellIndex = gridColumnQ12 >> Q12_SHIFT, -1 < columnOrCellIndex)) &&
      (gridRowIndex < (int)fieldGrid->gridHeight)) && (columnOrCellIndex < (int)fieldGrid->gridWidth)) {
    columnOrCellIndex = gridRowIndex * fieldGrid->gridWidth + columnOrCellIndex;
    fieldGrid->cells[columnOrCellIndex].flagsAndMaterial =
         preserveMask & fieldGrid->cells[columnOrCellIndex].flagsAndMaterial | setMask;
  }
}


/* Address: 0x005052E0.
   Recomputes a cell's two vertex normals from its six lattice neighbours and stores them as packed
   (azimuth | elevation << 16) angle pairs: triangle0NormalAngles (+0x08) for the terrain surface and
   triangle1NormalAngles (+0x78) for the secondary surface (terrainHeight + waterSurfaceDelta). The lighting in
   FieldGridCell_ComputeDirectionalLightColor reads the first one.
*/
void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell)

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
  neighborDeltaA = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaB = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaC = cell[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaD = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaE = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].terrainHeight - cell->terrainHeight;
  normalAngles = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaA) - (cell[1].worldY - cell->worldY) * rightDeltaOrNegativeStride
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldY - cell->worldY)
                                     * neighborDeltaB) - (cell[-1].worldY - cell->worldY) * neighborDeltaC) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldY - cell->worldY) * neighborDeltaE
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaA) - (cell[1].worldX - cell->worldX) * rightDeltaOrNegativeStride) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldX - cell->worldX) * neighborDeltaB) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaC) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaD
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldX - cell->worldX) * neighborDeltaE);
  cell->triangle0NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
  rightDeltaOrNegativeStride = -rowStrideBytes;
  neighborDeltaA = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaB = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaC = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)->terrainHeight + FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)->waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  neighborDeltaD = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaE = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaF = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)[1].terrainHeight + FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)[1].waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  normalAngles = FixedMath_VectorToAngles3Regs
                    (0xc00000,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaB) - (cell[1].worldY - cell->worldY) * neighborDeltaA
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)->worldY - cell->worldY) * neighborDeltaC) -
                               (cell[-1].worldY - cell->worldY) * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaE) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)[1].worldY - cell->worldY) * neighborDeltaF
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaB) - (cell[1].worldX - cell->worldX) * neighborDeltaA) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)->worldX - cell->worldX) * neighborDeltaC) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaD) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaE
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rightDeltaOrNegativeStride)[1].worldX - cell->worldX) * neighborDeltaF);
  cell->triangle1NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
}


/* Address: 0x00505690.
   Diffuse terrain lighting for one cell: turns the terrain normal (triangle0NormalAngles) back into a Q28
   direction, dots it with the global light direction and looks the result up in the directional light colour
   table (groundDirectionalLightColor). The secondary surface always gets the one fixed secondary colour
   (secondarySurfaceDirectionalLightColor).
*/
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  FixedDirection normalDirection;
  PackedArgb32 directionalLightColor;

  /* packed as azimuth (low word) | elevation (high word) */
  normalDirection = FixedMath_DirectionFromAnglesQ28Regs
                    ((int)cell->triangle0NormalAngles >> 16,cell->triangle0NormalAngles & FIXED_ANGLE16_MASK);
  /* the signed Q8 dot product indexes -256..256: g_TerrainLightingColorRampArgb256 lies directly
     before this table and holds the shaded half */
  directionalLightColor =
       ((PackedArgb32 *)g_TerrainDirectionalLightColorLut)
       [(int)((uint64_t)((int64_t)(int)normalDirection.x * (int64_t)(int)g_TerrainLightDirectionX) >> 32) +
        (int)((uint64_t)((int64_t)(int)normalDirection.y * (int64_t)(int)g_TerrainLightDirectionY) >> 32) +
        (int)((uint64_t)((int64_t)(int)normalDirection.z * (int64_t)(int)g_TerrainLightDirectionZ) >> 32) >>
        16];
  cell->secondarySurfaceDirectionalLightColor = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor = directionalLightColor;
}

