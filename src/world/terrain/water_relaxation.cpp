/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/water_relaxation.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Editor water relaxation (command 0x3200): directional passes that relax neighbour heights. */

#include <thandor/world/terrain/water_relaxation.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* In-game command INGAME_COMMAND_TERRAIN_RELAXATION (0x3200, handler at INGAME_COMMAND_CODE_BASE + code):
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

/* Water flow pass A (tick-wheel case 1): scans the interior cells row by row and pulls the water surface
   (terrainHeight + waterSurfaceDelta) of the six hexagonal neighbours 1/8 of the way toward the source
   cell's surface. Sources with negative water or FIELD_CELL_FLUID_SOURCE_EXCLUDED are skipped, receivers
   with FIELD_CELL_FLUID_RECEIVER_EXCLUDED are left alone.
   The source is the centre cell itself (its waterSurfaceDelta); verified against the original
   machine code by the former relaxcmp self-test.
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
        if ((centerCell[(int32_t)(-gridWidth)].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          centerCell[(int32_t)(-gridWidth)].waterSurfaceDelta =
               centerCell[(int32_t)(-gridWidth)].waterSurfaceDelta -
               ((centerCell[(int32_t)(-gridWidth)].waterSurfaceDelta +
                centerCell[(int32_t)(-gridWidth)].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((centerCell[(int32_t)(1 - gridWidth)].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          centerCell[(int32_t)(1 - gridWidth)].waterSurfaceDelta =
               centerCell[(int32_t)(1 - gridWidth)].waterSurfaceDelta -
               ((centerCell[(int32_t)(1 - gridWidth)].waterSurfaceDelta +
                centerCell[(int32_t)(1 - gridWidth)].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
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

/* Water flow pass B (tick-wheel case 5): the same neighbour relaxation as pass A, scanning the interior
   cells backwards from the bottom-right, so water spreads evenly in both directions over two ticks.
   Cells are addressed by raw byte offsets (cell size 0x80; +0x48 terrainHeight, +0x4C waterSurfaceDelta,
   +0x50 flagsAndMaterial; +/-0x80 is the next/previous cell).
   The source is the centre cell itself; verified by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  intptr_t rowStartCellAddress;
  intptr_t centerCellAddress;
  intptr_t upperCellAddress;
  int *neighborWaterDelta;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  rowStartCellAddress =
       (intptr_t)fieldGrid - (intptr_t)rowLength * (intptr_t)sizeof(FieldGridCell) +
       (rowLength * fieldGrid->gridHeight + -1) * sizeof(FieldGridCell) + offsetof(FieldGridAsset,cells);
  do {
    columnsRemaining = rowLength - 2;
    centerCellAddress = rowStartCellAddress;
    do {
      centerCellAddress = centerCellAddress - (int)sizeof(FieldGridCell);
      if ((-1 < ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta) &&
         (((uint32_t)((FieldGridCell *)centerCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0)) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)centerCellAddress)->waterSurfaceDelta + ((FieldGridCell *)centerCellAddress)->terrainHeight;
        upperCellAddress = centerCellAddress - (intptr_t)rowLength * (intptr_t)sizeof(FieldGridCell);
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
        if ((*(uint32_t *)(upperCellAddress + offsetof(FieldGridCell,flagsAndMaterial) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) +
                               *(int *)(upperCellAddress + offsetof(FieldGridCell,terrainHeight) + rowLength * FIELD_GRID_TWO_CELLS_BYTES)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
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
    rowStartCellAddress = centerCellAddress + -(int)FIELD_GRID_TWO_CELLS_BYTES;
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return;
}

/* Water relaxation, forward, without the sign gate: like pass A
   (TerrainGrid_RelaxNeighborHeightsForwardWithSignGate) it pulls the water surface of the six neighbours of
   every interior cell 1/8 of the way toward that cell's surface, but cells with negative water are sources too.
   FIELD_CELL_FLUID_SOURCE_EXCLUDED and FIELD_CELL_FLUID_RECEIVER_EXCLUDED are honoured. Run by
   TerrainGrid_RunDirectionalRelaxationPasses when mode bit 0 is set (the editor's land tool); verified against
   the original by the former relaxcmp self-test. (cellBeforeSource is advanced at the top of the inner loop, so
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
        if ((cellBeforeSource[(int32_t)(-gridWidth)].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) ==
            0) {
          cellBeforeSource[(int32_t)(-gridWidth)].waterSurfaceDelta =
               cellBeforeSource[(int32_t)(-gridWidth)].waterSurfaceDelta -
               ((cellBeforeSource[(int32_t)(-gridWidth)].waterSurfaceDelta +
                cellBeforeSource[(int32_t)(-gridWidth)].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((cellBeforeSource[(int32_t)(1 - gridWidth)].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED)
            == 0) {
          cellBeforeSource[(int32_t)(1 - gridWidth)].waterSurfaceDelta =
               cellBeforeSource[(int32_t)(1 - gridWidth)].waterSurfaceDelta -
               ((cellBeforeSource[(int32_t)(1 - gridWidth)].waterSurfaceDelta +
                cellBeforeSource[(int32_t)(1 - gridWidth)].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
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

/* Water relaxation, backward, without the sign gate: TerrainGrid_RelaxNeighborHeightsForward scanning the
   interior cells from the bottom-right, the partner pass of the land tool in
   TerrainGrid_RunDirectionalRelaxationPasses. Cells are addressed by raw byte offsets (cell size 0x80; +0x48
   terrainHeight, +0x4C waterSurfaceDelta, +0x50 flagsAndMaterial; 0x40000000 = FIELD_CELL_FLUID_SOURCE_EXCLUDED,
   0x20000000 = FIELD_CELL_FLUID_RECEIVER_EXCLUDED). Verified by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

{
  int *lowerNeighborWaterDelta;
  FieldGridDimension rowLength;
  int columnsRemaining;
  int rowsRemaining;
  int sourceSurfaceHeightQ12;
  intptr_t sourceCellAddress;
  intptr_t cellAfterSourceAddress;
  intptr_t upperRowCellAddress;
  int *neighborWaterDelta;
  FieldGridDimension gridWidth;
  
  rowLength = fieldGrid->gridWidth;
  rowsRemaining = fieldGrid->gridHeight - 2;
  sourceCellAddress =
       (intptr_t)fieldGrid - (intptr_t)rowLength * (intptr_t)sizeof(FieldGridCell) +
       (rowLength * fieldGrid->gridHeight + -1) * sizeof(FieldGridCell) + offsetof(FieldGridAsset,cells);
  do {
    columnsRemaining = rowLength - 2;
    cellAfterSourceAddress = sourceCellAddress;
    do {
      cellAfterSourceAddress = cellAfterSourceAddress + -(int)sizeof(FieldGridCell);
      if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) == 0) {
        sourceSurfaceHeightQ12 =
             ((FieldGridCell *)cellAfterSourceAddress)->waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)->terrainHeight;
        upperRowCellAddress = cellAfterSourceAddress - (intptr_t)rowLength * (intptr_t)sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          neighborWaterDelta = &((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta;
          *neighborWaterDelta =
               *neighborWaterDelta -
               ((((FieldGridCell *)upperRowCellAddress)->waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)upperRowCellAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)upperRowCellAddress)[1].waterSurfaceDelta + ((FieldGridCell *)upperRowCellAddress)[1].terrainHeight) - sourceSurfaceHeightQ12 >> 3);
        }
        if ((*(uint32_t *)(upperRowCellAddress + offsetof(FieldGridCell,flagsAndMaterial) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = (int *)(upperRowCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES);
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((*(int *)(upperRowCellAddress + offsetof(FieldGridCell,waterSurfaceDelta) + rowLength * FIELD_GRID_TWO_CELLS_BYTES) +
                               *(int *)(upperRowCellAddress + offsetof(FieldGridCell,terrainHeight) + rowLength * FIELD_GRID_TWO_CELLS_BYTES)) - sourceSurfaceHeightQ12 >> 3
                              );
        }
        if ((((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          lowerNeighborWaterDelta = &((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta;
          *lowerNeighborWaterDelta = *lowerNeighborWaterDelta - ((((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].waterSurfaceDelta +
                               ((FieldGridCell *)(upperRowCellAddress + rowLength * FIELD_GRID_TWO_CELLS_BYTES))[-1].terrainHeight) - sourceSurfaceHeightQ12 >>
                              3);
        }
        cellAfterSourceAddress = upperRowCellAddress + rowLength * sizeof(FieldGridCell);
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[-1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[-1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[-1].terrainHeight
                ) - sourceSurfaceHeightQ12 >> 3);
        }
        if (((uint32_t)((FieldGridCell *)cellAfterSourceAddress)[1].flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
          ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta =
               ((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta -
               ((((FieldGridCell *)cellAfterSourceAddress)[1].waterSurfaceDelta + ((FieldGridCell *)cellAfterSourceAddress)[1].terrainHeight) -
                sourceSurfaceHeightQ12 >> 3);
        }
      }
      columnsRemaining = columnsRemaining + -1;
      cellAfterSourceAddress = cellAfterSourceAddress;
    } while (columnsRemaining != 0);
    sourceCellAddress = cellAfterSourceAddress + -(int)FIELD_GRID_TWO_CELLS_BYTES;
    rowsRemaining = rowsRemaining + -1;
  } while (rowsRemaining != 0);
  return;
}
