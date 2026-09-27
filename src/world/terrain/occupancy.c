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
   Ownership: world/terrain/occupancy.
   Purpose: Converts a world point to the field grid, bounds the directional radius, sets bit 1 in the selected
   occupancy-mask byte at the center cell, and marks all six surrounding wedges. Typed parameters: p3
   worldXQ12→Q12, p4 worldYQ12→Q12, p5 occupancyByteOffset→FieldGridOccupancyByteIndex. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p2
   radiusWorldUnits→FieldGridRadiusUnits.
   Local calls: TerrainOccupancyBit2_MarkWedge0, TerrainOccupancyBit2_MarkWedge1, TerrainOccupancyBit2_MarkWedge2,
   TerrainOccupancyBit2_MarkWedge3, TerrainOccupancyBit2_MarkWedge4, TerrainOccupancyBit2_MarkWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainOccupancyBit2_MarkAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 worldXQ12,Q12 worldYQ12,
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
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint32_t)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex = occupancyByteOffset;
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    cellColumn = gridCoordinates.columnQ12 >> 0xc;
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    cellRow = gridCoordinates.rowQ12 >> 0xc;
    if ((((-1 < (int)cellColumn) && (gridColumnCount = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)cellRow)) &&
        (cellRow < fieldGrid->gridHeight)) &&
       ((cellColumn < gridColumnCount &&
        (cellIndex = cellRow * gridColumnCount + cellColumn, (fieldGrid->cells[cellIndex].flagsAndMaterial & 0x88006000) == 0
        )))) {
      centerMaskByte = fieldGrid->cells[cellIndex].runtime60_6B + occupancyByteOffset + 0x10;
      *centerMaskByte = *centerMaskByte | 2;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      wedge0Or3Cell = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               cellIndex * 0x80 + -0x28);
      wedge1Or4Cell = (FieldGridCell *)((int)wedge0Or3Cell - g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkWedge0(0,wedge0Or3Cell);
      cell = wedge1Or4Cell + -1;
      TerrainOccupancyBit2_MarkWedge1(0,wedge1Or4Cell);
      wedge0Or3Cell = (FieldGridCell *)(cell[-1].runtime0C_3F + rowStrideBytes + -0xc);
      TerrainOccupancyBit2_MarkWedge2(0,cell);
      wedge1Or4Cell = (FieldGridCell *)(wedge0Or3Cell->runtime0C_3F + rowStrideBytes + -0xc);
      TerrainOccupancyBit2_MarkWedge3(0,wedge0Or3Cell);
      TerrainOccupancyBit2_MarkWedge4(0,wedge1Or4Cell);
      TerrainOccupancyBit2_MarkWedge5(0,wedge1Or4Cell + 1);
    }
  }
  return;
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
   Ownership: world/terrain/occupancy.
   Purpose: Collects occupancy masks from the six bounded directional runs around a world point, filters the
   combined mask through the fixed SIMD lookup constants, and returns the encoded neighborhood classification.
   Typed parameters: p1 worldXQ12→Q12, p2 worldYQ12→Q12. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
uint32_t __thandor_void_preserve_eax_ecx
TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid)

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
  uint64_t mm1PackedValue0;
  uint64_t signBiasMatchBytes;
  uint64_t mm2PackedValue0;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
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
    gridCoordinates = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    radiusStepsOrGridWidth = fieldGrid->gridWidth;
    cellColumn = (gridCoordinates.columnQ12 >> 0xb) + 1 >> 1;
    cellRow = (gridCoordinates.rowQ12 >> 0xb) + 1 >> 1;
    if ((((-1 < (int)cellColumn) && (-1 < (int)cellRow)) && (cellRow < fieldGrid->gridHeight)) &&
       (cellColumn < radiusStepsOrGridWidth)) {
      centerCell = fieldGrid->cells + cellRow * radiusStepsOrGridWidth + cellColumn;
      combinedMask = centerCell->occupancyMask;
      if ((centerCell->flagsAndMaterial & 0x88006000) == 0) {
        runStepCount = runStepCount + -1;
        runCursorA = centerCell;
        runRemainingA = runStepCount;
        if (runStepCount != 0) {
          do {
            combinedMask = combinedMask | *(uint64_t *)((int)(runCursorA + 1) + 0x70);
            runCursorB = centerCell;
            runRemainingB = runStepCount;
            if ((*(uint32_t *)((int)(runCursorA + 1) + 0x50) & 0x88006000) != 0) break;
            runRemainingA = runRemainingA + -1;
            runCursorA = runCursorA + 1;
          } while (runRemainingA != 0);
          do {
            combinedMask = combinedMask | *(uint64_t *)((int)(runCursorB + -1) + 0x70);
            runCursorA = centerCell;
            runRemainingA = runStepCount;
            if ((*(uint32_t *)((int)(runCursorB + -1) + 0x50) & 0x88006000) != 0) break;
            runRemainingB = runRemainingB + -1;
            runCursorB = runCursorB + -1;
          } while (runRemainingB != 0);
          do {
            runCursorA = runCursorA + (1 - radiusStepsOrGridWidth);
            combinedMask = combinedMask | runCursorA->occupancyMask;
            runCursorB = centerCell;
            runRemainingB = runStepCount;
            if ((runCursorA->flagsAndMaterial & 0x88006000) != 0) break;
            runRemainingA = runRemainingA + -1;
          } while (runRemainingA != 0);
          do {
            runCursorB = runCursorB + -radiusStepsOrGridWidth;
            combinedMask = combinedMask | runCursorB->occupancyMask;
            runCursorA = centerCell;
            runRemainingA = runStepCount;
            if ((runCursorB->flagsAndMaterial & 0x88006000) != 0) break;
            runRemainingB = runRemainingB + -1;
          } while (runRemainingB != 0);
          do {
            runCursorA = runCursorA + (radiusStepsOrGridWidth - 1);
            combinedMask = combinedMask | runCursorA->occupancyMask;
            if ((runCursorA->flagsAndMaterial & 0x88006000) != 0) break;
            runRemainingA = runRemainingA + -1;
          } while (runRemainingA != 0);
          do {
            centerCell = centerCell + radiusStepsOrGridWidth;
            combinedMask = combinedMask | centerCell->occupancyMask;
            if ((centerCell->flagsAndMaterial & 0x88006000) != 0) break;
            runStepCount = runStepCount + -1;
          } while (runStepCount != 0);
        }
        combinedMask = combinedMask & g_TerrainOccupancyMmxClearBits1And2Mask;
        signBiasMatchBytes = TerrainOccupancy_Pcmpeqb((uint64_t)g_TerrainOccupancyMmxSignBiasBytes,combinedMask);
        mm2PackedValue0 =
             pmaddwd(signBiasMatchBytes & g_TerrainOccupancyMmxPackedScale0280,
                     g_TerrainOccupancyMmxPackedWeights02_20);
        mm1PackedValue0 =
             pmaddwd((TerrainOccupancy_Pcmpeqb(0,combinedMask) ^
                      g_TerrainOccupancyMmxAllBitsMask ^ signBiasMatchBytes) &
                     g_TerrainOccupancyMmxPackedScale0280,g_TerrainOccupancyMmxPackedWeights04_40);
        return (int)((uint64_t)mm1PackedValue0 >> 0x20) + (int)((uint64_t)mm2PackedValue0 >> 0x20)
               | (uint32_t)((int)mm1PackedValue0 + (int)mm2PackedValue0) >> 8;
      }
    }
  }
  return 0;
}


/* Address: 0x005138F0.
   Ownership: world/terrain/occupancy.
   Purpose: Combines the base runtime flags with two terrain occupancy masks for the selected two-bit class channel
   and returns the verified state bits 0x08 and 0x04. Typed parameters: p0 baseRuntimeFlags→FieldGridRuntimeFlags,
   p1 secondaryOccupancyMask→FieldGridRegionMask, p2 primaryOccupancyMask→FieldGridRegionMask. Nearby but non-
   identical semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
TerrainOccupancyResolvedMasksRegs12
TerrainOccupancyMask_ResolveRuntimeClassFlags
          (FieldGridRuntimeFlags baseRuntimeFlags,FieldGridRegionMask secondaryOccupancyMask,
          FieldGridRegionMask primaryOccupancyMask,char runtimeClassIndex)

{
  FieldGridRuntimeFlags resolvedClassFlags;
  uint32_t combinedOccupancyMask;
  uint32_t runtimeClassBit;
  TerrainOccupancyResolvedMasksRegs12 resolvedMasks;
  
  resolvedMasks.secondaryOccupancyMask =
       secondaryOccupancyMask | ((primaryOccupancyMask & 0xaaaaaaaa) >> 1) * 3;
  runtimeClassBit = 1 << (runtimeClassIndex * '\x02' & 0x1fU);
  combinedOccupancyMask = primaryOccupancyMask & resolvedMasks.secondaryOccupancyMask;
  resolvedClassFlags = 0;
  if ((baseRuntimeFlags & 0x10) == 0) {
    combinedOccupancyMask = combinedOccupancyMask | combinedOccupancyMask * 2 & 0xaaaaaaaa;
  }
  if ((runtimeClassBit & combinedOccupancyMask) != 0) {
    resolvedClassFlags = 8;
  }
  if ((primaryOccupancyMask & runtimeClassBit * 2) != 0) {
    resolvedClassFlags = 4;
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

