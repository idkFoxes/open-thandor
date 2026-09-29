/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/occupancy.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/occupancy.h>
#include <thandor/thandor.h>

/* Implementation ownership: world/terrain/occupancy. */

/* Address: 0x00507460.
   Sets occupancy bit 1 (FIELD_CELL_OCCUPANCY_BIT1) in one faction slot's byte for every cell within the given
   radius of a world point: the centre cell here, the rest through the six hexagon sectors. Part of the occupancy
   rebuild that runs over every owned army (the army's radius at +0x90). Nothing happens when the centre is outside
   the grid or on a map-edge cell.
*/
void TerrainOccupancyBit2_MarkAroundWorldPoint(FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid)

{
  uint8_t *centerMaskByte;
  int rowStrideBytes;
  uint32_t cellColumn;
  uint32_t gridColumnCount;
  uint32_t cellRow;
  int cellIndex;
  FieldGridCell *wedge0Or3Cell;
  FieldGridCell *wedge1Or4Cell;
  FieldGridCell *cell;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;

  if (fieldGrid != NULL) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / TERRAIN_SCAN_RADIUS_PER_STEP;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (TERRAIN_SCAN_STEP_LIMIT_MAX < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = TERRAIN_SCAN_STEP_LIMIT_MAX;
    }
    g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex = occupancyByteOffset;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    cellColumn = gridCoordinates.columnQ12 >> 12;
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7; /* 0x80-byte cells */
    cellRow = gridCoordinates.rowQ12 >> 12;
    if ((((-1 < (int)cellColumn) && (gridColumnCount = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)cellRow)) &&
        (cellRow < fieldGrid->gridHeight)) &&
       ((cellColumn < gridColumnCount &&
        (cellIndex = cellRow * gridColumnCount + cellColumn,
         (fieldGrid->cells[cellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0)))) {
      centerMaskByte = &FIELD_CELL_OCCUPANCY_BYTE(&fieldGrid->cells[cellIndex],occupancyByteOffset);
      *centerMaskByte = *centerMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width;
         the first address is cells[cellIndex + 1]) */
      wedge0Or3Cell = &fieldGrid->cells[cellIndex + 1];
      wedge1Or4Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedge0Or3Cell,-g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkWedge0(0,wedge0Or3Cell);
      cell = wedge1Or4Cell - 1;
      TerrainOccupancyBit2_MarkWedge1(0,wedge1Or4Cell);
      wedge0Or3Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,rowStrideBytes);
      TerrainOccupancyBit2_MarkWedge2(0,cell);
      wedge1Or4Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedge0Or3Cell,rowStrideBytes);
      TerrainOccupancyBit2_MarkWedge3(0,wedge0Or3Cell);
      TerrainOccupancyBit2_MarkWedge4(0,wedge1Or4Cell);
      TerrainOccupancyBit2_MarkWedge5(0,wedge1Or4Cell + 1);
    }
  }
}


/* C model of the MMX PCMPEQB instruction: 0xFF in every byte lane where a and b are equal, 0 elsewhere. */
static __inline uint64_t TerrainOccupancy_Pcmpeqb(uint64_t a,uint64_t b)

{
  ThandorMmx aLanes;
  ThandorMmx bLanes;
  ThandorMmx result;
  int lane;

  aLanes.q = a;
  bLanes.q = b;
  for (lane = 0; lane < 8; lane++) {
    result.ub[lane] = (aLanes.ub[lane] == bLanes.ub[lane]) ? 0xff : 0;
  }
  return result.q;
}


/* Address: 0x00507610.
   Answers "which factions are around this point": ORs the occupancy masks of the centre cell and of six straight
   rays (right, left, up-right, up, down-left, down; length from the radius, 1..255 cells, stopping at map-edge
   cells) and packs two bits per faction slot i: bit 2i+1 = a current presence bit is set, bit 2i = only the
   persistent bit 7 is (bits 1 and 2 are ignored). TerrainOccupancyMask_ResolveRuntimeClassFlags consumes the
   result. Returns 0 (in EDX) when the grid is missing or the point lies outside it or on a map-edge cell.
*/
uint32_t TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  uint32_t radiusStepsOrGridWidth;
  uint32_t cellColumn;
  int runStepCount;
  uint32_t cellRow;
  FieldGridCell *runCursorB;
  FieldGridCell *runCursorA;
  FieldGridCell *centerCell;
  int runRemainingA;
  int runRemainingB;
  uint64_t combinedMask;
  uint64_t currentPresenceSums;
  uint64_t persistentOnlyBytes;
  uint64_t persistentOnlySums;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;

  if (fieldGrid != NULL) {
    /* cells per ray, rounded; the original divides by 0x901, not by the cell size 0x900 */
    radiusStepsOrGridWidth = (neighborhoodRadiusQ12 + 0x7ffU) / 0x901;
    if (radiusStepsOrGridWidth == 0) {
      runStepCount = 2;
    }
    else if (radiusStepsOrGridWidth < 0x100) {
      runStepCount = radiusStepsOrGridWidth + 1;
    }
    else {
      runStepCount = 0x100;
    }
    gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
    radiusStepsOrGridWidth = fieldGrid->gridWidth;
    /* round the Q12 grid coordinates to the nearest cell */
    cellColumn = (gridCoordinates.columnQ12 >> 11) + 1 >> 1;
    cellRow = (gridCoordinates.rowQ12 >> 11) + 1 >> 1;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < radiusStepsOrGridWidth)) {
      centerCell = fieldGrid->cells + cellRow * radiusStepsOrGridWidth + cellColumn;
      combinedMask = centerCell->occupancyMask;
      if ((centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
        runStepCount--;
        runCursorA = centerCell;
        runRemainingA = runStepCount;
        if (runStepCount != 0) {
          /* each ray restarts at the centre; the edge cell that stops a ray is still counted */
          do { /* right */
            combinedMask = combinedMask | runCursorA[1].occupancyMask;
            runCursorB = centerCell;
            runRemainingB = runStepCount;
            if ((runCursorA[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingA--;
            runCursorA = runCursorA + 1;
          } while (runRemainingA != 0);
          do { /* left */
            combinedMask = combinedMask | runCursorB[-1].occupancyMask;
            runCursorA = centerCell;
            runRemainingA = runStepCount;
            if ((runCursorB[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingB--;
            runCursorB = runCursorB - 1;
          } while (runRemainingB != 0);
          do { /* up and right */
            runCursorA = runCursorA + (1 - radiusStepsOrGridWidth);
            combinedMask = combinedMask | runCursorA->occupancyMask;
            runCursorB = centerCell;
            runRemainingB = runStepCount;
            if ((runCursorA->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingA--;
          } while (runRemainingA != 0);
          do { /* up */
            runCursorB = runCursorB + -radiusStepsOrGridWidth;
            combinedMask = combinedMask | runCursorB->occupancyMask;
            runCursorA = centerCell;
            runRemainingA = runStepCount;
            if ((runCursorB->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingB--;
          } while (runRemainingB != 0);
          do { /* down and left */
            runCursorA = runCursorA + (radiusStepsOrGridWidth - 1);
            combinedMask = combinedMask | runCursorA->occupancyMask;
            if ((runCursorA->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingA--;
          } while (runRemainingA != 0);
          do { /* down */
            centerCell = centerCell + radiusStepsOrGridWidth;
            combinedMask = combinedMask | centerCell->occupancyMask;
            if ((centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runStepCount--;
          } while (runStepCount != 0);
        }
        /* MMX byte classification per faction byte (0xF9 mask drops bits 1 and 2): 0x80 alone -> persistent
           only; any other non-zero value -> currently present. PAND 0x0280 and PMADDWD with the 2/0x20 and
           4/0x40 weights move the flag of faction i to bit 2i (persistent) or 2i+1 (present), bytes 0..3 in
           bits 8..15 of the low dword and bytes 4..7 in bits 8..15 of the high dword. */
        combinedMask = combinedMask & g_TerrainOccupancyMmxClearBits1And2Mask;
        persistentOnlyBytes = TerrainOccupancy_Pcmpeqb((uint64_t)g_TerrainOccupancyMmxSignBiasBytes,combinedMask);
        persistentOnlySums =
             pmaddwd(persistentOnlyBytes & g_TerrainOccupancyMmxPackedScale0280,
                     g_TerrainOccupancyMmxPackedWeights02_20);
        currentPresenceSums =
             pmaddwd((TerrainOccupancy_Pcmpeqb(0,combinedMask) ^
                      g_TerrainOccupancyMmxAllBitsMask ^ persistentOnlyBytes) &
                     g_TerrainOccupancyMmxPackedScale0280,g_TerrainOccupancyMmxPackedWeights04_40);
        return (int)((uint64_t)currentPresenceSums >> 32) + (int)((uint64_t)persistentOnlySums >> 32)
               | (uint32_t)((int)currentPresenceSums + (int)persistentOnlySums) >> 8;
      }
    }
  }
  return 0;
}


/* Address: 0x005138F0.
   Turns a neighbourhood classification (primaryOccupancyMask, two bits per faction slot from
   TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint) into model-node flags for the active faction:
   TERRAIN_OCCUPANCY_FLAG_PRESENT when that faction is present around the object now, otherwise
   TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE when its persistent bit is there and the object's history
   (secondaryOccupancyMask, which gains both bits of every faction present now) says it was seen. Also returns
   the updated history and the combined mask the callers store as terrain class state.
*/
TerrainOccupancyResolvedMasksRegs12
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char activeFactionIndex)

{
  FieldGridRuntimeFlags resolvedClassFlags;
  uint32_t combinedOccupancyMask;
  uint32_t factionSeenBit;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;

  /* every faction present now (odd bit) sets both of its bits in the history */
  resolvedMasks.secondaryOccupancyMask =
       secondaryOccupancyMask | ((primaryOccupancyMask & TERRAIN_OCCUPANCY_CLASS_PRESENT_BITS) >> 1) * 3;
  factionSeenBit = 1 << (activeFactionIndex * 2 & 0x1fU);
  combinedOccupancyMask = primaryOccupancyMask & resolvedMasks.secondaryOccupancyMask;
  resolvedClassFlags = 0;
  if ((baseRuntimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) == 0) {
    /* remembered objects: a seen-before bit also counts as present in the stored mask */
    combinedOccupancyMask =
         combinedOccupancyMask | combinedOccupancyMask * 2 & TERRAIN_OCCUPANCY_CLASS_PRESENT_BITS;
  }
  if ((factionSeenBit & combinedOccupancyMask) != 0) {
    resolvedClassFlags = TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE;
  }
  if ((primaryOccupancyMask & factionSeenBit * 2) != 0) {
    resolvedClassFlags = TERRAIN_OCCUPANCY_FLAG_PRESENT;
  }
  resolvedMasks.primaryOccupancyMask = combinedOccupancyMask;
  resolvedMasks.runtimeFlags = resolvedClassFlags;
  return resolvedMasks;
}


/* Address: 0x005070A0.
   Sets occupancy bit 1 in the scan's faction byte for every cell of the 60-degree sector between directions 0
   (C+1) and 1 (C+1-W) of TerrainOccupancyBit2_MarkAroundWorldPoint. The sector's spine steps by C+2-W (scan step
   +7); from every spine cell a straight leg runs along each bounding direction, and the sector ends at the step
   limit or at a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *diagonalMaskByte;
  FieldGridCell *legStartCell;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStartCell = cell + 1;
      TerrainOccupancyBit2_MarkDirection0(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the direction-1 neighbour C+1-W */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) !=
          0) {
        return;
      }
      diagonalMaskByte = &FIELD_CELL_OCCUPANCY_BYTE(FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell,-rowStrideBytes),
                                                    occupancyMarkByteIndex.occupancyMaskByteIndex);
      *diagonalMaskByte = *diagonalMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell + 1,-rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainOccupancyBit2_MarkDirection1(scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507140.
   Sets occupancy bit 1 for the sector between directions 1 (C+1-W) and 2 (C-W), built like
   TerrainOccupancyBit2_MarkWedge0: spine step C+1-2W (scan step +7), a straight leg along each bounding direction
   from every spine cell, ending at the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *diagonalMaskByte;
  FieldGridCell *legStartCell;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      TerrainOccupancyBit2_MarkDirection1
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the direction-2 neighbour C-W */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      diagonalMaskByte = &FIELD_CELL_OCCUPANCY_BYTE(FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes),
                                                    occupancyMarkByteIndex.occupancyMaskByteIndex);
      *diagonalMaskByte = *diagonalMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
      legStartCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes - rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = legStartCell + 1;
      TerrainOccupancyBit2_MarkDirection2(scanStep,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005071E0.
   Sets occupancy bit 1 for the sector between directions 2 (C-W) and 3 (C-1), built like
   TerrainOccupancyBit2_MarkWedge0: spine step C-1-W (scan step +7), a straight leg along each bounding direction
   from every spine cell, ending at the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      TerrainOccupancyBit2_MarkDirection2
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      /* the direction-3 neighbour C-1 */
      FIELD_CELL_OCCUPANCY_BYTE(cell - 1,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      legStartCell = cell - 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,-g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkDirection3(scanStep,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507280.
   Sets occupancy bit 1 for the sector between directions 3 (C-1) and 4 (C-1+W), built like
   TerrainOccupancyBit2_MarkWedge0: spine step C-2+W (scan step +7), a straight leg along each bounding direction
   from every spine cell, ending at the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *centerMaskByte;
  int rowStrideBytes;
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      centerMaskByte = &FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex);
      *centerMaskByte = *centerMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStartCell = cell - 1;
      TerrainOccupancyBit2_MarkDirection3(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the direction-4 neighbour C-1+W */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) !=
          0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell,rowStrideBytes),
                                occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      /* C-2+W */
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(legStartCell - 1,rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      TerrainOccupancyBit2_MarkDirection4(scanStep,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507320.
   Sets occupancy bit 1 for the sector between directions 4 (C-1+W) and 5 (C+W), built like
   TerrainOccupancyBit2_MarkWedge0: spine step C-1+2W (scan step +7), a straight leg along each bounding direction
   from every spine cell, ending at the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *twoRowsDownCell;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      TerrainOccupancyBit2_MarkDirection4
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      /* the direction-5 neighbour C+W */
      if ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes),
                                occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      twoRowsDownCell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes + rowStrideBytes);
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = twoRowsDownCell - 1;
      TerrainOccupancyBit2_MarkDirection5(scanStep,twoRowsDownCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005073C0.
   Sets occupancy bit 1 for the sector between directions 5 (C+W) and 0 (C+1), built like
   TerrainOccupancyBit2_MarkWedge0: spine step C+1+W (scan step +7), a straight leg along each bounding direction
   from every spine cell, ending at the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) == 0) {
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      TerrainOccupancyBit2_MarkDirection5
                (scanStep + TERRAIN_SCAN_STEP_STRAIGHT,FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
        return;
      }
      if ((cell[1].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell + 1,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      legStartCell = cell + 2;
      scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkDirection0(scanStep,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506EA0.
   Straight leg of the occupancy scan along direction 0 (C+1, right): sets occupancy bit 1 in the scan's faction
   byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell++;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506EF0.
   Straight leg of the occupancy scan along direction 1 (C+1-W, up and right): sets occupancy bit 1 in the scan's
   faction byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell + 1,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506F50.
   Straight leg of the occupancy scan along direction 2 (C-W, up): sets occupancy bit 1 in the scan's faction
   byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506FA0.
   Straight leg of the occupancy scan along direction 3 (C-1, left): sets occupancy bit 1 in the scan's faction
   byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell--;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506FF0.
   Straight leg of the occupancy scan along direction 4 (C-1+W, down and left): sets occupancy bit 1 in the
   scan's faction byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell - 1,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00507050.
   Straight leg of the occupancy scan along direction 5 (C+W, down): sets occupancy bit 1 in the scan's faction
   byte of each cell, 4 scan steps per cell, until the step limit or a map-edge cell.
*/
void TerrainOccupancyBit2_MarkDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;

  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
        return;
      }
      FIELD_CELL_OCCUPANCY_BYTE(cell,occupancyMarkByteIndex.occupancyMaskByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
      scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
      cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

