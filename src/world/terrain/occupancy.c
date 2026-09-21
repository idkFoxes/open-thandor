/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/occupancy.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/occupancy.h>

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
  byte *pbVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *pFVar7;
  FieldGridCell *pFVar8;
  FieldGridCell *cell;
  FieldGridCoordinatesEaxEdx8 FVar9;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex = occupancyByteOffset;
    FVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar3 = FVar9.columnQ12 >> 0xc;
    g_TerrainScanRowStrideBytes = fieldGrid->gridWidth << 7;
    uVar5 = FVar9.rowQ12 >> 0xc;
    if ((((-1 < (int)uVar3) && (uVar4 = fieldGrid->gridWidth & 0x1ffffff, -1 < (int)uVar5)) &&
        (uVar5 < fieldGrid->gridHeight)) &&
       ((uVar3 < uVar4 &&
        (iVar6 = uVar5 * uVar4 + uVar3, (fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0
        )))) {
      pbVar1 = fieldGrid->cells[iVar6].runtime60_6B + occupancyByteOffset + 0x10;
      *pbVar1 = *pbVar1 | 2;
      iVar2 = g_TerrainScanRowStrideBytes;
      pFVar7 = (FieldGridCell *)
               (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
               iVar6 * 0x80 + -0x28);
      pFVar8 = (FieldGridCell *)((int)pFVar7 - g_TerrainScanRowStrideBytes);
      TerrainOccupancyBit2_MarkWedge0(0,pFVar7);
      cell = pFVar8 + -1;
      TerrainOccupancyBit2_MarkWedge1(0,pFVar8);
      pFVar7 = (FieldGridCell *)(cell[-1].runtime0C_3F + iVar2 + -0xc);
      TerrainOccupancyBit2_MarkWedge2(0,cell);
      pFVar8 = (FieldGridCell *)(pFVar7->runtime0C_3F + iVar2 + -0xc);
      TerrainOccupancyBit2_MarkWedge3(0,pFVar7);
      TerrainOccupancyBit2_MarkWedge4(0,pFVar8);
      TerrainOccupancyBit2_MarkWedge5(0,pFVar8 + 1);
    }
  }
  return;
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
dword __thandor_void_preserve_eax_ecx
TerrainOccupancyMask_ClassifyNeighborhoodAtWorldPoint
          (Q12 neighborhoodRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,FieldGridAsset *fieldGrid)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  FieldGridCell *pFVar5;
  FieldGridCell *pFVar6;
  FieldGridCell *pFVar7;
  int iVar8;
  int iVar9;
  char cVar11;
  char cVar12;
  char cVar13;
  char cVar14;
  char cVar15;
  char cVar16;
  ulonglong uVar10;
  char cVar17;
  undefined8 mm1PackedValue0;
  ulonglong uVar18;
  undefined8 mm2PackedValue0;
  FieldGridCoordinatesEaxEdx8 FVar19;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    uVar1 = (neighborhoodRadiusQ12 + 0x7ffU) / 0x901;
    if (uVar1 == 0) {
      iVar3 = 2;
    }
    else if (uVar1 < 0x100) {
      iVar3 = uVar1 + 1;
    }
    else {
      iVar3 = 0x100;
    }
    FVar19 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar1 = fieldGrid->gridWidth;
    uVar2 = (FVar19.columnQ12 >> 0xb) + 1 >> 1;
    uVar4 = (FVar19.rowQ12 >> 0xb) + 1 >> 1;
    if ((((-1 < (int)uVar2) && (-1 < (int)uVar4)) && (uVar4 < fieldGrid->gridHeight)) &&
       (uVar2 < uVar1)) {
      pFVar7 = fieldGrid->cells + uVar4 * uVar1 + uVar2;
      uVar10 = pFVar7->occupancyMask;
      if ((pFVar7->flagsAndMaterial & 0x88006000) == 0) {
        iVar3 = iVar3 + -1;
        pFVar6 = pFVar7;
        iVar8 = iVar3;
        if (iVar3 != 0) {
          do {
            uVar10 = uVar10 | *(ulonglong *)((int)(pFVar6 + 1) + 0x70);
            pFVar5 = pFVar7;
            iVar9 = iVar3;
            if ((*(uint *)((int)(pFVar6 + 1) + 0x50) & 0x88006000) != 0) break;
            iVar8 = iVar8 + -1;
            pFVar6 = pFVar6 + 1;
          } while (iVar8 != 0);
          do {
            uVar10 = uVar10 | *(ulonglong *)((int)(pFVar5 + -1) + 0x70);
            pFVar6 = pFVar7;
            iVar8 = iVar3;
            if ((*(uint *)((int)(pFVar5 + -1) + 0x50) & 0x88006000) != 0) break;
            iVar9 = iVar9 + -1;
            pFVar5 = pFVar5 + -1;
          } while (iVar9 != 0);
          do {
            pFVar6 = pFVar6 + (1 - uVar1);
            uVar10 = uVar10 | pFVar6->occupancyMask;
            pFVar5 = pFVar7;
            iVar9 = iVar3;
            if ((pFVar6->flagsAndMaterial & 0x88006000) != 0) break;
            iVar8 = iVar8 + -1;
          } while (iVar8 != 0);
          do {
            pFVar5 = pFVar5 + -uVar1;
            uVar10 = uVar10 | pFVar5->occupancyMask;
            pFVar6 = pFVar7;
            iVar8 = iVar3;
            if ((pFVar5->flagsAndMaterial & 0x88006000) != 0) break;
            iVar9 = iVar9 + -1;
          } while (iVar9 != 0);
          do {
            pFVar6 = pFVar6 + (uVar1 - 1);
            uVar10 = uVar10 | pFVar6->occupancyMask;
            if ((pFVar6->flagsAndMaterial & 0x88006000) != 0) break;
            iVar8 = iVar8 + -1;
          } while (iVar8 != 0);
          do {
            pFVar7 = pFVar7 + uVar1;
            uVar10 = uVar10 | pFVar7->occupancyMask;
            if ((pFVar7->flagsAndMaterial & 0x88006000) != 0) break;
            iVar3 = iVar3 + -1;
          } while (iVar3 != 0);
        }
        uVar10 = uVar10 & g_TerrainOccupancyMmxClearBits1And2Mask;
        cVar11 = (char)(uVar10 >> 8);
        cVar12 = (char)(uVar10 >> 0x10);
        cVar13 = (char)(uVar10 >> 0x18);
        cVar14 = (char)(uVar10 >> 0x20);
        cVar15 = (char)(uVar10 >> 0x28);
        cVar16 = (char)(uVar10 >> 0x30);
        cVar17 = (char)(uVar10 >> 0x38);
        uVar18 = CONCAT17(-((char)((ulonglong)g_TerrainOccupancyMmxSignBiasBytes >> 0x38) == cVar17)
                          ,CONCAT16(-((char)((ulonglong)g_TerrainOccupancyMmxSignBiasBytes >> 0x30)
                                     == cVar16),
                                    CONCAT15(-((char)((ulonglong)g_TerrainOccupancyMmxSignBiasBytes
                                                     >> 0x28) == cVar15),
                                             CONCAT14(-((char)((ulonglong)
                                                               g_TerrainOccupancyMmxSignBiasBytes >>
                                                              0x20) == cVar14),
                                                      CONCAT13(-((char)((ulonglong)
                                                                                                                                                
                                                  g_TerrainOccupancyMmxSignBiasBytes >> 0x18) ==
                                                  cVar13),CONCAT12(-((char)((ulonglong)
                                                                                                                                                        
                                                  g_TerrainOccupancyMmxSignBiasBytes >> 0x10) ==
                                                  cVar12),CONCAT11(-((char)((ulonglong)
                                                                                                                                                        
                                                  g_TerrainOccupancyMmxSignBiasBytes >> 8) == cVar11
                                                  ),-((char)g_TerrainOccupancyMmxSignBiasBytes ==
                                                     (char)uVar10))))))));
        mm2PackedValue0 =
             pmaddwd(uVar18 & g_TerrainOccupancyMmxPackedScale0280,
                     g_TerrainOccupancyMmxPackedWeights02_20);
        mm1PackedValue0 =
             pmaddwd((CONCAT17(-(cVar17 == '\0'),
                               CONCAT16(-(cVar16 == '\0'),
                                        CONCAT15(-(cVar15 == '\0'),
                                                 CONCAT14(-(cVar14 == '\0'),
                                                          CONCAT13(-(cVar13 == '\0'),
                                                                   CONCAT12(-(cVar12 == '\0'),
                                                                            CONCAT11(-(cVar11 ==
                                                                                      '\0'),-((char)
                                                  uVar10 == '\0')))))))) ^
                      g_TerrainOccupancyMmxAllBitsMask ^ uVar18) &
                     g_TerrainOccupancyMmxPackedScale0280,g_TerrainOccupancyMmxPackedWeights04_40);
        return (int)((ulonglong)mm1PackedValue0 >> 0x20) + (int)((ulonglong)mm2PackedValue0 >> 0x20)
               | (uint)((int)mm1PackedValue0 + (int)mm2PackedValue0) >> 8;
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
  uint combinedOccupancyMask;
  uint runtimeClassBit;
  TerrainOccupancyResolvedMasksRegs12 TVar1;
  
  TVar1.secondaryOccupancyMask =
       secondaryOccupancyMask | ((primaryOccupancyMask & 0xaaaaaaaa) >> 1) * 3;
  runtimeClassBit = 1 << (runtimeClassIndex * '\x02' & 0x1fU);
  combinedOccupancyMask = primaryOccupancyMask & TVar1.secondaryOccupancyMask;
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
  TVar1.primaryOccupancyMask = combinedOccupancyMask;
  TVar1.runtimeFlags = resolvedClassFlags;
  return TVar1;
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
  byte *pbVar1;
  FieldGridCell *cell_00;
  int rowStrideBytes;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10] | 2;
      rowStrideBytes = g_TerrainScanRowStrideBytes;
      cell_00 = cell + 1;
      TerrainOccupancyBit2_MarkDirection0(scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      pbVar1 = (byte *)((int)cell_00 +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + (0x70 - rowStrideBytes));
      *pbVar1 = *pbVar1 | 2;
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - rowStrideBytes));
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
  byte *pbVar1;
  FieldGridCell *cell_00;
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
      if ((*(uint *)((int)cell + (0x50 - rowStrideBytes)) & 0x88006000) != 0) {
        return;
      }
      pbVar1 = (byte *)((int)cell +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + (0x70 - rowStrideBytes));
      *pbVar1 = *pbVar1 | 2;
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - rowStrideBytes));
      scanStep = scanStep + 7;
      cell = cell_00 + 1;
      TerrainOccupancyBit2_MarkDirection2(scanStep,cell_00);
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
  FieldGridCell *cell_00;
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
      cell_00 = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainOccupancyBit2_MarkDirection3(scanStep,cell_00);
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
  byte *pbVar1;
  int iVar2;
  FieldGridCell *cell_00;
  TerrainScanSelectorUnion occupancyMarkByteIndex;
  int rowStrideBytes;
  
  occupancyMarkByteIndex.occupancyMaskByteIndex =
       g_TerrainScanSharedSelectorValue.occupancyMaskByteIndex;
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      pbVar1 = (byte *)((int)cell->runtime60_6B +
                       occupancyMarkByteIndex.occupancyMaskByteIndex + 0x10);
      *pbVar1 = *pbVar1 | 2;
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + -1;
      TerrainOccupancyBit2_MarkDirection3(scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(cell_00->runtime60_6B + iVar2 + -0x10) & 0x88006000) != 0) {
        return;
      }
      cell_00->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + iVar2 + 0x10] =
           cell_00->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + iVar2 + 0x10] | 2;
      cell = (FieldGridCell *)(cell_00[-1].runtime0C_3F + iVar2 + -0xc);
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
  byte *pbVar1;
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
      if ((*(uint *)(cell->runtime60_6B + rowStrideBytes + -0x10) & 0x88006000) != 0) {
        return;
      }
      cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + rowStrideBytes + 0x10] =
           cell->runtime60_6B[occupancyMarkByteIndex.occupancyMaskByteIndex + rowStrideBytes + 0x10]
           | 2;
      pbVar1 = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(pbVar1 + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc) + -1;
      TerrainOccupancyBit2_MarkDirection5
                (scanStep,(FieldGridCell *)
                          (pbVar1 + g_TerrainScanRowStrideBytes + rowStrideBytes + -0xc));
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
  FieldGridCell *cell_00;
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
      cell_00 = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      TerrainOccupancyBit2_MarkDirection0(scanStep,cell_00);
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

