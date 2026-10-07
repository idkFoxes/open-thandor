/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/water_relaxation.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Water relaxation: directional passes that pull the water surface of each cell's six neighbours toward the
   cell's own. The sign-gated pair is simulation (tick-wheel cases 1 and 5, gameplay/session/tick.cpp); command
   0x3200 (the editor's smoothing page) runs either pair in bulk. */

#include <thandor/world/terrain/water_relaxation.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* In-game command INGAME_COMMAND_TERRAIN_RELAXATION (0x3200, handler at INGAME_COMMAND_CODE_BASE + code):
   runs passCount pairs of forward and reverse water relaxation sweeps over the active field grid, the
   sign-gated pair or (mode bit 0 set) the ungated land-tool pair. Queued or called directly by
   InGameCommandRange_DispatchState0/1 (ui/ingame/editor_tool_selection.cpp) with 0x80 passes. passCount must not be 0.
*/
void TerrainGrid_RunDirectionalRelaxationPasses(FrontendPlayerRuntimeId playerRuntimeId,uint32_t reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode)

{
  FieldGridAsset *fieldGrid;
  
  fieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  /* only bit 0 of the mode selects (a received payload may carry other bits) */
  if ((static_cast<int>(mode) & static_cast<int>(TERRAIN_RELAXATION_UNGATED_LAND_TOOL)) == 0) {
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

/* Pulls the water surface (terrainHeight + waterSurfaceDelta) of receiver 1/8 of the way toward
   sourceSurfaceHeightQ12, unless the receiver is FIELD_CELL_FLUID_RECEIVER_EXCLUDED. */
static inline void FieldGridCell_PullWaterSurface(FieldGridCell *receiver,int sourceSurfaceHeightQ12)

{
  if ((receiver->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) == 0) {
    receiver->waterSurfaceDelta =
         receiver->waterSurfaceDelta -
         ((receiver->waterSurfaceDelta + receiver->terrainHeight) - sourceSurfaceHeightQ12 >> 3);
  }
}

/* One source cell: unless it is FIELD_CELL_FLUID_SOURCE_EXCLUDED (or, SignGated, has negative water), pulls its
   six hexagonal neighbours toward its own water surface, in the original's order: the row above (same column,
   column + 1), the row below (same column, column - 1), then the left and the right cell. rowStep is the row
   length in cells. */
template <bool SignGated>
static inline void FieldGridCell_RelaxNeighbors(FieldGridCell *source,ptrdiff_t rowStep)

{
  if (SignGated && source->waterSurfaceDelta < 0) {
    return;
  }
  if ((source->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) != 0) {
    return;
  }
  const int sourceSurfaceHeightQ12 = source->waterSurfaceDelta + source->terrainHeight;
  FieldGridCell_PullWaterSurface(source - rowStep,sourceSurfaceHeightQ12);
  FieldGridCell_PullWaterSurface(source + (1 - rowStep),sourceSurfaceHeightQ12);
  FieldGridCell_PullWaterSurface(source + rowStep,sourceSurfaceHeightQ12);
  FieldGridCell_PullWaterSurface(source + (rowStep - 1),sourceSurfaceHeightQ12);
  FieldGridCell_PullWaterSurface(source - 1,sourceSurfaceHeightQ12);
  FieldGridCell_PullWaterSurface(source + 1,sourceSurfaceHeightQ12);
}

/* Relaxes around every interior cell (rows 1 .. gridHeight - 2, columns 1 .. gridWidth - 2) as a source: forward
   row by row from the top-left, or (Reverse) backwards from the bottom-right. Each source sees the neighbour
   values its predecessors in the scan left. Original quirk: both counts are do/while counters, so a grid with
   fewer than 3 rows or columns runs 2^32 times (kept as in the original; step 11). */
template <bool SignGated,bool Reverse>
static void TerrainGrid_RelaxInteriorCells(FieldGridAsset *fieldGrid)

{
  const FieldGridDimension rowLength = fieldGrid->gridWidth;
  const ptrdiff_t rowStep = rowLength;
  const ptrdiff_t cellStep = Reverse ? -1 : 1;
  int rowsRemaining = fieldGrid->gridHeight - 2;
  /* the cell before the first source of a row: column 0 of row 1, or (Reverse) column gridWidth - 1 of row
     gridHeight - 2 */
  FieldGridCell *rowStartCell =
       Reverse ? fieldGrid->cells + (rowLength * fieldGrid->gridHeight - 1) - rowLength : fieldGrid->cells + rowLength;
  FieldGridCell *sourceCell;

  do {
    int columnsRemaining = rowLength - 2;
    sourceCell = rowStartCell;
    do {
      sourceCell += cellStep;
      FieldGridCell_RelaxNeighbors<SignGated>(sourceCell,rowStep);
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowStartCell = sourceCell + 2 * cellStep; /* past the border cells of this row and the next */
    rowsRemaining--;
  } while (rowsRemaining != 0);
}

/* Water flow pass A (tick-wheel case 1): scans the interior cells row by row and pulls the water surface
   (terrainHeight + waterSurfaceDelta) of the six hexagonal neighbours 1/8 of the way toward the source
   cell's surface. Sources with negative water or FIELD_CELL_FLUID_SOURCE_EXCLUDED are skipped, receivers
   with FIELD_CELL_FLUID_RECEIVER_EXCLUDED are left alone. The source is the centre cell itself (its
   waterSurfaceDelta); verified against the original machine code by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid)

{
  TerrainGrid_RelaxInteriorCells<true,false>(fieldGrid);
}

/* Water flow pass B (tick-wheel case 5): the same neighbour relaxation as pass A, scanning the interior
   cells backwards from the bottom-right, so water spreads evenly in both directions over two ticks.
   Verified by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid)

{
  TerrainGrid_RelaxInteriorCells<true,true>(fieldGrid);
}

/* Water relaxation, forward, without the sign gate: like pass A it pulls the water surface of the six
   neighbours of every interior cell 1/8 of the way toward that cell's surface, but cells with negative water
   are sources too. FIELD_CELL_FLUID_SOURCE_EXCLUDED and FIELD_CELL_FLUID_RECEIVER_EXCLUDED are honoured. Run by
   TerrainGrid_RunDirectionalRelaxationPasses when mode bit 0 is set (the editor's land tool); verified against
   the original by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid)

{
  TerrainGrid_RelaxInteriorCells<false,false>(fieldGrid);
}

/* Water relaxation, backward, without the sign gate: TerrainGrid_RelaxNeighborHeightsForward scanning the
   interior cells from the bottom-right, the partner pass of the land tool in
   TerrainGrid_RunDirectionalRelaxationPasses. Verified by the former relaxcmp self-test.
*/
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid)

{
  TerrainGrid_RelaxInteriorCells<false,true>(fieldGrid);
}
