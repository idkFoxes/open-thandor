/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/occupancy.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/occupancy.h>
#include <thandor/thandor.h>

/* Module data. */

static const uint64_t g_TerrainOccupancyMmxSignBiasBytes = 0x8080808080808080ull;

static const uint64_t g_TerrainOccupancyMmxClearBits1And2Mask = 0xF9F9F9F9F9F9F9F9ull;

static const uint64_t g_TerrainOccupancyMmxAllBitsMask = 0xFFFFFFFFFFFFFFFFull;

static const uint64_t g_TerrainOccupancyMmxPackedScale0280 = 0x280028002800280ull;

static const uint64_t g_TerrainOccupancyMmxPersistentWeights = 0x20000200200002ull;

static const uint64_t g_TerrainOccupancyMmxCurrentWeights = 0x40000400400004ull;

/* Implementation ownership: world/terrain/occupancy. */

/* Sets occupancy bit 1 (FIELD_CELL_OCCUPANCY_BIT1) in one faction slot's byte for every cell within the given
   radius of a world point: the centre cell here, the rest through the six hexagon sectors. Part of the occupancy
   rebuild that runs over every owned army (the radius is ArmyRuntimeSlot.occupancyMarkRadius). Nothing happens when the centre is outside
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
  FieldGridCell *wedge0Cell;
  FieldGridCell *wedge1Cell;
  FieldGridCell *wedge2Cell;
  FieldGridCell *wedge3Cell;
  FieldGridCell *wedge4Cell;
  FieldGridCell *wedge5Cell;
  FieldGridCoordinates gridCoordinates;

  if (fieldGrid == NULL) {
    return;
  }
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
  gridColumnCount = fieldGrid->gridWidth & FIELD_GRID_ROW_STRIDE_WIDTH_MASK;
  if ((int)cellColumn < 0 || (int)cellRow < 0 || cellRow >= fieldGrid->gridHeight ||
      cellColumn >= gridColumnCount) {
    return;
  }
  cellIndex = cellRow * gridColumnCount + cellColumn;
  if ((fieldGrid->cells[cellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return;
  }
  centerMaskByte = &FIELD_CELL_OCCUPANCY_BYTE(&fieldGrid->cells[cellIndex],occupancyByteOffset);
  *centerMaskByte = *centerMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
  rowStrideBytes = g_TerrainScanRowStrideBytes;
  /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width) */
  wedge0Cell = &fieldGrid->cells[cellIndex + 1];
  wedge1Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedge0Cell,-rowStrideBytes);
  wedge2Cell = wedge1Cell - 1;
  wedge3Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedge2Cell - 1,rowStrideBytes);
  wedge4Cell = FIELD_GRID_CELL_AT_BYTE_OFFSET(wedge3Cell,rowStrideBytes);
  wedge5Cell = wedge4Cell + 1;
  TerrainOccupancyBit2_MarkWedge0(0,wedge0Cell);
  TerrainOccupancyBit2_MarkWedge1(0,wedge1Cell);
  TerrainOccupancyBit2_MarkWedge2(0,wedge2Cell);
  TerrainOccupancyBit2_MarkWedge3(0,wedge3Cell);
  TerrainOccupancyBit2_MarkWedge4(0,wedge4Cell);
  TerrainOccupancyBit2_MarkWedge5(0,wedge5Cell);
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


/* ORs into mask the occupancy masks of up to stepCount cells along one ray from centerCell (cellStep cells per
   step); the ray stops after the first map-edge cell, which is still included. */
static uint64_t TerrainOccupancyMask_OrRay(uint64_t mask,const FieldGridCell *centerCell,int cellStep,int stepCount)

{
  const FieldGridCell *cell;
  int step;

  cell = centerCell;
  for (step = 0; step < stepCount; step++) {
    cell = cell + cellStep;
    mask = mask | cell->occupancyMask;
    if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
      break;
    }
  }
  return mask;
}


/* Answers "which factions are around this point": ORs the occupancy masks of the centre cell and of six straight
   rays (right, left, up-right, up, down-left, down; length from the radius, 1..255 cells, stopping at map-edge
   cells) and packs two bits per faction slot i: bit 2i+1 = a current presence bit is set, bit 2i = only the
   persistent bit 7 is (bits 1 and 2 are ignored). TerrainOccupancyMask_ResolveRuntimeClassFlags consumes the
   result. Returns 0 when the grid is missing or the point lies outside it or on a map-edge cell.
*/
uint32_t TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid)

{
  uint32_t radiusSteps;
  uint32_t gridWidth;
  uint32_t cellColumn;
  int runStepCount;
  uint32_t cellRow;
  int rowStep;
  const FieldGridCell *centerCell;
  uint64_t combinedMask;
  uint64_t currentPresenceSums;
  uint64_t persistentOnlyBytes;
  uint64_t persistentOnlySums;
  FieldGridCoordinates gridCoordinates;

  if (fieldGrid == NULL) {
    return 0;
  }
  /* cells per ray, rounded; the original divides by 0x901, not by the cell size 0x900 */
  radiusSteps = (neighborhoodRadiusQ12 + TERRAIN_OCCUPANCY_RADIUS_ROUND_Q12) / FIELD_GRID_WORLD_COLUMN_STEP_X;
  if (radiusSteps == 0) {
    runStepCount = 2;
  }
  else if (radiusSteps < 256) {
    runStepCount = radiusSteps + 1;
  }
  else {
    runStepCount = 256;
  }
  gridCoordinates = FieldGrid_WorldToGridQ12(worldYQ12,worldXQ12);
  gridWidth = fieldGrid->gridWidth;
  /* round the Q12 grid coordinates to the nearest cell */
  cellColumn = ((gridCoordinates.columnQ12 >> 11) + 1) >> 1;
  cellRow = ((gridCoordinates.rowQ12 >> 11) + 1) >> 1;
  if ((int)cellColumn < 0 || (int)cellRow < 0 || cellRow >= fieldGrid->gridHeight || cellColumn >= gridWidth) {
    return 0;
  }
  centerCell = fieldGrid->cells + cellRow * gridWidth + cellColumn;
  combinedMask = centerCell->occupancyMask;
  if ((centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return 0;
  }
  /* 1..255 cells per ray; every ray starts at the centre */
  runStepCount--;
  rowStep = (int)gridWidth;
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,1,runStepCount);            /* right */
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,-1,runStepCount);           /* left */
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,1 - rowStep,runStepCount);  /* up and right */
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,-rowStep,runStepCount);     /* up */
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,rowStep - 1,runStepCount);  /* down and left */
  combinedMask = TerrainOccupancyMask_OrRay(combinedMask,centerCell,rowStep,runStepCount);      /* down */
  /* byte classification per faction byte (0xF9 mask drops bits 1 and 2): 0x80 alone -> persistent only; any
     other non-zero value -> currently present. PAND 0x0280 and PMADDWD with the 2/0x20 and 4/0x40 weights move
     the flag of faction i to bit 2i (persistent) or 2i+1 (present), bytes 0..3 in bits 8..15 of the low dword
     and bytes 4..7 in bits 8..15 of the high dword. */
  combinedMask = combinedMask & g_TerrainOccupancyMmxClearBits1And2Mask;
  persistentOnlyBytes = TerrainOccupancy_Pcmpeqb((uint64_t)g_TerrainOccupancyMmxSignBiasBytes,combinedMask);
  persistentOnlySums =
       pmaddwd(persistentOnlyBytes & g_TerrainOccupancyMmxPackedScale0280,g_TerrainOccupancyMmxPersistentWeights);
  currentPresenceSums =
       pmaddwd((TerrainOccupancy_Pcmpeqb(0,combinedMask) ^ g_TerrainOccupancyMmxAllBitsMask ^ persistentOnlyBytes) &
               g_TerrainOccupancyMmxPackedScale0280,g_TerrainOccupancyMmxCurrentWeights);
  return ((int)(currentPresenceSums >> 32) + (int)(persistentOnlySums >> 32)) |
         ((uint32_t)((int)currentPresenceSums + (int)persistentOnlySums) >> 8);
}


/* Turns a neighbourhood classification (primaryOccupancyMask, two bits per faction slot from
   TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint) into model-node flags for the active faction:
   TERRAIN_OCCUPANCY_FLAG_PRESENT when that faction is present around the object now, otherwise
   TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE when its persistent bit is there and the object's history
   (secondaryOccupancyMask, which gains both bits of every faction present now) says it was seen. Also returns
   the updated history and the combined mask the callers store as terrain class state.
*/
TerrainOccupancyResolvedMasks
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char activeFactionIndex)

{
  FieldGridRuntimeFlags resolvedClassFlags;
  uint32_t combinedOccupancyMask;
  uint32_t factionSeenBit;
  TerrainOccupancyResolvedMasks resolvedMasks;

  /* every faction present now (odd bit) sets both of its bits in the history */
  resolvedMasks.secondaryOccupancyMask =
       secondaryOccupancyMask | ((primaryOccupancyMask & TERRAIN_OCCUPANCY_CLASS_PRESENT_BITS) >> 1) * 3;
  factionSeenBit = 1 << (activeFactionIndex * 2 & 31U);
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


/* Sets occupancy bit 1 in the scan's faction byte for every cell of the 60-degree sector between directions 0
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


/* Sets occupancy bit 1 for the sector between directions 1 (C+1-W) and 2 (C-W), built like
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


/* Sets occupancy bit 1 for the sector between directions 2 (C-W) and 3 (C-1), built like
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


/* Sets occupancy bit 1 for the sector between directions 3 (C-1) and 4 (C-1+W), built like
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


/* Sets occupancy bit 1 for the sector between directions 4 (C-1+W) and 5 (C+W), built like
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


/* Sets occupancy bit 1 for the sector between directions 5 (C+W) and 0 (C+1), built like
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


/* Straight leg of the occupancy scan along direction 0 (C+1, right): sets occupancy bit 1 in the scan's faction
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


/* Straight leg of the occupancy scan along direction 1 (C+1-W, up and right): sets occupancy bit 1 in the scan's
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


/* Straight leg of the occupancy scan along direction 2 (C-W, up): sets occupancy bit 1 in the scan's faction
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


/* Straight leg of the occupancy scan along direction 3 (C-1, left): sets occupancy bit 1 in the scan's faction
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


/* Straight leg of the occupancy scan along direction 4 (C-1+W, down and left): sets occupancy bit 1 in the
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


/* Straight leg of the occupancy scan along direction 5 (C+W, down): sets occupancy bit 1 in the scan's faction
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

