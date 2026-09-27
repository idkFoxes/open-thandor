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
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
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
      centerMaskByte =
           fieldGrid->cells[cellIndex].runtime60_6B + occupancyByteOffset + FIELD_CELL_RUNTIME60_INDEX_OCCUPANCY_MASK;
      *centerMaskByte = *centerMaskByte | FIELD_CELL_OCCUPANCY_BIT1;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      /* the six neighbours of centre cell C, one per sector: C+1, C+1-W, C-W, C-1, C-1+W, C+W (W = grid width;
         the first address is cells[cellIndex + 1], and runtime0C_3F - 0xC is a cell's own address) */
      wedge0Or3Cell = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               cellIndex * 0x80 - 0x28);
      wedge1Or4Cell = (FieldGridCell *)((int)wedge0Or3Cell - g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkWedge0(0,wedge0Or3Cell);
      cell = wedge1Or4Cell - 1;
      TerrainOccupancyBit2_MarkWedge1(0,wedge1Or4Cell);
      wedge0Or3Cell = (FieldGridCell *)(cell[-1].runtime0C_3F + rowStrideBytes - 0xc);
      TerrainOccupancyBit2_MarkWedge2(0,cell);
      wedge1Or4Cell = (FieldGridCell *)(wedge0Or3Cell->runtime0C_3F + rowStrideBytes - 0xc);
      TerrainOccupancyBit2_MarkWedge3(0,wedge0Or3Cell);
      TerrainOccupancyBit2_MarkWedge4(0,wedge1Or4Cell);
      TerrainOccupancyBit2_MarkWedge5(0,wedge1Or4Cell + 1);
    }
  }
}


/* PCMPEQB: 0xFF in every byte lane where a and b are equal, 0 elsewhere. */
static __inline uint64_t TerrainOccupancy_Pcmpeqb(uint64_t a,uint64_t b)

{
  ThandorMmx x;
  ThandorMmx y;
  ThandorMmx r;
  int lane;

  x.q = a;
  y.q = b;
  for (lane = 0; lane < 8; lane = lane + 1) {
    r.ub[lane] = (x.ub[lane] == y.ub[lane]) ? 0xff : 0;
  }
  return r.q;
}


/* Address: 0x00507610.
   Answers "which factions are around this point": ORs the occupancy masks of the centre cell and of six straight
   rays (right, left, up-right, up, down-left, down; length from the radius, 1..255 cells, stopping at map-edge
   cells) and packs two bits per faction slot i: bit 2i+1 = a current presence bit is set, bit 2i = only the
   persistent bit 7 is (bits 1 and 2 are ignored). TerrainOccupancyMask_ResolveRuntimeClassFlags consumes the
   result. Returns 0 (in EDX) when the grid is missing or the point lies outside it or on a map-edge cell.
*/
uint32_t __thandor_void_preserve_eax_ecx
TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
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
          do { /* right: +0x70 occupancyMask, +0x50 flags of the next cell */
            combinedMask = combinedMask | *(uint64_t *)((int)(runCursorA + 1) + 0x70);
            runCursorB = centerCell;
            runRemainingB = runStepCount;
            if ((*(uint32_t *)((int)(runCursorA + 1) + 0x50) & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
            runRemainingA--;
            runCursorA = runCursorA + 1;
          } while (runRemainingA != 0);
          do { /* left */
            combinedMask = combinedMask | *(uint64_t *)((int)(runCursorB - 1) + 0x70);
            runCursorA = centerCell;
            runRemainingA = runStepCount;
            if ((*(uint32_t *)((int)(runCursorB - 1) + 0x50) & FIELD_CELL_GRID_EDGE_MASK) != 0) break;
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
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 0, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection0, TerrainOccupancyBit2_MarkDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *diagonalMaskByte;
  FieldGridCell *legStartCell;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      legStartCell = cell + 1;
      TerrainOccupancyBit2_MarkDirection0(scanStep + 4,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint32_t *)((int)legStartCell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      diagonalMaskByte = (uint8_t *)((int)legStartCell +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + (0x70 - rowStrideBytes));
      *diagonalMaskByte = *diagonalMaskByte | 2;
      cell = (FieldGridCell *)((int)legStartCell + (0x80 - rowStrideBytes));
      scanStep = scanStep + 7;
      TerrainOccupancyBit2_MarkDirection1
                (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507140.
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 1, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection1, TerrainOccupancyBit2_MarkDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *diagonalMaskByte;
  FieldGridCell *legStartCell;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      TerrainOccupancyBit2_MarkDirection1
                (scanStep + 4,(FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint32_t *)((int)cell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      diagonalMaskByte = (uint8_t *)((int)cell +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + (0x70 - rowStrideBytes));
      *diagonalMaskByte = *diagonalMaskByte | 2;
      legStartCell = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
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
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 2, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection2, TerrainOccupancyBit2_MarkDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      TerrainOccupancyBit2_MarkDirection2
                (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime0C_3F[occupancyMarkByteIndex.occupancyMaskByteIndex + -0x1c] =
           cell->runtime0C_3F[occupancyMarkByteIndex.occupancyMaskByteIndex + -0x1c] | 2;
      legStartCell = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainOccupancyBit2_MarkDirection3(scanStep,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507280.
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 3, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection3, TerrainOccupancyBit2_MarkDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *centerMaskByte;
  int currentRowStrideBytes;
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  int rowStrideBytes;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      centerMaskByte = (uint8_t *)((int)cell->runtime60_6B +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10);
      *centerMaskByte = *centerMaskByte | 2;
      currentRowStrideBytes = g_TerrainScanRowStrideBytes;
      legStartCell = cell + -1;
      TerrainOccupancyBit2_MarkDirection3(scanStep + 4,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint32_t *)(legStartCell->runtime60_6B + currentRowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      legStartCell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + currentRowStrideBytes + 0x10] =
           legStartCell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + currentRowStrideBytes + 0x10] | 2;
      cell = (FieldGridCell *)(legStartCell[-1].runtime0C_3F + currentRowStrideBytes + -0xc);
      scanStep = scanStep + 7;
      TerrainOccupancyBit2_MarkDirection4
                (scanStep,(FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
                );
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00507320.
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 4, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection4, TerrainOccupancyBit2_MarkDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint8_t *cellRuntimeBytes;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      TerrainOccupancyBit2_MarkDirection4
                (scanStep + 4,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint32_t *)(cell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + rowStrideBytes + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + rowStrideBytes + 0x10]
           | 2;
      cellRuntimeBytes = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cellRuntimeBytes + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) + -1;
      TerrainOccupancyBit2_MarkDirection5
                (scanStep,(FieldGridCell *)
                          (cellRuntimeBytes + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x005073C0.
   Ownership: world/terrain/occupancy.
   Purpose: Marks the two adjacent directional legs of terrain wedge 5, stopping at excluded cells and forwarding
   the shared occupancy-byte selector across both legs. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainOccupancyBit2_MarkDirection5, TerrainOccupancyBit2_MarkDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *legStartCell;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      TerrainOccupancyBit2_MarkDirection5
                (scanStep + 4,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell[1].runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell[1].runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      legStartCell = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      TerrainOccupancyBit2_MarkDirection0(scanStep,legStartCell);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00506EA0.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 0 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = cell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506EF0.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 1 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506F50.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 2 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506FA0.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 3 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = cell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00506FF0.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 4 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00507050.
   Ownership: world/terrain/occupancy.
   Purpose: Walks directional terrain run 5 until the radius or an excluded cell is reached and sets bit 1 in the
   selected byte of the cell occupancy mask. Typed parameters: p2 scanStep→TerrainDirectionalScanStep_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

