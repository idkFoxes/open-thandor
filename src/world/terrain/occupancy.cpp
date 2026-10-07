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

static const uint64_t g_FieldGridOccupancyMmxHighBitMask = 0x8080808080808080ull;

/* Sets occupancy bit 1 (FIELD_CELL_OCCUPANCY_BIT1) in one faction slot's byte for every cell within the given
   radius of a world point: the centre cell here, the rest through the six hexagon sectors of hex_scan.h (formerly
   12 functions TerrainOccupancyBit2_MarkWedge0..5 and _MarkDirection0..5). Part of the occupancy rebuild that runs
   over every owned army (the radius is ArmyRuntimeSlot.occupancyMarkRadius). Nothing happens when the centre is
   outside the grid or on a map-edge cell.
   Original quirk: the centre is the cell the point lies in (grid coordinates rounded down), not the nearest grid
   vertex the other radius scans use; and despite the "Bit2" in the name the bit set is FIELD_CELL_OCCUPANCY_BIT1.
*/
void TerrainOccupancyBit2_MarkAroundWorldPoint(FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridOccupancyByteIndex occupancyByteOffset,FieldGridAsset *fieldGrid)

{
  uint8_t *centerMaskByte;
  uint32_t cellColumn;
  uint32_t gridColumnCount;
  uint32_t cellRow;
  int cellIndex;
  FieldGridOccupancyByteIndex markByteIndex;
  FieldGridCoordinates gridCoordinates;

  if (fieldGrid == nullptr) {
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
  if (Any(fieldGrid->cells[cellIndex].flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
    return;
  }
  centerMaskByte = &FieldGridCell_OccupancyByte(&fieldGrid->cells[cellIndex],occupancyByteOffset);
  *centerMaskByte = *centerMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
  /* Original quirk: each of the former walkers copied the byte index once from the shared selector union at its
     entry instead of taking it as an argument; it is read back from there once here. */
  markByteIndex = g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  TerrainHexScan_AllSectors(&fieldGrid->cells[cellIndex],
                            TerrainHexScan_MarkPolicy([markByteIndex](FieldGridCell *fieldCell) {
                              FieldGridCell_OccupancyByte(fieldCell,markByteIndex) |= FIELD_CELL_OCCUPANCY_BIT1;
                            }));
}


/* C model of the MMX PCMPEQB instruction: 0xFF in every byte lane where a and b are equal, 0 elsewhere. */
static inline uint64_t TerrainOccupancy_Pcmpeqb(uint64_t a,uint64_t b)

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
    if (Any(cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
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

  if (fieldGrid == nullptr) {
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
  if (Any(centerCell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK)) {
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
          (uint32_t baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char activeFactionIndex)

{
  uint32_t resolvedClassFlags;
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


/* Clears the rebuilt bits 0..6 of every faction byte of every cell's occupancyMask, keeping bit 7. Head of
   tick-wheel case 7, before the per-class occupancy-rebuild callbacks repopulate the mask.
   The original (MMX) handles eight cells per step and then exactly three more per row, i.e. it assumes a row
   width of 8k + 3 (k >= 1); bounded here because the width comes from the level data: a width below 8
   underflowed the block counter (about 2^32 blocks), any other width wrote past the cells or left some
   uncleared. For 8k + 3 the original visits row by row k blocks of 8 cells and then the 3 cells right after
   them, and the next row starts right behind those (blockBase + 11), i.e. exactly the width * height cells in
   address order, each masked once with the same value - which is what this plain loop does.
*/
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid)

{
  uint32_t cellsRemaining;
  uint64_t occupancyHighBitMask;
  FieldGridCell *currentCell;

  /* FIELD_CELL_OCCUPANCY_PERSISTENT_BIT in all eight bytes (0x8080808080808080) */
  occupancyHighBitMask = g_FieldGridOccupancyMmxHighBitMask;
  currentCell = fieldGrid->cells;
  for (cellsRemaining = fieldGrid->gridWidth * fieldGrid->gridHeight; cellsRemaining != 0; cellsRemaining--) {
    currentCell->occupancyMask = currentCell->occupancyMask & occupancyHighBitMask;
    currentCell++;
  }
}

/* Sets FIELD_CELL_OCCUPANCY_BIT0 in one faction's occupancy byte of every cell. Tick-wheel case 7 calls it
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

  /* The original's do-while loops run 2^32 times for a 0 width or height; for loops here (the same cells
     otherwise). */
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
      FieldGridCell_OccupancyByte(currentCell,occupancyMaskByteIndex) =
           FieldGridCell_OccupancyByte(currentCell,occupancyMaskByteIndex) |
           FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
    }
  }
}

/* Counterpart of FieldGrid_SetOccupancyMaskByteBit0AllCells: clears FIELD_CELL_OCCUPANCY_BIT0 in one
   faction's occupancy byte of every cell.
*/
void FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension columnsRemaining;
  FieldGridDimension rowsRemaining;
  FieldGridCell *currentCell;
  FieldGridDimension gridWidth;

  /* for loops instead of the original's do-while (0 width or height: no pass instead of 2^32) */
  gridWidth = fieldGrid->gridWidth;
  currentCell = fieldGrid->cells;
  for (rowsRemaining = fieldGrid->gridHeight; rowsRemaining != 0; rowsRemaining--) {
    for (columnsRemaining = gridWidth; columnsRemaining != 0; columnsRemaining--) {
      FieldGridCell_OccupancyByte(currentCell,occupancyMaskByteIndex) =
           FieldGridCell_OccupancyByte(currentCell,occupancyMaskByteIndex) &
           (uint8_t)~FIELD_CELL_OCCUPANCY_BIT0;
      currentCell++;
    }
  }
}

/* Rounds a world point to the nearest field-grid cell and tests occupancy bits 0/1 of the active faction there.
   Returns false when one of them is set, true when the point is outside the grid or neither bit is
   set. Unit, shot and effect code play positioned sounds only when this returns false.
*/
Bool8 TerrainGrid_TestProjectedCellMaskBits01(Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  FieldGridAsset *activeFieldGrid;
  int gridColumnIndex;
  uint32_t gridHalfRowCoordinateQ12;
  int gridRowIndex;
  uint8_t occupancyByte;

  activeFieldGrid = worldRuntime->fieldGrid;
  /* The original has no field grid check here; added like the siblings (no grid: treated as outside) */
  if (activeFieldGrid == nullptr) {
    return true;
  }
  /* FieldGrid_WorldToGridQ12 inlined, then rounded (+0x800 = half a cell) to whole cells */
  gridHalfRowCoordinateQ12 = FIXED_MUL_SHR(worldYQ12, FIELD_GRID_WORLD_Y_TO_ROW_Q20, Q20_SHIFT + 1);
  gridColumnIndex = (int)((FIXED_MUL_SHR(worldXQ12, FIELD_GRID_WORLD_X_TO_COLUMN_Q20, Q20_SHIFT) - gridHalfRowCoordinateQ12) +
                 FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  gridRowIndex = (int)(gridHalfRowCoordinateQ12 * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((gridColumnIndex < 0) || (gridRowIndex < 0) || ((int)activeFieldGrid->gridWidth <= gridColumnIndex) ||
      ((int)activeFieldGrid->gridHeight <= gridRowIndex)) {
    return true;
  }
  occupancyByte =
       FieldGridCell_OccupancyByte(&activeFieldGrid->cells[(int32_t)(activeFieldGrid->gridWidth * gridRowIndex + gridColumnIndex)],
                                   worldRuntime->activeFactionRuntimeIndex);
  return (occupancyByte & FIELD_CELL_OCCUPANCY_BITS01) == 0;
}

