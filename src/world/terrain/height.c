/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/height.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/height.h>

/* Implementation ownership: world/terrain/height. */

/* Address: 0x00508000.
   Ownership: world/terrain/height.
   Purpose: Converts a world point to the hexagonal grid, verifies the center cell against the configured relative-
   height band, and dispatches all six wedge tests within the bounded radius. Terrain-class placement test
   callback; CF carries acceptance and EAX carries the direct-call result. Typed parameters: p0
   radiusWorldUnits→FieldGridRadiusUnits. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestWedge0, TerrainHeightBand_TestWedge1, TerrainHeightBand_TestWedge2,
   TerrainHeightBand_TestWedge3, TerrainHeightBand_TestWedge4, TerrainHeightBand_TestWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_void_preserve_ecx_edx
TerrainHeightBand_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *pFVar7;
  FieldGridCell *pFVar8;
  bool bVar9;
  FieldGridCoordinatesEaxEdx8 FVar10;
  uint uVar11;
  uint uVar12;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    FVar10 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar2 = FVar10.columnQ12 >> 0xc;
    uVar11 = FVar10.rowQ12 >> 0xc;
    uVar3 = (uint)((ulonglong)FVar10 & 0xfff00000fff);
    uVar5 = (uint)(((ulonglong)FVar10 & 0xfff00000fff) >> 0x20);
    uVar1 = uVar5 + uVar3 * 2;
    uVar12 = uVar2;
    if (uVar1 < 0x1000) {
      if (0xfff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    else if (uVar1 < 0x2001) {
      uVar12 = uVar2 + 1;
      if (uVar3 < uVar5) {
        uVar11 = uVar11 + 1;
        uVar12 = uVar2;
      }
    }
    else {
      uVar12 = uVar2 + 1;
      if (0x1fff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    uVar1 = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = uVar1 * 0x80;
    if ((((-1 < (int)uVar12) && (-1 < (int)uVar11)) && (uVar11 < fieldGrid->gridHeight)) &&
       (uVar12 < (uVar1 & 0x1ffffff))) {
      iVar6 = uVar11 * (uVar1 & 0x1ffffff) + uVar12;
      if ((((fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0) &&
          (iVar4 = fieldGrid->cells[iVar6].terrainHeight - g_TerrainScanReferenceHeight,
          fieldGrid->cells[iVar6].waterSurfaceDelta < 1)) &&
         ((iVar4 <= g_TerrainHeightBandMaximumDelta && (g_TerrainHeightBandMinimumDelta <= iVar4))))
      {
        pFVar8 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar6 * 0x80 + -0x28);
        pFVar7 = pFVar8 + -uVar1;
        bVar9 = TerrainHeightBand_TestWedge0(0,pFVar8);
        if (!bVar9) {
          pFVar8 = pFVar7 + -1;
          bVar9 = TerrainHeightBand_TestWedge1(0,pFVar7);
          if (!bVar9) {
            pFVar7 = pFVar8 + (uVar1 - 1);
            bVar9 = TerrainHeightBand_TestWedge2(0,pFVar8);
            if (!bVar9) {
              bVar9 = TerrainHeightBand_TestWedge3(0,(byte *)pFVar7);
              if (!bVar9) {
                bVar9 = TerrainHeightBand_TestWedge4(0,pFVar7 + uVar1);
                if ((!bVar9) && (bVar9 = TerrainHeightBand_TestWedge5(0,pFVar7 + uVar1 + 1), !bVar9)
                   ) {
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}


/* Address: 0x00508920.
   Ownership: world/terrain/height.
   Purpose: Converts a world point to the hexagonal grid, validates the center cell auxiliary height, and
   dispatches all six threshold wedge tests within the bounded radius. Terrain-class placement test callback; CF
   carries acceptance and EAX carries the direct-call result. Typed parameters: p0
   radiusWorldUnits→FieldGridRadiusUnits. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestWedge0, TerrainAuxHeightThreshold_TestWedge1,
   TerrainAuxHeightThreshold_TestWedge2, TerrainAuxHeightThreshold_TestWedge3,
   TerrainAuxHeightThreshold_TestWedge4, TerrainAuxHeightThreshold_TestWedge5.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_void_preserve_ecx_edx
TerrainAuxHeightThreshold_TestAroundWorldPoint
          (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          FieldGridAsset *fieldGrid)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  FieldGridCell *pFVar6;
  FieldGridCell *pFVar7;
  bool bVar8;
  FieldGridCoordinatesEaxEdx8 FVar9;
  uint uVar10;
  uint uVar11;
  
  if (fieldGrid != (FieldGridAsset *)0x0) {
    g_TerrainScanStepLimit = (uint)radiusWorldUnits / 0x240;
    if (g_TerrainScanStepLimit == 0) {
      g_TerrainScanStepLimit = 1;
    }
    else if (0xff < g_TerrainScanStepLimit) {
      g_TerrainScanStepLimit = 0xff;
    }
    g_TerrainScanReferenceHeight = referenceHeightQ12;
    FVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar2 = FVar9.columnQ12 >> 0xc;
    uVar10 = FVar9.rowQ12 >> 0xc;
    uVar3 = (uint)((ulonglong)FVar9 & 0xfff00000fff);
    uVar4 = (uint)(((ulonglong)FVar9 & 0xfff00000fff) >> 0x20);
    uVar1 = uVar4 + uVar3 * 2;
    uVar11 = uVar2;
    if (uVar1 < 0x1000) {
      if (0xfff < uVar3 + uVar4 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    else if (uVar1 < 0x2001) {
      uVar11 = uVar2 + 1;
      if (uVar3 < uVar4) {
        uVar10 = uVar10 + 1;
        uVar11 = uVar2;
      }
    }
    else {
      uVar11 = uVar2 + 1;
      if (0x1fff < uVar3 + uVar4 * 2) {
        uVar10 = uVar10 + 1;
      }
    }
    uVar1 = fieldGrid->gridWidth;
    g_TerrainScanRowStrideBytes = uVar1 * 0x80;
    if ((((-1 < (int)uVar11) && (-1 < (int)uVar10)) && (uVar10 < fieldGrid->gridHeight)) &&
       (uVar11 < (uVar1 & 0x1ffffff))) {
      iVar5 = uVar10 * (uVar1 & 0x1ffffff) + uVar11;
      if ((((fieldGrid->cells[iVar5].flagsAndMaterial & 0x88006000) == 0) &&
          (-1 < fieldGrid->cells[iVar5].waterSurfaceDelta)) &&
         (g_TerrainAuxHeightMinimum <= (int)fieldGrid->cells[iVar5].triangle0NormalAngles >> 0x10))
      {
        pFVar7 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar5 * 0x80 + -0x28);
        pFVar6 = pFVar7 + -uVar1;
        bVar8 = TerrainAuxHeightThreshold_TestWedge0(0,pFVar7);
        if (!bVar8) {
          pFVar7 = pFVar6 + -1;
          bVar8 = TerrainAuxHeightThreshold_TestWedge1(0,pFVar6);
          if (!bVar8) {
            pFVar6 = pFVar7 + (uVar1 - 1);
            bVar8 = TerrainAuxHeightThreshold_TestWedge2(0,pFVar7);
            if (!bVar8) {
              bVar8 = TerrainAuxHeightThreshold_TestWedge3(0,pFVar6);
              if (!bVar8) {
                bVar8 = TerrainAuxHeightThreshold_TestWedge4(0,pFVar6 + uVar1);
                if ((!bVar8) &&
                   (bVar8 = TerrainAuxHeightThreshold_TestWedge5(0,pFVar6 + uVar1 + 1), !bVar8)) {
                  return false;
                }
              }
            }
          }
        }
      }
    }
  }
  return true;
}


/* Address: 0x00508D20.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection0, TerrainHeightDelta_ApplyDirection1.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *piVar1;
  int iVar2;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *cell_00;
  int *adjacentHeightField;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar2;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar2;
      iVar2 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + 1;
      TerrainHeightDelta_ApplyDirection0(scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - iVar2)) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)((int)cell_00 + (0x48 - iVar2));
      adjacentHeightField = (int *)((int)cell_00 + (0x48 - iVar2));
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      piVar1 = (int *)((int)cell_00 + (0x4c - iVar2));
      *piVar1 = *piVar1 - adjacentHeightAdjustmentQ12;
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - iVar2));
      scanStep = scanStep + 7;
      TerrainHeightDelta_ApplyDirection1
                (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00508DC0.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection1, TerrainHeightDelta_ApplyDirection2.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *piVar1;
  int iVar2;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *cell_00;
  int *adjacentHeightField;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar2 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar2;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar2;
      iVar2 = g_TerrainScanRowStrideBytes;
      TerrainHeightDelta_ApplyDirection1
                (scanStep + 4,(FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)((int)cell + (0x50 - iVar2)) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)((int)cell + (0x48 - iVar2));
      adjacentHeightField = (int *)((int)cell + (0x48 - iVar2));
      *adjacentHeightField = *adjacentHeightField + adjacentHeightAdjustmentQ12;
      piVar1 = (int *)((int)cell + (0x4c - iVar2));
      *piVar1 = *piVar1 - adjacentHeightAdjustmentQ12;
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar2));
      scanStep = scanStep + 7;
      cell = cell_00 + 1;
      TerrainHeightDelta_ApplyDirection2(scanStep,cell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00508E60.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection2, TerrainHeightDelta_ApplyDirection3.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      TerrainHeightDelta_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[-1].terrainHeight;
      cell[-1].terrainHeight = cell[-1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[-1].waterSurfaceDelta = cell[-1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      cell_00 = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainHeightDelta_ApplyDirection3(scanStep,cell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00508F00.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection3, TerrainHeightDelta_ApplyDirection4.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int adjacentHeightAdjustmentQ12;
  FieldGridCell *cell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar1 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar1;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar1;
      iVar1 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + -1;
      TerrainHeightDelta_ApplyDirection3(scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(cell_00->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(cell_00->runtime60_6B + iVar1 + -0x18);
      *(int *)(cell_00->runtime60_6B + iVar1 + -0x18) =
           *(int *)(cell_00->runtime60_6B + iVar1 + -0x18) + adjacentHeightAdjustmentQ12;
      *(int *)(cell_00->runtime60_6B + iVar1 + -0x14) =
           *(int *)(cell_00->runtime60_6B + iVar1 + -0x14) - adjacentHeightAdjustmentQ12;
      cell = (FieldGridCell *)(cell_00[-1].runtime0C_3F + iVar1 + -0xc);
      scanStep = scanStep + 7;
      TerrainHeightDelta_ApplyDirection4
                (scanStep,(FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc)
                );
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00508FA0.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection4, TerrainHeightDelta_ApplyDirection5.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int adjacentHeightAdjustmentQ12;
  byte *currentCellRuntimeBase;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar1 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar1;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar1;
      iVar1 = g_TerrainScanRowStrideBytes;
      TerrainHeightDelta_ApplyDirection4
                (scanStep + 4,
                 (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((*(uint *)(cell->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(cell->runtime60_6B + iVar1 + -0x18);
      *(int *)(cell->runtime60_6B + iVar1 + -0x18) =
           *(int *)(cell->runtime60_6B + iVar1 + -0x18) + adjacentHeightAdjustmentQ12;
      *(int *)(cell->runtime60_6B + iVar1 + -0x14) =
           *(int *)(cell->runtime60_6B + iVar1 + -0x14) - adjacentHeightAdjustmentQ12;
      currentCellRuntimeBase = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(currentCellRuntimeBase + g_TerrainScanRowStrideBytes + iVar1 + -0xc)
             + -1;
      TerrainHeightDelta_ApplyDirection5
                (scanStep,(FieldGridCell *)
                          (currentCellRuntimeBase + g_TerrainScanRowStrideBytes + iVar1 + -0xc));
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00509040.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection5, TerrainHeightDelta_ApplyDirection0.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      TerrainHeightDelta_ApplyDirection5
                (scanStep + 4,
                 (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[1].terrainHeight;
      cell[1].terrainHeight = cell[1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[1].waterSurfaceDelta = cell[1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      cell_00 = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      TerrainHeightDelta_ApplyDirection0(scanStep,cell_00);
      if (g_TerrainScanStepLimit <= scanStep) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x00504520.
   Ownership: world/terrain/height.
   Purpose: Tests a directed fixed-point ray against one terrain-cell triangle assembled from four corner heights.
   On intersection it returns the ray distance in EAX with CF set; otherwise it returns zero or the carried
   traversal value with CF clear. Storage remains one signed 32-bit word. Typed parameters: p3
   gridRayDelta0Q12→Q12, p4 gridRayDelta1Q12→Q12, p6 cornerHeight0Q12→Q12, p7 cornerHeight1Q12→Q12, p8
   cornerHeight2Q12→Q12, p9 cornerHeight3Q12→Q12, p10 cellLocalCoord1Q12→Q12, p11 cellLocalCoord0Q12→Q12. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged.
   Cross-module calls: FixedMath_Length3 [core/math/fixed].
*/
TerrainDistanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TerrainTriangle_IntersectRayDistanceCf
          (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
          Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,Q12 cornerHeight3Q12,
          Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  ulonglong uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  Q12 QVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  TerrainDistanceEaxCf5 TVar22;
  TerrainDistanceEaxCf5 TVar23;
  TerrainDistanceEaxCf5 TVar24;
  TerrainDistanceEaxCf5 TVar25;
  TerrainDistanceEaxCf5 TVar26;
  
  QVar13 = rayOriginZQ12;
  if (rayDeltaZQ12 < 0) {
    QVar13 = rayOriginZQ12 + rayDeltaZQ12;
  }
  if ((((cornerHeight3Q12 <= QVar13) && (cornerHeight2Q12 <= QVar13)) &&
      (cornerHeight1Q12 <= QVar13)) && (uVar6 = cornerHeight2Q12, cornerHeight0Q12 <= QVar13))
  goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  lVar1 = ((longlong)(cornerHeight1Q12 - cornerHeight3Q12) * (longlong)gridRayDelta0Q12 +
          (longlong)(cornerHeight2Q12 - cornerHeight3Q12) * (longlong)gridRayDelta1Q12) -
          CONCAT44(rayDeltaZQ12 >> 0x14,rayDeltaZQ12 << 0xc);
  lVar4 = (longlong)(cornerHeight1Q12 - cornerHeight3Q12) * (longlong)cellLocalCoord1Q12 +
          (longlong)(cornerHeight2Q12 - cornerHeight3Q12) * (longlong)cellLocalCoord0Q12 +
          CONCAT44(rayOriginZQ12 - cornerHeight3Q12 >> 0x14,
                   (rayOriginZQ12 - cornerHeight3Q12) * 0x1000);
  if (lVar4 < 0) {
    if ((lVar1 < 0) && (-1 < lVar4 - lVar1))
    goto TerrainTriangle_IntersectRayDistanceCf_BeginFirstTriangleEdgeTestsAfterPlaneCrossing;
  }
  else if ((-1 < lVar1) && (lVar4 - lVar1 < 0)) {
TerrainTriangle_IntersectRayDistanceCf_BeginFirstTriangleEdgeTestsAfterPlaneCrossing:
    uVar5 = (longlong)gridRayDelta1Q12 * (longlong)cellLocalCoord1Q12 -
            (longlong)cellLocalCoord0Q12 * (longlong)gridRayDelta0Q12;
    iVar15 = (int)(uVar5 >> 0x20);
    uVar6 = cornerHeight1Q12 - cornerHeight3Q12;
    if ((int)uVar6 < 0) {
      lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
      uVar7 = (uint)lVar4;
      iVar16 = (iVar15 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
    }
    else {
      lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
      uVar7 = (uint)lVar4;
      iVar16 = iVar15 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
    }
    lVar4 = (longlong)(rayOriginZQ12 - cornerHeight3Q12) * (longlong)gridRayDelta1Q12;
    uVar6 = (uint)lVar4;
    uVar8 = uVar6 * 0x1000;
    uVar19 = uVar7 + uVar8;
    uVar9 = (uint)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord0Q12);
    uVar10 = uVar9 * 0x1000;
    uVar20 = uVar19 + uVar10;
    iVar16 = iVar16 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar6 >> 0x14) +
             (uint)CARRY4(uVar7,uVar8) +
             ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord0Q12) >> 0x20) <<
              0xc | uVar9 >> 0x14) + (uint)CARRY4(uVar19,uVar10);
    if (iVar16 < 0) {
      if (lVar1 < 0) {
TerrainTriangle_IntersectRayDistanceCf_EvaluateFirstTriangleRemainingEdgeConstraints:
        uVar6 = cornerHeight3Q12 - cornerHeight2Q12;
        if ((int)uVar6 < 0) {
          lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
          uVar7 = (uint)lVar4;
          iVar15 = (iVar15 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
        }
        else {
          lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
          uVar7 = (uint)lVar4;
          iVar15 = iVar15 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
        }
        lVar4 = (longlong)(rayOriginZQ12 - cornerHeight3Q12) * (longlong)gridRayDelta0Q12;
        uVar6 = (uint)lVar4;
        uVar8 = uVar6 * 0x1000;
        uVar21 = uVar7 + uVar8;
        uVar9 = (uint)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord1Q12);
        uVar10 = uVar9 * 0x1000;
        uVar11 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
        uVar14 = (uint)lVar1 * 0x1000;
        uVar19 = uVar10 + uVar21;
        iVar15 = ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord1Q12) >> 0x20)
                  << 0xc | uVar9 >> 0x14) +
                 iVar15 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar6 >> 0x14) +
                 (uint)CARRY4(uVar7,uVar8) + (uint)CARRY4(uVar10,uVar21);
        if (iVar15 < 0) {
          if ((int)uVar11 < 0) {
            uVar6 = (uVar14 - uVar19) - uVar20;
            iVar12 = (((uVar11 - iVar15) - (uint)(uVar14 < uVar19)) - iVar16) -
                     (uint)(uVar14 - uVar19 < uVar20);
            if (iVar12 < 0) {
TerrainTriangle_IntersectRayDistanceCf_ComputeFirstTriangleIntersectionDistance:
              iVar12 = iVar12 + iVar15 + (uint)CARRY4(uVar6,uVar19) + iVar16 +
                       (uint)CARRY4(uVar6 + uVar19,uVar20);
              uVar6 = uVar6 + uVar19 + uVar20 >> 0xc | iVar12 * 0x100000;
              iVar12 = iVar12 >> 0xc;
              if (uVar6 == 0) {
                TVar26.distanceQ12 = 0;
                TVar26.carry = false;
                return TVar26;
              }
              if ((int)uVar6 < 0) {
                if (iVar12 != -1) {
                  uVar20 = uVar20 >> 0xc | iVar16 * 0x100000;
                  uVar19 = uVar19 >> 0xc | iVar15 * 0x100000;
                  uVar6 = uVar6 >> 0xc | iVar12 << 0x14;
                  iVar15 = iVar15 >> 0xc;
                  iVar16 = iVar16 >> 0xc;
                }
              }
              else if (iVar12 != 0) {
                uVar20 = uVar20 >> 0xc | iVar16 * 0x100000;
                uVar19 = uVar19 >> 0xc | iVar15 * 0x100000;
                uVar6 = uVar6 >> 0xc | iVar12 << 0x14;
                iVar15 = iVar15 >> 0xc;
                iVar16 = iVar16 >> 0xc;
              }
              iVar15 = (int)(CONCAT44(iVar15,uVar19) / (longlong)(int)uVar6);
              iVar12 = cellLocalCoord1Q12 + iVar15;
              iVar16 = (int)(CONCAT44(iVar16,uVar20) / (longlong)(int)uVar6);
              lVar4 = (longlong)iVar16 * (longlong)(cornerHeight2Q12 - cornerHeight3Q12);
              lVar1 = (longlong)iVar15 * (longlong)(cornerHeight1Q12 - cornerHeight3Q12);
              lVar2 = (longlong)(iVar12 + (cellLocalCoord0Q12 + iVar16) * 2) * 0x901;
              lVar3 = (longlong)iVar12 * -1999;
              TVar23.distanceQ12 =
                   FixedMath_Length3(((cornerHeight3Q12 +
                                      ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc)
                                      ) - rayOriginZQ12) +
                                     ((int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc),
                                     (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc,
                                     (int)((ulonglong)lVar2 >> 0x20) << 0x13 | (uint)lVar2 >> 0xd);
              TVar23.carry = false;
              return TVar23;
            }
          }
        }
        else if (-1 < (int)uVar11) {
          uVar6 = (uVar14 - uVar19) - uVar20;
          iVar12 = (((uVar11 - iVar15) - (uint)(uVar14 < uVar19)) - iVar16) -
                   (uint)(uVar14 - uVar19 < uVar20);
          if (-1 < iVar12)
          goto TerrainTriangle_IntersectRayDistanceCf_ComputeFirstTriangleIntersectionDistance;
        }
      }
    }
    else if (-1 < lVar1)
    goto TerrainTriangle_IntersectRayDistanceCf_EvaluateFirstTriangleRemainingEdgeConstraints;
  }
  iVar15 = cellLocalCoord0Q12 + 0x1000;
  iVar16 = cellLocalCoord1Q12 + 0x1000;
  lVar4 = (longlong)(cornerHeight2Q12 - cornerHeight0Q12) * (longlong)gridRayDelta0Q12 +
          (longlong)(cornerHeight1Q12 - cornerHeight0Q12) * (longlong)gridRayDelta1Q12 +
          CONCAT44(rayDeltaZQ12 >> 0x14,rayDeltaZQ12 << 0xc);
  uVar7 = (uint)lVar4;
  iVar12 = (int)((ulonglong)lVar4 >> 0x20);
  uVar6 = (cornerHeight0Q12 - rayOriginZQ12) * 0x1000;
  lVar1 = (longlong)(cornerHeight2Q12 - cornerHeight0Q12) * (longlong)iVar16 +
          (longlong)(cornerHeight1Q12 - cornerHeight0Q12) * (longlong)iVar15 +
          CONCAT44(cornerHeight0Q12 - rayOriginZQ12 >> 0x14,uVar6);
  iVar17 = (int)((ulonglong)lVar1 >> 0x20);
  if (lVar1 < 0) {
    if ((-1 < lVar4) || ((int)((iVar17 - iVar12) - (uint)((uint)lVar1 < uVar7)) < 0))
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if ((lVar4 < 0) || (-1 < (int)((iVar17 - iVar12) - (uint)((uint)lVar1 < uVar7))))
  goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  uVar5 = (longlong)gridRayDelta1Q12 * (longlong)iVar16 -
          (longlong)iVar15 * (longlong)gridRayDelta0Q12;
  iVar17 = (int)(uVar5 >> 0x20);
  uVar6 = cornerHeight0Q12 - cornerHeight2Q12;
  if ((int)uVar6 < 0) {
    lVar1 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar1;
    iVar18 = (iVar17 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar1 >> 0x20);
  }
  else {
    lVar1 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar1;
    iVar18 = iVar17 * uVar6 + (int)((ulonglong)lVar1 >> 0x20);
  }
  lVar1 = (longlong)(rayOriginZQ12 - cornerHeight0Q12) * (longlong)gridRayDelta1Q12;
  uVar9 = (uint)lVar1;
  uVar10 = uVar9 * 0x1000;
  uVar20 = uVar8 + uVar10;
  uVar19 = (uint)((longlong)rayDeltaZQ12 * (longlong)iVar15);
  uVar6 = uVar19 * 0x1000;
  uVar11 = uVar20 + uVar6;
  iVar18 = iVar18 + ((int)((ulonglong)lVar1 >> 0x20) << 0xc | uVar9 >> 0x14) +
           (uint)CARRY4(uVar8,uVar10) +
           ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)iVar15) >> 0x20) << 0xc |
           uVar19 >> 0x14) + (uint)CARRY4(uVar20,uVar6);
  if (iVar18 < 0) {
    if (-1 < lVar4)
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if (lVar4 < 0)
  goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  uVar6 = cornerHeight1Q12 - cornerHeight0Q12;
  if ((int)uVar6 < 0) {
    lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar4;
    iVar17 = (iVar17 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
  }
  else {
    lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar4;
    iVar17 = iVar17 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
  }
  lVar4 = (longlong)(rayOriginZQ12 - cornerHeight0Q12) * (longlong)gridRayDelta0Q12;
  uVar9 = (uint)lVar4;
  uVar10 = uVar9 * 0x1000;
  uVar21 = uVar8 + uVar10;
  uVar19 = (uint)((longlong)rayDeltaZQ12 * (longlong)iVar16);
  uVar20 = uVar19 * 0x1000;
  uVar14 = iVar12 << 0xc | uVar7 >> 0x14;
  uVar7 = uVar7 * 0x1000;
  uVar6 = uVar20 + uVar21;
  iVar12 = ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)iVar16) >> 0x20) << 0xc |
           uVar19 >> 0x14) +
           iVar17 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar9 >> 0x14) +
           (uint)CARRY4(uVar8,uVar10) + (uint)CARRY4(uVar20,uVar21);
  if (iVar12 < 0) {
    if ((int)uVar14 < 0) {
      uVar8 = (uVar7 - uVar6) - uVar11;
      iVar17 = (((uVar14 - iVar12) - (uint)(uVar7 < uVar6)) - iVar18) -
               (uint)(uVar7 - uVar6 < uVar11);
      if (iVar17 < 0) {
LAB_00504912:
        iVar17 = iVar17 + iVar12 + (uint)CARRY4(uVar8,uVar6) + iVar18 +
                 (uint)CARRY4(uVar8 + uVar6,uVar11);
        uVar7 = uVar8 + uVar6 + uVar11 >> 0xc | iVar17 * 0x100000;
        iVar17 = iVar17 >> 0xc;
        if (uVar7 == 0) {
          TVar25.distanceQ12 = 0;
          TVar25.carry = false;
          return TVar25;
        }
        if ((int)uVar7 < 0) {
          if (iVar17 != -1) {
            uVar11 = uVar11 >> 0xc | iVar18 * 0x100000;
            uVar6 = uVar6 >> 0xc | iVar12 * 0x100000;
            uVar7 = uVar7 >> 0xc | iVar17 << 0x14;
            iVar12 = iVar12 >> 0xc;
            iVar18 = iVar18 >> 0xc;
          }
        }
        else if (iVar17 != 0) {
          uVar11 = uVar11 >> 0xc | iVar18 * 0x100000;
          uVar6 = uVar6 >> 0xc | iVar12 * 0x100000;
          uVar7 = uVar7 >> 0xc | iVar17 << 0x14;
          iVar12 = iVar12 >> 0xc;
          iVar18 = iVar18 >> 0xc;
        }
        iVar12 = (int)(CONCAT44(iVar12,uVar6) / (longlong)(int)uVar7);
        iVar16 = iVar16 - iVar12;
        iVar17 = (int)(CONCAT44(iVar18,uVar11) / (longlong)(int)uVar7);
        lVar4 = (longlong)iVar17 * (longlong)(cornerHeight1Q12 - cornerHeight0Q12);
        lVar1 = (longlong)iVar12 * (longlong)(cornerHeight2Q12 - cornerHeight0Q12);
        lVar2 = (longlong)(iVar16 + (iVar15 - iVar17) * 2) * 0x901;
        lVar3 = (longlong)iVar16 * -1999;
        TVar24.distanceQ12 =
             FixedMath_Length3(((cornerHeight0Q12 +
                                ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc)) -
                               rayOriginZQ12) +
                               ((int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc),
                               (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc,
                               (int)((ulonglong)lVar2 >> 0x20) << 0x13 | (uint)lVar2 >> 0xd);
        TVar24.carry = false;
        return TVar24;
      }
    }
  }
  else if (-1 < (int)uVar14) {
    uVar8 = (uVar7 - uVar6) - uVar11;
    iVar17 = (((uVar14 - iVar12) - (uint)(uVar7 < uVar6)) - iVar18) - (uint)(uVar7 - uVar6 < uVar11)
    ;
    if (-1 < iVar17) goto LAB_00504912;
  }
TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet:
  TVar22.carry = true;
  TVar22.distanceQ12 = uVar6;
  return TVar22;
}


/* Address: 0x005049E0.
   Ownership: world/terrain/height.
   Purpose: Advances the staggered-grid ray traversal to the next X or Y cell boundary. It updates the traversal
   coordinates through the engine register convention, clears CF when another boundary step is available, and sets
   CF when the current point is already within the destination cell. Storage remains one signed 32-bit word. Typed
   parameters: p0 rayEndCoord0Q12→Q12, p1 rayEndCoord1Q12→Q12, p2 rayStartCoord0Q12→Q12, p3 rayStartCoord1Q12→Q12,
   p6 currentGridCoord0Q12→Q12, p7 currentGridCoord1Q12→Q12. Calling convention, storage, body bytes, control flow,
   and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax
TerrainRay_AdvanceGridTraversalCf
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *currentCell,Q12 currentGridCoord0Q12
          ,Q12 currentGridCoord1Q12)

{
  longlong lVar1;
  int iVar2;
  longlong boundaryCrossProductQ24;
  
  if ((((rayEndCoord1Q12 < currentGridCoord1Q12) || (rayEndCoord0Q12 < currentGridCoord0Q12)) ||
      (0x1000 < rayEndCoord1Q12 - currentGridCoord1Q12)) ||
     (0x1000 < rayEndCoord0Q12 - currentGridCoord0Q12)) {
    iVar2 = rayEndCoord0Q12 - rayStartCoord0Q12;
    if (iVar2 != 0) {
      if (iVar2 < 0) {
        lVar1 = (longlong)(rayStartCoord1Q12 - currentGridCoord1Q12) *
                (longlong)(rayEndCoord0Q12 - rayStartCoord0Q12) +
                (longlong)(rayEndCoord1Q12 - rayStartCoord1Q12) *
                (longlong)(currentGridCoord0Q12 - rayStartCoord0Q12);
        if ((lVar1 < 0) &&
           ((int)(((iVar2 >> 0x14) - (int)((ulonglong)lVar1 >> 0x20)) -
                 (uint)((uint)(iVar2 * 0x1000) < (uint)lVar1)) < 0)) {
          return false;
        }
      }
      else {
        boundaryCrossProductQ24 =
             (longlong)(rayStartCoord1Q12 - currentGridCoord1Q12) *
             (longlong)(rayEndCoord0Q12 - rayStartCoord0Q12) +
             (longlong)(rayEndCoord1Q12 - rayStartCoord1Q12) *
             (longlong)((currentGridCoord0Q12 + 0x1000) - rayStartCoord0Q12);
        if ((-1 < boundaryCrossProductQ24) &&
           (-1 < (int)(((iVar2 >> 0x14) - (int)((ulonglong)boundaryCrossProductQ24 >> 0x20)) -
                      (uint)((uint)(iVar2 * 0x1000) < (uint)boundaryCrossProductQ24)))) {
          return false;
        }
      }
    }
    if (rayEndCoord1Q12 != rayStartCoord1Q12) {
      return false;
    }
  }
  return true;
}


/* Address: 0x00507AB0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 0, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection0, TerrainHeightBand_TestDirection1.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int iVar2;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar2 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar2)) ||
         (iVar2 < g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      cell_00 = cell + 1;
      bVar3 = TerrainHeightBand_TestDirection0(scanStep + 4,cell_00);
      if (bVar3) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - iVar1)) & 0x88006000) != 0) {
        return true;
      }
      iVar2 = *(int *)((int)cell_00 + (0x48 - iVar1)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)((int)cell_00 + (0x4c - iVar1))) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar2) {
        return true;
      }
      if (iVar2 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - iVar1));
      scanStep = scanStep + 7;
      bVar3 = TerrainHeightBand_TestDirection1
                        (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (bVar3) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507BA0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection1, TerrainHeightBand_TestDirection2.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int iVar2;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar2 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar2)) ||
         (iVar2 < g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      bVar3 = TerrainHeightBand_TestDirection1
                        (scanStep + 4,
                         (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (bVar3) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)((int)cell + (0x50 - iVar1)) & 0x88006000) != 0) {
        return true;
      }
      iVar2 = *(int *)((int)cell + (0x48 - iVar1)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)((int)cell + (0x4c - iVar1))) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar2) {
        return true;
      }
      if (iVar2 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar1));
      scanStep = scanStep + 7;
      cell = cell_00 + 1;
      bVar3 = TerrainHeightBand_TestDirection2(scanStep,cell_00);
      if (bVar3) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507C80.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 2, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection2, TerrainHeightBand_TestDirection3.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int iVar1;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar1 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar1)) ||
         (iVar1 < g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      bVar2 = TerrainHeightBand_TestDirection2
                        (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (bVar2) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      iVar1 = cell[-1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar1) {
        return true;
      }
      if (iVar1 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell_00 = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      bVar2 = TerrainHeightBand_TestDirection3(scanStep,cell_00);
      if (bVar2) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507D60.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection3, TerrainHeightBand_TestDirection4.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,byte *cell)

{
  int iVar1;
  int iVar2;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if (((((*(uint *)(cell + 0x50) & 0x88006000) != 0) ||
           (0 < (int)*(PackedArgb32 *)(cell + 0x4c))) ||
          (g_TerrainHeightBandMaximumDelta <
           (int)(*(FieldCellPersistedAux *)(cell + 0x48) - g_TerrainScanReferenceHeight))) ||
         ((int)(*(FieldCellPersistedAux *)(cell + 0x48) - g_TerrainScanReferenceHeight) <
          g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      cell_00 = (FieldGridCell *)(cell + -0x80);
      bVar3 = TerrainHeightBand_TestDirection3(scanStep + 4,cell_00);
      if (bVar3) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)(cell_00->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return true;
      }
      iVar2 = *(int *)(cell_00->runtime60_6B + iVar1 + -0x18) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)(cell_00->runtime60_6B + iVar1 + -0x14)) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar2) {
        return true;
      }
      if (iVar2 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell = cell_00[-1].runtime0C_3F + iVar1 + -0xc;
      scanStep = scanStep + 7;
      bVar3 = TerrainHeightBand_TestDirection4
                        (scanStep,(FieldGridCell *)(cell + g_TerrainScanRowStrideBytes));
      if (bVar3) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507E40.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection4, TerrainHeightBand_TestDirection5.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar2 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar3 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar3)) ||
         (iVar3 < g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      bVar4 = TerrainHeightBand_TestDirection4
                        (scanStep + 4,
                         (FieldGridCell *)
                         (cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (bVar4) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)(cell->runtime60_6B + iVar2 + -0x10) & 0x88006000) != 0) {
        return true;
      }
      iVar3 = *(int *)(cell->runtime60_6B + iVar2 + -0x18) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)(cell->runtime60_6B + iVar2 + -0x14)) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar3) {
        return true;
      }
      if (iVar3 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      pbVar1 = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(pbVar1 + g_TerrainScanRowStrideBytes + iVar2 + -0xc) + -1;
      bVar4 = TerrainHeightBand_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (pbVar1 + g_TerrainScanRowStrideBytes + iVar2 + -0xc));
      if (bVar4) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00507F20.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 5, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged. Typed parameters: p3
   cell→FieldGridCell *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection5, TerrainHeightBand_TestDirection0.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int iVar1;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar1 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar1)) ||
         (iVar1 < g_TerrainHeightBandMinimumDelta)) {
        return true;
      }
      bVar2 = TerrainHeightBand_TestDirection5
                        (scanStep + 4,
                         (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (bVar2) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      iVar1 = cell[1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        return true;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar1) {
        return true;
      }
      if (iVar1 < g_TerrainHeightBandMinimumDelta) {
        return true;
      }
      cell_00 = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      bVar2 = TerrainHeightBand_TestDirection0(scanStep,cell_00);
      if (bVar2) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508470.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 0 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection0, TerrainAuxHeightThreshold_TestDirection1.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      cell_00 = cell + 1;
      bVar2 = TerrainAuxHeightThreshold_TestDirection0(scanStep + 4,cell_00);
      if (bVar2) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - iVar1)) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)((int)cell_00 + (0x4c - iVar1)) < 0) {
        return true;
      }
      if (*(int *)((int)cell_00 + (0x78 - iVar1)) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - iVar1));
      scanStep = scanStep + 7;
      bVar2 = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (bVar2) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508540.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection1, TerrainAuxHeightThreshold_TestDirection2.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      bVar2 = TerrainAuxHeightThreshold_TestDirection1
                        (scanStep + 4,
                         (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (bVar2) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)((int)cell + (0x50 - iVar1)) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)((int)cell + (0x4c - iVar1)) < 0) {
        return true;
      }
      if (*(int *)((int)cell + (0x78 - iVar1)) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar1));
      scanStep = scanStep + 7;
      cell = cell_00 + 1;
      bVar2 = TerrainAuxHeightThreshold_TestDirection2(scanStep,cell_00);
      if (bVar2) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508600.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 2 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection2, TerrainAuxHeightThreshold_TestDirection3.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  bool bVar1;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      bVar1 = TerrainAuxHeightThreshold_TestDirection2
                        (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (bVar1) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      if (cell[-1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[-1].triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell_00 = cell + -2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      bVar1 = TerrainAuxHeightThreshold_TestDirection3(scanStep,cell_00);
      if (bVar1) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x005086C0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection3, TerrainAuxHeightThreshold_TestDirection4.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      cell_00 = cell + -1;
      bVar2 = TerrainAuxHeightThreshold_TestDirection3(scanStep + 4,cell_00);
      if (bVar2) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)(cell_00->runtime60_6B + iVar1 + -0x10) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)(cell_00->runtime60_6B + iVar1 + -0x14) < 0) {
        return true;
      }
      if (*(int *)(cell_00->runtime60_6B + iVar1 + 0x18) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell = (FieldGridCell *)(cell_00[-1].runtime0C_3F + iVar1 + -0xc);
      scanStep = scanStep + 7;
      bVar2 = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep,(FieldGridCell *)
                                  (cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (bVar2) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508790.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection4, TerrainAuxHeightThreshold_TestDirection5.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  byte *pbVar1;
  int iVar2;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar2 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      bVar3 = TerrainAuxHeightThreshold_TestDirection4
                        (scanStep + 4,
                         (FieldGridCell *)
                         (cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc));
      if (bVar3) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((*(uint *)(cell->runtime60_6B + iVar2 + -0x10) & 0x88006000) != 0) {
        return true;
      }
      if (*(int *)(cell->runtime60_6B + iVar2 + -0x14) < 0) {
        return true;
      }
      if (*(int *)(cell->runtime60_6B + iVar2 + 0x18) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      pbVar1 = cell->runtime0C_3F;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(pbVar1 + g_TerrainScanRowStrideBytes + iVar2 + -0xc) + -1;
      bVar3 = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep,(FieldGridCell *)
                                  (pbVar1 + g_TerrainScanRowStrideBytes + iVar2 + -0xc));
      if (bVar3) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x00508850.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 5 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection5, TerrainAuxHeightThreshold_TestDirection0.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  bool bVar1;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return true;
      }
      bVar1 = TerrainAuxHeightThreshold_TestDirection5
                        (scanStep + 4,
                         (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc))
      ;
      if (bVar1) {
        return true;
      }
      if (g_TerrainScanStepLimit <= scanStep + 4) {
        return false;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return true;
      }
      if (cell[1].waterSurfaceDelta < 0) {
        return true;
      }
      if ((int)cell[1].triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum) {
        return true;
      }
      cell_00 = cell + 2;
      scanStep = scanStep + 7;
      cell = (FieldGridCell *)(cell[1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
      bVar1 = TerrainAuxHeightThreshold_TestDirection0(scanStep,cell_00);
      if (bVar1) {
        return true;
      }
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return false;
}


/* Address: 0x005077F0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return true;
}


/* Address: 0x00507860.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return true;
}


/* Address: 0x005078E0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00507950.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return true;
}


/* Address: 0x005079C0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00507A40.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int terrainHeightDeltaQ12;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x005081D0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return true;
}


/* Address: 0x00508240.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return true;
}


/* Address: 0x005082B0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return true;
}


/* Address: 0x00508320.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return true;
}


/* Address: 0x00508390.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00508400.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TerrainAuxHeightThreshold_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return false;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
  }
  return true;
}


/* Address: 0x00508AE0.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 0 and transfers the current reference-height delta between terrainHeight and
   waterSurfaceDelta while cells remain eligible. Typed parameters: p0 scanStep→TerrainDirectionalScanStep_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = cell + 1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508B40.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 1 and applies the reference-height delta to eligible cells. Typed parameters:
   p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508BA0.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 2 and applies the reference-height delta to eligible cells. Typed parameters:
   p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508C00.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 3 and applies the reference-height delta to eligible cells. Typed parameters:
   p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = cell + -1;
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508C60.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 4 and applies the reference-height delta to eligible cells. Typed parameters:
   p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell[-1].runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}


/* Address: 0x00508CC0.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 5 and applies the reference-height delta to eligible cells. Typed parameters:
   p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int heightAdjustmentQ12;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((cell->flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      scanStep = scanStep + 4;
      cell = (FieldGridCell *)(cell->runtime0C_3F + g_TerrainScanRowStrideBytes + -0xc);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}

