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
int TerrainHeightBand_TestAroundWorldPoint
              (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,
              Q12 worldYQ12,FieldGridAsset *fieldGrid)

{
  uint uVar1;
  byte *cell;
  uint uVar2;
  uint uVar3;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint extraout_EAX_01;
  uint extraout_EAX_02;
  FieldGridCell *pFVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *cell_00;
  FieldGridCell *pFVar7;
  bool bVar8;
  qword qVar9;
  undefined8 uVar10;
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
    qVar9 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar2 = (int)qVar9 >> 0xc;
    uVar11 = (uint)((longlong)qVar9 >> 0x2c);
    uVar3 = (uint)(qVar9 & 0xfff00000fff);
    uVar5 = (uint)((qVar9 & 0xfff00000fff) >> 0x20);
    uVar1 = uVar5 + uVar3 * 2;
    radiusWorldUnits = uVar2;
    if (uVar1 < 0x1000) {
      if (0xfff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    else if (uVar1 < 0x2001) {
      radiusWorldUnits = uVar2 + 1;
      if (uVar3 < uVar5) {
        uVar11 = uVar11 + 1;
        radiusWorldUnits = uVar2;
      }
    }
    else {
      radiusWorldUnits = uVar2 + 1;
      if (0x1fff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    uVar1 = fieldGrid->gridWidth;
    pFVar4 = (FieldGridCell *)(uVar1 * 0x80);
    g_TerrainScanRowStrideBytes = pFVar4;
    if ((((-1 < radiusWorldUnits) && (-1 < (int)uVar11)) && (uVar11 < fieldGrid->gridHeight)) &&
       ((uint)radiusWorldUnits < (uVar1 & 0x1ffffff))) {
      iVar6 = uVar11 * (uVar1 & 0x1ffffff) + radiusWorldUnits;
      radiusWorldUnits = fieldGrid->cells[iVar6].terrainHeight;
      if ((((fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0) &&
          (radiusWorldUnits = radiusWorldUnits - g_TerrainScanReferenceHeight,
          fieldGrid->cells[iVar6].waterSurfaceDelta < 1)) &&
         ((radiusWorldUnits <= g_TerrainHeightBandMaximumDelta &&
          (g_TerrainHeightBandMinimumDelta <= radiusWorldUnits)))) {
        pFVar7 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar6 * 0x80 + -0x28);
        bVar8 = pFVar7 < pFVar4;
        cell_00 = pFVar7 + -uVar1;
        TerrainHeightBand_TestWedge0(0,pFVar7);
        radiusWorldUnits = extraout_EAX;
        if (!bVar8) {
          bVar8 = cell_00 < (FieldGridCell *)0x80;
          uVar10 = TerrainHeightBand_TestWedge1(0,cell_00);
          radiusWorldUnits = (FieldGridRadiusUnits)uVar10;
          if (!bVar8) {
            cell = cell_00[-1].runtime00_07 + (int)pFVar4[-1].runtime00_07;
            TerrainHeightBand_TestWedge2(0,cell_00 + -1);
            radiusWorldUnits = extraout_EAX_00;
            if (!bVar8) {
              bVar8 = CARRY4((uint)cell,(uint)pFVar4);
              pFVar7 = (FieldGridCell *)(cell + (int)pFVar4);
              TerrainHeightBand_TestWedge3(0,cell);
              radiusWorldUnits = extraout_EAX_01;
              if (!bVar8) {
                bVar8 = (FieldGridCell *)0xffffff7f < pFVar7;
                uVar10 = TerrainHeightBand_TestWedge4(0,pFVar7);
                radiusWorldUnits = (FieldGridRadiusUnits)uVar10;
                if (!bVar8) {
                  bVar8 = pFVar7 + 1 < pFVar4;
                  TerrainHeightBand_TestWedge5(0,pFVar7 + 1);
                  radiusWorldUnits = extraout_EAX_02;
                  if (!bVar8) {
                    return extraout_EAX_02;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return radiusWorldUnits;
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
int TerrainAuxHeightThreshold_TestAroundWorldPoint
              (FieldGridRadiusUnits radiusWorldUnits,Q12 referenceHeightQ12,Q12 worldXQ12,
              Q12 worldYQ12,FieldGridAsset *fieldGrid)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint extraout_EAX_01;
  uint extraout_EAX_02;
  uint extraout_EAX_03;
  uint extraout_EAX_04;
  FieldGridCell *pFVar4;
  uint uVar5;
  int iVar6;
  FieldGridCell *pFVar7;
  FieldGridCell *pFVar8;
  bool bVar9;
  qword qVar10;
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
    qVar10 = FieldGrid_WorldToGridQ12(worldXQ12,worldYQ12);
    uVar2 = (int)qVar10 >> 0xc;
    uVar11 = (uint)((longlong)qVar10 >> 0x2c);
    uVar3 = (uint)(qVar10 & 0xfff00000fff);
    uVar5 = (uint)((qVar10 & 0xfff00000fff) >> 0x20);
    uVar1 = uVar5 + uVar3 * 2;
    radiusWorldUnits = uVar2;
    if (uVar1 < 0x1000) {
      if (0xfff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    else if (uVar1 < 0x2001) {
      radiusWorldUnits = uVar2 + 1;
      if (uVar3 < uVar5) {
        uVar11 = uVar11 + 1;
        radiusWorldUnits = uVar2;
      }
    }
    else {
      radiusWorldUnits = uVar2 + 1;
      if (0x1fff < uVar3 + uVar5 * 2) {
        uVar11 = uVar11 + 1;
      }
    }
    uVar1 = fieldGrid->gridWidth;
    pFVar4 = (FieldGridCell *)(uVar1 * 0x80);
    g_TerrainScanRowStrideBytes = pFVar4;
    if ((((-1 < radiusWorldUnits) && (-1 < (int)uVar11)) && (uVar11 < fieldGrid->gridHeight)) &&
       ((uint)radiusWorldUnits < (uVar1 & 0x1ffffff))) {
      iVar6 = uVar11 * (uVar1 & 0x1ffffff) + radiusWorldUnits;
      if ((((fieldGrid->cells[iVar6].flagsAndMaterial & 0x88006000) == 0) &&
          (-1 < fieldGrid->cells[iVar6].waterSurfaceDelta)) &&
         (g_TerrainAuxHeightMinimum <= (int)fieldGrid->cells[iVar6].triangle0NormalAngles >> 0x10))
      {
        pFVar7 = (FieldGridCell *)
                 (fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 +
                 iVar6 * 0x80 + -0x28);
        bVar9 = pFVar7 < pFVar4;
        pFVar8 = pFVar7 + -uVar1;
        TerrainAuxHeightThreshold_TestWedge0(0,pFVar7);
        radiusWorldUnits = extraout_EAX;
        if (!bVar9) {
          bVar9 = pFVar8 < (FieldGridCell *)0x80;
          TerrainAuxHeightThreshold_TestWedge1(0,pFVar8);
          radiusWorldUnits = extraout_EAX_00;
          if (!bVar9) {
            pFVar7 = (FieldGridCell *)(pFVar8[-1].runtime00_07 + (int)pFVar4[-1].runtime00_07);
            TerrainAuxHeightThreshold_TestWedge2(0,pFVar8 + -1);
            radiusWorldUnits = extraout_EAX_01;
            if (!bVar9) {
              bVar9 = CARRY4((uint)pFVar7,(uint)pFVar4);
              pFVar8 = pFVar7 + uVar1;
              TerrainAuxHeightThreshold_TestWedge3(0,pFVar7);
              radiusWorldUnits = extraout_EAX_02;
              if (!bVar9) {
                bVar9 = (FieldGridCell *)0xffffff7f < pFVar8;
                TerrainAuxHeightThreshold_TestWedge4(0,pFVar8);
                radiusWorldUnits = extraout_EAX_03;
                if (!bVar9) {
                  bVar9 = pFVar8 + 1 < pFVar4;
                  TerrainAuxHeightThreshold_TestWedge5(0,pFVar8 + 1);
                  radiusWorldUnits = extraout_EAX_04;
                  if (!bVar9) {
                    return extraout_EAX_04;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return radiusWorldUnits;
}

/* Address: 0x00508D20.
   Ownership: world/terrain/height.
   Purpose: Applies one terrain-height propagation wedge and invokes the two adjacent directional mutators. Typed
   parameters: p0 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightDelta_ApplyDirection0, TerrainHeightDelta_ApplyDirection1.
*/
void TerrainHeightDelta_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *piVar1;
  int iVar2;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
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
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
      TerrainHeightDelta_ApplyDirection1
                (extraout_ECX + 3,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainHeightDelta_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int *piVar1;
  int iVar2;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
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
      if (g_TerrainScanStepLimit <= extraout_ECX) {
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
      cell = cell_00 + 1;
      TerrainHeightDelta_ApplyDirection2(extraout_ECX + 3,cell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainHeightDelta_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      TerrainHeightDelta_ApplyDirection2
                (scanStep + 4,(FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[-1].terrainHeight;
      cell[-1].terrainHeight = cell[-1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[-1].waterSurfaceDelta = cell[-1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      cell_00 = cell + -2;
      cell = (FieldGridCell *)((int)cell + (-0x80 - g_TerrainScanRowStrideBytes));
      TerrainHeightDelta_ApplyDirection3(extraout_ECX + 3,cell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainHeightDelta_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar1 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar1;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar1;
      iVar1 = g_TerrainScanRowStrideBytes;
      cell_00 = cell + -1;
      TerrainHeightDelta_ApplyDirection3(scanStep + 4,cell_00);
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell_00->runtime58_6F + iVar1 + -8) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(cell_00->runtime58_6F + iVar1 + -0x10);
      *(int *)(cell_00->runtime58_6F + iVar1 + -0x10) =
           *(int *)(cell_00->runtime58_6F + iVar1 + -0x10) + adjacentHeightAdjustmentQ12;
      *(int *)(cell_00->runtime58_6F + iVar1 + -0xc) =
           *(int *)(cell_00->runtime58_6F + iVar1 + -0xc) - adjacentHeightAdjustmentQ12;
      cell = (FieldGridCell *)(cell_00[-1].runtime00_07 + iVar1);
      TerrainHeightDelta_ApplyDirection4
                (extraout_ECX + 3,
                 (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainHeightDelta_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
  byte *currentCellRuntimeBase;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      iVar1 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + iVar1;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - iVar1;
      iVar1 = g_TerrainScanRowStrideBytes;
      TerrainHeightDelta_ApplyDirection4
                (scanStep + 4,(FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes)
                );
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell->runtime58_6F + iVar1 + -8) & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 =
           g_TerrainScanReferenceHeight - *(int *)(cell->runtime58_6F + iVar1 + -0x10);
      *(int *)(cell->runtime58_6F + iVar1 + -0x10) =
           *(int *)(cell->runtime58_6F + iVar1 + -0x10) + adjacentHeightAdjustmentQ12;
      *(int *)(cell->runtime58_6F + iVar1 + -0xc) =
           *(int *)(cell->runtime58_6F + iVar1 + -0xc) - adjacentHeightAdjustmentQ12;
      currentCellRuntimeBase = cell->runtime00_07;
      cell = (FieldGridCell *)(currentCellRuntimeBase + g_TerrainScanRowStrideBytes + iVar1) + -1;
      TerrainHeightDelta_ApplyDirection5
                (extraout_ECX + 3,
                 (FieldGridCell *)(currentCellRuntimeBase + g_TerrainScanRowStrideBytes + iVar1));
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
void TerrainHeightDelta_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int heightAdjustmentQ12;
  int adjacentHeightAdjustmentQ12;
  uint extraout_ECX;
  uint extraout_ECX_00;
  
  if (scanStep < g_TerrainScanStepLimit) {
    while ((cell->flagsAndMaterial & 0x88006000) == 0) {
      heightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell->terrainHeight;
      cell->terrainHeight = cell->terrainHeight + heightAdjustmentQ12;
      cell->waterSurfaceDelta = cell->waterSurfaceDelta - heightAdjustmentQ12;
      TerrainHeightDelta_ApplyDirection5
                (scanStep + 4,(FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      adjacentHeightAdjustmentQ12 = g_TerrainScanReferenceHeight - cell[1].terrainHeight;
      cell[1].terrainHeight = cell[1].terrainHeight + adjacentHeightAdjustmentQ12;
      cell[1].waterSurfaceDelta = cell[1].waterSurfaceDelta - adjacentHeightAdjustmentQ12;
      cell_00 = cell + 2;
      cell = (FieldGridCell *)(cell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      TerrainHeightDelta_ApplyDirection0(extraout_ECX + 3,cell_00);
      scanStep = extraout_ECX_00;
      if (g_TerrainScanStepLimit <= extraout_ECX_00) {
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
longlong TerrainTriangle_IntersectRayDistanceCf
                   (Q12 rayDeltaZQ12,Q12 gridRayDelta0Q12,Q12 gridRayDelta1Q12,Q12 rayOriginZQ12,
                   Q12 cornerHeight0Q12,Q12 cornerHeight1Q12,Q12 cornerHeight2Q12,
                   Q12 cornerHeight3Q12,Q12 cellLocalCoord1Q12,Q12 cellLocalCoord0Q12)

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
  dword dVar11;
  uint uVar12;
  int iVar13;
  uint in_EDX;
  Q12 QVar14;
  uint uVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  
  QVar14 = rayOriginZQ12;
  if (rayDeltaZQ12 < 0) {
    QVar14 = rayOriginZQ12 + rayDeltaZQ12;
  }
  if ((((cornerHeight3Q12 <= QVar14) && (cornerHeight2Q12 <= QVar14)) &&
      (cornerHeight1Q12 <= QVar14)) && (uVar6 = cornerHeight2Q12, cornerHeight0Q12 <= QVar14))
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
    iVar16 = (int)(uVar5 >> 0x20);
    uVar6 = cornerHeight1Q12 - cornerHeight3Q12;
    if ((int)uVar6 < 0) {
      lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
      uVar7 = (uint)lVar4;
      iVar17 = (iVar16 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
    }
    else {
      lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
      uVar7 = (uint)lVar4;
      iVar17 = iVar16 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
    }
    lVar4 = (longlong)(rayOriginZQ12 - cornerHeight3Q12) * (longlong)gridRayDelta1Q12;
    uVar6 = (uint)lVar4;
    uVar8 = uVar6 * 0x1000;
    uVar20 = uVar7 + uVar8;
    uVar9 = (uint)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord0Q12);
    uVar10 = uVar9 * 0x1000;
    uVar21 = uVar20 + uVar10;
    iVar17 = iVar17 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar6 >> 0x14) +
             (uint)CARRY4(uVar7,uVar8) +
             ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord0Q12) >> 0x20) <<
              0xc | uVar9 >> 0x14) + (uint)CARRY4(uVar20,uVar10);
    if (iVar17 < 0) {
      if (lVar1 < 0) {
TerrainTriangle_IntersectRayDistanceCf_EvaluateFirstTriangleRemainingEdgeConstraints:
        uVar6 = cornerHeight3Q12 - cornerHeight2Q12;
        if ((int)uVar6 < 0) {
          lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
          uVar7 = (uint)lVar4;
          iVar16 = (iVar16 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
        }
        else {
          lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
          uVar7 = (uint)lVar4;
          iVar16 = iVar16 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
        }
        lVar4 = (longlong)(rayOriginZQ12 - cornerHeight3Q12) * (longlong)gridRayDelta0Q12;
        uVar6 = (uint)lVar4;
        uVar8 = uVar6 * 0x1000;
        uVar22 = uVar7 + uVar8;
        uVar9 = (uint)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord1Q12);
        uVar10 = uVar9 * 0x1000;
        uVar12 = (int)((ulonglong)lVar1 >> 0x20) << 0xc | (uint)lVar1 >> 0x14;
        uVar15 = (uint)lVar1 * 0x1000;
        uVar20 = uVar10 + uVar22;
        iVar16 = ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)cellLocalCoord1Q12) >> 0x20)
                  << 0xc | uVar9 >> 0x14) +
                 iVar16 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar6 >> 0x14) +
                 (uint)CARRY4(uVar7,uVar8) + (uint)CARRY4(uVar10,uVar22);
        if (iVar16 < 0) {
          if ((int)uVar12 < 0) {
            uVar6 = (uVar15 - uVar20) - uVar21;
            iVar13 = (((uVar12 - iVar16) - (uint)(uVar15 < uVar20)) - iVar17) -
                     (uint)(uVar15 - uVar20 < uVar21);
            if (iVar13 < 0) {
TerrainTriangle_IntersectRayDistanceCf_ComputeFirstTriangleIntersectionDistance:
              iVar13 = iVar13 + iVar16 + (uint)CARRY4(uVar6,uVar20) + iVar17 +
                       (uint)CARRY4(uVar6 + uVar20,uVar21);
              uVar6 = uVar6 + uVar20 + uVar21 >> 0xc | iVar13 * 0x100000;
              iVar13 = iVar13 >> 0xc;
              if (uVar6 != 0) {
                if ((int)uVar6 < 0) {
                  if (iVar13 != -1) {
                    uVar21 = uVar21 >> 0xc | iVar17 * 0x100000;
                    uVar20 = uVar20 >> 0xc | iVar16 * 0x100000;
                    uVar6 = uVar6 >> 0xc | iVar13 << 0x14;
                    iVar16 = iVar16 >> 0xc;
                    iVar17 = iVar17 >> 0xc;
                  }
                }
                else if (iVar13 != 0) {
                  uVar21 = uVar21 >> 0xc | iVar17 * 0x100000;
                  uVar20 = uVar20 >> 0xc | iVar16 * 0x100000;
                  uVar6 = uVar6 >> 0xc | iVar13 << 0x14;
                  iVar16 = iVar16 >> 0xc;
                  iVar17 = iVar17 >> 0xc;
                }
                iVar16 = (int)(CONCAT44(iVar16,uVar20) / (longlong)(int)uVar6);
                iVar13 = cellLocalCoord1Q12 + iVar16;
                iVar17 = (int)(CONCAT44(iVar17,uVar21) / (longlong)(int)uVar6);
                lVar4 = (longlong)iVar17 * (longlong)(cornerHeight2Q12 - cornerHeight3Q12);
                lVar1 = (longlong)iVar16 * (longlong)(cornerHeight1Q12 - cornerHeight3Q12);
                lVar2 = (longlong)(iVar13 + (cellLocalCoord0Q12 + iVar17) * 2) * 0x901;
                lVar3 = (longlong)iVar13 * -1999;
                dVar11 = FixedMath_Length3(((cornerHeight3Q12 +
                                            ((int)((ulonglong)lVar4 >> 0x20) << 0x14 |
                                            (uint)lVar4 >> 0xc)) - rayOriginZQ12) +
                                           ((int)((ulonglong)lVar1 >> 0x20) << 0x14 |
                                           (uint)lVar1 >> 0xc),
                                           (int)((ulonglong)lVar3 >> 0x20) << 0x14 |
                                           (uint)lVar3 >> 0xc,
                                           (int)((ulonglong)lVar2 >> 0x20) << 0x13 |
                                           (uint)lVar2 >> 0xd);
                return CONCAT44(in_EDX,dVar11);
              }
              goto TerrainTriangle_IntersectRayDistanceCf_ReturnZeroWithCarryClear;
            }
          }
        }
        else if (-1 < (int)uVar12) {
          uVar6 = (uVar15 - uVar20) - uVar21;
          iVar13 = (((uVar12 - iVar16) - (uint)(uVar15 < uVar20)) - iVar17) -
                   (uint)(uVar15 - uVar20 < uVar21);
          if (-1 < iVar13)
          goto TerrainTriangle_IntersectRayDistanceCf_ComputeFirstTriangleIntersectionDistance;
        }
      }
    }
    else if (-1 < lVar1)
    goto TerrainTriangle_IntersectRayDistanceCf_EvaluateFirstTriangleRemainingEdgeConstraints;
  }
  iVar16 = cellLocalCoord0Q12 + 0x1000;
  iVar17 = cellLocalCoord1Q12 + 0x1000;
  lVar4 = (longlong)(cornerHeight2Q12 - cornerHeight0Q12) * (longlong)gridRayDelta0Q12 +
          (longlong)(cornerHeight1Q12 - cornerHeight0Q12) * (longlong)gridRayDelta1Q12 +
          CONCAT44(rayDeltaZQ12 >> 0x14,rayDeltaZQ12 << 0xc);
  uVar7 = (uint)lVar4;
  iVar13 = (int)((ulonglong)lVar4 >> 0x20);
  uVar6 = (cornerHeight0Q12 - rayOriginZQ12) * 0x1000;
  lVar1 = (longlong)(cornerHeight2Q12 - cornerHeight0Q12) * (longlong)iVar17 +
          (longlong)(cornerHeight1Q12 - cornerHeight0Q12) * (longlong)iVar16 +
          CONCAT44(cornerHeight0Q12 - rayOriginZQ12 >> 0x14,uVar6);
  iVar18 = (int)((ulonglong)lVar1 >> 0x20);
  if (lVar1 < 0) {
    if ((-1 < lVar4) || ((int)((iVar18 - iVar13) - (uint)((uint)lVar1 < uVar7)) < 0))
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if ((lVar4 < 0) || (-1 < (int)((iVar18 - iVar13) - (uint)((uint)lVar1 < uVar7))))
  goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  uVar5 = (longlong)gridRayDelta1Q12 * (longlong)iVar17 -
          (longlong)iVar16 * (longlong)gridRayDelta0Q12;
  iVar18 = (int)(uVar5 >> 0x20);
  uVar6 = cornerHeight0Q12 - cornerHeight2Q12;
  if ((int)uVar6 < 0) {
    lVar1 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar1;
    iVar19 = (iVar18 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar1 >> 0x20);
  }
  else {
    lVar1 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar1;
    iVar19 = iVar18 * uVar6 + (int)((ulonglong)lVar1 >> 0x20);
  }
  lVar1 = (longlong)(rayOriginZQ12 - cornerHeight0Q12) * (longlong)gridRayDelta1Q12;
  uVar9 = (uint)lVar1;
  uVar10 = uVar9 * 0x1000;
  uVar21 = uVar8 + uVar10;
  uVar20 = (uint)((longlong)rayDeltaZQ12 * (longlong)iVar16);
  uVar6 = uVar20 * 0x1000;
  uVar12 = uVar21 + uVar6;
  iVar19 = iVar19 + ((int)((ulonglong)lVar1 >> 0x20) << 0xc | uVar9 >> 0x14) +
           (uint)CARRY4(uVar8,uVar10) +
           ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)iVar16) >> 0x20) << 0xc |
           uVar20 >> 0x14) + (uint)CARRY4(uVar21,uVar6);
  if (iVar19 < 0) {
    if (-1 < lVar4)
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else if (lVar4 < 0)
  goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  uVar6 = cornerHeight1Q12 - cornerHeight0Q12;
  if ((int)uVar6 < 0) {
    lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar4;
    iVar18 = (iVar18 * uVar6 - (int)uVar5) + (int)((ulonglong)lVar4 >> 0x20);
  }
  else {
    lVar4 = (ulonglong)uVar6 * (uVar5 & 0xffffffff);
    uVar8 = (uint)lVar4;
    iVar18 = iVar18 * uVar6 + (int)((ulonglong)lVar4 >> 0x20);
  }
  lVar4 = (longlong)(rayOriginZQ12 - cornerHeight0Q12) * (longlong)gridRayDelta0Q12;
  uVar9 = (uint)lVar4;
  uVar10 = uVar9 * 0x1000;
  uVar22 = uVar8 + uVar10;
  uVar20 = (uint)((longlong)rayDeltaZQ12 * (longlong)iVar17);
  uVar21 = uVar20 * 0x1000;
  uVar15 = iVar13 << 0xc | uVar7 >> 0x14;
  uVar7 = uVar7 * 0x1000;
  uVar6 = uVar21 + uVar22;
  iVar13 = ((int)((ulonglong)((longlong)rayDeltaZQ12 * (longlong)iVar17) >> 0x20) << 0xc |
           uVar20 >> 0x14) +
           iVar18 + ((int)((ulonglong)lVar4 >> 0x20) << 0xc | uVar9 >> 0x14) +
           (uint)CARRY4(uVar8,uVar10) + (uint)CARRY4(uVar21,uVar22);
  if (iVar13 < 0) {
    if (-1 < (int)uVar15) {
TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet:
      return CONCAT44(in_EDX,uVar6);
    }
    uVar8 = (uVar7 - uVar6) - uVar12;
    iVar18 = (((uVar15 - iVar13) - (uint)(uVar7 < uVar6)) - iVar19) - (uint)(uVar7 - uVar6 < uVar12)
    ;
    if (-1 < iVar18)
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  else {
    if ((int)uVar15 < 0)
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
    uVar8 = (uVar7 - uVar6) - uVar12;
    iVar18 = (((uVar15 - iVar13) - (uint)(uVar7 < uVar6)) - iVar19) - (uint)(uVar7 - uVar6 < uVar12)
    ;
    if (iVar18 < 0)
    goto TerrainTriangle_IntersectRayDistanceCf_ReturnHeightOrEdgeRejectWithCarrySet;
  }
  iVar18 = iVar18 + iVar13 + (uint)CARRY4(uVar8,uVar6) + iVar19 + (uint)CARRY4(uVar8 + uVar6,uVar12)
  ;
  uVar7 = uVar8 + uVar6 + uVar12 >> 0xc | iVar18 * 0x100000;
  iVar18 = iVar18 >> 0xc;
  if (uVar7 != 0) {
    if ((int)uVar7 < 0) {
      if (iVar18 != -1) {
        uVar12 = uVar12 >> 0xc | iVar19 * 0x100000;
        uVar6 = uVar6 >> 0xc | iVar13 * 0x100000;
        uVar7 = uVar7 >> 0xc | iVar18 << 0x14;
        iVar13 = iVar13 >> 0xc;
        iVar19 = iVar19 >> 0xc;
      }
    }
    else if (iVar18 != 0) {
      uVar12 = uVar12 >> 0xc | iVar19 * 0x100000;
      uVar6 = uVar6 >> 0xc | iVar13 * 0x100000;
      uVar7 = uVar7 >> 0xc | iVar18 << 0x14;
      iVar13 = iVar13 >> 0xc;
      iVar19 = iVar19 >> 0xc;
    }
    iVar13 = (int)(CONCAT44(iVar13,uVar6) / (longlong)(int)uVar7);
    iVar17 = iVar17 - iVar13;
    iVar18 = (int)(CONCAT44(iVar19,uVar12) / (longlong)(int)uVar7);
    lVar4 = (longlong)iVar18 * (longlong)(cornerHeight1Q12 - cornerHeight0Q12);
    lVar1 = (longlong)iVar13 * (longlong)(cornerHeight2Q12 - cornerHeight0Q12);
    lVar2 = (longlong)(iVar17 + (iVar16 - iVar18) * 2) * 0x901;
    lVar3 = (longlong)iVar17 * -1999;
    dVar11 = FixedMath_Length3(((cornerHeight0Q12 +
                                ((int)((ulonglong)lVar4 >> 0x20) << 0x14 | (uint)lVar4 >> 0xc)) -
                               rayOriginZQ12) +
                               ((int)((ulonglong)lVar1 >> 0x20) << 0x14 | (uint)lVar1 >> 0xc),
                               (int)((ulonglong)lVar3 >> 0x20) << 0x14 | (uint)lVar3 >> 0xc,
                               (int)((ulonglong)lVar2 >> 0x20) << 0x13 | (uint)lVar2 >> 0xd);
    return CONCAT44(in_EDX,dVar11);
  }
TerrainTriangle_IntersectRayDistanceCf_ReturnZeroWithCarryClear:
  return (ulonglong)in_EDX << 0x20;
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
undefined8
TerrainRay_AdvanceGridTraversalCf
          (Q12 rayEndCoord0Q12,Q12 rayEndCoord1Q12,Q12 rayStartCoord0Q12,Q12 rayStartCoord1Q12,
          undefined4 param_5,undefined4 param_6,Q12 currentGridCoord0Q12,Q12 currentGridCoord1Q12)

{
  longlong lVar1;
  undefined4 in_EAX;
  int iVar2;
  longlong boundaryCrossProductQ24;
  
  if ((((rayEndCoord1Q12 < currentGridCoord1Q12) || (rayEndCoord0Q12 < currentGridCoord0Q12)) ||
      (0x1000 < rayEndCoord1Q12 - currentGridCoord1Q12)) ||
     (iVar2 = rayEndCoord0Q12 - currentGridCoord0Q12,
     0x1000 < rayEndCoord0Q12 - currentGridCoord0Q12)) {
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
          currentGridCoord0Q12 = currentGridCoord0Q12 + -0x1000;
          goto TerrainRay_AdvanceGridTraversal_ReturnCurrentGridCoord0;
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
          currentGridCoord0Q12 = currentGridCoord0Q12 + 0x1000;
          goto TerrainRay_AdvanceGridTraversal_ReturnCurrentGridCoord0;
        }
      }
    }
    iVar2 = currentGridCoord0Q12;
    if (rayEndCoord1Q12 != rayStartCoord1Q12) {
TerrainRay_AdvanceGridTraversal_ReturnCurrentGridCoord0:
      return CONCAT44(currentGridCoord0Q12,in_EAX);
    }
  }
  return CONCAT44(iVar2,in_EAX);
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
void TerrainHeightBand_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *pFVar1;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      pFVar1 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar2 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar2)) ||
         (iVar2 < g_TerrainHeightBandMinimumDelta)) {
        return;
      }
      cell_00 = cell + 1;
      bVar3 = cell_00 < g_TerrainScanRowStrideBytes;
      TerrainHeightBand_TestDirection0(scanStep + 4,cell_00);
      if (bVar3) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - (int)pFVar1)) & 0x88006000) != 0) {
        return;
      }
      iVar2 = *(int *)((int)cell_00 + (0x48 - (int)pFVar1)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)((int)cell_00 + (0x4c - (int)pFVar1))) {
        return;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar2) {
        return;
      }
      if (iVar2 < g_TerrainHeightBandMinimumDelta) {
        return;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - (int)pFVar1));
      bVar3 = CARRY4(-(int)g_TerrainScanRowStrideBytes,(uint)cell);
      TerrainHeightBand_TestDirection1
                (extraout_ECX + 3,(FieldGridCell *)(-(int)g_TerrainScanRowStrideBytes + (int)cell));
      if (bVar3) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00507BA0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection1, TerrainHeightBand_TestDirection2.
*/
undefined8 TerrainHeightBand_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar2 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar2)) ||
         (iVar2 < g_TerrainHeightBandMinimumDelta)) {
TerrainHeightBand_TestWedge1_ReturnAfterCellRejectOrTraversalBoundary:
        return CONCAT44(in_EDX,in_EAX);
      }
      bVar3 = 0xfffffffb < scanStep;
      TerrainHeightBand_TestDirection1
                (scanStep + 4,(FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (bVar3) goto TerrainHeightBand_TestWedge1_ReturnAfterCellRejectOrTraversalBoundary;
      if (g_TerrainScanStepLimit <= extraout_ECX) break;
      if ((((*(uint *)((int)cell + (0x50 - iVar1)) & 0x88006000) != 0) ||
          (iVar2 = *(int *)((int)cell + (0x48 - iVar1)) - g_TerrainScanReferenceHeight,
          0 < *(int *)((int)cell + (0x4c - iVar1)))) ||
         ((g_TerrainHeightBandMaximumDelta < iVar2 || (iVar2 < g_TerrainHeightBandMinimumDelta))))
      goto TerrainHeightBand_TestWedge1_ReturnAfterCellRejectOrTraversalBoundary;
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar1));
      bVar3 = (FieldGridCell *)0xffffff7f < cell_00;
      cell = cell_00 + 1;
      TerrainHeightBand_TestDirection2(extraout_ECX + 3,cell_00);
      if (bVar3) goto TerrainHeightBand_TestWedge1_ReturnAfterCellRejectOrTraversalBoundary;
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return CONCAT44(in_EDX,in_EAX);
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
void TerrainHeightBand_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int iVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar1 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar1)) ||
         (iVar1 < g_TerrainHeightBandMinimumDelta)) {
        return;
      }
      bVar2 = cell < (FieldGridCell *)0x80;
      TerrainHeightBand_TestDirection2
                (scanStep + 4,(FieldGridCell *)((int)cell - (int)g_TerrainScanRowStrideBytes));
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar1 = cell[-1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[-1].waterSurfaceDelta) {
        return;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar1) {
        return;
      }
      if (iVar1 < g_TerrainHeightBandMinimumDelta) {
        return;
      }
      cell_00 = cell + -2;
      bVar2 = cell + -1 < g_TerrainScanRowStrideBytes;
      cell = (FieldGridCell *)((int)(cell + -1) - (int)g_TerrainScanRowStrideBytes);
      TerrainHeightBand_TestDirection3(extraout_ECX + 3,cell_00);
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00507D60.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection3, TerrainHeightBand_TestDirection4.
*/
void TerrainHeightBand_TestWedge3(TerrainDirectionalScanStep scanStep,byte *cell)

{
  uint uVar1;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      uVar1 = g_TerrainScanRowStrideBytes;
      if (((((*(FieldCellPackedFlagsAndMaterial *)(cell + 0x50) & 0x88006000) != 0) ||
           (0 < *(int *)(cell + 0x4c))) ||
          (g_TerrainHeightBandMaximumDelta < *(int *)(cell + 0x48) - g_TerrainScanReferenceHeight))
         || (*(int *)(cell + 0x48) - g_TerrainScanReferenceHeight < g_TerrainHeightBandMinimumDelta)
         ) {
        return;
      }
      cell_00 = (FieldGridCell *)(cell + -0x80);
      bVar3 = CARRY4((uint)cell_00,g_TerrainScanRowStrideBytes);
      TerrainHeightBand_TestDirection3(scanStep + 4,cell_00);
      if (bVar3) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell_00->runtime58_6F + (uVar1 - 8)) & 0x88006000) != 0) {
        return;
      }
      iVar2 = *(int *)(cell_00->runtime58_6F + (uVar1 - 0x10)) - g_TerrainScanReferenceHeight;
      if (0 < *(int *)(cell_00->runtime58_6F + (uVar1 - 0xc))) {
        return;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar2) {
        return;
      }
      if (iVar2 < g_TerrainHeightBandMinimumDelta) {
        return;
      }
      cell = cell_00[-1].runtime00_07 + uVar1;
      bVar3 = CARRY4(g_TerrainScanRowStrideBytes,(uint)cell);
      TerrainHeightBand_TestDirection4
                (extraout_ECX + 3,(FieldGridCell *)(cell + g_TerrainScanRowStrideBytes));
      if (bVar3) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00507E40.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4, stops when a cell leaves the configured height band or
   becomes excluded, and preserves the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainHeightBand_TestDirection4, TerrainHeightBand_TestDirection5.
*/
undefined8 TerrainHeightBand_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  undefined4 in_EDX;
  FieldGridCell *cell_00;
  bool bVar3;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar2 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar2)) ||
         (iVar2 < g_TerrainHeightBandMinimumDelta)) {
TerrainHeightBand_TestWedge4_ReturnAfterCellRejectOrTraversalBoundary:
        return CONCAT44(in_EDX,in_EAX);
      }
      bVar3 = 0xfffffffb < scanStep;
      TerrainHeightBand_TestDirection4
                (scanStep + 4,(FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes)
                );
      if (bVar3) goto TerrainHeightBand_TestWedge4_ReturnAfterCellRejectOrTraversalBoundary;
      if (g_TerrainScanStepLimit <= extraout_ECX) break;
      if ((((*(uint *)(cell->runtime58_6F + iVar1 + -8) & 0x88006000) != 0) ||
          (iVar2 = *(int *)(cell->runtime58_6F + iVar1 + -0x10) - g_TerrainScanReferenceHeight,
          0 < *(int *)(cell->runtime58_6F + iVar1 + -0xc))) ||
         ((g_TerrainHeightBandMaximumDelta < iVar2 || (iVar2 < g_TerrainHeightBandMinimumDelta))))
      goto TerrainHeightBand_TestWedge4_ReturnAfterCellRejectOrTraversalBoundary;
      cell_00 = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes + iVar1);
      bVar3 = cell_00 < (FieldGridCell *)0x80;
      cell = cell_00 + -1;
      TerrainHeightBand_TestDirection5(extraout_ECX + 3,cell_00);
      if (bVar3) goto TerrainHeightBand_TestWedge4_ReturnAfterCellRejectOrTraversalBoundary;
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return CONCAT44(in_EDX,in_EAX);
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
void TerrainHeightBand_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  int iVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
           (iVar1 = cell->terrainHeight - g_TerrainScanReferenceHeight, 0 < cell->waterSurfaceDelta)
           ) || (g_TerrainHeightBandMaximumDelta < iVar1)) ||
         (iVar1 < g_TerrainHeightBandMinimumDelta)) {
        return;
      }
      bVar2 = (FieldGridCell *)0xffffff7f < cell;
      TerrainHeightBand_TestDirection5
                (scanStep + 4,(FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      iVar1 = cell[1].terrainHeight - g_TerrainScanReferenceHeight;
      if (0 < cell[1].waterSurfaceDelta) {
        return;
      }
      if (g_TerrainHeightBandMaximumDelta < iVar1) {
        return;
      }
      if (iVar1 < g_TerrainHeightBandMinimumDelta) {
        return;
      }
      cell_00 = cell + 2;
      bVar2 = CARRY4((uint)(cell + 1),g_TerrainScanRowStrideBytes);
      cell = (FieldGridCell *)(cell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      TerrainHeightBand_TestDirection0(extraout_ECX + 3,cell_00);
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00508470.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 0 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection0, TerrainAuxHeightThreshold_TestDirection1.
*/
void TerrainAuxHeightThreshold_TestWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *pFVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      pFVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      cell_00 = cell + 1;
      bVar2 = cell_00 < g_TerrainScanRowStrideBytes;
      TerrainAuxHeightThreshold_TestDirection0(scanStep + 4,cell_00);
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)cell_00 + (0x50 - (int)pFVar1)) & 0x88006000) != 0) {
        return;
      }
      if (*(int *)((int)cell_00 + (0x4c - (int)pFVar1)) < 0) {
        return;
      }
      if (*(int *)((int)cell_00 + (0x78 - (int)pFVar1)) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell = (FieldGridCell *)((int)cell_00 + (0x80 - (int)pFVar1));
      bVar2 = CARRY4(-(int)g_TerrainScanRowStrideBytes,(uint)cell);
      TerrainAuxHeightThreshold_TestDirection1
                (extraout_ECX + 3,(FieldGridCell *)(-(int)g_TerrainScanRowStrideBytes + (int)cell));
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00508540.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 1 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection1, TerrainAuxHeightThreshold_TestDirection2.
*/
void TerrainAuxHeightThreshold_TestWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      bVar2 = 0xfffffffb < scanStep;
      TerrainAuxHeightThreshold_TestDirection1
                (scanStep + 4,(FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes)));
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)((int)cell + (0x50 - iVar1)) & 0x88006000) != 0) {
        return;
      }
      if (*(int *)((int)cell + (0x4c - iVar1)) < 0) {
        return;
      }
      if (*(int *)((int)cell + (0x78 - iVar1)) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell_00 = (FieldGridCell *)((int)cell + (-g_TerrainScanRowStrideBytes - iVar1));
      bVar2 = (FieldGridCell *)0xffffff7f < cell_00;
      cell = cell_00 + 1;
      TerrainAuxHeightThreshold_TestDirection2(extraout_ECX + 3,cell_00);
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00508600.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 2 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection2, TerrainAuxHeightThreshold_TestDirection3.
*/
void TerrainAuxHeightThreshold_TestWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  bool bVar1;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      bVar1 = cell < (FieldGridCell *)0x80;
      TerrainAuxHeightThreshold_TestDirection2
                (scanStep + 4,(FieldGridCell *)((int)cell - (int)g_TerrainScanRowStrideBytes));
      if (bVar1) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[-1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (cell[-1].waterSurfaceDelta < 0) {
        return;
      }
      if ((int)cell[-1].triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell_00 = cell + -2;
      bVar1 = cell + -1 < g_TerrainScanRowStrideBytes;
      cell = (FieldGridCell *)((int)(cell + -1) - (int)g_TerrainScanRowStrideBytes);
      TerrainAuxHeightThreshold_TestDirection3(extraout_ECX + 3,cell_00);
      if (bVar1) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x005086C0.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 3 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection3, TerrainAuxHeightThreshold_TestDirection4.
*/
void TerrainAuxHeightThreshold_TestWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  uint uVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      uVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      cell_00 = cell + -1;
      bVar2 = CARRY4((uint)cell_00,g_TerrainScanRowStrideBytes);
      TerrainAuxHeightThreshold_TestDirection3(scanStep + 4,cell_00);
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell_00->runtime58_6F + (uVar1 - 8)) & 0x88006000) != 0) {
        return;
      }
      if (*(int *)(cell_00->runtime58_6F + (uVar1 - 0xc)) < 0) {
        return;
      }
      if (*(int *)(cell_00->runtime58_6F + uVar1 + 0x20) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell = (FieldGridCell *)(cell_00[-1].runtime00_07 + uVar1);
      bVar2 = CARRY4(g_TerrainScanRowStrideBytes,(uint)cell);
      TerrainAuxHeightThreshold_TestDirection4
                (extraout_ECX + 3,
                 (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00508790.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 4 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection4, TerrainAuxHeightThreshold_TestDirection5.
*/
void TerrainAuxHeightThreshold_TestWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  int iVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  FieldGridCell *cell_00;
  bool bVar2;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      iVar1 = g_TerrainScanRowStrideBytes;
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      bVar2 = 0xfffffffb < scanStep;
      TerrainAuxHeightThreshold_TestDirection4
                (scanStep + 4,(FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes)
                );
      if (bVar2) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((*(uint *)(cell->runtime58_6F + iVar1 + -8) & 0x88006000) != 0) {
        return;
      }
      if (*(int *)(cell->runtime58_6F + iVar1 + -0xc) < 0) {
        return;
      }
      if (*(int *)(cell->runtime58_6F + iVar1 + 0x20) >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell_00 = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes + iVar1);
      bVar2 = cell_00 < (FieldGridCell *)0x80;
      cell = cell_00 + -1;
      TerrainAuxHeightThreshold_TestDirection5(extraout_ECX + 3,cell_00);
      if (bVar2) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x00508850.
   Ownership: world/terrain/height.
   Purpose: Tests both directional legs of terrain wedge 5 against the shared auxiliary-height threshold, stopping
   at excluded or invalid cells and preserving the verified carry-style failure path. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: TerrainAuxHeightThreshold_TestDirection5, TerrainAuxHeightThreshold_TestDirection0.
*/
void TerrainAuxHeightThreshold_TestWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  FieldGridCell *cell_00;
  uint extraout_ECX;
  uint extraout_ECX_00;
  bool bVar1;
  
  if (scanStep < g_TerrainScanStepLimit) {
    do {
      if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
         ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) {
        return;
      }
      bVar1 = (FieldGridCell *)0xffffff7f < cell;
      TerrainAuxHeightThreshold_TestDirection5
                (scanStep + 4,(FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes));
      if (bVar1) {
        return;
      }
      if (g_TerrainScanStepLimit <= extraout_ECX) {
        return;
      }
      if ((cell[1].flagsAndMaterial & 0x88006000) != 0) {
        return;
      }
      if (cell[1].waterSurfaceDelta < 0) {
        return;
      }
      if ((int)cell[1].triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum) {
        return;
      }
      cell_00 = cell + 2;
      bVar1 = CARRY4((uint)(cell + 1),g_TerrainScanRowStrideBytes);
      cell = (FieldGridCell *)(cell[1].runtime00_07 + g_TerrainScanRowStrideBytes);
      TerrainAuxHeightThreshold_TestDirection0(extraout_ECX + 3,cell_00);
      if (bVar1) {
        return;
      }
      scanStep = extraout_ECX_00;
    } while (extraout_ECX_00 < g_TerrainScanStepLimit);
  }
  return;
}

/* Address: 0x005077F0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00507860.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005078E0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00507950.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005079C0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00507A40.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have no positive surface delta, and their
   terrain height relative to the shared origin stays inside the configured lower and upper band. Typed parameters:
   p2 scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 TerrainHeightBand_TestDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  undefined4 in_EAX;
  int terrainHeightDeltaQ12;
  undefined4 in_EDX;
  
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return CONCAT44(in_EDX,in_EAX);
    }
    if (((((cell->flagsAndMaterial & 0x88006000) != 0) ||
         (terrainHeightDeltaQ12 = cell->terrainHeight - g_TerrainScanReferenceHeight,
         0 < cell->waterSurfaceDelta)) || (g_TerrainHeightBandMaximumDelta < terrainHeightDeltaQ12))
       || (terrainHeightDeltaQ12 < g_TerrainHeightBandMinimumDelta)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x005081D0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 0 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection0
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + 1;
  }
  return;
}

/* Address: 0x00508240.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 1 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection1
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell + (0x80 - g_TerrainScanRowStrideBytes));
  }
  return;
}

/* Address: 0x005082B0.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 2 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection2
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)((int)cell - g_TerrainScanRowStrideBytes);
  }
  return;
}

/* Address: 0x00508320.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 3 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection3
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = cell + -1;
  }
  return;
}

/* Address: 0x00508390.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 4 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection4
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
  }
  return;
}

/* Address: 0x00508400.
   Ownership: world/terrain/height.
   Purpose: Walks directional terrain run 5 while cells are eligible, have a nonnegative surface delta, and the
   signed high word of the auxiliary height field remains at or above the shared threshold. Typed parameters: p2
   scanStep→TerrainDirectionalScanStep_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void TerrainAuxHeightThreshold_TestDirection5
               (TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

{
  while( true ) {
    if (g_TerrainScanStepLimit <= scanStep) {
      return;
    }
    if ((((cell->flagsAndMaterial & 0x88006000) != 0) || (cell->waterSurfaceDelta < 0)) ||
       ((int)cell->triangle1NormalAngles >> 0x10 < g_TerrainAuxHeightMinimum)) break;
    scanStep = scanStep + 4;
    cell = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes);
  }
  return;
}

/* Address: 0x00508AE0.
   Ownership: world/terrain/height.
   Purpose: Walks terrain direction 0 and transfers the current reference-height delta between terrainHeight and
   waterSurfaceDelta while cells remain eligible. Typed parameters: p0 scanStep→TerrainDirectionalScanStep_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
void TerrainHeightDelta_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void TerrainHeightDelta_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void TerrainHeightDelta_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void TerrainHeightDelta_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
void TerrainHeightDelta_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      cell = (FieldGridCell *)(cell[-1].runtime00_07 + g_TerrainScanRowStrideBytes);
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
void TerrainHeightDelta_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *cell)

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
      cell = (FieldGridCell *)(cell->runtime00_07 + g_TerrainScanRowStrideBytes);
    } while (scanStep < g_TerrainScanStepLimit);
  }
  return;
}
